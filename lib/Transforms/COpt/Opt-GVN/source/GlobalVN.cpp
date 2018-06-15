#include "llvm/Pass.h"
#include "llvm/ADT/BitVector.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/DenseSet.h"
#include "llvm/ADT/PostOrderIterator.h"
#include "llvm/Analysis/AssumptionCache.h"
#include "llvm/Analysis/AliasAnalysis.h"
#include "llvm/Analysis/ConstantFolding.h"
#include "llvm/Analysis/GlobalsModRef.h"
#include "llvm/Analysis/InstructionSimplify.h"	
#include "llvm/IR/Function.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/IntrinsicInst.h"
#include "llvm/Transforms/Scalar/GVNExpression.h"
#include "llvm/Transforms/Utils/Local.h"
#include "llvm/Transforms/Utils/MemorySSA.h"
#include "llvm/Support/raw_ostream.h"
#include <cstring>
#include <vector>

using namespace llvm;
using namespace std;
using namespace llvm::GVNExpression;

namespace llvm
{
	template <> struct DenseMapInfo<const Expression *>
	{
		static const Expression *getEmptyKey()
		{
			auto Val = static_cast<uintptr_t>(-1);
			Val <<= PointerLikeTypeTraits<const Expression *>::NumLowBitsAvailable;
			return reinterpret_cast<const Expression *>(Val);
		}
		static const Expression *getTombstoneKey()
		{
			auto Val = static_cast<uintptr_t>(~1U);
			Val <<= PointerLikeTypeTraits<const Expression *>::NumLowBitsAvailable;
			return reinterpret_cast<const Expression *>(Val);
		}
		static unsigned getHashValue(const Expression *V)
		{
			return static_cast<unsigned>(V->getHashValue());
		}
		static bool isEqual(const Expression *LHS, const Expression *RHS)
		{
			if (LHS == RHS)
				return true;
			if (LHS == getTombstoneKey() || RHS == getTombstoneKey() || LHS == getEmptyKey() || RHS == getEmptyKey())
				return false;
			return *LHS == *RHS;
		}
	};
} // end namespace llvm

namespace
{
	static BasicBlock *getBlockForValue(Value *V)
	{
		if (auto *I = dyn_cast<Instruction>(V))
			return I->getParent();
		return nullptr;
	}

	struct CongruenceClass
	{
		using MemberSet = SmallPtrSet<Value *, 4>;
		unsigned ID;
		Value *RepLeader = nullptr; // representative leader of class
		const Expression *DefiningExpr = nullptr; // defining Expression
		MemberSet Members; // actual members of the class
		bool Dead = false; // true if the class has no members left - for assertion purposes, for skipping empty classes
		int StoreCount = 0; // number of stores in the congruence class
		std::pair<Value *, unsigned int> NextLeader = {nullptr, ~0U}; // next most dominating after current leader
		explicit CongruenceClass(unsigned ID) : ID(ID) {}
		CongruenceClass(unsigned ID, Value *Leader, const Expression *E) : ID(ID), RepLeader(Leader), DefiningExpr(E) {}
	};

	static bool alwaysAvailable(Value *V)
	{
		return isa<Constant>(V) || isa<Argument>(V);
	}

	class GlobalVN : public FunctionPass
	{	
		MemorySSA *MSSA;
		const TargetLibraryInfo *TLI;
		DominatorTree *DT;
		const DataLayout *DL;
		AssumptionCache *AC;
		AliasAnalysis *AA;
		MemorySSAWalker *MSSAWalker;
		BumpPtrAllocator ExpressionAllocator;
		ArrayRecycler<Value *> ArgRecycler;
		
		SmallPtrSet<Value *, 8> LeaderChanges;	// which values have changed as a result of leader changes
			
		DenseMap<const BasicBlock *, std::pair<int, int>> DFSDomMap; // DFS info
		DenseMap<const Value *, unsigned> InstrDFS;
		SmallVector<Value *, 32> DFSToInstr;
		DenseMap<const BasicBlock *, std::pair<unsigned, unsigned>> BlockInstRange;
		DenseMap<const DomTreeNode *, std::pair<unsigned, unsigned>> DominatedInstRange;
		
		SmallPtrSet<Instruction *, 8> InstructionsToErase;
		
		#ifndef NDEBUG
			DenseMap<const Value *, unsigned> ProcessedCount; // how many times each block and instruction gets processed
		#endif
		
		using BlockEdge = BasicBlockEdge; 	// reachability info
		DenseSet<BlockEdge> ReachableEdges;
		SmallPtrSet<const BasicBlock *, 8> ReachableBlocks;
		
		CongruenceClass *InitialClass;		// Congruence class info
		std::vector<CongruenceClass *> CongruenceClasses;
		unsigned NextCongruenceNum;

		DenseMap<Value *, CongruenceClass *> ValueToClass; // value mappings
		DenseMap<Value *, const Expression *> ValueToExpression;

		BitVector TouchedInstructions;

		DenseMap<const MemoryAccess *, MemoryAccess *> MemoryAccessEquiv; // table stores which memorydefs/phis represent memory state equivalent to another memory state

		using ExpressionClassMap = DenseMap<const Expression *, CongruenceClass *>; // typedef
		ExpressionClassMap ExpressionToClass;	// map Expression to class
		
		struct ValueDFS
		{
			int DFSIn = 0;
			int DFSOut = 0;
			int LocalNum = 0;

			Value *Val = nullptr;	// only one of these will be set
			Use *U = nullptr;

			bool operator<(const ValueDFS &Other) const
			{
				return std::tie(DFSIn, DFSOut, LocalNum, Val, U) < std::tie(Other.DFSIn, Other.DFSOut, Other.LocalNum, Other.Val, Other.U);
			}
		};

		class ValueDFSStack
		{
			public:
			Value *back() const { return ValueStack.back(); }
			std::pair<int, int> dfs_back() const { return DFSStack.back(); }

			void push_back(Value *V, int DFSIn, int DFSOut)
			{
				ValueStack.emplace_back(V);
				DFSStack.emplace_back(DFSIn, DFSOut);
			}
			bool empty() const { return DFSStack.empty(); }
			bool isInScope(int DFSIn, int DFSOut) const
			{
				if (empty())
					return false;
				return DFSIn >= DFSStack.back().first && DFSOut <= DFSStack.back().second;
			}

			void popUntilDFSScope(int DFSIn, int DFSOut)
			{
				assert(ValueStack.size() == DFSStack.size() && "Mismatch between ValueStack and DFSStack");
				while (!DFSStack.empty() && !(DFSIn >= DFSStack.back().first && DFSOut <= DFSStack.back().second))
				{
					DFSStack.pop_back();
					ValueStack.pop_back();
				}
			}

			private:
			SmallVector<Value *, 8> ValueStack;
			SmallVector<std::pair<int, int>, 8> DFSStack;
		};

		public:
		static char ID;
		
		GlobalVN() : FunctionPass(ID)
		{							}

		void getAnalysisUsage(AnalysisUsage& AU) const
		{
			AU.addRequired<AssumptionCacheTracker>();
			AU.addRequired<DominatorTreeWrapperPass>();
			AU.addRequired<TargetLibraryInfoWrapperPass>();
			AU.addRequired<MemorySSAWrapperPass>();
			AU.addRequired<AAResultsWrapperPass>();
			AU.addPreserved<DominatorTreeWrapperPass>();
			AU.addPreserved<GlobalsAAWrapperPass>();
		}
		
		std::pair<unsigned, unsigned> assignDFSNumbers(BasicBlock *B, unsigned Start)
		{
			unsigned End = Start;
			if (MemoryAccess *MemPhi = MSSA->getMemoryAccess(B))
			{
				InstrDFS[MemPhi] = End++;
				DFSToInstr.emplace_back(MemPhi);
			}
			for (auto &I : *B)
			{
				InstrDFS[&I] = End++;
				DFSToInstr.emplace_back(&I);
			}
			return std::make_pair(Start, End); 
		}
		
		CongruenceClass *createSingletonCongruenceClass(Value *Member)
		{
			CongruenceClass *CClass = createCongruenceClass(Member, nullptr);
			CClass->Members.insert(Member);
			ValueToClass[Member] = CClass;
			return CClass;
		}

		CongruenceClass *createCongruenceClass(Value *Leader, const Expression *E)
		{
			auto *result = new CongruenceClass(NextCongruenceNum++, Leader, E);
			CongruenceClasses.emplace_back(result);
			return result;
		}

		void initializeCongruenceClasses(Function &F)
		{
			NextCongruenceNum = 2;
			CongruenceClass::MemberSet InitialValues;
			InitialClass = createCongruenceClass(nullptr, nullptr);
			for (auto &B : F)
			{
				if (auto *MP = MSSA->getMemoryAccess(&B))
					MemoryAccessEquiv.insert({MP, MSSA->getLiveOnEntryDef()});
				for (auto &I : B)
				{
					InitialValues.insert(&I);
					ValueToClass[&I] = InitialClass;
					if (isa<StoreInst>(&I))
					{
						MemoryAccessEquiv.insert({MSSA->getMemoryAccess(&I), MSSA->getLiveOnEntryDef()});
						++InitialClass->StoreCount;
						assert(InitialClass->StoreCount > 0);
					}
				}
			}
			InitialClass->Members.swap(InitialValues); 
			for (auto &FA : F.args()) 				// initialize arguments to be in their own unique congruence classes
				createSingletonCongruenceClass(&FA);
		}

		void updateProcessedCount(Value *V)
		{
			#ifndef NDEBUG
				if (ProcessedCount.count(V) == 0)
					ProcessedCount.insert({V, 1});
				else
				{
					ProcessedCount[V] += 1;
					assert(ProcessedCount[V] < 100 && "Seem to have processed the same Value a lot");
				}
			#endif
		}
		
		MemoryAccess *lookupMemoryAccessEquiv(MemoryAccess *MA) const
		{
			MemoryAccess *Result = MemoryAccessEquiv.lookup(MA);
			return Result ? Result : MA;
		}
		
		bool setMemoryAccessEquivTo(MemoryAccess *From, MemoryAccess *To)
		{
			auto LookupResult = MemoryAccessEquiv.find(From);
			bool Changed = false;
			if (LookupResult != MemoryAccessEquiv.end()) 	// if it's in the table, see if value changed
			{
				if (To && LookupResult->second != To) 		// it wasn't equivalent before, now it is
				{
					LookupResult->second = To;
					Changed = true;
				}
				else if (!To) 					// it used to be equivalent to something, now it's not
				{
					MemoryAccessEquiv.erase(LookupResult);
					Changed = true;
				}
			}
			else
				assert(!To && "Memory equivalence should never change from nothing to something");
			return Changed;
		}
		
		void markMemoryUsersTouched(MemoryAccess *MA)
		{
			for (auto U : MA->users())
				if (auto *MUD = dyn_cast<MemoryUseOrDef>(U))
					TouchedInstructions.set(InstrDFS[MUD->getMemoryInst()]);
				else
					TouchedInstructions.set(InstrDFS[U]);
		}
		
		template <class T>	// basic blocks or basic block edges
		Value *lookupOperandLeader(Value *V, const User *U, const T &B) const
		{
			CongruenceClass *CC = ValueToClass.lookup(V);
			if (CC && (CC != InitialClass))
				return CC->RepLeader;
			return V;
		}
		
		bool setBasicExpressionInfo(Instruction *I, BasicExpression *E, const BasicBlock *B)
		{
			bool AllConstant = true;
			if (auto *GEP = dyn_cast<GetElementPtrInst>(I))
				E->setType(GEP->getSourceElementType());
			else
				E->setType(I->getType());
			E->setOpcode(I->getOpcode());
			E->allocateOperands(ArgRecycler, ExpressionAllocator);

			std::transform(I->op_begin(), I->op_end(), op_inserter(E), [&](Value *O) { auto Operand = lookupOperandLeader(O, I, B); AllConstant &= isa<Constant>(Operand); return Operand; });
			return AllConstant; // transform operand array into operand leader array, keep track of whether all members are constant
		}
		
		const Expression *checkSimplificationResults(Expression *E, Instruction *I, Value *V)
		{
			if (!V)
				return nullptr;
			if (auto *C = dyn_cast<Constant>(V))
			{
				assert(isa<BasicExpression>(E) && "We should always have had a basic expression here");
				cast<BasicExpression>(E)->deallocateOperands(ArgRecycler);
				ExpressionAllocator.Deallocate(E);
				return createConstantExpression(C);
			}
			else if (isa<Argument>(V) || isa<GlobalVariable>(V))
			{
				cast<BasicExpression>(E)->deallocateOperands(ArgRecycler);
				ExpressionAllocator.Deallocate(E);
				return createVariableExpression(V);
			}
			CongruenceClass *CC = ValueToClass.lookup(V);
			if (CC && CC->DefiningExpr)
			{
				assert(isa<BasicExpression>(E) && "We should always have had a basic expression here");
				cast<BasicExpression>(E)->deallocateOperands(ArgRecycler);
				ExpressionAllocator.Deallocate(E);
				return CC->DefiningExpr;
			}
			return nullptr;
		}
		
		const Expression *createExpression(Instruction *I, const BasicBlock *B)
		{
			auto *E = new (ExpressionAllocator) BasicExpression(I->getNumOperands());
			bool AllConstant = setBasicExpressionInfo(I, E, B);

			if (I->isCommutative())
			{
				assert(I->getNumOperands() == 2 && "Unsupported commutative instruction!");
				if (E->getOperand(0) > E->getOperand(1))
					E->swapOperands(0, 1); // to get the same value number 
			}
			
			// perform simplificaiton
			if (auto *CI = dyn_cast<CmpInst>(I))
			{
				CmpInst::Predicate Predicate = CI->getPredicate(); // sort operand value numbers so x<y and y>x get the same value
				if (E->getOperand(0) > E->getOperand(1))
				{
					E->swapOperands(0, 1);
					Predicate = CmpInst::getSwappedPredicate(Predicate);
				}
				E->setOpcode((CI->getOpcode() << 8) | Predicate);
				assert(I->getOperand(0)->getType() == I->getOperand(1)->getType() && "Wrong types on cmp instruction");
				if ((E->getOperand(0)->getType() == I->getOperand(0)->getType() && E->getOperand(1)->getType() == I->getOperand(1)->getType()))
				{
					Value *V = SimplifyCmpInst(Predicate, E->getOperand(0), E->getOperand(1), *DL, TLI, DT, AC);
					if (const Expression *SimplifiedE = checkSimplificationResults(E, I, V))
						return SimplifiedE;
				}
			}
			else if (isa<SelectInst>(I))
			{
				if (isa<Constant>(E->getOperand(0)) || (E->getOperand(1)->getType() == I->getOperand(1)->getType() && E->getOperand(2)->getType() == I->getOperand(2)->getType()))
				{
					Value *V = SimplifySelectInst(E->getOperand(0), E->getOperand(1), E->getOperand(2), *DL, TLI, DT, AC);
					if (const Expression *SimplifiedE = checkSimplificationResults(E, I, V))
						return SimplifiedE;
				}
			}
			else if (I->isBinaryOp())
			{
				Value *V = SimplifyBinOp(E->getOpcode(), E->getOperand(0), E->getOperand(1), *DL, TLI, DT, AC);
				if (const Expression *SimplifiedE = checkSimplificationResults(E, I, V))
					return SimplifiedE;
			}
			else if (auto *BI = dyn_cast<BitCastInst>(I))
			{
				Value *V = SimplifyInstruction(BI, *DL, TLI, DT, AC);
				if (const Expression *SimplifiedE = checkSimplificationResults(E, I, V))
					return SimplifiedE;
			}
			else if (isa<GetElementPtrInst>(I))
			{
			Value *V = SimplifyGEPInst(E->getType(), ArrayRef<Value *>(E->op_begin(), E->op_end()), *DL, TLI, DT, AC);
			if (const Expression *SimplifiedE = checkSimplificationResults(E, I, V))
				return SimplifiedE;
			}
			else if (AllConstant)
			{
				SmallVector<Constant *, 8> C;
				for (Value *Arg : E->operands())
					C.emplace_back(cast<Constant>(Arg));

				if (Value *V = ConstantFoldInstOperands(I, C, *DL, TLI))
					if (const Expression *SimplifiedE = checkSimplificationResults(E, I, V))
						return SimplifiedE;
			}
			return E;
		}
		
		const ConstantExpression *createConstantExpression(Constant *C)
		{
			auto *E = new (ExpressionAllocator) ConstantExpression(C);
			E->setOpcode(C->getValueID());
			return E;
		}
		
		const VariableExpression *createVariableExpression(Value *V)
		{
			auto *E = new (ExpressionAllocator) VariableExpression(V);
			E->setOpcode(V->getValueID());
			return E;
		}
		
		const Expression *createBinaryExpression(unsigned Opcode, Type *T, Value *Arg1, Value *Arg2, const BasicBlock *B)
		{
			auto *E = new (ExpressionAllocator) BasicExpression(2);

			E->setType(T);
			E->setOpcode(Opcode);
			E->allocateOperands(ArgRecycler, ExpressionAllocator);
			if (Instruction::isCommutative(Opcode))
				if (Arg1 > Arg2)
					std::swap(Arg1, Arg2);
					
			E->op_push_back(lookupOperandLeader(Arg1, nullptr, B));
			E->op_push_back(lookupOperandLeader(Arg2, nullptr, B));

			Value *V = SimplifyBinOp(Opcode, E->getOperand(0), E->getOperand(1), *DL, TLI, DT, AC);
			if (const Expression *SimplifiedE = checkSimplificationResults(E, nullptr, V))
				return SimplifiedE;
			return E;
		}
		
		PHIExpression *createPHIExpression(Instruction *I)
		{
			BasicBlock *PHIBlock = I->getParent();
			auto *PN = cast<PHINode>(I);
			auto *E = new (ExpressionAllocator) PHIExpression(PN->getNumOperands(), PHIBlock);

			E->allocateOperands(ArgRecycler, ExpressionAllocator);
			E->setType(I->getType());
			E->setOpcode(I->getOpcode());

			auto ReachablePhiArg = [&](const Use &U) { return ReachableBlocks.count(PN->getIncomingBlock(U)); };

			auto Filtered = make_filter_range(PN->operands(), ReachablePhiArg); 	// filter unreachable operands

			std::transform(Filtered.begin(), Filtered.end(), op_inserter(E), [&](const Use &U) -> Value * { if (U == PN) return PN; const BasicBlockEdge BBE(PN->getIncomingBlock(U), PHIBlock); return lookupOperandLeader(U, I, BBE); });
			return E;
		}
		
		const UnknownExpression *createUnknownExpression(Instruction *I)
		{
			auto *E = new (ExpressionAllocator) UnknownExpression(I);
			E->setOpcode(I->getOpcode());
			return E;
		}
		
		LoadExpression *createLoadExpression(Type *LoadType, Value *PointerOp, LoadInst *LI, MemoryAccess *DA, const BasicBlock *B)
		{
			auto *E = new (ExpressionAllocator) LoadExpression(1, LI, DA);
			E->allocateOperands(ArgRecycler, ExpressionAllocator);
			E->setType(LoadType);

			E->setOpcode(0);		//  same opcode for store and loads so they value number together
			E->op_push_back(lookupOperandLeader(PointerOp, LI, B));
			if (LI)
				E->setAlignment(LI->getAlignment());

			return E;
		}
		
		const StoreExpression *createStoreExpression(StoreInst *SI, MemoryAccess *DA, const BasicBlock *B)
		{
			auto *E = new (ExpressionAllocator) StoreExpression(SI->getNumOperands(), SI, DA);
			E->allocateOperands(ArgRecycler, ExpressionAllocator);
			E->setType(SI->getValueOperand()->getType());

			E->setOpcode(0);		//  same opcode for store and loads so they value number together
			E->op_push_back(lookupOperandLeader(SI->getPointerOperand(), SI, B));

			return E;
		}
		
		const CallExpression *createCallExpression(CallInst *CI, MemoryAccess *HV, const BasicBlock *B)
		{
			auto *E = new (ExpressionAllocator) CallExpression(CI->getNumOperands(), CI, HV);
			setBasicExpressionInfo(CI, E, B);
			return E;
		}

		bool hasMemberOtherThanUs(const CongruenceClass *CC, Instruction *I) // check if congruence class has member other than given instruction
		{
			return CC->StoreCount > 1 || CC->Members.count(I) == 0;
		}
		
		const AggregateValueExpression *createAggregateValueExpression(Instruction *I, const BasicBlock *B)
		{
			if (auto *II = dyn_cast<InsertValueInst>(I))
			{
				auto *E = new (ExpressionAllocator) AggregateValueExpression(I->getNumOperands(), II->getNumIndices());
				setBasicExpressionInfo(I, E, B);
				E->allocateIntOperands(ExpressionAllocator);
				std::copy(II->idx_begin(), II->idx_end(), int_op_inserter(E));
				return E;
			}
			else if (auto *EI = dyn_cast<ExtractValueInst>(I))
			{
				auto *E = new (ExpressionAllocator) AggregateValueExpression(I->getNumOperands(), EI->getNumIndices());
				setBasicExpressionInfo(EI, E, B);
				E->allocateIntOperands(ExpressionAllocator);
				std::copy(EI->idx_begin(), EI->idx_end(), int_op_inserter(E));
				return E;
			}
			llvm_unreachable("Unhandled type of aggregate value operation");
		}
		
		const Expression *performSymbolicStoreEvaluation(Instruction *I, const BasicBlock *B)
		{
			auto *SI = cast<StoreInst>(I);	// avoid value numbering stores
			MemoryAccess *StoreAccess = MSSA->getMemoryAccess(SI);
			if (SI->isSimple())
			{
				MemoryAccess *StoreRHS = lookupMemoryAccessEquiv(cast<MemoryDef>(StoreAccess)->getDefiningAccess()); // get expression for RHS of MemoryDef
				const Expression *OldStore = createStoreExpression(SI, StoreRHS, B);
				CongruenceClass *CC = ExpressionToClass.lookup(OldStore);
				if (CC && CC->DefiningExpr && isa<StoreExpression>(CC->DefiningExpr) && CC->RepLeader == lookupOperandLeader(SI->getValueOperand(), SI, B) && hasMemberOtherThanUs(CC, I))
					return createStoreExpression(SI, StoreRHS, B);
			}
			return createStoreExpression(SI, StoreAccess, B);
		}

		const Expression *performSymbolicLoadEvaluation(Instruction *I, const BasicBlock *B)
		{
			auto *LI = cast<LoadInst>(I);
			if (!LI->isSimple())
				return nullptr;
				
			Value *LoadAddressLeader = lookupOperandLeader(LI->getPointerOperand(), I, B);
			if (isa<UndefValue>(LoadAddressLeader))
				return createConstantExpression(UndefValue::get(LI->getType()));
				
			MemoryAccess *DefiningAccess = MSSAWalker->getClobberingMemoryAccess(I);
			if (!MSSA->isLiveOnEntryDef(DefiningAccess))
			{
				if (auto *MD = dyn_cast<MemoryDef>(DefiningAccess))
				{
					Instruction *DefiningInst = MD->getMemoryInst();
					if (!ReachableBlocks.count(DefiningInst->getParent()))
						return createConstantExpression(UndefValue::get(LI->getType()));
				}
			}

			const Expression *E = createLoadExpression(LI->getType(), LI->getPointerOperand(), LI, lookupMemoryAccessEquiv(DefiningAccess), B);
			return E;
		}

		const Expression *performSymbolicCallEvaluation(Instruction *I, const BasicBlock *B)
		{
			auto *CI = cast<CallInst>(I);
			if (AA->doesNotAccessMemory(CI))
				return createCallExpression(CI, nullptr, B);
			if (AA->onlyReadsMemory(CI))
			{
				MemoryAccess *DefiningAccess = MSSAWalker->getClobberingMemoryAccess(CI);
				return createCallExpression(CI, lookupMemoryAccessEquiv(DefiningAccess), B);
			}
			return nullptr;
		}
		
		const Expression *performSymbolicPHIEvaluation(Instruction *I, const BasicBlock *B)
		{
			auto *E = cast<PHIExpression>(createPHIExpression(I));
			bool HasUndef = false;
			auto Filtered = make_filter_range(E->operands(), [&](const Value *Arg) { if (Arg == I) return false; if (isa<UndefValue>(Arg)) { HasUndef = true; return false; } return true; });

			if (Filtered.begin() == Filtered.end()) 	// if no operands, then undef
			{
				E->deallocateOperands(ArgRecycler);
				ExpressionAllocator.Deallocate(E);
				return createConstantExpression(UndefValue::get(I->getType()));
			}
			
			Value *AllSameValue = *(Filtered.begin());
			++Filtered.begin();

			if (llvm::all_of(Filtered, [AllSameValue](const Value *V) { return V == AllSameValue; }))
			{
				if (HasUndef)
				{
					if (auto *AllSameInst = dyn_cast<Instruction>(AllSameValue))	// check for instructions
						if (!DT->dominates(AllSameInst, I))
							return E;
				}
				
				E->deallocateOperands(ArgRecycler);
				ExpressionAllocator.Deallocate(E);
				if (auto *C = dyn_cast<Constant>(AllSameValue))
					return createConstantExpression(C);
				return createVariableExpression(AllSameValue);
			}
			return E;
		}

		const Expression *performSymbolicAggrValueEvaluation(Instruction *I, const BasicBlock *B)
		{
			if (auto *EI = dyn_cast<ExtractValueInst>(I))
			{
				auto *II = dyn_cast<IntrinsicInst>(EI->getAggregateOperand());
				if (II && EI->getNumIndices() == 1 && *EI->idx_begin() == 0)
				{
					unsigned Opcode = 0;
					switch (II->getIntrinsicID())
					{
						case Intrinsic::sadd_with_overflow:
						case Intrinsic::uadd_with_overflow:	Opcode = Instruction::Add;
															break;
						case Intrinsic::ssub_with_overflow:
						case Intrinsic::usub_with_overflow:	Opcode = Instruction::Sub;
															break;
						case Intrinsic::smul_with_overflow:
						case Intrinsic::umul_with_overflow:	Opcode = Instruction::Mul;
															break;
						default:	break;
					}

					if (Opcode != 0)	// intrinsic
					{
						assert(II->getNumArgOperands() == 2 && "Expect two args for recognised intrinsics.");
						return createBinaryExpression(Opcode, EI->getType(), II->getArgOperand(0), II->getArgOperand(1), B);
					}
				}
			}
			return createAggregateValueExpression(I, B);
		}
		
		const Expression *performSymbolicEvaluation(Value *V, const BasicBlock *B)
		{
			const Expression *E = nullptr;
			if (auto *C = dyn_cast<Constant>(V))
				E = createConstantExpression(C);
			else if (isa<Argument>(V) || isa<GlobalVariable>(V))
				E = createVariableExpression(V);
			else
			{
				auto *I = cast<Instruction>(V);
				switch (I->getOpcode())
				{
					case Instruction::ExtractValue:
					case Instruction::InsertValue:  E = performSymbolicAggrValueEvaluation(I, B); break;
					case Instruction::PHI: E = performSymbolicPHIEvaluation(I, B); break;
					case Instruction::Call: E = performSymbolicCallEvaluation(I, B); break;
					case Instruction::Store: E = performSymbolicStoreEvaluation(I, B); break;
					case Instruction::Load: E = performSymbolicLoadEvaluation(I, B); break;
					case Instruction::BitCast: { E = createExpression(I, B); } break;
					case Instruction::Add: 
					case Instruction::FAdd:
					case Instruction::Sub: 
					case Instruction::FSub:
					case Instruction::Mul: 
					case Instruction::FMul:
					case Instruction::UDiv: 
					case Instruction::SDiv: 
					case Instruction::FDiv:
					case Instruction::URem: 
					case Instruction::SRem: 
					case Instruction::FRem:
					case Instruction::Shl: 
					case Instruction::LShr: 
					case Instruction::AShr:
					case Instruction::And: 
					case Instruction::Or: 
					case Instruction::Xor:
					case Instruction::ICmp: 
					case Instruction::FCmp:
					case Instruction::Trunc: 
					case Instruction::ZExt: 
					case Instruction::SExt:
					case Instruction::FPToUI: 
					case Instruction::FPToSI: 
					case Instruction::UIToFP: 
					case Instruction::SIToFP:
					case Instruction::FPTrunc: 
					case Instruction::FPExt:
					case Instruction::PtrToInt: 
					case Instruction::IntToPtr:
					case Instruction::Select:
					case Instruction::ExtractElement: 
					case Instruction::InsertElement: 
					case Instruction::ShuffleVector:
					case Instruction::GetElementPtr: E = createExpression(I, B); break;
					default: return nullptr;
				}
			}
			return E;
		}

		void markUsersTouched(Value *V)
		{
			for (auto *User : V->users())
			{
				assert(isa<Instruction>(User) && "Use of value not within an instruction?");
				TouchedInstructions.set(InstrDFS[User]);
			}
		}

		void markLeaderChangeTouched(CongruenceClass *CC)
		{
			for (auto M : CC->Members)
			{
				if (auto *I = dyn_cast<Instruction>(M))
					TouchedInstructions.set(InstrDFS[I]);
				LeaderChanges.insert(M);
			}
		}

		void moveValueToNewCongruenceClass(Instruction *I, CongruenceClass *OldClass,CongruenceClass *NewClass)
		{
			if (I == OldClass->NextLeader.first)
				OldClass->NextLeader = {nullptr, ~0U};

			if (isa<Instruction>(NewClass->RepLeader) && NewClass->RepLeader && I != NewClass->RepLeader && DT->properlyDominates(I->getParent(), cast<Instruction>(NewClass->RepLeader)->getParent()))
				assert(!isa<PHINode>(I) && "New class for instruction should not be dominated by instruction");

			if (NewClass->RepLeader != I)
			{
				auto DFSNum = InstrDFS.lookup(I);
				if (DFSNum < NewClass->NextLeader.second)
					NewClass->NextLeader = {I, DFSNum};
			}

			OldClass->Members.erase(I);
			NewClass->Members.insert(I);
			if (isa<StoreInst>(I))
			{
				--OldClass->StoreCount;
				assert(OldClass->StoreCount >= 0);
				++NewClass->StoreCount;
				assert(NewClass->StoreCount > 0);
			}

			ValueToClass[I] = NewClass;
			if (OldClass->Members.empty() && OldClass != InitialClass) 	// see if class is destroyed or need to swap leaders
			{
				if (OldClass->DefiningExpr)
				{
					OldClass->Dead = true;
					ExpressionToClass.erase(OldClass->DefiningExpr);
				}
			}
			else if (OldClass->RepLeader == I)
			{
				if (OldClass->Members.size() == 1 || OldClass == InitialClass)
					OldClass->RepLeader = *(OldClass->Members.begin());
				else if (OldClass->NextLeader.first)
				{
					OldClass->RepLeader = OldClass->NextLeader.first;
					OldClass->NextLeader = {nullptr, ~0U};
				}
				else
				{
					std::pair<Value *, unsigned> MinDFS = {nullptr, ~0U};
					for (const auto X : OldClass->Members)
					{
						auto DFSNum = InstrDFS.lookup(X);
						if (DFSNum < MinDFS.second)
						MinDFS = {X, DFSNum};
					}
					OldClass->RepLeader = MinDFS.first;
				}
				markLeaderChangeTouched(OldClass);
			}
		}

		void performCongruenceFinding(Instruction *I, const Expression *E)
		{
			ValueToExpression[I] = E;
			CongruenceClass *IClass = ValueToClass[I];
			assert(IClass && "Should have found a IClass");
			assert(!IClass->Dead && "Found a dead class");

			CongruenceClass *EClass;
			if (const auto *VE = dyn_cast<VariableExpression>(E))
			{
				EClass = ValueToClass[VE->getVariableValue()];
			}
			else
			{
				auto lookupResult = ExpressionToClass.insert({E, nullptr});

				if (lookupResult.second) 				// if not in value table, create new congruence class
				{
					CongruenceClass *NewClass = createCongruenceClass(nullptr, E);
					auto place = lookupResult.first;
					place->second = NewClass;

					if (const auto *CE = dyn_cast<ConstantExpression>(E)) 	// constants and variables should always be made leader 
						NewClass->RepLeader = CE->getConstantValue();
					else if (const auto *SE = dyn_cast<StoreExpression>(E)) 
					{
						StoreInst *SI = SE->getStoreInst();
						NewClass->RepLeader = lookupOperandLeader(SI->getValueOperand(), SI, SI->getParent());
					} 
					else 
						NewClass->RepLeader = I;
					assert(!isa<VariableExpression>(E) && "VariableExpression should have been handled already");

					EClass = NewClass;
				}
				else
				{
					EClass = lookupResult.first->second;
					if (isa<ConstantExpression>(E))
						assert(isa<Constant>(EClass->RepLeader) && "Any class with a constant expression should have a constant leader");
					assert(EClass && "Somehow don't have an eclass");
					assert(!EClass->Dead && "We accidentally looked up a dead class");
				}
			}
			
			bool ClassChanged = IClass != EClass;
			bool LeaderChanged = LeaderChanges.erase(I);
			if (ClassChanged || LeaderChanged)
			{
				if (ClassChanged)
					moveValueToNewCongruenceClass(I, IClass, EClass);
				markUsersTouched(I);
				if (MemoryAccess *MA = MSSA->getMemoryAccess(I))
				{
					if (!isa<MemoryUse>(MA) && isa<StoreExpression>(E) && EClass->Members.size() != 1)
					{
						auto *DefAccess = cast<StoreExpression>(E)->getDefiningAccess();
						setMemoryAccessEquivTo(MA, DefAccess != MA ? DefAccess : nullptr);
					}
					else
						setMemoryAccessEquivTo(MA, nullptr);
					markMemoryUsersTouched(MA);
				}
			}
			else if (auto *SI = dyn_cast<StoreInst>(I))
			{
				auto ProperLeader = lookupOperandLeader(SI->getValueOperand(), SI, SI->getParent());
				if (EClass->RepLeader != ProperLeader)
				{
					EClass->RepLeader = ProperLeader;
					markLeaderChangeTouched(EClass);
					markMemoryUsersTouched(MSSA->getMemoryAccess(SI));
				}
			}
		}
		
		void updateReachableEdge(BasicBlock *From, BasicBlock *To)
		{
			if (ReachableEdges.insert({From, To}).second) 	// check if Edge was reachable
			{
				if (ReachableBlocks.insert(To).second)		// if block wasn't reachable before, all instructions are touched
				{
					const auto &InstRange = BlockInstRange.lookup(To);
					TouchedInstructions.set(InstRange.first, InstRange.second);
				}
				else
				{
					if (MemoryAccess *MemPhi = MSSA->getMemoryAccess(To))
						TouchedInstructions.set(InstrDFS[MemPhi]);
					auto BI = To->begin();
					while (isa<PHINode>(BI))
					{
						TouchedInstructions.set(InstrDFS[&*BI]);
						++BI;
					}
				}
			}
		}
		
		Value *findConditionEquivalence(Value *Cond, BasicBlock *B) const 	// given predicate condition and block, find constant value
		{
			auto Result = lookupOperandLeader(Cond, nullptr, B);
			if (isa<Constant>(Result))
				return Result;
			return nullptr;
		}	

		void processOutgoingEdges(TerminatorInst *TI, BasicBlock *B)	// process outgoing edges of block for reachability
		{
			BranchInst *BR;				// get reachability of terminator instruction
			if ((BR = dyn_cast<BranchInst>(TI)) && BR->isConditional())
			{
				Value *Cond = BR->getCondition();
				Value *CondEvaluated = findConditionEquivalence(Cond, B);
				if (!CondEvaluated)
				{
					if (auto *I = dyn_cast<Instruction>(Cond))
					{
						const Expression *E = createExpression(I, B);
						if (const auto *CE = dyn_cast<ConstantExpression>(E))
							CondEvaluated = CE->getConstantValue();
					}
					else if (isa<ConstantInt>(Cond))
						CondEvaluated = Cond;
				}
				ConstantInt *CI;
				BasicBlock *TrueSucc = BR->getSuccessor(0);
				BasicBlock *FalseSucc = BR->getSuccessor(1);
				if (CondEvaluated && (CI = dyn_cast<ConstantInt>(CondEvaluated)))
				{
					if (CI->isOne())
						updateReachableEdge(B, TrueSucc);
					else if (CI->isZero())
						updateReachableEdge(B, FalseSucc);
				}
				else 
				{
					updateReachableEdge(B, TrueSucc);
					updateReachableEdge(B, FalseSucc);
				}
			}
			else if (auto *SI = dyn_cast<SwitchInst>(TI))
			{
				SmallDenseMap<BasicBlock *, unsigned, 16> SwitchEdges;
				Value *SwitchCond = SI->getCondition();
				Value *CondEvaluated = findConditionEquivalence(SwitchCond, B);

				if (CondEvaluated && isa<ConstantInt>(CondEvaluated)) 	// if switch statement could be turned into constant
				{
					auto *CondVal = cast<ConstantInt>(CondEvaluated); 
					auto CaseVal = SI->findCaseValue(CondVal);					// case value for value
					if (CaseVal.getCaseSuccessor() == SI->getDefaultDest())
					{
						updateReachableEdge(B, SI->getDefaultDest());
						return;
					}
					BasicBlock *TargetBlock = CaseVal.getCaseSuccessor(); 	// get successor and mark it reachable
					updateReachableEdge(B, TargetBlock);
				}
				else
				{
					for (unsigned i = 0, e = SI->getNumSuccessors(); i != e; ++i)
					{
						BasicBlock *TargetBlock = SI->getSuccessor(i);
						++SwitchEdges[TargetBlock];
						updateReachableEdge(B, TargetBlock);
					}
				}
			}
			else
			{
				for (unsigned i = 0, e = TI->getNumSuccessors(); i != e; ++i) // mark successors as reachable
				{
					BasicBlock *TargetBlock = TI->getSuccessor(i);
					updateReachableEdge(B, TargetBlock);
				}
				if (MemoryAccess *MA = MSSA->getMemoryAccess(TI))
					setMemoryAccessEquivTo(MA, nullptr);
			}
		}
		
		void valueNumberInstruction(Instruction *I) // value number single instruction, evaluate, find congruence, update mappings
		{
			if (isInstructionTriviallyDead(I, TLI))
			{
				InstructionsToErase.insert(I);
				return;
			}
			if (!I->isTerminator())
			{
				const auto *Symbolized = performSymbolicEvaluation(I, I->getParent()); 	// if no symbolic expr, use unknown expr
				if (Symbolized == nullptr)
					Symbolized = createUnknownExpression(I);
				performCongruenceFinding(I, Symbolized);
			}
			else
			{
				if (!I->getType()->isVoidTy()) // handle terminators that return values
				{
					auto *Symbolized = createUnknownExpression(I);
					performCongruenceFinding(I, Symbolized);
				}
				processOutgoingEdges(dyn_cast<TerminatorInst>(I), I->getParent());
			}
		}
		
		void valueNumberMemoryPhi(MemoryPhi *MP)
		{
			auto Filtered = make_filter_range(MP->operands(), [&](const Use &U) { return ReachableBlocks.count(MP->getIncomingBlock(U));});

			assert(Filtered.begin() != Filtered.end() && "We should not be processing a MemoryPhi in a completely unreachable block");

			auto LookupFunc = [&](const Use &U) 	// transform the remaining operands into operand leaders
			{
				return lookupMemoryAccessEquiv(cast<MemoryAccess>(U));
			};
			auto MappedBegin = map_iterator(Filtered.begin(), LookupFunc);
			auto MappedEnd = map_iterator(Filtered.end(), LookupFunc);

			MemoryAccess *AllSameValue = *MappedBegin; 	// check if all elements are equal
			++MappedBegin;
			bool AllEqual = std::all_of(MappedBegin, MappedEnd, [&AllSameValue](const MemoryAccess *V) { return V == AllSameValue; });

			if (setMemoryAccessEquivTo(MP, AllEqual ? AllSameValue : nullptr))
				markMemoryUsersTouched(MP);
		}
		
		static void patchReplacementInstruction(Instruction *I, Value *Repl)
		{
			auto *Op = dyn_cast<BinaryOperator>(I);		// patch replacement -> not more restrictive than the value being replaced
			auto *ReplOp = dyn_cast<BinaryOperator>(Repl);

			if (Op && ReplOp)
				ReplOp->andIRFlags(Op);

			if (auto *ReplInst = dyn_cast<Instruction>(Repl))
			{
				unsigned KnownIDs[] = {LLVMContext::MD_tbaa, LLVMContext::MD_alias_scope, LLVMContext::MD_noalias, LLVMContext::MD_range, LLVMContext::MD_fpmath, LLVMContext::MD_invariant_load, LLVMContext::MD_invariant_group};
				combineMetadata(ReplInst, I, KnownIDs);
			}
		}
		
		void convertDenseToDFSOrdered(CongruenceClass::MemberSet &Dense, SmallVectorImpl<ValueDFS> &DFSOrderedSet)
		{
			for (auto D : Dense)
			{
				BasicBlock *BB = getBlockForValue(D);		// add the value
				assert(BB && "Should have figured out a basic block for value");
				ValueDFS VD;

				std::pair<int, int> DFSPair = DFSDomMap[BB];
				assert(DFSPair.first != -1 && DFSPair.second != -1 && "Invalid DFS Pair");
				VD.DFSIn = DFSPair.first;
				VD.DFSOut = DFSPair.second;
				VD.Val = D;

				if (auto *I = dyn_cast<Instruction>(D))
					VD.LocalNum = InstrDFS[I];
				else
					llvm_unreachable("Should have been an instruction");

				DFSOrderedSet.emplace_back(VD);

				for (auto &U : D->uses())		// add the users
				{
					if (auto *I = dyn_cast<Instruction>(U.getUser()))
					{
						ValueDFS VD;
						BasicBlock *IBlock;
						if (auto *P = dyn_cast<PHINode>(I))
						{
							IBlock = P->getIncomingBlock(U);
							VD.LocalNum = InstrDFS.size() + 1;
						}
						else
						{
							IBlock = I->getParent();
							VD.LocalNum = InstrDFS[I];
						}
						std::pair<int, int> DFSPair = DFSDomMap[IBlock];
						VD.DFSIn = DFSPair.first;
						VD.DFSOut = DFSPair.second;
						VD.U = &U;
						DFSOrderedSet.emplace_back(VD);
					}
				}
			}
		}
		
		bool eliminateInstructions(Function &F)
		{
			bool AnythingReplaced = false;

			DT->updateDFSNumbers();

			for (auto &B : F)
			{
				if (!ReachableBlocks.count(&B))
				{
					for (const auto S : successors(&B))
					{
						for (auto II = S->begin(); isa<PHINode>(II); ++II)
						{
							auto &Phi = cast<PHINode>(*II);
							for (auto &Operand : Phi.incoming_values())
								if (Phi.getIncomingBlock(Operand) == &B)
									Operand.set(UndefValue::get(Phi.getType()));
						}
					}
				}
				DomTreeNode *Node = DT->getNode(&B);
				if (Node)
					DFSDomMap[&B] = {Node->getDFSNumIn(), Node->getDFSNumOut()};
			}

			for (CongruenceClass *CC : CongruenceClasses)
			{
				if (CC == InitialClass || CC->Dead)
					continue;
				assert(CC->RepLeader && "We should have had a leader");

				if (alwaysAvailable(CC->RepLeader))
				{
					SmallPtrSet<Value *, 4> MembersLeft;
					for (auto M : CC->Members)
					{
						Value *Member = M;

						if (Member == CC->RepLeader || Member->getType()->isVoidTy())	// no uses to be replaced for void
						{
							MembersLeft.insert(Member);
							continue;
						}
						if (auto *I = dyn_cast<Instruction>(Member))
						{
							assert(CC->RepLeader != I && "About to accidentally remove our leader");
							patchReplacementInstruction(I, CC->RepLeader);
  							I->replaceAllUsesWith(CC->RepLeader);
							InstructionsToErase.insert(I);
							AnythingReplaced = true;
							continue;
						}
						else
							MembersLeft.insert(I);
					}
					CC->Members.swap(MembersLeft);
				}
				else
				{
					if (CC->Members.size() != 1)		// skip singleton
					{
						ValueDFSStack EliminationStack;

						SmallVector<ValueDFS, 8> DFSOrderedSet;		// convert members to DFS ordered sets, merge them.
						convertDenseToDFSOrdered(CC->Members, DFSOrderedSet);

						std::sort(DFSOrderedSet.begin(), DFSOrderedSet.end());
						for (auto &VD : DFSOrderedSet)
						{
							int MemberDFSIn = VD.DFSIn;
							int MemberDFSOut = VD.DFSOut;
							Value *Member = VD.Val;
							Use *MemberUse = VD.U;

							if (Member)
							{
								if (Member->getType()->isVoidTy())
									continue;
							}

							bool ShouldPush = Member && (EliminationStack.empty() || isa<Constant>(Member));
							bool OutOfScope = !EliminationStack.isInScope(MemberDFSIn, MemberDFSOut);

							if (OutOfScope || ShouldPush)
							{
								EliminationStack.popUntilDFSScope(MemberDFSIn, MemberDFSOut);	// sync to current scope
								ShouldPush |= Member && EliminationStack.empty();
								if (ShouldPush)
									EliminationStack.push_back(Member, MemberDFSIn, MemberDFSOut);
							}

							if (EliminationStack.empty())
								continue;
								
							if (Member)
								continue;
							Value *Result = EliminationStack.back();

							if (MemberUse->get() == Result)
								continue;
							
							if (auto *ReplacedInst = dyn_cast<Instruction>(MemberUse->get()))
								patchReplacementInstruction(ReplacedInst, Result);

							assert(isa<Instruction>(MemberUse->getUser()));
							MemberUse->set(Result);
							AnythingReplaced = true;
						}
					}
				}

				SmallPtrSet<Value *, 4> MembersLeft;
				for (Value *Member : CC->Members)
				{
					if (Member->getType()->isVoidTy())
					{
						MembersLeft.insert(Member);
						continue;
					}
					if (auto *MemberInst = dyn_cast<Instruction>(Member))
					{
						if (isInstructionTriviallyDead(MemberInst))
						{
							InstructionsToErase.insert(MemberInst);
							continue;
						}
					}
					MembersLeft.insert(Member);
				}
				CC->Members.swap(MembersLeft);
			}

			return AnythingReplaced;
		}

		void deleteInstructionsInBlock(BasicBlock *BB)
		{
			if (isa<TerminatorInst>(BB->begin()))	// check if there are non-terminating instructions to be deleted
				return;

			auto StartPoint = BB->rbegin();		// delete instructions backwards
			++StartPoint;
			// Note that we explicitly recalculate BB->rend() on each iteration,
			// as it may change when we remove the first instruction.
			for (BasicBlock::reverse_iterator I(StartPoint); I != BB->rend();)
			{
				Instruction &Inst = *I++;
				if (!Inst.use_empty())
					Inst.replaceAllUsesWith(UndefValue::get(Inst.getType()));
				if (isa<LandingPadInst>(Inst))
					continue;
				Inst.eraseFromParent();
			}
		}

		void cleanupTables()
		{
			for (unsigned i = 0, e = CongruenceClasses.size(); i != e; ++i)
			{
				delete CongruenceClasses[i];
				CongruenceClasses[i] = nullptr;
			}

			ValueToClass.clear();
			ArgRecycler.clear(ExpressionAllocator);
			ExpressionAllocator.Reset();
			CongruenceClasses.clear();
			ExpressionToClass.clear();
			ValueToExpression.clear();
			ReachableBlocks.clear();
			ReachableEdges.clear();
			#ifndef NDEBUG
				ProcessedCount.clear();
			#endif
			InstructionsToErase.clear();
			DFSDomMap.clear();
			InstrDFS.clear();
			DFSToInstr.clear();
			BlockInstRange.clear();
			TouchedInstructions.clear();
			DominatedInstRange.clear();
			MemoryAccessEquiv.clear();
		}

		virtual bool runOnFunction(Function &F)		
		{
			bool Changed = false;
			DL = &F.getParent()->getDataLayout();
			AC = &getAnalysis<AssumptionCacheTracker>().getAssumptionCache(F);
			MSSA = &getAnalysis<MemorySSAWrapperPass>().getMSSA();
			TLI = &getAnalysis<TargetLibraryInfoWrapperPass>().getTLI();
			DT = &getAnalysis<DominatorTreeWrapperPass>().getDomTree();
			AA = &getAnalysis<AAResultsWrapperPass>().getAAResults();
			MSSAWalker = MSSA->getWalker();

			unsigned ICount = 1; // instruction count for size of hash table
			DFSToInstr.emplace_back(nullptr); // 0th instruction, start at 1
			
			DenseMap<const DomTreeNode *, unsigned> RPOOrdering; // to get in order of reverse post order
			ReversePostOrderTraversal<Function *> RPOT(&F);
			
			unsigned Counter = 0;
			for (auto &B : RPOT) 
			{
				auto *Node = DT->getNode(B);
				assert(Node && "RPO and Dominator tree should have same reachability");
				RPOOrdering[Node] = ++Counter;
			}
			for (auto &B : RPOT) // sort dominator tree children arrays into reverse post order
			{
				auto *Node = DT->getNode(B);
				if (Node->getChildren().size() > 1)
					std::sort(Node->begin(), Node->end(), [&RPOOrdering](const DomTreeNode *A, const DomTreeNode *B) { return RPOOrdering[A] < RPOOrdering[B]; });
			}
			auto DFI = df_begin(DT->getRootNode()); // depth first search of domtree gives reverse post order
			for (auto DFE = df_end(DT->getRootNode()); DFI != DFE; ++DFI)
			{
				BasicBlock *B = DFI->getBlock();
				const auto &BlockRange = assignDFSNumbers(B, ICount);
				BlockInstRange.insert({B, BlockRange});
				ICount += BlockRange.second - BlockRange.first;
			}
			for (auto &B : F) // handle forward unreachable blocks, find which blocks have single predecessors
			{
				if (!DFI.nodeVisited(DT->getNode(&B))) // assign numbers to unreachable blocks
				{
					const auto &BlockRange = assignDFSNumbers(&B, ICount);
					BlockInstRange.insert({&B, BlockRange});
					ICount += BlockRange.second - BlockRange.first;
				}
			}
			
			TouchedInstructions.resize(ICount);
			DominatedInstRange.reserve(F.size());
			ExpressionToClass.reserve(ICount);

			const auto &InstRange = BlockInstRange.lookup(&F.getEntryBlock()); // touched instructions include entry block
			TouchedInstructions.set(InstRange.first, InstRange.second);
			ReachableBlocks.insert(&F.getEntryBlock());

			initializeCongruenceClasses(F);
			
			unsigned int Iterations = 0;
			BasicBlock *LastBlock = &F.getEntryBlock();  // start at entry block
			while (TouchedInstructions.any())
			{
				++Iterations; 		// traverse instructions in all the blocks in reverse post order
				for (int InstrNum = TouchedInstructions.find_first(); InstrNum != -1; InstrNum = TouchedInstructions.find_next(InstrNum))
				{
					assert(InstrNum != 0 && "Bit 0 should never be set, something touched an instruction not in the lookup table");
					Value *V = DFSToInstr[InstrNum];
					BasicBlock *CurrBlock = nullptr;

					if (auto *I = dyn_cast<Instruction>(V))
						CurrBlock = I->getParent();
					else if (auto *MP = dyn_cast<MemoryPhi>(V))
						CurrBlock = MP->getBlock();
					else
						llvm_unreachable("DFSToInstr gave us an unknown type of instruction");

					if (CurrBlock != LastBlock)
					{
						LastBlock = CurrBlock;
						bool BlockReachable = ReachableBlocks.count(CurrBlock);
						const auto &CurrInstRange = BlockInstRange.lookup(CurrBlock);

						if (!BlockReachable)
						{
							TouchedInstructions.reset(CurrInstRange.first, CurrInstRange.second);
							continue;
						}
						updateProcessedCount(CurrBlock);
					}

					if (auto *MP = dyn_cast<MemoryPhi>(V))
						valueNumberMemoryPhi(MP);
					else if (auto *I = dyn_cast<Instruction>(V))
						valueNumberInstruction(I);
					else
						llvm_unreachable("Should have been a MemoryPhi or Instruction");
					updateProcessedCount(V);
					TouchedInstructions.reset(InstrNum);
				}
			}
			
			Changed |= eliminateInstructions(F);
			
			for (Instruction *ToErase : InstructionsToErase)
			{
				if (!ToErase->use_empty())
					ToErase->replaceAllUsesWith(UndefValue::get(ToErase->getType()));

				ToErase->eraseFromParent();
			}

			auto UnreachableBlockPred = [&](const BasicBlock &BB) 	// delete all unreachable blocks
			{
				return !ReachableBlocks.count(&BB);
			};

			for (auto &BB : make_filter_range(F, UnreachableBlockPred))
			{
				deleteInstructionsInBlock(&BB);
				Changed = true;
			}

			cleanupTables();
			return Changed;
		}
	};
}

char GlobalVN::ID = 0;			// these two lines are always added to carry out the pass
static RegisterPass<GlobalVN> temp("globalvn","- to do global value numbering");

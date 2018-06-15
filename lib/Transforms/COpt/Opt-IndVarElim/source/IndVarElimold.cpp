#include "llvm/Pass.h"
#include "llvm/Analysis/LoopPass.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/ScalarEvolution.h"
#include "llvm/Analysis/ScalarEvolutionExpander.h"
#include "llvm/Analysis/TargetTransformInfo.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Instruction.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Utils/Local.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include "llvm/Transforms/Utils/LoopUtils.h"
#include "llvm/Transforms/Utils/SimplifyIndVar.h"
#include "llvm/Support/raw_ostream.h"
#include <cstring>
#include <map>
#include <iterator>

using namespace llvm;
using namespace std;

namespace
{	
	static bool isLoopInvariant(Value *V, const Loop *L, const DominatorTree *DT)
	{
		Instruction *Inst = dyn_cast<Instruction>(V);
		if (!Inst)
			return true;
		return DT->properlyDominates(Inst->getParent(), L->getHeader());
	}

	static PHINode *getLoopPhiForCounter(Value *IncV, Loop *L, DominatorTree *DT)
	{
		Instruction *IncI = dyn_cast<Instruction>(IncV);
		if (!IncI)
			return nullptr;

		switch (IncI->getOpcode())
		{
			case Instruction::Add:
			case Instruction::Sub:	break;
			case Instruction::GetElementPtr:	if (IncI->getNumOperands() == 2)
												break;
			default:	return nullptr;
		}

		PHINode *Phi = dyn_cast<PHINode>(IncI->getOperand(0));
		if (Phi && Phi->getParent() == L->getHeader())
		{
			if (isLoopInvariant(IncI->getOperand(1), L, DT))
				return Phi;
			return nullptr;
		}
		if (IncI->getOpcode() == Instruction::GetElementPtr)
			return nullptr;

		Phi = dyn_cast<PHINode>(IncI->getOperand(1));
		if (Phi && Phi->getParent() == L->getHeader())
		{
			if (isLoopInvariant(IncI->getOperand(0), L, DT))
				return Phi;
		}
		return nullptr;
	}
	
	static Value *genLoopLimit(PHINode *IndVar, const SCEV *IVCount, Loop *L, SCEVExpander &Rewriter, ScalarEvolution *SE)
	{
		const SCEVAddRecExpr *AR = dyn_cast<SCEVAddRecExpr>(SE->getSCEV(IndVar));
		assert(AR && AR->getLoop() == L && AR->isAffine() && "bad loop counter");
		const SCEV *IVInit = AR->getStart();

		if (IndVar->getType()->isPointerTy() && !IVCount->getType()->isPointerTy())
		{
			Type *OfsTy = SE->getEffectiveSCEVType(IVInit->getType());
			const SCEV *IVOffset = SE->getTruncateOrZeroExtend(IVCount, OfsTy);

			assert(SE->isLoopInvariant(IVOffset, L) && "Computed iteration count is not loop invariant!");
			BranchInst *BI = cast<BranchInst>(L->getExitingBlock()->getTerminator());
			Value *GEPOffset = Rewriter.expandCodeFor(IVOffset, OfsTy, BI);

			Value *GEPBase = IndVar->getIncomingValueForBlock(L->getLoopPreheader());
			assert(AR->getStart() == SE->getSCEV(GEPBase) && "bad loop counter");
			assert(SE->getSizeOfExpr(IntegerType::getInt64Ty(IndVar->getContext()), cast<PointerType>(GEPBase->getType())->getElementType())->isOne() && "unit stride pointer IV must be i8*");

			IRBuilder<> Builder(L->getLoopPreheader()->getTerminator());
			return Builder.CreateGEP(nullptr, GEPBase, GEPOffset, "lftr.limit");
		}
		else
		{
			const SCEV *IVLimit = nullptr;
			if (AR->getStart()->isZero())
				IVLimit = IVCount;
			else
			{
				assert(AR->getStepRecurrence(*SE)->isOne() && "only handles unit stride");
				
				const SCEV *IVInit = AR->getStart();
				if (SE->getTypeSizeInBits(IVInit->getType()) > SE->getTypeSizeInBits(IVCount->getType()))
					IVInit = SE->getTruncateExpr(IVInit, IVCount->getType());
				IVLimit = SE->getAddExpr(IVInit, IVCount);
			}

			BranchInst *BI = cast<BranchInst>(L->getExitingBlock()->getTerminator());
			IRBuilder<> Builder(BI);
			assert(SE->isLoopInvariant(IVLimit, L) && "Computed iteration count is not loop invariant!");
			
			Type *LimitTy = IVCount->getType()->isPointerTy() ? IndVar->getType() : IVCount->getType();
			return Rewriter.expandCodeFor(IVLimit, LimitTy, BI);
		}
	}
	
	static bool hasConcreteDefImpl(Value *V, SmallPtrSetImpl<Value*> &Visited, unsigned Depth)
	{
		if (isa<Constant>(V))
			return !isa<UndefValue>(V);
		if (Depth >= 6)
		return false;
		
		Instruction *I = dyn_cast<Instruction>(V);
		if (!I)
			return false;
		if(I->mayReadFromMemory() || isa<CallInst>(I) || isa<InvokeInst>(I))
			return false;

		for (Value *Op : I->operands())
		{
			if (!Visited.insert(Op).second)
				continue;
			if (!hasConcreteDefImpl(Op, Visited, Depth+1))
				return false;
		}
		return true;
	}
	static bool hasConcreteDef(Value *V)
	{
		SmallPtrSet<Value*, 8> Visited;
		Visited.insert(V);
		return hasConcreteDefImpl(V, Visited, 0);
	}
	
	static ICmpInst *getLoopTest(Loop *L)
	{
		assert(L->getExitingBlock() && "expected loop exit");

		BasicBlock *LatchBlock = L->getLoopLatch();
		if (!LatchBlock)
			return nullptr;

		BranchInst *BI = dyn_cast<BranchInst>(L->getExitingBlock()->getTerminator());
		assert(BI && "expected exit branch");

		return dyn_cast<ICmpInst>(BI->getCondition());
	}

	static bool AlmostDeadIV(PHINode *Phi, BasicBlock *LatchBlock, Value *Cond)
	{
		int LatchIdx = Phi->getBasicBlockIndex(LatchBlock);
		Value *IncV = Phi->getIncomingValue(LatchIdx);

		for (User *U : Phi->users())
			if (U != Cond && U != IncV) return false;

		for (User *U : IncV->users())
			if (U != Cond && U != Phi) return false;
		return true;
	}

	static PHINode *FindLoopCounter(Loop *L, const SCEV *BECount, ScalarEvolution *SE, DominatorTree *DT)
	{
		uint64_t BCWidth = SE->getTypeSizeInBits(BECount->getType());
		Value *Cond = cast<BranchInst>(L->getExitingBlock()->getTerminator())->getCondition();

		PHINode *BestPhi = nullptr;
		const SCEV *BestInit = nullptr;
		BasicBlock *LatchBlock = L->getLoopLatch();
		assert(LatchBlock && "needsLFTR should guarantee a loop latch");
		const DataLayout &DL = L->getHeader()->getModule()->getDataLayout();

		for (BasicBlock::iterator I = L->getHeader()->begin(); isa<PHINode>(I); ++I)
		{
			PHINode *Phi = cast<PHINode>(I);
			if (!SE->isSCEVable(Phi->getType()))
				continue;
			if (BECount->getType()->isPointerTy() && !Phi->getType()->isPointerTy())
				continue;

			const SCEVAddRecExpr *AR = dyn_cast<SCEVAddRecExpr>(SE->getSCEV(Phi));
			if (!AR || AR->getLoop() != L || !AR->isAffine())
				continue;

			uint64_t PhiWidth = SE->getTypeSizeInBits(AR->getType());
			if (PhiWidth < BCWidth || !DL.isLegalInteger(PhiWidth))
				continue;

			const SCEV *Step = dyn_cast<SCEVConstant>(AR->getStepRecurrence(*SE));
			if (!Step || !Step->isOne())
				continue;

			int LatchIdx = Phi->getBasicBlockIndex(LatchBlock);
			Value *IncV = Phi->getIncomingValue(LatchIdx);
			if (getLoopPhiForCounter(IncV, L, DT) != Phi)
				continue;
				
			if (!hasConcreteDef(Phi))
			{
				if (ICmpInst *Cond = getLoopTest(L))
				{
					if (Phi != getLoopPhiForCounter(Cond->getOperand(0), L, DT) && Phi != getLoopPhiForCounter(Cond->getOperand(1), L, DT))
						continue;
				}
			}
			const SCEV *Init = AR->getStart();

			if (BestPhi && !AlmostDeadIV(BestPhi, LatchBlock, Cond))
			{
				if (AlmostDeadIV(Phi, LatchBlock, Cond))
					continue;
				if (BestInit->isZero() != Init->isZero())
				{
					if (BestInit->isZero())
						continue;
				}
				else if (PhiWidth <= SE->getTypeSizeInBits(BestPhi->getType()))
				continue;
			}
			BestPhi = Phi;
			BestInit = Init;
		}
		return BestPhi;
	}
	
	static bool ConvertToSInt(const APFloat &APF, int64_t &IntVal)
	{
		bool isExact = false;
		uint64_t UIntVal;
		if (APF.convertToInteger(&UIntVal, 64, true, APFloat::rmTowardZero, &isExact) != APFloat::opOK || !isExact)
			return false;
		IntVal = UIntVal;
		return true;
	}
	
	class IndVarElim : public LoopPass
	{	
		LoopInfo *LI;
		ScalarEvolution *SE;
		DominatorTree *DT;
		const DataLayout &DL;
		TargetLibraryInfo *TLI;
		const TargetTransformInfo *TTI;
		bool Changed = false;
		SmallVector<WeakVH, 16> DeadInsts;
		
		public:
		static char ID;
		
		IndVarElim() : LoopPass(ID), LI(LI), SE(SE), DT(DT), DL(DL), TLI(TLI), TTI(TTI)
		{							}

		void getAnalysisUsage(AnalysisUsage& AU) const
		{
			AU.setPreservesCFG();
			getLoopAnalysisUsage(AU);
		}
		
		void rewriteFirstIterationLoopExitValues(Loop *L)
		{
			assert(L->isLCSSAForm(*DT));

			SmallVector<BasicBlock *, 8> ExitBlocks;
			L->getUniqueExitBlocks(ExitBlocks);
			auto *LoopHeader = L->getHeader();
			assert(LoopHeader && "Invalid loop");

			for (auto *ExitBB : ExitBlocks)
			{
				BasicBlock::iterator BBI = ExitBB->begin();
				
				while (auto *PN = dyn_cast<PHINode>(BBI++))
				{
					for (unsigned IncomingValIdx = 0, E = PN->getNumIncomingValues(); IncomingValIdx != E; ++IncomingValIdx)
					{
						auto *IncomingBB = PN->getIncomingBlock(IncomingValIdx);
						if (IncomingBB != LoopHeader)
							continue;

						auto *TermInst = IncomingBB->getTerminator();
						Value *Cond = nullptr;
						if (auto *BI = dyn_cast<BranchInst>(TermInst))
							Cond = BI->getCondition();
						else if (auto *SI = dyn_cast<SwitchInst>(TermInst))
							Cond = SI->getCondition();
						else
							continue;

						if (!L->isLoopInvariant(Cond))
							continue;

						auto *ExitVal = dyn_cast<PHINode>(PN->getIncomingValue(IncomingValIdx));
						if (!ExitVal)
							continue;

						auto *LoopPreheader = L->getLoopPreheader();
						assert(LoopPreheader && "Invalid loop");
						int PreheaderIdx = ExitVal->getBasicBlockIndex(LoopPreheader);
						if (PreheaderIdx != -1)
						{
							assert(ExitVal->getParent() == LoopHeader && "ExitVal must be in loop header");
							PN->setIncomingValue(IncomingValIdx, ExitVal->getIncomingValue(PreheaderIdx));
						}
					}
				}
			}
		}

		void handleFloatingPointIV(Loop *L, PHINode *PN)
		{
			unsigned IncomingEdge = L->contains(PN->getIncomingBlock(0));
			unsigned BackEdge = IncomingEdge^1;
			auto *InitValueVal = dyn_cast<ConstantFP>(PN->getIncomingValue(IncomingEdge));
			int64_t InitValue;
			
			if (!InitValueVal || !ConvertToSInt(InitValueVal->getValueAPF(), InitValue))
				return;

			auto *Incr = dyn_cast<BinaryOperator>(PN->getIncomingValue(BackEdge));
			if (Incr == nullptr || Incr->getOpcode() != Instruction::FAdd) return;

			ConstantFP *IncValueVal = dyn_cast<ConstantFP>(Incr->getOperand(1));
			int64_t IncValue;
			if (IncValueVal == nullptr || Incr->getOperand(0) != PN || !ConvertToSInt(IncValueVal->getValueAPF(), IncValue))
				return;

			Value::user_iterator IncrUse = Incr->user_begin();
			Instruction *U1 = cast<Instruction>(*IncrUse++);
			if (IncrUse == Incr->user_end()) return;
			Instruction *U2 = cast<Instruction>(*IncrUse++);
			if (IncrUse != Incr->user_end()) return;

			FCmpInst *Compare = dyn_cast<FCmpInst>(U1);
			if (!Compare)
				Compare = dyn_cast<FCmpInst>(U2);
			if (!Compare || !Compare->hasOneUse() || !isa<BranchInst>(Compare->user_back()))
				return;

			BranchInst *TheBr = cast<BranchInst>(Compare->user_back());
			assert(TheBr->isConditional() && "Can't use fcmp if not conditional");
			if (!L->contains(TheBr->getParent()) || (L->contains(TheBr->getSuccessor(0)) && L->contains(TheBr->getSuccessor(1))))
				return;

			ConstantFP *ExitValueVal = dyn_cast<ConstantFP>(Compare->getOperand(1));
			int64_t ExitValue;
			if (ExitValueVal == nullptr || !ConvertToSInt(ExitValueVal->getValueAPF(), ExitValue))
				return;
				
			CmpInst::Predicate NewPred = CmpInst::BAD_ICMP_PREDICATE;
			switch (Compare->getPredicate())
			{
				default: return;		// unknown comparison
				case CmpInst::FCMP_OEQ:
				case CmpInst::FCMP_UEQ: NewPred = CmpInst::ICMP_EQ; break;
				case CmpInst::FCMP_ONE:
				case CmpInst::FCMP_UNE: NewPred = CmpInst::ICMP_NE; break;
				case CmpInst::FCMP_OGT:
				case CmpInst::FCMP_UGT: NewPred = CmpInst::ICMP_SGT; break;
				case CmpInst::FCMP_OGE:
				case CmpInst::FCMP_UGE: NewPred = CmpInst::ICMP_SGE; break;
				case CmpInst::FCMP_OLT:
				case CmpInst::FCMP_ULT: NewPred = CmpInst::ICMP_SLT; break;
				case CmpInst::FCMP_OLE:
				case CmpInst::FCMP_ULE: NewPred = CmpInst::ICMP_SLE; break;
			}

			if (!isInt<32>(InitValue) || !isInt<32>(IncValue) || !isInt<32>(ExitValue))
				return;
			if (IncValue == 0)
				return;

			if (IncValue > 0)
			{
				if (InitValue >= ExitValue)
					return;

				uint32_t Range = uint32_t(ExitValue-InitValue);
				if (NewPred == CmpInst::ICMP_SLE || NewPred == CmpInst::ICMP_SGT)
					if (++Range == 0) return;  // range overflows

				unsigned Leftover = Range % uint32_t(IncValue);
				if ((NewPred == CmpInst::ICMP_EQ || NewPred == CmpInst::ICMP_NE) && Leftover != 0)
					return;
				if (Leftover != 0 && int32_t(ExitValue+IncValue) < ExitValue)
					return;
			}
			else
			{
				if (InitValue <= ExitValue)
					return;

				uint32_t Range = uint32_t(InitValue-ExitValue);
				if (NewPred == CmpInst::ICMP_SGE || NewPred == CmpInst::ICMP_SLT)
					if (++Range == 0) return;  // range overflows

				unsigned Leftover = Range % uint32_t(-IncValue);
				if ((NewPred == CmpInst::ICMP_EQ || NewPred == CmpInst::ICMP_NE) && Leftover != 0)
					return;
				if (Leftover != 0 && int32_t(ExitValue+IncValue) > ExitValue)
					return;
			}

			IntegerType *Int32Ty = Type::getInt32Ty(PN->getContext());
			PHINode *NewPHI = PHINode::Create(Int32Ty, 2, PN->getName()+".int", PN);
			NewPHI->addIncoming(ConstantInt::get(Int32Ty, InitValue),
			PN->getIncomingBlock(IncomingEdge));

			Value *NewAdd = BinaryOperator::CreateAdd(NewPHI, ConstantInt::get(Int32Ty, IncValue), Incr->getName()+".int", Incr);
			NewPHI->addIncoming(NewAdd, PN->getIncomingBlock(BackEdge));

			ICmpInst *NewCompare = new ICmpInst(TheBr, NewPred, NewAdd, ConstantInt::get(Int32Ty, ExitValue), Compare->getName());

			WeakVH WeakPH = PN;
			NewCompare->takeName(Compare);
			Compare->replaceAllUsesWith(NewCompare);
			RecursivelyDeleteTriviallyDeadInstructions(Compare, TLI);
			
			Incr->replaceAllUsesWith(UndefValue::get(Incr->getType()));
			RecursivelyDeleteTriviallyDeadInstructions(Incr, TLI);

			if (WeakPH)
			{
				Value *Conv = new SIToFPInst(NewPHI, PN->getType(), "indvar.conv",
				&*PN->getParent()->getFirstInsertionPt());
				PN->replaceAllUsesWith(Conv);
				RecursivelyDeleteTriviallyDeadInstructions(PN, TLI);
			}
			Changed = true;
		}

		void rewriteNonIntegerIVs(Loop *L)
		{
			BasicBlock *Header = L->getHeader();
			SmallVector<WeakVH, 8> PHIs;
			
			for (BasicBlock::iterator I = Header->begin(); PHINode *PN = dyn_cast<PHINode>(I); ++I)
				PHIs.push_back(PN);

			for (unsigned i = 0, e = PHIs.size(); i != e; ++i)
				if (PHINode *PN = dyn_cast_or_null<PHINode>(&*PHIs[i]))
					handleFloatingPointIV(L, PN);

			if (Changed)
				SE->forgetLoop(L);
		}

		static bool canExpandBackedgeTakenCount(Loop *L, ScalarEvolution *SE, SCEVExpander &Rewriter)
		{
			const SCEV *BackedgeTakenCount = SE->getBackedgeTakenCount(L);
			if (isa<SCEVCouldNotCompute>(BackedgeTakenCount) || BackedgeTakenCount->isZero())
				return false;
			if (!L->getExitingBlock())
				return false;
			if (!isa<BranchInst>(L->getExitingBlock()->getTerminator()))
				return false;
			if (Rewriter.isHighCostExpansion(BackedgeTakenCount, L))
				return false;
			return true;
		}

		static bool needsLFTR(Loop *L, DominatorTree *DT)
		{
			ICmpInst *Cond = getLoopTest(L);	// LFTR to simplify the exit condition to an ICMP
			if (!Cond)
				return true;

			ICmpInst::Predicate Pred = Cond->getPredicate();		// LFTR to simplify the exit condition to an ICMP
			if (Pred != ICmpInst::ICMP_NE && Pred != ICmpInst::ICMP_EQ)
				return true;

			Value *LHS = Cond->getOperand(0);		// look for a loop invariant RHS
			Value *RHS = Cond->getOperand(1);
			if (!isLoopInvariant(RHS, L, DT))
			{
				if (!isLoopInvariant(LHS, L, DT))
					return true;
				std::swap(LHS, RHS);
			}

			PHINode *Phi = dyn_cast<PHINode>(LHS);		// look for a simple IV counter LHS
			if (!Phi)
			Phi = getLoopPhiForCounter(LHS, L, DT);

			if (!Phi)
				return true;

			int Idx = Phi->getBasicBlockIndex(L->getLoopLatch());	// LFTR if PHI node is defined in the loop
			if (Idx < 0)
				return true;

			Value *IncV = Phi->getIncomingValue(Idx);
			return Phi != getLoopPhiForCounter(IncV, L, DT);
		}

		Value *linearFunctionTestReplace(Loop *L, const SCEV *BackedgeTakenCount, PHINode *IndVar, SCEVExpander &Rewriter)
		{
			assert(canExpandBackedgeTakenCount(L, SE, Rewriter) && "precondition");
errs()<<"2\n";
			Value *CmpIndVar = IndVar;
			const SCEV *IVCount = BackedgeTakenCount;

			if (L->getExitingBlock() == L->getLoopLatch())
			{
				IVCount = SE->getAddExpr(BackedgeTakenCount, SE->getOne(BackedgeTakenCount->getType()));
				CmpIndVar = IndVar->getIncomingValueForBlock(L->getExitingBlock());
			}

			Value *ExitCnt = genLoopLimit(IndVar, IVCount, L, Rewriter, SE);
			assert(ExitCnt->getType()->isPointerTy() == IndVar->getType()->isPointerTy() && "genLoopLimit missed a cast");

			BranchInst *BI = cast<BranchInst>(L->getExitingBlock()->getTerminator());
			ICmpInst::Predicate P;
			if (L->contains(BI->getSuccessor(0)))
				P = ICmpInst::ICMP_NE;
			else
				P = ICmpInst::ICMP_EQ;

			IRBuilder<> Builder(BI);

			if (auto *Cond = dyn_cast<Instruction>(BI->getCondition()))
				Builder.SetCurrentDebugLocation(Cond->getDebugLoc());

			unsigned CmpIndVarSize = SE->getTypeSizeInBits(CmpIndVar->getType());
			unsigned ExitCntSize = SE->getTypeSizeInBits(ExitCnt->getType());
			if (CmpIndVarSize > ExitCntSize)
			{
				const SCEVAddRecExpr *AR = cast<SCEVAddRecExpr>(SE->getSCEV(IndVar));
				const SCEV *ARStart = AR->getStart();
				const SCEV *ARStep = AR->getStepRecurrence(*SE);
				
				if (isa<SCEVConstant>(ARStart) && isa<SCEVConstant>(IVCount))
				{
					const APInt &Start = cast<SCEVConstant>(ARStart)->getAPInt();
					APInt Count = cast<SCEVConstant>(IVCount)->getAPInt();
					
					if (IVCount != BackedgeTakenCount && Count == 0)
					{
						Count = APInt::getMaxValue(Count.getBitWidth()).zext(CmpIndVarSize);
						++Count;
					}
					else
						Count = Count.zext(CmpIndVarSize);
						
					APInt NewLimit;
					if (cast<SCEVConstant>(ARStep)->getValue()->isNegative())
						NewLimit = Start - Count;
					else
						NewLimit = Start + Count;
					ExitCnt = ConstantInt::get(CmpIndVar->getType(), NewLimit);
				}
				else
				{
					bool Extended = false;
					const SCEV *IV = SE->getSCEV(CmpIndVar);
					const SCEV *ZExtTrunc = SE->getZeroExtendExpr(SE->getTruncateExpr(SE->getSCEV(CmpIndVar), ExitCnt->getType()), CmpIndVar->getType());

					if (ZExtTrunc == IV)
					{
						Extended = true;
						ExitCnt = Builder.CreateZExt(ExitCnt, IndVar->getType(), "wide.trip.count");
					}
					else
					{
						const SCEV *SExtTrunc = SE->getSignExtendExpr(SE->getTruncateExpr(SE->getSCEV(CmpIndVar), ExitCnt->getType()), CmpIndVar->getType());
						if (SExtTrunc == IV)
						{
							Extended = true;
							ExitCnt = Builder.CreateSExt(ExitCnt, IndVar->getType(), "wide.trip.count");
						}
					}

					if (!Extended)
						CmpIndVar = Builder.CreateTrunc(CmpIndVar, ExitCnt->getType(), "lftr.wideiv");
				}
			}

			Value *Cond = Builder.CreateICmp(P, CmpIndVar, ExitCnt, "exitcond");
			Value *OrigCond = BI->getCondition();
			BI->setCondition(Cond);
			DeadInsts.push_back(OrigCond);

			Changed = true;
			return Cond;
		}

		virtual bool runOnLoop(Loop *L, LPPassManager &LPM)		
		{
			if (skipLoop(L))
				return false;

			auto *LI = &getAnalysis<LoopInfoWrapperPass>().getLoopInfo();
			auto *SE = &getAnalysis<ScalarEvolutionWrapperPass>().getSE();
			auto *DT = &getAnalysis<DominatorTreeWrapperPass>().getDomTree();
			auto *TLIP = getAnalysisIfAvailable<TargetLibraryInfoWrapperPass>();
			auto *TLI = TLIP ? &TLIP->getTLI() : nullptr;
			auto *TTIP = getAnalysisIfAvailable<TargetTransformInfoWrapperPass>();
			auto *TTI = TTIP ? &TTIP->getTTI(*L->getHeader()->getParent()) : nullptr;
			const DataLayout &DL = L->getHeader()->getModule()->getDataLayout();
			
			assert(L->isRecursivelyLCSSAForm(*DT, *LI) && "LCSSA required.");
			
			if (!L->isLoopSimplifyForm())
				return false;

			rewriteNonIntegerIVs(L);	// convert float to int

			const SCEV *BackedgeTakenCount = SE->getBackedgeTakenCount(L);
			SCEVExpander Rewriter(*SE, DL, "indvars");	// transform code
			Rewriter.disableCanonicalMode();			// canonical -> single IV

			/*simplifyAndExtend(L, Rewriter, LI);
			
			if (ReplaceExitValue != NeverRepl && !isa<SCEVCouldNotCompute>(BackedgeTakenCount))
				rewriteLoopExitValues(L, Rewriter);		// for values that can be computed, rewrite outside loop
			*/	
			Rewriter.replaceCongruentIVs(L, DT, DeadInsts);
			
			if (canExpandBackedgeTakenCount(L, SE, Rewriter) && needsLFTR(L, DT))	// rewite loop's exit condition
			{
				PHINode *IndVar = FindLoopCounter(L, BackedgeTakenCount, SE, DT);
				if (IndVar)
				{
					const SCEVAddRecExpr *AR = dyn_cast<SCEVAddRecExpr>(BackedgeTakenCount);
					if (!AR || AR->getLoop()->getLoopPreheader())
						(void)linearFunctionTestReplace(L, BackedgeTakenCount, IndVar, Rewriter);
				}
  			}
  			
			Rewriter.clear();
			
			rewriteFirstIterationLoopExitValues(L);
			
			Changed |= DeleteDeadPHIs(L->getHeader(), TLI);

			assert(L->isRecursivelyLCSSAForm(*DT, *LI) && "Indvars did not preserve LCSSA!");
			
			#ifndef NDEBUG
				if (!isa<SCEVCouldNotCompute>(BackedgeTakenCount))
				{
					SE->forgetLoop(L);
					const SCEV *NewBECount = SE->getBackedgeTakenCount(L);
					if (SE->getTypeSizeInBits(BackedgeTakenCount->getType()) < SE->getTypeSizeInBits(NewBECount->getType()))
						NewBECount = SE->getTruncateOrNoop(NewBECount, BackedgeTakenCount->getType());
					else
						BackedgeTakenCount = SE->getTruncateOrNoop(BackedgeTakenCount, NewBECount->getType());
					assert(BackedgeTakenCount == NewBECount && "indvars must preserve SCEV");
				}
			#endif

			return Changed;
		}
	};
}

char IndVarElim::ID = 0;			// these two lines are always added to carry out the pass
static RegisterPass<IndVarElim> temp("indvarelim","- to do induction variable elimination");

#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/DiagnosticInfo.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Analysis/Loads.h"
#include "llvm/Analysis/InstructionSimplify.h"
#include "llvm/Transforms/Utils/Local.h"
#include "llvm/Support/raw_ostream.h"
#include <cstring>

using namespace llvm;
using namespace std;

static bool isDynamicConstant(Value *V, CallInst *CI, ReturnInst *RI)
{
	if (isa<Constant>(V))
		return true; // static constants are always dyn consts

	if (Argument *Arg = dyn_cast<Argument>(V))
	{
		unsigned ArgNo = 0;
		Function *F = CI->getParent()->getParent();
		for (Function::arg_iterator AI = F->arg_begin(); &*AI != Arg; ++AI)
			++ArgNo;
		if (CI->getArgOperand(ArgNo) == Arg)
			return true;
	}
	
	if (BasicBlock *UniquePred = RI->getParent()->getUniquePredecessor())
		if (SwitchInst *SI = dyn_cast<SwitchInst>(UniquePred->getTerminator()))
			if (SI->getCondition() == V)
				return SI->getDefaultDest() != RI->getParent();
				
	return false;
}


static Value *getCommonReturnValue(ReturnInst *IgnoreRI, CallInst *CI)
{
	Function *F = CI->getParent()->getParent();
	Value *ReturnedValue = nullptr;

	for (BasicBlock &BBI : *F)
	{
		ReturnInst *RI = dyn_cast<ReturnInst>(BBI.getTerminator());
		if (RI == nullptr || RI == IgnoreRI)
			continue;
			
		Value *RetOp = RI->getOperand(0);
		if (!isDynamicConstant(RetOp, CI, RI))
			return nullptr;

		if (ReturnedValue && RetOp != ReturnedValue)
			return nullptr;     // cannot transform if differing values are returned.
		ReturnedValue = RetOp;
	}
	return ReturnedValue;
}

static bool canMoveAboveCall(Instruction *I, CallInst *CI)
{
	if (I->mayHaveSideEffects())
		return false;

	if (LoadInst *L = dyn_cast<LoadInst>(I))
	{
		if (CI->mayHaveSideEffects())
		{
			const DataLayout &DL = L->getModule()->getDataLayout();
			if (CI->mayWriteToMemory() || !isSafeToLoadUnconditionally(L->getPointerOperand(), L->getAlignment(), DL, L))
				return false;
		}
	}
	return !is_contained(I->operands(), CI);
}

static Value *canTransformAccumulatorRecursion(Instruction *I, CallInst *CI)
{
	if (!I->isAssociative() || !I->isCommutative())
		return nullptr;
	assert(I->getNumOperands() == 2 && "Associative/commutative operations should have 2 args!");

	if ((I->getOperand(0) == CI && I->getOperand(1) == CI) || (I->getOperand(0) != CI && I->getOperand(1) != CI)) 
		return nullptr; // exactly one operand should be the result of the call instruction
		
	if (!I->hasOneUse() || !isa<ReturnInst>(I->user_back()))
		return nullptr; // the only user of this instruction we allow is a single return instruction
		
	return getCommonReturnValue(cast<ReturnInst>(I->user_back()), CI);
}

static bool eliminateRecursiveTailCall(CallInst *CI, ReturnInst *Ret, BasicBlock *&OldEntry, SmallVectorImpl<PHINode *> &ArgumentPHIs)
{
	Value *AccumulatorRecursionEliminationInitVal = nullptr;
	Instruction *AccumulatorRecursionInstr = nullptr;
	
	BasicBlock::iterator BBI(CI);
	for (++BBI; &*BBI != Ret; ++BBI)
	{
		if (canMoveAboveCall(&*BBI, CI))
			continue;
		if ((AccumulatorRecursionEliminationInitVal = canTransformAccumulatorRecursion(&*BBI, CI)))
			AccumulatorRecursionInstr = &*BBI;
		else
			return false;
    }
	
	if (Ret->getNumOperands() == 1 && Ret->getReturnValue() != CI && !isa<UndefValue>(Ret->getReturnValue()) && AccumulatorRecursionEliminationInitVal == nullptr && !getCommonReturnValue(nullptr, CI))
	{
		if (!isDynamicConstant(Ret->getReturnValue(), CI, Ret))
			return false; // current return instruction does not return a constant

		AccumulatorRecursionEliminationInitVal = getCommonReturnValue(Ret, CI);
		if (!AccumulatorRecursionEliminationInitVal)
			return false;
	}
	
	BasicBlock *BB = Ret->getParent();
	Function *F = BB->getParent();

	emitOptimizationRemark(F->getContext(), "tailcallelim", *F, CI->getDebugLoc(), "transforming tail recursion to loop");
	
	if (!OldEntry)
	{
		OldEntry = &F->getEntryBlock();
		BasicBlock *NewEntry = BasicBlock::Create(F->getContext(), "", F, OldEntry);
		NewEntry->takeName(OldEntry);
		OldEntry->setName("tailrecurse");
		BranchInst::Create(OldEntry, NewEntry);

		for (BasicBlock::iterator OEBI = OldEntry->begin(), E = OldEntry->end(), NEBI = NewEntry->begin(); OEBI != E;)
			if (AllocaInst *AI = dyn_cast<AllocaInst>(OEBI++))
				if (isa<ConstantInt>(AI->getArraySize()))
					AI->moveBefore(&*NEBI);
						
		Instruction *InsertPos = &OldEntry->front();
		for (Function::arg_iterator I = F->arg_begin(), E = F->arg_end(); I != E; ++I)
		{
			PHINode *PN = PHINode::Create(I->getType(), 2, I->getName() + ".tr", InsertPos);
			I->replaceAllUsesWith(PN); 
			PN->addIncoming(&*I, NewEntry);
			ArgumentPHIs.push_back(PN);
		}
	}
	
	for (unsigned i = 0, e = CI->getNumArgOperands(); i != e; ++i)
		ArgumentPHIs[i]->addIncoming(CI->getArgOperand(i), BB);
		
	if (AccumulatorRecursionEliminationInitVal)
	{
		Instruction *AccRecInstr = AccumulatorRecursionInstr;
    
		pred_iterator PB = pred_begin(OldEntry), PE = pred_end(OldEntry);
		PHINode *AccPN = PHINode::Create(AccumulatorRecursionEliminationInitVal->getType(),distance(PB, PE) + 1, "accumulator.tr", &OldEntry->front());
		
		for (pred_iterator PI = PB; PI != PE; ++PI)
		{
			BasicBlock *P = *PI;
			if (P == &F->getEntryBlock())
				AccPN->addIncoming(AccumulatorRecursionEliminationInitVal, P);
			else
				AccPN->addIncoming(AccPN, P);
		}

		if (AccRecInstr)
		{
			AccPN->addIncoming(AccRecInstr, BB);
			AccRecInstr->setOperand(AccRecInstr->getOperand(0) != CI, AccPN);
		}
		else
			AccPN->addIncoming(Ret->getReturnValue(), BB);
	
	
		for (BasicBlock &BBI : *F)
			if (ReturnInst *RI = dyn_cast<ReturnInst>(BBI.getTerminator()))
				RI->setOperand(0, AccPN);
	}
	
	BranchInst *NewBI = BranchInst::Create(OldEntry, Ret);
	NewBI->setDebugLoc(CI->getDebugLoc());
	BB->getInstList().erase(Ret);  // remove return
	BB->getInstList().erase(CI);   // remove call
	return true;
}

namespace
{
	class TRE : public FunctionPass
	{
		public:
		static char ID;

		TRE() : FunctionPass(ID)
		{				}

		virtual bool runOnFunction(Function &F)
		{
			for (Function::iterator FI = F.begin(), FE = F.end(); FI != FE; ++FI)
			{
			BasicBlock* block = &*FI;

			for (BasicBlock::iterator i = block->begin(), e = block->end(); i!=e; ++i)
			{
			Instruction * I = &*i;
			if (CallInst *CI = dyn_cast<CallInst>(I))	// call instruction
			{
				if (isTRECandidate(CI))	// check if instruction is a candidate for performing TRE
				{
							if (F.getFunctionType()->isVarArg())
								return false;
							BasicBlock *OldEntry = nullptr;
							SmallVector<PHINode*, 8> ArgumentPHIs;
							if (ReturnInst *RI = dyn_cast<ReturnInst>(CI->getNextNode()))
							{
								eliminateRecursiveTailCall(CI,RI,OldEntry,ArgumentPHIs);
								return true;
							}
							for (PHINode *PN : ArgumentPHIs)
							{
								if (Value *PNV = SimplifyInstruction(PN, F.getParent()->getDataLayout()))
								{
									PN->replaceAllUsesWith(PNV);
									PN->eraseFromParent();
								}
							}

			}
			}
			}
			}
		}

		bool isTRECandidate(CallInst *CI)
		{
			BasicBlock *BB = CI->getParent();
			Function *F = BB->getParent();
		
			if (CI->getCalledFunction() == F)
			{
				if (CI->isTailCall())
					return true;
					
				Instruction *nextI = CI->getNextNode();
				
				if (isa<ReturnInst>(nextI))
					return true;
			}
			
			return false;
		}
	};
}


char TRE::ID = 0;
static RegisterPass<TRE> temp("tre","- to do tail recursion elimination");
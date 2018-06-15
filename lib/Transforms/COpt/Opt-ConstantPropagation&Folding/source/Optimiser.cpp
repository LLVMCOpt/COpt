#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Utils/Local.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/Analysis/ConstantFolding.h"
#include "llvm/IR/Constant.h"
#include "llvm/IR/Instruction.h"
#include "llvm/Pass.h"
#include "llvm/Analysis/TargetLibraryInfo.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/LoopPass.h"

using namespace llvm;
using namespace std;

namespace
{
	class ConstantFolding : public FunctionPass
	{
		public:
		static char ID;

		ConstantFolding() : FunctionPass(ID)
		{				}

		virtual bool runOnFunction(Function &F)
		{
			const DataLayout &DL = F.getParent()->getDataLayout();
			TargetLibraryInfo *TLI = &getAnalysis<TargetLibraryInfoWrapperPass>().getTLI();

			for (Function::iterator FI = F.begin(), FE = F.end(); FI != FE; ++FI) 
			{
				BasicBlock* block = &*FI;
			
			for (BasicBlock::iterator i = block->begin(), e = block->end(); i!=e; ++i)
			{
			Instruction * I = &*i;
				if (Constant *C = ConstantFoldInstruction(I, DL, TLI))
					I->replaceAllUsesWith(C);
			}
			}
		}
		void getAnalysisUsage(AnalysisUsage &AU) const override
		{
				AU.setPreservesCFG();
			AU.addRequired<TargetLibraryInfoWrapperPass>();
		}
	};
}
char ConstantFolding::ID = 0;
static RegisterPass<ConstantFolding> temp("constfold","- to do constant folding");
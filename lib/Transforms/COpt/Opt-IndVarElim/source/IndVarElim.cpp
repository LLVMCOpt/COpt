#include "llvm/Pass.h"
#include "llvm/Analysis/LoopPass.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/ScalarEvolution.h"
#include "llvm/Analysis/TargetTransformInfo.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Instruction.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Utils/Local.h"
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
	class IndVarElim : public LoopPass
	{	
		public:
		static char ID;
		
		IndVarElim() : LoopPass(ID)
		{							}

		void getAnalysisUsage(AnalysisUsage& AU) const
		{
			AU.setPreservesCFG();
			getLoopAnalysisUsage(AU);
		}
		
		virtual bool runOnLoop(Loop *L, LPPassManager &LPM)		
		{
			if (skipLoop(L))
				return false;

			ScalarEvolution &SE = getAnalysis<ScalarEvolutionWrapperPass>().getSE();
			
			LoopInfo &LI = getAnalysis<LoopInfoWrapperPass>().getLoopInfo();
			
			DominatorTreeWrapperPass *DTWP = getAnalysisIfAvailable<DominatorTreeWrapperPass>();
			DominatorTree *DT = DTWP ? &DTWP->getDomTree() : nullptr;
			
			SmallVector<WeakVH, 16> DeadInsts;
			
			for (LoopInfo::iterator i = LI.begin(), e=LI.end(); i != e; ++i)
			{
				Loop *L = *i;
				simplifyLoopIVs(L, &SE, DT, &LI, DeadInsts);
			}
			
			bool Changed = false;
			for (BasicBlock::iterator I = L->getHeader()->begin(); isa<PHINode>(I); ++I)
			{
				Changed |= simplifyUsersOfIV(cast<PHINode>(I), &SE, DT, &LI, DeadInsts);
			}
			return Changed;
			
		}
	};
}

char IndVarElim::ID = 0;			// these two lines are always added to carry out the pass
static RegisterPass<IndVarElim> temp("indvarelim","- to do induction variable elimination");

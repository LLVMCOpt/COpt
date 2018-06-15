#include "llvm/ADT/DenseMap.h"
#include "llvm/IR/Function.h"
#include "llvm/Pass.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/LoopPass.h"
#include "llvm/Analysis/ValueTracking.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/LegacyPassManagers.h"

#include <set>
#include <queue>
#include <vector>

#include "dataflow.h"
#include "dominance.h"

using namespace std;

namespace llvm
{

	class LICM : public LoopPass
	{
		public:
		
		static char ID;
		LICM() : LoopPass(ID) {}

		bool isInvariant(Loop* L, std::vector<Instruction*> invStmts, Instruction* inst)
		{
			if (!isSafeToSpeculativelyExecute(inst) || inst->mayReadFromMemory() || isa<LandingPadInst>(inst))
				return false;

			for (User::op_iterator OI = inst->op_begin(), OE = inst->op_end(); OI != OE; ++OI)	
			{																		// check if operands are constants or loop invariants
				Value *op = *OI;
				if (Instruction* op_inst = dyn_cast<Instruction>(op))
				{
					if (L->contains(op_inst) && std::find(invStmts.begin(), invStmts.end(), op_inst) == invStmts.end())
						return false;		// operand is an instruction inside loop, not loop invariant
				}
			}

			return true;
		}

		virtual void getAnalysisUsage(AnalysisUsage& AU) const
		{
			AU.addRequired<LoopInfo>();
		}

		virtual bool runOnLoop(Loop *L, LPPassManager &no_use)	// outermost loop is processed last
		{
			bool modified = false;

			BasicBlock* preheader = L->getLoopPreheader();	// if null cannot hoist instructions into predecessor
			if (!preheader) 
				return false;

			LoopInfo& LI = getAnalysis<LoopInfo>();		// get the loop info

			DataFlowResult dominanceResult = computeDom(L);
			std::map<BasicBlock*,BasicBlock*> idom = computeIdom(dominanceResult);
			printIdom(idom,L);
			DomTree* tree = getDominanceTree(idom,L);

			std::vector<DomTreeNode*> worklist;
			worklist.push_back(tree->root);

			std::vector<Instruction*> invStmts;	// loop invariant statements

			while (!worklist.empty())
			{
				DomTreeNode* n = worklist.back();
				BasicBlock* b = n->block;
				worklist.pop_back();

				if (LI.getLoopFor(b) != L) // already processed
					return false;

				for (BasicBlock::iterator II = b->begin(), IE = b->end(); II != IE; ++II) // look for invariant statements
				{
					Instruction* inst = &(*II);
					if (isInvariant(L, invStmts, inst)) 
						invStmts.push_back(inst);
				}

				for (int i = 0; i < n->children.size(); ++i) 
					worklist.push_back(n->children[i]);
			}

			for (int j = 0; j < invStmts.size(); ++j) // moving code outside the loop
			{
				Instruction* inst = invStmts[j];
				BasicBlock* b = inst->getParent();

				Instruction* end = &(preheader->back());
				inst->moveBefore(end);
				if (!modified) 
					modified = true;
				
			}

			return modified;
		}
	};

	char LICM::ID = 0;
	RegisterPass<LICM> Y("loop icm", "Loop ICM");

}

#include "llvm/Pass.h"						
#include "llvm/IR/Function.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/ValueMap.h"
#include "llvm/IR/Dominators.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/SmallPtrSet.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/ValueTracking.h"
#include "llvm/Transforms/Utils/Local.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/Casting.h"
#include <cstring>
#include <deque>
#include <set>
#include <stack>
#include <iostream>

#include "reachingdefs.cpp"

using namespace llvm;
using namespace std;
using namespace reachdef;

namespace
{	
	class LICM : public FunctionPass
	{	
		protected:
			deque<Loop *> LQ;
			
			void addLoopIntoQueue(Loop* L)
			{
				this->LQ.push_back(L);
				for (Loop::reverse_iterator I = L->rbegin(), E = L->rend(); I != E; ++I)
					addLoopIntoQueue(*I); 
			}
			
			bool checkFunctionCalls(Loop *L, Instruction *I)
			{
				std::vector<BasicBlock*> loopBlocks = L->getBlocks();
				for (std::vector<BasicBlock*>::iterator blockIter = loopBlocks.begin(); blockIter != loopBlocks.end(); ++blockIter)
				{
					for (BasicBlock::iterator instIter = (*blockIter)->begin(), e = (*blockIter)->end(); instIter != e; ++instIter)
					{
						Instruction* i = &*instIter;
						if (isa<CallInst>(i))
						{
							for (User::op_iterator opIter = I->op_begin(), e = I->op_end(); opIter != e; ++opIter)
							{
								Value* v = *opIter;
								if (isa<GlobalValue>(v))
									return false;
							}
						}
					}
				}
				return true;
			}
			
			bool checkOperandsForPHI(Instruction *I)
			{
				for (User::op_iterator opIter = I->op_begin(), e = I->op_end(); opIter != e; ++opIter)
				{
					Value* i = *opIter;
					if (isa<PHINode>((Instruction*)i))
						return false;
				}
			}
			
			std::set<Value*> computeLoopInvariantStatements(Loop* L, Function &F)
			{
				std::set<Value*> loopInvariantStatements;
				std::vector<BasicBlock*> loopBlocks = L->getBlocks();

				for (std::vector<BasicBlock*>::iterator blockIter = loopBlocks.begin(); blockIter != loopBlocks.end(); ++blockIter)
				{
					for (BasicBlock::iterator instIter = (*blockIter)->begin(), e = (*blockIter)->end(); instIter != e; ++instIter)
					{
						Value* v = &*instIter;
						
						if (isa<Constant>(v) || isa<Argument>(v) || isa<GlobalValue>(v)) // definitely loop invariant
							loopInvariantStatements.insert(v);
						else if (isa<Instruction>(v))
						{
							Instruction* I = static_cast<Instruction*>(v);
							
							if (isSafeToSpeculativelyExecute(I) && !I->mayReadFromMemory() && !isa<LandingPadInst>(I)) // may be invariant
							{
								bool allOperandsOnlyDefinedOutsideLoop = true; // check if definitions of operands reach inside the loop
								for (User::op_iterator opIter = I->op_begin(), e = I->op_end(); opIter != e; ++opIter)
								{
									Value* opVal = *opIter;
									for (std::vector<BasicBlock*>::iterator i1 = loopBlocks.begin(); i1 != loopBlocks.end(); ++i1)
									{
										for (BasicBlock::iterator i2 = (*i1)->begin(), e = (*i1)->end(); i2 != e; ++i2)
										{
											Instruction* i = &*i2;
											std::vector<string> defs = ReachDefs[F.getName().str().c_str()][InstrToIndex[i]];
											
											string percentage = "%";
											string opValName = percentage + opVal->getName().str();
											
											if (find(defs.begin(), defs.end(),opValName)!=defs.end())
											{
												allOperandsOnlyDefinedOutsideLoop = false;
												break;
											}		
										}
										if (!allOperandsOnlyDefinedOutsideLoop)
											break;
									}
								}
								if (allOperandsOnlyDefinedOutsideLoop && !isa<PHINode>(I) && checkOperandsForPHI(I))
									loopInvariantStatements.insert(v);
							}
						}
					}
				}
				bool converged = false;
				int invariantSetSize = loopInvariantStatements.size();

				while (!converged) // statements can be loop invariant if the reaching definitions of their operands are invariant 
				{
					int prevInvariantSetSize = invariantSetSize;
					for (std::vector<BasicBlock*>::iterator blockIter = loopBlocks.begin(); blockIter != loopBlocks.end(); ++blockIter)
					{
						for (BasicBlock::iterator instIter = (*blockIter)->begin(), e = (*blockIter)->end(); instIter != e; ++instIter)
						{
							Value* v = &*instIter;
							if (loopInvariantStatements.find(v) != loopInvariantStatements.end()) // if already marked as loop invariant
								continue;
							if (isa<Instruction>(v))
							{
								Instruction* I = static_cast<Instruction*>(v);
								if (L->hasLoopInvariantOperands(I) && !isa<PHINode>(I) && checkOperandsForPHI(I))
									loopInvariantStatements.insert(v);
								
							}
						}
					}
					invariantSetSize = loopInvariantStatements.size();
					converged = (invariantSetSize == prevInvariantSetSize);
				}	
				return loopInvariantStatements;
			}
			
			bool applyMotionToCandidates(Loop* L, set<Value*> motionCandidates) 
			{
				bool motionApplied = false;		// do DFS over the blocks of the loop and move each candidate to end of preheader 
				BasicBlock* preheader = L->getLoopPreheader(); // if all of its dependencies have also been moved to the preheader
				set<Instruction*> toMoveSet;
				set<BasicBlock*> visited;

				stack<BasicBlock*> work;
				work.push(*succ_begin(preheader)); //start at loop header, sole successor of the pre-header
				while (!work.empty())
				{
					BasicBlock* block = work.top();
					work.pop();
					visited.insert(block);
			
					for (BasicBlock::iterator instIter = block->begin(), e = block->end(); instIter != e; ++instIter) 
					{
						Instruction* I = &*instIter;
						if (motionCandidates.count(I) > 0) // move to preheader if code motion candidate 
						{
							motionApplied = true;
							toMoveSet.insert(I);
						}
					}

					for (succ_iterator successorBlock = succ_begin(block), E = succ_end(block); successorBlock != E; ++successorBlock) 
					{
						if (L->contains(*successorBlock)) 
						{
							if (visited.count(*successorBlock) == 0)
								work.push(*successorBlock);
						}
					}
				}

				for (set<Instruction*>::iterator it = toMoveSet.begin(); it != toMoveSet.end(); ++it) // moving
				{
					Instruction* instructionToMove = *it;
					Instruction* preheaderEnd = &(preheader->back()); // insert before last instruction of preheader
					instructionToMove->removeFromParent();
					instructionToMove->insertBefore(preheaderEnd);
				}
				return motionApplied;
			}
			
		public:
			static char ID;
		
			LICM() : FunctionPass(ID) 
			{						  }
		
			void getAnalysisUsage(AnalysisUsage& AU) const
			{
				AU.setPreservesCFG();
				AU.addRequired<LoopInfoWrapperPass>();
				AU.addRequired<DominatorTreeWrapperPass>();
			}
		
			virtual bool runOnFunction(Function &F)				
			{	
				bool modified = false;
				LoopInfo &LI = getAnalysis<LoopInfoWrapperPass>().getLoopInfo();
				assignIndicesToInstrs(&F);
				
				for (LoopInfo::reverse_iterator I = LI.rbegin(), E = LI.rend(); I != E; ++I)
					addLoopIntoQueue(*I);
					
				while (!LQ.empty())
				{
					Loop* L  = LQ.back();
					
					if (L->getLoopPreheader() == NULL)
						return false;
							
					set<Value*> loopInvariantStatements = computeLoopInvariantStatements(L,F);

					std::vector<BasicBlock*> loopBlocks = L->getBlocks();
					SmallVector<BasicBlock*, 0> ExitBlocks;
					L->getExitBlocks(ExitBlocks);

					DominatorTreeWrapperPass *DTWP = getAnalysisIfAvailable<DominatorTreeWrapperPass>();
					DominatorTree *DT = DTWP ? &DTWP->getDomTree() : nullptr;
					
					set<Value*> motionCandidates;
					
					for (std::set<Value*>::iterator i1 = loopInvariantStatements.begin(); i1 != loopInvariantStatements.end(); ++i1)
					{
						Instruction *I = static_cast<Instruction*>(*i1);
						bool dominatesCheck = true;
						
						for (int i2=0; i2<ExitBlocks.size(); i2++)
							if (!DT->dominates(I->getParent(), ExitBlocks[i2]))
								dominatesCheck = false;
								
						if (dominatesCheck && checkFunctionCalls(L,I))
						{
							bool dominatesAllUseBlocksInLoop = true;
							Value* assignedVar = getDefinitionVar(I);
							if (assignedVar) 
							{
								for (Value::use_iterator ui = assignedVar->use_begin(), e = assignedVar->use_end(); ui != e; ++ui)
								{
									if (Instruction* userInstruction = dyn_cast<Instruction>(*ui))
									{
										BasicBlock* userBlock = userInstruction->getParent();
										if (L->contains(userBlock) && !DT->dominates(I->getParent(), userBlock))
										{
											dominatesAllUseBlocksInLoop = false;
											break;
										}
									}
								}
								if (dominatesAllUseBlocksInLoop)
								{
									bool hasNoOtherAssignmentsInLoop = true;
									string assignedVarStr = valueToDefinitionVarStr(assignedVar);
									for (std::vector<BasicBlock*>::iterator it1 = loopBlocks.begin(); it1 != loopBlocks.end(); ++it1)
									{
										for (BasicBlock::iterator it2 = (*it1)->begin(), e = (*it1)->end(); it2 != e; ++it2)
										{
											if (&*it2!=&*I && valueToDefinitionVarStr(&*it2) == assignedVarStr)
											{
												hasNoOtherAssignmentsInLoop = false;
												break;
											}
										}
										if (hasNoOtherAssignmentsInLoop)
											break;
									}
									if (hasNoOtherAssignmentsInLoop)
										motionCandidates.insert(I);
								}
							}
						}
					}
						
					bool loopModified = applyMotionToCandidates(L, motionCandidates);
					modified |= loopModified;
					LQ.pop_back();
				}
			}
	};
}

char LICM::ID = 0;			// these two lines are always added to carry out the pass
static RegisterPass<LICM> temp("newlicm","- to do loop invariant code motion");

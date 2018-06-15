//#include "loop-invariant-code-motion.h"

#include "llvm/IR/Function.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/ADT/SmallPtrSet.h"

#include "llvm/Pass.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/IR/ValueMap.h"
#include "llvm/ADT/SmallVector.h"
	
#include "llvm/Analysis/ValueTracking.h"

#include "dataflow.h"
#include "reachingdefs.cpp"

#include <iomanip>
#include <queue>
#include <deque>
#include <stack>
#include <map>

using namespace llvm;
using namespace std;

namespace
{
	class DominanceDataFlow : public DataFlow
	{
		protected:
		BitVector applyMeetOp(BitVectorList meetInputs)
		{
			BitVector meetResult;
			if (!meetInputs.empty())	// meet op = intersection of inputs for dominance
			{
				for (int i = 0; i < meetInputs.size(); i++) 
				{
					if (i == 0)
						meetResult = meetInputs[i];
					else
						meetResult &= meetInputs[i];
				}
			}
			return meetResult;
		}
//virtual TransferOutput transferFn(BitVector input, std::vector<void*> domain, std::map<void*, int> domainToIndex, BasicBlock* block) = 0;
		virtual TransferOutput transferFn(BitVector value, vector<void*> domain, map<void*, int> domainToIndex, BasicBlock* block)
		{
			TransferOutput transfer;
			transfer.element = value;
			unsigned blockIdx = domainToIndex[(void *)block];	// add the current block to the dominance set
			transfer.element.set(blockIdx);
			return transfer;
		}
		
		public:
		DominanceDataFlow(Direction d, MeetOp m) : DataFlow(d,m){ }
		DataFlowResult run(Function &F, std::vector<void*> domain, BitVector boundaryCond, BitVector initCond)
		{
			return DataFlow::run(F, domain, boundaryCond, initCond);
		}
		
	};

	bool dominates(BasicBlock* A, BasicBlock* B, DataFlowResult dominanceResults)	// check if A dominates B
	{
		BlockResult dominanceOfB = dominanceResults.result[B];
		bool aDomsB = dominanceOfB.out[dominanceResults.domainToIndex[(void*)A]];
		return aDomsB;
	}

	class LoopInvariantCodeMotion : public FunctionPass
	{
		public:
			static char ID;

			LoopInvariantCodeMotion() : FunctionPass(ID) { }

			bool doInitialization(Module& M)
			{
			  return false;
			}

			virtual bool runOnFunction(Function& F)
			{
  				bool modified = false;
				//map<Value*, ReachingDefinitionInfo> reachingDefs = ReachingDefinitions().computeReachingDefinitions(F); // rdefs info

				//LoopInfo& LI = getAnalysis<LoopInfo>();		// add loops to queue
				LoopInfo &LI = getAnalysis<LoopInfoWrapperPass>().getLoopInfo();
				for (LoopInfo::reverse_iterator I = LI.rbegin(), E = LI.rend(); I != E; ++I)
					addLoopIntoQueue(*I);

				while (!LQ.empty()) // apply LICM to each loop in queue
				{
					Loop* L  = LQ.back();
					
					if (L->getLoopPreheader() == NULL)	// loops without a preheader
						return false;

					DataFlowResult dominanceResults = computeDominance(L, F);

					map<BasicBlock*, BasicBlock*> immDoms;
					immDoms = computeImmediateDominance(dominanceResults);
					printDominanceInfo(dominanceResults, immDoms);
					
					set<Value*> loopInvariantStatements = computeLoopInvariantStatements(L, RDAtF[F.getName().str()]);
					for (std::set<Value*>::iterator liIter = loopInvariantStatements.begin(); liIter != loopInvariantStatements.end(); ++liIter)
				errs() <<"value to str of li iter : "<< valueToStr(*liIter) << "\n";
				
					set<Value*> codeMotionCandidateStatements = computeCodeMotionCandidateStatements(L,dominanceResults, loopInvariantStatements);
			
					bool loopModified = applyMotionToCandidates(L, codeMotionCandidateStatements);
					modified |= loopModified;
					LQ.pop_back();
  				}

				return modified;
			}		

			void getAnalysisUsage(AnalysisUsage& AU) const 
			{
				AU.addRequired<LoopInfoWrapperPass>();
			}

		protected:
		
			deque<Loop *> LQ;
			
			SmallPtrSet<BasicBlock*, 32> getLoopExits(Loop* L) 
			{
				SmallVector<BasicBlock*, 32> loopSuccessors;
				L->getUniqueExitBlocks(loopSuccessors);

				SmallPtrSet<BasicBlock*, 32> loopExits;
				for (SmallVector<BasicBlock*, 32>::iterator i = loopSuccessors.begin(); i < loopSuccessors.end(); ++i) 
					loopExits.insert(*pred_begin(*i)); // result of the loop-simplify pass - each out-of-loop successor's sole predecessor
														// should be part of this loop
			  	return loopExits;
			}

			DataFlowResult computeDominance(Loop* L, Function& F)
			{
				
				std::set<BasicBlock*> blocksSet; // domain - set of all basic blocks in the loop, their parents
				std::vector<BasicBlock*> loopBlocks = L->getBlocks();
				for (std::vector<BasicBlock*>::iterator blockIter = loopBlocks.begin(); blockIter != loopBlocks.end(); ++blockIter)
				{	
					BasicBlock* block = *blockIter;
					for (pred_iterator predBlock = pred_begin(block), E = pred_end(block); predBlock != E; ++predBlock)
					  blocksSet.insert(*predBlock);	// add parents
					  
					blocksSet.insert(block); // add block
				}

				std::vector<void*> domain;
				std::vector<BasicBlock*> blocks;
				for (std::set<BasicBlock*>::iterator it = blocksSet.begin(); it != blocksSet.end(); ++it) 
				{
					blocks.push_back(*it);
					domain.push_back((void*)*it);
				}

				int numVars = domain.size();
				BitVector boundaryCond(numVars, false);	// boundary value at entry is entry block (entry dominates itself)
				BitVector initInteriorCond(numVars, true);	// initial interior set is full all blocks
				DominanceDataFlow flow(FORWARD,INTERSECTION); // = new DominanceDataFlow(FORWARD, INTERSECTION);	// dataflow values at IN and OUT of each block
				return flow.run(F, domain, boundaryCond, initInteriorCond);
				
			}

		map<BasicBlock*, BasicBlock*> computeImmediateDominance(DataFlowResult dominanceResults)
		{
			map<BasicBlock*, BasicBlock*> immDoms;	// find the immediate dominators 
			for (map<BasicBlock*,  BlockResult>::iterator resultsIter = dominanceResults.result.begin();resultsIter != dominanceResults.result.end(); ++resultsIter)
			{
				BlockResult& blockResult = resultsIter->second;
				BitVector visited(dominanceResults.result.size(), false);
				std::queue<BasicBlock*> work;
				work.push(resultsIter->first);
				while (!work.empty())
				{
					BasicBlock* currAncestor = work.front();
					work.pop();
					int currIdx = dominanceResults.domainToIndex[currAncestor];
					visited.set(currIdx);

					if (blockResult.in[currIdx])	// if ancestor is contained in dom set for the results block, mark as imm and quit
					{
						immDoms[resultsIter->first] = currAncestor;
						break;
					}

					for (pred_iterator predBlock = pred_begin(currAncestor), E = pred_end(currAncestor); predBlock != E; ++predBlock)
					{
						int predIdx = dominanceResults.domainToIndex[*predBlock];
						if (!visited[predIdx])
							work.push(*predBlock);
					}
				}
			}

			return immDoms;
		}

		void printDominanceInfo(DataFlowResult dominanceResults, map<BasicBlock*, BasicBlock*> immDoms)
		{
			errs() << "Dominance domain: {";	// print immediate dominance information
			for (map<BasicBlock*, BlockResult>::iterator resultsIter = dominanceResults.result.begin(); resultsIter != dominanceResults.result.end(); ++resultsIter)
			errs() << resultsIter->first->getName() << "  ";
			errs() << "}\n";
			errs() << "\nImmediate Dominance Relationships: \n";

			for (map<BasicBlock*, BlockResult>::iterator resultsIter = dominanceResults.result.begin(); resultsIter != dominanceResults.result.end(); ++resultsIter)
			{
				char str[100];
				BasicBlock* idom = immDoms[resultsIter->first];
				if (idom)
				{
				  sprintf(str,"%s is idom'd by %s",((std::string)resultsIter->first->getName()).c_str(),((std::string)idom->getName()).c_str());
				  errs() << str << "\n";
				}
				else {
				  sprintf(str, "%s has no idom", ((std::string)resultsIter->first->getName()).c_str());
				  errs() << str << "\n";
				}
			}
			errs() << "\n";
		}

		//std::set<Value*> computeLoopInvariantStatements(Loop* L, map<Value*, ReachingDefinitionInfo> reachingDefs)
		std::set<Value*> computeLoopInvariantStatements(Loop* L, ItoV reachdefinfo)
		{
			std::set<Value*> loopInvariantStatements;
			std::vector<BasicBlock*> loopBlocks = L->getBlocks();

			for (std::vector<BasicBlock*>::iterator blockIter = loopBlocks.begin(); blockIter != loopBlocks.end(); ++blockIter)
			{																						// initialize invariant statement set
				for (BasicBlock::iterator instIter = (*blockIter)->begin(), e = (*blockIter)->end(); instIter != e; ++instIter)
				{
					Value* v = &*instIter;

					if (isa<Constant>(v) || isa<Argument>(v) || isa<GlobalValue>(v))	// invariance
						loopInvariantStatements.insert(v);
					else if (isa<Instruction>(v))	// statements with its operands having reaching defs outside loop
					{
						Instruction* I = static_cast<Instruction*>(v);

						bool mightBeLoopInvariant = (isSafeToSpeculativelyExecute(I) && !I->mayReadFromMemory() && !isa<LandingPadInst>(I));

						if (mightBeLoopInvariant)
						{
							bool allOperandsOnlyDefinedOutsideLoop = true;

							for (User::op_iterator opIter = I->op_begin(), e = I->op_end(); opIter != e; ++opIter)
							{
								Value* opVal = *opIter;
								//ReachingDefinitionInfo varDefsInfo = reachdefinfo[opVal];

								//vector<Value*> varDefsAtStatement = varDefsInfo.defsByPoint[I];
								vector<Value*> varDefsAtStatement = reachdefinfo[InstrToIndex[&*I]];
								for (int i = 0; i < varDefsAtStatement.size(); i++) 
								{
									if (isa<Instruction>(varDefsAtStatement[i])) 
									{
										if (L->contains(((Instruction*)varDefsAtStatement[i])->getParent()))  
										{
											allOperandsOnlyDefinedOutsideLoop = false;
											break;
										}
									}
								}

								if (!allOperandsOnlyDefinedOutsideLoop)
									break;
							}

							if (allOperandsOnlyDefinedOutsideLoop)
							loopInvariantStatements.insert(v);
						}
					}
				}
			}

			bool converged = false;		// iteratively update invariant statement set until convergence
			int invariantSetSize = loopInvariantStatements.size();
			while (!converged)
			{
				int prevInvariantSetSize = invariantSetSize;

				for (std::vector<BasicBlock*>::iterator blockIter = loopBlocks.begin(); blockIter != loopBlocks.end(); ++blockIter)
				{	
					for (BasicBlock::iterator instIter = (*blockIter)->begin(), e = (*blockIter)->end(); instIter != e; ++instIter)
					{
						Value* v = &*instIter;	// adding statements with an operand which is loop invariant itself

						if (loopInvariantStatements.find(v) != loopInvariantStatements.end()) // if invariant, stop
							continue;

						if (isa<Instruction>(v))
						{
							Instruction* I = static_cast<Instruction*>(v);

							bool mightBeLoopInvariant=(isSafeToSpeculativelyExecute(I)&&!I->mayReadFromMemory()&&!isa<LandingPadInst>(I));

							if (mightBeLoopInvariant)
							{
								bool allOperandsHaveSingleLoopInvariantDef = true;

								for (User::op_iterator opIter = I->op_begin(), e = I->op_end(); opIter != e; ++opIter)
								{
									Value* opVal = *opIter;	// see if operand has single loop-invariant definition
									//ReachingDefinitionInfo varDefsInfo = reachingDefs[opVal]; 

									//vector<Value*> varDefsAtStatement = varDefsInfo.defsByPoint[I];
									vector<Value*> varDefsAtStatement = reachdefinfo[InstrToIndex[&*I]];
									if (varDefsAtStatement.size() != 1 || loopInvariantStatements.count(varDefsAtStatement[0]) == 0)
									{
										allOperandsHaveSingleLoopInvariantDef = false;
										break;
									}
								}

								if (allOperandsHaveSingleLoopInvariantDef)
									loopInvariantStatements.insert(v);
							}
						}
					}
				}

				invariantSetSize = loopInvariantStatements.size();
				converged = (invariantSetSize == prevInvariantSetSize);
			}

			errs() << "Loop invariant statements: {\n";
			for (std::set<Value*>::iterator liIter = loopInvariantStatements.begin(); liIter != loopInvariantStatements.end(); ++liIter)
				errs() << valueToStr(*liIter) << "\n";
			errs() << "}\n\n";
			return loopInvariantStatements;
		}

		set<Value*> computeCodeMotionCandidateStatements(Loop* L, DataFlowResult dominanceResults, set<Value*> invariantStatements)
		{
			errs()<<"code motion candidate statements 0\n";
			set<Value*> motionCandidates;
			errs()<<"code motion candidate statements 1\n";
			// statements for LICM must : be loop invariant, be in a block that dominates all exits of loop, be in a block that 
			// dominates all blocks in loop where statement is used, assign to a variable that has no other assignments in the loop

			std::vector<BasicBlock*> loopBlocks = L->getBlocks();
			SmallPtrSet<BasicBlock*, 32> loopExits = getLoopExits(L);

			for (std::vector<BasicBlock*>::iterator blockIter = loopBlocks.begin(); blockIter != loopBlocks.end(); ++blockIter)
			{
				for (BasicBlock::iterator instIter = (*blockIter)->begin(), e = (*blockIter)->end(); instIter != e; ++instIter)
				{
					Instruction* I = &*instIter;

					if (invariantStatements.count(I) == 0)
					continue;

					bool isInExitDominatingBlock = true;	// check exit dominance
					for (SmallPtrSet<BasicBlock*,32>::iterator loopExitIter=loopExits.begin();loopExitIter!=loopExits.end();++loopExitIter) 					{	
						if (!dominates(*blockIter, *loopExitIter, dominanceResults)) // if block doens't dominate exit, not exit dominator
						{
							isInExitDominatingBlock = false;
							break;
						}
					}
					if (!isInExitDominatingBlock)
						continue;

					bool dominatesAllUseBlocksInLoop = true; // check if statement dominates other uses of assigned variable in the block
					Value* assignedVar = getDefinitionVar(I);
					if (assignedVar)
					{
						for (Value::use_iterator useIter = assignedVar->use_begin(), e = assignedVar->use_end(); useIter != e; ++useIter)
						{
							if (Instruction* userInstruction = dyn_cast<Instruction>(*useIter)) 
							{
								BasicBlock* userBlock = userInstruction->getParent();
								if (L->contains(userBlock) && !dominates(*blockIter, userBlock, dominanceResults)) 
								{
									dominatesAllUseBlocksInLoop = false;
									break;
								}
							}
						}
					}
					if (!dominatesAllUseBlocksInLoop)
						continue;

					bool hasNoOtherAssignmentsInLoop = true; // see if assigned variable has other assignments
					if (assignedVar)
					{
						string assignedVarStr = valueToDefinitionVarStr(assignedVar);
						for (std::vector<BasicBlock*>::iterator blockIter = loopBlocks.begin(); blockIter != loopBlocks.end(); ++blockIter) 						{
			for (BasicBlock::iterator otherInstIter = (*blockIter)->begin(), e = (*blockIter)->end(); otherInstIter != e; ++otherInstIter)
			{
							if (otherInstIter != instIter && valueToDefinitionVarStr(&*otherInstIter) == assignedVarStr)
							{
								hasNoOtherAssignmentsInLoop = false;
								break;
							}
			}
							if (hasNoOtherAssignmentsInLoop)
								break;
						}
					}
					if (!hasNoOtherAssignmentsInLoop)
						continue;
						
					motionCandidates.insert(I);
				}
			}
			return motionCandidates;
		}

		bool applyMotionToCandidates(Loop* L, set<Value*> motionCandidates) 
		{
			bool motionApplied = false;					  // do DFS over the blocks of the loop and move each candidate to end of preheader 
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

		void addLoopIntoQueue(Loop* L)
		{
			this->LQ.push_back(L);
			for (Loop::reverse_iterator I = L->rbegin(), E = L->rend(); I != E; ++I)
				addLoopIntoQueue(*I); 
		}
	};	
}
char LoopInvariantCodeMotion::ID = 0;			// these two lines are always added to carry out the pass
static RegisterPass<LoopInvariantCodeMotion> temp("loopicm","- to do LICM");

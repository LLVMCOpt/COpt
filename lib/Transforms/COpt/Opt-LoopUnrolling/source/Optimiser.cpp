#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Metadata.h"
#include "llvm/Analysis/LoopPass.h"
#include "llvm/Analysis/LoopUnrollAnalyzer.h"
#include "llvm/Analysis/TargetTransformInfo.h"
#include "llvm/Analysis/AssumptionCache.h"
#include "llvm/Analysis/ScalarEvolution.h"
#include "llvm/Analysis/OptimizationDiagnosticInfo.h"
#include "llvm/Analysis/CodeMetrics.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Transforms/Utils/Local.h"
#include "llvm/Transforms/Utils/LoopUtils.h"
#include "llvm/Transforms/Utils/UnrollLoop.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Support/raw_ostream.h"
#include <cstring>

using namespace llvm;
using namespace std;
using namespace ore;

static const unsigned NoThreshold = UINT_MAX;
unsigned PragmaUnrollThreshold = 16*1024;
unsigned FlatLoopTripCountThreshold = 5;
unsigned UnrollMaxIterationsCountToAnalyze = 10;

namespace
{
	struct EstimatedUnrollCost
	{
		unsigned UnrolledCost; // estimated cost after unrolling
		unsigned RolledDynamicCost; // estimated dynamic cost of executing the instructions in the rolled form
	};
	struct UnrolledInstState
	{
		Instruction *I;
		int Iteration : 30;
		unsigned IsFree : 1;
		unsigned IsCounted : 1;
	};
	struct UnrolledInstStateKeyInfo
	{
		typedef DenseMapInfo<Instruction *> PtrInfo;
		typedef DenseMapInfo<std::pair<Instruction *, int>> PairInfo;
		static inline UnrolledInstState getEmptyKey()
		{
			return {PtrInfo::getEmptyKey(), 0, 0, 0};
		}
		static inline UnrolledInstState getTombstoneKey()
		{
			return {PtrInfo::getTombstoneKey(), 0, 0, 0};
		}
		static inline unsigned getHashValue(const UnrolledInstState &S)
		{
			return PairInfo::getHashValue({S.I, S.Iteration});
		}
		static inline bool isEqual(const UnrolledInstState &LHS, const UnrolledInstState &RHS)
		{
			return PairInfo::isEqual({LHS.I, LHS.Iteration}, {RHS.I, RHS.Iteration});
		}
	};
	class LoopUnrolling : public LoopPass
	{
		public:
		static char ID;

		LoopUnrolling() : LoopPass(ID)
		{				}

		virtual bool runOnLoop(Loop *L, LPPassManager &LPM)
		{
			if (skipLoop(L))
				return false;

			Function &F = *L->getHeader()->getParent();

			auto &DT = getAnalysis<DominatorTreeWrapperPass>().getDomTree();
			LoopInfo *LI = &getAnalysis<LoopInfoWrapperPass>().getLoopInfo();
			ScalarEvolution *SE = &getAnalysis<ScalarEvolutionWrapperPass>().getSE();
			const TargetTransformInfo &TTI = getAnalysis<TargetTransformInfoWrapperPass>().getTTI(F);
			auto &AC = getAnalysis<AssumptionCacheTracker>().getAssumptionCache(F);
			OptimizationRemarkEmitter ORE(&F);
			bool PreserveLCSSA = mustPreserveAnalysisID(LCSSAID);

			if (countNoOfInstrInLoop(L)<10)
			return tryToUnrollLoop(L, DT, LI, SE, TTI, AC, ORE, PreserveLCSSA);
		}

int countNoOfInstrInLoop(Loop *L)
		{
			int count = 0;
			for(Loop::block_iterator block = L->block_begin(), y = L->block_end(); block!= y; ++block)
			{
				for (BasicBlock::iterator i = (*block)->begin(), e = (*block)->end(); i!=e; ++i)
					count++;
			}
			return count;
		}
		void getAnalysisUsage(AnalysisUsage& AU) const
		{
			AU.addRequired<AssumptionCacheTracker>();
			AU.addRequired<TargetTransformInfoWrapperPass>();
			AU.addRequiredID(LCSSAID);
			getLoopAnalysisUsage(AU);
		}
		
		static Optional<EstimatedUnrollCost> analyzeLoopUnrollCost(const Loop *L, unsigned TripCount, DominatorTree &DT, ScalarEvolution &SE, const TargetTransformInfo &TTI, unsigned MaxUnrolledLoopSize)
		{
			assert(UnrollMaxIterationsCountToAnalyze < (INT_MAX / 2) && "The unroll iterations max is too large!");
			
			if (!L->empty())
				return None;
				
			if (!UnrollMaxIterationsCountToAnalyze || !TripCount || TripCount > UnrollMaxIterationsCountToAnalyze)
				return None; // loops with a big or unknown trip count

			SmallSetVector<BasicBlock *, 16> BBWorklist;
			SmallSetVector<std::pair<BasicBlock *, BasicBlock *>, 4> ExitWorklist;
			DenseMap<Value *, Constant *> SimplifiedValues;
			SmallVector<std::pair<Value *, Constant *>, 4> SimplifiedInputValues;
			
			unsigned UnrolledCost = 0;
			unsigned RolledDynamicCost = 0;
			
			DenseSet<UnrolledInstState, UnrolledInstStateKeyInfo> InstCostMap;
			SmallVector<Instruction *, 16> CostWorklist;
			SmallVector<Instruction *, 4> PHIUsedList;

			auto AddCostRecursively = [&](Instruction &RootI, int Iteration) // function to accumulate cost for instructions in the loop
			{
				assert(Iteration >= 0 && "Cannot have a negative iteration!");
				assert(CostWorklist.empty() && "Must start with an empty cost list");
				assert(PHIUsedList.empty() && "Must start with an empty phi used list");
				CostWorklist.push_back(&RootI);
				for (;; --Iteration)
				{
					do 
					{
						Instruction *I = CostWorklist.pop_back_val();
						auto CostIter = InstCostMap.find({I, Iteration, 0, 0});
						if (CostIter == InstCostMap.end())
							continue;
						auto &Cost = *CostIter;
						if (Cost.IsCounted)
							continue;
							
						Cost.IsCounted = true; // counting the cost of this instruction now

						if (auto *PhiI = dyn_cast<PHINode>(I)) // if there is PHI node in the loop header, add to PHI set
							if (PhiI->getParent() == L->getHeader())
							{
								assert(Cost.IsFree && "Loop PHIs shouldn't be evaluated as they inherently simplify during unrolling.");
								if (Iteration == 0)
									continue;

								if (auto *OpI = dyn_cast<Instruction>(PhiI->getIncomingValueForBlock(L->getLoopLatch())))
									if (L->contains(OpI))
										PHIUsedList.push_back(OpI);
								continue;
							}

						if (!Cost.IsFree) // accumulate the cost of this instruction
							UnrolledCost += TTI.getUserCost(I);
							
						for (Value *Op : I->operands())
						{
							auto *OpI = dyn_cast<Instruction>(Op); // check if operand is free due to being a constant or outside the loop
							if (!OpI || !L->contains(OpI))
								continue;
							CostWorklist.push_back(OpI); // accumulate its cost
						}
					} while (!CostWorklist.empty());

					if (PHIUsedList.empty()) // search exhausted
						break;

					assert(Iteration > 0 && "Cannot track PHI-used values past the first iteration!");
					CostWorklist.append(PHIUsedList.begin(), PHIUsedList.end());
					PHIUsedList.clear();
				}
			};
			
			assert(L->isLoopSimplifyForm() && "Must put loop into normal form first.");
			assert(L->isLCSSAForm(DT) && "Must have loops in LCSSA form to track live-out values.");

			for (unsigned Iteration = 0; Iteration < TripCount; ++Iteration)
			{
				for (Instruction &I : *L->getHeader()) // collect any simplified entry or backedge inputs
				{
					auto *PHI = dyn_cast<PHINode>(&I);
					if (!PHI)
						break;
						
					assert(PHI->getNumIncomingValues() == 2 && "Must have an incoming value only for the preheader and the latch.");

					Value *V = PHI->getIncomingValueForBlock(Iteration == 0 ? L->getLoopPreheader() : L->getLoopLatch());
					Constant *C = dyn_cast<Constant>(V);
					if (Iteration != 0 && !C)
						C = SimplifiedValues.lookup(V);
					if (C)
						SimplifiedInputValues.push_back({PHI, C});
				}

				SimplifiedValues.clear(); // clear and re-populate the map for the next iteration
				while (!SimplifiedInputValues.empty())
					SimplifiedValues.insert(SimplifiedInputValues.pop_back_val());

				UnrolledInstAnalyzer Analyzer(Iteration, SimplifiedValues, SE, L);

				BBWorklist.clear();
				BBWorklist.insert(L->getHeader());
				
				for (unsigned Idx = 0; Idx != BBWorklist.size(); ++Idx)
				{
					BasicBlock *BB = BBWorklist[Idx];

					// Visit all instructions in the given basic block and try to simplify
					// it.  We don't change the actual IR, just count optimization
					// opportunities.
					for (Instruction &I : *BB)
					{
						if (isa<DbgInfoIntrinsic>(I))
							continue;

						RolledDynamicCost += TTI.getUserCost(&I);
						bool IsFree = Analyzer.visit(I);
						bool Inserted = InstCostMap.insert({&I, (int)Iteration, (unsigned)IsFree, /*IsCounted*/ false}).second;
						(void)Inserted;
						assert(Inserted && "Cannot have a state for an unvisited instruction!");

						if (IsFree)
							continue;
						if(isa<CallInst>(&I))
							return None;
						if (I.mayHaveSideEffects())
							AddCostRecursively(I, Iteration);
						if (UnrolledCost > MaxUnrolledLoopSize)
							return None;
					}

					TerminatorInst *TI = BB->getTerminator();
					BasicBlock *KnownSucc = nullptr;
					if (BranchInst *BI = dyn_cast<BranchInst>(TI))
					{
						if (BI->isConditional())
						{
							if (Constant *SimpleCond = SimplifiedValues.lookup(BI->getCondition()))
							{
								if (isa<UndefValue>(SimpleCond))
									KnownSucc = BI->getSuccessor(0);
								else if (ConstantInt *SimpleCondVal = dyn_cast<ConstantInt>(SimpleCond))
									KnownSucc = BI->getSuccessor(SimpleCondVal->isZero() ? 1 : 0);
							}
						}
					}
					else if (SwitchInst *SI = dyn_cast<SwitchInst>(TI))
					{
						if (Constant *SimpleCond = SimplifiedValues.lookup(SI->getCondition()))
						{
							if (isa<UndefValue>(SimpleCond))
								KnownSucc = SI->getSuccessor(0);
							else if (ConstantInt *SimpleCondVal = dyn_cast<ConstantInt>(SimpleCond))
								KnownSucc = SI->findCaseValue(SimpleCondVal).getCaseSuccessor();
						}
					}
					if (KnownSucc)
					{
						if (L->contains(KnownSucc))
							BBWorklist.insert(KnownSucc);
						else
							ExitWorklist.insert({BB, KnownSucc});
						continue;
					}

					for (BasicBlock *Succ : successors(BB)) // add BB's successors to the worklist
						if (L->contains(Succ))
							BBWorklist.insert(Succ);
						else
							ExitWorklist.insert({BB, Succ});
					AddCostRecursively(*TI, Iteration);
				}
				if (UnrolledCost == RolledDynamicCost)
					return None;
			}

			while (!ExitWorklist.empty())
			{
				BasicBlock *ExitingBB, *ExitBB;
				std::tie(ExitingBB, ExitBB) = ExitWorklist.pop_back_val();

				for (Instruction &I : *ExitBB)
				{
					auto *PN = dyn_cast<PHINode>(&I);
					if (!PN)
						break;

					Value *Op = PN->getIncomingValueForBlock(ExitingBB);
					if (auto *OpI = dyn_cast<Instruction>(Op))
						if (L->contains(OpI))
							AddCostRecursively(*OpI, TripCount - 1);
				}
			}
			return {{UnrolledCost, RolledDynamicCost}};
		}
		
		static void SetLoopAlreadyUnrolled(Loop *L)
		{
			MDNode *LoopID = L->getLoopID(); // remove existing loop unrolling metadata
			SmallVector<Metadata *, 4> MDs; // reserve first location for self reference to LoopID metadata node.
			MDs.push_back(nullptr);

			if (LoopID)
			{
				for (unsigned i = 1, ie = LoopID->getNumOperands(); i < ie; ++i)
				{
					bool IsUnrollMetadata = false;
					MDNode *MD = dyn_cast<MDNode>(LoopID->getOperand(i));
					if (MD)
					{
						const MDString *S = dyn_cast<MDString>(MD->getOperand(0));
						IsUnrollMetadata = S && S->getString().startswith("llvm.loop.unroll.");
					}
					if (!IsUnrollMetadata)
					MDs.push_back(LoopID->getOperand(i));
				}
			}

			LLVMContext &Context = L->getHeader()->getContext();  // add unroll(disable) metadata to disable future unrolling
			SmallVector<Metadata *, 1> DisableOperands;
			DisableOperands.push_back(MDString::get(Context, "llvm.loop.unroll.disable"));
			MDNode *DisableNode = MDNode::get(Context, DisableOperands);
			MDs.push_back(DisableNode);

			MDNode *NewLoopID = MDNode::get(Context, MDs);
			NewLoopID->replaceOperandWith(0, NewLoopID); // set operand 0 to refer to the loop id itself
			L->setLoopID(NewLoopID);
		}

		static uint64_t getUnrolledLoopSize(unsigned LoopSize,TargetTransformInfo::UnrollingPreferences &UP)
		{
			assert(LoopSize >= UP.BEInsns && "LoopSize should not be less than BEInsns!");
			return (uint64_t)(LoopSize - UP.BEInsns) * UP.Count + UP.BEInsns;
		}
		
		static unsigned UnrollCountPragmaValue(const Loop *L)
		{
			MDNode *MD = GetUnrollMetadataForLoop(L, "llvm.loop.unroll.count");
			if (MD)
			{
				assert(MD->getNumOperands() == 2 && "Unroll count hint metadata should have two operands.");
				unsigned Count = mdconst::extract<ConstantInt>(MD->getOperand(1))->getZExtValue();
				assert(Count >= 1 && "Unroll count must be positive.");
				return Count;
			}
			return 0;
		}
		static MDNode *GetUnrollMetadataForLoop(const Loop *L, StringRef Name)
		{
			if (MDNode *LoopID = L->getLoopID())
				return GetUnrollMetadata(LoopID, Name);
			return nullptr;
		}

		static unsigned getFullUnrollBoostingFactor(const EstimatedUnrollCost &Cost, unsigned MaxPercentThresholdBoost)
		{
			if (Cost.RolledDynamicCost >= UINT_MAX / 100)
				return 100;
			else if (Cost.UnrolledCost != 0)
				return std::min(100 * Cost.RolledDynamicCost / Cost.UnrolledCost, MaxPercentThresholdBoost); // boosting factor
			else
				return MaxPercentThresholdBoost;
		}


		static bool HasUnrollFullPragma(const Loop *L) // returns true if the loop has an unroll(full) pragma
		{
			return GetUnrollMetadataForLoop(L, "llvm.loop.unroll.full");
		}

		static bool HasUnrollEnablePragma(const Loop *L) // returns true if the loop has an unroll(enable) pragma
		{
			return GetUnrollMetadataForLoop(L, "llvm.loop.unroll.enable");
		}

		// Calculates unroll count and writes it to UP.Count. Returns true if unroll count was set explicitly.
		static bool computeUnrollCount(Loop *L, const TargetTransformInfo &TTI, DominatorTree &DT, LoopInfo *LI, ScalarEvolution *SE, OptimizationRemarkEmitter *ORE, unsigned &TripCount,unsigned MaxTripCount, unsigned &TripMultiple, unsigned LoopSize, TargetTransformInfo::UnrollingPreferences &UP, bool &UseUpperBound)
		{
			unsigned PragmaCount = UnrollCountPragmaValue(L); // 1st priority is unroll count set by pragma
			if (PragmaCount > 0)
			{
				UP.Count = PragmaCount;
				UP.Runtime = true;
				UP.AllowExpensiveTripCount = true;
				UP.Force = true;
				if (UP.AllowRemainder && getUnrolledLoopSize(LoopSize, UP)<PragmaUnrollThreshold)
					return true;
			}
	
			bool PragmaFullUnroll = HasUnrollFullPragma(L);
			if (PragmaFullUnroll && TripCount != 0)
			{
				UP.Count = TripCount;
				if (getUnrolledLoopSize(LoopSize, UP)<PragmaUnrollThreshold)
					return false;
			}

			bool PragmaEnableUnroll = HasUnrollEnablePragma(L);
			bool ExplicitUnroll = PragmaCount > 0 || PragmaFullUnroll || PragmaEnableUnroll;

			if (ExplicitUnroll && TripCount != 0)
			{
				UP.Threshold = std::max<unsigned>(UP.Threshold, PragmaUnrollThreshold); // If there is unrolling pragma, be aggressive with
				UP.PartialThreshold = std::max<unsigned>(UP.PartialThreshold, PragmaUnrollThreshold); // unrolling limits
			}

			unsigned ExactTripCount = TripCount; // 2nd priority is full unroll count
			assert((ExactTripCount == 0 || MaxTripCount == 0) && "ExtractTripCound and MaxTripCount cannot both be non zero.");
			unsigned FullUnrollTripCount = ExactTripCount ? ExactTripCount : MaxTripCount;
			UP.Count = FullUnrollTripCount;
			if (FullUnrollTripCount && FullUnrollTripCount <= UP.FullUnrollMaxCount)
			{
				if (getUnrolledLoopSize(LoopSize, UP) < UP.Threshold)
				{
					UseUpperBound = (MaxTripCount == FullUnrollTripCount);
					TripCount = FullUnrollTripCount;
					TripMultiple = UP.UpperBound ? 1 : TripMultiple;
					return ExplicitUnroll;
				}
				else
				{
					if (Optional<EstimatedUnrollCost> Cost = analyzeLoopUnrollCost(L, FullUnrollTripCount, DT, *SE, TTI,UP.Threshold * UP.MaxPercentThresholdBoost / 100))
					{
						unsigned Boost = getFullUnrollBoostingFactor(*Cost, UP.MaxPercentThresholdBoost);
						if (Cost->UnrolledCost < UP.Threshold * Boost / 100)
						{
							UseUpperBound = (MaxTripCount == FullUnrollTripCount);
							TripCount = FullUnrollTripCount;
							TripMultiple = UP.UpperBound ? 1 : TripMultiple;
							return ExplicitUnroll;
						}
					}
				}
			}
	
			if (TripCount)  // 3rd priority is partial unrolling
			{
				UP.Partial |= ExplicitUnroll;
				if (!UP.Partial)
				{
					UP.Count = 0;
					return false;
				}
				if (UP.Count == 0)
					UP.Count = TripCount;
				if (UP.PartialThreshold != NoThreshold) // new count is modulo of trip count
				{
					if (getUnrolledLoopSize(LoopSize, UP) > UP.PartialThreshold)
						UP.Count = (std::max(UP.PartialThreshold, UP.BEInsns + 1) - UP.BEInsns)/(LoopSize - UP.BEInsns);
					if (UP.Count > UP.MaxCount)
						UP.Count = UP.MaxCount;
					while (UP.Count != 0 && TripCount % UP.Count != 0)
						UP.Count--;
					if (UP.AllowRemainder && UP.Count <= 1)
					{
						UP.Count = UP.DefaultUnrollRuntimeCount;
						while (UP.Count != 0 && getUnrolledLoopSize(LoopSize, UP) > UP.PartialThreshold)
							UP.Count >>= 1;
					}
					if (UP.Count < 2)
						UP.Count = 0;
				}
				else
					UP.Count = TripCount;
				return ExplicitUnroll;
			}
			assert(TripCount == 0 && "All cases when TripCount is constant should be covered here.");
			
			computePeelCount(L, LoopSize, UP); // 4th priority is loop peeling
			if (UP.PeelCount)
			{
				UP.Runtime = false;
				UP.Count = 1;
				return ExplicitUnroll;
			}
			if (L->getHeader()->getParent()->getEntryCount()) // 5th priority is runtime unrolling
				if (auto ProfileTripCount = getLoopEstimatedTripCount(L)) 
					if (*ProfileTripCount < FlatLoopTripCountThreshold)
						return false;
					else
						UP.AllowExpensiveTripCount = true;  
	
			UP.Runtime |= PragmaEnableUnroll || PragmaCount > 0; // Reduce count based on type of unrolling and threshold values
			if (!UP.Runtime)
			{
				UP.Count = 0;
				return false;
			}
			if (UP.Count == 0)
				UP.Count = UP.DefaultUnrollRuntimeCount;
		
			while (UP.Count != 0 && getUnrolledLoopSize(LoopSize, UP) > UP.PartialThreshold)
				UP.Count >>= 1;
			
			if (!UP.AllowRemainder && UP.Count != 0 && (TripMultiple % UP.Count) != 0)
			{
				while (UP.Count != 0 && TripMultiple % UP.Count != 0)
					UP.Count >>= 1;
			}

			if (UP.Count > UP.MaxCount)
				UP.Count = UP.MaxCount;
			if (UP.Count < 2)
				UP.Count = 0;
			return ExplicitUnroll;
		}
		
		static unsigned ApproximateLoopSize(const Loop *L, unsigned &NumCalls, bool &NotDuplicatable, bool &Convergent, const TargetTransformInfo &TTI, AssumptionCache *AC, unsigned BEInsns)
		{
			SmallPtrSet<const Value *, 32> EphValues;
			CodeMetrics::collectEphemeralValues(L, AC, EphValues);

			CodeMetrics Metrics;
			for (BasicBlock *BB : L->blocks())
				Metrics.analyzeBasicBlock(BB, TTI, EphValues);
			NumCalls = Metrics.NumInlineCandidates;
			NotDuplicatable = Metrics.notDuplicatable;
			Convergent = Metrics.convergent;

			unsigned LoopSize = Metrics.NumInsts;
			LoopSize = std::max(LoopSize, BEInsns + 1);	// do not allow an estimate of loop size zero

			return LoopSize;
		}
		
		static bool tryToUnrollLoop(Loop *L, DominatorTree &DT, LoopInfo *LI, ScalarEvolution *SE, const TargetTransformInfo &TTI, AssumptionCache &AC, OptimizationRemarkEmitter &ORE, bool PreserveLCSSA)
		{
			if (!L->isLoopSimplifyForm())
				return false;

			unsigned NumInlineCandidates;
			bool NotDuplicatable;
			bool Convergent;
			
			TargetTransformInfo::UnrollingPreferences UP;
			UP.Threshold = 150;
			UP.MaxPercentThresholdBoost = 400;
			UP.OptSizeThreshold = 0;
			UP.PartialThreshold = UP.Threshold;
			UP.PartialOptSizeThreshold = 0;
			UP.Count = 0;
			UP.PeelCount = 0;
			UP.DefaultUnrollRuntimeCount = 8;
			UP.MaxCount = UINT_MAX;
			UP.FullUnrollMaxCount = UINT_MAX;
			UP.BEInsns = 2;
			UP.Partial = false;
			UP.Runtime = false;
			UP.AllowRemainder = true;
			UP.AllowExpensiveTripCount = false;
			UP.Force = false;
			UP.UpperBound = false;
			UP.AllowPeeling = false; 
			unsigned UnrollMaxUpperBound = 8;			if (L->getHeader()->getParent()->optForSize())
			{
				UP.Threshold = UP.OptSizeThreshold;
				UP.PartialThreshold = UP.PartialOptSizeThreshold;
			} 
			
			if (UP.Threshold == 0 && (!UP.Partial || UP.PartialThreshold == 0)) // exit early if unrolling is disabled
				return false;
			unsigned LoopSize = ApproximateLoopSize(L, NumInlineCandidates, NotDuplicatable, Convergent, TTI, &AC, UP.BEInsns);
			
			if (NotDuplicatable)
				return false;

			if (NumInlineCandidates != 0)
				return false;
			
			unsigned TripCount = 0; // find trip count (no of times loop executes)
			unsigned MaxTripCount = 0;
			unsigned TripMultiple = 1; // consider multiple loop exits
			
			BasicBlock *ExitingBlock = L->getLoopLatch();
			if (!ExitingBlock || !L->isLoopExiting(ExitingBlock))
			ExitingBlock = L->getExitingBlock();
			if (ExitingBlock)
			{
				TripCount = SE->getSmallConstantTripCount(L, ExitingBlock);
				TripMultiple = SE->getSmallConstantTripMultiple(L, ExitingBlock);
			}
			
			if (Convergent)
				UP.AllowRemainder = false;

			bool MaxOrZero = false; // find the trip count upper bound if we cannot find the exact trip count
			if (!TripCount) 
			{
				MaxTripCount = SE->getSmallConstantMaxTripCount(L);
				MaxOrZero = SE->isBackedgeTakenCountMaxOrZero(L);
				if (!(UP.UpperBound || MaxOrZero) || MaxTripCount > UnrollMaxUpperBound)
					MaxTripCount = 0;
			}

			// computeUnrollCount() decides whether it is beneficial to use upper bound to fully unroll the loop
			bool UseUpperBound = false;
			bool IsCountSetExplicitly =
						computeUnrollCount(L, TTI, DT, LI, SE, &ORE, TripCount, MaxTripCount, TripMultiple, LoopSize, UP, UseUpperBound);
			if (!UP.Count)
				return false;
			
			if (TripCount && UP.Count > TripCount) // unroll factor (Count) must be less or equal to TripCount.
				UP.Count = TripCount;

			
			if (!UnrollLoop(L, UP.Count, TripCount, UP.Force, UP.Runtime,UP.AllowExpensiveTripCount, UseUpperBound, MaxOrZero, TripMultiple, UP.PeelCount, LI, SE, &DT, &AC, &ORE, PreserveLCSSA)) // unroll the loop
				return false;

			if (IsCountSetExplicitly || UP.PeelCount)
				SetLoopAlreadyUnrolled(L);
			
			return true;
		}
	};
}

char LoopUnrolling::ID = 0;
static RegisterPass<LoopUnrolling> temp("loopunroll","- to do loop unrolling");
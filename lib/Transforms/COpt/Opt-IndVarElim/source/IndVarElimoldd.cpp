#include "llvm/Transforms/Utils/SimplifyIndVar.h"
#include "llvm/Transforms/Utils/LoopUtils.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/LoopPass.h"
#include "llvm/Analysis/ScalarEvolutionExpressions.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/Dominators.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IntrinsicInst.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

namespace
{
	static void pushIVUsers(Instruction *Def, SmallPtrSet<Instruction*,16> &Simplified, SmallVectorImpl<std::pair<Instruction*,Instruction*>> &SimpleIVUsers)
	{
		for (User *U : Def->users())
		{
			Instruction *UI = cast<Instruction>(U);
		
			if (UI != Def && Simplified.insert(UI).second)
				SimpleIVUsers.push_back(std::make_pair(UI, Def));
		}
	}

	
	
	class IndVarElim : public LoopPass
	{
		Loop             *L;
		LoopInfo         *LI;
		ScalarEvolution  *SE;
		DominatorTree    *DT;
		bool Changed;
		SmallVectorImpl<WeakVH> &DeadInsts;

		public:
			static char ID;
		
			IndVarElim() : LoopPass(ID), L(L), LI(LI), SE(SE), DT(DT), DeadInsts(DeadInsts), Changed(false)
			{
				assert(LI && "IV simplification requires LoopInfo");
			}

			bool hasChanged() const { return Changed; }

			static bool isSimpleIVUser(Instruction *I, const Loop *L, ScalarEvolution *SE)
			{
				if (!SE->isSCEVable(I->getType()))
				return false;

				const SCEV *S = SE->getSCEV(I);

				const SCEVAddRecExpr *AR = dyn_cast<SCEVAddRecExpr>(S);
				if (AR && AR->getLoop() == L)
					return true;

				return false;
			}

			void simplifyUsers(PHINode *CurrIV, IVVisitor *V = nullptr)
			{
				if (!SE->isSCEVable(CurrIV->getType()))
					return;

				SmallPtrSet<Instruction*,16> Simplified;
				SmallVector<std::pair<Instruction*, Instruction*>, 8> SimpleIVUsers;

				pushIVUsers(CurrIV, Simplified, SimpleIVUsers);

				while (!SimpleIVUsers.empty())
				{
					std::pair<Instruction*, Instruction*> UseOper = SimpleIVUsers.pop_back_val();
					Instruction *UseInst = UseOper.first;

					if (UseInst == CurrIV) continue;

					Instruction *IVOperand = UseOper.second;
					for (unsigned N = 0; IVOperand; ++N)
					{
						assert(N <= Simplified.size() && "runaway iteration");

						Value *NewOper = foldIVUser(UseOper.first, IVOperand);
						if (!NewOper)
							break; // done folding
						IVOperand = dyn_cast<Instruction>(NewOper);
					}
					if (!IVOperand)
						continue;

					if (eliminateIVUser(UseOper.first, IVOperand))
					{
						pushIVUsers(IVOperand, Simplified, SimpleIVUsers);
						continue;
					}

					if (BinaryOperator *BO = dyn_cast<BinaryOperator>(UseOper.first))
					{
						if (isa<OverflowingBinaryOperator>(BO) && strengthenOverflowingOperation(BO, IVOperand))
							pushIVUsers(IVOperand, Simplified, SimpleIVUsers);
					}

					CastInst *Cast = dyn_cast<CastInst>(UseOper.first);
					if (V && Cast)
					{
						V->visitCast(Cast);
						continue;
					}
					
					if (isSimpleIVUser(UseOper.first, L, SE))
						pushIVUsers(UseOper.first, Simplified, SimpleIVUsers);
				}
			}

			Value *foldIVUser(Instruction *UseInst, Instruction *IVOperand)
			{
				Value *IVSrc = nullptr;
				unsigned OperIdx = 0;
				const SCEV *FoldedExpr = nullptr;
				
				switch (UseInst->getOpcode())
				{
					default:					return nullptr;
					case Instruction::UDiv:
					case Instruction::LShr:		if (IVOperand != UseInst->getOperand(OperIdx) || !isa<ConstantInt>(UseInst->getOperand(1)))
													return nullptr;
												if (!isa<BinaryOperator>(IVOperand) || !isa<ConstantInt>(IVOperand->getOperand(1)))
													return nullptr;

												IVSrc = IVOperand->getOperand(0);
												assert(SE->isSCEVable(IVSrc->getType()) && "Expect SCEVable IV operand");

												ConstantInt *D = cast<ConstantInt>(UseInst->getOperand(1));
												if (UseInst->getOpcode() == Instruction::LShr)
												{
														uint32_t BitWidth = cast<IntegerType>(UseInst->getType())->getBitWidth();
														if (D->getValue().uge(BitWidth))
															return nullptr;

														D = ConstantInt::get(UseInst->getContext(),
														APInt::getOneBitSet(BitWidth, D->getZExtValue()));
												}
												FoldedExpr = SE->getUDivExpr(SE->getSCEV(IVSrc), SE->getSCEV(D));
				}

				if (!SE->isSCEVable(UseInst->getType()))
					return nullptr;
				if (SE->getSCEV(UseInst) != FoldedExpr)
					return nullptr;

				UseInst->setOperand(OperIdx, IVSrc);
				assert(SE->getSCEV(UseInst) == FoldedExpr && "bad SCEV with folded oper");

				Changed = true;
				if (IVOperand->use_empty())
					DeadInsts.emplace_back(IVOperand);
				return IVSrc;
			}

			bool eliminateIdentitySCEV(Instruction *UseInst, Instruction *IVOperand)
			{
				if (!SE->isSCEVable(UseInst->getType()) || (UseInst->getType() != IVOperand->getType()) || (SE->getSCEV(UseInst) != SE->getSCEV(IVOperand)))
					return false;

				if (isa<PHINode>(UseInst))
					if (!DT || !DT->dominates(IVOperand, UseInst))
						return false;

				if (!LI->replacementPreservesLCSSAForm(UseInst, IVOperand))
					return false;

				UseInst->replaceAllUsesWith(IVOperand);
				Changed = true;
				DeadInsts.emplace_back(UseInst);
				return true;
			}

			bool eliminateOverflowIntrinsic(CallInst *CI)
			{
				auto *F = CI->getCalledFunction();
				if (!F)
					return false;

				typedef const SCEV *(ScalarEvolution::*OperationFunctionTy)(const SCEV *, const SCEV *, SCEV::NoWrapFlags);
				typedef const SCEV *(ScalarEvolution::*ExtensionFunctionTy)(const SCEV *, Type *);

				OperationFunctionTy Operation;
				ExtensionFunctionTy Extension;

				Instruction::BinaryOps RawOp;

				bool NoSignedOverflow;

				switch (F->getIntrinsicID())
				{
					default:								return false;

					case Intrinsic::sadd_with_overflow:		Operation = &ScalarEvolution::getAddExpr;
															Extension = &ScalarEvolution::getSignExtendExpr;
															RawOp = Instruction::Add;
															NoSignedOverflow = true;
															break;

					case Intrinsic::uadd_with_overflow:		Operation = &ScalarEvolution::getAddExpr;
															Extension = &ScalarEvolution::getZeroExtendExpr;
															RawOp = Instruction::Add;
															NoSignedOverflow = false;
															break;

					case Intrinsic::ssub_with_overflow:		Operation = &ScalarEvolution::getMinusSCEV;
															Extension = &ScalarEvolution::getSignExtendExpr;
															RawOp = Instruction::Sub;
															NoSignedOverflow = true;
															break;

					case Intrinsic::usub_with_overflow:		Operation = &ScalarEvolution::getMinusSCEV;
															Extension = &ScalarEvolution::getZeroExtendExpr;
															RawOp = Instruction::Sub;
															NoSignedOverflow = false;
															break;
				}

				const SCEV *LHS = SE->getSCEV(CI->getArgOperand(0));
				const SCEV *RHS = SE->getSCEV(CI->getArgOperand(1));

				auto *NarrowTy = cast<IntegerType>(LHS->getType());
				auto *WideTy = IntegerType::get(NarrowTy->getContext(), NarrowTy->getBitWidth() * 2);

				const SCEV *A = (SE->*Extension)((SE->*Operation)(LHS, RHS, SCEV::FlagAnyWrap), WideTy);
				const SCEV *B = (SE->*Operation)((SE->*Extension)(LHS, WideTy), (SE->*Extension)(RHS, WideTy), SCEV::FlagAnyWrap);

				if (A != B)
					return false;

				BinaryOperator *NewResult = BinaryOperator::Create(RawOp, CI->getArgOperand(0), CI->getArgOperand(1), "", CI);

				if (NoSignedOverflow)
					NewResult->setHasNoSignedWrap(true);
				else
					NewResult->setHasNoUnsignedWrap(true);

				SmallVector<ExtractValueInst *, 4> ToDelete;

				for (auto *U : CI->users())
				{
					if (auto *EVI = dyn_cast<ExtractValueInst>(U))
					{
						if (EVI->getIndices()[0] == 1)
							EVI->replaceAllUsesWith(ConstantInt::getFalse(CI->getContext()));
						else
						{
							assert(EVI->getIndices()[0] == 0 && "Only two possibilities!");
							EVI->replaceAllUsesWith(NewResult);
						}
						ToDelete.push_back(EVI);
					}
				}

				for (auto *EVI : ToDelete)
					EVI->eraseFromParent();

				if (CI->use_empty())
					CI->eraseFromParent();

				return true;
			}

			bool eliminateIVUser(Instruction *UseInst, Instruction *IVOperand)
			{
				if (ICmpInst *ICmp = dyn_cast<ICmpInst>(UseInst))
				{
					eliminateIVComparison(ICmp, IVOperand);
					return true;
				}
				if (BinaryOperator *Rem = dyn_cast<BinaryOperator>(UseInst))
				{
					bool IsSigned = Rem->getOpcode() == Instruction::SRem;
					if (IsSigned || Rem->getOpcode() == Instruction::URem)
					{
						eliminateIVRemainder(Rem, IVOperand, IsSigned);
						return true;
					}
				}
				if (auto *CI = dyn_cast<CallInst>(UseInst))
					if (eliminateOverflowIntrinsic(CI))
						return true;
				if (eliminateIdentitySCEV(UseInst, IVOperand))
					return true;
				return false;
			}

			void eliminateIVComparison(ICmpInst *ICmp, Value *IVOperand)
			{
				unsigned IVOperIdx = 0;
				ICmpInst::Predicate Pred = ICmp->getPredicate();
				
				if (IVOperand != ICmp->getOperand(0))
				{
					assert(IVOperand == ICmp->getOperand(1) && "Can't find IVOperand");
					IVOperIdx = 1;
					Pred = ICmpInst::getSwappedPredicate(Pred);
				}

				const SCEV *S = SE->getSCEV(ICmp->getOperand(IVOperIdx));
				const SCEV *X = SE->getSCEV(ICmp->getOperand(1 - IVOperIdx));

				const Loop *ICmpLoop = LI->getLoopFor(ICmp->getParent());
				S = SE->getSCEVAtScope(S, ICmpLoop);
				X = SE->getSCEVAtScope(X, ICmpLoop);

				ICmpInst::Predicate InvariantPredicate;
				const SCEV *InvariantLHS, *InvariantRHS;

				if (SE->isKnownPredicate(Pred, S, X))
				{
					ICmp->replaceAllUsesWith(ConstantInt::getTrue(ICmp->getContext()));
					DeadInsts.emplace_back(ICmp);
				}
				else if (SE->isKnownPredicate(ICmpInst::getInversePredicate(Pred), S, X))
				{
					ICmp->replaceAllUsesWith(ConstantInt::getFalse(ICmp->getContext()));
					DeadInsts.emplace_back(ICmp);
				}
				else if (isa<PHINode>(IVOperand) && SE->isLoopInvariantPredicate(Pred, S, X, L, InvariantPredicate, InvariantLHS, InvariantRHS))
				{
					Value *NewLHS = nullptr, *NewRHS = nullptr;

					if (S == InvariantLHS || X == InvariantLHS)
						NewLHS = ICmp->getOperand(S == InvariantLHS ? IVOperIdx : (1 - IVOperIdx));

					if (S == InvariantRHS || X == InvariantRHS)
						NewRHS = ICmp->getOperand(S == InvariantRHS ? IVOperIdx : (1 - IVOperIdx));

					auto *PN = cast<PHINode>(IVOperand);
					for (unsigned i = 0, e = PN->getNumIncomingValues(); i != e && (!NewLHS || !NewRHS); ++i)
					{
						if (L->contains(PN->getIncomingBlock(i)))
							continue;

						Value *Incoming = PN->getIncomingValue(i);

						#ifndef NDEBUG
						if (auto *I = dyn_cast<Instruction>(Incoming))
							assert(DT->dominates(I, ICmp) && "Should be a unique loop dominating value!");
						#endif

						const SCEV *IncomingS = SE->getSCEV(Incoming);

						if (!NewLHS && IncomingS == InvariantLHS)
							NewLHS = Incoming;
						if (!NewRHS && IncomingS == InvariantRHS)
							NewRHS = Incoming;
					}

					if (!NewLHS || !NewRHS)
						return;
						
					ICmp->setPredicate(InvariantPredicate);
					ICmp->setOperand(0, NewLHS);
					ICmp->setOperand(1, NewRHS);
				}
				else
					return;
				Changed = true;
			}

			void eliminateIVRemainder(BinaryOperator *Rem, Value *IVOperand, bool IsSigned)
			{
				if (IVOperand != Rem->getOperand(0))
					return;

				const SCEV *S = SE->getSCEV(Rem->getOperand(0));
				const SCEV *X = SE->getSCEV(Rem->getOperand(1));

				const Loop *ICmpLoop = LI->getLoopFor(Rem->getParent());
				S = SE->getSCEVAtScope(S, ICmpLoop);
				X = SE->getSCEVAtScope(X, ICmpLoop);

				if ((!IsSigned || SE->isKnownNonNegative(S)) && SE->isKnownPredicate(IsSigned ? ICmpInst::ICMP_SLT : ICmpInst::ICMP_ULT, S, X))
					Rem->replaceAllUsesWith(Rem->getOperand(0));
				else
				{
					const SCEV *LessOne = SE->getMinusSCEV(S, SE->getOne(S->getType()));
					if (IsSigned && !SE->isKnownNonNegative(LessOne))
						return;

					if (!SE->isKnownPredicate(IsSigned ? ICmpInst::ICMP_SLT : ICmpInst::ICMP_ULT, LessOne, X))
						return;

					ICmpInst *ICmp = new ICmpInst(Rem, ICmpInst::ICMP_EQ, Rem->getOperand(0), Rem->getOperand(1));
					SelectInst *Sel = SelectInst::Create(ICmp, ConstantInt::get(Rem->getType(), 0), Rem->getOperand(0), "tmp", Rem);
					Rem->replaceAllUsesWith(Sel);
				}

				Changed = true;
				DeadInsts.emplace_back(Rem);
			}
			
			bool strengthenOverflowingOperation(BinaryOperator *BO, Value *IVOperand)
			{
				if (BO->hasNoUnsignedWrap() && BO->hasNoSignedWrap())
					return false;

				const SCEV *(ScalarEvolution::*GetExprForBO)(const SCEV *, const SCEV *, SCEV::NoWrapFlags);

				switch (BO->getOpcode())
				{
					default:					return false;

					case Instruction::Add:		GetExprForBO = &ScalarEvolution::getAddExpr;
												break;

					case Instruction::Sub:		GetExprForBO = &ScalarEvolution::getMinusSCEV;
												break;

					case Instruction::Mul:		GetExprForBO = &ScalarEvolution::getMulExpr;
												break;
				}

				unsigned BitWidth = cast<IntegerType>(BO->getType())->getBitWidth();
				Type *WideTy = IntegerType::get(BO->getContext(), BitWidth * 2);
				const SCEV *LHS = SE->getSCEV(BO->getOperand(0));
				const SCEV *RHS = SE->getSCEV(BO->getOperand(1));

				bool Changed = false;

				if (!BO->hasNoUnsignedWrap())
				{
					const SCEV *ExtendAfterOp = SE->getZeroExtendExpr(SE->getSCEV(BO), WideTy);
					const SCEV *OpAfterExtend = (SE->*GetExprForBO)(SE->getZeroExtendExpr(LHS, WideTy), SE->getZeroExtendExpr(RHS, WideTy), SCEV::FlagAnyWrap);
					if (ExtendAfterOp == OpAfterExtend)
					{
						BO->setHasNoUnsignedWrap();
						SE->forgetValue(BO);
						Changed = true;
					}
				}

				if (!BO->hasNoSignedWrap())
				{
					const SCEV *ExtendAfterOp = SE->getSignExtendExpr(SE->getSCEV(BO), WideTy);
					const SCEV *OpAfterExtend = (SE->*GetExprForBO)(SE->getSignExtendExpr(LHS, WideTy), SE->getSignExtendExpr(RHS, WideTy), SCEV::FlagAnyWrap);
					if (ExtendAfterOp == OpAfterExtend)
					{
						BO->setHasNoSignedWrap();
						SE->forgetValue(BO);
						Changed = true;
					}
				}
				return Changed;
			}
			
			bool simplifyUsersOfIV(PHINode *CurrIV, ScalarEvolution *SE, DominatorTree *DT, LoopInfo *LI, SmallVectorImpl<WeakVH> &Dead, IVVisitor *V = nullptr)
			{
				simplifyUsers(CurrIV, V);
				return hasChanged();
			}
			
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
				for (BasicBlock::iterator I = L->getHeader()->begin(); isa<PHINode>(I); ++I)
					Changed |= simplifyUsersOfIV(cast<PHINode>(I), &SE, DT, &LI, DeadInsts);
					
				return Changed;
			}
	};
}

namespace llvm
{
	void IVVisitor::anchor() { }
}

char IndVarElim::ID = 0;			// these two lines are always added to carry out the pass
static RegisterPass<IndVarElim> temp("indvarelim","- to do induction variable elimination");

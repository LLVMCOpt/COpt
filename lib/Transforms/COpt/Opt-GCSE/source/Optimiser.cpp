#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Instruction.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Transforms/Utils/Local.h"
#include "llvm/Support/raw_ostream.h"
#include <cstring>
#include "OptimiserAvEx.cpp"

using namespace llvm;
using namespace std;

namespace
{
	class GCSE : public ModulePass
	{
		public:
		static char ID;

		GCSE() : ModulePass(ID)
		{				}

		virtual bool runOnModule(Module &M)
		{
			for (Module::iterator MI = M.begin(), ME = M.end(); MI != ME; ++MI) 
			{
				Function &F = *MI;
			if (countNoOfExprInFn(F)>=3 )
			{
				assignIndicesToInstrs(&F);
				for (Function::iterator FI = F.begin(), FE = F.end(); FI != FE; ++FI)
				{
					BasicBlock* block = &*FI;
					
					for (BasicBlock::iterator i = block->begin(), e = block->end(); i!=e; ++i)
					{
						Instruction * I = &*i;
						if (BinaryOperator *BI = dyn_cast<BinaryOperator>(i))
						{
							Expression *expr = new Expression(BI);
							
							ItoE temp = AvExF[F.getName().str().c_str()];
																
							expati etemp = temp[InstrToIndex[I]];
							
							for (auto exi = etemp.begin(); exi!=etemp.end(); ++exi)
							if (strcmp((*exi)->toString().c_str(),expr->toString().c_str())==0)
							{	
								for (auto itera = etemp.begin(); itera!=etemp.end(); ++itera)
								{	
									bool c = (*itera)->v1==expr->v1 && (*itera)->v2==expr->v2 && (*itera)->op==expr->op;
									
									if (!c)
									{
										if (checkExpr(expr)==checkExpr(*itera))
										{
											if (checkExpr(expr)==1)
											{
												if (compareExpr(expr,*itera,1))	
												{	
													Value *v1 = where[(*itera)->toString()];
													I->replaceAllUsesWith(v1);
													removeFromAvailable(F.getName().str().c_str(),expr);
												}
											}
											else if (checkExpr(expr)==2)
											{
												if (compareExpr(expr,*itera,2))
												{	
													Value *v1 = where[(*itera)->toString()];
													I->replaceAllUsesWith(v1);
													removeFromAvailable(F.getName().str().c_str(),expr);
												}
											}
											else if (checkExpr(expr)==3)
											{
												if (compareExpr(expr,*itera,3))
												{
													Value *v1 = where[(*itera)->toString()];
													I->replaceAllUsesWith(v1);
													removeFromAvailable(F.getName().str().c_str(),expr);
												}
											}
										}
									}
									else if (possibleExps.find(expr->toString())!=possibleExps.end())
									{
										int noOfPred = 0;
										for (auto it = pred_begin(block), et = pred_end(block); it != et; ++it)
											noOfPred++;
											
										if (noOfPred==possibleExps[expr->toString()].size())
										{
											expati avtemp = AvExF[F.getName().str().c_str()][InstrToIndex[I]];
											for (auto avtempi: avtemp)
											{
												if (strcmp(avtempi->toString().c_str(),expr->toString().c_str())==0)
												{
													PHINode *phi = PHINode::Create(I->getType(),0,"whichExp",&*block->begin());
													for (auto ch:possibleExps[expr->toString()])
														phi->addIncoming(where[ch->toString()],where[ch->toString()]->getParent());
										
													I->replaceAllUsesWith(phi);
												
													removeFromAvailable(F.getName().str().c_str(),expr);
												}
											}
										}
									}
								}
							}
						}
					}
				}
		}
		}
		}
		int countNoOfExprInFn(Function &F)
		{
			int count = 0;
			for (Function::iterator FI = F.begin(), FE = F.end(); FI != FE; ++FI)
			{
				BasicBlock* block = &*FI;
				for (BasicBlock::iterator i = block->begin(), e = block->end(); i!=e; ++i)
					if (BinaryOperator *BI = dyn_cast<BinaryOperator>(i))
						count++;
			}
			return count;
		}	
	};
}

char GCSE::ID = 0;
static RegisterPass<GCSE> temp("gcse","- to do global common subexpression elimination");
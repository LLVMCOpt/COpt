#include "llvm/Pass.h"						
#include "llvm/IR/Function.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Instruction.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Transforms/Utils/Local.h"
#include "llvm/Support/raw_ostream.h"
#include <cstring>
#include "avex.cpp"

using namespace llvm;
using namespace std;
using namespace av;
namespace
{
	class GCSE : public FunctionPass
	{	
		public:
		static char ID;
		
		GCSE() : FunctionPass(ID) 
		{						  }
		
		virtual bool runOnFunction(Function &F)				// - all code till here is always generated
		{
				assignIndicesToInstrs(&F);	// mandatory
															// exp e
			
				std::vector<Instruction *> delI;
				delI.clear();
				for (Function::iterator FI = F.begin(), FE = F.end(); FI != FE; ++FI) // basicblock b, for (b in c)
				{
					BasicBlock* block = &*FI;
					
					for (BasicBlock::iterator i = block->begin(), e = block->end(); i!=e; ++i)	// instruction i, for (i in b) 
					{
						Instruction * I = &*i;
						if (BinaryOperator *BI = dyn_cast<BinaryOperator>(i))	// if (e in i)
						{
							Expression *expr = new Expression(BI);				// e is assigned the exp in i
							
							ItoE temp = AvExF[F.getName().str().c_str()];			// here, I is each I as you iterate through basic block
																// in other programs I could be what is specified, fetched with index
																
							expati etemp = temp[InstrToIndex[I]];	// fetch all expressions available at I
							
							for (auto exi = etemp.begin(); exi!=etemp.end(); ++exi)					// to check if available
							if (strcmp((*exi)->toString().c_str(),expr->toString().c_str())==0)		// if available(e,i)
							{	
								for (auto itera = etemp.begin(); itera!=etemp.end(); ++itera)
								{	
									bool c = (*itera)->v1==expr->v1 && (*itera)->v2==expr->v2 && (*itera)->op==expr->op;
									
									if (!c)	
									{	
										if (checkExpr(expr)==checkExpr(*itera)) // if both expressions are of the same form
										{
											if (checkExpr(expr)==1)	// op1 and op2 are variables
											{
												if (compareExpr(expr,*itera,1))	
												{	
													Value *v1 = where[(*itera)->toString()];	// fetches temp 
													I->replaceAllUsesWith(v1);		// replaceInstruction(i,t);
													delI.push_back(I);
													delI.push_back(dyn_cast<Instruction>(dyn_cast<LoadInst>(expr->v1)));
													delI.push_back(dyn_cast<Instruction>(dyn_cast<LoadInst>(expr->v2)));
													removeFromAvailable(F.getName().str().c_str(),expr);
												}
											}
											else if (checkExpr(expr)==2)
											{
												if (compareExpr(expr,*itera,2))
												{	
													Value *v1 = where[(*itera)->toString()];// fetching temp in which exp was stored 
													I->replaceAllUsesWith(v1);	// replacing with temp
													delI.push_back(I);			// removing unneccessary instructions
													delI.push_back(dyn_cast<Instruction>(dyn_cast<LoadInst>(expr->v1)));
													removeFromAvailable(F.getName().str().c_str(),expr);
												}
											}
											else if (checkExpr(expr)==3)
											{
												if (compareExpr(expr,*itera,3))
												{
													Value *v1 = where[(*itera)->toString()];
													I->replaceAllUsesWith(v1);
													delI.push_back(I);
													delI.push_back(dyn_cast<Instruction>(dyn_cast<LoadInst>(expr->v2)));
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
													delI.push_back(I);
												
													switch(checkExpr(expr))
													{
														case 1: delI.push_back(dyn_cast<Instruction>(dyn_cast<LoadInst>(expr->v1)));
																delI.push_back(dyn_cast<Instruction>(dyn_cast<LoadInst>(expr->v2)));
																break;
														case 2: delI.push_back(dyn_cast<Instruction>(dyn_cast<LoadInst>(expr->v1)));
																break;
														case 3:	delI.push_back(dyn_cast<Instruction>(dyn_cast<LoadInst>(expr->v2)));
																break;
													}
												
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
				for (auto deli = delI.begin(); deli!=delI.end(); ++deli) // this portion is automatically generated every time 
					if (isInstructionTriviallyDead(*deli))				//  snippet from DSL contains replaceInstruction
						(*deli)->eraseFromParent();								
		}
	};
}

char GCSE::ID = 0;			// these two lines are always added to carry out the pass
static RegisterPass<GCSE> temp("gcse","- to do global common subexpression elimination");

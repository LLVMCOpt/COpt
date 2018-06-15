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
		
			if (strcmp("foo",F.getName().str().c_str())==0) // cfg c = "main", if DSL has cfg c, for (c in all), this if is not present
			{
				assignIndicesToInstrs(&F);
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
							
							ItoE temp = AvExF["foo"];			// for snippet 4, I is each I as you iterate through basic block
							expati etemp = temp[InstrToIndex[I]];	// in other programs I is what is specified, fetched with index
							
							for (auto exi = etemp.begin(); exi!=etemp.end(); ++exi)					// to check if available
							if (strcmp((*exi)->toString().c_str(),expr->toString().c_str())==0)		// if available(e,i)
							{	
								for (auto itera = etemp.begin(); itera!=etemp.end(); ++itera) // this is the code generated for find(e)
								{	
									bool c = (*itera)->v1==expr->v1 && (*itera)->v2==expr->v2 && (*itera)->op==expr->op;
									
									if (!c)	// need to add case, refer to test4
									{	
										if (LoadInst *LI1=dyn_cast<LoadInst>(expr->v1))	
										{	
											if (LoadInst *newLI1 = dyn_cast<LoadInst>((*itera)->v1)) // operand 1 is a variable
											{
												if (LoadInst *LI2 = dyn_cast<LoadInst>(expr->v2))	// operand 2 is a variable
												{
													if (LoadInst *newLI2 = dyn_cast<LoadInst>((*itera)->v2))
													{
														if (LI1->getPointerOperand()==newLI1->getPointerOperand() && LI2->getPointerOperand()==newLI2->getPointerOperand())	// going backwards to check where the same expression was found
														{	
															Value *v1 = where[(*itera)->toString()];	// fetches temp t
															I->replaceAllUsesWith(v1);		// replaceInstruction(i,t);
															delI.push_back(I);
															delI.push_back(dyn_cast<Instruction>(LI1));
															delI.push_back(dyn_cast<Instruction>(LI2));
															removeFromAvailable("foo",expr);
														}
													}
												}
												else	// operand 2 is a constant
												{	
													if (LI1->getPointerOperand()==newLI1->getPointerOperand() && expr->v2==(*itera)->v2)
													{	
														Value *v1 = where[(*itera)->toString()];// fetching temp in which exp was stored 
														I->replaceAllUsesWith(v1);	// replacing with temp
														delI.push_back(I);			// removing unneccessary instructions
														delI.push_back(dyn_cast<Instruction>(LI1));
														removeFromAvailable("foo",expr);
													}
												}
											}
										}
										else	// operand 1 is a constant
										{
											if (LoadInst *LI2 = dyn_cast<LoadInst>(expr->v2))	// operand 2 is a variable
											{	
												if (LoadInst *newLI2 = dyn_cast<LoadInst>((*itera)->v2))
												{
													if (expr->v1==(*itera)->v1 && LI2->getPointerOperand()==newLI2->getPointerOperand())
													{
														Value *v1 = where[(*itera)->toString()];
														I->replaceAllUsesWith(v1);
														delI.push_back(I);
														delI.push_back(dyn_cast<Instruction>(LI2));
														removeFromAvailable("foo",expr);
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
				for (auto deli = delI.begin(); deli!=delI.end(); ++deli) // this portion is automatically generated every time 
					if (isInstructionTriviallyDead(*deli))				//  snippet from DSL contains replaceInstruction
						(*deli)->eraseFromParent();	
			}
		}
	};
}

char GCSE::ID = 0;			// these two lines are always added to carry out the pass
static RegisterPass<GCSE> temp("gcse","- to do global common subexpression elimination");

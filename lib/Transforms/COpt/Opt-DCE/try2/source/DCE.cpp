#include "llvm/Pass.h"						
#include "llvm/IR/Function.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Instruction.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Transforms/Utils/Local.h"
#include "llvm/Support/raw_ostream.h"
#include <cstring>

using namespace llvm;
using namespace std;

namespace
{
	class DCE : public FunctionPass
	{	
		public:
		static char ID;
		
		DCE() : FunctionPass(ID) 
		{						  }
		
		virtual bool runOnFunction(Function &F)				
		{	
			std::vector<Instruction *> deleteI;
			deleteI.clear();
			for (Function::iterator FI = F.begin(), FE = F.end(); FI != FE; ++FI) 
			{
				BasicBlock* block = &*FI;
				
				for (BasicBlock::iterator i = block->begin(), e = block->end(); i!=e; ++i)	
				{
					Instruction * I = &*i;
					
					if (I->hasNUses(0) && isInstructionTriviallyDead(I)) 
					{
						deleteI.push_back(I);
					}
				}	
			}
			
			for (auto deli = deleteI.begin(); deli!=deleteI.end(); ++deli)		
				(*deli)->eraseFromParent();	
		}
	};
}

char DCE::ID = 0;			// these two lines are always added to carry out the pass
static RegisterPass<DCE> temp("dcelim","- to do dead code elimination");

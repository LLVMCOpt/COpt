#include "llvm/Pass.h"						
#include "llvm/IR/Function.h"
#include "llvm/IR/Instruction.h"
#include "llvm/Support/raw_ostream.h"
#include <cstring>

using namespace llvm;
using namespace std;

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
			
			if (strcmp("main",(const char *)F.getName())==0) 				// cfg c = "main", if DSL has cfg c, for (c in all), this if is not present
			{
			std::vector<void*> domain;BitVector boundaryCond(domain.size(), false);	BitVector initCond(domain.size(), true);	
				pass.run(F, domain, boundaryCond, initCond);	
															// exp e
			
				for (auto &B : F)							// basicblock b, for (b in c)
				{
					 for (auto &i : B)						// instruction i, for (i in b)
					 {
					 	
					 }	
				}
			
			}
		}
	};


	char GCSE::ID = 0;
	static RegisterPass<GCSE> temp("gcse","- to do global common subexpression elimination");
}

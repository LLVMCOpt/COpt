#include "llvm/Pass.h"						
#include "llvm/IR/Function.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Instruction.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Transforms/Utils/Local.h"
#include "llvm/Support/raw_ostream.h"
#include <cstring>
#include "liveness.cpp"

using namespace llvm;
using namespace std;
using namespace live;
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
			assignIndicesToInstrs(&F);
			errs()<<F.getName().str();
			for (Function::iterator FI = F.begin(), FE = F.end(); FI != FE; ++FI) // basicblock b, for (b in c)
			{
					BasicBlock* block = &*FI;
					
					for (BasicBlock::iterator i = block->begin(), e = block->end(); i!=e; ++i)	// instruction i, for (i in b) 
					{
						Instruction * I = &*i;
						errs()<<InstrToIndex[I]<<" : \n";
							ItoS temp = LiveAtF[F.getName().str()];			// for snippet 4, I is each I as you iterate through basic block
							varati etemp = temp[InstrToIndex[I]];	// in other programs I is what is specified, fetched with index
							
							for (auto exi = etemp.begin(); exi!=etemp.end(); ++exi)
							{
								errs()<<*exi<<" ";
							}	
							errs()<<"\n";			
					}
			}	
		}
	};
}

char GCSE::ID = 0;			// these two lines are always added to carry out the pass
static RegisterPass<GCSE> temp("liv","- to check liveness analysis");

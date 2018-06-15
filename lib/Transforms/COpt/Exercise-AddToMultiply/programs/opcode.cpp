#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instruction.h"
#include "llvm/Support/raw_ostream.h"
#include <map>
#include <cstring>

using namespace llvm;
using namespace std;

namespace
{
	struct opcode : public FunctionPass
	{	
		static char ID;
		
		opcode() : FunctionPass(ID) 
		{						  }
		
		virtual bool runOnFunction(Function &F)
		{
			map<string,int> ocount;
			
			if (strcmp("foo",F.getName())==0) errs()<<"WOOOOOOOOOOOOOOOOOOOOOOOOOOOOHHOOOOOOO\n";
		
			errs()<<"Function "<<F.getName()<<"\n";
			for (auto &B : F)
			{
				 for (auto &i : B)
				 {
				 	if(ocount.find(i.getOpcodeName()) == ocount.end()) 
						ocount[i.getOpcodeName()] = 1;
     				else 
						ocount[i.getOpcodeName()]+= 1;
				 }	
			}
			
			for (auto &I : ocount)
			{
				        errs()<<I.first<<": "<<I.second<<"\n";
			}
			
			ocount.clear();
			return false;
		}
	};
}

char opcode::ID = 0;
static RegisterPass<opcode> temp("opcode","Find Opcode Per Function Pass");

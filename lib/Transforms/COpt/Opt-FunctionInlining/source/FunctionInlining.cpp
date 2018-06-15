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
	std::vector<std::pair<Module*, Function *>> removeFns;
	class FunctionInlining : public ModulePass
	{	
		public:
		static char ID;
		
		FunctionInlining() : ModulePass(ID) 
		{						  }
		
		int countNoOfInstrInFn(Function &F)
		{
			int count = 0;
			for (Function::iterator FI = F.begin(), FE = F.end(); FI != FE; ++FI) 
			{
				BasicBlock* block = &*FI;
				for (BasicBlock::iterator i = block->begin(), e = block->end(); i!=e; ++i)	
					count++;
			}
			return count;
		}
		
		virtual bool runOnModule(Module &M)				
		{
			for (Module::iterator MI = M.begin(), ME = M.end(); MI != ME; ++MI) 
			{
				Function* F = &*MI;
				if (strcmp("main",F->getName().str().c_str())!=0 && countNoOfInstrInFn(*F)<10 && F->getNumUses()>=0 ) // depends on user ip
				{
					F->addFnAttr(llvm::Attribute::AlwaysInline);
					removeFns.push_back(std::pair<Module*, Function *>(&M,F));
				}
			}
		}	
	};

	class RemoveInlinedFunctions : public ModulePass
	{	
		public:
		static char ID;
		
		RemoveInlinedFunctions() : ModulePass(ID) 
		{						  }
		
		virtual bool runOnModule(Module &M)				
		{
			for (Module::iterator MI = M.begin(), ME = M.end(); MI != ME; ++MI) 
			{
				Function* F = &*MI;
				//F->addFnAttr(llvm::Attribute::AlwaysInline);
				if (!(F->use_empty() && find(removeFns.begin(), removeFns.end(), pair<Module*, Function *>(&M,F)) != removeFns.end()))
					removeFns.erase(remove(removeFns.begin(), removeFns.end(), pair<Module*, Function *>(&M,F)),removeFns.end());
			}
			
			for (auto remF = removeFns.begin(); remF!=removeFns.end(); ++remF)
				if (&(*remF->first)==&M)	
					(*remF->second).eraseFromParent();	
		}
	};
	
}

char FunctionInlining::ID = 0;			// these two lines are always added to carry out the pass
static RegisterPass<FunctionInlining> temp1("fninlining","- to do function inlining");

char RemoveInlinedFunctions::ID = 0;			// these two lines are always added to carry out the pass
static RegisterPass<RemoveInlinedFunctions> temp2("removefn","- to remove inlined unused functions");

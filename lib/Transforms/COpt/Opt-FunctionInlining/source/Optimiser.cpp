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
		{				}

		virtual bool runOnModule(Module &M)
		{
			for (Module::iterator MI = M.begin(), ME = M.end(); MI != ME; ++MI) 
			{
				Function* F = &*MI;
				if (strcmp("main",F->getName().str().c_str())!=0)
				{
					F->addFnAttr(llvm::Attribute::AlwaysInline);
					removeFns.push_back(std::pair<Module*, Function *>(&M,F));
				}
			}
		}

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
		};
	class RemoveInlinedFunctions : public ModulePass
	{
		public:
		static char ID;

		RemoveInlinedFunctions() : ModulePass(ID)

		{						  }

		virtual bool runOnModule(Module &M)
		{

		}
	};
}

char FunctionInlining::ID = 0;
static RegisterPass<FunctionInlining> temp1("fninlining","- to do function inlining");

char RemoveInlinedFunctions::ID = 0;
static RegisterPass<RemoveInlinedFunctions> temp2("removefn","- to remove inlined unused functions");
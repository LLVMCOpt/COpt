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
	struct InstrNode
	{
		Instruction *I;
		InstrNode *next;	
	};
	class InstrList
	{
		InstrNode *head;

		public:
		InstrList()
		{
			head = NULL;
		}
		InstrNode* getHead()
		{
		return head;
		}
		void addNode(Instruction *ins)
		{
		InstrNode *tmp = new InstrNode;
		tmp->I = ins;
		tmp->next = NULL;
		if (head == NULL)
		head = tmp;
		else
		{
		InstrNode *curr = head;
		while (curr->next != NULL)
				curr = curr->next;
		curr->next = tmp;
		}
		}
	};
	struct DAGInfoNode
	{
		int binoperator;
		InstrList instructions;
		DAGInfoNode *next;
	};
	class DAGInfoList
	{
		DAGInfoNode *first;

		public:
		DAGInfoList()
		{
		first = NULL;
		}
		Instruction* findInDAG(int opCode, Instruction *newi)
		{
		DAGInfoNode *curr = first;
		while (curr!=NULL)
		{
		if (curr->binoperator==opCode)
		{
		InstrNode *searchcurr = curr->instructions.getHead();
		while (searchcurr!=NULL)
		{
		if (searchcurr->I->getOperand(0)==newi->getOperand(0) && searchcurr->I->getOperand(1)==newi->getOperand(1))
		return searchcurr->I;
		searchcurr = searchcurr->next;
		}
		addNewInstr(curr,newi);
		return NULL;
		}
		curr = curr->next;
		}
		addNewOp(opCode,newi);
		return NULL;
		}
		void addNewInstr(DAGInfoNode* opNode, Instruction *newi)
		{
		opNode->instructions.addNode(newi);
		}
		void addNewOp(int binop, Instruction *newi)
		{
		DAGInfoNode *tmp = new DAGInfoNode;
		tmp->binoperator = binop;
		tmp->instructions.addNode(newi);
		tmp->next = NULL;
		if (first == NULL)
		first = tmp;
		else
		{
		DAGInfoNode *curr = first;
		while (curr->next != NULL)
			curr = curr->next;
		curr->next = tmp;
		}
		}
	};

	class LCSE : public BasicBlockPass
	{
		public:
		static char ID;

		LCSE() : BasicBlockPass(ID)
		{				}

		virtual bool runOnBasicBlock(BasicBlock &B)
		{
			DAGInfoList l;
			BasicBlock* block = &B;
			for (BasicBlock::iterator i = B.begin(), e = B.end(); i!=e; ++i)
			{
				Instruction * I = &*i;
				if (BinaryOperator *BI = dyn_cast<BinaryOperator>(i))
		if (countNoOfExprs(*block)>=3 )
				{
					Instruction *instr = l.findInDAG(I->getOpcode(),I);
					if (instr!=NULL)
					I->replaceAllUsesWith(instr);
				}
			}
		}
		int countNoOfExprs(BasicBlock &B)
		{
			int count = 0;
			for (BasicBlock::iterator i = B.begin(), e = B.end(); i!=e; ++i)	
			{
			Instruction * I = &*i;
			if (BinaryOperator *BI = dyn_cast<BinaryOperator>(i))
			count++;
			}
			return count;
		}
	};
}

char LCSE::ID = 0;
static RegisterPass<LCSE> temp("lcse","- to do local common subexpression elimination");
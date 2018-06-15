#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Analysis/CFG.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Function.h>
#include <iostream>
#include <unordered_map>
#include <set>
#include<vector>
#include <algorithm> 
namespace
{
	struct BasicBlockInfo
	{
		std::set<std::string> gen;
		std::set<std::string> in;
		std::set<std::string> out;
	};
	std::set<std::string> dummy;
	std::set<std::string> seto;
	std::set<std::string> seti;
	std::set<std::string> setg;
	 struct Instruction
	 {
		std::set<std::string> inseti;
		std::set<std::string> inseto;
		std::set<std::string> insetg;
	 };
	std::vector< std::vector<std::string> > vresult;
	std::vector<std::string> vresult1;
	std::vector<std::string> vresult2;
	
	std::set<std::string> dset;
	
	bool check1;
	bool &check2=check1;
	bool check3;
	bool &check4=check3;
	int i=0,i2=0,ig=0,count=0;
	int n,n1,ng;
	std::string exp="";
	std::string exp1;
	std::string expi;
	std::unordered_map<std::string,BasicBlockInfo*> basic_block_table;
	std::map<std::string,Instruction*> Instruction_table;
	BasicBlockInfo *BBI;
	Instruction* INS;
	bool ExtendSet(std::set<std::string> &set1,std::set<std::string> &set2)
	{
		int last_size=set1.size();
		for(const std::string &s : set2)
			set1.insert(s);
		return set1.size()-last_size;
	}

	void PrintSetg(std::set<std::string> &set)
	{
		llvm::errs()<<"gen="<<"{";
		for(const std::string &s:set)
			llvm::errs()<<' '<<s;
		llvm::errs()<<"}\n";
	}
	void PrintSeto(std::set<std::string> &set)
	{
			llvm::errs()<<"out="<<"{";
			for(const std::string &s:set)
					llvm::errs()<<' '<<s;
			llvm::errs()<<"}\n";
	}
	void PrintSeti(std::set<std::string> &set)
	{
			llvm::errs()<<"in="<<"{";
			for(const std::string &s:set)
					llvm::errs()<<' '<<s;
			llvm::errs()<<"}\n";
	}
class AvEx : public llvm::FunctionPass
{
	public:
	//Structure

	// Identifier
	static char ID;
	// Constructor. The first argument of the parent constructor is
	// a unique pass identifier.
	AvEx() : llvm::FunctionPass(ID) { }
	// Virtual function overridden to implement the pass functionality.
	bool runOnFunction(llvm::Function &function) override
	{
		// Print function name
		llvm::errs() << "Analysis Pass on function: " << function.getName() <<"Available Expressions"<< '\n';
		// Traverse basic blocks in function
		for (llvm::Function::iterator basic_block = function.begin(),
		e = function.end();
		basic_block != e;
		++basic_block)
		{
			BBI=new BasicBlockInfo;
			basic_block_table.insert({(*basic_block).getName(),BBI});
			i++;
			for(llvm::BasicBlock::iterator ins=basic_block->begin(),e=basic_block->end();ins!=e;++ins)
			{
				INS=new Instruction;
				Instruction_table.insert({(*ins).getName(),INS});
			}
		}
		n=i;
		i=0;
		//---------Project---------------------//
		for (llvm::Function::iterator basic_block = function.begin(),
		e = function.end();
		basic_block != e;
		++basic_block,i++)
		{
		if(i==(n-1))
		{
			i2=0;
			for(llvm::BasicBlock::iterator ins=basic_block->begin(),e=basic_block->end();ins!=e;++ins)
			{
			i2++;
			}
			n1=i2;
			i2=0;
			for(llvm::BasicBlock::iterator ins=basic_block->begin(),e=basic_block->end();ins!=e;++ins,i2++)
			{
			if(i2<n1-1)
			{
			expi=ins->getName();
			exp=ins->getOpcodeName();
			for (unsigned i1 = 0; i1 < (ins->getNumOperands()); i1++)
			{
				llvm::Value *arg = ins->getOperand(i1);
				exp1=(arg->getName());
				if(exp1=="")
				{
					llvm::ConstantInt* I=llvm::dyn_cast<llvm::ConstantInt>(arg);
					int ss1=I->getSExtValue();
					exp1=std::to_string(ss1);
				}
				exp=exp+ exp1;
			}
			std::string d= ins->getOpcodeName();
			if(d!="store")
			{
			//exp=expi+"="+exp;
			}
			dummy.insert(exp);
			}
			}
			//llvm::errs() << basic_block->getName() << " :";
			((basic_block_table[basic_block->getName()])->out).clear();
		 
			llvm::errs() <<'\n' ;
		}
		else
		{
			i2=0;
			for(llvm::BasicBlock::iterator ins=basic_block->begin(),e=basic_block->end();ins!=e;++ins)
			{
			i2++;
			}
			n1=i2;
			i2=0;
			for(llvm::BasicBlock::iterator ins=basic_block->begin(),e=basic_block->end();ins!=e;++ins,i2++)
			{
				if(i2<n1-1)
				{
				expi=ins->getName();
				exp=ins->getOpcodeName();
				for (unsigned i1 = 0; i1 < (ins->getNumOperands()); i1++)
				{
				llvm::Value *arg = ins->getOperand(i1);
				exp1=(arg->getName());
				if(exp1=="")
				{
					llvm::ConstantInt* I=llvm::dyn_cast<llvm::ConstantInt>(arg);
					int ss=I->getSExtValue();
					exp1=std::to_string(ss);
				}
				exp=exp+ exp1;
				}
				std::string d1= ins->getOpcodeName();
				if(d1!="store")
				{
				//exp=expi+"="+exp;
				}
				dummy.insert(exp);
				//((basic_block_table[basic_block->getName()])->out.insert(exp));
				}
			}
		}
	}
	i=0;
	for (llvm::Function::iterator basic_block = function.begin(),
	e = function.end();
	basic_block != e;
	++basic_block,i++)
	{
		if(i<n-1)
		{
			ExtendSet((basic_block_table[basic_block->getName()])->out,dummy);
		}
		llvm::errs() << basic_block->getName() << ": ";
		PrintSeto((basic_block_table[basic_block->getName()])->out);
		llvm::errs() <<'\n';
	}
	//-----------------------Project--------------------------//
		llvm::errs() << "--------" << '\n';
		do
		{
			check1=0;
			check3=0;
			for (llvm::Function::iterator basic_block = function.begin(),
			e = function.end();
			basic_block != e;
			++basic_block)
			{
				printBasicBlock(*basic_block);
			}
		}while(check2||check4);
		return true;
	}
	void printBasicBlock(llvm::BasicBlock &basic_block)
	{
		llvm::errs() << " Basic block " << basic_block.getName() << '\n';

		llvm::errs() << " Predecessors: {";
		std::set<std::string>::iterator its;
		count=0;
		vresult.clear();
		for (llvm::pred_iterator it = pred_begin(&basic_block),
		e = llvm::pred_end(&basic_block);
		it != e;
		++it)
		{
		llvm::BasicBlock *pred = *it;
		llvm::errs() << ' ' << pred->getName();
		//ExtendSet((basic_block_table[basic_block.getName()])->in,(basic_block_table[pred->getName()])->out);
		//vresult.push_back((basic_block_table[pred->getName()])->out);
		vresult2.clear();
		for(its=((basic_block_table[pred->getName()])->out.begin());its!=(basic_block_table[pred->getName()])->out.end();++its)
		{
			vresult2.push_back(*its);
		}
		vresult.push_back(vresult2);
		count++;
		}
		if(count>=1)
		{
		vresult1=vresult[0];
		//llvm::errs()<<"size"<<vresult.size();
		for(int i=1;i<vresult.size();i++)
		{
			vresult1=instersection(vresult1,vresult[i]);
		}
		std::vector<std::string>::iterator itv;
		(basic_block_table[basic_block.getName()])->in.clear();
		for (itv=vresult1.begin();itv!=vresult1.end();++itv)
		{
			(basic_block_table[basic_block.getName()])->in.insert(*itv);
			//llvm::errs() <<"itv"<<*itv<<'\n';
		}
		}
		llvm::errs() << " }\n";
		PrintSeti((basic_block_table[basic_block.getName()])->in);
		// Traverse successors
		llvm::errs() << " Successors: {";
		for (llvm::succ_iterator it = succ_begin(&basic_block),
		e = llvm::succ_end(&basic_block);
		it != e;
		++it)
		{
		llvm::BasicBlock *succ = *it;
		llvm::errs() << ' ' << succ->getName();
		}
		llvm::errs() << " }\n";
		// Traverse instructions
		ig=0;
		for (llvm::BasicBlock::iterator instruction = basic_block.begin(),
		e = basic_block.end();
		instruction != e;
		++instruction)
		{
			ig++;
		}
		ng=ig;
		ig=0;
		for (llvm::BasicBlock::iterator instruction = basic_block.begin(),
		e = basic_block.end();
		instruction != e;
		++instruction,++ig)
		{
		if(ig<ng-1)
		{
			expi=instruction->getName();
			exp=instruction->getOpcodeName();
			if(exp!="alloca"&&exp!="store")
			{
			for (unsigned i1 = 0; i1 < (instruction->getNumOperands()); i1++)
			{
			llvm::Value *arg = instruction->getOperand(i1);
			exp1=(arg->getName());
				if(exp1=="")
				{
					llvm::ConstantInt* I=llvm::dyn_cast<llvm::ConstantInt>(arg);
					int ss1=I->getSExtValue();
					exp1=std::to_string(ss1);
				}
			exp=exp+exp1;
			}
			std::string d2= instruction->getOpcodeName();
			if(d2!="store")
			{
			//exp=expi+"="+exp;
			}
			Instruction_table[instruction->getName()]->insetg.insert(exp);
			(basic_block_table[basic_block.getName()])->gen.insert(exp);
			}
		}
		
		}
		seti=((basic_block_table[basic_block.getName()])->in);
		setg=((basic_block_table[basic_block.getName()])->gen);
		seto=((basic_block_table[basic_block.getName()])->out);
		(ExtendSet(seti,setg));
		(basic_block_table[basic_block.getName()])->out=seti;
		if(seto!=((basic_block_table[basic_block.getName()])->out))
		{
			check3=check3+1;
		}
		PrintSetg((basic_block_table[basic_block.getName()])->gen);
		PrintSeto((basic_block_table[basic_block.getName()])->out);
		llvm::errs()<<"-----"<<"\n";
	}

	std::vector<std::string> instersection(std::vector<std::string> &v1, std::vector<std::string> &v2)
	{

		std::vector<std::string> v3;

		sort(v1.begin(), v1.end());
		sort(v2.begin(), v2.end());

		set_intersection(v1.begin(),v1.end(),v2.begin(),v2.end(),back_inserter(v3));

		return v3;
	}
	
};
}
char AvEx::ID = 0;
static llvm::RegisterPass<AvEx> X("avexp", "- available expressions analysis pass", false, false);

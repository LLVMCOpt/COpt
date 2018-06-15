class CSE : public llvm::FunctionPass
{
	std::set<std::string> st;
	public:
		static char ID;
	// Constructor. The first argument of the parent constructor is
	// a unique pass identifier.
	CSE() : llvm::FunctionPass(ID) { }
	// Virtual function overridden to implement the pass functionality.
		std::vector<llvm::Instruction*> rpins;
		std::vector<llvm::Instruction*> rmins;
	bool runOnFunction(llvm::Function &function) override
	{
		// Print function name
		llvm::errs() << "Transform Pass on function: " << function.getName() << " Common Subexpression Elimination" <<'\n';
		for (llvm::Function::iterator basic_block = function.begin(),
		e = function.end();
		basic_block != e;
		++basic_block)
		{
			   llvm::errs() << basic_block->getName() << " :" <<'\n';
			for(llvm::BasicBlock::iterator ins=basic_block->begin(),e=basic_block->end();
				ins!=e;++ins)
			{
				llvm::BasicBlock::iterator stop=ins;
				stop++;
				llvm::BasicBlock::iterator stit;
				if(stop!=basic_block->end())
				{
					dset.clear();
					llvm::BasicBlock::iterator p=ins;
					p--;
					llvm::errs()<<"-------------------"<<'\n';
					//llvm::errs()<<ins->getName()<<'\n';
					ins->dump();
					llvm::errs()<<'\n';
					if(ins!=basic_block->begin())
					{
					Instruction_table[ins->getName()]->inseti=Instruction_table[p->getName()]->inseto;
					}
					else
					{
						Instruction_table[ins->getName()]->inseti=basic_block_table[basic_block->getName()]->in;
					}
		
					std::string cmps=ins->getOpcodeName();
					if(cmps!="store")
					PrintSeti(Instruction_table[ins->getName()]->inseti);
					ExtendSet(dset,Instruction_table[ins->getName()]->insetg);
					ExtendSet(dset,Instruction_table[ins->getName()]->inseti);
					Instruction_table[ins->getName()]->inseto=dset;
					if(cmps!="store")
					{
					PrintSetg(Instruction_table[ins->getName()]->insetg);
					
					
					PrintSeto(Instruction_table[ins->getName()]->inseto);
					}
				}
				
			}
				
		}
		llvm::errs()<<"-------------------"<<'\n';
		std::map<std::string,Instruction*>::iterator itm;
			std::map<std::string,Instruction*>::iterator itmc;
			std::set<std::string>::iterator its1;
			std::string expression="";
			std::string expression1="";
			int c_str=0;
			std::string inst_name;
			int c=0;int c1=0;
			//llvm::BasicBlock iterator insf;
			for(itm=Instruction_table.begin();itm!=Instruction_table.end();++itm)
			{
				for(its1=itm->second->insetg.begin();its1!=itm->second->insetg.end();++its1)
				{
					if(itm->second->inseti.find(*its1)!=itm->second->inseti.end())
					{
						for (llvm::Function::iterator basic_block = function.begin(),e = function.end();basic_block != e;++basic_block)
						{
							for(llvm::BasicBlock::iterator ins2=basic_block->begin();ins2!=basic_block->end();++ins2)
							{
								inst_name=ins2->getName();
								//llvm::errs()<<ins2->getName();
								if(inst_name==itm->first)
								{
									llvm::errs()<<"CSE:"<<'\n';
									llvm::errs()<<"Removed Instruction:";
									ins2->dump();
									 rmins.push_back(ins2);
								//	c++;
									//llvm::errs()<<c<<"\n";
								}
							}
						}
					 	//llvm::errs()<<"removed instruction:"<<itm->first<<"="<<*its1<<"==>";
						std::string rm=itm->first;
						for (llvm::Function::iterator basic_block1 = function.begin(),e = function.end();basic_block1 != e;++basic_block1)
						{
							for(llvm::BasicBlock::iterator ins1=basic_block1->begin();ins1!=basic_block1->end();++ins1)
							{
								expression=ins1->getOpcodeName();
								for(unsigned i=0;i<(ins1->getNumOperands());i++)
								{
									llvm::Value *arg = ins1->getOperand(i);
									expression1=(arg->getName());
									if(expression1=="")
									{
										llvm::ConstantInt* I=llvm::dyn_cast<llvm::ConstantInt>(arg);
										int ss1=I->getSExtValue();
										expression1=std::to_string(ss1);
									}
									expression=expression+expression1;
									//llvm::ReplaceInstWithInst(ins2,ins1);
								}
								std::string check_str=*its1;
								c_str=0;
								if(expression==check_str)
								{
									llvm::errs()<<"Replaced With:";
									ins1->dump();
									rpins.push_back(ins1);
									/*llvm::errs()<<"**********"<<'\n';
									rmins[c1]->dump();
									rpins[c1]->dump();
									c1++;
									llvm::errs()<<"**********"<<'\n';*/
									//llvm::errs()<<c1<<"\n";
									llvm::errs()<<"-------------------"<<'\n';
									
									
									c_str=1;
									break;
								}
								if(c_str)
								{
									break;
								}
							}
							 if(c_str)
							{
								break;
							}
						}
					}
				}
			}
			for(int re=0;re<rmins.size();re++)
			{
				//rmins[re]->dump();
				//rpins[re]->dump();
				
				rmins[re]->replaceAllUsesWith(rpins[re]);  //***//
		
				rmins[re]->getParent()->getInstList().erase(rmins[re]);	//***//
			}
			
	}

};

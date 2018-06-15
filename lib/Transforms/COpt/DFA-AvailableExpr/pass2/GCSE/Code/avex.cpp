#include "dataflow.cpp"

using namespace llvm;

namespace av
{
	typedef std::vector<Expression*> expati;// set of expressions
	std::map<int, expati> AvAtI;			// at every instruction
	typedef std::map<int, expati> ItoE;		
	std::map<string,ItoE> AvExF;			// for every function
	std::map<string,Instruction *> where;
	
	void removeFromAvailable(string fname, Expression *remexp)
	{
		ItoE *t1 = &AvExF[fname];
		for (auto itemp : *t1)
		{
			for (auto itemp2 : itemp.second)
			{
				if (strcmp(itemp2->toString().c_str(),remexp->toString().c_str())==0)
				{
					itemp.second.erase(std::remove(itemp.second.begin(), itemp.second.end(), itemp2), itemp.second.end());
					AvExF[fname][itemp.first]=itemp.second;
				}
			}
		}
		//AvExF[fname] = *t1;
	}
	
	class AvailableExpressions : public FunctionPass 
	{
		public:
        	static char ID;

			AvailableExpressions() : FunctionPass(ID)
			{
				Direction direction = Direction::FORWARD;
				MeetOp meet_op = MeetOp::INTERSECTION;
				pass = AEAnalysis(direction, meet_op);
			}
			
		//private:
		
		class AEAnalysis : public DataFlow 
		{
			public:

				AEAnalysis() : DataFlow(Direction::INVALID_DIRECTION, MeetOp::INVALID_MEETOP) {	}
				AEAnalysis(Direction direction, MeetOp meet_op) : DataFlow(direction, meet_op) { }

			protected:

				TransferOutput transferFn(BitVector input, std::vector<void*> domain, std::map<void*, int> domainToIndex, BasicBlock* block)
				{
					TransferOutput transferOutput;

					// expressions generated and killed in basic block
					int domainSize = domainToIndex.size();
					BitVector GenSet(domainSize);
					BitVector KillSet(domainSize);

					for (BasicBlock::iterator i = block->begin(), e = block->end(); i!=e; ++i)
					{
						Instruction * I = &*i;
						if (BinaryOperator * BI = dyn_cast<BinaryOperator>(I))	// available expressions for BinaryOperators
						{
							Expression *expr = new Expression(BI);	// new Expression to capture the RHS of the BinaryOperator
							Expression *match = NULL;
							bool found = false;

							for(void* element : domain)
							{
								if((*expr) == *((Expression *) element))
								{
									found = true;
									match = (Expression *) element;
									break;
								}
							}

							if (found)	// gen
							{
								int valInd = domainToIndex[(void*)match];
								GenSet.set(valInd);	// instruction evaluates expression in RHS here
							}						// will be killed if one of its operands is redefined subsequently in the BB
						}

						// killed expressions - kill all expressions in which the LHS is an operand.
						StringRef insn  =  I->getName();
						if(!insn.empty())
						{
							for(auto domain_itr = domain.begin() ; domain_itr != domain.end() ; domain_itr++)
							{
								Expression* expr = (Expression*) (*domain_itr);

								StringRef op1 = expr->v1->getName();
								StringRef op2 = expr->v2->getName();

								if(op1.equals(insn) || op2.equals(insn)) // if either operand is on LHS, kill
								{
									std::map<void*, int>::iterator iter = domainToIndex.find((void*) expr);

									if (iter != domainToIndex.end())
									{
										KillSet.set((*iter).second);
										GenSet.reset((*iter).second);
									}
								}
							}
						}
					}

					// transfer function = GenSet U (input - KillSet)
					transferOutput.element = KillSet;
					transferOutput.element.flip();	// complement of KillSet
					transferOutput.element &= input;	// input - KillSet 
					transferOutput.element |= GenSet;	// union GenSet

					return transferOutput;
				}

		};
		
		AEAnalysis pass;	// pass
		
		virtual bool runOnFunction(Function &F) 
        {
			// print -> 
			std::string function_name = F.getName(); errs() << "FUNCTION :: " << function_name  << "\n";
			
			DataFlowResult output;
			std::vector<void*> domain;
			
			AvAtI.clear();
			assignIndicesToInstrs(&F);
			
			for (Function::iterator FI = F.begin(), FE = F.end(); FI != FE; ++FI) // fill in domain
			{
				BasicBlock* block = &*FI;
				for (BasicBlock::iterator i = block->begin(), e = block->end(); i!=e; ++i) 
				{
					Instruction * I = &*i;
					//errs()<<*IndexToInstr[InstrToIndex[I]]<<"    check   ----\n";
					if (BinaryOperator * BI = dyn_cast<BinaryOperator>(I))	// av ex for binary operators 
					{
						Expression *expr = new Expression(BI);	// new Expression to capture the RHS of the BinaryOperator
						bool found = false;

						for(void* element : domain)
						{
						    if((*expr) == *((Expression *) element))
						    {
						        found = true;
						        break;
						    }
						}

						if(found == false)
						{
						    domain.push_back(expr);
						    where.insert(std::pair<string,Instruction*>(expr->toString(),I));
						}
						else
						    delete expr;
					}
				}
			}

			errs() << "------------------------------------------\n\n";
			errs() << "DOMAIN :: " << domain.size() << "\n";
			for(void* element : domain)
			{
				errs() << "Element : " << ((Expression*) element)->toString() << "\n";
			}
			errs() << "------------------------------------------\n\n";

			BitVector boundaryCond(domain.size(), false);	// For AEA, the boundary condition is phi
			BitVector initCond(domain.size(), true);	// For AEA, init condition is U

			output = pass.run(F, domain, boundaryCond, initCond);	// apply pass, printResult(output);

			// map domain values to index in bitvector
			std::map<void*, int> domainToIndex;
			for (int i = 0; i < domain.size(); i++)
				domainToIndex[(void*)domain[i]] = i;

			// We use the results to compute the available expressions
			std::stringstream ss;

			for (Function::iterator BI = F.begin(), BE = F.end(); BI != BE; ++BI) 
			{
				BasicBlock* block = &*BI;

				BitVector availableExprs = output.result[block].in;	// AE at IN

				// Generate Print Information
				std::vector<std::string> out;

				// Print live variables at the entry of the block
				ss.clear();
				ss << std::setw(WIDTH) << std::right;
				ss.str(std::string());
				ss << printSet(domain, availableExprs, 1) << " :: BB Entry" ;
				out.push_back(ss.str());

				// Iterate forward through the block, updating availability of expressions
				for (BasicBlock::iterator insn = block->begin(), IE = block->end(); insn != IE; ++insn) 
				{
				
					std::vector<Expression*> e1; // hold available expressions at an instruction
				
					out.push_back(std::string(WIDTH, ' ') + printValue(&*insn));	// Add the instruction itself

					Instruction * I = &*insn;	// Gen expressions

					if (BinaryOperator * BI = dyn_cast<BinaryOperator>(I)) 
					{
						Expression *expr = new Expression(BI);
						Expression *match = NULL;
						bool found = false;

						for(void* element : domain)
						{
						    if((*expr) == *((Expression *) element))
						    {
						        found = true;
						        match = (Expression *) element;
						        break;
						    }
						}

						if(found)		// generated expression
						{
						    int valInd = domainToIndex[(void*)match];
						    availableExprs.set(valInd); // instruction evaluates expression in RHS, will be killed if one of its operands is
						}								// redefined subsequently in the BB.
					}

					// kill all expressions in which the LHS is an operand, will be generated if subsequently recomputed in BB.
					StringRef insn_str  =  I->getName();

					if(!insn_str.empty())
					{
						errs() << "Insn : " << insn_str  << "\n";

						for(auto domain_itr = domain.begin() ; domain_itr != domain.end() ; domain_itr++)
						{
						    Expression* expr = (Expression*) (*domain_itr);

						    StringRef op1 = expr->v1->getName();
						    StringRef op2 = expr->v2->getName();

						    if(op1.equals(insn_str) || op2.equals(insn_str))
						    {
						        errs() << "Expr : " << expr->toString()  << " ";

						        // kill if either operand 1 or 2 match the variable assigned
						        std::map<void*, int>::iterator iter = domainToIndex.find((void*) expr);

						        if (iter != domainToIndex.end())
						        {
						            errs() << "Index : " << (*iter).second  << "\n";
						            availableExprs.reset((*iter).second);
						        }
						    }
						}
					}

					//printBitVector(availableExprs);

					// Print live variables at this program point
					PHINode* phiInst = dyn_cast<PHINode>(&*insn);
					// Skip printing for phi instructions
					if(phiInst == NULL)
					{
						ss.clear();
						ss.str(std::string());
						ss << std::setw(WIDTH) << std::right;
						ss << printSet(domain, availableExprs, 1);
						
							/*------store available expressions for later use-------*/
							for (int i = 0; i < domain.size(); i++) 
								if (availableExprs[i]) 
									e1.push_back((Expression*)domain[i]);	// Expression* - set of expr available
					
							AvAtI.insert(std::pair<int,expati>(InstrToIndex[I],e1));	// at the instruction point
							/*------------------------------------------------------*/
							
						out.push_back(ss.str());
					}
				}

				// Print strings
				for (std::vector<std::string>::iterator it = out.begin(); it != out.end(); ++it)
				   errs() << *it << "\n";

			}
			
			AvExF.insert(std::pair<string,ItoE>(function_name,AvAtI));	// for every instr in a function set of expr available

			// No modification
			return false;
        }

        virtual bool runOnModule(Module& M) 
        {
            bool modified = false;

            // Run analysis on each function
            for (Module::iterator MI = M.begin(), ME = M.end(); MI != ME; ++MI) 
            {
                bool function_modified = false;

                function_modified = runOnFunction(*MI);

                modified |= function_modified;
            }

            return modified;
        }
        
        AEAnalysis getPass()	{
        return pass;			}

    };
}
using namespace av;
char AvailableExpressions::ID = 0;
RegisterPass<AvailableExpressions> X("available", "Available Expressions");

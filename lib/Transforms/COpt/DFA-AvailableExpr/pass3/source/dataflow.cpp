#include "dataflow.h"

using namespace std;

namespace llvm 
{

	void assignIndicesToInstrs(Function * F) 
	{
			InstrToIndex[nullptr] = 0;
			IndexToInstr[0] = nullptr;

			unsigned counter = 1;
			for (inst_iterator I = inst_begin(F), E = inst_end(F); I != E; ++I) 
			{
				Instruction * instr = &*I;
				InstrToIndex[instr] = counter;
				IndexToInstr[counter] = instr;
				counter++;
			}

			return;
	}

	BitVector DataFlow::applyMeetOp(BitVectorList inputs)	// implementing meet operator
	{
		BitVector result;

		if (!inputs.empty())
		{
		    for (int i = 0; i < inputs.size(); i++)
		    {
		        if (i == 0)
		        {
		            result = inputs[i];
				}
		        else 
		        {
		            if (meetup_op == MeetOp::UNION)
		            {
		                result |= inputs[i];
		            }
		            else if (meetup_op == MeetOp::INTERSECTION)
		            {
		                result &= inputs[i];
		            }
		            else
		            {
		                errs() << "Unknown Meetop\n";
		                exit(EXIT_FAILURE);
		            }
		        }
		    }
		}

		return result;
	}

	DataFlowResult DataFlow::run(Function &F, std::vector<void*> domain, BitVector boundaryCond, BitVector initCond)	// analyse F
	{
		std::map<BasicBlock*, BlockResult> result;
		bool modified = false;
		
		std::map<void*, int> domainToIndex;	// mapping domain values to index in bitvector
		for (int i = 0; i < domain.size(); i++)
			domainToIndex[(void*)domain[i]] = i;

		BasicBlockList boundaryBlocks;	// get extreme blocks
		switch (direction) 
		{
			case Direction::FORWARD: 	boundaryBlocks.push_back(&F.front());	// entry - first block
										break;		
				
			case Direction::BACKWARD:	for(Function::iterator I = F.begin(), E = F.end(); I != E; ++I)	// return statements
											if (isa<ReturnInst>(I->getTerminator()))
												boundaryBlocks.push_back(&*I);
										break;

			default:	errs() << "Invalid direction \n";
						break;
		}

		BlockResult boundaryRes = BlockResult();	// initialise boundary blocks
		BitVector *value = (direction == Direction::FORWARD) ? &boundaryRes.in : &boundaryRes.out;	// forward analysis - initialise IN 																									// backward analysis - initialise OUT	
		*value = boundaryCond;
		boundaryRes.transferOutput.element = boundaryCond;
		for (BasicBlockList::iterator I = boundaryBlocks.begin(), E = boundaryBlocks.end(); I != E; ++I) 
			result[*I] = boundaryRes;	


		BlockResult intRes = BlockResult();	// initialise interior blocks
		BitVector* intVal = (direction == Direction::FORWARD) ? &intRes.out : &intRes.in;	// forward analysis - initialise IN
		BitVector* intOtherVal = (direction == Direction::FORWARD) ? &intRes.in : &intRes.out;	// backward analysis - initialise OUT

		*intVal = initCond;
		*intOtherVal = initCond;
		intRes.transferOutput.element = initCond;
		for (Function::iterator BB = F.begin(), BE = F.end(); BB != BE; ++BB)
		{
		    if (std::find(boundaryBlocks.begin(),boundaryBlocks.end(),(BasicBlock*)&*BB) == boundaryBlocks.end())
		    {
		        result[(BasicBlock*)&*BB] = intRes;	// not one of the boundary blocks
		    }
		}

		// neighbor list - forward analysis->predecessors, backward analysis->successors
		std::map<BasicBlock*, BasicBlockList > blockNeighbors;

		for (Function::iterator BB = F.begin(), BE = F.end(); BB != BE; ++BB) 
		{
			BasicBlockList neighborList;

		    switch (direction) 
		    {
				case Direction::FORWARD:	for (pred_iterator neighbor = pred_begin(&*BB), E = pred_end(&*BB); neighbor != E; ++neighbor)
				        						neighborList.push_back(*neighbor);
				    						break;

				case Direction::BACKWARD:	for (succ_iterator neighbor = succ_begin(&*BB), E = succ_end(&*BB); neighbor != E; ++neighbor)
												neighborList.push_back(*neighbor);
											break;

				default:	errs() << "Invalid direction \n";
				    		break;
		    }

			blockNeighbors[&*BB] = neighborList;
		}

		// order to traverse BasicBlocks
		BasicBlockList traversalOrder;

		switch (direction) 
		{

			case Direction::FORWARD:{
										ReversePostOrderTraversal<Function*> TR(&F);
										for (ReversePostOrderTraversal<Function*>::rpo_iterator I = TR.begin(), E = TR.end(); I != E; ++I) 
											traversalOrder.push_back(*I);
									}
									break;
									
			case Direction::BACKWARD:{
							for (po_iterator<BasicBlock*> I = po_begin(&F.getEntryBlock()), E = po_end(&F.getEntryBlock()); I != E; ++I) 
				    			traversalOrder.push_back(*I);			
									}
		    						break;

			default:	errs() << "Invalid direction \n";
						break;
		}

		// keep processing blocks until convergence
		bool converged = false;
		while (!converged) 
		{
			converged = true;

			for (BasicBlockList::iterator BB = traversalOrder.begin(), BE = traversalOrder.end(); BB != BE; ++BB)
			{
				BlockResult& blockRes = result[*BB];	// current result for the block

				BlockResult oldBlockRes = blockRes;		// store it to check if it changed at the end
				BitVector oldVal = (direction == Direction::FORWARD) ? blockRes.out : blockRes.in;

				BitVectorList meetInputs;	// collecting neighbor results for meet
				for (BasicBlockList::iterator NI = blockNeighbors[*BB].begin(), NE = blockNeighbors[*BB].end(); NI != NE; ++NI)
				{
					BlockResult& neighborRes = result[*NI];
					BitVector neighVal = neighborRes.transferOutput.element;

				    // union the value if we find a match with neighbor-specific value
					std::map<BasicBlock*, BitVector>::iterator match = neighborRes.transferOutput.neighborVals.find(*BB);
					if (match != neighborRes.transferOutput.neighborVals.end()) 
						neighVal |= match->second;

				    meetInputs.push_back(neighVal);
				}

				// apply the meet operator and store in in or out based on forward or backward
				BitVector* blockInput = (direction == Direction::FORWARD) ? &blockRes.in : &blockRes.out;
				if (!meetInputs.empty())
					*blockInput = applyMeetOp(meetInputs);

				// apply transfer function for input to output
				blockRes.transferOutput = transferFn(*blockInput, domain, domainToIndex, *BB);
				BitVector* blockOutput = (direction == Direction::FORWARD) ? &blockRes.out : &blockRes.in;
				*blockOutput = blockRes.transferOutput.element;

				if (converged)	// if there's a change, converged becomes false
				{
					if (*blockOutput!= oldVal||blockRes.transferOutput.neighborVals.size()!= oldBlockRes.transferOutput.neighborVals.size())
				        converged = false;
				}
			}
		}

		// output
		DataFlowResult output;
		output.domainToIndex = domainToIndex;
		output.result = result;
		output.modified = modified;

		return output;
	}

	// Expression class - expressions for the Available Expression analysis
	Expression::Expression(Instruction * I) 
	{
		if (BinaryOperator * BO = dyn_cast<BinaryOperator>(I)) 
		{
			this->v1 = BO->getOperand(0);
			this->v2 = BO->getOperand(1);
			this->op = BO->getOpcode();
		}
	}

    // equal expressions -> same operation and operands
    bool Expression::operator== (const Expression &e2) const 
	{
		return this->v1 == e2.v1 && this->v2 == e2.v2 && this->op == e2.op;
	}

    // less than
	bool Expression::operator< (const Expression &e2) const 
	{
		if (this->v1 == e2.v1)
			if (this->v2 == e2.v2)
				if (this->op == e2.op)
					return false;
				else
					return this->op < e2.op;
			else
				return this->v2 < e2.v2;
		else
			return this->v1 < e2.v1;
	}

	// print for Expression objects
    std::string Expression::toString() const 
    {
		std::string op = "?";
		switch (this->op) 
		{
			case Instruction::Add:
			case Instruction::FAdd: op = "+"; break;
			case Instruction::Sub:
			case Instruction::FSub: op = "-"; break;
			case Instruction::Mul:
			case Instruction::FMul: op = "*"; break;
			case Instruction::UDiv:
			case Instruction::FDiv:
			case Instruction::SDiv: op = "/"; break;
			case Instruction::URem:
			case Instruction::FRem:
			case Instruction::SRem: op = "%"; break;
			case Instruction::Shl: op = "<<"; break;
			case Instruction::AShr:
			case Instruction::LShr: op = ">>"; break;
			case Instruction::And: op = "&"; break;
			case Instruction::Or: op = "|"; break;
			case Instruction::Xor: op = "xor"; break;
			default: op = "op"; break;
		}
		return getShortValueName(v1) + " " + op + " " + getShortValueName(v2);
    }

	// print out a set of expressions 
    void printSet(std::vector<Expression> * x) 
    {
		bool first = true;
		errs() << "{";
		for (std::vector<Expression>::iterator it=x->begin(), iend=x->end(); it!=iend; ++it)
		{
			if (!first) 
				errs() << ", ";
			else  
				first = false; 
			errs() << (it->toString());
		}
		errs() << "}\n";
    }

    // get the variable on the left hand side function in LLVM
    //can call getName() -> does not work for instructions that are publically 'unnamed', but internally assigned a name like '%0' or '%1' 

	std::string getShortValueName(Value * v) 
	{
		if (v->getName().str().length() > 0) 
		    return "%" + v->getName().str();	
		else if (isa<Instruction>(v)) 
		{
			std::string s = "";
			raw_string_ostream * strm = new raw_string_ostream(s);
			v->print(*strm);
			std::string inst = strm->str();
			size_t idx1 = inst.find("%");
			size_t idx2 = inst.find(" ",idx1);
			if (idx1 != std::string::npos && idx2 != std::string::npos) 
				return inst.substr(idx1,idx2-idx1);
			else 
				return "\"" + inst + "\"";
		    
		}
		else if (ConstantInt * cint = dyn_cast<ConstantInt>(v)) 
		{
			std::string s = "";
			raw_string_ostream * strm = new raw_string_ostream(s);
			cint->getValue().print(*strm,true);
			return strm->str();
		}
		else 
		{
			std::string s = "";
			raw_string_ostream * strm = new raw_string_ostream(s);
			v->print(*strm);
			std::string inst = strm->str();
			return "\"" + inst + "\"";
		}
	}


    // printing utility functions
    void printBitVector(BitVector b)
    {
        unsigned int i;
        unsigned int b_size = b.size();

        if(b_size == 0)
            errs() << "-";
        else
        {
            for(i = 0; i < b.size() ; i++)
            {
                if(b[i] == true)
                    errs() << "1";
                else
                    errs() << "0";
            }
        }
        errs() << "\n";
    }

    void printResult(DataFlowResult output)
    {
        for(auto entry : output.result)
        {
           errs() << "BB " << entry.first->getName() << "\n";

            printBitVector(entry.second.in);
            printBitVector(entry.second.out);
        }
    }

    std::string printValue(Value* v)
    {
        std::string res; llvm::raw_string_ostream raw_st(res);
        v->print(raw_st);
        return res;
    }

    std::string getShortValueName(Value * v);

    std::string printSet(std::vector<void*> domain, BitVector on, int  mode) 
    {
        std::stringstream ss;
        ss << "{";
        int ind = 0;

        for (int i = 0; i < domain.size(); i++) 
        {
            // If element i is on
            if (on[i]) 
            {
                if (ind > 0)
                    ss << ",";

                switch(mode)
                {
                case 0:
                    // Value*
                    ss << " " << getShortValueName((Value*)domain[i]);
                    break;

                case 1:
                    // Expression*
                    ss << " " << ((Expression*)domain[i])->toString();
                    break;


                default:
                    errs() << "Invalid mode :: " << mode << "\n";
                    break;
                }


                ind++;
            }
        }

        ss << " }";

        return ss.str();
    }


}

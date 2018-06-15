#include "dataflow.cpp"

using namespace llvm;

namespace live
{

	/*--------Liveness Information--------*/
	
	typedef std::vector<string> varati;		// set of variable names
	std::map<int, varati> VarAtI;			// at every instruction
	typedef std::map<int, varati> ItoS;		
	std::map<string,ItoS> LiveAtF;			// for every function
	
	/*-------------------------------------*/

	class Liveness : public FunctionPass 
	{
		public:
		static char ID;

		Liveness() : FunctionPass(ID) 
		{
			Direction direction = Direction::BACKWARD;
			MeetOp meet_op = MeetOp::UNION;
			pass = LivenessAnalysis(direction, meet_op);
		}

		virtual void getAnalysisUsage(AnalysisUsage& AU) const 
		{
			AU.setPreservesAll();
		}

		private:

		class LivenessAnalysis : public DataFlow	// liveness analysis 
		{
			public:

			LivenessAnalysis() : DataFlow(Direction::INVALID_DIRECTION, MeetOp::INVALID_MEETOP)	{ }
			LivenessAnalysis(Direction direction, MeetOp meet_op) : DataFlow(direction, meet_op) { }

			protected:
			TransferOutput transferFn(BitVector input,  std::vector<void*> domain, std::map<void*, int> domainToIndex, BasicBlock* block)
			{

				TransferOutput transferOutput;

				int domainSize = domainToIndex.size();
				BitVector defSet(domainSize);	// defs
				BitVector useSet(domainSize);	// uses
				for (BasicBlock::iterator insn = block->begin(); insn != block->end(); ++insn) // locally exposed uses
				{
					if (PHINode* phi_insn = dyn_cast<PHINode>(&*insn))	// phi nodes - add operands to the list in transferOutput
					{
						for (int ind = 0; ind < phi_insn->getNumIncomingValues(); ind++) 
						{
							Value* val = phi_insn->getIncomingValue(ind);
							if (isa<Instruction>(val) || isa<Argument>(val))
							{
								BasicBlock* valBlock = phi_insn->getIncomingBlock(ind);
								
								if (transferOutput.neighborVals.find(valBlock) == transferOutput.neighborVals.end()) // neighborVals - no
								    transferOutput.neighborVals[valBlock] = BitVector(domainSize); // mapping for block, create one

								int valInd = domainToIndex[(void*)val];
								transferOutput.neighborVals[valBlock].set(valInd);	// set the bit corresponding to "val"
							}
						}
					}
					else // non-phi nodes - add operands to the use set
					{
						for (User::op_iterator opnd = insn->op_begin(), opE = insn->op_end(); opnd != opE; ++opnd) // go through uses
						{
							Value* val = *opnd;
							if (isa<Instruction>(val) || isa<Argument>(val)) 
							{
								int valInd = domainToIndex[(void*)val];
								if (!defSet[valInd])	// add to useSet only if not already defined in the block somewhere earlier
									useSet.set(valInd);
							}
						}
					}
					
					std::map<void*, int>::iterator iter = domainToIndex.find((void*)&*insn);	// definitions
					if (iter != domainToIndex.end())
						defSet.set((*iter).second);
				}

				transferOutput.element = defSet;	// Transfer function = useSet U (input - defSet)
				transferOutput.element.flip();		// complement of defSet
				transferOutput.element &= input;	// input - defSet = input INTERSECTION Complement of defSet
				transferOutput.element |= useSet;	// union useSet

				return transferOutput;
			}
		};

		LivenessAnalysis pass;	// pass

		virtual bool runOnFunction(Function &F)
		{
			std::string function_name = F.getName();	// print Information
			errs() << "FUNCTION :: " << function_name  << "\n";
			DataFlowResult output;

			std::vector<void*> domain;	// setup the pass
			
			VarAtI.clear();
			assignIndicesToInstrs(&F);

			for(inst_iterator II = inst_begin(F), IE = inst_end(F); II!=IE; ++II)	// fill in domain
			{
				Instruction& insn(*II);

				for (User::op_iterator OI = insn.op_begin(), OE = insn.op_end(); OI != OE; ++OI) // find insn-defined values, function args
				{
				    Value *val = *OI;
				    if (isa<Instruction>(val) || isa<Argument>(val))	// val is used by insn
				    {
				        if(std::find(domain.begin(),domain.end(),val) == domain.end())
				            domain.push_back((void*)val);
				    }
				}
			}

			errs() << "------------------------------------------\n\n";
			errs() << "DOMAIN :: " << domain.size() << "\n";
			for(void* element : domain)
			{
				errs() << "Element : " << *((Value*) element) << "\n"; // or use getShortValueName((Value*) element)
			}
			errs() << "------------------------------------------\n\n";

			BitVector boundaryCond(domain.size(), false);	// empty sets for liveness of variables
			BitVector initCond(domain.size(), false);

			output = pass.run(F, domain, boundaryCond, initCond);	// apply pass

			// PRINTING RESULTS - printResult(output);
			std::stringstream ss;	// use results to compute the final liveness - handle phi nodes

			for (Function::iterator BI = F.begin(), BE = F.end(); BI != BE; ++BI)
			{
				BasicBlock* block = &*BI;

				BitVector liveValues = output.result[block].out;	// liveness at OUT

				std::vector<std::string> revOut;	// print information in reverse order

				revOut.push_back("//===--------------------------------------------------------------------------------------------------------------------------===//");
				ss.clear();	// print live variables at the end of the block
				ss.str(std::string());
				ss << std::setw(WIDTH) << std::right;
				ss << printSet(domain, liveValues, 0);
				revOut.push_back(ss.str());

				for (BasicBlock::reverse_iterator insn = block->rbegin(), IE = block->rend(); insn != IE; ++insn)
				{	// iterate backward through the block, update liveness
				
					std::vector<string> s1; // hold live variables at an instruction

					revOut.push_back(std::string(WIDTH, ' ') + printValue(&*insn));	// add the instruction itself

					if (PHINode* phiInst = dyn_cast<PHINode>(&*insn))	// phi inst: kill LHS, but don't output liveness here
					{
					    std::map<void*, int>::const_iterator it = output.domainToIndex.find((void*)phiInst);
					    if (it != output.domainToIndex.end())
					        liveValues.reset(it->second);
					}
					else
					{
					    for (Instruction::op_iterator opnd = insn->op_begin(), opE = insn->op_end(); opnd != opE; ++opnd)
					    {	// Make values live when used as operands
					        Value* val = *opnd;
					        if (isa<Instruction>(val) || isa<Argument>(val))
					        {
					            int ind = output.domainToIndex[(void*)val];
					            liveValues.set(ind);
					        }
					    }

					    std::map<void*, int>::iterator it = output.domainToIndex.find((void*)(&*insn));
					    if (it != output.domainToIndex.end())	// if a value is defined, remove it from live set before that instruction
					        liveValues.reset(it->second);

					    // Print live variables
					    ss.clear();
					    ss.str(std::string());
					    ss << std::setw(WIDTH) << std::right;
					    ss << printSet(domain, liveValues, 0);
					    
					    	/*------store liveness information for later use-------*/
					    
							for (int i = 0; i < domain.size(); i++) 
								if (liveValues[i]) 
									s1.push_back(getShortValueName((Value*)domain[i]));	// set of variables available
					
							VarAtI.insert(std::pair<int,varati>(InstrToIndex[&*insn],s1));	// at the instruction point
							errs()<<*insn<<" Inserting here \n";
							/*------------------------------------------------------*/
					    
					    revOut.push_back(ss.str());
					}
				}

				revOut.push_back("//===--------------------------------------------------------------------------------------------------------------------------===//");

				for (std::vector<std::string>::reverse_iterator it = revOut.rbegin(); it != revOut.rend(); ++it)
				        errs() << *it << "\n";	// since we added strings in the reverse order, print them in reverse
			}

			LiveAtF.insert(std::pair<string,ItoS>(function_name,VarAtI));	// for every instr in a function set of vars available
			return false;	// no change in function
		}

		virtual bool runOnModule(Module& M)
		{
			bool modified = false;

			for (Module::iterator MI = M.begin(), ME = M.end(); MI != ME; ++MI)	// run analysis on each function
			{
				bool function_modified = false;
				function_modified = runOnFunction(*MI);
				modified |= function_modified;
			}

			return modified;
		}

	};
}

using namespace live;
char Liveness::ID = 0;
RegisterPass<Liveness> X("liveness", "Liveness Analysis");

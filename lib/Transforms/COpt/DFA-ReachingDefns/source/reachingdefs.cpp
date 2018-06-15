#include "dataflow.cpp"

namespace llvm	// utilities for finding names of definitions
{	
	Value* getDefinitionVar(Value* v) // Definitions are function args or store instructions or instructions with names
	{
		if (isa<Argument>(v))
			return v;
		else if (isa<StoreInst>(v)) 
			return ((StoreInst*)v)->getPointerOperand();
		else if (isa<Instruction>(v))
		{
			std::string str = printValue(v);
			const int VAR_NAME_START_IDX = 2;
			if (str.length() > VAR_NAME_START_IDX && str.substr(0,VAR_NAME_START_IDX+1) == "  %")
				return v;
		}
		return 0;
	}
	const int VAR_NAME_START_IDX = 2;
	std::string valueToDefinitionStr(Value* v) 
	{
		Value* def = getDefinitionVar(v);
		if (def == 0)
			return "";

		std::string str = printValue(v);
		if (isa<Argument>(v)) 
			return str;
		
		else 
		{
		  str = str.substr(VAR_NAME_START_IDX);
		  return str;
		}

		return "";
	}
	
	std::string valueToDefinitionVarStr(Value* v)	// return just the defined variable if definition
	{
  		Value* def = getDefinitionVar(v);
  		
  		if (def == 0)
    		return "";
		if (isa<Argument>(def) || isa<StoreInst>(def)) 
			return "%" + def->getName().str();
		else 
		{
			std::string str = printValue(def);
			int varNameEndIdx = str.find(' ',VAR_NAME_START_IDX);
			str = str.substr(VAR_NAME_START_IDX,varNameEndIdx-VAR_NAME_START_IDX);
			return str;
		}
	}	
}

using namespace llvm;

namespace reachdef
{

	/*--------Utilities for Reaching Definitions Information--------*/
	
	typedef std::vector<string> varati;		// set of definitions
	std::map<int, varati> VarAtI;			// at every instruction
	typedef std::map<int, varati> ItoS;		
	std::map<string,ItoS> LiveAtF;			// for every function
	
	/*-------------------------------------------------------------*/

	class ReachingDefinitions : public FunctionPass 
	{
		public:
		static char ID;

		ReachingDefinitions() : FunctionPass(ID) 
		{
			Direction direction = Direction::FORWARD;
			MeetOp meet_op = MeetOp::UNION;
			pass = ReachingDefinitionsAnalysis(direction, meet_op);
		}

		virtual void getAnalysisUsage(AnalysisUsage& AU) const 
		{
			AU.setPreservesAll();
		}

		private:

		class ReachingDefinitionsAnalysis : public DataFlow	// reaching definitions analysis 
		{
			public:

			ReachingDefinitionsAnalysis() : DataFlow(Direction::INVALID_DIRECTION, MeetOp::INVALID_MEETOP)	{ }
			ReachingDefinitionsAnalysis(Direction direction, MeetOp meet_op) : DataFlow(direction, meet_op) { }

			protected:
			
			TransferOutput transferFn(BitVector input,  std::vector<void*> domain, std::map<void*, int> domainToIndex, BasicBlock* block)
			{

				TransferOutput transferOutput;

					// expressions generated and killed in basic block
				int domainSize = domainToIndex.size();
				BitVector GenSet(domainSize);
				BitVector KillSet(domainSize);

				for (BasicBlock::iterator i = block->begin(), e = block->end(); i!=e; ++i)
				{
					Instruction * I = &*i;
					std::map<void*, int>::iterator defIt = domainToIndex.find((void*)(I));
					if (defIt != domainToIndex.end())	 //kill prior definitions , can be in this block's gen set
					{
						for (std::map<void*,int>::iterator prevDefIt=domainToIndex.begin(); prevDefIt!= domainToIndex.end(); ++prevDefIt)
						{
							std::string prevDefStr = valueToDefinitionVarStr((Value*)prevDefIt->first);
            				std::string currDefStr = valueToDefinitionVarStr((Value*)defIt->first);
            				if (prevDefStr == currDefStr)
            				{
								KillSet.set(prevDefIt->second);
								GenSet.reset(prevDefIt->second);
            				}
						}
						GenSet.set((*defIt).second);
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

		ReachingDefinitionsAnalysis pass;	// pass

		virtual bool runOnFunction(Function &F)
		{
			std::string function_name = F.getName();	// print Information
			errs() << "FUNCTION :: " << function_name  << "\n";
			DataFlowResult output;

			std::vector<void*> domain;	// setup the pass
			
			VarAtI.clear();
			assignIndicesToInstrs(&F);
			
			for (Function::arg_iterator arg = F.arg_begin(); arg != F.arg_end(); ++arg)	// fill in domain
				domain.push_back((void*)&*arg);
			for(inst_iterator II = inst_begin(F), IE = inst_end(F); II!=IE; ++II)	
			{
				if (!valueToDefinitionStr(&*II).empty())
					domain.push_back((void*)&*II);
			}

			errs() << "------------------------------------------\n\n";
			errs() << "DOMAIN :: " << domain.size() << "\n";
			for(void* element : domain)
			{
				errs() << "Element : " << *((Value*) element) << "\n"; // or use getShortValueName((Value*) element)
			}
			errs() << "------------------------------------------\n\n";

			BitVector boundaryCond(domain.size(), false);	// empty sets for reaching definitions
			for (int i = 0; i < domain.size(); i++)
      			if (isa<Argument>((Value*)domain[i]))
					boundaryCond.set(i);
			BitVector initCond(domain.size(), false);

			output = pass.run(F, domain, boundaryCond, initCond);	// apply pass

			// PRINTING RESULTS - printResult(output);
			std::stringstream ss;	// use results to compute the final liveness - handle phi nodes

			for (Function::iterator BI = F.begin(), BE = F.end(); BI != BE; ++BI)
			{
				BasicBlock* block = &*BI;

				BitVector reachDefs = output.result[block].in;	// value at IN, forward analysis

				std::vector<std::string> rOut;	// print information in order

				rOut.push_back("//===--------------------------------------------------------------------------------------------------------------------------===//");
				ss.clear();	// print definitions reaching the beginning of the block
				ss.str(std::string());
				ss << std::setw(WIDTH) << std::right;
				ss << printSet(domain, reachDefs, 0);
				rOut.push_back(ss.str());

				for (BasicBlock::iterator insn = block->begin(), IE = block->end(); insn != IE; ++insn)
				{	// iterate forward through the block, update liveness
				
					std::vector<string> s1; // hold live variables at an instruction

					rOut.push_back(std::string(WIDTH, ' ') + printValue(&*insn));	// add the instruction itself

				    std::map<void*, int>::iterator it = output.domainToIndex.find((void*)(&*insn));
				    std::string currDefStr = valueToDefinitionVarStr(&*insn);
				    for (it = output.domainToIndex.begin(); it != output.domainToIndex.end(); ++it)	// kill existing defs for this variable
				    {
      					std::string prevDefStr = valueToDefinitionVarStr((Value*)it->first);
      					if (prevDefStr == currDefStr)
        					reachDefs.reset(it->second);
    				}
				    if (it != output.domainToIndex.end())	// add this definition to the reaching set
				        reachDefs.set(it->second);
					
				    // print reaching definitions
				    ss.clear();
				    ss.str(std::string());
				    ss << std::setw(WIDTH) << std::right;
				    ss << printSet(domain, reachDefs, 0);
				    
				    	/*------store reaching definitions information for later use-------*/
						for (int i = 0; i < domain.size(); i++) 
							if (reachDefs[i])
							{ 
								string insertstr = getShortValueName((Value*)domain[i]);//.substr(0,getShortValueName((Value*)domain[i]).find()",");
								if (insertstr.find(",")<strlen(insertstr.c_str()))
									insertstr = insertstr.substr(0,insertstr.find(","));
					
								if (find(s1.begin(),s1.end(),insertstr)==s1.end())
									s1.push_back(getShortValueName((Value*)domain[i]));	// set of variables available
							}
				
						VarAtI.insert(std::pair<int,varati>(InstrToIndex[&*insn],s1));	// at the instruction point
						/*------------------------------------------------------*/
				    
				    rOut.push_back(ss.str());
					
				}

				rOut.push_back("//===--------------------------------------------------------------------------------------------------------------------------===//");

				for (std::vector<std::string>::iterator it = rOut.begin(); it != rOut.end(); ++it)
				        errs() << *it << "\n";	
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

using namespace reachdef;
char ReachingDefinitions::ID = 0;
RegisterPass<ReachingDefinitions> X("reachdefs", "Reaching Definitions Analysis");

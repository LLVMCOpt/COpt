#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "coptast.h"
#include "coptsemantic.h"

FILE *file;
FILE *file1;
FILE *file2;
FILE *file3;
FILE *file4;

extern int check;	// semantic error
LinkedListNode *qhead;	// head of linked queue of AST nodes

char path[75];
char temppath[75];

int iterate_functionLine = 0;
int iterate_BBLine = 0;
int iterate_instrLine = 0;
int iterate_loopLine = 0;
int iterate_loopBBLine = 0;
int iterate_loopInstrLine = 0;

int bracesCount = 0;

int FI_makeInlineIterCheck = 1;
int FI_functionExists = 0;
int FI_iterateClose = 0;
int FI_paramsCount = 0;
int FI_removeFn = 0;

int CF_instrExists = 0;
int CF_ifStatement = 0;
int CF_IterCheck = 0;
int CF_IterCheckHelper1 = 0;
int CF_IterCheckHelper2 = 0;
int CF_IterCheckHelper3 = 0;

int DCE_noTimes = 1;

int LCSE_dagCreated = 0;
int LCSE_paramsCount = 0;
int LCSE_threshSpecified = 0;

int GCSE_fnRef = 0;
int GCSE_functionExists = 0;
int GCSE_findAvExIterCheck = 0;
int GCSE_gcseIterCheck = 0;
int GCSE_avexComputed = 0;
int GCSE_paramsCount = 0;
int GCSE_threshSpecified = 0;

int LICM_fnRef = 0;
int LICM_functionExists = 0;
int LICM_helper1 = 0;
int LICM_fnhelper = 0;
int LICM_loophelper = 0;
int LICM_fnIterCheck = 0;
int LICM_loopIterCheck = 0;
int LICM_findReachDefsIterCheck = 0;
int LICM_findLoopInvStmts = 0;
int LICM_findMotionCands = 0;
int LICM_applyCodeMotion = 0;
int LICM_rdefsComputed = 0;
int LICM_loopinvComputed = 0;
int LICM_motionCanComputed = 0;
int LICM_loopExists = 0;

int GVN_instrExists = 0;

void semanticAnalysis(nodeType *root, int optNo)
{
	iterate_functionLine = 0;iterate_BBLine = 0;iterate_instrLine = 0;iterate_loopLine = 0;iterate_loopBBLine = 0;iterate_loopInstrLine = 0;bracesCount = 0;FI_makeInlineIterCheck = 1;FI_functionExists = 0;FI_iterateClose = 0;FI_paramsCount = 0;FI_removeFn = 0;CF_instrExists = 0;CF_ifStatement = 0;CF_IterCheck = 0;CF_IterCheckHelper1 = 0;CF_IterCheckHelper2 = 0;CF_IterCheckHelper3 = 0;DCE_noTimes = 1;LCSE_dagCreated = 0;LCSE_paramsCount = 0;LCSE_threshSpecified = 0;GCSE_fnRef = 0;GCSE_functionExists = 0;GCSE_findAvExIterCheck = 0;GCSE_gcseIterCheck = 0;GCSE_avexComputed = 0;GCSE_paramsCount = 0;GCSE_threshSpecified = 0;LICM_fnRef = 0;LICM_functionExists = 0;LICM_helper1 = 0;LICM_fnhelper = 0;LICM_loophelper = 0;LICM_fnIterCheck = 0;LICM_loopIterCheck = 0;LICM_findReachDefsIterCheck = 0;LICM_findLoopInvStmts = 0;LICM_findMotionCands = 0;LICM_applyCodeMotion = 0;LICM_rdefsComputed = 0;LICM_loopinvComputed = 0;LICM_motionCanComputed = 0;LICM_loopExists = 0;GVN_instrExists = 0;
	
	switch(optNo)
	{
		case 1:	strcpy(path,"../Opt-FunctionInlining/source/");
				break;
		case 2:	strcpy(path,"../Opt-ConstantPropagation&Folding/source/");
				break;
		case 3:	strcpy(path,"../Opt-DCE/try2/source/");
				break;
		case 4:	strcpy(path,"../Opt-LCSE/source/");
				break;
		case 5:	strcpy(path,"../Opt-IndVarElim/source/");
				break;
		case 6:	strcpy(path,"../Opt-GCSE/source/");
				break;
		case 7:	strcpy(path,"../Opt-LICM/try2/source/");
				break;
		case 8:	strcpy(path,"../Opt-GVN/source/");
				break;
		case 9:	strcpy(path,"../Opt-LoopUnrolling/source/");
				break;
		case 10: strcpy(path,"../Opt-TRE/source/");
				break;
		default: strcpy(path,"");
				break;
	}
	
	strcpy(temppath,path);
	remove(strcat(temppath,"Optimiser.cpp"));
	strcpy(temppath,path);
	remove(strcat(temppath,"OptimiserDataflow.h"));
	strcpy(temppath,path);
	remove(strcat(temppath,"OptimiserDataflow.cpp"));
	strcpy(temppath,path);
	remove(strcat(temppath,"OptimiserAvEx.cpp"));
	strcpy(temppath,path);
	remove(strcat(temppath,"OptimiserReachingDefs.cpp"));
	strcpy(temppath,path);

	file = fopen(strcat(temppath,"Optimiser.cpp"),"a");
	qhead = NULL;
	semanticAnalysisRecursive(root);
	processNodeQueue();
	
	fclose(file);
}

void semanticAnalysisRecursive(nodeType *node)
{
	if (node == NULL)
		return;

	if (node->type==typeConst)
	{
		addToLLQueue(node);
		return;
	}
	else
	{
		if (strcmp(node->opr.oper,"fi_iteratestart")==0)
		{
			FI_makeInlineIterCheck = 0;
			checkFunctionInliningIter(node);
		}
		if (strcmp(node->opr.oper,"fiparams")==0)
		{
			FI_paramsCount = 0;
			countMakeInlineParams(node);
		}
		if (strcmp(node->opr.oper,"cf_iteratestart")==0)
		{
			CF_IterCheck = 0; CF_IterCheckHelper1 = 0; CF_IterCheckHelper2 = 0; CF_IterCheckHelper3 = 0;
			checkConstantFoldingIter(node);
		}
		if (strcmp(node->opr.oper,"gcse_iteratestart")==0)
		{
			checkGCSEIter(node);
		}
		if (strcmp(node->opr.oper,"licm_iteratestart")==0)
		{
			LICM_helper1 = 0; LICM_fnIterCheck = 0; LICM_loopIterCheck = 0; LICM_fnhelper = 0; LICM_loophelper = 0;
			checkLICMIter(node);
		}
		if (strcmp(node->opr.oper,"gvn_iteratestart")==0)
		{
			checkGVNIter(node);
		}
		if (strcmp(node->opr.oper,"lcseparams")==0)
		{
			LCSE_paramsCount = 0;
			countLCSEParams(node);
		}
		if (strcmp(node->opr.oper,"gcseparams")==0)
		{
			GCSE_paramsCount = 0;
			countGCSEParams(node);
		}
		for (int i = 0; i < node->opr.nops; i++)
			semanticAnalysisRecursive(node->opr.op[i]);
	}
}

void addToLLQueue(nodeType *node)
{
	LinkedListNode *newnode = (LinkedListNode *)malloc(sizeof(LinkedListNode));
	newnode->llnode = node;
	newnode->next = NULL;
	
	if (qhead == NULL)
	{
		qhead = newnode;
		return;
	}

	LinkedListNode *curr = qhead;
	while (curr->next!=NULL)
		curr = curr->next;
		
	curr->next = newnode;
}

void writeDataflowFiles()
{
	strcpy(temppath,path);
	file1 = fopen(strcat(temppath,"OptimiserDataflow.h"),"a");
	fprintf(file1, "#ifndef DATAFLOW_H\
	\n#define DATAFLOW_H\
	\
	\n#include \"llvm/ADT/BitVector.h\"\
	\n#include \"llvm/ADT/DenseMap.h\"\
	\n#include \"llvm/ADT/PostOrderIterator.h\"\
	\n#include \"llvm/ADT/SmallSet.h\"\
	\n#include \"llvm/IR/Constants.h\"\
	\n#include \"llvm/IR/CFG.h\"\
	\n#include \"llvm/IR/Function.h\"\
	\n#include \"llvm/IR/Instructions.h\"\
	\n#include \"llvm/IR/InstIterator.h\"\
	\n#include \"llvm/IR/Module.h\"\
	\n#include \"llvm/IR/ValueMap.h\"\
	\n#include \"llvm/Pass.h\"\
	\n#include \"llvm/Support/raw_ostream.h\"\
	\
	\n#include <cstdio>\
	\n#include <iostream>\
	\n#include <vector>\
	\n#include <map>\
	\n#include <sstream>\
	\n#include <string>\
	\n#include <iomanip>\
	\
	\nusing namespace std;\
	\
	\n#define WIDTH 60\
	\
	\nnamespace llvm\
	\n{\
	\
	\n	std::map<int, Instruction *> IndexToInstr;\
	\n	std::map<Instruction *, int> InstrToIndex;\
	\n	void assignIndicesToInstrs(Function * F); \
	\
	\n	typedef std::vector<BitVector> BitVectorList;\
	\n	typedef std::vector<BasicBlock*> BasicBlockList;\
	"       
	"\n	struct TransferOutput\
	\n	{\
	\n		BitVector element;			// for output of a transfer function.\
	\n		std::map<BasicBlock*, BitVector> neighborVals;	// list of BitVectors for predecessors/successors, used to handle phi nodes"			
	"\n	};\
	\n\
	\n	struct BlockResult	// IN and OUT sets for a basic block\
	\n	{\
	\n		BitVector in;\
	\n		BitVector out;\
	\n		TransferOutput transferOutput;	// variable to store the temporary output of the transfer function\
	\n	};\
	\n\
	\n	enum Direction\
	\n	{\
	\n		INVALID_DIRECTION,\
	\n		FORWARD,\
	\n		BACKWARD\
	\n	};\
	\n\
	\n	enum MeetOp\
	\n	{\
	\n		INVALID_MEETOP,\
	\n		UNION,\
	\n		INTERSECTION\
	\n	};\
	\n\
	\n	struct DataFlowResult	// result of a data flow pass on a function\
	\n	{\
	\n		std::map<BasicBlock*, BlockResult> result;	// basic block to result\
	\n		std::map<void*, int> domainToIndex;	// domain elements to indices in bitvectors - which bits are values\
	\n		bool modified;\
	\n	};\
	\n\
	\n	class DataFlow	// base class for data flow analysis, each analysis will be a subclass\
	\n	{\
	\n		public:\
	\n\
	\n			DataFlow(Direction direction, MeetOp meetup_op) : direction(direction), meetup_op(meetup_op)	// constructor\
	\n			{																							}\
	\n\
	\n			BitVector applyMeetOp(BitVectorList inputs);	// apply meet operator\
	\n	\
	\n			DataFlowResult run(Function &F, std::vector<void*> domain, BitVector boundaryCond, BitVector initCond); // analyse F\
	\n\
	\n		protected:\
	\n\
	\n			virtual TransferOutput transferFn(BitVector input, std::vector<void*> domain, std::map<void*, int> domainToIndex, BasicBlock* block) = 0;	// transfer function - takes IN/OUT, gives IN/OUT\
	\n\
	\n		private:\
	\n\
	\n			Direction direction;\
	\n			MeetOp meetup_op;\
	\n	};\
	\n	// printing utility functions\
	\n\
	\n	void printBitVector(BitVector b);\
	\n\
	\n	void printResult(DataFlowResult output);\
	\n\
	\n	std::string printValue(Value* v);\
	\n\
	\n	std::string printSet(std::vector<void*> domain, BitVector on, int mode);\
	\n\
	\n	class Expression // EXPRESSION-RELATED UTILS\
	\n	{\
	\n		public:\
	\n			Value * v1;\
	\n			Value * v2;\
	\n			Instruction::BinaryOps op;\
	\n			Expression () {}\
	\n			Expression (Instruction * I);\
	\n			bool operator== (const Expression &e2) const;\
	\n			bool operator< (const Expression &e2) const;\
	\n			std::string toString() const;\
	\n	};\n\n\tstd::string getShortValueName(Value * v);\n\tvoid printSet(std::vector<Expression> * x);	// print out a set of expressions in a nice format\n}\n#endif");
	fclose(file1);
					
	strcpy(temppath,path);
	file2 = fopen(strcat(temppath,"OptimiserDataflow.cpp"),"a");
	fprintf(file2, "#include \"OptimiserDataflow.h\" \
	\n\
	\nusing namespace std;\
	\n\
	\nnamespace llvm \
	\n{\
	\n\
	\n	void assignIndicesToInstrs(Function * F) \
	\n	{ \n\t\t\tInstrToIndex[nullptr] = 0;\n\t\t\tIndexToInstr[0] = nullptr;\
	\n			unsigned counter = 1;\
	\n			for (inst_iterator I = inst_begin(F), E = inst_end(F); I != E; ++I) \
	\n			{\
	\n				Instruction * instr = &*I;\
	\n				InstrToIndex[instr] = counter;\
	\n				IndexToInstr[counter] = instr;\
	\n				counter++;\
	\n			}\
	\n\
	\n			return;\
	\n	}\
	\n\
	\n	BitVector DataFlow::applyMeetOp(BitVectorList inputs)	// implementing meet operator\
	\n	{\
	\n		BitVector result;\
	\n\
	\n		if (!inputs.empty())\
	\n		{\
	\n		    for (int i = 0; i < inputs.size(); i++)\
	\n		    {\
	\n		        if (i == 0)\
	\n		        {\
	\n		            result = inputs[i];\
	\n				}\
	\n		        else \
	\n		        {\
	\n		            if (meetup_op == MeetOp::UNION)\
	\n		            {\
	\n		                result |= inputs[i];\
	\n		            }\
	\n		            else if (meetup_op == MeetOp::INTERSECTION)\
	\n		            {\
	\n		                result &= inputs[i];\
	\n		            }\
	\n		            else\
	\n		            {\
	\n		                errs() << \"Unknown Meetop\\n\";\
	\n		                exit(EXIT_FAILURE);\
	\n		            }\
	\n		        }\
	\n		    }\
	\n		}\
	\n\
	\n		return result;\
	\n	}\
	\n\
	\n	DataFlowResult DataFlow::run(Function &F, std::vector<void*> domain, BitVector boundaryCond, BitVector initCond)	// analyse F\
	\n	{\
	\n		std::map<BasicBlock*, BlockResult> result;\
	\n		bool modified = false;\
	\n		\
	\n		std::map<void*, int> domainToIndex;	// mapping domain values to index in bitvector\
	\n		for (int i = 0; i < domain.size(); i++)\
	\n			domainToIndex[(void*)domain[i]] = i;\
	\n\
	\n		BasicBlockList boundaryBlocks;	// get extreme blocks\
	\n		switch (direction) \
	\n		{\
	\n			case Direction::FORWARD: 	boundaryBlocks.push_back(&F.front());	// entry - first block\
	\n										break;		\
	\n				\
	\n			case Direction::BACKWARD:	for(Function::iterator I = F.begin(), E = F.end(); I != E; ++I)	// return statements\
	\n											if (isa<ReturnInst>(I->getTerminator()))\
	\n												boundaryBlocks.push_back(&*I);\
	\n										break;\
	\n\
	\n			default:	errs() << \"Invalid direction \\n\";\
	\n						break;\
	\n		}\
	\n\
	\n		BlockResult boundaryRes = BlockResult();	// initialise boundary blocks\
	\n		BitVector *value = (direction == Direction::FORWARD) ? &boundaryRes.in : &boundaryRes.out;	// forward analysis - initialise IN 																									// backward analysis - initialise OUT	\
	\n		*value = boundaryCond;\
	\n		boundaryRes.transferOutput.element = boundaryCond;\
	\n		for (BasicBlockList::iterator I = boundaryBlocks.begin(), E = boundaryBlocks.end(); I != E; ++I) \
	\n			result[*I] = boundaryRes;	\
	\n\
	\n\
	\n		BlockResult intRes = BlockResult();	// initialise interior blocks\
	\n		BitVector* intVal = (direction == Direction::FORWARD) ? &intRes.out : &intRes.in;	// forward analysis - initialise IN\
	\n		BitVector* intOtherVal = (direction == Direction::FORWARD) ? &intRes.in : &intRes.out;	// backward analysis - initialise OUT\
	\n\
	\n		*intVal = initCond;\
	\n		*intOtherVal = initCond;\
	\n		intRes.transferOutput.element = initCond;\
	\n		for (Function::iterator BB = F.begin(), BE = F.end(); BB != BE; ++BB)\
	\n		{\
	\n		    if (std::find(boundaryBlocks.begin(),boundaryBlocks.end(),(BasicBlock*)&*BB) == boundaryBlocks.end())\
	\n		    {\
	\n		        result[(BasicBlock*)&*BB] = intRes;	// not one of the boundary blocks\
	\n		    }\
	\n		}\
	\n\
	\n		// neighbor list - forward analysis->predecessors, backward analysis->successors\
	\n		std::map<BasicBlock*, BasicBlockList > blockNeighbors;\
	\n\
	\n		for (Function::iterator BB = F.begin(), BE = F.end(); BB != BE; ++BB) \
	\n		{\
	\n			BasicBlockList neighborList;\
	\n\
	\n		    switch (direction) \
	\n		    {\
	\n				case Direction::FORWARD:	for (pred_iterator neighbor = pred_begin(&*BB), E = pred_end(&*BB); neighbor != E; ++neighbor)\
	\n				        						neighborList.push_back(*neighbor);\
	\n				    						break;\
	\n\
	\n				case Direction::BACKWARD:	for (succ_iterator neighbor = succ_begin(&*BB), E = succ_end(&*BB); neighbor != E; ++neighbor)\
	\n												neighborList.push_back(*neighbor);\
	\n											break;\
	\n\
	\n				default:	errs() << \"Invalid direction \\n\";\
	\n				    		break;\
	\n		    }\
	\n\
	\n			blockNeighbors[&*BB] = neighborList;\
	\n		}\
	\n\
	\n		// order to traverse BasicBlocks\
	\n		BasicBlockList traversalOrder;\
	\n\
	\n		switch (direction) \
	\n		{\
	\n\
	\n			case Direction::FORWARD:{\
	\n										ReversePostOrderTraversal<Function*> TR(&F);\
	\n										for (ReversePostOrderTraversal<Function*>::rpo_iterator I = TR.begin(), E = TR.end(); I != E; ++I) \
	\n											traversalOrder.push_back(*I);\
	\n									}\
	\n									break;\
	\n									\
	\n			case Direction::BACKWARD:{\
	\n							for (po_iterator<BasicBlock*> I = po_begin(&F.getEntryBlock()), E = po_end(&F.getEntryBlock()); I != E; ++I) \
	\n				    			traversalOrder.push_back(*I);			\
	\n									}\
	\n		    						break;\
	\n\
	\n			default:	errs() << \"Invalid direction \\n\";\
	\n						break;\
	\n		}\
	\n\
	\n		// keep processing blocks until convergence\
	\n		bool converged = false;\
	\n		while (!converged) \
	\n		{\
	\n			converged = true;\
	\n\
	\n			for (BasicBlockList::iterator BB = traversalOrder.begin(), BE = traversalOrder.end(); BB != BE; ++BB)\
	\n			{\
	\n				BlockResult& blockRes = result[*BB];	// current result for the block\
	\n\
	\n				BlockResult oldBlockRes = blockRes;		// store it to check if it changed at the end\
	\n				BitVector oldVal = (direction == Direction::FORWARD) ? blockRes.out : blockRes.in;\
	\n\
	\n				BitVectorList meetInputs;	// collecting neighbor results for meet\
	\n				for (BasicBlockList::iterator NI = blockNeighbors[*BB].begin(), NE = blockNeighbors[*BB].end(); NI != NE; ++NI)\
	\n				{\
	\n					BlockResult& neighborRes = result[*NI];\
	\n					BitVector neighVal = neighborRes.transferOutput.element;\
	\n\
	\n				    // union the value if we find a match with neighbor-specific value\
	\n					std::map<BasicBlock*, BitVector>::iterator match = neighborRes.transferOutput.neighborVals.find(*BB);\
	\n					if (match != neighborRes.transferOutput.neighborVals.end()) \
	\n						neighVal |= match->second;\
	\n\
	\n				    meetInputs.push_back(neighVal);\
	\n				}\
	\n\
	\n				// apply the meet operator and store in in or out based on forward or backward\
	\n				BitVector* blockInput = (direction == Direction::FORWARD) ? &blockRes.in : &blockRes.out;\
	\n				if (!meetInputs.empty())\
	\n					*blockInput = applyMeetOp(meetInputs);\
	\n\
	\n				// apply transfer function for input to output\
	\n				blockRes.transferOutput = transferFn(*blockInput, domain, domainToIndex, *BB);\
	\n				BitVector* blockOutput = (direction == Direction::FORWARD) ? &blockRes.out : &blockRes.in;\
	\n				*blockOutput = blockRes.transferOutput.element;\
	\n\
	\n				if (converged)\
	\n				{\
	\n					if (*blockOutput!= oldVal||blockRes.transferOutput.neighborVals.size()!= oldBlockRes.transferOutput.neighborVals.size())\
	\n				        converged = false;\
	\n				}\
	\n			}\
	\n		}\
	\n\
	\n		// output\
	\n		DataFlowResult output;\
	\n		output.domainToIndex = domainToIndex;\
	\n		output.result = result;\
	\n		output.modified = modified;\
	\n\
	\n		return output;\
	\n	}\
	\n	// Expression class - expressions for the Available Expression analysis\
	\n	Expression::Expression(Instruction * I) \
	\n	{\
	\n		if (BinaryOperator * BO = dyn_cast<BinaryOperator>(I)) \
	\n		{\
	\n			this->v1 = BO->getOperand(0);\
	\n			this->v2 = BO->getOperand(1);\
	\n			this->op = BO->getOpcode();\
	\n		}\
	\n	}\
	\n\
	\n    // equal expressions -> same operation and operands\
	\n    bool Expression::operator== (const Expression &e2) const \
	\n	{\
	\n		return this->v1 == e2.v1 && this->v2 == e2.v2 && this->op == e2.op;\
	\n	}\
	\n\
	\n    // less than\
	\n	bool Expression::operator< (const Expression &e2) const \
	\n	{\
	\n		if (this->v1 == e2.v1)\
	\n			if (this->v2 == e2.v2)\
	\n				if (this->op == e2.op)\
	\n					return false;\
	\n				else\
	\n					return this->op < e2.op;\
	\n			else\
	\n				return this->v2 < e2.v2;\
	\n		else\
	\n			return this->v1 < e2.v1;\
	\n	}\
	\n// print for Expression objects\
	\n    std::string Expression::toString() const \
	\n    {\
	\n		std::string op = \"?\";\
	\n		switch (this->op) \
	\n		{\
	\n			case Instruction::Add:\
	\n			case Instruction::FAdd: op = \"+\"; break;\
	\n			case Instruction::Sub:\
	\n			case Instruction::FSub: op = \"-\"; break;\
	\n			case Instruction::Mul:\
	\n			case Instruction::FMul: op = \"*\"; break;\
	\n			case Instruction::UDiv:\
	\n			case Instruction::FDiv:\
	\n			case Instruction::SDiv: op = \"/\"; break;\
	\n			case Instruction::URem:\
	\n			case Instruction::FRem:\
	\n			case Instruction::SRem: op = \"%%\"; break;\
	\n			case Instruction::Shl: op = \"<<\"; break;\
	\n			case Instruction::AShr:\
	\n			case Instruction::LShr: op = \">>\"; break;\
	\n			case Instruction::And: op = \"&\"; break;\
	\n			case Instruction::Or: op = \"|\"; break;\
	\n			case Instruction::Xor: op = \"xor\"; break;\
	\n			default: op = \"op\"; break;\
	\n		}\
	\n		return getShortValueName(v1) + \" \" + op + \" \" + getShortValueName(v2);\
	\n    }\
	\n\
	\n	// print out a set of expressions \
	\n    void printSet(std::vector<Expression> * x) \
	\n    {\
	\n		bool first = true;\
	\n		errs() << \"{\";\
	\n		for (std::vector<Expression>::iterator it=x->begin(), iend=x->end(); it!=iend; ++it)\
	\n		{\
	\n			if (!first) \
	\n				errs() << \", \";\
	\n			else  \
	\n				first = false; \
	\n			errs() << (it->toString());\
	\n		}\
	\n		errs() << \"}\\n\";\
	\n    }\
	\n\
	\n    // get the variable on the left hand side function in LLVM \
	\n\
	\n	std::string getShortValueName(Value * v) \
	\n	{\
	\n		if (v->getName().str().length() > 0) \
	\n		    return \"%%\" + v->getName().str();	\
	\n		else if (isa<Instruction>(v)) \
	\n		{\
	\n			std::string s = \"\";\
	\n			raw_string_ostream * strm = new raw_string_ostream(s);\
	\n			v->print(*strm);\
	\n			std::string inst = strm->str();\
	\n			size_t idx1 = inst.find(\"%%\");\
	\n			size_t idx2 = inst.find(\" \",idx1);\
	\n			if (idx1 != std::string::npos && idx2 != std::string::npos) \
	\n				return inst.substr(idx1,idx2-idx1);\
	\n			else \
	\n				return \"\\\"\" + inst + \"\\\"\";\
	\n		    \
	\n		}\
	\n		else if (ConstantInt * cint = dyn_cast<ConstantInt>(v)) \
	\n		{\
	\n			std::string s = \"\";\
	\n			raw_string_ostream * strm = new raw_string_ostream(s);\
	\n			cint->getValue().print(*strm,true);\
	\n			return strm->str();\
	\n		}\
	\n		else \
	\n		{\
	\n			std::string s = \"\";\
	\n			raw_string_ostream * strm = new raw_string_ostream(s);\
	\n			v->print(*strm);\
	\n			std::string inst = strm->str();\
	\n			return \"\\\"\" + inst + \"\\\"\";\
	\n		}\
	\n	}\
	\n\
	\n\
	\n    // printing utility functions\
	\n    void printBitVector(BitVector b)\
	\n    {\
	\n        unsigned int i;\
	\n        unsigned int b_size = b.size();\
	\n\
	\n        if(b_size == 0)\
	\n            errs() << \"-\";\
	\n        else\
	\n        {\
	\n            for(i = 0; i < b.size() ; i++)\
	\n            {\
	\n                if(b[i] == true)\
	\n                    errs() << \"1\";\
	\n                else\
	\n                    errs() << \"0\";\
	\n            }\
	\n        }\
	\n        errs() << \"\\n\";\
	\n    }\
	\n\
	\n    void printResult(DataFlowResult output)\
	\n    {\
	\n        for(auto entry : output.result)\
	\n        {\
	\n           errs() << \"BB \" << entry.first->getName() << \"\\n\";\
	\n\
	\n            printBitVector(entry.second.in);\
	\n            printBitVector(entry.second.out);\
	\n        }\
	\n    }\
	\n\
	\n    std::string printValue(Value* v)\
	\n    {\
	\n        std::string res; llvm::raw_string_ostream raw_st(res);\
	\n        v->print(raw_st);\
	\n        return res;\
	\n    }\
	\n\
	\n    std::string getShortValueName(Value * v);\
	\n\
	\n    std::string printSet(std::vector<void*> domain, BitVector on, int  mode) \
	\n    {\
	\n        std::stringstream ss;\
	\n        ss << \"{\";\
	\n        int ind = 0;\
	\n\
	\n        for (int i = 0; i < domain.size(); i++) \
	\n        {\
	\n            // If element i is on\
	\n            if (on[i]) \
	\n            {\
	\n                if (ind > 0)\
	\n                    ss << \",\";\
	\n\
	\n                switch(mode)\
	\n                {\
	\n                case 0:\
	\n                    // Value*\
	\n\n                    ss << \" \" << getShortValueName((Value*)domain[i]);\
	\n                    break;\
	\n\
	\n                case 1:\
	\n                    // Expression*\
	\n                    ss << \" \" << ((Expression*)domain[i])->toString();\
	\n                    break;\
	\n\
	\n\
	\n                default:\
	\n                    errs() << \"Invalid mode :: \" << mode << \"\\n\";\
	\n                    break;\
	\n                }\
	\n\
	\n\
	\n                ind++;\
	\n            }\
	\n        }\
	\n\
	\n        ss << \" }\";\
	\n\
	\n        return ss.str();\
	\n    }\
	\n\
	\n\
	\n}");
	fclose(file2);
}

void writeAvailableExprsFiles()
{
	strcpy(temppath,path);
	file3 = fopen(strcat(temppath,"OptimiserAvEx.cpp"),"a");
	fprintf(file3, "#include \"OptimiserDataflow.cpp\"\
	\n\
	\nusing namespace llvm;\
	\n\
	\nnamespace av\
	\n{\
	\n	/*--------Utilities for Available Expression Information--------*/\
	\n	\
	\n	typedef std::vector<Expression*> expati;// set of expressions\
	\n	std::map<int, expati> AvAtI;			// at every instruction\
	\n	typedef std::map<int, expati> ItoE;		\
	\n	std::map<string,ItoE> AvExF;			// for every function\
	\n	std::map<string,Instruction *> where;\
	\n	\
	\n	std::map<string,expati> possibleExps;\
	\n	\
	\n	void removeFromAvailable(string fname, Expression *remexp)\
	\n	{\
	\n		ItoE *t1 = &AvExF[fname];\
	\n		for (auto itemp : *t1)\
	\n		{\
	\n			for (auto itemp2 : itemp.second)\
	\n			{\
	\n				if (strcmp(itemp2->toString().c_str(),remexp->toString().c_str())==0)\
	\n				{\
	\n					itemp.second.erase(std::remove(itemp.second.begin(), itemp.second.end(), itemp2), itemp.second.end());\
	\n					AvExF[fname][itemp.first]=itemp.second;\
	\n				}\
	\n			}\
	\n		}\
	\n	}\
	\n	\
	\n	bool compareExpr(Expression *e1, Expression *e2, int n)\
	\n	{\
	\n		if (n==1)\
	\n		{\
	\n			LoadInst *LI1 = dyn_cast<LoadInst>(e1->v1);\
	\n			LoadInst *LI2 = dyn_cast<LoadInst>(e1->v2);\
	\n			LoadInst *newLI1 = dyn_cast<LoadInst>(e2->v1);\
	\n			LoadInst *newLI2 = dyn_cast<LoadInst>(e2->v2);\
	\n												\
	\n			if (LI1->getPointerOperand()==newLI1->getPointerOperand() && LI2->getPointerOperand()==newLI2->getPointerOperand())\
	\n				return true;\
	\n		}\
	\n		else if (n==2)\
	\n		{\
	\n			LoadInst *LI1 = dyn_cast<LoadInst>(e1->v1);\
	\n			LoadInst *newLI1 = dyn_cast<LoadInst>(e2->v1);\
	\n												\
	\n			if (LI1->getPointerOperand()==newLI1->getPointerOperand() && e1->v2==e2->v2)\
	\n				return true;\
	\n		}\
	\n		else if (n==3)\
	\n		{\
	\n			LoadInst *LI2 = dyn_cast<LoadInst>(e1->v2);\
	\n			LoadInst *newLI2 = dyn_cast<LoadInst>(e2->v2);\
	\n												\
	\n			if (e1->v1==e2->v1 && LI2->getPointerOperand()==newLI2->getPointerOperand())\
	\n				return true;\
	\n		}\
	\n		return false;\
	\n	}\
	\n	\
	\n	int checkExpr(Expression *e)\
	\n	{\
	\n		if (dyn_cast<LoadInst>(e->v1))\
	\n		{\
	\n			if (dyn_cast<LoadInst>(e->v2))\
	\n				return 1;\
	\n			else\
	\n				return 2;\
	\n		}\
	\n		else\
	\n		{\
	\n			if (dyn_cast<LoadInst>(e->v2))\
	\n				return 3;\
	\n			else\
	\n				return 4;\
	\n		}\
	\n	}\
	\n	/*----------------------------------------------------------------------*/\
	\n	\
	\n	class AvailableExpressions : public FunctionPass \
	\n	{\
	\n		public:\
	\n        	static char ID;\
	\n\
	\n			AvailableExpressions() : FunctionPass(ID)\
	\n			{\
	\n				Direction direction = Direction::FORWARD;\
	\n				MeetOp meet_op = MeetOp::INTERSECTION;\
	\n				pass = AEAnalysis(direction, meet_op);\
	\n			}\
	\n			\
	\n		//private:\
	\n		\
	\n		class AEAnalysis : public DataFlow \
	\n		{\
	\n			public:\
	\n\
	\n				AEAnalysis() : DataFlow(Direction::INVALID_DIRECTION, MeetOp::INVALID_MEETOP) {	}\
	\n				AEAnalysis(Direction direction, MeetOp meet_op) : DataFlow(direction, meet_op) { }\
	\n\
	\n			protected:\
	\n\
	\n				TransferOutput transferFn(BitVector input, std::vector<void*> domain, std::map<void*, int> domainToIndex, BasicBlock* block)\
	\n				{\
	\n					TransferOutput transferOutput;\
	\n\
	\n					// expressions generated and killed in basic block\
	\n					int domainSize = domainToIndex.size();\
	\n					BitVector GenSet(domainSize);\
	\n					BitVector KillSet(domainSize);\
	\n\
	\n					for (BasicBlock::iterator i = block->begin(), e = block->end(); i!=e; ++i)\
	\n					{\
	\n						Instruction * I = &*i;\
	\n						if (BinaryOperator * BI = dyn_cast<BinaryOperator>(I))	// available expressions for BinaryOperators\
	\n						{\
	\n							Expression *expr = new Expression(BI);	// new Expression to capture the RHS of the BinaryOperator\
	\n							Expression *match = NULL;\
	\n							bool found = false;\
	\n\
	\n							for(void* element : domain)\
	\n							{\
	\n								if((*expr) == *((Expression *) element))\
	\n								{\
	\n									found = true;\
	\n									match = (Expression *) element;\
	\n									break;\
	\n								}\
	\n							}\
	\n\
	\n							if (found)	// gen\
	\n							{\
	\n								int valInd = domainToIndex[(void*)match];\
	\n								GenSet.set(valInd);	// instruction evaluates expression in RHS here\
	\n							}						// will be killed if one of its operands is redefined subsequently in the BB\
	\n						}\
	\n\
	\n						// killed expressions - kill all expressions in which the LHS is an operand.\
	\n						StringRef insn  =  I->getName();\
	\n						if(!insn.empty())\
	\n						{\
	\n							for(auto domain_itr = domain.begin() ; domain_itr != domain.end() ; domain_itr++)\
	\n							{\
	\n								Expression* expr = (Expression*) (*domain_itr);\
	\n\
	\n								StringRef op1 = expr->v1->getName();\
	\n								StringRef op2 = expr->v2->getName();\
	\n\
	\n								if(op1.equals(insn) || op2.equals(insn)) // if either operand is on LHS, kill\
	\n								{\
	\n									std::map<void*, int>::iterator iter = domainToIndex.find((void*) expr);\
	\n\
	\n									if (iter != domainToIndex.end())\
	\n									{\
	\n										KillSet.set((*iter).second);\
	\n										GenSet.reset((*iter).second);\
	\n									}\
	\n								}\
	\n							}\
	\n						}\
	\n					}\
	\n\
	\n					// transfer function = GenSet U (input - KillSet)\
	\n					transferOutput.element = KillSet;\
	\n					transferOutput.element.flip();	// complement of KillSet\
	\n					transferOutput.element &= input;	// input - KillSet \
	\n					transferOutput.element |= GenSet;	// union GenSet\
	\n\
	\n					return transferOutput;\
	\n				}\
	\n\
	\n		};\
	\n		\
	\n		AEAnalysis pass;	// pass\
	\n		\
	\n		virtual bool runOnFunction(Function &F) \
	\n        {\
	\n			// print -> \
	\n			std::string function_name = F.getName(); errs() << \"FUNCTION :: \" << function_name  << \"\\n\";\
	\n			\
	\n			DataFlowResult output;\
	\n			std::vector<void*> domain;\
	\n			\
	\n			AvAtI.clear();\
	\n			assignIndicesToInstrs(&F);\
	\n			\
	\n			for (Function::iterator FI = F.begin(), FE = F.end(); FI != FE; ++FI) // fill in domain\
	\n			{\
	\n				BasicBlock* block = &*FI;\
	\n				\
	\n				for (BasicBlock::iterator i = block->begin(), e = block->end(); i!=e; ++i) \
	\n				{\
	\n					Instruction * I = &*i;\
	\n					if (BinaryOperator * BI = dyn_cast<BinaryOperator>(I))	// av ex for binary operators \
	\n					{\
	\n						Expression *expr = new Expression(BI);	// new Expression to capture the RHS of the BinaryOperator\
	\n						bool found = false;\
	\n\
	\n						for(void* element : domain)\
	\n						{\
	\n						    if((*expr) == *((Expression *) element))\
	\n						    {\
	\n						        found = true;\
	\n						        break;\
	\n						    }\
	\n						}\
	\n\
	\n						if(found == false)\
	\n						{\
	\n						    domain.push_back(expr);\
	\n						    where.insert(std::pair<string,Instruction*>(expr->toString(),I));\
	\n						}\
	\n						else\
	\n						    delete expr;\
	\n					}\
	\n				}\
	\n			}\
	\n\
	\n			errs() << \"------------------------------------------\\n\\n\";\
	\n			errs() << \"DOMAIN :: \" << domain.size() << \"\\n\";\
	\n			for(void* element : domain)\
	\n			{\
	\n				errs() << \"Element : \" << ((Expression*) element)->toString() << \"\\n\";\
	\n			}\
	\n			errs() << \"------------------------------------------\\n\\n\";\
	\n\
	\n			BitVector boundaryCond(domain.size(), false);	// For AEA, the boundary condition is phi\
	\n			BitVector initCond(domain.size(), true);	// For AEA, init condition is U\
	\n\
	\n			output = pass.run(F, domain, boundaryCond, initCond);	// apply pass, printResult(output);\
	\n\
	\n			// map domain values to index in bitvector\
	\n			std::map<void*, int> domainToIndex;\
	\n			for (int i = 0; i < domain.size(); i++)\
	\n				domainToIndex[(void*)domain[i]] = i;\
	\n\
	\n			// We use the results to compute the available expressions\
	\n			std::stringstream ss;\
	\n\
	\n			for (Function::iterator BI = F.begin(), BE = F.end(); BI != BE; ++BI) \
	\n			{\
	\n				BasicBlock* block = &*BI;\
	\n\
	\n				BitVector availableExprs = output.result[block].in;	// AE at IN\
	\n\
	\n				// Generate Print Information\
	\n				std::vector<std::string> out;\
	\n\
	\n				// Print live variables at the entry of the block\
	\n				ss.clear();\
	\n				ss << std::setw(WIDTH) << std::right;\
	\n				ss.str(std::string());\
	\n				ss << printSet(domain, availableExprs, 1) << \" :: BB Entry\" ;\
	\n				out.push_back(ss.str());\
	\n\
	\n				// Iterate forward through the block, updating availability of expressions\
	\n				for (BasicBlock::iterator insn = block->begin(), IE = block->end(); insn != IE; ++insn) \
	\n				{\
	\n				\
	\n					expati e1; // hold available expressions at an instruction\
	\n				\
	\n					out.push_back(std::string(WIDTH, ' ') + printValue(&*insn));	// Add the instruction itself\
	\n\
	\n					Instruction * I = &*insn;	// Gen expressions\
	\n\
	\n					if (BinaryOperator * BI = dyn_cast<BinaryOperator>(I)) \
	\n					{\
	\n						Expression *expr = new Expression(BI);\
	\n						Expression *match = NULL;\
	\n						bool found = false;\
	\n\
	\n						for(void* element : domain)\
	\n						{\
	\n						    if((*expr) == *((Expression *) element))\
	\n						    {\
	\n						        found = true;\
	\n						        match = (Expression *) element;\
	\n						        break;\
	\n						    }\
	\n						}\
	\n\
	\n						if(found)		// generated expression\
	\n						{\
	\n						    int valInd = domainToIndex[(void*)match];\
	\n						    availableExprs.set(valInd); // instruction evaluates expression in RHS, will be killed if one of its operands is\
	\n						}								// redefined subsequently in the BB.\
	\n					}\
	\n\
	\n					// kill all expressions in which the LHS is an operand, will be generated if subsequently recomputed in BB.\
	\n					StringRef insn_str  =  I->getName();\
	\n\
	\n					if(!insn_str.empty())\
	\n					{\
	\n						errs() << \"Insn : \" << insn_str  << \"\\n\";\
	\n\
	\n						for(auto domain_itr = domain.begin() ; domain_itr != domain.end() ; domain_itr++)\
	\n						{\
	\n						    Expression* expr = (Expression*) (*domain_itr);\
	\n\
	\n						    StringRef op1 = expr->v1->getName();\
	\n						    StringRef op2 = expr->v2->getName();\
	\n\
	\n						    if(op1.equals(insn_str) || op2.equals(insn_str))\
	\n						    {\
	\n						        errs() << \"Expr : \" << expr->toString()  << \" \";\
	\n\
	\n						        // kill if either operand 1 or 2 match the variable assigned\
	\n						        std::map<void*, int>::iterator iter = domainToIndex.find((void*) expr);\
	\n\
	\n						        if (iter != domainToIndex.end())\
	\n						        {\
	\n						            errs() << \"Index : \" << (*iter).second  << \"\\n\";\
	\n						            availableExprs.reset((*iter).second);\
	\n						        }\
	\n						    }\
	\n						}\
	\n					}\
	\n\
	\n					//printBitVector(availableExprs);\
	\n\
	\n					// Print live variables at this program point\
	\n					PHINode* phiInst = dyn_cast<PHINode>(&*insn);\
	\n					// Skip printing for phi instructions\
	\n					if(phiInst == NULL)\
	\n					{\
	\n						ss.clear();\
	\n						ss.str(std::string());\
	\n						ss << std::setw(WIDTH) << std::right;\
	\n						ss << printSet(domain, availableExprs, 1);\
	\n						\
	\n							/*------store available expressions for later use-------*/\
	\n							for (int i = 0; i < domain.size(); i++) \
	\n								if (availableExprs[i]) \
	\n									e1.push_back((Expression*)domain[i]);	// Expression* - set of expr available\
	\n					\
	\n							AvAtI.insert(std::pair<int,expati>(InstrToIndex[I],e1));	// at the instruction point\
	\n							/*------------------------------------------------------*/\
	\n							\
	\n						out.push_back(ss.str());\
	\n					}\
	\n				}\
	\n\
	\n				// Print strings\
	\n				for (std::vector<std::string>::iterator it = out.begin(); it != out.end(); ++it)\
	\n				   errs() << *it << \"\\n\";\
	\n\
	\n			}\
	\n			\
	\n			AvExF.insert(std::pair<string,ItoE>(function_name,AvAtI));	// for every instr in a function set of expr available\
	\n			\
	\n			/*---------------------------------Store info for PHI Nodes------------------------------------------------*/\
	\n			for (Function::iterator BI = F.begin(), BE = F.end(); BI != BE; ++BI) \
	\n			{\
	\n				BasicBlock* block = &*BI;\
	\n				\
	\n				std::vector<expati> terms;\
	\n				int noOfPred = 0;\
	\n				for (auto it = pred_begin(block), et = pred_end(block); it != et; ++it)\
	\n						noOfPred++;\
	\n				if (noOfPred>1)\
	\n				{\
	\n					for (auto it = pred_begin(block), et = pred_end(block); it != et; ++it)\
	\n					{\
	\n						Instruction *terminst = (*it)->getTerminator();\
	\n						\
	\n						if (terminst!=NULL)\
	\n							terms.push_back(AvExF[F.getName().str()][InstrToIndex[terminst]]);\
	\n						\
	\n						++noOfPred;					\
	\n					}\
	\n					for (void *ele : domain)\
	\n					{\
	\n						Expression *elem = (Expression *)ele;\
	\n						int no = checkExpr(elem);\
	\n						if (where[elem->toString()]->getParent()==block)\
	\n						{	\
	\n							expati adde;\
	\n							adde.clear();\
	\n							for (auto q1:terms)\
	\n							{	\
	\n								\
	\n								for (auto q2:q1)\
	\n								{	\
	\n									if (checkExpr(q2)==no && compareExpr(elem,q2,no))\
	\n									{\
	\n										adde.push_back(q2);\
	\n									}\
	\n								}\
	\n							}\
	\n							possibleExps.insert(std::pair<string,expati>(elem->toString(),adde));\
	\n						}\
	\n					}\
	\n				}\
	\n			/*------------------------------------------------------------------------------------------------------*/\
	\n			}\
	\n			// No modification\
	\n			return false;\
	\n        }\
	\n\
	\n        virtual bool runOnModule(Module& M) \
	\n        {\
	\n            bool modified = false;\
	\n\
	\n            // Run analysis on each function\
	\n            for (Module::iterator MI = M.begin(), ME = M.end(); MI != ME; ++MI) \
	\n            {\
	\n                bool function_modified = false;\
	\n\
	\n                function_modified = runOnFunction(*MI);\
	\n\
	\n                modified |= function_modified;\
	\n            }\
	\n\
	\n            return modified;\
	\n        }\
	\n        \
	\n        AEAnalysis getPass()	\
	\n        {\
	\n        	return pass;			\
	\n        }\
	\n\
	\n    };\
	\n}\
	\nusing namespace av;\
	\nchar AvailableExpressions::ID = 0;\
	\nRegisterPass<AvailableExpressions> X(\"available\", \"Available Expressions Analysis\");");
	fclose(file3);
}

void writeReachingDefsFiles()
{
	strcpy(temppath,path);
	file4 = fopen(strcat(temppath,"OptimiserReachingDefs.cpp"),"a");
	fprintf(file4, "#include \"OptimiserDataflow.cpp\"\
	\n\
namespace llvm	// utilities for finding names of definitions\
\n{	\
\n	Value* getDefinitionVar(Value* v) // Definitions are function args or store instructions or instructions with names\
\n	{\
\n		if (isa<Argument>(v))\
\n			return v;\
\n		else if (isa<StoreInst>(v)) \
\n			return ((StoreInst*)v)->getPointerOperand();\
\n		else if (isa<Instruction>(v))\
\n		{\
\n			std::string str = printValue(v);\
\n			const int VAR_NAME_START_IDX = 2;\
\n			if (str.length() > VAR_NAME_START_IDX && str.substr(0,VAR_NAME_START_IDX+1) == \"  %%\")\
\n				return v;\
\n		}\
\n		return 0;\
\n	}\
\n	const int VAR_NAME_START_IDX = 2;\
\n	std::string valueToDefinitionStr(Value* v) \
\n	{\
\n		Value* def = getDefinitionVar(v);\
\n		if (def == 0)\
\n			return \"\";\
\n\
\n		std::string str = printValue(v);\
\n		if (isa<Argument>(v)) \
\n			return str;\
\n		\
\n		else \
\n		{\
\n		  str = str.substr(VAR_NAME_START_IDX);\
\n		  return str;\
\n		}\
\n\
\n		return \"\";\
\n	}\
\n	\
\n	std::string valueToDefinitionVarStr(Value* v)	// return just the defined variable if definition\
\n	{\
\n  		Value* def = getDefinitionVar(v);\
\n  		\
\n  		if (def == 0)\
\n    		return \"\";\
\n		if (isa<Argument>(def) || isa<StoreInst>(def)) \
\n			return \"%%\" + def->getName().str();\
\n		else \
\n		{\
\n			std::string str = printValue(def);\
\n			int varNameEndIdx = str.find(' ',VAR_NAME_START_IDX);\
\n			str = str.substr(VAR_NAME_START_IDX,varNameEndIdx-VAR_NAME_START_IDX);\
\n			return str;\
\n		}\
\n	}	\
\n}\
\n\
\nusing namespace llvm;\
\n\
\nnamespace reachdef\
\n{\
\n\
\n	/*--------Utilities for Reaching Definitions Information--------*/\
\n	\
\n	typedef std::vector<string> varati;		// set of definitions\
\n	std::map<int, varati> VarAtI;			// at every instruction\
\n	typedef std::map<int, varati> ItoS;		\
\n	std::map<string,ItoS> ReachDefs;			// for every function\
\n	\
\n	/*-------------------------------------------------------------*/\
\n\
\n	class ReachingDefinitions : public FunctionPass \
\n	{\
\n		public:\
\n		static char ID;\
\n\
\n		ReachingDefinitions() : FunctionPass(ID) \
\n		{\
\n			Direction direction = Direction::FORWARD;\
\n			MeetOp meet_op = MeetOp::UNION;\
\n			pass = ReachingDefinitionsAnalysis(direction, meet_op);\
\n		}\
\n\
\n		virtual void getAnalysisUsage(AnalysisUsage& AU) const \
\n		{\
\n			AU.setPreservesAll();\
\n		}\
\n\
\n		private:\
\n\
\n		class ReachingDefinitionsAnalysis : public DataFlow	// reaching definitions analysis \
\n		{\
\n			public:\
\n\
\n			ReachingDefinitionsAnalysis() : DataFlow(Direction::INVALID_DIRECTION, MeetOp::INVALID_MEETOP)	{ }\
\n			ReachingDefinitionsAnalysis(Direction direction, MeetOp meet_op) : DataFlow(direction, meet_op) { }\
\n\
\n			protected:\
\n			\
\n			TransferOutput transferFn(BitVector input,  std::vector<void*> domain, std::map<void*, int> domainToIndex, BasicBlock* block)\
\n			{\
\n\
\n				TransferOutput transferOutput;\
\n\
\n					// expressions generated and killed in basic block\
\n				int domainSize = domainToIndex.size();\
\n				BitVector GenSet(domainSize);\
\n				BitVector KillSet(domainSize);\
\n\
\n				for (BasicBlock::iterator i = block->begin(), e = block->end(); i!=e; ++i)\
\n				{\
\n					Instruction * I = &*i;\
\n					std::map<void*, int>::iterator defIt = domainToIndex.find((void*)(I));\
\n					if (defIt != domainToIndex.end())	 //kill prior definitions , can be in this block's gen set\
\n					{\
\n						for (std::map<void*,int>::iterator prevDefIt=domainToIndex.begin(); prevDefIt!= domainToIndex.end(); ++prevDefIt)\
\n						{\
\n							std::string prevDefStr = valueToDefinitionVarStr((Value*)prevDefIt->first);\
\n            				std::string currDefStr = valueToDefinitionVarStr((Value*)defIt->first);\
\n            				if (prevDefStr == currDefStr)\
\n            				{\
\n								KillSet.set(prevDefIt->second);\
\n								GenSet.reset(prevDefIt->second);\
\n            				}\
\n						}\
\n						GenSet.set((*defIt).second);\
\n					}\
\n				}\
\n\
\n				// transfer function = GenSet U (input - KillSet)\
\n				transferOutput.element = KillSet;\
\n				transferOutput.element.flip();	// complement of KillSet\
\n				transferOutput.element &= input;	// input - KillSet \
\n				transferOutput.element |= GenSet;	// union GenSet\
\n\
\n				return transferOutput;\
\n			}\
\n		};\
\n\
\n		ReachingDefinitionsAnalysis pass;	// pass\
\n\
\n		virtual bool runOnFunction(Function &F)\
\n		{\
\n			std::string function_name = F.getName();	// print Information\
\n			//errs() << \"FUNCTION :: \" << function_name  << \"\\n\";\
\n			DataFlowResult output;\
\n\
\n			std::vector<void*> domain;	// setup the pass\
\n			\
\n			VarAtI.clear();\
\n			assignIndicesToInstrs(&F);\
\n			\
\n			for (Function::arg_iterator arg = F.arg_begin(); arg != F.arg_end(); ++arg)	// fill in domain\
\n				domain.push_back((void*)&*arg);\
\n			for(inst_iterator II = inst_begin(F), IE = inst_end(F); II!=IE; ++II)	\
\n			{\
\n				if (!valueToDefinitionStr(&*II).empty())\
\n					domain.push_back((void*)&*II);\
\n			}\
\n\
\n			//errs() << \"------------------------------------------\\n\\n\";\
\n			//errs() << \"DOMAIN :: \" << domain.size() << \"\\n\";\
\n			for(void* element : domain)\
\n			{\
\n				//errs() << \"Element : \" << *((Value*) element) << \"\\n\"; // or use getShortValueName((Value*) element)\
\n			}\
\n			//errs() << \"------------------------------------------\\n\\n\";\
\n\
\n			BitVector boundaryCond(domain.size(), false);	// empty sets for reaching definitions\
\n			for (int i = 0; i < domain.size(); i++)\
\n      			if (isa<Argument>((Value*)domain[i]))\
\n					boundaryCond.set(i);\
\n			BitVector initCond(domain.size(), false);\
\n\
\n			output = pass.run(F, domain, boundaryCond, initCond);	// apply pass\
\n\
\n			// PRINTING RESULTS - printResult(output);\
\n			std::stringstream ss;	// use results to compute the final liveness - handle phi nodes\
\n\
\n			for (Function::iterator BI = F.begin(), BE = F.end(); BI != BE; ++BI)\
\n			{\
\n				BasicBlock* block = &*BI;\
\n\
\n				BitVector reachDefs = output.result[block].in;	// value at IN, forward analysis\
\n\
\n				std::vector<std::string> rOut;	// print information in order\
\n\
\n				rOut.push_back(\"//===--------------------------------------------------------------------------------------------------------------------------===//\");\
\n				ss.clear();	// print definitions reaching the beginning of the block\
\n				ss.str(std::string());\
\n				ss << std::setw(WIDTH) << std::right;\
\n				ss << printSet(domain, reachDefs, 0);\
\n				rOut.push_back(ss.str());\
\n\
\n				for (BasicBlock::iterator insn = block->begin(), IE = block->end(); insn != IE; ++insn)\
\n				{	// iterate forward through the block, update liveness\
\n				\
\n					std::vector<string> s1; // hold live variables at an instruction\
\n\
\n					rOut.push_back(std::string(WIDTH, ' ') + printValue(&*insn));	// add the instruction itself\
\n\
\n				    std::map<void*, int>::iterator it = output.domainToIndex.find((void*)(&*insn));\
\n				    std::string currDefStr = valueToDefinitionVarStr(&*insn);\
\n				    for (it = output.domainToIndex.begin(); it != output.domainToIndex.end(); ++it)	// kill existing defs for this variable\
\n				    {\
\n      					std::string prevDefStr = valueToDefinitionVarStr((Value*)it->first);\
\n      					if (prevDefStr == currDefStr)\
\n        					reachDefs.reset(it->second);\
\n    				}\
\n				    if (it != output.domainToIndex.end())	// add this definition to the reaching set\
\n				        reachDefs.set(it->second);\
\n					\
\n				    // print reaching definitions\
\n				    ss.clear();\
\n				    ss.str(std::string());\
\n				    ss << std::setw(WIDTH) << std::right;\
\n				    ss << printSet(domain, reachDefs, 0);\
\n				    \
\n				    	/*------store reaching definitions information for later use-------*/\
\n						for (int i = 0; i < domain.size(); i++) \
\n							if (reachDefs[i])\
\n							{ \
\n								string insertstr = getShortValueName((Value*)domain[i]);\
\n								if (insertstr.find(\",\")<strlen(insertstr.c_str()))\
\n									insertstr = insertstr.substr(0,insertstr.find(\",\"));\
\n					\
\n								if (find(s1.begin(),s1.end(),insertstr)==s1.end())\
\n									s1.push_back(getShortValueName((Value*)domain[i]));	// set of variables available\
\n							}\
\n				\
\n						VarAtI.insert(std::pair<int,varati>(InstrToIndex[&*insn],s1));	// at the instruction point\
\n						/*------------------------------------------------------*/\
\n				    \
\n				    rOut.push_back(ss.str());\
\n					\
\n				}\
\n\
\n				rOut.push_back(\"//===--------------------------------------------------------------------------------------------------------------------------===//\");\
\n			\
\n			}\
\n\
\n			ReachDefs.insert(std::pair<string,ItoS>(function_name,VarAtI));	// for every instr in a function set of vars available\
\n			return false;	// no change in function\
\n		}\
\n\
\n		virtual bool runOnModule(Module& M)\
\n		{\
\n			bool modified = false;\
\n\
\n			for (Module::iterator MI = M.begin(), ME = M.end(); MI != ME; ++MI)	// run analysis on each function\
\n			{\
\n				bool function_modified = false;\
\n				function_modified = runOnFunction(*MI);\
\n				modified |= function_modified;\
\n			}\
\n\
\n			return modified;\
\n		}\
\n\
\n	};\
\n}\
\n\
\nusing namespace reachdef;\
\nchar ReachingDefinitions::ID = 0;\
\nRegisterPass<ReachingDefinitions> X(\"reachdefs\", \"Reaching Definitions Analysis\");");
	fclose(file4);
}

void checkLICMIter(nodeType *node)
{
	if (node == NULL)
		return;

	if (node->type==typeConst)
	{
		LICM_helper1++;
		if (strcmp(node->con.alpha,"iterate")==0)	// add this block for all functions inside iterate block
		{
			LICM_fnhelper = LICM_helper1;
			LICM_loophelper = LICM_helper1;
			return;
		}
		else if (strcmp(node->con.alpha,"loop")==0)
		{
			if (LICM_helper1 == LICM_loophelper+1)
			{ LICM_loopIterCheck = 1; LICM_fnIterCheck = 0; LICM_findLoopInvStmts = 0; LICM_findMotionCands = 0; LICM_applyCodeMotion = 0;}
		}
		else if (strcmp(node->con.alpha,"function")==0)
		{
			if (LICM_helper1 == LICM_fnhelper+1)
			{	LICM_fnIterCheck = 1; LICM_loopIterCheck = 0;	}
		}
		else if (strcmp(node->con.alpha,"findReachingDefinitionsInfo")==0 && LICM_fnIterCheck==1)
		{
			LICM_findReachDefsIterCheck = 1;
			return;
		}
		else if (strcmp(node->con.alpha,"findLoopInvariantStatements")==0 && LICM_loopIterCheck==1)
		{
			LICM_findLoopInvStmts = 1;
			return;
		}
		else if (node->type==typeConst && strcmp(node->con.alpha,"findMotionCandidates")==0 && LICM_loopIterCheck==1)
		{
			LICM_findMotionCands = 1;
			return;
		}
		else if (node->type==typeConst && strcmp(node->con.alpha,"applyCodeMotion")==0 && LICM_loopIterCheck==1)
		{
			LICM_applyCodeMotion = 1;
			return;
		}
	}
	else
		for (int i = 0; i < node->opr.nops; i++)
			checkLICMIter(node->opr.op[i]);
}

void checkGCSEIter(nodeType *node)
{
	if (node == NULL)
		return;

	if (node->type==typeConst)
	{
		if (strcmp(node->con.alpha,"findAvailableExpressionsInfo")==0)
		{
			GCSE_findAvExIterCheck = 1;
			return;
		}
		if (node->type==typeConst && strcmp(node->con.alpha,"gcse")==0)
		{
			GCSE_gcseIterCheck = 1;
			return;
		}
	}
	else
		for (int i = 0; i < node->opr.nops; i++)
			checkGCSEIter(node->opr.op[i]);
}

void checkGVNIter(nodeType *node)
{
	if (node == NULL)
		return;

	if (node->type==typeConst)
	{
		if (strcmp(node->con.alpha,"valuenumber")==0)
		{
			GVN_instrExists = 1;
			return;
		}
	}
	else
		for (int i = 0; i < node->opr.nops; i++)
			checkGVNIter(node->opr.op[i]);
}

void checkConstantFoldingIter(nodeType *node)
{
	if (node == NULL)
		return;

	if (node->type==typeConst)
	{
		CF_IterCheckHelper1++;
		if (strcmp(node->con.alpha,"iterate")==0)	// add this block for all functions inside iterate block
		{
			CF_IterCheckHelper2 = CF_IterCheckHelper1;
			return;
		}
		else if (strcmp(node->con.alpha,"instruction")==0)
		{
			if (CF_IterCheckHelper1 == CF_IterCheckHelper2+1)
				CF_IterCheckHelper3 = 1;
		}
		else if (strcmp(node->con.alpha,"foldpropagate")==0)
		{
			if (CF_IterCheckHelper3 == 1)
				CF_IterCheck = 1;
		}
	}
	else
		for (int i = 0; i < node->opr.nops; i++)
			checkConstantFoldingIter(node->opr.op[i]);
}

void checkFunctionInliningIter(nodeType *node)
{
	if (node == NULL)
		return;

	if (node->type==typeConst)
	{
		if (strcmp(node->con.alpha,"makeInline")==0)	// add this block for all functions inside iterate block
		{
			FI_makeInlineIterCheck = 1;
			return;
		}
	}
	else
		for (int i = 0; i < node->opr.nops; i++)
			checkFunctionInliningIter(node->opr.op[i]);
}

void countMakeInlineParams(nodeType *node)
{
	if (node == NULL)
		return;

	if (node->type==typeConst)
	{
		FI_paramsCount++;
		return;
	}
	else
		for (int i = 0; i < node->opr.nops; i++)
			countMakeInlineParams(node->opr.op[i]);
}

void processRemoveInlineFn()
{
	fprintf(file, "\n\tclass RemoveInlinedFunctions : public ModulePass\n\t{\n\t\tpublic:\n\t\tstatic char ID;\n\n\t\tRemoveInlinedFunctions() : ModulePass(ID)\n\n\t\t{						  }\n\n\t\tvirtual bool runOnModule(Module &M)\n\t\t{");
	
	if (FI_removeFn)
		fprintf(file, "\n\t\t\tfor (Module::iterator MI = M.begin(), ME = M.end(); MI != ME; ++MI)\n\t\t\t{\n\t\t\t\tFunction* F = &*MI;\n\t\t\t\tif (!(F->use_empty() && find(removeFns.begin(), removeFns.end(), pair<Module*, Function *>(&M,F)) != removeFns.end()))\n\t\t\t\t\tremoveFns.erase(remove(removeFns.begin(), removeFns.end(), pair<Module*, Function *>(&M,F)),removeFns.end());\n\t\t\t}\n\t\t\tfor (auto remF = removeFns.begin(); remF!=removeFns.end(); ++remF)\n\t\t\t\tif (&(*remF->first)==&M)	\n\t\t\t\t\t(*remF->second).eraseFromParent();	\n");
	
	fprintf(file, "\n\n\t\t}\n\t};");
}

void processMakeInlineParams(LinkedListNode *node)
{
	LinkedListNode *param = node->next;
	
	if (strcmp("function",param->llnode->con.alpha)!=0)
		printerror("Only functions can be made inline.\n");
		
	if (FI_paramsCount==1)
	{
		fprintf(file, ")\n");
		return;
	}
	
	for (int i=2; i<FI_paramsCount; i+=3)
	{
		param = param->next;	
		if (strcmp(param->llnode->con.alpha,"noLines")==0)
			fprintf(file," && countNoOfInstrInFn(*F)");
		else if (strcmp(param->llnode->con.alpha,"noUses")==0)
			fprintf(file," && F->getNumUses()");
			
		param = param->next;
		fprintf(file, "%s", param->llnode->con.alpha);
		
		param = param->next;
		fprintf(file, "%d", param->llnode->con.num);
	}
	fprintf(file, " )\n");
	bracesCount++;
}

void checkIfInstrExists()
{
	if (CF_instrExists)
		return;
	else
		printerror("Instruction does not exist in scope.\n");
}

void countGCSEParams(nodeType *node)
{
	if (node == NULL)
		return;

	if (node->type==typeConst)
	{
		GCSE_paramsCount++;
		return;
	}
	else
		for (int i = 0; i < node->opr.nops; i++)
			countGCSEParams(node->opr.op[i]);
}

void processGCSEParams(LinkedListNode *node)
{
	LinkedListNode *param = node->next;
	
	if (strcmp("function",param->llnode->con.alpha)!=0)
		printerror("Global CSE can be performed only on functions.\n");
		
	if (GCSE_paramsCount==1)
		printerror("Available expression info is required for global CSE.");

	int i = 1;

	while (i < GCSE_paramsCount)
	{
		param = param->next;

		if (strcmp(param->llnode->con.alpha,"availableexprs")==0)
		{
			if (GCSE_avexComputed==0)
				printerror("Available expression info is required for global CSE.");
			
			i+=1;
		}
		else if (strcmp(param->llnode->con.alpha,"noExprs")==0)
		{
			GCSE_threshSpecified = 1;
			fprintf(file, "\t\t\tif (countNoOfExprInFn(F)");
			param = param->next;
			fprintf(file, "%s", param->llnode->con.alpha);
		
			param = param->next;
			fprintf(file, "%d", param->llnode->con.num);
			
			bracesCount++;
			
			i+=3;
		}
	}
	
	if (GCSE_threshSpecified == 1)
		fprintf(file, " )\n\t\t\t{\n");
}

void countLCSEParams(nodeType *node)
{
	if (node == NULL)
		return;

	if (node->type==typeConst)
	{
		LCSE_paramsCount++;
		return;
	}
	else
		for (int i = 0; i < node->opr.nops; i++)
			countLCSEParams(node->opr.op[i]);
}

void processLCSEParams(LinkedListNode *node)
{
	LinkedListNode *param = node->next;
	
	if (strcmp("basicblock",param->llnode->con.alpha)!=0)
		printerror("Local CSE can be performed only on basic blocks.\n");
		
	if (LCSE_paramsCount==1)
		printerror("DAG is required for local CSE.");
		
	int i = 1;

	while (i < LCSE_paramsCount)
	{
		param = param->next;

		if (strcmp(param->llnode->con.alpha,"dag")==0)
		{
			if (LCSE_dagCreated==0)
				printerror("DAG is required for local CSE.");
				
			fprintf(file,"\n\t\t\tDAGInfoList l;\n\t\t\tBasicBlock* block = &B;\n\t\t\tfor (BasicBlock::iterator i = B.begin(), e = B.end(); i!=e; ++i)\n\t\t\t{\n\t\t\t\tInstruction * I = &*i;\n\t\t\t\tif (BinaryOperator *BI = dyn_cast<BinaryOperator>(i))\n");
			i+=1;
		}
		else if (strcmp(param->llnode->con.alpha,"noExprs")==0)
		{
			LCSE_threshSpecified = 1;
			fprintf(file, "\t\tif (countNoOfExprs(*block)");
			param = param->next;
			fprintf(file, "%s", param->llnode->con.alpha);
		
			param = param->next;
			fprintf(file, "%d", param->llnode->con.num);
			
			i+=3;
		}
	}
	
	if (LCSE_threshSpecified == 1)
		fprintf(file, " )");
}

void writeGVNHelperCode()
{
	fprintf(file,"		\n		struct ValueDFS\n		{\n			int DFSIn = 0;\n			int DFSOut = 0;\n			int LocalNum = 0;\n\n			Value *Val = nullptr;	// only one of these will be set\n			Use *U = nullptr;\n\n			bool operator<(const ValueDFS &Other) const\n			{\n				return std::tie(DFSIn, DFSOut, LocalNum, Val, U) < std::tie(Other.DFSIn, Other.DFSOut, Other.LocalNum, Other.Val, Other.U);\n			}\n		};\n\n		class ValueDFSStack\n		{\n			public:\n			Value *back() const { return ValueStack.back(); }\n			std::pair<int, int> dfs_back() const { return DFSStack.back(); }\n\n			void push_back(Value *V, int DFSIn, int DFSOut)\n			{\n				ValueStack.emplace_back(V);\n				DFSStack.emplace_back(DFSIn, DFSOut);\n			}\n			bool empty() const { return DFSStack.empty(); }\n			bool isInScope(int DFSIn, int DFSOut) const\n			{\n				if (empty())\n					return false;\n				return DFSIn >= DFSStack.back().first && DFSOut <= DFSStack.back().second;\n			}\n\n			void popUntilDFSScope(int DFSIn, int DFSOut)\n			{\n				assert(ValueStack.size() == DFSStack.size() && \"Mismatch between ValueStack and DFSStack\");\n				while (!DFSStack.empty() && !(DFSIn >= DFSStack.back().first && DFSOut <= DFSStack.back().second))\n				{\n					DFSStack.pop_back();\n					ValueStack.pop_back();\n				}\n			}\n\n			private:\n			SmallVector<Value *, 8> ValueStack;\n			SmallVector<std::pair<int, int>, 8> DFSStack;\n		};\n");
}

void processUnrollLoopParams(LinkedListNode *node)
{
	if (node == NULL)
		return;
		
	if (strcmp(node->llnode->con.alpha,"noLines")==0)
		fprintf(file,"\n\n\t\t\tif (countNoOfInstrInLoop(L)");
			
	node = node->next;
	fprintf(file, "%s", node->llnode->con.alpha);
		
	node = node->next;
	fprintf(file, "%d)", node->llnode->con.num);
}

void processIterateNode(char start[], char end[])
{
	if (strcmp(start,"module")==0)
	{
		if (strcmp(end,"function")==0 && iterate_functionLine==0)
		{
			fprintf(file, "\n\t\t\tfor (Module::iterator MI = M.begin(), ME = M.end(); MI != ME; ++MI) ");

			if (GCSE_fnRef==0 && LICM_fnRef==0)
				fprintf(file, "\n\t\t\t{\n\t\t\t\tFunction* F = &*MI;\n");
			else
				fprintf(file, "\n\t\t\t{\n\t\t\t\tFunction &F = *MI;\n");

			iterate_functionLine = 1;
			bracesCount++;
		}
		else if (strcmp(end,"basicblock")==0 && iterate_BBLine==0)
		{
			if (iterate_functionLine==0)
			{
				fprintf(file, "\n\t\t\tfor (Module::iterator MI = M.begin(), ME = M.end(); MI != ME; ++MI) \n\t\t\t{\n\t\t\t\tFunction* F = &*MI;\n\t\t\t");
				iterate_functionLine = 1;
				bracesCount++;
			}
			fprintf(file, "\n\t\t\tfor (Function::iterator FI = F->begin(), FE = F->end(); FI != FE; ++FI)\n\t\t\t{");
			fprintf(file, "\n\t\t\tBasicBlock* block = &*FI;\n");
			iterate_BBLine = 1;
			bracesCount++;
		}
		else if (strcmp(end,"instruction")==0 && iterate_instrLine==0)
		{
			if (iterate_functionLine==0)
			{
				fprintf(file, "\n\t\t\tfor (Module::iterator MI = M.begin(), ME = M.end(); MI != ME; ++MI) \n\t\t\t{\n\t\t\t\tFunction* F = &*MI;\n\t\t\t");
				iterate_functionLine = 1;
				bracesCount++;
			}
			if (iterate_BBLine==0)
			{
				fprintf(file, "\n\t\t\tfor (Function::iterator FI = F->begin(), FE = F->end(); FI != FE; ++FI)\n\t\t\t{");
				fprintf(file, "\n\t\t\tBasicBlock* block = &*FI;\n");
				iterate_BBLine = 1;
				bracesCount++;
			}
			fprintf(file, "\n\t\t\tfor (BasicBlock::iterator i = block->begin(), e = block->end(); i!=e; ++i)\n\t\t\t{\n\t\t\tInstruction * I = &*i;");
			iterate_instrLine = 1;
			CF_instrExists = 1;
			bracesCount++;
		}
		else
			printerror(strcat(end," cannot be iterated through when run on module.\n"));
	}
	else if (strcmp(start,"function")==0)
	{
		if (strcmp(end,"basicblock")==0 && iterate_BBLine==0)
		{
			fprintf(file, "\n\t\t\tfor (Function::iterator FI = F.begin(), FE = F.end(); FI != FE; ++FI) \n\t\t\t{\n\t\t\t\tBasicBlock* block = &*FI;\n\t\t\t");
			iterate_BBLine = 1;
			bracesCount++;
		}
		else if (strcmp(end,"instruction")==0 && iterate_instrLine==0)
		{
			if (iterate_BBLine==0)
			{
				fprintf(file, "\n\t\t\tfor (Function::iterator FI = F.begin(), FE = F.end(); FI != FE; ++FI)\n\t\t\t{");
				fprintf(file, "\n\t\t\tBasicBlock* block = &*FI;\n");
				iterate_BBLine = 1;
				bracesCount++;
			}
			fprintf(file, "\n\t\t\tfor (BasicBlock::iterator i = block->begin(), e = block->end(); i!=e; ++i)\n\t\t\t{\n\t\t\tInstruction * I = &*i;");
			iterate_instrLine = 1;
			CF_instrExists = 1;
			bracesCount++;
		}
		else
			printerror(strcat(end," cannot be iterated through when run on function.\n"));
	}
	else if (strcmp(start,"basicblock")==0)
	{
		if (strcmp(end,"instruction")==0 && iterate_instrLine==0)
		{
			fprintf(file, "\n\t\t\tfor (BasicBlock::iterator i = B.begin(), e = B.end(); i!=e; ++i)\n\t\t\t{\n\t\t\tInstruction * I = &*i;");
			iterate_instrLine = 1;
			CF_instrExists = 1;
			bracesCount++;
		}
		else
			printerror(strcat(end," cannot be iterated through when run on basic block.\n"));
	}
	else if (strcmp(start,"loop")==0)
	{
		if (strcmp(end,"basicblock")==0 && iterate_BBLine==0)
		{
			fprintf(file, "\n\t\t\tfor (std::vector<BasicBlock*>::iterator blockIter = loopBlocks.begin(); blockIter != loopBlocks.end(); ++blockIter) \n\t\t\t{\n\t\t\t\tBasicBlock *block = *blockIter;\n\t\t\t");
			iterate_BBLine = 1;
			bracesCount++;
		}
		else if (strcmp(end,"instruction")==0 && iterate_instrLine==0)
		{
			if (iterate_BBLine==0)
			{
				fprintf(file, "\n\t\t\tfor (std::vector<BasicBlock*>::iterator blockIter = loopBlocks.begin(); blockIter != loopBlocks.end(); ++blockIter)\n\t\t\t{");
				fprintf(file, "\n\t\t\tBasicBlock* block = *blockIter;\n");
				iterate_BBLine = 1;
				bracesCount++;
			}
			fprintf(file, "\n\t\t\tfor (BasicBlock::iterator i = block->begin(), e = block->end(); i!=e; ++i)\n\t\t\t{\n\t\t\tInstruction * I = &*i;");
			iterate_instrLine = 1;
			CF_instrExists = 1;
			bracesCount++;
		}
		else
			printerror(strcat(end," cannot be iterated through when run on loop.\n"));
	}
	else
		printerror(strcat(start," cannot be run on.\n"));
}

void processRunOnNode(char s[], char name[])
{
	fprintf(file, "%sPass\n\t{\n\t\tpublic:\n\t\tstatic char ID;\n\n", s);
	fprintf(file, "\t\t%s() : %sPass(ID)\n\t\t{\t\t\t\t}\n", name, s);
	
	if (strcmp(name,"GlobalVN")==0)
		writeGVNHelperCode();
	
	if (strcmp(s,"Loop")==0)
		fprintf(file, "\n\t\tvirtual bool runOn%s(%s *%c, LPPassManager &LPM)\n\t\t{", s, s, s[0]);
	else
		fprintf(file, "\n\t\tvirtual bool runOn%s(%s &%c)\n\t\t{", s, s, s[0]);
}

void processOptNode()
{
	LinkedListNode *optnode = qhead->next;
	LinkedListNode *runoniter = optnode->next->next;
	LinkedListNode *curr;
	
	int i=0;
	int j=0;

	if (strcmp(optnode->llnode->con.alpha,"FunctionInlining") == 0)
	{
		fprintf(file, "#include \"llvm/Pass.h\"\n#include \"llvm/IR/Function.h\"\n#include \"llvm/IR/Type.h\"\n#include \"llvm/IR/Instruction.h\"\n#include \"llvm/ADT/StringRef.h\"\n#include \"llvm/Transforms/Utils/Local.h\"\n#include \"llvm/Support/raw_ostream.h\"\n#include <cstring>\n\nusing namespace llvm;\nusing namespace std;\n\nnamespace\n{\n\tstd::vector<std::pair<Module*, Function *>> removeFns;\n\tclass FunctionInlining : public ");
		
		if (strcmp(optnode->next->llnode->con.alpha,"runOn") != 0)
			printerror("runOn not declared.\n");
		
		if (strcmp(runoniter->llnode->con.alpha,"module") == 0)
		{
			processRunOnNode("Module", "FunctionInlining");
			
			curr = runoniter->next;
			while (curr!=NULL)
			{
				if (strcmp(curr->llnode->con.alpha,"iterate")==0)
				{
					curr = curr->next;

					if (strcmp(curr->llnode->con.alpha,"function")!=0)
						printerror(strcat(curr->llnode->con.alpha," cannot be iterated through to make a function inline.\n"));

					FI_functionExists = 1; FI_iterateClose = 1; 		
					processIterateNode("module","function");
					curr = curr->next;
				}
				else if (strcmp(curr->llnode->con.alpha,"makeInline")==0)
				{
					if (!FI_makeInlineIterCheck || !FI_functionExists)
						printerror("Function does not exist in scope.\n");
						
					fprintf(file, "\t\t\t\tif (strcmp(\"main\",F->getName().str().c_str())!=0");
					processMakeInlineParams(curr);
					
					for (i=0; i <= FI_paramsCount; i++)
						curr = curr->next;
					
					fprintf(file, "\t\t\t\t{\n\t\t\t\t\tF->addFnAttr(llvm::Attribute::AlwaysInline);\n\t\t\t\t\tremoveFns.push_back(std::pair<Module*, Function *>(&M,F));\n\t\t\t\t}\n");
					
				}
				else if (strcmp(curr->llnode->con.alpha,"removeInlinedFunctions")==0)
				{
					FI_removeFn = 1;
					curr = curr->next;		
				}
			}
			
			if (FI_iterateClose)	fprintf(file, "\t\t\t}\n");
		}
		else if (strcmp(runoniter->llnode->con.alpha,"function") == 0)
		{
			char *s = "Function";
			char *name = "FunctionInlining";
			fprintf(file, "%sPass\n\t{\n\t\tpublic:\n\t\tstatic char ID;\n\n", s);
			fprintf(file, "\t\t%s() : %sPass(ID)\n\t\t{\t\t\t\t}\n", name, s);
			fprintf(file, "\n\t\tvirtual bool runOn%s(%s &%c1)\n\t\t{", s, s, s[0]);
			fprintf(file, "\n\t\t\tFunction *F = &F1;\n");
			fprintf(file, "\t\t\tModule *M = F->getParent();\n");
			
			FI_functionExists = 1;
			curr = runoniter->next;
			while (curr!=NULL)
			{
				if (strcmp(curr->llnode->con.alpha,"iterate")==0)
						printerror(strcat(curr->next->llnode->con.alpha," cannot be iterated through to make a function inline.\n"));

				else if (strcmp(curr->llnode->con.alpha,"makeInline")==0)
				{
					if (!FI_makeInlineIterCheck || !FI_functionExists)
						printerror("Function does not exist in scope.\n");
						
					fprintf(file, "\t\t\t\tif (strcmp(\"main\",F->getName().str().c_str())!=0");
					processMakeInlineParams(curr);
					
					for (i=0; i <= FI_paramsCount; i++)
						curr = curr->next;
					
					fprintf(file, "\t\t\t\t{\n\t\t\t\t\tF->addFnAttr(llvm::Attribute::AlwaysInline);\n\t\t\t\t\tremoveFns.push_back(std::pair<Module*, Function *>(M,F));\n\t\t\t\t}\n");
					
				}
				else if (strcmp(curr->llnode->con.alpha,"removeInlinedFunctions")==0)
				{
					FI_removeFn = 1;
					curr = curr->next;		
				}
			}
		}
		else
			printerror(strcat(runoniter->llnode->con.alpha," cannot be run on to make a function inline.\n"));
		fprintf(file, "\t\t}\n");
		
		fprintf(file, "\n\t\tint countNoOfInstrInFn(Function &F)\n\t\t{\n\t\t\tint count = 0;\n\t\t\tfor (Function::iterator FI = F.begin(), FE = F.end(); FI != FE; ++FI)\n\t\t\t{\n\t\t\t\tBasicBlock* block = &*FI;\n\t\t\t\tfor (BasicBlock::iterator i = block->begin(), e = block->end(); i!=e; ++i)\n\t\t\t\t\tcount++;\n\t\t\t\t}\n\t\t\t\treturn count;\n\t\t\t}	\n\t\t};");
		
		processRemoveInlineFn();
		
		fprintf(file,"\n}\n\nchar FunctionInlining::ID = 0;\nstatic RegisterPass<FunctionInlining> temp1(\"fninlining\",\"- to do function inlining\");\n\nchar RemoveInlinedFunctions::ID = 0;\nstatic RegisterPass<RemoveInlinedFunctions> temp2(\"removefn\",\"- to remove inlined unused functions\");");
	}
	else if (strcmp(optnode->llnode->con.alpha,"ConstantFolding") == 0)
	{
		bracesCount = 0;
		
		fprintf(file, "#include \"llvm/Transforms/Scalar.h\"\n#include \"llvm/Transforms/Utils/Local.h\"\n#include \"llvm/ADT/Statistic.h\"\n#include \"llvm/Analysis/ConstantFolding.h\"\n#include \"llvm/IR/Constant.h\"\n#include \"llvm/IR/Instruction.h\"\n#include \"llvm/Pass.h\"\n#include \"llvm/Analysis/TargetLibraryInfo.h\"\n#include \"llvm/Transforms/Utils/BasicBlockUtils.h\"\n#include \"llvm/Analysis/LoopInfo.h\"\n#include \"llvm/Analysis/LoopPass.h\"\n\nusing namespace llvm;\nusing namespace std;\n\nnamespace\n{\n\tclass ConstantFolding : public ");
		
		char start[15];
		
		if (strcmp(optnode->next->llnode->con.alpha,"runOn") != 0)
			printerror("runOn not declared.\n");
	
		else if (strcmp(runoniter->llnode->con.alpha,"module") == 0)
		{
			processRunOnNode("Module", "ConstantFolding");
			strcpy(start,"module");
		}
		else if (strcmp(runoniter->llnode->con.alpha,"function") == 0)
		{
			processRunOnNode("Function", "ConstantFolding");
			strcpy(start,"function");
		}
		else if (strcmp(runoniter->llnode->con.alpha,"basicblock") == 0)
		{
			processRunOnNode("BasicBlock", "ConstantFolding");
			strcpy(start,"basicblock");
		}
		else if (strcmp(runoniter->llnode->con.alpha,"loop") == 0)
		{
			processRunOnNode("Loop", "ConstantFolding");
			strcpy(start,"loop");
		}
		else
			printerror(strcat(runoniter->llnode->con.alpha," cannot be run on for constant folding.\n"));
			
		fprintf(file, "\n\t\t\tconst DataLayout &DL = F.getParent()->getDataLayout();\n\t\t\t");
		fprintf(file, "TargetLibraryInfo *TLI = &getAnalysis<TargetLibraryInfoWrapperPass>().getTLI();\n");
		
		curr = runoniter->next;
		while (curr!=NULL)
		{
			if (strcmp(curr->llnode->con.alpha,"iterate")==0)
			{
				curr = curr->next;
				processIterateNode(start, curr->llnode->con.alpha);
			}

			if (strcmp(curr->llnode->con.alpha,"foldpropagate")==0)
			{
				checkIfInstrExists();
				curr = curr->next;
				
				if (!CF_IterCheck)
					printerror("Instruction does not exist in scope.\n");
				
				if (strcmp(curr->llnode->con.alpha,"instruction")!=0)
					printerror(strcat(curr->llnode->con.alpha, " cannot be folded."));
				curr = curr->next;
				
				if (curr==NULL)
					printerror("All instructions cannot be folded.");
				else if (strcmp(curr->llnode->con.alpha, "precondition")==0)
				{
					curr = curr->next;
					if (strcmp(curr->llnode->con.alpha, "foldable")==0)
					{
						curr = curr->next;
						if (strcmp(curr->llnode->con.alpha,"instruction")!=0)
							printerror(strcat(curr->llnode->con.alpha, " cannot be folded."));
						fprintf(file, "\n\t\t\t\tif (Constant *C = ConstantFoldInstruction(I, DL, TLI))");
					}
				}
				else
					printerror("All instructions cannot be folded.");					
				fprintf(file, "\n\t\t\t\t\tI->replaceAllUsesWith(C);");
			}
			curr = curr->next;
		}
		
		for (i=0; i<bracesCount; i++)
			fprintf(file, "\n\t\t\t}");	
		
		fprintf(file, "\n\t\t}");
		fprintf(file, "\n\t\tvoid getAnalysisUsage(AnalysisUsage &AU) const override\n\t\t{");
		fprintf(file, "\n\t\t\t\tAU.setPreservesCFG();\n\t\t\tAU.addRequired<TargetLibraryInfoWrapperPass>();\n\t\t}");
		fprintf(file, "\n\t};\n}");
		fprintf(file, "\nchar ConstantFolding::ID = 0;\nstatic RegisterPass<ConstantFolding> temp(\"constfold\",\"- to do constant folding\");");
	}
	else if (strcmp(optnode->llnode->con.alpha,"DeadCodeElimination") == 0)
	{
		bracesCount = 0;
		
		fprintf(file, "#include \"llvm/Pass.h\"\n#include \"llvm/IR/Function.h\"\n#include \"llvm/IR/Type.h\"\n#include \"llvm/IR/Instruction.h\"\n#include \"llvm/ADT/StringRef.h\"\n#include \"llvm/Transforms/Utils/Local.h\"\n#include \"llvm/Support/raw_ostream.h\"\n#include \"llvm/Analysis/LoopInfo.h\"\n#include \"llvm/Analysis/LoopPass.h\"\n#include <cstring>\n\nusing namespace llvm;\nusing namespace std;\n\nnamespace\n{\n\tclass DCE : public ");
		
		char start[15];
		
		if (strcmp(optnode->next->llnode->con.alpha,"runOn") != 0)
			printerror("runOn not declared.\n");
		else if (strcmp(runoniter->llnode->con.alpha,"module") == 0)
		{
			processRunOnNode("Module", "DCE");
			strcpy(start,"module");
		}
		else if (strcmp(runoniter->llnode->con.alpha,"function") == 0)
		{
			processRunOnNode("Function", "DCE");
			strcpy(start,"function");
		}
		else if (strcmp(runoniter->llnode->con.alpha,"basicblock") == 0)
		{
			processRunOnNode("BasicBlock", "DCE");
			strcpy(start,"basicblock");
		}
		else if (strcmp(runoniter->llnode->con.alpha,"loop") == 0)
		{
			processRunOnNode("Loop", "DCE");
			strcpy(start,"loop");
			fprintf(file, "\n\t\t\tstd::vector<BasicBlock*> loopBlocks = L->getBlocks();");
		}
		else
			printerror(strcat(runoniter->llnode->con.alpha," cannot be run on for dead code elimination.\n"));
		
		fprintf(file, "\n\t\t\tstd::vector<Instruction *> deleteI;");
		
		curr = runoniter->next;
		while (curr!=NULL)
		{
			if (strcmp(curr->llnode->con.alpha,"repeat")==0)
			{
				curr = curr->next;
				DCE_noTimes = curr->llnode->con.num;
			}
			
			if (strcmp(curr->llnode->con.alpha,"deadcodeelim")==0)
			{
				for (i=0; i<DCE_noTimes; i++)
				{
					fprintf(file,"\n\t\t\tdeleteI.clear();");
					bracesCount = 0;
					iterate_functionLine = 0; iterate_BBLine = 0; iterate_instrLine = 0;
					processIterateNode(start, "instruction");
					
					fprintf(file,"\n\t\t\t\tif (I->hasNUses(0) && isInstructionTriviallyDead(I))\n\t\t\t\t{\n\t\t\t\t\tdeleteI.push_back(I);\n\t\t\t\t}");
					for (j=0; j<bracesCount; j++)
						fprintf(file, "\n\t\t\t}");	
			
					fprintf(file,"\n\t\t\tfor (auto deli = deleteI.begin(); deli!=deleteI.end(); ++deli)\n\t\t\t\t(*deli)->eraseFromParent();");
				}
			}
			curr = curr->next;
		}
		
		fprintf(file,"\n\t\t}\n\t};\n}\n\nchar DCE::ID = 0;\nstatic RegisterPass<DCE> temp(\"dcelim\",\"- to do dead code elimination\");");
	}
	else if (strcmp(optnode->llnode->con.alpha,"LocalCSE") == 0)
	{
			fprintf(file, "#include \"llvm/Pass.h\"\n#include \"llvm/IR/Function.h\"\n#include \"llvm/IR/Type.h\"\n#include \"llvm/IR/Instruction.h\"\n#include \"llvm/ADT/StringRef.h\"\n#include \"llvm/Transforms/Utils/Local.h\"\n#include \"llvm/Support/raw_ostream.h\"\n#include <cstring>\n\nusing namespace llvm;\nusing namespace std;\n\nnamespace\n{");
			
		if (strcmp(optnode->next->llnode->con.alpha,"runOn") != 0)
			printerror("runOn not declared.\n");
			
		if (strcmp(runoniter->llnode->con.alpha,"basicblock") != 0)
			printerror(strcat(runoniter->llnode->con.alpha," cannot be run on for local common subexpression elimination.\n"));
		
		curr = runoniter->next;
		while (curr!=NULL)
		{
			if (strcmp(curr->llnode->con.alpha,"dag")==0)
			{
				curr = curr->next;
				if (strcmp(curr->llnode->con.alpha,"makeDAG") == 0)
				{
					curr = curr->next;
					if (strcmp(curr->llnode->con.alpha,"basicblock") != 0)
						printerror(strcat(curr->llnode->con.alpha," cannot be run on for local common subexpression elimination.\n"));
				
					LCSE_dagCreated = 1;
					fprintf(file,"\n\tstruct InstrNode\n\t{\n\t\tInstruction *I;\n\t\tInstrNode *next;	\n\t};\n\tclass InstrList\n\t{\n\t\tInstrNode *head;\n\n\t\tpublic:\n\t\tInstrList()\n\t\t{\n\t\t	head = NULL;\n\t\t}\n\t\tInstrNode* getHead()\n\t\t{\n\t\treturn head;\n\t\t}\n\t\tvoid addNode(Instruction *ins)\n\t\t{\n\t\tInstrNode *tmp = new InstrNode;\n\t\ttmp->I = ins;\n\t\ttmp->next = NULL;\n\t\tif (head == NULL)\n\t\thead = tmp;\n\t\telse\n\t\t{\n\t\tInstrNode *curr = head;\n\t\twhile (curr->next != NULL)\n\t\t\t	curr = curr->next;\n\t\tcurr->next = tmp;\n\t\t}\n\t\t}\n\t};\n\tstruct DAGInfoNode\n\t{\n\t\tint binoperator;\n\t\tInstrList instructions;\n\t\tDAGInfoNode *next;\n\t};\n\tclass DAGInfoList\n\t{\n\t\tDAGInfoNode *first;\n\n\t\tpublic:\n\t\tDAGInfoList()\n\t\t{\n\t\tfirst = NULL;\n\t\t}\n\t\tInstruction* findInDAG(int opCode, Instruction *newi)\n\t\t{\n\t\tDAGInfoNode *curr = first;\n\t\twhile (curr!=NULL)\n\t\t{\n\t\tif (curr->binoperator==opCode)\n\t\t{\n\t\tInstrNode *searchcurr = curr->instructions.getHead();\n\t\twhile (searchcurr!=NULL)\n\t\t{\n\t\tif (searchcurr->I->getOperand(0)==newi->getOperand(0) && searchcurr->I->getOperand(1)==newi->getOperand(1))\n\t\treturn searchcurr->I;\n\t\tsearchcurr = searchcurr->next;\n\t\t}\n\t\taddNewInstr(curr,newi);\n\t\treturn NULL;\n\t\t}\n\t\tcurr = curr->next;\n\t\t}\n\t\taddNewOp(opCode,newi);\n\t\treturn NULL;\n\t\t}\n\t\tvoid addNewInstr(DAGInfoNode* opNode, Instruction *newi)\n\t\t{\n\t\topNode->instructions.addNode(newi);\n\t\t}\n\t\tvoid addNewOp(int binop, Instruction *newi)\n\t\t{\n\t\tDAGInfoNode *tmp = new DAGInfoNode;\n\t\ttmp->binoperator = binop;\n\t\ttmp->instructions.addNode(newi);\n\t\ttmp->next = NULL;\n\t\tif (first == NULL)\n\t\tfirst = tmp;\n\t\telse\n\t\t{\n\t\tDAGInfoNode *curr = first;\n\t\twhile (curr->next != NULL)\n\t\t\tcurr = curr->next;\n\t\tcurr->next = tmp;\n\t\t}\n\t\t}\n\t};");
				}
				else 
					printerror("DAG needs to be created.");
					
				fprintf(file,"\n\n\tclass LCSE : public ");
				processRunOnNode("BasicBlock", "LCSE");

			}
			else if (strcmp(curr->llnode->con.alpha,"lcse")==0)
			{
				if (LCSE_dagCreated==0)
					printerror("DAG needs to be created.");
					
				processLCSEParams(curr);
				fprintf(file, "\n\t\t\t\t{\n\t\t\t\t\tInstruction *instr = l.findInDAG(I->getOpcode(),I);\n\t\t\t\t\tif (instr!=NULL)\n\t\t\t\t\tI->replaceAllUsesWith(instr);\n\t\t\t\t}"); 
				
				for (i=0; i < LCSE_paramsCount; i++)
					curr = curr->next;
			}

			curr = curr->next;
		}
		fprintf(file, "\n\t\t\t}\n\t\t}\n\t\tint countNoOfExprs(BasicBlock &B)\n\t\t{\n\t\t\tint count = 0;\n\t\t\tfor (BasicBlock::iterator i = B.begin(), e = B.end(); i!=e; ++i)	\n\t\t\t{\n\t\t\tInstruction * I = &*i;\n\t\t\tif (BinaryOperator *BI = dyn_cast<BinaryOperator>(i))\n\t\t\tcount++;\n\t\t\t}\n\t\t\treturn count;\n\t\t}\n\t};\n}");
		
		fprintf(file, "\n\nchar LCSE::ID = 0;\nstatic RegisterPass<LCSE> temp(\"lcse\",\"- to do local common subexpression elimination\");");
	}
	else if (strcmp(optnode->llnode->con.alpha,"IndVarSimplify")==0)
	{
		fprintf(file, "#include \"llvm/Pass.h\"\n#include \"llvm/Analysis/LoopPass.h\"\n#include \"llvm/Analysis/LoopInfo.h\"\n#include \"llvm/Analysis/ScalarEvolution.h\"\n#include \"llvm/Analysis/TargetTransformInfo.h\"\n#include \"llvm/IR/Function.h\"\n#include \"llvm/IR/Type.h\"\n#include \"llvm/IR/Instruction.h\"\n#include \"llvm/Transforms/Scalar.h\"\n#include \"llvm/Transforms/Utils/Local.h\"\n#include \"llvm/Transforms/Utils/LoopUtils.h\"\n#include \"llvm/Transforms/Utils/SimplifyIndVar.h\"\n#include \"llvm/Support/raw_ostream.h\"\n#include <cstring>\n#include <map>\n#include <iterator>\n\nusing namespace llvm;\n\nusing namespace std;\n\nnamespace\n{");
		
		if (strcmp(optnode->next->llnode->con.alpha,"runOn") != 0)
			printerror("runOn not declared.\n");
			
		if (strcmp(runoniter->llnode->con.alpha,"loop") != 0)
			printerror(strcat(runoniter->llnode->con.alpha," cannot be run on for induction variable simplification.\n"));
			
		fprintf(file,"\n\n\tclass IndVarElimHelp : public ");
		processRunOnNode("Loop", "IndVarElimHelp");
		
		curr = runoniter->next;
		while (curr!=NULL)
		{
			if (strcmp(curr->llnode->con.alpha,"indvarsimplify")==0)
			{
				curr = curr->next;
				if (strcmp(curr->llnode->con.alpha,"loop")!=0)
					printerror(strcat(strcat("induction variable simplification cannot be done on ",curr->llnode->con.alpha),"."));
				
				fprintf(file, "\n\t\t\tif (skipLoop(L))\n\t\t\treturn false;\n\t\t\tScalarEvolution &SE = getAnalysis<ScalarEvolutionWrapperPass>().getSE();\n\t\t\tLoopInfo &LI = getAnalysis<LoopInfoWrapperPass>().getLoopInfo();\n\t\t\tDominatorTreeWrapperPass *DTWP = getAnalysisIfAvailable<DominatorTreeWrapperPass>();\n\t\t\tDominatorTree *DT = DTWP ? &DTWP->getDomTree() : nullptr;\n\t\t\tSmallVector<WeakVH, 16> DeadInsts;\n\t\t\tfor (LoopInfo::iterator i = LI.begin(), e=LI.end(); i != e; ++i)\n\t\t\t{\n\t\t\tLoop *L = *i;\n\t\t\tsimplifyLoopIVs(L, &SE, DT, &LI, DeadInsts);\n\t\t\t}\n\t\t\tbool Changed = false;\n\t\t\tfor (BasicBlock::iterator I = L->getHeader()->begin(); isa<PHINode>(I); ++I)\n\t\t\t{\n\t\t\tChanged |= simplifyUsersOfIV(cast<PHINode>(I), &SE, DT, &LI, DeadInsts);\n\t\t\t}\n\t\t\treturn Changed;");
			}
			
			curr = curr->next;
		}
		
		fprintf(file, "\n\t\t}\n\n\t\tvoid getAnalysisUsage(AnalysisUsage& AU) const\n\t\t{\n\t\t\tAU.setPreservesCFG();\n\t\t\tgetLoopAnalysisUsage(AU);\n\t\t}\n\t};\n}");
		
		fprintf(file, "\nchar IndVarElimHelp::ID = 0;\nstatic RegisterPass<IndVarElimHelp> temp(\"indvarelim\",\"- to do induction variable elimination\");");
	}
	else if (strcmp(optnode->llnode->con.alpha,"GlobalCSE") == 0)
	{
		bracesCount = 0;
	
		fprintf(file, "#include \"llvm/Pass.h\"\n#include \"llvm/IR/Function.h\"\n#include \"llvm/IR/Type.h\"\n#include \"llvm/IR/Instruction.h\"\n#include \"llvm/ADT/StringRef.h\"\n#include \"llvm/Transforms/Utils/Local.h\"\n#include \"llvm/Support/raw_ostream.h\"\n#include <cstring>\n#include \"OptimiserAvEx.cpp\"\n\nusing namespace llvm;\nusing namespace std;\n\nnamespace\n{\n\tclass GCSE : public ");
		
		GCSE_fnRef = 1;	
				
		if (strcmp(optnode->next->llnode->con.alpha,"runOn") != 0)
			printerror("runOn not declared.\n");
		else if (strcmp(runoniter->llnode->con.alpha,"module") == 0)
		{
			processRunOnNode("Module", "GCSE");
			processIterateNode("module","function");
		}
		else if (strcmp(runoniter->llnode->con.alpha,"function") == 0)
		{
			GCSE_functionExists = 1; GCSE_findAvExIterCheck = 1; GCSE_gcseIterCheck = 1;
			processRunOnNode("Function", "GCSE");
		}
		else
			printerror(strcat(runoniter->llnode->con.alpha," cannot be run on for global common subexpression elimination.\n"));

		bracesCount++;

		curr = runoniter->next;
		while (curr!=NULL)
		{
			if (strcmp(curr->llnode->con.alpha,"iterate")==0)
			{
				curr = curr->next;

				if (strcmp(curr->llnode->con.alpha,"function")!=0)
					printerror(strcat(curr->llnode->con.alpha," cannot be iterated through for global common subexpression elimination.\n"));
				if (strcmp(runoniter->llnode->con.alpha,"function") == 0)
					printerror("Function is being run on, cannot be iterated through.");
					
				GCSE_functionExists = 1; 
			}
			else if (strcmp(curr->llnode->con.alpha,"availableexprs")==0)
			{
				curr = curr->next;
				if (strcmp(curr->llnode->con.alpha,"findAvailableExpressionsInfo") == 0)
				{
					if (!GCSE_findAvExIterCheck || !GCSE_functionExists)
						printerror("Function does not exist in scope.\n");
				
					curr = curr->next;

					if (strcmp(curr->llnode->con.alpha,"function") != 0)
						printerror(strcat(curr->llnode->con.alpha," cannot be used for finding available expressions.\n"));
				
					GCSE_avexComputed = 1;
					
					writeDataflowFiles();
					writeAvailableExprsFiles();
				}
				else 
					printerror("Available expressions info needs to be computed.");
			}
			else if (strcmp(curr->llnode->con.alpha,"gcse")==0)
			{	
				if (GCSE_avexComputed==0)
					printerror("Available expressions info needs to be computed.");
				
				if (!GCSE_gcseIterCheck || !GCSE_functionExists)
					printerror("Function does not exist in scope.\n");
				
				processGCSEParams(curr);
				
				fprintf(file,"\t\t\t\tassignIndicesToInstrs(&F);\
\n				for (Function::iterator FI = F.begin(), FE = F.end(); FI != FE; ++FI)\
\n				{\
\n					BasicBlock* block = &*FI;\
\n					\
\n					for (BasicBlock::iterator i = block->begin(), e = block->end(); i!=e; ++i)\
\n					{\
\n						Instruction * I = &*i;\
\n						if (BinaryOperator *BI = dyn_cast<BinaryOperator>(i))\
\n						{\
\n							Expression *expr = new Expression(BI);\
\n							\
\n							ItoE temp = AvExF[F.getName().str().c_str()];\
\n																\
\n							expati etemp = temp[InstrToIndex[I]];\
\n							\
\n							for (auto exi = etemp.begin(); exi!=etemp.end(); ++exi)\
\n							if (strcmp((*exi)->toString().c_str(),expr->toString().c_str())==0)\
\n							{	\
\n								for (auto itera = etemp.begin(); itera!=etemp.end(); ++itera)\
\n								{	\
\n									bool c = (*itera)->v1==expr->v1 && (*itera)->v2==expr->v2 && (*itera)->op==expr->op;\
\n									\
\n									if (!c)"	
									"\n\t\t\t\t\t\t\t\t\t{\
\n										if (checkExpr(expr)==checkExpr(*itera))\
\n										{\
\n											if (checkExpr(expr)==1)\
\n											{\
\n												if (compareExpr(expr,*itera,1))	\
\n												{	\
\n													Value *v1 = where[(*itera)->toString()];\
\n													I->replaceAllUsesWith(v1);\
\n													removeFromAvailable(F.getName().str().c_str(),expr);\
\n												}\
\n											}\
\n											else if (checkExpr(expr)==2)\
\n											{\
\n												if (compareExpr(expr,*itera,2))\
\n												{	\
\n													Value *v1 = where[(*itera)->toString()];\
\n													I->replaceAllUsesWith(v1);\
\n													removeFromAvailable(F.getName().str().c_str(),expr);\
\n												}\
\n											}\
\n											else if (checkExpr(expr)==3)\
\n											{\
\n												if (compareExpr(expr,*itera,3))\
\n												{\
\n													Value *v1 = where[(*itera)->toString()];\
\n													I->replaceAllUsesWith(v1);\
\n													removeFromAvailable(F.getName().str().c_str(),expr);\
\n												}\
\n											}\
\n										}\
\n									}\
\n									else if (possibleExps.find(expr->toString())!=possibleExps.end())\
\n									{\
\n										int noOfPred = 0;\
\n										for (auto it = pred_begin(block), et = pred_end(block); it != et; ++it)\
\n											noOfPred++;\
\n											\
\n										if (noOfPred==possibleExps[expr->toString()].size())\
\n										{\
\n											expati avtemp = AvExF[F.getName().str().c_str()][InstrToIndex[I]];\
\n											for (auto avtempi: avtemp)\
\n											{\
\n												if (strcmp(avtempi->toString().c_str(),expr->toString().c_str())==0)\
\n												{\
\n													PHINode *phi = PHINode::Create(I->getType(),0,\"whichExp\",&*block->begin());\
\n													for (auto ch:possibleExps[expr->toString()])\
\n														phi->addIncoming(where[ch->toString()],where[ch->toString()]->getParent());\
\n										\
\n													I->replaceAllUsesWith(phi);\
\n												\
\n													removeFromAvailable(F.getName().str().c_str(),expr);\
\n												}\
\n											}\
\n										}\
\n									}\
\n								}\
\n							}"				
						"\n\t\t\t\t\t\t}\
\n					}\
\n				}");

				for (i=0;i<bracesCount;i++)
					fprintf(file, "\n\t\t}");

				for (i=0; i < GCSE_paramsCount; i++)
					curr = curr->next;
			}

			
			curr = curr->next;
		}
		
		fprintf(file, "\n\t\tint countNoOfExprInFn(Function &F)\n\t\t{\n\t\t\tint count = 0;\n\t\t\tfor (Function::iterator FI = F.begin(), FE = F.end(); FI != FE; ++FI)\n\t\t\t{\n\t\t\t\tBasicBlock* block = &*FI;\n\t\t\t\tfor (BasicBlock::iterator i = block->begin(), e = block->end(); i!=e; ++i)\n\t\t\t\t\tif (BinaryOperator *BI = dyn_cast<BinaryOperator>(i))\n\t\t\t\t\t\tcount++;\n\t\t\t}\n\t\t\treturn count;\n\t\t}	\n\t};\n}");
		
		fprintf(file, "\n\nchar GCSE::ID = 0;\nstatic RegisterPass<GCSE> temp(\"gcse\",\"- to do global common subexpression elimination\");");
	}
	else if (strcmp(optnode->llnode->con.alpha,"LoopInvariantCodeMotion") == 0)
	{	
		bracesCount = 0;
		
		fprintf(file, "#include \"llvm/Pass.h\"\n#include \"llvm/IR/Function.h\"\n#include \"llvm/IR/Type.h\"\n#include \"llvm/IR/Instruction.h\"\n#include \"llvm/IR/InstIterator.h\"\n#include \"llvm/IR/ValueMap.h\"\n#include \"llvm/IR/Dominators.h\"\n#include \"llvm/ADT/StringRef.h\"\n#include \"llvm/ADT/SmallPtrSet.h\"\n#include \"llvm/ADT/SmallVector.h\"\n#include \"llvm/Analysis/LoopInfo.h\"\n#include \"llvm/Analysis/ValueTracking.h\"\n#include \"llvm/Transforms/Utils/Local.h\"\n#include \"llvm/Support/raw_ostream.h\"\n#include \"llvm/Support/Casting.h\"\n#include <cstring>\n#include <deque>\n#include <set>\n#include <stack>\n#include <iostream>\n\n#include \"OptimiserReachingDefs.cpp\"\nusing namespace llvm;\nusing namespace std;\nusing namespace reachdef;\n\nnamespace\n{\n\tclass LICM : public ");
			
		LICM_fnRef = 1;	

		if (strcmp(optnode->next->llnode->con.alpha,"runOn") != 0)
			printerror("runOn not declared.\n");
		else if (strcmp(runoniter->llnode->con.alpha,"module") == 0)
		{
			LICM_functionExists = 1; LICM_findReachDefsIterCheck = 1;
			processRunOnNode("Function", "LICM");
		}
		else if (strcmp(runoniter->llnode->con.alpha,"function") == 0)
		{
			LICM_functionExists = 1; LICM_findReachDefsIterCheck = 1;
			processRunOnNode("Function", "LICM");
		}
		else
			printerror(strcat(runoniter->llnode->con.alpha," cannot be run on for loop invariant code motion.\n"));
			
		bracesCount++;
		
		fprintf(file, "\n\t\t\t\tbool modified = false;\n\t\t\t\tLoopInfo &LI = getAnalysis<LoopInfoWrapperPass>().getLoopInfo();\n\t\t\t\tassignIndicesToInstrs(&F);");
		
		curr = runoniter->next;
		while (curr!=NULL)
		{
			if (strcmp(curr->llnode->con.alpha,"iterate")==0)
			{
				curr = curr->next;

				if (strcmp(curr->llnode->con.alpha,"function")==0)
				{
					LICM_functionExists = 1;
					if (LICM_loopExists)
						printerror("Function is out of scope.");
					if (strcmp(runoniter->llnode->con.alpha,"function") == 0)
						printerror("Function is being run on, cannot be iterated through.");
				}
				else if (strcmp(curr->llnode->con.alpha,"loop")==0)
				{
					LICM_loopExists = 1;
					fprintf(file, "\n\t\t\t\tfor (LoopInfo::reverse_iterator I = LI.rbegin(), E = LI.rend(); I != E; ++I)\n\t\t\t\t\taddLoopIntoQueue(*I);\n\n\t\t\t\twhile (!LQ.empty())\n\t\t\t\t{\n\t\t\t\t\tLoop* L  = LQ.back();\n\t\t\t\t\tif (L->getLoopPreheader() == NULL)\n\t\t\t\t\treturn false;");
					bracesCount++;
				}
				else
					printerror(strcat(curr->llnode->con.alpha," cannot be iterated through for loop invariant code motion.\n"));
					
				
			}
			else if (strcmp(curr->llnode->con.alpha,"reachingdefs")==0)
			{
				curr = curr->next;
				if (strcmp(curr->llnode->con.alpha,"findReachingDefinitionsInfo") == 0)
				{
					if (!LICM_findReachDefsIterCheck || !LICM_functionExists)
						printerror("Function does not exist in scope.\n");
					
					curr = curr->next;

					if (strcmp(curr->llnode->con.alpha,"function") != 0)
						printerror(strcat(curr->llnode->con.alpha," cannot be used for finding reaching definitions.\n"));
				
					LICM_rdefsComputed = 1;
					
					writeDataflowFiles();
					writeReachingDefsFiles();
				}
				else 
					printerror("Reaching Definitions info needs to be computed.");
			}
			else if (strcmp(curr->llnode->con.alpha,"loopinvstmts")==0)
			{
				curr = curr->next;
				if (strcmp(curr->llnode->con.alpha,"findLoopInvariantStatements") == 0)
				{	
					if (LICM_rdefsComputed==0)
						printerror("Reaching definitions info needs to be computed.");
				
					if (!LICM_functionExists)
						printerror("Function does not exist in scope.\n");
					if (!LICM_findLoopInvStmts || !LICM_loopExists)
						printerror("Loop does not exist in scope.\n");
				
					curr = curr->next;
					if (strcmp(curr->llnode->con.alpha,"loop") != 0)
						printerror(strcat(curr->llnode->con.alpha," cannot be used for finding loop invariant statements.\n"));
					
					curr = curr->next;
					if (strcmp(curr->llnode->con.alpha,"reachingdefs") != 0)
						printerror("Incorrect parameters to findLoopInvariantStatements.\n");
					
					LICM_loopinvComputed = 1;
					fprintf(file,"\n\n\t\t\t\t\tset<Value*> loopInvariantStatements = computeLoopInvariantStatements(L,F);\n");
				}
				else 
					printerror("Loop invariant statements need to be computed.");
			}
			else if (strcmp(curr->llnode->con.alpha,"motionstmts")==0)
			{
				fprintf(file,"\n\t\t\t\t\tstd::vector<BasicBlock*> loopBlocks = L->getBlocks();\n\t\t\t\t\tSmallVector<BasicBlock*, 0> ExitBlocks;\n\t\t\t\t\tL->getExitBlocks(ExitBlocks);\n\t\t\t\t\tDominatorTreeWrapperPass *DTWP = getAnalysisIfAvailable<DominatorTreeWrapperPass>();\n\t\t\t\t\tDominatorTree *DT = DTWP ? &DTWP->getDomTree() : nullptr;\n\n\t\t\t\t\tset<Value*> motionCandidates;\n");
				
				curr = curr->next;
				if (strcmp(curr->llnode->con.alpha,"findMotionCandidates") == 0)
				{	
					if (LICM_rdefsComputed==0)
						printerror("Reaching definitions info needs to be computed.");
					if (LICM_loopinvComputed==0)
						printerror("Loop invariant statements need to be computed.");
				
					if (!LICM_functionExists)
						printerror("Function does not exist in scope.\n");
					if (!LICM_findMotionCands || !LICM_loopExists)
						printerror("Loop does not exist in scope.\n");
				
					curr = curr->next;
					if (strcmp(curr->llnode->con.alpha,"loop") != 0)
						printerror(strcat(curr->llnode->con.alpha," cannot be used for finding code motion statements.\n"));
					
					curr = curr->next;
					if (strcmp(curr->llnode->con.alpha,"loopinvstmts") != 0)
						printerror("Incorrect parameters to findMotionCandidates.\n");
					
					LICM_motionCanComputed = 1;
					fprintf(file,"\n\t\t\t\t\tfor (std::set<Value*>::iterator i1 = loopInvariantStatements.begin(); i1 != loopInvariantStatements.end(); ++i1)\n\t\t\t\t\t{\n\t\t\t\t\t\tInstruction *I = static_cast<Instruction*>(*i1);\n\t\t\t\t\t\tbool dominatesCheck = true;\n\n\t\t\t\t\t\tfor (int i2=0; i2<ExitBlocks.size(); i2++)\n\t\t\t\t\t\t\tif (!DT->dominates(I->getParent(), ExitBlocks[i2]))\n\t\t\t\t\t\t\tdominatesCheck = false;\n\n\t\t\t\t\t\tif (dominatesCheck && checkFunctionCalls(L,I))\n\t\t\t\t\t\t{\n\t\t\t\t\t\tbool dominatesAllUseBlocksInLoop = true;\n\t\t\t\t\t\tValue* assignedVar = getDefinitionVar(I);\n\t\t\t\t\t\tif (assignedVar)\n\t\t\t\t\t\t{\n\t\t\t\t\t\tfor (Value::use_iterator ui = assignedVar->use_begin(), e = assignedVar->use_end(); ui != e; ++ui)\n\t\t\t\t\t\t{\n\t\t\t\t\t\tif (Instruction* userInstruction = dyn_cast<Instruction>(*ui))\n\t\t\t\t\t\t{\n\t\t\t\t\t\tBasicBlock* userBlock = userInstruction->getParent();\n\t\t\t\t\t\tif (L->contains(userBlock) && !DT->dominates(I->getParent(), userBlock))\n\t\t\t\t\t\t{\n\t\t\t\t\t\tdominatesAllUseBlocksInLoop = false;\n\t\t\t\t\t\tbreak;\n\t\t\t\t\t\t}\n\t\t\t\t\t\t}\n\t\t\t\t\t\t}\n\t\t\t\t\t\tif (dominatesAllUseBlocksInLoop)\n\t\t\t\t\t\t{\n\t\t\t\t\t\tbool hasNoOtherAssignmentsInLoop = true;\n\t\t\t\t\t\tstring assignedVarStr = valueToDefinitionVarStr(assignedVar);\n\t\t\t\t\t\tfor (std::vector<BasicBlock*>::iterator it1 = loopBlocks.begin(); it1 != loopBlocks.end(); ++it1)\n\t\t\t\t\t\t{\n\t\t\t\t\t\tfor (BasicBlock::iterator it2 = (*it1)->begin(), e = (*it1)->end(); it2 != e; ++it2)\n\t\t\t\t\t\t{\n\t\t\t\t\t\tif (&*it2!=&*I && valueToDefinitionVarStr(&*it2) == assignedVarStr)\n\t\t\t\t\t\t{\n\t\t\t\t\t\thasNoOtherAssignmentsInLoop = false;\n\t\t\t\t\t\tbreak;\n\t\t\t\t\t\t}\n\t\t\t\t\t\t}\n\t\t\t\t\t\tif (hasNoOtherAssignmentsInLoop)\n\t\t\t\t\t\tbreak;\n\t\t\t\t\t\t}\n\t\t\t\t\t\tif (hasNoOtherAssignmentsInLoop)\n\t\t\t\t\t\tmotionCandidates.insert(I);\n\t\t\t\t\t\t}\n\t\t\t\t\t\t}\n\t\t\t\t\t\t}\n\t\t\t\t\t}");
				}
				else 
					printerror("Code motion statements need to be computed.");
			}
			else if (strcmp(curr->llnode->con.alpha,"applyCodeMotion")==0)
			{
				if (LICM_rdefsComputed==0)
					printerror("Reaching definitions info needs to be computed.");
				if (LICM_loopinvComputed==0)
					printerror("Loop invariant statements needs to be computed.");
				if (LICM_motionCanComputed==0)
					printerror("Code motion statements need to be computed.");
			
				if (!LICM_functionExists)
					printerror("Function does not exist in scope.\n");
				if (!LICM_applyCodeMotion || !LICM_loopExists)
					printerror("Loop does not exist in scope.\n");
			
				curr = curr->next;
				if (strcmp(curr->llnode->con.alpha,"loop") != 0)
					printerror(strcat(curr->llnode->con.alpha," cannot be used for applying code motion.\n"));
				
				curr = curr->next;
				if (strcmp(curr->llnode->con.alpha,"motionstmts") != 0)
					printerror("Incorrect parameters to applyCodeMotion.\n");
					
				fprintf(file, "\n\n\t\t\t\t\tbool loopModified = applyMotionToCandidates(L, motionCandidates);\n\t\t\t\t\tmodified |= loopModified;\n\t\t\t\t\tLQ.pop_back();");
			}
			curr = curr->next;
		}
		for (i=0;i<bracesCount;i++)
			fprintf(file, "\n\t\t}");
			
		fprintf(file, "\n\n\t\tvoid getAnalysisUsage(AnalysisUsage& AU) const\n\t\t{\n\t\t\tAU.setPreservesCFG();\n\t\t\tAU.addRequired<LoopInfoWrapperPass>();\n\t\t\tAU.addRequired<DominatorTreeWrapperPass>();\n\t\t}\n\n\t\tprotected:\n			deque<Loop *> LQ;\n			\n			void addLoopIntoQueue(Loop* L)\n			{\n				this->LQ.push_back(L);\n				for (Loop::reverse_iterator I = L->rbegin(), E = L->rend(); I != E; ++I)\n					addLoopIntoQueue(*I); \n			}\n			\n			bool checkFunctionCalls(Loop *L, Instruction *I)\n			{\n				std::vector<BasicBlock*> loopBlocks = L->getBlocks();\n				for (std::vector<BasicBlock*>::iterator blockIter = loopBlocks.begin(); blockIter != loopBlocks.end(); ++blockIter)\n				{\n					for (BasicBlock::iterator instIter = (*blockIter)->begin(), e = (*blockIter)->end(); instIter != e; ++instIter)\n					{\n						Instruction* i = &*instIter;\n						if (isa<CallInst>(i))\n						{\n							for (User::op_iterator opIter = I->op_begin(), e = I->op_end(); opIter != e; ++opIter)\n							{\n								Value* v = *opIter;\n								if (isa<GlobalValue>(v))\n									return false;\n							}\n						}\n					}\n				}\n				return true;\n			}\n			\n			bool checkOperandsForPHI(Instruction *I)\n			{\n				for (User::op_iterator opIter = I->op_begin(), e = I->op_end(); opIter != e; ++opIter)\n				{\n					Value* i = *opIter;\n					if (isa<PHINode>((Instruction*)i))\n						return false;\n				}\n			}\n			\n			std::set<Value*> computeLoopInvariantStatements(Loop* L, Function &F)\n			{\n				std::set<Value*> loopInvariantStatements;\n				std::vector<BasicBlock*> loopBlocks = L->getBlocks();\n\n				for (std::vector<BasicBlock*>::iterator blockIter = loopBlocks.begin(); blockIter != loopBlocks.end(); ++blockIter)\n				{\n					for (BasicBlock::iterator instIter = (*blockIter)->begin(), e = (*blockIter)->end(); instIter != e; ++instIter)\n					{\n						Value* v = &*instIter;\n						\n						if (isa<Constant>(v) || isa<Argument>(v) || isa<GlobalValue>(v)) // definitely loop invariant\n							loopInvariantStatements.insert(v);\n						else if (isa<Instruction>(v))\n						{\n							Instruction* I = static_cast<Instruction*>(v);\n							\n							if (isSafeToSpeculativelyExecute(I) && !I->mayReadFromMemory() && !isa<LandingPadInst>(I)) // may be invariant\n							{\n								bool allOperandsOnlyDefinedOutsideLoop = true; // check if definitions of operands reach inside the loop\n								for (User::op_iterator opIter = I->op_begin(), e = I->op_end(); opIter != e; ++opIter)\n								{\n									Value* opVal = *opIter;\n									for (std::vector<BasicBlock*>::iterator i1 = loopBlocks.begin(); i1 != loopBlocks.end(); ++i1)\n");
		fprintf(file,"									{\n										for (BasicBlock::iterator i2 = (*i1)->begin(), e = (*i1)->end(); i2 != e; ++i2)\n										{\n											Instruction* i = &*i2;\n											std::vector<string> defs = ReachDefs[F.getName().str().c_str()][InstrToIndex[i]];\n											\n											string percentage = \"%%\";\n											string opValName = percentage + opVal->getName().str();\n											\n											if (find(defs.begin(), defs.end(),opValName)!=defs.end())\n											{\n												allOperandsOnlyDefinedOutsideLoop = false;\n												break;\n											}		\n										}\n										if (!allOperandsOnlyDefinedOutsideLoop)\n											break;\n									}\n								}\n								if (allOperandsOnlyDefinedOutsideLoop && !isa<PHINode>(I) && checkOperandsForPHI(I))\n									loopInvariantStatements.insert(v);\n							}\n						}\n					}\n				}\n				bool converged = false;\n				int invariantSetSize = loopInvariantStatements.size();\n\n				while (!converged) // statements can be loop invariant if the reaching definitions of their operands are invariant \n				{\n					int prevInvariantSetSize = invariantSetSize;\n					for (std::vector<BasicBlock*>::iterator blockIter = loopBlocks.begin(); blockIter != loopBlocks.end(); ++blockIter)\n					{\n						for (BasicBlock::iterator instIter = (*blockIter)->begin(), e = (*blockIter)->end(); instIter != e; ++instIter)\n						{\n							Value* v = &*instIter;\n							if (loopInvariantStatements.find(v) != loopInvariantStatements.end()) // if already marked as loop invariant\n								continue;\n							if (isa<Instruction>(v))\n							{\n								Instruction* I = static_cast<Instruction*>(v);\n								if (L->hasLoopInvariantOperands(I) && !isa<PHINode>(I) && checkOperandsForPHI(I))\n									loopInvariantStatements.insert(v);\n								\n							}\n						}\n					}\n					invariantSetSize = loopInvariantStatements.size();\n					converged = (invariantSetSize == prevInvariantSetSize);\n				}	\n");
		fprintf(file,"				return loopInvariantStatements;\n			}\n			\n			bool applyMotionToCandidates(Loop* L, set<Value*> motionCandidates) \n			{\n				bool motionApplied = false;		// do DFS over the blocks of the loop and move each candidate to end of preheader \n				BasicBlock* preheader = L->getLoopPreheader(); // if all of its dependencies have also been moved to the preheader\n				set<Instruction*> toMoveSet;\n				set<BasicBlock*> visited;\n\n				stack<BasicBlock*> work;\n				work.push(*succ_begin(preheader)); //start at loop header, sole successor of the pre-header\n				while (!work.empty())\n				{\n					BasicBlock* block = work.top();\n					work.pop();\n					visited.insert(block);\n			\n					for (BasicBlock::iterator instIter = block->begin(), e = block->end(); instIter != e; ++instIter) \n					{\n						Instruction* I = &*instIter;\n						if (motionCandidates.count(I) > 0) // move to preheader if code motion candidate \n						{\n							motionApplied = true;\n							toMoveSet.insert(I);\n						}\n					}\n\n					for (succ_iterator successorBlock = succ_begin(block), E = succ_end(block); successorBlock != E; ++successorBlock) \n					{\n						if (L->contains(*successorBlock)) \n						{\n							if (visited.count(*successorBlock) == 0)\n								work.push(*successorBlock);\n						}\n					}\n				}\n\n				for (set<Instruction*>::iterator it = toMoveSet.begin(); it != toMoveSet.end(); ++it) // moving\n				{\n					Instruction* instructionToMove = *it;\n					Instruction* preheaderEnd = &(preheader->back()); // insert before last instruction of preheader\n					instructionToMove->removeFromParent();\n					instructionToMove->insertBefore(preheaderEnd);\n				}\n				return motionApplied;\n			}\n");
		
		fprintf(file,"\n\t};\n}\n\nchar LICM::ID = 0;\nstatic RegisterPass<LICM> temp(\"newlicm\",\"- to do loop invariant code motion\");");
	}
	else if (strcmp(optnode->llnode->con.alpha,"GlobalVN") == 0)
	{
		fprintf(file, "#include \"llvm/Pass.h\"\n#include \"llvm/ADT/BitVector.h\"\n#include \"llvm/ADT/DenseMap.h\"\n#include \"llvm/ADT/DenseSet.h\"\n#include \"llvm/ADT/PostOrderIterator.h\"\n#include \"llvm/Analysis/AssumptionCache.h\"\n#include \"llvm/Analysis/AliasAnalysis.h\"\n#include \"llvm/Analysis/ConstantFolding.h\"\n#include \"llvm/Analysis/GlobalsModRef.h\"\n#include \"llvm/Analysis/InstructionSimplify.h\"	\n#include \"llvm/IR/Function.h\"\n#include \"llvm/IR/Type.h\"\n#include \"llvm/IR/Instruction.h\"\n#include \"llvm/IR/DataLayout.h\"\n#include \"llvm/IR/IntrinsicInst.h\"\n#include \"llvm/Transforms/Scalar/GVNExpression.h\"\n#include \"llvm/Transforms/Utils/Local.h\"\n#include \"llvm/Transforms/Utils/MemorySSA.h\"\n#include \"llvm/Support/raw_ostream.h\"\n#include <cstring>\n#include <vector>\n\nusing namespace llvm;\nusing namespace std;\nusing namespace llvm::GVNExpression;\n\nnamespace llvm\n{\n	template <> struct DenseMapInfo<const Expression *>\n	{\n		static const Expression *getEmptyKey()\n		{\n			auto Val = static_cast<uintptr_t>(-1);\n			Val <<= PointerLikeTypeTraits<const Expression *>::NumLowBitsAvailable;\n			return reinterpret_cast<const Expression *>(Val);\n		}\n		static const Expression *getTombstoneKey()\n		{\n			auto Val = static_cast<uintptr_t>(~1U);\n			Val <<= PointerLikeTypeTraits<const Expression *>::NumLowBitsAvailable;\n			return reinterpret_cast<const Expression *>(Val);\n		}\n		static unsigned getHashValue(const Expression *V)\n		{\n			return static_cast<unsigned>(V->getHashValue());\n		}\n		static bool isEqual(const Expression *LHS, const Expression *RHS)\n		{\n			if (LHS == RHS)\n				return true;\n			if (LHS == getTombstoneKey() || RHS == getTombstoneKey() || LHS == getEmptyKey() || RHS == getEmptyKey())\n				return false;\n			return *LHS == *RHS;\n		}\n	};\n} // end namespace llvm\n\nnamespace\n{\n\tstatic BasicBlock *getBlockForValue(Value *V)\n	{\n		if (auto *I = dyn_cast<Instruction>(V))\n			return I->getParent();\n		return nullptr;\n	}\n\n	struct CongruenceClass\n	{\n		using MemberSet = SmallPtrSet<Value *, 4>;\n		unsigned ID;\n		Value *RepLeader = nullptr; // representative leader of class\n		const Expression *DefiningExpr = nullptr; // defining Expression\n		MemberSet Members; // actual members of the class\n		bool Dead = false; // true if the class has no members left - for assertion purposes, for skipping empty classes\n		int StoreCount = 0; // number of stores in the congruence class\n		std::pair<Value *, unsigned int> NextLeader = {nullptr, ~0U}; // next most dominating after current leader\n		explicit CongruenceClass(unsigned ID) : ID(ID) {}\n		CongruenceClass(unsigned ID, Value *Leader, const Expression *E) : ID(ID), RepLeader(Leader), DefiningExpr(E) {}\n	};\n\n	static bool alwaysAvailable(Value *V)\n	{\n		return isa<Constant>(V) || isa<Argument>(V);\n	}\n\n\tclass GlobalVN : public ");
		
		if (strcmp(optnode->next->llnode->con.alpha,"runOn") != 0)
			printerror("runOn not declared.\n");
		else if (strcmp(runoniter->llnode->con.alpha,"function") == 0)
		{
			processRunOnNode("Function", "GlobalVN");
		}
		else
			printerror(strcat(runoniter->llnode->con.alpha," cannot be run on for global value numbering.\n"));
			
		fprintf(file, "\n\t\t\tbool Changed = false;\n			DL = &F.getParent()->getDataLayout();\n			AC = &getAnalysis<AssumptionCacheTracker>().getAssumptionCache(F);\n			MSSA = &getAnalysis<MemorySSAWrapperPass>().getMSSA();\n			TLI = &getAnalysis<TargetLibraryInfoWrapperPass>().getTLI();\n			DT = &getAnalysis<DominatorTreeWrapperPass>().getDomTree();\n			AA = &getAnalysis<AAResultsWrapperPass>().getAAResults();\n			MSSAWalker = MSSA->getWalker();\n\n			unsigned ICount = 1; // instruction count for size of hash table\n			DFSToInstr.emplace_back(nullptr); // 0th instruction, start at 1");
		
		curr = runoniter->next;
		while (curr!=NULL)
		{
			if (strcmp(curr->llnode->con.alpha,"iterate")==0)
			{
				curr = curr->next;
				if (strcmp(curr->llnode->con.alpha,"instruction")==0)
				{
					curr = curr->next;
					if (strcmp(curr->llnode->con.alpha,"reversepostorder")==0)
					{
						fprintf(file, "\n\t\t\tDenseMap<const DomTreeNode *, unsigned> RPOOrdering; // to get in order of reverse post order\n			ReversePostOrderTraversal<Function *> RPOT(&F);\n			\n			unsigned Counter = 0;\n			for (auto &B : RPOT) \n			{\n				auto *Node = DT->getNode(B);\n				assert(Node && \"RPO and Dominator tree should have same reachability\");\n				RPOOrdering[Node] = ++Counter;\n			}\n			for (auto &B : RPOT) // sort dominator tree children arrays into reverse post order\n			{\n				auto *Node = DT->getNode(B);\n				if (Node->getChildren().size() > 1)\n					std::sort(Node->begin(), Node->end(), [&RPOOrdering](const DomTreeNode *A, const DomTreeNode *B) { return RPOOrdering[A] < RPOOrdering[B]; });\n			}\n			auto DFI = df_begin(DT->getRootNode()); // depth first search of domtree gives reverse post order\n			for (auto DFE = df_end(DT->getRootNode()); DFI != DFE; ++DFI)\n			{\n				BasicBlock *B = DFI->getBlock();\n				const auto &BlockRange = assignDFSNumbers(B, ICount);\n				BlockInstRange.insert({B, BlockRange});\n				ICount += BlockRange.second - BlockRange.first;\n			}\n			for (auto &B : F) // handle forward unreachable blocks, find which blocks have single predecessors\n			{\n				if (!DFI.nodeVisited(DT->getNode(&B))) // assign numbers to unreachable blocks\n				{\n					const auto &BlockRange = assignDFSNumbers(&B, ICount);\n					BlockInstRange.insert({&B, BlockRange});\n					ICount += BlockRange.second - BlockRange.first;\n				}\n			}\n			\n			TouchedInstructions.resize(ICount);\n			DominatedInstRange.reserve(F.size());\n			ExpressionToClass.reserve(ICount);\n\n			const auto &InstRange = BlockInstRange.lookup(&F.getEntryBlock()); // touched instructions include entry block\n			TouchedInstructions.set(InstRange.first, InstRange.second);\n			ReachableBlocks.insert(&F.getEntryBlock());\n\n			initializeCongruenceClasses(F);\n");
					}
				}
				else if (strcmp(curr->llnode->con.alpha,"function")==0)
					printerror("Function is being run on, cannot be iterated through.");
			}
			else if (strcmp(curr->llnode->con.alpha,"valuenumber")==0)
			{
				curr = curr->next;
				
				if (strcmp(curr->llnode->con.alpha,"instruction")!=0)
					printerror("Value numbering can be done only on instructions.");
					
				if (GVN_instrExists==0)
					printerror("Instruction is out of scope.");
					
				fprintf(file, "\n\t\t\tunsigned int Iterations = 0;\n			BasicBlock *LastBlock = &F.getEntryBlock();  // start at entry block\n			while (TouchedInstructions.any())\n			{\n				++Iterations; 		// traverse instructions in all the blocks in reverse post order\n				for (int InstrNum = TouchedInstructions.find_first(); InstrNum != -1; InstrNum = TouchedInstructions.find_next(InstrNum))\n				{\n					assert(InstrNum != 0 && \"Bit 0 should never be set, something touched an instruction not in the lookup table\");\n					Value *V = DFSToInstr[InstrNum];\n					BasicBlock *CurrBlock = nullptr;\n\n					if (auto *I = dyn_cast<Instruction>(V))\n						CurrBlock = I->getParent();\n					else if (auto *MP = dyn_cast<MemoryPhi>(V))\n						CurrBlock = MP->getBlock();\n					else\n						llvm_unreachable(\"DFSToInstr gave us an unknown type of instruction\");\n\n					if (CurrBlock != LastBlock)\n					{\n						LastBlock = CurrBlock;\n						bool BlockReachable = ReachableBlocks.count(CurrBlock);\n						const auto &CurrInstRange = BlockInstRange.lookup(CurrBlock);\n\n						if (!BlockReachable)\n						{\n							TouchedInstructions.reset(CurrInstRange.first, CurrInstRange.second);\n							continue;\n						}\n						updateProcessedCount(CurrBlock);\n					}\n\n					if (auto *MP = dyn_cast<MemoryPhi>(V))\n						valueNumberMemoryPhi(MP);\n					else if (auto *I = dyn_cast<Instruction>(V))\n						valueNumberInstruction(I);\n					else\n						llvm_unreachable(\"Should have been a MemoryPhi or Instruction\");\n					updateProcessedCount(V);\n					TouchedInstructions.reset(InstrNum);\n				}\n			}\n");
			}
			else if (strcmp(curr->llnode->con.alpha,"eliminateInstructions")==0)
			{
				curr = curr->next;
				if (strcmp(curr->llnode->con.alpha,"function")!=0)
					printerror("Can only pass function as parameter to eliminateInstructions.");
				fprintf(file, "\n\t\t\tChanged |= eliminateInstructions(F);");
			}
			else if (strcmp(curr->llnode->con.alpha,"eraseUnusedInstructions")==0)
			{
				fprintf(file, "\n\t\t\tfor (Instruction *ToErase : InstructionsToErase)\n			{\n				if (!ToErase->use_empty())\n					ToErase->replaceAllUsesWith(UndefValue::get(ToErase->getType()));\n\n				ToErase->eraseFromParent();\n			}\n");
			}
			curr = curr->next;
		}
		
		fprintf(file,"\n\t\t\tauto UnreachableBlockPred = [&](const BasicBlock &BB) 	// delete all unreachable blocks\n			{\n				return !ReachableBlocks.count(&BB);\n			};\n\n			for (auto &BB : make_filter_range(F, UnreachableBlockPred))\n			{\n				deleteInstructionsInBlock(&BB);\n				Changed = true;\n			}\n\n			cleanupTables();\n			return Changed;\n		}");
		
		fprintf(file,"\n\nvoid getAnalysisUsage(AnalysisUsage& AU) const\n		{\n			AU.addRequired<AssumptionCacheTracker>();\n			AU.addRequired<DominatorTreeWrapperPass>();\n			AU.addRequired<TargetLibraryInfoWrapperPass>();\n			AU.addRequired<MemorySSAWrapperPass>();\n			AU.addRequired<AAResultsWrapperPass>();\n			AU.addPreserved<DominatorTreeWrapperPass>();\n			AU.addPreserved<GlobalsAAWrapperPass>();\n		}\n		\n		std::pair<unsigned, unsigned> assignDFSNumbers(BasicBlock *B, unsigned Start)\n		{\n			unsigned End = Start;\n			if (MemoryAccess *MemPhi = MSSA->getMemoryAccess(B))\n			{\n				InstrDFS[MemPhi] = End++;\n				DFSToInstr.emplace_back(MemPhi);\n			}\n			for (auto &I : *B)\n			{\n				InstrDFS[&I] = End++;\n				DFSToInstr.emplace_back(&I);\n			}\n			return std::make_pair(Start, End); \n		}\n		\n		CongruenceClass *createSingletonCongruenceClass(Value *Member)\n		{\n			CongruenceClass *CClass = createCongruenceClass(Member, nullptr);\n			CClass->Members.insert(Member);\n			ValueToClass[Member] = CClass;\n			return CClass;\n		}\n\n		CongruenceClass *createCongruenceClass(Value *Leader, const Expression *E)\n		{\n			auto *result = new CongruenceClass(NextCongruenceNum++, Leader, E);\n			CongruenceClasses.emplace_back(result);\n			return result;\n		}\n\n		void initializeCongruenceClasses(Function &F)\n		{\n			NextCongruenceNum = 2;\n			CongruenceClass::MemberSet InitialValues;\n			InitialClass = createCongruenceClass(nullptr, nullptr);\n			for (auto &B : F)\n			{\n				if (auto *MP = MSSA->getMemoryAccess(&B))\n					MemoryAccessEquiv.insert({MP, MSSA->getLiveOnEntryDef()});\n				for (auto &I : B)\n				{\n					InitialValues.insert(&I);\n					ValueToClass[&I] = InitialClass;\n					if (isa<StoreInst>(&I))\n					{\n						MemoryAccessEquiv.insert({MSSA->getMemoryAccess(&I), MSSA->getLiveOnEntryDef()});\n						++InitialClass->StoreCount;\n						assert(InitialClass->StoreCount > 0);\n					}\n				}\n			}\n			InitialClass->Members.swap(InitialValues); \n			for (auto &FA : F.args()) 				// initialize arguments to be in their own unique congruence classes\n				createSingletonCongruenceClass(&FA);\n		}\n\n		void updateProcessedCount(Value *V)\n		{\n			#ifndef NDEBUG\n				if (ProcessedCount.count(V) == 0)\n					ProcessedCount.insert({V, 1});\n				else\n				{\n					ProcessedCount[V] += 1;\n					assert(ProcessedCount[V] < 100 && \"Seem to have processed the same Value a lot\");\n				}\n			#endif\n		}\n		\n		MemoryAccess *lookupMemoryAccessEquiv(MemoryAccess *MA) const\n		{\n			MemoryAccess *Result = MemoryAccessEquiv.lookup(MA);\n			return Result ? Result : MA;\n		}\n		\n		bool setMemoryAccessEquivTo(MemoryAccess *From, MemoryAccess *To)\n		{\n");
		fprintf(file,"			auto LookupResult = MemoryAccessEquiv.find(From);\n			bool Changed = false;\n			if (LookupResult != MemoryAccessEquiv.end()) 	// if it's in the table, see if value changed\n			{\n				if (To && LookupResult->second != To) 		// it wasn't equivalent before, now it is\n				{\n					LookupResult->second = To;\n					Changed = true;\n				}\n				else if (!To) 					// it used to be equivalent to something, now it's not\n				{\n					MemoryAccessEquiv.erase(LookupResult);\n					Changed = true;\n				}\n			}\n			else\n				assert(!To && \"Memory equivalence should never change from nothing to something\");\n			return Changed;\n		}\n		\n		void markMemoryUsersTouched(MemoryAccess *MA)\n		{\n			for (auto U : MA->users())\n				if (auto *MUD = dyn_cast<MemoryUseOrDef>(U))\n					TouchedInstructions.set(InstrDFS[MUD->getMemoryInst()]);\n				else\n					TouchedInstructions.set(InstrDFS[U]);\n		}\n		\n		template <class T>	// basic blocks or basic block edges\n		Value *lookupOperandLeader(Value *V, const User *U, const T &B) const\n		{\n			CongruenceClass *CC = ValueToClass.lookup(V);\n			if (CC && (CC != InitialClass))\n				return CC->RepLeader;\n			return V;\n		}\n		\n		bool setBasicExpressionInfo(Instruction *I, BasicExpression *E, const BasicBlock *B)\n		{\n			bool AllConstant = true;\n			if (auto *GEP = dyn_cast<GetElementPtrInst>(I))\n				E->setType(GEP->getSourceElementType());\n			else\n				E->setType(I->getType());\n			E->setOpcode(I->getOpcode());\n			E->allocateOperands(ArgRecycler, ExpressionAllocator);\n\n			std::transform(I->op_begin(), I->op_end(), op_inserter(E), [&](Value *O) { auto Operand = lookupOperandLeader(O, I, B); AllConstant &= isa<Constant>(Operand); return Operand; });\n			return AllConstant; // transform operand array into operand leader array, keep track of whether all members are constant\n		}\n		\n		const Expression *checkSimplificationResults(Expression *E, Instruction *I, Value *V)\n		{\n			if (!V)\n				return nullptr;\n			if (auto *C = dyn_cast<Constant>(V))\n			{\n				assert(isa<BasicExpression>(E) && \"We should always have had a basic expression here\");\n				cast<BasicExpression>(E)->deallocateOperands(ArgRecycler);\n				ExpressionAllocator.Deallocate(E);\n				return createConstantExpression(C);\n			}\n			else if (isa<Argument>(V) || isa<GlobalVariable>(V))\n			{\n				cast<BasicExpression>(E)->deallocateOperands(ArgRecycler);\n				ExpressionAllocator.Deallocate(E);\n				return createVariableExpression(V);\n			}\n			CongruenceClass *CC = ValueToClass.lookup(V);\n			if (CC && CC->DefiningExpr)\n			{\n				assert(isa<BasicExpression>(E) && \"We should always have had a basic expression here\");\n				cast<BasicExpression>(E)->deallocateOperands(ArgRecycler);\n				ExpressionAllocator.Deallocate(E);\n				return CC->DefiningExpr;\n			}\n			return nullptr;\n		}\n		\n		const Expression *createExpression(Instruction *I, const BasicBlock *B)\n		{\n			auto *E = new (ExpressionAllocator) BasicExpression(I->getNumOperands());\n");
		fprintf(file,"			bool AllConstant = setBasicExpressionInfo(I, E, B);\n\n			if (I->isCommutative())\n			{\n				assert(I->getNumOperands() == 2 && \"Unsupported commutative instruction!\");\n				if (E->getOperand(0) > E->getOperand(1))\n					E->swapOperands(0, 1); // to get the same value number \n			}\n			\n			// perform simplificaiton\n			if (auto *CI = dyn_cast<CmpInst>(I))\n			{\n				CmpInst::Predicate Predicate = CI->getPredicate(); // sort operand value numbers so x<y and y>x get the same value\n				if (E->getOperand(0) > E->getOperand(1))\n				{\n					E->swapOperands(0, 1);\n					Predicate = CmpInst::getSwappedPredicate(Predicate);\n				}\n				E->setOpcode((CI->getOpcode() << 8) | Predicate);\n				assert(I->getOperand(0)->getType() == I->getOperand(1)->getType() && \"Wrong types on cmp instruction\");\n				if ((E->getOperand(0)->getType() == I->getOperand(0)->getType() && E->getOperand(1)->getType() == I->getOperand(1)->getType()))\n				{\n					Value *V = SimplifyCmpInst(Predicate, E->getOperand(0), E->getOperand(1), *DL, TLI, DT, AC);\n					if (const Expression *SimplifiedE = checkSimplificationResults(E, I, V))\n						return SimplifiedE;\n				}\n			}\n			else if (isa<SelectInst>(I))\n			{\n				if (isa<Constant>(E->getOperand(0)) || (E->getOperand(1)->getType() == I->getOperand(1)->getType() && E->getOperand(2)->getType() == I->getOperand(2)->getType()))\n				{\n					Value *V = SimplifySelectInst(E->getOperand(0), E->getOperand(1), E->getOperand(2), *DL, TLI, DT, AC);\n					if (const Expression *SimplifiedE = checkSimplificationResults(E, I, V))\n						return SimplifiedE;\n				}\n			}\n			else if (I->isBinaryOp())\n			{\n				Value *V = SimplifyBinOp(E->getOpcode(), E->getOperand(0), E->getOperand(1), *DL, TLI, DT, AC);\n				if (const Expression *SimplifiedE = checkSimplificationResults(E, I, V))\n					return SimplifiedE;\n			}\n			else if (auto *BI = dyn_cast<BitCastInst>(I))\n			{\n				Value *V = SimplifyInstruction(BI, *DL, TLI, DT, AC);\n				if (const Expression *SimplifiedE = checkSimplificationResults(E, I, V))\n					return SimplifiedE;\n			}\n			else if (isa<GetElementPtrInst>(I))\n			{\n			Value *V = SimplifyGEPInst(E->getType(), ArrayRef<Value *>(E->op_begin(), E->op_end()), *DL, TLI, DT, AC);\n			if (const Expression *SimplifiedE = checkSimplificationResults(E, I, V))\n				return SimplifiedE;\n			}\n			else if (AllConstant)\n			{\n				SmallVector<Constant *, 8> C;\n				for (Value *Arg : E->operands())\n					C.emplace_back(cast<Constant>(Arg));\n\n				if (Value *V = ConstantFoldInstOperands(I, C, *DL, TLI))\n					if (const Expression *SimplifiedE = checkSimplificationResults(E, I, V))\n						return SimplifiedE;\n			}\n			return E;\n		}\n		\n		const ConstantExpression *createConstantExpression(Constant *C)\n		{\n			auto *E = new (ExpressionAllocator) ConstantExpression(C);\n			E->setOpcode(C->getValueID());\n			return E;\n		}\n		\n");
		fprintf(file,"		const VariableExpression *createVariableExpression(Value *V)\n		{\n			auto *E = new (ExpressionAllocator) VariableExpression(V);\n			E->setOpcode(V->getValueID());\n			return E;\n		}\n		\n		const Expression *createBinaryExpression(unsigned Opcode, Type *T, Value *Arg1, Value *Arg2, const BasicBlock *B)\n		{\n			auto *E = new (ExpressionAllocator) BasicExpression(2);\n\n			E->setType(T);\n			E->setOpcode(Opcode);\n			E->allocateOperands(ArgRecycler, ExpressionAllocator);\n			if (Instruction::isCommutative(Opcode))\n				if (Arg1 > Arg2)\n					std::swap(Arg1, Arg2);\n					\n			E->op_push_back(lookupOperandLeader(Arg1, nullptr, B));\n			E->op_push_back(lookupOperandLeader(Arg2, nullptr, B));\n\n			Value *V = SimplifyBinOp(Opcode, E->getOperand(0), E->getOperand(1), *DL, TLI, DT, AC);\n			if (const Expression *SimplifiedE = checkSimplificationResults(E, nullptr, V))\n				return SimplifiedE;\n			return E;\n		}\n		\n		PHIExpression *createPHIExpression(Instruction *I)\n		{\n			BasicBlock *PHIBlock = I->getParent();\n			auto *PN = cast<PHINode>(I);\n			auto *E = new (ExpressionAllocator) PHIExpression(PN->getNumOperands(), PHIBlock);\n\n			E->allocateOperands(ArgRecycler, ExpressionAllocator);\n			E->setType(I->getType());\n			E->setOpcode(I->getOpcode());\n\n			auto ReachablePhiArg = [&](const Use &U) { return ReachableBlocks.count(PN->getIncomingBlock(U)); };\n\n			auto Filtered = make_filter_range(PN->operands(), ReachablePhiArg); 	// filter unreachable operands\n\n			std::transform(Filtered.begin(), Filtered.end(), op_inserter(E), [&](const Use &U) -> Value * { if (U == PN) return PN; const BasicBlockEdge BBE(PN->getIncomingBlock(U), PHIBlock); return lookupOperandLeader(U, I, BBE); });\n			return E;\n		}\n		\n		const UnknownExpression *createUnknownExpression(Instruction *I)\n		{\n			auto *E = new (ExpressionAllocator) UnknownExpression(I);\n			E->setOpcode(I->getOpcode());\n			return E;\n		}\n		\n		LoadExpression *createLoadExpression(Type *LoadType, Value *PointerOp, LoadInst *LI, MemoryAccess *DA, const BasicBlock *B)\n		{\n			auto *E = new (ExpressionAllocator) LoadExpression(1, LI, DA);\n			E->allocateOperands(ArgRecycler, ExpressionAllocator);\n			E->setType(LoadType);\n\n			E->setOpcode(0);		//  same opcode for store and loads so they value number together\n			E->op_push_back(lookupOperandLeader(PointerOp, LI, B));\n			if (LI)\n				E->setAlignment(LI->getAlignment());\n\n			return E;\n		}\n		\n		const StoreExpression *createStoreExpression(StoreInst *SI, MemoryAccess *DA, const BasicBlock *B)\n		{\n			auto *E = new (ExpressionAllocator) StoreExpression(SI->getNumOperands(), SI, DA);\n			E->allocateOperands(ArgRecycler, ExpressionAllocator);\n			E->setType(SI->getValueOperand()->getType());\n\n			E->setOpcode(0);		//  same opcode for store and loads so they value number together\n			E->op_push_back(lookupOperandLeader(SI->getPointerOperand(), SI, B));\n\n			return E;\n		}\n		\n		const CallExpression *createCallExpression(CallInst *CI, MemoryAccess *HV, const BasicBlock *B)\n		{\n			auto *E = new (ExpressionAllocator) CallExpression(CI->getNumOperands(), CI, HV);\n			setBasicExpressionInfo(CI, E, B);\n			return E;\n		}\n\n		bool hasMemberOtherThanUs(const CongruenceClass *CC, Instruction *I) // check if congruence class has member other than given instruction\n		{\n			return CC->StoreCount > 1 || CC->Members.count(I) == 0;\n		}\n		\n		const AggregateValueExpression *createAggregateValueExpression(Instruction *I, const BasicBlock *B)\n		{\n			if (auto *II = dyn_cast<InsertValueInst>(I))\n			{\n				auto *E = new (ExpressionAllocator) AggregateValueExpression(I->getNumOperands(), II->getNumIndices());\n				setBasicExpressionInfo(I, E, B);\n				E->allocateIntOperands(ExpressionAllocator);\n				std::copy(II->idx_begin(), II->idx_end(), int_op_inserter(E));\n				return E;\n			}\n			else if (auto *EI = dyn_cast<ExtractValueInst>(I))\n			{\n				auto *E = new (ExpressionAllocator) AggregateValueExpression(I->getNumOperands(), EI->getNumIndices());\n				setBasicExpressionInfo(EI, E, B);\n				E->allocateIntOperands(ExpressionAllocator);\n				std::copy(EI->idx_begin(), EI->idx_end(), int_op_inserter(E));\n");
		fprintf(file,"				return E;\n			}\n			llvm_unreachable(\"Unhandled type of aggregate value operation\");\n		}\n		\n		const Expression *performSymbolicStoreEvaluation(Instruction *I, const BasicBlock *B)\n		{\n			auto *SI = cast<StoreInst>(I);	// avoid value numbering stores\n			MemoryAccess *StoreAccess = MSSA->getMemoryAccess(SI);\n			if (SI->isSimple())\n			{\n				MemoryAccess *StoreRHS = lookupMemoryAccessEquiv(cast<MemoryDef>(StoreAccess)->getDefiningAccess()); // get expression for RHS of MemoryDef\n				const Expression *OldStore = createStoreExpression(SI, StoreRHS, B);\n				CongruenceClass *CC = ExpressionToClass.lookup(OldStore);\n				if (CC && CC->DefiningExpr && isa<StoreExpression>(CC->DefiningExpr) && CC->RepLeader == lookupOperandLeader(SI->getValueOperand(), SI, B) && hasMemberOtherThanUs(CC, I))\n					return createStoreExpression(SI, StoreRHS, B);\n			}\n			return createStoreExpression(SI, StoreAccess, B);\n		}\n\n		const Expression *performSymbolicLoadEvaluation(Instruction *I, const BasicBlock *B)\n		{\n			auto *LI = cast<LoadInst>(I);\n			if (!LI->isSimple())\n				return nullptr;\n				\n			Value *LoadAddressLeader = lookupOperandLeader(LI->getPointerOperand(), I, B);\n			if (isa<UndefValue>(LoadAddressLeader))\n				return createConstantExpression(UndefValue::get(LI->getType()));\n\n			MemoryAccess *DefiningAccess = MSSAWalker->getClobberingMemoryAccess(I);\n			if (!MSSA->isLiveOnEntryDef(DefiningAccess))\n			{\n				if (auto *MD = dyn_cast<MemoryDef>(DefiningAccess))\n				{\n					Instruction *DefiningInst = MD->getMemoryInst();\n					if (!ReachableBlocks.count(DefiningInst->getParent()))\n						return createConstantExpression(UndefValue::get(LI->getType()));\n				}\n			}\n");
		fprintf(file,"\n\t\t\tconst Expression *E = createLoadExpression(LI->getType(), LI->getPointerOperand(), LI, lookupMemoryAccessEquiv(DefiningAccess), B);\n			return E;\n		}\n\n		const Expression *performSymbolicCallEvaluation(Instruction *I, const BasicBlock *B)\n		{\n			auto *CI = cast<CallInst>(I);\n			if (AA->doesNotAccessMemory(CI))\n				return createCallExpression(CI, nullptr, B);\n			if (AA->onlyReadsMemory(CI))\n			{\n				MemoryAccess *DefiningAccess = MSSAWalker->getClobberingMemoryAccess(CI);\n				return createCallExpression(CI, lookupMemoryAccessEquiv(DefiningAccess), B);\n			}\n			return nullptr;\n		}\n		\n		const Expression *performSymbolicPHIEvaluation(Instruction *I, const BasicBlock *B)\n		{\n			auto *E = cast<PHIExpression>(createPHIExpression(I));\n			bool HasUndef = false;\n			auto Filtered = make_filter_range(E->operands(), [&](const Value *Arg) { if (Arg == I) return false; if (isa<UndefValue>(Arg)) { HasUndef = true; return false; } return true; });\n\n			if (Filtered.begin() == Filtered.end()) 	// if no operands, then undef\n			{\n				E->deallocateOperands(ArgRecycler);\n				ExpressionAllocator.Deallocate(E);\n				return createConstantExpression(UndefValue::get(I->getType()));\n			}\n			\n			Value *AllSameValue = *(Filtered.begin());\n			++Filtered.begin();\n\n			if (llvm::all_of(Filtered, [AllSameValue](const Value *V) { return V == AllSameValue; }))\n			{\n				if (HasUndef)\n				{\n					if (auto *AllSameInst = dyn_cast<Instruction>(AllSameValue))	// check for instructions\n						if (!DT->dominates(AllSameInst, I))\n							return E;\n				}\n				\n				E->deallocateOperands(ArgRecycler);\n				ExpressionAllocator.Deallocate(E);\n				if (auto *C = dyn_cast<Constant>(AllSameValue))\n					return createConstantExpression(C);\n				return createVariableExpression(AllSameValue);\n			}\n			return E;\n		}\n\n		const Expression *performSymbolicAggrValueEvaluation(Instruction *I, const BasicBlock *B)\n		{\n			if (auto *EI = dyn_cast<ExtractValueInst>(I))\n			{\n				auto *II = dyn_cast<IntrinsicInst>(EI->getAggregateOperand());\n				if (II && EI->getNumIndices() == 1 && *EI->idx_begin() == 0)\n				{\n					unsigned Opcode = 0;\n					switch (II->getIntrinsicID())\n					{\n						case Intrinsic::sadd_with_overflow:\n						case Intrinsic::uadd_with_overflow:	Opcode = Instruction::Add;\n															break;\n						case Intrinsic::ssub_with_overflow:\n						case Intrinsic::usub_with_overflow:	Opcode = Instruction::Sub;\n															break;\n						case Intrinsic::smul_with_overflow:\n						case Intrinsic::umul_with_overflow:	Opcode = Instruction::Mul;\n															break;\n						default:	break;\n					}\n\n					if (Opcode != 0)	// intrinsic\n					{\n						assert(II->getNumArgOperands() == 2 && \"Expect two args for recognised intrinsics.\");\n						return createBinaryExpression(Opcode, EI->getType(), II->getArgOperand(0), II->getArgOperand(1), B);\n					}\n				}\n			}\n			return createAggregateValueExpression(I, B);\n		}\n		\n		const Expression *performSymbolicEvaluation(Value *V, const BasicBlock *B)\n		{\n			const Expression *E = nullptr;\n");
		fprintf(file,"			if (auto *C = dyn_cast<Constant>(V))\n				E = createConstantExpression(C);\n			else if (isa<Argument>(V) || isa<GlobalVariable>(V))\n				E = createVariableExpression(V);\n			else\n			{\n				auto *I = cast<Instruction>(V);\n				switch (I->getOpcode())\n				{\n					case Instruction::ExtractValue:\n					case Instruction::InsertValue:  E = performSymbolicAggrValueEvaluation(I, B); break;\n					case Instruction::PHI: E = performSymbolicPHIEvaluation(I, B); break;\n					case Instruction::Call: E = performSymbolicCallEvaluation(I, B); break;\n					case Instruction::Store: E = performSymbolicStoreEvaluation(I, B); break;\n					case Instruction::Load: E = performSymbolicLoadEvaluation(I, B); break;\n					case Instruction::BitCast: { E = createExpression(I, B); } break;\n					case Instruction::Add: \n					case Instruction::FAdd:\n					case Instruction::Sub: \n					case Instruction::FSub:\n					case Instruction::Mul: \n					case Instruction::FMul:\n					case Instruction::UDiv: \n					case Instruction::SDiv: \n					case Instruction::FDiv:\n					case Instruction::URem: \n					case Instruction::SRem: \n					case Instruction::FRem:\n					case Instruction::Shl: \n					case Instruction::LShr: \n					case Instruction::AShr:\n					case Instruction::And: \n					case Instruction::Or: \n					case Instruction::Xor:\n					case Instruction::ICmp: \n					case Instruction::FCmp:\n					case Instruction::Trunc: \n					case Instruction::ZExt: \n					case Instruction::SExt:\n					case Instruction::FPToUI: \n					case Instruction::FPToSI: \n					case Instruction::UIToFP: \n					case Instruction::SIToFP:\n					case Instruction::FPTrunc: \n					case Instruction::FPExt:\n					case Instruction::PtrToInt: \n					case Instruction::IntToPtr:\n					case Instruction::Select:\n					case Instruction::ExtractElement: \n					case Instruction::InsertElement: \n					case Instruction::ShuffleVector:\n					case Instruction::GetElementPtr: E = createExpression(I, B); break;\n					default: return nullptr;\n				}\n			}\n			return E;\n		}\n");
		fprintf(file,"\n\t\tvoid markUsersTouched(Value *V)\n		{\n			for (auto *User : V->users())\n			{\n				assert(isa<Instruction>(User) && \"Use of value not within an instruction?\");\n				TouchedInstructions.set(InstrDFS[User]);\n			}\n		}\n\n		void markLeaderChangeTouched(CongruenceClass *CC)\n		{\n			for (auto M : CC->Members)\n			{\n				if (auto *I = dyn_cast<Instruction>(M))\n					TouchedInstructions.set(InstrDFS[I]);\n				LeaderChanges.insert(M);\n			}\n		}\n\n		void moveValueToNewCongruenceClass(Instruction *I, CongruenceClass *OldClass,CongruenceClass *NewClass)\n		{\n			if (I == OldClass->NextLeader.first)\n				OldClass->NextLeader = {nullptr, ~0U};\n\n			if (isa<Instruction>(NewClass->RepLeader) && NewClass->RepLeader && I != NewClass->RepLeader && DT->properlyDominates(I->getParent(), cast<Instruction>(NewClass->RepLeader)->getParent()))\n				assert(!isa<PHINode>(I) && \"New class for instruction should not be dominated by instruction\");\n\n			if (NewClass->RepLeader != I)\n			{\n				auto DFSNum = InstrDFS.lookup(I);\n				if (DFSNum < NewClass->NextLeader.second)\n					NewClass->NextLeader = {I, DFSNum};\n			}\n\n			OldClass->Members.erase(I);\n			NewClass->Members.insert(I);\n			if (isa<StoreInst>(I))\n			{\n				--OldClass->StoreCount;\n				assert(OldClass->StoreCount >= 0);\n				++NewClass->StoreCount;\n				assert(NewClass->StoreCount > 0);\n			}\n\n			ValueToClass[I] = NewClass;\n			if (OldClass->Members.empty() && OldClass != InitialClass) 	// see if class is destroyed or need to swap leaders\n			{\n				if (OldClass->DefiningExpr)\n				{\n					OldClass->Dead = true;\n					ExpressionToClass.erase(OldClass->DefiningExpr);\n				}\n			}\n			else if (OldClass->RepLeader == I)\n			{\n				if (OldClass->Members.size() == 1 || OldClass == InitialClass)\n					OldClass->RepLeader = *(OldClass->Members.begin());\n				else if (OldClass->NextLeader.first)\n				{\n					OldClass->RepLeader = OldClass->NextLeader.first;\n					OldClass->NextLeader = {nullptr, ~0U};\n				}\n				else\n				{\n					std::pair<Value *, unsigned> MinDFS = {nullptr, ~0U};\n					for (const auto X : OldClass->Members)\n					{\n						auto DFSNum = InstrDFS.lookup(X);\n						if (DFSNum < MinDFS.second)\n						MinDFS = {X, DFSNum};\n					}\n					OldClass->RepLeader = MinDFS.first;\n				}\n				markLeaderChangeTouched(OldClass);\n			}\n		}\n\n");
		fprintf(file,"		void performCongruenceFinding(Instruction *I, const Expression *E)\n		{\n			ValueToExpression[I] = E;\n			CongruenceClass *IClass = ValueToClass[I];\n			assert(IClass && \"Should have found a IClass\");\n			assert(!IClass->Dead && \"Found a dead class\");\n\n			CongruenceClass *EClass;\n			if (const auto *VE = dyn_cast<VariableExpression>(E))\n			{\n				EClass = ValueToClass[VE->getVariableValue()];\n			}\n			else\n			{\n				auto lookupResult = ExpressionToClass.insert({E, nullptr});\n\n				if (lookupResult.second) 				// if not in value table, create new congruence class\n				{\n					CongruenceClass *NewClass = createCongruenceClass(nullptr, E);\n					auto place = lookupResult.first;\n					place->second = NewClass;\n\n					if (const auto *CE = dyn_cast<ConstantExpression>(E)) 	// constants and variables should always be made leader \n						NewClass->RepLeader = CE->getConstantValue();\n					else if (const auto *SE = dyn_cast<StoreExpression>(E)) \n					{\n						StoreInst *SI = SE->getStoreInst();\n						NewClass->RepLeader = lookupOperandLeader(SI->getValueOperand(), SI, SI->getParent());\n					} \n					else \n						NewClass->RepLeader = I;\n					assert(!isa<VariableExpression>(E) && \"VariableExpression should have been handled already\");\n\n					EClass = NewClass;\n				}\n				else\n				{\n					EClass = lookupResult.first->second;\n					if (isa<ConstantExpression>(E))\n						assert(isa<Constant>(EClass->RepLeader) && \"Any class with a constant expression should have a constant leader\");\n					assert(EClass && \"Somehow don't have an eclass\");\n					assert(!EClass->Dead && \"We accidentally looked up a dead class\");\n				}\n			}\n			\n			bool ClassChanged = IClass != EClass;\n			bool LeaderChanged = LeaderChanges.erase(I);\n			if (ClassChanged || LeaderChanged)\n			{\n				if (ClassChanged)\n					moveValueToNewCongruenceClass(I, IClass, EClass);\n				markUsersTouched(I);\n				if (MemoryAccess *MA = MSSA->getMemoryAccess(I))\n				{\n					if (!isa<MemoryUse>(MA) && isa<StoreExpression>(E) && EClass->Members.size() != 1)\n					{\n						auto *DefAccess = cast<StoreExpression>(E)->getDefiningAccess();\n						setMemoryAccessEquivTo(MA, DefAccess != MA ? DefAccess : nullptr);\n					}\n					else\n						setMemoryAccessEquivTo(MA, nullptr);\n					markMemoryUsersTouched(MA);\n				}\n			}\n			else if (auto *SI = dyn_cast<StoreInst>(I))\n			{\n				auto ProperLeader = lookupOperandLeader(SI->getValueOperand(), SI, SI->getParent());\n				if (EClass->RepLeader != ProperLeader)\n				{\n					EClass->RepLeader = ProperLeader;\n					markLeaderChangeTouched(EClass);\n					markMemoryUsersTouched(MSSA->getMemoryAccess(SI));\n				}\n			}\n		}\n		\n		void updateReachableEdge(BasicBlock *From, BasicBlock *To)\n		{\n			if (ReachableEdges.insert({From, To}).second) 	// check if Edge was reachable\n			{\n				if (ReachableBlocks.insert(To).second)		// if block wasn't reachable before, all instructions are touched\n				{\n					const auto &InstRange = BlockInstRange.lookup(To);\n					TouchedInstructions.set(InstRange.first, InstRange.second);\n				}\n				else\n				{\n					if (MemoryAccess *MemPhi = MSSA->getMemoryAccess(To))\n						TouchedInstructions.set(InstrDFS[MemPhi]);\n");
		fprintf(file,"					auto BI = To->begin();\n					while (isa<PHINode>(BI))\n					{\n						TouchedInstructions.set(InstrDFS[&*BI]);\n						++BI;\n					}\n				}\n			}\n		}\n		");
		fprintf(file,"\n		Value *findConditionEquivalence(Value *Cond, BasicBlock *B) const 	// given predicate condition and block, find constant value\n		{\n			auto Result = lookupOperandLeader(Cond, nullptr, B);\n			if (isa<Constant>(Result))\n				return Result;\n			return nullptr;\n		}	\n\n		void processOutgoingEdges(TerminatorInst *TI, BasicBlock *B)	// process outgoing edges of block for reachability\n		{\n			BranchInst *BR;				// get reachability of terminator instruction\n			if ((BR = dyn_cast<BranchInst>(TI)) && BR->isConditional())\n			{\n				Value *Cond = BR->getCondition();\n				Value *CondEvaluated = findConditionEquivalence(Cond, B);\n				if (!CondEvaluated)\n				{\n					if (auto *I = dyn_cast<Instruction>(Cond))\n					{\n						const Expression *E = createExpression(I, B);\n						if (const auto *CE = dyn_cast<ConstantExpression>(E))\n							CondEvaluated = CE->getConstantValue();\n					}\n					else if (isa<ConstantInt>(Cond))\n						CondEvaluated = Cond;\n				}\n				ConstantInt *CI;\n				BasicBlock *TrueSucc = BR->getSuccessor(0);\n				BasicBlock *FalseSucc = BR->getSuccessor(1);\n				if (CondEvaluated && (CI = dyn_cast<ConstantInt>(CondEvaluated)))\n				{\n					if (CI->isOne())\n						updateReachableEdge(B, TrueSucc);\n					else if (CI->isZero())\n						updateReachableEdge(B, FalseSucc);\n				}\n				else \n				{\n					updateReachableEdge(B, TrueSucc);\n					updateReachableEdge(B, FalseSucc);\n				}\n			}\n			else if (auto *SI = dyn_cast<SwitchInst>(TI))\n			{\n				SmallDenseMap<BasicBlock *, unsigned, 16> SwitchEdges;\n				Value *SwitchCond = SI->getCondition();\n				Value *CondEvaluated = findConditionEquivalence(SwitchCond, B);\n\n				if (CondEvaluated && isa<ConstantInt>(CondEvaluated)) 	// if switch statement could be turned into constant\n				{\n					auto *CondVal = cast<ConstantInt>(CondEvaluated); \n					auto CaseVal = SI->findCaseValue(CondVal);					// case value for value\n					if (CaseVal.getCaseSuccessor() == SI->getDefaultDest())\n					{\n						updateReachableEdge(B, SI->getDefaultDest());\n						return;\n					}\n					BasicBlock *TargetBlock = CaseVal.getCaseSuccessor(); 	// get successor and mark it reachable\n					updateReachableEdge(B, TargetBlock);\n				}\n				else\n				{\n					for (unsigned i = 0, e = SI->getNumSuccessors(); i != e; ++i)\n					{\n						BasicBlock *TargetBlock = SI->getSuccessor(i);\n						++SwitchEdges[TargetBlock];\n						updateReachableEdge(B, TargetBlock);\n					}\n				}\n			}\n			else\n			{\n				for (unsigned i = 0, e = TI->getNumSuccessors(); i != e; ++i) // mark successors as reachable\n				{\n					BasicBlock *TargetBlock = TI->getSuccessor(i);\n					updateReachableEdge(B, TargetBlock);\n				}\n				if (MemoryAccess *MA = MSSA->getMemoryAccess(TI))\n					setMemoryAccessEquivTo(MA, nullptr);\n			}\n		}\n		\n		void valueNumberInstruction(Instruction *I) // value number single instruction, evaluate, find congruence, update mappings\n");
		fprintf(file,"		{\n			if (isInstructionTriviallyDead(I, TLI))\n			{\n				InstructionsToErase.insert(I);\n				return;\n			}\n			if (!I->isTerminator())\n			{\n				const auto *Symbolized = performSymbolicEvaluation(I, I->getParent()); 	// if no symbolic expr, use unknown expr\n				if (Symbolized == nullptr)\n					Symbolized = createUnknownExpression(I);\n				performCongruenceFinding(I, Symbolized);\n			}\n			else\n			{\n				if (!I->getType()->isVoidTy()) // handle terminators that return values\n				{\n					auto *Symbolized = createUnknownExpression(I);\n					performCongruenceFinding(I, Symbolized);\n				}\n				processOutgoingEdges(dyn_cast<TerminatorInst>(I), I->getParent());\n			}\n		}\n		\n		void valueNumberMemoryPhi(MemoryPhi *MP)\n		{\n			auto Filtered = make_filter_range(MP->operands(), [&](const Use &U) { return ReachableBlocks.count(MP->getIncomingBlock(U));});\n\n			assert(Filtered.begin() != Filtered.end() && \"We should not be processing a MemoryPhi in a completely unreachable block\");\n\n			auto LookupFunc = [&](const Use &U) 	// transform the remaining operands into operand leaders\n			{\n				return lookupMemoryAccessEquiv(cast<MemoryAccess>(U));\n			};\n			auto MappedBegin = map_iterator(Filtered.begin(), LookupFunc);\n			auto MappedEnd = map_iterator(Filtered.end(), LookupFunc);\n\n			MemoryAccess *AllSameValue = *MappedBegin; 	// check if all elements are equal\n			++MappedBegin;\n			bool AllEqual = std::all_of(MappedBegin, MappedEnd, [&AllSameValue](const MemoryAccess *V) { return V == AllSameValue; });\n\n			if (setMemoryAccessEquivTo(MP, AllEqual ? AllSameValue : nullptr))\n				markMemoryUsersTouched(MP);\n		}\n		\n		static void patchReplacementInstruction(Instruction *I, Value *Repl)\n		{\n			auto *Op = dyn_cast<BinaryOperator>(I);		// patch replacement -> not more restrictive than the value being replaced\n			auto *ReplOp = dyn_cast<BinaryOperator>(Repl);\n\n			if (Op && ReplOp)\n				ReplOp->andIRFlags(Op);\n\n			if (auto *ReplInst = dyn_cast<Instruction>(Repl))\n			{\n				unsigned KnownIDs[] = {LLVMContext::MD_tbaa, LLVMContext::MD_alias_scope, LLVMContext::MD_noalias, LLVMContext::MD_range, LLVMContext::MD_fpmath, LLVMContext::MD_invariant_load, LLVMContext::MD_invariant_group};\n				combineMetadata(ReplInst, I, KnownIDs);\n			}\n		}\n		\n		void convertDenseToDFSOrdered(CongruenceClass::MemberSet &Dense, SmallVectorImpl<ValueDFS> &DFSOrderedSet)\n		{\n			for (auto D : Dense)\n			{\n				BasicBlock *BB = getBlockForValue(D);		// add the value\n				assert(BB && \"Should have figured out a basic block for value\");\n				ValueDFS VD;\n\n				std::pair<int, int> DFSPair = DFSDomMap[BB];\n				assert(DFSPair.first != -1 && DFSPair.second != -1 && \"Invalid DFS Pair\");\n				VD.DFSIn = DFSPair.first;\n				VD.DFSOut = DFSPair.second;\n				VD.Val = D;\n\n				if (auto *I = dyn_cast<Instruction>(D))\n					VD.LocalNum = InstrDFS[I];\n				else\n					llvm_unreachable(\"Should have been an instruction\");\n\n				DFSOrderedSet.emplace_back(VD);\n\n				for (auto &U : D->uses())		// add the users\n				{\n					if (auto *I = dyn_cast<Instruction>(U.getUser()))\n					{\n						ValueDFS VD;\n						BasicBlock *IBlock;\n						if (auto *P = dyn_cast<PHINode>(I))\n						{\n							IBlock = P->getIncomingBlock(U);\n							VD.LocalNum = InstrDFS.size() + 1;\n						}\n						else\n						{\n							IBlock = I->getParent();\n							VD.LocalNum = InstrDFS[I];\n						}\n						std::pair<int, int> DFSPair = DFSDomMap[IBlock];\n						VD.DFSIn = DFSPair.first;\n						VD.DFSOut = DFSPair.second;\n						VD.U = &U;\n						DFSOrderedSet.emplace_back(VD);\n					}\n				}\n			}\n		}\n	");
		fprintf(file, "\n\t\tbool eliminateInstructions(Function &F)\n		{\n			bool AnythingReplaced = false;\n\n			DT->updateDFSNumbers();\n\n			for (auto &B : F)\n			{\n				if (!ReachableBlocks.count(&B))\n				{\n					for (const auto S : successors(&B))\n					{\n						for (auto II = S->begin(); isa<PHINode>(II); ++II)\n						{\n							auto &Phi = cast<PHINode>(*II);\n							for (auto &Operand : Phi.incoming_values())\n								if (Phi.getIncomingBlock(Operand) == &B)\n									Operand.set(UndefValue::get(Phi.getType()));\n						}\n					}\n				}\n				DomTreeNode *Node = DT->getNode(&B);\n				if (Node)\n					DFSDomMap[&B] = {Node->getDFSNumIn(), Node->getDFSNumOut()};\n			}\n\n			for (CongruenceClass *CC : CongruenceClasses)\n			{\n				if (CC == InitialClass || CC->Dead)\n					continue;\n				assert(CC->RepLeader && \"We should have had a leader\");\n\n				if (alwaysAvailable(CC->RepLeader))\n				{\n					SmallPtrSet<Value *, 4> MembersLeft;\n					for (auto M : CC->Members)\n					{\n						Value *Member = M;\n\n						if (Member == CC->RepLeader || Member->getType()->isVoidTy())	// no uses to be replaced for void\n						{\n							MembersLeft.insert(Member);\n							continue;\n						}\n						if (auto *I = dyn_cast<Instruction>(Member))\n						{\n							assert(CC->RepLeader != I && \"About to accidentally remove our leader\");\n							patchReplacementInstruction(I, CC->RepLeader);\n  							I->replaceAllUsesWith(CC->RepLeader);\n							InstructionsToErase.insert(I);\n							AnythingReplaced = true;\n							continue;\n						}\n");
		fprintf(file,"						else\n							MembersLeft.insert(I);\n					}\n					CC->Members.swap(MembersLeft);\n				}\n				else\n				{\n					if (CC->Members.size() != 1)		// skip singleton\n					{\n						ValueDFSStack EliminationStack;\n\n						SmallVector<ValueDFS, 8> DFSOrderedSet;		// convert members to DFS ordered sets, merge them.\n						convertDenseToDFSOrdered(CC->Members, DFSOrderedSet);\n\n						std::sort(DFSOrderedSet.begin(), DFSOrderedSet.end());\n						for (auto &VD : DFSOrderedSet)\n						{\n							int MemberDFSIn = VD.DFSIn;\n							int MemberDFSOut = VD.DFSOut;\n							Value *Member = VD.Val;\n							Use *MemberUse = VD.U;\n\n							if (Member)\n							{\n								if (Member->getType()->isVoidTy())\n									continue;\n							}\n\n							bool ShouldPush = Member && (EliminationStack.empty() || isa<Constant>(Member));\n							bool OutOfScope = !EliminationStack.isInScope(MemberDFSIn, MemberDFSOut);\n\n							if (OutOfScope || ShouldPush)\n							{\n								EliminationStack.popUntilDFSScope(MemberDFSIn, MemberDFSOut);	// sync to current scope\n								ShouldPush |= Member && EliminationStack.empty();\n								if (ShouldPush)\n									EliminationStack.push_back(Member, MemberDFSIn, MemberDFSOut);\n							}\n\n							if (EliminationStack.empty())\n								continue;\n								\n							if (Member)\n								continue;\n							Value *Result = EliminationStack.back();\n\n							if (MemberUse->get() == Result)\n								continue;\n							\n							if (auto *ReplacedInst = dyn_cast<Instruction>(MemberUse->get()))\n								patchReplacementInstruction(ReplacedInst, Result);\n\n							assert(isa<Instruction>(MemberUse->getUser()));\n							MemberUse->set(Result);\n							AnythingReplaced = true;\n						}\n					}\n				}\n\n				SmallPtrSet<Value *, 4> MembersLeft;\n				for (Value *Member : CC->Members)\n				{\n					if (Member->getType()->isVoidTy())\n					{\n						MembersLeft.insert(Member);\n						continue;\n					}\n					if (auto *MemberInst = dyn_cast<Instruction>(Member))\n");
		fprintf(file,"					{\n						if (isInstructionTriviallyDead(MemberInst))\n						{\n							InstructionsToErase.insert(MemberInst);\n							continue;\n						}\n					}\n					MembersLeft.insert(Member);\n				}\n				CC->Members.swap(MembersLeft);\n			}\n\n			return AnythingReplaced;\n		}\n\n		void deleteInstructionsInBlock(BasicBlock *BB)\n		{\n			if (isa<TerminatorInst>(BB->begin()))	// check if there are non-terminating instructions to be deleted\n				return;\n\n			auto StartPoint = BB->rbegin();		// delete instructions backwards\n			++StartPoint;\n			// Note that we explicitly recalculate BB->rend() on each iteration,\n			// as it may change when we remove the first instruction.\n			for (BasicBlock::reverse_iterator I(StartPoint); I != BB->rend();)\n			{\n				Instruction &Inst = *I++;\n				if (!Inst.use_empty())\n					Inst.replaceAllUsesWith(UndefValue::get(Inst.getType()));\n				if (isa<LandingPadInst>(Inst))\n					continue;\n				Inst.eraseFromParent();\n			}\n		}\n\n		void cleanupTables()\n		{\n			for (unsigned i = 0, e = CongruenceClasses.size(); i != e; ++i)\n			{\n				delete CongruenceClasses[i];\n				CongruenceClasses[i] = nullptr;\n			}\n\n			ValueToClass.clear();\n			ArgRecycler.clear(ExpressionAllocator);\n			ExpressionAllocator.Reset();\n			CongruenceClasses.clear();\n			ExpressionToClass.clear();\n			ValueToExpression.clear();\n			ReachableBlocks.clear();\n			ReachableEdges.clear();\n			#ifndef NDEBUG\n				ProcessedCount.clear();\n			#endif\n			InstructionsToErase.clear();\n			DFSDomMap.clear();\n			InstrDFS.clear();\n			DFSToInstr.clear();\n			BlockInstRange.clear();\n			TouchedInstructions.clear();\n			DominatedInstRange.clear();\n			MemoryAccessEquiv.clear();\n		}\n");
		
		fprintf(file,"\n\t\tprivate:\n\t\tMemorySSA *MSSA;\n		const TargetLibraryInfo *TLI;\n		DominatorTree *DT;\n		const DataLayout *DL;\n		AssumptionCache *AC;\n		AliasAnalysis *AA;\n		MemorySSAWalker *MSSAWalker;\n		BumpPtrAllocator ExpressionAllocator;\n		ArrayRecycler<Value *> ArgRecycler;\n		\n		SmallPtrSet<Value *, 8> LeaderChanges;	// which values have changed as a result of leader changes\n			\n		DenseMap<const BasicBlock *, std::pair<int, int>> DFSDomMap; // DFS info\n		DenseMap<const Value *, unsigned> InstrDFS;\n		SmallVector<Value *, 32> DFSToInstr;\n		DenseMap<const BasicBlock *, std::pair<unsigned, unsigned>> BlockInstRange;\n		DenseMap<const DomTreeNode *, std::pair<unsigned, unsigned>> DominatedInstRange;\n		\n		SmallPtrSet<Instruction *, 8> InstructionsToErase;\n		\n		#ifndef NDEBUG\n			DenseMap<const Value *, unsigned> ProcessedCount; // how many times each block and instruction gets processed\n		#endif\n		\n		using BlockEdge = BasicBlockEdge; 	// reachability info\n		DenseSet<BlockEdge> ReachableEdges;\n		SmallPtrSet<const BasicBlock *, 8> ReachableBlocks;\n		\n		CongruenceClass *InitialClass;		// Congruence class info\n		std::vector<CongruenceClass *> CongruenceClasses;\n		unsigned NextCongruenceNum;\n\n		DenseMap<Value *, CongruenceClass *> ValueToClass; // value mappings\n		DenseMap<Value *, const Expression *> ValueToExpression;\n\n		BitVector TouchedInstructions;\n\n		DenseMap<const MemoryAccess *, MemoryAccess *> MemoryAccessEquiv; // table stores which memorydefs/phis represent memory state equivalent to another memory state\n\n		using ExpressionClassMap = DenseMap<const Expression *, CongruenceClass *>; // typedef\n		ExpressionClassMap ExpressionToClass;	// map Expression to class\n\n\t};\n");
		
		fprintf(file,"\n}\n\nchar GlobalVN::ID = 0;			// these two lines are always added to carry out the pass\nstatic RegisterPass<GlobalVN> temp(\"globalvn\",\"- to do global value numbering\");");
	}
	else if (strcmp(optnode->llnode->con.alpha,"LoopUnrolling") == 0)
	{
		fprintf(file,"#include \"llvm/Pass.h\"\n#include \"llvm/IR/Function.h\"\n#include \"llvm/IR/Type.h\"\n#include \"llvm/IR/Instruction.h\"\n#include \"llvm/IR/Metadata.h\"\n#include \"llvm/Analysis/LoopPass.h\"\n#include \"llvm/Analysis/LoopUnrollAnalyzer.h\"\n#include \"llvm/Analysis/TargetTransformInfo.h\"\n#include \"llvm/Analysis/AssumptionCache.h\"\n#include \"llvm/Analysis/ScalarEvolution.h\"\n#include \"llvm/Analysis/OptimizationDiagnosticInfo.h\"\n#include \"llvm/Analysis/CodeMetrics.h\"\n#include \"llvm/ADT/StringRef.h\"\n#include \"llvm/Transforms/Utils/Local.h\"\n#include \"llvm/Transforms/Utils/LoopUtils.h\"\n#include \"llvm/Transforms/Utils/UnrollLoop.h\"\n#include \"llvm/Transforms/Scalar.h\"\n#include \"llvm/Support/raw_ostream.h\"\n#include <cstring>\n\nusing namespace llvm;\nusing namespace std;\nusing namespace ore;\n\nstatic const unsigned NoThreshold = UINT_MAX;\nunsigned PragmaUnrollThreshold = 16*1024;\nunsigned FlatLoopTripCountThreshold = 5;\nunsigned UnrollMaxIterationsCountToAnalyze = 10;\n\nnamespace\n{\n");
		fprintf(file,"\tstruct EstimatedUnrollCost\n	{\n		unsigned UnrolledCost; // estimated cost after unrolling\n		unsigned RolledDynamicCost; // estimated dynamic cost of executing the instructions in the rolled form\n	};\n	struct UnrolledInstState\n	{\n		Instruction *I;\n		int Iteration : 30;\n		unsigned IsFree : 1;\n		unsigned IsCounted : 1;\n	};\n	struct UnrolledInstStateKeyInfo\n	{\n		typedef DenseMapInfo<Instruction *> PtrInfo;\n		typedef DenseMapInfo<std::pair<Instruction *, int>> PairInfo;\n		static inline UnrolledInstState getEmptyKey()\n		{\n			return {PtrInfo::getEmptyKey(), 0, 0, 0};\n		}\n		static inline UnrolledInstState getTombstoneKey()\n		{\n			return {PtrInfo::getTombstoneKey(), 0, 0, 0};\n		}\n		static inline unsigned getHashValue(const UnrolledInstState &S)\n		{\n			return PairInfo::getHashValue({S.I, S.Iteration});\n		}\n		static inline bool isEqual(const UnrolledInstState &LHS, const UnrolledInstState &RHS)\n		{\n			return PairInfo::isEqual({LHS.I, LHS.Iteration}, {RHS.I, RHS.Iteration});\n		}\n	};\n	class LoopUnrolling : public ");
		
		if (strcmp(optnode->next->llnode->con.alpha,"runOn") != 0)
			printerror("runOn not declared.\n");
		else if (strcmp(runoniter->llnode->con.alpha,"loop") == 0)
		{
			processRunOnNode("Loop", "LoopUnrolling");
		}
		else
			printerror(strcat(runoniter->llnode->con.alpha," cannot be run on for loop unrolling.\n"));
			
		curr = runoniter->next;
		while (curr!=NULL)
		{
			if (strcmp(curr->llnode->con.alpha,"unrollLoop")==0)
			{
				curr = curr->next;
				if (strcmp(curr->llnode->con.alpha,"loop") != 0) 
					printerror("loop needs to be the first parameter to unrollLoop.");
					
				fprintf(file, "\n\t\t\tif (skipLoop(L))\n				return false;\n\n			Function &F = *L->getHeader()->getParent();\n\n			auto &DT = getAnalysis<DominatorTreeWrapperPass>().getDomTree();\n			LoopInfo *LI = &getAnalysis<LoopInfoWrapperPass>().getLoopInfo();\n			ScalarEvolution *SE = &getAnalysis<ScalarEvolutionWrapperPass>().getSE();\n			const TargetTransformInfo &TTI = getAnalysis<TargetTransformInfoWrapperPass>().getTTI(F);\n			auto &AC = getAnalysis<AssumptionCacheTracker>().getAssumptionCache(F);\n			OptimizationRemarkEmitter ORE(&F);\n			bool PreserveLCSSA = mustPreserveAnalysisID(LCSSAID);");
				
				processUnrollLoopParams(curr->next);
				
				fprintf(file, "\n\t\t\treturn tryToUnrollLoop(L, DT, LI, SE, TTI, AC, ORE, PreserveLCSSA);");
			}
			curr = curr->next;
		}
		fprintf(file, "\n\t\t}\n\nint countNoOfInstrInLoop(Loop *L)\n		{\n			int count = 0;\n			for(Loop::block_iterator block = L->block_begin(), y = L->block_end(); block!= y; ++block)\n			{\n				for (BasicBlock::iterator i = (*block)->begin(), e = (*block)->end(); i!=e; ++i)\n					count++;\n			}\n			return count;\n		}");
		
		fprintf(file,"\n\t\tvoid getAnalysisUsage(AnalysisUsage& AU) const\n		{\n			AU.addRequired<AssumptionCacheTracker>();\n			AU.addRequired<TargetTransformInfoWrapperPass>();\n			AU.addRequiredID(LCSSAID);\n			getLoopAnalysisUsage(AU);\n		}\n		\n		static Optional<EstimatedUnrollCost> analyzeLoopUnrollCost(const Loop *L, unsigned TripCount, DominatorTree &DT, ScalarEvolution &SE, const TargetTransformInfo &TTI, unsigned MaxUnrolledLoopSize)\n		{\n			assert(UnrollMaxIterationsCountToAnalyze < (INT_MAX / 2) && \"The unroll iterations max is too large!\");\n			\n			if (!L->empty())\n				return None;\n				\n			if (!UnrollMaxIterationsCountToAnalyze || !TripCount || TripCount > UnrollMaxIterationsCountToAnalyze)\n				return None; // loops with a big or unknown trip count\n\n			SmallSetVector<BasicBlock *, 16> BBWorklist;\n			SmallSetVector<std::pair<BasicBlock *, BasicBlock *>, 4> ExitWorklist;\n			DenseMap<Value *, Constant *> SimplifiedValues;\n			SmallVector<std::pair<Value *, Constant *>, 4> SimplifiedInputValues;\n			\n			unsigned UnrolledCost = 0;\n			unsigned RolledDynamicCost = 0;\n			\n			DenseSet<UnrolledInstState, UnrolledInstStateKeyInfo> InstCostMap;\n			SmallVector<Instruction *, 16> CostWorklist;\n			SmallVector<Instruction *, 4> PHIUsedList;\n\n			auto AddCostRecursively = [&](Instruction &RootI, int Iteration) // function to accumulate cost for instructions in the loop\n			{\n				assert(Iteration >= 0 && \"Cannot have a negative iteration!\");\n				assert(CostWorklist.empty() && \"Must start with an empty cost list\");\n				assert(PHIUsedList.empty() && \"Must start with an empty phi used list\");\n				CostWorklist.push_back(&RootI);\n				for (;; --Iteration)\n				{\n					do \n					{\n						Instruction *I = CostWorklist.pop_back_val();\n						auto CostIter = InstCostMap.find({I, Iteration, 0, 0});\n						if (CostIter == InstCostMap.end())\n							continue;\n						auto &Cost = *CostIter;\n						if (Cost.IsCounted)\n							continue;\n							\n						Cost.IsCounted = true; // counting the cost of this instruction now\n\n						if (auto *PhiI = dyn_cast<PHINode>(I)) // if there is PHI node in the loop header, add to PHI set\n							if (PhiI->getParent() == L->getHeader())\n							{\n								assert(Cost.IsFree && \"Loop PHIs shouldn't be evaluated as they inherently simplify during unrolling.\");\n								if (Iteration == 0)\n									continue;\n\n								if (auto *OpI = dyn_cast<Instruction>(PhiI->getIncomingValueForBlock(L->getLoopLatch())))\n									if (L->contains(OpI))\n										PHIUsedList.push_back(OpI);\n								continue;\n							}\n\n						if (!Cost.IsFree) // accumulate the cost of this instruction\n							UnrolledCost += TTI.getUserCost(I);\n							\n						for (Value *Op : I->operands())\n						{\n							auto *OpI = dyn_cast<Instruction>(Op); // check if operand is free due to being a constant or outside the loop\n							if (!OpI || !L->contains(OpI))\n								continue;\n							CostWorklist.push_back(OpI); // accumulate its cost\n						}\n					} while (!CostWorklist.empty());\n\n					if (PHIUsedList.empty()) // search exhausted\n						break;\n\n					assert(Iteration > 0 && \"Cannot track PHI-used values past the first iteration!\");\n					CostWorklist.append(PHIUsedList.begin(), PHIUsedList.end());\n					PHIUsedList.clear();\n				}\n			};\n			\n			assert(L->isLoopSimplifyForm() && \"Must put loop into normal form first.\");\n			assert(L->isLCSSAForm(DT) && \"Must have loops in LCSSA form to track live-out values.\");\n");
		fprintf(file,"\n\t\t\tfor (unsigned Iteration = 0; Iteration < TripCount; ++Iteration)\n			{\n				for (Instruction &I : *L->getHeader()) // collect any simplified entry or backedge inputs\n				{\n					auto *PHI = dyn_cast<PHINode>(&I);\n					if (!PHI)\n						break;\n						\n					assert(PHI->getNumIncomingValues() == 2 && \"Must have an incoming value only for the preheader and the latch.\");\n\n					Value *V = PHI->getIncomingValueForBlock(Iteration == 0 ? L->getLoopPreheader() : L->getLoopLatch());\n					Constant *C = dyn_cast<Constant>(V);\n					if (Iteration != 0 && !C)\n						C = SimplifiedValues.lookup(V);\n					if (C)\n						SimplifiedInputValues.push_back({PHI, C});\n				}\n\n				SimplifiedValues.clear(); // clear and re-populate the map for the next iteration\n				while (!SimplifiedInputValues.empty())\n					SimplifiedValues.insert(SimplifiedInputValues.pop_back_val());\n\n				UnrolledInstAnalyzer Analyzer(Iteration, SimplifiedValues, SE, L);\n\n				BBWorklist.clear();\n				BBWorklist.insert(L->getHeader());\n				\n				for (unsigned Idx = 0; Idx != BBWorklist.size(); ++Idx)\n				{\n					BasicBlock *BB = BBWorklist[Idx];\n\n					// Visit all instructions in the given basic block and try to simplify\n					// it.  We don't change the actual IR, just count optimization\n					// opportunities.\n					for (Instruction &I : *BB)\n					{\n						if (isa<DbgInfoIntrinsic>(I))\n							continue;\n\n						RolledDynamicCost += TTI.getUserCost(&I);\n						bool IsFree = Analyzer.visit(I);\n						bool Inserted = InstCostMap.insert({&I, (int)Iteration, (unsigned)IsFree, /*IsCounted*/ false}).second;\n						(void)Inserted;\n						assert(Inserted && \"Cannot have a state for an unvisited instruction!\");\n\n						if (IsFree)\n							continue;\n						if(isa<CallInst>(&I))\n							return None;\n						if (I.mayHaveSideEffects())\n							AddCostRecursively(I, Iteration);\n						if (UnrolledCost > MaxUnrolledLoopSize)\n							return None;\n					}\n\n					TerminatorInst *TI = BB->getTerminator();\n					BasicBlock *KnownSucc = nullptr;\n					if (BranchInst *BI = dyn_cast<BranchInst>(TI))\n					{\n						if (BI->isConditional())\n						{\n							if (Constant *SimpleCond = SimplifiedValues.lookup(BI->getCondition()))\n							{\n								if (isa<UndefValue>(SimpleCond))\n									KnownSucc = BI->getSuccessor(0);\n								else if (ConstantInt *SimpleCondVal = dyn_cast<ConstantInt>(SimpleCond))\n									KnownSucc = BI->getSuccessor(SimpleCondVal->isZero() ? 1 : 0);\n							}\n						}\n					}\n					else if (SwitchInst *SI = dyn_cast<SwitchInst>(TI))\n					{\n						if (Constant *SimpleCond = SimplifiedValues.lookup(SI->getCondition()))\n						{\n							if (isa<UndefValue>(SimpleCond))\n								KnownSucc = SI->getSuccessor(0);\n							else if (ConstantInt *SimpleCondVal = dyn_cast<ConstantInt>(SimpleCond))\n								KnownSucc = SI->findCaseValue(SimpleCondVal).getCaseSuccessor();\n						}\n					}\n					if (KnownSucc)\n					{\n						if (L->contains(KnownSucc))\n							BBWorklist.insert(KnownSucc);\n						else\n							ExitWorklist.insert({BB, KnownSucc});\n						continue;\n					}\n\n					for (BasicBlock *Succ : successors(BB)) // add BB's successors to the worklist\n						if (L->contains(Succ))\n							BBWorklist.insert(Succ);\n						else\n							ExitWorklist.insert({BB, Succ});\n					AddCostRecursively(*TI, Iteration);\n				}");

				fprintf(file,"\n\t\t\t\tif (UnrolledCost == RolledDynamicCost)\n					return None;\n			}\n\n			while (!ExitWorklist.empty())\n			{\n				BasicBlock *ExitingBB, *ExitBB;\n				std::tie(ExitingBB, ExitBB) = ExitWorklist.pop_back_val();\n\n				for (Instruction &I : *ExitBB)\n				{\n					auto *PN = dyn_cast<PHINode>(&I);\n					if (!PN)\n						break;\n\n					Value *Op = PN->getIncomingValueForBlock(ExitingBB);\n					if (auto *OpI = dyn_cast<Instruction>(Op))\n						if (L->contains(OpI))\n							AddCostRecursively(*OpI, TripCount - 1);\n				}\n			}\n			return {{UnrolledCost, RolledDynamicCost}};\n		}\n		\n		static void SetLoopAlreadyUnrolled(Loop *L)\n		{\n			MDNode *LoopID = L->getLoopID(); // remove existing loop unrolling metadata\n			SmallVector<Metadata *, 4> MDs; // reserve first location for self reference to LoopID metadata node.\n			MDs.push_back(nullptr);\n\n			if (LoopID)\n			{\n				for (unsigned i = 1, ie = LoopID->getNumOperands(); i < ie; ++i)\n				{\n					bool IsUnrollMetadata = false;\n					MDNode *MD = dyn_cast<MDNode>(LoopID->getOperand(i));\n					if (MD)\n					{\n						const MDString *S = dyn_cast<MDString>(MD->getOperand(0));\n						IsUnrollMetadata = S && S->getString().startswith(\"llvm.loop.unroll.\");\n					}\n					if (!IsUnrollMetadata)\n					MDs.push_back(LoopID->getOperand(i));\n				}\n			}\n\n			LLVMContext &Context = L->getHeader()->getContext();  // add unroll(disable) metadata to disable future unrolling\n			SmallVector<Metadata *, 1> DisableOperands;\n			DisableOperands.push_back(MDString::get(Context, \"llvm.loop.unroll.disable\"));\n			MDNode *DisableNode = MDNode::get(Context, DisableOperands);\n			MDs.push_back(DisableNode);\n\n			MDNode *NewLoopID = MDNode::get(Context, MDs);\n			NewLoopID->replaceOperandWith(0, NewLoopID); // set operand 0 to refer to the loop id itself\n			L->setLoopID(NewLoopID);\n		}\n\n		static uint64_t getUnrolledLoopSize(unsigned LoopSize,TargetTransformInfo::UnrollingPreferences &UP)\n		{\n			assert(LoopSize >= UP.BEInsns && \"LoopSize should not be less than BEInsns!\");\n			return (uint64_t)(LoopSize - UP.BEInsns) * UP.Count + UP.BEInsns;\n		}\n		\n		static unsigned UnrollCountPragmaValue(const Loop *L)\n		{\n			MDNode *MD = GetUnrollMetadataForLoop(L, \"llvm.loop.unroll.count\");\n			if (MD)\n			{\n				assert(MD->getNumOperands() == 2 && \"Unroll count hint metadata should have two operands.\");\n				unsigned Count = mdconst::extract<ConstantInt>(MD->getOperand(1))->getZExtValue();\n				assert(Count >= 1 && \"Unroll count must be positive.\");\n				return Count;\n			}\n			return 0;\n		}");
				fprintf(file,"\n		static MDNode *GetUnrollMetadataForLoop(const Loop *L, StringRef Name)\n		{\n			if (MDNode *LoopID = L->getLoopID())\n				return GetUnrollMetadata(LoopID, Name);\n			return nullptr;\n		}\n\n		static unsigned getFullUnrollBoostingFactor(const EstimatedUnrollCost &Cost, unsigned MaxPercentThresholdBoost)\n		{\n			if (Cost.RolledDynamicCost >= UINT_MAX / 100)\n				return 100;\n			else if (Cost.UnrolledCost != 0)\n				return std::min(100 * Cost.RolledDynamicCost / Cost.UnrolledCost, MaxPercentThresholdBoost); // boosting factor\n			else\n				return MaxPercentThresholdBoost;\n		}\n\n\n		static bool HasUnrollFullPragma(const Loop *L) // returns true if the loop has an unroll(full) pragma\n		{\n			return GetUnrollMetadataForLoop(L, \"llvm.loop.unroll.full\");\n		}\n\n		static bool HasUnrollEnablePragma(const Loop *L) // returns true if the loop has an unroll(enable) pragma\n		{\n			return GetUnrollMetadataForLoop(L, \"llvm.loop.unroll.enable\");\n		}\n\n		// Calculates unroll count and writes it to UP.Count. Returns true if unroll count was set explicitly.\n		static bool computeUnrollCount(Loop *L, const TargetTransformInfo &TTI, DominatorTree &DT, LoopInfo *LI, ScalarEvolution *SE, OptimizationRemarkEmitter *ORE, unsigned &TripCount,unsigned MaxTripCount, unsigned &TripMultiple, unsigned LoopSize, TargetTransformInfo::UnrollingPreferences &UP, bool &UseUpperBound)\n		{\n			unsigned PragmaCount = UnrollCountPragmaValue(L); // 1st priority is unroll count set by pragma\n			if (PragmaCount > 0)\n			{\n				UP.Count = PragmaCount;\n				UP.Runtime = true;\n				UP.AllowExpensiveTripCount = true;\n				UP.Force = true;\n				if (UP.AllowRemainder && getUnrolledLoopSize(LoopSize, UP)<PragmaUnrollThreshold)\n					return true;\n			}\n	\n			bool PragmaFullUnroll = HasUnrollFullPragma(L);\n			if (PragmaFullUnroll && TripCount != 0)\n			{\n				UP.Count = TripCount;\n				if (getUnrolledLoopSize(LoopSize, UP)<PragmaUnrollThreshold)\n					return false;\n			}");
				fprintf(file,"\n\n			bool PragmaEnableUnroll = HasUnrollEnablePragma(L);\n			bool ExplicitUnroll = PragmaCount > 0 || PragmaFullUnroll || PragmaEnableUnroll;\n\n			if (ExplicitUnroll && TripCount != 0)\n			{\n				UP.Threshold = std::max<unsigned>(UP.Threshold, PragmaUnrollThreshold); // If there is unrolling pragma, be aggressive with\n				UP.PartialThreshold = std::max<unsigned>(UP.PartialThreshold, PragmaUnrollThreshold); // unrolling limits\n			}\n\n			unsigned ExactTripCount = TripCount; // 2nd priority is full unroll count\n			assert((ExactTripCount == 0 || MaxTripCount == 0) && \"ExtractTripCound and MaxTripCount cannot both be non zero.\");\n			unsigned FullUnrollTripCount = ExactTripCount ? ExactTripCount : MaxTripCount;\n			UP.Count = FullUnrollTripCount;\n			if (FullUnrollTripCount && FullUnrollTripCount <= UP.FullUnrollMaxCount)\n			{\n				if (getUnrolledLoopSize(LoopSize, UP) < UP.Threshold)\n				{\n					UseUpperBound = (MaxTripCount == FullUnrollTripCount);\n					TripCount = FullUnrollTripCount;\n					TripMultiple = UP.UpperBound ? 1 : TripMultiple;\n					return ExplicitUnroll;\n				}\n				else\n				{\n					if (Optional<EstimatedUnrollCost> Cost = analyzeLoopUnrollCost(L, FullUnrollTripCount, DT, *SE, TTI,UP.Threshold * UP.MaxPercentThresholdBoost / 100))\n					{\n						unsigned Boost = getFullUnrollBoostingFactor(*Cost, UP.MaxPercentThresholdBoost);\n						if (Cost->UnrolledCost < UP.Threshold * Boost / 100)\n						{\n							UseUpperBound = (MaxTripCount == FullUnrollTripCount);\n							TripCount = FullUnrollTripCount;\n							TripMultiple = UP.UpperBound ? 1 : TripMultiple;\n							return ExplicitUnroll;\n						}\n					}\n				}\n			}\n	\n			if (TripCount)  // 3rd priority is partial unrolling\n			{\n				UP.Partial |= ExplicitUnroll;\n				if (!UP.Partial)\n");
				fprintf(file,"				{\n					UP.Count = 0;\n					return false;\n				}\n				if (UP.Count == 0)\n					UP.Count = TripCount;\n				if (UP.PartialThreshold != NoThreshold) // new count is modulo of trip count\n				{\n					if (getUnrolledLoopSize(LoopSize, UP) > UP.PartialThreshold)\n						UP.Count = (std::max(UP.PartialThreshold, UP.BEInsns + 1) - UP.BEInsns)/(LoopSize - UP.BEInsns);\n					if (UP.Count > UP.MaxCount)\n						UP.Count = UP.MaxCount;\n					while (UP.Count != 0 && TripCount %% UP.Count != 0)\n						UP.Count--;\n					if (UP.AllowRemainder && UP.Count <= 1)\n					{\n						UP.Count = UP.DefaultUnrollRuntimeCount;\n						while (UP.Count != 0 && getUnrolledLoopSize(LoopSize, UP) > UP.PartialThreshold)\n							UP.Count >>= 1;\n					}\n					if (UP.Count < 2)\n						UP.Count = 0;\n				}\n				else\n					UP.Count = TripCount;\n				return ExplicitUnroll;\n			}\n			assert(TripCount == 0 && \"All cases when TripCount is constant should be covered here.\");\n			\n			computePeelCount(L, LoopSize, UP); // 4th priority is loop peeling\n			if (UP.PeelCount)\n			{\n				UP.Runtime = false;\n				UP.Count = 1;\n				return ExplicitUnroll;\n			}\n");
				fprintf(file,"			if (L->getHeader()->getParent()->getEntryCount()) // 5th priority is runtime unrolling\n				if (auto ProfileTripCount = getLoopEstimatedTripCount(L)) \n					if (*ProfileTripCount < FlatLoopTripCountThreshold)\n						return false;\n					else\n						UP.AllowExpensiveTripCount = true;  \n	\n			UP.Runtime |= PragmaEnableUnroll || PragmaCount > 0; // Reduce count based on type of unrolling and threshold values\n			if (!UP.Runtime)\n			{\n				UP.Count = 0;\n				return false;\n			}\n			if (UP.Count == 0)\n				UP.Count = UP.DefaultUnrollRuntimeCount;\n		\n			while (UP.Count != 0 && getUnrolledLoopSize(LoopSize, UP) > UP.PartialThreshold)\n				UP.Count >>= 1;\n			\n			if (!UP.AllowRemainder && UP.Count != 0 && (TripMultiple %% UP.Count) != 0)\n			{\n				while (UP.Count != 0 && TripMultiple %% UP.Count != 0)\n					UP.Count >>= 1;\n			}\n\n			if (UP.Count > UP.MaxCount)\n				UP.Count = UP.MaxCount;\n			if (UP.Count < 2)\n				UP.Count = 0;\n			return ExplicitUnroll;\n		}\n		\n		static unsigned ApproximateLoopSize(const Loop *L, unsigned &NumCalls, bool &NotDuplicatable, bool &Convergent, const TargetTransformInfo &TTI, AssumptionCache *AC, unsigned BEInsns)\n		{\n			SmallPtrSet<const Value *, 32> EphValues;\n			CodeMetrics::collectEphemeralValues(L, AC, EphValues);\n\n			CodeMetrics Metrics;\n			for (BasicBlock *BB : L->blocks())\n				Metrics.analyzeBasicBlock(BB, TTI, EphValues);\n			NumCalls = Metrics.NumInlineCandidates;\n			NotDuplicatable = Metrics.notDuplicatable;\n			Convergent = Metrics.convergent;\n\n			unsigned LoopSize = Metrics.NumInsts;\n			LoopSize = std::max(LoopSize, BEInsns + 1);	// do not allow an estimate of loop size zero\n\n			return LoopSize;\n		}\n		\n		static bool tryToUnrollLoop(Loop *L, DominatorTree &DT, LoopInfo *LI, ScalarEvolution *SE, const TargetTransformInfo &TTI, AssumptionCache &AC, OptimizationRemarkEmitter &ORE, bool PreserveLCSSA)\n		{\n			if (!L->isLoopSimplifyForm())\n				return false;\n\n			unsigned NumInlineCandidates;\n			bool NotDuplicatable;\n			bool Convergent;\n			\n			TargetTransformInfo::UnrollingPreferences UP;\n			UP.Threshold = 150;\n			UP.MaxPercentThresholdBoost = 400;\n			UP.OptSizeThreshold = 0;\n			UP.PartialThreshold = UP.Threshold;\n			UP.PartialOptSizeThreshold = 0;\n			UP.Count = 0;\n			UP.PeelCount = 0;\n			UP.DefaultUnrollRuntimeCount = 8;\n			UP.MaxCount = UINT_MAX;\n			UP.FullUnrollMaxCount = UINT_MAX;\n			UP.BEInsns = 2;\n			UP.Partial = false;\n			UP.Runtime = false;\n			UP.AllowRemainder = true;\n			UP.AllowExpensiveTripCount = false;\n			UP.Force = false;\n			UP.UpperBound = false;\n			UP.AllowPeeling = false; \n			unsigned UnrollMaxUpperBound = 8;");
							
			fprintf(file,"			if (L->getHeader()->getParent()->optForSize())\n			{\n				UP.Threshold = UP.OptSizeThreshold;\n				UP.PartialThreshold = UP.PartialOptSizeThreshold;\n			} \n			\n			if (UP.Threshold == 0 && (!UP.Partial || UP.PartialThreshold == 0)) // exit early if unrolling is disabled\n				return false;\n			unsigned LoopSize = ApproximateLoopSize(L, NumInlineCandidates, NotDuplicatable, Convergent, TTI, &AC, UP.BEInsns);\n			\n			if (NotDuplicatable)\n				return false;\n\n			if (NumInlineCandidates != 0)\n				return false;\n			\n			unsigned TripCount = 0; // find trip count (no of times loop executes)\n			unsigned MaxTripCount = 0;\n			unsigned TripMultiple = 1; // consider multiple loop exits\n			\n			BasicBlock *ExitingBlock = L->getLoopLatch();\n			if (!ExitingBlock || !L->isLoopExiting(ExitingBlock))\n			ExitingBlock = L->getExitingBlock();\n			if (ExitingBlock)\n			{\n				TripCount = SE->getSmallConstantTripCount(L, ExitingBlock);\n				TripMultiple = SE->getSmallConstantTripMultiple(L, ExitingBlock);\n			}\n			\n			if (Convergent)\n				UP.AllowRemainder = false;\n\n			bool MaxOrZero = false; // find the trip count upper bound if we cannot find the exact trip count\n			if (!TripCount) \n			{\n				MaxTripCount = SE->getSmallConstantMaxTripCount(L);\n				MaxOrZero = SE->isBackedgeTakenCountMaxOrZero(L);\n				if (!(UP.UpperBound || MaxOrZero) || MaxTripCount > UnrollMaxUpperBound)\n					MaxTripCount = 0;\n			}\n\n			// computeUnrollCount() decides whether it is beneficial to use upper bound to fully unroll the loop\n			bool UseUpperBound = false;\n			bool IsCountSetExplicitly =\n						computeUnrollCount(L, TTI, DT, LI, SE, &ORE, TripCount, MaxTripCount, TripMultiple, LoopSize, UP, UseUpperBound);\n			if (!UP.Count)\n				return false;\n			\n			if (TripCount && UP.Count > TripCount) // unroll factor (Count) must be less or equal to TripCount.\n				UP.Count = TripCount;\n\n			\n			if (!UnrollLoop(L, UP.Count, TripCount, UP.Force, UP.Runtime,UP.AllowExpensiveTripCount, UseUpperBound, MaxOrZero, TripMultiple, UP.PeelCount, LI, SE, &DT, &AC, &ORE, PreserveLCSSA)) // unroll the loop\n				return false;\n\n			if (IsCountSetExplicitly || UP.PeelCount)\n				SetLoopAlreadyUnrolled(L);\n			\n			return true;\n		}\n\t};\n}\n\nchar LoopUnrolling::ID = 0;\nstatic RegisterPass<LoopUnrolling> temp(\"loopunroll\",\"- to do loop unrolling\");");
	}
	else if (strcmp(optnode->llnode->con.alpha,"TailCallElim") == 0)
	{
		bracesCount = 0;
		
		fprintf(file,"#include \"llvm/Pass.h\"\n#include \"llvm/IR/Function.h\"\n#include \"llvm/IR/Type.h\"\n#include \"llvm/IR/Instruction.h\"\n#include \"llvm/IR/DiagnosticInfo.h\"\n#include \"llvm/ADT/StringRef.h\"\n#include \"llvm/Analysis/Loads.h\"\n#include \"llvm/Analysis/InstructionSimplify.h\"\n#include \"llvm/Transforms/Utils/Local.h\"\n#include \"llvm/Support/raw_ostream.h\"\n#include <cstring>\n\nusing namespace llvm;\nusing namespace std;\n\n");
		fprintf(file,"static bool isDynamicConstant(Value *V, CallInst *CI, ReturnInst *RI)\n{\n	if (isa<Constant>(V))\n		return true; // static constants are always dyn consts\n\n	if (Argument *Arg = dyn_cast<Argument>(V))\n	{\n		unsigned ArgNo = 0;\n		Function *F = CI->getParent()->getParent();\n		for (Function::arg_iterator AI = F->arg_begin(); &*AI != Arg; ++AI)\n			++ArgNo;\n		if (CI->getArgOperand(ArgNo) == Arg)\n			return true;\n	}\n	\n	if (BasicBlock *UniquePred = RI->getParent()->getUniquePredecessor())\n		if (SwitchInst *SI = dyn_cast<SwitchInst>(UniquePred->getTerminator()))\n			if (SI->getCondition() == V)\n				return SI->getDefaultDest() != RI->getParent();\n				\n	return false;\n}\n\n\nstatic Value *getCommonReturnValue(ReturnInst *IgnoreRI, CallInst *CI)\n{\n	Function *F = CI->getParent()->getParent();\n	Value *ReturnedValue = nullptr;\n\n	for (BasicBlock &BBI : *F)\n	{\n		ReturnInst *RI = dyn_cast<ReturnInst>(BBI.getTerminator());\n		if (RI == nullptr || RI == IgnoreRI)\n			continue;\n			\n		Value *RetOp = RI->getOperand(0);\n		if (!isDynamicConstant(RetOp, CI, RI))\n			return nullptr;\n\n		if (ReturnedValue && RetOp != ReturnedValue)\n			return nullptr;     // cannot transform if differing values are returned.\n		ReturnedValue = RetOp;\n	}\n	return ReturnedValue;\n}\n\nstatic bool canMoveAboveCall(Instruction *I, CallInst *CI)\n{\n	if (I->mayHaveSideEffects())\n		return false;\n\n	if (LoadInst *L = dyn_cast<LoadInst>(I))\n	{\n		if (CI->mayHaveSideEffects())\n		{\n			const DataLayout &DL = L->getModule()->getDataLayout();\n			if (CI->mayWriteToMemory() || !isSafeToLoadUnconditionally(L->getPointerOperand(), L->getAlignment(), DL, L))\n				return false;\n		}\n	}\n	return !is_contained(I->operands(), CI);\n}\n\nstatic Value *canTransformAccumulatorRecursion(Instruction *I, CallInst *CI)\n{\n	if (!I->isAssociative() || !I->isCommutative())\n		return nullptr;\n	assert(I->getNumOperands() == 2 && \"Associative/commutative operations should have 2 args!\");\n\n	if ((I->getOperand(0) == CI && I->getOperand(1) == CI) || (I->getOperand(0) != CI && I->getOperand(1) != CI)) \n		return nullptr; // exactly one operand should be the result of the call instruction\n		\n	if (!I->hasOneUse() || !isa<ReturnInst>(I->user_back()))\n		return nullptr; // the only user of this instruction we allow is a single return instruction\n		\n	return getCommonReturnValue(cast<ReturnInst>(I->user_back()), CI);\n}\n");
		fprintf(file,"\nstatic bool eliminateRecursiveTailCall(CallInst *CI, ReturnInst *Ret, BasicBlock *&OldEntry, SmallVectorImpl<PHINode *> &ArgumentPHIs)\n{\n	Value *AccumulatorRecursionEliminationInitVal = nullptr;\n	Instruction *AccumulatorRecursionInstr = nullptr;\n	\n	BasicBlock::iterator BBI(CI);\n	for (++BBI; &*BBI != Ret; ++BBI)\n	{\n		if (canMoveAboveCall(&*BBI, CI))\n			continue;\n		if ((AccumulatorRecursionEliminationInitVal = canTransformAccumulatorRecursion(&*BBI, CI)))\n			AccumulatorRecursionInstr = &*BBI;\n		else\n			return false;\n    }\n	\n	if (Ret->getNumOperands() == 1 && Ret->getReturnValue() != CI && !isa<UndefValue>(Ret->getReturnValue()) && AccumulatorRecursionEliminationInitVal == nullptr && !getCommonReturnValue(nullptr, CI))\n	{\n		if (!isDynamicConstant(Ret->getReturnValue(), CI, Ret))\n			return false; // current return instruction does not return a constant\n\n		AccumulatorRecursionEliminationInitVal = getCommonReturnValue(Ret, CI);\n		if (!AccumulatorRecursionEliminationInitVal)\n			return false;\n	}\n	\n	BasicBlock *BB = Ret->getParent();\n	Function *F = BB->getParent();\n\n	emitOptimizationRemark(F->getContext(), \"tailcallelim\", *F, CI->getDebugLoc(), \"transforming tail recursion to loop\");\n	\n	if (!OldEntry)\n	{\n		OldEntry = &F->getEntryBlock();\n		BasicBlock *NewEntry = BasicBlock::Create(F->getContext(), \"\", F, OldEntry);\n		NewEntry->takeName(OldEntry);\n		OldEntry->setName(\"tailrecurse\");\n		BranchInst::Create(OldEntry, NewEntry);\n\n		for (BasicBlock::iterator OEBI = OldEntry->begin(), E = OldEntry->end(), NEBI = NewEntry->begin(); OEBI != E;)\n			if (AllocaInst *AI = dyn_cast<AllocaInst>(OEBI++))\n				if (isa<ConstantInt>(AI->getArraySize()))\n					AI->moveBefore(&*NEBI);\n						\n		Instruction *InsertPos = &OldEntry->front();\n		for (Function::arg_iterator I = F->arg_begin(), E = F->arg_end(); I != E; ++I)\n		{\n			PHINode *PN = PHINode::Create(I->getType(), 2, I->getName() + \".tr\", InsertPos);\n			I->replaceAllUsesWith(PN); \n			PN->addIncoming(&*I, NewEntry);\n			ArgumentPHIs.push_back(PN);\n		}\n	}\n	\n	for (unsigned i = 0, e = CI->getNumArgOperands(); i != e; ++i)\n		ArgumentPHIs[i]->addIncoming(CI->getArgOperand(i), BB);\n		\n	if (AccumulatorRecursionEliminationInitVal)\n	{\n		Instruction *AccRecInstr = AccumulatorRecursionInstr;\n    \n		pred_iterator PB = pred_begin(OldEntry), PE = pred_end(OldEntry);\n		PHINode *AccPN = PHINode::Create(AccumulatorRecursionEliminationInitVal->getType(),distance(PB, PE) + 1, \"accumulator.tr\", &OldEntry->front());\n		\n		for (pred_iterator PI = PB; PI != PE; ++PI)\n		{\n			BasicBlock *P = *PI;\n			if (P == &F->getEntryBlock())\n				AccPN->addIncoming(AccumulatorRecursionEliminationInitVal, P);\n			else\n				AccPN->addIncoming(AccPN, P);\n		}\n\n		if (AccRecInstr)\n		{\n			AccPN->addIncoming(AccRecInstr, BB);\n			AccRecInstr->setOperand(AccRecInstr->getOperand(0) != CI, AccPN);\n		}\n		else\n			AccPN->addIncoming(Ret->getReturnValue(), BB);\n	\n	\n		for (BasicBlock &BBI : *F)\n			if (ReturnInst *RI = dyn_cast<ReturnInst>(BBI.getTerminator()))\n				RI->setOperand(0, AccPN);\n	}\n	\n	BranchInst *NewBI = BranchInst::Create(OldEntry, Ret);\n	NewBI->setDebugLoc(CI->getDebugLoc());\n	BB->getInstList().erase(Ret);  // remove return\n	BB->getInstList().erase(CI);   // remove call\n	return true;\n}\n");
		fprintf(file,"\nnamespace\n{\n\tclass TRE : public ");
		
		if (strcmp(optnode->next->llnode->con.alpha,"runOn") != 0)
			printerror("runOn not declared.\n");
		else if (strcmp(runoniter->llnode->con.alpha,"function") == 0)
		{
			processRunOnNode("Function", "TRE");
		}
		else
			printerror(strcat(runoniter->llnode->con.alpha," cannot be run on for tail call elimination.\n"));
		
		processIterateNode("function","instruction");	
		curr = runoniter->next;
		while (curr!=NULL)
		{
			if (strcmp(curr->llnode->con.alpha,"tailcallelim")==0)
			{
				curr = curr->next;
				
				if (strcmp(curr->llnode->con.alpha,"function")!=0)
					printerror(strcat(curr->llnode->con.alpha, " cannot be passed here, function is the first parameter."));
				curr = curr->next;
				
				if (curr==NULL)
					printerror("All functions may not have a tail recursive call.");
					
				else if (strcmp(curr->llnode->con.alpha, "precondition")==0)
				{
					curr = curr->next;
					if (strcmp(curr->llnode->con.alpha, "hasRecursiveTailCall")==0)
					{
						curr = curr->next;
						if (strcmp(curr->llnode->con.alpha,"function")!=0)
							printerror(strcat(curr->llnode->con.alpha, " cannot have tail recursive calls."));
						fprintf(file, "\n\t\t\tif (CallInst *CI = dyn_cast<CallInst>(I))	// call instruction\n\t\t\t{\n\t\t\t\tif (isTRECandidate(CI))	// check if instruction is a candidate for performing TRE\n\t\t\t\t{");
						
						bracesCount+=2;
					}
				}
				else
					printerror("All functions may not have a tail recursive call.");					
				fprintf(file, "\n\t\t\t\t\t\t\tif (F.getFunctionType()->isVarArg())\n								return false;\n							BasicBlock *OldEntry = nullptr;\n							SmallVector<PHINode*, 8> ArgumentPHIs;\n							if (ReturnInst *RI = dyn_cast<ReturnInst>(CI->getNextNode()))\n							{\n								eliminateRecursiveTailCall(CI,RI,OldEntry,ArgumentPHIs);\n								return true;\n							}\n							for (PHINode *PN : ArgumentPHIs)\n							{\n								if (Value *PNV = SimplifyInstruction(PN, F.getParent()->getDataLayout()))\n								{\n									PN->replaceAllUsesWith(PNV);\n									PN->eraseFromParent();\n								}\n							}\n");
			}
			curr = curr->next;
		}
		for (i=0;i<bracesCount;i++)
			fprintf(file, "\n\t\t\t}");
			
		fprintf(file,"\n\t\t}\n\n\t\tbool isTRECandidate(CallInst *CI)\n		{\n			BasicBlock *BB = CI->getParent();\n			Function *F = BB->getParent();\n		\n			if (CI->getCalledFunction() == F)\n			{\n				if (CI->isTailCall())\n					return true;\n					\n				Instruction *nextI = CI->getNextNode();\n				\n				if (isa<ReturnInst>(nextI))\n					return true;\n			}\n			\n			return false;\n		}\n\t};\n}\n\n");
		fprintf(file,"\nchar TRE::ID = 0;\nstatic RegisterPass<TRE> temp(\"tre\",\"- to do tail recursion elimination\");");
	}
}

void processNodeQueue()
{
	if (strcmp(qhead->llnode->con.alpha,"opt") == 0)
		processOptNode();
	else
		printerror("Optimization not enclosed in opt.\n");
}

void printerror(char *s)
{
	check = 1;
	
	strcpy(temppath,path);
	remove(strcat(temppath,"Optimiser.cpp"));
	strcpy(temppath,path);
	remove(strcat(temppath,"OptimiserDataflow.h"));
	strcpy(temppath,path);
	remove(strcat(temppath,"OptimiserDataflow.cpp"));
	strcpy(temppath,path);
	remove(strcat(temppath,"OptimiserAvEx.cpp"));
	strcpy(temppath,path);
	remove(strcat(temppath,"OptimiserReachingDefs.cpp"));
	strcpy(temppath,path);
	
	fclose(file);
	
	printf("%s\n",s);
	exit(0);
}

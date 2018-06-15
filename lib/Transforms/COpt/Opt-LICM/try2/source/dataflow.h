#ifndef DATAFLOW_H
#define DATAFLOW_H

#include "llvm/ADT/BitVector.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/PostOrderIterator.h"
#include "llvm/ADT/SmallSet.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/CFG.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/ValueMap.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"

#include <cstdio>
#include <iostream>
#include <vector>
#include <map>
#include <sstream>
#include <string>
#include <iomanip>

using namespace std;

#define WIDTH 60

namespace llvm 
{

	std::map<int, Instruction *> IndexToInstr;
	std::map<Instruction *, int> InstrToIndex;
	void assignIndicesToInstrs(Function * F); 

	typedef std::vector<BitVector> BitVectorList;
	typedef std::vector<BasicBlock*> BasicBlockList;
         
	struct TransferOutput	
	{
		BitVector element;			// for output of a transfer function.
		std::map<BasicBlock*, BitVector> neighborVals;	// list of BitVectors for predecessors/successors, used to handle phi nodes
									
	};

	struct BlockResult	// IN and OUT sets for a basic block
	{
		BitVector in;
		BitVector out;
		TransferOutput transferOutput;	// variable to store the temporary output of the transfer function 
	};

	enum Direction 
	{
		INVALID_DIRECTION,
		FORWARD,
		BACKWARD
	};

	enum MeetOp 
	{
		INVALID_MEETOP,
		UNION,
		INTERSECTION
	};

	struct DataFlowResult	// result of a data flow pass on a function
	{
		std::map<BasicBlock*, BlockResult> result;	// basic block to result
		std::map<void*, int> domainToIndex;	// domain elements to indices in bitvectors - which bits are values
		bool modified;
	};

    
	class DataFlow	// base class for data flow analysis, each analysis will be a subclass
	{
		public:

			DataFlow(Direction direction, MeetOp meetup_op) : direction(direction), meetup_op(meetup_op)	// constructor
			{																							}

			BitVector applyMeetOp(BitVectorList inputs);	// apply meet operator
	
			DataFlowResult run(Function &F, std::vector<void*> domain, BitVector boundaryCond, BitVector initCond); // analyse F

		protected:

			virtual TransferOutput transferFn(BitVector input, std::vector<void*> domain, std::map<void*, int> domainToIndex, BasicBlock* block) = 0;	// transfer function - takes IN/OUT, gives IN/OUT

		private:

			Direction direction;
			MeetOp meetup_op;
	};

	// printing utility functions

	void printBitVector(BitVector b);

	void printResult(DataFlowResult output);

	std::string printValue(Value* v);

	std::string printSet(std::vector<void*> domain, BitVector on, int mode);

	class Expression // EXPRESSION-RELATED UTILS
	{
		public:
			Value * v1;
			Value * v2;
			Instruction::BinaryOps op;
			Expression (Instruction * I);
			bool operator== (const Expression &e2) const;
			bool operator< (const Expression &e2) const;
			std::string toString() const;
	};

	std::string getShortValueName(Value * v);

	void printSet(std::vector<Expression> * x);	// print out a set of expressions in a nice format

}
#endif

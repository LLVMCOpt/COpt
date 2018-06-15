/* -----------------------------Information Required For Data Flow Analysis------------------------------ */
class Info
{	
	Info()			// constructor
	{		}
	
    static bool compare(Info * inf1, Info * inf2);	// compare 
    
    static Info* join(Info * inf1, Info * inf2, Info * info);	// meet operator
    
    static Info* intersection(Info * inf1, Info * inf2, Info * info);	// meet operator
};

/* -----------------------------Data Flow Analysis------------------------------ */
template <class Info, bool Direction> // pass info, Direction specifies direction of analysis ( forward, backward )
class DataFlowAnalysis 
{
	
	typedef std::pair<int, int> Edge;	// from one point to another
	
	std::map<int, Instruction*> IndexToInstr; // map an index to an instruction
	
	std::map<Edge, Info*> EdgeToInfo;	// map an edge to Info

	Info InitialState;	// initial state

	void assignIndicesToInstrs(Function * F) { } // each instruction within a function has an index starting from 1
		
	void getIncomingEdges(int index, std::vector<int> * IncomingEdges) { } // get all the incoming edges
	
	void getOutgoingEdges(int index, std::vector<int> * OutgoingEdges) { } // get all the outgoing edges
		
	void addEdge(Instruction * src, Instruction * dst, Info * content) { }	// add an edge to EdgeToInfo

	void initializeForward(Function * func)	// for forward analysis
	{
		// assign indices to the instructions 

		// initialize incoming edges to each basic block
				
		// initialize edges within the basic block
			
		// Initialize outgoing edges of the basic block
				
	}

	void initializeBackward(Function * func) { }	// for backward analysis

    virtual void flowfunction(Instruction * I, std::vector<int> & IncomingEdges, std::vector<int> & OutgoingEdges, std::vector<Info *> & Infos) = 0;	// to be implemented in subclass

	public:
	
	DataFlowAnalysis(Info &initialState) { }	// constructor
    
    virtual void runIterativeAlgorithm(Function * func) = 0;	// implement in subclass, call initializeForward or initializeBackward based on direction of analysis
    	
};

/* -----------------------------Reaching Definitions------------------------------ */

class ReachingInfo : public Info 
{
	// a data member that contains the definitions associated with each edge	
};
class ReachingDefinitionsAnalysis : public DataFlowAnalysis<ReachingInfo,true> DataFlowAnalysis
{
	
	ReachingDefinitionsAnalysis(ReachingInfo &initialState) { }	// constructor
	
	virtual void flowfunction(Instruction * I, std::vector<int> & IncomingEdges, std::vector<int> & OutgoingEdges, std::vector<Info *> & Infos)	{ } // implement the flow function for reaching definitions
	
};

/* An object of ReachingDefinitionsAnalysis is created when reachingDefinitions() is called in the DSL.
	The information stored in ReachingInfo will be returned to defs */

typedef struct LinkedListNode LinkedListNode;	// linked queue of AST nodes

struct LinkedListNode
{
	nodeType *llnode;
	LinkedListNode *next;
};

void semanticAnalysis(nodeType *root, int optNo);	// opens output file and initiates analysis

void semanticAnalysisRecursive(nodeType *node);	// calls processing of nodes in order

void addToLLQueue(nodeType *node);	// insert AST node into linked queue

void processNodeQueue();	// process nodes
void processOptNode();		// main processing
void processRunOnNode(char s[], char name[]);	// generates class, inheritance etc.
void processIterateNode(char start[], char end[]);
void processMakeInlineParams(LinkedListNode *node);
void checkFunctionInliningIter(nodeType *node); // checks if statements that need to be inside the iterate block are
void countMakeInlineParams(nodeType *node);
void checkIfInstrExists();
void checkConstantFoldingIter(nodeType *node);	// checks if statements that need to be inside the iterate block are
void processLCSEParams(LinkedListNode *node);
void countLCSEParams(nodeType *node);
void processGCSEParams(LinkedListNode *node);
void countGCSEParams(nodeType *node);
void checkGCSEIter(nodeType *node);
void checkLICMIter(nodeType *node);
void checkGVNIter(nodeType *node);
void writeGVNHelperCode();
void processUnrollLoopParams(LinkedListNode *node);

void writeDataflowFiles();	// helper files for Optimiser
void writeAvailableExprsFiles();
void writeReachingDefsFiles();

void printerror(char *s);	// display error messages

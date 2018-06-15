typedef enum {typeConst, typeOpr} nodeEnum;	// store type

/* constants */
typedef struct
{
	int typ;
	union		// value of constant
	{
		int num;
		char alpha[36];
	};					
	
} constNodeType;

/* operators */
typedef struct
{
	char oper[20];				// operator 
	int nops;					// number of operands
	struct nodeTypeTag *op[1];	// operands, extended at runtime

} oprNodeType;

/* node types */
typedef struct nodeTypeTag
{
	nodeEnum type;				// type of node

	union
	{
		constNodeType con;		// constants
		oprNodeType opr;		// operators
	};
	
} nodeType;

void printTree(nodeType *root, int count);	// print AST

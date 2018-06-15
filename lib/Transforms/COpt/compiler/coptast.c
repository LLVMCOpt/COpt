#include <stdio.h>
#include "coptast.h"

static int printcount = 0;

void printTree(nodeType *node, int count)
{
	if (node == NULL)
		return;

	if (count==0)
		printcount = 0;

	if (node->type==typeConst)
	{		
		for (int i=0; i<=printcount; i++)
			printf(" ");
		printcount++;

		if (node->con.typ==1)
			printf("%d\n", node->con.num);
		else if (node->con.typ==2)
			printf("%s\n", node->con.alpha);
		return;
	}	
	else
	{	
		for (int i = 0; i < node->opr.nops; i++)
				printTree(node->opr.op[i], 1);
	}
}

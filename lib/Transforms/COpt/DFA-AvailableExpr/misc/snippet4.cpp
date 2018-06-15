/*--------------------- Global CSE -------------------------*/
include AvailableExpressions // this line is to include dataflow.h, dataflow.cpp, avex.cpp and run the -available pass
int main()
{
	cfg c = "main";
	
	basicblock b;
	
	instruction i;
	
	exp e;
	
	for (b in c)
	{
		for (i in b)
		{
			if (e in i) // this loop is entered only if i has a binary expression and e is assigned that expression
			{				
				if (available(e,i))	// available(e,i) is a function that takes an exp e in an instruction i and tells you if it is available
				{					
					temp t = find(e);	// goes to the point where e is computed, returns the temporary that e is stored in
					replaceInstruction(i,t);	// replaces computation in i by variable t
				}
			}			
		}
	}
}
/*
def defs[];
defs d1, d2;
defs = reachingDefinitions(i);
sameDefs(d1,d2,e); // returns bool

// print, int and bool
*/

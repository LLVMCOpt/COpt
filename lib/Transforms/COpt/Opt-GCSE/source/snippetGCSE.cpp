/*--------------------- Global CSE -------------------------*/

int main()
{
	cfg c = "foo"; // for function foo
	
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
					eliminate(e); // perform CSE
				}
			}			
		}
	}
}

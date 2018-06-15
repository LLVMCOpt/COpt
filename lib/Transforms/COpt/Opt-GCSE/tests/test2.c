void foo(int x, int y, int z)
{
	int d = x*y;
	int e = x*y*z;
	
	int f = x+1;
	
	if (f>e)
		f = x+1;
	d = x+1;
	
	if (d>e)
		e = 1+y;
	e = 1+y;
}

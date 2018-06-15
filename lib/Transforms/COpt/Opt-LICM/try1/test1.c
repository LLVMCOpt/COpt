int foo(int a)
{
	int x, y=0;
	for (int i = 0; i < 3; i++)
	{
		x = 5;
		y*= i;
	}
	
	int r = 10;
	for (int i = 0; i < 42*a; i++) 
	{
		y = 5;
		x = 6;
		r += y;
	}
	return r;
}

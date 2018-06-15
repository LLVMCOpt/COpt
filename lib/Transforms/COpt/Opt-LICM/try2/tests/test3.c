int foo(int a, int b, int c, int d, int e)
{
	a = b+c;
	b = a-d;
	c = b+c;
	a = e;
	d = a-d;
	return d;
}

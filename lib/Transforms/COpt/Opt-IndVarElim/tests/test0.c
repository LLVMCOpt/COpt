#include <stdio.h>
int foo(int a, int b, int c, int d, int e)
{
	int i;
	for (i=0;i<5;i++)
	{
		b = a;
		c = b+d;
		a = i*i;	// make this a for another test
		e = b/i;
	}
	printf("%d %d %d %d %d %d\n",a,b,c,d,e,i);
	return c;
}
int main()
{
	printf("%d\n",foo(1,2,3,4,5));
	return 0;
}

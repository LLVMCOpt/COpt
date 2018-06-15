#include <stdio.h>
int foo(int a, int b)
{
	int c;
	c = a+b;
	if (a>b)
		c = a+b;
	c = a+b;
	printf("%d %d %d\n",a,b,c);
	return c;
}
int main()
{
	printf("%d\n",foo(1,2));
	return 0;
}

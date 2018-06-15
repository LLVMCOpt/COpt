void foo()
{
	int c = 6*5;
}

#include <stdio.h>
int main()
{
	
	int a = 5;
	int b = 4;
	int c;
	c = a*b;
	int d = a*b;
	
	if (c==d)
	{
		c=1;
	}
	else
	{
		c=2;
	}
	foo();
	
	printf("%d",a*b*c*d);
}


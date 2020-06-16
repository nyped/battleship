#include <stdio.h>

void
incr(int *x)
{
	++(*x);
}

void
decr(int *x)
{
	--(*x);
}

int main()
{
	int x = 5;
	void (*f)(int *);
	f = (++);
	f(&x);
	printf("%d\n", x);
	x = 5;
	f = decr;
	f(&x);
	printf("%d\n", x);
	return 0;
}

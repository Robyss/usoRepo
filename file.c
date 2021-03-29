#include <stdio.h>

void salute()
{
	char nume[101];
	scanf("%100s", nume);
	printf("Hello, %s!", nume);
}

int main()
{
	salute();
	return 0;
}

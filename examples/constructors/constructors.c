#include "ch32fun.h"
#include <stdio.h>

// Functions with __attribute__((constructor)) will be run by SystemInit()
// and increment this counter
static unsigned ctor_count = 0;

__attribute__((constructor)) static void ctor_fn1(void)
{
	ctor_count++;
}

__attribute__((constructor)) static void ctor_fn2(void)
{
	ctor_count++;
}

int main()
{
	SystemInit();

	printf("Constructors called: %u\n", ctor_count);

	while (1) {
		__WFE(); // Sleep
	}
}

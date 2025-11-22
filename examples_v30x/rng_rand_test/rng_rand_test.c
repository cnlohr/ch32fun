// Simple example that shows how to use the Random Number Generator functions

#include "ch32fun.h"
#include <stdio.h>
#include "ch32v30x_rng.h"

int main() {
	SystemInit();
	Delay_Ms(100);

	printf("\n~ Random Number Test ~\n");
	RNG_init();

	while(1) {
		u32 rand = RNG_rand();
		printf("Dec=%10u, Hex=0x%08X\n", rand, rand);
		Delay_Ms(1000);
	}
}
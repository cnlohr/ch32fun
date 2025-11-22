// Simple example that demonstrates how to configure different PLL multipliers
// for the HSI clock source.

#include "ch32fun.h"
#include <stdio.h>

void PLL_configure(u8 pll_indexValue) {
	if (pll_indexValue > 15) return;

	// Switch to HSI first
	RCC->CFGR0 &= ~RCC_SW;		   // Clear clock source bits
	RCC->CFGR0 |= RCC_SW_HSI;		// Switch to HSI clock source
	while((RCC->CFGR0 & RCC_SWS) != RCC_SWS_HSI); // Wait for switch

	// Turn off PLL and wait for it to stop
	RCC->CTLR &= ~RCC_PLLON;
	while(RCC->CTLR & RCC_PLLRDY);   // Wait for PLL to fully stop
	
	// Clear existing multiplier bits and set new one
	RCC->CFGR0 &= ~RCC_PLLMULL;	  // Clear all PLLMUL bits
	RCC->CFGR0 |= (pll_indexValue << 18); // Set new multiplier
	
	// Turn on PLL and wait for it to lock
	RCC->CTLR |= RCC_PLLON;
	while(!(RCC->CTLR & RCC_PLLRDY)); // Wait for PLL to lock
	
	// Switch to PLL
	RCC->CFGR0 &= ~RCC_SW;		   // Clear clock source bits
	RCC->CFGR0 |= RCC_SW_PLL;		// Switch clock source to PLL
	while((RCC->CFGR0 & RCC_SWS) != RCC_SWS_PLL); // Wait for switch
}

int main() {
	SystemInit();
	Delay_Ms(100);

	printf("\n~ PLL Test ~\n");

	while(1) {
		for (int i = 0; i < 16; i++) {
			printf("PLL multiplier index %d\n", i);
			PLL_configure(i);
			Delay_Ms(12000);
		}
	}
}
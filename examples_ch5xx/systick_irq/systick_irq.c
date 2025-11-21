#include "ch32fun.h"
#include <stdio.h>

#ifdef CH570_CH572
#define LED PA9
#else
#define LED PA8
#endif

#define INTERVAL 300*DELAY_MS_TIME

/*
 * Initialises the SysTick to trigger an IRQ with auto-reload, using HCLK/1 as
 * its clock source
 */
void systick_init(void)
{
	// Reset any pre-existing configuration
	SysTick->CTLR = 0x0000;
	
	// Set the compare register to trigger once in 300 milliseconds
	SysTick->CMP = INTERVAL;

	// Reset the Count Register, and the global millis counter to 0
	SysTick->CNT = 0x00000000;
	
	// Set the SysTick Configuration
	// NOTE: By not setting SYSTICK_CTLR_STRE, we maintain compatibility with
	// busywait delay funtions used by ch32v003_fun.
	SysTick->CTLR |= SYSTICK_CTLR_STE   |  // Enable Counter
	                 SYSTICK_CTLR_STIE  |  // Enable Interrupts
	                 SYSTICK_CTLR_STCLK ;  // Set Clock Source to HCLK/1
	
	// Enable the SysTick IRQ
	NVIC_EnableIRQ(SysTick_IRQn);
}

/*
 * SysTick ISR - must be lightweight to prevent the CPU from bogging down.
 * Increments Compare Register and systick_millis when triggered (every 1ms)
 * NOTE: the `__attribute__((interrupt))` attribute is very important
 */
void SysTick_Handler(void) __attribute__((interrupt));
void SysTick_Handler(void)
{
	// Increment CMP for the next trigger
	SysTick->CMP += INTERVAL;

	// Clear the trigger state for the next IRQ
	SysTick->SR = 0x00000000;

	// Flip the gpio (GPIO_InverseBits is a ch5xx specific macro)
	GPIO_InverseBits(LED);
}


int main(void)
{
	SystemInit();
	systick_init();

	funGpioInitAll(); // no-op on ch5xx

	funPinMode(LED, GPIO_CFGLR_OUT_2Mhz_PP);
		
	while(1);
}

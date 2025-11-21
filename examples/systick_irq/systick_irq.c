/*
 * Example for using SysTick with IRQs
 * 03-25-2023 E. Brombaugh
 * 05-12-2023 C. Lohr (Modified to reflect updated sysclk)
 * 09-25-2024 ADBeta (Minor updates to main loop, comments and added
 *                    convenient macro function)
 */

#include "ch32fun.h"
#include <stdio.h>

// Simple macro functions to give a arduino-like functions to call
// millis() reads the incremented systick variable
// micros() reads the raw SysTick Count, and divides it by the number of 
// ticks per microsecond ( WARN: Wraps every 90 seconds!)
#define millis() (systick_millis)
#define micros() (SysTick->CNT / DELAY_US_TIME)

// Incremented in the SysTick IRQ - in this example once per millisecond
volatile uint32_t systick_millis;

/*
 * Initialises the SysTick to trigger an IRQ with auto-reload, using HCLK/1 as
 * its clock source
 */
void systick_init(void)
{
	// Reset any pre-existing configuration
	SysTick->CTLR = 0x0000;
	
	// Set the compare register to trigger once per millisecond
	SysTick->CMP = DELAY_MS_TIME - 1;

	// Reset the Count Register, and the global millis counter to 0
	SysTick->CNT = 0x00000000;
	systick_millis = 0x00000000;
	
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
	// Increment the Compare Register for the next trigger
	// If more than this number of ticks elapse before the trigger is reset,
	// you may miss your next interrupt trigger
	// (Make sure the IQR is lightweight and CMP value is reasonable)
	SysTick->CMP += DELAY_MS_TIME;

	// Clear the trigger state for the next IRQ
	SysTick->SR = 0x00000000;

	// Increment the milliseconds count
	systick_millis++;
}


int main(void)
{
	SystemInit();
	Delay_Ms(100);

	printf("\n\nsystick_irq example\n");

	// Initialise the IRQ 
	printf("initializing systick...");
	systick_init();
	printf("done.\n");
	
	// Enable GPIOs for demonstration
	funGpioInitAll();
	funPinMode(PD0, GPIO_Speed_10MHz | GPIO_CNF_OUT_PP);
	funPinMode(PD4, GPIO_Speed_10MHz | GPIO_CNF_OUT_PP);
	funPinMode(PC0, GPIO_Speed_10MHz | GPIO_CNF_OUT_PP);
		
	printf("Beginning Loop...\n");
	while(1)
	{
		// Toggle the GPIO Pins with a delay - total delay will be 500ms
		uint32_t start_millis = millis();
		// On
		funDigitalWrite(PD0, FUN_HIGH);
		funDigitalWrite(PD4, FUN_HIGH);
		funDigitalWrite(PC0, FUN_HIGH);
		Delay_Ms(250);
		// Off
		funDigitalWrite(PD0, FUN_LOW);
		funDigitalWrite(PD4, FUN_LOW);
		funDigitalWrite(PC0, FUN_LOW);
		Delay_Ms(250);
		uint32_t end_millis = millis();
		
		// NOTE: Due to the time it takes for printf(), the Current Millis will
		// increment more than 500 per loop
		printf("\nMilliseconds taken:\t%lu\n", end_millis - start_millis);
		printf("Current Milliseconds:\t%lu\n", millis());
		printf("Current Microseconds:\t%lu\n", micros());
		printf("SysTick->CNT:\t\t%lu\n", SysTick->CNT);
	}
}

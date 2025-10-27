/*
 * Example for using SysTick with IRQs
 * 03-25-2023 E. Brombaugh
 * 05-12-2023 C. Lohr (Modified to reflect updated sysclk)
 * 09-25-2024 ADBeta (Minor updates to main loop, comments and added
 *                    convenient macro function)
 * 05-24-2025 Bogdan Ionescu (Modified original example to be used on 64bit counters)
 */

#include "ch32fun.h"
#include <stdio.h>

// Number of ticks elapsed per millisecond (48,000 when using 48MHz Clock)
#define SYSTICK_ONE_MILLISECOND ((uint32_t)FUNCONF_SYSTEM_CORE_CLOCK / 1000)
// Number of ticks elapsed per microsecond (48 when using 48MHz Clock)
#define SYSTICK_ONE_MICROSECOND ((uint32_t)FUNCONF_SYSTEM_CORE_CLOCK / 1000000)

// Simple macro functions to give a arduino-like functions to call
// millis() reads the incremented systick variable
// micros() reads the raw SysTick Count, and divides it by the number of 
// ticks per microsecond (WARN: This only uses the lower 32 bits of the SysTick)
#define millis() (systick_millis)
#define micros() (SysTick->CNT / SYSTICK_ONE_MICROSECOND)

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
	
	// Set the SysTick Compare Register to trigger in 1 millisecond
	SysTick->CMP = SysTick->CNT + SYSTICK_ONE_MILLISECOND;

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
	// Set the SysTick Compare Register to trigger in 1 millisecond
	SysTick->CMP = SysTick->CNT + SYSTICK_ONE_MILLISECOND;

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
      // cannot print 64-bit value directly
		printf("Current Microseconds:\t%lu\n", (uint32_t)micros());
		printf("SysTick->CNTH:\t\t%lu\n", SysTick->CNTH);
		printf("SysTick->CNTL:\t\t%lu\n", SysTick->CNTL);
	}
}

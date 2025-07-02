/*
* Example for using SysTick with IRQs
* 03-25-2023 E. Brombaugh
* 05-12-2023 C. Lohr (Modified to reflect updated sysclk)
* 09-25-2024 ADBeta (Minor updates to main loop, comments and added
*                    convenient macro function)
* 29-06-2025 monte-monte (Adapted to CH32V103)
*/

#include "ch32fun.h"
#include <stdio.h>

// Number of ticks elapsed per millisecond (10,000 when using 80MHz Clock)
// Need to divide by 8, because this is the only option for CH32V103
#define SYSTICK_ONE_MILLISECOND ((uint32_t)FUNCONF_SYSTEM_CORE_CLOCK / 8 / 1000)
// Number of ticks elapsed per microsecond (10 when using 80MHz Clock)
#define SYSTICK_ONE_MICROSECOND ((uint32_t)FUNCONF_SYSTEM_CORE_CLOCK / 8 / 1000000)

// Simple macro functions to give a arduino-like functions to call
// millis() reads the incremented systick variable
// micros() reads the raw SysTick Count, and divides it by the number of 
// ticks per microsecond ( WARN: Wraps every 90 seconds!)
#define millis() (systick_millis)
#define micros() (SysTick->CNT / SYSTICK_ONE_MICROSECOND)

// Incremented in the SysTick IRQ - in this example once per millisecond
volatile uint64_t systick_millis;

/*
* Initialises the SysTick to trigger an IRQ with auto-reload, using HCLK/1 as
* its clock source
*/
void systick_init(void)
{
	// Disable Systick
	SysTick->CTLR = 0;
	
	// Set the compare register to trigger once per millisecond
	uint64_t cmp_tmp = SYSTICK_ONE_MILLISECOND - 1;
	// In CH32V103 we can write systick regs only in 8bit manner
	SysTick->CMP0 = (uint8_t)cmp_tmp;
	SysTick->CMP1 = (uint8_t)(cmp_tmp >> 8);
	SysTick->CMP2 = (uint8_t)(cmp_tmp >> 16);
	SysTick->CMP3 = (uint8_t)(cmp_tmp >> 24);
	SysTick->CMP4 = (uint8_t)(cmp_tmp >> 32);
	SysTick->CMP5 = (uint8_t)(cmp_tmp >> 40);
	SysTick->CMP6 = (uint8_t)(cmp_tmp >> 48);
	SysTick->CMP7 = (uint8_t)(cmp_tmp >> 56);

	// Reset the Count Register, and the global millis counter to 0
	SysTick->CNT0 = 0;
	SysTick->CNT1 = 0;
	SysTick->CNT2 = 0;
	SysTick->CNT3 = 0;
	SysTick->CNT4 = 0;
	SysTick->CNT5 = 0;
	SysTick->CNT6 = 0;
	SysTick->CNT7 = 0;

	systick_millis = 0;
	
	// Enable Systick
	SysTick->CTLR = 1;
	
	// Enable the SysTick IRQ
	NVIC_EnableIRQ(SysTicK_IRQn);
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
	uint64_t cmp_tmp = SysTick->CMP + SYSTICK_ONE_MILLISECOND - 1;
	
	SysTick->CMP0 = (uint8_t)cmp_tmp;
	SysTick->CMP1 = (uint8_t)(cmp_tmp >> 8);
	SysTick->CMP2 = (uint8_t)(cmp_tmp >> 16);
	SysTick->CMP3 = (uint8_t)(cmp_tmp >> 24);
	SysTick->CMP4 = (uint8_t)(cmp_tmp >> 32);
	SysTick->CMP5 = (uint8_t)(cmp_tmp >> 40);
	SysTick->CMP6 = (uint8_t)(cmp_tmp >> 48);
	SysTick->CMP7 = (uint8_t)(cmp_tmp >> 56);

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
	funPinMode(PC8, GPIO_Speed_10MHz | GPIO_CNF_OUT_PP);
		
	printf("Beginning Loop...\n");
	while(1)
	{
		// Toggle the GPIO Pins with a delay - total delay will be 500ms
		uint32_t start_millis = millis();
		// On
		funDigitalWrite(PD0, FUN_HIGH);
		funDigitalWrite(PD4, FUN_HIGH);
		funDigitalWrite(PC8, FUN_HIGH);
		Delay_Ms(250);
		// Off
		funDigitalWrite(PD0, FUN_LOW);
		funDigitalWrite(PD4, FUN_LOW);
		funDigitalWrite(PC8, FUN_LOW);
		Delay_Ms(250);
		uint32_t end_millis = millis();
		
		// NOTE: Due to the time it takes for printf(), the Current Millis will
		// increment more than 500 per loop
		printf("\nMilliseconds taken:\t%lu\n", end_millis - start_millis);
		printf("Current Milliseconds:\t%lu\n", (uint32_t)millis());
		printf("Current Microseconds:\t%lu\n", micros());
		printf("SysTick->CNT:\t\t%lu\n", (uint32_t)SysTick->CNT);
	}
}

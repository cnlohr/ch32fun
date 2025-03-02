#include "ch32fun.h"
#include <stdio.h>

uint32_t count;

/*
 This code uses fast interrupts, but be warned, in this mode your hardware stack 
 is only 2 interrupt calls deep!!!

 When you execute code here, from RAM, the latency is between 310 and 480ns.
 From RAM is using  __attribute__((section(".srodata")));

Chart is in Cycles Spent @ 48MHz

|   __attribute__ |  HPE ON  |  HPE OFF |
| ((interrupt))   |   29  |   28  |
| ((section(".srodata"))) and ((interrupt)) | 28 | 23 |
| ((naked)) | 23 | 21 |
| ((section(".srodata"))) and ((naked)) | 15 | 16| 

  These were done with an empty (nop) loop in main.

  HPE ON  = 0x804 = 3
  HPE OFF = 0x804 = 0

  Bog-standard interrupt test with variance. I.e.
	__attribute__((interrupt)) with cursed code in main loop.

  Variance tests: 27-30 cycles
  
  Which will manifest as interurpt jitter.
*/

void EXTI7_0_IRQHandler( void ) __attribute__((interrupt));
void EXTI7_0_IRQHandler( void ) 
{
	// Flash just a little blip.
	funDigitalWrite( PC1, FUN_HIGH );
	funDigitalWrite( PC1, FUN_LOW );

	// Acknowledge the interrupt
	EXTI->INTFR = EXTI_Line3;
}

int main()
{
	SystemInit();

	// Enable GPIOs
	RCC->APB2PCENR = RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO;

	// GPIO D3 for input pin change.
	funPinMode( PD3, GPIO_CFGLR_IN_FLOAT );
	// funPinMode( PD4, GPIO_CFGLR_IN_PUPD ); // Keep SWIO enabled / seems to be unnecessary

	// GPIO C0 Push-Pull (our output)
	funPinMode( PC0,  GPIO_CFGLR_OUT_10Mhz_PP );
	funPinMode( PC1,  GPIO_CFGLR_OUT_10Mhz_PP );

	// Ugh this is tricky.
	// This is how we set (INTSYSCR) to enable hardware interrupt nesting
	// and hardware stack. BUT this means we can only stack intterrupts 2 deep.
	//
	// This feature is called "HPE"
	//
	// Note: If you don't do this, you will need to set your functions to be
	// __attribute__((interrupt)) instead of  __attribute__((naked))  with an
	// mret.
	//
	// PLEASE BE CAREFUL WHEN DOING THIS THOUGH.  There are a number of things
	// you should know with HPE.  The issue is that HPE doesn't preserve s0,
	// and s1. You should review the following material before using HPE.
	//   https://github.com/cnlohr/ch32v003fun/issues/90
	//   https://www.reddit.com/r/RISCV/comments/126262j/notes_on_wch_fast_interrupts/
	//   https://www.eevblog.com/forum/microcontrollers/bizarre-problem-on-ch32v003-with-systick-isr-corrupting-uart-tx-data
	asm volatile(
#if __GNUC__ > 10
		".option arch, +zicsr\n"
#endif
 		"addi t1, x0, 3\n"
		"csrrw x0, 0x804, t1\n"
		 : : :  "t1" );

	// Configure the IO as an interrupt.
	AFIO->EXTICR = AFIO_EXTICR1_EXTI3_PD;
	EXTI->INTENR = EXTI_INTENR_MR3; // Enable EXT3
	EXTI->RTENR = EXTI_RTENR_TR3;  // Rising edge trigger

	// enable interrupt
	NVIC_EnableIRQ( EXTI7_0_IRQn );

	while(1)
	{
		//GPIOC->BSHR = 1;
		//if( ((uint32_t*)count++) )
		//	GPIOC->BSHR = (1<<16);
		//if( count == 100 ) count = 0;
		asm volatile( "nop" );
	}
}

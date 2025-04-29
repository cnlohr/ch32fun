// Could be defined here, or in the processor defines.
#include "ch32fun.h"
#include <stdio.h>

uint32_t count;

// There's a few reasons you might want to run from RAM, for instance
// it's faster than running from flash, especially if you're running 
// on PLL.  Or maybe you want to power down the flash for some reaso.
//
// Well, no worries!  You can just stick it in the .data segment!

void RamFunction() __attribute__((section(".srodata"))) __attribute__((used));
void RamFunction()
{
	// GPIO D0 Push-Pull
	GPIOD->CFGLR &= ~(0xf<<(4*0));
	GPIOD->CFGLR |= (GPIO_Speed_10MHz | GPIO_CNF_OUT_PP)<<(4*0);

	// GPIO D4 Push-Pull
	GPIOD->CFGLR &= ~(0xf<<(4*4));
	GPIOD->CFGLR |= (GPIO_Speed_10MHz | GPIO_CNF_OUT_PP)<<(4*4);

	// GPIO C0 Push-Pull
	GPIOC->CFGLR &= ~(0xf<<(4*0));
	GPIOC->CFGLR |= (GPIO_Speed_10MHz | GPIO_CNF_OUT_PP)<<(4*0);

	while(1)
	{
		GPIOD->BSHR = 1 | (1<<4);	 // Turn on GPIOs
		GPIOC->BSHR = 1;
		Delay_Ms( 250 );
		GPIOD->BSHR = (1<<16) | (1<<(16+4)); // Turn off GPIODs
		GPIOC->BSHR = (1<<16);
		Delay_Ms( 250 );

		// But we turbo through twiddling a pin.
		
		asm volatile("\n\
			.option rvc\n\
			li a0, 1 | (1<<4)\n\
			li a1, (1<<16) | (1<<(16+4))\n\
			la a2, 0x40011410 /* GPIO D*/ \n\
			c.nop\n\
			c.sw a0, 0(a2)\n\
			c.sw a1, 0(a2)\n\
			c.sw a0, 0(a2)       /* Writing out takes 2 cycles from what I can tell*/ \n\
			c.addi %[count], 1   /* Insert this for comparative timing,  it's 1 cycle */ \n\
			c.sw a1, 0(a2)\n\
			c.sw a0, 0(a2)\n\
			c.sw a1, 0(a2)\n\
			c.sw a0, 0(a2)\n\
			c.sw a1, 0(a2)\n\
			" : [count]"+r"(count) : : "a0", "a1", "a2" );
	}
}

int main()
{
	SystemInit();

	// Enable GPIOs
	RCC->APB2PCENR |= RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOC;

	RamFunction();
}


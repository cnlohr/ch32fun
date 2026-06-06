#include "ch32fun.h"
#include <stdio.h>

#define TEST_VTF 1

// Inherits the right "interrupt" flags for HPE/normal interrupts.
void EXTI7_0_IRQHandler( void ) INTERRUPT_DECORATOR __attribute__((section(".srodata")));

void EXTI7_0_IRQHandler( void ) 
{
	// Flash just a little blip.
	funDigitalWrite( PD3, FUN_HIGH );
	funDigitalWrite( PD3, FUN_LOW );

	// Acknowledge the interrupt
	EXTI->INTFR = EXTI_Line3;

#if FUNCONF_ENABLE_HPE
	// The HPE target doesn't know how to function-prologue.
	asm volatile( "mret" );
#endif
}

int ramloop() __attribute__((noinline))  __attribute__((section(".srodata")));
int ramloop()
{
	while(1)
	{
		funDigitalWrite( PD3, FUN_HIGH );
		funDigitalWrite( PD3, FUN_LOW );
		ADD_N_NOPS(5);
		DelaySysTick(10);
	}

}

int main()
{
	SystemInit();
	// Enable GPIOs
	RCC->APB2PCENR = RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO;

	funPinMode( PD3, GPIO_CFGLR_OUT_10Mhz_PP );
	funDigitalWrite( PD3, FUN_HIGH );

	// If you set FUNCONF_CUSTOM_MTVEC, make MTVEC point to the function that handles all IRQs.
#if FUNCONF_TINYVECTOR
#if TEST_VTF
	PFIC->VTFADDR[0] = 1|(uint32_t)&EXTI7_0_IRQHandler;
	PFIC->VTFIDR[0] = EXTI7_0_IRQn;
#else
	__set_MTVEC( (uint32_t)&EXTI7_0_IRQHandler );
#endif
#endif

	// Configure the IO as an interrupt.
	AFIO->EXTICR = AFIO_EXTICR_EXTI3_PD;
	EXTI->INTENR = EXTI_INTENR_MR3; // Enable EXT3
	EXTI->FTENR = EXTI_FTENR_TR3;  // Falling edge trigger

	// enable interrupt
	NVIC_EnableIRQ( EXTI7_0_IRQn );

	ramloop();
}


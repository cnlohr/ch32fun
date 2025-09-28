// Example using Timer 1 capture to look for down-going events
// on PD2.  Then using DMA to capture that event and write it
// to a circular queue that can be read out later.
//
// For it to produce interesting output, you can wire PD2 to PD3.

#include "ch32fun.h"
#include <stdio.h>

uint32_t count;
uint16_t reply_buffer[256]; // For generating the mask/modulus, this must be a power of 2 size.

int main()
{
	SystemInit();
	funGpioInitAll();

	// Enable GPIOs
	RCC->APB2PCENR |= RCC_APB2Periph_TIM1 | RCC_APB2Periph_AFIO;
	RCC->AHBPCENR |= RCC_AHBPeriph_DMA1;

	// TIM2_CH1 -> PD2 -> DMA1_CH2
	#define DMA_IN  DMA1_Channel2

	// T1C1 -> What we are sampling.
	funPinMode( PD2, GPIO_CFGLR_IN_PUPD );
	funDigitalWrite( PD2, 1 );

	// PD3 output to send something back into PD2.
	funPinMode( PD3, GPIO_CFGLR_OUT_2Mhz_PP );

	// Enable TIM2
	TIM1->CTLR1 = TIM_ARPE | TIM_CEN;
	TIM1->DMAINTENR = TIM_CC1DE | TIM_UDE; // Enable DMA for T1CC1

	int samples_in_buffer = sizeof(reply_buffer) / sizeof(reply_buffer[0]);

	// TIM1_TRIG/TIM1_COM/TIM1_CH4
	DMA_IN->MADDR = (uint32_t)reply_buffer;
	DMA_IN->PADDR = (uint32_t)&TIM1->CH1CVR; // Input
	DMA_IN->CFGR = 
		0                 |                  // PERIPHERAL to MEMORY
		0                 |                  // Low priority.
		DMA_CFGR1_MSIZE_0 |                  // 16-bit memory
		DMA_CFGR1_PSIZE_0 |                  // 16-bit peripheral
		DMA_CFGR1_MINC    |                  // Increase memory.
		DMA_CFGR1_CIRC    |                  // Circular mode.
		0                 |                  // NO Half-trigger
		0                 |                  // NO Whole-trigger
		DMA_CFGR1_EN;                        // Enable
	DMA_IN->CNTR = samples_in_buffer;

	TIM1->PSC = 0x0fff;		// set TIM1 clock prescaler divider (Massive prescaler)
	TIM1->ATRLR = 65535;	// set PWM total cycle width

	// Tim 1 input / capture (CC1S = 01)
	TIM1->CHCTLR1 = TIM_CC1S_0;

	// Add here CC1P to switch from UP->GOING to DOWN->GOING log times.
	TIM1->CCER = TIM_CC1E;// | TIM_CC1P;
	
	// initialize counter
	TIM1->SWEVGR = TIM_UG;


	int tail = 0;
	while(1)
	{
		// Must perform modulus here, in case DMA_IN->CNTR == 0.
		int head = (samples_in_buffer - DMA_IN->CNTR) & (samples_in_buffer-1);

		while( head != tail )
		{
			uint32_t time_of_event = reply_buffer[tail];
			printf( "%d/%d %d\n", tail, head, (int)time_of_event );

			// Performs modulus to loop back.
			tail = (tail+1)&(samples_in_buffer-1);
		}

		Delay_Ms(100);
		funDigitalWrite( PD3, 1 );
		Delay_Ms(10);
		funDigitalWrite( PD3, 0 );
		printf( "." );
	}
}


/* CH570/CH572 PWM output and TIMER input, to track a pin change.
	PLEASE NOTE: YOU CANNOT RUN THIS on CH570Q or CH570E.
	They do not have PWM outputs.
*/

#include "ch32fun.h"
#include <stdio.h>

#ifdef CH570_CH572
#define LED PA9
#else
#define LED PA8
#endif

uint32_t count;

int last = 0;
void handle_debug_input( int numbytes, uint8_t * data )
{
	last = data[0];
	count += numbytes;
}

uint32_t timer_data[256];

int main()
{
	SystemInit();

	funGpioInitAll(); // no-op on ch5xx

	funPinMode( LED, GPIO_CFGLR_OUT_10Mhz_PP );

	funPinMode(PA7, GPIO_CFGLR_OUT_2Mhz_PP); //PWM1


	// Setup 26-bit capture timer - default on PA7.
	R8_TMR_CTRL_MOD = 
		(0b01<<6) | // RB_TMR_CAP_EDGE = Capture the time between any edge changes;
		0 |         // RB_TMR_CAP_COUNT = Capture mode.
		(1<<2) | 	// RB_TMR_COUNT_EN = 1
		(1<<1) |	// RB_TMR_ALL_CLEAR = 1
		(1<<0); 	// RB_TMR_MODE_IN = 1 (Input mode)
	R8_TMR_CTRL_MOD &= ~(1<<1); // Clear R8_TMR_CTRL_MOD.
	R16_TMR_DMA_BEG = (uintptr_t)&timer_data[0];
	R16_TMR_DMA_END = (uintptr_t)&timer_data[sizeof(timer_data)/sizeof(timer_data[0])]; // END is not inclusive.

	R8_TMR_CTRL_DMA =
		(1<<2) | // RB_TMR_DMA_LOOP
		(1<<0);  // RB_TMR_DMA_ENABLE


	// Base is 952.4 @ 5-bit (31 period) = 60MHz base clock.
	// Output at 11.9kHz with this setup.
	R16_PWM_CLOCK_DIV = 80;
	R8_PWM_OUT_EN |= (1 << (0));
	R8_PWM_CONFIG &= ~(RB_PWM_CYC_MOD | RB_PWM_CYCLE_SEL);
	R8_PWM_CONFIG |= 3 << 2; // 0 = 8-bit, 3 << 2 = 5-bit, 2 << 2 = 6-bit, 1 << 2 = 7-bit.
	R8_PWM_CONFIG |= RB_PWM_CYCLE_SEL;
	R8_PWM1_DATA = 1;

	while(1)
	{
		//printf( "%08x\n", R16_TMR_DMA_NOW );
		// Should print 00001310 020000a0 00001310 020000a0
		printf( "%08x %08x %08x %08x @ %08x\n", (int)timer_data[0], (int)timer_data[1], (int)timer_data[2], (int)timer_data[3], R16_TMR_DMA_NOW );
		Delay_Ms(1);
		//if( R8_PWM1_DATA++ > 28 ) R8_PWM1_DATA = 1;
	}
}

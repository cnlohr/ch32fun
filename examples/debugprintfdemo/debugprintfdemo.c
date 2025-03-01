/* Small example showing how to use the SWIO programming pin to 
   do printf through the debug interface */

#include "ch32fun.h"
#include <stdio.h>

uint32_t count;

int last = 0;
void handle_debug_input( int numbytes, uint8_t * data )
{
	last = data[0];
	count += numbytes;
}

int main()
{
	SystemInit();

	while( !DebugPrintfBufferFree() );

	// Enable GPIOs
	RCC->APB2PCENR |= RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOC;

	funPinMode( PD0, GPIO_Speed_10MHz | GPIO_CNF_OUT_PP );
	funPinMode( PC0, GPIO_Speed_10MHz | GPIO_CNF_OUT_PP );

	while(1)
	{
		funDigitalWrite( PD0, 1 );
		funDigitalWrite( PC0, 1 );

		printf( "+%lu\n", count++ );
		Delay_Ms(100);
		int i;
		for( i = 0; i < 10000; i++ )
			poll_input();

		funDigitalWrite( PD0, 0 );
		funDigitalWrite( PC0, 0 );

		printf( "-%lu[%c]\n", count++, last );
		Delay_Ms(100);
	}
}


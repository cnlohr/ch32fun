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
	RCC->HB2PCENR |= RCC_HB2Periph_GPIOE;

	funPinMode( PE2, GPIO_CNF_OUT_PP, GPIO_Speed_10MHz );
	funPinMode( PE3, GPIO_CNF_OUT_PP, GPIO_Speed_10MHz );

	while(1)
	{
		funDigitalWrite( PE2, 1 );
		funDigitalWrite( PE3, 1 );

		printf( "+%lu\n", count++ );
		Delay_Ms(100);
		int i;
		for( i = 0; i < 10000; i++ )
			poll_input();

		funDigitalWrite( PE2, 0 );
		funDigitalWrite( PE3, 0 );

		printf( "-%lu[%c]\n", count++, last );
		Delay_Ms(100);
	}
}


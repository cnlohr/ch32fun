/* Small example showing how to use the SWIO programming pin to 
   do printf through the debug interface */

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

int main()
{
	SystemInit();

	funGpioInitAll(); // no-op on ch5xx

	funPinMode( LED, GPIO_CFGLR_OUT_10Mhz_PP );

	while(1)
	{
		funDigitalWrite( LED, FUN_LOW ); // Turn on LED
		printf( "+%lu\n", count++ );
		Delay_Ms(100);
		int i;
		for( i = 0; i < 10000; i++ )
			poll_input();
		funDigitalWrite( LED, FUN_HIGH ); // Turn off LED
		printf( "-%lu[%c]\n", count++, last );
		Delay_Ms(100);
	}
}

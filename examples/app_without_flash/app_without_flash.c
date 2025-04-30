/* Example of code that is compiled to run from RAM, pushed to RAM, then run from there. */

#include "ch32fun.h"
#include <stdio.h>

uint32_t count;


#if CH32V003
	#define USEPIN PA1
#else
	#define USEPIN PA8
#endif


int main()
{
	SystemInit();

#ifdef funGpioInitAll
	funGpioInitAll();
#endif

	funPinMode( USEPIN, GPIO_CFGLR_OUT_10Mhz_PP );

	while(1)
	{
		funDigitalWrite( USEPIN, FUN_LOW ); // Turn on LED
		printf( "+%lu\n", count++ );
		Delay_Ms(1000);
		funDigitalWrite( USEPIN, FUN_HIGH ); // Turn off LED
		Delay_Ms(1000);
	}
}

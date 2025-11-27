// See funconfig.h to see how this uses the uart.

#include "ch32fun.h"
#include <stdio.h>

uint32_t count;

int main()
{
	SystemInit();

	funGpioInitAll();

	funPinMode( PE2, GPIO_CNF_OUT_PP, GPIO_Speed_180MHz );

	while(1)
	{
		funDigitalWrite( PE2,     FUN_LOW );
		Delay_Ms( 50 );
		funDigitalWrite( PE2,     FUN_HIGH );
		Delay_Ms( 50 );
		printf( "Count: %lu\r\n", count++ );
	}
}

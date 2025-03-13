#include "ch32fun.h"
#include <stdio.h>

int main()
{
	SystemInit();

	funPinMode( PA8, GPIO_CFGLR_OUT_2Mhz_PP );

	while(1)
	{
		funDigitalWrite( PA8, FUN_LOW );	 // Turn on LED
		Delay_Ms( 33 );
		funDigitalWrite( PA8, FUN_HIGH );	 // Turn off LED
		Delay_Ms( 300 );
	}
}

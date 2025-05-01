#include "ch32fun.h"
#include <stdio.h>

#if MCU_PACKAGE == 0 || MCU_PACKAGE == 2
#define LED PA9
#else
#define LED PA8
#endif

int main()
{
	SystemInit();

	// Enable GPIOs
	funGpioInitAll();

	funPinMode( LED, GPIO_CFGLR_OUT_2Mhz_PP );

	while(1)
	{
		funDigitalWrite( LED, FUN_LOW );	 // Turn on LED
		Delay_Ms( 33 );
		funDigitalWrite( LED, FUN_HIGH );	 // Turn off LED
		Delay_Ms( 300 );
	}
}

#include "ch32fun.h"
#include <stdio.h>

void __ITCM spin()
{
	while(1)
	{
		funDigitalWrite( PE3,     FUN_HIGH );
		funDigitalWrite( PE3,     FUN_LOW );
	}
}

int main_V5F()
{
	printf( "Starting spin\r\n" );
	funDigitalWrite( PE2,     FUN_HIGH );
	spin();

	while(1)
	{
	}
}

int main()
{
	SystemInit();

	printf( "Started V5F.\r\n" );

	funGpioInitAll(); // Enable GPIOs

	funPinMode( PE2, GPIO_CNF_OUT_PP, GPIO_Speed_180MHz );
	funPinMode( PE3, GPIO_CNF_OUT_PP, GPIO_Speed_180MHz );
	funDigitalWrite( PE2,     FUN_LOW );

	while(1)
	{
		// Uncomment this to make chip sad
		/*Delay_Ms(250);
		funDigitalWrite( PE2,     FUN_HIGH );
		Delay_Ms(250);
		funDigitalWrite( PE2,     FUN_LOW );*/
	}
}

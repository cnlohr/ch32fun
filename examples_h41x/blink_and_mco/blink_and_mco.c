#include "ch32fun.h"
#include <stdio.h>

int main_V5F()
{

	funPinMode( PE3, GPIO_CNF_OUT_PP, GPIO_Speed_180MHz );

	while(1)
	{
		funDigitalWrite( PE3,     FUN_HIGH );
		Delay_Ms( 250 );
		funDigitalWrite( PE3,     FUN_LOW );
		Delay_Ms( 300 );
	}
}

int main()
{
	SystemInit();

	funGpioInitAll(); // Enable GPIOs

	StartV5F(main_V5F);
	
	funPinMode( PE2, GPIO_CNF_OUT_PP, GPIO_Speed_180MHz );
	funPinMode( PB0, GPIO_CNF_OUT_PP_AF, GPIO_Speed_180MHz );

	while(1)
	{
		funDigitalWrite( PE2,     FUN_LOW );
		Delay_Ms( 250 );
		funDigitalWrite( PE2,     FUN_HIGH );
		Delay_Ms( 250 );
	}
}

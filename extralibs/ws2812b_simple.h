/* Single-File-Header for using asynchronous LEDs with the CH32V003 using GPIO.

   Copyright 2023 <>< Charles Lohr, under the MIT-x11 or NewBSD License, you choose!

   If you are including this in main, simply
	#define WS2812BSIMPLE_IMPLEMENTATION

   You may also want to define
	#define WS2812BSIMPLE_NO_IRQ_TWEAKING

*/

#ifndef _WS2812B_SIMPLE
#define _WS2812B_SIMPLE

#include <stdint.h>

void WS2812BSimpleSend( int pin, uint8_t * data, int len_in_bytes );

#ifdef WS2812BSIMPLE_IMPLEMENTATION

#include "funconfig.h"

#if FUNCONF_SYSTICK_USE_HCLK != 1
#error WS2812B Driver Requires FUNCONF_SYSTICK_USE_HCLK
#endif

#include "ch32fun.h"

void WS2812BSimpleSend( int pin, uint8_t * data, int len_in_bytes )
{
	funGpioInitAll();

	funPinMode(pin, GPIO_CFGLR_OUT_10Mhz_PP);


	funDigitalWrite(pin, FUN_LOW);

	uint8_t * end = data + len_in_bytes;
	while( data != end )
	{
		uint8_t byte = *data;

		int i;
		for( i = 0; i < 8; i++ )
		{
			if( byte & 0x80 )
			{
				// WS2812B's need AT LEAST 625ns for a logical "1"
				funDigitalWrite(pin, FUN_HIGH);
				DelaySysTick(25);
				funDigitalWrite(pin, FUN_LOW);
				DelaySysTick(1);
			}
			else
			{
				// WS2812B's need BETWEEN 62.5 to about 500 ns for a logical "0"
#ifndef WS2812BSIMPLE_NO_IRQ_TWEAKING
				__disable_irq();
#endif
				funDigitalWrite(pin, FUN_HIGH);
				asm volatile( "nop\nnop\nnop\nnop" );
				funDigitalWrite(pin, FUN_LOW);
#ifndef WS2812BSIMPLE_NO_IRQ_TWEAKING
				__enable_irq();
#endif
				DelaySysTick(15);
			}
			byte <<= 1;
		}

		data++;
	}

	funDigitalWrite(pin, FUN_LOW);
}

#endif

#endif


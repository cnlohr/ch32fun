// Small example showing how to use SPI and DMA to drive rgb leds
// NOTE: CONNECT WS2812's to PA7 (CH570/2) or to PA14 (CH582/3, CH591/2)

#include "ch32fun.h"
#include <stdio.h>
#include <string.h>

#define WS2812DMA_IMPLEMENTATION
// #define WSRBG //For WS2816C's.
#define WSGRB // For SK6805-EC15
#define NR_LEDS 64

#ifdef CH570_CH572
#define DEBUG_LED PA9
#else
#define DEBUG_LED PA8
#endif

#include "ws2812b_dma_spi_led_driver.h"
#include "color_utilities.h"

uint16_t phases[NR_LEDS];
int frameno;
volatile int tween = -NR_LEDS;

// Callbacks that you must implement.
uint32_t WS2812BLEDCallback( int ledno )
{
	uint8_t index = (phases[ledno])>>8;
	uint8_t rsbase = sintable[index];
	uint8_t rs = rsbase>>3;
	uint32_t fire = ((huetable[(rs+190)&0xff]>>1)<<16) | (huetable[(rs+30)&0xff]) | ((huetable[(rs+0)]>>1)<<8);
	uint32_t ice  = 0x7f0000 | ((rsbase>>1)<<8) | ((rsbase>>1));

	return TweenHexColors( fire, ice, tween ); // Where "tween" is a value from 0 ... 255
}

int main()
{
	int k;
	SystemInit();

	// Enable GPIO PA9 (for debugging)
	funPinMode(DEBUG_LED, GPIO_CFGLR_OUT_10Mhz_PP);
	funDigitalWrite(DEBUG_LED, 0); // Turn on GPIO PA9
	Delay_Ms(500);

	WS2812BDMAInit();
	frameno = 0;

	for( k = 0; k < NR_LEDS; k++ ) phases[k] = k<<8;

	int tweendir = 0;

	while(1)
	{
	
		// funDigitalWrite( DEBUG_LED, 0 ); // Turn on GPIO PA9
		// Wait for LEDs to totally finish.
		while( WS2812BLEDInUse );

		frameno++;

		if( frameno == 1024 )
		{
			tweendir = 1;
		}
		if( frameno == 2048 )
		{
			tweendir = -1;
			frameno = 0;
		}

		if( tweendir )
		{
			int t = tween + tweendir;
			if( t > 255 ) t = 255;
			if( t < -NR_LEDS ) t = -NR_LEDS;
			tween = t;
		}

		for( k = 0; k < NR_LEDS; k++ )
		{
			phases[k] += ((((rands[k&0xff])+0xf)<<2) + (((rands[k&0xff])+0xf)<<1))>>1;
		}

		WS2812BDMAStart( NR_LEDS );

		funDigitalWrite( DEBUG_LED, 1 ); // Turn it off
		Delay_Ms( 20 );
	}
}

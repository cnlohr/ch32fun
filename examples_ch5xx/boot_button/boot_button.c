/*
 * This demo jumps to the built-in bootloader, so it can be programmed over ISP.
 * Most ch5xx dev boards come with a "boot" or "download" button, when this
 * is pressed the chip resets and executes the ISP bootloader which presents
 * itself on USB to the host.
 * NOTE: if "FUNCONF_USE_USBPRINTF" is used, a call to USBFSReset() is needed
 *       just before jump_isprom()
 */

#include "ch32fun.h"

#ifdef CH570_CH572
#define PIN_LED    PA9
#define PIN_BUTTON PA1
#define BUTTON_PRESSED funDigitalRead( PIN_BUTTON )
#else
#define PIN_LED    PA8
#define PIN_BUTTON PB22
#define BUTTON_PRESSED !funDigitalRead( PIN_BUTTON )
#endif

void blink(int n) {
	for(int i = n-1; i >= 0; i--) {
		funDigitalWrite( PIN_LED, FUN_LOW ); // Turn on LED
		Delay_Ms(33);
		funDigitalWrite( PIN_LED, FUN_HIGH ); // Turn off LED
		if(i) Delay_Ms(33);
	}
}

int main()
{
	SystemInit();

	funGpioInitAll(); // no-op on ch5xx

	funPinMode( PIN_LED,    GPIO_CFGLR_OUT_10Mhz_PP ); // Set PIN_LED to output
	funPinMode( PIN_BUTTON, GPIO_CFGLR_IN_PUPD ); // Set PIN_BUTTON to input
	blink(1);

	while(1)
	{
		if( BUTTON_PRESSED ) {
			blink(5);
			jump_isprom();
		}
	}
}

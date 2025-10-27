#include "ch32fun.h"
#include <stdio.h>
#include <string.h>
#include "fsusb.h"

#if defined(CH57x) && (MCU_PACKAGE == 0 || MCU_PACKAGE == 2) // ch570/2
#define PIN_LED    PA9
#define PIN_BUTTON PA1
#define BUTTON_PRESSED funDigitalRead( PIN_BUTTON )
#else
#define PIN_LED    PA8
#define PIN_BUTTON PB22
#define BUTTON_PRESSED !funDigitalRead( PIN_BUTTON )
#endif

uint8_t run = 1; // print stuff or not

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

	funGpioInitAll();

	funPinMode( PIN_LED,    GPIO_CFGLR_OUT_10Mhz_PP ); // Set PIN_LED to output
	funPinMode( PIN_BUTTON, GPIO_CFGLR_IN_PUPD ); // Set PIN_BUTTON to input

	if( BUTTON_PRESSED ) {
		blink(5);
		jump_isprom();
	}

	USBFSSetup();
	blink(1);

	int i = 0;
	while(1) {
		if(run) {
			printf("Press 'p' to pause counting, 'c' to continue, 1,2, or 3 to blink. %d\r", i++);
		}
		Delay_Ms(100);
	}
}

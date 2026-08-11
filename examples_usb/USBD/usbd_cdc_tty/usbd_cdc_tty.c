#include "ch32fun.h"
#include <stdio.h>
#include <string.h>
#include "usbd.h"

#define PIN_LED PC0 // activity LED on rj45

uint8_t run = 0; // print stuff or not

void blink(int n) {
	for(int i = n-1; i >= 0; i--) {
		funDigitalWrite( PIN_LED, FUN_LOW ); // Turn on LED
		Delay_Ms(33);
		funDigitalWrite( PIN_LED, FUN_HIGH ); // Turn off LED
		if(i) Delay_Ms(33);
	}
}

// this callback is mandatory when FUNCONF_USE_USBPRINTF is defined,
// can be empty though
void HandleUSBInput(int numbytes, uint8_t *data) {
	if(numbytes == 1) {
		switch(data[0]) {
		case '1':
			blink(1);
			break;
		case '2':
			blink(2);
			break;
		case '3':
			blink(3);
			break;
		case 'c':
			run = 1;
			break;
		case 'p':
			run = 0;
			break;
		}
	}
	else {
		_write(0, (const char*)data, numbytes);
	}
}


int main()
{
	SystemInit();

	funGpioInitAll();

	funPinMode( PIN_LED, GPIO_CFGLR_OUT_10Mhz_PP ); // Set PIN_LED to output

	USBDSetup();
	blink(5);
	printf("CPU speed: %dMHz\n", ((FUNCONF_SYSTEM_CORE_CLOCK)/1000000));

	int i = 0;
	while(1) {
		poll_input(); // check if there is input from the tty

		if(run) {
			printf("Counting: %d\r", i++);
		}

		Delay_Ms(100);
	}
}

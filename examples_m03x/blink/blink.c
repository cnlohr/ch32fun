#include "ch32fun.h"
#include <stdio.h>

// CH32M030G8R7-PMSM dev board:
//   - PB0 (pin 28) is the middle pin of one of the JP1-3 hall-sensor
//     jumpers. With the jumper unbridged (default), PB0 is a free GPIO.
//     Confirmed via DMM beep-test and microscope inspection.
//   - LED wired with series resistor from PB0 to the adjacent GND pin
//     on the JP2-2 header. No soldering required.
#define LED_PIN PB0

int main()
{
	SystemInit();
	funGpioInitAll();   // Enable APB2 clock for GPIOA, GPIOB, GPIOC

	funPinMode( LED_PIN, GPIO_Speed_10MHz | GPIO_CNF_OUT_PP );

	while(1)
	{
		funDigitalWrite( LED_PIN, FUN_HIGH );
		Delay_Ms( 250 );
		funDigitalWrite( LED_PIN, FUN_LOW );
		Delay_Ms( 250 );
	}
}
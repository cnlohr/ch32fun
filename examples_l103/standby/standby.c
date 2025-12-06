/*
A very simple example to demonstrate 560nA standby current on a CH32L103 as measured by a PPKII.

On the CH32L103C8T6 reference board, this requires switching S2 to OFF (disconnects 3.3V regulator),
removing the PWR_LED, and removing R24 and R25 (0 ohm resistors that connect PB6 (CC1) and PB7 (CC2) pins to USB and
5.1K resistors to GND).

Unlike the CH32V003, it appears that in deep sleep all GPIOs go to analog input (high-z) state,
regardless of GPIO settings. Therefore a higher level sleep is needed for active output.

All pins except PB6 and PB7 that is ... The PDUSB pins have an 800K internal pull-up resistor (datasheet says
a pull-down resistor) and continue to output when in deep sleep mode. So to get the minimum standby current and
safely use the pins, make sure they don't have a path to GND or send voltage to an unpowered device.

In addition, while IWDG can reset the chip from higher levels of sleep, for standby it appears that the option byte
STANDY_RST must be enabled (set to 0). This is still untested.

In order to reset the chip with the below code, either power it off or toggle S1 (hardware reset)
*/

#include "ch32fun.h"
#include <stdio.h>

#define LED_PIN PB6

void blink_led(int time) {
	for (int i = 0; i < time; i++) {
		funDigitalWrite(LED_PIN, 1);
		Delay_Ms( 50 );
		funDigitalWrite(LED_PIN, 0);
		Delay_Ms( 50 );
	}
}

int main() {
	SystemInit();
	Delay_Ms(100);
	funGpioInitAll();

	printf("\n~ Standby Test ~\n");
	funPinMode(LED_PIN, GPIO_CFGLR_OUT_10Mhz_PP);
	blink_led(5);

	//! WARNING: need 5 seconds to allow reprogramming on power up
	//! DONT set GPIOA->CFGHR to pullup before the 5 seconds delay because they are used for SWCLK and SWDIO
	// if you want to get around this 5s delay you can configure a GPIO input and check it to determine
	// if it's in bypass mode so you can reprogram it
	Delay_Ms(5000);

	// Enable PWR clock
	RCC->APB1PCENR |= RCC_APB1Periph_PWR;

	// Disable flash, ensure no ram retention, etc.
	PWR->CTLR = PWR_CTLR_FLASH_LP_REG | PWR_CTLR_FLASH_LP_1 | PWR_CTLR_PDDS;
	// Set standby mode to deep sleep
	NVIC->SCTLR |= (1 << 2);

	__WFI();

	while(1);
}

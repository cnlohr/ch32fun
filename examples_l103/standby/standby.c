/*
A very simple example to demonstrate 560nA standby current on a CH32L103 as measured by a PPKII.

On the CH32L103C8T6 reference board, this requires switching S2 to OFF (disconnects 3.3V regulator),
removing the power LED (D0), and removing R24 and R25 (0 ohm resistors that connect PB6 (CC1) and PB7 (CC2) pins to USB and
5.1K resistors to GND).

Unlike the CH32V003, it appears that in deep sleep all GPIOs go to analog input (high-z) state,
regardless of GPIO settings. Therefore a higher level sleep is needed for active output.

All pins except PB6 and PB7 that is ... the PDUSB pins have an 800K internal pull-up resistor (datasheet says
a pull-down resistor) and continue to output when in deep sleep mode. So to get the minimum standby current and
safely use the pins, make sure they don't have a path to GND or send voltage to an unpowered device.

In addition, while IWDG can reset the chip from higher levels of sleep, for standby it appears that the option byte
STANDY_RST must be enabled (set to 0). This is still untested.

In order to reset the chip with the below code, either power it off or toggle S1 (hardware reset).
*/

#include "ch32fun.h"
#include <stdio.h>
#include <stdbool.h>

#define RTC_CLOCK_SOURCE_LSI
#define RTC_CLOCK_TICKS_PER_INCREMENT (RTC_CLOCK_TICKS_PER_SECOND / 1000) // ~ one ms resolution
#include "rtc.h"

#define LED_PIN PB6

void blink_led(int blinks) {
	for (int i = 0; i < blinks; i++) {
		funDigitalWrite(LED_PIN, 1);
		Delay_Ms( 100 );
		funDigitalWrite(LED_PIN, 0);
		Delay_Ms( 100 );
	}
}

bool is_reset_source_standby_exit() {
	uint32_t csr = PWR->CSR;
	return csr & PWR_CSR_WUF && csr & PWR_CSR_SBF;
}

int main() {
	SystemInit();
	funGpioInitAll();

	// Enable access to the RTC and backup registers
	RCC->APB1PCENR |= RCC_PWREN | RCC_BKPEN;
	PWR->CTLR |= PWR_CTLR_DBP;

	funPinMode(LED_PIN, GPIO_CFGLR_OUT_10Mhz_PP);
	funDigitalWrite(LED_PIN, 0);

	// minichlink monitor freaks out after standby, so printfs aren't very useful in practice
	printf("\n~ Standby Test ~\n");

	// LED blinks work though
	//blink_led(5);

	if (!is_reset_source_standby_exit()) {
		//! WARNING: need 5 seconds to allow reprogramming on power up
		//! DONT set GPIOA->CFGHR to pullup before the 5 seconds delay because they are used for SWCLK and SWDIO
		// if you want to get around this 5s delay you can configure a GPIO input and check it to determine
		// if it's in bypass mode so you can reprogram it
		Delay_Ms(5000);
	} 

	RTC_init();

	// clear wakeup flags before next standby
	PWR->CTLR |= PWR_CTLR_CWUF | PWR_CTLR_CSBF;

	// Disable flash, ensure no ram retention, etc.	
	PWR->CTLR &=~ (PWR_RAMLV | PWR_CTLR_R18KVBAT | PWR_CTLR_R2KVBAT | PWR_CTLR_R18KSTY | 
					PWR_CTLR_R2KSTY | PWR_CTLR_LDO_EC | PWR_CTLR_AUTO_LDO_EC | PWR_CTLR_FLASH_LP |
					PWR_CTLR_PVDE );
	PWR->CTLR |= PWR_CTLR_FLASH_LP_REG | PWR_CTLR_FLASH_LP_1 | PWR_CTLR_PDDS;
	// Set standby mode to deep sleep
	NVIC->SCTLR |= (1 << 2);

	RTC_setAlarmRelative(0, 50); // 50 ms wakeup period. Still works down to 1ms!
	__WFI();
}

// Send message to iSLER_sensors_gateway

#include "ch32fun.h"
#include <stdio.h>

#include "ch5xx_sleep.h"
#include "ch5xx_Mess.h"

#define SLEEPTIME_MS 3000
#define LED PA8

#define SHUTDOWN_MODE_ENABLED

remote_command_t button_cmd = {
	.command = 0xAA,
	.value1 = 11
};

#ifdef SHUTDOWN_MODE_ENABLED
	int main() {
		SystemInit();

		ch5xx_setClock(CLK_SOURCE_HSE_4MHz);

		// Turn on the LED - for WeAct board PA8 is active LOW
		funPinMode(PA8, GPIO_CFGLR_OUT_2Mhz_PP);
		funDigitalWrite(PA8, 0);

		DCDCEnable(); // Enable the internal DCDC
		LSIEnable(); // Disable LSE, enable LSI

		// Turn off the LED by doing pullup
		ch5xx_allPinsPullUp();
		ch5xx_sleep_rtc_init();
		DCDCEnable(); // Enable the internal DCDC
		LSIEnable(); // Disable LSE, enable LSI

		RFCoreInit(LL_TX_POWER_0_DBM);
		chMess_advertise(&button_cmd);

		// Disconnect PA15 from GND to enter power_down mode. PA15 Pull up will use less power
		//! WARNING: PA15 NEEDS TO BE IN PULLED UP MODE OR YOU WONT BE ABLE TO REFLASH THE CHIP
		//! without access to the boot pin PB22
		if (funDigitalRead(PA15)) {
			ch5xx_sleep_powerDown( MS_TO_RTC(SLEEPTIME_MS), (RB_PWR_RAM2K) );
		} 
		else {
			funPinMode(LED, GPIO_CFGLR_OUT_10Mhz_PP);

			while (1) {
				funDigitalWrite(LED, 0); Delay_Ms(50);
				funDigitalWrite(LED, 1); Delay_Ms(50);
			}
		}	
	}

#else
	int main() {
		SystemInit();

		printf("Size of message: %d\n", sizeof(iSLER_frame_t));

		ch5xx_setClock(CLK_SOURCE_HSE_4MHz);

		ch5xx_sleep_rtc_init();
		LSIEnable(); // Disable LSE, enable LSI

		while(1) {
			funGpioInitAll();
			funPinMode( LED, GPIO_CFGLR_OUT_2Mhz_PP );

			RFCoreInit(LL_TX_POWER_0_DBM); // RF wakes up in an odd state, we need to reinit after sleep
			DCDCEnable(); // DCDC gets disabled during sleep
			ch5xx_allPinsPullUp(); // this reduces sleep from ~70uA to 1uA

			// BLE advertisements are sent on channels 37, 38 and 39, over the 1M PHY
			for(int c = 0; c < sizeof(adv_channels); c++) {
				// Frame_TX(adv, sizeof(adv), adv_channels[c], PHY_1M);
				chMess_advertise(&button_cmd);
			}

			button_cmd.value1++;
			LowPower( MS_TO_RTC(SLEEPTIME_MS), (RB_PWR_RAM2K | RB_PWR_RAMX) ); // PWR_RAM can be optimized
		}
	}
#endif
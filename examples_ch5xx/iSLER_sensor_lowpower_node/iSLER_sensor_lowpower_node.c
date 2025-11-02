// Send message to iSLER_sensors_gateway

// When the chip is sleeping, you won't be able to flash it,
// Hold the BOOT pin while powering up the chip and flash it before it enters user mode

//! WARNING: WHEN USE SHUTDOWN_MODE_ENABLED
//! PA15 NEEDS TO BE IN PULLED UP MODE OR YOU WONT BE ABLE TO REFLASH THE CHIP
//! if you dont have access to the BOOT pin PB22
//# Ground PA15 to enter programming mode (in programming mode, PA8 led will be flashing)
//# after flashing done, unground PA15 to enter shutdown mode

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

		ch5xx_setClock(CLK_SOURCE_PLL_60MHz);

		//# Turn on the LED - for WeAct board PA8 is active LOW
		funPinMode(LED, GPIO_CFGLR_OUT_2Mhz_PP);
		funDigitalWrite(LED, 0);

		ch5xx_sleep_rtc_init();
		DCDCEnable(); // Enable the internal DCDC
		LSIEnable(); // Disable LSE, enable LSI

		//# Turn off the LED by doing pullup
		ch5xx_allPinsPullUp();

		RFCoreInit(LL_TX_POWER_0_DBM);
		chMess_advertise(&button_cmd);

		//# Disconnect PA15 from GND to enter power_down mode. PA15 Pull up will use less power
		if (funDigitalRead(PA15)) {
			ch5xx_sleep_powerDown( MS_TO_RTC(SLEEPTIME_MS), (RB_PWR_RAM2K) );
		} 
		else {
			funPinMode(LED, GPIO_CFGLR_OUT_10Mhz_PP);

			while (1) {
				if (funDigitalRead(PA15)) {
					ch5xx_sleep_powerDown( MS_TO_RTC(SLEEPTIME_MS), (RB_PWR_RAM2K) );
				}
				funDigitalWrite(LED, 0); Delay_Ms(50);
				funDigitalWrite(LED, 1); Delay_Ms(50);
			}
		}	
	}

#else
	int main() {
		SystemInit();
		funGpioInitAll();

		printf("Size of message: %d\n", sizeof(iSLER_frame_t));

		// ch5xx_setClock(CLK_SOURCE_HSE_4MHz);

		ch5xx_sleep_rtc_init();
		LSIEnable(); // Disable LSE, enable LSI

		while(1) {
			funPinMode( LED, GPIO_CFGLR_OUT_2Mhz_PP );

			RFCoreInit(LL_TX_POWER_0_DBM); // RF wakes up in an odd state, we need to reinit after sleep
			DCDCEnable(); // DCDC gets disabled during sleep
			ch5xx_allPinsPullUp(); // this reduces sleep from ~70uA to 1uA

			// BLE advertisements are sent on channels 37, 38 and 39, over the 1M PHY
			for(int c = 0; c < sizeof(adv_channels); c++) {
				chMess_advertise(&button_cmd);
			}

			button_cmd.value1++;
			// Delay_Ms(1000);
			LowPower( MS_TO_RTC(SLEEPTIME_MS), (RB_PWR_RAM2K | RB_PWR_RAMX) ); // PWR_RAM can be optimized
		}
	}
#endif
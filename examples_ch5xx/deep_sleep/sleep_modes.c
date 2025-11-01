#include "ch32fun.h"
#include <stdio.h>

#include "ch5xx_sleep.h"
#include "ch5xx_iSLER.h"

#define SLEEPTIME_MS 3000
#define LED PA8

remote_command_t button_cmd = {
	.command = 0xAA,
	.value1 = 11
};

int main2() {
    SystemInit();

	ch5xx_setClock(CLK_SOURCE_HSE_4MHz);

	DCDCEnable(); // Enable the internal DCDC
	LSIEnable(); // Disable LSE, enable LSI

	funPinMode(PA8, GPIO_CFGLR_OUT_2Mhz_PP);
	funDigitalWrite(PA8, 0);

	ch5xx_allPinsPullUp();
	ch5xx_sleep_rtc_init();

	RFCoreInit(LL_TX_POWER_0_DBM);
	fun_iSLER_adv_command(&button_cmd);

	// Disconnect PA15 from GND to enter power_down mode
	//! WARNING: PA15 NEEDS TO BE IN PULLED UP MODE OR YOU WONT BE ABLE TO REFLASH THE CHIP
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


void blink(int n) {
	for(int i = n-1; i >= 0; i--) {
		funDigitalWrite( LED, FUN_LOW ); // Turn on LED
		LowPowerIdle( MS_TO_RTC(33) );
		funDigitalWrite( LED, FUN_HIGH ); // Turn off LED
		if(i) LowPowerIdle( MS_TO_RTC(33) );
	}
}


iSLER_frame_t frame = {
	.mac = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66},
	.field_adv_flags = {0x02, 0x01, 0x06},
	.name_len = 21,
	.ad_type_local_name = 0x09,
	.name = { 'b','e', 'e', '-', '5', '5', '5' },
	.data_len = sizeof(MESS_DataFrame_t) + 3,
	.field_sev_data = {0xFF, 0xD7, 0x07},
};

uint8_t adv[] = {0x66, 0x55, 0x44, 0x33, 0x22, 0x11, // MAC (reversed)
				 0x03, 0x19, 0x00, 0x00, // 0x19: "Appearance", 0x00, 0x00: "Unknown"
				 0x08, 0x09, 'c', 'h', '3', '2', 'f', 'u', '2'}; // 0x09: "Complete Local Name"

int main() {
	SystemInit();

	ch5xx_setClock(CLK_SOURCE_HSE_4MHz);

	ch5xx_sleep_rtc_init();

	while(1) {
		funGpioInitAll();
		funPinMode( LED, GPIO_CFGLR_OUT_2Mhz_PP );

		RFCoreInit(LL_TX_POWER_0_DBM); // RF wakes up in an odd state, we need to reinit after sleep
		DCDCEnable(); // DCDC gets disabled during sleep

		ch5xx_allPinsPullUp(); // this reduces sleep from ~70uA to 1uA

		// BLE advertisements are sent on channels 37, 38 and 39, over the 1M PHY
		for(int c = 0; c < sizeof(adv_channels); c++) {
			Frame_TX(adv, sizeof(adv), adv_channels[c], PHY_1M);
		}

		LowPower( MS_TO_RTC(SLEEPTIME_MS), (RB_PWR_RAM2K | RB_PWR_RAMX | RB_PWR_EXTEND) ); // PWR_RAM can be optimized
	}
}

/* 
 * Minimal demo of iSLER used in a low power BLE beacon
 * It advertises just a "Complete Local Name", on a user defined MAC address.
 * For more info on the lowpower part or the iSLER part please refer
 * to their respective demos.
 */

#include "ch32fun.h"
#include "iSLER.h"
#include <stdio.h>

#ifdef CH570_CH572
#define LED PA9
#else
#define LED PA8
#endif

#define SLEEPTIME_MS 300

// The advertisement to be sent. The MAC address should be in the first 6 bytes in reversed byte order,
// after that any BLE flag can be used.
uint8_t adv[] = {0x66, 0x55, 0x44, 0x33, 0x22, 0x11, // MAC (reversed)
				 0x03, 0x19, 0x00, 0x00, // 0x19: "Appearance", 0x00, 0x00: "Unknown"
				 0x08, 0x09, 'c', 'h', '3', '2', 'f', 'u', 'n'}; // 0x09: "Complete Local Name"
uint8_t adv_channels[] = {37,38,39};

__attribute__((interrupt))
void RTC_IRQHandler(void) {
	// clear trigger flag
	R8_RTC_FLAG_CTRL = RB_RTC_TRIG_CLR;
}

void allPinPullUp(void)
{
	R32_PA_DIR = 0; //Direction input
	R32_PA_PD_DRV = 0; //Disable pull-down
	R32_PA_PU = P_All; //Enable pull-up
#if PB
	R32_PB_DIR = 0; //Direction input
	R32_PB_PD_DRV = 0; //Disable pull-down
	R32_PB_PU = P_All; //Enable pull-up
#endif
}

void blink(int n) {
	for(int i = n-1; i >= 0; i--) {
		funDigitalWrite( LED, FUN_LOW ); // Turn on LED
		LowPowerIdle( MS_TO_RTC(33) );
		funDigitalWrite( LED, FUN_HIGH ); // Turn off LED
		if(i) LowPowerIdle( MS_TO_RTC(33) );
	}
}

int main() {
	SystemInit();

	DCDCEnable(); // Enable the internal DCDC
	LSIEnable(); // Disable LSE, enable LSI
	RTCInit(); // Set the RTC counter to 0
	SleepInit(); // Enable wakeup from sleep by RTC, and enable RTC IRQ

	allPinPullUp(); // this reduces sleep from ~70uA to 1uA

	funGpioInitAll();
	funPinMode( LED, GPIO_CFGLR_OUT_2Mhz_PP );

	uint8_t txPower = LL_TX_POWER_0_DBM;
	RFCoreInit(txPower);

	blink(5);
	printf(".~ ch32fun BLE beacon ~.\n");

	while(1) {
		// BLE advertisements are sent on channels 37, 38 and 39, over the 1M PHY
		for(int c = 0; c < sizeof(adv_channels); c++) {
			Frame_TX(adv, sizeof(adv), adv_channels[c], PHY_1M);
		}

		LowPower( MS_TO_RTC(SLEEPTIME_MS), (RB_PWR_RAM2K | RB_PWR_RAMX | RB_PWR_EXTEND) ); // PWR_RAM can be optimized

		RFCoreInit(txPower); // RF wakes up in an odd state, we need to reinit after sleep
		DCDCEnable(); // DCDC gets disabled during sleep
		blink(1);
	}
}

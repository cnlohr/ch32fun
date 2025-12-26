/* 
 * Minimal demo of iSLER with transmit and receive, on configurable PHY (1M, 2M, S2 or S8 if supported by the mcu)
 * It listens for advertisements from other BLE devices, and when one is detected it
 * changes it's own "Complete Local Name" to RX:XX where XX is the first byte of the detected BLE device's MAC.
 * The RX process happens on channel 37 AccessAddress 0x8E89BED6, which is defined in extralibs/iSLER.h.
 * When a new frame is received, the callback "incoming_frame_handler()" is called to process it.
 */
#include "ch32fun.h"
#include "iSLER.h"
#include <stdio.h>

#ifdef CH570_CH572
#define LED PA9
#else
#define LED PA8
#endif
#define PHY_MODE       PHY_1M
#define ACCESS_ADDRESS 0x8E89BED6 // the "BED6" address for BLE advertisements

#define REPORT_ALL 1 // if 0 only report received Find My advertisements

__attribute__((aligned(4))) uint8_t adv[] = {
		0x02, 0x0d, // header for LL: PDU + frame length
		0x66, 0x55, 0x44, 0x33, 0x22, 0x11, // MAC (reversed)
		0x06, 0x09, 'R', 'X', ':', '?', '?'}; // 0x09: "Complete Local Name"

// BLE advertisements are sent on channels 37, 38 and 39
uint8_t adv_channels[] = {37,38,39};

uint8_t hex_lut[] = "0123456789ABCDEF";

void blink(int n) {
	for(int i = n-1; i >= 0; i--) {
		funDigitalWrite( LED, FUN_LOW ); // Turn on LED
		Delay_Ms(33);
		funDigitalWrite( LED, FUN_HIGH ); // Turn off LED
		if(i) Delay_Ms(33);
	}
}

void incoming_frame_handler() {
	// The chip stores the incoming frame in LLE_BUF, defined in extralibs/iSLER.h
	uint8_t *frame = (uint8_t*)LLE_BUF;
	int rssi = iSLERRSSI();

	// The first two bytes of the frame are metadata with PDU and length
	printf("RSSI:%d PDU:%d len:%d MAC:", rssi, frame[0], frame[1]);
	
	for(int i = 7; i > 2; i--) {
		printf("%02x:", frame[i]);
	}
	printf("%02x data:", frame[2]);
	for(int i = 8; i < frame[1] +2; i++) {
		printf("%02x ", frame[i]);
	}
	printf("\n");

	// advertise reception of a FindMy frame
	if(REPORT_ALL || (frame[8] == 0x1e && frame[10] == 0x4c)) {
		adv[sizeof(adv) -2] = hex_lut[(frame[7] >> 4)];
		adv[sizeof(adv) -1] = hex_lut[(frame[7] & 0xf)];
		for(int c = 0; c < sizeof(adv_channels); c++) {
			Frame_TX(ACCESS_ADDRESS, adv, sizeof(adv), adv_channels[c], PHY_MODE);
		}
	}
}

int main()
{
	SystemInit();

	funGpioInitAll();
	funPinMode( LED, GPIO_CFGLR_OUT_2Mhz_PP );

	iSLERInit(LL_TX_POWER_0_DBM);

	blink(5);
	printf(".~ ch32fun iSLER ~.\n");

	// send out a first RX:?? advertisement to show we are alive
	for(int c = 0; c < sizeof(adv_channels); c++) {
		iSLERTX(ACCESS_ADDRESS, adv, sizeof(adv), adv_channels[c], PHY_MODE);
	}

	while(1) {
		// now listen for frames on channel 37 on bed6. When the RF subsystem
		// detects and finalizes one, "rx_ready" in iSLER.h is set true
		iSLERRX(ACCESS_ADDRESS, 37, PHY_MODE);
		while(!rx_ready);

		// we stepped over !rx_ready so we got a frame
		blink(1);
		incoming_frame_handler();
	}
}

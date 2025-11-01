// Listen to messages from iSLER_sensor_lowpower_node

#include "ch32fun.h"
#include <stdio.h>
#include "../iSLER_sensor_Lowpower_node/ch5xx_Mess.h"

#define SLEEPTIME_MS 4000
#define LED PA8

uint8_t frame_info[] = {0xff, 0x10}; // PDU, len, (maybe not?) needed in RX mode

void blink(int n) {
	for(int i = n-1; i >= 0; i--) {
		funDigitalWrite( LED, FUN_LOW ); // Turn on LED
		Delay_Ms(33);
		funDigitalWrite( LED, FUN_HIGH ); // Turn off LED
		if(i) Delay_Ms(33);
	}
}

void handle_receiving_frame(uint32_t time) {
	// now listen for frames on channel 37. When the RF subsystem
	// detects and finalizes one, "rx_ready" in iSLER.h is set true
	Frame_RX(frame_info, 37, PHY_MODE);
	while(!rx_ready);
	
	// we stepped over !rx_ready so we got a frame
	remote_command_t* cmd = chMess_rx_handler();
	if (cmd) {
		blink(1);
		printf("Receiv Command: %02X\n", cmd->command);
		printf("Value1: %u\n", cmd->value1);
	}
}


int main() {
    SystemInit();

	printf("~ iSLER sensors gateway ~\n");
	RFCoreInit(LL_TX_POWER_0_DBM);

	funPinMode(PA8, GPIO_CFGLR_OUT_10Mhz_PP);
	funPinMode(PA9, GPIO_CFGLR_OUT_10Mhz_PP);

	while (1) {
		handle_receiving_frame(0);
	}
	
}
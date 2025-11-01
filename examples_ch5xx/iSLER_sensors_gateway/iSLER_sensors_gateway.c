#include "ch32fun.h"
#include <stdio.h>
#include "ch5xx_iSLER.h"

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
	remote_command_t* cmd = modiSLER_rx_handler();
	if (cmd) {
		blink(1);
		printf("Receiv Command: %02X\n", cmd->command);

		// switch (cmd->command) {
		// 	case 0xBB:
		// 		if (is_slave_device() > 0) {
		// 			Neo_loadCommand(cmd->value1);
		// 			WS2812BDMAStart(NR_LEDS);
		// 		}

		// 		break;
		// 	case 0xF1:
		// 		if (is_slave_device() > 0) {
		// 			ping_cmd.command = 0xF2;
		// 			ping_cmd.value1 = cmd->value1;
		// 			ping_cmd.value2 = cmd->value2;
		// 			delay_send = millis();
		// 		}

		// 		break;
		// 	case 0xF2:
		// 		if (is_slave_device() == 0) {
		// 			// printf("Received value1: %u, value2: %u\n", 
		// 			// 	cmd->value1, cmd->value2);
		// 			printf("time_diff: %d\n", time - cmd->value2);
		// 		}
		// 		break;
		// }
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
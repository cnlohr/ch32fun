// Example using Timer 1 capture to look for down-going events
// on PD2.  Then using DMA to capture that event and write it
// to a circular queue that can be read out later.
//
// For it to produce interesting output, you can wire PD2 to PD3.

#include "ch32fun.h"
#include <stdio.h>

#include "systick_irq.h"
#include "fun_irReceiver.h"

#define IR_PIN PD2

void on_handle_irReceiver(u16 address, u16 command) {
	printf("NEC: 0x%04X 0x%04X\r\n", address, command);
}

int main() {
	SystemInit();
	Delay_Ms(100);
	systick_init();			//! REQUIRED for millis()

	printf("\r\nIR Receiver Test.\r\n");
	funGpioInitAll();
	fun_irReceiver_init(IR_PIN);

	while(1) {
		fun_irReceiver_task(on_handle_irReceiver);
	}
}


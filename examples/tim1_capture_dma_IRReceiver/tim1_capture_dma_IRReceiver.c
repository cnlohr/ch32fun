#include "ch32fun.h"
#include <stdio.h>

#include "systick_irq.h"
#include "fun_irReceiver.h"

#define IR_RECEIVER_PIN PD2
#define IR_SENDER_PIN PD3

void on_handle_irReceiver(u16 address, u16 command) {
	printf("NEC: 0x%04X 0x%04X\r\n", address, command);
}

#define IR_TEST_RECEIVER

#ifdef IR_TEST_RECEIVER
	int main() {
		SystemInit();
		Delay_Ms(100);
		systick_init();			//! REQUIRED for millis()

		printf("\r\nIR Receiver Test.\r\n");
		funGpioInitAll();
		fun_irReceiver_init(IR_RECEIVER_PIN);

		while(1) {
			fun_irReceiver_task(on_handle_irReceiver);
		}
	}

#else
	int main() {
		SystemInit();
		Delay_Ms(100);
		systick_init();			//! REQUIRED for millis()

		printf("\r\nIR Sender Test.\r\n");
		funGpioInitAll();

		fun_irSender_init(IR_SENDER_PIN);

		while(1) {
			fun_irSender_send();
			Delay_Ms(3000);
			printf(".");
		}
	}
#endif
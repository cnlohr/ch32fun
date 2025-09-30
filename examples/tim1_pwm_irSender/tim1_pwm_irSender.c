#include "ch32fun.h"
#include <stdio.h>

#include "systick_irq.h"
#include "fun_irSender.h"

#define IR_SENDER_PIN PD0

int main() {
	SystemInit();
	Delay_Ms(100);
	systick_init();			//! REQUIRED for millis()

	printf("\r\nIR Sender Test.\r\n");
	funGpioInitAll();

	u8 mode = fun_irSender_init(IR_SENDER_PIN);
	printf("Mode: %s\r\n", mode ? "PWM" : "Manual GPIO");

	u32 time_ref = millis();
	fun_irSender_send(0x00FF, 0xA56D);

	while(1) {
		if ((millis() - time_ref) > 3000) {
			fun_irSender_send(0x00FF, 0xA56D);
			time_ref = millis();
			printf(".");
		}
	}
}
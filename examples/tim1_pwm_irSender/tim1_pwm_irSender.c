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
	
	while(1) {
		fun_irSender_send();
		Delay_Ms(3000);
		printf(".");
	}
}
// Simple ADC example to read temperature, battery voltage, and ADC channel 0 (PA4)
#include "ch32fun.h"
#include "systick_irq.h"
#include "fun_uart_ch5xx.h"

// R32_UART0_CTRL: RX PB4, TX PB7
// R32_UART1_CTRL: RX PA8, TX PA9
// R32_UART2_CTRL: RX PA6, TX PA7
// R32_UART3_CTRL: RX PA4, TX PA5

#define TARGET_UART &R32_UART3_CTRL

int main() {
	SystemInit();
	systick_init();		//! REQUIRES for millis()
	funGpioInitAll();

	printf("~CH5xx UART test~\r\n");
	uart_init_ch5xx(TARGET_UART, FUNCONF_UART_PRINTF_BAUD);

	u8 i = 0;
	char msg[] = "hello bee 123\r\n";
	char recv_msg[32] = {0};

	u32 time_ref = millis();

	while(1) {
		if (millis() - time_ref > 1000) {
			time_ref = millis();
			uart_send_ch5xx(TARGET_UART, msg, sizeof(msg));
		}

		u16 receiv_len = uart_receive_ch5xx(TARGET_UART, &recv_msg, sizeof(recv_msg));
		if (receiv_len > 0) {
			printf("received: %s\r\n", recv_msg);
			memset(recv_msg, 0, sizeof(recv_msg));
		}
	}
}
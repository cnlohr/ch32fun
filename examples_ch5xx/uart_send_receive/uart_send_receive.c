#include "ch32fun.h"
#include "fun_uart_ch5xx.h"

// R32_UART0_CTRL: RX PB4, TX PB7
// R32_UART1_CTRL: RX PA8, TX PA9
// R32_UART2_CTRL: RX PA6, TX PA7
// R32_UART3_CTRL: RX PA4, TX PA5

#define TARGET_UART UART0

int main() {
	SystemInit();
	funGpioInitAll();

	printf("~CH5xx UART test~\r\n");
	uart_init_ch5xx(TARGET_UART, FUNCONF_UART_PRINTF_BAUD);

	u8 i = 0;
	char send_msg[] = "hello bee 1234\r\n";
	char recv_msg[32] = {0};

	u32 time_ref = 0;

	while(1) {
		if (TimeElapsed32(SysTick->CNT, time_ref) > DELAY_MSEC_COUNT(1000)) {
			time_ref = SysTick->CNT;
			printf("send: %s\r\n", send_msg);
			sprintf(send_msg, "hello bee %d\r\n", i++);
			uart_send_ch5xx(TARGET_UART, send_msg, sizeof(send_msg));
		}

		u16 receiv_len = uart_receive_ch5xx(TARGET_UART, &recv_msg, sizeof(recv_msg));
		if (receiv_len > 0) {
			printf("received: %s\r\n", recv_msg);
			memset(recv_msg, 0, sizeof(recv_msg));
		}
	}
}

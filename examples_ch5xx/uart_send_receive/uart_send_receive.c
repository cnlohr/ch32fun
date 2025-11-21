#define FUNCONF_SYSTICK_USE_HCLK 1

// Simple ADC example to read temperature, battery voltage, and ADC channel 0 (PA4)
#include "ch32fun.h"
#include "fun_uart_ch5xx.h"

// R32_UART0_CTRL: RX PB4, TX PB7
// R32_UART1_CTRL: RX PA8, TX PA9
// R32_UART2_CTRL: RX PA6, TX PA7
// R32_UART3_CTRL: RX PA4, TX PA5

#define DELAY_SEC_TIME(n) (DELAY_MS_TIME * 1000 * n)

#define TARGET_UART &R32_UART3_CTRL

u32 get_tickCount() { return SysTick->CNT; }

void SysTick_Start() {
	SysTick->CTLR = 0x0000;
	// Set the compare register
	SysTick->CMP = 0xFFFFFFFF;
	// Reset the Count Register, and the global millis counter to 0
	SysTick->CNT = 0x00000000;
	// Set the SysTick Configuration
	SysTick->CTLR |= SYSTICK_CTLR_STE |  // Enable Counter
					SYSTICK_CTLR_STCLK ;  // Set Clock Source to HCLK/1	
}

int main() {
	SystemInit();
	SysTick_Start();
	funGpioInitAll();

	printf("~CH5xx UART test~\r\n");
	uart_init_ch5xx(TARGET_UART, FUNCONF_UART_PRINTF_BAUD);

	u8 i = 0;
	char msg[] = "hello bee 123\r\n";
	char recv_msg[32] = {0};

	u32 time_ref = get_tickCount();

	while(1) {
		if (TimeElapsed32(get_tickCount(), time_ref) > DELAY_SEC_TIME(1)) {
			time_ref = get_tickCount();
			printf("send: %s\r\n", msg);
			uart_send_ch5xx(TARGET_UART, msg, sizeof(msg));
		}

		u16 receiv_len = uart_receive_ch5xx(get_tickCount(), TARGET_UART, &recv_msg, sizeof(recv_msg));
		if (receiv_len > 0) {
			printf("received: %s\r\n", recv_msg);
			memset(recv_msg, 0, sizeof(recv_msg));
		}
	}
}
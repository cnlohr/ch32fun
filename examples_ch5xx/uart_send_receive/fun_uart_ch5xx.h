// MIT License
// Copyright (c) 2025 UniTheCat

#include "ch32fun.h"

#define FUNCONF_UART_PRINTF_BAUD 115200

void uart_init_ch5xx(UART_Typedef *UARTx, int baudrate) {
	//# Configure GPIOs for CH582
	if (UARTx == UART0) {
		funPinMode(PB4, GPIO_CFGLR_IN_PU);		   // RX0 (PB4)
		funPinMode(PB7, GPIO_CFGLR_OUT_2Mhz_PP);	 // TX0 (PB7)
		printf("UART0 initialized\r\n");
	}
	else if (UARTx == UART1) {
		funPinMode(PA8, GPIO_CFGLR_IN_PU);		   // RX1 (PA8)
		funPinMode(PA9, GPIO_CFGLR_OUT_2Mhz_PP);	 // TX1 (PA9)
		printf("UART1 initialized\r\n");
	}
	else if (UARTx == UART2) {
		funPinMode(PA6, GPIO_CFGLR_IN_PU);		   // RX2 (PA6)
		funPinMode(PA7, GPIO_CFGLR_OUT_2Mhz_PP);	 // TX2 (PA7)
		printf("UART2 initialized\r\n");
	}
	else if (UARTx == UART3) {
		funPinMode(PA4, GPIO_CFGLR_IN_PU);		   // RX3 (PA4)
		funPinMode(PA5, GPIO_CFGLR_OUT_2Mhz_PP);	 // TX3 (PA5)
		printf("UART3 initialized\r\n");
	}
	else {
		return; // Invalid UART
	}

	//# TXD enabled
	UARTx->IER = RB_IER_TXD_EN;

	//# FIFO Control register
	UARTx->FCR = RB_FCR_FIFO_EN | RB_FCR_TX_FIFO_CLR | RB_FCR_RX_FIFO_CLR |
										// Trigger points select of receiving FIFO: 4 bytes
										(0b10 << 6);

	//# Line Control register
	UARTx->LCR = RB_LCR_WORD_SZ;	// word length: 8 bits

	//# Baud rate = Fsys * 2 / R8_UART0_DIV / 16 / R16_UART0_DL
	u8 divider = 1;
	UARTx->DL = FUNCONF_SYSTEM_CORE_CLOCK / (8 * baudrate * divider);

	//# Prescaler divisor
	UARTx->DIV = divider;
}

void uart_send_ch5xx(UART_Typedef *UARTx, u8 *buf, u16 len) {
	for (int i = 0; i < len; i++) {
		while (!(UARTx->LSR & RB_LSR_TX_ALL_EMP));
		UARTx->THR_RBR = buf[i];
	}
}

//! NOTE: if you are debugging with printf, note that it introduces a delay   
u16 uart_receive_ch5xx( UART_Typedef *UARTx, u8 *buf, u16 max_len) {
	u16 len = 0;
	u32 start_time = SysTick->CNT;

	while (len < max_len -1) {
		while (UARTx->RFC && len < max_len - 1) {
			buf[len++] = UARTx->THR_RBR;
			start_time = SysTick->CNT;
		}

		if (SysTick->CNT - start_time > 1000) break;
	}

	buf[len] = '\0';
	return len;
}
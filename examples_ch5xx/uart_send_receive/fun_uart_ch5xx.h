
// MIT License
// Copyright (c) 2025 UniTheCat

#include "ch32fun.h"

#define FUNCONF_UART_PRINTF_BAUD 115200

#define GET_REG8(base, offset)  (*(vu8 *)((vu32)base + offset))
#define GET_REG16(base, offset) (*(vu16 *)((vu32)base + offset))

void uart_init_ch5xx(vu32 *uart_ctrl, int baudrate) {
	//# Configure GPIOs for CH582
    if (uart_ctrl == &R32_UART0_CTRL) {
        funPinMode(PB4, GPIO_CFGLR_IN_PU);           // RX0 (PB4)
        funPinMode(PB7, GPIO_CFGLR_OUT_2Mhz_PP);     // TX0 (PB7)
		printf("UART0 initialized\r\n");
    }
    else if (uart_ctrl == &R32_UART1_CTRL) {
        funPinMode(PA8, GPIO_CFGLR_IN_PU);           // RX1 (PA8)
        funPinMode(PA9, GPIO_CFGLR_OUT_2Mhz_PP);     // TX1 (PA9)
		printf("UART1 initialized\r\n");
    }
    else if (uart_ctrl == &R32_UART2_CTRL) {
        funPinMode(PA6, GPIO_CFGLR_IN_PU);           // RX2 (PA6)
        funPinMode(PA7, GPIO_CFGLR_OUT_2Mhz_PP);     // TX2 (PA7)
		printf("UART2 initialized\r\n");
    }
    else if (uart_ctrl == &R32_UART3_CTRL) {
        funPinMode(PA4, GPIO_CFGLR_IN_PU);           // RX3 (PA4)
        funPinMode(PA5, GPIO_CFGLR_OUT_2Mhz_PP);     // TX3 (PA5)
		printf("UART3 initialized\r\n");
    }
    else {
        return; // Invalid UART
    }

	//# TXD enabled
	GET_REG8(uart_ctrl, UART_IER) = RB_IER_TXD_EN;

	//# FIFO Control register
	GET_REG8(uart_ctrl, UART_FCR) = RB_FCR_FIFO_EN | RB_FCR_TX_FIFO_CLR | RB_FCR_RX_FIFO_CLR |
										// Trigger points select of receiving FIFO: 4 bytes
										(0b10 << 6);

	//# Line Control register
	GET_REG8(uart_ctrl, UART_LCR) = RB_LCR_WORD_SZ;		// word length: 8 bits

	//# Baud rate = Fsys * 2 / R8_UART0_DIV / 16 / R16_UART0_DL
	u8 divider = 1;
	GET_REG16(uart_ctrl, UART_DLL) = FUNCONF_SYSTEM_CORE_CLOCK / (8 * baudrate * divider);

	//# Prescaler divisor
	GET_REG8(uart_ctrl, UART_DIV) = divider;
}

void uart_send_ch5xx(vu32 *uart_ctrl, u8 *buf, u16 len) {
	for (int i = 0; i < len; i++) {
		while(!(GET_REG8(uart_ctrl, UART_LSR) & RB_LSR_TX_ALL_EMP));
		GET_REG8(uart_ctrl, UART_THR) = buf[i];
	}
}

u32 time_ref = 0;

u16 uart_receive_ch5xx(u32 time, vu32 *uart_ctrl, u8 *buf, u16 max_len) {
    u16 len = 0;

	//! NOTE: if you are debugging with printf, note that it introduces a delay
    while (len < max_len - 1) {
        while (GET_REG8(uart_ctrl, UART_RFC) && len < max_len - 1) {
            buf[len++] = GET_REG8(uart_ctrl, UART_RBR);
            time_ref = time;	// Reset timeout
        }
        
        // Wait 1ms to see if more data arrives. If no data for 1ms, assume message is complete
		// the wait time may need to be adjusted depending on the baudrate
        if ((time - time_ref) > 1) { break;}
    }
    
    buf[len] = '\0';
    return len;
}
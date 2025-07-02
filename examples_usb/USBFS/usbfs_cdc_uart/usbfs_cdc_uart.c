#include "ch32fun.h"
#include <stdio.h>
#include <string.h>
#include "fsusb.h"

#define UART_TX_PIN PA2
#define UART_RX_PIN PA3
#include "uart.h"

#ifndef CH32V10x
#define SYSTICK_DIV 1
#else
#define SYSTICK_DIV 8
#endif
#define SYSTICK_ONE_MILLISECOND ((uint32_t)FUNCONF_SYSTEM_CORE_CLOCK / SYSTICK_DIV / 1000)
#if !defined(CH32V10x) && !defined (CH32X03x)
volatile uint32_t millis_cnt = 0;
#else
volatile uint64_t millis_cnt = 0;
#endif
volatile char terminal_input;
extern volatile uint8_t usb_debug;
extern volatile uint8_t uart_debug;

void handle_debug_input( int numbytes, uint8_t * data )
{
	terminal_input = data[0];
}

int HandleInRequest( struct _USBState * ctx, int endp, uint8_t * data, int len )
{
	int ret = 0;  // Just NAK
	if (usb_debug) printf("HandleInRequest - EP%d\n", endp);
	switch (endp)
	{
		case 1:
			// ret = -1; // Just ACK
			break;
		case 3:
			cdc.rxing = 0;
			// ret = -1; // ACK, without it RX was stuck in some cases, leaving for now as a reminder
			break;
	}
	return ret;
}

void HandleDataOut( struct _USBState * ctx, int endp, uint8_t * data, int len )
{
	if (usb_debug) printf("HandleDataOut - EP%d\n", endp);
	if( endp == 0 )
	{
		ctx->USBFS_SetupReqLen = 0; // To ACK
		if( ctx->USBFS_SetupReqCode == CDC_SET_LINE_CODING )
		{
			if (usb_debug) printf("CDC_SET_LINE_CODING\n");
			/* Save relevant parameters such as serial port baud rate */
			/* The downlinked data is processed in the endpoint 0 OUT packet, the 7 bytes of the downlink are, in order
				4 bytes: baud rate value: lowest baud rate byte, next lowest baud rate byte, next highest baud rate byte, highest baud rate byte.
				1 byte: number of stop bits (0: 1 stop bit; 1: 1.5 stop bit; 2: 2 stop bits).
				1 byte: number of parity bits (0: None; 1: Odd; 2: Even; 3: Mark; 4: Space).
				1 byte: number of data bits (5,6,7,8,16); */

			// cdc.cdc_cfg[0] = CTRL0BUFF[0];
			// cdc.cdc_cfg[1] = CTRL0BUFF[1];
			// cdc.cdc_cfg[2] = CTRL0BUFF[2];
			// cdc.cdc_cfg[3] = CTRL0BUFF[3];
			// cdc.cdc_cfg[4] = CTRL0BUFF[4];
			// cdc.cdc_cfg[5] = CTRL0BUFF[5];
			// cdc.cdc_cfg[6] = CTRL0BUFF[6];
			// uint32_t baud = CTRL0BUFF[0];
			// baud += ((uint32_t)CTRL0BUFF[1] << 8);
			// baud += ((uint32_t)CTRL0BUFF[2] << 16);
			// baud += ((uint32_t)CTRL0BUFF[3] << 24);
			// cdc.uart->baud = baud;
			// cdc.uart->stop_bits = CTRL0BUFF[4];
			// cdc.uart->parity = CTRL0BUFF[5];
			// cdc.uart->word_length = CTRL0BUFF[6];
		}
	}
	if( endp == 2 )
	{
		if( cdc.tx_stop )
		{
			cdc.tx_stop = 0;
			return;
		}
		// printf("uart_tx_buffer: 0=%c, 1=%c, 2=%c, 3=%c\n", uart_tx_buffer[uart.tx_pos], uart_tx_buffer[uart.tx_pos + 1], uart_tx_buffer[uart.tx_pos + 2], uart_tx_buffer[uart.tx_pos + 3]);
		int pad = 4 - USBFS->RX_LEN;
		if( pad < 0 ) pad = 0;
		uint32_t write_pos = cdc.tx_pos + cdc.tx_remain + USBFS->RX_LEN;
		if( write_pos > cdc.tx_wrap_pos ) write_pos -= cdc.tx_wrap_pos;
		if( write_pos & 0x3 )
		{
			write_pos = (write_pos + 4) & ~0x3;
			USBFS_SendNAK( 2, 0 );
			// USBFS->UEP2_RX_CTRL &= ~USBFS_UEP_R_RES_MASK;
			// USBFS->UEP2_RX_CTRL |= USBFS_UEP_R_RES_NAK;
			cdc.tx_stop = 1;
		}
		// printf("[HandleDatOut] write_pos = %d, tx_pos = %d, tx_remain = %d, USBFS->RX_LEN = %d \n", write_pos, cdc.tx_pos, cdc.tx_remain, USBFS->RX_LEN);
		// if( write_pos >= uart.tx_wrap_pos) write_pos = uart.tx_remain - (uart.tx_wrap_pos - uart.tx_pos);
		// if( write_pos < uart.tx_pos && write_pos + 64 >= uart.tx_pos ) goto buffer_overflow;
		if( write_pos > ( UART_TX_BUF_SIZE - 64) )
		{
			// printf("WRAP -> write_pos = %d > 960, 961 = %c\n", write_pos, uart_tx_buffer[961]);
			cdc.tx_wrap_pos = cdc.tx_pos + cdc.tx_remain + USBFS->RX_LEN;
			write_pos = 0;
		}
		
		// if( write_pos + len > UART_TX_BUF_SIZE - 1 )
		// {
		//   uart.tx_wrap_pos = write_pos;
		//   write_pos = 0;
		// }
		USBFS->UEP2_DMA = (uint32_t)(uart_tx_buffer + write_pos);
		// printf("USBFS->UEP2_DMA = %08x\n", USBFS->UEP2_DMA);
		
		if( cdc.tx_remain >= ( UART_TX_BUF_SIZE ) )
		{
			if( usb_debug ) printf("Buffer overflow, %c\n", uart_tx_buffer[961]);
			USBFS_SendNAK( 2, 0 );
			// USBFS->UEP2_RX_CTRL &= ~USBFS_UEP_R_RES_MASK;
			// USBFS->UEP2_RX_CTRL |= USBFS_UEP_R_RES_NAK;
			cdc.tx_stop = 1;
		}
		cdc.tx_remain += USBFS->RX_LEN;
		// uart.tx_pos += USBFS->RX_LEN;
		// printf("write_pos = %d, tx_wrap_pos = %d, tx_remain = %d, txing = %d\n", write_pos, cdc.tx_wrap_pos, cdc.tx_remain, cdc.txing);
		// Prevent overwrite of the buffer
		
		// printf("\n");
		// printf("UART_TX_DMA->MADDR = %08x\n", UART_TX_DMA->MADDR);
	}
}

int HandleSetupCustom( struct _USBState * ctx, int setup_code)
{
	int ret = -1;
	if (usb_debug) printf("HandleSetupCustom - 0x%02x, len = %d\n", setup_code, ctx->USBFS_SetupReqLen);
	if( ctx->USBFS_SetupReqType & USB_REQ_TYP_CLASS )
	{
		switch( setup_code )
		{
			case CDC_SET_LINE_CODING:
			case CDC_SET_LINE_CTLSTE:
			case CDC_SEND_BREAK:
				ret = (ctx->USBFS_SetupReqLen)?ctx->USBFS_SetupReqLen:-1;

				break;
			case CDC_GET_LINE_CODING:
				ctx->pCtrlPayloadPtr = cdc.cdc_cfg;
				ret = ctx->USBFS_SetupReqLen;
				break;

			default:
				ret = 0;
				break;
		}
	}
	else if( ctx->USBFS_SetupReqType & USB_REQ_TYP_VENDOR )
	{
		/* Manufacturer request */
	}
	else
	{
		ret = 0; // Go to STALL
	}
	return ret;
}

void systick_init(void)
{

	SysTick->CTLR = 0;
#ifndef CH32V10x
	SysTick->CMP = SYSTICK_ONE_MILLISECOND - 1;
	SysTick->CNT = 0;
	SysTick->CTLR |= SYSTICK_CTLR_STE |  // Enable Counter
                  SYSTICK_CTLR_STIE |  // Enable Interrupts
#if SYSTICK_DIV==1
                  SYSTICK_CTLR_STCLK;  // Set Clock Source to HCLK/1
#else
                                    ;
#endif
#else
	uint64_t cmp_tmp = SYSTICK_ONE_MILLISECOND - 1;
	// In CH32V103 we can write systick regs only in 8bit manner
	SysTick->CMP0 = (uint8_t)cmp_tmp;
	SysTick->CMP1 = (uint8_t)(cmp_tmp >> 8);
	SysTick->CMP2 = (uint8_t)(cmp_tmp >> 16);
	SysTick->CMP3 = (uint8_t)(cmp_tmp >> 24);
	SysTick->CMP4 = (uint8_t)(cmp_tmp >> 32);
	SysTick->CMP5 = (uint8_t)(cmp_tmp >> 40);
	SysTick->CMP6 = (uint8_t)(cmp_tmp >> 48);
	SysTick->CMP7 = (uint8_t)(cmp_tmp >> 56);

	SysTick->CNT0 = 0;
	SysTick->CNT1 = 0;
	SysTick->CNT2 = 0;
	SysTick->CNT3 = 0;
	SysTick->CNT4 = 0;
	SysTick->CNT5 = 0;
	SysTick->CNT6 = 0;
	SysTick->CNT7 = 0;

	SysTick->CTLR = 1;
#endif
	
	cdc.rx_timeout = 0;
	cdc.usb_timeout = 0;
	millis_cnt = 0;

	NVIC_EnableIRQ(SysTicK_IRQn);
}

void SysTick_Handler(void) __attribute__((interrupt));
void SysTick_Handler(void)
{
#ifndef CH32V10x
	SysTick->CMP += SYSTICK_ONE_MILLISECOND;
	SysTick->SR = 0;
#else
	uint64_t cmp_tmp = SysTick->CMP + SYSTICK_ONE_MILLISECOND;
	
	SysTick->CMP0 = (uint8_t)cmp_tmp;
	SysTick->CMP1 = (uint8_t)(cmp_tmp >> 8);
	SysTick->CMP2 = (uint8_t)(cmp_tmp >> 16);
	SysTick->CMP3 = (uint8_t)(cmp_tmp >> 24);
	SysTick->CMP4 = (uint8_t)(cmp_tmp >> 32);
	SysTick->CMP5 = (uint8_t)(cmp_tmp >> 40);
	SysTick->CMP6 = (uint8_t)(cmp_tmp >> 48);
	SysTick->CMP7 = (uint8_t)(cmp_tmp >> 56);
#endif
	cdc.rx_timeout++;
	cdc.usb_timeout++;
	millis_cnt++;
}

int main()
{
	SystemInit();
	systick_init();

	funGpioInitAll();
	RCC->AHBPCENR = RCC_AHBPeriph_SRAM | RCC_AHBPeriph_DMA1;
	RCC->APB1PCENR |= RCC_APB1Periph_USART2;  // Change to an appropriate bit if you are using another UART
	
	printf("Starting\n");

#if defined (CH32V203F8)
#warning This package has USB IO on the same pins as SWD. SWD will be disabled after 3s delay
	printf("disabling SWD in 3s\n");
	Delay_Ms(3000);
	AFIO->PCFR1 |= AFIO_PCFR1_SWJ_CFG_DISABLE;
#endif
	
	funPinMode( UART_TX_PIN, GPIO_Speed_10MHz | GPIO_CNF_OUT_PP_AF );
	funPinMode( UART_RX_PIN, GPIO_Speed_10MHz | GPIO_CNF_OUT_PP_AF );

	cdc_init(&cdc);
	printf("Started UART%d at 0x%08lx\n", cdc.uart->number, (uint32_t)UART(cdc.uart->number));
	USBFSSetup();
	printf("Started USB\n\n");
	printf("You can enable debug messages:\n");
	printf("Type 'd' for USB and 'D' for UART\n");
	printf("---------------------------------\n");

	UEP_DMA(2) = (uintptr_t)uart_tx_buffer;

	while(1)
	{
#if FUNCONF_USE_DEBUGPRINTF
		poll_input();
#endif    
		if(terminal_input)
		{
			switch (terminal_input)
			{
				case 'd':
					usb_debug = (usb_debug)?0:1;
					printf("USB debug %s\n", (usb_debug)?"ON":"OFF");
					break;

				case 'D':
					uart_debug = (uart_debug)?0:1;
					printf("UART debug %s\n", (uart_debug)?"ON":"OFF");
					break;
			}
			terminal_input = 0;
		}
		uart_process_tx(&cdc);
		uart_process_rx(&cdc);
	}
}


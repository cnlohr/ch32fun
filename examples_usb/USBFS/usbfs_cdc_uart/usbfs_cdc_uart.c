#include "ch32fun.h"
#include <stdio.h>
#include <string.h>
#include "fsusb.h"
#include "uart.h"

#define SYSTICK_DIV 1
#define SYSTICK_ONE_MILLISECOND ((uint32_t)FUNCONF_SYSTEM_CORE_CLOCK / SYSTICK_DIV / 1000)
uint32_t millis_cnt = 0;
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
#ifdef CH32V20x      
      ret = -1; // ACK, whithout it RX is stuck on ch32v208, but is ok on ch32v307 (strange?)
#endif
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
    cdc.tx_remain +=  USBFS->RX_LEN;
    // uart.tx_pos += USBFS->RX_LEN;
    // printf("write_pos = %d, tx_wrap_pos = %d, tx_remain = %d, txing = %d\n", write_pos, cdc.tx_wrap_pos, cdc.tx_remain, cdc.txing);
    // Prevent overwrite of the buffer
    
    printf("\n");
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
#if defined (CH5xx) || defined (CH32X03x) || defined (CH32V10x)
	SysTick->CMPL = SYSTICK_ONE_MILLISECOND - 1;
  SysTick->CNTL = 0;
#else
	SysTick->CMP = SYSTICK_ONE_MILLISECOND - 1;
  SysTick->CNT = 0;
#endif
	SysTick->CTLR |= SYSTICK_CTLR_STE   |  // Enable Counter
	                 SYSTICK_CTLR_STIE  |  // Enable Interrupts
#if SYSTICK_DIV==1                    
                   SYSTICK_CTLR_STCLK ;  // Set Clock Source to HCLK/1
#else
                    ;
#endif
	
  cdc.rx_timeout = 0;
  cdc.usb_timeout = 0;
  millis_cnt = 0;

	NVIC_EnableIRQ(SysTicK_IRQn);
}

void SysTick_Handler(void) __attribute__((interrupt));
void SysTick_Handler(void)
{
#if defined (CH5xx) || defined (CH32X03x) || defined (CH32V10x)
  uint64_t systick_tmp = (SysTick->CNTH << 32 | SysTick->CNTL) + SYSTICK_ONE_MILLISECOND;
	SysTick->CMPL = (uint32_t)systick_tmp;
	SysTick->CMPH = systick_tmp >> 32;
#else
	SysTick->CMP += SYSTICK_ONE_MILLISECOND;
#endif
#ifndef CH32V10x
  SysTick->SR = 0;
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
  RCC->APB1PCENR |= RCC_APB1Periph_USART2;
  printf("Starting\n");
  printf("disabling SWD in 3s\n");
  Delay_Ms(3000);
  AFIO->PCFR1 |= AFIO_PCFR1_SWJ_CFG_DISABLE;
  
  funPinMode( PA2, GPIO_Speed_10MHz | GPIO_CNF_OUT_PP_AF );
  funPinMode( PB12, GPIO_Speed_10MHz | GPIO_CNF_OUT_PP_AF );

  cdc_init(&cdc);
  printf("Started UART%d %08lx\n", cdc.uart->number, (uint32_t)UART(cdc.uart->number));
  uint64_t test64 = 0x1000000000;
  printf("%016x\n", test64);
	USBFSSetup();
  printf("Started USB\n\n");
  printf("You can enable debug messages:\n");
  printf("Type 'd' for USB and 'D' for UART\n");
  printf("---------------------------------\n");
  // Delay_Ms(1000);
  // printf("delay 1s ended\n");
  // memcpy(uart_tx_buffer, message, strlen(message));
  
  // UART_TX_DMA->MADDR = uart_tx_buffer;
  // printf("UART_TX_DMA->MADDR = %08x\n", UART_TX_DMA->MADDR);
  // UART_TX_DMA->CNTR = strlen(message);
  // UART_TX_DMA->CFGR |= (uint16_t)(DMA_CFGR1_EN);  // Enable TX DMA
  // UART(uart.uart->number)->CTLR3 |= USART_DMAReq_Tx;  // Enable UART DMA transmission
  // uart.txing = 1;
  // Delay_Ms(1000);
  USBFS->UEP2_DMA = uart_tx_buffer;

	while(1)
	{
    poll_input();
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


#include "ch32fun.h"
#include <stdio.h>

#define RX_BUF_LEN 16
#define CMD_BUF_LEN 16

u8 rx_buf[RX_BUF_LEN] = {0};
u8 cmd_buf[CMD_BUF_LEN] = {0};

void process_cmd(u8* buf, u8 len)
{
	if ( strstr((char*)buf, "toggle") != 0 ) {
		GPIOC->OUTDR ^= (1<<7);
		printf("Horay!\r\n");
	} else {
		printf("Invalid cmd: %s\r\n", buf);
	}	
}

int main()
{
	SystemInit();

	// enable gpio that we will toggle
	funGpioInitC();
	funPinMode( PC7, GPIO_CFGLR_OUT_2Mhz_PP);
	
	// enable rx pin
	USART1->CTLR1 |= USART_CTLR1_RE;

	// enable usart dma rx requests
	USART1->CTLR3 |= USART_CTLR3_DMAR;

	// enable dma clock
	RCC->AHBPCENR |= RCC_DMA1EN;

	// configure dma
	DMA1_Channel5->CFGR |=
		DMA_MemoryDataSize_Byte | 
		DMA_PeripheralDataSize_Byte |
		DMA_MemoryInc_Enable |
		DMA_Mode_Circular |
		DMA_DIR_PeripheralSRC |
		DMA_Priority_High;
	DMA1_Channel5->CNTR = RX_BUF_LEN;
	DMA1_Channel5->MADDR = (u32)&rx_buf;
	DMA1_Channel5->PADDR = (u32)&USART1->DATAR;
	DMA1_Channel5->CFGR |= DMA_CFGR1_EN;

	while(1)
	{
		static u32 tail = 0;
		static u32 head = 0;
		static u32 cmd_st = 0;
		static u32 cmd_end = 0;

		head = RX_BUF_LEN - DMA1_Channel5->CNTR;
		while ( tail != head )
		{
			if (rx_buf[tail] == '\n')
			{
				cmd_end = tail;
				// to make life easier using string.h
				rx_buf[tail]= '\0';

				if (tail >= cmd_st)
				{
					memcpy(cmd_buf, &rx_buf[cmd_st], cmd_end+1);
					process_cmd(cmd_buf, cmd_end+1);
					cmd_st = (cmd_end + 1) % RX_BUF_LEN;
				}
				else // handle overlap
				{
					u8 st_len = RX_BUF_LEN - cmd_st;
					memcpy(cmd_buf, &rx_buf[cmd_st], st_len);
					memcpy(cmd_buf+st_len, &rx_buf[0], cmd_end+1);
					process_cmd(cmd_buf, st_len + cmd_end + 1);
					cmd_st = (cmd_end + 1) % RX_BUF_LEN;
				}
			}
			tail = (tail+1) % RX_BUF_LEN;
		}
	}
}

#include "ch32fun.h"
#include <stdio.h>

// Default SPI pins
#define PIN_MISO		PA15
#define PIN_MOSI		PA14
#define PIN_SCK			PA13
#define PIN_CS			PA12

#define BUF_LEN 8

// The DMA peripherals on the CH571/CH573 cannot access the low 2k of RAM
// ch32fun defines a dma_safe section for the CH571/CH573 guaranteed to be valid
// Note: this section is completely uninitalized at start
// We make it huge to demonstrate it will occupy no space in flash
// Observe the flash usage is less than the buffer size but the RAM usage is large
#ifdef CH571_CH573
__attribute__((section(".dma_safe"))) 
#endif
uint8_t dma_buf[4096];

const uint8_t spi_data[BUF_LEN] = {0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48};

int main()
{
	SystemInit();

	printf("SPI0 DMA TX demo\n");

	funGpioInitAll(); // no-op on ch5xx

	// funPinMode( PIN_MISO, GPIO_CFGLR_IN_PUPD );
	funPinMode( PIN_MOSI, GPIO_CFGLR_OUT_2Mhz_PP );
	funPinMode( PIN_SCK, GPIO_CFGLR_OUT_2Mhz_PP );
	funPinMode( PIN_CS, GPIO_CFGLR_OUT_2Mhz_PP );
	funDigitalWrite( PIN_CS, FUN_HIGH );

	// 1 MHz SPI at 60 MHz Fsys
	R8_SPI0_CLOCK_DIV = 60;

	R8_SPI0_CTRL_MOD = RB_SPI_MOSI_OE | RB_SPI_SCK_OE;

	// Note only the lower 14 bits are used.
	// The remainder are fixed at 0x20004000
	R32_SPI0_DMA_BEG = (uint32_t) &dma_buf;
	R32_SPI0_DMA_END = BUF_LEN + (uint32_t) &dma_buf;

	R8_SPI0_CTRL_CFG |= RB_SPI_DMA_LOOP | RB_SPI_DMA_ENABLE;

	// Initalize the DMA buffer with our payload
	memcpy(dma_buf, spi_data, BUF_LEN);

	printf("Using DMA buffer at 0x%08lx\n", (uint32_t) dma_buf);

	while(1)
	{
		printf("Sending %d bytes:", BUF_LEN);
		for (int i = 0; i < BUF_LEN; i++) {
			printf(" %02x", dma_buf[i]);
		}
		printf("\n");
		
		funDigitalWrite( PIN_CS, FUN_LOW );

		// Start SPI TX by setting number of bytes
		// Data is pulled via DMA
		R16_SPI0_TOTAL_CNT = BUF_LEN;

		// SPI is finished when count is zero
		while (R16_SPI0_TOTAL_CNT) { }

		funDigitalWrite( PIN_CS, FUN_HIGH );

		Delay_Ms(1000);
	}
}

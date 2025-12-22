// Simple example to test SPI ST7735 display
// This example uses SPI0, wire the display as follows:
// PA13:	SCK
// PA14:	MOSI
// PA10:	RST
// PA11:	DC

// The display should be filled with a purple color
// and display a text string with a counter

#include "ch32fun.h"
#include "lib_st7735.h"

// R32_SPI0_CONTROL
// PA13:	SCK
// PA14:	MOSI, TX0_

// R32_SPI1_CONTROL
// PA0: 	SCK1
// PA1: 	MOSI1

#define TARGETED_SPI SPI0

int main() {
	SystemInit();
	Delay_Ms(100);
	funGpioInitAll();

	SPI_Device_t spi_device = {
		.SPIx = TARGETED_SPI,
		.mosi_pin = PA14,
		.sck_pin = PA13,
		.rst_pin = PA12, 
		.dc_pin = PA11, 
		.cs_pin = PA10
	};

	printf("~SPI ST7735 TEST~\n");
	// clock div = 16, mode: 1 = slave, 0 = master
	SPI_init(&spi_device, 16, 0);
	ST7735_init(&spi_device, 160, 80);
	ST7735_fill_all(ST_PURPLE);

	char str[25] = {0};
	int counter = 0;

	while(1) {
		sprintf(str, "Hello Bee %d", counter++);
		tft_print(&default_font, str, 0, 0, ST_WHITE, ST_PURPLE);
		Delay_Ms(1000);
	}
}



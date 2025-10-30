#include "ch32fun.h"
#include "fun_st7735.h"

// R32_SPI0_CONTROL
// PA13:	SCK
// PA14:	MOSI, TX0_

// R32_SPI1_CONTROL
// PA0: 	SCK1
// PA1: 	MOSI1

int main() {
	SystemInit();
	Delay_Ms(100);
	funGpioInitAll();

	SPI_Device_t spi_device = {
		.spi_ctrl = &R32_SPI0_CONTROL,
		.rst_pin = PA10, 
		.dc_pin = PA11, 
		.cs_pin = PA15
	};

	printf("~SPI ST7735 TEST~\n");
	fun_st7335_init(160, 80, &spi_device);
	tft_fill_rect(0, 0, 160, 80, ST_PURPLE);

	char str[25] = {0};
	int counter = 0;

	while(1) {
		sprintf(str, "Hello Bee %d", counter++);
		tft_print(str, 0, 0, ST_WHITE, ST_PURPLE);
		Delay_Ms(1000);
	}
}



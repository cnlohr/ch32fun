#include "ch32fun.h"
#include "../../examples_ch5xx/spi_st7735_test/lib_st7735.h"

#define TARGETED_SPI SPI1

#define SPI_RST_PIN		PD4
#define SPI_DC_PIN		PD2
#define ST7735_CS_PIN	PD0

int main() {
	SystemInit();
	Delay_Ms(100);
	funGpioInitAll();

	SPI_Device_t spi_device = {
		.SPIx = TARGETED_SPI,
		.mosi_pin = PC6,
		.sck_pin = PC5,
		.rst_pin = PD4, 
		.dc_pin = PD2, 
		.cs_pin = -1
	};

	printf("~SPI ST7735 TEST~\n");
	// clock div = 16, mode: 1 = slave, 0 = master
	SPI_Init(&spi_device, 16, 0);
	SPI_DMA_init(DMA1_Channel3);

	ST7735_init(&spi_device, 160, 80);
	ST7735_fill_all(ST_PURPLE);
	// ST7735_fill_all(ST_GREEN);

	char str[25] = {0};
	int counter = 0;

	while(1) {
		sprintf(str, "Hello Bee %d", counter++);
		tft_print(&default_font, str, 0, 0, ST_WHITE, ST_PURPLE);
		Delay_Ms(1000);
	}
}



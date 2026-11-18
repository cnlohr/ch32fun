#include "ch32fun.h"
#include "200x200-bw-pic.h"
#include <stdio.h>

// specifically for GDEM0154D67WT, based on vendor (Good Display) provided examples

#define SPI_CS PC1
#define SPI_SCK PC5
#define SPI_MOSI PC6
#define SPI_RESET PD4
#define SPI_DC PD2
#define SPI_BUSY PD3
#define EPD_HEIGHT 200
#define EPD_WIDTH 200

static inline void SPI_wait_TX_complete() {
	while(!(SPI1->STATR & SPI_STATR_TXE)) {}
}

static inline void SPI_wait_not_busy() {
	while((SPI1->STATR & SPI_STATR_BSY) != 0) {}
}

static inline void SPI_write_8(uint8_t data) {
	SPI1->DATAR = data;
}

static inline void SPI_transfer_8(uint8_t data) {
	SPI_write_8(data);
	SPI_wait_TX_complete();
	SPI_wait_not_busy();
}

// wait until display is not busy
void ssd1681_not_busy() {
	while (1) {
		if (!funDigitalRead(SPI_BUSY)) break;
		//Delay_Ms(1);
	}
}

static inline void ssd1681_write_command(uint8_t command) {
	funDigitalWrite(SPI_CS, FUN_LOW);
	funDigitalWrite(SPI_DC, FUN_LOW);
	SPI_transfer_8(command);
	funDigitalWrite(SPI_CS, FUN_HIGH);
}

static inline void ssd1681_write_data(uint8_t data) {
	funDigitalWrite(SPI_CS, FUN_LOW);
	funDigitalWrite(SPI_DC, FUN_HIGH);
	SPI_transfer_8(data);
	funDigitalWrite(SPI_CS, FUN_HIGH);
}

// module reset with a delay
void ssd1681_reset() {
	printf("RESET to low...\n");
	funDigitalWrite(SPI_RESET, FUN_LOW);
	printf("wait...\n");
	Delay_Ms(1000);
	funDigitalWrite(SPI_RESET, FUN_HIGH);
	printf("RESET to high...\n");
	printf("wait...\n");
	Delay_Ms(1000);
	printf("wait for not busy...\n");
	ssd1681_not_busy();
}

// SW RESET with a delay
void ssd1681_sw_reset() {
	ssd1681_write_command(0x12);
	ssd1681_not_busy();
	// code examples are not consistent with init workflow from the docs.
	Delay_Ms(10);
}

void ssd1681_deep_sleep() {
	ssd1681_write_command(0x10);
	ssd1681_write_data(0x01);
	Delay_Ms(100);
}

void ssd1681_init() {
	printf("reset...\n");
	ssd1681_reset();
	printf("sw reset...\n");
	ssd1681_sw_reset();
	// driver output control
	printf("driver output control...\n");
	ssd1681_write_command(0x01);
	ssd1681_write_data((EPD_HEIGHT-1)%256);
	ssd1681_write_data((EPD_HEIGHT-1)/256);
	ssd1681_write_data(0x00);
	printf("data entry mode...\n");
	// data entry mode
	ssd1681_write_command(0x11);
	ssd1681_write_data(0x01);
	printf("Set RAM X - address Start / End position...\n");
	// Set RAM X - address Start / End position
	ssd1681_write_command(0x44);
	ssd1681_write_data(0x00);
	ssd1681_write_data(EPD_WIDTH/8-1);
	printf("Set Ram Y - address Start / End position\n");
	// Set Ram Y - address Start / End position
	ssd1681_write_command(0x45);
	ssd1681_write_data((EPD_HEIGHT-1)%256);
	ssd1681_write_data((EPD_HEIGHT-1)/256);
	ssd1681_write_data(0x00);
	ssd1681_write_data(0x00);
	printf("Border Waveform Control...\n");
	// Border Waveform Control
	ssd1681_write_command(0x3c);
	ssd1681_write_data(0x05);
	printf("Temperature Sensor Control...\n");
	// Temperature Sensor Control
	ssd1681_write_command(0x18);
	ssd1681_write_data(0x80);
	printf("Set RAM X address counter...\n");
	// Set RAM X address counter
	ssd1681_write_command(0x4e);
	ssd1681_write_data(0x00);
	// Set RAM Y address counter
	printf("Set RAM Y address counter...\n");
	ssd1681_write_command(0x4f);
	ssd1681_write_data(0x00);
	// starting counter at 0 index, so next two lines are commented out
	//ssd1681_write_data((EPD_HEIGHT-1)%256);
	//ssd1681_write_data((EPD_HEIGHT-1)/256);
	printf("final init wait...\n");
	ssd1681_not_busy();
}

int main()
{
	SystemInit();
	funGpioInitAll();

	/* Set up pins for Alternate Function*/
	// PC5 is SCK, 50MHz Output, alt func, p-p
	funPinMode(SPI_SCK, GPIO_CFGLR_OUT_50Mhz_AF_PP);

	// PC6 is MOSI, 50MHz Output, alt func, p-p
	funPinMode(SPI_MOSI, GPIO_CFGLR_OUT_50Mhz_AF_PP);

	// PC1 is Software NSS, 50MHz Output, GPIO, p-p
	funPinMode(SPI_CS, GPIO_CFGLR_OUT_50Mhz_PP);

	// PD2 is D/C
	funPinMode(PD2, GPIO_CFGLR_OUT_50Mhz_PP);
	funDigitalWrite(PD2, FUN_HIGH);

	// PD3 is BUSY
	funPinMode(SPI_BUSY, GPIO_CFGLR_IN_PUPD);

	// PD4 is RESet
	funPinMode(SPI_RESET, GPIO_CFGLR_OUT_50Mhz_PP);
	funDigitalWrite(PD4, FUN_HIGH);


	RCC->APB2PRSTR = RCC_SPI1RST;
	RCC->APB2PRSTR = 0;
	RCC->APB2PCENR |= RCC_APB2Periph_SPI1;

	// Configure SPI
	// SPI_BaudRatePrescaler_4 gives, in this case, ~12MHz; nominally, display/driver can handle up to 20MHz.
	SPI1->CTLR1 =
		SPI_NSS_Soft | SPI_CPHA_1Edge | SPI_CPOL_Low | SPI_DataSize_8b |
		SPI_Mode_Master | SPI_Direction_1Line_Tx | SPI_BaudRatePrescaler_4;

	// not sure if this has any effect...
	SPI1->HSCR = 1; // High-speed enable.
	SPI1->CTLR1 |= CTLR1_SPE_Set;
	printf("ssd1681 init...\n");
	ssd1681_init();
	ssd1681_write_command(0x24);
	for (uint16_t i=0; i<sizeof(image); i++) {
		ssd1681_write_data(image[i]);
	}
	// Display Update Control 2
	ssd1681_write_command(0x22);
	ssd1681_write_data(0xf7);
	// Master Activation
	ssd1681_write_command(0x20);
	ssd1681_not_busy();

	Delay_Ms(1000);
	printf("deep sleep...\n");
	ssd1681_deep_sleep();
}

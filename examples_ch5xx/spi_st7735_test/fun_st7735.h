// Init function derived from: https://github.com/moononournation/Arduino_GFX 
// ST7735 Datasheet: https://www.displayfuture.com/Display/datasheet/controller/ST7735.pdf

#include "ch32fun.h"
#include <stdint.h>
#include "lib_tft.h"
#include "lib_spi_ch5xx.h"


//# TODO: Improve DMA effiency by get rid of SWAP BYTES some how.

#define ST7735_USE_DMA

#define ST7735_RGB(r, g, b) ((r >> 3) | ((g & 0xFC) << 3) | ((b & 0xF8) << 8))

#define ST_BLACK		ST7735_RGB(0	 , 0	 , 0)
#define ST_WHITE		ST7735_RGB(0xFF  , 0xFF  , 0xFF)
#define ST_RED			ST7735_RGB(0xFF  , 0	 , 0)
#define ST_GREEN		ST7735_RGB(0	 , 0xFF  , 0)
#define ST_BLUE			ST7735_RGB(0	 , 0	 , 0xFF)
#define ST_PURPLE		ST7735_RGB(0x80  , 0	 , 0x80)
#define ST_YELLOW		ST7735_RGB(0xFF  , 0xFF  , 0)
#define ST_CYAN			ST7735_RGB(0	 , 0xFF  , 0xFF)
#define ST_MAGENTA		ST7735_RGB(0xFF  , 0	 , 0xFF)
#define ST_ORANGE		ST7735_RGB(0xFF  , 0x80  , 0)


//###########################################
//# INTERFACES
//###########################################

SPI_Device_t *SPI_Dev;

// Disable
void INT_TFT_CS_HIGH() {
	funDigitalWrite(SPI_Dev->cs_pin, 1);
}

// Enable
void INT_TFT_CS_LOW() {
	funDigitalWrite(SPI_Dev->cs_pin, 0);
}

#define ST7735_CASET 0x2A	// Column Address Set
#define ST7735_RASET 0x2B	// Row Address Set
#define ST7735_RAMWR 0x2C	// RAM Write

u8 ST7735_XOFFSET = 0;
u8 ST7735_YOFFSET = 0;

void INTF_TFT_SET_WINDOW(u16 x0, u16 y0, u16 x1, u16 y1) {
	SPI_Cmd_Reg8(SPI_Dev, ST7735_CASET);
	SPI_Cmd_Data16(SPI_Dev, x0 + ST7735_XOFFSET);
	SPI_Cmd_Data16(SPI_Dev, x1 + ST7735_XOFFSET);
	SPI_Cmd_Reg8(SPI_Dev, ST7735_RASET);
	SPI_Cmd_Data16(SPI_Dev, y0 + ST7735_YOFFSET);
	SPI_Cmd_Data16(SPI_Dev, y1 + ST7735_YOFFSET);
	SPI_Cmd_Reg8(SPI_Dev, ST7735_RAMWR);
}

void INTF_TFT_SEND_BUFF8(const u8* buffer, u16 len) {
	for (int i = 0; i < len; i++) {
		SPI_Cmd_Data8(SPI_Dev, buffer[i]);
	}
}

#define SWAP_BYTES(x) (((x) >> 8) | ((x) << 8))

void INTF_TFT_SEND_BUFF16(u16* buffer, u16 len) {
	#ifdef ST7735_USE_DMA
		funDigitalWrite(SPI_Dev->dc_pin, 1);

		for (u16 i = 0; i < len; i++) { buffer[i] = SWAP_BYTES(buffer[i]); }
		fun_spi0_dma_send((u8*)buffer, len*2, 1);
		for (u16 i = 0; i < len; i++) { buffer[i] = SWAP_BYTES(buffer[i]); }
	#else
		for (int i = 0; i < len; i++) {
			SPI_Cmd_Data16(SPI_Dev, buffer[i]);
		}
	#endif
}

void INTF_TFT_SEND_PIXEL(u16 color) {
	SPI_Cmd_Data16(SPI_Dev, color);
}


//###########################################
//# CONFIGURATIONS
//###########################################

#define ST7735_SWRESET 0x01
#define ST7735_SLEEPON 0x10		// Sleep ON
#define ST7735_SLEEPOFF 0x11	// Sleep OFF
#define ST7735_COLMODE 0x3A		// Color Mode bit/pixel

#define ST7735_INVERTON	0x21	// Invert ON
#define ST7735_INVERTOFF 0x20	// Invert OFF
#define ST7735_NORON 0x13		// Normal Display ON
#define ST7735_MADCTL 0x36		// Memory Access Control

#define ST7735_DISPON 0x29		// Display ON
#define ST7735_DISPOFF 0x28		// Display OFF
#define ST7735_GAMCTRP 0xE0		// Gamma Control Positive
#define ST7735_GAMCTRN 0xE1		// Gamma Control Neigative

u8 ST7735_WIDTH = 160;
u8 ST7735_HEIGHT = 80;

void fun_st7735_fill_all(u16 color) {
	tft_fill_rect(0, 0, ST7735_WIDTH, ST7735_HEIGHT, color);
}

void fun_st7335_init(u8 width, u8 height, SPI_Device_t *dev) {
	// clock div = 16, mode: 1 = slave, 0 = master
	fun_spi_init(dev, 16, 0);
	fun_spi_GPIOs(dev);

	SPI_Dev = dev;
	ST7735_WIDTH = width;
	ST7735_HEIGHT = height;

	// adjust offset base on different screen size
	if (height == 80) {
		ST7735_XOFFSET = 1;
		ST7735_YOFFSET = 26;
	}

	INT_TFT_CS_LOW();

	//# Software reset
	//! 1.8" display need at least 110ms delay after reset
	SPI_Cmd_Reg8(dev, ST7735_SWRESET);
	Delay_Ms(110);
	SPI_Cmd_Reg8(dev, ST7735_SLEEPOFF);
	Delay_Ms(10);

	//# Interface Pixel Format
	SPI_Cmd_Reg8(dev, ST7735_COLMODE);
	SPI_Cmd_Data8(dev, 0x05); // 0x03: 12-bit, 0x05: 16-bit, 0x06: 18-bit, 0x07: Not used

	//# Display inversion
	SPI_Cmd_Reg8(dev, ST7735_INVERTON);

	//# Normal display on
	SPI_Cmd_Reg8(dev, ST7735_NORON);

	//# MADCTL - Memory Access Control
	u8 MADCTL_MY = 0b10000000;	 // bit7: Row address order
	u8 MADCTL_MX = 0b01000000;	 // bit6: Column address order
	u8 MADCTL_MV = 0b00100000;	 // bit5: Row/Column exchange
	u8 MADCTL_ML = 0b00010000;	 // bit4: Vertical refresh order (0 = top to bottom, 1 = bottom to top)
	u8 MADCTL_MH = 0b00000100;	 // bit2: Horizontal refresh order (0 = left to right, 1 = right to left)
	u8 MADCTL_RGB = 0b00001000;	 // bit3: Color order (0 = RGB, 1 = BGR)

	// & 0xFF to turn ON, & 0x00 to turn OFF
	SPI_Cmd_Reg8(dev, ST7735_MADCTL);
	u8 ctrValue = (MADCTL_MY & 0xFF) | (MADCTL_MV & 0xFF);
	SPI_Cmd_Data8(dev, ctrValue);
	
	//# Gamma+ Adjustments Control (magic numbers)
	u8 gamma_pos[] = {
		0x09, 0x16, 0x09, 0x20, 0x21, 0x1B, 0x13, 0x19,
		0x17, 0x15, 0x1E, 0x2B, 0x04, 0x05, 0x02, 0x0E
	};
	SPI_Cmd_Reg8(dev, ST7735_GAMCTRP);
	INTF_TFT_SEND_BUFF8(gamma_pos, 16);

	//# Gamma- Adjustments Control (magic numbers)
	u8 gamma_neg[] = {
		0x0B, 0x14, 0x08, 0x1E, 0x22, 0x1D, 0x18, 0x1E,
		0x1B, 0x1A, 0x24, 0x2B, 0x06, 0x06, 0x02, 0x0F
	};
	SPI_Cmd_Reg8(dev, ST7735_GAMCTRN);
	INTF_TFT_SEND_BUFF8(gamma_neg, 16);
	Delay_Ms(10);

	//# Display On
	SPI_Cmd_Reg8(dev, ST7735_DISPON);
	Delay_Ms(10);

	INT_TFT_CS_HIGH();
}
// Init function derived from: https://github.com/moononournation/Arduino_GFX 
// ST7735 Datasheet: https://www.displayfuture.com/Display/datasheet/controller/ST7735.pdf

#include "ch32fun.h"
#include <stdint.h>
#include "lib_tft.h"
#include "lib_spi_ch5xx.h"


//# TODO: Improve DMA effiency by get rid of SWAP BYTES some how.

// #define ST7735_USE_DMA

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
SPI_Device_t *SELECTED_SPI_DEV = NULL;

void TFT_SELECT_SPI_DEV(void *spi_dev) {
	SELECTED_SPI_DEV = (SPI_Device_t *)spi_dev;
}

// SPI Device Disable
void TFT_CS_HIGH() {
	funDigitalWrite(SELECTED_SPI_DEV->cs_pin, 1);
}

// SPI DeviceEnable
void TFT_CS_LOW() {
	funDigitalWrite(SELECTED_SPI_DEV->cs_pin, 0);
}

#define ST7735_CASET 0x2A	// Column Address Set
#define ST7735_RASET 0x2B	// Row Address Set
#define ST7735_RAMWR 0x2C	// RAM Write

int ST7735_XOFFSET2 = 0;
int ST7735_YOFFSET2 = 0;

void TFT_SET_WINDOW(int x0, int y0, int x1, int y1) {
	SPI_Cmd_Reg8(SELECTED_SPI_DEV, ST7735_CASET);
	SPI_Cmd_Data16(SELECTED_SPI_DEV, x0 + ST7735_XOFFSET2);
	SPI_Cmd_Data16(SELECTED_SPI_DEV, x1 + ST7735_XOFFSET2);
	SPI_Cmd_Reg8(SELECTED_SPI_DEV, ST7735_RASET);
	SPI_Cmd_Data16(SELECTED_SPI_DEV, y0 + ST7735_YOFFSET2);
	SPI_Cmd_Data16(SELECTED_SPI_DEV, y1 + ST7735_YOFFSET2);
	SPI_Cmd_Reg8(SELECTED_SPI_DEV, ST7735_RAMWR);
}

void TFT_SEND_BUFF8(const u8* buffer, int len) {
	for (int i = 0; i < len; i++) {
		SPI_Cmd_Data8(SELECTED_SPI_DEV, buffer[i]);
	}
}

#define SWAP_BYTES(x) (((x) >> 8) | ((x) << 8))

void TFT_SEND_BUFF16(u16* buffer, int len) {
	#ifdef ST7735_USE_DMA
		funDigitalWrite(SELECTED_SPI_DEV->dc_pin, 1);

		for (int i = 0; i < len; i++) { buffer[i] = SWAP_BYTES(buffer[i]); }
		SPI0_dma_send((u8*)buffer, len*2, 1);
		for (int i = 0; i < len; i++) { buffer[i] = SWAP_BYTES(buffer[i]); }
	#else
		for (int i = 0; i < len; i++) {
			SPI_Cmd_Data16(SELECTED_SPI_DEV, buffer[i]);
		}
	#endif
}

void TFT_SEND_PIXEL(u16 color) {
	SPI_Cmd_Data16(SELECTED_SPI_DEV, color);
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

u8 ST7735_WIDTH2 = 160;
u8 ST7735_HEIGHT2 = 80;

void ST7735_fill_all(u16 color) {
	tft_fill_rect(0, 0, ST7735_WIDTH2, ST7735_HEIGHT2, color);
}

void ST7735_init(SPI_Device_t *dev, int width, int height) {
	TFT_SELECT_SPI_DEV(dev);

	ST7735_WIDTH2 = width;
	ST7735_HEIGHT2 = height;

	// adjust offset base on different screen size
	if (height == 80) {
		ST7735_XOFFSET2 = 1;
		ST7735_YOFFSET2 = 26;
	}

	TFT_CS_LOW();

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
	TFT_SEND_BUFF8(gamma_pos, 16);

	//# Gamma- Adjustments Control (magic numbers)
	u8 gamma_neg[] = {
		0x0B, 0x14, 0x08, 0x1E, 0x22, 0x1D, 0x18, 0x1E,
		0x1B, 0x1A, 0x24, 0x2B, 0x06, 0x06, 0x02, 0x0F
	};
	SPI_Cmd_Reg8(dev, ST7735_GAMCTRN);
	TFT_SEND_BUFF8(gamma_neg, 16);
	Delay_Ms(10);

	//# Display On
	SPI_Cmd_Reg8(dev, ST7735_DISPON);
	Delay_Ms(10);

	TFT_CS_HIGH();
}
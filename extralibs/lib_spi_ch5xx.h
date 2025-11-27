#include "ch32fun.h"

// PA13:	SCK
// PA14:	MOSI, TX0_
// PA15:	MISO, RX0_

// PB15: 	MISO_
// PB14: 	MOSI_
// PB13: 	SCL, SCK_, TX1_

// PA0: 	SCK1
// PA1: 	MOSI1
// PA2: 	MISO1

#define SPI_TIMEOUT_DEFAULT 1000

#define GET_REG8(base, offset)  (*(vu8 *)((vu32)base + offset))
#define GET_REG16(base, offset) (*(vu16 *)((vu32)base + offset))


//###########################################
//# INIT FUNCTIONS
//###########################################

typedef struct {
	vu32 *spi_ctrl;
	s16 mosi_pin;
	s16 miso_pin;
	s16 sck_pin;
	s16 rst_pin;
	s16 dc_pin;
	s16 cs_pin;
} SPI_Device_t;

void SPI_init(SPI_Device_t *dev, u8 divider, u8 slave_mode) {
	//# Setup SPI pins
	if (dev->mosi_pin >= 0) {
		funPinMode(dev->mosi_pin, GPIO_CFGLR_OUT_10Mhz_PP);		// MOSI
	}
	if (dev->miso_pin >= 0) {
		funPinMode(dev->miso_pin, GPIO_CFGLR_IN_FLOAT);		// MISO
	}
	if (dev->sck_pin >= 0) {
		funPinMode(dev->sck_pin, GPIO_CFGLR_OUT_10Mhz_PP);		// SCK
	}

	//# Setup cs pin
	if (dev->cs_pin >= 0) {
        funPinMode(dev->cs_pin, GPIO_CFGLR_OUT_10Mhz_PP);
        funDigitalWrite(dev->cs_pin, 1);
    }

	//# Setup dc pin
	if (dev->dc_pin >= 0) {
		funPinMode(dev->dc_pin, GPIO_CFGLR_OUT_10Mhz_PP);
	}

	//# Setup reset pin
	if (dev->rst_pin >= 0) {
		funPinMode(dev->rst_pin, GPIO_CFGLR_OUT_10Mhz_PP);
		// reset device
		funDigitalWrite(dev->rst_pin, 0);
		Delay_Ms(100);
		funDigitalWrite(dev->rst_pin, 1);
		Delay_Ms(100);	
	}

	if (divider < 2) divider = 2;
	GET_REG8(dev->spi_ctrl, SPI_CLOCK_DIV) = divider;

	GET_REG8(dev->spi_ctrl, SPI_CTRL_MOD) = RB_SPI_ALL_CLEAR;
	
	GET_REG8(dev->spi_ctrl, SPI_CTRL_MOD) = RB_SPI_SCK_OE |
											RB_SPI_MOSI_OE |
											RB_SPI_MISO_OE;

	if (slave_mode) {
		// 1 for slave Mode
		GET_REG8(dev->spi_ctrl, SPI_CTRL_MOD) |= RB_SPI_MODE_SLAVE;
	} else {
		// 0 for master Mode
		GET_REG8(dev->spi_ctrl, SPI_CTRL_MOD) &= ~RB_SPI_MODE_SLAVE;
	}

	// Enable auto clear flag when accessing buffer
	GET_REG8(dev->spi_ctrl, SPI_CTRL_CFG) |= RB_SPI_AUTO_IF;
}


//###########################################
//# SEND & RECEIVE FUNCTIONS
//###########################################

u8 SPI_send8(vu32 *spi_ctrl, u8 val) {
	// FIFO direction ~RB_SPI_FIFO_DIR output
	GET_REG8(spi_ctrl, SPI_CTRL_MOD) &= ~RB_SPI_FIFO_DIR;
	GET_REG8(spi_ctrl, SPI_BUFFER) = val;

	u32 timeout = SPI_TIMEOUT_DEFAULT;
	while (!(R8_SPI0_INT_FLAG & RB_SPI_FREE) && --timeout);
	if (timeout == 0) return 0x10;

	// read the receive data
	return GET_REG8(spi_ctrl, SPI_BUFFER);
}

u8 SPI_send16(vu32 *spi_ctrl, u16 val) {
	// FIFO direction ~RB_SPI_FIFO_DIR output
	GET_REG8(spi_ctrl, SPI_CTRL_MOD) &= ~RB_SPI_FIFO_DIR;

	// send one byte at a time. MSB first
	GET_REG8(spi_ctrl, SPI_BUFFER) = (val >> 8) & 0xFF;
	u32 timeout = SPI_TIMEOUT_DEFAULT;
	while (!(R8_SPI0_INT_FLAG & RB_SPI_FREE) && --timeout);
	if (timeout == 0) return 0x20;

	GET_REG8(spi_ctrl, SPI_BUFFER) = val & 0xFF;
	timeout = SPI_TIMEOUT_DEFAULT;
	while (!(R8_SPI0_INT_FLAG & RB_SPI_FREE) && --timeout);
	if (timeout == 0) return 0x21;

	// read the receive data
	return GET_REG8(spi_ctrl, SPI_BUFFER);
}

u8 SPI_receive8(vu32 *spi_ctrl) {
	// FIFO direction RB_SPI_FIFO_DIR input
	GET_REG8(spi_ctrl, SPI_CTRL_MOD) |= RB_SPI_FIFO_DIR;
	GET_REG8(spi_ctrl, SPI_BUFFER) = 0x00;
	u32 timeout = SPI_TIMEOUT_DEFAULT;
	while (!(R8_SPI0_INT_FLAG & RB_SPI_FREE) && --timeout);
	if (timeout == 0) return 0x30;

	// read the receive data
	return GET_REG8(spi_ctrl, SPI_BUFFER);
}

//# SPI DC Commands

u8 SPI_Cmd_Reg8(SPI_Device_t *dev, u8 cmd) {
	funDigitalWrite(dev->dc_pin, 0);
	return SPI_send8(dev->spi_ctrl, cmd);
}

void SPI_Cmd_Data8(SPI_Device_t *dev, u8 data) {
	funDigitalWrite(dev->dc_pin, 1);
	return SPI_send8(dev->spi_ctrl, data);
}

void SPI_Cmd_Data16(SPI_Device_t *dev, u16 data) {
	funDigitalWrite(dev->dc_pin, 1);
	return SPI_send16(dev->spi_ctrl, data);
}


//###########################################
//# DMA FUNCTIONS
//###########################################
//! ONLY SPI0 supports DMA

void SPI0_dma_stop() {
	// Disable DMA
    R8_SPI0_CTRL_CFG &= ~RB_SPI_DMA_ENABLE;
    
    // Clear total count to stop transmission
    R16_SPI0_TOTAL_CNT = 0;
    
    // Clear any pending DMA flags
    R8_SPI0_INT_FLAG |= RB_SPI_IF_DMA_END;
}

void SPI0_dma_send(u8 *buf, u16 len, u8 send_or_receive) {
	if (send_or_receive) {
		// Send
		R8_SPI0_CTRL_MOD &= ~RB_SPI_FIFO_DIR;
	} else {
		// Receive
		R8_SPI0_CTRL_MOD |= RB_SPI_FIFO_DIR;
	}

	R16_SPI0_DMA_BEG = (u32)buf;
	R16_SPI0_DMA_END = (u32)(buf + len);

	// Enable DMA
	R8_SPI0_CTRL_CFG |= RB_SPI_DMA_ENABLE;

	// Set FIFO lenght
	R16_SPI0_TOTAL_CNT = len;

	// Wait for total count to go to 0
	while (R16_SPI0_TOTAL_CNT != 0);

	// Disable DMA
	R8_SPI0_CTRL_CFG &= ~RB_SPI_DMA_ENABLE;
}
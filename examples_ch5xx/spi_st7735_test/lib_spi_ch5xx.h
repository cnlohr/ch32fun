#include "ch32fun.h"

#ifdef CH5xx
	typedef SPI_Typedef SPI_Type;
#else
	typedef SPI_TypeDef SPI_Type;
#endif


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

typedef struct {
	SPI_Type *SPIx;
	int mosi_pin;
	int miso_pin;
	int sck_pin;
	int rst_pin;
	int dc_pin;
	int cs_pin;
} SPI_Device_t;

//###########################################
//# INIT FUNCTIONS
//###########################################

void SPI_Init(SPI_Device_t *dev, int slave_mode) {
	#ifdef CH5xx
		dev->SPIx->CLOCK_DIV = 16;		// Minimum value is 2
		dev->SPIx->CTRL_MOD = RB_SPI_ALL_CLEAR;
		dev->SPIx->CTRL_MOD = RB_SPI_SCK_OE | RB_SPI_MOSI_OE | RB_SPI_MISO_OE;

		if (slave_mode) {
			// 1 for slave Mode
			dev->SPIx->CTRL_MOD |= RB_SPI_MODE_SLAVE;
		} else {
			// 0 for master Mode
			dev->SPIx->CTRL_MOD &= ~RB_SPI_MODE_SLAVE;
		}

		// Enable auto clear flag when accessing buffer
		dev->SPIx->CTRL_CFG |= RB_SPI_AUTO_IF;


		//# Setup SPI pins
		if (dev->miso_pin >= 0) {
			funPinMode(dev->miso_pin, GPIO_CFGLR_IN_FLOAT);		// MISO
		}
		if (dev->mosi_pin >= 0) {
			funPinMode(dev->mosi_pin, GPIO_CFGLR_OUT_10Mhz_PP);		// MOSI
		}
		if (dev->sck_pin >= 0) {
			funPinMode(dev->sck_pin, GPIO_CFGLR_OUT_10Mhz_PP);		// SCK
		}
		
	#else
		// reset control register
		SPI1->CTLR1 = 0;

		// Enable GPIO Port C and SPI peripheral
		RCC->APB2PCENR |= RCC_APB2Periph_SPI1;

		// Configure SPI
		SPI1->CTLR1 |= SPI_CPHA_1Edge | SPI_CPOL_Low
					| SPI_Mode_Master| SPI_BaudRatePrescaler_4
					| SPI_NSS_Soft | SPI_DataSize_8b;
		SPI1->CTLR1 |= SPI_Direction_2Lines_FullDuplex;
		// SPI1->CTLR1 |= SPI_Direction_1Line_Tx;
		SPI1->CTLR1 |= CTLR1_SPE_Set;            // Enable SPI Port

		//# Setup SPI pins
		if (dev->miso_pin >= 0) {
			funPinMode(dev->miso_pin, GPIO_CFGLR_IN_FLOAT);		// MISO
		}
		if (dev->mosi_pin >= 0) {
			funPinMode(dev->mosi_pin, GPIO_CFGLR_OUT_50Mhz_AF_PP);		// MOSI
		}
		if (dev->sck_pin >= 0) {
			funPinMode(dev->sck_pin, GPIO_CFGLR_OUT_50Mhz_AF_PP);		// SCK
		}
	#endif
	

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
}

#ifdef CH5xx
	//###########################################
	//# SEND & RECEIVE FUNCTIONS
	//###########################################
	u8 SPI_send8(SPI_Type *SPIx, u8 val) {
		// FIFO direction ~RB_SPI_FIFO_DIR output
		SPIx->CTRL_MOD  &= ~RB_SPI_FIFO_DIR;
		SPIx->BUFFER = val;

		u32 timeout = SPI_TIMEOUT_DEFAULT;
		while (!(SPIx->INT_FLAG & RB_SPI_FREE) && --timeout);
		if (timeout == 0) return 0x10;

		// read the receive data
		return SPIx->BUFFER;
	}

	u8 SPI_send16(SPI_Type *SPIx, u16 val) {
		// FIFO SPI_Type ~RB_SPI_FIFO_DIR output
		SPIx->CTRL_MOD  &= ~RB_SPI_FIFO_DIR;

		// send one byte at a time. MSB first
		SPIx->BUFFER = (val >> 8) & 0xFF;
		u32 timeout = SPI_TIMEOUT_DEFAULT;
		while (!(SPIx->INT_FLAG & RB_SPI_FREE) && --timeout);
		if (timeout == 0) return 0x20;

		SPIx->BUFFER = val & 0xFF;
		timeout = SPI_TIMEOUT_DEFAULT;
		while (!(SPIx->INT_FLAG & RB_SPI_FREE) && --timeout);
		if (timeout == 0) return 0x21;

		// read the receive data
		return SPIx->BUFFER;
	}

	u8 SPI_receive8(SPI_Type *SPIx) {
		// FIFO direction RB_SPI_FIFO_DIR input
		SPIx->CTRL_MOD |= RB_SPI_FIFO_DIR;
		SPIx->BUFFER = 0x00;
		u32 timeout = SPI_TIMEOUT_DEFAULT;
		while (!(SPIx->INT_FLAG & RB_SPI_FREE) && --timeout);
		if (timeout == 0) return 0x30;

		// read the receive data
		return SPIx->BUFFER;
	}

	//###########################################
	//# DMA FUNCTIONS
	//###########################################
	//! ONLY SPI0 supports DMA

	static void SPI0_dma_stop() {
		// Disable DMA
		SPI0->CTRL_CFG &= ~RB_SPI_DMA_ENABLE;
		
		// Clear total count to stop transmission
		SPI0->TOTAL_CNT = 0;
		
		// Clear any pending DMA flags
		SPI0->INT_FLAG |= RB_SPI_IF_DMA_END;
	}

	static void SPI0_dma_send(u8 *buf, int len, int send_or_receive) {
		if (send_or_receive) {
			// Send
			SPI0->CTRL_MOD &= ~RB_SPI_FIFO_DIR;
		} else {
			// Receive
			SPI0->CTRL_MOD |= RB_SPI_FIFO_DIR;
		}
		
		SPI0->DMA_BEG = (u16)buf;
		SPI0->DMA_END = (u16)(buf + len);

		// Enable DMA
		SPI0->CTRL_CFG |= RB_SPI_DMA_ENABLE;

		// Set FIFO lenght
		SPI0->TOTAL_CNT = len;

		// Wait for total count to go to 0
		while (SPI0->TOTAL_CNT != 0);

		// Disable DMA
		SPI0->CTRL_CFG &= ~RB_SPI_DMA_ENABLE;
	}

#else

	//###########################################
	//# SEND & RECEIVE FUNCTIONS
	//###########################################
	u8 SPI_send8(SPI_Type *SPIx, u8 val) {
		SPIx->DATAR = val;

		u32 timeout = SPI_TIMEOUT_DEFAULT;
		while(!(SPIx->STATR & SPI_STATR_TXE) && --timeout);
		asm volatile("nop");

		timeout = SPI_TIMEOUT_DEFAULT;
		while(!(SPIx->STATR & SPI_STATR_RXNE) && --timeout);
		return SPIx->DATAR;
	}

	u8 SPI_send16(SPI_Type *SPIx, u16 val) {
		SPI_send8(SPIx, (u8)((val >> 8) & 0xFF));	// trasfer MSB
		return SPI_send8(SPIx, (u8)(val & 0xFF));	// transfer LSB
	}

	static void SPI_DMA_init(DMA_Channel_TypeDef* DMA_Channel) {
		// Enable Tx DMA
		SPI1->CTLR2 |= SPI_I2S_DMAReq_Tx;

		// Enable DMA peripheral
		RCC->AHBPCENR |= RCC_AHBPeriph_DMA1;

		// Configure DMA
		DMA_Channel->PADDR = (uint32_t)&SPI1->DATAR;
		DMA_Channel->CFGR = DMA_M2M_Disable | DMA_Priority_VeryHigh 
							| DMA_MemoryDataSize_Byte | DMA_PeripheralDataSize_Byte
							| DMA_MemoryInc_Enable | DMA_PeripheralInc_Disable
							| DMA_Mode_Circular | DMA_DIR_PeripheralDST;
	}

	static void SPI0_dma_send(u8* buffer, int len, int send_or_receive) {		
		DMA1_Channel3->CNTR  = len;
		DMA1_Channel3->MADDR = (uint32_t)buffer;

		// Start DMA transfer
		DMA1_Channel3->CFGR |= DMA_CFGR1_EN;  

		DMA1->INTFCR = DMA1_FLAG_TC3;
		while (!(DMA1->INTFR & DMA1_FLAG_TC3));

		// Stop DMA transfer
		DMA1_Channel3->CFGR &= ~DMA_CFGR1_EN;
	}
#endif

//# SPI DC Commands

u8 SPI_Cmd_Reg8(SPI_Device_t *dev, u8 cmd) {
	funDigitalWrite(dev->dc_pin, 0);
	return SPI_send8(dev->SPIx, cmd);
}

void SPI_Cmd_Data8(SPI_Device_t *dev, u8 data) {
	funDigitalWrite(dev->dc_pin, 1);
	return SPI_send8(dev->SPIx, data);
}

void SPI_Cmd_Data16(SPI_Device_t *dev, u16 data) {
	funDigitalWrite(dev->dc_pin, 1);
	return SPI_send16(dev->SPIx, data);
}

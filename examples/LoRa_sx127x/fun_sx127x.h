// Datasheet
// Ref: https://www.mouser.com/datasheet/2/761/sx1276-1278113.pdf?srsltid=AfmBOorDqVAyhX5J-Lmr_4lud-BnEsvxK5Ckh002gdR3Nj9gTKy8Qciv

// Copyright (c) 2025 UniTheCat
// This is a large modification of the original code

// MIT License
// Copyright (c) 2016 Sandeep Mistry

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.


#include "ch32fun.h"
#include <stdint.h>
#include <stdio.h>

#include "../../extralibs/ch32v003_SPI.h"

//! ####################################
//! SPI FUNCTIONS
//! ####################################

u8 SX127X_CS_PIN = -1;

u8 sx127x_transfer(u8 reg, u8 value) {
	u8 resp;

	if (SX127X_CS_PIN != -1) funDigitalWrite(SX127X_CS_PIN, 0);
	resp = SPI_transfer_8(reg);
	resp = SPI_transfer_8(value);
	if (SX127X_CS_PIN != -1) funDigitalWrite(SX127X_CS_PIN, 1);
	return resp;
}

u8 sx127x_read(u8 reg) {
	return sx127x_transfer(reg & 0x7f, 0x00);
}

u8 sx127x_write(u8 reg, u8 value) {
	return sx127x_transfer(reg | 0x80, value);
}


//! ####################################
//! SETUP FUNCTIONS
//! ####################################

#define PA_BOOST					0x80

void fun_sx127x_setFrequency(uint32_t frequency) {
	uint64_t frf = ((uint64_t)frequency << 19) / 32E6;

	//# 0x06: freq_MSB, 0x07: freq_MID, 0x08: freq_LSB
	sx127x_write(0x06, (u8)(frf >> 16));
	sx127x_write(0x07, (u8)(frf >> 8));
	sx127x_write(0x08, (u8)(frf >> 0));

	// u8 fre0 = sx127x_read(REG_FRF_MSB);		// expect 0xE4 for 915Mhz
	// u8 fre1 = sx127x_read(REG_FRF_MID);		// expect 0xE4 for 915Mhz
	// u8 fre2 = sx127x_read(REG_FRF_LSB);		// expect 0x00 for 915Mhz
	// printf("Freq Read: 0x%02X 0x%02X 0x%02X\r\n", fre0, fre0, fre2);
}

void sx772xx_setOCP(u8 mA) {
	u8 ocpTrim = 27;

	if (mA <= 120) {
		ocpTrim = (mA - 45) / 5;
	} else if (mA <=240) {
		ocpTrim = (mA + 30) / 10;
	}

	//# 0x0B: RegOcp
	sx127x_write(0x0B, 0x20 | (0x1F & ocpTrim));
}

void fun_sx127x_setTxPower(u8 powerDbM) {
	//# 0x4D: RegPaDac
	u8 RegPaDac = 0x4D;

	if (powerDbM > 17) {
		if (powerDbM > 20) powerDbM = 20;
		// subtract 3 from powerDbM, so 18 - 20 maps to 15 - 17
		powerDbM -= 3;

		// High Power +20 dBm Operation (Semtech SX1276/77/78/79 5.4.3.)
		sx127x_write(RegPaDac, 0x87);
		sx772xx_setOCP(140);

	} else {
		if (powerDbM < 2) powerDbM = 2;
		sx127x_write(RegPaDac, 0x84);
		sx772xx_setOCP(100);
	}

	//# 0x09: RegPaConfig
	sx127x_write(0x09, PA_BOOST | (powerDbM - 2));
}

// Modem Config
#define REG_MODEM_CONFIG_2		0x1E

// Bandwidth
#define SX127X_BW_78000			0x00
#define SX127X_BW_10400			0x01
#define SX127X_BW_15600			0x02
#define SX127X_BW_20800			0x03
#define SX127X_BW_31250			0x04
#define SX127X_BW_41700			0x05
#define SX127X_BW_62500			0x06
#define SX127X_BW_125000		0x07
#define SX127X_BW_250000		0x08
#define SX127X_BW_500000		0x09

void fun_sx127x_config1(u8 headerMode, u8 cr, u8 bw) {
	// bit0 (LSB): 0 = explicit header mode, 1 = implicit header mode
	headerMode = headerMode & 0x01; // Ensure 1 bit value

	// bit1-3: Coding Rate. CR1 = 4/5, CR2 = 4/6, CR3 = 4/7, CR4 = 4/8
	cr = cr < 1 ? 1 : cr;
	cr = cr > 4 ? 4 : cr;
	cr = cr & 0b00000111;	// Ensure 3 bit value

	// bit4-7: Bandwidth
	bw = bw & 0b00001111;	// Ensure 4 bit value

	//# 0x1D: RegModemConfig1
	u8 configValue = (bw << 4) | (cr << 1) | headerMode ;
	sx127x_write(0x1D, configValue);
}


//! ####################################
//! INIT FUNCTIONS
//! ####################################

#define SX127X_LONGRANGE_MODE		0b10000000		// bit7: 1 = LoRa, 0 = FSK/OOK
#define SX127X_MODE_SLEEP			0b00000000		// bit0-2: 0 = Sleep
#define SX127X_MODE_STDBY			0b00000001		// bit0-2: 1 = Standby
#define SX127X_MODE_TX				0b00000011		// bit0-2: 3 = TX
#define SX127X_MODE_RX_SINGLE		0b00000110		// bit0-2: 6 = RX Single

#define SX127X_FIFO_RX_CURRENTADDR	0x10

u8 SX127X_OK = 0;

void sx127x_setMode(u8 mode) {
	//# 0x01: RegOpMode
	sx127x_write(0x01, SX127X_LONGRANGE_MODE | mode);
}

u8 sx127x_getMode() {
	//# 0x01: RegOpMode
	// mask the first 3 bits
	return sx127x_read(0x01) & 0b00000111;
}

void fun_sx127x_init(uint32_t frequency, u8 cs_pin) {
	//! configure CS Pin
	if (cs_pin != -1) {
		SX127X_CS_PIN = cs_pin;
		funPinMode(SX127X_CS_PIN, GPIO_Speed_10MHz | GPIO_CNF_OUT_PP);
		funDigitalWrite(SX127X_CS_PIN, 1);
	}

	//! 0x42: RegVersion = sanity check
	u8 version = sx127x_read(0x42);
	printf("LoRa version: 0x%02x\n", version);
	SX127X_OK = version == 0x12;		// expect 0x12

	//! 0x01: RegOpMode - Set Mode sleep (*REQUIRED*)
	sx127x_setMode(SX127X_MODE_SLEEP);

	//# Set frequency
	fun_sx127x_setFrequency(frequency);

	//# 0x1D: RegModemConfig1 (headerMode, cr, bw)
	fun_sx127x_config1(0, 1, SX127X_BW_125000);

	//# 0x0E: TX_baseAddr, 0x0F: RX_baseAddr
	sx127x_write(0x0E, 0);
	sx127x_write(0x0F, 0);

	//# 0x0C: RegLna
	// 0b00: boost off (default current), 0b11: boost on (150% LNA current)
	u8 read = sx127x_read(0x0C);
	sx127x_write(0x0C, read | 0b11);

	//# 0x26: RegModemConfig3
	// bit3: lowDataRateOptimize 0 = Disabled, 1 = Enabled
	// bit2: AGCAutoOn 0 = Disabled, 1 = Enabled
	// bit0-1: Reserved
	sx127x_write(0x26, 0x0010);		// AGCAutoOn

	//# 0x1D: RegModemConfig1
	sx127x_setMode(SX127X_MODE_STDBY);
}


//! ####################################
//! TRANSMITION FUNCTIONS
//! ####################################

#define SX127X_REG_FIFO_ADDR_PTR		0x0D
#define SX127X_REG_PAYLOAD_LENGTH		0x22
#define SX127X_REG_FIFO					0x00

void fun_sx127x_send(u8 *data, u8 size) {
	if (!SX127X_OK) {
		printf("Err: LoRa not initialized\n");
		return;
	}

	// reset FIFO address and payload length
	sx127x_write(SX127X_REG_FIFO_ADDR_PTR, 0);
	sx127x_write(SX127X_REG_PAYLOAD_LENGTH, 0);
	Delay_Ms(1);

	//# write data
	for (int i = 0; i < size; i++) {
		sx127x_write(SX127X_REG_FIFO, data[i]);
	}

	// update len
	sx127x_write(SX127X_REG_PAYLOAD_LENGTH, size);
	Delay_Ms(1);

	//# 0x01: RegOpMode - Send packet
	sx127x_setMode(SX127X_MODE_TX);
	Delay_Ms(1);

	//# 0x12: RegIrqFlags - clear IRQ's
	// 0b (0RxTimeout, RxDone, CrcErr, ValidHeader, TxDone, CadDone, FhssChange, CadDetected)
	u8 tx_done_mask = 0b0001000;
	sx127x_write(0x12, tx_done_mask);
}


//! ####################################
//! RECEIVING FUNCTIONS
//! ####################################

#define IRQ_RX_DONE_MASK			0x40
#define IRQ_PAYLOAD_CRC_ERROR_MASK	0x20
#define REG_RX_NB_BYTES				0x13	// Reg for number of payload bytes of latest packet

int fun_sx127x_parsePacket() {
	if (!SX127X_OK) return 0;
	int packetLength = 0;

	//# 0x12: RegIrqFlags - clear IRQ's
	//! A Read is *REQUIRED*
	int irqFlags = sx127x_read(0x12);
	sx127x_write(0x12, irqFlags);
	Delay_Ms(1);

	if ((irqFlags & IRQ_RX_DONE_MASK) && (irqFlags & IRQ_PAYLOAD_CRC_ERROR_MASK) == 0) {
		// received a packet

		// read packet length
		// if (_implicitHeaderMode) {
		//	 packetLength = sx127x_read(REG_PAYLOAD_LENGTH);
		// } else {
			//# get the number of payload bytes of latest packet
			packetLength = sx127x_read(REG_RX_NB_BYTES);
		// }

		// set FIFO address to current RX address
		u8 rxAdress = sx127x_read(SX127X_FIFO_RX_CURRENTADDR);
		sx127x_write(SX127X_REG_FIFO_ADDR_PTR, rxAdress);
		Delay_Ms(1);

		//# 0x01: RegOpMode - Set to Standby mode
		sx127x_setMode(SX127X_MODE_STDBY);

	} else if (sx127x_getMode() != SX127X_MODE_RX_SINGLE) {
		// not currently in RX mode
		// reset FIFO address
		sx127x_write(SX127X_REG_FIFO_ADDR_PTR, 0);

		//# 0x01: RegOpMode - Set to Single RX mode
		sx127x_setMode(SX127X_MODE_RX_SINGLE);
		Delay_Ms(1);
	}

	return packetLength;
}

void fun_sx127x_readPacket(char* buff, int len) {
	for (int i = 0; i < len; i++) {
		*buff++ = sx127x_read(SX127X_REG_FIFO);
	}
}

#define RF_MID_BAND_THRESHOLD		525E6

int fun_sx127x_getRssi(uint32_t frequency) {
	//# 0x1A: RegPktRssiValue
	// ref: RegPktRssiValue page 112
	int offset = (frequency < RF_MID_BAND_THRESHOLD) ? 157 : 164;
	return sx127x_read(0x1A) - offset;
}

int fun_sx127x_getSNR() {
	//# 0x1B: RegPktSnrValue
	return sx127x_read(0x19) / 4;
}
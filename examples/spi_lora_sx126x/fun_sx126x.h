// MIT License

// Copyright (c) 2025 UniTheCat

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

// DataSheet
// ref: https://www.mouser.com/pdfDocs/DS_LLCC68_V10-2.pdf?srsltid=AfmBOooSw4VnT0K2j8FhU1Ni7i8inGRofGu9IxMCgdq02lwq11JMnoPt


#include "ch32fun.h"
#include <stdint.h>
#include <stdio.h>

#include "../../extralibs/ch32v003_SPI.h"

//! ####################################
//! SPI FUNCTIONS
//! ####################################

u8 LORA_CS_PIN2 = -1;

//# Write/Read Commands

void sx126x_write_CMD(u8 opCode, u8* data, u8 len) {
	if (LORA_CS_PIN2 != -1) funDigitalWrite(LORA_CS_PIN2, 0);
	SPI_transfer_8(opCode);

	for (int i=0; i<len; i++) {
		SPI_transfer_8(data[i]);
	}
	if (LORA_CS_PIN2 != -1) funDigitalWrite(LORA_CS_PIN2, 1);
}

void sx126x_read_CMD(u8 opCode, u8* data, u8 len) {
	if (LORA_CS_PIN2 != -1) funDigitalWrite(LORA_CS_PIN2, 0);
	SPI_transfer_8(opCode);

	for (int i=0; i<len; i++) {
		data[i] = SPI_transfer_8(data[i]);
	}
	if (LORA_CS_PIN2 != -1) funDigitalWrite(LORA_CS_PIN2, 1);
}

//# Write/Read Registers

void sx126x_write_REG(u16 addr, u8 *data, u8 len) {
	//# 0x0D: Write Register
	if (LORA_CS_PIN2 != -1) funDigitalWrite(LORA_CS_PIN2, 0);
	SPI_transfer_8(0x0D);
	SPI_transfer_8((u8)((addr >> 8) & 0xFF));	// trasfer MSB
	SPI_transfer_8((u8)(addr & 0xFF));			// transfer LSB

	for (int i=0; i<len; i++) {
		SPI_transfer_8(data[i]);
	}
	if (LORA_CS_PIN2 != -1) funDigitalWrite(LORA_CS_PIN2, 1);
}

void sx126x_read_REG(u16 addr, u8 *data, u8 len) {
	//# 0x1D: Read Register
	if (LORA_CS_PIN2 != -1) funDigitalWrite(LORA_CS_PIN2, 0);
	SPI_transfer_8(0x1D);
	SPI_transfer_8((u8)((addr >> 8) & 0xFF));	// trasfer MSB
	SPI_transfer_8((u8)(addr & 0xFF));			// transfer LSB
	SPI_transfer_8(0x00);						// Dummy Byte

	// read data
	for (int i=0; i<len; i++) {
		data[i] = SPI_transfer_8(data[i]);
	}

	if (LORA_CS_PIN2 != -1) funDigitalWrite(LORA_CS_PIN2, 1);
}

//# Write/Read Buffer

void sx126x_write_BUFF(u8 offset, u8* data, u8 len) {
	//# 0x0E: Write Buffer
	if (LORA_CS_PIN2 != -1) funDigitalWrite(LORA_CS_PIN2, 0);
	SPI_transfer_8(0x0E);
	SPI_transfer_8(offset);

	for (int i=0; i<len; i++) SPI_transfer_8(data[i]);
	if (LORA_CS_PIN2 != -1) funDigitalWrite(LORA_CS_PIN2, 1);
}

void sx126x_read_BUFF(u8 offset, u8 *data, u8 len) {
	//# 0x1E: Read Buffer
	if (LORA_CS_PIN2 != -1) funDigitalWrite(LORA_CS_PIN2, 0);
	SPI_transfer_8(0x1E);
	SPI_transfer_8(offset);
	SPI_transfer_8(0x00);						// Dummy Byte

	// read data
	for (int i=0; i<len; i++) {
		data[i] = SPI_transfer_8(data[i]);
	}

	if (LORA_CS_PIN2 != -1) funDigitalWrite(LORA_CS_PIN2, 1);	
}

//! ####################################
//! CONFIG FUNCTIONS
//! ####################################

// LoRa bandwidth
#define SX126X_BW_7800			0x00		// 7.8 kHz
#define SX126X_BW_10400			0x08		// 10.4 kHz
#define SX126X_BW_15600			0x01		// 15.6 kHz
#define SX126X_BW_20800			0x09		// 20.8 kHz
#define SX126X_BW_31250			0x02		// 31.25 kHz
#define SX126X_BW_41700			0x0A		// 41.7 kHz
#define SX126X_BW_62500			0x03		// 62.5 kHz
#define SX126X_BW_125000		0x04		// 125 kHz
#define SX126X_BW_250000		0x05		// 250 kHz
#define SX126X_BW_500000		0x06		// 500 kHz

void fun_sx126x_setFreq(uint32_t frequency) {
	if (frequency < 150E6 || frequency > 960E6) { return; }

	u8 buf[4];
	if (frequency < 446E6) {			// 430 - 440 Mhz
		buf[0] = 0x6B;
		buf[1] = 0x6F;
	}
	else if (frequency < 734E6) {		// 470 - 510 Mhz
		buf[0] = 0x75;
		buf[1] = 0x81;
	}
	else if (frequency < 828E6) {		// 779 - 787 Mhz
		buf[0] = 0xC1;
		buf[1] = 0xC5;
	}
	else if (frequency < 877E6) {		// 863 - 870 Mhz
		buf[0] = 0xD7;
		buf[1] = 0xDB;
	}
	else if (frequency < 1100E6) {		// 902 - 928 Mhz
		buf[0] = 0xE1;
		buf[1] = 0xE9;
	}

	// ref: `9.2.1 Image Calibration for Specific Frequency Bands`
	// default 902-928Mhz band.
	//# 0x98: set calibration image (OPTIONAL?)
	sx126x_write_CMD(0x98, buf, 2);
	Delay_Ms(10);

	uint32_t freq = ((uint64_t) frequency << 25) / 32E6;
	buf[0] = (u8)((freq >> 24) & 0xFF);
	buf[1] = (u8)((freq >> 16) & 0xFF);
	buf[2] = (u8)((freq >> 8) & 0xFF);
	buf[3] = (u8)(freq & 0xFF);

	//# 0x84: set frequency
	sx126x_write_CMD(0x86, buf, 4);
}

void fun_sx126x_setModulation(u8 sf, u8 bw, u8 cr, u8 lowDataRateOptimization) {
	// ref: `6.1.1.1 Spreading Factor`
	// Symbol_Rate = BW / (2^SF)
	// Higher SF provides better receiver sensitivity at the expense of longer transmission time
	// Higher BW allows higher data rate at the expense of reduced sensitivity	
	// Higher CR (Coding Rate) allows higher data rate at the expense of reduced sensitivity
	// CR 0x01 = 4/5, CR 0x02 = 4/6, CR 0x03 = 4/7, CR 0x04 = 4/8
	// lowDataRateOptimization can be used for low data rates (high FS for low BW) and payloads
	// which last longer time on air in order to allow the receiver to get better LoRa signal

	// ref: `Time On Air`
	// ToA = 2^FS * Nsymbol / BW(kHz)

	// SF5 and SF6 works for SX1262 and LLCC68 but NOT SX1276
	// valid spreading factor is between 5 and 12
	sf = (sf < 5) ? 5 : sf;
	sf = (sf > 12) ? 12 : sf;

	u8 buf[4] = {
		sf, bw, cr, lowDataRateOptimization & 0x01
	};

	//# 0x8B: set modulation
	sx126x_write_CMD(0x8B, buf, 4);
}

// setTxPower
#define SX126X_PA_DUTYCYCLE_22DBM		0x04		// +22 dBm
#define SX126X_PA_DUTYCYCLE_20DBM		0x03		// +20 dBm
#define SX126X_PA_DUTYCYCLE_LOWDBM		0x02		// +14 dBm or +17 dBm

// Ramp time
#define SX126X_PA_RAMP_10US				0x00		// 10 us
#define SX126X_PA_RAMP_20US				0x01		// 20 us
#define SX126X_PA_RAMP_40US				0x02		// 40 us
#define SX126X_PA_RAMP_80US				0x03		// 80 us
#define SX126X_PA_RAMP_200US			0x04		// 200 us
#define SX126X_PA_RAMP_800US			0x05		// 800 us
#define SX126X_PA_RAMP_1700US			0x06		// 1700 us
#define SX126X_PA_RAMP_3400US			0x07		// 3400 us

void fun_sx126x_setTxPower(s8 power, u8 paDutyCycle) {
	// ref: `Table 13-21: PA Operating Modes with Optimal Settings`
	u8 hpMax = 0x02;	// 0x02 = +14 dBm, 0x03 = +17 dBm

	switch (paDutyCycle) {
		case SX126X_PA_DUTYCYCLE_22DBM: hpMax = 0x04; break;
		case SX126X_PA_DUTYCYCLE_20DBM: hpMax = 0x03; break;
		default: break;
	}

	u8 buf[4] = {
		paDutyCycle,
		hpMax,
		0x00,			// Device Select
		0x01			// PowerLUT. Reserved value 0x01
	};

	//# 0x95: set PA (Power Amplifier) and TX power setting
	sx126x_write_CMD(0x95, buf, 4);
	Delay_Ms(10);
	u8 rampTime = SX126X_PA_RAMP_200US;

	// High Power Mode -9 dBm (0xF7) to +22 dBm (0x16) in step of 1 dB
	power = (power < -9) ? -9 : power;
	power = (power > 22) ? 22 : power;
	buf[0] = power;
	buf[1] = rampTime;

	//# 0x8E: set TX power
	sx126x_write_CMD(0x8E, buf, 2);
	Delay_Ms(10);
}

void fun_sx126x_setPacketParams(
	u16 preambleLen, u8 headerType, u8 payloadLen, u8 crcOn, u8 invertIQ
) {
	// ref: `6.1.3.1 Explicit Header Mode`
	// header type 0 = explicit, 1 = implicit
	headerType = (headerType == 0) ? 0 : 1;
	u8 buff[6] = {
		preambleLen >> 8, preambleLen,
		headerType,
		payloadLen,
		crcOn,
		invertIQ,
	};

	//# 0x8C: set packet params
	sx126x_write_CMD(0x8C, buff, 6);
}

void fun_sx126x_setDioIrqParams(
	u16 irqMask, u16 dio1Mask, u16 dio2Mask, u16 dio3Mask
) {
	u8 buff[8] = {
		(u8)((irqMask >> 8) & 0xFF),
		(u8)(irqMask & 0xFF),
		(u8)((dio1Mask >> 8) & 0xFF),
		(u8)(dio1Mask & 0xFF),
		(u8)((dio2Mask >> 8) & 0xFF),
		(u8)(dio2Mask & 0xFF),
		(u8)((dio3Mask >> 8) & 0xFF), 
		(u8)(dio3Mask & 0xFF)
	};

	//# 0x08: set IRQ params
	sx126x_write_CMD(0x08, buff, 8);
}

// ref: `Table 9-3: Rx Gain Configuration`
void fun_sx126x_setRxGain(u8 boost) {
	//# addr 0x08AC: set Rx gain
	u8 buf[1] = { 0x94 };			// 0x94 = 0dBm default, 0x96 = 14dBm
	if (boost) buf[0] = 0x96;
	sx126x_write_REG(0x08AC, buf, 1);
}

void fun_sx126x_setSyncWord(u16 syncWord) {
	u8 buf[2];
	buf[0] = (u8)((syncWord >> 8) & 0xFF),
	buf[1] = (u8)(syncWord & 0xFF),

	//# addr 0x0740: set sync word
	sx126x_write_REG(0x0740, buf, 2);
}

void fun_sx126x_setBufferBaseAddr(u8 txAddress, u8 rxAddress) {
	//# 0x8F: set buffer base address
	u8 buff[2];
	buff[0] = txAddress;
	buff[1] = rxAddress;
	sx126x_write_CMD(0x8F, buff, 2);
}

//! ####################################
//! INIT FUNCTIONS
//! ####################################

// Chip mode
#define SX126X_MODE_STDBY_RC			0x20
#define SX126X_MODE_STDBY_XOSC			0x30
#define SX126X_STATUS_MODE_FS			0x40
#define SX126X_STATUS_MODE_RX			0x50
#define SX126X_STATUS_MODE_TX			0x60

// SetDioIrqParams
#define SX126X_IRQ_NONE					0x0000		// no interrupts
#define SX126X_IRQ_TX_DONE				0x0001		// packet transmission completed
#define SX126X_IRQ_RX_DONE				0x0002		// packet received
#define SX126X_IRQ_PREAMBLE_DETECTED	0x0004		// preamble detected
#define SX126X_IRQ_SYNC_WORD_VALID		0x0008		// valid sync word detected
#define SX126X_IRQ_HEADER_VALID			0x0010		// valid LoRa header received
#define SX126X_IRQ_HEADER_ERR			0x0020		// LoRa header CRC error
#define SX126X_IRQ_CRC_ERR				0x0040		// wrong CRC received
#define SX126X_IRQ_CAD_DONE				0x0080		// channel activity detection finished
#define SX126X_IRQ_CAD_DETECTED			0x0100		// channel activity detected
#define SX126X_IRQ_TIMEOUT				0x0200		// Rx or Tx timeout
#define SX126X_IRQ_ALL					0x03FF		// all interrupts

u8 SX126X_OK = 0;

// #define DIO_PIN		 PD4

#define SX126X_PREAMBLE_LEN			12
#define SX126X_HEADER_IMPLICIT		0x00	// 0x00: implicit, 0x01: explicit

void fun_sx126x_RXMode(u32 timeoutMs) {
	u8 buf[3];
	//# packet configuration 
	// (preambleLen, headerType, payloadLen, crcOn, invertIQ)
	fun_sx126x_setPacketParams(
		SX126X_PREAMBLE_LEN, SX126X_HEADER_IMPLICIT,
		255, 0, 0
	);
	Delay_Ms(10);

	//! 0x82: START Rx with timeout - 0xFFFFFF to listen continously
	buf[0] = (u8)((timeoutMs >> 16) & 0xFF);
	buf[1] = (u8)((timeoutMs >> 8) & 0xFF);
	buf[2] = (u8)(timeoutMs & 0xFF);
	sx126x_write_CMD(0x82, buf, 3);
	Delay_Ms(10);
}

void fun_sx126x_init(uint32_t frequency, u8 cs_pin) {
	// funPinMode(DIO_PIN, GPIO_CFGLR_IN_PUPD);
	u8 buf[9];

	//! configure CS Pin
	if (cs_pin != -1) {
		LORA_CS_PIN2 = cs_pin;
		funPinMode(cs_pin, GPIO_Speed_10MHz | GPIO_CNF_OUT_PP);
		funDigitalWrite(cs_pin, 1);
	}

	//! 0x0740 addr: default sync_word - sanity check
	u8 default_syncWord[2];
	sx126x_read_REG(0x0740, default_syncWord, 2);
	printf("Default SyncWord: 0x%02X 0x%02X\n", default_syncWord[0], default_syncWord[1]);
	SX126X_OK = default_syncWord[1] == 0x24;	//! Expect 0x2414
	printf("SX126X_OK1: %d\n", SX126X_OK);

	//# 0x8A: set modem (*REQUIRED*)
	buf[0] = 0x01;	// 0x00 = GFSK, 0x01 = LoRa
	sx126x_write_CMD(0x8A, buf, 1);
	Delay_Ms(10);

	//! 0x11: Get modem - sanity check
	sx126x_read_CMD(0x11, buf, 2);
	printf("packetType: 0x%02X 0x%02X\n", buf[0], buf[1]);
	SX126X_OK = buf[1] == 0x01;				 //! Expect 0x01
	printf("SX126X_OK2: %d\n", SX126X_OK);

	// # 0x80: set standby mode
	buf[0] = 0x00;	  // 0x00 = RC (low power), 0x01 = XOSC (performant)
	sx126x_write_BUFF(0x80, buf, 1);
	Delay_Ms(10);

	//# 0x98: set calibration image
	//# 0x84: set frequency
	fun_sx126x_setFreq(frequency);
	Delay_Ms(10);

	//# 0x8B: set modulation (*REQUIRED*)
	u8 cr = 0x01;	 // CR 0x01 = 4/5, CR 0x02 = 4/6, CR 0x03 = 4/7, CR 0x04 = 4/8
	fun_sx126x_setModulation(7, SX126X_BW_125000, cr, 0);
	Delay_Ms(10);

	//# 0x95: set PA and TX power setting
	//# 0x8E: set TX power
	fun_sx126x_setTxPower(22, SX126X_PA_DUTYCYCLE_22DBM);
	Delay_Ms(10);

	//# 0x08: set IRQ params
	fun_sx126x_setDioIrqParams(
		0x0002,		// IRQ mask for receiving
		0xFFFF,		// DIO1 mask turn ON
		0x0000,		// DIO2 mask
		0x0000		// DIO3 mask
	);
	Delay_Ms(10);

	//! 0x82: START Rx with timeout
	fun_sx126x_RXMode(0xFFFFFF);
}

void fun_sx126x_clearIQR_status() {
	u16 clearMask = 0xFFFF;		// flag to select all IRQs to clear
	u8 buf[2] = {
		(u8)((clearMask >> 8) & 0xFF),
		(u8)(clearMask & 0xFF)
	};

	//# 0x02: clear all IRQ status
	sx126x_write_CMD(0x02, buf, 2);
}

// ref: 13.5.1 `GetStatus`
#define SX126X_CHIPMODE_STBY_RC 0x02
#define SX126X_CHIPMODE_STBY_XOSC 0x03
#define SX126X_CHIPMODE_FS 0x04
#define SX126X_CHIPMODE_RX 0x05
#define SX126X_CHIPMODE_TX 0x06

void fun_sx126x_printIQR_status(u8 chipMode, u8 cmdStatus) {
	//# 0x12: get IRQ status
	// ref: `13.3.3 GetIrqStatus`
	u8 buf[2];
	sx126x_read_CMD(0x12, buf, 2);

	// ref: Table 8-4: IRQ Status Registers
	// bit0: TxDone
	// bit1: RxDone
	// bit2: PreambleDetected
	// bit3: SyncWordValid
	// bit4: HeaderValid
	// bit5: HeaderErr
	// bit6: CRCErr
	// bit7: CadDone
	// bit8: CadDetected

	const char *chipModeStr = (chipMode == SX126X_CHIPMODE_RX) ? "RX" : 
					(chipMode == SX126X_CHIPMODE_TX) ? "TX" : "__";
	printf("chipMode %s, cmdStatus %d	 ", chipModeStr, cmdStatus);
	
	for (int i = 7; i >= 1; i--) printf("%d ", (buf[0] >> i) & 1);
	printf("\n");
	Delay_Ms(1);
}


// #define SX126X_RECEIVE_DEBUG
// #define SX126X_SEND_DEBUG

//! ####################################
//! RECEIVE FUNCTION
//! ####################################

// ref: `Table 13-76: Status Bytes Definition`
#define SX126X_CMDSTATUS_RX_AVAILABLE	0x02
#define SX126X_CMDSTATUS_TIMEOUT		0x03
#define SX126X_CMDSTATUS_TX_DONE		0x06

u8 is_transmiting = 0;

void fun_sx126x_getReceivedMessage(
	u8 *data, u8 len, u8 memoryIndex,
	s16 *rssi, s16 *snr
) {
	sx126x_read_BUFF(memoryIndex, data, len);
	u8 buf[4];

	// ref: `7.2 Data Buffer in Receive Mode`
	//# 0x14: get packet status
	sx126x_read_CMD(0x14, buf, 4);
	u8 status = buf[0];
	*rssi = - buf[1] / 2;
	*snr = buf[2] / 4;
}

u8 fun_sx126x_parsePacket(u8 *memoryIndex, u32 timeoutMs) {
	// return;

	//# 0xC0: get Status - ref: 13.5.1 `GetStatus`
	u8 status;
	sx126x_read_CMD(0xC0, &status, 1);
	u8 chipMode = (status >> 4) & 0b111;	// bit 6:4
	u8 cmdStatus = (status >> 1) & 0b111;	// bit 3:1

	//! filter for error
	// 0x04 = Processing Error, 0x05 = Command Error
	if (cmdStatus == 0x04 | cmdStatus == 0x05) return 0;

	if (cmdStatus == SX126X_CMDSTATUS_TX_DONE || is_transmiting == 0) {
		//! 0x82: START Rx with timeout
		fun_sx126x_RXMode(timeoutMs);

		is_transmiting = 0;
		Delay_Ms(10);
	}

#ifdef SX126X_RECEIVE_DEBUG
	# 0x12: get IRQ status
	printf("RX: ");
	fun_sx126x_printIQR_status(chipMode, cmdStatus);
#endif

	//! check if rx available flag
	if (cmdStatus != SX126X_CMDSTATUS_RX_AVAILABLE) return 0;
	if (chipMode == SX126X_CHIPMODE_RX) return 0;

	// if (funDigitalRead(DIO_PIN) == 0) return 0;

	//# 0x13: get buffer status
	u8 buf[4];
	sx126x_read_CMD(0x13, buf, 3);
	u8 payloadLen = buf[1];
	*memoryIndex = buf[2];

	//# 0x02: clear all IRQ status
	fun_sx126x_clearIQR_status();
	Delay_Ms(10);

	return payloadLen;
}


//! ####################################
//! SEND FUNCTION
//! ####################################

// u32 send_time = 0;

void fun_sx126x_send(char* message, u8 len, u32 timeoutMs) {
	if (!SX126X_OK) {
		printf("sx126x not ok\n");
		return;
	}
	
	//# 0xC0: get Status - ref: 13.5.1 `GetStatus`
	u8 status;
	sx126x_read_CMD(0xC0, &status, 1);
	u8 chipMode = (status >> 4) & 0b111;	// bit 6:4
	u8 cmdStatus = (status >> 1) & 0b111;	// bit 3:1

	//# filter for error
	// 0x04 = Processing Error, 0x05 = Command Error
	if (cmdStatus == 0x04 | cmdStatus == 0x05) return;

	// if (millis() - send_time < 5000) return;
	// send_time = millis();

#ifdef SX126X_SEND_DEBUG
	//# 0x12: get IRQ status
	printf("*******TX: ");
	fun_sx126x_printIQR_status(chipMode, cmdStatus);
#endif
	
	//! packet configuration
	// (preambleLen, headerType, payloadLen, crcOn, invertIQ)
	fun_sx126x_setPacketParams(
		SX126X_PREAMBLE_LEN, SX126X_HEADER_IMPLICIT,
		len, 1, 0
	);
	Delay_Ms(10);

	//# 0x8F: reset buffer base address
	u8 buf[4];
	buf[0] = 0x00;
	buf[1] = 0x00;
	sx126x_write_CMD(0x8F, buf, 2);
	Delay_Ms(10);

	// ref: `7.3 Data Buffer in Transmit Mode`
	//# 0x0E: Write Buffer
	sx126x_write_BUFF(0x00, (u8*)message, len);
	Delay_Ms(10);

	//! 0x83: START Tx with timeout
	buf[0] = (u8)((timeoutMs >> 16) & 0xFF);
	buf[1] = (u8)((timeoutMs >> 8) & 0xFF);
	buf[2] = (u8)(timeoutMs & 0xFF);
	sx126x_write_CMD(0x83, buf, 3);
	Delay_Ms(10);

	//# 0x02: clear all IRQ status
	fun_sx126x_clearIQR_status();
	Delay_Ms(10);
	
	is_transmiting = 1;
}
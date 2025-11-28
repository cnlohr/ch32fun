// MIT License (MIT)
// Copyright (c) 2025 UniTheCat
// i2c_error() method borrowed from https://github.com/ADBeta/CH32V003_lib_i2c

#include "ch32fun.h"
#include <stdio.h>

#define I2C_DEFAULT_TIMEOUT 100000
#define SYSTEM_CLOCK_MHZ (FUNCONF_SYSTEM_CORE_CLOCK/1000000)

#define BUF_MAKE_U16(buff) ((buff[0] << 8) | buff[1])

u8 i2c_get_error() {
	if (R16_I2C_STAR1 & RB_I2C_BERR)	{ R16_I2C_STAR1 &= ~RB_I2C_BERR;	return 1; }		// Bus Error
	if (R16_I2C_STAR1 & RB_I2C_ARLO)	{ R16_I2C_STAR1 &= ~RB_I2C_ARLO;	return 2; }		// Arbitration lost
	if (R16_I2C_STAR1 & RB_I2C_AF)		{ R16_I2C_STAR1 &= ~RB_I2C_AF;		return 3; }		// ACK Failure
	if (R16_I2C_STAR1 & RB_I2C_OVR)		{ R16_I2C_STAR1 &= ~RB_I2C_OVR;		return 4; }		// Overrun
	if (R16_I2C_STAR1 & RB_I2C_PECERR)	{ R16_I2C_STAR1 &= ~RB_I2C_PECERR;	return 5; }		// PEC Error
	return 0;
}


//! ####################################
//! INIT FUNCTION
//! ####################################

u8 i2c_init(u16 i2c_speed_khz) {
	//! REQUIRED: Software reset
	R16_I2C_CTRL1 |= RB_I2C_SWRST;
	R16_I2C_CTRL1 &= ~RB_I2C_SWRST;

	// Clear FREQ bits and set system clock frequency
	R16_I2C_CTRL2 &= ~RB_I2C_FREQ;
	R16_I2C_CTRL2 |= (SYSTEM_CLOCK_MHZ & RB_I2C_FREQ);
	
	// Clear pheriphera
	R16_I2C_CTRL1 &= ~RB_I2C_PE;
	u16 clock_config;
	
	if (i2c_speed_khz <= 100) {
		R16_I2C_RTR = (1 + SYSTEM_CLOCK_MHZ) > 0x3F ? 0x3F : (1 + SYSTEM_CLOCK_MHZ);

		clock_config = (SYSTEM_CLOCK_MHZ * 1000 / (i2c_speed_khz << 1)) & RB_I2C_CCR;
		if (clock_config < 4) clock_config = 4;
		printf("clock_config: %d\r\n", clock_config);
		printf("RTR: %d\r\n", R16_I2C_RTR);
	} 
	else {
		R16_I2C_RTR = 1 + SYSTEM_CLOCK_MHZ * 300 / 1000;

		clock_config = (SYSTEM_CLOCK_MHZ * 1000 / (i2c_speed_khz * 3)) & RB_I2C_CCR;
		if (clock_config == 0) clock_config = 1;
		clock_config |= RB_I2C_F_S;
	}

	// Set CCR value
	R16_I2C_CKCFGR = clock_config;

	// Enable peripheral
	R16_I2C_CTRL1 = RB_I2C_PE;
	R16_I2C_CTRL1 |= RB_I2C_ACK;	// Enable ACK

	return i2c_get_error();
}


//! ####################################
//! I2C START FUNCTION
//! ####################################

u8 i2c_start(u8 address, u8 isRead) {
	//# STEP 1: Generate START
	R16_I2C_CTRL1 |= RB_I2C_START;
	
	//# STEP 2: Wait for SB with timeout
	u32 timeout = I2C_DEFAULT_TIMEOUT;
	while (!(R16_I2C_STAR1 & RB_I2C_SB) && --timeout);
	if (timeout == 0) {
		R16_I2C_CTRL1 |= RB_I2C_STOP;
		return i2c_get_error() | 0x20;
	}

	//# STEP 3: Clear SB (READ STAR1 then write DATAR)
	volatile u16 status = R16_I2C_STAR1;  	// Read to clear SB
	R16_I2C_DATAR = (address << 1) | isRead;
	
	//# STEP 4: Wait for ADDR with timeout
	timeout = I2C_DEFAULT_TIMEOUT;
	while (!(R16_I2C_STAR1 & RB_I2C_ADDR) && --timeout);
	if (timeout == 0) {
		R16_I2C_CTRL1 |= RB_I2C_STOP;
		return i2c_get_error() | 0x30;
	}

	//# STEP 5: Clear ADDR by reading registers
	status = R16_I2C_STAR1;
	status = R16_I2C_STAR2;

	return i2c_get_error();
}

u8 i2c_ping(u8 address) {
	u8 ret = i2c_start(address, 0);

	//# STOP I2C
	R16_I2C_CTRL1 |= RB_I2C_STOP;
	return ret;
}

void i2c_scan(void (*onPingFound)(u8 address)) {
	// mininum 0x08 to 0x77 (0b1110111)
	for (int i = 0x08; i < 0x77; i++) {
		u8 ping = i2c_ping(i);
		if (ping == 0) onPingFound(i);
	}
}

//! ####################################
//! WRITE DATA FUNCTION
//! ####################################

u8 i2c_writeData(u8 address, u8 *data, u8 len) {
	//# STEP 1: Send Start
	u8 ret = i2c_start(address, 0);
	if (ret != 0) return ret;

	for (u8 i = 0; i < len; i++) {
		u32 timeout = I2C_DEFAULT_TIMEOUT;
		while(!(R16_I2C_STAR1 & RB_I2C_TxE) && --timeout);
		if (timeout == 0) {
			R16_I2C_CTRL1 |= RB_I2C_STOP;
			return i2c_get_error() | 0x40;
		}
		R16_I2C_DATAR = data[i];

		//# STEP 2: Wait for BTF (Byte Transfer Finished) - all data shifted out
		timeout = I2C_DEFAULT_TIMEOUT;
		while(!(R16_I2C_STAR1 & RB_I2C_BTF) && --timeout);
		// skip error checking to handle clock stretching devices
		if (timeout == 0) {
			R16_I2C_CTRL1 |= RB_I2C_STOP;
			return i2c_get_error() | 0x50;
		}
	}

	//# STEP 3: Clear BTF - Read STAR1
	volatile u16 status = R16_I2C_STAR1;

	//# STOP I2C
	R16_I2C_CTRL1 |= RB_I2C_STOP;
	return i2c_get_error();
}


//! ####################################
//! READ DATA FUNCTION
//! ####################################

u8 i2c_readData(u8 address, u8 *data, u8 len) {
	//# STEP 1: Send Start
	u8 check = i2c_start(address, 1);
	if (check != 0) {
		R16_I2C_CTRL1 |= RB_I2C_STOP;
		return check;
	}

	// Configure ACK for multi-byte read
	if(len > 1) {
		// Enable ACK for all bytes except last
		R16_I2C_CTRL1 |= RB_I2C_ACK;
	} else {
		// Single byte - disable ACK (send NACK after first byte)
		R16_I2C_CTRL1 &= ~RB_I2C_ACK;
	}

	// Read data bytes
	for(u8 i = 0; i < len; i++) {
		if(i == len - 1) {
			// Last byte - disable ACK to send NACK
			R16_I2C_CTRL1 &= ~RB_I2C_ACK;
		}

		//# STEP 2: Wait for RxNE (Receive Data Register Not Empty)
		u32 timeout = I2C_DEFAULT_TIMEOUT;
		while(!(R16_I2C_STAR1 & RB_I2C_RxNE) && --timeout);
		if (timeout == 0) {
			R16_I2C_CTRL1 |= RB_I2C_STOP;
			return i2c_get_error() | 0x60;
		}
		
		// Read data byte
		data[i] = R16_I2C_DATAR;

		//# STEP 3: Check for BTF and clear if needed
		if (R16_I2C_STAR1 & RB_I2C_BTF) {
			volatile uint16_t status = R16_I2C_STAR1;
			data[i] = R16_I2C_DATAR; // Read again to clear BTF
		}
	}

	//# STEP 4: STOP I2C
	R16_I2C_CTRL1 |= RB_I2C_STOP;
	return i2c_get_error();
}
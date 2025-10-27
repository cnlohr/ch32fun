/* MIT License (MIT)
 * Copyright (c) 2025 UniTheCat
 * i2c_error() method borrowed from https://github.com/ADBeta/CH32V003_lib_i2c
*/ 

#include "ch32fun.h"
#include <stdio.h>

#define I2C_SDA PB12
#define I2C_SCL PB13

#define I2C_TIMEOUT 100000
#define SYSTEM_CLOCK_MHZ (FUNCONF_SYSTEM_CORE_CLOCK/1000000)

#define I2C_ADDR_WRITE_MASK 0xFE
#define I2C_ADDR_READ_MASK 0x01

#define BUF_MAKE_U16(buff) ((buff[0] << 8) | buff[1])

// return 1 = Success, 0 = Timeout
#define I2C_WAIT_FOR(condition) ({ \
	u32 timeout = I2C_TIMEOUT; \
	u8 result = 0; \
	while (!(condition)) { \
		if (--timeout == 0) { result = 0; break; } \
	} \
	if (timeout > 0) result = 1; \
	result; \
})


void print_bits(u16 val) {
	for (int i = 15; i >= 0; i--) { 
		printf("%d ", (val >> i) & 1);
		if (i == 8) printf("| ");
	}
}

// I2C_CTRL1
// [15] SWRST, [10] ACK_EN, [9] STOP, [8] START, [3] SMBTYPE, [2] Reserved, [1] MSBUS, [0] PE

// I2C_STAR1
// [15] SMBALERT, [14] TIMEOUT, [13] Reserved, [12] PECERR, [11] OVR, [10] AF, [9] ARLO, 
//[8] BEER, [7] TxE, [6] RxNE, [5] Reserved, [4] STOPF, [3] DD10, [2] BTF, [1] ADDR, [0] SB

// I2C_STAR2
// [2] TRA, [1] BUSY, [0] MSL

void i2c_debug_print(void) {
	printf("CTRL1:  0x%04X \t", R16_I2C_CTRL1);		print_bits(R16_I2C_CTRL1); printf("\r\n");
	printf("CTRL2:  0x%04X \t", R16_I2C_CTRL2);		print_bits(R16_I2C_CTRL2); printf("\r\n");
	printf("STAR1:  0x%04X \t", R16_I2C_STAR1); 	print_bits(R16_I2C_STAR1); printf("\r\n");
	printf("STAR2:  0x%04X \t", R16_I2C_STAR2);		print_bits(R16_I2C_STAR2); printf("\r\n");
	// printf("CKCFGR: 0x%04X \t", R16_I2C_CKCFGR);	print_bits(R16_I2C_CKCFGR); printf("\r\n");
	// printf("RTR:	0x%04X \t", R16_I2C_RTR);		print_bits(R16_I2C_RTR); printf("\r\n");
	// printf("OADDR1: 0x%04X \t", R16_I2C_OADDR1);	print_bits(R16_I2C_OADDR1); printf("\r\n");
	printf("DATAR:  0x%04X\r\n", R16_I2C_DATAR);
	printf("=========================\r\n\n");
}

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
	funPinMode( I2C_SDA, GPIO_CFGLR_IN_PUPD );
	funPinMode( I2C_SCL, GPIO_CFGLR_IN_PUPD );
	funDigitalWrite( I2C_SDA, 1 );
	funDigitalWrite( I2C_SCL, 1 );

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

u8 _I2C_START(u8 address, u8 addr_mask) {
	//# STEP 1: Generate START
	R16_I2C_CTRL1 |= RB_I2C_START;
	
	//# STEP 2: Wait for SB with timeout
	I2C_WAIT_FOR(R16_I2C_STAR1 & RB_I2C_SB);
	u8 ret = i2c_get_error();
	if (ret != 0) {
		R16_I2C_CTRL1 |= RB_I2C_STOP;
		return 0x10 | ret;
	}

	//# STEP 3: Clear SB (READ STAR1 then write DATAR)
	volatile u16 status = R16_I2C_STAR1;  	// Read to clear SB

	// mask: 0xFE = write, 0x01 = read
	if (addr_mask == I2C_ADDR_WRITE_MASK) {
		R16_I2C_DATAR = (address << 1) & addr_mask;
	} else {
		R16_I2C_DATAR = (address << 1) | addr_mask;
	}
	
	//# STEP 4: Wait for ADDR with timeout
	I2C_WAIT_FOR(R16_I2C_STAR1 & RB_I2C_ADDR);
	ret = i2c_get_error();
	if (ret != 0) {
		R16_I2C_CTRL1 |= RB_I2C_STOP;
		return 0x20 | ret;
	}

	//# STEP 5: Clear ADDR by reading registers
	status = R16_I2C_STAR1;
	status = R16_I2C_STAR2;

	return i2c_get_error();
}

u8 i2c_ping(u8 address) {
	u8 ret = _I2C_START(address, I2C_ADDR_WRITE_MASK);

	//# STOP I2C
	R16_I2C_CTRL1 |= RB_I2C_STOP;
	return ret;
}


//! ####################################
//! WRITE DATA FUNCTION
//! ####################################

u8 i2c_writeData(u8 address, u8 *data, u8 len) {
	//# STEP 1: Send Start
	u8 ret = _I2C_START(address, I2C_ADDR_WRITE_MASK);
	if (ret != 0) return ret;

	for (u8 i = 0; i < len; i++) {
		u8 check = I2C_WAIT_FOR(R16_I2C_STAR1 & RB_I2C_TxE);
		if (!check) {
			R16_I2C_CTRL1 |= RB_I2C_STOP;
			return 0x30 | i2c_get_error();
		}
		R16_I2C_DATAR = data[i];

		//# STEP 2: Wait for BTF (Byte Transfer Finished) - all data shifted out
		check = I2C_WAIT_FOR(R16_I2C_STAR1 & RB_I2C_BTF);
		// skip error checking to handle clock stretching devices
		// if (!check) return 0x40 | i2c_get_error();
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
	u8 check = _I2C_START(address, I2C_ADDR_READ_MASK);
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
		u8 check = I2C_WAIT_FOR(R16_I2C_STAR1 & RB_I2C_RxNE);
		if (!check) {
			R16_I2C_CTRL1 |= RB_I2C_STOP;
			return 0x50 | i2c_get_error();
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
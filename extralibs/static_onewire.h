/*
	Copyright 2025 monte-monte
	MIT license

	Generic 1-wire library for any GPIO-based system.

	WiP.

	Include this in your .c file only!!!
	Include it after the following are defined:

	ONEPREFIX   -> #define to be the prefix, i.e. BOB will cause BOBConfigONE
		to be generated.
	ONENOSTATIC -> #define if you want the functions to be generated as
		not-static code.
*/

/* Put this in your code:

	#define DELAY(n)   Delay_Us(n)
	#define ONE_INPUT  { funPinMode(PIN_ONE, GPIO_CFGLR_IN_PUPD); funDigitalWrite(PIN_ONE, 1); } 
	#define ONE_OUTPUT { funDigitalWrite(PIN_ONE, 0); funPinMode(PIN_ONE, GPIO_CFGLR_OUT_2Mhz_PP); } 
	#define ONE_SET    { funDigitalWrite(PIN_ONE, 1); }
	#define ONE_CLEAR  { funDigitalWrite(PIN_ONE, 0); }
	#define ONE_READ   { funDigitalRead(PIN_ONE); }

	#define ONENEEDSCAN       1
	#define ONENEEDCRC8_TABLE 1
	#define __ONE_TABLE_IN_FLASH __attribute__((section(".srodata"), used))

	#include "static_onewire.h"
*/

typedef union {
	unsigned char rom[8];
	struct {
		unsigned char code;
		unsigned char serial[6];
		unsigned char crc;
	};
} __attribute__((packed)) OneWire_dev_t;

#define ONEDELAY_A DELAY(6)  // (3)
#define ONEDELAY_B DELAY(64) // (55) 
#define ONEDELAY_C DELAY(60) // (65)
#define ONEDELAY_D DELAY(10) // (5)
#define ONEDELAY_E DELAY(9)  // (10)
#define ONEDELAY_F DELAY(55) // (53)
#define ONEDELAY_G 
#define ONEDELAY_H DELAY(480)
#define ONEDELAY_I DELAY(70)
#define ONEDELAY_J DELAY(410)

#ifndef INTERRUPT_OFF
#define INTERRUPT_OFF __disable_irq()
#endif
#ifndef INTERRUPT_ON
#define INTERRUPT_ON __enable_irq()
#endif

#define ONEWIRE_SEARCH_ROM_CMD      0xF0
#define ONEWIRE_COND_SEARCH_ROM_CMD 0xEC
#define ONEWIRE_READ_ROM_CMD        0x33
#define ONEWIRE_SELECT_CMD          0x55
#define ONEWIRE_SKIP_CMD            0xCC

#ifndef ONEPREFIX
	#define ONEPREFIX
#endif

#ifndef ONENOSTATIC
	#define ONESTATICODE
#else
	#define ONESTATICODE static
#endif

#ifndef ONEFNCOLLAPSE
	#define INTONEFNCOLLAPSE(PFX, name) PFX##name
	#define ONEFNCOLLAPSE(PFX, name)    INTONEFNCOLLAPSE(PFX, name)
#endif

#ifndef ONENEEDGETBYTE
	#define ONENEEDGETBYTE 1
#endif

#ifndef __ONE_TABLE_IN_FLASH
	#define __ONE_TABLE_IN_FLASH
#endif

#ifndef ONE_CRC_POLYNOMIAL
	#define ONE_CRC_POLYNOMIAL 0x8C
#endif

ONESTATICODE unsigned int ONEFNCOLLAPSE(ONEPREFIX, Reset)() {
	unsigned int state;
	unsigned char retries = 125;
	ONE_INPUT;

	do {
		DELAY(2);
	} while(!ONE_READ && --retries);

	INTERRUPT_OFF;
	ONE_OUTPUT;
	ONE_CLEAR;
	ONEDELAY_H;
	// ONE_SET;
	ONE_INPUT;
	ONEDELAY_I;

	state = ONE_READ;
	ONEDELAY_J;
	INTERRUPT_ON;
	// ONE_OUTPUT;

	return !!state;
}

ONESTATICODE void ONEFNCOLLAPSE(ONEPREFIX, SendBit)(const unsigned char bit) {
	ONE_OUTPUT;
	ONE_CLEAR;

	INTERRUPT_OFF;
	if (bit) ONEDELAY_A;
	else ONEDELAY_C;
	// if (bit) DELAY(10);
	// else DELAY(65);

	ONE_SET;

	if (bit) ONEDELAY_B;
	else ONEDELAY_D;
	INTERRUPT_ON;
	// if (bit) DELAY(55);
	// else DELAY(5);
}

ONESTATICODE void ONEFNCOLLAPSE(ONEPREFIX, SendByte)(const unsigned char data) {
	for (unsigned char i = 1; i != 0; i <<= 1) {
		ONEFNCOLLAPSE(ONEPREFIX, SendBit)(data & i);
	}
	// ONE_CLEAR; // This makes reads unreliable on my knock-offs of db18b20
}

ONESTATICODE unsigned char ONEFNCOLLAPSE(ONEPREFIX, GetBit)() {
	unsigned char ret = 0;

	INTERRUPT_OFF;
	ONE_OUTPUT;
	ONE_CLEAR;
	ONEDELAY_A;
	ONE_INPUT;
	ONEDELAY_E;

	ret = (ONE_READ);
	ONEDELAY_F;
	INTERRUPT_ON;

	// ONE_OUTPUT;
	return ret;
}

ONESTATICODE unsigned char ONEFNCOLLAPSE(ONEPREFIX, GetByte)() {
	unsigned char ret = 0;

	for (unsigned char i = 0; i < 8; i++) {
		ret |= ONEFNCOLLAPSE(ONEPREFIX, GetBit)() << i;
	}

	return ret;
}

ONESTATICODE void ONEFNCOLLAPSE(ONEPREFIX, Select)(OneWire_dev_t device) {
	ONEFNCOLLAPSE(ONEPREFIX, SendByte)(ONEWIRE_SELECT_CMD);

	for (int i = 0; i < 8; i++) {
		ONEFNCOLLAPSE(ONEPREFIX, SendByte)(device.rom[i]);
	}
}

ONESTATICODE void ONEFNCOLLAPSE(ONEPREFIX, PowerOff)() {
	ONE_INPUT;
	ONE_CLEAR;
}

#if ONENEEDREADROM

ONESTATICODE char ONEFNCOLLAPSE(ONEPREFIX, ReadRom)(OneWire_dev_t * dev) {

	if (!dev) return -1;

	uint8_t * ptr = (uint8_t*)dev;

	if (ONEFNCOLLAPSE(ONEPREFIX, Reset)()) return -2;
	ONEFNCOLLAPSE(ONEPREFIX, SendByte)(ONEWIRE_READ_ROM_CMD);

	for (int i = 0; i < sizeof(OneWire_dev_t); i++) {
		*(ptr++) = ONEFNCOLLAPSE(ONEPREFIX, GetByte)();
	}

	return 0;
}
#endif

#if ONENEEDCRC8_TABLE
// Dow-CRC using polynomial X^8 + X^5 + X^4 + X^0
// Tiny 32 bytes CRC table. Copyright (C) 1992-2017 Arjen Lentz
// http://lentz.com.au/blog/calculating-crc-with-a-tiny-32-entry-lookup-table
__ONE_TABLE_IN_FLASH
const unsigned char crc8_table[] = {
	0x00, 0x5E, 0xBC, 0xE2, 0x61, 0x3F, 0xDD, 0x83,
	0xC2, 0x9C, 0x7E, 0x20, 0xA3, 0xFD, 0x1F, 0x41,
	0x00, 0x9D, 0x23, 0xBE, 0x46, 0xDB, 0x65, 0xF8,
	0x8C, 0x11, 0xAF, 0x32, 0xCA, 0x57, 0xE9, 0x74
};

ONESTATICODE unsigned char ONEFNCOLLAPSE(ONEPREFIX, CRC8)(unsigned char crc, unsigned char * bytes, unsigned char size) {

	while (size--) {
		crc = *bytes++ ^ crc;  // just re-using crc as intermediate
		crc = crc8_table[crc & 0x0f] ^ crc8_table[16 + ((crc >> 4) & 0x0f)];
	}

	return crc;
}
#elif ONENEEDCRC8
// Following functions were written by M. P. Hayes for https://github.com/mph-/mmculib/

// Update CRC for a single byte. 
ONESTATICODE unsigned char ONEFNCOLLAPSE(ONEPREFIX, CRC8_byte)(unsigned char crc, unsigned char val) {

	crc = crc ^ val;
	for (int i = 0; i < 8; i++) {
		if (crc & 0x01)
			crc = (crc >> 1) ^ 0x8C;
		else
			crc >>= 1;
	}

	return crc;
}

// Update CRC for a number of bytes.
ONESTATICODE unsigned char ONEFNCOLLAPSE(ONEPREFIX, CRC8)(unsigned char crc, unsigned char * bytes, unsigned char size) {

	for (int i = 0; i < size; i++) {
		crc = ONEFNCOLLAPSE(ONEPREFIX, CRC8_byte)(crc, bytes[i]);
	}

	return crc;
}

#endif

#if ONENEEDCRC16
// Following function is taken from crcutil.c of Public Domain 1-Wire Net Functions
// Copyright (C) 2000 Dallas Semiconductor Corporation, All Rights Reserved.
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.

__ONE_TABLE_IN_FLASH
static const char oddparity[16] = { 0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0 };

ONESTATICODE unsigned short ONEFNCOLLAPSE(ONEPREFIX, CRC16)(unsigned short crc, unsigned char * bytes, unsigned char size) {
	for (int i = 0 ; i < len ; i++) {
		// Even though we're just copying a byte from the input,
		// we'll be doing 16-bit computation with it.
		unsigned short cdata = input[i];
		cdata = (cdata ^ crc) & 0xff;
		crc >>= 8;

		if (oddparity[cdata & 0x0F] ^ oddparity[cdata >> 4]) crc ^= 0xC001;

		cdata <<= 6;
		crc ^= cdata;
		cdata <<= 1;
		crc ^= cdata;
	}

	return crc;
}

#endif

#if ONENEEDSCAN

// As described here https://www.analog.com/en/resources/app-notes/1wire-search-algorithm.html
ONESTATICODE int ONEFNCOLLAPSE(ONEPREFIX, Scan)(OneWire_dev_t * dev_buffer, unsigned char size) {

	int ret = 0;
	unsigned char retries = 10;

	unsigned char bit = 0;
	unsigned char bit_comp = 0;
	unsigned char direction = 0;
	unsigned char discrepancy = 0;

	// unsigned char family_discrepancy = 0;
	unsigned char last_zero = 0;

	unsigned char bit_n = 1;
	unsigned char byte_mask = 1;
	unsigned char byte_n = 0;

	unsigned char device_n = 0;

search_start:

	if ((ret = ONEFNCOLLAPSE(ONEPREFIX, Reset)())) goto search_end;
	ONEFNCOLLAPSE(ONEPREFIX, SendByte)(ONEWIRE_SEARCH_ROM_CMD);

	do {
		
		bit = ONEFNCOLLAPSE(ONEPREFIX, GetBit)();
		bit_comp = ONEFNCOLLAPSE(ONEPREFIX, GetBit)();

		if (bit && bit_comp) break; // 1 1 - No more devices on the bus

		if (bit == bit_comp) { // 0 0 - Discrepancy
			if (bit_n == discrepancy) {
				direction = 1;
			} else {
				if (bit_n > discrepancy) direction = 0;
				else direction = !!(dev_buffer[device_n?device_n-1:0].rom[byte_n] & byte_mask);
			}

			if (!direction) {
				last_zero = bit_n;
				// if (last_zero < 9) family_discrepancy = last_zero;
			}
			
		} else {
			direction = bit;
		}
		
		if (direction) dev_buffer[device_n].rom[byte_n] |= byte_mask;
		else dev_buffer[device_n].rom[byte_n] &= ~byte_mask;

		bit_n++;
		if (!(byte_mask <<= 1)) {
			byte_mask = 1;
			byte_n++;
		}

		ONEFNCOLLAPSE(ONEPREFIX, SendBit)(direction);
	} while(byte_n < 8);

#if (ONENEEDCRC8) || (ONENEEDCRC8_TABLE)
	if (dev_buffer[device_n].crc != ONEFNCOLLAPSE(ONEPREFIX, CRC8)(0, dev_buffer[device_n].rom, 7)) {
		if (--retries) goto search_start;
		else retries = 10;
	}
#endif

	if ((++device_n < size) && last_zero) {
		discrepancy = last_zero;
		last_zero = 0;
		bit_n = 1;
		byte_n = 0;
		direction = 0;
		goto search_start;
	}

search_end:
	if (device_n) return device_n;
	else return 0;
}
#endif
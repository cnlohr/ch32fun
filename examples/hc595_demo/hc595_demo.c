// 74HC595 Datasheet: https://www.ti.com/lit/ds/symlink/sn74hc595.pdf

#include "ch32fun.h"
#include <stdio.h>
#include <stdint.h>

#define CH32V003_SPI_SPEED_HZ 1000000

#include "../../extralibs/ch32v003_SPI.h"

//# 74HC595 Pinout
// Q1  - 1	 |	16 - Vcc
// Q2  - 2	 |	15 - Q0
// Q3  - 3	 |	14 - DATA	(SPI MOSI) - PC6
// Q4  - 4	 |	13 - CE	 	(Set Low)
// Q5  - 5	 |	12 - CLK	(SPI CLK) - PC5
// Q6  - 6	 |	11 - LATCH	- PC3
// Q7  - 7	 |	10 - RST	(Set High)
// GND - 8	 |	9  - Q7'	(Data output - for chaining to another 74HC595)

//! ####################################
//! SPI FUNCTIONS
//! ####################################

void SPI_Configure() {
	// reset control register
	SPI1->CTLR1 = 0;

	// Enable SPI peripheral
	RCC->APB2PCENR |= RCC_APB2Periph_SPI1;

	// PC5 is SCLK
	funPinMode(  PC5, GPIO_Speed_50MHz | GPIO_CNF_OUT_PP_AF );

	// PC6 is MOSI
	funPinMode(  PC6, GPIO_Speed_50MHz | GPIO_CNF_OUT_PP_AF );

	// Configure SPI
	SPI1->CTLR1 |= SPI_CPHA_1Edge | SPI_CPOL_Low
				| SPI_Mode_Master| SPI_BaudRatePrescaler_2
				| SPI_NSS_Soft | SPI_DataSize_8b;
	
	SPI1->CTLR1 |= SPI_Direction_2Lines_FullDuplex;

	SPI1->CTLR1 |= CTLR1_SPE_Set;			// Enable SPI Port
}

int HC595_LATCH_PIN = PC3;

void HC595_RELEASE_LATCH() {
	if (HC595_LATCH_PIN != -1) funDigitalWrite(HC595_LATCH_PIN, 0);
}

void HC595_SET_LATCH() {
	if (HC595_LATCH_PIN != -1) funDigitalWrite(HC595_LATCH_PIN, 1);
}

void hc595_send8(u8 data) {
	HC595_RELEASE_LATCH();		// latch OFF
	SPI_transfer_8(data);
	HC595_SET_LATCH();			// latch ON
}

void hc595_send16(u16 data) {
	HC595_RELEASE_LATCH();		// latch OFF
	u8 msb = (data >> 8) & 0xFF;
	u8 lsb = data & 0xFF;
	SPI_transfer_8(msb);
	SPI_transfer_8(lsb);
	HC595_SET_LATCH();			// latch ON
}


//! ####################################
//! SHIFTOUT FUNCTIONS
//! ####################################

u8 HC595_CLOCK_PIN = PC5;
u8 HC595_DATA_PIN = PC6;

//! REQUIRES: clock and data pins
void hc595_shiftOut8(u8 val, u8 msbFirst) {
	HC595_RELEASE_LATCH();

	for (int i = 0; i < 8; i++) {
		if (msbFirst == 1) {
			funDigitalWrite(HC595_DATA_PIN, (val & 0x80) != 0); // Send MSB first
			val <<= 1;	// Shift left
		} else {
			funDigitalWrite(HC595_DATA_PIN, val & 1); // Send LSB first
			val >>= 1;	// Shift right
		}
		funDigitalWrite(HC595_CLOCK_PIN, 1);	// Clock pulse
		funDigitalWrite(HC595_CLOCK_PIN, 0);	// Clock off
	}

	HC595_SET_LATCH();
}

//! REQUIRES: clock and data pins
void hc595_shiftOut16(u16 val, u8 msbFirst) {
	HC595_RELEASE_LATCH();

	for (int i = 0; i < 16; i++) {
		if (msbFirst == 1) {
			funDigitalWrite(HC595_DATA_PIN, (val & 0x8000) != 0); // Send MSB first
			val <<= 1;	// Shift left
		} else {
			funDigitalWrite(HC595_DATA_PIN, val & 1); // Send LSB first
			val >>= 1;	// Shift right
		}
		funDigitalWrite(HC595_CLOCK_PIN, 1);	// Clock pulse
		funDigitalWrite(HC595_CLOCK_PIN, 0);	// Clock off
	}

	HC595_SET_LATCH();
}


//! ####################################
//! MAIN FUNCTIONS
//! ####################################

int main(void) {
	SystemInit();
	Delay_Ms(100);

	printf("\r\n74HC595 test.\r\n");
	funGpioInitAll();
	funPinMode(HC595_LATCH_PIN, GPIO_CFGLR_OUT_50Mhz_PP);

	#ifdef CH32V003_SPI_SPEED_HZ
		SPI_Configure();
	#else
		funPinMode(HC595_CLOCK_PIN, GPIO_CFGLR_OUT_50Mhz_PP);
		funPinMode(HC595_DATA_PIN, GPIO_CFGLR_OUT_50Mhz_PP);
	#endif
	

	while (1) {
		u8 value = 0x00000001;
		u16 counter = 3;

		while(counter-- > 0) {
			for (int i = 0; i < 8; i++) {
				value = value << 1;		// Shift left by 1

				#ifdef CH32V003_SPI_SPEED_HZ
					hc595_send8(value);
				#else
					hc595_shiftOut8(value, 0);
				#endif
				Delay_Ms(200);
			}

			value = 0x00000001;
		}

		value = 0b00000011;
		counter = 3;

		while(counter-- > 0) {
			for (int i = 0; i < 8; i++) {
				value = value << 1;		// Shift left by 1

				#ifdef CH32V003_SPI_SPEED_HZ
					hc595_send8(value);
				#else
					hc595_shiftOut8(value, 0);
				#endif

				Delay_Ms(200);
			}

			value = 0b00000011;
		}

		printf(".");
	}
}
/*
 * spi_lsm6dsv32x - Example for LSM6DSV32X sensor implementation using SPI interface
 * with CH32V20x microcontroller. Created by Alexander Mandera 
 *
 * Portions of this file are derived from the STM32 LSM6DSV32X library,
 * licensed by ST under BSD 3-Clause license.
 *
 * Copyright (c) 2021 STMicroelectronics.
 * All rights reserved.
 */

#include "ch32fun.h"
#include <stdio.h>
#include <math.h>

#include "lsm6dsv32x_spi.h"

// Functions
void initialize_lsm6dsv32x( void );
void read_data( void );

/*
    Pinout for SPI:
    - PA5: SCK (SPI clock)
    - PA6: MISO (Master In Slave Out)
    - PA7: MOSI (Master Out Slave In)
    - PA4: CS (Chip Select, NSS pin for SPI1)
*/

uint8_t register_setup[] = {
	LSM6DSV32X_CTRL3,			0b00000010, // CTRL3: Block Data Update
	LSM6DSV32X_CTRL1, 			0b00100000, // CTRL1: Accelerometer ODR 7.5Hz
	LSM6DSV32X_CTRL2, 			0b01100000, // CTRL2: Gyroscope ODR 15Hz
	LSM6DSV32X_CTRL8, 			0b00000100, // CTRL8: Accelerometer Full Scale 4g, Strong Bandwidth
	LSM6DSV32X_CTRL6, 			0b01000000, // CTRL6: Gyroscope Full Scale 2000dps, Ultra Light Bandwidth
	LSM6DSV32X_CTRL7, 			0b10000000, // CTRL7: Gyroscope LP1 filter enabled
	LSM6DSV32X_CTRL4, 			0b00010000, // CTRL4: DRDY
	LSM6DSV32X_CTRL9, 			0b00010000, // CTRL9: Accelerometer LP2 filter enabled
	LSM6DSV32X_EMB_FUNC_CFG, 	0b00001100, // EMB_FUNC_CFG: Enable IRQ Mask XL/G
};

int main()
{
	SystemInit();

	// Turn on SPI1 and GPIOA
	RCC->APB2PCENR |= RCC_APB2Periph_GPIOA | RCC_APB2Periph_SPI1;

	// Reset SPI1 control register
	SPI1->CTLR1 = 0;

	// Set SPI clock prescaler
	SPI1->CTLR1 |= SPI_BaudRatePrescaler_128;

	// Configure software NSS pin, master mode on PA4
	SPI1->CTLR1 |= SPI_NSS_Soft;
	funPinMode( SPI_CS_PIN, GPIO_CFGLR_OUT_50Mhz_PP ); // Set PA4 as output, 50MHz, push-pull
	funDigitalWrite( SPI_CS_PIN, FUN_HIGH ); // Set NSS high

	// Set alternate function for SPI1
	AFIO->PCFR1 |= GPIO_Remap_SPI1;

	// SCK on PA5, 10MHz Output, alt func, push-pull
	GPIOA->CFGLR &= ~( 0xf << ( 4 * 5 ) ); // Clear PA5 config
	GPIOA->CFGLR |= ( GPIO_Speed_50MHz | GPIO_CNF_OUT_PP_AF ) << ( 4 * 5 );

	// SPI Master mode
	SPI1->CTLR1 |= SPI_Mode_Master;

	// Enable Full duplex mode
	SPI1->CTLR1 |= SPI_Direction_2Lines_FullDuplex;

	// MOSI on PA7, 10MHz Output, alt func, push-pull
	GPIOA->CFGLR &= ~( 0xf << ( 4 * 7 ) ); // Clear PA7 config
	GPIOA->CFGLR |= ( GPIO_Speed_50MHz | GPIO_CNF_OUT_PP_AF ) << ( 4 * 7 );

	// MISO on PA6, 10MHz input, floating
	GPIOA->CFGLR &= ~( 0xf << ( 4 * 6 ) ); // Clear PA6 config
	GPIOA->CFGLR |= GPIO_CNF_IN_FLOATING << ( 4 * 6 ); // Set PA6 as floating input

	// Start SPI transmission
	SPI1->CTLR1 &= ~( SPI_CTLR1_DFF );
	SPI1->CTLR1 |= SPI_CTLR1_SPE;

	// Start sensor initialization
	printf( "Initializing LSM6DSV32X...\n" );
	initialize_lsm6dsv32x();

	printf( "LSM6DSV32X initialization complete.\n" );

	read_data();

	while ( 1 );
}

static int16_t data_raw_acceleration[3] = { 0, 0, 0 };
static int16_t data_raw_angular_rate[3] = { 0, 0, 0 };
static int16_t data_raw_temperature = 0;
static float acceleration_mg[3];
static float angular_rate_mdps[3];
static float temperature_degC;

/*
    Derived from
    https://github.com/STMicroelectronics/STMems_Standard_C_drivers/blob/master/lsm6dsv32x_STdC/examples/lsm6dsv32x_read_data_polling.c
    (distributed under BSD-3-Clause license by ST)
*/
void read_data( void )
{
	uint8_t status_reg = 0;

	initialize_registers( register_setup, sizeof( register_setup ) );

	printf( "Read samples in polling mode (no interrupt)...\n" );

	/* Read samples in polling mode (no int) */
	while ( 1 )
	{
		/* Read output only if new xl value is available */
		lsm6dsv32x_get_status_reg( &status_reg );

		if ( ( status_reg & 0b10000000 ) == 0b10000000 )	// Acceleration data ready
		{
			/* Read acceleration field data */
			lsm6dsv32_get_acceleration_raw( data_raw_acceleration );

			acceleration_mg[0] = lsm6dsv32x_from_fs4_to_mg( data_raw_acceleration[0] );
			acceleration_mg[1] = lsm6dsv32x_from_fs4_to_mg( data_raw_acceleration[1] );
			acceleration_mg[2] = lsm6dsv32x_from_fs4_to_mg( data_raw_acceleration[2] );

			printf( "Acceleration [mg]:\t%d.%02d\t%d.%02d\t%d.%02d\r\n", (int)acceleration_mg[0],
				(int)fabs( acceleration_mg[0] * 100 ) % 100, (int)acceleration_mg[1],
				(int)fabs( acceleration_mg[1] * 100 ) % 100, (int)acceleration_mg[2],
				(int)fabs( acceleration_mg[2] * 100 ) % 100 );
		}

		if ( ( status_reg & 0b01000000 ) == 0b01000000 ) // Angular rate data ready
		{
			/* Read angular rate field data */
			lsm6dsv32_get_angular_rate_raw( data_raw_angular_rate );

			angular_rate_mdps[0] = lsm6dsv32x_from_fs2000_to_mdps( data_raw_angular_rate[0] );
			angular_rate_mdps[1] = lsm6dsv32x_from_fs2000_to_mdps( data_raw_angular_rate[1] );
			angular_rate_mdps[2] = lsm6dsv32x_from_fs2000_to_mdps( data_raw_angular_rate[2] );

			printf( "Angular rate [mdps]:\t%d.%02d\t%d.%02d\t%d.%02d\r\n", (int)angular_rate_mdps[0],
				(int)fabs( angular_rate_mdps[0] * 100 ) % 100, (int)angular_rate_mdps[1],
				(int)fabs( angular_rate_mdps[1] * 100 ) % 100, (int)angular_rate_mdps[2],
				(int)fabs( angular_rate_mdps[2] * 100 ) % 100 );
		}

		if ( ( status_reg & 0b00100000 ) == 0b00100000 ) // Temperature data ready
		{
			/* Read temperature data */
			
			lsm6dsv32_get_temperature_raw( &data_raw_temperature );

			temperature_degC = lsm6dsv32x_from_lsb_to_celsius( data_raw_temperature );

			printf(
				"Temperature [degC]:\t%d.%02d\r\n", (int)temperature_degC, (int)fabs( temperature_degC * 100 ) % 100 );
		}

		Delay_Ms( 1000 ); // Delay for 1 second before next read
	}
}
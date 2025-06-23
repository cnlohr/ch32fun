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
#include <string.h>

#include "lsm6dsv32x-pid/lsm6dsv32x_reg.h"

#define BOOT_TIME 20 // Boot time for LSM6DSV32X in ms
#define SPI_CS_PIN PA4 // Chip select pin for SPI (PA4)
// #define SPI_DEBUG 1 // Enable debug prints for SPI operations
#define USE_ACC_ROT_TEMP 1  	// Enable accelerometer, gyroscope, and temperature example
//#define USE_GAME_ROT 1			// Enable game rotation vector example

// Pre-defined functions for platform-specific implementations
static int32_t platform_write( void *handle, uint8_t reg, const uint8_t *bufp, uint16_t len );
static int32_t platform_read( void *handle, uint8_t reg, uint8_t *bufp, uint16_t len );
static void platform_delay( uint32_t ms );

// Functions
void initialize_lsm6dsv32x( void );
void read_data( void );

stmdev_ctx_t dev_ctx;

/*
    Pinout for SPI:
    - PA5: SCK (SPI clock)
    - PA6: MISO (Master In Slave Out)
    - PA7: MOSI (Master Out Slave In)
    - PA4: CS (Chip Select, NSS pin for SPI1)
*/

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

void initialize_lsm6dsv32x( void )
{
	uint8_t device_id;
	lsm6dsv32x_reset_t rst;

	// Initialize driver context
	dev_ctx.write_reg = platform_write;
	dev_ctx.read_reg = platform_read;
	dev_ctx.handle = SPI1; // Use SPI1 as handle

	// Wait for boot time
	platform_delay( BOOT_TIME );

	// Check device ID
	if ( lsm6dsv32x_device_id_get( &dev_ctx, &device_id ) != 0 )
	{
		printf( "Failed to read device ID\n" );
		return;
	}

	if ( device_id != LSM6DSV32X_ID )
	{
		printf( "Device ID mismatch: expected 0x%02X, got 0x%02X\n", LSM6DSV32X_ID, device_id );
		while ( 1 );
	}

	printf( "Device ID: 0x%02X\n", device_id );
	printf( "LSM6DSV32X initialized successfully.\n" );

	/* Restore default configuration */
	lsm6dsv32x_reset_set( &dev_ctx, LSM6DSV32X_RESTORE_CTRL_REGS );
	do
	{
		lsm6dsv32x_reset_get( &dev_ctx, &rst );
	} while ( rst != LSM6DSV32X_READY );
}

#ifdef USE_ACC_ROT_TEMP
static int16_t data_raw_acceleration[3];
static int16_t data_raw_angular_rate[3];
static int16_t data_raw_temperature;
static float_t acceleration_mg[3];
static float_t angular_rate_mdps[3];
static float_t temperature_degC;

/*
    Derived from
    https://github.com/STMicroelectronics/STMems_Standard_C_drivers/blob/master/lsm6dsv32x_STdC/examples/lsm6dsv32x_read_data_polling.c
    (distributed under BSD-3-Clause license by ST)
*/
void read_data( void )
{
	lsm6dsv32x_filt_settling_mask_t filt_settling_mask;
	lsm6dsv32x_data_ready_t drdy;

	lsm6dsv32x_block_data_update_set( &dev_ctx, PROPERTY_ENABLE );
	/* Set Output Data Rate.
	 * Selected data rate have to be equal or greater with respect
	 * with MLC data rate.
	 */
	lsm6dsv32x_xl_data_rate_set( &dev_ctx, LSM6DSV32X_ODR_AT_7Hz5 );
	lsm6dsv32x_gy_data_rate_set( &dev_ctx, LSM6DSV32X_ODR_AT_15Hz );
	/* Set full scale */
	lsm6dsv32x_xl_full_scale_set( &dev_ctx, LSM6DSV32X_4g );
	lsm6dsv32x_gy_full_scale_set( &dev_ctx, LSM6DSV32X_2000dps );
	/* Configure filtering chain */
	filt_settling_mask.drdy = PROPERTY_ENABLE;
	filt_settling_mask.irq_xl = PROPERTY_ENABLE;
	filt_settling_mask.irq_g = PROPERTY_ENABLE;
	lsm6dsv32x_filt_settling_mask_set( &dev_ctx, filt_settling_mask );
	lsm6dsv32x_filt_gy_lp1_set( &dev_ctx, PROPERTY_ENABLE );
	lsm6dsv32x_filt_gy_lp1_bandwidth_set( &dev_ctx, LSM6DSV32X_GY_ULTRA_LIGHT );
	lsm6dsv32x_filt_xl_lp2_set( &dev_ctx, PROPERTY_ENABLE );
	lsm6dsv32x_filt_xl_lp2_bandwidth_set( &dev_ctx, LSM6DSV32X_XL_STRONG );

	printf( "Read samples in polling mode (no interrupt)...\n" );

	/* Read samples in polling mode (no int) */
	while ( 1 )
	{
		/* Read output only if new xl value is available */
		lsm6dsv32x_flag_data_ready_get( &dev_ctx, &drdy );

		if ( drdy.drdy_xl )
		{
			/* Read acceleration field data */
			memset( data_raw_acceleration, 0x00, 3 * sizeof( int16_t ) );
			lsm6dsv32x_acceleration_raw_get( &dev_ctx, data_raw_acceleration );

			acceleration_mg[0] = lsm6dsv32x_from_fs4_to_mg( data_raw_acceleration[0] );
			acceleration_mg[1] = lsm6dsv32x_from_fs4_to_mg( data_raw_acceleration[1] );
			acceleration_mg[2] = lsm6dsv32x_from_fs4_to_mg( data_raw_acceleration[2] );

			printf( "Acceleration [mg]:\t%d.%02d\t%d.%02d\t%d.%02d\r\n", (int)acceleration_mg[0],
				(int)fabs( acceleration_mg[0] * 100 ) % 100, (int)acceleration_mg[1],
				(int)fabs( acceleration_mg[1] * 100 ) % 100, (int)acceleration_mg[2],
				(int)fabs( acceleration_mg[2] * 100 ) % 100 );
		}

		if ( drdy.drdy_gy )
		{
			/* Read angular rate field data */
			memset( data_raw_angular_rate, 0x00, 3 * sizeof( int16_t ) );
			lsm6dsv32x_angular_rate_raw_get( &dev_ctx, data_raw_angular_rate );

			angular_rate_mdps[0] = lsm6dsv32x_from_fs2000_to_mdps( data_raw_angular_rate[0] );
			angular_rate_mdps[1] = lsm6dsv32x_from_fs2000_to_mdps( data_raw_angular_rate[1] );
			angular_rate_mdps[2] = lsm6dsv32x_from_fs2000_to_mdps( data_raw_angular_rate[2] );

			printf( "Angular rate [mdps]:\t%d.%02d\t%d.%02d\t%d.%02d\r\n", (int)angular_rate_mdps[0],
				(int)fabs( angular_rate_mdps[0] * 100 ) % 100, (int)angular_rate_mdps[1],
				(int)fabs( angular_rate_mdps[1] * 100 ) % 100, (int)angular_rate_mdps[2],
				(int)fabs( angular_rate_mdps[2] * 100 ) % 100 );
		}

		if ( drdy.drdy_temp )
		{
			/* Read temperature data */
			memset( &data_raw_temperature, 0x00, sizeof( int16_t ) );
			lsm6dsv32x_temperature_raw_get( &dev_ctx, &data_raw_temperature );

			temperature_degC = lsm6dsv32x_from_lsb_to_celsius( data_raw_temperature );

			printf(
				"Temperature [degC]:\t%d.%02d\r\n", (int)temperature_degC, (int)fabs( temperature_degC * 100 ) % 100 );
		}

		platform_delay( 1000 ); // Delay for 1 second before next read
	}
}
#endif

#ifdef USE_GAME_ROT
#define FIFO_WATERMARK    32

// Fast square root function for float values
// Source: https://www.codeproject.com/Articles/69941/Best-Square-Root-Method-Algorithm-Function-Precisi
float sqrtf(float x) {
  union {
    int i;
    float x;
  } u;
  u.x = x;
  u.i = (1<<29) + (u.i >> 1) - (1 << 22);

  u.x = u.x + x/u.x;
  u.x = 0.25f*u.x + x/u.x;

  return u.x;
}

/*
    Derived from
    https://github.com/STMicroelectronics/STMems_Standard_C_drivers/blob/49017e86342adda7087d99169f9c4971608eee0d/lsm6dsv32x_STdC/examples/lsm6dsv32x_sensor_fusion.c
    (distributed under BSD-3-Clause license by ST)
*/

static lsm6dsv32x_fifo_sflp_raw_t fifo_sflp;

static float_t npy_half_to_float(uint16_t h)
{
    union { float_t ret; uint32_t retbits; } conv;
    conv.retbits = lsm6dsv32x_from_f16_to_f32(h);
    return conv.ret;
}

static void sflp2q(float_t quat[4], uint16_t sflp[3])
{
  float_t sumsq = 0;

  quat[0] = npy_half_to_float(sflp[0]);
  quat[1] = npy_half_to_float(sflp[1]);
  quat[2] = npy_half_to_float(sflp[2]);

  for (uint8_t i = 0; i < 3; i++)
    sumsq += quat[i] * quat[i];

  if (sumsq > 1.0f)
    sumsq = 1.0f;

  quat[3] = sqrtf(1.0f - sumsq);
}

void read_data( void )
{
lsm6dsv32x_fifo_status_t fifo_status;

   /* Enable Block Data Update */
  lsm6dsv32x_block_data_update_set(&dev_ctx, PROPERTY_ENABLE);
  /* Set full scale */
  lsm6dsv32x_xl_full_scale_set(&dev_ctx, LSM6DSV32X_8g);
  lsm6dsv32x_gy_full_scale_set(&dev_ctx, LSM6DSV32X_2000dps);
  /*
   * Set FIFO watermark (number of unread sensor data TAG + 6 bytes
   * stored in FIFO) to FIFO_WATERMARK samples
   */
  lsm6dsv32x_fifo_watermark_set(&dev_ctx, FIFO_WATERMARK);

  /* Set FIFO batch of sflp data */
  fifo_sflp.game_rotation = 1;
  lsm6dsv32x_fifo_sflp_batch_set(&dev_ctx, fifo_sflp);

  /* Set FIFO mode to Stream mode (aka Continuous Mode) */
  lsm6dsv32x_fifo_mode_set(&dev_ctx, LSM6DSV32X_STREAM_MODE);

  /* Set Output Data Rate */
  lsm6dsv32x_xl_data_rate_set(&dev_ctx, LSM6DSV32X_ODR_AT_30Hz);
  lsm6dsv32x_gy_data_rate_set(&dev_ctx, LSM6DSV32X_ODR_AT_30Hz);
  lsm6dsv32x_sflp_data_rate_set(&dev_ctx, LSM6DSV32X_SFLP_30Hz);

  lsm6dsv32x_sflp_game_rotation_set(&dev_ctx, PROPERTY_ENABLE);

  while(1) {
    uint16_t num = 0;

    /* Read watermark flag */
    lsm6dsv32x_fifo_status_get(&dev_ctx, &fifo_status);

    num = fifo_status.fifo_level;

    while (num--) {
        lsm6dsv32x_fifo_out_raw_t f_data;
        float_t quat[4];

        /* Read FIFO sensor value */
        lsm6dsv32x_fifo_out_raw_get(&dev_ctx, &f_data);

        if(f_data.tag == LSM6DSV32X_SFLP_GAME_ROTATION_VECTOR_TAG) {
          sflp2q(quat, (uint16_t *)&f_data.data[0]);
          printf("Game Rotation \tX: %d.%03d\tY: %d.%03d\tZ: %d.%03d\tW: %d.%03d\r\n",
                 (int)quat[0], (int)(fabs(quat[0] * 1000)) % 1000,
                 (int)quat[1], (int)(fabs(quat[1] * 1000)) % 1000,
                 (int)quat[2], (int)(fabs(quat[2] * 1000)) % 1000,
                 (int)quat[3], (int)(fabs(quat[3] * 1000)) % 1000 );
        } 
      }
  }
}
#endif


// Platform-specific functions for LSM6DSV32X

static inline uint8_t spi_transfer_8( SPI_TypeDef *spi, uint8_t data )
{
	// Write data to the SPI data register
	spi->DATAR = data;

	// Wait for transmission to complete
	while ( !( spi->STATR & SPI_STATR_TXE ) );

	asm volatile( "nop" );

	// Wait for RX buffer not empty
	while ( !( spi->STATR & SPI_STATR_RXNE ) );

	return spi->DATAR; // Read received data
}

static int32_t platform_write( void *handle, uint8_t reg, const uint8_t *bufp, uint16_t len )
{
	SPI_TypeDef *spi = (SPI_TypeDef *)handle;

#ifdef SPI_DEBUG
	printf( "Writing to register 0x%02X, length %d\n", reg, len );
#endif

	// Set CS pin low to select the device
	funDigitalWrite( SPI_CS_PIN, FUN_LOW );

	// Write the register address
	spi_transfer_8( spi, reg & 0x7F ); // Clear MSB for write operation

#ifdef SPI_DEBUG
	printf( "SPI TX bytes: " );
#endif
	while ( len > 0 )
	{
#ifdef SPI_DEBUG
		printf( "0x%02X ", *bufp ); // Debug print for each byte
#endif

		// Write the data
		spi_transfer_8( spi, *bufp++ ); // Send data byte
		len--;
	}

#ifdef SPI_DEBUG
	printf( "\n" ); // New line after all bytes written
#endif

	// Set CS pin high to deselect the device
	funDigitalWrite( SPI_CS_PIN, FUN_HIGH );

	return 0; // Return 0 for success
}

static int32_t platform_read( void *handle, uint8_t reg, uint8_t *bufp, uint16_t len )
{
	SPI_TypeDef *spi = (SPI_TypeDef *)handle;

#ifdef SPI_DEBUG
	printf( "Reading from register 0x%02X, length %d\n", reg | 0x80, len );
#endif

	// Set CS pin low to select the device
	funDigitalWrite( SPI_CS_PIN, FUN_LOW );

	// Write the register address
	spi_transfer_8( spi, reg | 0x80 ); // Set MSB to indicate read operation

#ifdef SPI_DEBUG
	printf( "SPI RX bytes: " );
#endif
	while ( len > 0 )
	{
		uint8_t data = spi_transfer_8( spi, 0xFF ); // Read received data
		*bufp++ = data;
#ifdef SPI_DEBUG
		printf( "0x%02X ", data ); // Debug print for each byte
#endif
		len--;
	}
#ifdef SPI_DEBUG
	printf( "\n" ); // New line after all bytes read
#endif

	// Set CS pin high to deselect the device
	funDigitalWrite( SPI_CS_PIN, FUN_HIGH );

	return 0; // Return 0 for success
}

static void platform_delay( uint32_t ms )
{
	Delay_Ms( ms );
}
#include "ch32fun.h"
#include <stdio.h>

#ifndef LSM6DSV32X_SPI_H
#define LSM6DSV32X_SPI_H

// Configuration
#define BOOT_TIME 20 // Boot time for LSM6DSV32X in ms
#define SPI_CS_PIN PA4 // Chip select pin for SPI (PA4)
#define SPI_DEBUG 1 // Enable debug prints for SPI operations

#define LSM6DSV32X_ID                           0x70U
#define LSM6DSV32X_FUNC_CFG_ACCESS              0x1U
#define LSM6DSV32X_PIN_CTRL                     0x2U
#define LSM6DSV32X_IF_CFG                       0x3U
#define LSM6DSV32X_ODR_TRIG_CFG                 0x6U
#define LSM6DSV32X_FIFO_CTRL1                   0x7U
#define LSM6DSV32X_FIFO_CTRL2                   0x8U
#define LSM6DSV32X_FIFO_CTRL3                   0x9U
#define LSM6DSV32X_FIFO_CTRL4                   0x0AU
#define LSM6DSV32X_COUNTER_BDR_REG1            0x0BU
#define LSM6DSV32X_COUNTER_BDR_REG2            0x0CU
#define LSM6DSV32X_INT1_CTRL                   0x0DU
#define LSM6DSV32X_INT2_CTRL                   0x0EU
#define LSM6DSV32X_WHO_AM_I                    0x0FU
#define LSM6DSV32X_CTRL1                       0x10U
#define LSM6DSV32X_CTRL2                       0x11U
#define LSM6DSV32X_CTRL3                       0x12U
#define LSM6DSV32X_CTRL4                       0x13U
#define LSM6DSV32X_CTRL5                       0x14U
#define LSM6DSV32X_CTRL6                       0x15U
#define LSM6DSV32X_CTRL7                       0x16U
#define LSM6DSV32X_CTRL8                       0x17U
#define LSM6DSV32X_CTRL9                       0x18U
#define LSM6DSV32X_CTRL10                      0x19U
#define LSM6DSV32X_CTRL_STATUS                 0x1AU
#define LSM6DSV32X_FIFO_STATUS1                0x1BU
#define LSM6DSV32X_FIFO_STATUS2                0x1CU
#define LSM6DSV32X_ALL_INT_SRC                 0x1DU
#define LSM6DSV32X_STATUS_REG                  0x1EU
#define LSM6DSV32X_OUT_TEMP_L                  0x20U
#define LSM6DSV32X_OUT_TEMP_H                  0x21U
#define LSM6DSV32X_OUTX_L_G                    0x22U
#define LSM6DSV32X_OUTX_H_G                    0x23U
#define LSM6DSV32X_OUTY_L_G                    0x24U
#define LSM6DSV32X_OUTY_H_G                    0x25U
#define LSM6DSV32X_OUTZ_L_G                    0x26U
#define LSM6DSV32X_OUTZ_H_G                    0x27U
#define LSM6DSV32X_OUTX_L_A                    0x28U
#define LSM6DSV32X_OUTX_H_A                    0x29U
#define LSM6DSV32X_OUTY_L_A                    0x2AU
#define LSM6DSV32X_OUTY_H_A                    0x2BU
#define LSM6DSV32X_OUTZ_L_A                    0x2CU
#define LSM6DSV32X_OUTZ_H_A                    0x2DU
#define LSM6DSV32X_UI_OUTX_L_G_OIS_EIS          0x2EU
#define LSM6DSV32X_UI_OUTX_H_G_OIS_EIS          0x2FU
#define LSM6DSV32X_UI_OUTY_L_G_OIS_EIS          0x30U
#define LSM6DSV32X_UI_OUTY_H_G_OIS_EIS          0x31U
#define LSM6DSV32X_UI_OUTZ_L_G_OIS_EIS          0x32U
#define LSM6DSV32X_UI_OUTZ_H_G_OIS_EIS          0x33U
#define LSM6DSV32X_UI_OUTX_L_A_OIS_DUALC        0x34U
#define LSM6DSV32X_UI_OUTX_H_A_OIS_DUALC        0x35U
#define LSM6DSV32X_UI_OUTY_L_A_OIS_DUALC        0x36U
#define LSM6DSV32X_UI_OUTY_H_A_OIS_DUALC        0x37U
#define LSM6DSV32X_UI_OUTZ_L_A_OIS_DUALC        0x38U
#define LSM6DSV32X_UI_OUTZ_H_A_OIS_DUALC        0x39U
#define LSM6DSV32X_AH_QVAR_OUT_L                0x3AU
#define LSM6DSV32X_AH_QVAR_OUT_H                0x3BU
#define LSM6DSV32X_TIMESTAMP0                   0x40U
#define LSM6DSV32X_TIMESTAMP1                   0x41U
#define LSM6DSV32X_TIMESTAMP2                   0x42U
#define LSM6DSV32X_TIMESTAMP3                   0x43U
#define LSM6DSV32X_UI_STATUS_REG_OIS            0x44U
#define LSM6DSV32X_WAKE_UP_SRC                  0x45U
#define LSM6DSV32X_TAP_SRC                      0x46U
#define LSM6DSV32X_D6D_SRC                      0x47U
#define LSM6DSV32X_STATUS_MASTER_MAINPAGE       0x48U
#define LSM6DSV32X_EMB_FUNC_STATUS_MAINPAGE     0x49U
#define LSM6DSV32X_FSM_STATUS_MAINPAGE          0x4AU
#define LSM6DSV32X_MLC_STATUS_MAINPAGE          0x4BU
#define LSM6DSV32X_INTERNAL_FREQ                 0x4FU
#define LSM6DSV32X_FUNCTIONS_ENABLE              0x50U
#define LSM6DSV32X_DEN                           0x51U
#define LSM6DSV32X_INACTIVITY_DUR                0x54U
#define LSM6DSV32X_INACTIVITY_THS                0x55U
#define LSM6DSV32X_TAP_CFG0                      0x56U
#define LSM6DSV32X_TAP_CFG1                      0x57U
#define LSM6DSV32X_TAP_CFG2                      0x58U
#define LSM6DSV32X_TAP_THS_6D                    0x59U
#define LSM6DSV32X_TAP_DUR                       0x5AU
#define LSM6DSV32X_WAKE_UP_THS                   0x5BU
#define LSM6DSV32X_WAKE_UP_DUR                   0x5CU
#define LSM6DSV32X_FREE_FALL                     0x5DU
#define LSM6DSV32X_MD1_CFG                       0x5EU
#define LSM6DSV32X_MD2_CFG                       0x5FU
#define LSM6DSV32X_HAODR_CFG                     0x62U
#define LSM6DSV32X_EMB_FUNC_CFG                  0x63U
#define LSM6DSV32X_UI_HANDSHAKE_CTRL             0x64U
#define LSM6DSV32X_UI_SPI2_SHARED_0              0x65U
#define LSM6DSV32X_UI_SPI2_SHARED_1              0x66U
#define LSM6DSV32X_UI_SPI2_SHARED_2              0x67U
#define LSM6DSV32X_UI_SPI2_SHARED_3              0x68U
#define LSM6DSV32X_UI_SPI2_SHARED_4              0x69U
#define LSM6DSV32X_UI_SPI2_SHARED_5              0x6AU
#define LSM6DSV32X_CTRL_EIS                      0x6BU
#define LSM6DSV32X_UI_INT_OIS                    0x6FU
#define LSM6DSV32X_UI_CTRL1_OIS                  0x70U
#define LSM6DSV32X_UI_CTRL2_OIS                  0x71U
#define LSM6DSV32X_UI_CTRL3_OIS                  0x72U
#define LSM6DSV32X_X_OFS_USR                     0x73U
#define LSM6DSV32X_Y_OFS_USR                     0x74U
#define LSM6DSV32X_Z_OFS_USR                     0x75U
#define LSM6DSV32X_FIFO_DATA_OUT_TAG             0x78U
#define LSM6DSV32X_FIFO_DATA_OUT_X_L             0x79U
#define LSM6DSV32X_FIFO_DATA_OUT_X_H             0x7AU
#define LSM6DSV32X_FIFO_DATA_OUT_Y_L             0x7BU
#define LSM6DSV32X_FIFO_DATA_OUT_Y_H             0x7CU
#define LSM6DSV32X_FIFO_DATA_OUT_Z_L             0x7DU
#define LSM6DSV32X_FIFO_DATA_OUT_Z_H             0x7EU

// SPI2 page
#define LSM6DSV32X_SPI2_WHO_AM_I                 0x0FU
#define LSM6DSV32X_SPI2_STATUS_REG_OIS           0x1EU
#define LSM6DSV32X_SPI2_OUT_TEMP_L               0x20U
#define LSM6DSV32X_SPI2_OUT_TEMP_H               0x21U
#define LSM6DSV32X_SPI2_OUTX_L_G_OIS             0x22U
#define LSM6DSV32X_SPI2_OUTX_H_G_OIS             0x23U
#define LSM6DSV32X_SPI2_OUTY_L_G_OIS             0x24U
#define LSM6DSV32X_SPI2_OUTY_H_G_OIS             0x25U
#define LSM6DSV32X_SPI2_OUTZ_L_G_OIS             0x26U
#define LSM6DSV32X_SPI2_OUTZ_H_G_OIS             0x27U
#define LSM6DSV32X_SPI2_OUTX_L_A_OIS             0x28U
#define LSM6DSV32X_SPI2_OUTX_H_A_OIS             0x29U
#define LSM6DSV32X_SPI2_OUTY_L_A_OIS             0x2AU
#define LSM6DSV32X_SPI2_OUTY_H_A_OIS             0x2BU
#define LSM6DSV32X_SPI2_OUTZ_L_A_OIS             0x2CU
#define LSM6DSV32X_SPI2_OUTZ_H_A_OIS             0x2DU
#define LSM6DSV32X_SPI2_HANDSHAKE_CTRL           0x6EU
#define LSM6DSV32X_SPI2_INT_OIS                  0x6FU
#define LSM6DSV32X_SPI2_CTRL1_OIS                0x70U
#define LSM6DSV32X_SPI2_CTRL2_OIS                0x71U
#define LSM6DSV32X_SPI2_CTRL3_OIS                0x72U

// Embedded page
#define LSM6DSV32X_PAGE_SEL                      0x2U
#define LSM6DSV32X_EMB_FUNC_EN_A                 0x4U
#define LSM6DSV32X_EMB_FUNC_EN_B                 0x5U
#define LSM6DSV32X_EMB_FUNC_EXEC_STATUS          0x7U
#define LSM6DSV32X_PAGE_ADDRESS                  0x8U
#define LSM6DSV32X_PAGE_VALUE                    0x9U
#define LSM6DSV32X_EMB_FUNC_INT1                 0x0AU
#define LSM6DSV32X_FSM_INT1                      0x0BU
#define LSM6DSV32X_MLC_INT1                      0x0DU
#define LSM6DSV32X_EMB_FUNC_INT2                 0x0EU
#define LSM6DSV32X_FSM_INT2                      0x0FU
#define LSM6DSV32X_MLC_INT2                      0x11U
#define LSM6DSV32X_EMB_FUNC_STATUS               0x12U
#define LSM6DSV32X_FSM_STATUS                    0x13U
#define LSM6DSV32X_MLC_STATUS                    0x15U
#define LSM6DSV32X_PAGE_RW                       0x17U
#define LSM6DSV32X_EMB_FUNC_FIFO_EN_A            0x44U
#define LSM6DSV32X_EMB_FUNC_FIFO_EN_B            0x45U
#define LSM6DSV32X_FSM_ENABLE                    0x46U
#define LSM6DSV32X_FSM_LONG_COUNTER_L            0x48U
#define LSM6DSV32X_FSM_LONG_COUNTER_H            0x49U
#define LSM6DSV32X_INT_ACK_MASK                  0x4BU
#define LSM6DSV32X_FSM_OUTS1                     0x4CU
#define LSM6DSV32X_FSM_OUTS2                     0x4DU
#define LSM6DSV32X_FSM_OUTS3                     0x4EU
#define LSM6DSV32X_FSM_OUTS4                     0x4FU
#define LSM6DSV32X_FSM_OUTS5                     0x50U
#define LSM6DSV32X_FSM_OUTS6                     0x51U
#define LSM6DSV32X_FSM_OUTS7                     0x52U
#define LSM6DSV32X_FSM_OUTS8                     0x53U
#define LSM6DSV32X_SFLP_ODR                      0x5EU
#define LSM6DSV32X_FSM_ODR                       0x5FU
#define LSM6DSV32X_MLC_ODR                       0x60U
#define LSM6DSV32X_STEP_COUNTER_L                0x62U
#define LSM6DSV32X_STEP_COUNTER_H                0x63U
#define LSM6DSV32X_EMB_FUNC_SRC                  0x64U
#define LSM6DSV32X_EMB_FUNC_INIT_A               0x66U
#define LSM6DSV32X_EMB_FUNC_INIT_B               0x67U
#define LSM6DSV32X_MLC1_SRC                      0x70U
#define LSM6DSV32X_MLC2_SRC                      0x71U
#define LSM6DSV32X_MLC3_SRC                      0x72U
#define LSM6DSV32X_MLC4_SRC                      0x73U

// Embedded advanced pages (partial, main ones)
#define LSM6DSV32X_SFLP_GAME_GBIASX_L            0x6EU
#define LSM6DSV32X_SFLP_GAME_GBIASX_H            0x6FU
#define LSM6DSV32X_SFLP_GAME_GBIASY_L            0x70U
#define LSM6DSV32X_SFLP_GAME_GBIASY_H            0x71U
#define LSM6DSV32X_SFLP_GAME_GBIASZ_L            0x72U
#define LSM6DSV32X_SFLP_GAME_GBIASZ_H            0x73U
#define LSM6DSV32X_FSM_EXT_SENSITIVITY_L         0xBAU
#define LSM6DSV32X_FSM_EXT_SENSITIVITY_H         0xBBU
#define LSM6DSV32X_FSM_EXT_OFFX_L                0xC0U
#define LSM6DSV32X_FSM_EXT_OFFX_H                0xC1U
#define LSM6DSV32X_FSM_EXT_OFFY_L                0xC2U
#define LSM6DSV32X_FSM_EXT_OFFY_H                0xC3U
#define LSM6DSV32X_FSM_EXT_OFFZ_L                0xC4U
#define LSM6DSV32X_FSM_EXT_OFFZ_H                0xC5U
#define LSM6DSV32X_FSM_EXT_MATRIX_XX_L           0xC6U
#define LSM6DSV32X_FSM_EXT_MATRIX_XX_H           0xC7U
#define LSM6DSV32X_FSM_EXT_MATRIX_XY_L           0xC8U
#define LSM6DSV32X_FSM_EXT_MATRIX_XY_H           0xC9U
#define LSM6DSV32X_FSM_EXT_MATRIX_XZ_L           0xCAU
#define LSM6DSV32X_FSM_EXT_MATRIX_XZ_H           0xCBU
#define LSM6DSV32X_FSM_EXT_MATRIX_YY_L           0xCCU
#define LSM6DSV32X_FSM_EXT_MATRIX_YY_H           0xCDU
#define LSM6DSV32X_FSM_EXT_MATRIX_YZ_L           0xCEU
#define LSM6DSV32X_FSM_EXT_MATRIX_YZ_H           0xCFU
#define LSM6DSV32X_FSM_EXT_MATRIX_ZZ_L           0xD0U
#define LSM6DSV32X_FSM_EXT_MATRIX_ZZ_H           0xD1U
#define LSM6DSV32X_EXT_CFG_A                     0xD4U
#define LSM6DSV32X_EXT_CFG_B                     0xD5U

// Embedded advanced page 1
#define LSM6DSV32X_FSM_LC_TIMEOUT_L              0x7AU
#define LSM6DSV32X_FSM_LC_TIMEOUT_H              0x7BU
#define LSM6DSV32X_FSM_PROGRAMS                  0x7CU
#define LSM6DSV32X_FSM_START_ADD_L               0x7EU
#define LSM6DSV32X_FSM_START_ADD_H               0x7FU
#define LSM6DSV32X_PEDO_CMD_REG                  0x83U
#define LSM6DSV32X_PEDO_DEB_STEPS_CONF           0x84U
#define LSM6DSV32X_PEDO_SC_DELTAT_L              0xD0U
#define LSM6DSV32X_PEDO_SC_DELTAT_H              0xD1U
#define LSM6DSV32X_MLC_EXT_SENSITIVITY_L         0xE8U
#define LSM6DSV32X_MLC_EXT_SENSITIVITY_H         0xE9U

// Embedded advanced page 2
#define LSM6DSV32X_EXT_FORMAT                    0x00
#define LSM6DSV32X_EXT_3BYTE_SENSITIVITY_L       0x02U
#define LSM6DSV32X_EXT_3BYTE_SENSITIVITY_H       0x03U
#define LSM6DSV32X_EXT_3BYTE_OFFSET_XL           0x06U
#define LSM6DSV32X_EXT_3BYTE_OFFSET_L            0x07U
#define LSM6DSV32X_EXT_3BYTE_OFFSET_H            0x08U

// Sensor hub page
#define LSM6DSV32X_SENSOR_HUB_1                  0x2U
#define LSM6DSV32X_SENSOR_HUB_2                  0x3U
#define LSM6DSV32X_SENSOR_HUB_3                  0x4U
#define LSM6DSV32X_SENSOR_HUB_4                  0x5U
#define LSM6DSV32X_SENSOR_HUB_5                  0x6U
#define LSM6DSV32X_SENSOR_HUB_6                  0x7U
#define LSM6DSV32X_SENSOR_HUB_7                  0x8U
#define LSM6DSV32X_SENSOR_HUB_8                  0x9U
#define LSM6DSV32X_SENSOR_HUB_9                  0x0AU
#define LSM6DSV32X_SENSOR_HUB_10                 0x0BU
#define LSM6DSV32X_SENSOR_HUB_11                 0x0CU
#define LSM6DSV32X_SENSOR_HUB_12                 0x0DU
#define LSM6DSV32X_SENSOR_HUB_13                 0x0EU
#define LSM6DSV32X_SENSOR_HUB_14                 0x0FU
#define LSM6DSV32X_SENSOR_HUB_15                 0x10U
#define LSM6DSV32X_SENSOR_HUB_16                 0x11U
#define LSM6DSV32X_SENSOR_HUB_17                 0x12U
#define LSM6DSV32X_SENSOR_HUB_18                 0x13U
#define LSM6DSV32X_MASTER_CONFIG                 0x14U
#define LSM6DSV32X_SLV0_ADD                      0x15U
#define LSM6DSV32X_SLV0_SUBADD                   0x16U
#define LSM6DSV32X_SLV0_CONFIG                   0x17U
#define LSM6DSV32X_SLV1_ADD                      0x18U
#define LSM6DSV32X_SLV1_SUBADD                   0x19U
#define LSM6DSV32X_SLV1_CONFIG                   0x1AU
#define LSM6DSV32X_SLV2_ADD                      0x1BU
#define LSM6DSV32X_SLV2_SUBADD                   0x1CU
#define LSM6DSV32X_SLV2_CONFIG                   0x1DU
#define LSM6DSV32X_SLV3_ADD                      0x1EU
#define LSM6DSV32X_SLV3_SUBADD                   0x1FU
#define LSM6DSV32X_SLV3_CONFIG                   0x20U
#define LSM6DSV32X_DATAWRITE_SLV0                0x21U
#define LSM6DSV32X_STATUS_MASTER                 0x22U

// SPI Functions
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

// Sensor access
static inline float lsm6dsv32x_from_fs4_to_mg(int16_t lsb)
{
  return ((float)lsb) * 0.122f;
}

static inline float lsm6dsv32x_from_fs2000_to_mdps(int16_t lsb)
{
  return ((float)lsb) * 70.0f;
}

static inline float lsm6dsv32x_from_lsb_to_celsius(int16_t lsb)
{
  return (((float)lsb / 256.0f) + 25.0f);
}

static inline void initialize_lsm6dsv32x(void) {
    // Device ID check
    uint8_t device_id;
    platform_read( SPI1, LSM6DSV32X_WHO_AM_I, &device_id, 1 );
    if ( device_id != LSM6DSV32X_ID ) {
        printf( "LSM6DSV32X not found! Device ID: 0x%02X\n", device_id );
        return;
    }
    printf( "LSM6DSV32X found! Device ID: 0x%02X\n", device_id );

    // Restore default configuration
    uint8_t ctrl3, func_cfg_access;
    platform_read( SPI1, LSM6DSV32X_CTRL3, &ctrl3, 1 );
    ctrl3 |= 0b00000001; // Set SW Reset and Boot bit to 1
    platform_write( SPI1, LSM6DSV32X_CTRL3, &ctrl3, 1 );

    uint8_t ready = 0;
    do {
        platform_read( SPI1, LSM6DSV32X_CTRL3, &ctrl3, 1 );
        platform_read( SPI1, LSM6DSV32X_FUNC_CFG_ACCESS, &func_cfg_access, 1 );
        ready = ( (ctrl3 | func_cfg_access) & 0b10100001 ) == 0; // Check if SW Reset, Boot and SW POR bit is cleared
    } while ( !ready );
}

static inline void initialize_registers(uint8_t *reg, size_t len ) {
    for ( size_t i = 0; i < len; i += 2 )
    {
        uint8_t reg_addr = reg[i];
        uint8_t reg_value = reg[i + 1];

        // Write the register address and value
        platform_write( SPI1, reg_addr, &reg_value, 1 );
    }
}

static inline void lsm6dsv32_get_three_values_raw(uint8_t reg, int16_t *val)
{
    uint8_t buf[6];
    platform_read( SPI1, reg, buf, 6 );

    // Combine low and high bytes for each axis
    val[0] = (int16_t)((buf[1] << 8) | buf[0]);
    val[1] = (int16_t)((buf[3] << 8) | buf[2]);
    val[2] = (int16_t)((buf[5] << 8) | buf[4]);
}

static void lsm6dsv32_get_acceleration_raw(int16_t *val) {
    lsm6dsv32_get_three_values_raw(LSM6DSV32X_OUTX_L_A, val);
}

static void lsm6dsv32_get_angular_rate_raw(int16_t *val) {
    lsm6dsv32_get_three_values_raw(LSM6DSV32X_OUTX_L_G, val);
}

static void lsm6dsv32_get_temperature_raw(int16_t *val) {
    uint8_t buf[2];
    platform_read( SPI1, LSM6DSV32X_OUT_TEMP_L, buf, 2 );
    *val = (int16_t)((buf[1] << 8) | buf[0]);
}

static inline void lsm6dsv32x_get_status_reg(uint8_t *status_reg)
{
    platform_read( SPI1, LSM6DSV32X_STATUS_REG, status_reg, 1 );
}

#endif
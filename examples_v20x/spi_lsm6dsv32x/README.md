# Example for LSM6DSV32x IMU Sensor using SPI

This example demonstrates how to use the LSM6DSV32x IMU Sensor using SPI for readouts of
acceleration, gyroscope, temperature data, as well as game rotation vectors,
based on the ST driver based on this chip.

This implements the needed platform-specific SPI functions (`platform_write`, `platform_read`, `platform_delay`).

## Hardware Setup

Pinout for SPI:
- PA5: SCK (SPI clock)
- PA6: MISO / SDO (Master In Slave Out)
- PA7: MOSI / SDA (Master Out Slave In)
- PA4: CS (Chip Select)

## Running

Select either the `USE_ACC_ROT_TEMP` or `USE_GAME_ROT` define, depending on what data you want to retrieve.
Build and flash the project. The data can be seen in the minichlink terminal using `make terminal`.

## Licensing

Portions of this file are derived from the STM32 LSM6DSV32X library, licensed by ST under BSD 3-Clause license.
This includes the two data retrieval implementations (`read_data`), which source files are linked in the code.
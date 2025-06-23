# Example for LSM6DSV32x IMU Sensor using SPI (No Lib)

This example demonstrates how to use the LSM6DSV32x IMU Sensor using SPI for readouts of
acceleration, gyroscope, temperature data, as well as game rotation vectors,
without using the ST-provided library.

## Hardware Setup

Pinout for SPI:
- PA5: SCK (SPI clock)
- PA6: MISO / SDO (Master In Slave Out)
- PA7: MOSI / SDA (Master Out Slave In)
- PA4: CS (Chip Select)

## Running

Build and flash the project. The data can be seen in the minichlink terminal using `make terminal`.
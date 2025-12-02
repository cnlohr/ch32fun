/**
 * I2C Slave Communication Example
 * You can use this with the i2c_sensor_test example
 * 
 * 0x3x Slave Command Set:
 * 0x01 - Minick BH1750 Power on command (returns 1 byte)
 * 0x23 - Minmick BH1750 Resolution command (returns 1 byte) 
 * 0x13 - Read 2 bytes from slave
 * 0x14 - Read 4 bytes from slave
 * 0x30 - Read from slave's writable buffer (32 bytes)
 * 0x31 - Write to slave's writable buffer (32 bytes)
 * 
 * Command 0x30: Read from writable buffer
 * Format: { 0x30, start_index }
 * - start_index: Position to start reading from (0-31)
 * Since buffer size is 32 bytes (0-31), reading from index 29:
 * - Returns bytes 29, 30, 31 (3 valid bytes)
 * - Remaining requested bytes return 0xFF (buffer boundary exceeded)
 * 
 * Command 0x31: Write to writable buffer  
 * Format: { 0x31, start_index, data0, data1, ... }
 * - start_index: Position to start writing to (0-31)
 * - dataX: Bytes to write (up to buffer boundary)
 * Since buffer ends at index 31:
 * - Writes 0xAA to index 29, 0xBB to index 30, 0xCC to index 31
 * - 0xDD is not written (buffer full)
 * - Returns success for written bytes only
 */


#include "ch32fun.h"
#include <stdio.h>
#include "../i2c_sensor_test/lib_i2c.h"

#define SYSTEM_CLOCK_HZ 48000000
#define I2C_SELF_ADDR 0x66

#define I2C_SLAVE_READ_BYTE_CMD1 0x01
#define I2C_SLAVE_READ_BYTE_CMD2 0x23
#define I2C_SLAVE_READ_2BYTES_CMD 0x13
#define I2C_SLAVE_READ_4BYTES_CMD 0x14
#define I2C_SLAVE_READ_CMD 0x30
#define I2C_SLAVE_WRITE_CMD 0x31

#define I2C_WRITABLE_SIZE 32
volatile u8 writable_buffer[I2C_WRITABLE_SIZE] = { 0 };
volatile u8 readonly_buffer[] = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA };

int main() {
	SystemInit();
	funGpioInitAll(); // Enable GPIOs
	
	printf("\n~ I2C slave Example ~\n");
	printf("Chip ID: %08lX\n", ESIG->UID0);
	printf("Chip Capacity: %d KB\n", ESIG->CAP);

	funPinMode(PA10, GPIO_CFGLR_OUT_50Mhz_AF_PP);	// I2C1 SCL
	funPinMode(PA11, GPIO_CFGLR_OUT_50Mhz_AF_PP);  // I2C1 SDA

	// Initialize I2C as slave
	i2c_slave_init(I2C1, I2C_SELF_ADDR, SYSTEM_CLOCK_HZ, 100000);
	printf("CTLR1: 0x%04X\n", I2C1->CTLR1);	
	printf("CTLR2: 0x%04X\n", I2C1->CTLR2);	
	printf("OADDR1: 0x%04X\n", I2C1->OADDR1);
	printf("CKCFGR: 0x%04X\n", I2C1->CKCFGR);

	// Update sensor data periodically
	while(1) {
		readonly_buffer[5] = readonly_buffer[5] + 1;  
		Delay_Ms(1000);
	}
}


volatile u8 SLAVE_CMD = 0;
volatile u8 slave_tx_idx = 0;			// to be transmitted
volatile u8 slave_rx_idx = 0;			// to be received
volatile u8 slave_temp_value = 0xFF;	// 0xFF = invalid

// Not in master/slave mode
// if(!(I2C1->STAR2 & I2C_STAR2_MSL)) { return 0; }
u8 i2c_isTransmitter() { return I2C1->STAR2 & I2C_STAR2_TRA; }

void I2C1_EV_IRQHandler(void) __attribute__((interrupt));
void I2C1_EV_IRQHandler(void) {
	// printf("\nSLAVE ISR: STAR1=0x%04X\n", I2C1->STAR1);
	
	//# Master request address, slave address match
	if(I2C1->STAR1 & I2C_STAR1_ADDR) {
		printf("\n***SLAVE ADDR MATCH\n");
		(void)I2C1->STAR1;
		(void)I2C1->STAR2;

		if (!i2c_isTransmitter()) {
			// WRITE mode - reset for new command
			SLAVE_CMD = 0;
			slave_rx_idx = 0;
			slave_tx_idx = 0;  // Also reset tx index for new command
		}
	}
	
	//# Master sends data, slave is ready to receive
	if(I2C1->STAR1 & I2C_STAR1_RXNE) {
		u8 received_byte = I2C1->DATAR;
		printf("Receive [%d]: 0x%02X, cmd: 0x%02X", slave_rx_idx, received_byte, SLAVE_CMD);

		if (SLAVE_CMD == 0) {
			SLAVE_CMD = received_byte;
			printf(" | SLAVE_CMD: 0x%02X", SLAVE_CMD);
		}
		else {
			switch (SLAVE_CMD) {
				case I2C_SLAVE_READ_CMD:
					// set slave_tx_index to prepare for transfer
					printf(" | slave_tx_index: %d", received_byte);
					slave_tx_idx = received_byte;
					break;

				case I2C_SLAVE_WRITE_CMD:
					// save received data
					if (slave_temp_value == 0xFF) {
						printf(" | slave_temp_value: 0x%02X", received_byte);
						// slave_temp_value holds the starting index of the transmitting data
						slave_temp_value = received_byte;
					}
					else {
						u8 target_index = slave_rx_idx + slave_temp_value;

						if (target_index < I2C_WRITABLE_SIZE) {
							printf(" | saving 0x%02X", received_byte);
							writable_buffer[target_index] = received_byte;
							slave_rx_idx++;
						} else {
							// buffer is exceeded, reset SLAVE_CMD
							SLAVE_CMD = 0;
						}
					}

					break;
				default:
					SLAVE_CMD = 0;
					break;
			}
		}

		printf("\n");
	}
	
	//# Master requests data, slave is ready to send
	if(I2C1->STAR1 & I2C_STAR1_TXE && i2c_isTransmitter()) {
		// printf("Prepared to Transmit: cmd 0x%02X, tx_idx: %d\n", SLAVE_CMD, slave_tx_idx);

		switch(SLAVE_CMD) {
			// return 1 byte
			case I2C_SLAVE_READ_BYTE_CMD1: case I2C_SLAVE_READ_BYTE_CMD2:
				I2C1->DATAR = readonly_buffer[1];
				break;

			// return 2 bytes
			case I2C_SLAVE_READ_2BYTES_CMD:
				if (slave_tx_idx < 2) {  // Hardcoded length
					I2C1->DATAR = readonly_buffer[5 + slave_tx_idx++];
				} else {
					I2C1->DATAR = 0xFF;
				}
				break;

			// return 4 bytes
			case I2C_SLAVE_READ_4BYTES_CMD:
				if (slave_tx_idx < 4) {  // Hardcoded length
					I2C1->DATAR = readonly_buffer[7 + slave_tx_idx++];
				} else {
					I2C1->DATAR = 0xFF;
				}
				break;

			// return writable buffer
			case I2C_SLAVE_READ_CMD:
				if (slave_tx_idx < I2C_WRITABLE_SIZE) {
					// printf("\n return writable buffer %d: \n", slave_tx_idx);
					// for (int i = 0; i < I2C_WRITABLE_SIZE; i++) {
					// 	printf("0x%02X ", writable_buffer[i]);
					// }
					// printf("\n");

					u8 data = writable_buffer[slave_tx_idx++];
					I2C1->DATAR = data;
					printf(" | transmitting 0x%02X\n", data);
				} else {
					printf(" | transmitting 0xFF\n");
					I2C1->DATAR = 0xFF;
				}
				break;
			default:
				I2C1->DATAR = 0xFF;
				break;
		}
	}

	//# Master sends stop
	if(I2C1->STAR1 & I2C_STAR1_STOPF) {
		// Clear STOPF flag by reading SR1 and writing CR1
		(void)I2C1->STAR1;
		I2C1->CTLR1 |= 0;
		printf("\n***STOP DETECTED\n");

		// Reset state
		SLAVE_CMD = 0;
		slave_rx_idx = 0;
		slave_tx_idx = 0;
		slave_temp_value = 0xFF;
	}
}

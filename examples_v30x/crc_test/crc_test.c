// Simple example that shows how to use the CRC functions

#include "ch32fun.h"
#include <stdio.h>
#include "lib_crc.h"

int main() {
	SystemInit();
	Delay_Ms(100);

	printf("\n~ CRC Test ~\n");
	CRC_init();
	
	// Single 32-bit value
	u32 single_data1 = 0x12345678;
	u32 crc = CRC_calculate32(single_data1);
	printf("CRC of 0x%08X: 0x%08X\n", single_data1, crc);	// expected 0xDF8A8A2B

	// Single 32-bit value
	u32 single_data2 = 0x12345677;
	crc = CRC_calculate32(single_data2);
	printf("CRC of 0x%08X: 0x%08X\n", single_data2, crc);	// expected 0xE7C53796

	// Array of 32-bit values
	u32 data_array[] = {0x11223344, 0x55667788, 0x99AABBCC, 0xDDEEFF00};
	u32 array_size = sizeof(data_array) / sizeof(data_array[0]);
	crc = CRC_calculateArray32(data_array, array_size);
	printf("CRC of array: 0x%08X\n", crc);

	while(1) {}
}
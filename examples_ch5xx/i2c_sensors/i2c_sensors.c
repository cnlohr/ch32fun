#include "ch32fun.h"
#include <stdio.h>

#include "lib_i2c_ch5xx.h"

#define I2C_SDA PB12
#define I2C_SCL PB13

u16 bh1750_read() {
	u8 address = 0x23;

	//# request reading
	u8 ret = i2c_writeData(address, (u8[]){0x13}, 1);
	if (ret != 0) { printf("\nERROR: I2C request 0x%02X\r\n", ret); return; }

	//# parse reading
	u8 data[2];
	ret = i2c_readData(address, data, 2);
	if (ret != 0) { printf("\nERROR: I2C reading 0x%02X\r\n", ret); return; }

	u16 lux_raw = BUF_MAKE_U16(data);
	return lux_raw * 12 / 10;
}

void sht3x_read(u16 *temp, u16 *hum) {
	u8 addres = 0x44;

	//# parse reading
	u8 data[6];
	u8 ret = i2c_readData(addres, data, 6);
	if (ret != 0) { printf("\nERROR: I2C reading 0x%02X\r\n", ret); return; }

	u16 temp_raw = BUF_MAKE_U16(data);
	u16 hum_raw = (data[3] << 8) | data[4];
	*temp = (175 * temp_raw) >> 16;		// >> 16 is equivalent to / 65536
	*hum = (100 * hum_raw) >> 16;		// >> 16 is equivalent to / 65536
}

int main()
{
	SystemInit();
	funGpioInitAll(); // no-op on ch5xx

	funPinMode(I2C_SCL, GPIO_CFGLR_IN_PUPD);
	funPinMode(I2C_SDA, GPIO_CFGLR_IN_PUPD);

	u8 err = i2c_init(100);
	printf("\nI2C init: %d\r\n", err);

	//# setup BH1750
	u8 address = 0x23;

	// power on
	u8 ret = i2c_writeData(address, (u8[]){0x01}, 1);
	if (ret != 0) { printf("\nERROR: I2C powerON 0x%02X\r\n", ret); return; }

	// set resolution
	ret = i2c_writeData(address, (u8[]){0x23}, 1);
	if (ret != 0) { printf("\nERROR: I2C resolution 0x%02X\r\n", ret); return; }

	//# setup SHT3x
	address = 0x44;

	// soft reset
	ret = i2c_writeData(address, (u8[]){0x30, 0xA2}, 2);
	// this command will alwasy be busy, don't check for error
	Delay_Ms(5);	//! REQUIRED

	// config
	ret = i2c_writeData(address, (u8[]){0x21, 0x30}, 2);
	if (ret != 0) { printf("\nERROR: I2C Config 0x%02X\r\n", ret); return; }
	Delay_Ms(5);	//! REQUIRED

	u16 temp, hum, lux;

	while(1) {
		lux = bh1750_read();
		printf("\nlux: %d\n", lux);

		sht3x_read(&temp, &hum);
		printf("temp: %d, hum: %d\n", temp, hum);

		Delay_Ms(1000);
	}
}

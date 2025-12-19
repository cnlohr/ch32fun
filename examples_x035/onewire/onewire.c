#include "ch32fun.h"
#include <stdio.h>
#include <stdbool.h>

#define PIN_ONE    PA3

#define ONEPREFIX  oneWire
#define DELAY(n)   Delay_Us(n)
#define ONE_INPUT  { funPinMode(PIN_ONE, GPIO_CFGLR_IN_PUPD); funDigitalWrite(PIN_ONE, 1); } 
#define ONE_OUTPUT { funDigitalWrite(PIN_ONE, 0); funPinMode(PIN_ONE, GPIO_CFGLR_OUT_2Mhz_PP); } 
#define ONE_SET    { funDigitalWrite(PIN_ONE, 1); }
#define ONE_CLEAR  { funDigitalWrite(PIN_ONE, 0); }
#define ONE_READ   funDigitalRead(PIN_ONE)
#define ONENEEDSCAN          1
#define ONENEEDREADROM       1
#define ONENEEDCRC8_TABLE    1

#define BUS_SIZE             10 // x8 bytes of RAM will be used

#include "static_onewire.h"

OneWire_dev_t sensor_list[BUS_SIZE];

// Default temp ADC conversion time is 750ms at 12bit precision
// You can lower it if you decide to change precision config (9 bit - 94ms, 10 - 188ms, 11 - 375ms)
#define TEMP_DELAY 1000

bool temp_requested, temp_ready;
uint32_t timestamp = 0;

void printSensorType(OneWire_dev_t dev) {
	switch (dev.code) {
		case 0x10:
			printf("Found a DS18S20\n");  // or an old DS1820
			break;
		case 0x28:
			printf("Found a DS18B20\n");
			break;
		case 0x22:
			printf("Found a DS1822\n");
			break;
		default:
			printf("Device is not from a DS18x20 family.\n");
			break;
	}
}

void requestTemperature(OneWire_dev_t sensor) {
	oneWireReset();
	oneWireSelect(sensor);
	oneWireSendByte(0x44);
}

int readTemperature(OneWire_dev_t sensor, int32_t * output) {
	oneWireReset();
	oneWireSelect(sensor);
	oneWireSendByte(0xbe);

	uint8_t data[9];
	memset(data, 0, 9);

	for (int i = 0; i < 9; i++) {
		data[i] = oneWireGetByte();
	}

	for (int i = 0; i < 9; i++) {
		printf("%02x ", data[i]);
	}
	printf("\n");

#if ONENEEDCRC8
	if (oneWireCRC8(0, data, 8) != data[8]) return -1;
#endif

	int32_t raw = (data[1] << 8) | data[0];

	// Precision truncation part, if you care or want to have a nice float
	{
		if (sensor.code == 0x10) {
			raw = raw << 3; // 9 bit resolution by default
			if (data[7] == 0x10) {
				raw = (raw & 0xfff0) + 12 - data[6];
			}
		} else {
			uint8_t cfg = data[4] & 0x60;
			if (!cfg) raw &= ~7; // 9 bit resolution
			else if (cfg == 0x20) raw &= ~3; // 10 bit
			else if (cfg == 0x40) raw &= ~1; // 11 bit
		}
	}

	*output = (raw * 100) / 16;

	return 0;
}

int main() {
	SystemInit();

	funGpioInitAll();

	printf("---Scanning 1-Wire Bus for Devices---\n");
	int result = oneWireScan(sensor_list, BUS_SIZE);
	if (result) {
		for (int i = 0; i < result; i++) {
			printf("Sensor #%d ", i);
			for (int n = 0; n < 8; n++) {
				printf("%02x", sensor_list[i].rom[n]);
			}
			printf("\n");
			printSensorType(sensor_list[i]);
			printf("\n");
		}
	} else {
		printf("No devices found\n");
	}
	printf("---Done Scanning---\n\n");

	while(1) {
		if (SysTick->CNT > timestamp && temp_requested) temp_ready = true;
		if (!temp_requested || temp_ready) {
			for (int i = 0; i < result; i++) {
				if (!temp_requested) {
					requestTemperature(sensor_list[i]);
				}

				if (temp_ready) {
					int temp = 0;
					if (!readTemperature(sensor_list[i], &temp)) {
						printf("Sensor #%d, Temp: %d.%02dC\n\n", i, temp/100, temp%100);
					} else {
						printf("Couldn't read temperature.\n");
					}
				}
			}

			if (!temp_requested) {
				temp_requested = true;
				timestamp = SysTick->CNT + Ticks_from_Ms(TEMP_DELAY);
			}

			if (temp_ready) {
				temp_ready = false;
				temp_requested = false;
			}
		}
	}
}
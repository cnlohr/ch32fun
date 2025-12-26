#include "ch32fun.h"
#include "iSLER.h"
#include <stdio.h>

//------- CONFIG SECTION -------//
#ifdef CH570_CH572
#define MAC_ADDR (0x0003f018)
#define LED      PA9
#else
#define MAC_ADDR (0x0007f018)
#define LED      PA8
#endif
#define LED_ON   0
#define LED_OFF  1

// Pins for I2C
#define PIN_SCL  PA3
#define PIN_SDA  PA2

// Pin for 1-Wire
#define PIN_ONE  PA2

// Sensor type. Pick one
// #define SENSOR_SHT20
// #define SENSOR_SHT30
// #define SENSOR_BME280
#define SENSOR_HDC1080
// #define SENSOR_DS18B20

// BLE advertisement packet format. Pick one
// #define ADV_ATC
// #define ADV_BTHOME
#define ADV_CUSTOM


// Set to 1 to use MCU's MAC as BLE MAC
#define USE_MCU_MAC              1
// Set to 1 to use sensor's serial number as BLE MAC
#define USE_SENSOR_SERIAL_AS_MAC 0
//------- END OF CONFIG -------//

#define ERROR_BAD_CRC        0xffff

#if defined(SENSOR_SHT20) || defined(SENSOR_SHT30) || defined(SENSOR_BME280) || defined(SENSOR_HDC1080)
#define SENSOR_I2C
#elif defined(SENSOR_DS18B20)
#define SENSOR_ONEWIRE
#else
#error "No sensor defined"
#endif

#if defined(SENSOR_I2C)
#define DELAY1               Delay_Us(1);
#define DELAY2               Delay_Us(2);
#define DSCL_IHIGH           { funPinMode(PIN_SCL, GPIO_CFGLR_IN_PUPD); funDigitalWrite(PIN_SCL, 1); }
#define DSDA_IHIGH           { funPinMode(PIN_SDA, GPIO_CFGLR_IN_PUPD); funDigitalWrite(PIN_SDA, 1); }
#define DSDA_INPUT           { funPinMode(PIN_SDA, GPIO_CFGLR_IN_PUPD); funDigitalWrite(PIN_SDA, 1); }
#define DSCL_OUTPUT          { funDigitalWrite(PIN_SCL, 0); funPinMode(PIN_SCL, GPIO_CFGLR_OUT_2Mhz_PP); }
#define DSDA_OUTPUT          { funDigitalWrite(PIN_SDA, 0); funPinMode(PIN_SDA, GPIO_CFGLR_OUT_2Mhz_PP); }
#define READ_DSDA            funDigitalRead(PIN_SDA)
#define I2CNEEDGETBYTE       1
#define I2CNEEDSCAN          1

#include "static_i2c.h"
#endif

#if defined(SENSOR_ONEWIRE)
#define ONEPREFIX            oneWire
#define DELAY(n)             Delay_Us(n)
#define ONE_INPUT            { funPinMode(PIN_ONE, GPIO_CFGLR_IN_PUPD); funDigitalWrite(PIN_ONE, 1); }
#define ONE_OUTPUT           { funDigitalWrite(PIN_ONE, 0); funPinMode(PIN_ONE, GPIO_CFGLR_OUT_2Mhz_PP); }
#define ONE_SET              { funDigitalWrite(PIN_ONE, 1); }
#define ONE_CLEAR            { funDigitalWrite(PIN_ONE, 0); }
#define ONE_READ             funDigitalRead(PIN_ONE)
#define ONENEEDREADROM       1
#define ONENEEDCRC8_TABLE    1

#include "static_onewire.h"
#endif

#if defined(SENSOR_SHT20)

#define SHT20_I2C_ADDR                        0x40
#define SHT20_TEMP_MEASURE_HOLD               0xE3
#define SHT20_HUM_MEASURE_HOLD                0xE5
#define SHT20_TEMP_MEASURE_NOHOLD             0xF3
#define SHT20_HUM_MEASURE_NOHOLD              0xF5
#define SHT20_WRITE_USER_REG                  0xE6
#define SHT20_READ_USER_REG                   0xE7
#define SHT20_SOFT_RESET                      0xFE
#define SHT20_RESOLUTION_14BIT                0x00
#define SHT20_RESOLUTION_13BIT                0x80
#define SHT20_RESOLUTION_12BIT                0x01
#define SHT20_RESOLUTION_11BIT                0x81
#define SHT20_HEATER_ENABLED                  0x04

#define initSensor()

uint8_t checkCRC(uint16_t message_from_sensor, uint8_t check_value_from_sensor) {
	uint32_t remainder = (uint32_t)message_from_sensor << 8;
	remainder |= check_value_from_sensor;
	uint32_t divsor = 0x988000;
	for (int i = 0 ; i < 16 ; i++) {
		if (remainder & (uint32_t)1 << (23 - i)) remainder ^= divsor;
		divsor >>= 1;
	}
	return (uint8_t)remainder;
}

uint16_t readValue(uint8_t cmd) {
	uint8_t msb, lsb, checksum;

	SendStart();
	SendByte(SHT20_I2C_ADDR << 1 | 0);
	SendByte(cmd);
	SendStop();
	Delay_Ms(100);
	
	SendStart();
	SendByte(SHT20_I2C_ADDR << 1 | 1);
	msb = GetByte(0);
	lsb = GetByte(0);
	checksum = GetByte(1);

	uint16_t rawValue = ((uint16_t) msb << 8) | (uint16_t) lsb;
	
	if (checkCRC(rawValue, checksum)) return (ERROR_BAD_CRC);

	return rawValue &= ~0x0003; // clear status bits
}

uint32_t readHumidity() {
  uint32_t hum = readValue(SHT20_HUM_MEASURE_NOHOLD);
  if (hum == ERROR_BAD_CRC) return(ERROR_BAD_CRC);

	hum = ((12500 * hum) >> 16) - 600;
  return hum;
}

int readTemperature() {
  int temp = readValue(SHT20_TEMP_MEASURE_NOHOLD);
  if (temp == ERROR_BAD_CRC) return(ERROR_BAD_CRC);

  temp = ((17572 * temp) >> 16) - 4685;
  return temp;
}

void readSerial(uint8_t * buf, uint8_t size) {
	if (size > 8) size = 8;

	uint8_t l_buf[8];

	SendStart();
	SendByte(SHT20_I2C_ADDR << 1 | 0);
	SendByte(0xFA);
	SendByte(0x0F);
	SendStop();
	Delay_Ms(1);

	SendStart();
	SendByte(SHT20_I2C_ADDR << 1 | 1);
	for (int i = 2; i < 5; i++) {
		buf[size - 1 - i] = GetByte(0);
	}
	buf[3] = GetByte(1);
	SendStop();

	SendStart();
	SendByte(SHT20_I2C_ADDR << 1 | 0);
	SendByte(0xFC);
	SendByte(0xC9);
	SendStop();
	Delay_Ms(1);

	SendStart();
	SendByte(SHT20_I2C_ADDR << 1 | 1);
	buf[6] = GetByte(0);
	buf[7] = GetByte(0);
	buf[0] = GetByte(0);
	buf[1] = GetByte(1);
	SendStop();

	for (int i = 0; i < size; i++) {
		buf[i] = l_buf[i];
	}
}

#elif defined(SENSOR_SHT30)

#define SHT3x_I2C_ADDR          0x44
#define SHT3x_REQUEST_CMD       ((uint16_t)0x2416)
#define SHT3x_READ_CMD          ((uint16_t)0xE000)

uint8_t updated = 0;
uint16_t sht3x_temp_raw = 0;
uint16_t sht3x_hum_raw = 0;

#define initSensor()

uint8_t checkCRC(const uint16_t message_from_sensor) {
	uint8_t crc = 0xFF;

	// process MSB first
	crc ^= (message_from_sensor >> 8) & 0xFF;
	for (int i = 0; i < 8; i++)
		crc = (crc & 0x80) ? (crc << 1) ^ 0x31 : (crc << 1);

	// process LSB
	crc ^= message_from_sensor & 0xFF;
	for (int i = 0; i < 8; i++)
		crc = (crc & 0x80) ? (crc << 1) ^ 0x31 : (crc << 1);

	return crc;
}

int sht3x_update() {
	uint8_t msb, lsb, crc;

	SendStart();
	SendByte(SHT3x_I2C_ADDR << 1 | 0);
	SendByte(SHT3x_REQUEST_CMD >> 8);
	SendByte(SHT3x_REQUEST_CMD & 0xff);
	SendStop();
	Delay_Ms(15);

	SendStart();
	SendByte(SHT3x_I2C_ADDR << 1 | 1);
	msb = GetByte(0);
	lsb = GetByte(0);
	crc = GetByte(0); 
	sht3x_temp_raw = (msb<<8) | lsb;

	if (crc != checkCRC(sht3x_temp_raw)) {
		SendStop();
		return (ERROR_BAD_CRC);
	}

	msb = GetByte(0);
	lsb = GetByte(0);
	crc = GetByte(1);
	sht3x_hum_raw = (msb<<8) | lsb;

	if (crc != checkCRC(sht3x_hum_raw)) {
		SendStop();
		return (ERROR_BAD_CRC);
	}

	SendStop();
	updated = 3;
	return 0;
}

int readTemperature() {
	if (!(updated & 1)) {
		if (sht3x_update() == ERROR_BAD_CRC) return ERROR_BAD_CRC;
	}
	updated &= ~1;
	return ((17500 * (int32_t)sht3x_temp_raw) >> 16) - 4500;
}

uint32_t readHumidity() {
	if (!(updated & 2)) {
		if (sht3x_update() == ERROR_BAD_CRC) return ERROR_BAD_CRC;
	}
	updated &= ~2;
	return ((10000 * (uint32_t)sht3x_hum_raw) >> 16);
}

void readSerial(uint8_t * buf, uint8_t size) {
	if (size > 4) size = 4;

	SendStart();
	SendByte(SHT3x_I2C_ADDR << 1 | 0);
	SendByte(0x36);
	SendByte(0x82);
	SendStop();
	Delay_Ms(1);

	SendStart();
	SendByte(SHT3x_I2C_ADDR << 1 | 1);
	for (int i = 0; i < size - 1; i++) {
		buf[size - 1 - i] = GetByte(0);
	}
	buf[0] = GetByte(1);
	SendStop();
}

#elif defined(SENSOR_BME280)

#define BME280_I2C_ADDR         0x76
#define BME280_ID_ADDR          0xD0
#define BME280_CFG_ADDR         0xF5
#define BME280_TEMP_ADDR        0xFA
#define BME280_HUM_ADDR         0xFD
#define BME280_PRES_ADDR        0xF7
#define BME280_STA_ADDR         0xF3
#define BME280_CTRL_ADDR        0xF4
#define BME280_CTRL_H_ADDR      0xF2

typedef union {
	uint8_t data[35];
	struct {
		uint16_t t1;
		int16_t t2;
		int16_t t3;
		uint16_t p1;
		int16_t p2;
		int16_t p3;
		int16_t p4;
		int16_t p5;
		int16_t p6;
		int16_t p7;
		int16_t p8;
		int16_t p9;
		uint8_t reserved;
		uint8_t h1;
		int16_t h2;
		uint8_t reserved2;
		uint8_t h3;
		int16_t h4;
		uint16_t h5;
		char h6;
	};
} bme280_calib_t;

bme280_calib_t sensor_calib;

typedef struct {
	uint8_t id;
	int32_t temp_raw;
	int32_t hum_raw;
	uint8_t temp_os;
	uint8_t hum_os;
	uint8_t mode;
	uint8_t updated;
	int32_t temp_comp;
} bme280_dev_t;

bme280_dev_t sensor;

void bme280_config(uint8_t temp_os, uint8_t hum_os, uint8_t mode) {

	sensor.temp_os = temp_os;
	sensor.hum_os = hum_os;
	sensor.mode = mode;

	SendStart();
	SendByte(BME280_I2C_ADDR << 1 | 0);
	SendByte(BME280_ID_ADDR);

	SendStart();
	SendByte(BME280_I2C_ADDR << 1 | 1);
	sensor.id = GetByte(1);
	SendStop();

	printf("BMx280 ID = %02x\n", sensor.id);

	if (sensor.id != 0x60 && sensor.id != 0x58) return;

	SendStart();
	SendByte(BME280_I2C_ADDR << 1 | 0);
	SendByte(BME280_CTRL_ADDR);
	SendByte((temp_os<<5)|mode);
	if (sensor.id == 0x60) {
		SendByte(BME280_CTRL_H_ADDR);
		SendByte(hum_os);
	}
	SendStop();

	Delay_Ms(15);

	SendStart();
	SendByte(BME280_I2C_ADDR << 1 | 0);
	SendByte(0x88);

	memset(sensor_calib.data, 0, sizeof(sensor_calib.data));
	SendStart();
	SendByte(BME280_I2C_ADDR << 1 | 1);

	for (int i = 0; i < 25; i++) {
		sensor_calib.data[i] = GetByte(0);
	}
	sensor_calib.data[25] = GetByte(1);

	SendStart();
	SendByte(BME280_I2C_ADDR << 1 | 0);
	SendByte(0xE1);

	SendStart();
	SendByte(BME280_I2C_ADDR << 1 | 1);

	// This part is very sus
	sensor_calib.data[26] = GetByte(0);
	sensor_calib.data[27] = GetByte(0);
	sensor_calib.data[28] = GetByte(0);
	sensor_calib.h4 = GetByte(0) << 4;
	uint8_t frac = GetByte(0);
	sensor_calib.h4 |= frac & 0xf;
	sensor_calib.h5 = frac >> 4;
	sensor_calib.h5 |= GetByte(0) << 4;
	sensor_calib.h6 = GetByte(1);
	SendStop();
}

void initSensor() {
	bme280_config(3, 2, 1);
}

void bme280_update() {
	SendStart();
	SendByte(BME280_I2C_ADDR << 1 | 0);
	SendByte(BME280_CTRL_ADDR);
	SendByte((sensor.temp_os << 5) | sensor.mode);
	SendStop();
	Delay_Ms(150);

	uint8_t buf[5];

	SendStart();
	SendByte(BME280_I2C_ADDR << 1 | 0);
	SendByte(0xFA);

	SendStart();
	SendByte(BME280_I2C_ADDR << 1 | 1);
	for (int i = 0; i < 4; i++) {
		buf[i] = GetByte(0);
	}
	buf[4] = GetByte(1);
	SendStop();

	sensor.temp_raw = ((int32_t)buf[0] << 12) | ((int32_t)buf[1] << 4) | (buf[2] >> 4);
	sensor.hum_raw = ((uint16_t)buf[3] << 8) | buf[4];

	sensor.updated = 3;
}

int readTemperature() {
	if (!(sensor.updated & 1)) {
		bme280_update();
	}
	sensor.updated &= ~1;

	int32_t chunk1, chunk2;

	chunk1 = ((((sensor.temp_raw >> 3) - ((int32_t)sensor_calib.t1 << 1)))
					* ((int32_t)sensor_calib.t2)) >> 11;

	chunk2 = (((((sensor.temp_raw >> 4) - ((int32_t)sensor_calib.t1))
	        * ((sensor.temp_raw >> 4) - ((int32_t)sensor_calib.t1))) >> 12)
	        * ((int32_t)sensor_calib.t3)) >> 14;

	sensor.temp_comp = chunk1 + chunk2;

	return (sensor.temp_comp * 5 + 128) >> 8;
}

uint32_t readHumidity() {
	if (sensor.id != 0x60) return 0;
	if (!(sensor.updated & 2)) {
		bme280_update();
	}
	sensor.updated &= ~2;

	int32_t hum = sensor.temp_comp - 76800;

	hum = (((((sensor.hum_raw << 14) - (((int32_t)sensor_calib.h4) << 20) - (((int32_t)sensor_calib.h5) * hum)) + ((int32_t)16384)) >> 15)
	      * (((((((hum * ((int32_t)sensor_calib.h6)) >> 10) * (((hum * ((int32_t)sensor_calib.h3)) >> 11) + ((int32_t)32768))) >> 10)
	      + ((int32_t)2097152)) * ((int32_t)sensor_calib.h2) + 8192) >> 14));

	hum -= (((((hum >> 15) * (hum >> 15)) >> 7) * (int32_t)sensor_calib.h1) >> 4);

	if (hum < 0) hum = 0;
	if (hum > 419430400) hum = 419430400;

	return (uint32_t)(hum >> 12)/10; // %RH × 1024
}

#define readSerial(x,y)

#elif defined(SENSOR_HDC1080)

#define HDC1080_I2C_ADDR         0x40
#define HDC1080_TEMP_ADDR        0x00
#define HDC1080_HUM_ADDR         0x01
#define HDC1080_CFG_ADDR         0x02
#define HDC1080_SERIAL1_ADDR     0xFB
#define HDC1080_SERIAL2_ADDR     0xFC
#define HDC1080_SERIAL3_ADDR     0xFD

uint8_t hdc1080_current_config = 0;

#define HDC1080_RESOLUTION_14BIT 0
#define HDC1080_RESOLUTION_11BIT 1
#define HDC1080_RESOLUTION_8BIT  2

void hdc1080_config(uint8_t temp_resolution, uint8_t hum_resolution, uint8_t measure_both) {
	SendStart();
	SendByte(HDC1080_I2C_ADDR << 1 | 0);
	SendByte(HDC1080_CFG_ADDR);
	SendByte((measure_both << 12) | (temp_resolution << 10) | hum_resolution);
	SendStop();
}

void initSensor() {
	hdc1080_config(0,0,0);
	hdc1080_current_config = 1;
}

uint16_t readValue(uint8_t cmd) {

	SendStart();
	SendByte(HDC1080_I2C_ADDR << 1 | 0);
	SendByte(cmd);
	SendStop();
	Delay_Ms(15);

	uint8_t msb, lsb;

	SendStart();
	SendByte(HDC1080_I2C_ADDR << 1 | 1);
	msb = GetByte(0);
	lsb = GetByte(1);
	SendStop();

	uint16_t rawValue = ((uint16_t) msb << 8) | (uint16_t) lsb;

	return rawValue;
}

int readTemperature() {
	int temp = readValue(HDC1080_TEMP_ADDR);
	temp = ((16500 * (int32_t)temp) >> 16) - 4000;
	return temp;
}

uint32_t readHumidity() {
	uint32_t hum = readValue(HDC1080_HUM_ADDR);
	hum = ((10000 * (int32_t)hum) >> 16);
	return hum;
}

void readSerial(uint8_t * buf, uint8_t size) {
	if (size > 6) size = 6;

	uint8_t l_buf[6];

	for (int i = 0; i < 3; i++) {
		SendStart();
		SendByte(HDC1080_I2C_ADDR << 1 | 0);
		SendByte((uint8_t)(HDC1080_SERIAL1_ADDR) + i);
		SendStop();
		Delay_Ms(1);
		
		SendStart();
		SendByte(HDC1080_I2C_ADDR << 1 | 1);
		l_buf[2*i] = GetByte(0);
		l_buf[2*i+1] = GetByte(1);
		SendStop();
	}

	for (int i = 0; i < size; i++) {
		buf[i] = l_buf[i];
	}
}

#elif defined(SENSOR_DS18B20)

OneWire_dev_t one_sensor;

#define initSensor()
#define readHumidity() 0

void requestTemperature(OneWire_dev_t sensor) {
	oneWireReset();
	oneWireSelect(sensor);
	oneWireSendByte(0x44);
}

int getTemperature(OneWire_dev_t sensor, int32_t * output) {
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
	if (oneWireCRC8(0, data, 8) != data[8]) return ERROR_BAD_CRC;
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

int readTemperature(void) {
	int ret = 0;
	requestTemperature(one_sensor);
	Delay_Ms(800);
	getTemperature(one_sensor, (int32_t*)&ret);
	return ret;
}

void readSerial(uint8_t * buf, uint8_t size) {
	if (size > 6) size = 6;
	for (int i = 0; i < size; i++) {
		buf[i] = one_sensor.serial[5-i];
	}
}

#endif

#define PHY_MODE       PHY_1M
#define ACCESS_ADDRESS 0x8E89BED6

// BLE advertisements are sent on channels 37, 38 and 39
uint8_t adv_channels[] = {37,38,39};

void blink(int n) {
	for(int i = n-1; i >= 0; i--) {
		funDigitalWrite(LED, LED_ON); // Turn on LED
		Delay_Ms(33);
		funDigitalWrite(LED, LED_OFF); // Turn off LED
		if(i) Delay_Ms(33);
	}
}

// BLE Advertisement Packet
#ifdef ADV_ATC // ATC1441 format (LYWSD03MMC custom firmware)
__attribute__((aligned(4))) uint8_t adv[] = {
	0x02, 0x00, // header for LL: PDU + frame length
	0xFF, 0xEE, 0xDD, 0xCC, 0xBB, 0xAA, // MAC reversed: AA:BB:CC:DD:EE:FF
	0x10, 0x16,       // AD Type: Service Data (UUID 16-bit)

	0x1A, 0x18, // UUID = 0x181A (Environmental Sensing)

	0xA4, 0xC1, 0x38, 0xDD, 0xEE, 0xFF, // MAC again - first 3 bytes need to be A4C138 for OpenMQTT to pick it up

	0x00, 0x87, // Temperature (0.1 °C) = 0x0087 = 135 → 13.5°C
	0x4C,       // Humidity = 0x4C = 76%
	0x45,       // Battery % = 0x3B = 69%
	0x0A, 0xBE, // Battery voltage (mV) = 0x0ABE = 2750 mV
	0x00,       // Frame counter

	0x0B, 0x09, // AD Type: Complete Local Name
	'A', 'T', 'C', '_', 'F', 'U', 'N', 'F', 'U', 'N'
};
#elif defined(ADV_BTHOME) // As described here https://bthome.io/format/
__attribute__((aligned(4))) uint8_t adv[] = {
	0x02, 0x00, // header for LL: PDU + frame length
	0xFF, 0xEE, 0xDD, 0xCC, 0xBB, 0xAA, // MAC reversed: AA:BB:CC:DD:EE:FF
	0x02, 0x01, 0x06, // Flags: LE General Discoverable Mode, BR/EDR Not Supported
	0x0A, 0x16, // Service data
	0xD2, 0xFC, // BTHome registered UUID
	0x40, // BTHome device info byte
	0x02, 0x00, 0x00, // Temperature
	0x03, 0x00, 0x00, // Humidity
	0x08, 0x09, // Complete local name
	'f', 'u', 'n', 't', 'e', 'm', 'p'
};
#else
__attribute__((aligned(4))) uint8_t adv[] = {
	0x02, 0x00, // header for LL: PDU + frame length
	0xFF, 0xEE, 0xDD, 0xCC, 0xBB, 0xAA, // MAC reversed: AA:BB:CC:DD:EE:FF
	// ---- Advertising Flags Field ----
	0x02, 0x01, 0x06, // Flags: LE General Discoverable Mode, BR/EDR Not Supported

	// ---- Service Data Field (Health Thermometer) ----
	0x07, // Length of field
	0x16, // AD Type: Service Data - 16-bit UUID
	0x09, // Service UUID LSB (0x1809)
	0x18, // Service UUID MSB
	
	// Temperature Data: 69.42 (Mantissa: 6942, Exponent: -2)
	0x34, // Mantissa LSB
	0x08, // Mantissa MID
	0x00, // Mantissa MSB
	0xFE,  // Exponent (-2)

	// ---- Complete Local Name Field ----
	0x08, // Length of field (1 byte for type + 8 bytes for name)
	0x09, // AD Type: Complete Local Name
	'f', 'u', 'n', 't', 'e', 'm', 'p'
};
#endif

int main() {
	SystemInit();

	funGpioInitAll();
	
	funPinMode(LED, GPIO_CFGLR_OUT_2Mhz_PP);

	adv[1] = sizeof(adv)-2; // Set correct frame length

	RFCoreInit(LL_TX_POWER_0_DBM);

	blink(5);

#if defined(SENSOR_I2C)
	printf("---Scanning I2C Bus for Devices---\n");
	Scan();
	printf("---Done Scanning---\n\n");
#elif defined(SENSOR_ONEWIRE)
	if (oneWireReadRom(&one_sensor)) printf("No 1-wire sensor found\n");
#endif

#if (USE_MCU_MAC)
	for (int i = 0; i < 6; i++) {
		adv[7-i] = *(uint8_t*)(MAC_ADDR+i);
	}
#elif (USE_SENSOR_SERIAL_AS_MAC)
	readSerial(adv+2, 6);
#endif

#if defined(ADV_ATC)
	for (int i = 0; i < 3; i++) {
		adv[15+i] = adv[4-i];  // Set 3 last bytes of MAC in service data
	}
#endif

	// This MAC address will be used for BLE advertisement
	printf("MAC: %02X:%02X:%02X:%02X:%02X:%02X\n", adv[7], adv[6], adv[5], adv[4], adv[3], adv[2]);

	initSensor();

	while(1) {
		int temperature = readTemperature();
		uint32_t hum = readHumidity();

		if (temperature == ERROR_BAD_CRC || hum == ERROR_BAD_CRC) {
			printf("CRC error\n");
			Delay_Ms(1000);
			continue; // Retry
		}

		// Update advertisement with actual values
#if defined(ADV_ATC)
		adv[18] = ((temperature/10)&0xff00) >> 8;
		adv[19] = ((temperature/10)&0xff);
		adv[20] = ((hum/100)&0xff);
		adv[24]++; // Advance frame counter
#elif defined(ADV_BTHOME)
		adv[18] = ((temperature)&0xff00) >> 8;
		adv[17] = ((temperature)&0xff);
		adv[21] = ((hum)&0xff00) >> 8;
		adv[20] = ((hum)&0xff);
#else
		adv[17] = ((temperature)&0xff0000) >> 16;
		adv[16] = ((temperature)&0xff00) >> 8;
		adv[15] = ((temperature)&0xff);
#endif

		printf("Temp: %d.%02d Hum:%ld.%02ld\n", temperature/100, temperature%100, hum/100, hum%100);
		for(int c = 0; c < sizeof(adv_channels); c++) {
			Frame_TX(ACCESS_ADDRESS, adv, sizeof(adv), adv_channels[c], PHY_MODE);
		}

		blink(1);
		Delay_Ms(1000);
	}
}

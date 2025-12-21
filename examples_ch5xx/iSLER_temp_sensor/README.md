# iSLER temperature sensor

Industry Standard Low Energy Radio (iSLER) temperature sensor for ch32fun. This example reads temperature and humidity from a sensor and advertises it over BLE. This can be used to create a temperature sensor that will appear in you Home Assistant, or other software that can scan BLE for advertising sensors.

The purpose of this example is to provide you with the base setup and get working sensor without much hassle. It exposes bare minimum from sensor's capabilities, if you want more functionality you would have to add it yourself, or use full-fledged library that would expose full capabilities of a particular chip.

Also this example doesn't do any power saving stuff. To build efficient battery-powered node you would need to figure it out yourself (and maybe enhance this example? :))

This should work without any changes on any BLE-capable CH5xx chip (including CH570).

## Sensor types

This example is designed to use one of these temperature (and humidity) sensors:

I2C:

- SHT2x (HTU21)
- SHT3x
- BME280
- BMP280 (doesn't support humidity readings)
- HDC1080

1-Wire:

- DS18B20 (none of these support humidity readings)
- DS18S20
- DS1820
- DS1822

## Packet types

This example provides 3 types of BLE advertisement formats to choose from.

``ADV_ATC`` - will emulate the [ATC_MiThermometer](https://github.com/atc1441/ATC_MiThermometer) firmware format. It can be read by OpenMQTT Gateway and then used in Home Assistant.

``ADV_BTHOME`` - encodes temperature and humidity values in a [BTHOME](https://bthome.io/) format. Can be easily integrated as a sensor in Home Assistant.

``ADV_CUSTOM`` - encodes temperature value as a *Health Thermometer* BLE standard service type. Will show temperature in a readable form in apps like *nRF Connect*.

## MAC address

Every BLE device needs a MAC address, it's the first 2-8 bytes of the advertisement packet that we are sending. You can make your own MAC, use MAC address of the MCU or use serial number of the temperature sensor as the BLE MAC. To use your own MAC - edit these 6 bytes (in reverse order) of the ``adv`` you're using. To use MAC of the MCU set ``#define USE_MCU_MAC 1``, to use sensor's serial number instead set ``#define USE_SENSOR_SERIAL_AS_MAC`` (Bosch sensors don't have serial number register, so it won't work with them).

## Using the example

Specify MCU model you're using in ``Makefile``.

Set config options at the top of ``iSLER_temp_sensor.c``. Connect the sensor of your choosing to SDA and SCL pins (if you're using I2C) or to a 1-Wire GPIO pin if you're using sensor of the DS18x20 family. Connect VCC and GND of the sensor to VCC and GND of the MCU board you're using.

Type ``make`` to program your board. Type ``make terminal`` if you want to see temperature readings in the terminal too. If everything goes as planned, you should be able to find your board with Bluetooth application of your choice.

# 1-wire example using a static lib

This example shows how to use ``extralibs/static_onewire.h`` lib to be able to talk to 1-wire devices.

``static_onewire.h`` follows ``static_i2c.h`` design and allows you to flexibly configure functionality and HW API for your exact needs.

You need to have this defines in your C file before you include the library:

``define PIN_ONE`` - a GPIO pin to be used for 1-wire bus

``#define ONEPREFIX`` - the prefix the will be used with all lib function names

``#define DELAY(n)`` - macro for the delay us function

``#define ONE_INPUT`` - macro for setting the 1-wire pin to input

``#define ONE_OUTPUT`` - macro for setting the pin to output

``#define ONE_SET`` - macro for setting the pin HIGH

``#define ONE_CLEAR`` - macro for setting the pin LOW

``#define ONE_READ`` - macro for reading the state of the pin

``#define ONENEEDSCAN`` - this will include a bus scan function that will populate an array of ``OneWire_dev_t`` structs that you will be able to use later.

``#define ONENEEDREADROM`` - this will include ``ReadRom`` function. It will read the first device it sees on the bus, you can use it to simplify things, if you have only one device connected.

``#define ONENEEDCRC8_TABLE`` - this will include a ``CRC8`` function that uses a tiny LUT and will enable CRC checking for functions, where it applies.

``define ONENEEDCRC16`` - will include ``CRC16`` function that is needed for checking CRC of transmissions of some 1-wire devices. Temperature sensors of DS18x20 series use CRC8 for everything.

## Reading DS18x20

To read temperature from these sensors you need first request the internal ADC conversion and wait a specified in the datasheet delay, before reading the temperature value. The delay depends on a resolution of the ADC that device is currently configured to. By default it will use 12 bit resolution, and conversion will take 750ms, other options are as such:

- 11 bit - 375ms
- 10 bit - 187.5ms
- 9 bit - 93.75ms

Sometimes the first reading can be 85°C, this is a normal behavior, described in the datasheet, this means that temperature register was not yet updated after power-on reset of the sensor.

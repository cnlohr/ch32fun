# LCD peripheral in CH5xx

Some of MCUs from CH5xx series have a LCD peripheral for driving simple segmented LCD displays. All supported chips has 4 common pins and up to 28 pins for segments depending on a model and a package.

To use a pin as common or segment pin for LCD you need to disable all other function for the particular GPIO pin and set it's function to input. You also need to disable digital input for these pins in corresponding registers (in this example it all is done in ``LCDInit`` function).

Turning segments on and off is done by writing bits to ``LCD_RAMn`` registers, where each segment takes 4 bits (1 bit per 1 common pin).

You can choose LCD duty cycle (number of commons used - 2, 3 or 4), LCD bias voltage ratio (1/3 or 1/2), LCD driver voltage (3.3V or 2.5V) and LCD scan frequency (128Hz, 256Hz, 512Hz and 1000Hz).

Apparently LCD peripheral can be run in sleep mode with 32kHz crystal enabled (not covered in this example).

Depending on number of pins used you may need to disable debug interface or reset button to be able to use particular segments. **Debug interface** can be disabled in firmware (ideally after a small delay at startup) but **Reset button** has to be disabled in **option bytes**.

## Trying this example

Select *common* and *segment* pins that you want to use by editing defines in the top of the file. Set actual ``NUMBER_OF_SEGMENTS`` and ``NUMBER_OF_COMMONS``. Set the model of your MCU in the Makefile, do ``make``.

## Pinout

|     | CH592/CH591F      | CH584/CH585        |
|:-:  |:-:                |:-:                 |
|COM0 |**PB12**           |**PB4**             |
|COM1 |**PB13**           |**PB5**             |
|COM2 |**PB14**           |**PB6**             |
|COM3 |**PB15**           |**PB7**             |
&nbsp;
|SEG0 |**PB7** *(D/F/X)*  |**PB0** *(M/F)*     |
|SEG1 |**PB4** *(D/F/X)*  |**PB1** *(M/F)*     |
|SEG2 |**PB23** *(D/F/X)* |**PB2** *(M/F)*     |
|SEG3 |**PB22** *(D/F/X)* |**PB3** *(M/F)*     |
|SEG4 |**PA4** *(D/F/X)*  |**PA0** *(M/F)*     |
|SEG5 |**PA5** *(D/F/X)*  |**PA1** *(M/F)*     |
|SEG6 |**PA15** *(D/F/X)* |**PA2** *(M/F)*     |
|SEG7 |**PA14** *(D/F/X)* |**PA3** *(M/F)*     |
|SEG8 |**PA13** *(F/X)*   |**PB8** *(M/F)*     |
|SEG9 |**PA12** *(F/X)*   |**PB9** *(M/F)*     |
|SEG10|**PA11** *(F/X)*   |**PB10** *(M/F)*    |
|SEG11|**PA10** *(F/X)*   |**PB11** *(M/F)*    |
|SEG12|**PA8** *(F/X)*    |**PB12** *(M/F)*    |
|SEG13|**PA9** *(F/X)*    |**PB13** *(M/F)*    |
|SEG14|**PB11** *(F/X)*   |**PB14** *(F)*      |
|SEG15|**PB10** *(F/X)*   |**PB15** *(F)*      |
|SEG16|**PB6** *(X)*      |**PB16** *(F)*      |
|SEG17|**PB0** *(X)*      |**PB17** *(F)*      |
|SEG18|**PA6** *(X)*      |**PB18** *(F)*      |
|SEG19|**PA7** *(X)*      |**PB19** *(F)*      |
|SEG20|                   |**PB20** *(F)*      |
|SEG21|                   |**PB21** *(F)*      |
|SEG22|                   |**PB22** *(F)*      |
|SEG23|                   |**PB23** *(F)*      |
|SEG24|                   |**PA7** *(F)*       |
|SEG25|                   |**PA8** *(F)*       |
|SEG26|                   |**PA9** *(F)*       |
|SEG27|                   |**PA13** *(F)*      |

# 74HC595 Shift Register Example
This example demonstrates how to control a 74HC595 shift register, showcasing
its similarity to the SPI communication protocol. The 74HC595 allows you to
expand your microcontroller's output capabilities using only a few pins.

## Key Features
SPI-like Interface: The 74HC595 operates similarly to SPI devices:
	DATA (DS) = SPI MOSI (Master Out Slave In)
	CLOCK (SH_CP) = SPI SCLK (Serial Clock)
	LATCH (ST_CP) = Similar to SPI CS (Chip Select)
Flexible Control: Choose between:
	Bit-banged GPIO (manual pin control)
	Hardware SPI (faster and more efficient)
Daisy-Chaining:
	Support for multiple 74HC595s to control more outputs


Comment this line out to see how to control the IC without using SPI
`#define CH32V003_SPI_SPEED_HZ		1000000`

## Hardware setup
//# 74HC595 Pinout
// Q1  - 1	 |	16 - Vcc
// Q2  - 2	 |	15 - Q0
// Q3  - 3	 |	14 - DATA	(SPI MOSI) - PC6
// Q4  - 4	 |	13 - CE	 	(Set Low) - to GND
// Q5  - 5	 |	12 - CLK	(SPI CLK) - PC5
// Q6  - 6	 |	11 - LATCH	- PC3
// Q7  - 7	 |	10 - RST	(Set High) - to Vcc
// GND - 8	 |	9  - Q7'	(Data output - for chaining to another 74HC595)


## Code Behavior
The demonstration sequence:
	Single LED Chase: Turns on each LED sequentially from Q7 to Q0
	Dual LED Chase: Shifts two lit LEDs through all positions
Repeating Patterns:
	Cycles through these patterns continuously

## Bit Order Note
	Q7 is the MSB (Most Significant Bit) - receives the first bit sent
	Q0 is the LSB (Least Significant Bit) - receives the last bit sent

HAPPY Shifting :)
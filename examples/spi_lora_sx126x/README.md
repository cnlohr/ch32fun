# LoRa SX126X Communication Example
This example demonstrates bidirectional communication between two 
LoRa devices using SX126X-series modules. Each device send out a message
every 3 seconds and then switch back to RX mode.

## Tested Hardware:

SX1262 (XL12620-PD1, blue board)
LLCC68 (E220-900M)

// DataSheet
// ref: https://www.mouser.com/pdfDocs/DS_LLCC68_V10-2.pdf?srsltid=AfmBOooSw4VnT0K2j8FhU1Ni7i8inGRofGu9IxMCgdq02lwq11JMnoPt

## WEIRD:

SPI prescaller 4 is needed for SX127X to work, Prescaller 2 works fine with SX126X

## WARNING:

Frequency Compliance
Always use frequencies approved for your region:

US915: 902-928 MHz (United States, Canada)
EU868: 863-870 MHz (Europe)
CN470: 470-510 MHz (China)
AS923: 915-928 MHz (Asia, other regions)

Time on Air is also limited in commercial applications.
https://www.thethingsnetwork.org/airtime-calculator

Antenna Safety
NEVER transmit without an antenna connected! RF energy reflects back into the transmitter without an antenna, which can:
Overheat and destroy the power amplifier (PA)
Cause permanent damage to the module

I've already damaged several modules this way - learn from my mistakes!

## SETUP:

Add an antenna to your module if it doesn't have one. Mine uses
915Mhz base band so I used a wire cut to length of about 8.0-8.2cm or 3.2"

- PC3 to RST
- PC4 to CS
- SPI SCLK to PC5
- SPI MOSI to PC6
- SPI MISO to PC7

Optional: you can wire DIO1 to an LED to see the status. It should
flash when receiving a message. You can also check the status of this pin to know when a message is received.

Note: CS needed to be control for operation, wire it directly to GND
to keep the module active won't work. The LoRa modules are very susceptible to noise on the SPI wires, use shorter wires for the SPI
connections. I had issues with dupont jumper wires.

## PARAMETERS:

Main parameters:
SF - Spreading Factor (larger FS (eg FS11) can travel further but take more air time)
BW - Bandwidth (Higher BW allows higher data rate at the expense of reduced sensitivity)
CR - Coding Rate (better correction and range at cost of data rate and air time)

Note:
- You can send and receive messages between SX126X and SX127X with a few exceptions.
- SX126X supports SF (5-11), power range -9 dBm to +22 dBm
- SX127X supports SF (6-12), power range -4 dBm to +20 dBm
- LLCC68 uses SX126X loRa
- FS6 on SX126X is not backward compatible with SF6 on SX127X.
- larger FS (eg FS11) can travel further but take more air time
  
## TODO:

Implment CAD (Channel Activity Detection) to allow sweeping for signals

ENJOY :)
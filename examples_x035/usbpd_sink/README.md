# USB-PD Sink Example

## Hardware
- PA12/PC14 CC1
- PA13/PC15 CC2

Connect each CC line though *its own* 5.1k resistor to gnd.

> [!NOTE]
> The CC lines can be swapped around.

## Usage
> [!WARNING]
> This example can cycle though the different voltage levels, make sure the circuit can handle the voltages.

The example will wait for a USB-PD source to be plugged in, and display all available voltages (including programmable voltages).

Simple command line interface:
 - 'c' - enable cycling through the different voltage levels
 - 'r' - reset the USB-PD controller
 - 'q' - quit the program (Resets the MCU)


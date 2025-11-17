This example demonstrates the MCO feature of the CH32V003.  This allows the device to output one of four internal
clock signals to the PC4 pin.  It cycles through no signal and each of the four different clock signals.  Each
signal is output for five seconds before moving on to the next.  The serial output displays the signals as they
are selected.

The different signals are:
0) Nothing
1) SYSCLK (48MHz)
2) HSI (24MHz)
3) HSE (depends on external XTAL)
4) PLL clock output (48MHz)

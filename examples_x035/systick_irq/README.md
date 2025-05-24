# Systick IRQ demonstration
This example shows how to set up the built-in Systick IRQ to generate periodic
interrupts for timing. Many bare-metal and RTOS based embedded applications will
use the Systick IRQ for timing, periodic housekeeping and task arbitration so
knowing how to set that up is useful.

This code is compatible with the busywait delay functions in the ch32fun
library because it allows the Systick CNT register to keep running over its full
32-bit range. It does not set the STRE bit in the CTRL register so the counter
does not reset when the CMP register matches. In order to do this, the CMP register
is advanced at every IRQ.

Note also the use of the  `__attribute__((interrupt))` syntax in declaring the
IRQ handler. Some of the IRQ examples from the WCH HAL library have slightly
different syntax to make use of a fast IRQ mode but which is not compatible with
generic RISC-V GCC so that feature is not used here.

# Use
Connect LEDs (with proper current limit resistors) to GPIO pins C0, D0, D4 and
The LEDs will flash and the Current Milliseconds since system power-on,
Microseconds since power-on, Milliseconds taken per loop, and a raw SysTick Counter
value will be printed via the debug terminal, or over UART on pin PD5.

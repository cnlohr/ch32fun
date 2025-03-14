# Polled ADC example
This example code demonstrates the use of the CH32V006 on-chip ADC in polled mode
wherein a single analog input channel is configured on GPIO and then digitized
under software control. When a sample is needed the ADC is started and the CPU
busy-waits until the conversion is complete before reading the result.

This is just about the simplest and easiest use-case for the ADC and wastes a lot
of CPU cycles but it does show the basics of doing conversions in a situation
where high performance is not needed.

## Use
Connect an analog voltage source that varies between 0V and 3.3v to GPIO pin PD4
and a 3.3V logic level serial terminal to pin PD5. The 12-bit result will be
printed periodically on the serial output.

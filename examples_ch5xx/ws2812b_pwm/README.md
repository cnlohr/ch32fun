# WS2812B PWM and TMR DMA example

This shows how to write WS2812B LED data using PWM, TMR & DMA on the CH5xx.
The main reasons you might want to do this instead of using SPI is if you don't have or want
to use the SPI pins or you want to drive more than one set of LEDs.  In this example the
entire buffer is generated and then sent by DMA in one go.  With long strips an interrupt
could be used to refill the buffer like the SPI driver does.

Also included is an example of using UART0 to write WS2812B data on CH584/5 which is possible
due to its facility to invert the output.  This is not DMA driven though and the 8 byte FIFO 
only gives a buffer of about 30 microseconds. (3 bits per FIFO entry = 24 bits x 1.25 
microseconds).  In theory UART could be used with an external inverter too but SPI or PWM is
usually a better option.

Tested on CH572, CH592 and CH584.

## Memory usage

The TMR buffer uses 32 bits per 1 bit of RGB pixel data.

On CH57x the PWM buffers use 16 bits per 1 bit but you always pay the cost of at least 2 
channels so it works out the same if you're only driving one set of LEDs.

The UART method is currently set up to use 32 bits per 24 bits of data because it converts
as it goes.

## CH57x

Two or three sets of LEDs can be driven simultaneously on the PWM channels. In addition to this
another strip can be driven by the TMR.
 
PWM channels 1-5 are on pins PA7, PA2, PA3, PA4, PA8

TMR can be on pin PA7, PA2, PA4 or PA9

## CH584/5 and CH592

Two sets of LEDs can be driven simultaneously by the timers.

TMR1 is PA10 or PB10

TMR2 is PA11 or PB11

## CH584/5

UART0 is PB7 or PA14
# WS2812B SPI DMA example

Single-file-header for SPI-DMA to output WS2812B LEDs.

This outputs the LED data on the MOSI (PA7) pin of the CH570/2 or PA14 if you're using CH582/3 or CH591/2.

The timing on the SPI bus is not terribly variable, the best I was able to find was:

* Ton0 = 333ns
* Ton1 = 1us
* Toff0 = 1us
* Toff1 = 333ns
* Treset = 68us

## Usage

If you are including this in main, simply 
```c
#define WS2812DMA_IMPLEMENTATION
#include "ws2812b_dma_spi_led_driver.h"
```

You will need to implement the following two functions, as callbacks from the ISR.
```c
uint32_t CallbackWS2812BLED( int ledno );
```

You will also need to call
```c
InitWS2812DMA();
```

Then, when you want to update the LEDs, call:
```c
WS2812BStart( int num_leds );
```

If you want to see if it's done sending, examine `WS2812BLEDInUse`.


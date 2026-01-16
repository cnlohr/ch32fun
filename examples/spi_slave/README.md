# SPI Slave/Peripheral Example

This example demonstrates how to configure a microcontroller as an SPI slave (peripheral) device using the SPI protocol. The SPI slave will receive data from an SPI master and can also send data back to the master, using MOSI and MISO lines
respectively. As long as the CH32 microcontroller supports SPI, this example should work with only minimal modifications.

This example does not use DMA for data transfer, and is more so intended to demonstrate the basic SPI peripheral functionality using interrupts and a simple polling loop.
As with most demos, there is still room for optimization and improvements; such as the previously mentioned DMA, handling errors, and more loosely defined buffers instead of fixed-size ones depending on your application needs. This is a
good
base to learn from and build upon.

## Hardware Setup

To test this example, you will need two microcontroller boards: one configured as an SPI master and the other as an SPI slave. The master SPI device can be whatever you like. The wiring follows the default SPI pinout of the specific CH32
board.
For example on a CH32V003F4P6, it'll be:

- MOSI -> MOSI `PC6`
- MISO -> MISO `PC7`
- SCK -> SCK `PC5`
- NSS -> NSS (aka CS) `PC1`

Refer to the datasheet or pinout diagram for the exact SPI pin locations.

## How it works

- First, we initialize the SPI in slave mode with all the desired settings. This demo just picked some sensible defaults e.g. baud rate prescaler. It's important to note that it sets the mode to `SPI_Mode_Slave` instead of
  `SPI_Mode_Master`.
- Once initialization is done, we enter the main loop. In there we simply wait for CS to be low. You could do something in the meantime whilst CS is high, but this demo just waits. To keep things simple this doesn't use an interrupt for CS
  and thus be careful to not over utilize the CPU in the main loop.
- When CS goes low, this is the signal the SPI master/controller is starting a transaction to this device. We enable the `RXNE` (RX buffer Not Empty) interrupt so we can receive data. We change the state to receiving, and simply wait for
  the state to be changed to received from within the interrupt.
- Within the interrupt, we read byte by byte from the SPI data register as long as there is data to read (RXNE flag is set). Once the MOSI buffer is full, we change the state to received, disable the RXNE interrupt, prepare the MISO buffer,
  preload the first byte, and finally re-enable to RXNE interrupt.
- Within state sending, we send each byte from the MOSI buffer back to the master based on the clock cycles provided by the master. Once all bytes are sent, we wait for CS to be back high, indicating the end of the transaction. At this
  point the full transfer is complete: data was received from MOSI, and sent through MISO back to the master.
- At the end we also log it over serial for debugging purposes as `RX/MOSI(4): 00 00 00 00  | TX/MISO(4): 00 00 00 00` per transaction. Do note this adds about 100us of delay at least each cycle at the end, so if the clock rate is high
  enough you logically need to remove the serial logging to keep up. (at which
  point you should probably move to DMA and more interrupts where needed.)

## Example Master SPI code

For simplicity sake here is an example SPI master code snippet in CircuitPython you can flash to another microcontroller. You can of course also use ch32fun for the master side as well, that's all your call.
The code below sends 4 bytes to the slave and reads back 4 bytes from the slave. both buffers are printed to the console. CS is set to pin D6.
The four bytes it writes are incrementing numbers from 0 to 255, letters from A to Z, and 41 & 42.

```python
import digitalio
import time
from board import *

spi_bus = SPI()
try:
    cs = digitalio.DigitalInOut(D6)
    bytes_read = bytearray(4)
    bytes_wrote = bytearray(4)
    bytes_wrote[2] = 41
    bytes_wrote[3] = 42

    while True:
        bytes_wrote[0] = (bytes_wrote[0] + 1) % 256  # 0 to 255
        bytes_wrote[1] = ord('A') + (bytes_wrote[1] - ord('A') + 1) % 26  # A to Z

        spi_bus.try_lock()
        spi_bus.configure(baudrate=100000, polarity=0, phase=0)
        cs.value = False
        spi_bus.write(bytes_wrote)
        spi_bus.readinto(bytes_read)
        cs.value = True
        spi_bus.unlock()

        print("MOSI(4): " + " ".join(hex(b) for b in bytes_wrote) + " | " +
              "MISO(4): " + " ".join(hex(b) for b in bytes_read))

        time.sleep(.1)
finally:
    spi_bus.deinit()
```
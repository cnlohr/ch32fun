# CDC-ACM example

This example lets turns any WCH's MCU with a USB peripheral into a USB to UART adapter.

## How to use

- Select your chip model and package type in the Makefile.

- Select UART number (by default it uses the second UART peripheral).

- Select GPIO pins for TX/RX and DTR. You can disable DTR by removing the define.

- If you want to change other critical variables or default UART settings, you can find them in ``uart.h``.

- Connect your MCU using a method of your choice and do ``make`` inside this folder.

- On linux you may have to install udev rules with ``make install_udev_rules`` or copying the ``99-ch32fun.rules`` file manually.

## Things to mention

CH32 chips use DMA for receiving/sending UART data, CH5xx have only 8 bytes FIFO, so they use polling method to acquire data. At first glance it may seem as a drawback, but on practice it produces simpler, smaller code and helps to eliminate garbage in buffers when switching UART settings (baudrate for example).

If this firmware proves to be stable enough, CH570 becomes the cheapest USB to Serial converter on the market :)

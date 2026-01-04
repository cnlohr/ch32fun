# I2C to USB adapter

This is a ch32 port/reimplementation of [I2C-Tiny-USB](https://github.com/harbaum/I2C-Tiny-USB/) for Atmel chips (atmega328, attiny85). That one has a linux kernel driver that is present on most of modern linux distros, so it should work out of the box. Original firmware was using bit-banged USB driver, so it was only LS capable, since this is using hardware USB FS, EP buffer is larger - 64 bytes.

Since linux driver is hardcoded for specific VID:PID pair, this firmware uses that too. If you change it, it will stop working with ``i2c-tiny-usb`` device driver.

You can change other strings in ``usb_config.h``, though.

## How to use

- Select your MCU model in the ``Makefile``
- Change I2C SDA and CLK pins if you want
- Check other options.
- Connect your board and do ``make`` in this folder.
- Connect via USB to a PC and use software that can use default i2c driver, for example ``i2cdetect``. You may need to run commands as sudo. If you want to be able to use your i2c device as user, you can install udev rules with ``make install_udev_rules`` or copy it manually.

## Notable mention

I was testing this firmware on this https://github.com/armlabs/ssd1306_linux

Almost everyone has one of those OLEDs in their stash and it nice to see it works from the linux command line :)

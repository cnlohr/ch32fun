# Execute code from DataFlash

This example builds the blink loop in `blink.c` as a separate binary at
DataFlash address `0x00070000`. The app initializes the chip, then calls the
blink function and passes its `DelaySysTick` and `printf` functions as callbacks.
The blink image is reserved 256 bytes. `printf` output uses the debug interface.
Tested on CH582.

you may need to update minichlink to get the eeprom address for your board if it's out of date.
`make -C ../../minichlink`

To rewrite only the DataFlash image:
`make flash-dataflash`

then just build:
`make all monitor`

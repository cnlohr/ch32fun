# Cheap 433 MHz transmitter OOK demo

This example shows how to send simple OOK protocols using a cheap 433 MHz trasnmitter board similar to the popular [rc-switch library](https://github.com/sui77/rc-switch).

Timing is probably not very accurate since it uses only delays and isn't optimized, but a Flipper Zero recognizes it as princeton signals.

## Pinout

| CH32V003 | Transmitter | Description |
| --- | --- | --- |
| PD4 | NC | Button to enable send |
| PD6 | DATA | Data in port |

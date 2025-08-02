# USB CDC TTY
This example creates a `/dev/ttyACM0` (or higher number if something went wrong on 0) which you can monitor for messages, and send simple one-character commands to.

It is made with ISP programming in mind (although using the debug interface works too),
so one can enter the ISP bootloader by pressing the reset button while holding the boot/download button. (although this does not work on 570/2 currently)

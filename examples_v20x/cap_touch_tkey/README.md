# Captive Touch using Tkey peripheral

This example shows how to use the Tkey peripheral to sample captive buttons.

It assumes that there are leds on pins PA4 and PB11, and 4 touch pads on PB1, PB0, PA7 and PA5. (This was made for my [comu](https://github.com/cheyao/comu) pcb, for pad size reference)

It turns on the left led when PB1 touched, off when PB0 touched. Same for right led and PA7 and PA5.

----

The Tkey peripheral returns a value between 0 and 4096. On my board when pressed the value drops from ~4096 to ~1000, but this is probably dependant on pad size.

You can also adjust the charge time and sampling delay with the IDATAR1 and RDATAR registers.

# one GPIO libaray to rule them all

This Arduino-like GPIO library offers
 * digital IO
 * analog-to-digital
 * digital-to-analog (PWM)

Great care has been taken to make the resulting code as fast and tiny as possible. Let the compiler suffer!  
Hand-written blink compiles to 500 bytes, blink using this library compiles to 504 bytes!  

# GPIO Example

Connect LED + 1k resistor to each LED pin (C0 to C7 and D4) and GND.  
Connect a button to GND and D3.  
Connect a 10k pot between GND and VCC, wiper to D6.  

The desired demo may be selected in GPIO.c by setting it to 1 and the others to 0.  
Marvel at the colorful glory.  



https://github.com/recallmenot/ch32v003fun/assets/104343143/afb4027d-a609-467a-96c5-0cc3283366a4

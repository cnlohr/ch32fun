#include "ch32fun.h"

// This code generates a signal on a GPIO pin meant to drive an LED or similar device
// Since the human visual system deals on an exponential scale, the output needs to be
// scaled apropriately.  Instead of linearly increasing the brightness, increase it
// in proportion to the square of the desired value.
//
// Since the 003 has no mulitply instruction, this would be expensive, so use the
// equation:
// x^2 = (x-1)^2 + 2*x - 1
// or:
// (x-1)^2 = x^2 - 2*x + 1
//
// This allows us to take an x^2 value and quickly calculate (x+1)^2 or (x-1)^2 with nothing
// but a few addition or subtractions.  Note that the 2*x is implemented as a shift operation.

// The inner loop performs an accumulate operation and--depending on if it overflows--sets the 
// GPIO high, it otherwise clears it.  This produces a Pulse Density Modulation signal on the 
// chosen GPIO pin.  This can driectly drive an LED at a reasonable brightness.

// Pick a GPIO pin
#define PIN PD6

// SCALE will determine how many inner loop iterations we do between intensity calculation, 
// alter this to adjust the speed of the breath (smaller is faster)
#define SCALE  64

int main(){
	uint32_t lung = 1, roll = 0, volume = 1;
	int32_t io = 1;

	// Setup the clocks and other default values with the ch32fun environment
	SystemInit();
	// Enable GPIOs
	funGpioInitAll();
	// Set PIN to output
	funPinMode( PIN, GPIO_Speed_10MHz | GPIO_CNF_OUT_PP );

	while(1){
		for(uint32_t r = SCALE; r != 0; r--){
			// if we overflow this calculation, set the output to high, otherwise set it low
			roll += lung;
			funDigitalWrite(PIN, (roll < lung)?FUN_HIGH:FUN_LOW);
		}

		volume += io;
		if((volume == 1) || (volume == 65535))
		  // If we've hit the top or bottom, change direction
			io = -io;
		else{
		  // Otherwise, we are in between and going up or down, fill or deflate our lungs appropriately
		  if(io == 1)
				lung += ((volume << 1) - 1);
			else
				lung -= ((volume << 1) - 1);
		}
	}
}

#include "ch32fun.h"

static void RNG_init(void) {
	// Enable RNG clock
	RCC->AHBPCENR |= RCC_AHBPeriph_RNG;
	
	// Enable the RNG
	RNG->CR |= RNG_CR_RNGEN;
}

// Get a random number (blocking)
static u32 RNG_rand(void) {
	// Wait until data is ready
	while(!(RNG->SR & RNG_SR_DRDY)) {
		// Handle seed error
		if(!(RNG->SR & RNG_SR_SECS)) continue;

		RNG->CR &= ~RNG_CR_RNGEN;		// Disable RNG
		RNG->SR = 0;					// Clear error flags
		RNG->CR |= RNG_CR_RNGEN;		// Re-enable RNG
	}
	
	// Read and return random number
	return RNG->DR;
}
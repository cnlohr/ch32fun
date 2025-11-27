// Tested with CH32V002, CH32V006, and CH32V303

#include "ch32fun.h"

static void PVD_init(u8 threshold) {
	if (threshold > PVD_MAX_THRESHOLD_LVL) threshold = PVD_MAX_THRESHOLD_LVL;

	// Enable PWR clock
	RCC->APB1PCENR |= RCC_APB1Periph_PWR;

	// Enable PVD
	PWR->CTLR |= PWR_CTLR_PVDE;
	
	// Clear the existing PLS bits and set new threshold
	PWR->CTLR = (PWR->CTLR & ~PWR_CTLR_PLS) | (threshold << 5);
}

// Get threshold setting: the PLS[1:0] bits
static int PVD_getThreshold() {
	return (PWR->CTLR & PWR_CTLR_PLS) >> 5;
}	

// return PVD flag: 1 if VDD below threshold
static int PVD_getAlert() {
	return PWR->CSR & PWR_CSR_PVDO;
}

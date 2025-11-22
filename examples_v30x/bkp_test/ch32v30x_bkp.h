// MIT License
// Copyright (c) 2025 UniTheCat

#include "ch32fun.h"

void BKP_init(void) {
	// Enable power and backup interface clock
	RCC->APB1PCENR |= RCC_APB1Periph_PWR | RCC_APB1Periph_BKP;
	
	// Enable access to backup registers
	PWR->CTLR |= PWR_CTLR_DBP;

	//! IMPORTANT: Clear tamper event to prevent disabling of data write
	//! after a tamper detection
	BKP->TPCSR |= BKP_CTE;
}

u32 _BKP_calculateAddress(u8 reg_num) {
	if(reg_num < 1 || reg_num > 42) return 0;

	if(reg_num <= 10) {
		// Registers 1-10: 0x40006C04 to 0x40006C28
		return BKP_BASE + 0x04 + ((reg_num - 1) * 4);
	} else {
		// Registers 11-42: 0x40006C40 to 0x40006CBC  
		return BKP_BASE + 0x40 + ((reg_num - 11) * 4);
	}
}

// Write data to backup register
void BKP_WriteData(u8 reg_num, u16 data) {
	u32 address = _BKP_calculateAddress(reg_num);
	if (address == 0) return;
	*(volatile u32*)address = data;
}

// Read data from backup register
u16 BKP_ReadData(u8 reg_num) {
	u32 address = _BKP_calculateAddress(reg_num);
	if (address == 0) return 0;
	return *(volatile u32*)address;
}

// only PC13 is supported as alarm pulse for tamper detection
void BKP_tamperInit(u8 pc13_tamper_highActive) {
	// Clear tamper event
	BKP->TPCSR |= BKP_CTE;
	
	// Configure tamper pin active level
	BKP->TPCTLR &= ~BKP_TPE;	// Disable tamper first

	BKP->TPCTLR &= ~BKP_TPAL;   // Set active level high (0=high, 1=low)

	if (!pc13_tamper_highActive) {
		BKP->TPCTLR |= BKP_TPAL;	// low-active
	}

	// Enable tamper interrupt (optional)
	BKP->TPCSR |= BKP_TPIE;
	
	// Enable tamper detection
	BKP->TPCTLR |= BKP_TPE;
	
	// Enable interrupt if using interrupts
	NVIC_EnableIRQ(TAMPER_IRQn);
}

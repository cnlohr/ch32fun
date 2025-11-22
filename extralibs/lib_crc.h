// should work for CH32v10x, CH32v20x, and CH32v30x
// Although has only been tested on CH32v303

static void CRC_init(void) {
	// Enable CRC clock
	RCC->AHBPCENR |= RCC_AHBPeriph_CRC;
}

// Calculate CRC for a single 32-bit value
static u32 CRC_calculate32(u32 data) {
	// Reset CRC unit
	CRC->CTLR = CRC_CTLR_RESET;
	
	// Write data to trigger CRC calculation
	CRC->DATAR = data;
	
	// Wait for CRC calculation
	u32 timeout = 10000;
	while (CRC->DATAR == 0 && --timeout);

	// read the CRC result
	return CRC->DATAR;
}

// Calculate CRC for an array of 32-bit values
static u32 CRC_calculateArray32(u32 *data, u32 length) {
	CRC->CTLR = CRC_CTLR_RESET;
	
	// Process each 32-bit word
	for(u32 i = 0; i < length; i++) {
		CRC->DATAR = data[i];
	}
	
	// Return final CRC
	return CRC->DATAR;
}

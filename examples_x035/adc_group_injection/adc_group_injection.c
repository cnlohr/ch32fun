// Simple example that show how to do adc group injection readings
// on Channel 1, 2, 3, 4 corresponding to PA1, PA2, PA3, PA4 respectively

// Differences between injection and rule group:
// Features			Rule Group			Injection Group
// Max Channel		16					4
// Data Registers	1 (overwritten)		4 (one per channel)
// DMA support		Yes					No
// NOTE: Injection group requires Scan mode and External Trigger enabled 
// even if using software trigger

#include "ch32fun.h"
#include <stdio.h>
#include "register_debug_utilities.h"

void adc_injection_init(u8 sampling_time) {
	// Enable the ADC1 module
	RCC->APB2PCENR |= RCC_ADC1EN;

	// Reset the ADC to init all regs
	RCC->APB2PRSTR |= RCC_ADC1RST;
	RCC->APB2PRSTR &= ~RCC_ADC1RST;

	// Turn on ADC
	ADC1->CTLR2 |= ADC_ADON;

	//! REQUIRED: SCAN mode for injection group
	ADC1->CTLR1 = ADC_SCAN;
	
	//! REQUIRED: Configure software trigger for injection group
	// ADC1->CTLR2 |= ADC_JEXTTRIG;		// JEXTTRIG = 1 (enable external trigger)
	ADC1->CTLR2 |= (0b111 << 12); 		// JEXTSEL = 0b111 (software trigger)

	// Set to divide by 8
	ADC1->CTLR3 = (ADC1->CTLR3 & ~ADC_CTLR3_CLK_DIV) | ADC_CTLR3_CLK_DIV_3;

	funPinMode(PA1, GPIO_CFGLR_IN_ANALOG);
	funPinMode(PA2, GPIO_CFGLR_IN_ANALOG);
	funPinMode(PA3, GPIO_CFGLR_IN_ANALOG);
	funPinMode(PA4, GPIO_CFGLR_IN_ANALOG);

	// set the sampling_time for all channels
	for (int i = 0; i < 10; i++) {
		ADC1->SAMPTR2 |= sampling_time << (3 * i);
		ADC1->SAMPTR1 |= sampling_time << (3 * i);
	}

	u8 channels[] = {1, 2, 3, 4};

	// set number of injected channels
	ADC1->ISQR = ((sizeof(channels) - 1) << 20);

	// set inject sequence per channel
	for (int i = 0; i < sizeof(channels); i++) {
		ADC1->ISQR |= channels[i] << (5 * i);
	}

	// Power up ADC again
	ADC1->CTLR2 |= ADC_ADON;
}

void adc_Injection_Conversion() {	
	// Clear any pending flags
	ADC1->STATR &= ~(ADC_JEOC);
	
	// Start injection conversion using external trigger method
	ADC1->CTLR2 |= ADC_JSWSTART;
	
	// Wait for all conversions to complete
	u32 timeout = 1000000;
	while(!(ADC1->STATR & ADC_JEOC) && timeout--) {}
	
	if (timeout <= 0) {
		printf("ERROR: Injection timeout (JEOC not set)\n");
		UTIL_PRINT_REG32(ADC1->STATR, "STATR");
		return;
	}
	
	// Read all results
	u16 read1 = ADC1->IDATAR1 & 0x0FFF;
	u16 read2 = ADC1->IDATAR2 & 0x0FFF;
	u16 read3 = ADC1->IDATAR3 & 0x0FFF;
	u16 read4 = ADC1->IDATAR4 & 0x0FFF;

	// Clear JEOC flag
	ADC1->STATR &= ~ADC_JEOC;

	printf("injection readings: %d, %d, %d, %d\n", read1, read2, read3, read4);
}


int main() {
	SystemInit();
	funGpioInitAll(); // Enable GPIOs

	printf("\n~ ADC Group Injection Example ~\n");
	printf("Chip ID: %08lX\n", ESIG->UID0);
	printf("Chip Capacity: %d KB\n", ESIG->CAP);

	// 0b111 = 11 cycles of sampling time
	adc_injection_init(0b111);

	while(1) {
		adc_Injection_Conversion();
		Delay_Ms(1000);
	}
}

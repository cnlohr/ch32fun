// Simple example that show how to do adc touch key readings on Channel 5 (PA5)
// add a wire to PA5 for ease of access, touch or hold on it for a short time to see the readings
// adjust the threshold to print when a touch is detected
// you can set thethreshold to 0xFFFF to see all of the readings
// the threshold will change depending on the values set for charge and discharge time
// increase the charge and discharge values to get more sensitive touch, too high will cause noise

#include "ch32fun.h"
#include <stdio.h>
#include "register_debug_utilities.h"

#define TOUCHKEY_PIN PA5
#define TOUCHKEY_THRESHOLD 3900

void adc_touchKey_init(u8 sampling_time) {
	// Enable the ADC1 module
	RCC->APB2PCENR |= RCC_ADC1EN;

	// Reset the ADC to init all regs
	RCC->APB2PRSTR |= RCC_ADC1RST;
	RCC->APB2PRSTR &= ~(RCC_ADC1RST);

	// Turn on ADC
	ADC1->CTLR2 |= ADC_ADON;

	// Set to divide by 8
	ADC1->CTLR3 = (ADC1->CTLR3 & ~ADC_CTLR3_CLK_DIV) | ADC_CTLR3_CLK_DIV_3;

	ADC1->RSQR1 = 0;				// number of converstions in sequence = n + 1
	ADC1->RSQR3 = TOUCHKEY_PIN;		// Channel 5 (PA5) is 1st in sequence

	// set the sampling_time for all channels
	for (int i = 0; i < 10; i++) {
		ADC1->SAMPTR2 |= sampling_time << (3 * i);
		ADC1->SAMPTR1 |= sampling_time << (3 * i);
	}

	// Enable TKEY module
	ADC1->CTLR1 |= (1 << 24);
	printf("\nPrint Regs:\n");
	UTIL_PRINT_REG32(ADC1->CTLR1, "CTLR1");

	printf("\nPrint Bit Ranges:");
	UTIL_PRINT_BIT_RANGE(
		ADC1->SAMPTR2,
		"\nSMP2", 8, 6,
		"\nSMP1", 5, 3,
		"\nSMP0", 2, 0,
		NULL
	);

	printf("\nPrint bits (default): \n");
	UTIL_PRINT_BITS(ADC1->CTLR1, 32, 16);

	printf("\nPrint bits values:");
	UTIL_PRINT_BITS_VALUES(
		ADC1->CTLR1,
		"\nTKEYEN", 24,
		"\nEOCIE", 5
	);

	// Turn on ADC
	ADC1->CTLR2 |= ADC_ADON;
}

u16 adc_touchKey_read() {
	//! REQUIRED: Configure charge and discharge time to start a conversion
	TKey->CHGOFFSET = 0xA0;   	// Set Charge time Offset
	TKey->DR_ACT_DCG = 0x10;   	// Set Discharging Time

	// Wait for conversion complete
	int timeout = 100000;
	while(!(ADC1->STATR & ADC_EOC) && timeout--) {}
	
	if (timeout <= 0) { return 0xFFFF; }
	u16 touch_value = ADC1->RDATAR;

	if (touch_value < TOUCHKEY_THRESHOLD) {
		printf("touch value: %d\n", touch_value);
	}
	
	return touch_value;
}

int main() {
	SystemInit();
	funGpioInitAll(); // Enable GPIOs

	printf("\n~ ADC Group Injection Example ~\n");
	printf("Chip ID: %08lX\n", ESIG->UID0);
	printf("Chip Capacity: %d KB\n", ESIG->CAP);

	funPinMode(TOUCHKEY_PIN, GPIO_CFGLR_IN_ANALOG);

	// 0b111 = 11 cycles of sampling time
	adc_touchKey_init(0b111);

	while(1) {
		adc_touchKey_read();
	}
}

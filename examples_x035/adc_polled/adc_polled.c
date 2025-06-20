#include "ch32fun.h"
#include <stdio.h>

// Initialize ADC1
void adc_init( void ) {

	// Enable the ADC1 module
	RCC->APB2PCENR |= RCC_ADC1EN;

	// Reset the ADC to init all regs
	RCC->APB2PRSTR |= RCC_ADC1RST;
	RCC->APB2PRSTR &= ~(RCC_ADC1RST);

	// Turn on ADC
	ADC1->CTLR2 |= ADC_ADON;
	
	// Set to divide by 8
	ADC1->CTLR3 &= ~(ADC_CTLR3_CLK_DIV);
	ADC1->CTLR3 |= (ADC_CTLR3_CLK_DIV_3);
	
	// Set up single conversion on chl 15
	ADC1->RSQR1 = 0;
	ADC1->RSQR2 = 0;
	ADC1->RSQR3 = 15;
	
	// set sampling times to 11 cycles 0b11 for all channels
	ADC1->SAMPTR1 = 0x00FFFFFF;
	ADC1->SAMPTR2 = 0x3FFFFFFF;
		
	// turn on ADC and set rule group to sw trig
	ADC1->CTLR2 |= ADC_ADON | ADC_EXTSEL;
}

// Do a conversion
uint16_t adc_get( void ) {
	ADC1->CTLR2 |= ADC_SWSTART;
	
	// Wait for it to complete
	while(!(ADC1->STATR & ADC_EOC));
	
	return ADC1->RDATAR;
}

int main() {
	uint32_t count = 0;
	
	SystemInit();

	printf("adc_polled example\ninitializing adc...");
	adc_init();
	printf("done\n");
	
	// Enable GPIOs
	RCC->APB2PCENR |= RCC_APB2Periph_GPIOA 
									| RCC_APB2Periph_GPIOB 
									| RCC_APB2Periph_GPIOC;

	// GPIO A0, 1, 2, 3, 4, 5, 6, 7, 11 as input/analog
	GPIOA->CFGLR &= ~( (0xf<<(4*0)) | (0xf<<(4*1)) | (0xf<<(4*2)) 
									 | (0xf<<(4*3)) | (0xf<<(4*4)) | (0xf<<(4*6)) 
									 | (0xf<<(4*7)) );
	GPIOA->CFGHR &= ~( (0xf<<(4*3)) );

	// GPIO B0, 1, 7 as input/analog
	GPIOB->CFGLR &= ~( (0xf<<(4*0)) | (0xf<<(4*1)) | (0xf<<(4*7)) );

	// GPIO C3, 16, 17, 19 as input/analog
	GPIOC->CFGLR &= ~( (0xf<<(4*3)) );
	GPIOC->CFGXR &= ~( (0xf<<(4*0)) | (0xf<<(4*1)) | (0xf<<(4*3)) );

	// GPIO A9 as output for blink
	GPIOA->CFGHR &= ~( (0xf<<(4*1)) );
	GPIOA->CFGHR |= ( (GPIO_CFGLR_OUT_10Mhz_PP) << (4*1) );

	printf("looping:\n");

	while(1)
	{
		uint16_t adc_value;

		// Set A9 high
		GPIOA->BSHR = 0x02;
		Delay_Ms( 250 );

		// Set A9 low
		GPIOA->BSHR = (0x02<<16);
		Delay_Ms( 250 );

		adc_value = adc_get();
		printf( "Count: %lu adc: %d\n", count++, adc_value );
	}
}

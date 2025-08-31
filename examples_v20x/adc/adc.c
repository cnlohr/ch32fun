#include "ch32fun.h"
#include "ch32v20xhw.h"
#include <stdio.h>


void adc_init( void )
{
	// ADCCLK = 12 MHz (APB2 CLK=72MHz, divide by 6)

	RCC->CFGR0 &= ~RCC_ADCPRE;
	RCC->CFGR0 |= RCC_ADCPRE_DIV6;
	
	// // Enable GPIOA and ADC
	RCC->APB2PCENR |= RCC_APB2Periph_GPIOA | RCC_APB2Periph_ADC1;
	
	// // PA7 is analog input chl 7
	GPIOA->CFGLR &= ~(0xf<<(4*7));	// CNF = 00: Analog, MODE = 00: Input
	
	// Reset the ADC to init all regs
	RCC->APB2PRSTR |= RCC_APB2Periph_ADC1;
	RCC->APB2PRSTR &= ~RCC_APB2Periph_ADC1;
	
	// Set up single conversion on chl 7
	ADC1->RSQR1 = 0;
	ADC1->RSQR2 = 0;
	ADC1->RSQR3 = 7;	// 0-9 for 8 ext inputs and two internals
	
	// set sampling time for chl 7
	ADC1->SAMPTR2 &= ~(ADC_SMP0<<(3*7));
	ADC1->SAMPTR2 |= 7<<(3*7);	// 0:7 => 3/9/15/30/43/57/73/241 cycles
		
	// turn on ADC and set rule group to sw trig
	ADC1->CTLR2 |= ADC_ADON | ADC_EXTSEL | ADC_EXTTRIG;
	
	// Reset calibration
	ADC1->CTLR2 |= ADC_RSTCAL;
	while(ADC1->CTLR2 & ADC_RSTCAL);
	
	// Calibrate
	ADC1->CTLR2 |= ADC_CAL;
	while(ADC1->CTLR2 & ADC_CAL);
	
	// should be ready for SW conversion now
}

/*
 * start conversion, wait and return result
 */
uint16_t adc_get( void )
{
	// start sw conversion (auto clears)
	ADC1->CTLR2 |= ADC_SWSTART;
	
	// wait for conversion complete
	while(!(ADC1->STATR & ADC_EOC));
	
	// get result
	return ADC1->RDATAR;
}

/*
 * entry
 */
int main()
{
	SystemInit();

	// APB2 clock 72MHz (144MHz HCLK / 2)
	RCC->CFGR0 &= ~RCC_PPRE2;
	RCC->CFGR0 |= RCC_PPRE2_DIV2;

	printf("initializing adc...");
	adc_init();
	printf("done.\n\r");

	while(1)
	{
		printf( "adc: %d\n\r",  adc_get());
	}
}
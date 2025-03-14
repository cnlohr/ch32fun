/*
 * Example for using ADC with polling
 * 03-27-2023 E. Brombaugh
 */

#include "ch32fun.h"
#include <stdio.h>

/*
 * initialize adc for polling
 */
void adc_init( void )
{
	// ADCCLK = 24 MHz => RCC_ADCPRE divide by 2
	// ADC_CLK_MODE = 0 -- HB clock divides to make ADC clock
	// ADC_CLK_ADJ = 0 -- 1/2 duty cycle high level ADC clock
	RCC->CFGR0 = (RCC->CFGR0 & ~(RCC_ADCPRE | RCC_CFGR0_ADC_CLK_MODE | RCC_CFGR0_ADC_CLK_ADJ)) | RCC_ADCPRE_DIV2;
	
	// Enable GPIOD and ADC
	RCC->PB2PCENR |= RCC_ADCEN | RCC_IOPDEN;
	
	// PD4 is analog input chl 7
	GPIOD->CFGLR &= ~(GPIO_CFGLR_MASK<<(4*4));
	GPIOD->CFGLR |= (GPIO_CFGLR_IN_ANALOG<<(4*4));
	
	// Reset the ADC to init all regs
	RCC->PB2PRSTR |= RCC_ADC1RST;
	RCC->PB2PRSTR &= ~RCC_ADC1RST;
	
	// Turn on ADC module
	ADC1->CTLR2 |= ADC_ADON;

	// Set up single conversion on chl 7  ### is this necessary?
	ADC1->RSQR1 = 0;
	ADC1->RSQR2 = 0;
	ADC1->RSQR3 = 7;	// 0-9 for 8 ext inputs and two internals
	
	// set sampling time for chl 7
	ADC1->SAMPTR2 &= ~(ADC_SMP0<<(3*7));
	ADC1->SAMPTR2 |= 7<<(3*7);	// 0:7 => 3.5/7.5/11.5/19.5/35.5/55.5/71.5/239.5 cycles
		
	// set External trigger event to SWSTART
	ADC1->CTLR2 = ADC1->CTLR2 & ~ADC_EXTSEL_SWSTART;

	// should be ready for SW conversion now
}

/*
 * start conversion, wait and return result
 */
uint16_t adc_get( void )
{
	// start sw conversion (auto clears)
	ADC1->CTLR2 |= ADC_FLAG_STRT;
	
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
	uint32_t count = 0;
	
	SystemInit();

	printf("\r\r\n\nadc_polled example\n\r");

	// init systick @ 1ms rate
	printf("initializing adc...");
	adc_init();
	printf("done.\n\r");
	
	// Enable GPIOs
	RCC->APB2PCENR |= RCC_APB2Periph_GPIOD;

	// GPIO D0 Push-Pull
	GPIOD->CFGLR &= ~(0xf<<(4*0));
	GPIOD->CFGLR |= (GPIO_Speed_10MHz | GPIO_CNF_OUT_PP)<<(4*0);

	printf("looping...\n\r");
	while(1)
	{
		GPIOD->BSHR = 1;	 // Turn on GPIOs
		Delay_Ms( 100 );
		GPIOD->BSHR = (1<<16); // Turn off GPIODs
		printf( "Count: %lu adc: %d\n\r", count++, adc_get() );
		Delay_Ms( 100 );
	}
}

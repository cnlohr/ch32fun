#include "ch32fun.h"

// 4096 = max
// <1024 = pressed
// -> 2560 = threshold
#define THRESHOLD 2560

// Captive keys (left to right):
// A9 (PB1)
// A8 (PB0)
// A7 (PA7)
// A5 (PA5)
#define KEY_L1 9
#define KEY_L2 8
#define KEY_R2 7
#define KEY_R1 5

// Our clock runs at 8*18=144MHz
// PB1 CLK is 72MHz, PB2 is 144MHz
uint16_t sample_touch( const uint8_t key )
{
	// Select converted channel
	ADC1->RSQR3 = key;

	TKey1->IDATAR1 = 0x10; // CHGOFFSET
	TKey1->RDATAR = 0x8; // ACT_DCG (How long delay to sample)
	while ( !( ADC1->STATR & ADC_FLAG_EOC ) );

	return TKey1->RDATAR;
}

int main()
{
	SystemInit();

	// We dont use funGpioInitAll() as we also want to init ADC1
	RCC->APB2PCENR |= ( RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD |
						RCC_APB2Periph_ADC1 );
	RCC->CFGR0 |= ( 0b11 << 14 ); // Max 14MHz. PCLK2 is 144Mhz by default so /8

	// PA4 is left led, PB11 is right
	funPinMode( PA4, GPIO_Speed_10MHz | GPIO_CNF_OUT_PP );
	funPinMode( PB11, GPIO_Speed_10MHz | GPIO_CNF_OUT_PP );

	// Tkeys
	funPinMode( PB1, GPIO_CFGLR_IN_ANALOG );
	funPinMode( PB0, GPIO_CFGLR_IN_ANALOG );
	funPinMode( PA5, GPIO_CFGLR_IN_ANALOG );
	funPinMode( PA7, GPIO_CFGLR_IN_ANALOG );

	// Reset ADC
	RCC->APB2PRSTR |= RCC_APB2Periph_ADC1;
	RCC->APB2PRSTR &= ~RCC_APB2Periph_ADC1;

	// Configure sampling times for channels
	TKey1->SAMPTR2 = ( ADC_SampleTime_7Cycles5 << ( 3 * KEY_L1 ) ) | ( ADC_SampleTime_7Cycles5 << ( 3 * KEY_L2 ) ) |
	                 ( ADC_SampleTime_7Cycles5 << ( 3 * KEY_R2 ) ) | ( ADC_SampleTime_7Cycles5 << ( 3 * KEY_R1 ) );

	// Always one channel at a time
	ADC1->RSQR1 = ( 0 << 20 );
	ADC1->RSQR2 = 0;
	ADC1->CTLR2 |= ADC_ADON;

	// The ADC calibration isn't really needed
	// But we include it just in case

	// Reset calibration
	ADC1->CTLR2 |= ADC_RSTCAL;
	while ( ADC1->CTLR2 & ADC_RSTCAL );

	// Calibrate
	ADC1->CTLR2 |= ADC_CAL;
	while ( ADC1->CTLR2 & ADC_CAL );

	TKey1->CTLR1 |= ADC_BUFEN | ( 1 << 24 ); // Enable TKey
	while ( 1 )
	{
		// Left pressed
		if ( sample_touch( KEY_L1 ) < THRESHOLD )
		{
			funDigitalWrite( PB11, FUN_LOW );
		}
		if ( sample_touch( KEY_L2 ) < THRESHOLD )
		{
			funDigitalWrite( PB11, FUN_HIGH );
		}

		// Right pressed
		if ( sample_touch( KEY_R1 ) < THRESHOLD )
		{
			funDigitalWrite( PA4, FUN_LOW );
		}
		if ( sample_touch( KEY_R2 ) < THRESHOLD )
		{
			funDigitalWrite( PA4, FUN_HIGH );
		}

		Delay_Ms( 10 );
	}
}

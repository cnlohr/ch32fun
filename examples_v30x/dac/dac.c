#include "ch32fun.h"
#include <stdio.h>
#include "wave.h"

// This example shows 3 basic DAC modes:
// 1. Custom waveform – DMA + Timer  
// Generates user-defined waves (32 samples) with zero CPU load.

// 2. Triangle wave – Hardware mode  
// Built-in linear ramp generator. Requires timer.

// 3. Software trigger – Manual mode  
// Single-value conversion on demand.

void DAC_SoftwareTrigger_Init()
{
	// Enable DAC
	RCC->APB1PCENR |= RCC_APB1Periph_DAC;

	// Reset DAC registers
	RCC->APB1PRSTR |= RCC_APB1Periph_DAC;
	RCC->APB1PRSTR &=~RCC_APB1Periph_DAC;

	// Enable software trigger for both channels
	DAC->CTLR = DAC_SWTRIG1 | DAC_SWTRIG2;

	// Configure IO (recommendation from RM)
	funGpioInitA();
	funPinMode( PA4, GPIO_CFGLR_IN_ANALOG ); // ch1
	funPinMode( PA5, GPIO_CFGLR_IN_ANALOG ); // ch2

	// Enable both channels
	DAC->CTLR |= DAC_EN1 | DAC_EN2;
	
	// Enable DMA
	DAC->CTLR |= DAC_DMAEN1 | DAC_DMAEN2;	
}

void DAC_CustomWave_Init()
{
	// Enable DAC
	RCC->APB1PCENR |= RCC_APB1Periph_DAC;

	// Reset DAC registers
	RCC->APB1PRSTR |= RCC_APB1Periph_DAC;
	RCC->APB1PRSTR &=~RCC_APB1Periph_DAC;

	// Set TIM2 TRGO event to generate DMA requenst
	DAC->CTLR |= DAC_TSEL1_2 | DAC_TEN1 |
				 DAC_TSEL2_2 | DAC_TEN2;

	// Configure IO (recommendation from RM)
	funGpioInitA();
	funPinMode( PA4, GPIO_CFGLR_IN_ANALOG ); // ch1
	funPinMode( PA5, GPIO_CFGLR_IN_ANALOG ); // ch2

	// Enable both channels
	DAC->CTLR |= DAC_EN1 | DAC_EN2;
	
	// Enable DMA
	DAC->CTLR |= DAC_DMAEN1 | DAC_DMAEN2;	
}

void DAC_Triangle_Init()
{
	// Enable DAC
	RCC->APB1PCENR |= RCC_APB1Periph_DAC;

	// Reset DAC registers
	RCC->APB1PRSTR |= RCC_APB1Periph_DAC;
	RCC->APB1PRSTR &=~RCC_APB1Periph_DAC;

	// Set TIM2 TRGO event to generate DMA requenst
	DAC->CTLR |= DAC_TSEL1_2 | DAC_TEN1 |
				 DAC_TSEL2_2 | DAC_TEN2 |
				 DAC_MAMP1 | // max triangle amplitude for channel1
				 DAC_MAMP2_3 | DAC_MAMP2_1 | // triangle amplitude 2047 for channel 2
				 DAC_WAVE1 | DAC_WAVE2; // set triangular wave to be generated
	
	// Configure IO (recommendation from RM)
	funGpioInitA();
	funPinMode( PA4, GPIO_CFGLR_IN_ANALOG ); // ch1
	funPinMode( PA5, GPIO_CFGLR_IN_ANALOG ); // ch2

	// Enable both channels
	DAC->CTLR |= DAC_EN1 | DAC_EN2;
}

void DMA_Init()
{
	// Enable DMA
	RCC->AHBPCENR |= RCC_AHBPeriph_DMA2;

	// Reset DMA registers
	RCC->AHBRSTR |= RCC_AHBPeriph_DMA2;
	RCC->AHBRSTR &=~RCC_AHBPeriph_DMA2;
	
	DMA2_Channel3->CNTR = DAC_BUF_SIZE;
	
	DMA2_Channel3->MADDR = (uint32_t) &dac_buf[0];

	// Dual channel righ alligned 12-bit data
	DMA2_Channel3->PADDR = (uint32_t) &DAC->RD12BDHR;
	
	DMA2_Channel3->CFGR = 
		DMA_CFGR1_DIR |					// MEM2PERIPHERAL
		DMA_CFGR1_PL |					// High priority.
		DMA_MemoryDataSize_Word |		// 32-bit memory
		DMA_PeripheralDataSize_Word |	// 32-bit peripheral
		DMA_CFGR1_MINC |				// Increase memory.
		DMA_CFGR1_CIRC |				// Circular mode.
		0 |								// Half-trigger
		0 |								// Whole-trigger
		DMA_CFGR1_EN;					// Enable
}

void TIM2_Init()
{
	// Enable TIM2
	RCC->APB1PCENR |= RCC_APB1Periph_TIM2;

	// Reset TIM2 registers
	RCC->APB1PRSTR |= RCC_APB1Periph_TIM2;
	RCC->APB1PRSTR &=~RCC_APB1Periph_TIM2;

	// Prescaler so 1 tick = 10 us
	TIM2->PSC = FUNCONF_SYSTEM_CORE_CLOCK / 100000 - 1;
	
	// The goal is to generate a waves with 300ms period.
	// 0.3 / DAC_BUF_SIZE / 10e-6 = 14.6 ticks
	TIM2->ATRLR = 15-1;
	
	// Reload immediately
	TIM2->SWEVGR |= TIM_UG;

	// Enable TRIGO signal on counter update
	TIM2->CTLR2 |= TIM_TRGOSource_Update;

	// Start counting
	TIM2->CTLR1 |= TIM_CEN;
}

int main()
{
	SystemInit();

	DMA_Init();
	TIM2_Init();
	while (1)
	{
		DAC_CustomWave_Init();
		Delay_Ms(300);
		
		DAC_Triangle_Init();
		Delay_Ms(300);
		
		// for software trigger to work you just write desired output value into hold register
		DAC_SoftwareTrigger_Init();
		DAC->R12BDHR1 = 0; //ch1
		DAC->R12BDHR2 = 2047; // ch2
		Delay_Ms(100);
		DAC->R12BDHR1 = 1023; //ch1
		DAC->R12BDHR2 = 4095; // ch2
		Delay_Ms(100);
		DAC->R12BDHR1 = 2047; //ch1
		DAC->R12BDHR2 = 0; // ch2
		Delay_Ms(100);
	}
}
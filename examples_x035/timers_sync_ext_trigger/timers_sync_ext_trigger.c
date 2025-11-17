// Simple example showing how to synchronize two timers (TIM1 and TIM2) using
// an external trigger on TIM2 Channel2 (PA1).
// wire a button Or a single wire to PA1 and trigger it to GND to start the timers.

#include "ch32fun.h"
#include <stdio.h>

void timers_sync_init() {
	// Enable Timers clocks
	RCC->APB1PCENR |= RCC_APB1Periph_TIM2;
	RCC->APB2PCENR |= RCC_APB2Periph_TIM1;

	TIM2->CTLR1 &= ~(TIM_DIR);
	TIM2->CTLR1 |= TIM_CounterMode_Up;
	TIM2->PSC = 48000-1;		// 1KHz frequency
	TIM2->ATRLR = 0xFFFF;		// Max period
	TIM2->SWEVGR = TIM_UG;
	
	//# STEP 1: Configure TI2 as external trigger
	// CompareCaptureControlReg1: select CompareCapture Channel 1 as input
	TIM2->CHCTLR1 &= ~(TIM_CC2S);
	TIM2->CHCTLR1 |= TIM_ICSelection_DirectTI << 8;
	// CompareCaptureEnableReg: use CompareCapture Channel 1 for capture on falling edge
	TIM2->CCER &= ~(TIM_CC2P);
	TIM2->CCER |= (TIM_ICPolarity_Falling << 4) | TIM_CC2E;

	TIM2->SMCFGR &= ~(TIM_MSM | TIM_SMS | TIM_TS);
	TIM2->SMCFGR |= TIM_MasterSlaveMode_Enable | TIM_SlaveMode_Trigger | TIM_TS_TI2FP2;
	TIM2->CTLR2 &= ~(TIM_MMS);
	TIM2->CTLR2 |= TIM_TRGOSource_Enable;

	//# Configure TIM1 as Slave triggered by TIM2
	TIM1->SMCFGR &= ~(TIM_SMS | TIM_TS);
	TIM1->SMCFGR |= TIM_SlaveMode_Trigger | TIM_TS_ITR1;
}

int main() {
	SystemInit();
	funGpioInitAll(); // Enable GPIOs

	printf("\n~ ADC Group Injection Example ~\n");
	printf("Chip ID: %08lX\n", ESIG->UID0);
	printf("Chip Capacity: %d KB\n", ESIG->CAP);

	// TIM2 CH2 input
	funPinMode(PA1, GPIO_CFGLR_IN_PUPD);
	funDigitalWrite(PA1, 1); // Pull-up
	timers_sync_init();

	while(1) {
		printf("TIM1 counter: %d, TIM2 counter: %d\n", TIM1->CNT, TIM2->CNT);
		Delay_Ms(1000);
	}
}

// Simple example of Timer2 Slave modes
// This example uses Timer2 Channel2 (PA1) as external trigger input
// You can connect PA1 to a button (1 side to PA1, other side to GND)
// OR just run a wire to PA1 and short it to GND and release to trigger an event

// Reset Mode: Counter resets on trigger event
// ... 8, 9, 10, (trigger low), (trigger release), 0, 1 , 2, 3 ...

// Gated Mode: Counter counts only while trigger is high
// ... 8, 9, 10, (trigger low) 11, 12, 13, (trigger release) 13, 13, 13 ...

// Trigger Mode: Counter starts on trigger event, and it keep counting up regardless of trigger state
// 0, 0, 0, ... 0, 0, 0, (trigger low), (trigger release), 0, 1 , 2, 3 ...

#include "ch32fun.h"
#include <stdio.h>

#define TIM_SLAVE_MODE TIM_SlaveMode_Reset
// #define TIM_SLAVE_MODE TIM_SlaveMode_Gated
// #define TIM_SLAVE_MODE TIM_SlaveMode_Trigger

void tim2_external_trigger_init(u16 slave_mode) {
	// Enable TIM2 clock
	RCC->APB1PCENR |= RCC_APB1Periph_TIM2;

	TIM2->CTLR1 &= ~(TIM_DIR);
	TIM2->CTLR1 |= TIM_CounterMode_Up;
	TIM2->PSC = 48000-1;		// 1KHz frequency
	TIM2->ATRLR = 0xFFFF;		// Max period
	TIM2->SWEVGR = TIM_UG;

	//# STEP 1: Configure TI2 as external trigger
	// CompareCaptureControlReg1: select CompareCapture Channel 1 as input
	TIM2->CHCTLR1 &= ~(TIM_CC1S);
	TIM2->CHCTLR1 |= TIM_ICSelection_DirectTI << 8;
	// CompareCaptureEnableReg: use CompareCapture Channel 1 for capture on falling edge
	TIM2->CCER &= ~(TIM_CC1P);
	TIM2->CCER |= (TIM_ICPolarity_Falling << 4) | TIM_CC1E;

	//# STEP 2: SlaveModeConfigReg: slave mode for Filtered Timer Input 2
	TIM2->SMCFGR &= ~(TIM_SMS | TIM_MSM | TIM_TS);
	TIM2->SMCFGR |= slave_mode | TIM_MasterSlaveMode_Enable | TIM_TS_TI2FP2;

	//# STEP 3: start the counter except in Trigger mode
	if (slave_mode != TIM_SlaveMode_Trigger) {
		TIM2->CTLR1 |= TIM_CEN;
	}
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
	tim2_external_trigger_init(TIM_SLAVE_MODE);

	while(1) {
		printf("counter: %d\n", TIM2->CNT);
		Delay_Ms(1000);
	}
}

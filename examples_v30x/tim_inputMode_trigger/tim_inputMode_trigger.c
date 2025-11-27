// Simple example of Timer1 Slave modes
// This example uses Timer1 Channel1 (PA8) as external trigger input
// You can connect PA8 to a button (other side to GND)
// OR just run a wire to PA8 and short it to GND and release to trigger an event

// Reset Mode: Counter resets on trigger event
// ... 8, 9, 10, (trigger low), (trigger release), 0, 1 , 2, 3 ...

// Gated Mode: Counter counts only while trigger is high
// ... 8, 9, 10, (trigger low) 11, 12, 13, (trigger release) 13, 13, 13 ...

// Trigger Mode: Counter starts on trigger event, and it keep counting up regardless of trigger state
// 0, 0, 0, ... 0, 0, 0, (trigger low), (trigger release), 0, 1 , 2, 3 ...

#include "ch32fun.h"
#include <stdio.h>

// #define TIM_SLAVE_MODE TIM_SlaveMode_Reset
// #define TIM_SLAVE_MODE TIM_SlaveMode_Gated
#define TIM_SLAVE_MODE TIM_SlaveMode_Trigger

void TIM_init(TIM_TypeDef * TIMx, u16 prescaler, u16 period) {
	// Enable TIMER clock
	if (TIMx == TIM1) {
		RCC->APB2PCENR |= RCC_APB2Periph_TIM1;
	}
	#ifdef TIM2
	else if (TIMx == TIM2) {
		RCC->APB1PCENR |= RCC_APB1Periph_TIM2;
	}
	#endif

	TIMx->CTLR1 = (TIMx->CTLR1 & ~TIM_DIR) | TIM_CounterMode_Up;
	TIMx->PSC = prescaler-1;
	TIMx->ATRLR = period;
	TIMx->SWEVGR = TIM_UG;
}

void TIM_external_trigger_init(TIM_TypeDef * TIMx, u16 slave_mode) {
	// CompareCaptureControlReg1: select CompareCapture Channel 1 as input
	TIMx->CHCTLR1 &= ~(TIM_CC1S);
	TIMx->CHCTLR1 |= TIM_ICSelection_DirectTI;
	// CompareCaptureEnableReg: polarity use CompareCapture Channel 1 for capture on falling edge
	TIM1->CCER &= ~(TIM_CC1P);
	TIMx->CCER |= (TIM_ICPolarity_Falling) | TIM_CC1E;

	//# configure TIMER for master/slave mode
	// use TI1FP1 (filtered TI1 signal) as trigger source
	TIMx->SMCFGR &= ~(TIM_SMS | TIM_MSM | TIM_TS);
	TIMx->SMCFGR |= slave_mode | TIM_MasterSlaveMode_Enable | TIM_TS_TI1FP1;

	//# STEP 3: start the counter except in Trigger mode
	if (slave_mode != TIM_SlaveMode_Trigger) {
		TIMx->CTLR1 |= TIM_CEN;
	}
}

int main() {
	SystemInit();
	funGpioInitAll(); // Enable GPIOs

	printf("\n~ ADC Group Injection Example ~\n");
	printf("Chip ID: %08lX\n", ESIG->UID0);
	printf("Chip Capacity: %d KB\n", ESIG->CAP);

	// TIM2 CH2 input
	funPinMode(PA8, GPIO_CFGLR_IN_PUPD);
	funDigitalWrite(PA8, 1); // Pull-up

	TIM_init(TIM1, 48000, 0xFFFF);
	TIM_external_trigger_init(TIM1, TIM_SLAVE_MODE);

	while(1) {
		printf("TIM1: %d\n", TIM1->CNT);
		Delay_Ms(1000);
	}
}

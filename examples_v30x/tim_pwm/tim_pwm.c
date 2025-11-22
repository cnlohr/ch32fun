// Simple example that show how to use the TIM to generate a PWM signal
// connect an LED to PA8 (TIM1C1) to see the PWM signal output

#include "ch32fun.h"
#include <stdio.h>

#define PWM_PIN PA8
u16 period = 100;

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
	TIMx->ATRLR = period-1;
	TIMx->SWEVGR = TIM_UG;
}

void TIM_pwm_init(TIM_TypeDef * TIMx, u16 cycle_control) {
	// configure PWM mode
	TIMx->CHCTLR1 |= (TIMx->CHCTLR1 & ~TIM_OC1PE) | TIM_OCMode_PWM1;

	// configure output polarity
	TIMx->CCER = (TIMx->CCER & ~(TIM_CC1E | TIM_CC1P)) |
					TIM_OutputState_Enable | TIM_OCPolarity_High;
	TIMx->CH1CVR = cycle_control;

	TIMx->BDTR |= TIM_MOE;				// Main output enable
	TIMx->CTLR1 = TIM_ARPE;				// Auto-reload preload enable
	TIMx->CTLR1 |= TIM_CEN;				// Start timer
}

int main() {
	SystemInit();
	funGpioInitAll(); // Enable GPIOs

	printf("\n~ TIM PWM Example ~\n");
	printf("Chip ID: %08lX\n", ESIG->UID0);
	printf("Chip Capacity: %d KB\n", ESIG->CAP);

	funPinMode(PWM_PIN, GPIO_CFGLR_OUT_10Mhz_AF_PP);

	TIM_init(TIM1, 48000, period);
	
	// set duty cycle 50%
	TIM_pwm_init(TIM1, period / 2);

	while(1) {

	}
}

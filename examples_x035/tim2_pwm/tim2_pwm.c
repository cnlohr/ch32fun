// Simple example to show PWM output on TIM2 Channel 4 (PA3)
// Connect an LED to PA3 to see the PWM signal output

#include "ch32fun.h"
#include <stdio.h>

// T1C1: PB9
// T1C2: PB10
// T1C3: PB11
// T1C4: PC16/PC11

// T2C1: PA0
// T2C2: PA1
// T2C3: PA2
// T2C4: PA3

#define PWM_PIN PA3
u16 period = 1000;

void TIM2_ch4_pwm_init() {
	// Enable TIM2 clock
	RCC->APB1PCENR |= RCC_APB1Periph_TIM2;
	
	// Timer base configuration
	TIM2->PSC = 4 - 1;					// Prescaler
	TIM2->ATRLR = period - 1;			// PWM period
	TIM2->CH4CVR = period / 2;			// Duty cycle (50%)

	//! REQUIRED: Channel 4 PWM configuration
	TIM2->CHCTLR2 |= (0b110 << 12);		// OC4M = 110 (PWM Mode 1) - bits [14:12]
	TIM2->CHCTLR2 |= TIM_OC4PE;			// Channel 4 Preload enable - bit 11
	TIM2->CTLR1 = TIM_ARPE;				// Auto-reload preload enable

	// Enable Channel 4 output
	TIM2->CCER = (TIM2->CCER & ~(TIM_CC4E | TIM_CC4P)) | TIM_CC4E;
	
	// Main output enable and update
	TIM2->BDTR |= TIM_MOE;
	TIM2->SWEVGR |= TIM_UG;				// Update registers
	TIM2->CTLR1 |= TIM_CEN;				// Start timer
}

int main() {
	SystemInit();
	funGpioInitAll(); // Enable GPIOs

	printf("\n~ ADC Group Injection Example ~\n");
	printf("Chip ID: %08lX\n", ESIG->UID0);
	printf("Chip Capacity: %d KB\n", ESIG->CAP);

	// TIM2 CH4 output
	funPinMode(PWM_PIN, GPIO_CFGLR_OUT_10Mhz_AF_PP);
	TIM2_ch4_pwm_init();

	// fade blocking loop
	while(1) {
		// Fade from 0% to 100%
		for(int i = 0; i <= period; i += 10) {
			TIM2->CH4CVR = i;  // Increase brightness
			Delay_Ms(10);	  // 10ms delay between steps
		}
		
		// Fade from 100% to 0%
		for(int i = period; i >= 0; i -= 10) {
			TIM2->CH4CVR = i;  // Decrease brightness
			Delay_Ms(10);	  // 10ms delay between steps
		}
	}
}

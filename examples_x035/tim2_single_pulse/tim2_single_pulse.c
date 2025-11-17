// Simple example to show how to use Timer2 in One Pulse Mode
// You can use this to indicate a respond to a specific event by causing it to
// generate a pulse after a delay.

// Setup:
// Connect a led to T2 Ch1 (PA0) for PWM output
// Connect a button from T2 Ch2 (PA1) to GND for trigger input

// Behavior:
// When the button is pressed (PA1 goes LOW), T2 will wait for 900ms (off time)
// then generate a 100ms pulse on PA0 (T2 CH1 PWM output)
// you can adjust the off time by changing the CH1CVR value
// Or adjust the pulse time by changing the ATRLR value
// equation: pulse_time = period - off_time
// when CH1CVR == ATRLR, there will be no pulse output because period == off_time

#include "ch32fun.h"
#include <stdio.h>

// T2C1: PA0
// T2C2: PA1
// T2C3: PA2
// T2C4: PA3

#define PWM_PIN PA0			// T2 CH1
#define INPUT_PIN PA1		// T2 CH2

void TIM2_single_pulse_init() {
	//# Enable TIM2 clock
	RCC->APB1PCENR |= RCC_APB1Periph_TIM2;
	
	//# TIM2 basic setup
	TIM2->CTLR1 &= ~(TIM_DIR | TIM_OPM);
	TIM2->CTLR1 |= TIM_CounterMode_Up;
	TIM2->CTLR1 |= TIM_OPMode_Single;		//! REQUIRED One Pulse Mode
	TIM2->PSC = 48000-1;					// 1KHz frequency
	TIM2->ATRLR = 1000-1;					// PWM period
	
	//# CompareCaptureValue:
	TIM2->CH1CVR = 900;		// offtime = 900ms, pulse time = period - off time

	// 	1) Setting the CC2S field to 01b to map TI2FP2 to TI2; 
	// setting the CC2P bit to 0b to set TI2FP2 as rising edge detection; 
	// setting the TS field to 110b to set TI2FP2 as trigger source; setting the SMS 
	// field to 110b to set TI2FP2 to be used to start the counter. 

	// 2) Tdelay is determined by the value of the CompareCaptureReg, and Tpulse is
	// determined by the value of the Auto Reload Value Register and the CompareCaptureReg.

	//# SlaveModeConfigReg: Slave Trigger Mode, Configure TIM2 to be triggered by TI2 input
	TIM2->SMCFGR &= ~(TIM_SMS | TIM_TS);
	TIM2->SMCFGR |= TIM_SlaveMode_Trigger | TIM_TS_TI2FP2;

	//# CaptureCompareControlReg1: Compare Channel1 PWM output
	// Set 110b or 111b in the OCxM field to use PWM mode 1 or mode 2
	TIM2->CHCTLR1 &= ~(TIM_OC1M);
	TIM2->CHCTLR1 |= TIM_OCMode_PWM2;			// bit [6:4] PWM Mode
	TIM2->CCER &= ~(TIM_CC1P | TIM_CC1E);		// Clear CC1P and CC1E
	TIM2->CCER |= TIM_CC1E;						// Enable CampareCapture CH1 output

	//# CaptureCompareControlReg1: Capture Channel2 as input
	TIM2->CHCTLR1 &= ~(TIM_CC2S);							// Clear CC2S bits [9:8]
	TIM2->CHCTLR1 |= (TIM_ICSelection_DirectTI << 8);		// CC2S = 01 (configured as input on TI2)
	TIM2->CCER &= ~(TIM_CC2P | TIM_CC2E);					// Clear CC2E and CC2P
	TIM2->CCER |= (TIM_ICPolarity_Falling << 4) | TIM_CC2E;

	TIM2->BDTR |= TIM_MOE;
	TIM2->SWEVGR = TIM_UG;
}

int main() {
	SystemInit();
	funGpioInitAll(); // Enable GPIOs

	printf("\n~ ADC Single Pulse Example ~\n");
	printf("Chip ID: %08lX\n", ESIG->UID0);
	printf("Chip Capacity: %d KB\n", ESIG->CAP);

	// TIM2 CH1 output
	funPinMode(PWM_PIN, GPIO_CFGLR_OUT_10Mhz_AF_PP);
	funPinMode(INPUT_PIN, GPIO_CFGLR_IN_PUPD);
	funDigitalWrite(INPUT_PIN, 1); //! REQUIRED Pull-up
	TIM2_single_pulse_init();

	while(1) {
		// Delay_Ms(1000);
	}
}

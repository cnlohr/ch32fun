//# TODO: Figure out why it consumes 12mA in shutdown mode.

#include "ch32fun.h"
#include <stdio.h>
#include "debug_utilities.h"

// use PIN_LED to indicate wake up
#define PIN_LED PA9

void funSetPullups(GPIO_TypeDef* GPIOx) {
	// Configure all pins 0-23 as input pull-up
	// Mode [0,0] = input, CNF [1,0] = pull-up/pulldown mode
	GPIOx->CFGLR = 0x88888888;  // Pins 0-7: input pull-up/pull-down
	GPIOx->CFGHR = 0x88888888;  // Pins 8-15: input pull-up/pull-down  
	GPIOx->CFGXR = 0x88888888;  // Pins 16-23: input pull-up/pull-down
	
	// Set bits 0-23 to 1 for pull-up
	GPIOx->OUTDR = 0x00FFFFFF;
}

void interupt_setup(
	uint32_t EXTI_Line, EXTIMode_TypeDef mode, EXTITrigger_TypeDef trigger
) {
	// Clear existing configuration
	EXTI->INTENR &= ~EXTI_Line;
	EXTI->EVENR &= ~EXTI_Line;
	EXTI->RTENR &= ~EXTI_Line;
	EXTI->FTENR &= ~EXTI_Line;

	// Set mode (interrupt or event)
	if(mode == EXTI_Mode_Interrupt) {
		EXTI->INTENR |= EXTI_Line;
	} else {
		EXTI->EVENR |= EXTI_Line;
	}

	// Set trigger (rising, falling, or both)
	if(trigger == EXTI_Trigger_Rising_Falling) {
		EXTI->RTENR |= EXTI_Line;
		EXTI->FTENR |= EXTI_Line;
	} else if(trigger == EXTI_Trigger_Rising) {
		EXTI->RTENR |= EXTI_Line;
	} else {
		EXTI->FTENR |= EXTI_Line;
	}
}

void enter_sleep() {
	funGpioInitAll(); // Enable GPIOs

	funPinMode(PIN_LED, GPIO_CFGLR_OUT_10Mhz_PP); 	// Set PIN_LED to output
	funDigitalWrite( PIN_LED, 1 ); 					// Turn on PIN_LED
	Delay_Ms( 50 );

	funSetPullups(GPIOA);
	funSetPullups(GPIOB);
	funSetPullups(GPIOC);

	RCC->APB1PCENR |= RCC_APB1Periph_PWR;

	interupt_setup(EXTI_Line27, EXTI_Mode_Interrupt, EXTI_Trigger_Rising);
	NVIC_EnableIRQ(AWU_IRQn);

	u32 prescaler = AWU_Prescaler_10240;
	AWU->PSC |= prescaler;

	// 20/(48M/1024/10240) = 4.37sec
	u8 wakeupWindow = 10;
	AWU->WR = (AWU->WR & ~0x3f) | (wakeupWindow & 0x3f);

	// Autowake enabled
	AWU->CSR |= (1 << 1);

	// select standby on power-down
	// PWR->CTLR |= PWR_CTLR_PDDS;
	PWR->CTLR &= ~PWR_CTLR_PDDS;  // Stop mode

	// peripheral interrupt controller send to deep sleep
	NVIC->SCTLR |= (1 << 2);

	__WFI();
	
}

void AWU_IRQHandler(void) __attribute((interrupt));
void AWU_IRQHandler() {
	// Interrupt Line27 is required to wakeup the MCU from shutdown mode
	// handle interrupt: write to clear the flag and wait for another
	EXTI->INTFR = EXTI_Line27;
}

// ref: CH32x035RM Chapter 4 page 20
int main() {
	SystemInit();
	Delay_Ms(4000);

	printf("entering sleep loop\n");

	while(1) {
		enter_sleep();
	}
}

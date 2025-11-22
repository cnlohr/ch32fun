// Simple example showing how to use the low power stop mode
// and how to use the EXTI to wake up the system
// The current consumption while sleep is around 50uA

// Setup:
// PA3 - WAKE Pin (connect to a button that goes low when pressed)
// PB10 - LED indicator

// Program behavior:
// 1. 5 second delay to allow reprogramming
// 2. Set all GPIOs to input pull up
// 3. Enter stop mode
// 4. When the WAKE pin PA3 is pulled low, wake up and blink LED
// 5. Go back to sleep

#include "ch32fun.h"
#include <stdio.h>

#define WAKE_EXTI_LINE EXTI_Line3

#define WAKE_PIN PA3
#define LED_PIN PB10

void blink_led(int time) {
	for (int i = 0; i < time; i++) {
		funDigitalWrite(LED_PIN, 1);
		Delay_Ms( 50 );
		funDigitalWrite(LED_PIN, 0);
		Delay_Ms( 50 );
	}
}

int main() {
	SystemInit();
	Delay_Ms(100);
	funGpioInitA();
	funGpioInitB();

	printf("\n~ Low Power Stop Mode Test ~\n");
	//! WARNING: need 5 seconds to allow reprogramming on power up
	Delay_Ms(5000);

	// Set all GPIOs to input pull up to reduce power consumption
	GPIOA->CFGLR = 0x88888888;
	GPIOB->CFGLR = 0x88888888;
	GPIOC->CFGLR = 0x88888888;
	GPIOD->CFGLR = 0x88888888;
	GPIOE->CFGLR = 0x88888888;

	GPIOA->CFGHR = 0x88888888;
	GPIOB->CFGHR = 0x88888888;
	GPIOC->CFGHR = 0x88888888;
	GPIOD->CFGHR = 0x88888888;
	GPIOE->CFGHR = 0x88888888;

	funDigitalWrite(WAKE_PIN, 1);
	funPinMode(LED_PIN, GPIO_CFGLR_OUT_10Mhz_PP);

	// Configure WAKE EXTI on WAKE_PIN
	AFIO->EXTICR[0] &= ~AFIO_EXTICR1_EXTI3;
	AFIO->EXTICR[0] |= AFIO_EXTICR1_EXTI3_PA;

	// External interrupt trigger on falling edge
	EXTI->INTENR |= WAKE_EXTI_LINE;
	EXTI->FTENR |= WAKE_EXTI_LINE;

	NVIC_EnableIRQ(EXTI3_IRQn);

	// Enable PWR clock
	RCC->APB1PCENR |= RCC_APB1Periph_PWR;

	// Disable SRAM clock
	RCC->AHBPCENR &= ~RCC_SRAMEN;

	// Set voltage regulator in low power mode
	PWR->CTLR = PWR_CTLR_LPDS;

	// set RAMLV for RAM low voltage mode
	PWR->CTLR |=  (1 << 20);

	// Send to deepsleep
	NVIC->SCTLR |= (1 << 2);

	while(1) {
		blink_led(3);
		__WFE();
		printf("Woke up from Stop Mode!\n");
	}
}

__attribute__((interrupt)) void EXTI3_IRQHandler() {
	// Check if EXTI0 interrupt is pending
	if(EXTI->INTFR & WAKE_EXTI_LINE == 0) return;

	// Clear the interrupt flag (IMPORTANT!)
	EXTI->INTFR = WAKE_EXTI_LINE;
}

// this example show how to enter the sleep mode on the CH32V00x with the lowest power consumption
// and wake up on a timer event. The current conspumption is around 10uA.
// The sleep time is configured with AWUPSC and AWUWR
// t = AWUWR (Auto-wakeup Window Comparision Register) / (fLSI / AWUPSC (Auto-wakeup Prescaler Value))

#include "ch32fun.h"
#include <stdio.h>

int main()
{
	SystemInit();
	printf("\r\n\r\nlow power example\r\n\r\n");

	// This delay gives us some time to reprogram the device. 
	// Otherwise if the device enters standby mode we can't 
	// program it any more.
	Delay_Ms(5000);
	funGpioInitAll();

	// Set all GPIOs to input pull up to reduce the power consumption otherwise they may cause current leak
	// [0] Mode -> 0 for input, [1] RO, [3:2] Config -> 0b10 for pullup
	// so 0b1000 = 0x8 for Input Pullup
	GPIOA->CFGLR = 0x88888888;
	GPIOC->CFGLR = 0x88888888;
	GPIOD->CFGLR = 0x88888888;

	// enable power interface module clock
	RCC->APB1PCENR |= RCC_APB1Periph_PWR;

	// enable low speed oscillator (LSI)
	RCC->RSTSCKR |= RCC_LSION;
	while ((RCC->RSTSCKR & RCC_LSIRDY) == 0) {}

	// enable AutoWakeUp event
	EXTI->EVENR |= EXTI_Line9;
	EXTI->FTENR |= EXTI_Line9;

	// configure AWU prescaler
	PWR->AWUPSC |= PWR_AWU_Prescaler_4096;

	// configure AWU window comparison value
	PWR->AWUWR &= ~0x3f;
	PWR->AWUWR |= 63;

	// enable AWU
	PWR->AWUCSR |= (1 << 1);

	// select standby on power-down
	PWR->CTLR |= PWR_CTLR_PDDS;

	// peripheral interrupt controller send to deep sleep
	PFIC->SCTLR |= (1 << 2);

	uint16_t counter = 0;
	printf("entering sleep loop\r\n");

	for (;;) {
		__WFE();
		// restore clock to full speed
		SystemInit();
		printf("\r\nawake, %u\r\n", counter++);
	}
}

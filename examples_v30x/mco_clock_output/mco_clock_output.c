// copied from the mco_clock_output example in the ch32fun example
// This allows the device to output one of four internal
// clock signals to the PC8 pin.  It cycles through no signal and 
// each of the four different clock signals.  Each
// signal is output for five seconds before moving on to the next. 
// The serial output displays the signals as they are selected.

// The different signals are:
// 0) Nothing
// 1) SYSCLK (48MHz)
// 2) HSI (24MHz)
// 3) HSE (depends on external XTAL)
// 4) PLL clock output (48MHz)

#include "ch32fun.h"
#include <stdio.h>

int main() {
	SystemInit();
	Delay_Ms(100);
	printf("\n~ MCO Clock Test ~\n");

	funGpioInitAll();
	funPinMode(PA8, GPIO_CFGLR_OUT_50Mhz_AF_PP);

	// uncomment this for the HSE clock source if you have an external crystal
	// // turn the HSE on
	// RCC->CTLR |= RCC_HSE_ON;
	// // Wait till HSE is ready
	// while(!(RCC->CTLR & RCC_HSERDY));

	int count = 0;
	int regtemp;

	while(1) {
		regtemp = (RCC->CFGR0 & ~RCC_CFGR0_MCO);

		switch(count) {
			case 0:
				printf("\r\nNo signal on MCO\r\n");
				printf("CFGR0 going from %08lX to %08lX\r\n", RCC->CFGR0, regtemp);
				RCC->CFGR0 = regtemp;
				count++;
				break;
			case 1:
				printf("\r\nSYSCLK signal on MCO\r\n");
				regtemp |= RCC_CFGR0_MCO_SYSCLK;
				printf("CFGR0 going from %08lX to %08lX\r\n", RCC->CFGR0, regtemp);
				RCC->CFGR0 = regtemp;
				count++;
				break;
			case 2:
				printf("\r\nHSI signal on MCO\r\n");
				regtemp |= RCC_CFGR0_MCO_HSI;
				printf("CFGR0 going from %08lX to %08lX\r\n", RCC->CFGR0, regtemp);
				RCC->CFGR0 = regtemp;
				count++;
				break;
			case 3:
				printf("\r\nHSE signal on MCO\r\n");
				regtemp |= RCC_CFGR0_MCO_HSE;
				printf("CFGR0 going from %08lX to %08lX\r\n", RCC->CFGR0, regtemp);
				RCC->CFGR0 = regtemp;
				count++;
				break;
			case 4:
				printf("\r\nPLLCLK signal on MCO\r\n");
				regtemp |= RCC_CFGR0_MCO_PLL;
				printf("CFGR0 going from %08lX to %08lX\r\n", RCC->CFGR0, regtemp);
				RCC->CFGR0 = regtemp;
				count=0;
				break;
		}
		Delay_Ms(5000);
	}
}
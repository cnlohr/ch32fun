#include "ch32fun.h"
#include <stdio.h>
#include "rtc.h"

//#define RTC_CLOCK_SOURCE_LSI
#define RTC_CLOCK_SOURCE_LSE

void RTC_IRQHandler(void) __attribute__((interrupt));
void RTC_IRQHandler(void)
{
	// Clear flag
	RTC->CTLRL &=~ RTC_CTLRL_ALRF;

	printf("Hello, Sailor.\r\n");
}

int main()
{
	SystemInit();

	// Enable access to the RTC and backup registers
	RCC->APB1PCENR |= RCC_PWREN | RCC_BKPEN;
	PWR->CTLR |= PWR_CTLR_DBP;

	// this is needed to reset RTC on external manual reset (nRST pin) and first time flashing
	if ( !(RCC->RSTSCKR & RCC_PORRSTF) && (BKP->DATAR1 != 0xDEAD) || (RCC->RSTSCKR & RCC_PINRSTF) )
	{
		// Clear nRST pin reset flag
		RCC->RSTSCKR |= RCC_RMVF;
		// store flag in backup domain register
		BKP->DATAR1 = 0xDEAD;

		RTC_init();

		RTC_setAlarmRelative(10, 0);
		printf("RTC Alarm set for 10 seconds from now.\r\n");
	}
	
	while (1)
	{
		printf("%ld s\r\n", RTC_getCounter());

		Delay_Ms(1000);
	}
	
}
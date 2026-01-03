#include "ch32fun.h"
#include <stdio.h>
#include "rtc.h"

//#define RTC_CLOCK_SOURCE_LSI
#define RTC_CLOCK_SOURCE_LSE

void RTCAlarm_IRQHandler(void) __attribute__((interrupt));
void RTCAlarm_IRQHandler(void)
{
	// Clear flags "manually"
	RTC->CTLRL &=~ RTC_CTLRL_ALRF;
	NVIC_ClearPendingIRQ(RTCAlarm_IRQn);
	EXTI->INTFR = EXTI_Line17;

	printf("Hello, Sailor.\r\n");
	// Set new alarm for 5 seconds from now
	RTC_setAlarmRelative(5, 0);

}

int main()
{
	SystemInit();

	funGpioInitAll();
	
	// In almost all cases after a reset (standby, NRST, software reset), something about the RTC (LSI?) seems to need to be reinitialized
	// and I've not tracked it down yet.
	RTC_init();

	// Clear reset cause flags if desired
	RCC->RSTSCKR |= RCC_RMVF;

	if (BKP->DATAR1 == 0xDEAD) {
		printf("Unexpected reset!");
	}
	BKP->DATAR1 = 0xDEAD;

	RTC_setAlarmRelative(5, 0);
	
	while (1)
	{
		printf("%ld s\r\n", RTC_getCounter());
		Delay_Ms(1000);
	}
	
}

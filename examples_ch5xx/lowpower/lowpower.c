/*
 * Demomstration of low power Idle and Sleep modes.
 * The RTC is used to trigger an interrupt to exit those modes, which was entered with WFI
 */
#include "ch32fun.h"
#include <stdio.h>

#ifdef CH570_CH572
#define LED PA9
#else
#define LED PA8
#endif

#define LED_ON FUN_LOW
#define LED_OFF FUN_HIGH
#define SLEEPTIME_MS 1000

// RTC interrupt handler to wake up from sleep
__attribute__((interrupt))
void RTC_IRQHandler(void)
{
	// clear trigger flag
	R8_RTC_FLAG_CTRL =  RB_RTC_TRIG_CLR;
}

void allPinPullUp(void)
{
	R32_PA_DIR = 0; //Direction input
	R32_PA_PD_DRV = 0; //Disable pull-down
	R32_PA_PU = P_All; //Enable pull-up
#if PB
	R32_PB_DIR = 0; //Direction input
	R32_PB_PD_DRV = 0; //Disable pull-down
	R32_PB_PU = P_All; //Enable pull-up
#endif	
}

void blink_led(int n) {
	// this blink function uses LowPowerIdle instead of Delay_Ms
	for(int i = n-1; i >= 0; i--) {
		funDigitalWrite( LED, LED_ON );
		LowPowerIdle( MS_TO_RTC(33) );
		funDigitalWrite( LED, LED_OFF );
		if(i) LowPowerIdle( MS_TO_RTC(33) );
	}
}

int main()
{
	SystemInit();

	DCDCEnable(); // Enable the internal DCDC
	LSIEnable(); // Disable LSE, enable LSI
	RTCInit(); // Set the RTC counter to 0 and enable RTC Trigger
	SleepInit(); // Enable wakeup from sleep by RTC, and enable RTC IRQ

	allPinPullUp();
	
	funPinMode( LED, GPIO_CFGLR_OUT_2Mhz_PP );
	funDigitalWrite( LED, LED_OFF );
	blink_led(1);

	uint8_t i = 5;
	while(i--)
	{ 
		LowPowerIdle( MS_TO_RTC(SLEEPTIME_MS) );
		DCDCEnable(); // During low power mode, the DCDC is disabled so we need to enable it
		blink_led(2);
		
		LowPowerSleep( MS_TO_RTC(SLEEPTIME_MS), (RB_PWR_RAM2K) );
		DCDCEnable(); // During low power mode, the DCDC is disabled so we need to enable it
		blink_led(3);
	}

	while(1); //halt so that the user don't need to unbrick everytime.
}

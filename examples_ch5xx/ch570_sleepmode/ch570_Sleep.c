#include "ch32fun.h"
#include <stdio.h>

#define LED PA9

// Datasheet refer to them differently.
// SFR is still in early stage when this code is written.
// So if anything breaks or won't compile
// Try checking out ch5xxhw.h
#define R16_RTC_CNT_LSI     R16_RTC_CNT_32K 
#define R16_RTC_CNT_DIV1    R16_RTC_CNT_2S 
#define RB_PWR_RAM12K       RB_PWR_RAM2K

__attribute__((interrupt))
void RTC_IRQHandler(void)
{
	// clear trigger flag
	R8_RTC_FLAG_CTRL =  RB_RTC_TRIG_CLR;

}

//Enable pullup on all pins to reduce power consumption
void allPinPullUp(void)
{
	R32_PA_DIR = 0; //Direction input
	R32_PA_PD_DRV = 0; //Disable pull-down
    R32_PA_PU = 0xFFFFFFFF; //Enable pull-up  
    
}

//go to sleep mode with 12K RAM retention.
void goToSleep(void)
{
	
    PFIC->SCTLR |= (1 << 2); //deep sleep
	SYS_SAFE_ACCESS
	(
 	   R8_SLP_POWER_CTRL |= 0x40;
  	   R16_POWER_PLAN = RB_PWR_PLAN_EN | RB_PWR_CORE | RB_PWR_RAM12K | (1<<12);
    );
	
    __WFI();
    __NOP();
    __NOP();

}

//load all counter register with 0
void rtcClear(void)
{
	SYS_SAFE_ACCESS
	(
		R32_RTC_TRIG = 0;
		R32_RTC_CTRL |= RB_RTC_LOAD_HI;
		R32_RTC_CTRL |= RB_RTC_LOAD_LO;
    );

}

void rtcSleepWait(uint16_t seconds)
{	
	//get the rtc current time
	uint32_t alarm = (uint32_t) R16_RTC_CNT_LSI | ( (uint32_t) R16_RTC_CNT_DIV1 << 16 );
	alarm += seconds*32768; //Set trigger value assuming LSI is 32.768khz

    SYS_SAFE_ACCESS
    (
		R32_RTC_TRIG = alarm;                 //set trigger value.
		R8_RTC_MODE_CTRL |= RB_RTC_TRIG_EN;  //enable trigger
   		R8_SLP_WAKE_CTRL |= RB_SLP_RTC_WAKE; // enable wakeup control
    );

    NVIC_EnableIRQ(RTC_IRQn);//enable RTC interrupt
    goToSleep();

}

int main()
{
	SystemInit();
	allPinPullUp();
	R32_PA_DIR |= LED;//Enable output on LED pin.

   	rtcClear();//init RTC.
	
    uint16_t i = 5; 
    while(i--)
    {
    	rtcSleepWait(1); //Sleep for 1s
    	R32_PA_SET |= LED; //turn off LED
    	rtcSleepWait(1); //SLeep for 1s
    	R32_PA_CLR |= LED;  //turn on LED
    }

    while(1);
	
}


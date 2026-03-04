/*
 * Low Power Sleep and Idle library for ch5xx
 */
#include "ch32fun.h"
#include "rtc.h"

void LSIEnable() {
	SYS_SAFE_ACCESS(
#ifdef CH570_CH572
		R8_LSI_CONFIG |= RB_CLK_LSI_PON; //turn on LSI
#else
		R8_CK32K_CONFIG &= ~(RB_CLK_OSC32K_XT | RB_CLK_XT32K_PON); // turn off LSE
		R8_CK32K_CONFIG |= RB_CLK_INT32K_PON; // turn on LSI
#endif
	);
}

void DCDCEnable() {
#ifndef CH570_CH572 // CH570/2 has no DCDC.
	SYS_SAFE_ACCESS(
		R16_AUX_POWER_ADJ |= RB_DCDC_CHARGE;
		R16_POWER_PLAN |= RB_PWR_DCDC_PRE;
	);

	RTC_WAIT_TICKS(2);

	SYS_SAFE_ACCESS(
		R16_POWER_PLAN |= RB_PWR_DCDC_EN;
	);
#endif
}

void SleepInit() {
	SYS_SAFE_ACCESS(
		R8_RTC_MODE_CTRL |= RB_RTC_TRIG_EN;  //enable RTC trigger
   		R8_SLP_WAKE_CTRL |= RB_SLP_RTC_WAKE; // enable wakeup control
	);
	NVIC_EnableIRQ(RTC_IRQn);
}

// enter idle state
void LowPowerIdleWFI() {
	NVIC->SCTLR &= ~(1 << 2); // don't deep sleep
	__WFI();
}

void LowPowerIdle(uint32_t cyc) {
	RTC_setAlarm(cyc);
	LowPowerIdleWFI();
}

// This macro defines which power pin 
// to use. If not defined correctly, sleep current
// will be higher than expected.
#ifndef FUNCONF_POWERED_BY_V5PIN
#define FUNCONF_POWERED_BY_V5PIN 0
#endif

void LowPowerSleepWFI(uint16_t power_plan) {
#ifdef CH570_CH572
#if (FUNCONF_POWERED_BY_V5PIN == 1)
	power_plan |= RB_PWR_LDO5V_EN;
#endif
#if ( CLK_SOURCE_CH5XX == CLK_SOURCE_PLL_75MHz ) || ( CLK_SOURCE_CH5XX == CLK_SOURCE_PLL_100MHz )
	//if system clock is higher than 60Mhz, it need to be reduced before sleep.
	SYS_SAFE_ACCESS(
		R8_CLK_SYS_CFG = CLK_SOURCE_PLL_60MHz;
	);
#endif
#else // !570/2
	SYS_SAFE_ACCESS(
		R8_BAT_DET_CTRL = 0;
		R8_XT32K_TUNE = (R16_RTC_CNT_32K > 0x3fff) ? (R8_XT32K_TUNE & 0xfc) | 0x01 : R8_XT32K_TUNE;
		R8_XT32M_TUNE = (R8_XT32M_TUNE & 0xfc) | 0x03;
	);
#endif

	NVIC->SCTLR |= (1 << 2); //deep sleep
	SYS_SAFE_ACCESS(
		R8_SLP_POWER_CTRL |= RB_RAM_RET_LV;
		R16_POWER_PLAN = RB_PWR_PLAN_EN | RB_PWR_CORE | power_plan;
		R8_PLL_CONFIG |= (1 << 5);
	);
	
	__WFI();

#ifdef CH570_CH572
#if ( CLK_SOURCE_CH5XX == CLK_SOURCE_PLL_75MHz ) || ( CLK_SOURCE_CH5XX == CLK_SOURCE_PLL_100MHz )
	//machine delay for a while.
	uint16_t i = 400;
	do {
		asm volatile("nop");
	} while(i--);

	//get system clock back to normal
	SYS_SAFE_ACCESS(
		R8_CLK_SYS_CFG = CLK_SOURCE_CH5XX;
	);
#endif
#else
	SYS_SAFE_ACCESS(
		R16_POWER_PLAN &= ~RB_XT_PRE_EN;
		R8_PLL_CONFIG &= ~(1 << 5);
		R8_XT32M_TUNE = (R8_XT32M_TUNE & 0xfc) | 0x01;
	);
#endif
}

void LowPowerSleep(uint32_t cyc, uint16_t power_plan) {
	RTC_setAlarm(cyc);
	LowPowerSleepWFI(power_plan);
}

void LowPower(uint32_t time, uint16_t power_plan) {
	if( time > 500 ) {
		LowPowerSleep( time, power_plan );
	}
	else {
		LowPowerIdle( time );
	}
}

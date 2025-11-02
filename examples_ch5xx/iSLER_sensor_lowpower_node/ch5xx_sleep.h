#include "ch32fun.h"

RV_STATIC_INLINE void ch5xx_sleep_powerDown(u32 cyc, u16 power_plan) {
	RTCTrigger(cyc);

	SYS_SAFE_ACCESS (
		R8_BAT_DET_CTRL = 0;
		R8_XT32K_TUNE = (R16_RTC_CNT_32K > 0x3fff) ? (R8_XT32K_TUNE & 0xfc) | 0x01 : R8_XT32K_TUNE;
		R8_XT32M_TUNE = (R8_XT32M_TUNE & 0xfc) | 0x03;
	);

	//# deep sleep
	NVIC->SCTLR |= (1 << 2);

	SYS_SAFE_ACCESS (
		R8_SLP_POWER_CTRL |= RB_RAM_RET_LV;
		R16_POWER_PLAN = RB_PWR_PLAN_EN;
		// R16_POWER_PLAN = RB_PWR_PLAN_EN | power_plan;
	);

	//# wfi
	NVIC->SCTLR &= ~(1 << 3);
	asm volatile ("wfi\nnop\nnop" );

	SYS_SAFE_ACCESS(
		R8_RST_WDOG_CTRL |= RB_SOFTWARE_RESET;
	);
}

// RTC interrupt handler to wake up from sleep
__attribute__((interrupt))
void RTC_IRQHandler(void) {
	// clear trigger flag
	R8_RTC_FLAG_CTRL =  RB_RTC_TRIG_CLR;
}

void ch5xx_setClock(u8 clock_source) {
	SYS_SAFE_ACCESS (
		R8_PLL_CONFIG &= ~(1<<5);
		R16_CLK_SYS_CFG =  (clock_source & 0x1f);
		asm volatile( "nop\nnop\nnop\nnop" );
	);
}

void ch5xx_allPinsPullUp() {
	//# all pins pull-up
	R32_PA_DIR = 0; //Direction input
	R32_PA_PD_DRV = 0; //Disable pull-down
	R32_PA_PU = P_All; //Enable pull-up
	#if PB
		R32_PB_DIR = 0; //Direction input
		R32_PB_PD_DRV = 0; //Disable pull-down
		R32_PB_PU = P_All; //Enable pull-up
	#endif		
}

void ch5xx_sleep_rtc_init() {
	//# configure RTC
	SYS_SAFE_ACCESS (
		R32_RTC_TRIG = 0;
		R32_RTC_CTRL |= RB_RTC_LOAD_HI;
		R32_RTC_CTRL |= RB_RTC_LOAD_LO;
		R8_RTC_MODE_CTRL |= RB_RTC_TRIG_EN;  //enable RTC trigger
		R8_SLP_WAKE_CTRL |= RB_SLP_RTC_WAKE; // enable wakeup control
	);

	//# enable RTC interrupt
	NVIC->IENR[((uint32_t)(RTC_IRQn) >> 5)] = (1 << ((uint32_t)(RTC_IRQn) & 0x1F));
}

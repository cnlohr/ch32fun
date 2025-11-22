// MIT License
// Copyright (c) 2025 UniTheCat

#include "rtc_helper.h"

//! ####################################
//! CORE FUNCTION
//! ####################################

void RTC_init() {
	// reset pheripherals
	RCC->APB1PCENR |= RCC_APB1Periph_PWR | RCC_APB1Periph_BKP;

	// prepare to configure clock source
	RCC->BDCTLR |= RCC_BDRST;
	RCC->BDCTLR &=~ RCC_BDRST;

	// enable low speed oscillator (LSI)
	RCC->RSTSCKR |= RCC_LSION;
	while (!(RCC->RSTSCKR & RCC_LSIRDY));

	// backup domain enable
	PWR->CTLR |= PWR_CTLR_DBP;
	
	// Set clock source for RTC
	RCC->BDCTLR |= RCC_RTCSEL_LSI;
	RCC->BDCTLR |= RCC_RTCEN;

	// Reset alarm, second, and overflow flags
	RTC->CTLRL &= ~(RTC_CTLRL_ALRF | RTC_CTLRL_SECF | RTC_CTLRL_OWF);

	RTC_CONFIG_CHANGE(
		// Set prescaler value
		RTC->PSCRH = ((RTC_TICKS_PER_SECOND-1) & PRLH_MSB_MASK) >> 16;
		RTC->PSCRL = (RTC_TICKS_PER_SECOND-1) & RTC_LSB_MASK;
	);
}

void RTC_setDateTime(rtc_date_t date, rtc_time_t time) {
	u32 seconds = RTC_get_seconds(date.year, date.month, date.day,
                                    time.hr, time.min, time.sec);

	RTC_CONFIG_CHANGE(
		// Set counter value
		RTC->CNTH = seconds >> 16;
		RTC->CNTL = seconds & RTC_LSB_MASK;	
	);
}

uint32_t RTC_GetCounter(void) {
    uint16_t high, low;
    
    // Wait for registers to be synchronized
    while (!(RTC->CTLRL & RTC_FLAG_RSF));
    
    // Read counter values
    low = RTC->CNTL;
    high = RTC->CNTH;
    return ((uint32_t)high << 16) | low;
}
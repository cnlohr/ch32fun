// MIT License
// Copyright (c) 2025 UniTheCat
// This library supports two WCH hardware implementations for an RTC
// and distinguishes between them using compile-time flags.

#if defined(CH32V10x) 
#include "ch32v103hw.h"
#elif defined (CH32L103)
#include "ch32l103hw.h"
#elif defined (CH32V20x)
#include "ch32v20xhw.h"
#elif defined (CH32V30x)
#include "ch32v30xhw.h"
#elif defined (CH5xx)
#include "ch5xxhw.h"
#else
#error "Currently unsupported RTC"
#endif

#if !defined(RTC_CLOCK_SOURCE_LSI) && !defined(RTC_CLOCK_SOURCE_LSE)
#define RTC_CLOCK_SOURCE_LSI
#endif

#define RTC_CLOCK_TICKS_PER_SECOND 32768

#ifndef CH5xx // No custom RTC divider support on CH5xx
#ifndef RTC_CLOCK_TICKS_PER_INCREMENT
#define RTC_CLOCK_TICKS_PER_INCREMENT RTC_CLOCK_TICKS_PER_SECOND
#endif
#define RTC_INCREMENTS_PER_SECOND (RTC_CLOCK_TICKS_PER_SECOND / RTC_CLOCK_TICKS_PER_INCREMENT)
#endif // !CH5xx

#define SECONDS_PER_MINUTE 60
#define SECONDS_PER_HOUR 3600
#define SECONDS_PER_DAY 86400

#define IS_LEAP_YEAR(year) ((((year) % 4 == 0) && ((year) % 100 != 0)) || ((year) % 400 == 0))

// Array of days in each month (non-leap year)
const u8 DAYS_IN_MONTH[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};


typedef struct {
	u16 year;
	u8 month;
	u8 day;
} rtc_date_t;

typedef struct {
	u8 hr;
	u8 min;
	u8 sec;
	u16 ms;
} rtc_time_t;

typedef struct {
	rtc_date_t date;
	rtc_time_t time;
} rtc_datetime_t;

const rtc_date_t date_reference = {2020, 1, 1};

// calculate days of the current year. eg. 2020-02-05 is 36 day
u32 RTC_days_of_year(u16 year, u8 month, u8 day) {
	u32 day_of_year = 0;
	
	// Add days from January to month-1
	for (u8 m = 0; m < month - 1; m++) {
		day_of_year += DAYS_IN_MONTH[m];
	}
	
	// Add extra day for February (full month) if it's a leap year
	if (month > 2 && IS_LEAP_YEAR(year)) {
		day_of_year += 1;
	}
	
	// Add days in the current month
	day_of_year += day;

	return day_of_year;
}

u32 RTC_get_seconds(u16 year, u8 month, u8 day, u8 hr, u8 min, u8 sec) {
	//# Validate input
	if (month < 1 || month > 12 || day < 1 || day > 31 || 
		hr > 23 || min > 59 || sec > 59 || year < 1970) { return 0; }

	// calculate days of the current year, -1 for 0-based days
	u32 days = RTC_days_of_year(year, month, day) - 1;

	// add the days count excluding the current year
	// (start from epoch time 1970-01-01 00:00:00)
	for (int y=1970; y < year; y++) {
		days += IS_LEAP_YEAR(y) ? 366 : 365;
	}

	// calculate total seconds
	return days * SECONDS_PER_DAY + 
			hr * SECONDS_PER_HOUR +
			min * SECONDS_PER_MINUTE + sec;
}

rtc_time_t RTC_get_time(u32 total_seconds, u16 ms) {
	u32 minutes = total_seconds / 60;

	return (rtc_time_t) {
		.sec = total_seconds % 60,
		.min = minutes % 60,
		.hr = (minutes / 60) % 24,
		.ms = ms
	};
}

rtc_date_t RTC_get_date(u32 total_seconds) {
	rtc_date_t output = {
		.year = date_reference.year,
		.month = date_reference.month,
		.day = date_reference.day
	};

	// Days since epoch
	u32 days_remaining = total_seconds / SECONDS_PER_DAY;

	// Find the year
	while(1) {
		u16 days_in_year = IS_LEAP_YEAR(output.year) ? 366 : 365;
		if (days_remaining < days_in_year) break;
		days_remaining -= days_in_year;
		output.year++;
	}

	// find the month
	for (u8 m = 0; m < 12; m++) {
		u8 days_in_month = DAYS_IN_MONTH[m];
		if (m == 1 && IS_LEAP_YEAR(output.year)) days_in_month = 29;
		if (days_remaining < days_in_month) break;

		days_remaining -= days_in_month;
		output.month++;
	}

	// add 1 because days_remaining is 0-based
	output.day = days_remaining + 1;

	return output;	
}

void RTC_print_date(rtc_date_t date, char *delimiter) {
	printf("%04d", date.year);
	printf("%s%02d", delimiter, date.month);
	printf("%s%02d", delimiter, date.day);
}

void RTC_print_time(rtc_time_t time) {
	printf("%02d:%02d:%02d.%03d",
		time.hr, time.min, time.sec, time.ms);
}


//! ####################################
//! CORE FUNCTION
//! ####################################

// There are potential speed improvements to this function, such as not resetting the
// backup domain or the LSI/LSE clock if they are already enabled. However, after
// testing with standby resets, it either didn't work at all or stopped working
// after a few minutes. Tread carefully!
void RTC_init() {
#ifdef CH5xx
	RTCInit();
#else
	RCC->APB1PCENR |= RCC_APB1Periph_PWR | RCC_APB1Periph_BKP;

    // backup domain enable
	PWR->CTLR |= PWR_CTLR_DBP;

	// prepare to configure clock source
	RCC->BDCTLR |= RCC_BDRST;
	RCC->BDCTLR &=~ RCC_BDRST;

	#ifdef RTC_CLOCK_SOURCE_LSI
	// Takes 300us, even if already enabled
	RCC->RSTSCKR |= RCC_LSION;
	while ( !(RCC->RSTSCKR & RCC_LSIRDY) );

	// Set clock source for RTC
	RCC->BDCTLR &=~ RCC_RTCSEL;
	RCC->BDCTLR |= RCC_RTCSEL_LSI;
	#elif defined(RTC_CLOCK_SOURCE_LSE)
	// Enable LSE
	RCC->BDCTLR |= RCC_LSEON;
	uint16_t timeout = 0;
	while ( !(RCC->BDCTLR & RCC_LSERDY) ) {
		Delay_Ms(1);
		timeout++;
		if (timeout > 1000) {
			printf("Could not start LSE.\r\n");
			break;
		}
	}
	// Set clock source for RTC
	RCC->BDCTLR &=~ RCC_RTCSEL;
	RCC->BDCTLR |= RCC_RTCSEL_LSE;
	#else 
	#error "No RTC clock source defined"
	#endif

	RCC->BDCTLR |= RCC_RTCEN;

	// Wait until we can safely communicate with RTC registers
	RTC->CTLRL &= ~RTC_FLAG_RSF;
	while (!(RTC->CTLRL & RTC_FLAG_RSF));

	// Reset alarm, second, and overflow flags
	RTC->CTLRL &= ~(RTC_CTLRL_ALRF | RTC_CTLRL_SECF | RTC_CTLRL_OWF);
	
	RTC_CONFIG_CHANGE(
		// Set prescaler value
		RTC->PSCRH = ((RTC_CLOCK_TICKS_PER_INCREMENT-1) & PRLH_MSB_MASK) >> 16;
		RTC->PSCRL = (RTC_CLOCK_TICKS_PER_INCREMENT-1) & RTC_LSB_MASK;
	);
#endif	
}

#ifdef CH5xx
void RTC_getCounter(u16 *days, u16 *seconds_2s, u16 *ticks_32k) {
	u16 days_1, days_2, seconds_2s_1, seconds_2s_2, ticks;

	// properly deal with asynchronous read of multiple registers in
	// rollover case
	days_1 = R32_RTC_CNT_DAY;
	seconds_2s_1 = R16_RTC_CNT_2S;
	ticks = R16_RTC_CNT_32K;
	days_2 = R32_RTC_CNT_DAY;
	seconds_2s_2 = R16_RTC_CNT_2S;

	if (days_1 != days_2 || seconds_2s_1 != seconds_2s_2) {
		// Rollover occurred, re-read all values
		*days = R32_RTC_CNT_DAY;
		*seconds_2s = R16_RTC_CNT_2S;
		*ticks_32k = R16_RTC_CNT_32K;
	} else {
		// No rollover, use the captured values
		*days = days_1;
		*seconds_2s = seconds_2s_1;
		*ticks_32k = ticks;
	}
}

#else
uint32_t RTC_getCounter(void) {
	uint16_t high1, high2, low;
	uint32_t s;
   
	// Wait for registers to be synchronized
	RTC->CTLRL &= ~RTC_FLAG_RSF;
	while (!(RTC->CTLRL & RTC_FLAG_RSF));

	high1 = RTC->CNTH;
	low = RTC->CNTL;
	high2 = RTC->CNTH;

	// properly deal with asynchronous read of multiple registers in
	// rollover case
	if (high1 != high2) {
		s = (high2 << 16) | RTC->CNTL;
	} else {
		s = (high1 << 16) | low;
	}
	
	return s;
}
#endif

rtc_datetime_t RTC_getDateTime() {
#ifdef CH5xx
	u16 days, seconds_2s, ticks_32k;
	RTC_getCounter(&days, &seconds_2s, &ticks_32k);
	u32 total_seconds = (days * SECONDS_PER_DAY) + (seconds_2s * 2) + (ticks_32k / RTC_CLOCK_TICKS_PER_SECOND);
	u16 ms = ((ticks_32k % RTC_CLOCK_TICKS_PER_SECOND) * 1000UL) / RTC_CLOCK_TICKS_PER_SECOND;
#else
	u32 count = RTC_getCounter();
	u32 total_seconds = count / RTC_INCREMENTS_PER_SECOND;
	u16 ms = ((count % RTC_INCREMENTS_PER_SECOND) * 1000UL) / RTC_INCREMENTS_PER_SECOND;
#endif
	
	rtc_date_t date = RTC_get_date(total_seconds);
	rtc_time_t time = RTC_get_time(total_seconds, ms);
	return (rtc_datetime_t) {date, time};
}

#ifdef CH5xx
void RTC_setDayCounter(u32 day_counter) {
	SYS_SAFE_ACCESS(
		R32_RTC_TRIG = day_counter - 1;
		R8_RTC_MODE_CTRL |= RB_RTC_LOAD_HI;
	);
}

void RTC_setTimeCounter(u32 time_counter) {
	SYS_SAFE_ACCESS(
		R32_RTC_TRIG = time_counter;
		R8_RTC_MODE_CTRL |= RB_RTC_LOAD_LO;
	);
}
#endif // CH5xx

void RTC_setDateTime(rtc_datetime_t *datetime) {
	// Clip year
	u16 year_min = date_reference.year;
#ifdef CH5xx
	u16 year_max = date_reference.year + 0x3FFF / 365; // 14 bits = ~44 years
#else
	u16 year_max = date_reference.year + 0xFFFFFFFF / 60 / 60 / 24 / 365 / RTC_INCREMENTS_PER_SECOND;
#endif

	if (datetime->date.year < year_min) datetime->date.year = year_min;
	if (datetime->date.year > year_max) datetime->date.year = year_max;

#ifdef CH5xx
	u32 days_counter = 0;
	// Add days for complete years from year_min to year-1
	for (u16 y = year_min; y < datetime->date.year; y++) {
		days_counter += IS_LEAP_YEAR(y) ? 366 : 365;
	}

	// Add days in the current year
	days_counter += RTC_days_of_year(datetime->date.year, datetime->date.month, datetime->date.day);
	RTC_setDayCounter(days_counter);

	u16 seconds = datetime->time.hr * 3600 + datetime->time.min * 60 + datetime->time.sec;
	u16 time_2s = seconds / 2;
	u32 time_32k = (seconds % 2) * RTC_CLOCK_TICKS_PER_SECOND + datetime->time.ms * RTC_CLOCK_TICKS_PER_SECOND / 1000;
	u32 time_counter = (time_2s << 16) | (time_32k & 0xFFFF);
	RTC_setTimeCounter(time_counter);
	
#else
	u32 total_seconds = RTC_get_seconds(datetime->date.year, datetime->date.month, datetime->date.day,
										datetime->time.hr, datetime->time.min, datetime->time.sec);
	u32 increments = total_seconds * RTC_INCREMENTS_PER_SECOND +
						(datetime->time.ms * RTC_INCREMENTS_PER_SECOND) / 1000;
	RTC_CONFIG_CHANGE(
		RTC->CNTH = increments >> 16;
		RTC->CNTL = increments & RTC_LSB_MASK;	
	);
#endif
}

#ifndef CH5xx
// Currently only implemented for non-CH5xx
// Should be possible to do with CH5xx using trigger and timer flags for RTC

void RTC_setAlarm(u32 increments) {
	// Make sure AFIO power is on for EXTI interrupts
	RCC->APB2PCENR |= RCC_AFIOEN;

	// Clear any pending alarm interrupt before reconfiguring
	NVIC_ClearPendingIRQ(RTCAlarm_IRQn);
	EXTI->INTFR = EXTI_Line17;	
	RTC->CTLRL &= ~RTC_CTLRL_ALRF;
	
	RTC_CONFIG_CHANGE(
		RTC->ALRMH = increments >> 16;
		RTC->ALRML = increments & RTC_LSB_MASK;	
	);

	// A few RTC ticks are necessary before enabling EXTI, as they are on two
	// different clock domains and <my current working theory> EXTI fires prematurely
	// on stale alarm register values?
	// What is weird is that waiting for the register synchronization flag (RSF) to be
	// set is not sufficient and doesn't appear to make any difference for the timing!
	Delay_Us(100);

	EXTI->INTENR   |= EXTI_Line17;
	EXTI->RTENR    |= EXTI_Line17;   // RTC alarm is rising-edge
	NVIC_EnableIRQ(RTCAlarm_IRQn);
	RTC->CTLRH |= RTC_CTLRH_ALRIE;
}

void RTC_setAlarmAbsolute(rtc_datetime_t *datetime) {
	u32 total_seconds = RTC_get_seconds(datetime->date.year, datetime->date.month, datetime->date.day,
									datetime->time.hr, datetime->time.min, datetime->time.sec);
	u32 increments = total_seconds * RTC_INCREMENTS_PER_SECOND +
						(datetime->time.ms * RTC_INCREMENTS_PER_SECOND) / 1000;
	RTC_setAlarm(increments);
}

void RTC_setAlarmRelative(u16 seconds_from_now, u16 milliseconds_from_now) {
	u32 current = RTC_getCounter();
	u32 alarm_value = current + (seconds_from_now * RTC_INCREMENTS_PER_SECOND) + 
		(milliseconds_from_now * RTC_INCREMENTS_PER_SECOND) / 1000;
	RTC_setAlarm(alarm_value);
}

#endif // !CH5xx

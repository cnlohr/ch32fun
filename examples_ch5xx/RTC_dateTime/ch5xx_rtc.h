// MIT License
// Copyright (c) 2025 UniTheCat

#include "ch32fun.h"
#include <stdio.h>
#include "rtc_helper.h"

//! ####################################
//! CORE FUNCTION
//! ####################################

void RTC_printRaw() {
	printf("\n32k (%d), 2s (%d), day (%ld)\r\n",
			R16_RTC_CNT_32K, R16_RTC_CNT_2S, R32_RTC_CNT_DAY);
}

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

//! ####################################
//! SET CALENDAR FUNCTIONS
//! ####################################

// the R32_RTC_CNT_DAY is only 14 bits.
// Therefore it can only store max 16383 calendar days ~ 44 years
// We will calculate the days counter from the year since 2020

void RTC_setDate(rtc_date_t date) {
	// Clip year since 2020
	u32 days_counter = 0;
	if (date.year < 2020) date.year = 2020;
	if (date.year > 2064) date.year = 2064;

	// Add days for complete years from 2020 to year-1
	for (u16 y = 2020; y < date.year; y++) {
		days_counter += IS_LEAP_YEAR(y) ? 366 : 365;
	}

	// Add days in the current year
	days_counter += RTC_days_of_year(date.year, date.month, date.day);
	RTC_setDayCounter(days_counter);
}

void RTC_setTime(rtc_time_t time) {
	u16 total_seconds = time.hr * 3600 + time.min * 60 + time.sec;
	u16 time_2s = total_seconds / 2;
	u32 time_32k = (total_seconds % 2) * RTC_TICKS_PER_SECOND;
	u32 time_counter = (time_2s << 16) | (time_32k & 0xFFFF);
	RTC_setTimeCounter(time_counter);
}
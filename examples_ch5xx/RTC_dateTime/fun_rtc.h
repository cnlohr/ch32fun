// MIT License
// Copyright (c) 2025 UniTheCat

#include "ch32fun.h"
#include <stdio.h>

#define RTC_TICKS_PER_SECOND 32768

//! ####################################
//! CORE FUNCTION
//! ####################################

void fun_rtc_printRaw() {
	printf("\n32k (%d), 2s (%d), day (%ld)\r\n",
			R16_RTC_CNT_32K, R16_RTC_CNT_2S, R32_RTC_CNT_DAY);
}

void _rtc_setDayCounter(u32 day_counter) {
	SYS_SAFE_ACCESS(
		R32_RTC_TRIG = day_counter;
		R8_RTC_MODE_CTRL |= RB_RTC_LOAD_HI;
	);
}

void _rtc_setTimeCounter(u32 time_counter) {
	SYS_SAFE_ACCESS(
		R32_RTC_TRIG = time_counter;
		R8_RTC_MODE_CTRL |= RB_RTC_LOAD_LO;
	);
}

//! ####################################
//! SET CALENDAR FUNCTIONS
//! ####################################

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


u32 _calculate_days_of_year(u16 year, u8 month, u8 day) {
	u32 day_of_year = 0;
	
	// Add days from January to month-1
	for (u8 m = 0; m < month - 1; m++) {
		day_of_year += DAYS_IN_MONTH[m];
	}
	
	// Add extra day for February if it's a leap year
	if (month > 2 && IS_LEAP_YEAR(year)) {
		day_of_year += 1;
	}
	
	// Add days in the current month
	day_of_year += day;

	return day_of_year - 1;	 // subtract 1 to make it 0-based
}

// the R32_RTC_CNT_DAY is only 14 bits.
// Therefore it can only store max 16383 calendar days ~ 44 years
// We will calculate the days counter from the year since 2020

void fun_rtc_setDate(rtc_date_t date) {
	// Clip year since 2020
	u32 days_counter = 0;
	if (date.year < 2020) date.year = 2020;
	if (date.year > 2064) date.year = 2064;

	// Add days for complete years from 2020 to year-1
	for (u16 y = 2020; y < date.year; y++) {
		days_counter += IS_LEAP_YEAR(y) ? 366 : 365;
	}

	// Add days in the current year (0-based)
	days_counter += _calculate_days_of_year(date.year, date.month, date.day);
	_rtc_setDayCounter(days_counter);
}

void fun_rtc_setTime(rtc_time_t time) {
	u16 total_seconds = time.hr * 3600 + time.min * 60 + time.sec;
	u16 time_2s = total_seconds / 2;
	u32 time_32k = (total_seconds % 2) * RTC_TICKS_PER_SECOND;
	u32 time_counter = (time_2s << 16) | (time_32k & 0xFFFF);
	_rtc_setTimeCounter(time_counter);
}

void fun_rtc_setDateTime(rtc_date_t date, rtc_time_t time) {
	fun_rtc_setDate(date);
	fun_rtc_setTime(time);
}

//! ####################################
//! GET CALENDAR FUNCTIONS
//! ####################################

void fun_rtc_getDate(rtc_date_t *date) {
	if (date == NULL) return;
	u32 days_remaining = R32_RTC_CNT_DAY;
	u16 year = 2020;
	u8 month, day;

	// 1. Find the year
	while (1) {
		u16 days_in_year = IS_LEAP_YEAR(year) ? 366 : 365;
		if (days_remaining < days_in_year) break;
		days_remaining -= days_in_year;
		year++;
	}
	
	// 2. Find the month
	u8 days_in_feb = IS_LEAP_YEAR(year) ? 29 : 28;
	month = 1;
	while (month <= 12) {
		u8 days_in_current_month = (month == 2) ? days_in_feb : DAYS_IN_MONTH[month - 1];
		
		if (days_remaining < days_in_current_month) break;
		days_remaining -= days_in_current_month;
		month++;
	}
	
	// 3. Set the date structure
	date->year = year;
	date->month = month;
	date->day = days_remaining + 1;  // Add 1 because days_remaining is 0-based
	
	// printf("\nDate: %04u-%02u-%02u", date->year, date->month, date->day);
	// printf("\t from day counter: %lu", days_counter);
	// printf("\n");	
}

void fun_rtc_getTime(rtc_time_t *time) {
	if (time == NULL) return;
	u32 seconds_2s = R16_RTC_CNT_2S;
	u32 ticks_32k = R16_RTC_CNT_32K;

	// Calculate total seconds directly
	u32 total_seconds = (seconds_2s * 2) + (ticks_32k / RTC_TICKS_PER_SECOND);

	// Get remaining ticks for milliseconds
	u32 remaining_ticks = ticks_32k % RTC_TICKS_PER_SECOND;

	// Convert seconds to h:m:s
	time->hr = total_seconds / 3600;
	time->min = (total_seconds % 3600) / 60;
	time->sec = total_seconds % 60;

	// Calculate milliseconds
	time->ms = (remaining_ticks * 1000UL) / RTC_TICKS_PER_SECOND;
	
	// printf("Time: %02u:%02u:%02u.%03u", time->hour, time->min, time->sec, time->ms);
	// printf("\t from 2s: %lu, 32k: %lu", seconds_2s, ticks_32k);
	// printf("\n");
}

void fun_rtc_getDateTime(rtc_date_t *date, rtc_time_t *time) {
	fun_rtc_getDate(date);
	fun_rtc_getTime(time);
}
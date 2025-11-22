// MIT License
// Copyright (c) 2025 UniTheCat

#define RTC_TICKS_PER_SECOND 32768

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

rtc_time_t RTC_get_time(u32 total_seconds, u32 ms) {
	u32 minutes = total_seconds / 60;

	return (rtc_time_t) {
		.sec = total_seconds % 60,
		.min = minutes % 60,
		.hr = (minutes / 60) % 24,
		.ms = ms
	};
}

rtc_date_t RTC_get_date(u32 total_seconds, u16 year_base) {
	rtc_date_t output = {
		.year = year_base,
		.month = 1,
		.day = 1
	};

	// Days since epoch
	u32 days_remaining = total_seconds / SECONDS_PER_DAY;

	// Find the year
	while(1) {
		u32 days_in_year = IS_LEAP_YEAR(output.year) ? 366 : 365;
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
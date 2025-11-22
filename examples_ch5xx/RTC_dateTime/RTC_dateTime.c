// Simple example to show how to use the RTC to get the date and time

#include "ch32fun.h"
#include <stdio.h>
#include "ch5xx_rtc.h"

int main()
{
	SystemInit();
	Delay_Ms(100);

	rtc_date_t date = {2025, 10, 25};
	rtc_time_t time = {8, 30, 5};
	RTC_setDate(date);
	RTC_setTime(time);

	while(1) {
		u32 seconds_2s = R16_RTC_CNT_2S;
		u32 ticks_32k = R16_RTC_CNT_32K;
		u32 total_seconds = (seconds_2s * 2) + (ticks_32k / RTC_TICKS_PER_SECOND);

		// Get remaining ticks for milliseconds
		u32 remaining_ticks = ticks_32k % RTC_TICKS_PER_SECOND;
		// Calculate milliseconds
		u32 ms = (remaining_ticks * 1000UL) / RTC_TICKS_PER_SECOND;

		rtc_time_t time = RTC_get_time(total_seconds, ms);
		rtc_date_t date = RTC_get_date(R32_RTC_CNT_DAY * SECONDS_PER_DAY, 2020);

		printf("\n");
		RTC_print_date(date, "/");
		printf(" ");
		RTC_print_time(time);
		Delay_Ms(1000);
	}
}

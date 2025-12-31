// Simple example to show how to use the RTC to get the date and time

#include "ch32fun.h"
#include <stdio.h>
#include "rtc.h"

int main()
{
	SystemInit();
	Delay_Ms(100);

	rtc_date_t date = {2025, 10, 25};
	rtc_time_t time = {8, 30, 5};
	rtc_datetime_t datetime = {date, time};
	RTC_setDateTime(&datetime);

	while(1) {
		datetime = RTC_getDateTime();
		printf("\n");
		RTC_print_date(datetime.date, "/");
		printf(" ");
		RTC_print_time(datetime.time);
		Delay_Ms(1000);
	}
}

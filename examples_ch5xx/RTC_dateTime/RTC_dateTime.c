#include "ch32fun.h"
#include <stdio.h>

#include "fun_rtc.h"

// use defines to make more meaningful names for our GPIO pins

int main()
{
	SystemInit();
	Delay_Ms(100);

	rtc_date_t date = {2025, 10, 25};
	rtc_time_t time = {8, 30, 5};
	fun_rtc_setDateTime(date, time);

	while(1)
	{
		fun_rtc_getDateTime(&date, &time);

		printf("%04d-%02d-%02d %02d:%02d:%02d.%03d\r\n",
			date.year, date.month, date.day,
			time.hr, time.min, time.sec, time.ms);
		Delay_Ms(1000);
	}
}

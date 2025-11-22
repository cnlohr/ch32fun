#include "ch32fun.h"
#include "register_debug_utilities.h"
#include "v2xx_v3xx_rtc.h"

int main() {
	SystemInit();

	printf("\r\n~~ RTC test ~~\r\n");

	printf("\nRCC->CTLR:\n");
	UTIL_PRINT_BITS(RCC->CTLR, 32, 32);
	UTIL_PRINT_BIT_RANGE(
		RCC->CTLR,
		"\nPLLON", 24, 24,
		"\nHSEON", 16, 16,
		"\nHSIRDY", 1, 1,
		"\nHSION", 0, 0, NULL
	);

	printf("\nRCC->CFGR0:\n");
	UTIL_PRINT_BITS(RCC->CFGR0, 32, 32);
	UTIL_PRINT_BIT_RANGE(
		RCC->CFGR0,
		"\nPLLMUL[3:0]", 21, 18,
		"\nPLLXTPRE", 17, 17,
		"\nPLLSRC", 16, 16,
		"\nHPRE[3:0]", 7, 4,
		"\nSWS[1:0]", 3, 2,
		"\nSW[1:0]", 1, 0, 				// 0: HSI, 1: HSE, 2: PLL
		NULL
	);
	printf("\n");

    // Get system clock source
    uint32_t sws = (RCC->CFGR0 >> 2) & 0x3;
	switch(sws) {
        case 0: printf("HSI used as system clock\n"); break;
        case 1: printf("HSE used as system clock\n"); break;
        case 2: printf("PLL used as system clock\n"); break;
        default: printf("Reserved\n"); break;
    }

	uint32_t pllmul = (RCC->CFGR0 >> 18) & 0xF;
	uint32_t pll_multiplier = pllmul + 2;
	if (pll_multiplier > 14) pll_multiplier = 18;
    printf("PLL Multiplier: %dx\n", pll_multiplier);
    printf("PLL Output: %lu MHz\n", 8000000 * pll_multiplier / 1000000);

	RTC_init();

	rtc_date_t date = {2025, 11, 18};
	rtc_time_t time = {7, 10, 5};
	RTC_setDateTime(date, time);

	while(1) {
		u32 seconds = RTC_GetCounter();
		rtc_date_t date = RTC_get_date(seconds, 1970);
		rtc_time_t time = RTC_get_time(seconds, 0);

		printf("\n");
		RTC_print_date(date, "/");
		printf(" ");
		RTC_print_time(time);
		Delay_Ms(1000);
	}
}

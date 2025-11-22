/**
* Window Watchdog (WWDG) Example
* 
* The WWDG uses a 7-bit downcounter connected to the APB1 bus (PCLK1 clock).
* The counter decrements from 0x7F to 0x3F. When it transitions from 0x40 to 0x3F,
* the system is reset.
* 
* Timing:
* - Total timeout period: (T[6:0] - 0x3F) × time_per_tick
* - Window opens at: (W[6:0] - 0x3F) × time_per_tick  
* - Window closes at: total timeout period
* 
* Refresh Rules:
* - Early refresh (before window opens): IGNORED
* - Refresh during window: ACCEPTED (counter reloaded)
* - Late refresh (after counter < 0x40): SYSTEM RESET
* 
* Calculations:
* Time_per_tick = (4096 × 2^WDGTB) ÷ PCLK1_frequency
* Total_timeout = Time_per_tick × (counter_value - 0x3F)
* Window_start = Time_per_tick × (window_value - 0x3F)
* 
* Example Configuration:
* - PCLK1 = 8 MHz, WDGTB = ÷8 (slowest prescaler)
* - Time per tick = 4.096 ms
* - Window value = 0x5F, Counter = 0x7F
* - Total timeout = 256 ms, Window opens at 132 ms
* 
* Program Behavior:
* PA3 is pull-up. When grounded, it feeds the watchdog during the open window.
* The demo feeds twice at 150ms intervals (within window), then waits 300ms
* (exceeding timeout), causing a watchdog reset.
*/

#include "ch32fun.h"
#include <stdio.h>

#define WWDG_CNT 0X7F

void WWDG_init(u32 PCLK1_freq, u32 prescaler, u8 window, u16 counter) {
	u8 WDGTB = (prescaler >> 7) & 0x03;
	printf("Prescaler: %d, WDGTB: %d\n", prescaler, WDGTB);
	
	u32 time_per_tick_ms = 4096 * (1 << WDGTB) * 1000 / PCLK1_freq;
	u32 total_timeout_ms = time_per_tick_ms * (counter - 0x3F);
	u32 window_start_ms = time_per_tick_ms * (window - 0x3F);
	u32 window_duration_ms = time_per_tick_ms * (counter - window);

	printf("PCLK1=%dMHz, WDGTB=/%d, Window=0x%02X, Counter=0x%02X:\n",
		PCLK1_freq / 1000000, (1 << WDGTB), window, counter);
	printf("  Time per tick: %lu ms\n", time_per_tick_ms);
	printf("  Total timeout: %lu ms\n", total_timeout_ms);
	printf("  Window opens: %lu ms\n", window_start_ms);
	printf("  Window duration: %lu ms\n", window_duration_ms);

	RCC->APB1PCENR |= RCC_APB1Periph_WWDG;

	// setup WWDG
	WWDG->CTLR = counter & WWDG_CTLR_T;
	WWDG->CFGR = (WWDG->CFGR & CFGR_WDGTB_Mask) | prescaler;
	WWDG->CFGR |= (WWDG->CFGR & CFGR_W_Mask) | window;

	// start WWDG
	WWDG->CTLR = CTLR_WDGA_Set | WWDG_CNT;
	WWDG->STATR = 0; // Reset WWDG
}

int main() {
	SystemInit();
	funGpioInitAll(); // Enable GPIOs

	// set PA3 to input pullup
	funPinMode(PA3, GPIO_CFGLR_IN_PUPD);

	printf("\n~ WWDG Test ~\n");
	WWDG_init(8000000, WWDG_Prescaler_8, 0x5F, WWDG_CNT);

	int read;

	while(1) {
		read = funDigitalRead( PA3 );
		printf("\n+++ Passing test 1...\n");
		printf("PA3: %d\n", read);
		Delay_Ms( 150 );

		if (read) {
			 // Reload watchdog
			printf("reload watchdog\n");
			WWDG->CTLR = WWDG_CNT & WWDG_CTLR_T;
		}

		read = funDigitalRead( PA3 );
		printf("\n+++ Passing test 2...\n");
		printf("PA3: %d\n", read);
		Delay_Ms( 150 );

		if (read) {
			 // Reload watchdog
			printf("reload watchdog 2\n");
			WWDG->CTLR = WWDG_CNT & WWDG_CTLR_T;
		}

		read = funDigitalRead( PA3 );
		printf("\n--- Failing test...\n");
		printf("PA3: %d\n", read);
		Delay_Ms( 300 );

		if (read) {
			 // Reload watchdog
			printf("reload watchdog\n");
			WWDG->CTLR = WWDG_CNT & WWDG_CTLR_T;
		}
	}
}

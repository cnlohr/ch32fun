// Simple example that shows how to use Independent Watchdog Timer IWDG
// In this example, IWDG is set to timeout every 5 seconds
// If the watchdog is not feeded within 5 seconds, the system will reset
// To feed the watchdog, PA3 need to be shorted to GND

// timeout = (reload + 1) * (prescaler/LSI_freq)
// reload = timeout * LSI_freq / prescaler - 1

// the IWDG timer is 12-bits counter, the maximum reload value is 4095, 
// the min prescaler is 4 and max prescaler is 256
// min_timeout = 1 * 4 / 40KHz = 0.1 millisecond
// max_timeout = 4095 * 256 / 40KHz = 26.2 seconds

#include "ch32fun.h"
#include <stdio.h>

void IWDG_init(u16 prescaler, u16 reload) {
    // 1. Enable access to IWDG registers
    IWDG->CTLR = IWDG_WriteAccess_Enable;
    
    // 2. Set prescaler and reload value
    IWDG->PSCR = prescaler;
    IWDG->RLDR = reload;

    // 3. Enable IWDG (cannot be disabled after this!)
	IWDG->CTLR = CTLR_KEY_Reload;
    IWDG->CTLR = CTLR_KEY_Enable;

	// 4. wait for LSI ready
	while(!(RCC->RSTSCKR & RCC_LSIRDY));
}

int main() {
	SystemInit();
	funGpioInitAll(); // Enable GPIOs

	printf("\n~ IWDG Test ~\n");

	// set PA3 to input pullup
	funPinMode(PA3, GPIO_CFGLR_IN_PUPD);

	// reload value for 5 second timeout = (5 * 40KHz / 64) - 1 = 3124
	IWDG_init(IWDG_Prescaler_64, 3124);

	while(1) {
		int read = funDigitalRead( PA3 );
		printf("PA3: %d\n", read);
		
		if (read) {
			// Reload watchdog
			printf("reload watchdog\n");
			IWDG->CTLR = CTLR_KEY_Reload;
		}
		Delay_Ms( 1000 );
	}
}

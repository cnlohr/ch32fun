// Simple ADC example to read temperature, battery voltage, and ADC channel 0 (PA4)
#include "ch32fun.h"
#include "fun_pwm_ch5xx.h"

u8 adc_channel = 4;		// PA12

int main() {
	SystemInit();
	Delay_Ms(100);
	funGpioInitAll();

	// 8-bit, cycle_sel = 1
	pwm_config(8, 1);

	// channel 4, clock div = 4, polarity 0 = active high
	pwm_init(adc_channel, 4, 0);

	u8 percent = 0;

	while(1) {
		pwm_set_duty_cycle(adc_channel, percent);
		if (++percent > 100) percent = 0;
		Delay_Ms(15);
	}
}



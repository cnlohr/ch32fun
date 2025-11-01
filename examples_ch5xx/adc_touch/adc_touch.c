// Simple ADC example to get capacitive touch readings

#include "../adc_basic/fun_adc_ch5xx.h"

// adjust this threshold to determine when a touch is detected
// put 0 to see all of the readings
#define CAP_TOUCH_THRESHOLD 0

int main() {
	SystemInit();
	Delay_Ms(100);
	funGpioInitAll();

	printf("ADC Capacitive Touch Demo\r\n");
	
	adc_touch_init();
	adc_set_channel(1);

	while(1) {
		s16 touch = acd_touch_convert(0x10, 0);

		if (touch > CAP_TOUCH_THRESHOLD) {
			printf("Touch: %d\r\n", touch);
		}
	}
}



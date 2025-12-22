// Simple ADC example to read temperature, battery voltage, and ADC channel 0

#include "fun_adc_ch5xx.h"

int main() {
	SystemInit();
	Delay_Ms(100);
	funGpioInitAll();

	u16 adc_raw = 0;

	while(1) {
		//# get temp reading
		adc_set_channel(ADC_TEMPERATURE_CHANNEL);
		adc_set_config(ADC_FREQ_DIV_10, ADC_PGA_GAIN_2, 1);
		adc_raw = adc_get_singleReading();
		printf("\nTemperature: %d mC\r\n", adc_to_mCelsius(adc_raw));

		//# get battery reading
		adc_set_channel(ADC_VBAT_CHANNEL);
		adc_set_config(ADC_FREQ_DIV_10, ADC_PGA_GAIN_1_2, 0);
		adc_raw = adc_get_singleReading();
		printf("Battery Voltage: %d mV\r\n", adc_to_mV(adc_raw, ADC_PGA_GAIN_1_2));

		//# get ch0 reading PA4
		adc_set_channel(0);
		adc_buf_enable(1);
		adc_set_config(ADC_FREQ_DIV_10, ADC_PGA_GAIN_1_4, 0);
		
		//# get single channel reading
		adc_raw = adc_get_singleReading();
		printf("PA4: %d mV\r\n", adc_to_mV(adc_raw, ADC_PGA_GAIN_1_4));

		Delay_Ms(1000);
	}
}



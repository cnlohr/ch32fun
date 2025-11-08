// Simple ADC example to get DMA readings on Channel 0 (PA4)

#include "../adc_basic/fun_adc_ch5xx.h"

int main() {
	SystemInit();
	Delay_Ms(100);
	funGpioInitAll();

	u16 adc_raws[100] = {0};

	adc_set_channel(0);
	adc_buf_enable(1);
	adc_set_config(ADC_FREQ_DIV_10, ADC_PGA_GAIN_1_4, 0);

	while(1) {
		//# get dma readings
		adc_dma_start(250, adc_raws, 100, 1);
		adc_dma_wait();
		Delay_Ms(100);
		adc_dma_stop();

		for (int i = 0; i < 100; i++) {
			printf("PA4: %d mV\r\n", adc_to_mV(adc_raws[i], ADC_PGA_GAIN_1_4));
		}
	}
}


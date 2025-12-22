// MIT License
// Copyright (c) 2025 UniTheCat

//# TODO: Add differential ADC

#include "ch32fun.h"
#include <stdio.h>

#define ADC_VBAT_CHANNEL 0x0E
#define ADC_TEMPERATURE_CHANNEL 0x0F

// Datasheet Ref: Chapter 15 (pg. 109)
// PA0: 	SCK1				- AIN9
// PA1: 	MOSI1				- AIN8
// PA2: 	MISO1				- AIN7
// PA3: 						- AIN6
// PA4: 	RX3					- AIN0
// PA5: 	TX3					- AIN1
// PA6: 	RX2,				- AIN10 	~ PWM4_
// PA7: 	TXD2				- AIN11 	~ PWM5_ 
// PA8: 	RX1,				- AIN12
// PA9:	 	TX1, TMR0			- AIN13 	~ PWM0 Capture input #0
// PA10:	TMR1							~ PWM1 Capture input #1
// PA11:	TMR2							~ PWM2 Capture input #2
// PA12:						- AIN2		~ PWM4
//! PA13:	SCK					- AIN3		~ PWM5
//! PA14:	MOSI, TX0_			- AIN4
//! PA15:	MISO, RX0_			- AIN5


//! ####################################
//! ADC CORE FUNCTIONS
//! ####################################

typedef enum {
	ADC_FREQ_DIV_10 = 0b00,		// 32/10 = 3.2MHz
	ADC_FREQ_DIV_4 = 0b01,		// 32/4 = 8MHz
	ADC_FREQ_DIV_6 = 0b10,		// 32/6 = 5.33MHz - Default
	ADC_FREQ_DIV_8 = 0b11		// 32/8 = 4MHz
} ADC_FREQ_DIV_t;

typedef enum {
	ADC_PGA_GAIN_1_4 = 0b00,	// -12dB 1/4 	range: 2.9V ~ VIO33
	ADC_PGA_GAIN_1_2 = 0b01,	// -6dB 1/2 	range: 1.9V ~ 3V
	ADC_PGA_GAIN_1 = 0b10,		// 0dB 1 		range: 0V ~ 2V - Default
	ADC_PGA_GAIN_2 = 0b11		// 6dB 2		range: 0.6V ~ 1.5V
} ADC_PGA_GAIN_t;

void adc_set_config(ADC_FREQ_DIV_t adc_freq, ADC_PGA_GAIN_t pa_gain, u8 differential_mode) {
	R8_TKEY_CFG &= ~RB_TKEY_PWR_ON;
	R8_ADC_CFG = RB_ADC_POWER_ON | (pa_gain << 4) | (adc_freq << 6);

	// not needed for touchkey
	if (differential_mode) {
		// differential
		R8_ADC_CFG |= RB_ADC_DIFF_EN;
	} else {
		// single-ended input
		R8_ADC_CFG &= ~RB_ADC_DIFF_EN;
	}
}


/* Datasheet Ref: Chapter 15 (pg. 111)
    It is recommended to enable the input buffer by default. Only when the internal
    resistance of the external signal  source is less than 1KΩ, the input buffer can be
    turned off for ADC. When using differential input, it is recommended to turn off the
    input buffer. When used for TouchKey detection, the input buffer must be enabled,
    and it is recommended to select one of 2 gains of 0dB (priority) or -6dB.
*/
void adc_buf_enable(u8 enable) {
	if (enable) {
		R8_ADC_CFG |= RB_ADC_BUF_EN;
	} else {
		R8_ADC_CFG &= ~RB_ADC_BUF_EN;
	}
}

u16 adc_get_singleReading() {
	R8_ADC_CONVERT |= RB_ADC_START;
	while( R8_ADC_CONVERT & RB_ADC_START );
	return R16_ADC_DATA;
}

void adc_set_channel(u8 channel) {
	u16 pin = 0;

	switch (channel) {
		case 0: 	pin = PA4; break;
		case 1: 	pin = PA5; break;
		case 2: 	pin = PA12; break;
		case 3: 	pin = PA13; break;
		case 4: 	pin = PA14; break;
		case 5: 	pin = PA15; break;
		case 6: 	pin = PA3; break;
		case 7: 	pin = PA2; break;
		case 8: 	pin = PA1; break;
		case 9: 	pin = PA0; break;
		case 10: 	pin = PA6; break;
		case 11: 	pin = PA7; break;
		case 12: 	pin = PA8; break;
		case 13: 	pin = PA9; break;
		case 14:
			break;
		case 15:
			R8_TEM_SENSOR = RB_TEM_SEN_PWR_ON;
			break;
	}

	if (pin != 0) {
		funPinMode( pin, GPIO_ModeIN_Floating );
	}

	R8_ADC_CHANNEL = channel;
}

void adc_dma_start(u8 auto_cycle, u16 *buf, u8 sample_len, u8 loop) {
    // auto_cycle controls the time interval between conscutive ADC conversions
    // higher auto_cycle = faster conversion. 16 clock per cycle
    // sample timing = (256 - auto_cycle) * 16 * Tsys
    R8_ADC_AUTO_CYCLE = auto_cycle;
    R16_ADC_DMA_BEG = (u32)buf;
    R16_ADC_DMA_END = (u32)buf + (sample_len*sizeof(u16));        // 2 bytes per sample
    R8_ADC_CTRL_DMA |= RB_ADC_DMA_ENABLE | RB_ADC_IE_DMA_END;

    if (loop) {
        R8_ADC_CTRL_DMA |= RB_ADC_DMA_LOOP;
    } else {
        R8_ADC_CTRL_DMA &= ~RB_ADC_DMA_LOOP;
    }

    // start DMA
    (R8_ADC_CTRL_DMA |= RB_ADC_AUTO_EN);
}

void adc_dma_wait() {
    while (!(R8_ADC_CTRL_DMA & RB_ADC_IF_DMA_END)) {}
}

void adc_dma_stop() {
    R8_ADC_CTRL_DMA &= ~RB_ADC_DMA_ENABLE;
}


//! ####################################
//! TOUCHKEY FUNCTIONS
//! ####################################

void adc_touch_init() {
	R8_TKEY_CFG |= RB_TKEY_PWR_ON;
    R8_ADC_CFG = RB_ADC_POWER_ON | RB_ADC_BUF_EN | (ADC_PGA_GAIN_1 << 4);
}

u16 acd_touch_convert(uint8_t charge_count, uint8_t discharge_count) {
	// charge_time = (RB_TKEY_CHARG_CNT+1)*Tadc
	// discharge_time = (RB_TKEY_DISCH_CNT+1)*Tadc
    R8_TKEY_COUNT = (discharge_count << 5) | (charge_count & 0x1f);
    R8_TKEY_CONVERT = RB_TKEY_START;
    while(R8_TKEY_CONVERT & RB_TKEY_START);
    return (R16_ADC_DATA & RB_ADC_DATA);
}



//! ####################################
//! ADC OTHER FUNCTIONS
//! ####################################

#define ROM_CFG_TMP_25C 0x7F014

s32 adc_to_mCelsius(u16 raw) {
    u32 C25 = *((volatile u32*)ROM_CFG_TMP_25C);
    s32 tempC_base = (C25 >> 16) & 0xFFFF;
    tempC_base = (tempC_base ? tempC_base : 25);
    
    // Scale by 1000 for milliCelsius
    return tempC_base * 1000 + ((s32)(raw - (C25 & 0xFFFF)) * 10000) / 27;
}

s32 adc_to_mV(u16 raw, ADC_PGA_GAIN_t pa_gain) {
	//? Table 15-2 PGA gain selection and input votlage range
	s32 res = 0;
	s32 vref_mV = 1050;			// +-15mV

    //! NOTE: expand multiplications to keep integer math precisions
	switch (pa_gain) {
		case ADC_PGA_GAIN_1_4:
			// -12dB: vref * (raw/512 - 3)
			res = vref_mV * raw / 512 - vref_mV * 3;
			break;
		case ADC_PGA_GAIN_1_2:
			// -6dB: vref * (raw/1024 - 1)
			res = vref_mV * raw / 1024 - vref_mV;
			break;
		case ADC_PGA_GAIN_1:
			// 0dB: vref * (raw/2048)
			res = vref_mV * raw / 2048;
			break;
		case ADC_PGA_GAIN_2:
			// 6dB: vref * (raw/4096 + 0.5)
			// vref_mv * raw/4090 + vref_mV/2;
            res = vref_mV * raw / 4096 + vref_mV / 2;
			break;
	}
	return res;
}


// MIT License
// Copyright (c) 2025 UniTheCat

#include "ch32fun.h"

// PA9:	 	TX1, TMR0			~ PWM0 Capture input #0
// PA10:	TMR1				~ PWM1 Capture input #1
// PA11:	TMR2				~ PWM2 Capture input #2
// PA12:						~ PWM4
//! PA13:	SCK					~ PWM5
// PB23: 	RST, TMR0_, TX2_	~ PWM11
// PB22: 	TMR3, RX2_			~ PWM3 Capture input #3
// PB14: 	MOSI_				~ PWM10
//! PB7: 	TX0					~ PWM9
// PB6: 						~ PWM8
//! PB4: 	RX0					~ PWM7
// PB0:							~ PWM6

void pwm_init(u8 channel, u8 clock_divider, u8 polarity) {
    // Validate channel
    if (channel < 4 || channel > 11) { return; }
	u16 pin;

    switch(channel) {
        case 4:		pin = PA12; break;
        case 5: 	pin = PA13; break;
        case 6: 	pin = PB0; break;
        case 7: 	pin = PB4; break;
        case 8: 	pin = PB6; break;
        case 9: 	pin = PB7; break;
        case 10: 	pin = PB14; break;
        case 11: 	pin = PB23; break;
	}
	funPinMode(pin, GPIO_CFGLR_OUT_10Mhz_PP);

    u8 polarity_bit = (1 << (channel - 4));
    (polarity) ? (R8_PWM_POLAR |= polarity_bit) : (R8_PWM_POLAR &= ~polarity_bit);

	R8_PWM_CLOCK_DIV = clock_divider;
    R8_PWM_OUT_EN |= (1 << (channel - 4));
}

void pwm_config(u8 bit_width, u8 cycle_sel) {
    // Clear the configuration bits first, then set new values
    R8_PWM_CONFIG &= ~(RB_PWM_CYC_MOD | RB_PWM_CYCLE_SEL);
    
    u8 bit_width_value;
    switch (bit_width) {
        case 7:
            bit_width_value = 1 << 2;  // 01 in bits [3:2] = 0x04
            break;
        case 6:
            bit_width_value = 2 << 2;  // 10 in bits [3:2] = 0x08
            break;
        case 5:
            bit_width_value = 3 << 2;  // 11 in bits [3:2] = 0x0C
            break;
        default: // 8-bit
            bit_width_value = 0;       // 00 in bits [3:2] = 0x00
            break;
    }
    
    // Set the configuration
    R8_PWM_CONFIG |= bit_width_value;
    if (cycle_sel) {
        R8_PWM_CONFIG |= RB_PWM_CYCLE_SEL;
    }	
}

void pwm_set_duty_cycle(u8 channel, u8 duty_cycle_percent) {
    if (channel < 4 || channel > 11) return;
    // Clamp duty cycle to valid range
    if (duty_cycle_percent > 100) duty_cycle_percent = 100;

	// Ncyc = cycle_sel ? (2^n-1) : (2^n) 
	// (n= data bit width), the Ncyc result ranges from 63 to 256. 
    u8 config = R8_PWM_CONFIG;
    u8 width_bits = (config & RB_PWM_CYC_MOD) >> 2;  // Extract bits [3:2]
    u8 cycle_sel = config & RB_PWM_CYCLE_SEL;

    // Calculate Ncyc based on current configuration
    u16 ncyc;
    switch(width_bits) {
        case 0: ncyc = cycle_sel ? 255 : 256; break; // 8-bit: 00
        case 1: ncyc = cycle_sel ? 127 : 128; break; // 7-bit: 01  
        case 2: ncyc = cycle_sel ? 63 : 64; break;   // 6-bit: 10
        case 3: ncyc = cycle_sel ? 31 : 32; break;   // 5-bit: 11
        default: ncyc = 256; break;
    }

    // Calculate PWM data value and write to register
	// Duty_cycle = (R8_PWMx_DATA / Ncyc) * 100%
	// R8_PWMx_DATA = (Duty_cycle * Ncyc) / 100
    u16 pwm_data = (u16)(duty_cycle_percent * ncyc / 100);

    switch (channel) {
        case 4: R8_PWM4_DATA = pwm_data; break;
        case 5: R8_PWM5_DATA = pwm_data; break;
        case 6: R8_PWM6_DATA = pwm_data; break;
        case 7: R8_PWM7_DATA = pwm_data; break;
        case 8: R8_PWM8_DATA = pwm_data; break;
        case 9: R8_PWM9_DATA = pwm_data; break;
        case 10: R8_PWM10_DATA = pwm_data; break;
        case 11: R8_PWM11_DATA = pwm_data; break;
    }
}

void pwm_stop_channel(u8 channel) {
    if (channel < 4 || channel > 11) { return; }
    R8_PWM_OUT_EN &= ~(1 << (channel - 4));
}
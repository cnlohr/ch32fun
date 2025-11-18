/**
 * HX711 Library for ch32fun
 * Derived from the Arduino HX711 library by Bogdan Necula
 * (https://github.com/bogde/HX711)
 * 
 * MIT License
 * (c) 2025 Alexander Mandera
 * (c) 2018 Bogdan Necula
 */

#include "ch32fun.h"
#include <stdbool.h>

static uint8_t hx711_data_pin = 0;
static uint8_t hx711_clock_pin = 0;
static uint8_t hx711_gain = 0;
static float hx711_scale = 0;
static uint32_t hx711_offset = 0;

uint8_t hx711_shift_in(uint8_t dataPin, uint8_t clockPin) {
    uint8_t value = 0;
    uint8_t i;

    for (i = 0; i < 8; ++i) {
        funDigitalWrite(clockPin, 1);
        Delay_Us(1);
        value |= funDigitalRead(dataPin) << i;
        funDigitalWrite(clockPin, 0);
        Delay_Us(1);
    }
    return value;
}

void hx711_set_gain(uint8_t gain) {
    switch(gain) {
        case 128:
            hx711_gain = 1;
            break;
        case 64:
            hx711_gain = 3;
            break;
        case 32:
            hx711_gain = 2;
            break;
    }
}

void hx711_init(uint8_t dataPin, uint8_t clockPin, uint8_t gain) {
    hx711_data_pin = dataPin;
    hx711_clock_pin = clockPin;

    // Clock pin as output
    funPinMode(hx711_clock_pin, GPIO_CFGLR_OUT_50Mhz_PP);

    // Data pin as input with pullup
    funPinMode(hx711_data_pin, GPIO_CFGLR_IN_PUPD);
    funDigitalWrite(hx711_data_pin, FUN_HIGH);

    hx711_set_gain(gain);    
}

void hx711_set_scale(float scale) {
    hx711_scale = scale;
}

float hx711_get_scale(void) {
    return hx711_scale;
}

void hx711_set_offset(uint32_t offset) {
    hx711_offset = offset;
}

uint32_t hx711_get_offset(void) {
    return hx711_offset;
}

static inline bool hx711_is_ready(void) {
    return funDigitalRead(hx711_data_pin) == FUN_LOW;
}

void hx711_wait_ready(uint32_t delay) {
    while(!hx711_is_ready()) {
        Delay_Ms(delay);
    }
}

bool hx711_wait_ready_retry(uint8_t retries, uint32_t delay) {
    uint8_t count = 0;
    while(count < retries) {
        if(hx711_is_ready()) {
            return true;
        }
        Delay_Ms(delay);
        count++;
    }
    return false;
}

bool hx711_wait_ready_timeout(uint32_t timeout, uint32_t delay) {
    uint64_t start = SysTick->CNT;
    uint64_t timeout_ticks = timeout * DELAY_MS_TIME;
    while((SysTick->CNT - start) < timeout_ticks) {
        if(hx711_is_ready()) {
            return true;
        }
        Delay_Ms(delay);
    }
    return false;
}

uint32_t hx711_read(void) {
    hx711_wait_ready(1); // Use 1ms delay

    uint32_t value = 0;
    uint8_t data[3] = {0};

    // Disable interrupts during reading
    __disable_irq();

    // Read 24 bits
    for (uint8_t i = 0; i < 3; i++) {
        data[i] = hx711_shift_in(hx711_data_pin, hx711_clock_pin);
    }

    // Set gain for next reading
    for (uint8_t i = 0; i < hx711_gain; i++) {
        funDigitalWrite(hx711_clock_pin, 1);
        Delay_Us(1);
        funDigitalWrite(hx711_clock_pin, 0);
        Delay_Us(1);
    }

    __enable_irq();

    // Combine bytes to 24-bit value
    value = ((uint32_t)data[0] << 16) | ((uint32_t)data[1] << 8) | data[2];

    // Convert to signed 24-bit
    if (value & 0x800000) {
        value |= 0xFF000000;
    }

    return value;
}

uint32_t hx711_read_average(uint8_t times) {
    uint64_t sum = 0;
    for (uint8_t i = 0; i < times; i++) {
        sum += hx711_read();
    }
    return (uint32_t)(sum / times);
}

double hx711_get_value(uint8_t times) {
    return (double)(hx711_read_average(times) - hx711_offset);
}

float hx711_get_units(uint8_t times) {
    return hx711_get_value(times) / hx711_scale;
}

void hx711_tare(uint8_t times) {
    uint32_t sum = hx711_read_average(times);
    hx711_set_offset(sum);
}

void hx711_power_down(void) {
    funDigitalWrite(hx711_clock_pin, FUN_LOW);
    funDigitalWrite(hx711_clock_pin, FUN_HIGH);
}

void hx711_power_up(void) {
    funDigitalWrite(hx711_clock_pin, FUN_LOW);
}
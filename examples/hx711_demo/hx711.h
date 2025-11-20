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

// control pins
#ifndef HX711_DATA_PIN
#define HX711_DATA_PIN PD4
#endif

#ifndef HX711_CLK_PIN
#define HX711_CLK_PIN PD5
#endif

#ifdef HX711_GAIN
    #if HX711_GAIN == 128
        #define HX711_GAIN 1
    #elif HX711_GAIN == 64
        #define HX711_GAIN 3
    #elif HX711_GAIN == 32
        #define HX711_GAIN 2
    #endif
#else
    #define HX711_GAIN_VALUE 1
#endif

static uint32_t hx711_scale = 0; // Scale factor * 100
static uint32_t hx711_offset = 0;

uint8_t hx711_shift_in() __attribute__((section(".srodata"))) __attribute__((used));
uint8_t hx711_shift_in() {
    uint8_t value = 0;
    uint8_t i;

    for (i = 0; i < 8; ++i) {
        funDigitalWrite(HX711_CLK_PIN, FUN_HIGH);
        Delay_Us(1);
        value |= funDigitalRead(HX711_DATA_PIN) << (7 - i);
        funDigitalWrite(HX711_CLK_PIN, FUN_LOW);
        Delay_Us(1);
    }
    return (uint8_t)value;
}

void hx711_init() {
    // Clock pin as output
    funPinMode(HX711_CLK_PIN, GPIO_Speed_10MHz | GPIO_CNF_OUT_PP);

    // Data pin as input with pullup
    funPinMode(HX711_DATA_PIN, GPIO_CFGLR_IN_PUPD);
    funDigitalWrite(HX711_DATA_PIN, FUN_HIGH);
}

static inline void hx711_set_scale(uint32_t scale) {
    hx711_scale = scale;
}

static inline uint32_t hx711_get_scale(void) {
    return hx711_scale;
}

static inline void hx711_set_offset(uint32_t offset) {
    hx711_offset = offset;
}

static inline uint32_t hx711_get_offset(void) {
    return hx711_offset;
}

inline bool hx711_is_ready(void) {
    return funDigitalRead(HX711_DATA_PIN) == FUN_LOW;
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
        data[2 - i] = hx711_shift_in();
    }

    // Set gain for next reading
    for (uint8_t i = 0; i < HX711_GAIN_VALUE; i++) {
        funDigitalWrite(HX711_CLK_PIN, 1);
        Delay_Us(1);
        funDigitalWrite(HX711_CLK_PIN, 0);
        Delay_Us(1);
    }

    __enable_irq();

    // Combine bytes to 24-bit value
    uint8_t filler = 0x00;
    if(data[2] & 0x80) {
        filler = 0xFF;
    }

    value = ( (uint32_t)filler << 24
            | (uint32_t)data[2] << 16) 
            | ((uint32_t)data[1] << 8) 
            | data[0];

    return value;
}

uint32_t hx711_read_average(uint8_t times) {
    uint64_t sum = 0;
    for (uint8_t i = 0; i < times; i++) {
        sum += hx711_read();
    }
    return (uint32_t)(sum / times);
}

static inline uint32_t hx711_get_value(uint8_t times) {
    return hx711_read_average(times) - hx711_offset;
}

static inline uint32_t hx711_get_units(uint8_t times) {
    return hx711_get_value(times) * 100 / hx711_scale;
}

static inline void hx711_tare(uint8_t times) {
    uint32_t sum = hx711_read_average(times);
    hx711_set_offset(sum);
}

void hx711_power_down(void) {
    funDigitalWrite(HX711_CLK_PIN, FUN_LOW);
    funDigitalWrite(HX711_CLK_PIN, FUN_HIGH);
}

void hx711_power_up(void) {
    funDigitalWrite(HX711_CLK_PIN, FUN_LOW);
}
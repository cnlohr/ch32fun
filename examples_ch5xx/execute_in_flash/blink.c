#include "ch32fun.h"

// Adapted from examples_ch5xx/blink/blink.c to run from DataFlash.
#define PIN_1 PA4
#define PIN_K PA5
#define PIN_BOB PA8
#define PIN_KEVIN PA9

void blink_from_flash(void (*delay_fn)(uint32_t),
                      int (*print_fn)(const char *, ...))
{
	funPinMode(PIN_1, GPIO_CFGLR_OUT_10Mhz_PP);
	funPinMode(PIN_K, GPIO_CFGLR_OUT_10Mhz_PP);
	funPinMode(PIN_BOB, GPIO_CFGLR_OUT_10Mhz_PP);
	funPinMode(PIN_KEVIN, GPIO_CFGLR_OUT_10Mhz_PP);

	while (1) {
		funDigitalWrite(PIN_1, FUN_HIGH);
		funDigitalWrite(PIN_K, FUN_HIGH);
		funDigitalWrite(PIN_BOB, FUN_HIGH);
		funDigitalWrite(PIN_KEVIN, FUN_HIGH);
		print_fn("LEDs on\n");
		delay_fn(250 * DELAY_MS_TIME);

		funDigitalWrite(PIN_1, FUN_LOW);
		funDigitalWrite(PIN_K, FUN_LOW);
		funDigitalWrite(PIN_BOB, FUN_LOW);
		funDigitalWrite(PIN_KEVIN, FUN_LOW);
		print_fn("LEDs off\n");
		delay_fn(250 * DELAY_MS_TIME);
	}
}

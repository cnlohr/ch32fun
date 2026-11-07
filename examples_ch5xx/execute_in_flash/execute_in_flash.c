#include "ch32fun.h"

#include <stdint.h>
#include <stdio.h>

#define BLINK_IN_FLASH_ADDRESS 0x00070000u

typedef void (*delay_fn)(uint32_t);
typedef int (*print_fn)(const char *, ...);
typedef void (*blink_fn)(delay_fn, print_fn);

int main(void)
{
	SystemInit();

	// Call blink code stored in DataFlash, passing delay and print callbacks.
	blink_fn blink = (blink_fn)(uintptr_t)BLINK_IN_FLASH_ADDRESS;
	blink(DelaySysTick, printf);
}

#include <stdio.h>
#include <inttypes.h>

#include "ch32fun.h"
#include "ch20x_30x_flash.h"

// Write past first 64 KiB
#define TEST_ADDR 0x08010000
#define MAGIC 0xF1A57151


int main()
{
	SystemInit();

	puts("FLASH DEMO START");

	// First position of the array holds the magic word,
	// second one holds the run count
	uint32_t data[2];

	ch20x_30x_flash_cmd_read(TEST_ADDR, (uint8_t*)data, sizeof(data));

	if (MAGIC != data[0]) {
		data[0] = MAGIC;
		data[1] = 1;
	} else {
		// Increment run count
		data[1]++;
	}

	printf("Run count: %"PRIu32"\n", data[1]);

	puts("Saving run count...");
	const uint32_t err = ch20x_30x_flash_cmd_erase(TEST_ADDR, CH20X_30X_FLASH_PAGE_LEN) ||
		ch20x_30x_flash_cmd_write(TEST_ADDR, (uint8_t*)data, sizeof(data));
	if (err) {
		printf("Saving failed, error: %"PRIu32"\n", err);
	} else {
		puts("Done. Reset to check if run count is properly updated!");
	}

	while(1) {
		__WFE(); // Sleep
	}

	return 0;
}

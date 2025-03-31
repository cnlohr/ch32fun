#include "ch32fun.h"
#include <stdio.h>

__HIGH_CODE
void flash_rom_beg(uint8_t beg) {
	R8_FLASH_CTRL = 0;
	R8_FLASH_CTRL = 0x5;
	asm volatile ("nop\nnop");
	R8_FLASH_DATA = beg;
	if(beg == 0xff) {
		while((char)R8_FLASH_CTRL < 0);
		R8_FLASH_DATA = beg;
		while((char)R8_FLASH_CTRL < 0);
	}
}

__HIGH_CODE
void flash_rom_end() {
	while((char)R8_FLASH_CTRL < 0);
	R8_FLASH_CTRL = 0;
}

__HIGH_CODE
void flash_start() {
	SYS_SAFE_ACCESS(
		R32_GLOBAL_CONFIG |= 0xe0; // 0xe0 for writing, otherwise 0x20
	);

	R8_FLASH_CTRL = 0x4;
	flash_rom_beg(0xff);
	flash_rom_end();
}

__HIGH_CODE
void flash_rom_out(uint8_t val) {
	while((char)R8_FLASH_CTRL < 0);
	R8_FLASH_DATA = val;
}

__HIGH_CODE
uint8_t flash_rom_in() {
	while((char)R8_FLASH_CTRL < 0);
	return R8_FLASH_DATA;
}

__HIGH_CODE
void flash_rom_addr(uint8_t beg, uint32_t addr) {
	uint8_t repeat = 5;
	if((beg & 0xbf) != 0xb) {
		flash_rom_beg(0x6);
		flash_rom_end();
		repeat = 3;
	}
	flash_rom_beg(beg);
	for(int i = 0; i < repeat; i++) {
		flash_rom_out((uint8_t)(addr >> 0x10));
		addr <<= 8;
	}
}

__HIGH_CODE
uint8_t flash_rom_wait() {
	uint8_t b;
	flash_rom_end();
	for(int i = 0; i < 0x80000; i++) {
		flash_rom_beg(0x5);
		flash_rom_in();
		b = flash_rom_in();
		flash_rom_end();
		if((b & 1) == 0) {
			return b | 1;
		}
	}
	return 0;
}

__HIGH_CODE
void flash_cmd_rom_write(uint32_t StartAddr, uint32_t *Buffer, uint32_t Length ) {
	uint8_t b = 0;
	flash_start();
	do {
		flash_rom_addr(0x2, StartAddr);
		for(int i = 0; i < Length; i++) {
			R32_FLASH_DATA = ((uint32_t*)Buffer)[i];
			for(int j = 0; j < 4; j++) {
				while((char)R8_FLASH_CTRL < 0);
				R8_FLASH_CTRL = 0x15;
			}
		}
		b = flash_rom_wait();
	} while(!b); // !!! this is wrong, libISP5xx has here b != 0. But only like this it works in ch592_minimal??
}

void flash_eeprom_cmd( uint8_t cmd, uint32_t StartAddr, void *Buffer, uint32_t Length ) {
	uint32_t isr0 = NVIC->ISR[0];
	uint32_t isr1 = NVIC->ISR[1];
	NVIC->IRER[0] = 0xffffffff;
	NVIC->IRER[1] = 0xffffffff;

	switch(cmd) {
	case CMD_FLASH_ROM_SW_RESET:
		flash_start();
		flash_rom_beg(0x66);
		flash_rom_end();
		flash_rom_beg(0x99);
		flash_rom_end();
		break;
	case CMD_FLASH_ROM_WRITE:
		flash_cmd_rom_write(StartAddr, (uint32_t*)Buffer, Length >> 2);
		break;
	default:
		break;
	}

	SYS_SAFE_ACCESS(
		R32_GLOBAL_CONFIG &= 0x10;
	);

	NVIC->IENR[0] = isr0;
	NVIC->IENR[1] = isr1;
}

extern void FLASH_EEPROM_CMD( uint8_t cmd, uint32_t StartAddr, void *Buffer, uint32_t Length ); // from libISP5xx
int main()
{
	SystemInit();

	uint8_t buf[4] = "ch32";
	FLASH_EEPROM_CMD(CMD_FLASH_ROM_WRITE, 0x77c, buf, 4); // 0x56c for non blob code
	printf("xxxxfun\n");

	while(1);
}

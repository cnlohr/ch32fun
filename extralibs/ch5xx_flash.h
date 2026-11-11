/*
  Flash library for ch5xx

  The FUNCONF_CH5XXFLASHLIB_SECTION macro is available for tight RAM situations,
  some critical functions need to be ran from RAM. This takes 480 bytes when
  compiled on my machine. With the macro you could do something like this:

  in funconfig.h:
  #define FUNCONF_CH5XXFLASHLIB_SECTION __attribute__( ( section( ".manual_ram" ) ) )

  in the project's linker file:
  .manual_ram_inflash :
  {
    . = ALIGN(4);
    PROVIDE(_flash_manual_ram_start = .);
  } >FLASH AT>FLASH

  .manual_ram :
  {
    . = ALIGN(4);
    PROVIDE(_manual_ram_vma_start = .);
    *(.manual_ram*)
    . = ALIGN(4);
    PROVIDE(_manual_ram_vma_end = .);
  } >RAM AT>FLASH

  in any C source:
  extern const uint32_t _manual_ram_vma_start;
  extern const uint32_t _manual_ram_vma_end;
  extern const uint32_t _flash_manual_ram_start;

  void load_manual_ram_section( void )
  {
    const size_t manual_ram_size = (size_t)( &_manual_ram_vma_end - &_manual_ram_vma_start );
    memcpy( (void *)&_manual_ram_vma_start, (void *)&_flash_manual_ram_start, manual_ram_size );
  }

  // when you don't use the function, you can use the ram area as scratch
  uint8_t *scratch = (uint8_t *)&_manual_ram_vma_start;

  // when need to write to flash:
  load_manual_ram_section();

  // all ch5xx_flash_cmd_* work now:
  ch5xx_flash_cmd_erase(...);

  // when done, just continue using the "scratch" buffer for whatever
*/
#ifdef FUNCONF_CH5XXFLASHLIB_SECTION
#undef __HIGH_CODE
#define __HIGH_CODE FUNCONF_CH5XXFLASHLIB_SECTION
#endif

__HIGH_CODE
uint8_t ch5xx_flash_rom_in() {
	while((int8_t)R8_FLASH_CTRL < 0);
	return R8_FLASH_DATA;
}

__HIGH_CODE
void ch5xx_flash_rom_out(uint8_t val) {
	while((int8_t)R8_FLASH_CTRL < 0);
	R8_FLASH_DATA = val;
}

__HIGH_CODE
void ch5xx_flash_rom_begin(uint8_t cmd) {
	R8_FLASH_CTRL = 0;
	R8_FLASH_CTRL = 0x05;
	asm volatile ("nop\nnop");
	R8_FLASH_DATA = cmd;
	if(cmd == 0xff) {
		while((int8_t)R8_FLASH_CTRL < 0);
		R8_FLASH_DATA = cmd;
		while((int8_t)R8_FLASH_CTRL < 0);
	}
}

__HIGH_CODE
void ch5xx_flash_rom_end() {
	while((int8_t)R8_FLASH_CTRL < 0);
	R8_FLASH_CTRL = 0;
}

__HIGH_CODE
void ch5xx_flash_rom_open(uint8_t op) {
	__disable_irq();

	SYS_SAFE_ACCESS(
		R8_GLOB_ROM_CFG = op;
	);


	R8_FLASH_CTRL = 0x04; // close
	ch5xx_flash_rom_begin(0xff);
	ch5xx_flash_rom_end();
}

__HIGH_CODE
void ch5xx_flash_rom_open_read() {
	ch5xx_flash_rom_open(RB_ROM_CTRL_EN);
}

__HIGH_CODE
void ch5xx_flash_rom_open_erase_write() {
	ch5xx_flash_rom_open(RB_ROM_CTRL_EN | RB_ROM_CODE_WE);
}

__HIGH_CODE
void ch5xx_flash_rom_close() {
	ch5xx_flash_rom_end();

	SYS_SAFE_ACCESS(
		R8_GLOB_ROM_CFG &= RB_ROM_CTRL_EN;
	);

	__enable_irq();
}

__HIGH_CODE
void ch5xx_flash_rom_addr(uint8_t cmd, uint32_t addr) {
	uint8_t repeat = 5;
	if((cmd & 0xbf) != 0x0b) {
		ch5xx_flash_rom_begin(0x06);
		ch5xx_flash_rom_end();
		repeat = 3;
	}
	ch5xx_flash_rom_begin(cmd);
	for(int i = 0; i < repeat; i++) {
		ch5xx_flash_rom_out((uint8_t)(addr >> 0x10));
		addr <<= 8;
	}
}

__HIGH_CODE
uint8_t ch5xx_flash_rom_wait() {
	uint8_t status;
	ch5xx_flash_rom_end();
	for(int i = 0; i < 524288; i++) {
		ch5xx_flash_rom_begin(0x05);
		ch5xx_flash_rom_in();
		status = ch5xx_flash_rom_in();
		ch5xx_flash_rom_end();
		if(!(status & 0x01)) { // check write in progress bit
			return status | 0x01;
		}
	}
	return 0; // timeout
}

__HIGH_CODE
void ch5xx_flash_cmd_unlock(uint8_t cmd) {
	// cmd: 0 - unlock all, 0x44 - lock boot code, 0x50 - lock all code and flash, 0x3c - lock something, unclear what exactly
	ch5xx_flash_rom_open_erase_write();
	ch5xx_flash_rom_wait();
	ch5xx_flash_rom_begin(0x06);
	ch5xx_flash_rom_end();
	ch5xx_flash_rom_begin(0x01);
	ch5xx_flash_rom_out(cmd);
	ch5xx_flash_rom_out(0x02);
	ch5xx_flash_rom_wait();
	ch5xx_flash_rom_close();
}

__HIGH_CODE
void ch5xx_flash_cmd_reset() {
	ch5xx_flash_rom_open_erase_write();
	ch5xx_flash_rom_begin(0x66);
	ch5xx_flash_rom_end();
	ch5xx_flash_rom_begin(0x99);
	ch5xx_flash_rom_close();
}

__HIGH_CODE
void ch5xx_flash_eeprom_read(uint32_t addr, uint8_t* buf, uint32_t len) {
	ch5xx_flash_rom_open_read();
	ch5xx_flash_rom_addr(0xb, addr);
	uint8_t* end_addr = (uint8_t*)((uint32_t)buf + len);
	for (; buf != (uint8_t*)end_addr; buf++) {
		uint8_t data = ch5xx_flash_rom_in();
		*(uint8_t *)buf = (uint8_t)data;
	}
	ch5xx_flash_rom_end();
}

void ch5xx_flash_cmd_read(uint32_t addr, uint8_t *buf, int len) {
	uint32_t *buf32 = (uint32_t*)buf;
	int len32 = (len +3) >> 2;

	for(int i = 0; i < len32; i++) {
		buf32[i] = *((uint32_t*)addr);
		addr += 4;
	}
}

__HIGH_CODE
uint32_t ch5xx_flash_cmd_write(uint32_t addr, uint8_t *buf, int len ) {
	uint32_t *buf32 = (uint32_t*)buf;
	int len32 = (len +3) >> 2;

	ch5xx_flash_rom_open_erase_write();

	while(len32 > 0) {
		ch5xx_flash_rom_addr(0x02, addr);

		while(1) {
			uint32_t data = *buf32++;
			R32_FLASH_DATA = data;
			for(int i = 4; i > 0; i--) {
				while((int8_t)R8_FLASH_CTRL < 0);
				R8_FLASH_CTRL = 0x15;
			}

			len32--;
			addr += 4;
			// Check for page boundary (typically 256 bytes)
			if (len32 == 0 || (addr & 0xff) == 0) break;
		}

		if (ch5xx_flash_rom_wait() == 0)
			return -1;
	}

	ch5xx_flash_rom_close();
	return 0;
}

__HIGH_CODE
uint32_t ch5xx_flash_cmd_erase(uint32_t addr, int len) {
	addr &= 0xfffff000; // start at a Sector boundary
	len = (len + (addr & 0xfff) + 0xfff) & 0xfffff000; // adjust length to Sector boundaries

	ch5xx_flash_rom_open_erase_write();
	uint32_t step;

	while (len > 0) {
		uint8_t cmd;
		if (len >= 65536 && !(addr & (65536 -1))) {
			cmd = 0xd8; // Block Erase
			step = 65536;
		}
#ifdef CH570_CH572
		else if (len >= 32768 && !(addr & (32768 -1))) {
			cmd = 0x52; // Half-Block Erase
			step = 32768;
		}
		else {
			cmd = 0x20; // Sector Erase
			step = 4096;
		}
#else
		else if (len >= 4096 && (addr & (4096 -1))) {
			cmd = 0x20; // Sector Erase
			step = 4096;
		}
		else {
			cmd = 0x81; // Page Erase
			step = 256;
		}
#endif

		ch5xx_flash_rom_addr(cmd, addr);
		if (ch5xx_flash_rom_wait() == 0) {
			ch5xx_flash_rom_close();
			return (uint32_t)-1;
		}
		addr += step;
		len -= step;
	}

	ch5xx_flash_rom_close();
	return 0;
}

__HIGH_CODE
uint32_t ch5xx_flash_cmd_verify(uint32_t addr, uint8_t *buf, int len) {
	uint32_t *buf32 = (uint32_t*)buf;
	int len32 = (len +3) >> 2;

	ch5xx_flash_rom_open_read();
	ch5xx_flash_rom_addr(0x0b, addr); // Fast Read command

	for (int i = 0; i < len32; i++) {
		ch5xx_flash_rom_in(); // Dummy/Data read
		if ((i & 0x03) == 0) {
			if (R32_FLASH_DATA != *buf32++) {
				ch5xx_flash_rom_close();
				return -1;
			}
		}
	}

	ch5xx_flash_rom_close();
	return 0;
}

__HIGH_CODE
void ch5xx_flash_cmd_powerup() {
	ch5xx_flash_rom_open_erase_write();
	ch5xx_flash_rom_begin(0xab);
	ch5xx_flash_rom_close();
}

__HIGH_CODE
void ch5xx_flash_cmd_powerdown() {
	ch5xx_flash_rom_open_erase_write();
	ch5xx_flash_rom_begin(0xb9);
	ch5xx_flash_rom_close();
}

__HIGH_CODE
void ch5xx_flash_cmd_get_rom_info(uint32_t addr, uint8_t *buf) {
	uint32_t *buf32 = (uint32_t*)buf;

	ch5xx_flash_rom_open_read();
	ch5xx_flash_rom_addr(0x0b, addr);
	for (int i = 0; i < 8; i++) {
		ch5xx_flash_rom_in();
		if(i == 3) {
			buf32[0] = R32_FLASH_DATA;
		}
	}
	uint32_t val = R32_FLASH_DATA;
	if (!((addr << 18) & 0x80000000)) {
		buf32[1] = val;
	}
	else {
		((uint16_t*)buf32)[2] = (uint16_t)val; // Store 16-bit at offset 4
	}
	ch5xx_flash_rom_close();
}

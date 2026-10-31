/*
	Set of functions to program CH5xx series of chips by WCH.
	For use with rv003usb rvswdio programmer.
	Copyright 2025-2026 monte-monte
*/

#include <stdio.h>
#include <stdint.h>
#include "funprog_ch5xx.h"
#include "funprog_rvswdio.h"

// Assembly for these binary blobs can be found it the attic folder
__attribute__ ((aligned(4))) unsigned char ch5xx_write_block_bin[236] = {
	0x80, 0x41, 0xc4, 0x41, 0xe3, 0x9e, 0x84, 0xfe, 0x65, 0xfc, 0x1a, 0x85,
	0x23, 0x83, 0x06, 0x00, 0x15, 0x47, 0x23, 0x83, 0xe6, 0x00, 0x19, 0x47,
	0x23, 0x82, 0xe6, 0x00, 0x03, 0x87, 0x66, 0x00, 0xe3, 0x4e, 0x07, 0xfe,
	0x23, 0x83, 0x06, 0x00, 0x23, 0x83, 0x06, 0x00, 0x15, 0x47, 0x23, 0x83,
	0xe6, 0x00, 0x09, 0x47, 0x23, 0x82, 0xe6, 0x00, 0x0d, 0x46, 0x93, 0x52,
	0x05, 0x01, 0x93, 0xf2, 0xf2, 0x0f, 0x03, 0x87, 0x66, 0x00, 0xe3, 0x4e,
	0x07, 0xfe, 0x23, 0x82, 0x56, 0x00, 0x22, 0x05, 0x7d, 0x16, 0x65, 0xf6,
	0x23, 0xa0, 0x05, 0x00, 0x93, 0x07, 0x00, 0x04, 0x80, 0x41, 0xc4, 0x41,
	0xb5, 0xec, 0x6d, 0xdc, 0x80, 0xc2, 0x11, 0x47, 0x03, 0x86, 0x66, 0x00,
	0xe3, 0x4e, 0x06, 0xfe, 0x55, 0x46, 0x23, 0x83, 0xc6, 0x00, 0x7d, 0x17,
	0x65, 0xfb, 0xfd, 0x17, 0x23, 0xa0, 0x05, 0x00, 0xf1, 0xff, 0x13, 0x03,
	0x03, 0x10, 0x23, 0xa0, 0x65, 0x00, 0xb7, 0x02, 0x08, 0x00, 0x03, 0x87,
	0x66, 0x00, 0xe3, 0x4e, 0x07, 0xfe, 0x23, 0x83, 0x06, 0x00, 0x23, 0x83,
	0x06, 0x00, 0x15, 0x47, 0x23, 0x83, 0xe6, 0x00, 0x23, 0x82, 0xe6, 0x00,
	0x03, 0x87, 0x66, 0x00, 0xe3, 0x4e, 0x07, 0xfe, 0x03, 0xc6, 0x46, 0x00,
	0x03, 0x87, 0x66, 0x00, 0xe3, 0x4e, 0x07, 0xfe, 0x03, 0x86, 0x46, 0x00,
	0x03, 0x87, 0x66, 0x00, 0xe3, 0x4e, 0x07, 0xfe, 0x23, 0x83, 0x06, 0x00,
	0x13, 0x77, 0x16, 0x00, 0x11, 0xe3, 0x25, 0xbf, 0xfd, 0x12, 0xe3, 0x92,
	0x02, 0xfc, 0x02, 0x90, 0x23, 0xa2, 0x05, 0x00, 0xe3, 0x87, 0xb4, 0xfa,
	0x23, 0xa0, 0x06, 0x00, 0x11, 0x47, 0xbd, 0xbf
};
unsigned int ch5xx_write_block_bin_len = 236;

// By default all chips run at the lowest possible frequency. (For example CH585 runs at 5.33MHz)
// Some of them needs to set it higher to be able to be programmed reliably.
// WCH-LinkE sets needed frequency automatically with it's built-in functions. But it will be reset if we reset the chip.
// Also we still need to do that every time we use other programmers.
int ch5xx_set_clock(struct SWIOState * iss, uint32_t clock) {
	struct SWIOState * dev = iss;
	int r = 0;
	if (clock == 0) {
		uint32_t rr = 0;
		r = MCFReadWord(dev, 0x40001008, &rr);
		switch (iss->target_chip_type)
		{
		case CHIP_CH585:
			iss->clock_set = 24000;
			if ((rr&0xfff) != 0x14d) {
				ch5xx_write_safe(dev, 0x4000100A, 0x16, 0);
				ch5xx_write_safe(dev, 0x40001008, 0x14d, 1);
			} else {
				return r;
			}
			break;

		case CHIP_CH570:
			iss->clock_set = 20000;
			if ((rr&0xff) != 0x5e) {
				ch5xx_write_safe(dev, 0x4000100A, 0x14, 0); // Enable PLL
				// Set flash clock (undocumented)
				// Before it was 75MHz and we needed to set flash clock divider
				// but there were some stability issues, so now it's 20Mhz
				// ch5xx_write_safe(dev, 0x40001805, 0x8, 0); // Flash SCK 50MHz - doesn't work on funprog for some reason
				// ch5xx_write_safe(dev, 0x40001807, 0x1, 0); // Flash CFG something about clock source (PLL)
				// Set core clock
				ch5xx_write_safe(dev, 0x40001008, 0x5e, 0); // 20MHz
				// Disable watchdog
				MCFWriteWord(dev, 0x40001000, 0x5555);
				MCFWriteWord(dev, 0x40001004, 0x7fff);
			} else {
				return r;
			}
			break;

		case CHIP_CH57x:
		case CHIP_CH58x:
		case CHIP_CH59x:
			iss->clock_set = 16000;
			if ((rr&0xff) != 0x82) {
				ch5xx_write_safe(dev, 0x40001008, 0x82, 0);
			} else {
				return r;
			}
			break;
		
		default:
			return 0;
		}
	}
	return r;
}

// Some critical registers of CH5xx are only writeable in "safe mode", this requires a special procedure.
// Such registers marked "RWA" in the datasheet
void ch5xx_write_safe(struct SWIOState * iss, uint32_t addr, uint32_t value, uint8_t mode) {
	struct SWIOState * dev = iss;

	if ((iss->statetag & 0xFFFF00FF) != (STTAG( "5WWS" ) &0xFFFF00FF)) {
		MCFWriteReg32(dev, DMABSTRACTAUTO, 0x00000000); // Disable Autoexec.

		MCFWriteReg32(dev, BDMDATA0, 0x40001040);
		MCFWriteReg32(dev, DMCOMMAND, 0x00230000 | 0x100d); // Write a3 from DATA0.
		MCFWriteReg32(dev, BDMDATA0, 0x57);
		MCFWriteReg32(dev, DMCOMMAND, 0x00230000 | 0x100e); // Write a4 from DATA0.
		MCFWriteReg32(dev, BDMDATA0, 0xa8);
		MCFWriteReg32(dev, DMCOMMAND, 0x00230000 | 0x100f); // Write a5 from DATA0.

		MCFWriteReg32(dev, DMPROGBUF0, 0x00e68023); // sb  a4,0x0(a3)
		MCFWriteReg32(dev, DMPROGBUF1, 0x00f68023); // sb  a5,0x0(a3)
		MCFWriteReg32(dev, DMPROGBUF2, 0x00010001); // c.nop c.nop
		MCFWriteReg32(dev, DMPROGBUF4, 0x00100073); // c.ebreak
	}

	if (mode == 0 && iss->statetag != STTAG( "5WBS" )) {
		MCFWriteReg32(dev, DMPROGBUF3, 0x00c28023); // sb  a2,0x0(t0)
		iss->statetag = STTAG( "5WBS" );
	} else if (mode == 1 && iss->statetag != STTAG( "5WHS" )) {
		MCFWriteReg32(dev, DMPROGBUF3, 0x00c29023); // sh  a2,0x0(t0)  
		iss->statetag = STTAG( "5WHS" );
	} else if (mode == 2 && iss->statetag != STTAG( "5WBS" )) {
		MCFWriteReg32(dev, DMPROGBUF3, 0x00c2a023); // sw  a2,0x0(t0)  
		iss->statetag = STTAG( "5WWS" );
	}

	MCFWriteReg32(dev, BDMDATA0, addr);
	MCFWriteReg32(dev, DMCOMMAND, 0x00230000 | 0x1005); // Write t0 from DATA0.
	MCFWriteReg32(dev, BDMDATA0, value);
	MCFWriteReg32(dev, DMCOMMAND, 0x00230000 | 0x100c); // Write a2 from DATA0.

	MCFWriteReg32(dev, DMCOMMAND, 0x00271000); // Execute program.

	MCFWaitForDoneOp(dev);
	iss->currentstateval = -1;

}

uint8_t ch5xx_flash_begin(struct SWIOState * iss, uint8_t cmd) {
	struct SWIOState * dev = iss;

	if (iss->statetag != STTAG("FBEG")) {
		if ((iss->statetag & 0xff) != 'F') {
			MCFWriteReg32(dev, DMABSTRACTAUTO, 0x00000000); // Disable Autoexec.
			MCFWriteReg32(dev, BDMDATA0, R32_FLASH_DATA_T);
			MCFWriteReg32(dev, DMCOMMAND, 0x00230000 | 0x100d); // Write a3 from DATA0.
		}
		MCFWriteReg32(dev, DMPROGBUF0, 0x00068323); // sb zero,6(a3);
		MCFWriteReg32(dev, DMPROGBUF1, 0x00014715); // c.li a4,5; c.nop;
		MCFWriteReg32(dev, DMPROGBUF2, 0x00e68323); // sb a4,6(a3);
		MCFWriteReg32(dev, DMPROGBUF3, 0x00f68223); // sb a5,4(a3);
		MCFWriteReg32(dev, DMPROGBUF4, 0x00100073); // c.ebreak

		iss->statetag = STTAG("FBEG");
	}

	MCFWriteReg32(dev, BDMDATA0, cmd); // Write command to a5
	MCFWriteReg32(dev, DMCOMMAND, 0x0027100f); // Execute program.
	
	return cmd;
}

void ch5xx_flash_end(struct SWIOState * iss) {
	struct SWIOState * dev = iss;

	if (iss->statetag != STTAG("FEND")) {
		if ((iss->statetag & 0xff) != 'F') {
			MCFWriteReg32(dev, DMABSTRACTAUTO, 0x00000000); // Disable Autoexec.
			MCFWriteReg32(dev, BDMDATA0, R32_FLASH_DATA_T);
			MCFWriteReg32(dev, DMCOMMAND, 0x00230000 | 0x100d); // Write a3 from DATA0.
		}
		MCFWriteReg32(dev, DMPROGBUF0, 0x00668703); // lb a4,6(a3);
		MCFWriteReg32(dev, DMPROGBUF1, 0xfe074ee3); // blt a4,zero,-4;
		MCFWriteReg32(dev, DMPROGBUF2, 0x00068323); // sb zero,6(a3);
		MCFWriteReg32(dev, DMPROGBUF3, 0x00100073); // c.ebreak

		iss->statetag = STTAG("FEND");
	}

	MCFWriteReg32(dev, DMCOMMAND, 0x00271000); // Execute program.
}

void ch5xx_flash_out(struct SWIOState * iss, uint8_t addr)
{
	struct SWIOState * dev = iss;

	if (iss->statetag != STTAG("FOUT")) {
		if ((iss->statetag & 0xff) != 'F') {
			MCFWriteReg32(dev, DMABSTRACTAUTO, 0x00000000); // Disable Autoexec.
			MCFWriteReg32(dev, BDMDATA0, R32_FLASH_DATA_T);
			MCFWriteReg32(dev, DMCOMMAND, 0x00230000 | 0x100d); // Write a3 from DATA0.
		}
		MCFWriteReg32(dev, DMPROGBUF0, 0x00668703); // lb a4,6(a3);
		MCFWriteReg32(dev, DMPROGBUF1, 0xfe074ee3); // blt a4,zero,-4;
		MCFWriteReg32(dev, DMPROGBUF2, 0x00f68223); // sb a5,4(a3);
		MCFWriteReg32(dev, DMPROGBUF3, 0x00100073); // c.ebreak

		iss->statetag = STTAG("FOUT");  
	}

	MCFWriteReg32(dev, BDMDATA0, addr); // Write command to a4
	MCFWriteReg32(dev, DMCOMMAND, 0x0027100f); // Execute program.
	
}

uint8_t ch5xx_flash_in(struct SWIOState * iss)
{
	struct SWIOState * dev = iss;
	uint8_t r;

	if (iss->statetag != STTAG("FLIN")) {
		if ((iss->statetag & 0xff) != 'F') {
			MCFWriteReg32(dev, DMABSTRACTAUTO, 0x00000000); // Disable Autoexec.
			MCFWriteReg32(dev, BDMDATA0, R32_FLASH_DATA_T);
			MCFWriteReg32(dev, DMCOMMAND, 0x00230000 | 0x100d); // Write a3 from DATA0.
		}
		MCFWriteReg32(dev, DMPROGBUF0, 0x00668703); // lb a4,6(a3);
		MCFWriteReg32(dev, DMPROGBUF1, 0xfe074ee3); // blt a4,zero,-4;
		MCFWriteReg32(dev, DMPROGBUF2, 0x00468783); // lb a5,4(a3);
		MCFWriteReg32(dev, DMPROGBUF3, 0x00100073); // c.ebreak

		iss->statetag = STTAG( "FLIN" );  
	}

	MCFWriteReg32(dev, DMCOMMAND, 0x00271000); // Execute program.
	
	r = MCFWaitForDoneOp(dev);
	if (r) return 0;

	uint32_t rr;
	MCFWriteReg32(dev, DMCOMMAND, 0x0022100f); // Read a5 into DATA0.
	MCFReadReg32(dev, BDMDATA0, &rr);
	r = rr & 0xff;
	
	return r;
}

uint8_t ch5xx_flash_addr(struct SWIOState * iss, uint8_t cmd, uint32_t addr) {
  struct SWIOState * dev = iss;
	uint8_t ret;
	int len = 5;

	if ((cmd & 0xbf) != 0xb) {
		ch5xx_flash_begin(dev, 6);
		ch5xx_flash_end(dev);
		
		len = 3;
	}
	ret = ch5xx_flash_begin(dev, cmd);
	while (len--, len != -1) {
		ch5xx_flash_out(dev, (addr >> 16) & 0xff);
		addr = addr << 8;
	}
	return ret;
}

uint8_t ch5xx_flash_open(struct SWIOState * iss, uint8_t op) {
	struct SWIOState * dev = iss;

	uint8_t glob_rom_cfg;
	MCFReadByte(dev, 0x40001044, &glob_rom_cfg);
	// //fprintf(stderr, "RS = %02x, op = %02x\n", glob_rom_cfg, op);
	if ((glob_rom_cfg & 0xe0) != op) {
		ch5xx_write_safe(dev, 0x40001044, op, 0);
		MCFReadByte(dev, 0x40001044, &glob_rom_cfg);
		// //fprintf(stderr, "RS = %02x\n", glob_rom_cfg);
	}
	MCFWriteByte(dev, 0x40001806, 4);
	if (iss->target_chip_type == CHIP_CH570){
		ch5xx_flash_begin(dev, 0xff);
		ch5xx_flash_out(dev, 0xff);
		// ch5xx_flash_in(dev);
	} else {
		ch5xx_flash_begin(dev, 0xff);
	}
	ch5xx_flash_end(dev);
	return glob_rom_cfg;
}

void ch5xx_flash_close(struct SWIOState * iss) {
	struct SWIOState * dev = iss;
	uint8_t glob_rom_cfg;
	MCFReadByte(dev, 0x40001044, &glob_rom_cfg);
	ch5xx_flash_end(dev);
	ch5xx_write_safe(dev, 0x40001044, glob_rom_cfg & 0x10, 0);
}

uint8_t ch5xx_flash_wait(struct SWIOState * iss) {
	struct SWIOState * dev = iss;
	uint32_t timer = 0x80000;
	uint8_t ret = 0;
	ch5xx_flash_end(dev);
	do {
		ch5xx_flash_begin(dev, 5);
		ch5xx_flash_in(dev);
		ret = ch5xx_flash_in(dev);
		ch5xx_flash_end(dev);
		if ((ret & 1) == 0) {
			return (ret & 0xff) | 1;
		}
		// MCFDelayUS(dev, 100);
		timer--;
	} while (timer != 0);
	return 0;
}

int ch5xx_read_eeprom(struct SWIOState * iss, uint32_t addr, uint8_t* buffer, uint32_t len) {
	struct SWIOState * dev = iss;

	enum RiscVChip chip = iss->target_chip_type;
	uint32_t rrv;
	int r;

	ch5xx_flash_open(dev, 0x20);

	if (chip == CHIP_CH57x ||
			chip == CHIP_CH58x ||
			chip == CHIP_CH585 ||
			chip == CHIP_CH59x)
	{
		ch5xx_flash_addr(dev, 0xb, (addr | 0x80000));
	} else if (chip == CHIP_CH570) {
		return -1;
	}

	if (iss->statetag != STTAG("FREP")) {
		if ((iss->statetag & 0xff) != 'F') {
			MCFWriteReg32(dev, DMABSTRACTAUTO, 0x00000000); // Disable Autoexec.
			MCFWriteReg32(dev, BDMDATA0, R32_FLASH_DATA_T);
			MCFWriteReg32(dev, DMCOMMAND, 0x00230000 | 0x100d); // Write a3 from DATA0.
		}

		MCFWriteReg32(dev, DMPROGBUF0, 0x00668703); // lb a4,6(a3);
		MCFWriteReg32(dev, DMPROGBUF1, 0xfe074ee3); // blt a4,zero,-4;
		MCFWriteReg32(dev, DMPROGBUF2, 0x00468583); // lb a1,4(a3);
		MCFWriteReg32(dev, DMPROGBUF7, 0xfe5ff06f); // jal zero,-28
		iss->statetag = STTAG("FREP");
	}
	
	for (int i = 0; i < len; i++) {
		uint32_t local_buffer = 0;
		MCFWriteReg32(dev, DMCOMMAND, 0x00271000); // Execute program.
		do {
			r = MCFReadReg32(dev, DMABSTRACTCS, &rrv);
			if(r) return r;
			if (rrv & (0x700)) {
				return rrv;
			}
		} while((rrv & (1<<12)));
		MCFWriteReg32(dev, DMCOMMAND, 0x0022100b); // Read a0 into DATA0.
		MCFReadReg32(dev, BDMDATA0, &local_buffer);
		*(buffer + i) = local_buffer & 0xff;
	}
	
	ch5xx_flash_close(dev);
	return 0;
}

int ch5xx_read_options(struct SWIOState * iss, uint32_t addr, uint8_t* buffer) {
	struct SWIOState * dev = iss;

	enum RiscVChip chip = iss->target_chip_type;

	ch5xx_flash_open(dev, 0x20);

	if (chip == CHIP_CH57x ||
	    chip == CHIP_CH58x ||
	    chip == CHIP_CH585 ||
	    chip == CHIP_CH59x)
	{
		ch5xx_flash_addr(dev, 0xb, (addr | 0x80000));
	} else if (chip == CHIP_CH570) {
		ch5xx_flash_addr(dev, 0xb, addr);
	}

	if (iss->statetag != STTAG("FOPT")) {
		if ((iss->statetag & 0xff) != 'F') {
			MCFWriteReg32(dev, DMABSTRACTAUTO, 0x00000000); // Disable Autoexec.
			MCFWriteReg32(dev, BDMDATA0, R32_FLASH_DATA_T);
			MCFWriteReg32(dev, DMCOMMAND, 0x00230000 | 0x100d); // Write a3 from DATA0.
		}
		MCFWriteReg32(dev, BDMDATA0, 4);
		MCFWriteReg32(dev, DMCOMMAND, 0x00230000 | 0x100f); // Write a5 from DATA0
		MCFWriteReg32(dev, BDMDATA0, 8);
		MCFWriteReg32(dev, DMCOMMAND, 0x00230000 | 0x100c); // Write a2 from DATA0

		MCFWriteReg32(dev, DMPROGBUF0, 0x00668703); // lb a4,6(a3);
		MCFWriteReg32(dev, DMPROGBUF1, 0xfe074ee3); // blt a4,zero,-4;
		MCFWriteReg32(dev, DMPROGBUF2, 0x00468583); // lb a1,4(a3);
		MCFWriteReg32(dev, DMPROGBUF3, 0xc9e3167d); // c.addi a2,-1; blt a5,a2,-14 [0xfec7c9e3]
		MCFWriteReg32(dev, DMPROGBUF4, 0xe219fec7); //               c.bnez a2,6
		MCFWriteReg32(dev, DMPROGBUF5, 0x9002428c); // c.lw a1,0(a3) c.ebrake 
		MCFWriteReg32(dev, DMPROGBUF6, 0x17f14288); // c.lw a0,0(a3) c.addi a5,-4
		MCFWriteReg32(dev, DMPROGBUF7, 0xfe5ff06f); // jal zero,-28
		iss->statetag = STTAG("FOPT");
	}
	
	MCFWriteReg32(dev, DMCOMMAND, 0x00271000); // Execute program.
	
	uint32_t rrv;
	int r;
	do {
		r = MCFReadReg32(dev, DMABSTRACTCS, &rrv);
		if(r) return r;
		if (rrv & (0x700)) {
			//fprintf(stderr, "Error in ch5xx_read_options: %08x\n", rrv);
			return rrv;
		}
	} while((rrv & (1<<12)));
	
	MCFWriteReg32(dev, DMCOMMAND, 0x0022100a); // Read a0 into DATA0.
	MCFReadReg32(dev, BDMDATA0, (uint32_t*)buffer);
	MCFWriteReg32(dev, DMCOMMAND, 0x0022100b); // Read a1 into DATA0.
	MCFReadReg32(dev, BDMDATA0, (uint32_t*)(buffer+4));

	ch5xx_flash_close(dev);
	return 0;
}

int ch5xx_read_options_bulk(struct SWIOState * iss, uint32_t addr, uint8_t* buffer, uint32_t len) {
	struct SWIOState * dev = iss;

	if(!iss->target_chip_type) {
		MCFDetermineChipType(dev);
	}
	enum RiscVChip chip = iss->target_chip_type;

	int r;
	uint32_t dmdata0;

	uint32_t dmdata0_offset = 0xe0000380;
	MCFReadReg32(dev, DMHARTINFO, &dmdata0_offset);
	dmdata0_offset = 0xe0000000 | (dmdata0_offset & 0x7ff);

	ch5xx_flash_open(dev, 0x20);

	if (chip == CHIP_CH57x ||
	    chip == CHIP_CH58x ||
	    chip == CHIP_CH585 ||
	    chip == CHIP_CH59x)
	{
		ch5xx_flash_addr(dev, 0xb, (addr | 0x80000));
	} else if (chip == CHIP_CH570) {
		ch5xx_flash_addr(dev, 0xb, (addr | 0x40000));
	}
	
	if (iss->statetag != STTAG("FOPB") || iss->statetag != STTAG("FVER")) {
		if ((iss->statetag & 0xff) != 'F') {
			MCFWriteReg32(dev, DMABSTRACTAUTO, 0x00000000); // Disable Autoexec.
			MCFWriteReg32(dev, BDMDATA0, R32_FLASH_DATA_T);
			MCFWriteReg32(dev, DMCOMMAND, 0x00230000 | 0x100d); // Write a3 from DATA0.
		}
		MCFWriteReg32(dev, BDMDATA0, dmdata0_offset);
		MCFWriteReg32(dev, DMCOMMAND, 0x00230000 | 0x100a); // Write a0 from DATA0.

		MCFWriteReg32(dev, DMPROGBUF0, 0x47910001); // c.nop; li a5,4;
		MCFWriteReg32(dev, DMPROGBUF1, 0x00668703); // lb a4,6(a3);
		MCFWriteReg32(dev, DMPROGBUF2, 0xfe074ee3); // blt a4,zero,-4;
		MCFWriteReg32(dev, DMPROGBUF3, 0x00468703); // lb a4,4(a3);
		MCFWriteReg32(dev, DMPROGBUF4, 0xfbed17fd); // c.addi a5,-1; c.bnez a5,-14;
		MCFWriteReg32(dev, DMPROGBUF5, 0xc10c428c); // c.lw a1,0(a3); c.sw a1,0,(a0);
		MCFWriteReg32(dev, DMPROGBUF6, 0x00100073); // ebreak

		iss->statetag = STTAG("FOPB");
	}

for (int i = 0; i < len; i += 4) {
		uint8_t timeout = 100;
		MCFWriteReg32(dev, DMCOMMAND, 0x00271000); // Execute program.
		do {
			r = MCFReadReg32(dev, DMABSTRACTCS, &dmdata0);
			if(r) return r;
			if (dmdata0 & (0x700)) {
				//fprintf(stderr, "Error in ch5xx_read_option_bulk: %08x\n", dmdata0);
				return -2;
			}
			timeout--;
		} while((dmdata0 & (1<<12)) && timeout);
		MCFReadReg32(dev, BDMDATA0, (uint32_t*)(buffer + i));
	}

	ch5xx_flash_close(dev);
	return 0;
}

// On some of CH5xx chips (CH592 for example) there is mysterious second UUID.
// What's the purpose of it? It's not documented, it's only mentioned with the function that reads it.
int ch5xx_read_secret_uuid(struct SWIOState * iss, uint8_t * buffer) {
	struct SWIOState * dev = iss;
	if(!iss->target_chip_type) {
		MCFDetermineChipType(dev);
	}

	iss->statetag = STTAG("5SID");

	uint8_t local_buffer[8] = {0, 0, 0, 0, 0, 0, 0, 0};
	
	uint32_t dmdata0_offset = 0xe0000380;
	MCFReadReg32(dev, DMHARTINFO, &dmdata0_offset);
	dmdata0_offset = 0xe0000000 | (dmdata0_offset & 0x7ff);

	ch5xx_flash_open(dev, 0x20);
	ch5xx_flash_addr(dev, 0x4b, 0);
	
	MCFWriteReg32(dev, BDMDATA0, 0xffffffff);
	MCFWriteReg32(dev, DMCOMMAND, 0x00230000 | 0x1005); // Write t0 from DATA0
	MCFWriteReg32(dev, BDMDATA0, dmdata0_offset);
	MCFWriteReg32(dev, DMCOMMAND, 0x00230000 | 0x100a); // Write a0 from DATA0
	MCFWriteReg32(dev, BDMDATA0, 0xf);
	MCFWriteReg32(dev, DMCOMMAND, 0x00230000 | 0x100b); // Write a1 from DATA0
	MCFWriteReg32(dev, BDMDATA0, 0);
	MCFWriteReg32(dev, BDMDATA1, 0);

	MCFWriteReg32(dev, DMPROGBUF0, 0x00668703); // lb a4,6(a3);
	MCFWriteReg32(dev, DMPROGBUF1, 0xfe074ee3); // blt a4,zero,-4;
	MCFWriteReg32(dev, DMPROGBUF2, 0xf79342d8); // c.lw a4,4(a3); andi a5,a1,0x7;
	MCFWriteReg32(dev, DMPROGBUF3, 0x97aa0075); //                c.add a5,a0;
	MCFWriteReg32(dev, DMPROGBUF4, 0x00078603); // lb a2,0x0(a5);
	MCFWriteReg32(dev, DMPROGBUF5, 0x8f3115fd); // c.addi a1,-1; c.xor a4,a2;
	MCFWriteReg32(dev, DMPROGBUF6, 0x00e78023); // sb a4,0(a5);
	MCFWriteReg32(dev, DMPROGBUF7, 0x00100073); // c.ebreak
	// MCFWriteReg32(dev, DMPROGBUF7, 0xfe5592e3); // bne a1,t0,-28;
	// MCFWriteReg32(dev, DMCOMMAND, 0x00271000); // Execute program.
	uint32_t rrv;
	int r;
	for (int i = 0xf; i > -1; i--) {
		MCFWriteReg32(dev, DMCOMMAND, 0x00271000); // Execute program.
		do {
			r = MCFReadReg32(dev, DMABSTRACTCS, &rrv);
			if(r) return r;
			if (rrv & (0x700)) {
				MCFWriteReg32(dev, DMABSTRACTCS, 0x08000700); // Clear out any dmabstractcs errors.
			}
		} while((rrv & (1<<12)));
	}
	MCFReadReg32(dev, BDMDATA0, (uint32_t*)local_buffer);
	MCFReadReg32(dev, BDMDATA1, (uint32_t*)(local_buffer+4));

	for (int i = 0; i < 8; i++) {
		buffer[i] = local_buffer[i];
	}

	ch5xx_flash_close(dev);
	return 0;
}

// All CH5xx have UUID which is also used to produce unique MAC address for BLE
int ch5xx_read_uuid(struct SWIOState * iss, uint8_t * buffer) {

	struct SWIOState * dev = iss;
	if(!iss->target_chip_type) {
		MCFDetermineChipType(dev);
	}

	uint32_t addr = 0x7F018;
	if (iss->target_chip_type == CHIP_CH570) {
		addr = 0x3F018;
	}

	int r = MCFReadWord(dev, addr, (uint32_t*)(buffer));
	if (r) {
		return r;
	}
	r = MCFReadWord(dev, addr+4, (uint32_t*)(buffer+4));
	if (r) {
		return r;
	}

	return 0;
}

int ch5xx_microblob_init(struct SWIOState * iss, uint32_t start_addr) {
	struct SWIOState * dev = iss;
	int r = 0;
	
	uint32_t dmdata0_offset = 0xe0000380;
	uint32_t ram_base = 0x20000000;
	if (iss->target_chip_type == CHIP_CH57x) ram_base = 0x20003800;
	MCFReadReg32(dev, DMHARTINFO, &dmdata0_offset);
	dmdata0_offset = 0xe0000000 | (dmdata0_offset & 0x7ff);
	
	if (iss->microblob_running < -1) {
		for (int i = 0; i < ch5xx_write_block_bin_len; i+=4) {
			MCFWriteWord(dev, ram_base+i, *((uint32_t*)(ch5xx_write_block_bin + i)));
		}
	}

	ch5xx_flash_open(dev, 0xe0);

	if ((iss->statetag & 0xff) != 'F') {
		MCFWriteReg32(dev, DMABSTRACTAUTO, 0x00000000); // Disable Autoexec.
		MCFWriteReg32(dev, BDMDATA0, R32_FLASH_DATA_T);
		MCFWriteReg32(dev, DMCOMMAND, 0x00230000 | 0x100d); // Write a3 from DATA0.
	}
	MCFWriteReg32(dev, BDMDATA0, dmdata0_offset); // DMDATA0 offset
	MCFWriteReg32(dev, DMCOMMAND, 0x00230000 | 0x100b); // Write a1 from DATA0.
	
	MCFWriteReg32(dev, BDMDATA0, start_addr);
	MCFWriteReg32(dev, DMCOMMAND, 0x00230000 | 0x1006); // Write t1 from DATA0.

	MCFWriteReg32(dev, BDMDATA0, 0x000090c3);
	MCFWriteReg32(dev, DMCOMMAND, 0x002307b0);
	MCFWriteReg32(dev, BDMDATA0, 0); 
	MCFWriteReg32(dev, DMCOMMAND, 0x00230300); // Clear mstatus
	MCFWriteReg32(dev, BDMDATA0, ram_base); 
	MCFWriteReg32(dev, DMCOMMAND, 0x002307b1); // Write dpc from DATA0.
	
	MCFWriteReg32(dev, BDMDATA1, 1);
	MCFWriteReg32(dev, DMCONTROL, 0x40000001);
	if (iss->target_chip_type == CHIP_CH570 || iss->target_chip_type == CHIP_CH585) MCFWriteReg32(dev, DMCONTROL, 0x40000001);

	MCFWriteReg32(dev, BDMDATA0, 0);
	MCFWriteReg32(dev, BDMDATA1, 0);
	
	iss->microblob_running = start_addr;
	return r;
}

void ch5xx_microblob_end(struct SWIOState * iss) {
	struct SWIOState * dev = iss;
	uint32_t dmdata0_offset = 0xe0000380;
	MCFReadReg32(dev, DMHARTINFO, &dmdata0_offset);
	dmdata0_offset = 0xe0000000 | (dmdata0_offset & 0x7ff);
	MCFWriteReg32(dev, BDMDATA1, dmdata0_offset); // Signal to microblob that we're done writing
	MCFDelayUS(10000);
	MCFWriteReg32(dev, DMCONTROL, 0x80000001);
	ch5xx_flash_close(dev);
	iss->microblob_running = -1;
}

int ch5xx_write_flash_using_microblob(struct SWIOState * iss, uint32_t start_addr, uint8_t* data, uint32_t len) {
	struct SWIOState * dev = iss;
	int r = 0;
	if ( iss->microblob_running < 0 ) ch5xx_microblob_init(iss, start_addr);
	uint32_t dmdata0;
	uint8_t timer = 0;
	uint32_t byte = 0;
	while(byte < len) {
		// uint32_t current_word = *((uint32_t*)(data+byte));
		uint32_t current_word = data[byte] | data[byte+1] << 8 | data[byte+2] << 16 | data[byte+3] << 24;

		if (current_word) MCFWriteReg32( dev, BDMDATA0, current_word);
		else MCFWriteReg32( dev, BDMDATA1, 2);
		byte += 4;
		// Wait every block to be written to flash and also new address set it takes ~1.5-2ms
		if(!(byte & 0xFF)) {
			do {
				if ((timer++) > 200) {
					return -2;
				}
				MCFReadReg32(dev, BDMDATA0, &dmdata0);
			} while(dmdata0);
			timer = 0;
		}
	}
	return r;
}

// Writing flash using only PROGBUF. It's slower, but it doesn't touch RAM.
int ch5xx_write_flash(struct SWIOState * iss, uint32_t start_addr, uint8_t* data, uint32_t len) {
	struct SWIOState * dev = iss;

	uint32_t position = 0;

	if (iss->writing_flash == 0) {
		ch5xx_flash_open(dev, 0xe0);
		iss->writing_flash = 1;
	}
	
	while(len) {
		ch5xx_flash_addr(dev, 2, start_addr);

		if (iss->statetag != STTAG("FWRT")) {
			if ((iss->statetag & 0xff) != 'F') {
				MCFWriteReg32(dev, DMABSTRACTAUTO, 0x00000000); // Disable Autoexec.
				MCFWriteReg32(dev, BDMDATA0, R32_FLASH_DATA_T);
				MCFWriteReg32(dev, DMCOMMAND, 0x00230000 | 0x100d); // Write a3 from DATA0.
			}
			
			MCFWriteReg32(dev, BDMDATA0, 21);
			MCFWriteReg32(dev, DMCOMMAND, 0x00230000 | 0x1005); // Write t0 from DATA0.
			
			MCFWriteReg32(dev, DMPROGBUF0, 0x4791c298); // c.sw a4,0(a3); c.li a5,0x4;
			MCFWriteReg32(dev, DMPROGBUF1, 0x00668703); // lb a4,6(a3);
			MCFWriteReg32(dev, DMPROGBUF2, 0xfe074ee3); // blt a4,zero,-4;
			MCFWriteReg32(dev, DMPROGBUF3, 0x00568323); // sb t0,6(a3);
			MCFWriteReg32(dev, DMPROGBUF4, 0xfbed17fd); // c.addi a5,-1; c.bnez a5,-14;
			MCFWriteReg32(dev, DMPROGBUF5, 0x00100073); // c.ebreak
			iss->statetag = STTAG("FWRT");
		}
		
		int r;
		uint32_t rrv;
		uint32_t block = len>256?256:len;
		
		for (uint32_t i = 0; i < block/4; i++) {
			// uint32_t word = data[position] | data[position+1] << 8 | data[position+2] << 16 | data[position+3] << 24;
			// MCFWriteReg32(dev, BDMDATA0, word);
			MCFWriteReg32(dev, BDMDATA0, *((uint32_t*)(data+position)));
			MCFWriteReg32(dev, DMCOMMAND, 0x0027100e); // Write a4 and execute.
			
			do {
				r = MCFReadReg32(dev, DMABSTRACTCS, &rrv);
				if(r) return r;
				if (rrv & (0x700)) {
					return rrv;
				}
			} while((rrv & (1<<12)));
			position += 4;
		}

		r = ch5xx_flash_wait(dev);
		len -= block;
		start_addr += block;
	}
	// ch5xx_flash_close(dev);
	
	return 0;
}

// This is only present in CH570/2 (as far as I know). I believe this serves the same purpose as "write safe" procedure.
// Apparently the difference is that it enters "safe mode" permanently until it's closed, or chip is reset.
// Maybe this is related to increased number of RWA registers in CH570/2.
void ch570_disable_acauto(struct SWIOState * iss) {
	struct SWIOState * dev = iss;
	iss->statetag = STTAG( "XXXX" );
	
	MCFWriteReg32(dev, DMABSTRACTAUTO, 0x00000000); // Disable Autoexec.

	MCFWriteReg32(dev, BDMDATA0, 0x4000101f);
	MCFWriteReg32(dev, DMCOMMAND, 0x00230000 | 0x1005); // Write t0 from DATA0.
	MCFWriteReg32(dev, BDMDATA0, 0x749da58b);
	MCFWriteReg32(dev, DMCOMMAND, 0x00230000 | 0x100a); // Write a0 from DATA0.
	MCFWriteReg32(dev, BDMDATA0, 0x40001808);
	MCFWriteReg32(dev, DMCOMMAND, 0x00230000 | 0x100b); // Write a1 from DATA0.
	MCFWriteReg32(dev, BDMDATA0, 0);
	MCFWriteReg32(dev, DMCOMMAND, 0x00230000 | 0x100c); // Write a2 from DATA0.
	MCFWriteReg32(dev, BDMDATA0, 0x40001040);
	MCFWriteReg32(dev, DMCOMMAND, 0x00230000 | 0x100d); // Write a3 from DATA0.
	MCFWriteReg32(dev, BDMDATA0, 0x57);
	MCFWriteReg32(dev, DMCOMMAND, 0x00230000 | 0x100e); // Write a4 from DATA0.
	MCFWriteReg32(dev, BDMDATA0, 0xa8);
	MCFWriteReg32(dev, DMCOMMAND, 0x00230000 | 0x100f); // Write a5 from DATA0.

	MCFWriteReg32(dev, DMPROGBUF0, 0x00c28023); // sb  a2,0x0(t0)
	MCFWriteReg32(dev, DMPROGBUF1, 0x00e68023); // sb  a4,0x0(a3)
	MCFWriteReg32(dev, DMPROGBUF2, 0x00f68023); // sb  a5,0x0(a3)
	
	MCFWriteReg32(dev, DMPROGBUF3, 0x00100073); // c.ebreak	
	MCFWriteReg32(dev, DMCOMMAND, 0x00271000); // Execute program.

	MCFWaitForDoneOp(dev);

	iss->currentstateval = -1;
}

// Proper read/write protection is only available on CH570/2.
// Funnily the chips I have only prevent you from WRITING the flash, when this protection is enabled.
// But you won't be able to write them. Spent some time to figure out how to do this properly,
// without relying on internal LinkE functionality
void ch570_disable_read_protection(struct SWIOState * iss) {
  struct SWIOState * dev = iss;
	uint8_t options[4];

	// MCFWriteByte(dev, 0x40001805, 0x10);
	// MCFWriteByte(dev, 0x40001807, 0x5);
	// ch5xx_write_safe(dev, 0x4000100A, 0x14, 0);
	// ch5xx_write_safe(dev, 0x40001008, 0x48, 0);
	// MCFWriteWord(dev, 0x40001000, 0x5555);
	// MCFWriteWord(dev, 0x40001004, 0x7fff);
	// ch5xx_write_safe(dev, 0x4000101f, 1, 0);
	
	CH5xxErase(dev, 0, 0, 1);
	ch570_disable_acauto(dev);
	MCFWriteWord(dev, 0x40001808, 0x749da58b);
	
	ch5xx_read_options_bulk(dev, 0x3EFFC, options, 4);
	options[2] = 0x3a;
	
	CH5xxErase(dev, 0x3e000, 0x1000, 0);
	ch5xx_write_flash(dev, 0x3EFFC, options, 4);
}

// Flash controller in CH5xx has built-in verify function. Not sure how usefule it is.
// Minichlink currently doesn't use it anywhere. And even if we would start using it,
// we probably would write a generic one the would work on all chips, including CH32.
int ch5xx_verify_data(struct SWIOState * iss, uint32_t addr, uint8_t* data, uint32_t len) {
	struct SWIOState * dev = iss;

	uint32_t dmdata0;
	int r;

	// if (addr >= iss->target_chip->flash_size || (addr + len) > iss->target_chip->flash_size) {
	// 	return -1;
	// }
	
	uint32_t dmdata0_offset = 0xe0000380;
	MCFReadReg32(dev, DMHARTINFO, &dmdata0_offset);
	dmdata0_offset = 0xe0000000 | (dmdata0_offset & 0x7ff);

	ch5xx_flash_open(dev, 0x20);

	ch5xx_flash_addr(dev, 0xb, addr);

	if (iss->statetag != STTAG("FVER")) {
		if ((iss->statetag & 0xff) != 'F') {
			MCFWriteReg32(dev, DMABSTRACTAUTO, 0x00000000); // Disable Autoexec.
			MCFWriteReg32(dev, BDMDATA0, R32_FLASH_DATA_T);
			MCFWriteReg32(dev, DMCOMMAND, 0x00230000 | 0x100d); // Write a3 from DATA0.
		}
		MCFWriteReg32(dev, BDMDATA0, dmdata0_offset);
		MCFWriteReg32(dev, DMCOMMAND, 0x00230000 | 0x100a); // Write a0 from DATA0.

		MCFWriteReg32(dev, DMPROGBUF0, 0x47910001); // c.nop; li a5,4;
		MCFWriteReg32(dev, DMPROGBUF1, 0x00668703); // lb a4,6(a3);
		MCFWriteReg32(dev, DMPROGBUF2, 0xfe074ee3); // blt a4,zero,-4;
		MCFWriteReg32(dev, DMPROGBUF3, 0x00468703); // lb a4,4(a3);
		MCFWriteReg32(dev, DMPROGBUF4, 0xfbed17fd); // c.addi a5,-1; c.bnez a5,-14;
		MCFWriteReg32(dev, DMPROGBUF5, 0xc10c428c); // c.lw a1,0(a3); c.sw a1,0,(a0);
		MCFWriteReg32(dev, DMPROGBUF6, 0x00100073); // ebreak

		iss->statetag = STTAG("FVER");
	}
	
	for (int i = 0; i < len; i += 4) {
		uint8_t timeout = 200;
		MCFWriteReg32(dev, DMCOMMAND, 0x00271000); // Execute program.
		MCFReadReg32(dev, BDMDATA0, &dmdata0);
		uint32_t word = data[i] | data[i+1] << 8 | data[i+2] << 16 | data[i+3] << 24;
		// if (dmdata0 != *((uint32_t*)(data+i))) {
		if (dmdata0 != word) {
			do {
				r = MCFReadReg32(dev, DMABSTRACTCS, &dmdata0);
				if(r) return r;
				if (dmdata0 & (0x700)) {
					return -2;
				}
				timeout--;
			} while((dmdata0 & (1<<12)) && timeout);
			MCFReadReg32(dev, BDMDATA0, &dmdata0);
			if (dmdata0 != word) {
				return -1;
			}
		}
	}
	ch5xx_flash_close(dev);
	return 0;
}

// Flash controller of CH5xx chips has different mode of erasing for better performance.
// You can erase in 256/4096/32768/65536 bytes chunks. 32K option is only present on CH32.
int CH5xxErase(struct SWIOState * iss, uint32_t addr, uint32_t len, enum MemoryArea area) {
	struct SWIOState * dev = iss;
	
	int ret = 0;
	int sector_size = iss->sectorsize;
	uint8_t flash_cmd;

	ch5xx_set_clock(dev, 0);

	uint32_t left = len;
	int chunk_to_erase = addr;

	ch5xx_flash_open(dev, 0xe0);
	chunk_to_erase = chunk_to_erase & ~(sector_size-1);
	while(left) {
		if (!(chunk_to_erase & 0xFFFF) && left >= (64*1024)) {
			sector_size = 64*1024;
			flash_cmd = 0xd8;
		} else if (iss->target_chip_type == CHIP_CH570 && !(chunk_to_erase & 0x7FFF) && left >= (32*1024)) {
			sector_size = 32*1024;
			flash_cmd = 0x52;
		} else if (!(chunk_to_erase & 0xFFF) && left >= (4*1024) ) {
			sector_size = 4*1024;
			flash_cmd = 0x20;
		} else {
			if (iss->sectorsize == 256 || area == EEPROM_AREA) {
				sector_size = 256;
				flash_cmd = 0x81;
			} else {
				sector_size = 4*1024;
				flash_cmd = 0x20;
			}
		}

		ch5xx_flash_addr(dev, flash_cmd, chunk_to_erase);
		ret = ch5xx_flash_wait(dev);
		if (!ret) return -1;
		chunk_to_erase += sector_size;
		if (left < sector_size) left = 0;
		else left -= sector_size;
	}
	ch5xx_flash_close(dev);
	return 0;
}

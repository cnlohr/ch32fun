/*
	Copyright 2025 monte-monte
*/

#include "minichlink.h"
#include "ch5xx.h"
#include <stdio.h>
#include "chips.h"
#include <stdlib.h>
#include <string.h>
#include "terminalhelp.h"

// Assembly for these binary blobs can be found it the attic folder
unsigned char ch5xx_write_block_bin[] = {
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

unsigned char ch5xx_blink_bin[] = {
	0x18, 0xc2, 0xb6, 0x87, 0x23, 0x24, 0x06, 0x00, 0xfd, 0x17, 0xfd, 0xff,
	0xb6, 0x87, 0x18, 0xc6, 0xfd, 0x17, 0xfd, 0xff, 0xf5, 0xb7, 0x02, 0x90
};
unsigned int ch5xx_blink_bin_len = 24;

// By default all chips run at the lowest possible frequency. (For example CH585 runs at 5.33MHz)
// WCH-LinkE sets needed frequency automatically with it's built-in functions. But it will be reset if we reset the chip.
// Also we still need to do that every time we use other programmers.
int CH5xxSetClock(void * dev, uint32_t clock) {
	struct InternalState * iss = (struct InternalState*)(((struct ProgrammerStructBase*)dev)->internal);
	if (clock == 0) {
		uint32_t rr = 0;
		MCF.ReadWord(dev, 0x40001008, &rr);
		fprintf(stderr, "Setting clock, current = %08x\n", rr);
		switch (iss->target_chip_type)
		{
		case CHIP_CH585:
			iss->clock_set = 24000;
			if ((rr&0xfff) != 0x14d) {
				ch5xx_write_safe(dev, 0x4000100A, 0x16, 0);
				ch5xx_write_safe(dev, 0x40001008, 0x14d, 1);
			} else {
				return 0;
			}
			break;

		case CHIP_CH570:
			iss->clock_set = 75000;
			if ((rr&0xff) != 0x48) {
				ch5xx_write_safe(dev, 0x4000100A, 0x14, 0); // Enable PLL
				// Set flash clock (undocumented)
				ch5xx_write_safe(dev, 0x40001805, 8, 0); // Flash SCK
				ch5xx_write_safe(dev, 0x40001807, 1, 0); // Flash CFG
				//Set core clock
				ch5xx_write_safe(dev, 0x40001008, 0x48, 0); // 75MHz
				// Disable watchdog
				MCF.WriteWord(dev, 0x40001000, 0x5555);
				MCF.WriteWord(dev, 0x40001004, 0x7fff);
			} else {
				return 0;
			}
			break;

		case CHIP_CH57x:
		case CHIP_CH58x:
		case CHIP_CH59x:
			iss->clock_set = 16000;
			if ((rr&0xff) != 0x82) {
				ch5xx_write_safe(dev, 0x40001008, 0x82, 0);
			} else {
				return 0;
			}
			break;
		
		default:
			return 0;
		}
#if DEBUG_CH5xx_MINICHLINK
		fprintf(stderr, "Setting reliable clock\n");
#endif
	}
	return 0;
}

// Such registers marked "RWA" in the datasheet
void ch5xx_write_safe(void * dev, uint32_t addr, uint32_t value, uint8_t mode) {
	struct InternalState * iss = (struct InternalState*)(((struct ProgrammerStructBase*)dev)->internal);
	
	if ((iss->statetag & 0xFFFF00FF) != (STTAG( "5WWS" ) &0xFFFF00FF)) {
		MCF.WriteReg32(dev, DMABSTRACTAUTO, 0x00000000); // Disable Autoexec.

		MCF.WriteReg32(dev, DMDATA0, 0x40001040);
		MCF.WriteReg32(dev, DMCOMMAND, 0x00230000 | 0x100d); // Write a3 from DATA0.
		MCF.WriteReg32(dev, DMDATA0, 0x57);
		MCF.WriteReg32(dev, DMCOMMAND, 0x00230000 | 0x100e); // Write a4 from DATA0.
		MCF.WriteReg32(dev, DMDATA0, 0xa8);
		MCF.WriteReg32(dev, DMCOMMAND, 0x00230000 | 0x100f); // Write a5 from DATA0.

		MCF.WriteReg32(dev, DMPROGBUF0, 0x00e68023); // sb  a4,0x0(a3)
		MCF.WriteReg32(dev, DMPROGBUF1, 0x00f68023); // sb  a5,0x0(a3)
		MCF.WriteReg32(dev, DMPROGBUF2, 0x00010001); // c.nop c.nop
		MCF.WriteReg32(dev, DMPROGBUF4, 0x00100073); // c.ebreak	
	}

	if (mode == 0 && iss->statetag != STTAG( "5WBS" )) {
		MCF.WriteReg32(dev, DMPROGBUF3, 0x00c28023); // sb  a2,0x0(t0)
		iss->statetag = STTAG( "5WBS" );
	} else if (mode == 1 && iss->statetag != STTAG( "5WHS" )) {
		MCF.WriteReg32(dev, DMPROGBUF3, 0x00c29023); // sh  a2,0x0(t0)  
		iss->statetag = STTAG( "5WHS" );
	} else if (mode == 2 && iss->statetag != STTAG( "5WBS" )) {
		MCF.WriteReg32(dev, DMPROGBUF3, 0x00c2a023); // sw  a2,0x0(t0)  
		iss->statetag = STTAG( "5WWS" );
	}

	MCF.WriteReg32(dev, DMDATA0, addr);
	MCF.WriteReg32(dev, DMCOMMAND, 0x00230000 | 0x1005); // Write t0 from DATA0.
	MCF.WriteReg32(dev, DMDATA0, value);
	MCF.WriteReg32(dev, DMCOMMAND, 0x00230000 | 0x100c); // Write a2 from DATA0.

	MCF.WriteReg32(dev, DMCOMMAND, 0x00271000); // Execute program.

	int ret = 0;
	ret |= MCF.WaitForDoneOp(dev, 1);
	iss->currentstateval = -1;

	if(ret) fprintf(stderr, "Fault on ch5xx_write_safe\n");
}

uint8_t ch5xx_flash_begin(void * dev, uint8_t cmd) {
	struct InternalState * iss = (struct InternalState*)(((struct ProgrammerStructBase*)dev)->internal);

	if (iss->statetag != STTAG("FBEG")) {
		if ((iss->statetag & 0xff) != 'F') {
			MCF.WriteReg32(dev, DMABSTRACTAUTO, 0x00000000); // Disable Autoexec.
			MCF.WriteReg32(dev, DMDATA0, R32_FLASH_DATA);
			MCF.WriteReg32(dev, DMCOMMAND, 0x00230000 | 0x100d); // Write a3 from DATA0.
		}
		MCF.WriteReg32(dev, DMPROGBUF0, 0x00068323); // sb zero,6(a3);
		MCF.WriteReg32(dev, DMPROGBUF1, 0x00014715); // c.li a4,5; c.nop;
		MCF.WriteReg32(dev, DMPROGBUF2, 0x00e68323); // sb a4,6(a3);
		MCF.WriteReg32(dev, DMPROGBUF3, 0x00f68223); // sb a5,4(a3);
		MCF.WriteReg32(dev, DMPROGBUF4, 0x00100073); // c.ebreak

		iss->statetag = STTAG("FBEG");  
	}

	MCF.WriteReg32(dev, DMDATA0, cmd); // Write command to a5
	MCF.WriteReg32(dev, DMCOMMAND, 0x0027100f); // Execute program.
	
	return cmd;
}

void ch5xx_flash_end(void * dev) {
	struct InternalState * iss = (struct InternalState*)(((struct ProgrammerStructBase*)dev)->internal);

	if (iss->statetag != STTAG("FEND")) {
		if ((iss->statetag & 0xff) != 'F') {
			MCF.WriteReg32(dev, DMABSTRACTAUTO, 0x00000000); // Disable Autoexec.
			MCF.WriteReg32(dev, DMDATA0, R32_FLASH_DATA);
			MCF.WriteReg32(dev, DMCOMMAND, 0x00230000 | 0x100d); // Write a3 from DATA0.
		}
		MCF.WriteReg32(dev, DMPROGBUF0, 0x00668703); // lb a4,6(a3);
		MCF.WriteReg32(dev, DMPROGBUF1, 0xfe074ee3); // blt a4,zero,-4;
		MCF.WriteReg32(dev, DMPROGBUF2, 0x00068323); // sb zero,6(a3);
		MCF.WriteReg32(dev, DMPROGBUF3, 0x00100073); // c.ebreak

		iss->statetag = STTAG("FEND");  
	}

	MCF.WriteReg32(dev, DMCOMMAND, 0x00271000); // Execute program.
}

void ch5xx_flash_out(void * dev, uint8_t addr)
{
	struct InternalState * iss = (struct InternalState*)(((struct ProgrammerStructBase*)dev)->internal);

	if (iss->statetag != STTAG("FOUT")) {
		if ((iss->statetag & 0xff) != 'F') {
			MCF.WriteReg32(dev, DMABSTRACTAUTO, 0x00000000); // Disable Autoexec.
			MCF.WriteReg32(dev, DMDATA0, R32_FLASH_DATA);
			MCF.WriteReg32(dev, DMCOMMAND, 0x00230000 | 0x100d); // Write a3 from DATA0.
		}
		MCF.WriteReg32(dev, DMPROGBUF0, 0x00668703); // lb a4,6(a3);
		MCF.WriteReg32(dev, DMPROGBUF1, 0xfe074ee3); // blt a4,zero,-4;
		MCF.WriteReg32(dev, DMPROGBUF2, 0x00f68223); // sb a5,4(a3);
		MCF.WriteReg32(dev, DMPROGBUF3, 0x00100073); // c.ebreak

		iss->statetag = STTAG("FOUT");  
	}

	MCF.WriteReg32(dev, DMDATA0, addr); // Write command to a4
	MCF.WriteReg32(dev, DMCOMMAND, 0x0027100f); // Execute program.
	
}

uint8_t ch5xx_flash_in(void * dev)
{
	struct InternalState * iss = (struct InternalState*)(((struct ProgrammerStructBase*)dev)->internal);
	uint8_t r;

	if (iss->statetag != STTAG("FLIN")) {
		if ((iss->statetag & 0xff) != 'F') {
			MCF.WriteReg32(dev, DMABSTRACTAUTO, 0x00000000); // Disable Autoexec.
			MCF.WriteReg32(dev, DMDATA0, R32_FLASH_DATA);
			MCF.WriteReg32(dev, DMCOMMAND, 0x00230000 | 0x100d); // Write a3 from DATA0.
		}
		MCF.WriteReg32(dev, DMPROGBUF0, 0x00668703); // lb a4,6(a3);
		MCF.WriteReg32(dev, DMPROGBUF1, 0xfe074ee3); // blt a4,zero,-4;
		MCF.WriteReg32(dev, DMPROGBUF2, 0x00468783); // lb a5,4(a3);
		MCF.WriteReg32(dev, DMPROGBUF3, 0x00100073); // c.ebreak

		iss->statetag = STTAG( "FLIN" );  
	}

	MCF.WriteReg32(dev, DMCOMMAND, 0x00271000); // Execute program.
	
	r = MCF.WaitForDoneOp(dev, 0);
	if (r) return 0;

	uint32_t rr;
	MCF.WriteReg32(dev, DMCOMMAND, 0x0022100f); // Read a5 into DATA0.
	MCF.ReadReg32(dev, DMDATA0, &rr);
	r = rr & 0xff;
	
	return r;
}

uint8_t ch5xx_flash_addr(void * dev, uint8_t cmd, uint32_t addr) {
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

uint8_t ch5xx_flash_open(void* dev, uint8_t op) {
	struct InternalState * iss = (struct InternalState*)(((struct ProgrammerStructBase*)dev)->internal);

	uint8_t glob_rom_cfg;
	MCF.ReadByte(dev, 0x40001044, &glob_rom_cfg);
	// fprintf(stderr, "RS = %02x, op = %02x\n", glob_rom_cfg, op);
	if ((glob_rom_cfg & 0xe0) != op) {
		ch5xx_write_safe(dev, 0x40001044, op, 0);
		MCF.ReadByte(dev, 0x40001044, &glob_rom_cfg);
		// fprintf(stderr, "RS = %02x\n", glob_rom_cfg);
	}
	MCF.WriteByte(dev, 0x40001806, 4);
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

void ch5xx_flash_close(void* dev) {
	uint8_t glob_rom_cfg;
	MCF.ReadByte(dev, 0x40001044, &glob_rom_cfg);
	ch5xx_flash_end(dev);
	ch5xx_write_safe(dev, 0x40001044, glob_rom_cfg & 0x10, 0);
}

uint8_t ch5xx_flash_wait(void* dev) {
	uint32_t timer = 0x80000;
	uint8_t ret = 0;
	ch5xx_flash_end(dev);
	// fprintf(stderr, "1\n");
	do {
		// fprintf(stderr, "%d\n", timer);
		ch5xx_flash_begin(dev, 5);
		ch5xx_flash_in(dev);
		ret = ch5xx_flash_in(dev);
		ch5xx_flash_end(dev);
		if ((ret & 1) == 0) {
			return (ret & 0xff) | 1;
		}
		// MCF.DelayUS(dev, 100);
		timer--;
	} while (timer != 0);
	return 0;
}

int ch5xx_read_eeprom(void * dev, uint32_t addr, uint8_t* buffer, uint32_t len) {
	struct InternalState * iss = (struct InternalState*)(((struct ProgrammerStructBase*)dev)->internal);

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
		fprintf(stderr, "CH570/2 don't have EEPROM\n");
		return -1;
	}

	if (iss->statetag != STTAG("FREP")) {
		if ((iss->statetag & 0xff) != 'F') {
			MCF.WriteReg32(dev, DMABSTRACTAUTO, 0x00000000); // Disable Autoexec.
			MCF.WriteReg32(dev, DMDATA0, R32_FLASH_DATA);
			MCF.WriteReg32(dev, DMCOMMAND, 0x00230000 | 0x100d); // Write a3 from DATA0.
		}

		MCF.WriteReg32(dev, DMPROGBUF0, 0x00668703); // lb a4,6(a3);
		MCF.WriteReg32(dev, DMPROGBUF1, 0xfe074ee3); // blt a4,zero,-4;
		MCF.WriteReg32(dev, DMPROGBUF2, 0x00468583); // lb a1,4(a3);
		MCF.WriteReg32(dev, DMPROGBUF7, 0xfe5ff06f); // jal zero,-28
		iss->statetag = STTAG("FREP");
	}
	
	for (int i = 0; i < len; i++) {
		uint32_t local_buffer = 0;
		MCF.WriteReg32(dev, DMCOMMAND, 0x00271000); // Execute program.
		do {
			r = MCF.ReadReg32(dev, DMABSTRACTCS, &rrv);
			if(r) return r;
			if (rrv & (0x700)) {
				fprintf(stderr, "Error in ch5xx_read_eeprom: %08x\n", rrv);
				return rrv;
			}
		} while((rrv & (1<<12)));
		MCF.WriteReg32(dev, DMCOMMAND, 0x0022100b); // Read a0 into DATA0.
		MCF.ReadReg32(dev, DMDATA0, &local_buffer);
		*(buffer + i) = local_buffer & 0xff;
	}
	
	ch5xx_flash_close(dev);
	return 0;
}

int ch5xx_read_options(void * dev, uint32_t addr, uint8_t* buffer) {
	struct InternalState * iss = (struct InternalState*)(((struct ProgrammerStructBase*)dev)->internal);

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
			MCF.WriteReg32(dev, DMABSTRACTAUTO, 0x00000000); // Disable Autoexec.
			MCF.WriteReg32(dev, DMDATA0, R32_FLASH_DATA);
			MCF.WriteReg32(dev, DMCOMMAND, 0x00230000 | 0x100d); // Write a3 from DATA0.
		}
		MCF.WriteReg32(dev, DMDATA0, 4);
		MCF.WriteReg32(dev, DMCOMMAND, 0x00230000 | 0x100f); // Write a5 from DATA0
		MCF.WriteReg32(dev, DMDATA0, 8);
		MCF.WriteReg32(dev, DMCOMMAND, 0x00230000 | 0x100c); // Write a2 from DATA0

		MCF.WriteReg32(dev, DMPROGBUF0, 0x00668703); // lb a4,6(a3);
		MCF.WriteReg32(dev, DMPROGBUF1, 0xfe074ee3); // blt a4,zero,-4;
		MCF.WriteReg32(dev, DMPROGBUF2, 0x00468583); // lb a1,4(a3);
		MCF.WriteReg32(dev, DMPROGBUF3, 0xc9e3167d); // c.addi a2,-1; blt a5,a2,-14 [0xfec7c9e3]
		MCF.WriteReg32(dev, DMPROGBUF4, 0xe219fec7); //               c.bnez a2,6
		MCF.WriteReg32(dev, DMPROGBUF5, 0x9002428c); // c.lw a1,0(a3) c.ebrake 
		MCF.WriteReg32(dev, DMPROGBUF6, 0x17f14288); // c.lw a0,0(a3) c.addi a5,-4
		MCF.WriteReg32(dev, DMPROGBUF7, 0xfe5ff06f); // jal zero,-28
		iss->statetag = STTAG("FOPT");
	}
	
	MCF.WriteReg32(dev, DMCOMMAND, 0x00271000); // Execute program.
	
	uint32_t rrv;
	int r;
	do {
		r = MCF.ReadReg32(dev, DMABSTRACTCS, &rrv);
		if(r) return r;
		if (rrv & (0x700)) {
			fprintf(stderr, "Error in ch5xx_read_options: %08x\n", rrv);
			return rrv;
		}
	} while((rrv & (1<<12)));
	
	MCF.WriteReg32(dev, DMCOMMAND, 0x0022100a); // Read a0 into DATA0.
	MCF.ReadReg32(dev, DMDATA0, (uint32_t*)buffer);
	MCF.WriteReg32(dev, DMCOMMAND, 0x0022100b); // Read a1 into DATA0.
	MCF.ReadReg32(dev, DMDATA0, (uint32_t*)(buffer+4));

	ch5xx_flash_close(dev);
	return 0;
}

int ch5xx_read_options_bulk(void* dev, uint32_t addr, uint8_t* buffer, uint32_t len) {
	struct InternalState * iss = (struct InternalState*)(((struct ProgrammerStructBase*)dev)->internal);

	enum RiscVChip chip = iss->target_chip_type;

	int r;
	uint32_t dmdata0;

	uint32_t dmdata0_offset = 0xe0000380;
	MCF.ReadReg32(dev, DMHARTINFO, &dmdata0_offset);
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
			MCF.WriteReg32(dev, DMABSTRACTAUTO, 0x00000000); // Disable Autoexec.
			MCF.WriteReg32(dev, DMDATA0, R32_FLASH_DATA);
			MCF.WriteReg32(dev, DMCOMMAND, 0x00230000 | 0x100d); // Write a3 from DATA0.
		}
		MCF.WriteReg32(dev, DMDATA0, dmdata0_offset);
		MCF.WriteReg32(dev, DMCOMMAND, 0x00230000 | 0x100a); // Write a0 from DATA0.

		MCF.WriteReg32(dev, DMPROGBUF0, 0x47910001); // c.nop; li a5,4;
		MCF.WriteReg32(dev, DMPROGBUF1, 0x00668703); // lb a4,6(a3);
		MCF.WriteReg32(dev, DMPROGBUF2, 0xfe074ee3); // blt a4,zero,-4;
		MCF.WriteReg32(dev, DMPROGBUF3, 0x00468703); // lb a4,4(a3);
		MCF.WriteReg32(dev, DMPROGBUF4, 0xfbed17fd); // c.addi a5,-1; c.bnez a5,-14;
		MCF.WriteReg32(dev, DMPROGBUF5, 0xc10c428c); // c.lw a1,0(a3); c.sw a1,0,(a0);
		MCF.WriteReg32(dev, DMPROGBUF6, 0x00100073); // ebreak

		iss->statetag = STTAG("FOPB");
	}

for (int i = 0; i < len; i += 4) {
		uint8_t timeout = 100;
		MCF.WriteReg32(dev, DMCOMMAND, 0x00271000); // Execute program.
		do {
			r = MCF.ReadReg32(dev, DMABSTRACTCS, &dmdata0);
			if(r) return r;
			if (dmdata0 & (0x700)) {
				fprintf(stderr, "Error in ch5xx_read_option_bulk: %08x\n", dmdata0);
				return -2;
			}
			timeout--;
		} while((dmdata0 & (1<<12)) && timeout);
		MCF.ReadReg32(dev, DMDATA0, (uint32_t*)(buffer + i));
	}

	ch5xx_flash_close(dev);
	return 0;
}

int ch5xx_read_secret_uuid(void * dev, uint8_t * buffer) {
	struct InternalState * iss = (struct InternalState*)(((struct ProgrammerStructBase*)dev)->internal);
	if(!iss->target_chip) {
		MCF.DetermineChipType(dev);
	}

	iss->statetag = STTAG("5SID");

	uint8_t local_buffer[8] = {0, 0, 0, 0, 0, 0, 0, 0};
	
	uint32_t dmdata0_offset = 0xe0000380;
	MCF.ReadReg32(dev, DMHARTINFO, &dmdata0_offset);
	dmdata0_offset = 0xe0000000 | (dmdata0_offset & 0x7ff);

	ch5xx_flash_open(dev, 0x20);
	ch5xx_flash_addr(dev, 0x4b, 0);
	
	MCF.WriteReg32(dev, DMDATA0, 0xffffffff);
	MCF.WriteReg32(dev, DMCOMMAND, 0x00230000 | 0x1005); // Write t0 from DATA0
	MCF.WriteReg32(dev, DMDATA0, dmdata0_offset);
	MCF.WriteReg32(dev, DMCOMMAND, 0x00230000 | 0x100a); // Write a0 from DATA0
	MCF.WriteReg32(dev, DMDATA0, 0xf);
	MCF.WriteReg32(dev, DMCOMMAND, 0x00230000 | 0x100b); // Write a1 from DATA0
	MCF.WriteReg32(dev, DMDATA0, 0);
	MCF.WriteReg32(dev, DMDATA1, 0);

	MCF.WriteReg32(dev, DMPROGBUF0, 0x00668703); // lb a4,6(a3);
	MCF.WriteReg32(dev, DMPROGBUF1, 0xfe074ee3); // blt a4,zero,-4;
	MCF.WriteReg32(dev, DMPROGBUF2, 0xf79342d8); // c.lw a4,4(a3); andi a5,a1,0x7;
	MCF.WriteReg32(dev, DMPROGBUF3, 0x97aa0075); //                c.add a5,a0;
	MCF.WriteReg32(dev, DMPROGBUF4, 0x00078603); // lb a2,0x0(a5);
	MCF.WriteReg32(dev, DMPROGBUF5, 0x8f3115fd); // c.addi a1,-1; c.xor a4,a2;
	MCF.WriteReg32(dev, DMPROGBUF6, 0x00e78023); // sb a4,0(a5);
	MCF.WriteReg32(dev, DMPROGBUF7, 0x00100073); // c.ebreak
	// MCF.WriteReg32(dev, DMPROGBUF7, 0xfe5592e3); // bne a1,t0,-28;
	// MCF.WriteReg32(dev, DMCOMMAND, 0x00271000); // Execute program.
	uint32_t rrv;
	int r;
	for (int i = 0xf; i > -1; i--) {
		MCF.WriteReg32(dev, DMCOMMAND, 0x00271000); // Execute program.
		do {
			r = MCF.ReadReg32(dev, DMABSTRACTCS, &rrv);
			if(r) return r;
			if (rrv & (0x700)) {
				MCF.WriteReg32(dev, DMABSTRACTCS, 0x08000700); // Clear out any dmabstractcs errors.
			}
		} while((rrv & (1<<12)));
	}
	MCF.ReadReg32(dev, DMDATA0, (uint32_t*)local_buffer);
	MCF.ReadReg32(dev, DMDATA1, (uint32_t*)(local_buffer+4));

	*((uint32_t*)buffer) = local_buffer[3]<<24|local_buffer[2]<<16|local_buffer[1]<<8|local_buffer[0];
	*(((uint32_t*)buffer)+1) = local_buffer[7]<<24|local_buffer[6]<<16|local_buffer[5]<<8|local_buffer[4];
	
	// fprintf(stderr, "%02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x\n", local_buffer[0], local_buffer[1], local_buffer[2], local_buffer[3], local_buffer[4], local_buffer[5], local_buffer[6], local_buffer[7]);

	ch5xx_flash_close(dev);
	return 0;
}

// All CH5xx have UUID which is also used to produce unique MAC address for BLE
int CH5xxReadUUID(void * dev, uint8_t * buffer) {
	struct InternalState * iss = (struct InternalState*)(((struct ProgrammerStructBase*)dev)->internal);
	if(!iss->target_chip) {
		MCF.DetermineChipType(dev);
	}

	iss->statetag = STTAG("5UID");

	enum RiscVChip chip = iss->target_chip_type;
	uint8_t local_buffer[8] = {0, 0, 0, 0, 0, 0, 0, 0};

	ch5xx_flash_open(dev, 0x20);

	if (chip == CHIP_CH57x ||
		  chip == CHIP_CH58x ||
		  chip == CHIP_CH585 ||
		  chip == CHIP_CH59x)
	{
		ch5xx_flash_addr(dev, 0xb, (0x7F018 | 0x80000));
	} else if (chip == CHIP_CH570) {
		ch5xx_flash_addr(dev, 0xb, (0x3F018 | 0x40000));
	}
	
	MCF.WriteReg32(dev, DMABSTRACTAUTO, 0x00000000); // Disable Autoexec.
	MCF.WriteReg32(dev, DMDATA0, 4);
	MCF.WriteReg32(dev, DMCOMMAND, 0x00230000 | 0x100f); // Write a5 from DATA0
	MCF.WriteReg32(dev, DMDATA0, 8);
	MCF.WriteReg32(dev, DMCOMMAND, 0x00230000 | 0x100c); // Write a2 from DATA0

	MCF.WriteReg32(dev, DMPROGBUF0, 0x00668703); // lb a4,6(a3);
	MCF.WriteReg32(dev, DMPROGBUF1, 0xfe074ee3); // blt a4,zero,-4;
	MCF.WriteReg32(dev, DMPROGBUF2, 0x00468583); // lb a1,4(a3);
	MCF.WriteReg32(dev, DMPROGBUF3, 0xc9e3167d); // c.addi a2,-1; blt a5,a2,-14 [0xfec7c9e3]
	MCF.WriteReg32(dev, DMPROGBUF4, 0xe219fec7); //               c.bnez a2,6
	MCF.WriteReg32(dev, DMPROGBUF5, 0x9002428c); // c.lw a1,0(a3) c.ebrake 
	MCF.WriteReg32(dev, DMPROGBUF6, 0x17f14288); // c.lw a0,0(a3) c.addi a5,-4
	MCF.WriteReg32(dev, DMPROGBUF7, 0xfe5ff06f); // jal zero,-28
	MCF.WriteReg32(dev, DMCOMMAND, 0x00271000); // Execute program.
	
	uint32_t rrv;
	int r;
	do {
		r = MCF.ReadReg32(dev, DMABSTRACTCS, &rrv);
		if(r) return r;
		if (rrv & (0x700)) {
			fprintf(stderr, "Error in CH5xxReadUUID: %08x\n", rrv);
			return rrv;
		}
	} while((rrv & (1<<12)));
	
	MCF.WriteReg32(dev, DMCOMMAND, 0x0022100a); // Read a0 into DATA0.
	MCF.ReadReg32(dev, DMDATA0, (uint32_t*)local_buffer);
	MCF.WriteReg32(dev, DMCOMMAND, 0x0022100b); // Read a1 into DATA0.
	MCF.ReadReg32(dev, DMDATA0, (uint32_t*)(local_buffer+4));
	
	uint16_t temp = (local_buffer[0]|(local_buffer[1]<<8)) + (local_buffer[2]|(local_buffer[3]<<8)) + (local_buffer[4]|(local_buffer[5]<<8));
		local_buffer[6] = temp&0xFF;
		local_buffer[7] = (temp>>8)&0xFF;

	*((uint32_t*)buffer) = local_buffer[3]<<24|local_buffer[2]<<16|local_buffer[1]<<8|local_buffer[0];
	*(((uint32_t*)buffer)+1) = local_buffer[7]<<24|local_buffer[6]<<16|local_buffer[5]<<8|local_buffer[4];

	ch5xx_flash_close(dev);
	return 0;
}

// Writing flash using a microblob in RAM. It's significantly faster on big ammounts compared to using PROGBUF.
// By the way, what happened to ch5xx_write_flash_using_microblob?!
int ch5xx_write_flash_using_microblob2(void * dev, uint32_t start_addr, uint8_t* data, uint32_t len) {
	struct InternalState * iss = (struct InternalState*)(((struct ProgrammerStructBase*)dev)->internal);

	if (len == 0) return 0;

	int r;
	uint8_t timer = 0;
	uint32_t dmdata0;
	uint32_t data_to_write = 0x00000001;
	uint32_t dmdata0_offset = 0xe0000380;
	MCF.ReadReg32(dev, DMHARTINFO, &dmdata0_offset);
	dmdata0_offset = 0xe0000000 | (dmdata0_offset & 0x7ff);
	
	if (iss->lastwriteflags != 100) {
		for (int i = 0; i < ch5xx_write_block_bin_len; i+=4) {
		MCF.WriteWord(dev, iss->target_chip->ram_base+i, *((uint32_t*)(ch5xx_write_block_bin + i)));
		// fprintf(stderr, "i= %i, data = %08x\n", i, *((uint32_t*)(ch5xx_write_block_bin + i)));
		}
		iss->lastwriteflags = 100; // This will indicate that we already have suitable microblob in RAM
	}

	// MCF.WriteReg32( dev, DMSHDWCFGR, 0x5aa50000 | (1<<10) ); // Shadow Config Reg
	// MCF.WriteReg32( dev, DMCFGR, 0x5aa50000 | (1<<10) ); // CFGR (1<<10 == Allow output from slave)

	ch5xx_flash_open(dev, 0xe0);

	if ((iss->statetag & 0xff) != 'F') {
		MCF.WriteReg32(dev, DMABSTRACTAUTO, 0x00000000); // Disable Autoexec.
		MCF.WriteReg32(dev, DMDATA0, R32_FLASH_DATA);
		MCF.WriteReg32(dev, DMCOMMAND, 0x00230000 | 0x100d); // Write a3 from DATA0.
	}
	MCF.WriteReg32(dev, DMDATA0, dmdata0_offset); // DMDATA0 offset
	MCF.WriteReg32(dev, DMCOMMAND, 0x00230000 | 0x100b); // Write a1 from DATA0.
	
	MCF.WriteReg32(dev, DMDATA0, start_addr);
	MCF.WriteReg32(dev, DMCOMMAND, 0x00230000 | 0x1006); // Write t1 from DATA0.

	MCF.WriteReg32(dev, DMDATA0, 0x000090c3); 
	MCF.WriteReg32(dev, DMCOMMAND, 0x002307b0);
	MCF.WriteReg32(dev, DMDATA0, 0); 
	MCF.WriteReg32(dev, DMCOMMAND, 0x00230300); // Clear mstatus
	MCF.WriteReg32(dev, DMDATA0, iss->target_chip->ram_base); 
	MCF.WriteReg32(dev, DMCOMMAND, 0x002307b1); // Write dpc from DATA0.
	
	MCF.WriteReg32(dev, DMDATA1, 1);
	MCF.WriteReg32(dev, DMCONTROL, 0x40000001);
	if (iss->target_chip_type == CHIP_CH570 || iss->target_chip_type == CHIP_CH585) MCF.WriteReg32(dev, DMCONTROL, 0x40000001);

	MCF.WriteReg32(dev, DMDATA0, 0);
	MCF.WriteReg32(dev, DMDATA1, 0);
	uint32_t byte = 0;
	while(byte < len) {
		uint32_t current_word;
		if (!data) current_word = data_to_write++;  // For testing purposes
		else current_word = *((uint32_t*)(data+byte));

		if (current_word) MCF.WriteReg32( dev, DMDATA0, current_word);
		else MCF.WriteReg32( dev, DMDATA1, 2);
		// do {
		//   if ((timer++) > 200) {
		//     fprintf(stderr, "Error3! Flash write timed out at byte %d\n", byte);
		//     fprintf(stderr, "dmdata0 = %08x, byte = %d\n", dmdata0, byte);
		//     r = -1;
		//     goto write_end;
		//   }
		//   MCF.ReadReg32(dev, DMDATA0, &dmdata0);
		// } while(dmdata0);
		byte += 4;
		// Wait every block to be written to flash and also new address set it takes ~1.5-2ms
		if(!(byte & 0xFF)) {
			fprintf(stderr, ".");
			do {
				if ((timer++) > 200) {
					fprintf(stderr, "Error1! Flash write timed out on block end. At byte %d\n", byte);
					fprintf(stderr, "dmdata0 = %08x, byte = %d\n", dmdata0, byte);
					r = -1;
					goto write_end;
				}
				MCF.ReadReg32(dev, DMDATA0, &dmdata0);
			} while(dmdata0 != byte + start_addr);
			timer = 0;
			do {
				if ((timer++) > 100) {
					fprintf(stderr, "Error2! Flash write timed out on block end. At byte %d\n", byte);
					fprintf(stderr, "dmdata0 = %08x, byte = %d\n", dmdata0, byte);
					
					r = -1;
					goto write_end;
				}
				MCF.ReadReg32(dev, DMDATA0, &dmdata0);
			} while(dmdata0);
		}
	}
	r = 0;
	MCF.WriteReg32(dev, DMDATA1, dmdata0_offset); // Signal to microblob that we're done writing
	MCF.DelayUS(dev,10000);
write_end:
	MCF.WriteReg32(dev, DMCONTROL, 0x80000001);
	ch5xx_flash_close(dev);
	
	return r;
}

// Writing flash using only PROGBUF. It's slower, but it doesn't touch RAM.
int ch5xx_write_flash(void * dev, uint32_t start_addr, uint8_t* data, uint32_t len) {
	struct InternalState * iss = (struct InternalState*)(((struct ProgrammerStructBase*)dev)->internal);
#if DEBUG_CH5xx_MINICHLINK
	fprintf(stderr, "Writing flash slowly. addr = %08x, len = %d\n", start_addr, len);
#endif

	// uint32_t data_to_write = 0x00000001;
	if (len == 0) return 0;

	uint32_t position = 0;

	ch5xx_flash_open(dev, 0xe0);
	
	while(len) {
		ch5xx_flash_addr(dev, 2, start_addr);

		if (iss->statetag != STTAG("FWRT")) {
			if ((iss->statetag & 0xff) != 'F') {
				MCF.WriteReg32(dev, DMABSTRACTAUTO, 0x00000000); // Disable Autoexec.
				MCF.WriteReg32(dev, DMDATA0, R32_FLASH_DATA);
				MCF.WriteReg32(dev, DMCOMMAND, 0x00230000 | 0x100d); // Write a3 from DATA0.
			}
			
			MCF.WriteReg32(dev, DMDATA0, 21);
			MCF.WriteReg32(dev, DMCOMMAND, 0x00230000 | 0x1005); // Write t0 from DATA0.
			
			MCF.WriteReg32(dev, DMPROGBUF0, 0x4791c298); // c.sw a4,0(a3); c.li a5,0x4;
			MCF.WriteReg32(dev, DMPROGBUF1, 0x00668703); // lb a4,6(a3);
			MCF.WriteReg32(dev, DMPROGBUF2, 0xfe074ee3); // blt a4,zero,-4;
			MCF.WriteReg32(dev, DMPROGBUF3, 0x00568323); // sb t0,6(a3);
			MCF.WriteReg32(dev, DMPROGBUF4, 0xfbed17fd); // c.addi a5,-1; c.bnez a5,-14;
			MCF.WriteReg32(dev, DMPROGBUF5, 0x00100073); // c.ebreak
			iss->statetag = STTAG("FWRT");
		}
		
		int r;
		uint32_t rrv;
		uint32_t block = len>256?256:len;
		
		for (uint32_t i = 0; i < block/4; i++) {
			MCF.WriteReg32(dev, DMDATA0, *((uint32_t*)(data+position)));
			// MCF.WriteReg32( dev, DMDATA0, data_to_write );
			// data_to_write++;
			MCF.WriteReg32(dev, DMCOMMAND, 0x0027100e); // Write a4 and execute.
			
			do {
				r = MCF.ReadReg32(dev, DMABSTRACTCS, &rrv);
				if(r) return r;
				if (rrv & (0x700)) {
					fprintf(stderr, "Error: %08x\n", rrv);
					return rrv;
				}
			} while((rrv & (1<<12)));
			position += 4;
		}
		// fprintf(stderr, "\nWaiting flash... block = %d, len = %d, start_addr = %d\n", block, len, start_addr);
		fprintf(stderr, ".");
		r = ch5xx_flash_wait(dev);
		len -= block;
		start_addr += block;
	}

	ch5xx_flash_close(dev);
	
	return 0;
}

// Redundand function to write word to memory. Currently not used.
void ch570_write_word_special(void * dev, uint32_t address, uint32_t word) {
	struct InternalState * iss = (struct InternalState*)(((struct ProgrammerStructBase*)dev)->internal);
	
	iss->statetag = STTAG("XXXX");
	MCF.WriteReg32( dev, DMABSTRACTAUTO, 0x00000000 ); // Disable Autoexec.

	MCF.WriteReg32( dev, DMPROGBUF0, 0x7b251073 ); // 
	MCF.WriteReg32( dev, DMPROGBUF1, 0x7b359073 ); // 
	MCF.WriteReg32( dev, DMPROGBUF2, 0xe0000537 ); // 
	MCF.WriteReg32( dev, DMPROGBUF3, 0x0f852583 ); // 
	MCF.WriteReg32( dev, DMPROGBUF4, 0x0f452503 ); // 
	MCF.WriteReg32( dev, DMPROGBUF5, 0x2573c188 ); // 
	MCF.WriteReg32( dev, DMPROGBUF6, 0x25f37b20 ); // 
	MCF.WriteReg32( dev, DMPROGBUF7, 0x90027b30 ); // 

	MCF.WriteReg32( dev, DMDATA0, word ); //R32_PA_DIR
	MCF.WriteReg32( dev, DMDATA1, address ); //R32_PA_DIR
	
	MCF.WriteReg32( dev, DMCOMMAND, 0x02210000 ); // Execute.
	MCF.WaitForDoneOp(dev, 0);

}

// This is only present in CH570/2 (as far as I know). I believe this serves the same purpose as "write safe" procedure.
// Apparently the difference is that it enters "safe mode" permanently until it's closed, or chip is reset.
void ch570_disable_acauto(void * dev) {
	struct InternalState * iss = (struct InternalState*)(((struct ProgrammerStructBase*)dev)->internal);
	// MCF.WriteReg32(dev, DMCOMMAND, 0x00230000 | 0x1005); // Write t0 from DATA0.
	
	MCF.WriteReg32(dev, DMABSTRACTAUTO, 0x00000000); // Disable Autoexec.

	MCF.WriteReg32(dev, DMDATA0, 0x40001808);
	MCF.WriteReg32(dev, DMCOMMAND, 0x00230000 | 0x100b); // Write a1 from DATA0.
	MCF.WriteReg32(dev, DMDATA0, 0);
	MCF.WriteReg32(dev, DMCOMMAND, 0x00230000 | 0x100c); // Write a2 from DATA0.
	MCF.WriteReg32(dev, DMDATA0, 0x40001040);
	MCF.WriteReg32(dev, DMCOMMAND, 0x00230000 | 0x100d); // Write a3 from DATA0.
	MCF.WriteReg32(dev, DMDATA0, 0x57);
	MCF.WriteReg32(dev, DMCOMMAND, 0x00230000 | 0x100e); // Write a4 from DATA0.
	MCF.WriteReg32(dev, DMDATA0, 0xa8);
	MCF.WriteReg32(dev, DMCOMMAND, 0x00230000 | 0x100f); // Write a5 from DATA0.

	MCF.WriteReg32(dev, DMPROGBUF0, 0x00c28023); // sb  a2,0x0(t0)
	MCF.WriteReg32(dev, DMPROGBUF1, 0x00e68023); // sb  a4,0x0(a3)
	MCF.WriteReg32(dev, DMPROGBUF2, 0x00f68023); // sb  a5,0x0(a3)
	
	MCF.WriteReg32(dev, DMPROGBUF3, 0x00100073); // c.ebreak	
	MCF.WriteReg32(dev, DMCOMMAND, 0x00271000); // Execute program.

	MCF.WaitForDoneOp(dev, 0);

	iss->currentstateval = -1;
}

// Proper read/write protection is only available on CH570/2.
// Funnily the chips I have only prevent you from WRITING the flash, when this protection is enabled.
// But you won't be able to write them. Spent some time to figure out how to do this properly,
// without relying on internal LinkE functionality
void ch570_disable_read_protection(void * dev) {
	uint8_t options[4];

  MCF.SetClock(dev, 0);
	
	CH5xxErase(dev, 0, 0, 1);
	ch570_disable_acauto(dev);
	MCF.WriteWord(dev, 0x40001808, 0x749da58b);
	
	ch5xx_read_options_bulk(dev, 0x3EFFC, options, 4);
	printf("Current option bytes - %08x\n", ((uint32_t*)options)[0]);
	options[2] = 0x3a;
	
	CH5xxErase(dev, 0x3e000, 0x1000, 0);
	ch5xx_write_flash(dev, 0x3EFFC, options, 4);
}

// Flash controller in CH5xx has built-in verify function. Not sure how usefule it is.
// Minichlink currently doesn't use it anywhere. And even if we would start using it,
// we probably would write a generic one the would work on all chips, including CH32.
int ch5xx_verify_data(void* dev, uint32_t addr, uint8_t* data, uint32_t len) {
	struct InternalState * iss = (struct InternalState*)(((struct ProgrammerStructBase*)dev)->internal);

	uint32_t dmdata0;
	int r;

	if (addr >= iss->target_chip->flash_size || (addr + len) > iss->target_chip->flash_size) {
		fprintf(stderr, "Address is beyound flash space\n");
		return -1;
	}
	
	uint32_t dmdata0_offset = 0xe0000380;
	MCF.ReadReg32(dev, DMHARTINFO, &dmdata0_offset);
	dmdata0_offset = 0xe0000000 | (dmdata0_offset & 0x7ff);

	ch5xx_flash_open(dev, 0x20);

	ch5xx_flash_addr(dev, 0xb, addr);

	if (iss->statetag != STTAG("FVER")) {
		if ((iss->statetag & 0xff) != 'F') {
			MCF.WriteReg32(dev, DMABSTRACTAUTO, 0x00000000); // Disable Autoexec.
			MCF.WriteReg32(dev, DMDATA0, R32_FLASH_DATA);
			MCF.WriteReg32(dev, DMCOMMAND, 0x00230000 | 0x100d); // Write a3 from DATA0.
		}
		MCF.WriteReg32(dev, DMDATA0, dmdata0_offset);
		MCF.WriteReg32(dev, DMCOMMAND, 0x00230000 | 0x100a); // Write a0 from DATA0.

		MCF.WriteReg32(dev, DMPROGBUF0, 0x47910001); // c.nop; li a5,4;
		MCF.WriteReg32(dev, DMPROGBUF1, 0x00668703); // lb a4,6(a3);
		MCF.WriteReg32(dev, DMPROGBUF2, 0xfe074ee3); // blt a4,zero,-4;
		MCF.WriteReg32(dev, DMPROGBUF3, 0x00468703); // lb a4,4(a3);
		MCF.WriteReg32(dev, DMPROGBUF4, 0xfbed17fd); // c.addi a5,-1; c.bnez a5,-14;
		MCF.WriteReg32(dev, DMPROGBUF5, 0xc10c428c); // c.lw a1,0(a3); c.sw a1,0,(a0);
		MCF.WriteReg32(dev, DMPROGBUF6, 0x00100073); // ebreak

		iss->statetag = STTAG("FVER");
	}
	
	for (int i = 0; i < len; i += 4) {
		uint8_t timeout = 200;
		MCF.WriteReg32(dev, DMCOMMAND, 0x00271000); // Execute program.
		MCF.ReadReg32(dev, DMDATA0, &dmdata0);
		if (dmdata0 != *((uint32_t*)(data+i))) {
			do {
				r = MCF.ReadReg32(dev, DMABSTRACTCS, &dmdata0);
				if(r) return r;
				if (dmdata0 & (0x700)) {
					fprintf(stderr, "Error in ch5xx_verify_data: %08x\n", dmdata0);
					return -2;
				}
				timeout--;
			} while((dmdata0 & (1<<12)) && timeout);
			MCF.ReadReg32(dev, DMDATA0, &dmdata0);
			if (dmdata0 != *((uint32_t*)(data+i))) {
				fprintf(stderr, "Verification failed at byte %d. dmdata0 = %08x, data = %08x\n", i, dmdata0, *((uint32_t*)(data+i)));
				return -1;
			}
		}
	}
	ch5xx_flash_close(dev);
	return 0;
}

// You can erase in 256/4096/32768/65536 bytes chunks. 32K option is only present on CH32.
int CH5xxErase(void* dev, uint32_t addr, uint32_t len, int type) {
	struct InternalState * iss = (struct InternalState*)(((struct ProgrammerStructBase*)dev)->internal);
	
	int ret = 0;
	int sector_size = iss->target_chip->sector_size;
	uint8_t flash_cmd;

	MCF.SetClock(dev, 0);

	if(type == 1) {
		// Whole-chip flash
		iss->statetag = STTAG("XXXX");
		printf("Whole-chip erase\n");
		addr = iss->target_chip->flash_offset;
		len = iss->target_chip->flash_size;
	}
#if DEBUG_CH5xx_MINICHLINK	
	printf( "len = %d, addr = %d\n", len, addr);
#endif

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
			if (iss->target_chip->sector_size == 256 || iss->current_area == EEPROM_AREA) {
				sector_size = 256;
				flash_cmd = 0x81;
			} else {
				sector_size = 4*1024;
				flash_cmd = 0x20; 
			}
		}
		int sector = ( chunk_to_erase & 0x00ffffff ) / iss->target_chip->sector_size;
		if( sector < MAX_FLASH_SECTORS ) {
			iss->flash_sector_status[sector] = 1;
		}
#if DEBUG_CH5xx_MINICHLINK
		fprintf(stderr, "Erasing chunk = %08x, using %d sector size, cmd = %02x\n", chunk_to_erase, sector_size, flash_cmd);
#endif
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

// General function to write data to flash. It will pad data to a single block, erase needed ammount,
int CH5xxWriteBinaryBlob(void * dev, uint32_t address_to_write, uint32_t blob_size, const uint8_t * blob) {
	struct InternalState * iss = (struct InternalState*)(((struct ProgrammerStructBase*)dev)->internal);

	int (*write_function)(void * dev, uint32_t start_addr, uint8_t* data, uint32_t len);

	int ret = 0;

	if (blob_size == 0) return 0;

	if (!iss->current_area) DetectMemoryArea(dev, address_to_write);
	if (!CheckMemoryLocation(dev, 0, address_to_write, blob_size)) {
		fprintf(stderr, "Data doesn't fit into memory location\n");
		return -1;
	}

	MCF.SetClock(dev, 0);
	
	uint32_t sector_size = iss->target_chip->sector_size;
	if (iss->current_area == RAM_AREA) sector_size = 4;
	uint8_t* start_pad = malloc(sector_size);
	uint8_t* end_pad = malloc(sector_size);
	
	uint32_t spad = address_to_write - ((address_to_write / sector_size) * sector_size);
	uint32_t epad = (address_to_write + blob_size) - ((address_to_write + blob_size) / sector_size) * sector_size;
	uint32_t new_blob_size = blob_size;
#if DEBUG_CH5xx_MINICHLINK
	fprintf(stderr, "spad = %d, epad = %d, blob_size = %d\n", spad, epad, blob_size);
#endif

	if (iss->target_chip_type == CHIP_CH570) {
		uint32_t options;
		MCF.ReadWord(dev, 0x40001058, &options);
		if ((options&0x800000) || (options&0x200000)) {
			printf("Flash is write/read protected. You may need to remove protection using 'minichlink -p'\n"); 
		}
	}

	if (iss->current_area == PROGRAM_AREA || iss->current_area == BOOTLOADER_AREA) {
		if (iss->current_area == BOOTLOADER_AREA) {
			uint8_t info_reg = 0;
			MCF.ReadByte(dev, R8_GLOB_CFG_INFO, &info_reg);
			if (!(info_reg & (1<<5))) {
				fprintf(stderr, "You need to be in bootloader mode to flash to BOOTLOADER partition.\n R8_GLOB_CFG_INFO = %02x\n", info_reg);
				ret = -2;
				goto end;
			} else {
				write_function = &ch5xx_write_flash;
			}
		} else {
			if (blob_size > 4096) write_function = &ch5xx_write_flash_using_microblob2;
			else write_function = &ch5xx_write_flash;
		}
		if (spad) {
			if (spad + blob_size <= sector_size) {
				MCF.ReadBinaryBlob(dev, (address_to_write - spad), sector_size, start_pad);
				memcpy(start_pad + spad, blob, blob_size);
				epad = 0;
			} else {
				MCF.ReadBinaryBlob(dev, (address_to_write - spad), spad, start_pad);
				memcpy(start_pad + spad, blob, sector_size - spad);
			}
			if (new_blob_size >= sector_size - spad) new_blob_size -= sector_size - spad;
			else new_blob_size = 0;
		}
		if (epad) {
			if (new_blob_size) memcpy(end_pad, blob + (blob_size - epad), epad);
			MCF.ReadBinaryBlob(dev, (address_to_write + blob_size), sector_size - epad, end_pad + epad);
			if (new_blob_size >= epad) new_blob_size -= epad;
			else new_blob_size = 0;
		}

		if (spad) {
			CH5xxErase(dev, ((address_to_write + spad) - sector_size), new_blob_size + spad + epad, 0);
			write_function(dev, address_to_write - spad, start_pad, sector_size);
			if (spad + blob_size > sector_size)
				write_function(dev, address_to_write + (sector_size - spad), (uint8_t*)(blob + (sector_size - spad)), new_blob_size);
		} else {
			CH5xxErase(dev, address_to_write, new_blob_size + epad, 0);
			write_function(dev, address_to_write, (uint8_t*)(blob), new_blob_size);
		}
		if (epad) write_function(dev, (address_to_write + blob_size) - epad, end_pad, sector_size);
		
		// ch5xx_verify_data(dev, address_to_write, blob, blob_size);
	} else if (iss->current_area == OPTIONS_AREA) {
		write_function = &ch5xx_write_flash;
		
		if (spad) {
			if (spad + blob_size <= sector_size) {
				ch5xx_read_options_bulk(dev, (address_to_write - spad), start_pad, sector_size);
				memcpy(start_pad + spad, blob, blob_size);
				epad = 0;
			} else {
				ch5xx_read_options_bulk(dev, (address_to_write - spad), start_pad, spad);
				memcpy(start_pad + spad, blob, sector_size - spad);
			}
			if (new_blob_size >= sector_size - spad) new_blob_size -= sector_size - spad;
			else new_blob_size = 0;
		}
		if (epad) {
			if (new_blob_size) memcpy(end_pad, blob + (blob_size - epad), epad);
			ch5xx_read_options_bulk(dev, (address_to_write + blob_size), end_pad + epad, sector_size - epad);
			if (new_blob_size >= epad) new_blob_size -= epad;
			else new_blob_size = 0;
		}

		if (spad) {
			CH5xxErase(dev, ((address_to_write + spad) - sector_size), new_blob_size + spad + epad, 0);
			write_function(dev, address_to_write - spad, start_pad, sector_size);
			if (spad + blob_size > sector_size)
				write_function(dev, address_to_write + (sector_size - spad), (uint8_t*)(blob + (sector_size - spad)), new_blob_size);
		} else {
			CH5xxErase(dev, address_to_write, new_blob_size + epad, 0);
			write_function(dev, address_to_write, (uint8_t*)(blob), new_blob_size);
		}
		if (epad) write_function(dev, (address_to_write + blob_size) - epad, end_pad, sector_size);
		
	} else if (iss->current_area == EEPROM_AREA) {
		write_function = &ch5xx_write_flash;

		if (spad) {
			if (spad + blob_size <= sector_size) {
				ch5xx_read_eeprom(dev, (address_to_write - spad), start_pad, sector_size);
				memcpy(start_pad + spad, blob, blob_size);
				epad = 0;
			} else {
				ch5xx_read_eeprom(dev, (address_to_write - spad), start_pad, spad);
				memcpy(start_pad + spad, blob, sector_size - spad);
			}
			if (new_blob_size >= sector_size - spad) new_blob_size -= sector_size - spad;
			else new_blob_size = 0;
		}
		if (epad) {
			if (new_blob_size) memcpy(end_pad, blob + (blob_size - epad), epad);
			ch5xx_read_eeprom(dev, (address_to_write + blob_size), end_pad + epad, sector_size - epad);
			if (new_blob_size >= epad) new_blob_size -= epad;
			else new_blob_size = 0;
		}

		if (spad) {
			CH5xxErase(dev, ((address_to_write + spad) - sector_size), new_blob_size + spad + epad, 0);
			write_function(dev, address_to_write - spad, start_pad, sector_size);
			if (spad + blob_size > sector_size)
				write_function(dev, address_to_write + (sector_size - spad), (uint8_t*)(blob + (sector_size - spad)), new_blob_size);
		} else {
			CH5xxErase(dev, address_to_write, new_blob_size + epad, 0);
			write_function(dev, address_to_write, (uint8_t*)(blob), new_blob_size);
		}
		if (epad) write_function(dev, (address_to_write + blob_size) - epad, end_pad, sector_size);

	} else if (iss->current_area == RAM_AREA) {
		if (spad) {
			for (int i = 0; i < spad; i++) {
				ret = MCF.WriteByte(dev, address_to_write+i, *((uint8_t*)(blob+i)));
				if (ret) {
					fprintf(stderr, "Error on WriteByte while writing to RAM.\n");
					goto end;
				}
			}
		} 
		for (int i = 0; i < blob_size-spad-epad; i += 4) {
			ret = MCF.WriteWord(dev, address_to_write+spad+i, *((uint32_t*)(blob+spad+i)));
			if (ret) {
				fprintf(stderr, "Error on WriteWord while writing to RAM.\n");
				goto end;
			}
		}
		if (epad) {
			uint32_t epad_address = address_to_write + blob_size - epad;
			for (int i = 0; i < spad; i++) {
				ret = MCF.WriteByte(dev, epad_address + i, *((uint8_t*)(blob + epad_address + i)));
				if (ret) {
					fprintf(stderr, "Error on WriteByte while writing to RAM.\n");
					goto end;
				}
			}
		}
	} else {
		fprintf(stderr, "Unknown memory region. Not writing.\n");
		ret = -2;
		goto end;
	}

	fprintf(stderr, "\nDone writing\n");
end:
	free(start_pad);
	free(end_pad);

	return ret;
}

// The only strangeness is that empty memory will be masked and will show as some magic word (For example 0xa9bdf9f3 for CH570/2).
// Other memory areas, like EEPROM or options require special procedure.
extern int DefaultReadBinaryBlob( void * dev, uint32_t address_to_read_from, uint32_t read_size, uint8_t * blob );
int CH5xxReadBinaryBlob(void* dev, uint32_t address_to_read_from, uint32_t read_size, uint8_t* blob) {
	struct InternalState * iss = (struct InternalState*)(((struct ProgrammerStructBase*)dev)->internal);

	int ret = 0;

	if (!iss->current_area) DetectMemoryArea(dev, address_to_read_from);
	if (!CheckMemoryLocation(dev, 0, address_to_read_from, read_size)) {
		fprintf(stderr, "Requested location should be within one memory area. Aborting\n");
		ret = -1;
		goto end;
	}

	if (iss->current_area == OPTIONS_AREA) {
		ret = ch5xx_read_options_bulk(dev, address_to_read_from, blob, read_size);
	} else if (iss->current_area == EEPROM_AREA) {
		ret = ch5xx_read_eeprom(dev, address_to_read_from, blob, read_size);
	} else if (iss->current_area == BOOTLOADER_AREA) {
		ret = DefaultReadBinaryBlob(dev, address_to_read_from, read_size, blob);
	} else if (iss->current_area == PROGRAM_AREA || iss->current_area == RAM_AREA) {
		ret = DefaultReadBinaryBlob(dev, address_to_read_from, read_size, blob);
	} else {
		fprintf(stderr, "Unknown memory region. Not reading.\n");
		ret = -2;
		goto end;
	}

	if (!ret) fprintf(stderr, "Done reading\n");
end:
	return ret;
}

void CH5xxBlink(void* dev, uint8_t port, uint8_t pin, uint32_t delay) {
	int r;
	uint32_t rrv;

	uint32_t port_reg = 0x400010A0 + (0x20 * port);
	uint32_t pin_mask = (1 << pin);
	if(!delay) delay = 500;
	uint32_t delay_count = 2133 * delay;

	MCF.WriteReg32(dev, DMABSTRACTAUTO, 0x00000000); // Disable Autoexec.
	
	MCF.WriteReg32(dev, DMDATA0, port_reg); //R32_PA_DIR
	MCF.WriteReg32(dev, DMCOMMAND, 0x00230000 | 0x100c); // Write a2 from DATA0.

	MCF.WriteReg32(dev, DMDATA0, delay_count);
	MCF.WriteReg32(dev, DMCOMMAND, 0x00230000 | 0x100d); // Write a3 from DATA0.

	MCF.WriteReg32(dev, DMDATA0, pin_mask );
	MCF.WriteReg32(dev, DMCOMMAND, 0x00230000 | 0x100e); // Write a4 from DATA0.

	MCF.WriteReg32(dev, DMPROGBUF0, 0x87b6c218); 
	MCF.WriteReg32(dev, DMPROGBUF1, 0x00062423); 
	MCF.WriteReg32(dev, DMPROGBUF2, 0xfffd17fd); 
	MCF.WriteReg32(dev, DMPROGBUF3, 0xc61887b6); 
	MCF.WriteReg32(dev, DMPROGBUF4, 0xfffd17fd); 
	MCF.WriteReg32(dev, DMPROGBUF5, 0x0000b7fd); 
	MCF.WriteReg32(dev, DMCOMMAND, 0x00271000); // Execute program.

	fprintf(stderr, "Running blink\n");
	fprintf(stderr, "Press Enter to stop, or Ctrl+C to close and keep blinking\n");
	
	do {
		if(IsKBHit()) break;
		r = MCF.ReadReg32(dev, DMABSTRACTCS, &rrv);
		if(r) return;
	} while((rrv & (1<<12)));

	MCF.WriteReg32(dev, DMCONTROL, 0x80000003);
}

// Test function that was used during development, will remove it later.
void CH5xxTestPC(void* dev) {
	// struct InternalState * iss = (struct InternalState*)(((struct ProgrammerStructBase*)dev)->internal);
	int r;
	uint32_t target_ram = 0x20001000;
	uint32_t rr;
	uint32_t port_reg = 0x400010A0;
	uint32_t pin_mask = (1 << 8);
	MCF.WriteReg32( dev, DMCONTROL, 0x80000001 );
	MCF.WriteReg32( dev, DMCONTROL, 0x80000001 );
	uint32_t delay_count = 2133 * 1000;
	for (int i = 0; i < ch5xx_blink_bin_len; i+=4) {
		r = MCF.WriteWord(dev, target_ram+i, *((uint32_t*)(ch5xx_blink_bin + i)));
		if (r) {
			fprintf(stderr, "Error writing to RAM at %d\n", i);
			return;
		}
		fprintf(stderr, "i= %i, data = %08x\n", i, *((uint32_t*)(ch5xx_blink_bin + i)));
	}

	for (int i = 0; i < 256; i+=4) {
		r = MCF.ReadWord(dev, target_ram+i, &rr);
		if (r) {
			fprintf(stderr, "Error reading to RAM at %d\n", i);
			return;
		}
		fprintf(stderr, "%08x-", rr);
	}

	//Set clock to 16MHz
	ch5xx_write_safe(dev, 0x40001008, 0x82, 0);

	MCF.ReadWord(dev, target_ram, &rr);
	fprintf(stderr, "%08x = %08x\n", target_ram, rr);
		

	MCF.WriteReg32( dev, DMABSTRACTAUTO, 0x00000000 ); // Disable Autoexec.

	MCF.WriteReg32( dev, DMDATA0, port_reg ); //R32_PA_DIR
	MCF.WriteReg32( dev, DMCOMMAND, 0x00230000 | 0x100c ); // Write a2 from DATA0.

	MCF.WriteReg32( dev, DMDATA0, delay_count );
	MCF.WriteReg32( dev, DMCOMMAND, 0x00230000 | 0x100d ); // Write a3 from DATA0.

	MCF.WriteReg32( dev, DMDATA0, pin_mask );
	MCF.WriteReg32( dev, DMCOMMAND, 0x00230000 | 0x100e ); // Write a4 from DATA0.

	// MCF.WriteReg32( dev, DMPROGBUF0, 0x00000797 ); // auipc a5, 0
	// MCF.WriteReg32( dev, DMPROGBUF0, 0x20000537 ); // lui a0, 0x20000
	// MCF.WriteReg32( dev, DMPROGBUF2, 0x40f505b3 ); // sub a1, a0, a5
	// MCF.WriteReg32( dev, DMPROGBUF1, 0x00018502 ); // c.jr a0; c.nop;
	// MCF.WriteReg32( dev, DMPROGBUF3, 0x40b787b3 ); // sub a5, a5, a1
	// MCF.WriteReg32( dev, DMPROGBUF3, 0x00058567 ); // jalr a0, 0(a5)

	// MCF.WriteReg32( dev, DMPROGBUF3, 0x00100073 ); // c.ebreak
	// MCF.WriteReg32( dev, DMCOMMAND, 0x00240000 ); // Execute.


	// MCF.WriteReg32( dev, DMDATA0, 0x20000000 ); //R32_PA_DIR
	// MCF.WriteReg32( dev, DMCOMMAND, 0x002707b1 ); // Execute.
	
	MCF.WriteReg32( dev, DMCOMMAND, 0x002207b0 ); // Read dcsr into DATA0.
	MCF.ReadReg32( dev, DMDATA0, &rr );
	fprintf(stderr, "dcsr = %08x\n", rr);

	MCF.WriteReg32( dev, DMDATA0, 0x000090c3 ); 
	MCF.WriteReg32( dev, DMCOMMAND, 0x002307b0 );
	MCF.WriteReg32(dev, DMDATA0, target_ram+0x3000); 
	MCF.WriteReg32( dev, DMCOMMAND, 0x00231002); // write sp
	MCF.WriteReg32(dev, DMDATA0, 0); 
	MCF.WriteReg32( dev, DMCOMMAND, 0x00230300); // Clear mstatus
	MCF.WriteReg32( dev, DMDATA0, target_ram ); 
	MCF.WriteReg32( dev, DMCOMMAND, 0x002307b1 ); // Execute.
	MCF.WriteReg32( dev, DMCONTROL, 0x40000001 );
	// MCF.WriteReg32( dev, DMCOMMAND, 0x00240000 ); // Execute.
	// MCF.DelayUS(dev, 2000000);
	// MCF.WriteReg32( dev, DMCONTROL, 0x80000001 );
	// MCF.WriteReg32( dev, DMCONTROL, 0x80000003 );
	// MCF.WriteReg32(dev, DMDATA0, 0); 
	// MCF.WriteReg32( dev, DMCOMMAND, 0x00230300); // Clear mstatus
	// uint32_t dmdata0;
	// do
	// {
	//   // if(ReadKBByte()) return 0;
	// 	MCF.ReadReg32( dev, DMABSTRACTCS, &rr );
	//   // fprintf( stderr, "DMABSTRACTCS = %08x\n", rr);
	//   // if (timer > 100) {
	//   //   MCF.WriteReg32( dev, DMCOMMAND, 0x002207b1); // Read xN into DATA0.
	//   //   MCF.ReadReg32( dev, DMDATA0, &rrv);
	//   //   fprintf(stderr, "Timeout at: %08x\n", rrv);
	//   //   return -10;
	//   // }
	//   // timer++;
	//   // fprintf(stderr, ".");
	//   // MCF.WriteReg32( dev, DMDATA1, 0x22222222);
	//   // do {
	//     // MCF.ReadReg32(dev, DMDATA1, &dmdata0);
	//     // fprintf(stderr, "%08x ", dmdata1);
	//   // } while(dmdata0 == 0x22222222);
	//   MCF.WriteReg32( dev, DMCOMMAND, 0x20100c); // Read xN into DATA0.
	//   MCF.ReadReg32( dev, DMDATA0, &dmdata0);
	//   fprintf(stderr, "%08x\n", dmdata0);
	// }
	// while( (rr & (1<<12)) );

	MCF.WriteReg32( dev, DMCOMMAND, 0x0022100f ); // Read a5 into DATA0.
	MCF.ReadReg32( dev, DMDATA0, &rr );
	fprintf(stderr, "a5 = %08x\n", rr);
	MCF.WriteReg32( dev, DMCOMMAND, 0x0022100a ); // Read a0 into DATA0.
	MCF.ReadReg32( dev, DMDATA0, &rr );
	fprintf(stderr, "a0 = %08x\n", rr);
	MCF.WriteReg32( dev, DMCOMMAND, 0x0022100b ); // Read a1 into DATA0.
	MCF.ReadReg32( dev, DMDATA0, &rr );
	fprintf(stderr, "a1 = %08x\n", rr);
}

// That's why we have a separate function to print infor about there chips.
int CH5xxPrintInfo(void* dev) {
	struct InternalState * iss = (struct InternalState*)(((struct ProgrammerStructBase*)dev)->internal);
	uint8_t info[8];

	MCF.SetClock(dev, 0);

	if (MCF.GetUUID(dev, info)) return -1;
	printf("UUID: %02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x\n", info[0], info[1], info[2], info[3], info[4], info[5], info[6], info[7]);
	printf("BLE MAC: %02x-%02x-%02x-%02x-%02x-%02x\n", info[0], info[1], info[2], info[3], info[4], info[5]);
	if (ch5xx_read_secret_uuid(dev, info)) return -1;
	printf("Secret UUID: %02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x\n", info[0], info[1], info[2], info[3], info[4], info[5], info[6], info[7]);
	uint32_t options_address;
	memset(info, 0, 8);
	if (iss->target_chip_type == CHIP_CH59x) {
		options_address = 0x7F010;
		if (ch5xx_read_options_bulk(dev, options_address, info, 8)) return -1;
		printf("ROM_CFG version: %08x-%08x:\n", ((uint32_t*)info)[0], ((uint32_t*)info)[1]);
	}
	options_address = 0x7EFFC;
	if (iss->target_chip_type == CHIP_CH570) options_address = 0x3EFFC;
	// if (ch5xx_read_options(dev, options_address, info)) return -1;
	if (ch5xx_read_options_bulk(dev, options_address, info, 4)) return -1;
	printf("Options bytes: %08x:\n", ((uint32_t*)info)[0]);
	uint32_t option_bytes = ((uint32_t*)info)[0];
	if ((option_bytes >> 28) != 4) {
		printf("Options signature is not valid: %02x\n", (option_bytes >> 28));
		if (iss->target_chip_type == CHIP_CH570) {
			MCF.ReadWord(dev, 0x40001058, &option_bytes);
			printf("FLASH_HALTED = %d\n", (option_bytes&0x800000)?1:0);
			printf("MANU_CFG_LOCK = %d\n", (option_bytes&0x400000)?1:0);
			printf("RD_PROTECT = %d\n", (option_bytes&0x200000)?1:0);
			if (option_bytes&0x200000) {
				printf("Chip is in read/write protection mode.\n");
				printf("You can disable it using 'minichlink -p'\n");
			}
		}
		return -1;
	}
	if (iss->target_chip_type == CHIP_CH570 || iss->target_chip_type == CHIP_CH585) {
		printf("Reset - %s\n", (option_bytes&0x8)?"enabled":"disabled");
		if (iss->target_chip_type == CHIP_CH570) printf("Reset pin - PA%d\n", (option_bytes&0x10)?7:8);
		else printf("Debug - %s\n", (option_bytes&0x10)?"enabled":"disabled");
		printf("IWDG - %s\n", (option_bytes&0x20)?"enabled":"disabled");
		printf("Bootloader - %s\n", (option_bytes&0x40)?"enabled":"disabled");
	} else {
		printf("Reset - %s\n", (option_bytes&0x8)?"enabled":"disabled");
		printf("Bootloader pin - PB%d\n", (option_bytes&0x200)?22:11);
		printf("Debug - %s\n", (option_bytes&0x10)?"enabled":"disabled");
		printf("Bootloader - %s\n", (option_bytes&0x40)?"enabled":"disabled");
		printf("UART_NO_KEY(what is that?) - %s\n", (option_bytes&0x100)?"enabled":"disabled");
	}

	return 0;
}

void ch5xx_special_reset_validate_options(void * dev) {
	struct InternalState * iss = (struct InternalState*)(((struct ProgrammerStructBase*)dev)->internal);
	
	MCF.WriteReg32(dev, DMABSTRACTAUTO, 0x00000000); // Disable Autoexec.

	MCF.WriteReg32(dev, DMDATA0, 0x40001000);
	MCF.WriteReg32(dev, DMCOMMAND, 0x00230000 | 0x100d); // Write a3 from DATA0.
	MCF.WriteReg32(dev, DMDATA0, 0x57);
	MCF.WriteReg32(dev, DMCOMMAND, 0x00230000 | 0x100e); // Write a4 from DATA0.
	MCF.WriteReg32(dev, DMDATA0, 0xa8);
	MCF.WriteReg32(dev, DMCOMMAND, 0x00230000 | 0x100f); // Write a5 from DATA0.
	MCF.WriteReg32(dev, DMDATA0, 0xffff);
	MCF.WriteReg32(dev, DMCOMMAND, 0x00230000 | 0x100c); // Write a2 from DATA0.
	MCF.WriteReg32(dev, DMDATA0, 1);
	MCF.WriteReg32(dev, DMCOMMAND, 0x00230000 | 0x100b); // Write a1 from DATA0.

	MCF.WriteReg32(dev, DMPROGBUF0, 0x04e68023); // sb  a4,0x40(a3)
	MCF.WriteReg32(dev, DMPROGBUF1, 0x04f68023); // sb  a5,0x40(a3)
	// Write 0xffff to R16_INT32K_TUNE
	MCF.WriteReg32(dev, DMPROGBUF2, 0x02c69623); // sh  a2,0x2c(a3)
	// Write 1 to R8_RST_WDOG_CTRL to reboot
	MCF.WriteReg32(dev, DMPROGBUF3, 0x04b68323); // sb  a1,0x46(a3)
	MCF.WriteReg32(dev, DMPROGBUF4, 0x04068023); // sb zero, 64(a3)
	// Endless loop, target should reset
	MCF.WriteReg32(dev, DMPROGBUF5, 0x9002a001); // c.j 0; c.ebreak
	
	iss->statetag = STTAG( "XXXX" );

	// fprintf(stderr, "Executing command\n");
	MCF.WriteReg32(dev, DMCOMMAND, 0x00271000); // Execute program.
}

void ch5xx_flash_powerup(void * dev) {
	ch5xx_flash_open(dev, 0xe0);
	ch5xx_flash_begin(dev, 0xab);
	ch5xx_flash_close(dev);
}

void ch5xx_flash_powerdown(void * dev) {
	ch5xx_flash_open(dev, 0xe0);
	ch5xx_flash_begin(dev, 0xb9);
	ch5xx_flash_close(dev);
}

void ch5xx_flash_sw_reset(void * dev) {
	ch5xx_flash_open(dev, 0xe0);
	ch5xx_flash_begin(dev, 0x66);
	ch5xx_flash_end(dev);
	ch5xx_flash_begin(dev, 0x99);
	ch5xx_flash_close(dev);
}

void ch5xx_flash_unlock(void * dev, uint8_t cmd) {
	// cmd: 0 - unlock all, 0x44 - lock boot code, 0x50 - lock all code and flash, 0x3c - lock something, unclear what exactly
	fprintf(stderr, "CH5xx flash unlock\n");
	ch5xx_flash_open(dev, 0xe0);
	ch5xx_flash_wait(dev);
	ch5xx_flash_begin(dev, 6);
	ch5xx_flash_end(dev);
	ch5xx_flash_begin(dev, 1);
	ch5xx_flash_out(dev, cmd);
	ch5xx_flash_out(dev, 2);
	ch5xx_flash_wait(dev);
	ch5xx_flash_close(dev);
}

int CH5xxConfigureNRSTAsGPIO(void * dev, int one_if_yes_gpio) {
	struct InternalState * iss = (struct InternalState*)(((struct ProgrammerStructBase*)dev)->internal);
	uint32_t options;

	MCF.SetClock(dev, 0);
	
	uint32_t options_address = 0x7EFFC;
	if (iss->target_chip_type == CHIP_CH570) options_address = 0x3EFFC;

	ch5xx_read_options_bulk(dev, options_address, (uint8_t*)&options, 4);
	printf("Current option bytes - %08x\n", options);

	if ((options&0x8) && one_if_yes_gpio == 0) {
		printf("Reset is already set as reset\n");
		return 0;
	} else if ( !(options&0x8) && one_if_yes_gpio > 0) {
		printf("Reset is already set as GPIO\n");
		return 0;
	} else {
		if (one_if_yes_gpio) options &= ~(0x08);
		else options |= 0x08;
		uint32_t options_array[3] = {0xffffffff, 0xffffffff, options};
		printf("New option bytes - %08x\n", options);
		if (iss->target_chip_type == CHIP_CH570) {
			CH5xxWriteBinaryBlob(dev, options_address - 8, 12, (uint8_t*)options_array);
		} else {
			uint8_t info_reg = 0;
			MCF.ReadByte(dev, R8_GLOB_CFG_INFO, &info_reg);
			if (!(info_reg & (1<<5))) {
				fprintf(stderr, "You need to be in bootloader mode to edit OPTIONS.\n R8_GLOB_CFG_INFO = %02x\n", info_reg);
				fprintf(stderr, "You can enter bootloader mode by prefexing -B before the command.\n");
				return -2;
			}
			CH5xxErase(dev, 0x7e000, 4096, 0);
			ch5xx_write_flash(dev, 0x7effc, (uint8_t*)&options, 4);
			ch5xx_special_reset_validate_options(dev);
		}
	}
	
	printf("New setting will be enabled after power reset.\n");
	return 0;
}
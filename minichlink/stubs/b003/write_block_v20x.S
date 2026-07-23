#
# Write a block of various length to flash.
# To be used with v20x and v30x chips
# Iterating through a data bigger than one block.
# Based on original code from pgm-b003fun.c by cnlohr
# Derived from monte-monte's write_block.S
#

csrr	a5, mstatus;
andi a5, a5, -137;
csrw mstatus, a5;   // Disabling interrupts globally

addi a4, a0, %lo(args+4);    // Start reading properties, after this binary in scratchpad
c.lw a1, 0(a4);     // a1 = Address to write to.
c.lw a5, 4(a4);     // a5 = Get flash address (0x4002200C)
lh t0, 8(a4);       // t0 = block size (64 or 256)
lh t1, 10(a4);      // t1 = total amount of data to write
add t1, a1, t1;     // t1 = end address of the section we are writing

lui a3, %hi(0x00010000);
c.sw a3, 4(a5);     // FLASH->CTLR = CR_PAGE_PG
2:
add a2, a1, t0;     // a2 = end of block to write to

4:                  // Wait for flash loop
c.lw a3, 0(a5);     // read FLASH->STATR
c.andi a3, 1;       // Mask off BUSY bit.
c.bnez a3, 4b;
5:
c.lw a3, 0(a5);
c.andi a3, 2;       // Mask off WRBSY bit.
c.bnez a3, 5b;

1:
c.lw a3, 12(a4);    // Read from RAM
c.sw a3, 0(a1);     // Store into flash

6:                  // Wait for flash loop
c.lw a3, 0(a5);     // read FLASH->STATR
c.andi a3, 2;       // Mask off WRBSY bit.
c.bnez a3, 6b;

c.addi a1, 4;       // Advance the destination pointer
c.addi a4, 4;       // Advance the source pointer

blt a1, a2, 1b;     // Loop untill the end of block.

lui a3, %hi(0x00200000);  // CR_PG_STRT
c.sw a3, 4(a5);     // FLASH->CTRL = CR_PG_STRT
lw a3, -4(a1);      // Can't read past the last byte of the flash, need to decrease

blt a1, t1, 2b;     // Loop to start writing the next block

c.li a3, -1;
c.sw a3, 0(a0);		  // Write -1 into 0x00 indicating all done.

csrr a5, mstatus;   // Enabling interrupts globally
ori a5, a5, 0x88;
csrw mstatus, a5;

ret;

.balign 4
args:

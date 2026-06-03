#
# Write a block of various length to flash.
# Iterating through a data bigger than one block.
# Based on original code from pgm-b003fun.c by cnlohr
# Copyright 2026 monte-monte
#

csrr	a5, mstatus;
andi a5, a5, -137;
csrw mstatus, a5;   /* Disabling interrupts globally */

addi a4, a0,108;    /* Start reading properties, starting from scratchpad + 88. */
c.lw a1, 0(a4);     /* a1 = Address to write to. */
c.lw a5, 4(a4);     /* a5 = Get flash address (0x4002200C) */
lh t0, 8(a4);       /* t0 = block size (64 or 256) */
lh t1, 10(a4);      /* t1 = total amount of data to write */
add t1, a1, t1;     /* t1 = end address of the section we are writing */

lui a3, %hi(0x00010000);
c.sw a3, 4(a5);     /* FLASH->CTLR = CR_PAGE_PG */
2:
add a2, a1, t0;     /* a2 = end of block to write to */
lui a3, %hi(0x00080000 | 0x00010000);
c.sw a3, 4(a5);     /* FLASH->CTLR = CR_BUF_RST | CR_PAGE_PG */

# 4:                  /* Wait for flash loop */
# c.lw a3, 0(a5)      /* read FLASH->STATR */
# c.andi a3, 1        /* Mask off BUSY bit. */
# c.bnez a3, 4b
c.lw a3, 0(a1);     /* Tricky: By reading from flash here, we force it to wait for completion. */

1:
c.lw a3, 12(a4);    /* lw a3, 0(a1)       // Read from RAM (Starting @64) */
c.sw a3, 0(a1);     /* sw a3, 0(a4)       // Store into flash */

lui a3, %hi(0x00010000 | 0x00040000);	/* CR_PAGE_PG | FLASH_CTLR_BUF_LOAD */
c.sw a3, 4(a5);     /* Load into flash write buffer. */

# 3:
# c.lw a3, 0(a5)      /* read FLASH->STATR */
# c.andi a3, 1        /* Mask off BUSY bit. */
# c.bnez a3, 3b
c.lw a3, 0(a1);     /* Tricky: By reading from flash here, we force it to wait for completion. */

c.addi a1, 4        /* Advance the destination pointer */
c.addi a4, 4        /* Advance the source pointer */

blt a1, a2, 1b      /* Loop untill the end of block. */

# c.nop;
# sub a3, a1, t0;
# c.sw a3, 8(a5);   /* Loading starting address of a block tto FLASH->ADDR. Seems not needed(?)
lui a3, %hi(0x00010000);  /* CR_PAGE_PG */
addi a3, a3, 0x40   /* CR_STRT_Set (can use ADD instead of OR here) */
c.sw a3, 4(a5);     /* FLASH->CTRL = CR_PAGE_PG|CR_STRT_Set */
lw a3, -4(a1);      /* Can't read past the last byte of the flash, need to decrease */

blt a1, t1, 2b      /* Loop to start writing the next block */

c.li a3, -1
c.sw a3, 0(a0)		  /* Write -1 into 0x00 indicating all done. */

csrr a5, mstatus;   /* Enabling interrupts globally */
ori a5, a5, 0x88;
csrw mstatus, a5;

ret

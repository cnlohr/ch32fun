#
# The simplest blink fuction that can fit into PROGBUF.
# This is the version suitable for CH5xx series of chips by WCH.
# You need to preload respective registers with values for port, pin and delay.
# Copyright 2025 monte-monte
#
#a2 = GPIO port (0 - A, 1 - B...)
#a3 = value to count down from for a delay between blinks
#a4 = pin mask
BEGIN:
c.sw a4, 0(a2);
c.mv a5, a3;
sw zero, 8(a2);
LOOP1:
c.addi a5, -1;
c.bnez a5, LOOP1;
c.mv a5, a3;
c.sw a4, 8(a2);
LOOP2:
c.addi a5, -1;
c.bnez a5, LOOP2;
c.j BEGIN;
c.ebreak

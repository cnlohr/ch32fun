#
# A procedure to write data to flash on CH5xx series of chips by WCH.
# This should be compiled into a binary blob and placed into RAM.
# Then you will be able to run it and send data via a programmer by writing it to DMDATA0/1
# Copyright 2025 monte-monte
#
#s0 = incomming data from dmdata0
#s1 = incomming data from dmdata1
#t0 = general reg
#t1 = address global
#a0 = address in loop
#a1 = dmdata0 offset
#a2 = general reg
#a3 = R32_FLASH_DATA
#a4 = general operational
#a5 = len counter
PREAMBLE:
#c.li t0,0;
#c.li a2,0;
#c.li a4,0;
#c.li a5,0;
lw s0,0(a1);
lw s1,4(a1);
bne s1,s0,PREAMBLE;#Wait for DMDATA0/1 to be cleared;
c.bnez s0,PREAMBLE;
START:
#BEGIN
addi a0,t1,0;
sb zero,6(a3);
c.li a4,5;
sb a4,6(a3);
c.li a4,6; 
sb a4,4(a3);
END:
lb a4,6(a3);
blt a4,zero,END;
sb zero,6(a3);
#BEGIN with CMD = 2
sb zero,6(a3);
c.li a4,5; 
sb a4,6(a3);
c.li a4,2;
sb a4,4(a3);
c.li a2,3;
OUT:
srli t0,a0,16;
andi t0,t0,0xff;
OUT_LOOP:
lb a4,6(a3);
blt a4,zero,OUT_LOOP;
sb t0,4(a3);
c.slli a0,8;
c.addi a2,-1;
c.bnez a2,OUT;
ADDRES_IS_SET:
sw zero,0(a1);#Clear DMDATA0 to indicate we are ready to write again
addi a5,zero,64;#Load up the byte counter
DATA_RETRIEVER:
c.lw s0,0(a1);
c.lw s1,4(a1);
c.bnez s1, WRITE_ZERO
c.beqz s0, DATA_RETRIEVER;
WRITE:
c.sw s0,0(a3); 
c.li a4,0x4;
WRITE_LOOP1:
lb a2,6(a3);
blt a2,zero,WRITE_LOOP1;
c.li a2,21;
sb a2,6(a3);
c.addi a4,-1; 
c.bnez a4,WRITE_LOOP1;
c.addi a5,-1;
sw zero,0(a1);
c.bnez a5,DATA_RETRIEVER;
addi t1,t1,256;
sw t1,0(a1);#Write current address to DMDATA0 to indicate that we are in wait loop
lui t0,0x80;
WAIT:
#END
lb a4,6(a3);
blt a4,zero,WAIT;
sb zero,6(a3)
#BEGIN with CMD = 5
WAIT_LOOP:
sb zero,6(a3);
c.li a4,5; 
sb a4,6(a3);
sb a4,4(a3)
IN_LOOP1:
lb a4,6(a3);
blt a4,zero,IN_LOOP1;
lbu a2,4(a3);
IN_LOOP2:
lb a4,6(a3);
blt a4,zero,IN_LOOP2;
lb a2,4(a3);
WAIT_END:
lb a4,6(a3);
blt a4,zero,WAIT_END;
sb zero,6(a3)
#IF
andi a4,a2,1;
c.bnez a4,WAIT_LOOP_BACK;
j START;
WAIT_LOOP_BACK:
c.addi t0,-1;
bne t0,zero,WAIT_LOOP;
EXIT:
c.ebreak;
WRITE_ZERO:
sw zero,4(a1);
beq s1,a1,WAIT;
sw zero,0(a3);
c.li a4,0x4;
c.j WRITE_LOOP1;

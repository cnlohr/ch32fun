c.nop;
li a5,4;
LOOP:
lb a4,6(a3);
blt a4,zero,LOOP;
lb a4,4(a3);
c.addi a5,-1;
c.bnez a5,LOOP;
c.lw a1,0(a3);
c.sw a1,0(a0);

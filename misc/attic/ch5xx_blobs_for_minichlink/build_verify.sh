#!/bin/bash
rm -f ch5xx_verify.o ch5xx_verify.bin
riscv64-unknown-elf-as ch5xx_verify.asm -march=rv32imac_zicsr -o ch5xx_verify.o
riscv64-unknown-elf-objcopy -O binary ch5xx_verify.o ch5xx_verify.bin
xxd -i ch5xx_verify.bin > ch5xx_verify.h

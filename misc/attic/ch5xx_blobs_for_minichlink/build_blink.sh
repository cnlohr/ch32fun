#!/bin/bash
rm -f ch5xx_blink.o ch5xx_blink.bin
riscv64-unknown-elf-as ch5xx_blink.asm -march=rv32imac -mabi=ilp32 -o ch5xx_blink.o
riscv64-unknown-elf-objcopy -O binary ch5xx_blink.o ch5xx_blink.bin
xxd -i ch5xx_blink.bin > ch5xx_blink.h

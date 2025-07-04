#!/bin/bash
rm -f ch5xx_write_block.o ch5xx_write_block.bin
riscv64-unknown-elf-as ch5xx_write_block.asm -march=rv32imac_zicsr -o ch5xx_write_block.o
riscv64-unknown-elf-objcopy -O binary ch5xx_write_block.o ch5xx_write_block.bin
xxd -i ch5xx_write_block.bin > ch5xx_write_block.h

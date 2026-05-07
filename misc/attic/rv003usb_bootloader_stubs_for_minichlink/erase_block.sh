#!/bin/bash
rm -f erase_block.o erase_block.bin
riscv64-unknown-elf-as erase_block.asm -march=rv32imac_zicsr -o erase_block.o
riscv64-unknown-elf-objcopy -O binary erase_block.o erase_block.bin
xxd -i erase_block.bin > erase_block.h

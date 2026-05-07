#!/bin/bash
rm -f write_block.o write_block.bin
riscv64-unknown-elf-as write_block.asm -march=rv32imac_zicsr -o write_block.o
riscv64-unknown-elf-objcopy -O binary write_block.o write_block.bin
xxd -i write_block.bin > write_block.h

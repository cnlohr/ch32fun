#!/usr/bin/env bash

SYMBOL=$1
if [ -z "$SYMBOL" ]; then
   echo "Usage: $0 <symbol>"
   exit 1
fi

echo "Searching for symbol: '$SYMBOL'"

ADDR=$(riscv64-unknown-elf-objdump -t *.elf | grep $SYMBOL | awk '{print "0x"$1}')

if [ -z "$ADDR" ]; then
   echo "Error: Symbol '$SYMBOL' not found in the object file."
   exit 1
fi

if ! [[ $ADDR =~ ^0x[0-9a-fA-F]+$ ]]; then
    echo "Error: Address must be in hexadecimal format (e.g., 0x20000000)."
    exit 1
fi

echo "Reading $SYMBOL@$ADDR"

commands=" -s 0x10 0x80000001" # Make the debug module work properly. 

commands+=" -s 0x20 0xc02a717d" # add sp,sp,-16;   sw a0,0(sp)
commands+=" -s 0x21 0xc432c22e" # sw a1,4(sp);     sw a2,8(sp)
commands+=" -s 0x22 0xe0000637" # lui a2,0xe0000
commands+=" -s 0x23 0x0f460613" # add a2,a2,244 # e00000f4 <data0>
commands+=" -s 0x24 0x410c4208" # lw a0,0(a2);  lw a1,0(a0)
commands+=" -s 0x25 0x4502c20c" # sw a1,0(a2);  lw a0,0(sp)
commands+=" -s 0x26 0x46224592" # lw a1,4(sp);  lw a2,8(sp)
commands+=" -s 0x27 0x90026141" # add sp,sp,16; ebreak

commands+=" -s 0x04 $ADDR" # Write address 0x20000000 to DATA0
commands+=" -s 0x17 0x00270000" # Abstact cmd exec progbuf
commands+=" -m 0x16" # Read ABSTRACTCS
commands+=" -m 0x04" # Read DATA0

commands+=" -s 0x10 0x40000001" # Resume(1<<30) without reset(1<<0)

echo $commands | nc localhost 4444

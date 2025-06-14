#!/usr/bin/env bash

SYMBOL=$1
VAL=$2
if [ -z "$SYMBOL" ] || [ -z "$VAL" ]; then
   echo "Usage: $0 <symbol> <value>"
   exit 1
fi

echo "Searching for symbol: '$SYMBOL'"

ADDR="0x$(riscv64-unknown-elf-objdump -t *.elf | grep $SYMBOL | awk '{print $1}')"

if [ -z "$ADDR" ]; then
   echo "Error: Symbol '$SYMBOL' not found in the object file."
   exit 1
fi

if ! [[ $ADDR =~ ^0x[0-9a-fA-F]+$ ]]; then
    echo "Error: Address must be in hexadecimal format (e.g., 0x20000000)."
    exit 1
fi

# Convert the value to hexadecimal format
if ! [[ $VAL =~ ^0x[0-9a-fA-F]+$ ]]; then
    VAL="0x$(printf '%x' "$VAL")"
fi

echo "Setting $SYMBOL@$ADDR -> $VAL"

commands=" -s 0x10 0x80000001" # Make the debug module work properly. 
commands+=" -s 0x20 0x0072a023" # Write wcode of sw x7,0(x5)
commands+=" -s 0x21 0x00100073" # Write wcode of ebreak
commands+=" -s 0x04 $ADDR" # Write address 0x20000000 to DATA0
commands+=" -s 0x17 0x00271005" # Abstract cmd data0->x5 and exec progbuf
commands+=" -s 0x04 $VAL" # Write value to DATA0
commands+=" -s 0x17 0x00271007" # Abstract cmd data0->x7
commands+=" -m 0x16" # Read ABSTRACTCS
commands+=" -s 0x10 0x40000001" # Resume(1<<30) without reset(1<<0)

echo $commands | nc localhost 4444

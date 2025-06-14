#!/usr/bin/env bash

SYMBOL=$1
COUNT=$2
if [ -z "$SYMBOL" ]; then
   echo "Usage: $0 <symbol> [<count>]"
   exit 1
fi

echo "Searching for symbol: '$SYMBOL'"

ADDR="$(riscv64-unknown-elf-objdump -t *.elf | grep $SYMBOL | awk '{print "0x"$1}')"
if [ -z "$COUNT" ]; then
   WORDS=$(riscv64-unknown-elf-objdump -t *.elf | grep $SYMBOL | awk '{print int(("0x"$5 + 3) / 4)}')
else
   WORDS=$COUNT
fi

if [ -z "$ADDR" ]; then
   echo "Error: Symbol '$SYMBOL' not found in the object file."
   exit 1
fi

if ! [[ $ADDR =~ ^0x[0-9a-fA-F]+$ ]]; then
    echo "Error: Address must be in hexadecimal format (e.g., 0x20000000)."
    exit 1
fi

echo "Reading $WORDS words from $SYMBOL@$ADDR"

commands=" -s 0x10 0x80000001" # Make the debug module work properly. 

# Prelude
commands+=" -s 0x20 0xc02a717d" # add sp,sp,-16; sw a0,0(sp)
commands+=" -s 0x21 0x9002c22e" # sw a1,4(sp); ebreak
commands+=" -s 0x17 0x00270000" # Abstact cmd exec progbuf
# commands+=" -m 0x16" # Read ABSTRACTCS

# Batch Read
commands+=" -s 0x20 0x05914188" # lw a0,0(a1); add a1,a1,4
commands+=" -s 0x21 0x90029002" # ebreak

commands+=" -s 0x04 $ADDR" # Write address 0x20000000 to DATA0
commands+=" -s 0x17 0x0027100b" # Abstract cmd data0->x11 and exec progbuf
# commands+=" -m 0x16" # Read ABSTRACTCS

for i in $(seq 0 $((WORDS - 2))); do
   commands+=" -s 0x17 0x0026100a" # Abstract exec and x10->data0
   # commands+=" -m 0x16" # Read ABSTRACTCS
   commands+=" -m 0x04" # Read DATA0
done

commands+=" -s 0x17 0x0022100a" # Abstract cmd x10->data0
# commands+=" -m 0x16" # Read ABSTRACTCS
commands+=" -m 0x04" # Read DATA0

# Postlude
commands+=" -s 0x20 0x45924502" # lw      a0,0(sp); lw      a1,4(sp)
commands+=" -s 0x21 0x90026141" # add     sp,sp,16; ebreak
# commands+=" -s 0x17 0x00270000" # Abstact cmd exec progbuf

commands+=" -s 0x10 0x40000001" # Resume(1<<30) without reset(1<<0)

echo $commands | nc localhost 4444

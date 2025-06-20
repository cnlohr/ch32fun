#!/usr/bin/env python3

import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
import sys
import subprocess
import socket


PERIOD_MS = 1000 / 30
SAMPLES = 200
symbol = sys.argv[1]

fig, ax = plt.subplots()
xdata, ydata = range(SAMPLES), [0] * SAMPLES
(ln,) = plt.plot([], [])

# find elf in current directory
result = subprocess.run(["find", ".", "-name", "*.elf"], capture_output=True, text=True)
if result.returncode != 0:
    print(f"Error: {result.stderr.strip()}")
    sys.exit(1)

elf = result.stdout.splitlines()[0]
print(f"Using ELF file: {elf}")

result = subprocess.run(
    ["riscv64-unknown-elf-objdump", "-t", elf], capture_output=True, text=True
)
if result.returncode != 0:
    print(f"Error: {result.stderr.strip()}")
    sys.exit(1)

symbol_address = -1
for line in result.stdout.splitlines()[4:]:
    name = line.split()[-1]
    if symbol == name:
        symbol_address = int(line.split()[0], 16)
        break

if symbol_address == -1:
    print(f"Error: Symbol '{symbol}' not found in ELF file.")
    sys.exit(1)

print(f"Symbol '{symbol}' found at address: {symbol_address:#x}")

commands = " -s 0x10 0x80000001"  # Make the debug module work properly.

commands += " -s 0x20 0xc02a717d"  # add sp,sp,-16;   sw a0,0(sp)
commands += " -s 0x21 0xc432c22e"  # sw a1,4(sp);     sw a2,8(sp)
commands += " -s 0x22 0xe0000637"  # lui a2,0xe0000
commands += " -s 0x23 0x0f460613"  # add a2,a2,244 # e00000f4 <data0>
commands += " -s 0x24 0x410c4208"  # lw a0,0(a2);  lw a1,0(a0)
commands += " -s 0x25 0x4502c20c"  # sw a1,0(a2);  lw a0,0(sp)
commands += " -s 0x26 0x46224592"  # lw a1,4(sp);  lw a2,8(sp)
commands += " -s 0x27 0x90026141"  # add sp,sp,16; ebreak

commands += f" -s 0x04 {symbol_address:#x}"  # Write address 0x20000000 to DATA0
commands += " -s 0x17 0x00270000"  # Abstact cmd exec progbuf
commands += " -m 0x16"  # Read ABSTRACTCS
commands += " -m 0x04"  # Read DATA0

commands += " -s 0x10 0x40000001"  # Resume(1<<30) without reset(1<<0)

command = commands.encode()


def read_memory():
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect(("127.0.0.1", 4444))
    s.send(command)

    data = s.recv(1024).decode()
    s.close()

    return data


def get_value():
    value = int(read_memory().splitlines()[-1].split()[-1], 16)
    print(f"Received value: {value}")
    return value


def init():
    ax.set_xlim(0, SAMPLES)
    ax.set_ylim(0, 1024)
    return (ln,)


def update(frame):
    global ydata
    value = get_value()
    ydata = ydata[1:] + [value]
    ln.set_data(xdata, ydata)
    return (ln,)


ani = FuncAnimation(
    fig, update, frames=SAMPLES, init_func=init, blit=True, interval=PERIOD_MS
)
plt.show()

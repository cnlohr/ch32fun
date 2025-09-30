# IRReceiver using capture DMA on Timer
This example demonstrates using Timer1 with DMA to capture falling edges
from an IR receiver and decode NEC protocol frames.

## Features
Hardware-efficient: Uses Timer1 + DMA for precise timing capture
NEC Protocol Support: Decodes most common IR remote signals
Tested: Tested with an original Apple TV remote and a generic Chinese remote 

## Code Behavior
Point the remote at the IR Sensor and press any button. The code should
display the first 2 words of the IR signal

Outputs:
NEC: 0x77E1 0xD0AA
NEC: 0x77E1 0x10AA
NEC: 0x77E1 0xBAAA
NEC: 0x77E1 0x20AA
NEC: 0x77E1 0xE0AA
NEC: 0x77E1 0xB0AA
...

## NEC Protocol

Start Sequence:   ┌─────────┐    ┌─────────┐
                  │  9ms    │    │  4.5ms  │
Leader Pulse:     │  BURST  │    │  SPACE  │  (38kHz carrier)
                  └─────────┘    └─────────┘        

Logical '0':      ┌─────┐    ┌─────┐
                  │560µs│    │560µs│  = 1.125ms total
                  └─────┘    └─────┘ 

Logical '1':      ┌─────┐    ┌──────────┐
                  │560µs│    │  1.69ms  │  = 2.25ms total  
                  └─────┘    └──────────┘


## SETUP
Wire the IR sensor as follow:
GND - GND
Vcc - Vcc
OUT - PD2 

ENJOY :)
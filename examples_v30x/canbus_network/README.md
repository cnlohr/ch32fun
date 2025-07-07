# Example CAN Bus Network

# Hardware
This example uses PD0 and PD1 as CAN RX and TX pin.
Normally, you would use a PHY to connect the CAN controller to the bus, but you can make a simple PHY for testing purposes with a few diodes and resistors.

> [!WARNING]
> DO NOT connect this simple PHY to an actual CAN bus! This is only for testing if you don't have a PCB with a proper PHY yet.
```
                    3.3V   
                     |     
                    .-.    
                    | | 470Ω
                    '-'    
              3.3V   |                            3.3V   
                |    +----+--- CAN_L ---+----+     |
               .-.   |    |             |    |    .-.    
          470Ω | |  _|_   |             |   _|_   | | 470Ω
               '-'  / \  \ /           \ /  / \   '-'    
                |    T    T             T    T     |     
   CAN_RX1 >----+----+    |             |    +-----+-----< CAN_RX2
   CAN_TX1 >--------------+             +----------------< CAN_TX2
       GND >---------------------------------------------< GND
```
You can add as many nodes as you want.
This circuit is not differential.

# Usage
Flash the code with `TRANSMIT 1` on one device and `TRANSMIT 0` on the other device.

If you add more nodes, change the `ID` so that you know which node is which.
You can mix extended and standard IDs.

All nodes must have the same `BAUD`.

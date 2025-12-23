# I2C Master Communication Example

This example demonstrates I2C communication with an I2C slave device.
You can use this with (one or the other):
- BH1750 light sensor (actual hardware)
- Another CH32X035 running i2c_slave_test firmware

if you are connecting a CH32X03X to a BH1750, the default slave address for the sensor is 0x23
if you are connecting to another CH32X03X, the default slave address for the slave CH32X03X is 0x66
For the CH32X03X slave, it needs to run the `i2c_slave_test` example

Setup for BH1750 sensor:
- Connect SCL to PA10 (I2C1 SCL)
- Connect SDA to PA11 (I2C1 SDA)

Setup for CH32X03X slave:
- Connect both boards to the same I2C bus
- Connect SCL (PA10) of both boards together
- Connect SDA (PA11) of both boards together

Expected output readings from CH32X03X slave:
Read 1 byte (cmd 0x01): 0x11
Read 2 bytes (cmd 0x13): 0x5D 0x66 
Read 4 bytes (cmd 0x14): 0x77 0x88 0x99 0xAA 
write buffer (cmd 0x31): successful
Read buffer (cmd 0x30): 0xAA 0xBB 0xCC 0xFF 0xFF 

0x3x Slave Command Set:
0x01 - Minick BH1750 Power on command (returns 1 byte)
0x23 - Minmick BH1750 Resolution command (returns 1 byte) 
0x13 - Read 2 bytes from slave
0x14 - Read 4 bytes from slave
0x30 - Read from slave's writable buffer (32 bytes)
0x31 - Write to slave's writable buffer (32 bytes)

Command 0x30: Read from writable buffer
Format: { 0x30, start_index }
- start_index: Position to start reading from (0-31)
Since buffer size is 32 bytes (0-31), reading from index 29:
- Returns bytes 29, 30, 31 (3 valid bytes)
- Remaining requested bytes return 0xFF (buffer boundary exceeded)

Command 0x31: Write to writable buffer  
Format: { 0x31, start_index, data0, data1, ... }
- start_index: Position to start writing to (0-31)
- dataX: Bytes to write (up to buffer boundary)
Since buffer ends at index 31:
- Writes 0xAA to index 29, 0xBB to index 30, 0xCC to index 31
- 0xDD is not written (buffer full)
- Returns success for written bytes only

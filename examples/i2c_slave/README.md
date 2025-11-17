# I2C peripheral in slave mode

This library and example show how to use the I2C peripheral in slave mode.

The library uses a one-byte address, allowing for up to 256 registers to be defined.

The first byte written to the device within a transaction determines the offset for following reads and writes, emulating a simple EEPROM.

The example will turn on a LED connected to PA2 when the LSB of register 0 is set to 1 and off when it's set to 0.

# Usage

## Pin Setup

Caution: CH32V003 only supports I2C on a few specific pins.

Initialize the default I2C1 SDA and SCL pins as open-drain outputs. By default, pins PC1 and PC2 are used.
```
funPinMode(PC1, GPIO_CFGLR_OUT_10Mhz_AF_OD); // SDA
funPinMode(PC2, GPIO_CFGLR_OUT_10Mhz_AF_OD); // SCL
```

For chips other than the CH32V003 you should change the above pin numbers to the pins corresponding with the I2C1 peripheral.

## Alternate Pins

Optional: If you want to use alternative pins for the I2C periperal, then you must take an additional step to configure the chip to use one of the 2 alternative pin mappings.

You can do this using the `I2C1_RM` and `I2C1REMAP1` fields of the `AFIO_PCFR1` register.

Remapping examples:

```c++
// 2 bits [bit 22 and bit 1] of AFIO_PCFR1 register are what control I2C1 pin remapping on ch32v003
// [high bit, low bit]
// [I2C1REMAP1, I2C1_RM]

// [0, 0]				default mapping (SCL on pin PC2, SDA on pin PC1)
// (note: you don't need to do this, since it's the defualt:)
AFIO->PCFR1 &= ~AFIO_PCFR1_I2C1_HIGH_BIT_REMAP;	  // set high bit = 0  (I2C1REMAP1)
AFIO->PCFR1 &= ~AFIO_PCFR1_I2C1_REMAP;            // set low bit = 0   (I2C1_RM)

// [0, 1]:			Remapping option #1 (SCL on pin PD1, SDA on pin PD0)
AFIO->PCFR1 &= ~AFIO_PCFR1_I2C1_HIGH_BIT_REMAP;   // set high bit = 0  (I2C1REMAP1)
AFIO->PCFR1 |= AFIO_PCFR1_I2C1_REMAP;             // set low bit = 1   (I2C1_RM)

// [1, X]:			Remapping option #2 (SCL on pin PC5, SDA on pin PC6)
AFIO->PCFR1 |= AFIO_PCFR1_I2C1_HIGH_BIT_REMAP;    // set high bit = 1  (I2C1REMAP1)
AFIO->PCFR1 |= AFIO_PCFR1_I2C1_REMAP;             // set low bit [ignored / don't care]    (I2C1_RM)
```

## Initialization

Initialize the I2C1 peripheral in slave mode using:

```
SetupI2CSlave(0x09, i2c_registers, sizeof(i2c_registers), onWrite, onRead, false);
```

In which `0x09` is the I2C address to listen on and i2c_registers is a pointer to a volitile uint8_t array.

The `onWrite` and `onRead` functions are optional callbacks used to react to the registers being written to or read from.

```
void onWrite(uint8_t reg, uint8_t length) {}
void onRead(uint8_t reg) {}
```

The last boolean argument is for making the registers read only via I2C.

You can also enable and disable writing using the functions

```
void SetI2CSlaveReadOnly(bool read_only);
void SetSecondaryI2CSlaveReadOnly(bool read_only);
```

The I2C1 peripheral can also listen on a secondary address. To enable this feature call the following function:

```
SetupSecondaryI2CSlave(0x42, i2c_registers2, sizeof(i2c_registers2), onWrite2, onRead2, false);
```

The function arguments are identical to the normal `SetupI2CSlave` function. The secondary I2C address acts like a completely separate I2C device with it's own registers.

Calling `SetupSecondaryI2CSlave` with the I2C address set to 0 disables listening on the secondary address.

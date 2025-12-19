# Hardware AES

This example shows how to use hardware AES encoder/decoder on ch5xx chips (the ones with BLE). All tested chips have this example working, except ch570. Even though it has a radio and can be used for BLE in some capacity, it seems that AES module is disabled/missing/broken on this chip. This results in the BLE failing in the pairing (bonding) operation.

## Registers

AES functions in WCH BLE lib are represented by this 4 functions:

``void AES_DevAESDec(uint32_t *key, uint32_t * plaintextData, uint32_t * decryptData)``

``void AES_DevAESEnc(uint32_t *key, uint32_t * plaintextData, uint32_t * encryptData)``

``void AES_DevPktDec(uint32_t param_1, uint32_t * param_2)``

``void AES_DevPktEnc(uint32_t param_1, uint32_t * param_2)``

The first two were RE'd into one ``doAES`` function in this example. Another two seem to be for bulk packet processing, maybe involving some DMA, but they haven't been reverse engineered yet. They use 4 registers for unknown purposes, and these registers are yet to be named.

The second register ``STA`` is used for tracking ongoing job, but it seems that it's used just as a global flag, that is manually set and then reset in the BLE interrupt, it doesn't clear automatically. I've found that you can just check the first bit of ``AES->CFG`` to see when it's done.

The second bit of ``AES->CFG`` determines if operation is encoding (0) or decoding (1).

Then there are two sets of 4 ``uint32_t`` registers for 128 bit key and data. Output is being put in the same registers where input was.

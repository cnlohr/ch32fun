// MIT License
// Copyright (c) 2025 UniTheCat
// Tested with Ch32X03x and CH32V30x

#define I2C_DEFAULT_TIMEOUT 100000

//! ####################################
//! I2C INIT FUNCTIONS
//! ####################################

void i2c_init(I2C_TypeDef* I2Cx, u32 PCLK, u32 i2cSpeed_Hz) {
	// Enable I2C clock
	if (I2Cx == I2C1) {
		RCC->APB1PCENR |= RCC_APB1Periph_I2C1;
	}
	#ifdef I2C2
	else if (I2Cx == I2C2) {
		RCC->APB1PCENR |= RCC_APB1Periph_I2C2;
	}
	#endif

	// Disable I2C before configuration
	I2Cx->CTLR1 &= ~I2C_CTLR1_PE;

	// configure I2C clock
	I2Cx->CTLR2 = (PCLK / 1000000);
	I2Cx->CKCFGR = PCLK / (i2cSpeed_Hz << 1);	// PeripheralClock / (100KHz * 2)

	// Enable I2C
	I2Cx->CTLR1 |= I2C_CTLR1_PE;

	// Enable ACK
	I2Cx->CTLR1 |= I2C_CTLR1_ACK;
}

u8 i2c_start(I2C_TypeDef* I2Cx, u8 i2cAddress, u8 isRead) {
	//# Wait while BUSY, when BUSY is set to 0 then continue
	u32 timeout = I2C_DEFAULT_TIMEOUT;
	while((I2Cx->STAR2 & I2C_STAR2_BUSY) && --timeout);
	// if (timeout == 0) { I2Cx->CTLR1 |= I2C_CTLR1_STOP; return 0x11; }

	//# Generate START condition
	I2Cx->CTLR1 |= I2C_CTLR1_START;

	//# Wait while SB is 0, when SB is set to 1 then continue
	timeout = I2C_DEFAULT_TIMEOUT;
	while(!(I2Cx->STAR1 & I2C_STAR1_SB) && --timeout);
	if (timeout == 0) { I2Cx->CTLR1 |= I2C_CTLR1_STOP; return 0x12; }
	// printf("timeoutB: %d\n", I2C_DEFAULT_TIMEOUT - timeout);

	//# Send address + read/write. Write = 0, Read = 1
	I2Cx->DATAR = (i2cAddress << 1) | isRead;

	//# Wait while ADDR is 0, if ADDR is set to 1 then continue
	timeout = I2C_DEFAULT_TIMEOUT;
	while(!(I2Cx->STAR1 & I2C_STAR1_ADDR) && --timeout);
	if (timeout == 0) { I2Cx->CTLR1 |= I2C_CTLR1_STOP; return 0x13; }
	// printf("timeoutC: %d\n", I2C_DEFAULT_TIMEOUT - timeout);

	//! REQUIRED. Clear ADDR by reading STAR1 then STAR2
	(void)I2Cx->STAR1;
	(void)I2Cx->STAR2;
	return 0;
}

void i2c_scan(I2C_TypeDef* I2Cx, void (*onPingFound)(u8 address)) {
	// mininum 0x08 to 0x77 (0b1110111)
	for (int i = 0x08; i < 0x77; i++) {
		u8 ping = i2c_start(I2Cx, i, 1);
		
		//# Generate STOP condition
		I2Cx->CTLR1 |= I2C_CTLR1_STOP;
		if (ping == 0) onPingFound(i);
	}
}

//! ####################################
//! I2C SEND FUNCTION
//! ####################################

u8 i2c_sendBytes_noStop(I2C_TypeDef* I2Cx, u8 i2cAddress, u8* buffer, u8 len) {
	u8 err = i2c_start(I2Cx, i2cAddress, 0); // Write mode
	if (err) return err;
	u32 timeout;

	for(u8 i = 0; i < len; i++) {
		//# Wait for register empty
		timeout = I2C_DEFAULT_TIMEOUT;
		while(!(I2Cx->STAR1 & I2C_STAR1_TXE) && --timeout);
		if (timeout == 0) { I2Cx->CTLR1 |= I2C_CTLR1_STOP; return 0x21; }
		I2Cx->DATAR = buffer[i];		// Send data
	}

	//# Wait for transmission complete. Wait while BTF is 0, when set to 1 continue
	timeout = I2C_DEFAULT_TIMEOUT;
	while(!(I2Cx->STAR1 & I2C_STAR1_BTF) && --timeout);
	if (timeout == 0) { I2Cx->CTLR1 |= I2C_CTLR1_STOP; return 0x22; }
	
	return 0;
}

u8 i2c_sendBytes(I2C_TypeDef* I2Cx, u8 i2cAddress, u8* buffer, u8 len) {
	u8 err = i2c_sendBytes_noStop(I2Cx, i2cAddress, buffer, len);
	//# Generate STOP condition
	I2Cx->CTLR1 |= I2C_CTLR1_STOP;
	return err;
}

u8 i2c_sendByte(I2C_TypeDef* I2Cx, u8 i2cAddress, u8 data) {
	return i2c_sendBytes(I2Cx, i2cAddress, &data, 1);
}


//! ####################################
//! I2C RECEIVE FUNCTIONS
//! ####################################

u8 i2c_readBytes(I2C_TypeDef* I2Cx, u8 i2cAddress, u8* buffer, u8 len) {
	u8 err = i2c_start(I2Cx, i2cAddress, 1); // Read mode
	if (err) return err;

	//# Enable ACK at the beginning
	I2Cx->CTLR1 |= I2C_CTLR1_ACK;

	for(u8 i = 0; i < len; i++) {
		//# Before reading the last bytes, disable ACK to signal the slave to stop sending
		if(i == len-1) I2Cx->CTLR1 &= ~I2C_CTLR1_ACK;
		
		//# Wait for data. Wait while RxNE is 0, when set to 1 continue
		u32 timeout = I2C_DEFAULT_TIMEOUT;
		while(!(I2Cx->STAR1 & I2C_STAR1_RXNE) && --timeout);
		if (timeout == 0) { I2Cx->CTLR1 |= I2C_CTLR1_STOP; return 0x31; }

		//# Read data
		buffer[i] = I2Cx->DATAR;
	}
	
	//# Generate STOP condition
	I2Cx->CTLR1 |= I2C_CTLR1_STOP;
	return 0;	
}

// Write to register and then do read data, no stop inbetween
u8 i2c_readRegTx_buffer(I2C_TypeDef* I2Cx, u8 i2cAddress,
						u8 *tx_buf, u8 tx_len, u8 *rx_buf, u8 rx_len
) {
	u8 err = i2c_sendBytes_noStop(I2Cx, i2cAddress, tx_buf, tx_len);	// Send register address
	if (err) return err;
	err = i2c_readBytes(I2Cx, i2cAddress, rx_buf, rx_len); 	// Read data
	return err;
}

u8 i2c_readReg_buffer(I2C_TypeDef* I2Cx, u8 i2cAddress, u8 reg, u8 *rx_buf, u8 rx_len) {
	return i2c_readRegTx_buffer(I2Cx, i2cAddress, &reg, 1, rx_buf, rx_len);
}

//! ####################################
//! I2C SLAVE FUNCTIONS
//! ####################################

void i2c_slave_init(I2C_TypeDef* I2Cx, u16 self_addr, u32 PCLK, u32 i2cSpeed_Hz) {
	i2c_init(I2Cx, PCLK, i2cSpeed_Hz);

	// Configure the CH32 I2C slave address to make it an I2C slave
	I2Cx->OADDR1  = (self_addr << 1);
	I2Cx->OADDR2 = 0;

	I2Cx->CTLR2 |= I2C_CTLR2_ITEVTEN | I2C_CTLR2_ITERREN | I2C_CTLR2_ITBUFEN;

	// Enable Event and Error Interrupts
	if (I2Cx == I2C1) {
		NVIC_EnableIRQ(I2C1_EV_IRQn); // I2C Event interrupt
		NVIC_EnableIRQ(I2C1_ER_IRQn); // I2C Error interrupt
	}

	#ifdef I2C2
		else if (I2Cx == I2C2) {
			NVIC_EnableIRQ(I2C2_EV_IRQn); // I2C Event interrupt
			NVIC_EnableIRQ(I2C2_ER_IRQn); // I2C Error interrupt
		}
	#endif
}

void I2C1_ER_IRQHandler(void) __attribute__((interrupt));
void I2C1_ER_IRQHandler(void) {
	// get I2C status
	uint16_t STAR1 = I2C1->STAR1;

	// Obtain and clear Bus error
	if (STAR1 & I2C_STAR1_BERR) {
		I2C1->STAR1 &= ~(I2C_STAR1_BERR);
	}

	// Obtain and clear Arbitration lost error
	if (STAR1 & I2C_STAR1_ARLO) {
		I2C1->STAR1 &= ~(I2C_STAR1_ARLO);
	}

	// Obtain and clear Acknowledge failure error
	if (STAR1 & I2C_STAR1_AF) {
		I2C1->STAR1 &= ~(I2C_STAR1_AF);
	}
}


#ifdef I2C2
	void I2C2_ER_IRQHandler(void) __attribute__((interrupt));
	void I2C2_ER_IRQHandler(void) {
		// get I2C status
		uint16_t STAR1 = I2C2->STAR1;

		// Obtain and clear Bus error
		if (STAR1 & I2C_STAR1_BERR) {
			I2C2->STAR1 &= ~(I2C_STAR1_BERR);
		}

		// Obtain and clear Arbitration lost error
		if (STAR1 & I2C_STAR1_ARLO) {
			I2C2->STAR1 &= ~(I2C_STAR1_ARLO);
		}

		// Obtain and clear Acknowledge failure error
		if (STAR1 & I2C_STAR1_AF) {
			I2C2->STAR1 &= ~(I2C_STAR1_AF);
		}
	}
#endif
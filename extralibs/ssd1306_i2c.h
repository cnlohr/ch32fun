/*
 * Single-File-Header for SSD1306 I2C interface
 * 05-07-2023 E. Brombaugh
 */

#ifndef _SSD1306_I2C_H
#define _SSD1306_I2C_H

#include <string.h>

// For the CH32V203, we support remapping, if so,
// #define SSD1306_REMAP_I2C

// SSD1306 I2C address
#define SSD1306_I2C_ADDR 0x3c

// I2C Bus clock rate - must be lower the Logic clock rate
#define SSD1306_I2C_CLKRATE 1000000

// I2C Logic clock rate - must be higher than Bus clock rate
#define SSD1306_I2C_PRERATE 2000000

// uncomment this for high-speed 36% duty cycle, otherwise 33%
#define SSD1306_I2C_DUTY

// I2C Timeout count
#define TIMEOUT_MAX 100000

// uncomment this to enable IRQ-driven operation
//#define SSD1306_I2C_IRQ

#ifdef SSD1306_I2C_IRQ
// some stuff that IRQ mode needs
volatile uint8_t ssd1306_i2c_send_buffer[64], *ssd1306_i2c_send_ptr, ssd1306_i2c_send_sz, ssd1306_i2c_irq_state;

// uncomment this to enable time diags in IRQ
//#define IRQ_DIAG
#endif

/*
 * init just I2C
 */
void ssd1306_i2c_setup(void)
{
	uint16_t tempreg;
	
	// Reset I2C1 to init all regs
	RCC->APB1PRSTR |= RCC_APB1Periph_I2C1;
	RCC->APB1PRSTR &= ~RCC_APB1Periph_I2C1;
	
	// set freq
	tempreg = I2C1->CTLR2;
	tempreg &= ~I2C_CTLR2_FREQ;
	tempreg |= (FUNCONF_SYSTEM_CORE_CLOCK/SSD1306_I2C_PRERATE)&I2C_CTLR2_FREQ;
	I2C1->CTLR2 = tempreg;
	
	// Set clock config
	tempreg = 0;
#if (SSD1306_I2C_CLKRATE <= 100000)
	// standard mode good to 100kHz
	tempreg = (FUNCONF_SYSTEM_CORE_CLOCK/(2*SSD1306_I2C_CLKRATE))&I2C_CKCFGR_CCR;
#else
	// fast mode over 100kHz
#ifndef SSD1306_I2C_DUTY
	// 33% duty cycle
	tempreg = (FUNCONF_SYSTEM_CORE_CLOCK/(3*SSD1306_I2C_CLKRATE))&I2C_CKCFGR_CCR;
#else
	// 36% duty cycle
	tempreg = (FUNCONF_SYSTEM_CORE_CLOCK/(25*SSD1306_I2C_CLKRATE))&I2C_CKCFGR_CCR;
	tempreg |= I2C_CKCFGR_DUTY;
#endif
	tempreg |= I2C_CKCFGR_FS;
#endif
	I2C1->CKCFGR = tempreg;

#ifdef SSD1306_I2C_IRQ
	// enable IRQ driven operation
	NVIC_EnableIRQ(I2C1_EV_IRQn);
	
	// initialize the state
	ssd1306_i2c_irq_state = 0;
#endif
	
	// Enable I2C
	I2C1->CTLR1 |= I2C_CTLR1_PE;

	// set ACK mode
	I2C1->CTLR1 |= I2C_CTLR1_ACK;
}

/*
 * error descriptions
 */
char *errstr[] =
{
	"not busy",
	"master mode",
	"transmit mode",
	"tx empty",
	"transmit complete",
};

/*
 * error handler
 */
uint8_t ssd1306_i2c_error(uint8_t err)
{
	// report error
	printf("ssd1306_i2c_error - timeout waiting for %s\n\r", errstr[err]);
	
	// reset & initialize I2C
	ssd1306_i2c_setup();

	return 1;
}

// event codes we use
#define  SSD1306_I2C_EVENT_MASTER_MODE_SELECT ((uint32_t)0x00030001)  /* BUSY, MSL and SB flag */
#define  SSD1306_I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED ((uint32_t)0x00070082)  /* BUSY, MSL, ADDR, TXE and TRA flags */
#define  SSD1306_I2C_EVENT_MASTER_BYTE_TRANSMITTED ((uint32_t)0x00070084)  /* TRA, BUSY, MSL, TXE and BTF flags */

/*
 * check for 32-bit event codes
 */
uint8_t ssd1306_i2c_chk_evt(uint32_t event_mask)
{
	/* read order matters here! STAR1 before STAR2!! */
	uint32_t status = I2C1->STAR1 | (I2C1->STAR2<<16);
	return (status & event_mask) == event_mask;
}

#ifdef SSD1306_I2C_IRQ
/*
 * packet send for IRQ-driven operation
 */
uint8_t ssd1306_i2c_send(uint8_t addr, uint8_t *data, uint8_t sz)
{
	int32_t timeout;
	
#ifdef IRQ_DIAG
	GPIOC->BSHR = (1<<(3));
#endif
	
	// error out if buffer under/overflow
	if((sz > sizeof(ssd1306_i2c_send_buffer)) || !sz)
		return 2;
	
	// wait for previous packet to finish
	while(ssd1306_i2c_irq_state);
	
#ifdef IRQ_DIAG
	GPIOC->BSHR = (1<<(16+3));
	GPIOC->BSHR = (1<<(4));
#endif
	
	// init buffer for sending
	ssd1306_i2c_send_sz = sz;
	ssd1306_i2c_send_ptr = ssd1306_i2c_send_buffer;
	memcpy((uint8_t *)ssd1306_i2c_send_buffer, data, sz);
	
	// wait for not busy
	timeout = TIMEOUT_MAX;
	while((I2C1->STAR2 & I2C_STAR2_BUSY) && (timeout--));
	if(timeout==-1)
		return ssd1306_i2c_error(0);

	// Set START condition
	I2C1->CTLR1 |= I2C_CTLR1_START;

	// wait for master mode select
	timeout = TIMEOUT_MAX;
	while((!ssd1306_i2c_chk_evt(SSD1306_I2C_EVENT_MASTER_MODE_SELECT)) && (timeout--));
	if(timeout==-1)
		return ssd1306_i2c_error(1);
	
	// send 7-bit address + write flag
	I2C1->DATAR = addr<<1;

	// wait for transmit condition
	timeout = TIMEOUT_MAX;
	while((!ssd1306_i2c_chk_evt(SSD1306_I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) && (timeout--));
	if(timeout==-1)
		return ssd1306_i2c_error(2);

	// Enable TXE interrupt
	I2C1->CTLR2 |= I2C_CTLR2_ITBUFEN | I2C_CTLR2_ITEVTEN;
	ssd1306_i2c_irq_state = 1;

#ifdef IRQ_DIAG
	GPIOC->BSHR = (1<<(16+4));
#endif
	
	// exit
	return 0;
}

/*
 * IRQ handler for I2C events
 */
void I2C1_EV_IRQHandler(void) __attribute__((interrupt));
void I2C1_EV_IRQHandler(void)
{
	uint16_t STAR1, STAR2 __attribute__((unused));
	
#ifdef IRQ_DIAG
	GPIOC->BSHR = (1<<(4));
#endif

	// read status, clear any events
	STAR1 = I2C1->STAR1;
	STAR2 = I2C1->STAR2;
	
	/* check for TXE */
	if(STAR1 & I2C_STAR1_TXE)
	{
		/* check for remaining data */
		if(ssd1306_i2c_send_sz--)
			I2C1->DATAR = *ssd1306_i2c_send_ptr++;

		/* was that the last byte? */
		if(!ssd1306_i2c_send_sz)
		{
			// disable TXE interrupt
			I2C1->CTLR2 &= ~(I2C_CTLR2_ITBUFEN | I2C_CTLR2_ITEVTEN);
			
			// reset IRQ state
			ssd1306_i2c_irq_state = 0;
			
			// wait for tx complete
			while(!ssd1306_i2c_chk_evt(SSD1306_I2C_EVENT_MASTER_BYTE_TRANSMITTED));

			// set STOP condition
			I2C1->CTLR1 |= I2C_CTLR1_STOP;
		}
	}

#ifdef IRQ_DIAG
	GPIOC->BSHR = (1<<(16+4));
#endif
}
#else
/*
 * low-level packet send for blocking polled operation via i2c
 */
uint8_t ssd1306_i2c_send(uint8_t addr, const uint8_t *data, int sz)
{
	int32_t timeout;
	
	// wait for not busy
	timeout = TIMEOUT_MAX;
	while((I2C1->STAR2 & I2C_STAR2_BUSY) && (timeout--));
	if(timeout==-1)
		return ssd1306_i2c_error(0);

	// Set START condition
	I2C1->CTLR1 |= I2C_CTLR1_START;
	
	// wait for master mode select
	timeout = TIMEOUT_MAX;
	while((!ssd1306_i2c_chk_evt(SSD1306_I2C_EVENT_MASTER_MODE_SELECT)) && (timeout--));
	if(timeout==-1)
		return ssd1306_i2c_error(1);
	
	// send 7-bit address + write flag
	I2C1->DATAR = addr<<1;

	// wait for transmit condition
	timeout = TIMEOUT_MAX;
	while((!ssd1306_i2c_chk_evt(SSD1306_I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) && (timeout--));
	if(timeout==-1)
		return ssd1306_i2c_error(2);

	// send data one byte at a time
	while(sz--)
	{
		// wait for TX Empty
		timeout = TIMEOUT_MAX;
		while(!(I2C1->STAR1 & I2C_STAR1_TXE) && (timeout--));
		if(timeout==-1)
			return ssd1306_i2c_error(3);
		
		// send command
		I2C1->DATAR = *data++;
	}

	// wait for tx complete
	timeout = TIMEOUT_MAX;
	while((!ssd1306_i2c_chk_evt(SSD1306_I2C_EVENT_MASTER_BYTE_TRANSMITTED)) && (timeout--));
	if(timeout==-1)
		return ssd1306_i2c_error(4);

	// set STOP condition
	I2C1->CTLR1 |= I2C_CTLR1_STOP;
	
	// we're happy
	return 0;
}
#endif

/*
 * high-level packet send for I2C
 */
uint8_t ssd1306_pkt_send(const uint8_t *data, int sz, uint8_t cmd)
{
	uint8_t pkt[33];
	
	/* build command or data packets */
	if(cmd)
	{
		pkt[0] = 0;
		pkt[1] = *data;
	}
	else
	{
		pkt[0] = 0x40;
		memcpy(&pkt[1], data, sz);
	}
	return ssd1306_i2c_send(SSD1306_I2C_ADDR, pkt, sz+1);
}

/*
 * init I2C and GPIO
 */
uint8_t ssd1306_i2c_init(void)
{
	// Enable GPIOC and I2C
	RCC->APB1PCENR |= RCC_APB1Periph_I2C1;

#ifdef CH32V20x
	RCC->APB2PCENR |= RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO;

#ifdef SSD1306_REMAP_I2C
	AFIO->PCFR1 |= AFIO_PCFR1_I2C1_REMAP;
	funPinMode( PB8, GPIO_CFGLR_OUT_10Mhz_AF_OD );
	funPinMode( PB9, GPIO_CFGLR_OUT_10Mhz_AF_OD );
#else
	funPinMode( PB6, GPIO_CFGLR_OUT_10Mhz_AF_OD );
	funPinMode( PB7, GPIO_CFGLR_OUT_10Mhz_AF_OD );
#endif

#else
	RCC->APB2PCENR |= RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO;
	// PC1 is SDA, 10MHz Output, alt func, open-drain
	GPIOC->CFGLR &= ~(0xf<<(4*1));
	GPIOC->CFGLR |= (GPIO_Speed_10MHz | GPIO_CNF_OUT_OD_AF)<<(4*1);
	
	// PC2 is SCL, 10MHz Output, alt func, open-drain
	GPIOC->CFGLR &= ~(0xf<<(4*2));
	GPIOC->CFGLR |= (GPIO_Speed_10MHz | GPIO_CNF_OUT_OD_AF)<<(4*2);
#endif

#ifdef IRQ_DIAG
	// GPIO diags on PC3/PC4
	GPIOC->CFGLR &= ~(0xf<<(4*3));
	GPIOC->CFGLR |= (GPIO_Speed_10MHz | GPIO_CNF_OUT_PP)<<(4*3);
	GPIOC->BSHR = (1<<(16+3));
	GPIOC->CFGLR &= ~(0xf<<(4*4));
	GPIOC->CFGLR |= (GPIO_Speed_10MHz | GPIO_CNF_OUT_PP)<<(4*4);
	GPIOC->BSHR = (1<<(16+4));
#endif

	// load I2C regs
	ssd1306_i2c_setup();
	
#if 0
	// test if SSD1306 is on the bus by sending display off command
	uint8_t command = 0xAF;
	return ssd1306_pkt_send(&command, 1, 1);
#else
	return 0;
#endif
}

/*
 * reset is not used for SSD1306 I2C interface
 */
void ssd1306_rst(void)
{
}
#endif

/*
 * Single-File-Header for SSD1306 emulated I2C interface
 * 02-12-2025 cnlohr
 */

#ifndef _SSD1306_I2C_BITBANG_H
#define _SSD1306_I2C_BITBANG_H

#include <string.h>

// SSD1306 I2C address
#ifndef SSD1306_I2C_ADDR
#define SSD1306_I2C_ADDR 0x3c
#endif

#ifndef SSD1306_I2C_BITBANG_SDA
#define SSD1306_I2C_BITBANG_SDA PC1
#endif

#ifndef SSD1306_I2C_BITBANG_SCL
#define SSD1306_I2C_BITBANG_SCL PC2
#endif

/*
 * init just I2C
 */
void ssd1306_i2c_setup(void)
{
	funGpioInitAll();
	funPinMode( SSD1306_I2C_BITBANG_SDA, GPIO_CFGLR_OUT_10Mhz_PP );
	funDigitalWrite( SSD1306_I2C_BITBANG_SDA, 1 );
	funPinMode( SSD1306_I2C_BITBANG_SCL, GPIO_CFGLR_OUT_10Mhz_PP );
	funDigitalWrite( SSD1306_I2C_BITBANG_SCL, 1 );
}

#define SDA_HIGH funDigitalWrite( SSD1306_I2C_BITBANG_SDA, 1 );
#define SCL_HIGH funDigitalWrite( SSD1306_I2C_BITBANG_SCL, 1 );
#define SDA_LOW  funDigitalWrite( SSD1306_I2C_BITBANG_SDA, 0 );
#define SCL_LOW  funDigitalWrite( SSD1306_I2C_BITBANG_SCL, 0 );
#define SDA_IN   funDigitalRead( SSD1306_I2C_BITBANG_SDA );
#ifndef I2CSPEEDBASE
#define I2CSPEEDBASE 1
#endif
#ifndef I2CDELAY_FUNC
#define I2CDELAY_FUNC(x) ADD_N_NOPS(x*1)
#endif

static void ssd1306_i2c_sendstart()
{
	SCL_HIGH
	I2CDELAY_FUNC( 1 * I2CSPEEDBASE );
	SDA_LOW
	I2CDELAY_FUNC( 1 * I2CSPEEDBASE );
	SCL_LOW
	I2CDELAY_FUNC( 1 * I2CSPEEDBASE );
}

void ssd1306_i2c_sendstop()
{
	SDA_LOW
	I2CDELAY_FUNC( 1 * I2CSPEEDBASE );
	SCL_LOW
	I2CDELAY_FUNC( 1 * I2CSPEEDBASE );
	SCL_HIGH
	I2CDELAY_FUNC( 1 * I2CSPEEDBASE );
	SDA_HIGH
	I2CDELAY_FUNC( 1 * I2CSPEEDBASE );
}

//Return nonzero on failure.
unsigned char ssd1306_i2c_sendbyte( unsigned char data )
{
	unsigned int i;
	for( i = 0; i < 8; i++ )
	{
		I2CDELAY_FUNC( 1 * I2CSPEEDBASE );
		if( data & 0x80 )
		{ SDA_HIGH; }
		else
		{ SDA_LOW; }
		data<<=1;
		I2CDELAY_FUNC( 1 * I2CSPEEDBASE );
		SCL_HIGH
		I2CDELAY_FUNC( 2 * I2CSPEEDBASE );
		SCL_LOW
	}

	//Immediately after sending last bit, open up DDDR for control.
	I2CDELAY_FUNC( 1 * I2CSPEEDBASE );
	funPinMode( SSD1306_I2C_BITBANG_SDA, GPIO_CFGLR_IN_PUPD );
	SDA_HIGH
	I2CDELAY_FUNC( 1 * I2CSPEEDBASE );
	SCL_HIGH
	I2CDELAY_FUNC( 1 * I2CSPEEDBASE );
	i = SDA_IN;
	I2CDELAY_FUNC( 1 * I2CSPEEDBASE );
	SCL_LOW
	I2CDELAY_FUNC( 1 * I2CSPEEDBASE );
	SDA_HIGH // Maybe?
	funPinMode( SSD1306_I2C_BITBANG_SDA, GPIO_CFGLR_OUT_10Mhz_PP );
	I2CDELAY_FUNC( 1 * I2CSPEEDBASE );
	return !!i;
}

uint8_t ssd1306_pkt_send(const uint8_t *data, int sz, uint8_t cmd)
{
	ssd1306_i2c_sendstart();
	int r = ssd1306_i2c_sendbyte( SSD1306_I2C_ADDR<<1 );
	if( r ) return r;
	//ssd1306_i2c_sendstart(); For some reason displays don't want repeated start
	if(cmd)
	{
		if( ssd1306_i2c_sendbyte( 0x00 ) )
			return 1; // Control
	}
	else
	{
		if( ssd1306_i2c_sendbyte( 0x40 ) )
			return 1; // Data
	}
	for( int i = 0; i < sz; i++ )
	{
		if( ssd1306_i2c_sendbyte( data[i] ) )
			return 1;
	}
	ssd1306_i2c_sendstop();
	return 0;
}

void ssd1306_rst(void)
{
	funPinMode( SSD1306_RST_PIN, GPIO_CFGLR_OUT_10Mhz_PP );
	funDigitalWrite( SSD1306_RST_PIN, 0 );
	Delay_Ms(10);
	funDigitalWrite( SSD1306_RST_PIN, 1 );
	Delay_Us(10);
}

#endif

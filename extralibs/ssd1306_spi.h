/*
 * Single-File-Header for SSD1306 SPI interface
 * 05-05-2023 E. Brombaugh
 */

#ifndef _SSD1306_SPI_H
#define _SSD1306_SPI_H

// control pins
#ifndef SSD1306_RST_PIN
#define SSD1306_RST_PIN PC2
#endif

#ifndef SSD1306_CS_PIN
#define SSD1306_CS_PIN PC3
#endif

#ifndef SSD1306_DC_PIN
#define SSD1306_DC_PIN PC4
#endif

#ifndef SSD1306_MOSI_PIN
#define SSD1306_MOSI_PIN PC6
#endif

#ifndef SSD1306_SCK_PIN
#define SSD1306_SCK_PIN  PC5
#endif

#ifndef SSD1306_BAUD_RATE_PRESCALER
#define SSD1306_BAUD_RATE_PRESCALER SPI_BaudRatePrescaler_2
#endif

#ifndef SSD1306_SOFT_SPI
#define SSD1306_SOFT_SPI 0
#endif

/*
 * init SPI and GPIO for SSD1306 OLED
 */
uint8_t ssd1306_spi_init(void)
{
	// Enable GPIOC and SPI
#ifdef CH5xx
#else
	RCC->APB2PCENR |= RCC_APB2Periph_SPI1;
#endif
	
	funGpioInitAll();
	funPinMode( SSD1306_RST_PIN, GPIO_CFGLR_OUT_50Mhz_PP );
	funPinMode( SSD1306_CS_PIN, GPIO_CFGLR_OUT_50Mhz_PP );
	funPinMode( SSD1306_DC_PIN, GPIO_CFGLR_OUT_50Mhz_PP );
#if defined( CH5xx ) || SSD1306_SOFT_SPI
	funPinMode( SSD1306_MOSI_PIN, GPIO_CFGLR_OUT_50Mhz_PP );
	funPinMode( SSD1306_SCK_PIN, GPIO_CFGLR_OUT_50Mhz_PP );
#else
	funPinMode( SSD1306_MOSI_PIN, GPIO_CFGLR_OUT_50Mhz_AF_PP );
	funPinMode( SSD1306_SCK_PIN, GPIO_CFGLR_OUT_50Mhz_AF_PP );
#endif

	funDigitalWrite( SSD1306_RST_PIN, FUN_HIGH );
	funDigitalWrite( SSD1306_CS_PIN, FUN_HIGH );
	funDigitalWrite( SSD1306_DC_PIN, FUN_LOW );

	// Configure SPI 
#if SSD1306_SOFT_SPI
	funDigitalWrite( SSD1306_SCK_PIN, FUN_HIGH );
#elif defined( CH5xx )
	R8_SPI0_CLOCK_DIV = FUNCONF_SYSTEM_CORE_CLOCK / 12000000; // 16MHz is the fastest I want to go - though it does seem to work up to ~60MHz. 
	R8_SPI0_CTRL_MOD  = RB_SPI_ALL_CLEAR;
	R8_SPI0_CTRL_MOD  = RB_SPI_MOSI_OE | RB_SPI_2WIRE_MOD | RB_SPI_SCK_OE;
	R8_SPI0_CTRL_MOD |= RB_SPI_MST_SCK_MOD;
	// | RB_SPI_MST_SCK_MOD; // Mode 3 / mode 0
	R8_SPI0_CTRL_CFG = RB_MST_CLK_SEL;
#else
	SPI1->CTLR1 = 
		SPI_NSS_Soft | SPI_CPHA_1Edge | SPI_CPOL_Low | SPI_DataSize_8b |
		SPI_Mode_Master | SPI_Direction_1Line_Tx |
		SSD1306_BAUD_RATE_PRESCALER;

	// enable SPI port
	SPI1->CTLR1 |= CTLR1_SPE_Set;
#endif
	
	// always succeed
	return 0;
}

/*
 * toggle reset line
 */
void ssd1306_rst(void)
{
	funDigitalWrite( SSD1306_RST_PIN, FUN_LOW );
	Delay_Ms(10);
	funDigitalWrite( SSD1306_RST_PIN, FUN_HIGH );
}

/*
 * packet send for blocking polled operation via spi
 */
uint8_t ssd1306_pkt_send(const uint8_t *data, int sz, uint8_t cmd)
{
	if(cmd)
	{
		funDigitalWrite( SSD1306_DC_PIN, FUN_LOW );
	}
	else
	{
		funDigitalWrite( SSD1306_DC_PIN, FUN_HIGH );
	}

	funDigitalWrite( SSD1306_CS_PIN, FUN_LOW );
	
	// send data
	while(sz--)
	{
#if SSD1306_SOFT_SPI
		uint8_t c = *data++;
		int i = 8;
		do
		{
			funDigitalWrite( SSD1306_SCK_PIN, FUN_LOW );       ADD_N_NOPS(1)
			funDigitalWrite( SSD1306_MOSI_PIN, !!(c & 0x80) ); ADD_N_NOPS(1)
			funDigitalWrite( SSD1306_SCK_PIN, FUN_HIGH );      ADD_N_NOPS(1)
			c<<=1;
		} while( --i );

#elif defined( CH5xx )
		while(! (R8_SPI0_INT_FLAG & RB_SPI_FREE) );
		R8_SPI0_BUFFER = *data++;
#else
		// wait for TXE
		while(!(SPI1->STATR & SPI_STATR_TXE));
		
		// Send byte
		SPI1->DATAR = *data++;
#endif
	}
	
	// wait for not busy before exiting
#if SSD1306_SOFT_SPI
	// Nothing needed here.
#elif defined( CH5xx )
	while( !(R8_SPI0_INT_FLAG & RB_SPI_FREE)) { }
#else
	while(SPI1->STATR & SPI_STATR_BSY) { }
#endif

	funDigitalWrite( SSD1306_CS_PIN, FUN_HIGH );
	
	// we're happy
	return 0;
}

#endif

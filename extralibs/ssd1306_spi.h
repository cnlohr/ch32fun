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

/*
 * init SPI and GPIO for SSD1306 OLED
 */
uint8_t ssd1306_spi_init(void)
{
	// Enable GPIOC and SPI
	RCC->APB2PCENR |= RCC_APB2Periph_SPI1;
	
	funGpioInitAll();
	funPinMode( SSD1306_RST_PIN, GPIO_CFGLR_OUT_50Mhz_PP );
	funPinMode( SSD1306_CS_PIN, GPIO_CFGLR_OUT_50Mhz_PP );
	funPinMode( SSD1306_DC_PIN, GPIO_CFGLR_OUT_50Mhz_PP );
	funPinMode( SSD1306_MOSI_PIN, GPIO_CFGLR_OUT_50Mhz_AF_PP );
	funPinMode( SSD1306_SCK_PIN, GPIO_CFGLR_OUT_50Mhz_AF_PP );

	funDigitalWrite( SSD1306_RST_PIN, FUN_HIGH );
	funDigitalWrite( SSD1306_CS_PIN, FUN_HIGH );
	funDigitalWrite( SSD1306_DC_PIN, FUN_LOW );

	// Configure SPI 
	SPI1->CTLR1 = 
		SPI_NSS_Soft | SPI_CPHA_1Edge | SPI_CPOL_Low | SPI_DataSize_8b |
		SPI_Mode_Master | SPI_Direction_1Line_Tx |
		SSD1306_BAUD_RATE_PRESCALER;

	// enable SPI port
	SPI1->CTLR1 |= CTLR1_SPE_Set;
	
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
		// wait for TXE
		while(!(SPI1->STATR & SPI_STATR_TXE));
		
		// Send byte
		SPI1->DATAR = *data++;
	}
	
	// wait for not busy before exiting
	while(SPI1->STATR & SPI_STATR_BSY) { }
	
	funDigitalWrite( SSD1306_CS_PIN, FUN_HIGH );
	
	// we're happy
	return 0;
}

#endif

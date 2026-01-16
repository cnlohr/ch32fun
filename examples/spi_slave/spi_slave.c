#include "ch32fun.h"
#include <stdio.h>

#define CH32V003_SPI_SPEED_HZ 1000000
#define CH32V003_SPI_DIRECTION_2LINE_TXRX
#define CH32V003_SPI_CLK_MODE_POL0_PHA0
#define CH32V003_SPI_NSS_SOFTWARE_ANY_MANUAL

#include "../../extralibs/ch32v003_SPI.h"

#define PIN_CS PC1
// Useful to determine the cycle duration with a scope. Goes high the moment CS goes low, and back to low at the last
// action within the while loop. Can be removed otherwise.
#define PIN_OUT PC3

volatile uint8_t buffer_miso[4];
volatile uint8_t spi_miso_index = 0;
volatile uint8_t buffer_mosi[4];
volatile uint8_t spi_mosi_index = 0;

typedef enum
{
	STATE_IDLE = 0,
	STATE_RECEIVING = 1,
	STATE_RECEIVED = 2,
	STATE_SENDING = 3,
	STATE_SENT = 4,
} SpiState;

volatile SpiState spi_state = STATE_IDLE;

void wait_for_state( SpiState desired_state )
{
	while ( spi_state != desired_state )
	{
	}
}

void SPI_Configure()
{
	// reset control register
	SPI1->CTLR1 = 0;

	// Enable GPIO Port C and SPI peripheral
	RCC->APB2PCENR |= RCC_APB2Periph_GPIOC | RCC_APB2Periph_SPI1;

	// PC5 is SCLK
	funPinMode( PC5, GPIO_CFGLR_IN_FLOAT );

	// PC6 is MOSI
	funPinMode( PC6, GPIO_CFGLR_IN_FLOAT );

	// PC7 is MISO
	funPinMode( PC7, GPIO_Speed_2MHz | GPIO_CNF_OUT_PP_AF );

	// Configure SPI
	SPI1->CTLR1 |= SPI_CPHA_1Edge | SPI_CPOL_Low | SPI_Mode_Slave | SPI_BaudRatePrescaler_2 | SPI_DataSize_8b;
	SPI1->CTLR1 |= SPI_Direction_2Lines_FullDuplex;
	SPI1->CTLR1 |= CTLR1_SPE_Set;
}

void SPI1_IRQHandler( void ) __attribute__( ( interrupt ) );
void SPI1_IRQHandler( void )
{
	if ( !( SPI1->STATR & SPI_STATR_RXNE ) )
	{
		return;
	}

	// Read received data first to clear RXNE. This is important: not reading the data register can leave RXNE set and
	// prevent further proper IRQ handling.
	uint8_t received = SPI_read_8();
	if ( spi_state == STATE_SENDING )
	{
		// When sending, the master clocks data in, and we should provide the next
		// MISO byte for the next clock. Write after reading the incoming byte.
		if ( spi_miso_index < sizeof( buffer_miso ) )
		{
			SPI_write_8( buffer_miso[spi_miso_index++] );
			if ( spi_miso_index >= sizeof( buffer_miso ) )
			{
				spi_state = STATE_SENT;
			}
		}
		else
		{
			spi_state = STATE_SENT;
		}
	}
	else if ( spi_state == STATE_RECEIVING )
	{
		// Store byte when in receiving state
		buffer_mosi[spi_mosi_index++] = received;
		if ( spi_mosi_index > sizeof( buffer_mosi ) )
		{
			spi_mosi_index = 0;
		}
		if ( spi_mosi_index == 4 )
		{
			spi_state = STATE_RECEIVED;
		}
	}
}

void send()
{
	spi_miso_index = 0;
	// Transition to sending: briefly disable RXNE interrupt to avoid the ISR
	// writing 0x00 while we flip state and preload the response bytes.
	SPI1->CTLR2 &= ~SPI_CTLR2_RXNEIE;
	spi_state = STATE_SENDING;
	// Preload the first MISO byte so the master reads the intended first byte.
	if ( spi_miso_index < sizeof( buffer_miso ) )
	{
		SPI_write_8( buffer_miso[spi_miso_index++] );
	}

	SPI1->CTLR2 |= SPI_CTLR2_RXNEIE;
	wait_for_state( STATE_SENT );
}


int main()
{
	SystemInit();
	printf( "--- init ---\n" );

	funGpioInitAll();

	funPinMode( PIN_CS, GPIO_CFGLR_IN_PUPD );
	funPinMode( PIN_OUT, GPIO_Speed_10MHz | GPIO_CNF_OUT_PP );

	SPI_Configure();
	NVIC_EnableIRQ( SPI1_IRQn );
	while ( 1 )
	{
		if ( funDigitalRead( PIN_CS ) )
		{
			continue;
		}

		funDigitalWrite( PIN_OUT, FUN_HIGH );
		SPI_write_8( 0x00 );
		// Enable the RXNE interrupt only when CS is low, and we are ready to receive; to avoid unnecessary IRQs.
		// Not exactly necessary, but if the CPU needs to do other things while CS is high, this ensures it will not be,
		// well, interrupted.
		SPI1->CTLR2 |= SPI_CTLR2_RXNEIE;
		spi_mosi_index = 0;
		spi_state = STATE_RECEIVING;
		wait_for_state( STATE_RECEIVED );
		buffer_miso[0] = buffer_mosi[0];
		buffer_miso[1] = buffer_mosi[1];
		buffer_miso[2] = 0xBE;
		buffer_miso[3] = 0xEF;
		send();
		while ( !funDigitalRead( PIN_CS ) )
		{
		}

		spi_state = STATE_IDLE;
		printf( "RX/MOSI(%d):", 4 );
		for ( uint8_t i = 0; i < 4; i++ )
		{
			printf( " %02X", buffer_mosi[i] );
		}

		printf( "  |  TX/MISO(%d):", 4 );
		for ( uint8_t i = 0; i < 4; i++ )
		{
			printf( " %02X", buffer_miso[i] );
		}
		printf( "\n" );
		SPI1->CTLR2 &= ~SPI_CTLR2_RXNEIE;
		funDigitalWrite( PIN_OUT, FUN_LOW );
	}
}

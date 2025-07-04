#include "ch32fun.h"
#include <stdint.h>
#include <stdio.h>

//                     TS1           TS2            BRP
#define BAUD_25kbps ( ( 5 << 16 ) | ( 4 << 20 ) | ( 239 ) )
#define BAUD_50kbps ( ( 5 << 16 ) | ( 4 << 20 ) | ( 119 ) )
#define BAUD_100kbps ( ( 5 << 16 ) | ( 4 << 20 ) | ( 59 ) )
#define BAUD_125kbps ( ( 5 << 16 ) | ( 4 << 20 ) | ( 47 ) )
#define BAUD_250kbps ( ( 5 << 16 ) | ( 4 << 20 ) | ( 23 ) )
#define BAUD_500kbps ( ( 5 << 16 ) | ( 4 << 20 ) | ( 11 ) )
#define BAUD_750kbps ( ( 5 << 16 ) | ( 4 << 20 ) | ( 7 ) )
#define BAUD_1Mbps ( ( 5 << 16 ) | ( 4 << 20 ) | ( 5 ) )

#define STATUS_OK ( CAN_TSTATR_RQCP0 | CAN_TSTATR_TXOK0 )

// USER CONFIGURATION
#define USE_EXTENDED_ID 1
#define TRANSMIT 0 // Set to 1 to transmit messages, 0 to receive messages
#define FIFO 0 // Use FIFO 0/1
#define BAUD BAUD_25kbps

#if USE_EXTENDED_ID
// 29 bit id
#define ID 0xBADC0DE
#else
// 11 bit id
#define ID 0x123
#endif

static inline uint32_t GetAPB1Div( void );
static size_t Receive( uint8_t *dst, uint32_t *id, uint8_t fifo );
static int Transmit( const uint8_t *src, size_t size );
static int MessageSent( int mailbox );

int main()
{
	SystemInit();

	Delay_Ms(1000);
	printf( "STARTUP 1000" );
		Delay_Ms(2000);
	printf( "STARTUP 2000" );
		Delay_Ms(3000);
	printf( "STARTUP 3000" );
		Delay_Ms(4000);
	printf( "STARTUP 4000" );
		Delay_Ms(5000);
	printf( "STARTUP 5000" );
		Delay_Ms(6000);
	printf( "STARTUP 6000" );

	// Enable Peripherals
	RCC->APB2PCENR |= RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOD;
	RCC->APB1PCENR |= RCC_APB1Periph_CAN1;

	// Configure AF remapping for CAN
	AFIO->PCFR1 &= ~AFIO_PCFR1_CAN_REMAP; // Clear remap bits
	AFIO->PCFR1 |= AFIO_PCFR1_CAN_REMAP_REMAP2; // Set PD0 and PD1 for CAN RX and TX

	// Configure GPIO
	funPinMode( PB8, GPIO_CFGLR_IN_FLOAT );
	funPinMode( PB9, GPIO_CFGLR_OUT_50Mhz_AF_PP );

	// Wake up
	CAN1->CTLR &= ( ~(uint32_t)CAN_CTLR_SLEEP );
	// Initialise
	CAN1->CTLR |= CAN_CTLR_INRQ | CAN_CTLR_NART;

	printf( "Entering CAN Init mode..." );
	// Wait for intialisation to complete
	while ( !( CAN1->STATR & CAN_STATR_INAK ) )
		;
	printf( "Done\n" );

	printf( "System Core Clock: %uMHz\n", FUNCONF_SYSTEM_CORE_CLOCK / 1000000 );

	CAN1->BTIMR = BAUD;

	const uint32_t ts1 = ( CAN1->BTIMR & CAN_BTIMR_TS1 ) >> 16;
	const uint32_t ts2 = ( CAN1->BTIMR & CAN_BTIMR_TS2 ) >> 20;
	const uint32_t brp = CAN1->BTIMR & CAN_BTIMR_BRP;
	const uint32_t baud = ( FUNCONF_SYSTEM_CORE_CLOCK / GetAPB1Div() ) / ( ( ts1 + ts2 + 3 ) * ( brp + 1 ) );
	printf( "CAN Baud Rate: %lubps\n", baud );

#if USE_EXTENDED_ID
	// Set the CAN filter for extended ID
	CAN1->sTxMailBox[FIFO].TXMIR = ( ( ID << 3 ) & CAN_TXMI0R_EXID ) | CAN_TXMI0R_IDE;
#else
	// Set the CAN filter for standard ID
	CAN1->sTxMailBox[FIFO].TXMIR = ( ID << 21 ) & CAN_TXMI0R_STID;
#endif

	// Set up rx filter
	CAN1->FCTLR |= FCTLR_FINIT; // Enter initialisation mode
	{
		static const size_t filter_id = 0; // Filter 0

		// Set ID to match
		CAN1->sFilterRegister[filter_id].FR1 = 0x0;
		// Set which bits of the ID to match (mask)
		CAN1->sFilterRegister[filter_id].FR2 = 0; // Accept all messages

		CAN1->FAFIFOR = ( FIFO << filter_id ); // assign filter to FIFO
		CAN1->FMCFGR = ( 0 << filter_id ); // 1: id mode, 0: mask mode
		CAN1->FSCFGR = ( 1 << filter_id ); // 1: 32 bit filter, 0: 16 bit filter
		CAN1->FWR = ( 1 << filter_id ); // enable filter
	}
	CAN1->FCTLR &= ~FCTLR_FINIT; // Exit initialisation mode

	CAN1->CTLR &= ~(uint32_t)CAN_CTLR_INRQ;
	printf( "Exiting CAN Init mode..." );
	// Wait for intialisation to complete
	while ( CAN1->STATR & CAN_STATR_INAK )
		;
	printf( "Done\n" );

	uint8_t data[8] = { 0xFE, 0x7A, 0xBE, 0xE5, 0xC0, 0xFF, 0xEE, 0x45 };

#if TRANSMIT
	while ( 1 )
	{
		printf( "Sending message with ID 0x%08X: ", ID );
		for ( size_t i = 0; i < 8; i++ )
		{
			printf( "%02X ", data[i] );
		}
		printf( "\n" );

		const int mailbox = Transmit( data, 8 );
		if ( mailbox != -1 )
		{
			while ( !MessageSent( mailbox ) )
				; // Wait for message to be sent
			printf( "Message sent successfully on mailbox %d\n", mailbox );
		}
		else
		{
			printf( "Failed to send message, no available mailboxes.\n" );
		}

		Delay_Ms( 1000 );
	}
#else

	while ( 1 )
	{
		const uint32_t messages = CAN1->RFIFO0 & CAN_RFIFO0_FMP0;
		if ( messages )
		{
			uint32_t id = 0;
			const size_t numbytes = Receive( data, &id, FIFO );
			printf( "FIFO: %d/3: %d bytes from 0x%08X: ", (int)messages, (int)numbytes, (int)id );
			for ( size_t i = 0; i < numbytes; i++ )
			{
				printf( "%02X ", data[i] );
			}
			printf( "\n" );
		}
		Delay_Ms( 50 );
	}
#endif
}

static inline uint32_t GetAPB1Div( void )
{
	// Get APB1 clock divider
	if ( RCC->CFGR0 & RCC_PPRE1_DIV2 )
		return 2;
	else if ( RCC->CFGR0 & RCC_PPRE1_DIV4 )
		return 4;
	else if ( RCC->CFGR0 & RCC_PPRE1_DIV8 )
		return 8;
	else if ( RCC->CFGR0 & RCC_PPRE1_DIV16 )
		return 16;
	else
		return 1; // No division
}

static size_t Receive( uint8_t *dst, uint32_t *id, uint8_t fifo )
{
	// Get ID
	if ( CAN_RXMI0R_IDE & CAN1->sFIFOMailBox[fifo].RXMIR )
		*id = ( CAN_RXMI0R_EXID & (uint32_t)CAN1->sFIFOMailBox[fifo].RXMIR ) >> 3;
	else
		*id = ( CAN_RXMI0R_STID & (uint32_t)CAN1->sFIFOMailBox[fifo].RXMIR ) >> 21;

	size_t size = CAN_RXMDT0R_DLC & CAN1->sFIFOMailBox[fifo].RXMDTR;

	for ( size_t i = 0; i < size; i++ )
		if ( i < 4 )
			dst[i] = CAN1->sFIFOMailBox[fifo].RXMDLR >> ( i * 8 );
		else
			dst[i] = CAN1->sFIFOMailBox[fifo].RXMDHR >> ( ( i - 4 ) * 8 );

	// Release the FIFO
	if ( fifo == 0 )
		CAN1->RFIFO0 |= CAN_RFIFO0_RFOM0;
	else
		CAN1->RFIFO1 |= CAN_RFIFO1_RFOM1;

	return size;
}

static int Transmit( const uint8_t *src, size_t size )
{
	int mailbox = -1;

	if ( CAN1->TSTATR & CAN_TSTATR_TME0 )
		mailbox = 0;
	else if ( CAN1->TSTATR & CAN_TSTATR_TME1 )
		mailbox = 1;
	else if ( CAN1->TSTATR & CAN_TSTATR_TME2 )
		mailbox = 2;

	if ( -1 != mailbox )
	{
		// Set data length
		CAN1->sTxMailBox[mailbox].TXMDTR = size & 0x0F;

		// Clear Data
		CAN1->sTxMailBox[mailbox].TXMDLR = 0;
		CAN1->sTxMailBox[mailbox].TXMDHR = 0;

		for ( size_t i = 0; i < size; i++ )
			if ( i < 4 )
				CAN1->sTxMailBox[mailbox].TXMDLR |= ( (uint32_t)src[i] << ( i * 8 ) );
			else
				CAN1->sTxMailBox[mailbox].TXMDHR |= ( (uint32_t)src[i] << ( ( i - 4 ) * 8 ) );

		CAN1->sTxMailBox[mailbox].TXMIR |= CAN_TXMI0R_TXRQ;
	}
	return mailbox;
}

static int MessageSent( int mailbox )
{
	if ( mailbox < 0 || mailbox > 2 ) return 0;

	const uint32_t status = ( CAN1->TSTATR >> ( mailbox * 8 ) ) & 0xFF;

	const int sent = ( status & STATUS_OK ) == STATUS_OK;
	const int mailbox_empty = ( CAN1->TSTATR >> ( 26 + mailbox ) ) & 1;

	return sent && mailbox_empty;
}


#include "usbd.h"
#include "ch32fun.h"
#include "usb_config.h"

#include <stdio.h>

// clang-format off
#define ANYPRINTF	(defined( FUNCONF_USE_DEBUGPRINTF ) && FUNCONF_USE_DEBUGPRINTF) || \
					(defined( FUNCONF_USE_UARTPRINTF ) && FUNCONF_USE_UARTPRINTF) || \
					(defined( FUNCONF_USE_USBPRINTF ) && FUNCONF_USE_USBPRINTF)

volatile uint8_t usb_debug = 0;
// Uncomment to enable debugging
// #define DEBUG
#define USBD_DEBUGPRINTF( ... ) {if ( usb_debug ) { printf( __VA_ARGS__ ); }}
#define min(a, b) (((a)<(b))?(a):(b))
// clang-format on

struct _USBState USBDCTX;

void USBD_InternalFinishSetup();

static inline void SetEPR_Status( const int ep, const uint16_t mask, const uint16_t value )
{
	const uint16_t reg = USBD->EPR[ep];
	const uint16_t current_stat = reg & mask;
	// Which bits we need to toggle
	const uint16_t toggle = current_stat ^ value;

	// Conserve EA, TYPE and KIND (Non-toggle)
	uint16_t write_val = ( reg & ( USBD_EPR_EA | USBD_EPR_EP_TYPE_MASK | USBD_EPR_EP_KIND ) );
	write_val |= toggle;
	// Write 1 has no effect for CTR_RX and CTR_TX (0 clears)
	write_val |= ( USBD_EPR_CTR_RX | USBD_EPR_CTR_TX );

	USBD->EPR[ep] = write_val;
}

int USBDSetup( void )
{
	// Initialize USB pins to pull-down to avoid mis-detection
	GPIOA->CFGHR &= ~( ( 0xf << ( 4 * 3 ) ) | ( 0xf << ( 4 * 4 ) ) );
	GPIOA->CFGHR |= ( GPIO_Speed_2MHz | GPIO_CNF_OUT_PP ) << ( 4 * 3 ) | ( GPIO_Speed_2MHz | GPIO_CNF_OUT_PP )
	                                                                         << ( 4 * 4 );
	GPIOA->BSHR = ( 1 << ( 16 + 11 ) ) | ( 1 << ( 16 + 12 ) );

	// We need 48MHz clock for USB
#if FUNCONF_SYSTEM_CORE_CLOCK == 240000000
	RCC->CFGR0 = ( RCC->CFGR0 & ~RCC_USBPRE ) | RCC_USBPRE_DIV5;
#elif FUNCONF_SYSTEM_CORE_CLOCK == 144000000
	RCC->CFGR0 = ( RCC->CFGR0 & ~RCC_USBPRE ) | RCC_USBPRE_DIV3;
#elif FUNCONF_SYSTEM_CORE_CLOCK == 96000000
	RCC->CFGR0 = ( RCC->CFGR0 & ~RCC_USBPRE ) | RCC_USBPRE_DIV2;
#elif FUNCONF_SYSTEM_CORE_CLOCK == 48000000
	RCC->CFGR0 = ( RCC->CFGR0 & ~RCC_USBPRE ) | RCC_USBPRE_DIV1;
#else
#error CH32V20x/30x need 240/144/96/48MHz main clock for USB to work
#endif

	RCC->APB2PCENR |= RCC_AFIOEN | RCC_IOPAEN;
	RCC->AHBPCENR = RCC_AHBPeriph_SRAM;
	RCC->APB1PCENR |= RCC_USBEN;

#if defined( CH32V203F8 ) || defined( CH32V203F8U6 )
	// Sometimes USB shares pins w/ SWD
	// SWD must be disabled in such cases
	Delay_Ms( 100 );
	AFIO->PCFR1 |= AFIO_PCFR1_SWJ_CFG_DISABLE;
#endif

	// Suspend & disable all interrupts
	USBD->CNTR = USBD_FRES;
	USBD->CNTR = 0;

	// Delay a tad (Slightly more optimized)
	for ( volatile int i = 0; i < 1000; i++ );

	// Initialize required registers & packet buffer description table
	USBD_InternalFinishSetup();

	EXTEN->EXTEN_CTR |= EXTEN_USBD_PU_EN;
	USBD->CNTR = USBD_CTRM | USBD_RESETM | USBD_SUSPM | USBD_WKUPM;
	USBD->ISTR = 0;
	NVIC_EnableIRQ( USB_LP_CAN1_RX0_IRQn );

	return 0;
}

WEAK void USBD_InternalFinishSetup( void )
{
	USBD->BTABLE = 0;

	// EP0 has to be RTX for control transfers
	USBDCTX.endpoints[0].mode = USBD_EP_MODE_RX | USBD_EP_MODE_TX;

#if FUSB_EP1_MODE
	USBDCTX.endpoints[1].mode = FUSB_EP1_MODE;
#endif

#if FUSB_EP2_MODE
	USBDCTX.endpoints[2].mode = FUSB_EP2_MODE;
#endif

#if FUSB_EP3_MODE
	USBDCTX.endpoints[3].mode = FUSB_EP3_MODE;
#endif

#if FUSB_EP4_MODE
	USBDCTX.endpoints[4].mode = FUSB_EP4_MODE;
#endif

#if FUSB_EP5_MODE
	USBDCTX.endpoints[5].mode = FUSB_EP5_MODE;
#endif

#if FUSB_EP6_MODE
	USBDCTX.endpoints[6].mode = FUSB_EP6_MODE;
#endif

#if FUSB_EP7_MODE
	USBDCTX.endpoints[7].mode = FUSB_EP7_MODE;
#endif

	// 64 bytes for each buffer
	uint16_t pma = USBD_PMA_BASE; // running allocation cursor
	for ( uint8_t ep = 0; ep < FUSB_MAX_EP_CNT; ++ep )
	{
		USBDCTX.usbd_ep_data_ptr[ep] = (uint32_t *)( CAN_USBD_SHARED_BASE + pma * 2 );

		if ( USBDCTX.endpoints[ep].mode & USBD_EP_MODE_TX )
		{
			USBD_BDT->EP[ep].ADDn_TX = pma;
			// Configured on the fly
			USBD_BDT->EP[ep].COUNTn_TX = 0;

			// If we have an RX node, we double-buffer
			if ( !( USBDCTX.endpoints[ep].mode & USBD_EP_MODE_RX ) )
			{
				pma += USBD_PACKET_SIZE;
			}
		}

		if ( USBDCTX.endpoints[ep].mode & USBD_EP_MODE_RX )
		{
			USBD_BDT->EP[ep].ADDn_RX = pma;
			// USBD_BLSIZE: Each cnt is 32 bytes
			USBD_BDT->EP[ep].COUNTn_RX = USBD_BLSIZE | ( ( USBD_PACKET_SIZE / 32 - 1 ) << 10 );
			pma += USBD_PACKET_SIZE;
		}
	}
}

void USBDReset( void )
{
	NVIC_DisableIRQ( USB_LP_CAN1_RX0_IRQn );
	USBD->CNTR = USBD_FRES;
	for ( volatile int i = 0; i < 1000; i++ );
	USBD->CNTR = USBD_CTRM | USBD_RESETM | USBD_SUSPM | USBD_WKUPM;
	USBD->ISTR = 0;
	// Delay 100us for the USB to fully reset
	// Not recognized otherwise
	Delay_Us( 100 );
	NVIC_EnableIRQ( USB_LP_CAN1_RX0_IRQn );
}

// It seems that even WCH doesn't use the high-priority usb interrupt
// maybe it's only for CAN?
#if FUSB_USE_HPE
// There is an issue with some registers apparently getting lost with HPE, just do it the slow way.
void USB_LP_CAN1_RX0_IRQHandler( void ) __attribute__( ( section( ".text.vector_handler" ) ) )
__attribute( ( interrupt ) );
#else
#if defined( FUSB_FROM_RAM ) && ( FUSB_FROM_RAM )
void USB_LP_CAN1_RX0_IRQHandler( void ) __USBFS_FUN_ATTRIBUTE __attribute( ( interrupt ) );
#else
void USB_LP_CAN1_RX0_IRQHandler( void ) __attribute__( ( section( ".text.vector_handler" ) ) )
__attribute( ( interrupt ) );
#endif
#endif

void USB_LP_CAN1_RX0_IRQHandler( void )
{
	static uint8_t zeros[] = { 0, 0 }; // Macro to reply 0s
	const uint32_t istr = USBD->ISTR;
	int len = 0;

	// Correct transfer
	if ( istr & USBD_CTR )
	{
		const int ep = istr & USBD_EP_ID;
		const int epr = USBD->EPR[ep];

#ifdef USB_DEBUG
		if ( epr & USBD_CTR_RX && epr & USBD_CTR_TX )
		{
			printf( "Both RX & TX flags are set! Processing is too slow\n" );
		}
#endif

		if ( epr & USBD_CTR_RX )
		{
			// EP0 Setup Packet
			if ( ep == 0 && epr & USBD_SETUP )
			{
				// Memory in USBD is stored in ranks of 16 bits (even though they can store
				// 32 bits)
				// What a weird design
				const uint8_t USBD_request_type = USBDCTX.USBD_SetupReqType = USBDCTX.usbd_ep_data_ptr[0][0] & 0xFF;
				const uint8_t USBD_request = USBDCTX.USBD_SetupReqCode = USBDCTX.usbd_ep_data_ptr[0][0] >> 8;
				const uint32_t USBD_index = ( (uint32_t)USBDCTX.usbd_ep_data_ptr[0][2] );
				const uint32_t USBD_indexValue = USBDCTX.USBD_IndexValue =
					( ( (uint32_t)( USBDCTX.usbd_ep_data_ptr[0][2] ) ) << 16 ) | USBDCTX.usbd_ep_data_ptr[0][1];
				const uint16_t USBD_length = USBDCTX.USBD_SetupReqLen = USBDCTX.usbd_ep_data_ptr[0][3];

				// Copy to CTRL0BUFF
				// Can be removed if CTRL0BUFF unused
				for ( int i = 0; i < min( USBD_length, 64 ); ++i )
				{
					CTRL0BUFF[i] = ( USBDCTX.usbd_ep_data_ptr[0][i / 2] >> ( ( i & 1 ) << 3 ) ) & 0xFF;
				}

				// Request 0b00100001 0x0A = SET IDLE, can be ignored
				if ( ( USBD_request_type & USBD_REQ_TYP_MASK ) == USBD_REQ_TYP_STANDARD )
				{
					// Standard setup
					switch ( USBD_request )
					{
						case USB_GET_STATUS:
							// Non remote wakeup and non self powered -> 0
							USBDCTX.pCtrlPayloadPtr = zeros;
							USBDCTX.USBD_SetupReqLen = 2;
							break;
						case USB_SET_ADDRESS:
							USBDCTX.USBD_DevAddr = USBD_indexValue & 0xFF;
							USBDCTX.USBD_SetupReqLen = USBD_SEND_ACK;
							break;
						case USB_GET_DESCRIPTOR:
							const struct descriptor_list_struct *e = descriptor_list;
							const struct descriptor_list_struct *e_end = e + DESCRIPTOR_LIST_ENTRIES;
							for ( ; e != e_end; e++ )
							{
								if ( e->lIndexValue == (uint32_t)USBD_indexValue )
								{
									USBDCTX.pCtrlPayloadPtr = (uint8_t *)e->addr;
									USBDCTX.USBD_SetupReqLen = min( e->length, USBD_length );
									break;
								}
							}
							if ( e == e_end ) goto stall;
							break;
						case USB_GET_CONFIG:
							USBDCTX.pCtrlPayloadPtr = &USBDCTX.USBD_DevConfig;
							USBDCTX.USBD_SetupReqLen = 1;
							break;
						case USB_SET_CONFIG:
							USBDCTX.USBD_DevConfig = USBD_indexValue & 0xFF;
							USBDCTX.USBD_DevEnumStatus = 0x01;
							USBDCTX.USBD_SetupReqLen = USBD_SEND_ACK;

							// Reset DTOG bits
							for ( int ep = 1; ep < FUSB_MAX_EP_CNT; ep++ )
							{
								if ( USBD->EPR[ep] & USBD_DTOG_RX )
								{
									USBD->EPR[ep] = ( USBD->EPR[ep] & ( USBD_EA | USBD_EPKIND | USBD_EPTYPE ) ) |
									                USBD_CTR_TX | USBD_CTR_RX | USBD_DTOG_RX;
								}
								if ( USBD->EPR[ep] & USBD_DTOG_TX )
								{
									USBD->EPR[ep] = ( USBD->EPR[ep] & ( USBD_EA | USBD_EPKIND | USBD_EPTYPE ) ) |
									                USBD_CTR_TX | USBD_CTR_RX | USBD_DTOG_TX;
								}
							}
							break;
						case USB_GET_INTERFACE:
							USBDCTX.pCtrlPayloadPtr = zeros;
							USBDCTX.USBD_SetupReqLen = 1;
							break;
						case USB_CLEAR_FEATURE:
#if FUSB_SUPPORTS_SLEEP
							if ( ( USBD_request_type & USB_REQ_RECIP_MASK ) == USB_REQ_RECIP_DEVICE )
							{
								/* clear one device feature */
								if ( (uint8_t)( USBD_indexValue & 0xFF ) == USB_REQ_FEAT_REMOTE_WAKEUP )
								{
									/* clear usb sleep status, device not prepare to sleep */
									USBDCTX.USBD_DevSleepStatus &= ~0x01;
								}
								else
								{
									goto stall;
								}
							}
							else
#endif
								if ( ( USBD_request_type & USB_REQ_RECIP_MASK ) == USB_REQ_RECIP_ENDP )
							{
								if ( (uint8_t)( USBD_indexValue & 0xFF ) == USB_REQ_FEAT_ENDP_HALT )
								{
									/* Clear End-point Feature */
									if ( USBDCTX.endpoints[ep].mode )
									{
										if ( USBD_index & DEF_UEP_IN &&
											 ( USBDCTX.endpoints[ep].mode & USBD_EP_MODE_TX ) )
											SetEPR_Status( ep, USBD_EPR_STAT_TX_MASK, USBD_EPR_STAT_TX_NAK );
										else if ( USBD_index & DEF_UEP_OUT &&
												  ( USBDCTX.endpoints[ep].mode & USBD_EP_MODE_RX ) )
											SetEPR_Status( ep, USBD_EPR_STAT_RX_MASK, USBD_EPR_STAT_RX_ACK );
										else
											goto stall;
									}
									else
									{
										goto stall;
									}
								}
								else
								{
									goto stall;
								}
							}
							else
							{
								goto stall;
							}
							break;
						case USB_SET_FEATURE:
							if ( ( USBD_request_type & USB_REQ_RECIP_MASK ) == USB_REQ_RECIP_DEVICE )
							{
#if FUSB_SUPPORTS_SLEEP
								/* Set Device Feature */
								if ( (uint8_t)( USBD_indexValue & 0xFF ) == USB_REQ_FEAT_REMOTE_WAKEUP )
								{
									/* Set Wake-up flag, device prepare to sleep */
									USBD_DevSleepStatus |= 0x01;
								}
								else
#endif
								{
									goto stall;
								}
							}
							else if ( ( USBD_request_type & USB_REQ_RECIP_MASK ) == USB_REQ_RECIP_ENDP )
							{
								/* Set Endpoint Feature */
								if ( (uint8_t)( USBD_indexValue & 0xFF ) == USB_REQ_FEAT_ENDP_HALT )
								{
									if ( USBDCTX.endpoints[ep].mode )
									{
										if ( ( USBD_index & DEF_UEP_IN ) &&
											 ( USBDCTX.endpoints[ep].mode & USBD_EP_MODE_TX ) )
											SetEPR_Status( ep, USBD_EPR_STAT_TX_MASK, USBD_EPR_STAT_TX_STALL );
										else if ( ( USBD_index & DEF_UEP_OUT ) &&
												  ( USBDCTX.endpoints[ep].mode & USBD_EP_MODE_RX ) )
											SetEPR_Status( ep, USBD_EPR_STAT_RX_MASK, USBD_EPR_STAT_RX_STALL );
										else
											goto stall;
									}
								}
								else
									goto stall;
							}
							else
								goto stall;
							break;

						default: goto stall;
					}
				}
				else
				{
					// EP0 Setup non-standard
#if FUSB_HID_INTERFACES > 0 || FUSB_USER_HANDLERS
#if FUSB_HID_USER_REPORTS
					const uint16_t packet_len = USBD_BDT->EP[ep].COUNTn_RX & 0x3FF;
					uint8_t buff[USBD_PACKET_SIZE];

					for ( int i = 0; i < packet_len; ++i )
					{
						buff[i] = ( USBDCTX.usbd_ep_data_ptr[ep][i / 2] >> ( ( i & 1 ) << 3 ) ) & 0xFF;
					}
#endif

					len = 0;
					switch ( USBD_request )
					{
#if FUSB_HID_INTERFACES > 0
						case HID_SET_REPORT:
#if FUSB_HID_USER_REPORTS
							len = HandleHidUserSetReportSetup( &USBDCTX, (tusb_control_request_t *)buff );
							if ( len < 0 ) goto stall;
							USBDCTX.USBD_SetupReqLen = 0;
							break;

						case HID_GET_REPORT:
							len = HandleHidUserGetReportSetup( &USBDCTX, (tusb_control_request_t *)buff );
							if ( len < 0 ) goto stall;
							if ( len == 0 )
							{
								USBDCTX.USBD_SetupReqLen = USBD_SEND_ACK;
								break;
							}

							// len > 0:
							// We would like to send something it seems
							USBDCTX.USBD_SetupReqLen = len;
							len = min( USBD_PACKET_SIZE, len );

							// If HandleHidUserGetReportSetup hasn't provided data to send,
							// we ask HandleHidUserReportDataIn for data
							if ( !USBDCTX.pCtrlPayloadPtr )
							{
								len = HandleHidUserReportDataIn( &USBDCTX, buff, len );
								USBDCTX.pCtrlPayloadPtr = buff;
							}

#endif
							break;
						case HID_SET_IDLE:
							if ( USBD_index < FUSB_HID_INTERFACES )
								USBDCTX.USBD_HidIdle[USBD_index] = (uint8_t)( USBD_indexValue >> 8 );

							// ACK
							USBDCTX.USBD_SetupReqLen = USBD_SEND_ACK;
							break;
						case HID_SET_PROTOCOL:
							if ( USBD_index < FUSB_HID_INTERFACES )
								USBDCTX.USBD_HidProtocol[USBD_index] = (uint8_t)USBD_indexValue;
							USBDCTX.USBD_SetupReqLen = USBD_SEND_ACK;
							break;
						case HID_GET_IDLE:
							if ( USBD_index < FUSB_HID_INTERFACES )
							{
								USBDCTX.pCtrlPayloadPtr = &USBDCTX.USBD_HidIdle[USBD_index];
								USBDCTX.USBD_SetupReqLen = 1;
							}
							break;

						case HID_GET_PROTOCOL:
							if ( USBD_index < FUSB_HID_INTERFACES )
							{
								USBDCTX.pCtrlPayloadPtr = &USBDCTX.USBD_HidProtocol[USBD_index];
								USBDCTX.USBD_SetupReqLen = 1;
							}
							break;
#endif
						default:
#if FUSB_USER_HANDLERS
							len = HandleSetupCustom( &USBDCTX, USBD_request );

							if ( len )
							{
								if ( len < 0 )
								{
									// ACK
									USBDCTX.USBD_SetupReqLen = USBD_SEND_ACK;
								}
								else
								{
									USBDCTX.USBD_SetupReqLen = min( len, USBD_length );
								}
							}
							else
#endif
							{
								goto stall;
							}
							break;
					}
#endif
				}
			}
			else
			{
				// OUT RX packet
				const uint16_t packet_len = USBD_BDT->EP[ep].COUNTn_RX & 0x3FF;

#if FUSB_HID_USER_REPORTS || FUSB_USER_HANDLERS
				uint8_t buff[USBD_PACKET_SIZE];

				for ( int i = 0; i < packet_len; ++i )
				{
					buff[i] = ( USBDCTX.usbd_ep_data_ptr[ep][i / 2] >> ( ( i & 1 ) << 3 ) ) & 0xFF;
				}
#endif
#if FUSB_HID_USER_REPORTS
				if ( USBDCTX.USBD_SetupReqCode == HID_SET_REPORT )
				{
					HandleHidUserReportDataOut( &USBDCTX, buff, len );
				}
#endif
#if FUSB_USER_HANDLERS
				if ( USBDCTX.USBD_SetupReqCode != HID_SET_REPORT )
				{
					HandleDataOut( &USBDCTX, ep, buff, packet_len );
				}
#endif
#if FUSB_HID_USER_REPORTS
				// Transaction finished
				if ( USBDCTX.USBD_SetupReqLen == 0 )
				{
					if ( USBDCTX.USBD_SetupReqCode == HID_SET_REPORT ) HandleHidUserReportOutComplete( &USBDCTX );
				}
#endif

				if ( ep == 0 && ( USBDCTX.USBD_SetupReqLen == 0 || packet_len != 0 ) )
				{
					USBDCTX.USBD_SetupReqLen = USBD_SEND_ACK;
				}

				SetEPR_Status( ep, USBD_EPR_STAT_RX_MASK, USBD_EPR_STAT_RX_ACK );
			}

			// Clear RX (Toggle, 1 conserves bit)
			USBD->EPR[ep] = ( USBD->EPR[ep] & ( USBD_EA | USBD_EPKIND | USBD_EPTYPE ) ) | USBD_CTR_TX;
		}

		// length < sizeof -> Return start
		// length = sizeof -> Return all
		// length > sizeof -> Return partial then 0

		// 1st load for RX and auto-reload for TX
		// We need to send something:
		// - Set tx_buf and tx_pending to non-null and set TX to ACK
		// - This piece of code shall handle loading to buffer
		// - TX part shall start next transaction if we need to
		// TX detects if the operation should be continued by checking
		// if the tx_buf is null

		// We are always handling either a TX or a RX transaction
		// This is only for the control endpoint
		if ( ep == 0 )
		{
			if ( USBDCTX.USBD_SetupReqLen == 0 )
			{
				USBDCTX.pCtrlPayloadPtr = NULL;
				// Enable recieving data
				SetEPR_Status( 0, USBD_EPR_STAT_RX_MASK, USBD_EPR_STAT_RX_ACK );
			}
			else
			{
				// USBD_SEND_ACK is special variable to say 0 byte tx
				if ( USBDCTX.USBD_SetupReqLen == USBD_SEND_ACK )
				{
					USBDCTX.USBD_SetupReqLen = 0;
				}

				const uint16_t tx_len = min( USBDCTX.USBD_SetupReqLen, USBD_PACKET_SIZE );
				// Overreads but eh
				for ( int i = 0; i < tx_len; i += 2 )
				{
					USBDCTX.usbd_ep_data_ptr[0][i / 2] = *( (const uint16_t *)( USBDCTX.pCtrlPayloadPtr + i ) );
				}
				USBD_BDT->EP[0].COUNTn_TX = tx_len;
				USBDCTX.pCtrlPayloadPtr += tx_len;
				USBDCTX.USBD_SetupReqLen -= tx_len;
				SetEPR_Status( 0, USBD_EPR_STAT_TX_MASK, USBD_EPR_STAT_TX_ACK );
			}
		}

		// IN request
		if ( epr & USBD_CTR_TX )
		{
			if ( ep == 0 )
			{
				if ( USBDCTX.USBD_SetupReqCode == USB_SET_ADDRESS )
				{
					USBD->DADDR = 0x80 | USBDCTX.USBD_DevAddr;
					USBDCTX.USBD_DevAddr = 0;
				}
			}
			else
			{
				// Send data for non-zero endpoint
#if FUSB_USER_HANDLERS
				len = HandleInRequest( &USBDCTX, ep, USBDCTX.endpoints[ep].in, 0 );
#endif

				if ( len )
				{
					if ( len < 0 ) len = 0;
					SetEPR_Status( ep, USBD_EPR_STAT_TX_MASK, USBD_EPR_STAT_TX_ACK );
				}
				else
				{
					SetEPR_Status( ep, USBD_EPR_STAT_RX_MASK, USBD_EPR_STAT_RX_NAK );
				}
			}

			// Sent, no longer busy
			USBDCTX.endpoints[ep].busy = 0;

			// Clear TX (Toggle)
			USBD->EPR[ep] = ( USBD->EPR[ep] & ( USBD_EA | USBD_EPKIND | USBD_EPTYPE ) ) | USBD_CTR_RX;
		}

		USBD->ISTR = ~USBD_CTR;
		return;

	stall:
		SetEPR_Status( 0, USBD_EPR_STAT_TX_MASK, USBD_EPR_STAT_TX_STALL );
		SetEPR_Status( 0, USBD_EPR_STAT_RX_MASK, USBD_EPR_STAT_RX_STALL );
		USBD->EPR[ep] = ( epr & ( USBD_EPR_EA | USBD_EPR_EP_TYPE_MASK | USBD_EPR_EP_KIND ) );
	}

	if ( istr & USBD_RESET )
	{
		USBD->ISTR = ~USBD_RESET;
		USBD->BTABLE = 0;

		for ( uint8_t ep = 0; ep < FUSB_MAX_EP_CNT; ++ep )
		{
			USBD->EPR[ep] = ep;

			// TODO: Add method to set EP type
			// Currently the user must override the function
			// It seems that using TYPE_BULK is fine for all types though
			SetEPR_Status( ep, USBD_EPR_EP_TYPE_MASK, USBD_EPR_EP_TYPE_BULK );
			SetEPR_Status( ep, USBD_EPR_STAT_RX_MASK, USBD_EPR_STAT_RX_ACK );
			SetEPR_Status( ep, USBD_EPR_STAT_TX_MASK, USBD_EPR_STAT_TX_NAK );

			// Disable unused modes
			if ( !( USBDCTX.endpoints[ep].mode & USBD_EP_MODE_RX ) )
			{
				SetEPR_Status( ep, USBD_EPR_STAT_RX_MASK, USBD_EPR_STAT_RX_DIS );
			}

			if ( !( USBDCTX.endpoints[ep].mode & USBD_EP_MODE_TX ) )
			{
				SetEPR_Status( ep, USBD_EPR_STAT_TX_MASK, USBD_EPR_STAT_TX_DIS );
			}
			USBDCTX.endpoints[ep].busy = 0;
		}

		// Override EP0 to control - mandatory
		SetEPR_Status( 0, USBD_EPR_EP_TYPE_MASK, USBD_EPR_EP_TYPE_CTRL );

		// Reset context
		USBDCTX.USBD_DevConfig = 0;
		USBDCTX.USBD_DevAddr = 0;
		USBDCTX.USBD_DevSleepStatus = 0;
		USBDCTX.USBD_DevEnumStatus = 0;

		// Enable USB function (we default to address 0)
		USBD->DADDR = USBD_EF;
		return;
	}

	// If size is a concern, merge this with the ISTR set bellow
	if ( istr & ( USBD_ERR | USBD_PMAOVR ) )
	{
#ifdef DEBUG
		printf( "ERROR! USB error\n" );
#endif
		USBD->ISTR = ~( USBD_ERR | USBD_PMAOVR );
		return;
	}

	// We don't need the rest of the events, so we just default to resetting them
	USBD->ISTR = ~( USBD_ESOF | USBD_SOF | USBD_WKUP | USBD_SUSP );
}

static uint8_t EP_buffer[USBD_PACKET_SIZE];

uint8_t *USBD_GetEPBufferIfAvailable( int endp )
{
	if ( USBDCTX.endpoints[endp].busy ) return 0;
	return EP_buffer;
}

int USBD_SendEndpoint( int endp, int len )
{
	if ( USBDCTX.endpoints[endp].busy ) return -1;
	if ( USBDCTX.USBD_SetupReqLen > 0 ) return -2;

	for ( int i = 0; i < len; i += 2 )
	{
		USBDCTX.usbd_ep_data_ptr[endp][i / 2] = *( (const uint16_t *)( EP_buffer + i ) );
	}

	USBD_BDT->EP[endp].COUNTn_TX = len;
	USBDCTX.endpoints[endp].busy = 1;
	SetEPR_Status( endp, USBD_EPR_STAT_TX_MASK, USBD_EPR_STAT_TX_ACK );

	return 0;
}

int USBD_SendEndpointNEW( int endp, uint8_t *data, int len, int copy )
{
	if ( USBDCTX.endpoints[endp].busy ) return -1;
	// This prevents sending while ep0 is receiving
	if ( USBDCTX.USBD_SetupReqLen > 0 ) return USBDCTX.USBD_SetupReqLen;

	// For FS compatability
	USBDCTX.endpoints[endp].in = data;

	for ( int i = 0; i + 1 < len; i += 2 )
	{
		USBDCTX.usbd_ep_data_ptr[endp][i / 2] = data[i] | ( (uint16_t)data[i + 1] << 8 );
	}
	if ( len % 2 != 0 )
	{
		USBDCTX.usbd_ep_data_ptr[endp][len / 2] = data[len - 1];
	}

	USBD_BDT->EP[endp].COUNTn_TX = len;
	USBDCTX.endpoints[endp].busy = 1;
	SetEPR_Status( endp, USBD_EPR_STAT_TX_MASK, USBD_EPR_STAT_TX_ACK );

	return 0;
}

int USBD_SendACK( int endp, int tx )
{
	if ( tx )
	{
		SetEPR_Status( endp, USBD_EPR_STAT_TX_MASK, USBD_EPR_STAT_TX_ACK );
	}
	else
	{
		SetEPR_Status( endp, USBD_EPR_STAT_RX_MASK, USBD_EPR_STAT_RX_ACK );
	}

	return 0;
}

int USBD_SendNAK( int endp, int tx )
{
	if ( tx )
	{
		SetEPR_Status( endp, USBD_EPR_STAT_TX_MASK, USBD_EPR_STAT_TX_NAK );
	}
	else
	{
		SetEPR_Status( endp, USBD_EPR_STAT_RX_MASK, USBD_EPR_STAT_RX_NAK );
	}

	return 0;
}

#if defined( FUNCONF_USE_USBPRINTF ) && FUNCONF_USE_USBPRINTF
uint8_t usb_inputbuffer[USBD_PACKET_SIZE];
int usb_inbuf_idx;

WEAK int HandleInRequest( struct _USBState *ctx, int endp, uint8_t *data, int len )
{
	return 0;
}

WEAK void HandleDataOut( struct _USBState *ctx, int endp, uint8_t *data, int len )
{
	if ( endp == 0 )
	{
		ctx->USBD_SetupReqLen = 0; // To ACK
	}
	else if ( endp == 2 )
	{
		// discard oldest if polling is too slow
		int headroom = ( sizeof( usb_inputbuffer ) - usb_inbuf_idx ) - len;
		if ( headroom < 0 )
		{
			// not enough space left, free up some
			int offset = -headroom;
			for ( int i = offset; i < sizeof( usb_inputbuffer ); i++ )
			{
				usb_inputbuffer[i - offset] = usb_inputbuffer[i];
			}
			usb_inbuf_idx -= offset;
		}

		for ( int i = 0; i < len; i++ )
		{
			usb_inputbuffer[usb_inbuf_idx++] = data[i];
		}
	}
}

void poll_input()
{
	if ( usb_inbuf_idx )
	{
		HandleUSBInput( usb_inbuf_idx, usb_inputbuffer );
		usb_inbuf_idx = 0;
	}
}

WEAK int HandleSetupCustom( struct _USBState *ctx, int setup_code )
{
	int ret = -1;
	if ( ctx->USBD_SetupReqType & USB_REQ_TYP_CLASS )
	{
		switch ( setup_code )
		{
			case CDC_SET_LINE_CODING:
			case CDC_SET_LINE_CTLSTE:
			case CDC_SEND_BREAK: ret = ( ctx->USBD_SetupReqLen ) ? ctx->USBD_SetupReqLen : -1; break;
			case CDC_GET_LINE_CODING: ret = ctx->USBD_SetupReqLen; break;
			default: ret = 0; break;
		}
	}
	else
	{
		ret = 0; // Go to STALL
	}

	return ret;
}
#endif

#if defined( FUNCONF_USE_USBPRINTF ) && FUNCONF_USE_USBPRINTF
int USBFS_SendEndpointNEW( int endp, uint8_t *data, int len, int copy )
{
	return USBD_SendEndpointNEW( endp, data, len, copy );
}
#endif

#if __STDC_VERSION__ >= 201112L || defined( __GNUC__ ) || defined( __clang__ )

// Very hacky way to ensure that we don't get a BDT overflow
// I miss C++...
// This should more logically be in usbd.h, but we get multiple objects
// if we do that
//
// No checking on compilers w/o static assert :(
static const char _number_of_endpoints[] = {
#if FUSB_EP1_MODE
	FUSB_EP1_MODE,
#endif
#if FUSB_EP2_MODE
	FUSB_EP2_MODE,
#endif
#if FUSB_EP3_MODE
	FUSB_EP3_MODE,
#endif
#if FUSB_EP4_MODE
	FUSB_EP4_MODE,
#endif
#if FUSB_EP5_MODE
	FUSB_EP5_MODE,
#endif
#if FUSB_EP6_MODE
	FUSB_EP6_MODE,
#endif
#if FUSB_EP7_MODE
	FUSB_EP7_MODE,
#endif
};

#if __STDC_VERSION__ >= 202311L
static_assert( ( sizeof( _number_of_endpoints ) * USBD_PACKET_SIZE + USBD_PMA_BASE ) <= 512,
	"USBD BDT overflow! Please use less endpoints, or make usbd packet size smaller" );
#elif defined( __GNUC__ ) || defined( __clang__ ) || __STDC_VERSION__ >= 201112L
_Static_assert( ( sizeof( _number_of_endpoints ) * USBD_PACKET_SIZE + USBD_PMA_BASE ) <= 512,
	"USBD BDT overflow! Please use less endpoints, or make usbd packet size smaller" );
#endif

#endif

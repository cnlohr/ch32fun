#include "hsusb.h"
#include "ch32fun.h"
#include <string.h>

struct _USBState USBHSCTX;
volatile uint8_t usb_debug = 0;

#ifdef CH584_CH585
static inline void mcpy_raw( void *dst, void *start, void *end )
{
	__asm__ volatile ( ".insn r 0x0f, 0x7, 0, x0, %3, %0, %1"
	      : "+r"(start), "+r"(dst)
	      :  "r"(0), "r"(end)
	      : "memory" );
}

void * fast_memcpy( void *dst, void *src, uint32_t size )
{
	uint32_t * end = src + size;
	mcpy_raw( dst, src, (void *)end );

	return dst;
}
#define copyBuffer fast_memcpy
// #define copyBuffer memcpy
#else
#define copyBuffer memcpy
#endif

#if (USBHS_IMPL==2)
#define USBHS_IRQHandler USB2_DEVICE_IRQHandler
#define USBHS_IRQn USB2_DEVICE_IRQn
#endif

#if FUSB_USE_HPE // Will it ever work?
// There is an issue with some registers apparently getting lost with HPE, just do it the slow way.
#if defined(CH584_CH585)
void USBHS_IRQHandler() __attribute__((section(".highcode"))) __attribute((interrupt));
#else
void USBHS_IRQHandler() __attribute__((section(".text.vector_handler"))) __attribute((interrupt));
#endif
// void USBHD_IRQHandler() __attribute__((section(".text.vector_handler"))) __attribute((naked));
#else
#if defined(CH584_CH585)
void USBHS_IRQHandler() __attribute__((section(".highcode"))) __attribute((interrupt));
#else
void USBHS_IRQHandler() __attribute__((section(".text.vector_handler"))) __attribute((interrupt));
#endif
#endif

void USBHS_InternalFinishSetup();

void USBHS_IRQHandler()
{
#if FUSB_IO_PROFILE
	funDigitalWrite( DEBUG_PIN, 1 );
#endif

#if (FUSB_SOF_HSITRIM)
	uint64_t systick_local = SysTick->CNT;
#endif

	// Combined FG + ST flag.
	uint16_t intfgst = *(uint16_t*)(&USBHS->INT_FG);
	int len = 0;
	struct _USBState * ctx = &USBHSCTX;
	uint8_t * ctrl0buff = ctx->CTRL0BUFF;
	// int ep = ( intfgst & CMASK_UIS_ENDP ) >> 8;

	// First check if this it setup
#if (USBHS_IMPL==1)
	if( intfgst & CRB_UIF_SETUP_ACT )
#else
	if( (intfgst & CRB_UIF_TRANSFER) && !(intfgst & (1<<12)) && !(( intfgst & CMASK_UIS_ENDP ) >> 8) && (USBHS->UEP0_RX_CTRL & USBHS_UEP_R_SETUP_IS) )
#endif
	{
		/* Setup stage processing */
		UEP_CTRL_TX(0) = USBHS_UEP_T_RES_NAK | USBHS_UEP_T_TOG_DATA1;
		UEP_CTRL_RX(0) = USBHS_UEP_R_RES_NAK | USBHS_UEP_R_TOG_DATA1;

		/* Store All Setup Values */
		int USBHS_SetupReqType = USBHSCTX.USBHS_SetupReqType  = pUSBHS_SetupReqPak->bmRequestType;
		int USBHS_SetupReqCode = USBHSCTX.USBHS_SetupReqCode  = pUSBHS_SetupReqPak->bRequest;
		int USBHS_SetupReqLen = USBHSCTX.USBHS_SetupReqLen    = pUSBHS_SetupReqPak->wLength;
		int USBHS_SetupReqIndex = pUSBHS_SetupReqPak->wIndex;
		int USBHS_IndexValue = USBHSCTX.USBHS_IndexValue = ( pUSBHS_SetupReqPak->wIndex << 16 ) | pUSBHS_SetupReqPak->wValue;
		if( usb_debug ) printf( "[USB] SETUP: %02x %02x %d %04x %08x\n", USBHS_SetupReqType, USBHS_SetupReqCode, USBHS_SetupReqLen, USBHS_SetupReqIndex, USBHS_IndexValue );

		if( ( USBHS_SetupReqType & USB_REQ_TYP_MASK ) != USB_REQ_TYP_STANDARD )
		{
#if FUSB_HID_INTERFACES > 0  || FUSB_USER_HANDLERS
			if( ( USBHS_SetupReqType & USB_REQ_TYP_MASK ) == USB_REQ_TYP_CLASS )
			{
				/* Class Request */
				switch( USBHS_SetupReqCode )
				{
#if FUSB_HID_INTERFACES > 0
					case HID_SET_REPORT:
#if FUSB_HID_USER_REPORTS
						len = HandleHidUserSetReportSetup( ctx, pUSBHS_SetupReqPak );
						if( len < 0 ) 
						{
							goto sendstall;
						}
						ctx->USBHS_SetupReqLen = len;
						UEP_CTRL_LEN(0) = 0;
						// Previously would have been a CTRL_RX = ACK && TOG, but not here on the 203.
						UEP_CTRL_RX(0) = USBHS_UEP_R_RES_ACK | USBHS_UEP_R_TOG_DATA1;
						UEP_CTRL_TX(0) = USBHS_UEP_T_TOG_DATA1;
						goto replycomplete;
					
					case HID_GET_REPORT:
						len = HandleHidUserGetReportSetup( ctx, pUSBHS_SetupReqPak );
						if( len < 0 ) 
						{
							goto sendstall;
						}
						ctx->USBHS_SetupReqLen = len;
						len = len >= USBHS_DEF_UEP0_SIZE ? USBHS_DEF_UEP0_SIZE : len;
						if( !ctx->pCtrlPayloadPtr )
						{
							len = HandleHidUserReportDataIn( ctx, ctrl0buff, len );
						}
						else
						{
							copyBuffer( ctrl0buff, ctx->pCtrlPayloadPtr, len );
							ctx->pCtrlPayloadPtr += len;
						}
						UEP_CTRL_LEN(0) = len;
						UEP_CTRL_TX(0) = USBHS_UEP_T_RES_ACK | USBHS_UEP_T_TOG_DATA1;
						ctx->USBHS_SetupReqLen -= len;
						goto replycomplete;
#endif
						break;

					case HID_SET_IDLE:
						if( USBHS_SetupReqIndex < FUSB_HID_INTERFACES )
							USBHSCTX.USBHS_HidIdle[ USBHS_SetupReqIndex ] = (uint8_t)( USBHS_IndexValue >> 8 );
						break;
					case HID_SET_PROTOCOL:
						if ( USBHS_SetupReqIndex < FUSB_HID_INTERFACES )
							USBHSCTX.USBHS_HidProtocol[USBHS_SetupReqIndex] = (uint8_t)USBHS_IndexValue;
						break;

					case HID_GET_IDLE:
						if( USBHS_SetupReqIndex < FUSB_HID_INTERFACES )
						{
							ctrl0buff[0] = USBHSCTX.USBHS_HidIdle[ USBHS_SetupReqIndex ];
							len = 1;
						}
						break;

					case HID_GET_PROTOCOL:
						if( USBHS_SetupReqIndex < FUSB_HID_INTERFACES )
						{
							ctrl0buff[0] = USBHSCTX.USBHS_HidProtocol[ USBHS_SetupReqIndex ];
							len = 1;
						}
						break;
#endif
					default:
#if FUSB_USER_HANDLERS
						len = HandleSetupCustom( ctx, USBHS_SetupReqCode );
					
						if( len )
						{
							if( len < 0 ) {
								len = 0;
								ctx->USBHS_SetupReqLen = 0;
							}
							else
							{
								ctx->USBHS_SetupReqLen = len;
								copyBuffer( ctrl0buff, ctx->pCtrlPayloadPtr, len );
								// printf("%02x-%02x-%02x-%02x-%02x-%02x-%02x\n", ctrl0buff[0], ctrl0buff[1], ctrl0buff[2], ctrl0buff[3], ctrl0buff[4], ctrl0buff[5], ctrl0buff[6]);
								ctx->pCtrlPayloadPtr += len;
							}
							
							if( ctx->USBHS_SetupReqType & USBHS_DEF_UEP_IN || ctx->USBHS_SetupReqLen == 0)
							{
								len = len >= USBHS_DEF_UEP0_SIZE ? USBHS_DEF_UEP0_SIZE : len;
								UEP_CTRL_LEN(0) = len;
								UEP_CTRL_TX(0) = USBHS_UEP_T_TOG_DATA1 | USBHS_UEP_T_RES_ACK;
								ctx->USBHS_SetupReqLen -= len;
							}
							else UEP_CTRL_RX(0)= USBHS_UEP_R_TOG_DATA1 | USBHS_UEP_R_RES_ACK;
							// UEP_CTRL_LEN(0) = len;
							// UEP_CTRL_TX(0) = CHECK_USBHS_UEP_T_AUTO_TOG | USBHS_UEP_T_RES_ACK | USBHS_UEP_T_TOG;
							// ctx->USBHS_SetupReqLen -= len;
							// goto epzero_rxtx;
							goto replycomplete;
						}
						else
#endif
						{
							goto sendstall;
						}
						break;
				}
			}
#else
			;
#endif
		}
		else
		{
			/* usb standard request processing */
			switch( USBHS_SetupReqCode )
			{
				/* get device/configuration/string/report/... descriptors */
				case USB_GET_DESCRIPTOR:
				{
					const struct descriptor_list_struct * e = descriptor_list;
					const struct descriptor_list_struct * e_end = e + DESCRIPTOR_LIST_ENTRIES;
					for( ; e != e_end; e++ )
					{
						if( e->lIndexValue == USBHS_IndexValue )
						{
							ctx->pCtrlPayloadPtr = (uint8_t*)e->addr;
							len = e->length;
							break;
						}
					}
					if( e == e_end ) // If descriptor list is empty
					{
						goto sendstall;
					}

					/* Copy Descriptors to Endp0 DMA buffer */
					if( ctx->USBHS_SetupReqLen > len ) ctx->USBHS_SetupReqLen = len;
					len = ( USBHS_SetupReqLen >= USBHS_DEF_UEP0_SIZE ) ? USBHS_DEF_UEP0_SIZE : USBHS_SetupReqLen;
					copyBuffer( ctrl0buff, ctx->pCtrlPayloadPtr, len );
					ctx->pCtrlPayloadPtr += len;
				}
				break;

				/* Set usb address */
				case USB_SET_ADDRESS:
					ctx->USBHS_DevAddr = (uint8_t)( ctx->USBHS_IndexValue & 0xFF );
					// NOTE: Do not actually set addres here!  If we do, we won't get the PID_IN associated with this SETUP.
					break;

				/* Get usb configuration now set */
				case USB_GET_CONFIGURATION:
					ctrl0buff[0] = ctx->USBHS_DevConfig;
					if( ctx->USBHS_SetupReqLen > 1 )
						ctx->USBHS_SetupReqLen = 1;
					break;

				/* Set usb configuration to use */
				case USB_SET_CONFIGURATION:
					ctx->USBHS_DevConfig = (uint8_t)( ctx->USBHS_IndexValue & 0xFF );
					ctx->USBHS_DevEnumStatus = 0x01;
					break;

				/* Clear or disable one usb feature */
				case USB_CLEAR_FEATURE:
#if FUSB_SUPPORTS_SLEEP
					if( ( USBHS_SetupReqType & USB_REQ_RECIP_MASK ) == USB_REQ_RECIP_DEVICE )
					{
						/* clear one device feature */
						if( (uint8_t)( USBHS_IndexValue & 0xFF ) == USB_REQ_FEAT_REMOTE_WAKEUP )
						{
							/* clear usb sleep status, device not prepare to sleep */
							ctx->USBHS_DevSleepStatus &= ~0x01;
						}
						else
						{
							goto sendstall;
						}
					}
					else
#endif
					if( ( USBHS_SetupReqType & USB_REQ_RECIP_MASK ) == USB_REQ_RECIP_ENDP )
					{
						if( (uint8_t)( USBHS_IndexValue & 0xFF ) == USB_REQ_FEAT_ENDP_HALT )
						{
							/* Clear End-point Feature */
							int ep = USBHS_SetupReqIndex & 0xf;
							if( ep < FUSB_CONFIG_EPS ) 
							{
								// UEP_CTRL_TX(ep) = USBHS_UEP_T_RES_STALL;
								if( USBHS_SetupReqIndex & USBHS_DEF_UEP_IN  && ctx->endpoint_mode[ep] == -1 ) UEP_CTRL_TX(ep) = USBHS_UEP_T_RES_NAK;
								else if( USBHS_SetupReqIndex & USBHS_DEF_UEP_OUT && ctx->endpoint_mode[ep] == 1 ) UEP_CTRL_RX(ep) = USBHS_UEP_R_RES_ACK;
								else 
								{
									goto sendstall;
								}
							}
							else
							{
								goto sendstall;
							}
						}
						else
						{
							goto sendstall;
						}
					}
					else
					{
						goto sendstall;
					}
					break;

				/* set or enable one usb feature */
				case USB_SET_FEATURE:
					if( ( USBHS_SetupReqType & USB_REQ_RECIP_MASK ) == USB_REQ_RECIP_DEVICE )
					{
#if FUSB_SUPPORTS_SLEEP
						/* Set Device Feature */
						if( (uint8_t)( USBHS_IndexValue & 0xFF ) == USB_REQ_FEAT_REMOTE_WAKEUP )
						{
							/* Set Wake-up flag, device prepare to sleep */
							USBHS_DevSleepStatus |= 0x01;
						}
						else
#endif
						{
							goto sendstall;
						}
					}
					else if( ( USBHS_SetupReqType & USB_REQ_RECIP_MASK ) == USB_REQ_RECIP_ENDP )
					{
						/* Set Endpoint Feature */
						if( (uint8_t)( USBHS_IndexValue & 0xFF ) == USB_REQ_FEAT_ENDP_HALT )
						{
							int ep = USBHS_SetupReqIndex & 0xf;
							if( ep < FUSB_CONFIG_EPS )
							{
								if( (USBHS_SetupReqIndex & USBHS_DEF_UEP_IN) && ctx->endpoint_mode[ep] == -1 ) UEP_CTRL_TX(ep) = ( UEP_CTRL_TX(ep) & ~USBHS_UEP_T_RES_MASK ) | USBHS_UEP_T_RES_STALL;
								else if( (USBHS_SetupReqIndex & USBHS_DEF_UEP_OUT) && ctx->endpoint_mode[ep] == 1 ) UEP_CTRL_RX(ep) = ( UEP_CTRL_RX(ep) & ~USBHS_UEP_R_RES_MASK ) | USBHS_UEP_R_RES_STALL;
								else 
								{
									goto sendstall;
								}
							}
						}
						else
						{
							goto sendstall;
						}
					}
					else
					{
						goto sendstall;
					}
					break;

				/* This request allows the host to select another setting for the specified interface  */
				case USB_GET_INTERFACE:
					ctrl0buff[0] = 0x00;
					if( USBHS_SetupReqLen > 1 ) USBHS_SetupReqLen = 1;
					break;

				case USB_SET_INTERFACE:
					break;

				/* host get status of specified device/interface/end-points */
				case USB_GET_STATUS:
					ctrl0buff[0] = 0x00;
					ctrl0buff[1] = 0x00;
					if( ( USBHS_SetupReqType & USB_REQ_RECIP_MASK ) == USB_REQ_RECIP_DEVICE )
					{
#if FUSB_SUPPORTS_SLEEP
						ctrl0buff[0] = (ctx->USBHS_DevSleepStatus & 0x01)<<1;
#else
						ctrl0buff[0] = 0x00;
#endif
					}
					else if( ( USBHS_SetupReqType & USB_REQ_RECIP_MASK ) == USB_REQ_RECIP_ENDP )
					{
						int ep = USBHS_SetupReqIndex & 0xf;
						if( ep < FUSB_CONFIG_EPS )
						{
							if( USBHS_SetupReqIndex & USBHS_DEF_UEP_IN && ctx->endpoint_mode[ep] == -1 ) ctrl0buff[0] = ( UEP_CTRL_TX(ep) & USBHS_UEP_T_RES_MASK ) == USBHS_UEP_T_RES_STALL;
							else if( USBHS_SetupReqIndex & USBHS_DEF_UEP_OUT && ctx->endpoint_mode[ep] == 1 ) ctrl0buff[0] = ( UEP_CTRL_RX(ep) & USBHS_UEP_R_RES_MASK ) == USBHS_UEP_R_RES_STALL;
							else goto sendstall;
						}
						else
						{
							goto sendstall;
						}
					}
					else
					{
						goto sendstall;
					}
					if( USBHS_SetupReqLen > 2 )
						USBHS_SetupReqLen = 2;
					break;

				default:
					goto sendstall;
					break;
			}
		}
	// epzero_rxtx:
		{
			/* end-point 0 data Tx/Rx */
			if( USBHS_SetupReqType & USBHS_DEF_UEP_IN )
			{
				len = ( ctx->USBHS_SetupReqLen > USBHS_DEF_UEP0_SIZE )? USBHS_DEF_UEP0_SIZE : ctx->USBHS_SetupReqLen;
				ctx->USBHS_SetupReqLen -= len;
				UEP_CTRL_LEN(0) = len;
				UEP_CTRL_TX(0) = USBHS_UEP_T_RES_ACK | USBHS_UEP_T_TOG_DATA1;
			}
			else
			{
				if( ctx->USBHS_SetupReqLen == 0 )
				{
					UEP_CTRL_LEN(0) = 0;
					UEP_CTRL_TX(0) = USBHS_UEP_T_RES_ACK | USBHS_UEP_T_TOG_DATA1;
				}
				else
				{
					UEP_CTRL_RX(0) = USBHS_UEP_R_RES_ACK | USBHS_UEP_R_TOG_DATA1;
				}
			}
		}

		goto replycomplete;
		
sendstall:
		{
			// if one request not support, return stall.  Stall means permanent error.
			UEP_CTRL_TX(0) = USBHS_UEP_T_TOG_DATA1 | USBHS_UEP_T_RES_STALL;
			UEP_CTRL_RX(0) = USBHS_UEP_R_TOG_DATA1 | USBHS_UEP_R_RES_STALL;
		}

replycomplete:
		USBHS->INT_FG = intfgst;
		USBHS_DONE_RX(0);

	}
	else if( intfgst & CRB_UIF_TRANSFER )
	{
		int token = ( intfgst & CMASK_UIS_TOKEN ) >> 12;
		int ep = ( intfgst & CMASK_UIS_ENDP ) >> 8;
		
		if( usb_debug ) printf( "[USB] TRANSFER, token = %02x, ep = %d, bmRequestType = %02x, bRequest = %02x\n", token, ep, pUSBHS_SetupReqPak->bmRequestType, pUSBHS_SetupReqPak->bRequest );
		switch ( token )
		{
		case CUIS_TOKEN_IN:
			if( ep )
			{
				if( ep < FUSB_CONFIG_EPS )
				{
#if FUSB_USER_HANDLERS
					len = HandleInRequest( ctx, ep, ctx->ENDPOINTS[ ep-1 ], 0 );
#endif
					UEP_CTRL_TX(ep) ^= USBHS_UEP_T_TOG_DATA1;
					if( len )
					{
						if( len < 0 ) len = 0;
						UEP_CTRL_LEN(ep) = len;
						UEP_CTRL_TX(ep) = ( UEP_CTRL_TX(ep) & ~USBHS_UEP_T_RES_MASK ) | USBHS_UEP_T_RES_ACK;
					}
					else
					{
						UEP_CTRL_TX(ep) = ( UEP_CTRL_TX(ep) & ~USBHS_UEP_T_RES_MASK ) | USBHS_UEP_T_RES_NAK;
					}
					ctx->USBHS_Endp_Busy[ep] = 0;
				}
			}
			else // EP0
			{
				/* end-point 0 data in interrupt */
				if( ctx->USBHS_SetupReqLen == 0 )
				{
					UEP_CTRL_RX(0) = USBHS_UEP_R_TOG_DATA1 | USBHS_UEP_R_RES_ACK;
				}

				ctx->USBHS_errata_dont_send_endpoint_in_window = 0;

				if( ctx->pCtrlPayloadPtr )
				{
					// Shortcut mechanism, for descriptors or if the user wants it.
					len = ctx->USBHS_SetupReqLen >= USBHS_DEF_UEP0_SIZE ? USBHS_DEF_UEP0_SIZE : ctx->USBHS_SetupReqLen;
					copyBuffer( ctrl0buff, ctx->pCtrlPayloadPtr, len ); // FYI -> Would need to do this if using DMA
					ctx->USBHS_SetupReqLen -= len;
					if( ctx->USBHS_SetupReqLen > 0 )
						ctx->pCtrlPayloadPtr += len;
					else
						ctx->pCtrlPayloadPtr = 0;

					UEP_CTRL_LEN(0) = len;
					UEP_CTRL_TX(0) ^= USBHS_UEP_T_TOG_DATA1;
#if (USBHS_IMPL==2)
					UEP_CTRL_TX(0) = ( UEP_CTRL_TX(0) & ~USBHS_UEP_T_RES_MASK) | USBHS_UEP_T_RES_ACK; // clear
#endif
				}
				else if ( ( ctx->USBHS_SetupReqType & USB_REQ_TYP_MASK ) != USB_REQ_TYP_STANDARD )
				{
					len = ctx->USBHS_SetupReqLen >= USBHS_DEF_UEP0_SIZE ? USBHS_DEF_UEP0_SIZE : ctx->USBHS_SetupReqLen;
#if FUSB_HID_USER_REPORTS
					if( len && USBHSCTX.USBHS_SetupReqCode == HID_GET_REPORT )
					{
						len = HandleHidUserReportDataIn( ctx, ctrl0buff, len );
						UEP_CTRL_LEN(0) = len;
						UEP_CTRL_TX(0) ^= USBHS_UEP_T_TOG_DATA1;
						ctx->USBHS_SetupReqLen -= len;
						ctx->pCtrlPayloadPtr += len;
					}	
#endif
#if FUSB_USER_HANDLERS
					if( len && USBHSCTX.USBHS_SetupReqCode != HID_GET_REPORT )
					{
						len = HandleInRequest( ctx, 0, ctrl0buff, len );
						UEP_CTRL_LEN(0) = len;
						UEP_CTRL_TX(0) ^= USBHS_UEP_T_TOG_DATA1;
						ctx->USBHS_SetupReqLen -= len;
						ctx->pCtrlPayloadPtr += len;
					}
#endif
				}
				else
				{
					switch( USBHSCTX.USBHS_SetupReqCode )
					{
						case USB_GET_DESCRIPTOR:
							break;

						case USB_SET_ADDRESS:
							USBHS->DEV_AD = ctx->USBHS_DevAddr;
							break;

						default:
							break;
					}
				}
			}
			USBHS_DONE_TX(ep);
			break;

		/* data-out stage processing */
		case CUIS_TOKEN_OUT:
#if (USBHS_IMPL == 1)
			len = USBHS->RX_LEN;
#else
			if( ep ) len = UEP_RX_LEN(ep);
			else len = USBHS->UEP0_RX_LEN;
#endif
			switch( ep )
			{
				/* end-point 0 data out interrupt */
				case USBHS_DEF_UEP0:
#if (USBHS_IMPL==1)
					if( intfgst & CRB_UIS_TOG_OK )
#else
					if( USBHS->UEP0_RX_CTRL & (1<<4) ) // RB_UEP_R_TOG_MATCH
#endif
					{
#if FUSB_HID_USER_REPORTS
						if( ctx->USBHS_SetupReqCode == HID_SET_REPORT )
						{
							uint8_t * cptr = ctx->pCtrlPayloadPtr;
							if( !cptr )
							{
								HandleHidUserReportDataOut( ctx, ctrl0buff, len );
							}
							else
							{
								int remain = ctx->USBHS_SetupReqLen - len;
								if( remain < 0 )
								{
									len += remain;
									remain = 0;
								}
								copyBuffer( cptr, ctrl0buff, len );
								ctx->USBHS_SetupReqLen = remain;
								if( remain > 0 )
									ctx->pCtrlPayloadPtr = cptr + len;
								else
									ctx->pCtrlPayloadPtr = 0;
							}
						}
#endif
#if FUSB_USER_HANDLERS
						if ( ctx->USBHS_SetupReqCode != HID_SET_REPORT )
						{
							HandleDataOut( ctx, ep, ctrl0buff, len );
						}
#endif
						if( ctx->USBHS_SetupReqLen == 0 )
						{
#if FUSB_HID_USER_REPORTS
							if( ctx->USBHS_SetupReqCode == HID_SET_REPORT )
								HandleHidUserReportOutComplete( ctx );
#endif
							// Only set this flag for OUT requests (e.g. SET_LINE_CODING).
							// For IN requests (e.g. GET_DESCRIPTOR), don't set it.
							if ( ( ctx->USBHS_SetupReqType & USB_REQ_TYP_IN ) == 0 )
							{
								ctx->USBHS_errata_dont_send_endpoint_in_window = 1;
							}
							UEP_CTRL_LEN(0) = 0;
							UEP_CTRL_TX(0) = USBHS_UEP_T_TOG_DATA1 | USBHS_UEP_T_RES_ACK;
						}
						else
						{
							UEP_CTRL_RX(0) ^= USBHS_UEP_R_TOG_DATA1;
#if (USBHS_IMPL==2)
							UEP_CTRL_RX(0) = ( UEP_CTRL_RX(0) & ~USBHS_UEP_R_RES_MASK) | USBHS_UEP_R_RES_ACK; // clear
#endif	
						}
					}
					USBHS_DONE_RX(0);
					break;

				default:
#if (USBHS_IMPL==1)
					if( intfgst & CRB_UIS_TOG_OK )
#else
					if( UEP_CTRL_RX(ep) & (1<<4) ) // RB_UEP_R_TOG_MATCH
#endif
					{
						UEP_CTRL_RX(ep) ^= USBHS_UEP_R_TOG_DATA1;
#if (USBHS_IMPL==2)
						UEP_CTRL_RX(ep) = ( UEP_CTRL_RX(ep) & ~USBHS_UEP_R_RES_MASK) | USBHS_UEP_R_RES_ACK; // clear
#endif
#if FUSB_USER_HANDLERS
						HandleDataOut( ctx, ep, ctx->ENDPOINTS[ep-1], len );
#endif
					}
					USBHS_DONE_RX(ep);
					break;
			}
			break;

#if (USBHS_IMPL==1)
		case CUIS_TOKEN_SOF:
			{
#if (FUSB_SOF_HSITRIM)
				int32_t diff = (int64_t)(systick_local - ctx->USBHS_sof_timestamp);
				uint32_t trim = (RCC->CTLR & RCC_HSITRIM) >> 3;
				if( diff > TICKS_PER_HSITRIM && (trim > 0)) {
					uint32_t regtemp;
					regtemp = RCC->CTLR & ~RCC_HSITRIM;
					RCC->CTLR = regtemp | (--trim)<<3;
				}
				else if( diff < 0 && diff < (TICKS_PER_HSITRIM*-1) && (trim < 31))
				{
					uint32_t regtemp;
					regtemp = RCC->CTLR & ~RCC_HSITRIM;
					RCC->CTLR = regtemp | (++trim)<<3;;
				}
				ctx->USBHS_sof_timestamp = systick_local + Ticks_from_Us(125);
#endif
				USBHS->INT_FG = CRB_UIF_HST_SOF;
			}
			break;
#endif

		default :
			break;
		}
		USBHS->INT_FG = CRB_UIF_TRANSFER;
	}
	else if( intfgst & CRB_UIF_BUS_RST )
	{
		if( usb_debug ) printf( "[USB] RESET\n" );
		/* usb reset interrupt processing */
		ctx->USBHS_DevConfig = 0;
		ctx->USBHS_DevAddr = 0;
		ctx->USBHS_DevSleepStatus = 0;
		ctx->USBHS_DevEnumStatus = 0;

		USBHS->DEV_AD = 0;
		USBHS_InternalFinishSetup();
		USBHS->INT_FG = CRB_UIF_BUS_RST;
	}
	else if( intfgst & CRB_UIF_SUSPEND )
	{
		if( usb_debug ) printf( "[USB] SUSPEND\n" );
		USBHS->INT_FG = CRB_UIF_SUSPEND;
		Delay_Us(10);
		/* usb suspend interrupt processing */
		if( USBHS->MIS_ST & USBHS_UMS_SUSPEND )
		{
			ctx->USBHS_DevSleepStatus |= 0x02;
			if( ctx->USBHS_DevSleepStatus == 0x03 )
			{
				/* Handling usb sleep here */
				//TODO: MCU_Sleep_Wakeup_Operate( );
			}
		}
		else
		{
			ctx->USBHS_DevSleepStatus &= ~0x02;
		}
	}
	else
	{
			/* other interrupts */
			USBHS->INT_FG = intfgst & 0xff;
	}


#if FUSB_IO_PROFILE
	funDigitalWrite( DEBUG_PIN, 0 );
#endif

#if FUSB_USE_HPE
	// asm volatile( "mret" );
#endif
}

void USBHS_InternalFinishSetup()
{

#if FUSB_EP1_MODE
	USBHSCTX.endpoint_mode[1] = FUSB_EP1_MODE;
#ifndef FUSB_EP1_SIZE
	USBHS->UEP1_MAX_LEN = FUSB_EP_SIZE;
#else
	USBHS->UEP1_MAX_LEN = FUSB_EP1_SIZE;
#endif
#if FUSB_EP1_MODE > 0 
	UEP_TX_EN(1);
	UEP_DMA_TX(1) = (uintptr_t)USBHSCTX.ENDPOINTS[0];
#else
	UEP_RX_EN(1);
	UEP_DMA_RX(1) = (uintptr_t)USBHSCTX.ENDPOINTS[0];
#endif
#endif

#if FUSB_EP2_MODE
	USBHSCTX.endpoint_mode[2] = FUSB_EP2_MODE;
#ifndef FUSB_EP2_SIZE
	USBHS->UEP2_MAX_LEN = FUSB_EP_SIZE;
#else
	USBHS->UEP2_MAX_LEN = FUSB_EP2_SIZE;
#endif
#if FUSB_EP2_MODE > 0 
	UEP_TX_EN(2);
	UEP_DMA_TX(2) = (uintptr_t)USBHSCTX.ENDPOINTS[1];
#else
	UEP_RX_EN(2);
	UEP_DMA_RX(2) = (uintptr_t)USBHSCTX.ENDPOINTS[1];
#endif
#endif

#if FUSB_EP3_MODE
	USBHSCTX.endpoint_mode[3] = FUSB_EP3_MODE;
#ifndef FUSB_EP3_SIZE
	USBHS->UEP3_MAX_LEN = FUSB_EP_SIZE;
#else
	USBHS->UEP3_MAX_LEN = FUSB_EP3_SIZE;
#endif
#if FUSB_EP3_MODE > 0 
	UEP_TX_EN(3);
	UEP_DMA_TX(3) = (uintptr_t)USBHSCTX.ENDPOINTS[2];
#else
	UEP_RX_EN(3);
	UEP_DMA_RX(3) = (uintptr_t)USBHSCTX.ENDPOINTS[2];
#endif
#endif

#if FUSB_EP4_MODE
	USBHSCTX.endpoint_mode[4] = FUSB_EP4_MODE;
#ifndef FUSB_EP4_SIZE
	USBHS->UEP4_MAX_LEN = FUSB_EP_SIZE;
#else
	USBHS->UEP4_MAX_LEN = FUSB_EP4_SIZE;
#endif
#if FUSB_EP4_MODE > 0 
	UEP_TX_EN(4);
	UEP_DMA_TX(4) = (uintptr_t)USBHSCTX.ENDPOINTS[3];
#else
	UEP_RX_EN(4);
	UEP_DMA_RX(4) = (uintptr_t)USBHSCTX.ENDPOINTS[3];
#endif
#endif

#if FUSB_EP5_MODE
	USBHSCTX.endpoint_mode[5] = FUSB_EP5_MODE;
#ifndef FUSB_EP5_SIZE
	USBHS->UEP5_MAX_LEN = FUSB_EP_SIZE;
#else
	USBHS->UEP5_MAX_LEN = FUSB_EP5_SIZE;
#endif
#if FUSB_EP5_MODE > 0 
	UEP_TX_EN(5);
	UEP_DMA_TX(5) = (uintptr_t)USBHSCTX.ENDPOINTS[4];
#else
	UEP_RX_EN(5);
	UEP_DMA_RX(5) = (uintptr_t)USBHSCTX.ENDPOINTS[4];
#endif
#endif

#if FUSB_EP6_MODE
	USBHSCTX.endpoint_mode[6] = FUSB_EP6_MODE;
#ifndef FUSB_EP6_SIZE
	USBHS->UEP6_MAX_LEN = FUSB_EP_SIZE;
#else
	USBHS->UEP6_MAX_LEN = FUSB_EP6_SIZE;
#endif
#if FUSB_EP6_MODE > 0 
	UEP_TX_EN(6);
	UEP_DMA_TX(6) = (uintptr_t)USBHSCTX.ENDPOINTS[5];
#else
	UEP_RX_EN(6);
	UEP_DMA_RX(6) = (uintptr_t)USBHSCTX.ENDPOINTS[5];
#endif
#endif

#if FUSB_EP7_MODE
	USBHSCTX.endpoint_mode[7] = FUSB_EP7_MODE;
#ifndef FUSB_EP7_SIZE
	USBHS->UEP7_MAX_LEN = FUSB_EP_SIZE;
#else
	USBHS->UEP7_MAX_LEN = FUSB_EP7_SIZE;
#endif
#if FUSB_EP7_MODE > 0 
	UEP_TX_EN(7);
	UEP_DMA_TX(7) = (uintptr_t)USBHSCTX.ENDPOINTS[6];
#else
	UEP_RX_EN(7);
	UEP_DMA_RX(7) = (uintptr_t)USBHSCTX.ENDPOINTS[6];
#endif
#endif

#if !defined (FUSB_CONFIG_EPS) || !FUSB_CONFIG_EPS
#error You must have at least EP0!
#endif
	
	USBHS->UEP0_DMA = (uintptr_t)USBHSCTX.CTRL0BUFF;
	USBHS->UEP0_MAX_LEN = USBHS_DEF_UEP0_SIZE;
	USBHS->UEP0_T_LEN = 0;
	UEP_TX_EN(0);
	UEP_RX_EN(0);
	UEP_CTRL_TX(0) = USBHS_UEP_T_RES_NAK;
	UEP_CTRL_RX(0) = USBHS_UEP_R_RES_ACK;

	for( int i = 1; i < FUSB_CONFIG_EPS; i++ )
	{
		if( USBHSCTX.endpoint_mode[i] > 0 )
		{
			UEP_CTRL_TX(i) = USBHS_UEP_T_RES_NAK;
		}
		else if( USBHSCTX.endpoint_mode[i] < 0 )
		{
			UEP_CTRL_RX(i) = USBHS_UEP_R_RES_ACK;
		}
		USBHSCTX.USBHS_Endp_Busy[i] = 0;
	}
}

int USBHSSetup()
{
#if defined (CH584_CH585)
	R8_USBHS_PLL_CTRL = USBHS_PLL_EN;
	R16_PIN_CONFIG |= RB_PIN_USB2_EN;
#elif defined (CH32V30x)
#if (FUNCONF_SYSTEM_CORE_CLOCK != 144000000) && (FUNCONF_SYSTEM_CORE_CLOCK != 96000000) && (FUNCONF_SYSTEM_CORE_CLOCK != 48000000)
#error "CH32V30x need 144/96/48MHz main clock for USB to work"
#endif
	RCC->CFGR2 &= ~((7 << 24) | (1 << 27) | (3 << 28) | (1<<31));
#if (FUNCONF_USE_HSE)
	RCC->CFGR2 = RCC_USBHSSRC | RCC_USBHSPLL | 1<< RCC_USBHSCLK_OFFSET | 1 << RCC_USBHSDIV_OFFSET;
#else
#warning "Using HSI"
	RCC->CFGR2 = RCC_USBHSSRC | RCC_USBHSPLL | RCC_USBHSPLLSRC | 1<< RCC_USBHSCLK_OFFSET | 1 << RCC_USBHSDIV_OFFSET;
#endif
	RCC->AHBPCENR |= RCC_USBHSEN | RCC_AHBPeriph_DMA1;
#else
#error "Need to add USB clock setup for this chip"
#endif

#if (USBHS_IMPL==1)
	// Force module to reset.
	USBHS->BASE_CTRL = USBHS_UC_CLR_ALL | USBHS_UC_RESET_SIE;
	Delay_Us(10);
	USBHS->BASE_CTRL = 0;
	USBHS->HOST_CTRL = USBHS_UH_PHY_SUSPENDM;
	// Initialize USB device config
	USBHS->HOST_CTRL = USBHS_UH_PHY_SUSPENDM;
	USBHS->BASE_CTRL = USBHS_UC_DMA_EN | USBHS_UC_INT_BUSY | (FUSB_SPEED<<5);
	USBHS->INT_EN = USBHS_UIE_SETUP_ACT | USBHS_UIE_TRANSFER | USBHS_UIE_DETECT | USBHS_UIE_SUSPEND;
#if (FUSB_SOF_HSITRIM)
	USBHS->INT_EN |= USBHS_UIE_SOF_ACT;
#endif
	USBHS_InternalFinishSetup();
	USBHS->BASE_CTRL |= USBHS_UC_DEV_PU_EN;
#else
	// Force module to reset.
	USBHS->BASE_CTRL = USBHS_UD_RST_LINK | USBHS_UD_PHY_SUSPENDM;
	Delay_Us(10);
	USBHS->BASE_CTRL = 0;
	// Initialize USB device config
	USBHS->INT_EN    = USBHS_UDIE_BUS_RST | USBHS_UDIE_SUSPEND | USBHS_UDIE_BUS_SLEEP | USBHS_UDIE_LPM_ACT | USBHS_UDIE_TRANSFER | USBHS_UDIE_LINK_RDY;
	USBHS_InternalFinishSetup();
	USBHS->BASE_MODE = FUSB_SPEED;
	USBHS->BASE_CTRL = USBHS_UD_DEV_EN | USBHS_UD_DMA_EN | USBHS_UD_LPM_EN | USBHS_UD_PHY_SUSPENDM;
#endif
	
	NVIC_EnableIRQ( USBHS_IRQn );

#if FUSB_IO_PROFILE
	funPinMode( DEBUG_PIN, GPIO_CFGLR_OUT_50Mhz_PP );
#endif

	// Go on-bus.
	return 0;
}

static inline uint8_t * USBHS_GetEPBufferIfAvailable( int endp )
{
	if( USBHSCTX.USBHS_Endp_Busy[endp] ) return 0;
	return USBHSCTX.ENDPOINTS[endp-1];
}

static inline int USBHS_SendEndpoint( int endp, int len )
{
	if( USBHSCTX.USBHS_errata_dont_send_endpoint_in_window || USBHSCTX.USBHS_Endp_Busy[endp] ) return -1;
	// This prevents sending while ep0 is receiving
	if( USBHSCTX.USBHS_SetupReqLen > 0 ) return -2;
	// Check if in SETUP request
	// if( USBHS->UEP0_RX_CTRL & USBHS_UEP_R_SETUP_IS ) return -3;
	NVIC_DisableIRQ( USBHS_IRQn );
	UEP_CTRL_LEN( endp ) = len;
	UEP_CTRL_TX( endp ) = ( UEP_CTRL_TX( endp ) & ~USBHS_UEP_T_RES_MASK ) | USBHS_UEP_T_RES_ACK;
	USBHSCTX.USBHS_Endp_Busy[endp] = 1;
	NVIC_EnableIRQ( USBHS_IRQn );
	return 0;
}

static inline int USBHS_SendEndpointNEW( int endp, uint8_t* data, int len, int copy)
{
	if( USBHSCTX.USBHS_errata_dont_send_endpoint_in_window || USBHSCTX.USBHS_Endp_Busy[endp] ) return -1;
	// This prevents sending while ep0 is receiving
	if( USBHSCTX.USBHS_SetupReqLen > 0 ) return USBHSCTX.USBHS_SetupReqLen;
	
	if ( len )
	{
		if( copy )
		{
			uint8_t* dest = (endp?USBHSCTX.ENDPOINTS[endp-1]:USBHSCTX.CTRL0BUFF);
			copyBuffer( dest, data, len );
		}
		else
		{
			if( !endp ) USBHS->UEP0_DMA = (uintptr_t)data;
			else UEP_DMA_TX( endp ) = (uintptr_t)data;
		}
	}
	// NVIC_DisableIRQ( USBHS_IRQn );
	UEP_CTRL_LEN( endp ) = len;
	UEP_CTRL_TX( endp ) = ( UEP_CTRL_TX( endp ) & ~USBHS_UEP_T_RES_MASK ) | USBHS_UEP_T_RES_ACK;
	USBHSCTX.USBHS_Endp_Busy[endp] = 1;
	// NVIC_EnableIRQ( USBHS_IRQn );
	return 0;
}

static inline int USBHS_SendACK( int endp, int tx )
{
	if( tx ) UEP_CTRL_TX( endp ) = ( UEP_CTRL_TX( endp ) & ~USBHS_UEP_T_RES_MASK ) | USBHS_UEP_T_RES_ACK;
	else UEP_CTRL_RX(endp) = ( UEP_CTRL_RX(endp) & ~USBHS_UEP_R_RES_MASK ) | USBHS_UEP_R_RES_ACK;
	return 0;
}

static inline int USBHS_SendNAK( int endp, int tx )
{
	if( tx ) UEP_CTRL_TX( endp ) = ( UEP_CTRL_TX( endp ) & ~USBHS_UEP_T_RES_MASK ) | USBHS_UEP_T_RES_NAK;
	else UEP_CTRL_RX(endp) = ( UEP_CTRL_RX(endp) & ~USBHS_UEP_R_RES_MASK ) | USBHS_UEP_R_RES_NAK;
	return 0;
}

#if defined( FUNCONF_USE_USBPRINTF ) && FUNCONF_USE_USBPRINTF
WEAK int HandleInRequest( struct _USBState *ctx, int endp, uint8_t *data, int len )
{
	return 0;
}

WEAK void HandleDataOut( struct _USBState *ctx, int endp, uint8_t *data, int len )
{
	if ( endp == 0 )
	{
		ctx->USBHS_SetupReqLen = 0; // To ACK
	}
}

WEAK int HandleSetupCustom( struct _USBState *ctx, int setup_code )
{
	int ret = -1;
	if ( ctx->USBHS_SetupReqType & USB_REQ_TYP_CLASS )
	{
		switch ( setup_code )
		{
			case CDC_SET_LINE_CODING:
			case CDC_SET_LINE_CTLSTE:
			case CDC_SEND_BREAK: ret = ( ctx->USBHS_SetupReqLen ) ? ctx->USBHS_SetupReqLen : -1; break;
			case CDC_GET_LINE_CODING: ret = ctx->USBHS_SetupReqLen; break;
			default: ret = 0; break;
		}
	}
	else
	{
		ret = 0; // Go to STALL
	}
	return ret;
}
#endif // FUNCONF_USE_USBPRINTF


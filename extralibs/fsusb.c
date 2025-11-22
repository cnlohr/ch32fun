#include "fsusb.h"
#include "ch32fun.h"
#include <string.h>

#ifdef __DMA_SAFE
// CH573 needs all buffers that will touch DMA be allocated at specific memory location
// Since we have EP buffers inside the context struct we put it all there.
// If you want to use your own external buffers, be sure to use this macro before their definitions.
__DMA_SAFE
#endif
struct _USBState USBFSCTX;
volatile uint8_t usb_debug = 0;

#if !defined CH5xx && FUSB_USE_DMA7_COPYs
static inline void copyBuffer( uint8_t * dest, const uint8_t * src, int len )
{
	while( DMA1_Channel7->CNTR );
	DMA1_Channel7->CFGR = 0;
	DMA1_Channel7->MADDR = (uintptr_t)src;
	DMA1_Channel7->PADDR = (uintptr_t)dest;
	DMA1_Channel7->CNTR  = (len+3)/4;
	DMA1_Channel7->CFGR  =
		DMA_M2M_Enable | 
		DMA_DIR_PeripheralDST |
		DMA_Priority_Low |
		DMA_MemoryDataSize_Word |
		DMA_PeripheralDataSize_Word |
		DMA_MemoryInc_Enable |
		DMA_PeripheralInc_Enable |
		DMA_Mode_Normal | DMA_CFGR1_EN;
#if !( FUSB_CURSED_TURBO_DMA == 1 )
	// Somehow, it seems to work (unsafely) without this.
	// Really, though, it's probably fine.
	while( DMA1_Channel7->CNTR );
#endif
}

static inline void copyBufferComplete() { while( DMA1_Channel7->CNTR ); }
#else
#define copyBuffer memcpy
#define copyBufferComplete() ((void)0)
#endif

#if defined (CH5xx)
#define USBFS_IRQHandler USB_IRQHandler
#endif

#if FUSB_USE_HPE
// There is an issue with some registers apparently getting lost with HPE, just do it the slow way.
void USBFS_IRQHandler() __attribute__((section(".text.vector_handler")))  __attribute((interrupt));
// void USBHD_IRQHandler() __attribute__((section(".text.vector_handler")))  __attribute((naked));
#else
#if defined(FUSB_FROM_RAM) && (FUSB_FROM_RAM)
void USBFS_IRQHandler() __USBFS_FUN_ATTRIBUTE __attribute((interrupt));
#else
void USBFS_IRQHandler() __attribute__((section(".text.vector_handler"))) __attribute((interrupt));
#endif
#endif

void USBFS_InternalFinishSetup();

void USBFS_IRQHandler()
{
#if FUSB_IO_PROFILE
	funDigitalWrite( DEBUG_PIN, 1 );
#endif

	// Combined FG + ST flag.
	uint16_t intfgst = *(uint16_t*)(&USBFS->INT_FG);
	int len = 0;
	struct _USBState * ctx = &USBFSCTX;
	uint8_t * ctrl0buff = CTRL0BUFF;
	
	// TODO: Check if needs to be do-while to re-check.
	if( intfgst & CRB_UIF_TRANSFER )
	{
		
		int token = ( intfgst & CMASK_UIS_TOKEN) >> 12;
		int ep = ( intfgst & CMASK_UIS_ENDP ) >> 8;
		if( usb_debug ) printf("[USB] TRANSFER, token = %02x, ep = %d, bmRequestType = %02x, bRequest = %02x\n", token, ep, pUSBFS_SetupReqPak->bmRequestType, pUSBFS_SetupReqPak->bRequest);
		switch ( token )
		{
		case CUIS_TOKEN_IN:
			if( ep )
			{
				if( ep < FUSB_CONFIG_EPS )
				{
#if FUSB_USER_HANDLERS
					len = HandleInRequest( ctx, ep, ctx->ENDPOINTS[ ep ], 0 );
#endif    
					UEP_CTRL_TX(ep) ^= USBFS_UEP_T_TOG;
					if( len )
					{
						if( len < 0 ) len = 0;
						UEP_CTRL_LEN( ep ) = len;
						UEP_CTRL_TX(ep) = ( UEP_CTRL_TX(ep) & ~USBFS_UEP_T_RES_MASK ) | USBFS_UEP_T_RES_ACK;
					}
					else
					{
						UEP_CTRL_TX(ep) = ( UEP_CTRL_TX(ep) & ~USBFS_UEP_T_RES_MASK ) | USBFS_UEP_T_RES_NAK;
					}
					ctx->USBFS_Endp_Busy[ ep ] = 0;
				}
			}
			else
			{
				/* end-point 0 data in interrupt */
				if( ctx->USBFS_SetupReqLen == 0 )
				{
#if defined (CH5xx) || defined (CH32X03x) || defined (CH32V10x)
					UEP_CTRL_TX(0) = USBFS_UEP_R_RES_ACK | USBFS_UEP_R_TOG;
					// R8_UEP0_CTRL = (R8_UEP0_CTRL & ~MASK_UEP_R_RES ) | UEP_R_RES_ACK;
#else
					UEP_CTRL_RX(0) = USBFS_UEP_R_RES_ACK | USBFS_UEP_R_TOG;
#endif
				}

				ctx->USBFS_errata_dont_send_endpoint_in_window = 0;

				if( ctx->pCtrlPayloadPtr )
				{
					// Shortcut mechanism, for descriptors or if the user wants it.
					len = ctx->USBFS_SetupReqLen >= DEF_USBD_UEP0_SIZE ? DEF_USBD_UEP0_SIZE : ctx->USBFS_SetupReqLen;
					copyBuffer( ctrl0buff, ctx->pCtrlPayloadPtr, len ); // FYI -> Would need to do this if using DMA
					ctx->USBFS_SetupReqLen -= len;
					if( ctx->USBFS_SetupReqLen > 0 )
						ctx->pCtrlPayloadPtr += len;
					else
						ctx->pCtrlPayloadPtr = 0;

					UEP_CTRL_LEN(0) = len;
					UEP_CTRL_TX(0) ^= USBFS_UEP_T_TOG;
				}
				else if ( ( ctx->USBFS_SetupReqType & USB_REQ_TYP_MASK ) != USB_REQ_TYP_STANDARD )
				{
					len = ctx->USBFS_SetupReqLen >= DEF_USBD_UEP0_SIZE ? DEF_USBD_UEP0_SIZE : ctx->USBFS_SetupReqLen;
#if FUSB_HID_USER_REPORTS
					if( len && USBFSCTX.USBFS_SetupReqCode == HID_GET_REPORT )
					{
						len = HandleHidUserReportDataIn( ctx, ctrl0buff, len );
						UEP_CTRL_LEN(0) = len;
						UEP_CTRL_TX(0) ^= USBFS_UEP_T_TOG;
						ctx->USBFS_SetupReqLen -= len;
						ctx->pCtrlPayloadPtr += len;
					}	
#endif
#if FUSB_USER_HANDLERS
					if( len && USBFSCTX.USBFS_SetupReqCode != HID_GET_REPORT )
					{
						len = HandleInRequest( ctx, 0, ctrl0buff, len );
						UEP_CTRL_LEN(0) = len;
						UEP_CTRL_TX(0) ^= USBFS_UEP_T_TOG;
						ctx->USBFS_SetupReqLen -= len;
						ctx->pCtrlPayloadPtr += len;
					}
#endif
				}
				else
				{
					switch( USBFSCTX.USBFS_SetupReqCode )
					{
						case USB_GET_DESCRIPTOR:
							break;

						case USB_SET_ADDRESS:
							USBFS->DEV_ADDR = ( USBFS->DEV_ADDR & USBFS_UDA_GP_BIT ) | ctx->USBFS_DevAddr;
							break;

						default:
							break;
					}
				}
			}
			break;

		/* data-out stage processing */
		case CUIS_TOKEN_OUT:
			len = USBFS->RX_LEN;
			switch( ep )
			{
				/* end-point 0 data out interrupt */
				case DEF_UEP0:
					if( intfgst & CRB_UIS_TOG_OK )
					{
#if FUSB_HID_USER_REPORTS
						if( ctx->USBFS_SetupReqCode == HID_SET_REPORT )
						{
							uint8_t * cptr = ctx->pCtrlPayloadPtr;
							if( !cptr )
							{
								HandleHidUserReportDataOut( ctx, ctrl0buff, len );
							}
							else
							{
								int remain = ctx->USBFS_SetupReqLen - len;
								if( remain < 0 )
								{
									len += remain;
									remain = 0;
								}
								copyBuffer( cptr, ctrl0buff, len );
								ctx->USBFS_SetupReqLen = remain;
								if( remain > 0 )
									ctx->pCtrlPayloadPtr = cptr + len;
								else
									ctx->pCtrlPayloadPtr = 0;
							}
						}
#endif
#if FUSB_USER_HANDLERS
						if ( ctx->USBFS_SetupReqCode != HID_SET_REPORT )
						{
							HandleDataOut( ctx, ep, ctrl0buff, len );
						}
#endif
						if( ctx->USBFS_SetupReqLen == 0 )
						{
#if FUSB_HID_USER_REPORTS
							copyBufferComplete();
							if( ctx->USBFS_SetupReqCode == HID_SET_REPORT )
								HandleHidUserReportOutComplete( ctx );
#endif
							// Only set this flag for OUT requests (e.g. SET_LINE_CODING).
							// For IN requests (e.g. GET_DESCRIPTOR), don't set it.
							if ( ( ctx->USBFS_SetupReqType & USB_REQ_TYP_IN ) == 0 )
							{
								ctx->USBFS_errata_dont_send_endpoint_in_window = 1;
							}
							UEP_CTRL_LEN(0) = 0;
							UEP_CTRL_TX(0) = USBFS_UEP_T_TOG | CHECK_USBFS_UEP_T_AUTO_TOG | USBFS_UEP_T_RES_ACK;
						}
						else
						{
#if defined (CH5xx) || defined (CH32X03x) || defined (CH32V10x)
							UEP_CTRL_TX(0) ^= USBFS_UEP_R_TOG;
#else
							UEP_CTRL_RX(0) ^= USBFS_UEP_R_TOG;
#endif
						}
					}
					break;

				default:
#if defined (CH5xx) || defined (CH32X03x) || defined (CH32V10x)
					UEP_CTRL_TX(ep) ^= USBFS_UEP_R_TOG;
#else
					UEP_CTRL_RX(ep) ^= USBFS_UEP_R_TOG;
#endif
#if FUSB_USER_HANDLERS
					HandleDataOut( ctx, ep, ctx->ENDPOINTS[ep], len );
#endif
					break;
			}
			break;

		/* Setup stage processing */
		case CUIS_TOKEN_SETUP:
#if defined (CH5xx) || defined (CH32X03x) || defined (CH32V10x)
#if !defined (CH32V10x)
			if (!(USBFS->INT_ST & 0x80)) goto replycomplete;
#endif
			UEP_CTRL_TX(0) = USBFS_UEP_T_RES_NAK | USBFS_UEP_T_TOG | USBFS_UEP_R_RES_NAK | USBFS_UEP_R_TOG | CHECK_USBFS_UEP_AUTO_TOG;
#else
			UEP_CTRL_TX(0) = USBFS_UEP_T_RES_NAK | CHECK_USBFS_UEP_T_AUTO_TOG | USBFS_UEP_T_TOG;
			UEP_CTRL_RX(0) = USBFS_UEP_R_RES_NAK | CHECK_USBFS_UEP_R_AUTO_TOG | USBFS_UEP_R_TOG;
#endif

			/* Store All Setup Values */
			int USBFS_SetupReqType = USBFSCTX.USBFS_SetupReqType  = pUSBFS_SetupReqPak->bmRequestType;
			int USBFS_SetupReqCode = USBFSCTX.USBFS_SetupReqCode  = pUSBFS_SetupReqPak->bRequest;
			int USBFS_SetupReqLen = USBFSCTX.USBFS_SetupReqLen    = pUSBFS_SetupReqPak->wLength;
			int USBFS_SetupReqIndex = pUSBFS_SetupReqPak->wIndex;
			int USBFS_IndexValue = USBFSCTX.USBFS_IndexValue = ( pUSBFS_SetupReqPak->wIndex << 16 ) | pUSBFS_SetupReqPak->wValue;
			if( usb_debug ) printf( "[USB] SETUP: %02x %02x %02d %02x %04x\n", USBFS_SetupReqType, USBFS_SetupReqCode, USBFS_SetupReqLen, USBFS_SetupReqIndex, USBFS_IndexValue );
			len = 0;

			if( ( USBFS_SetupReqType & USB_REQ_TYP_MASK ) != USB_REQ_TYP_STANDARD )
			{
#if FUSB_HID_INTERFACES > 0  || FUSB_USER_HANDLERS
				if( ( USBFS_SetupReqType & USB_REQ_TYP_MASK ) == USB_REQ_TYP_CLASS )
				{
					/* Class Request */
					switch( USBFS_SetupReqCode )
					{
#if FUSB_HID_INTERFACES > 0
						case HID_SET_REPORT:
#if FUSB_HID_USER_REPORTS
							len = HandleHidUserSetReportSetup( ctx, pUSBFS_SetupReqPak );
							if( len < 0 ) goto sendstall;
							ctx->USBFS_SetupReqLen = len;
							UEP_CTRL_LEN(0) = 0;
							// Previously would have been a CTRL_RX = ACK && TOG, but not here on the 203.
#if defined (CH5xx) || defined (CH32X03x) || defined (CH32V10x)
							UEP_CTRL_TX(0) = USBFS_UEP_R_RES_ACK | USBFS_UEP_R_TOG | CHECK_USBFS_UEP_T_AUTO_TOG | USBFS_UEP_T_TOG;
							// R8_UEP0_CTRL = (R8_UEP0_CTRL & ~MASK_UEP_R_RES) | UEP_R_RES_ACK ;
#else
							UEP_CTRL_RX(0) = CHECK_USBFS_UEP_R_AUTO_TOG | USBFS_UEP_R_RES_ACK | USBFS_UEP_R_TOG;
							UEP_CTRL_TX(0) = CHECK_USBFS_UEP_T_AUTO_TOG | USBFS_UEP_T_TOG;
#endif
							goto replycomplete;
						
						case HID_GET_REPORT:
							len = HandleHidUserGetReportSetup( ctx, pUSBFS_SetupReqPak );
							if( len < 0 ) goto sendstall;
							ctx->USBFS_SetupReqLen = len;
							len = len >= DEF_USBD_UEP0_SIZE ? DEF_USBD_UEP0_SIZE : len;
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
							UEP_CTRL_TX(0) = CHECK_USBFS_UEP_T_AUTO_TOG | USBFS_UEP_T_RES_ACK | USBFS_UEP_T_TOG;
							ctx->USBFS_SetupReqLen -= len;
							goto replycomplete;
#endif
							break;

						case HID_SET_IDLE:
							if( USBFS_SetupReqIndex < FUSB_HID_INTERFACES )
								USBFSCTX.USBFS_HidIdle[ USBFS_SetupReqIndex ] = (uint8_t)( USBFS_IndexValue >> 8 );
							break;
						case HID_SET_PROTOCOL:
							if ( USBFS_SetupReqIndex < FUSB_HID_INTERFACES )
								USBFSCTX.USBFS_HidProtocol[USBFS_SetupReqIndex] = (uint8_t)USBFS_IndexValue;
							break;

						case HID_GET_IDLE:
							if( USBFS_SetupReqIndex < FUSB_HID_INTERFACES )
							{
								ctrl0buff[0] = USBFSCTX.USBFS_HidIdle[ USBFS_SetupReqIndex ];
								len = 1;
							}
							break;

						case HID_GET_PROTOCOL:
							if( USBFS_SetupReqIndex < FUSB_HID_INTERFACES )
							{
								ctrl0buff[0] = USBFSCTX.USBFS_HidProtocol[ USBFS_SetupReqIndex ];
								len = 1;
							}
							break;
#endif
						default:
#if FUSB_USER_HANDLERS
							len = HandleSetupCustom( ctx, USBFS_SetupReqCode );
						
							if( len )
							{
								if( len < 0 ) {
									len = 0;
									ctx->USBFS_SetupReqLen = 0;
								} 
								else
								{
									ctx->USBFS_SetupReqLen = len;
									copyBuffer( ctrl0buff, ctx->pCtrlPayloadPtr, len );
									// printf("%02x-%02x-%02x-%02x-%02x-%02x-%02x\n", ctrl0buff[0], ctrl0buff[1], ctrl0buff[2], ctrl0buff[3], ctrl0buff[4], ctrl0buff[5], ctrl0buff[6]);
									ctx->pCtrlPayloadPtr += len;
								}
								
								if( ctx->USBFS_SetupReqType & DEF_UEP_IN || ctx->USBFS_SetupReqLen == 0)
								{
									len = len >= DEF_USBD_UEP0_SIZE ? DEF_USBD_UEP0_SIZE : len;
									UEP_CTRL_LEN(0) = len;
									UEP_CTRL_TX(0) = USBFS_UEP_T_TOG|USBFS_UEP_T_RES_ACK;
									ctx->USBFS_SetupReqLen -= len;
								}
#if defined (CH5xx) || defined (CH32X03x) || defined (CH32V10x)
								else UEP_CTRL_TX(0)= USBFS_UEP_R_TOG|USBFS_UEP_R_RES_ACK;
#else                
								else UEP_CTRL_RX(0)= USBFS_UEP_R_TOG|USBFS_UEP_R_RES_ACK;
#endif                
								// UEP_CTRL_LEN(0) = len;
								// UEP_CTRL_TX(0) = CHECK_USBFS_UEP_T_AUTO_TOG | USBFS_UEP_T_RES_ACK | USBFS_UEP_T_TOG;
								// ctx->USBFS_SetupReqLen -= len;
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
				switch( USBFS_SetupReqCode )
				{
					/* get device/configuration/string/report/... descriptors */
					case USB_GET_DESCRIPTOR:
					{
						const struct descriptor_list_struct * e = descriptor_list;
						const struct descriptor_list_struct * e_end = e + DESCRIPTOR_LIST_ENTRIES;
						for( ; e != e_end; e++ )
						{
							if( e->lIndexValue == USBFS_IndexValue )
							{
								ctx->pCtrlPayloadPtr = (uint8_t*)e->addr;
								len = e->length;
								break;
							}
						}
						if( e == e_end )
							goto sendstall;
						if( len > USBFS_SetupReqLen )
							len = USBFS_SetupReqLen;
						ctx->USBFS_SetupReqLen = len;
					}

					/* Set usb address */
					case USB_SET_ADDRESS:
						ctx->USBFS_DevAddr = (uint8_t)( ctx->USBFS_IndexValue & 0xFF );
						// NOTE: Do not actually set addres here!  If we do, we won't get the PID_IN associated with this SETUP.
						break;

					/* Get usb configuration now set */
					case USB_GET_CONFIGURATION:
						ctrl0buff[0] = ctx->USBFS_DevConfig;
						if( ctx->USBFS_SetupReqLen > 1 )
							ctx->USBFS_SetupReqLen = 1;
						break;

					/* Set usb configuration to use */
					case USB_SET_CONFIGURATION:
						ctx->USBFS_DevConfig = (uint8_t)( ctx->USBFS_IndexValue & 0xFF );
						ctx->USBFS_DevEnumStatus = 0x01;
						break;

					/* Clear or disable one usb feature */
					case USB_CLEAR_FEATURE:
#if FUSB_SUPPORTS_SLEEP
						if( ( USBFS_SetupReqType & USB_REQ_RECIP_MASK ) == USB_REQ_RECIP_DEVICE )
						{
							/* clear one device feature */
							if( (uint8_t)( USBFS_IndexValue & 0xFF ) == USB_REQ_FEAT_REMOTE_WAKEUP )
							{
								/* clear usb sleep status, device not prepare to sleep */
								ctx->USBFS_DevSleepStatus &= ~0x01;
							}
							else
							{
								goto sendstall;
							}
						}
						else
#endif
						if( ( USBFS_SetupReqType & USB_REQ_RECIP_MASK ) == USB_REQ_RECIP_ENDP )
						{
							if( (uint8_t)( USBFS_IndexValue & 0xFF ) == USB_REQ_FEAT_ENDP_HALT )
							{
								/* Clear End-point Feature */
								if( ep < FUSB_CONFIG_EPS ) 
								{
									// UEP_CTRL_TX(ep) = USBFS_UEP_T_RES_STALL | CHECK_USBFS_UEP_T_AUTO_TOG;
									if( USBFS_SetupReqIndex & DEF_UEP_IN  && ctx->endpoint_mode[ep] == -1 ) UEP_CTRL_TX(ep) = USBFS_UEP_T_RES_NAK;
#if defined (CH5xx) || defined (CH32X03x) || defined (CH32V10x)
									else if( USBFS_SetupReqIndex & DEF_UEP_OUT && ctx->endpoint_mode[ep] == 1 ) UEP_CTRL_TX(ep) = USBFS_UEP_R_RES_ACK;
#else
									else if( USBFS_SetupReqIndex & DEF_UEP_OUT && ctx->endpoint_mode[ep] == 1 ) UEP_CTRL_RX(ep) = USBFS_UEP_R_RES_ACK;
#endif
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
						}
						else
						{
							goto sendstall;
						}
						break;

					/* set or enable one usb feature */
					case USB_SET_FEATURE:
						if( ( USBFS_SetupReqType & USB_REQ_RECIP_MASK ) == USB_REQ_RECIP_DEVICE )
						{
#if FUSB_SUPPORTS_SLEEP
							/* Set Device Feature */
							if( (uint8_t)( USBFS_IndexValue & 0xFF ) == USB_REQ_FEAT_REMOTE_WAKEUP )
							{
								/* Set Wake-up flag, device prepare to sleep */
								USBFS_DevSleepStatus |= 0x01;
							}
							else
#endif
							{
								goto sendstall;
							}
						}
						else if( ( USBFS_SetupReqType & USB_REQ_RECIP_MASK ) == USB_REQ_RECIP_ENDP )
						{
							/* Set Endpoint Feature */
							if( (uint8_t)( USBFS_IndexValue & 0xFF ) == USB_REQ_FEAT_ENDP_HALT )
							{
								if( ep < FUSB_CONFIG_EPS )
								{
									if( (USBFS_SetupReqIndex & DEF_UEP_IN) && ctx->endpoint_mode[ep] == -1 )UEP_CTRL_TX(ep) = ( UEP_CTRL_TX(ep) & ~USBFS_UEP_T_RES_MASK ) | USBFS_UEP_T_RES_STALL;
#if defined (CH5xx) || defined (CH32X03x) || defined (CH32V10x)
									else if( (USBFS_SetupReqIndex & DEF_UEP_OUT) && ctx->endpoint_mode[ep] == 1 )UEP_CTRL_TX(ep) = ( UEP_CTRL_TX(ep) & ~USBFS_UEP_R_RES_MASK ) | USBFS_UEP_R_RES_STALL;
#else
									else if( (USBFS_SetupReqIndex & DEF_UEP_OUT) && ctx->endpoint_mode[ep] == 1 )UEP_CTRL_RX(ep) = ( UEP_CTRL_RX(ep) & ~USBFS_UEP_R_RES_MASK ) | USBFS_UEP_R_RES_STALL;
#endif
									else goto sendstall;
								}
							}
							else
								goto sendstall;
						}
						else
							goto sendstall;
						break;

					/* This request allows the host to select another setting for the specified interface  */
					case USB_GET_INTERFACE:
						ctrl0buff[0] = 0x00;
						if( USBFS_SetupReqLen > 1 ) USBFS_SetupReqLen = 1;
						break;

					case USB_SET_INTERFACE:
						break;

					/* host get status of specified device/interface/end-points */
					case USB_GET_STATUS:
						ctrl0buff[0] = 0x00;
						ctrl0buff[1] = 0x00;
						if( ( USBFS_SetupReqType & USB_REQ_RECIP_MASK ) == USB_REQ_RECIP_DEVICE )
						{
#if FUSB_SUPPORTS_SLEEP
							ctrl0buff[0] = (ctx->USBFS_DevSleepStatus & 0x01)<<1;
#else
							ctrl0buff[0] = 0x00;
#endif
						}
						else if( ( USBFS_SetupReqType & USB_REQ_RECIP_MASK ) == USB_REQ_RECIP_ENDP )
						{
							if( ep < FUSB_CONFIG_EPS )
							{
								if( USBFS_SetupReqIndex & DEF_UEP_IN && ctx->endpoint_mode[ep] == -1 ) ctrl0buff[0] = ( UEP_CTRL_TX(ep) & USBFS_UEP_T_RES_MASK ) == USBFS_UEP_T_RES_STALL;
#if defined (CH5xx) || defined (CH32X03x) || defined (CH32V10x)
								else if( USBFS_SetupReqIndex & DEF_UEP_OUT && ctx->endpoint_mode[ep] == 1 ) ctrl0buff[0] = ( UEP_CTRL_TX(ep) & USBFS_UEP_R_RES_MASK ) == USBFS_UEP_R_RES_STALL;
#else
								else if( USBFS_SetupReqIndex & DEF_UEP_OUT && ctx->endpoint_mode[ep] == 1 ) ctrl0buff[0] = ( UEP_CTRL_TX(ep) & USBFS_UEP_R_RES_MASK ) == USBFS_UEP_R_RES_STALL;
#endif
								else goto sendstall;
							}
								
							else
								goto sendstall;
						}
						else
							goto sendstall;
						if( USBFS_SetupReqLen > 2 )
							USBFS_SetupReqLen = 2;
						break;

					default:
						goto sendstall;
						break;
				}
			}
		// epzero_rxtx:
			{
				/* end-point 0 data Tx/Rx */
				if( USBFS_SetupReqType & DEF_UEP_IN )
				{
					len = ( USBFS_SetupReqLen > DEF_USBD_UEP0_SIZE )? DEF_USBD_UEP0_SIZE : USBFS_SetupReqLen;
					USBFS_SetupReqLen -= len;
					UEP_CTRL_LEN(0) = len;
					UEP_CTRL_TX(0) = USBFS_UEP_T_RES_ACK;
					// R8_UEP0_CTRL = (R8_UEP0_CTRL & ~MASK_UEP_T_RES) | UEP_T_RES_ACK;
				}
				else
				{
					if( USBFS_SetupReqLen == 0 )
					{
						UEP_CTRL_LEN(0) = 0;
						UEP_CTRL_TX(0) = USBFS_UEP_T_RES_ACK | USBFS_UEP_T_TOG;
						// R8_UEP0_CTRL = (R8_UEP0_CTRL & ~MASK_UEP_T_RES) | UEP_T_RES_ACK;
					}
					else
					{
#if defined (CH5xx) || defined (CH32X03x) || defined (CH32V10x)
						UEP_CTRL_TX(0) = CHECK_USBFS_UEP_T_AUTO_TOG | USBFS_UEP_R_RES_ACK | USBFS_UEP_R_TOG;
						// R8_UEP0_CTRL = (R8_UEP0_CTRL & ~MASK_UEP_R_RES) | UEP_R_RES_ACK ;
#else
						UEP_CTRL_RX(0) = CHECK_USBFS_UEP_R_AUTO_TOG | USBFS_UEP_R_RES_ACK | USBFS_UEP_R_TOG;
#endif
					}
				}
			}
			break;

			// This might look a little weird, for error handling but it saves a nontrivial amount of storage, and simplifies
			// control flow to hard-abort here.
		sendstall:
			// if one request not support, return stall.  Stall means permanent error.
#if defined (CH5xx) || defined (CH32X03x) || defined (CH32V10x)
			UEP_CTRL_TX(0) = USBFS_UEP_T_TOG | USBFS_UEP_T_RES_STALL | USBFS_UEP_R_TOG | USBFS_UEP_R_RES_STALL;
#else
			UEP_CTRL_TX(0) = USBFS_UEP_T_TOG | USBFS_UEP_T_RES_STALL;
			UEP_CTRL_RX(0) = USBFS_UEP_R_TOG | USBFS_UEP_R_RES_STALL;
#endif
		replycomplete:
			break;

		/* Sof pack processing */
		//case CUIS_TOKEN_SOF:
		//	break;

		default :
			break;
		}
		// printf("clear transfer int flag\n");
		USBFS->INT_FG = CRB_UIF_TRANSFER;
	}
	else if( intfgst & CRB_UIF_BUS_RST )
	{
		if( usb_debug ) printf("[USB] RESET\n");
		/* usb reset interrupt processing */
		ctx->USBFS_DevConfig = 0;
		ctx->USBFS_DevAddr = 0;
		ctx->USBFS_DevSleepStatus = 0;
		ctx->USBFS_DevEnumStatus = 0;

		USBFS->DEV_ADDR = 0;
		USBFS_InternalFinishSetup();
		USBFS->INT_FG = CRB_UIF_BUS_RST;
	}
	else if( intfgst & CRB_UIF_SUSPEND )
	{
		if( usb_debug ) printf("[USB] SUSPEND\n");
		USBFS->INT_FG = USBFS_UMS_SUSPEND;
		Delay_Us(10);
		/* usb suspend interrupt processing */
		if( USBFS->MIS_ST & USBFS_UMS_SUSPEND )
		{
			ctx->USBFS_DevSleepStatus |= 0x02;
			if( ctx->USBFS_DevSleepStatus == 0x03 )
			{
				/* Handling usb sleep here */
				//TODO: MCU_Sleep_Wakeup_Operate( );
			}
		}
		else
		{
			ctx->USBFS_DevSleepStatus &= ~0x02;
		}
		
	}
	else
	{
			/* other interrupts */
			USBFS->INT_FG = intfgst & 0xff;
			if( usb_debug) printf("[USB] intfgst = %04x\n",intfgst);
	}

#if FUSB_IO_PROFILE
	funDigitalWrite( DEBUG_PIN, 0 );
#endif

#if FUSB_USE_HPE
	// asm volatile( "mret" );
#endif
}

void USBFS_InternalFinishSetup()
{

#if FUSB_EP1_MODE
	USBFSCTX.endpoint_mode[1] = FUSB_EP1_MODE;
#if FUSB_EP1_MODE > 0 
	USBFS->UEP4_1_MOD = USBFS_UEP1_TX_EN;
#else
	USBFS->UEP4_1_MOD = USBFS_UEP1_RX_EN;
#endif
#endif
#if FUSB_EP4_MODE
	USBFSCTX.endpoint_mode[4] = FUSB_EP4_MODE;
#if FUSB_EP4_MODE > 0 
	USBFS->UEP4_1_MOD |= USBFS_UEP4_TX_EN;
#else
	USBFS->UEP4_1_MOD |= USBFS_UEP4_RX_EN;
#endif
#endif

#if FUSB_EP2_MODE
	USBFSCTX.endpoint_mode[2] = FUSB_EP2_MODE;
#if FUSB_EP2_MODE > 0 
	USBFS->UEP2_3_MOD = USBFS_UEP2_TX_EN;
#else
	USBFS->UEP2_3_MOD = USBFS_UEP2_RX_EN;
#endif
#endif
#if FUSB_EP3_MODE
	USBFSCTX.endpoint_mode[3] = FUSB_EP3_MODE;
#if FUSB_EP3_MODE > 0 
	USBFS->UEP2_3_MOD |= USBFS_UEP3_TX_EN;
#else
	USBFS->UEP2_3_MOD |= USBFS_UEP3_RX_EN;
#endif
#endif

#if FUSB_EP5_MODE
	USBFSCTX.endpoint_mode[5] = FUSB_EP5_MODE;
#if FUSB_EP5_MODE > 0
#if defined (CH5xx) || defined (CH32X03x)
	USBFS->UEP567_MOD = USBFS_UEP5_TX_EN;
#else
	USBFS->UEP5_6_MOD = USBFS_UEP5_TX_EN;
#endif
#else
#if defined (CH5xx) || defined (CH32X03x)
	USBFS->UEP567_MOD = USBFS_UEP5_RX_EN;
#else
	USBFS->UEP5_6_MOD = USBFS_UEP5_RX_EN;
#endif
#endif
#endif
#if FUSB_EP6_MODE
	USBFSCTX.endpoint_mode[6] = FUSB_EP6_MODE;
#if FUSB_EP6_MODE > 0 
#if defined (CH5xx) || defined (CH32X03x)
	USBFS->UEP567_MOD = USBFS_UEP6_TX_EN;
#else
	USBFS->UEP5_6_MOD |= USBFS_UEP6_TX_EN;
#endif
#else
#if defined (CH5xx) || defined (CH32X03x)
	USBFS->UEP567_MOD |= USBFS_UEP6_RX_EN;
#else
	USBFS->UEP5_6_MOD |= USBFS_UEP6_RX_EN;
#endif
#endif
#endif

#if FUSB_EP7_MODE
	USBFSCTX.endpoint_mode[7] = FUSB_EP7_MODE;
#if FUSB_EP7_MODE > 0
#if defined (CH5xx) || defined (CH32X03x)
	USBFS->UEP567_MOD |= USBFS_UEP7_TX_EN;
#else
	USBFS->UEP7_MOD = USBFS_UEP1_TX_EN;
#endif
#else
#if defined (CH5xx) || defined (CH32X03x)
	USBFS->UEP567_MOD |= USBFS_UEP7_RX_EN;
#else
	USBFS->UEP7_MOD = USBFS_UEP1_RX_EN;
#endif
#endif
#endif

#if !defined (FUSB_CONFIG_EPS) || !FUSB_CONFIG_EPS
#error You must have at least EP0!
#endif

	for( int i = 0; i < FUSB_CONFIG_EPS; i++ )
	{
		UEP_DMA(i) = (uintptr_t)USBFSCTX.ENDPOINTS[i];
	}
	
#if defined (CH5xx) || defined (CH32X03x) || defined (CH32V10x)
	UEP_CTRL_TX(0) = USBFS_UEP_T_RES_NAK | USBFS_UEP_R_RES_ACK | CHECK_USBFS_UEP_T_AUTO_TOG;
#else
	UEP_CTRL_TX(0) = USBFS_UEP_T_RES_NAK | CHECK_USBFS_UEP_T_AUTO_TOG;
	UEP_CTRL_RX(0) = USBFS_UEP_R_RES_ACK | CHECK_USBFS_UEP_R_AUTO_TOG;
#endif

	for( int i = 1; i < FUSB_CONFIG_EPS; i++ )
	{
		if( USBFSCTX.endpoint_mode[i] > 0 )
		{
			UEP_CTRL_TX(i) = USBFS_UEP_T_RES_NAK;
		}
		else if( USBFSCTX.endpoint_mode[i] < 0 )
		{
#if defined (CH5xx) || defined (CH32X03x) || defined (CH32V10x)
			UEP_CTRL_TX(i) = USBFS_UEP_R_RES_ACK;
#else
			UEP_CTRL_RX(i) = USBFS_UEP_R_RES_ACK;
#endif
		}
		USBFSCTX.USBFS_Endp_Busy[i] = 0;
	}
}

int USBFSSetup()
{
#if defined (CH32V10x)
#if (FUNCONF_SYSTEM_CORE_CLOCK != 72000000) && (FUNCONF_SYSTEM_CORE_CLOCK != 48000000)
#error CH32V103 needs 72MHz or 48MHz main clock for USB to work
#endif
#if FUNCONF_SYSTEM_CORE_CLOCK == 48000000
	RCC->CFGR0 |= RCC_USBPRE;  // Disable 1.5 divider for USB clock
#endif
#if defined (FUSB_VDD_5V) && FUSB_VDD_5V
	EXTEN->EXTEN_CTR |= EXTEN_USB_5V_SEL;
#endif
	EXTEN->EXTEN_CTR |= EXTEN_USBFS_IO_EN;
#endif

#if defined (CH32V20x) || defined (CH32V30x) || defined(CH32L103)
#ifdef CH32V30x_D8C
	RCC->CFGR2 = RCC_USBHSSRC | RCC_USBHSPLL | 1<< RCC_USBHSCLK_OFFSET | RCC_USBHSPLLSRC | 1 << RCC_USBHSDIV_OFFSET;
	RCC->AHBPCENR |= RCC_USBHSEN;
#else
	// USBPRE[1:0] = 10: Divided by 3 (when PLLCLK=144MHz);
	// Must be done before enabling clock to USBFS tree.
#if FUNCONF_SYSTEM_CORE_CLOCK == 144000000
	RCC->CFGR0 = (RCC->CFGR0 & ~(3<<22)) | (2<<22);
#elif FUNCONF_SYSTEM_CORE_CLOCK == 96000000
	RCC->CFGR0 = (RCC->CFGR0 & ~(3<<22)) | (1<<22);
#elif FUNCONF_SYSTEM_CORE_CLOCK == 48000000
	RCC->CFGR0 = (RCC->CFGR0 & ~(3<<22));
#elif FUNCONF_SYSTEM_CORE_CLOCK == 240000000
#error CH32V20x/30x is unstable at 240MHz
#else
#error CH32V20x/30x need 144/96/48MHz main clock for USB to work
#endif
#endif
#endif

#if defined (CH32X03x)
	RCC->APB2PCENR |= RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOC;
// #ifdef FUSB_VDD_5V
	// AFIO->CTLR = (AFIO->CTLR & ~(UDP_PUE_MASK | UDM_PUE_MASK | USB_PHY_V33)) | UDP_PUE_10K | USB_IOEN;
// #else
	AFIO->CTLR = (AFIO->CTLR & ~(UDP_PUE_MASK | UDM_PUE_MASK )) | USB_PHY_V33 | UDP_PUE_1K5 | USB_IOEN;
// #endif
	// Enable PC16/17 Alternate Function (USB)
	// According to EVT, GPIO16 = GPIO_Mode_IN_FLOATING, GPIO17 = GPIO_Mode_IPU
	GPIOC->CFGXR = 	( GPIOC->CFGXR & ~( (0xf<<(4*0)) | (0xf<<(4*1)) ) )  |
					(((GPIO_CFGLR_IN_FLOAT)<<(4*0)) | (((GPIO_CFGLR_IN_PUPD)<<(4*1)))); // MSBs are CNF, LSBs are MODE
	GPIOC->BSXR = 1<<1; // PC17 on.
#endif

#if defined (CH5xx)
#if defined (CH570_CH572)
	R16_PIN_ALTERNATE |= RB_PIN_USB_EN | RB_UDP_PU_EN;
#elif defined (CH584_CH585)
  R16_PIN_CONFIG |= RB_PIN_USB_EN | RB_UDP_PU_EN;
#else
	R16_PIN_ANALOG_IE |= RB_PIN_USB_IE | RB_PIN_USB_DP_PU;
#endif

#else

#if defined (CH32V10x) || defined (CH32V30x) || defined(CH32L103)
	RCC->APB2PCENR |= RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOA;
#endif

#if defined (CH32V20x)
	RCC->APB2PCENR |= RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOB;
#endif

	RCC->AHBPCENR |= RCC_USBFS | RCC_AHBPeriph_DMA1;
	
#endif

	// Force module to reset.
	USBFS->BASE_CTRL = USBFS_UC_RESET_SIE | USBFS_UC_CLR_ALL;
	USBFS->BASE_CTRL = 0x00;
	Delay_Us(10);

	// Enter device mode.
	USBFS->INT_EN    = USBFS_UIE_SUSPEND | USBFS_UIE_TRANSFER | USBFS_UIE_BUS_RST;
	USBFS->DEV_ADDR  = 0x00;
	USBFS->BASE_CTRL = USBFS_UC_INT_BUSY | USBFS_UC_DMA_EN | USBFS_UC_DEV_PU_EN;
	USBFS->INT_FG    = 0xff;
	USBFS_InternalFinishSetup();
	USBFS->UDEV_CTRL = USBFS_UD_PD_DIS | USBFS_UD_PORT_EN;

	NVIC_EnableIRQ( USB_IRQn );

#if defined (CH32V30x)
	USBFS->OTG_CR = 0; //Note says only valid on 305, 307. 
#endif

// #ifndef CH32X03x
// 	// Actually go on-bus.
// 	USBFS->BASE_CTRL |= USBFS_UC_DEV_PU_EN;
// #endif

#if FUSB_IO_PROFILE
	funPinMode( DEBUG_PIN, GPIO_CFGLR_OUT_50Mhz_PP );
#endif

	// Go on-bus.
	return 0;
}

#ifdef CH5xx
void USBFSReset()
{
	NVIC_DisableIRQ( USB_IRQn );
#if defined (CH570_CH572)
	R16_PIN_ALTERNATE &= ~(RB_PIN_USB_EN | RB_UDP_PU_EN);
#elif defined (CH584_CH585)
	R16_PIN_CONFIG &= ~(RB_PIN_USB_EN | RB_UDP_PU_EN);
#else
	R16_PIN_ANALOG_IE &= ~(RB_PIN_USB_IE | RB_PIN_USB_DP_PU);
#endif
	USBFS->BASE_CTRL = USBFS_UC_RESET_SIE | USBFS_UC_CLR_ALL;
	USBFS->BASE_CTRL = 0x00;
	Delay_Us(10);
}
#endif

static inline uint8_t * USBFS_GetEPBufferIfAvailable( int endp )
{
	if( USBFSCTX.USBFS_Endp_Busy[ endp ] ) return 0;
	return USBFSCTX.ENDPOINTS[ endp ];
}

static inline int USBFS_SendEndpoint( int endp, int len )
{
	if( USBFSCTX.USBFS_errata_dont_send_endpoint_in_window || USBFSCTX.USBFS_Endp_Busy[ endp ] ) return -1;
	// This prevents sending while ep0 is receiving
	if( USBFSCTX.USBFS_SetupReqLen > 0 ) return -2;
#if defined (CH5xx) || defined (CH32X03x)
	// Check RB_UIS_SETUP_ACT
	if( (USBFS->INT_ST & 0x80) ) return -3;
#endif
	NVIC_DisableIRQ( USB_IRQn );
	UEP_CTRL_LEN( endp ) = len;
	UEP_CTRL_TX( endp ) = ( UEP_CTRL_TX( endp ) & ~USBFS_UEP_T_RES_MASK ) | USBFS_UEP_T_RES_ACK;
	USBFSCTX.USBFS_Endp_Busy[ endp ] = 1;
	NVIC_EnableIRQ( USB_IRQn );
	return 0;
}

int USBFS_SendEndpointNEW( int endp, uint8_t* data, int len, int copy)
{
	if( USBFSCTX.USBFS_errata_dont_send_endpoint_in_window || USBFSCTX.USBFS_Endp_Busy[ endp ] ) return -1;
	// This prevents sending while ep0 is receiving
	if( USBFSCTX.USBFS_SetupReqLen > 0 ) return USBFSCTX.USBFS_SetupReqLen;
#if defined (CH5xx) || defined (CH32X03x)
	// Check RB_UIS_SETUP_ACT
	if( (USBFS->INT_ST & 0x80) ) return -3;
#endif
	if ( len )
	{
		if( copy )
		{
			UEP_DMA( endp ) = (uintptr_t)USBFSCTX.ENDPOINTS[endp];
			copyBuffer( USBFSCTX.ENDPOINTS[endp], data, len );
			copyBufferComplete();
		}
		else UEP_DMA( endp ) = (uintptr_t)data;
	}
	// NVIC_DisableIRQ( USB_IRQn );
	UEP_CTRL_LEN( endp ) = len;
	UEP_CTRL_TX( endp ) = ( UEP_CTRL_TX( endp ) & ~USBFS_UEP_T_RES_MASK ) | USBFS_UEP_T_RES_ACK;
	USBFSCTX.USBFS_Endp_Busy[ endp ] = 1;
	// NVIC_EnableIRQ( USB_IRQn );
	return 0;
}

static inline int USBFS_SendACK( int endp, int tx )
{
	if( tx ) UEP_CTRL_TX( endp ) = ( UEP_CTRL_TX( endp ) & ~USBFS_UEP_T_RES_MASK ) | USBFS_UEP_T_RES_ACK;
#if defined (CH5xx) || defined (CH32X03x) || defined (CH32V10x)
	else UEP_CTRL_TX(endp) = ( UEP_CTRL_TX(endp) & ~USBFS_UEP_R_RES_MASK ) | USBFS_UEP_R_RES_ACK;
#else
	else UEP_CTRL_RX(endp) = ( UEP_CTRL_RX(endp) & ~USBFS_UEP_R_RES_MASK ) | USBFS_UEP_R_RES_ACK;
#endif
	return 0;
}

static inline int USBFS_SendNAK( int endp, int tx )
{
	if( tx ) UEP_CTRL_TX( endp ) = ( UEP_CTRL_TX( endp ) & ~USBFS_UEP_T_RES_MASK ) | USBFS_UEP_T_RES_NAK;
#if defined (CH5xx) || defined (CH32X03x) || defined (CH32V10x)
	else UEP_CTRL_TX(endp) = ( UEP_CTRL_TX(endp) & ~USBFS_UEP_R_RES_MASK ) | USBFS_UEP_R_RES_NAK;
#else
	else UEP_CTRL_RX(endp) = ( UEP_CTRL_RX(endp) & ~USBFS_UEP_R_RES_MASK ) | USBFS_UEP_R_RES_NAK;
#endif
	return 0;
}

#if defined( FUNCONF_USE_USBPRINTF ) && FUNCONF_USE_USBPRINTF
int HandleInRequest( struct _USBState *ctx, int endp, uint8_t *data, int len )
{
	return 0;
}

void HandleDataOut( struct _USBState *ctx, int endp, uint8_t *data, int len )
{
	if ( endp == 0 )
	{
		ctx->USBFS_SetupReqLen = 0; // To ACK
	}
}

int HandleSetupCustom( struct _USBState *ctx, int setup_code )
{
	int ret = -1;
	if ( ctx->USBFS_SetupReqType & USB_REQ_TYP_CLASS )
	{
		switch ( setup_code )
		{
			case CDC_SET_LINE_CODING:
			case CDC_SET_LINE_CTLSTE:
			case CDC_SEND_BREAK: ret = ( ctx->USBFS_SetupReqLen ) ? ctx->USBFS_SetupReqLen : -1; break;
			case CDC_GET_LINE_CODING: ret = ctx->USBFS_SetupReqLen; break;
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


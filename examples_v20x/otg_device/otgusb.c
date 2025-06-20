#include "otgusb.h"
#include "ch32fun.h"
#include <string.h>

#define UEP_CTRL_LEN(n) (((uint16_t*)&USBOTG_FS->UEP0_TX_LEN)[n*2])
#define UEP_CTRL_TX(n)  (((uint8_t*)&USBOTG_FS->UEP0_TX_CTRL)[n*4])
#define UEP_CTRL_RX(n)  (((uint8_t*)&USBOTG_FS->UEP0_RX_CTRL)[n*4])

#define CHECK_USBOTG_UEP_T_AUTO_TOG USBOTG_UEP_T_AUTO_TOG
#define CHECK_USBOTG_UEP_R_AUTO_TOG USBOTG_UEP_R_AUTO_TOG
struct _USBState USBOTGCTX;

// Mask for the combined USBFSD->INT_FG + USBFSD->INT_ST
#define CRB_UIF_ISO_ACT   (1<<6)
#define CRB_UIF_SETUP_ACT (1<<5)
#define CRB_UIF_FIFO_OV   (1<<4)
#define CRB_UIF_HST_SOF   (1<<3)
#define CRB_UIF_SUSPEND   (1<<2)
#define CRB_UIF_TRANSFER  (1<<1)
#define CRB_UIF_BUS_RST   (1<<0)
#define CRB_UIS_IS_NAK    (1<<15)
#define CRB_UIS_TOG_OK    (1<<14)
#define CMASK_UIS_TOKEN   (3<<12)
#define CMASK_UIS_ENDP    (0xf<<8)

#define CUIS_TOKEN_OUT	   0x0
#define CUIS_TOKEN_SOF     0x1
#define CUIS_TOKEN_IN      0x2
#define CUIS_TOKEN_SETUP   0x3

static inline void DMA7FastCopy( uint8_t * dest, const uint8_t * src, int len )
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

static inline void DMA7FastCopyComplete() { while( DMA1_Channel7->CNTR ); }

#if FUSB_USE_HPE
// There is an issue with some registers apparently getting lost with HPE, just do it the slow way.
void USBFS_IRQHandler() __attribute__((section(".text.vector_handler")))  __attribute((interrupt));
//void USBHD_IRQHandler() __attribute__((section(".text.vector_handler")))  __attribute((naked));
#else
void USBFS_IRQHandler() __attribute__((section(".text.vector_handler")))  __attribute((interrupt));
#endif

void USBOTG_InternalFinishSetup();

void USBFS_IRQHandler()
{
#if FUSB_IO_PROFILE
	funDigitalWrite( PB0, 1 );
#endif

	// Combined FG + ST flag.
	uint16_t intfgst = *(uint16_t*)(&USBOTG_FS->INT_FG);
	int len = 0;
	struct _USBState * ctx = &USBOTGCTX;
	uint8_t * ctrl0buff = CTRL0BUFF;

	// TODO: Check if needs to be do-while to re-check.
	if( intfgst & CRB_UIF_TRANSFER )
	{
		int token = ( intfgst & CMASK_UIS_TOKEN) >> 12;
		int ep = ( intfgst & CMASK_UIS_ENDP ) >> 8;

		switch ( token )
		{
		case CUIS_TOKEN_IN:
			if( ep )
			{
				if( ep < FUSB_CONFIG_EPS )
				{
					UEP_CTRL_TX(ep) = ( UEP_CTRL_TX(ep) & ~USBOTG_UEP_T_RES_MASK ) | USBOTG_UEP_T_RES_NAK;
					UEP_CTRL_TX(ep) ^= USBOTG_UEP_T_TOG;
					ctx->USBOTG_Endp_Busy[ ep ] = 0;
				}
			}
			else
			{
				/* end-point 0 data in interrupt */
				if( ctx->USBOTG_SetupReqLen == 0 )
				{
					UEP_CTRL_RX(0) = USBOTG_UEP_R_RES_ACK | USBOTG_UEP_R_TOG;
				}

				ctx->USBOTG_errata_dont_send_endpoint_in_window = 0;

				if( ctx->pCtrlPayloadPtr )
				{
					// Shortcut mechanism, for descriptors or if the user wants it.
					len = ctx->USBOTG_SetupReqLen >= DEF_USBD_UEP0_SIZE ? DEF_USBD_UEP0_SIZE : ctx->USBOTG_SetupReqLen;
					DMA7FastCopy( ctrl0buff, ctx->pCtrlPayloadPtr, len ); // FYI -> Would need to do this if using DMA
					ctx->USBOTG_SetupReqLen -= len;
					if( ctx->USBOTG_SetupReqLen > 0 )
						ctx->pCtrlPayloadPtr += len;
					else
						ctx->pCtrlPayloadPtr = 0;

					UEP_CTRL_LEN(0) = len;
					UEP_CTRL_TX(0) ^= USBOTG_UEP_T_TOG;
				}
				else if ( ( ctx->USBOTG_SetupReqType & USB_REQ_TYP_MASK ) != USB_REQ_TYP_STANDARD )
				{

#if FUSB_HID_USER_REPORTS
					len = ctx->USBOTG_SetupReqLen >= DEF_USBD_UEP0_SIZE ? DEF_USBD_UEP0_SIZE : ctx->USBOTG_SetupReqLen;
					if( len && USBOTGCTX.USBOTG_SetupReqCode == HID_GET_REPORT )
					{
						len = HandleHidUserReportDataIn( ctx, ctrl0buff, len );
						UEP_CTRL_LEN(0) = len;
						UEP_CTRL_TX(0) ^= USBOTG_UEP_T_TOG;
						ctx->USBOTG_SetupReqLen -= len;
						ctx->pCtrlPayloadPtr += len;
					}	
#endif
				}
				else
				{
					switch( USBOTGCTX.USBOTG_SetupReqCode )
					{
						case USB_GET_DESCRIPTOR:
							break;

						case USB_SET_ADDRESS:
							USBOTG_FS->DEV_ADDR = ( USBOTG_FS->DEV_ADDR & USBOTG_UDA_GP_BIT ) | ctx->USBOTG_DevAddr;
							break;

						default:
							break;
					}
				}
			}
			break;

		/* data-out stage processing */
		case CUIS_TOKEN_OUT:
			switch( ep )
			{
				/* end-point 0 data out interrupt */
				case DEF_UEP0:
					if( intfgst & CRB_UIS_TOG_OK )
					{
#if FUSB_HID_USER_REPORTS
						int len = USBOTG_FS->RX_LEN;
						uint8_t * cptr = ctx->pCtrlPayloadPtr;
						if( !cptr )
						{
							HandleHidUserReportDataOut( ctx, ctrl0buff, len );
						}
						else
						{
							int remain = ctx->USBOTG_SetupReqLen - len;
							if( remain < 0 )
							{
								len += remain;
								remain = 0;
							}
							DMA7FastCopy( cptr, ctrl0buff, len );
							ctx->USBOTG_SetupReqLen = remain;
							if( remain > 0 )
								ctx->pCtrlPayloadPtr = cptr + len;
							else
								ctx->pCtrlPayloadPtr = 0;
						}
#endif

						if( ctx->USBOTG_SetupReqLen == 0 )
						{
#if FUSB_HID_USER_REPORTS
							DMA7FastCopyComplete();
							HandleHidUserReportOutComplete( ctx );
#endif

							ctx->USBOTG_errata_dont_send_endpoint_in_window = 1;
							UEP_CTRL_LEN(0) = 0;
							UEP_CTRL_TX(0) = USBOTG_UEP_T_TOG | CHECK_USBOTG_UEP_T_AUTO_TOG | USBOTG_UEP_T_RES_ACK;
						}
						else
						{
							UEP_CTRL_RX(0) ^= USBOTG_UEP_R_TOG;
						}
					}
					break;

				default:
					break;
			}
			break;

		/* Setup stage processing */
		case CUIS_TOKEN_SETUP:
			UEP_CTRL_TX(0) = USBOTG_UEP_T_RES_NAK | CHECK_USBOTG_UEP_T_AUTO_TOG | USBOTG_UEP_T_TOG;
			UEP_CTRL_RX(0) = USBOTG_UEP_R_RES_NAK | CHECK_USBOTG_UEP_R_AUTO_TOG | USBOTG_UEP_R_TOG;

			/* Store All Setup Values */
			int USBOTG_SetupReqType = USBOTGCTX.USBOTG_SetupReqType  = pUSBOTG_SetupReqPak->bmRequestType;
			int USBOTG_SetupReqCode = USBOTGCTX.USBOTG_SetupReqCode  = pUSBOTG_SetupReqPak->bRequest;
			int USBOTG_SetupReqLen = USBOTGCTX.USBOTG_SetupReqLen    = pUSBOTG_SetupReqPak->wLength;
			int USBOTG_SetupReqIndex = pUSBOTG_SetupReqPak->wIndex;
			int USBOTG_IndexValue = USBOTGCTX.USBOTG_IndexValue = ( pUSBOTG_SetupReqPak->wIndex << 16 ) | pUSBOTG_SetupReqPak->wValue;
//printf( "SETUP: %02x %02x %02d %02x %04x\n", USBOTG_SetupReqType, USBOTG_SetupReqCode, USBOTG_SetupReqLen, USBOTG_SetupReqIndex, USBOTG_IndexValue );
			len = 0;

			if( ( USBOTG_SetupReqType & USB_REQ_TYP_MASK ) != USB_REQ_TYP_STANDARD )
			{
#if FUSB_HID_INTERFACES > 0 
				if( ( USBOTG_SetupReqType & USB_REQ_TYP_MASK ) == USB_REQ_TYP_CLASS )
				{
					/* Class Request */
					switch( USBOTG_SetupReqCode )
					{
						case HID_SET_REPORT:
#if FUSB_HID_USER_REPORTS
							len = HandleHidUserSetReportSetup( ctx, pUSBOTG_SetupReqPak );
							if( len < 0 ) goto sendstall;
							ctx->USBOTG_SetupReqLen = len;
							UEP_CTRL_LEN(0) = 0;
							// Previously would have been a CTRL_RX = ACK && TOG, but not here on the 203.

							UEP_CTRL_RX(0) = CHECK_USBOTG_UEP_R_AUTO_TOG | USBOTG_UEP_R_RES_ACK | USBOTG_UEP_R_TOG;
							UEP_CTRL_TX(0) = CHECK_USBOTG_UEP_T_AUTO_TOG | USBOTG_UEP_T_TOG;
							goto replycomplete;
						case HID_GET_REPORT:
							len = HandleHidUserGetReportSetup( ctx, pUSBOTG_SetupReqPak );
							if( len < 0 ) goto sendstall;
							ctx->USBOTG_SetupReqLen = len;
							len = len >= DEF_USBD_UEP0_SIZE ? DEF_USBD_UEP0_SIZE : len;
							if( !ctx->pCtrlPayloadPtr )
							{
								len = HandleHidUserReportDataIn( ctx, ctrl0buff, len );
							}
							else
							{
								DMA7FastCopy( ctrl0buff, ctx->pCtrlPayloadPtr, len );
								ctx->pCtrlPayloadPtr += len;
							}
							UEP_CTRL_LEN(0) = len;
							UEP_CTRL_TX(0) = CHECK_USBOTG_UEP_T_AUTO_TOG | USBOTG_UEP_T_RES_ACK | USBOTG_UEP_T_TOG;
							ctx->USBOTG_SetupReqLen -= len;
							goto replycomplete;
#endif
							break;


						case HID_SET_IDLE:
							if( USBOTG_SetupReqIndex < FUSB_HID_INTERFACES )
								USBOTGCTX.USBOTG_HidIdle[ USBOTG_SetupReqIndex ] = (uint8_t)( USBOTG_IndexValue >> 8 );
							break;
						case HID_SET_PROTOCOL:
							if ( USBOTG_SetupReqIndex < FUSB_HID_INTERFACES )
								USBOTGCTX.USBOTG_HidProtocol[USBOTG_SetupReqIndex] = (uint8_t)USBOTG_IndexValue;
							break;

						case HID_GET_IDLE:
							if( USBOTG_SetupReqIndex < FUSB_HID_INTERFACES )
							{
								ctrl0buff[0] = USBOTGCTX.USBOTG_HidIdle[ USBOTG_SetupReqIndex ];
								len = 1;
							}
							break;

						case HID_GET_PROTOCOL:
							if( USBOTG_SetupReqIndex < FUSB_HID_INTERFACES )
							{
								ctrl0buff[0] = USBOTGCTX.USBOTG_HidProtocol[ USBOTG_SetupReqIndex ];
								len = 1;
							}
							break;

						default:
							goto sendstall;
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
				switch( USBOTG_SetupReqCode )
				{
					/* get device/configuration/string/report/... descriptors */
					case USB_GET_DESCRIPTOR:
					{
						const struct descriptor_list_struct * e = descriptor_list;
						const struct descriptor_list_struct * e_end = e + DESCRIPTOR_LIST_ENTRIES;
						for( ; e != e_end; e++ )
						{
							if( e->lIndexValue == USBOTG_IndexValue )
							{
								ctx->pCtrlPayloadPtr = (uint8_t*)e->addr;
								len = e->length;
								break;
							}
						}
						if( e == e_end )
							goto sendstall;
						if( len > USBOTG_SetupReqLen )
							len = USBOTG_SetupReqLen;
						ctx->USBOTG_SetupReqLen = len;
					}

					/* Set usb address */
					case USB_SET_ADDRESS:
						ctx->USBOTG_DevAddr = (uint8_t)( ctx->USBOTG_IndexValue & 0xFF );
						// NOTE: Do not actually set addres here!  If we do, we won't get the PID_IN associated with this SETUP.
						break;

					/* Get usb configuration now set */
					case USB_GET_CONFIGURATION:
						ctrl0buff[0] = ctx->USBOTG_DevConfig;
						if( ctx->USBOTG_SetupReqLen > 1 )
							ctx->USBOTG_SetupReqLen = 1;
						break;

					/* Set usb configuration to use */
					case USB_SET_CONFIGURATION:
						ctx->USBOTG_DevConfig = (uint8_t)( ctx->USBOTG_IndexValue & 0xFF );
						ctx->USBOTG_DevEnumStatus = 0x01;
						break;

					/* Clear or disable one usb feature */
					case USB_CLEAR_FEATURE:
#if FUSB_SUPPORTS_SLEEP
						if( ( USBOTG_SetupReqType & USB_REQ_RECIP_MASK ) == USB_REQ_RECIP_DEVICE )
						{
							/* clear one device feature */
							if( (uint8_t)( USBOTG_IndexValue & 0xFF ) == USB_REQ_FEAT_REMOTE_WAKEUP )
							{
								/* clear usb sleep status, device not prepare to sleep */
								ctx->USBOTG_DevSleepStatus &= ~0x01;
							}
							else
							{
								goto sendstall;
							}
						}
						else
#endif
						if( ( USBOTG_SetupReqType & USB_REQ_RECIP_MASK ) == USB_REQ_RECIP_ENDP )
						{
							if( (uint8_t)( USBOTG_IndexValue & 0xFF ) == USB_REQ_FEAT_ENDP_HALT )
							{
								/* Clear End-point Feature */
								if( ( USBOTG_SetupReqIndex & DEF_UEP_IN ) && ep < FUSB_CONFIG_EPS ) 
								{
									UEP_CTRL_TX(ep) = USBOTG_UEP_T_RES_STALL | CHECK_USBOTG_UEP_T_AUTO_TOG;
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
						if( ( USBOTG_SetupReqType & USB_REQ_RECIP_MASK ) == USB_REQ_RECIP_DEVICE )
						{
#if FUSB_SUPPORTS_SLEEP
							/* Set Device Feature */
							if( (uint8_t)( USBOTG_IndexValue & 0xFF ) == USB_REQ_FEAT_REMOTE_WAKEUP )
							{
								/* Set Wake-up flag, device prepare to sleep */
								USBOTG_DevSleepStatus |= 0x01;
							}
							else
#endif
							{
								goto sendstall;
							}
						}
						else if( ( USBOTG_SetupReqType & USB_REQ_RECIP_MASK ) == USB_REQ_RECIP_ENDP )
						{
							/* Set Endpoint Feature */
							if( (uint8_t)( USBOTG_IndexValue & 0xFF ) == USB_REQ_FEAT_ENDP_HALT )
							{
								if( ( USBOTG_SetupReqIndex & DEF_UEP_IN ) && ep < FUSB_CONFIG_EPS )
									UEP_CTRL_TX(ep) = ( UEP_CTRL_TX(ep) & ~USBOTG_UEP_T_RES_MASK ) | USBOTG_UEP_T_RES_STALL;
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
						if( USBOTG_SetupReqLen > 1 ) USBOTG_SetupReqLen = 1;
						break;

					case USB_SET_INTERFACE:
						break;

					/* host get status of specified device/interface/end-points */
					case USB_GET_STATUS:
						ctrl0buff[0] = 0x00;
						ctrl0buff[1] = 0x00;
						if( ( USBOTG_SetupReqType & USB_REQ_RECIP_MASK ) == USB_REQ_RECIP_DEVICE )
						{
#if FUSB_SUPPORTS_SLEEP
							ctrl0buff[0] = (ctx->USBOTG_DevSleepStatus & 0x01)<<1;
#else
							ctrl0buff[0] = 0x00;
#endif
						}
						else if( ( USBOTG_SetupReqType & USB_REQ_RECIP_MASK ) == USB_REQ_RECIP_ENDP )
						{
							if( ( USBOTG_SetupReqIndex & DEF_UEP_IN ) && ep < FUSB_CONFIG_EPS )
								ctrl0buff[0] = ( UEP_CTRL_TX(ep) & USBOTG_UEP_T_RES_MASK ) == USBOTG_UEP_T_RES_STALL;
							else
								goto sendstall;
						}
						else
							goto sendstall;
						if( USBOTG_SetupReqLen > 2 )
							USBOTG_SetupReqLen = 2;
						break;

					default:
						goto sendstall;
						break;
				}
			}


			{
				/* end-point 0 data Tx/Rx */
				if( USBOTG_SetupReqType & DEF_UEP_IN )
				{
					len = ( USBOTG_SetupReqLen > DEF_USBD_UEP0_SIZE )? DEF_USBD_UEP0_SIZE : USBOTG_SetupReqLen;
					USBOTG_SetupReqLen -= len;
					UEP_CTRL_LEN(0) = len;
					UEP_CTRL_TX(0) = CHECK_USBOTG_UEP_T_AUTO_TOG | USBOTG_UEP_T_RES_ACK;
				}
				else
				{
					if( USBOTG_SetupReqLen == 0 )
					{
						UEP_CTRL_LEN(0) = 0;
						UEP_CTRL_TX(0) = CHECK_USBOTG_UEP_T_AUTO_TOG | USBOTG_UEP_T_RES_ACK | USBOTG_UEP_T_TOG;
					}
					else
					{
						UEP_CTRL_RX(0) = CHECK_USBOTG_UEP_R_AUTO_TOG | USBOTG_UEP_R_RES_ACK | USBOTG_UEP_R_TOG;
					}
				}

			}
			break;

			// This might look a little weird, for error handling but it saves a nontrivial amount of storage, and simplifies
			// control flow to hard-abort here.
		sendstall:
			// if one request not support, return stall.  Stall means permanent error.
			UEP_CTRL_TX(0) = USBOTG_UEP_T_TOG | USBOTG_UEP_T_RES_STALL;
			UEP_CTRL_RX(0) = USBOTG_UEP_R_TOG | USBOTG_UEP_R_RES_STALL;
		replycomplete:
			break;

		/* Sof pack processing */
		//case CUIS_TOKEN_SOF:
		//	break;

		default :
			break;
		}


		USBOTG_FS->INT_FG = CRB_UIF_TRANSFER;
	}
	else if( intfgst & CRB_UIF_BUS_RST )
	{
		/* usb reset interrupt processing */
		ctx->USBOTG_DevConfig = 0;
		ctx->USBOTG_DevAddr = 0;
		ctx->USBOTG_DevSleepStatus = 0;
		ctx->USBOTG_DevEnumStatus = 0;

		USBOTG_FS->DEV_ADDR = 0;
		USBOTG_InternalFinishSetup( );
		USBOTG_FS->INT_FG = CRB_UIF_BUS_RST;
	}
	else if( intfgst & CRB_UIF_SUSPEND )
	{
		/* usb suspend interrupt processing */
		if( USBOTG_FS->MIS_ST & USBOTG_UMS_SUSPEND )
		{
			ctx->USBOTG_DevSleepStatus |= 0x02;
			if( ctx->USBOTG_DevSleepStatus == 0x03 )
			{
				/* Handling usb sleep here */
				//TODO: MCU_Sleep_Wakeup_Operate( );
			}
		}
		else
		{
			ctx->USBOTG_DevSleepStatus &= ~0x02;
		}
		USBOTG_FS->INT_FG = USBOTG_UMS_SUSPEND;
	}

#if FUSB_IO_PROFILE
	funDigitalWrite( PB0, 0 );
#endif

//#if FUSB_USE_HPE
//	asm volatile( "mret" );
//#endif
}

void USBOTG_InternalFinishSetup()
{
#if FUSB_CONFIG_EPS > 4
    USBOTG_FS->UEP4_1_MOD = USBOTG_UEP1_TX_EN | USBOTG_UEP4_TX_EN;
#elif FUSB_CONFIG_EPS > 1
    USBOTG_FS->UEP4_1_MOD = USBOTG_UEP1_TX_EN | USBOTG_UEP4_TX_EN;
#endif

#if FUSB_CONFIG_EPS > 3
    USBOTG_FS->UEP2_3_MOD = USBOTG_UEP2_TX_EN | USBOTG_UEP3_TX_EN;
#elif FUSB_CONFIG_EPS > 2
    USBOTG_FS->UEP2_3_MOD = USBOTG_UEP2_TX_EN;
#endif

	USBOTG_FS->UEP5_6_MOD = 0;

	// This is really cursed.  Somehow it doesn't explode.
	// But, normally the USB wants a separate buffer here.

#if FUSB_CONFIG_EPS > 4
	USBOTG_FS->UEP4_DMA = (uintptr_t)USBOTGCTX.ENDPOINTS[4];
#endif
#if FUSB_CONFIG_EPS > 3
	USBOTG_FS->UEP3_DMA = (uintptr_t)USBOTGCTX.ENDPOINTS[3];
#endif
#if FUSB_CONFIG_EPS > 2
	USBOTG_FS->UEP2_DMA = (uintptr_t)USBOTGCTX.ENDPOINTS[2];
#endif
#if FUSB_CONFIG_EPS > 1
	USBOTG_FS->UEP1_DMA = (uintptr_t)USBOTGCTX.ENDPOINTS[1];
#endif
#if FUSB_CONFIG_EPS > 0
	USBOTG_FS->UEP0_DMA = (uintptr_t)USBOTGCTX.ENDPOINTS[0];
#else
#error You must have at least EP0!
#endif

	UEP_CTRL_TX(0) = USBOTG_UEP_T_RES_NAK | CHECK_USBOTG_UEP_T_AUTO_TOG;
	UEP_CTRL_RX(0) = USBOTG_UEP_R_RES_ACK | CHECK_USBOTG_UEP_R_AUTO_TOG;
	int i;
	for( i = 1; i < FUSB_CONFIG_EPS; i++ )
	{
		UEP_CTRL_TX(i) = USBOTG_UEP_T_RES_NAK | CHECK_USBOTG_UEP_T_AUTO_TOG;
		UEP_CTRL_RX(i) = USBOTG_UEP_R_RES_NAK | CHECK_USBOTG_UEP_R_AUTO_TOG;
	}
                                                                    
    for(uint8_t i=0; i< sizeof(USBOTGCTX.USBOTG_Endp_Busy)/sizeof(USBOTGCTX.USBOTG_Endp_Busy[0]); i++ )
    {
        USBOTGCTX.USBOTG_Endp_Busy[ i ] = 0;
    }
}

int USBOTGSetup()
{
	// USBPRE[1:0] = 10: Divided by 3 (when PLLCLK=144MHz);
	// Must be done before enabling clock to USB OTG tree.
	RCC->CFGR0 = (RCC->CFGR0 & ~(3<<22)) | (2<<22);

	RCC->APB2PCENR |= RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOB;
	RCC->AHBPCENR |= RCC_USBFS | RCC_AHBPeriph_DMA1; // USBFS === OTG_FSEN

	// Force module to reset.
	USBOTG_FS->BASE_CTRL = USBOTG_UC_RESET_SIE | USBOTG_UC_CLR_ALL;
	USBOTG_FS->BASE_CTRL = 0x00;

	USBOTG_InternalFinishSetup();

	// Enter device mode.
	USBOTG_FS->DEV_ADDR  = 0x00;
	USBOTG_FS->INT_FG    = 0xff;
	USBOTG_FS->INT_EN    = USBOTG_UIE_SUSPEND | USBOTG_UIE_TRANSFER | USBOTG_UIE_BUS_RST;
	USBOTG_FS->BASE_CTRL = USBOTG_UC_INT_BUSY | USBOTG_UC_DMA_EN | USBOTG_UC_DEV_PU_EN;
	USBOTG_FS->UDEV_CTRL = USBOTG_UD_PD_DIS | USBOTG_UD_PORT_EN;

	NVIC_EnableIRQ( USBHD_IRQn );

	USBOTG_FS->OTG_CR = 0; //Note says only valid on 205, 207, 305, 307. 

	//printf( "CFGR0: %08x / %08x / %08x\n", RCC->CFGR0, USBOTG_FS->BASE_CTRL, USBOTG_FS->INT_FG);

	// Actually go on-bus.
	USBOTG_FS->BASE_CTRL |= USBOTG_UC_DEV_PU_EN;

#if FUSB_IO_PROFILE
	funPinMode( PB0, GPIO_CFGLR_OUT_50Mhz_PP );
#endif

	// Go on-bus.
	return 0;
}

// To TX, you can use USBOTG_GetEPBufferIfAvailable or USBFSD_UEP_DMA( endp )

static inline uint8_t * USBOTG_GetEPBufferIfAvailable( int endp )
{
	if( USBOTGCTX.USBOTG_Endp_Busy[ endp ] ) return 0;
	return USBOTGCTX.ENDPOINTS[ endp ];
}

static inline int USBOTG_SendEndpoint( int endp, int len )
{
	if( USBOTGCTX.USBOTG_errata_dont_send_endpoint_in_window || USBOTGCTX.USBOTG_Endp_Busy[ endp ] ) return -1;
	UEP_CTRL_LEN( endp ) = len;
	UEP_CTRL_TX( endp ) = ( UEP_CTRL_TX( endp ) & ~USBOTG_UEP_T_RES_MASK ) | USBOTG_UEP_T_RES_ACK;
	USBOTGCTX.USBOTG_Endp_Busy[ endp ] = 0x01;
	return 0;
}





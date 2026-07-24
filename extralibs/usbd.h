#ifndef USBD_H
#define USBD_H

#include "ch32fun.h"
#include "usb_config.h"
#include "usb_defines.h"
#include <stdint.h>

#if defined(FUSB_FROM_RAM) && (FUSB_FROM_RAM)
#define __USBD_FUN_ATTRIBUTE __attribute__((section( ".srodata" ), used))
#else
#define __USBD_FUN_ATTRIBUTE
#endif

// 0x400 of shared memory
// Up to 8 endpoints (0x10 each)

// Buffer Description Table
typedef struct __attribute__((__packed__))
{
	USBD_BTABLE_TypeDef EP[8];
} USBD_BDT_TypeDef;
#define USBD_BDT ( (USBD_BDT_TypeDef *)CAN_USBD_SHARED_BASE )

// Packet buffer (0x400 - 0x80 = 0x380) left
#define USBD_PMA_BASE 0x80 // Offset from USBD_BDT_BASE
#define USBD_PACKET_SIZE 64

#define USBD_EPR_CTR_RX 0x8000
#define USBD_EPR_DTOG_RX 0x4000
#define USBD_EPR_STAT_RX_MASK 0x3000
#define USBD_EPR_STAT_RX_DIS 0x0000
#define USBD_EPR_STAT_RX_STALL 0x1000
#define USBD_EPR_STAT_RX_NAK 0x2000
#define USBD_EPR_STAT_RX_ACK 0x3000
#define USBD_EPR_SETUP 0x0800
#define USBD_EPR_EP_TYPE_MASK 0x0600
#define USBD_EPR_EP_TYPE_BULK 0x0000
#define USBD_EPR_EP_TYPE_CTRL 0x0200
#define USBD_EPR_EP_TYPE_ISO 0x0400
#define USBD_EPR_EP_TYPE_INT 0x0600
#define USBD_EPR_EP_KIND 0x0100
#define USBD_EPR_CTR_TX 0x0080
#define USBD_EPR_DTOG_TX 0x0040
#define USBD_EPR_STAT_TX_MASK 0x0030
#define USBD_EPR_STAT_TX_DIS 0x0000
#define USBD_EPR_STAT_TX_STALL 0x0010
#define USBD_EPR_STAT_TX_NAK 0x0020
#define USBD_EPR_STAT_TX_ACK 0x0030
#define USBD_EPR_EA 0x000F

#define USB_GET_STATUS 0x00
#define USB_SET_ADDRESS 0x05
#define USB_GET_DESCRIPTOR 0x06
#define USB_GET_CONFIG 0x08
#define USB_SET_CONFIG 0x09
#define USB_GET_INTERFACE 0x0A

#define USB_CLEAR_FEATURE 0x01
#define USB_SET_FEATURE 0x03
#define USB_SET_DESCRIPTOR 0x07
#define USB_SET_INTERFACE 0x0B

#define USBD_DEVICE_DESCRIPTOR 0x01
#define USBD_CONFIG_DESCRIPTOR 0x02
#define USBD_HID_DESCRIPTOR 0x22
#define USBD_STRING_DESCRIPTOR 0x03

#define USBD_REQ_TYP_MASK 0b01100000
#define USBD_REQ_TYP_STANDARD 0b00000000
#define USBD_REQ_TYP_CLASS 0b00100000
#define USBD_REQ_TYP_VENDOR 0b01000000

#define USBD_EP_MODE_TX 4
#define USBD_EP_MODE_RX 8
#define USBD_EP_MODE_BDIR 12

// Magic value for ack
#define USBD_SEND_ACK (0xFFFF)

#ifndef USBD_SILENCE_EP_MODE
#ifdef USBFS_EP_MODE_RX
#warning USBFS_EP_MODE_RX "USBD should use USBD_EP_MODE_RX instead of USBFS_EP_MODE_RX unless also using USBFS (silence by defining USBD_SILENCE_EP_MODE)"
#endif
#ifdef USBFS_EP_MODE_TX
#warning USBFS_EP_MODE_TX "USBD should use USBD_EP_MODE_TX instead of USBFS_EP_MODE_TX unless also using USBFS (silence by defining USBD_SILENCE_EP_MODE)"
#endif
#endif

struct _USBState;

int USBDSetup( void );
void USBDReset( void );
uint8_t *USBD_GetEPBufferIfAvailable( int endp ); // Only 1 buffer might be active at a time
int USBD_SendEndpoint( int endp, int len );
int USBD_SendEndpointNEW( int endp, uint8_t *data, int len, int copy ); // Copy is ignored
int USBD_SendACK( int endp, int tx );
int USBD_SendNAK( int endp, int tx );

#if FUSB_HID_USER_REPORTS
int HandleHidUserGetReportSetup( struct _USBState *ctx, tusb_control_request_t *req );
int HandleHidUserSetReportSetup( struct _USBState *ctx, tusb_control_request_t *req );
void HandleHidUserReportDataOut( struct _USBState *ctx, uint8_t *data, int len );
int HandleHidUserReportDataIn( struct _USBState *ctx, uint8_t *data, int len );
void HandleHidUserReportOutComplete( struct _USBState *ctx );
#endif

#if FUSB_USER_HANDLERS
int HandleInRequest( struct _USBState *ctx, int endp, uint8_t *data, int len );
void HandleDataOut( struct _USBState *ctx, int endp, uint8_t *data, int len );
int HandleSetupCustom( struct _USBState *ctx, int setup_code );
#endif

#if FUNCONF_USE_USBPRINTF
void HandleUSBInput( int numbytes, uint8_t *data );
extern uint8_t usb_inputbuffer[USBD_PACKET_SIZE];
extern int usb_inbuf_idx;
#endif

#ifndef FUSB_MAX_EP_CNT
#define FUSB_MAX_EP_CNT 8
#endif

#if !defined( FUSB_BUFFERS_NUMBER ) || FUSB_BUFFERS_NUMBER == 0
#define FUSB_BUFFERS_NUMBER 1
#undef FUSB_EP1_MODE
#define FUSB_EP1_MODE 0
#undef FUSB_EP2_MODE
#define FUSB_EP2_MODE 0
#undef FUSB_EP3_MODE
#define FUSB_EP3_MODE 0
#undef FUSB_EP4_MODE
#define FUSB_EP4_MODE 0
#undef FUSB_EP5_MODE
#define FUSB_EP5_MODE 0
#undef FUSB_EP6_MODE
#define FUSB_EP6_MODE 0
#undef FUSB_EP7_MODE
#define FUSB_EP7_MODE 0
#warning "You may have forgotten to properly define used EPs in usb_config.h"
#endif

struct _USBState
{
	// Setup Request
	uint8_t USBD_SetupReqCode;
	uint8_t USBD_SetupReqType;
	uint16_t USBD_SetupReqLen; // Used for tracking place along send.
	uint32_t USBD_IndexValue;

	// USB Device Status
	uint8_t USBD_DevConfig;
	uint8_t USBD_DevAddr;
	uint8_t USBD_DevSleepStatus;
	uint8_t USBD_DevEnumStatus;

	uint8_t *pCtrlPayloadPtr;

	uint32_t *usbd_ep_data_ptr[FUSB_MAX_EP_CNT];

	struct
	{
		union
		{
			uint8_t *in;
			uint8_t *tx;
		};
		uint8_t mode;
		uint8_t busy;
	} endpoints[FUSB_MAX_EP_CNT];

#if FUSB_HID_INTERFACES > 0
	uint8_t USBD_HidIdle[FUSB_HID_INTERFACES];
	uint8_t USBD_HidProtocol[FUSB_HID_INTERFACES];
#endif

	// We don't really need buffers for other endpoints
	// We waste 64 bytes to prevent overflows (why are overflows happening??)
	// This was probably fine in other demos
	// But here we only have 1 buffer
	uint8_t ep0buf[2][USBD_PACKET_SIZE];
#define CTRL0BUFF ( USBDCTX.ep0buf[0] )
};

extern struct _USBState USBDCTX;

#endif // USBD_H

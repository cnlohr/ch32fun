#ifndef _FSUSB_H
#define _FSUSB_H

#include <stdint.h>
#include "ch32fun.h"
#include "usb_defines.h"
#include "usb_config.h"

#if defined(FUSB_FROM_RAM) && (FUSB_FROM_RAM)
#if defined(CH5xx)
#define __USBFS_FUN_ATTRIBUTE __attribute__((section(".srodata"), used))
#else
#define __USBFS_FUN_ATTRIBUTE
#endif
#else
#define __USBFS_FUN_ATTRIBUTE
#endif

#if defined(CH5xx) || defined(CH32X03x) || defined(CH32V10x)
#if !defined (CH32X03x) && !defined(CH32V10x)
#define USBFS_BASE USB_BASE_ADDR
typedef struct
{
	__IO uint8_t  BASE_CTRL;
	__IO uint8_t  UDEV_CTRL;
	__IO uint8_t  INT_EN;
	__IO uint8_t  DEV_ADDR;
	__IO uint8_t  USB_STATUS;
	__IO uint8_t  MIS_ST;
	__IO uint8_t  INT_FG; // "Combined" register in some situations. (ST_FG)
	__IO uint8_t  INT_ST;
	__IO uint8_t RX_LEN;
	__IO uint8_t  Reserved0;
	__IO uint16_t  Reserved1;
	__IO uint8_t  UEP4_1_MOD;
	__IO uint8_t  UEP2_3_MOD;
	__IO uint8_t  UEP567_MOD;
	__IO uint8_t  Reserved2;
	__IO uint16_t UEP0_DMA;
	__IO uint16_t Reserved3;
	__IO uint16_t UEP1_DMA;
	__IO uint16_t Reserved4;
	__IO uint16_t UEP2_DMA;
	__IO uint16_t Reserved5;
	__IO uint16_t UEP3_DMA;
	__IO uint16_t Reserved6;
	__IO uint8_t UEP0_TX_LEN;
	__IO uint8_t Reserved7;
	__IO uint8_t  UEP0_TX_CTRL;
	__IO uint8_t Reserved8;
	__IO uint8_t UEP1_TX_LEN;
	__IO uint8_t Reserved9;
	__IO uint8_t  UEP1_TX_CTRL;
	__IO uint8_t Reserved10;
	__IO uint8_t UEP2_TX_LEN;
	__IO uint8_t Reserved11;
	__IO uint8_t  UEP2_TX_CTRL;
	__IO uint8_t Reserved12;
	__IO uint8_t UEP3_TX_LEN;
	__IO uint8_t Reserved13;
	__IO uint8_t  UEP3_TX_CTRL;
	__IO uint8_t Reserved14;
	__IO uint8_t UEP4_TX_LEN;
	__IO uint8_t Reserved15;
	__IO uint8_t  UEP4_TX_CTRL;
	__IO uint8_t Reserved16;
	__IO uint32_t Reserved17;
	__IO uint32_t Reserved18;
	__IO uint32_t Reserved19;
	__IO uint32_t Reserved20;
	__IO uint32_t Reserved21;
	__IO uint32_t Reserved22;
	__IO uint32_t Reserved23;
	__IO uint32_t Reserved24;
	__IO uint16_t UEP5_DMA;
	__IO uint16_t UEP6_DMA;
	__IO uint16_t UEP7_DMA;
	__IO uint32_t Reserved25;
	__IO uint8_t UEP5_TX_LEN;
	__IO uint8_t Reserved26;
	__IO uint8_t  UEP5_TX_CTRL;
	__IO uint8_t Reserved27;
	__IO uint8_t UEP6_TX_LEN;
	__IO uint8_t Reserved28;
	__IO uint8_t  UEP6_TX_CTRL;
	__IO uint8_t Reserved29;
	__IO uint8_t UEP7_TX_LEN;
	__IO uint8_t  UEP7_TX_CTRL;
	__IO uint8_t Reserved30;
	__IO uint32_t EPX_MODE;
} USBFS_TypeDef;

#define UEP_CTRL_LEN(n) (((uint8_t*)&USBFS->UEP0_TX_LEN)[n*4])
#define UEP_CTRL_TX(n)  (((uint8_t*)&USBFS->UEP0_TX_CTRL)[n*4])
#define UEP_CTRL_RX(n)  (((uint8_t*)&USBFS->UEP0_TX_CTRL)[n*4])
#define UEP_DMA(n)      (((uint16_t*)&USBFS->UEP0_DMA)[n*2])
#endif

#if defined(CH32V10x)
#define USBFS_BASE 0x40023400
#define USB_IRQn USBFS_IRQn
typedef struct
{
	__IO uint8_t  BASE_CTRL;
	__IO uint8_t  UDEV_CTRL;
	__IO uint8_t  INT_EN;
	__IO uint8_t  DEV_ADDR;
	__IO uint8_t  USB_STATUS;
	__IO uint8_t  MIS_ST;
	__IO uint8_t  INT_FG; // "Combined" register in some situations. (ST_FG)
	__IO uint8_t  INT_ST;
	__IO uint8_t RX_LEN;
	__IO uint8_t  Reserved0;
	__IO uint16_t  Reserved1;
	__IO uint8_t  UEP4_1_MOD;
	__IO uint8_t  UEP2_3_MOD;
	__IO uint8_t  UEP5_6_MOD;
	__IO uint8_t  UEP7_MOD;
	__IO uint16_t UEP0_DMA;
	__IO uint16_t Reserved3;
	__IO uint16_t UEP1_DMA;
	__IO uint16_t Reserved4;
	__IO uint16_t UEP2_DMA;
	__IO uint16_t Reserved5;
	__IO uint16_t UEP3_DMA;
	__IO uint16_t Reserved6;
	__IO uint16_t UEP4_DMA;
	__IO uint16_t Reserved7;
	__IO uint16_t UEP5_DMA;
	__IO uint16_t Reserved8;
	__IO uint16_t UEP6_DMA;
	__IO uint16_t Reserved9;
	__IO uint16_t UEP7_DMA;
	__IO uint16_t Reserved10;
	__IO uint16_t UEP0_TX_LEN;
	__IO uint8_t UEP0_TX_CTRL;
	__IO uint8_t Reserved11;
	__IO uint16_t UEP1_TX_LEN;
	__IO uint8_t UEP1_TX_CTRL;
	__IO uint8_t Reserved12;
	__IO uint16_t UEP2_TX_LEN;
	__IO uint8_t UEP2_TX_CTRL;
	__IO uint8_t Reserved13;
	__IO uint16_t UEP3_TX_LEN;
	__IO uint8_t UEP3_TX_CTRL;
	__IO uint8_t Reserved14;
	__IO uint16_t UEP4_TX_LEN;
	__IO uint8_t UEP4_TX_CTRL;
	__IO uint8_t Reserved15;
	__IO uint16_t UEP5_TX_LEN;
	__IO uint8_t UEP5_TX_CTRL;
	__IO uint8_t Reserved16;
	__IO uint16_t UEP6_TX_LEN;
	__IO uint8_t UEP6_TX_CTRL;
	__IO uint8_t Reserved17;
	__IO uint16_t UEP7_TX_LEN;
	__IO uint8_t UEP7_TX_CTRL;
	__IO uint8_t Reserved18;
} USBFS_TypeDef;

#define UEP_CTRL_LEN(n) (((volatile uint16_t*)&USBFS->UEP0_TX_LEN)[n*2])
#define UEP_CTRL_TX(n)  (((volatile uint8_t*)&USBFS->UEP0_TX_CTRL)[n*4])
#define UEP_CTRL_RX(n)  (((volatile uint8_t*)&USBFS->UEP0_TX_CTRL)[n*4])
#define UEP_DMA(n)      (((volatile uint16_t*)&USBFS->UEP0_DMA)[n*2])
#endif

#if !defined (CH32X03x)
#define DEBUG_PIN PA8

#define DEF_USBD_UEP0_SIZE 64	 /* usb hs/fs device end-point 0 size */
#define UEP_SIZE 64

#define DEF_UEP_IN   0x80
#define DEF_UEP_OUT  0x00
#define DEF_UEP_BUSY 0x01
#define DEF_UEP_FREE 0x00

#define DEF_UEP0 0
#define DEF_UEP1 1
#define DEF_UEP2 2
#define DEF_UEP3 3
#define DEF_UEP4 4
#define DEF_UEP5 5
#define DEF_UEP6 6
#define DEF_UEP7 7
#define UNUM_EP 8

#define USBFS_UEP_T_RES_MASK MASK_UEP_T_RES
#define USBFS_UEP_T_RES_ACK UEP_T_RES_ACK
#define USBFS_UEP_T_RES_NAK UEP_T_RES_NAK
#define USBFS_UEP_T_TOG RB_UEP_T_TOG
#define USBFS_UEP_T_RES_STALL UEP_T_RES_STALL

#define USBFS_UEP_R_RES_MASK MASK_UEP_R_RES
#define USBFS_UEP_R_RES_ACK UEP_R_RES_ACK
#define USBFS_UEP_R_RES_NAK UEP_R_RES_NAK
#define USBFS_UEP_R_TOG RB_UEP_R_TOG
#define USBFS_UEP_R_RES_STALL UEP_R_RES_STALL

#define USBFS_UDA_GP_BIT RB_UDA_GP_BIT
#define USBFS_UMS_SUSPEND RB_UMS_SUSPEND

#define USBFS_UC_RESET_SIE RB_UC_RESET_SIE
#define USBFS_UC_CLR_ALL RB_UC_CLR_ALL
#define USBFS_UIE_SUSPEND RB_UIE_SUSPEND
#define USBFS_UIE_TRANSFER RB_UIE_TRANSFER
#define USBFS_UIE_BUS_RST RB_UIE_BUS_RST
#define USBFS_UC_INT_BUSY RB_UC_INT_BUSY
#define USBFS_UC_DMA_EN RB_UC_DMA_EN
#define USBFS_UC_DEV_PU_EN RB_UC_DEV_PU_EN

#endif

#define USBFS_UD_PORT_EN RB_UD_PORT_EN
#define USBFS_UD_PD_DIS RB_UD_PD_DIS

#define USBFS_UEP1_RX_EN RB_UEP1_RX_EN
#define USBFS_UEP2_RX_EN RB_UEP2_RX_EN
#define USBFS_UEP3_RX_EN RB_UEP3_RX_EN
#define USBFS_UEP4_RX_EN RB_UEP4_RX_EN

#define USBFS_UEP1_TX_EN RB_UEP1_TX_EN
#define USBFS_UEP2_TX_EN RB_UEP2_TX_EN
#define USBFS_UEP3_TX_EN RB_UEP3_TX_EN
#define USBFS_UEP4_TX_EN RB_UEP4_TX_EN

#define CHECK_USBFS_UEP_AUTO_TOG RB_UEP_AUTO_TOG
#define CHECK_USBFS_UEP_T_AUTO_TOG RB_UEP_AUTO_TOG
// #define CHECK_USBFS_UEP_R_AUTO_TOG USBOTG_UEP_R_AUTO_TOG

#define USBFS           ((USBFS_TypeDef *)USBFS_BASE)

#ifdef CH32X03x
#define UEP_CTRL_LEN(n) (((volatile uint16_t*)&USBFS->UEP0_TX_LEN)[n*2])
#define UEP_CTRL_TX(n)  (((volatile uint16_t*)&USBFS->UEP0_CTRL_H)[n*2])
#define UEP_CTRL_RX(n)  (((volatile uint16_t*)&USBFS->UEP0_CTRL_H)[n*2])
#define UEP_DMA(n)      (((volatile uint32_t*)&USBFS->UEP0_DMA)[n])
#define DEBUG_PIN PB12
#define USB_IRQn USBFS_IRQn
#endif

#else

#define DEBUG_PIN PB0

#define USBFS           ((USBOTG_FS_TypeDef *)USBFS_BASE)

#define UEP_CTRL_LEN(n) (((volatile uint16_t*)&USBFS->UEP0_TX_LEN)[n*2])
#define UEP_CTRL_TX(n)  (((volatile uint8_t*)&USBFS->UEP0_TX_CTRL)[n*4])
#define UEP_CTRL_RX(n)  (((volatile uint8_t*)&USBFS->UEP0_RX_CTRL)[n*4])
#define UEP_DMA(n)      (((volatile uint32_t*)&USBFS->UEP0_DMA)[n])

#define CHECK_USBFS_UEP_T_AUTO_TOG USBOTG_UEP_T_AUTO_TOG
#define CHECK_USBFS_UEP_R_AUTO_TOG USBOTG_UEP_R_AUTO_TOG

#define USBFS_UEP_T_RES_MASK USBOTG_UEP_T_RES_MASK
#define USBFS_UEP_T_RES_ACK USBOTG_UEP_T_RES_ACK
#define USBFS_UEP_T_RES_NAK USBOTG_UEP_T_RES_NAK
#define USBFS_UEP_T_TOG USBOTG_UEP_T_TOG
#define USBFS_UEP_T_RES_STALL USBOTG_UEP_T_RES_STALL

#define USBFS_UEP_R_RES_MASK USBOTG_UEP_R_RES_MASK
#define USBFS_UEP_R_RES_ACK USBOTG_UEP_R_RES_ACK
#define USBFS_UEP_R_RES_NAK USBOTG_UEP_R_RES_NAK
#define USBFS_UEP_R_TOG USBOTG_UEP_R_TOG
#define USBFS_UEP_R_RES_STALL USBOTG_UEP_R_RES_STALL

// #define USBFS_UDA_GP_BIT USBOTG_UDA_GP_BIT
#define USBFS_UMS_SUSPEND USBOTG_UMS_SUSPEND
#define USBFS_UEP1_RX_EN USBOTG_UEP1_RX_EN
#define USBFS_UEP2_RX_EN USBOTG_UEP2_RX_EN
#define USBFS_UEP3_RX_EN USBOTG_UEP3_RX_EN
#define USBFS_UEP4_RX_EN USBOTG_UEP4_RX_EN

#define USBFS_UEP1_TX_EN USBOTG_UEP1_TX_EN
#define USBFS_UEP2_TX_EN USBOTG_UEP2_TX_EN
#define USBFS_UEP3_TX_EN USBOTG_UEP3_TX_EN
#define USBFS_UEP4_TX_EN USBOTG_UEP4_TX_EN

#define USBFS_UC_RESET_SIE USBOTG_UC_RESET_SIE
#define USBFS_UC_CLR_ALL USBOTG_UC_CLR_ALL
#define USBFS_UIE_SUSPEND USBOTG_UIE_SUSPEND
#define USBFS_UIE_TRANSFER USBOTG_UIE_TRANSFER
#define USBFS_UIE_BUS_RST USBOTG_UIE_BUS_RST
#define USBFS_UC_INT_BUSY USBOTG_UC_INT_BUSY
#define USBFS_UC_DMA_EN USBOTG_UC_DMA_EN
#define USBFS_UC_DEV_PU_EN USBOTG_UC_DEV_PU_EN
#define USBFS_UD_PD_DIS USBOTG_UD_PD_DIS
#define USBFS_UD_PORT_EN USBOTG_UD_PORT_EN

#ifdef CH32V30x_D8C
#define USB_IRQn OTG_FS_IRQn
#else
#define USB_IRQn USBHD_IRQn
#endif

#endif

// Mask for the combined USBFS->INT_FG + USBFS->INT_ST
#define CRB_UIS_IS_NAK    (1<<7)
#define CRB_U_TOG_OK	    (1<<6)
#define CRB_U_SIE_FREE    (1<<5)
#define CRB_UIF_FIFO_OV   (1<<4)
#define CRB_UIF_HST_SOF   (1<<3)
#define CRB_UIF_SUSPEND   (1<<2)
#define CRB_UIF_TRANSFER  (1<<1)
#define CRB_UIF_BUS_RST   (1<<0)

#if defined(CH5xx) || defined(CH32X03x) || defined(CH32V10x)
#define CRB_UIF_SETUP_ACT (1<<15)
#endif
#define CRB_UIS_TOG_OK    (1<<14)
#define CMASK_UIS_TOKEN   (3<<12)
#define CMASK_UIS_ENDP    (0xf<<8)

#define CUIS_TOKEN_OUT	   0x0
#define CUIS_TOKEN_SOF     0x1
#define CUIS_TOKEN_IN      0x2
#define CUIS_TOKEN_SETUP   0x3

#define USBFS_PACKET_SIZE  64

extern uint32_t USBDEBUG0, USBDEBUG1, USBDEBUG2;

struct _USBState;

// Provided functions:
int USBFSSetup();
uint8_t USBFS_Endp_DataUp(uint8_t endp, const uint8_t *pbuf, uint16_t len, uint8_t mod);
static inline uint8_t * USBFS_GetEPBufferIfAvailable( int endp );
static inline int USBFS_SendEndpoint( int endp, int len );
static inline int USBFS_SendACK( int endp, int tx );
static inline int USBFS_SendNAK( int endp, int tx );

#if FUSB_USE_DMA7_COPY
static inline void copyBuffer( uint8_t * dest, const uint8_t * src, int len );
static inline void copyBufferComplete();
#endif

// Implement the following:
#if FUSB_HID_USER_REPORTS
__USBFS_FUN_ATTRIBUTE int HandleHidUserGetReportSetup( struct _USBState * ctx, tusb_control_request_t * req );
__USBFS_FUN_ATTRIBUTE int HandleHidUserSetReportSetup( struct _USBState * ctx, tusb_control_request_t * req );
void HandleHidUserReportDataOut( struct _USBState * ctx, uint8_t * data, int len );
int HandleHidUserReportDataIn( struct _USBState * ctx, uint8_t * data, int len );
void HandleHidUserReportOutComplete( struct _USBState * ctx );
#endif
#if FUSB_USER_HANDLERS
__USBFS_FUN_ATTRIBUTE int HandleInRequest( struct _USBState * ctx, int endp, uint8_t * data, int len );
__USBFS_FUN_ATTRIBUTE void HandleDataOut( struct _USBState * ctx, int endp, uint8_t * data, int len );
__USBFS_FUN_ATTRIBUTE int HandleSetupCustom( struct _USBState * ctx, int setup_code);
#endif

typedef enum
{
	USBFS_EP_OFF = 0,
	USBFS_EP_RX  = -1,
	USBFS_EP_TX = 1,
} USBFS_EP_mode;

#ifndef FUSB_EP1_MODE
#define FUSB_EP1_MODE  0
#endif
#ifndef FUSB_EP2_MODE
#define FUSB_EP2_MODE  0
#endif
#ifndef FUSB_EP3_MODE
#define FUSB_EP3_MODE  0
#endif
#ifndef FUSB_EP4_MODE
#define FUSB_EP4_MODE  0
#endif
#ifndef FUSB_EP5_MODE
#define FUSB_EP5_MODE  0
#endif
#ifndef FUSB_EP6_MODE
#define FUSB_EP6_MODE  0
#endif
#ifndef FUSB_EP7_MODE
#define FUSB_EP7_MODE  0
#endif


struct _USBState
{
	// Setup Request
	uint8_t  USBFS_SetupReqCode;
	uint8_t  USBFS_SetupReqType;
	uint16_t USBFS_SetupReqLen;   // Used for tracking place along send.
	uint32_t USBFS_IndexValue;

	// USB Device Status
	uint8_t  USBFS_DevConfig;
	uint8_t  USBFS_DevAddr;
	uint8_t  USBFS_DevSleepStatus;
	uint8_t  USBFS_DevEnumStatus;

	uint8_t* pCtrlPayloadPtr;

	uint8_t ENDPOINTS[FUSB_CONFIG_EPS][64];
	USBFS_EP_mode endpoint_mode[FUSB_CONFIG_EPS+1]; // IN -1, OUT 1, OFF 0

	#define CTRL0BUFF               (USBFSCTX.ENDPOINTS[0])
	#define pUSBFS_SetupReqPak      ((tusb_control_request_t*)CTRL0BUFF)

#if FUSB_HID_INTERFACES > 0
	uint8_t USBFS_HidIdle[FUSB_HID_INTERFACES];
	uint8_t USBFS_HidProtocol[FUSB_HID_INTERFACES];
#endif
	volatile uint8_t USBFS_Endp_Busy[FUSB_CONFIG_EPS];
	volatile uint8_t USBFS_errata_dont_send_endpoint_in_window;
};

extern struct _USBState USBFSCTX;

#include "fsusb.c"

#endif


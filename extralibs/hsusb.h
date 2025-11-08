#ifndef _HSUSB_H
#define _HSUSB_H

#include <stdint.h>
#include "ch32fun.h"
#include "usb_defines.h"
#include "usb_config.h"

#if !defined(CH584_CH585) && !defined(CH32V205) && !defined(CH32H4xx) && !defined(CH32V30x) && !defined(CH565_CH569)
#error "This lib doesn't work on this MCU"
#endif

#if defined(CH32V30x) || defined(CH565_CH569)
#define USBHS_IMPL 1
// #elif defined(CH584_CH585) || defined(CH32V205) || defined(CH32H4xx)
#else
#define USBHS_IMPL 2
#endif

#ifndef __HIGH_CODE
#define __HIGH_CODE
#endif

#ifndef USBHS_BASE
#if defined(CH32V30x) || defined(CH32V205)
#define USBHS_BASE (0x40023400)
#elif defined(CH32H4xx)
#define USBHS_BASE (0x40030000)
#else
#define USBHS_BASE (0x40009000)
#endif
#endif

#if (USBHS_IMPL==1)
#if (FUNCONF_USE_HSI) && !defined(FUSB_SOF_HSITRIM)
#define FUSB_SOF_HSITRIM 1
#endif
#define TICKS_PER_HSITRIM (FUNCONF_PLL_MULTIPLIER * 20000 * 125) / 1000000 // SOF is sent every 125uS, each HSITRIM changes HSI by 20kHz
typedef struct
{
	__IO uint8_t  BASE_CTRL;
	__IO uint8_t  HOST_CTRL;
	__IO uint8_t  INT_EN;
	__IO uint8_t  DEV_AD;
	__IO uint16_t FRAME_NO;
	__IO uint8_t  SUSPEND;
	__IO uint8_t  RESERVED0;
	__IO uint8_t  SPEED_TYPE;
	__IO uint8_t  MIS_ST;
	__IO uint8_t  INT_FG;
	__IO uint8_t  INT_ST;
	__IO uint16_t RX_LEN;
	__IO uint16_t RESERVED1;
	__IO uint32_t UEP_CONFIG;
	__IO uint32_t UEP_TYPE;
	__IO uint32_t UEP_BUF_MODE;
	__IO uint32_t UEP0_DMA;
	__IO uint32_t UEP1_RX_DMA;
	__IO uint32_t UEP2_RX_DMA;
	__IO uint32_t UEP3_RX_DMA;
	__IO uint32_t UEP4_RX_DMA;
	__IO uint32_t UEP5_RX_DMA;
	__IO uint32_t UEP6_RX_DMA;
	__IO uint32_t UEP7_RX_DMA;
	__IO uint32_t UEP8_RX_DMA;
	__IO uint32_t UEP9_RX_DMA;
	__IO uint32_t UEP10_RX_DMA;
	__IO uint32_t UEP11_RX_DMA;
	__IO uint32_t UEP12_RX_DMA;
	__IO uint32_t UEP13_RX_DMA;
	__IO uint32_t UEP14_RX_DMA;
	__IO uint32_t UEP15_RX_DMA;
	__IO uint32_t UEP1_TX_DMA;
	__IO uint32_t UEP2_TX_DMA;
	__IO uint32_t UEP3_TX_DMA;
	__IO uint32_t UEP4_TX_DMA;
	__IO uint32_t UEP5_TX_DMA;
	__IO uint32_t UEP6_TX_DMA;
	__IO uint32_t UEP7_TX_DMA;
	__IO uint32_t UEP8_TX_DMA;
	__IO uint32_t UEP9_TX_DMA;
	__IO uint32_t UEP10_TX_DMA;
	__IO uint32_t UEP11_TX_DMA;
	__IO uint32_t UEP12_TX_DMA;
	__IO uint32_t UEP13_TX_DMA;
	__IO uint32_t UEP14_TX_DMA;
	__IO uint32_t UEP15_TX_DMA;
	__IO uint16_t UEP0_MAX_LEN;
	__IO uint16_t RESERVED2;
	__IO uint16_t UEP1_MAX_LEN;
	__IO uint16_t RESERVED3;
	__IO uint16_t UEP2_MAX_LEN;
	__IO uint16_t RESERVED4;
	__IO uint16_t UEP3_MAX_LEN;
	__IO uint16_t RESERVED5;
	__IO uint16_t UEP4_MAX_LEN;
	__IO uint16_t RESERVED6;
	__IO uint16_t UEP5_MAX_LEN;
	__IO uint16_t RESERVED7;
	__IO uint16_t UEP6_MAX_LEN;
	__IO uint16_t RESERVED8;
	__IO uint16_t UEP7_MAX_LEN;
	__IO uint16_t RESERVED9;
	__IO uint16_t UEP8_MAX_LEN;
	__IO uint16_t RESERVED10;
	__IO uint16_t UEP9_MAX_LEN;
	__IO uint16_t RESERVED11;
	__IO uint16_t UEP10_MAX_LEN;
	__IO uint16_t RESERVED12;
	__IO uint16_t UEP11_MAX_LEN;
	__IO uint16_t RESERVED13;
	__IO uint16_t UEP12_MAX_LEN;
	__IO uint16_t RESERVED14;
	__IO uint16_t UEP13_MAX_LEN;
	__IO uint16_t RESERVED15;
	__IO uint16_t UEP14_MAX_LEN;
	__IO uint16_t RESERVED16;
	__IO uint16_t UEP15_MAX_LEN;
	__IO uint16_t RESERVED17;
	__IO uint16_t UEP0_T_LEN;
	__IO uint8_t  UEP0_TX_CTRL;
	__IO uint8_t  UEP0_RX_CTRL;
	__IO uint16_t UEP1_T_LEN;
	__IO uint8_t  UEP1_TX_CTRL;
	__IO uint8_t  UEP1_RX_CTRL;
	__IO uint16_t UEP2_T_LEN;
	__IO uint8_t  UEP2_TX_CTRL;
	__IO uint8_t  UEP2_RX_CTRL;
	__IO uint16_t UEP3_T_LEN;
	__IO uint8_t  UEP3_TX_CTRL;
	__IO uint8_t  UEP3_RX_CTRL;
	__IO uint16_t UEP4_T_LEN;
	__IO uint8_t  UEP4_TX_CTRL;
	__IO uint8_t  UEP4_RX_CTRL;
	__IO uint16_t UEP5_T_LEN;
	__IO uint8_t  UEP5_TX_CTRL;
	__IO uint8_t  UEP5_RX_CTRL;
	__IO uint16_t UEP6_T_LEN;
	__IO uint8_t  UEP6_TX_CTRL;
	__IO uint8_t  UEP6_RX_CTRL;
	__IO uint16_t UEP7_T_LEN;
	__IO uint8_t  UEP7_TX_CTRL;
	__IO uint8_t  UEP7_RX_CTRL;
	__IO uint16_t UEP8_T_LEN;
	__IO uint8_t  UEP8_TX_CTRL;
	__IO uint8_t  UEP8_RX_CTRL;
	__IO uint16_t UEP9_T_LEN;
	__IO uint8_t  UEP9_TX_CTRL;
	__IO uint8_t  UEP9_RX_CTRL;
	__IO uint16_t UEP10_T_LEN;
	__IO uint8_t  UEP10_TX_CTRL;
	__IO uint8_t  UEP10_RX_CTRL;
	__IO uint16_t UEP11_T_LEN;
	__IO uint8_t  UEP11_TX_CTRL;
	__IO uint8_t  UEP11_RX_CTRL;
	__IO uint16_t UEP12_T_LEN;
	__IO uint8_t  UEP12_TX_CTRL;
	__IO uint8_t  UEP12_RX_CTRL;
	__IO uint16_t UEP13_T_LEN;
	__IO uint8_t  UEP13_TX_CTRL;
	__IO uint8_t  UEP13_RX_CTRL;
	__IO uint16_t UEP14_T_LEN;
	__IO uint8_t  UEP14_TX_CTRL;
	__IO uint8_t  UEP14_RX_CTRL;
	__IO uint16_t UEP15_T_LEN;
	__IO uint8_t  UEP15_TX_CTRL;
	__IO uint8_t  UEP15_RX_CTRL;
} USBHS_TypeDef;

#define USBHS_CHECK_NAK_RX (USBHS->INT_ST & 0x80)

#define CRB_U_IS_NAK      (1<<7)
#define CTOG_MATCH_SYNC   (1<<6)
#define CRB_UIF_SETUP_ACT (1<<5) // CRB_U_SIE_FREE on USBFS
#define CRB_UIF_FIFO_OV   (1<<4)
#define CRB_UIF_HST_SOF   (1<<3)
#define CRB_UIF_SUSPEND   (1<<2)
#define CRB_UIF_TRANSFER  (1<<1)
#define CRB_UIF_BUS_RST   (1<<0)
#define CSETUP_ACT	      (1<<15)
#define CRB_UIS_TOG_OK    (1<<14)
#define CMASK_UIS_TOKEN   (3<<12)
#define CMASK_UIS_ENDP    (0xf<<8)

#define CUIS_TOKEN_OUT	   0x0
#define CUIS_TOKEN_SOF     0x1
#define CUIS_TOKEN_IN      0x2
#define CUIS_TOKEN_SETUP   0x3

#define UEP_TX_EN(n)    USBHS->UEP_CONFIG |= ((uint32_t)(1<<n))
#define UEP_RX_EN(n)    USBHS->UEP_CONFIG |= ((uint32_t)(1<<(n+16)))

#define USBHS_DONE_TX(n)
#define USBHS_DONE_RX(n)

#elif (USBHS_IMPL==2)
typedef struct
{
	__IO uint8_t  BASE_CTRL;
	__IO uint8_t  BASE_MODE;
	__IO uint8_t  INT_EN;
	__IO uint8_t  DEV_AD;
	__IO uint8_t  WAKE_CTRL;
	__IO uint8_t  TEST_MODE;
	__IO uint16_t LPM_DATA;
	__IO uint8_t  INT_FG; // "Combined" register in some situations. (ST_FG)
	__IO uint8_t  INT_ST;
	__IO uint8_t  MIS_ST;
	__IO uint16_t FRAME_NO;
	__IO uint16_t BUS;
	__IO uint16_t UEP_TX_EN;
	__IO uint16_t UEP_RX_EN;
	__IO uint16_t UEP_T_TOG_AUTO;
	__IO uint16_t UEP_R_TOG_AUTO;
	__IO uint8_t  UEP_T_BURST;
	__IO uint8_t  UEP_T_BURST_MODE;
	__IO uint8_t  UEP_R_BURST;
	__IO uint8_t  UEP_R_RES_MODE;
	__IO uint32_t UEP_AF_MODE;
	__IO uint32_t UEP0_DMA;
	__IO uint32_t UEP1_RX_DMA;
	__IO uint32_t UEP2_RX_DMA;
	__IO uint32_t UEP3_RX_DMA;
	__IO uint32_t UEP4_RX_DMA;
	__IO uint32_t UEP5_RX_DMA;
	__IO uint32_t UEP6_RX_DMA;
	__IO uint32_t UEP7_RX_DMA;
	__IO uint32_t UEP1_TX_DMA;
	__IO uint32_t UEP2_TX_DMA;
	__IO uint32_t UEP3_TX_DMA;
	__IO uint32_t UEP4_TX_DMA;
	__IO uint32_t UEP5_TX_DMA;
	__IO uint32_t UEP6_TX_DMA;
	__IO uint32_t UEP7_TX_DMA;
	__IO uint32_t UEP0_MAX_LEN;
	__IO uint32_t UEP1_MAX_LEN;
	__IO uint32_t UEP2_MAX_LEN;
	__IO uint32_t UEP3_MAX_LEN;
	__IO uint32_t UEP4_MAX_LEN;
	__IO uint32_t UEP5_MAX_LEN;
	__IO uint32_t UEP6_MAX_LEN;
	__IO uint32_t UEP7_MAX_LEN;
	__IO uint16_t UEP0_RX_LEN;
	__IO uint16_t reserved;
	__IO uint16_t UEP1_RX_LEN;
	__IO uint16_t UEP1_R_SIZE;
	__IO uint16_t UEP2_RX_LEN;
	__IO uint16_t UEP2_R_SIZE;
	__IO uint16_t UEP3_RX_LEN;
	__IO uint16_t UEP3_R_SIZE;
	__IO uint16_t UEP4_RX_LEN;
	__IO uint16_t UEP4_R_SIZE;
	__IO uint16_t UEP5_RX_LEN;
	__IO uint16_t UEP5_R_SIZE;
	__IO uint16_t UEP6_RX_LEN;
	__IO uint16_t UEP6_R_SIZE;
	__IO uint16_t UEP7_RX_LEN;
	__IO uint16_t UEP7_R_SIZE;
	__IO uint16_t UEP0_T_LEN;
	__IO uint8_t  UEP0_TX_CTRL;
	__IO uint8_t  UEP0_RX_CTRL;
	__IO uint16_t UEP1_T_LEN;
	__IO uint8_t  UEP1_TX_CTRL;
	__IO uint8_t  UEP1_RX_CTRL;
	__IO uint16_t UEP2_T_LEN;
	__IO uint8_t  UEP2_TX_CTRL;
	__IO uint8_t  UEP2_RX_CTRL;
	__IO uint16_t UEP3_T_LEN;
	__IO uint8_t  UEP3_TX_CTRL;
	__IO uint8_t  UEP3_RX_CTRL;
	__IO uint16_t UEP4_T_LEN;
	__IO uint8_t  UEP4_TX_CTRL;
	__IO uint8_t  UEP4_RX_CTRL;
	__IO uint16_t UEP5_T_LEN;
	__IO uint8_t  UEP5_TX_CTRL;
	__IO uint8_t  UEP5_RX_CTRL;
	__IO uint16_t UEP6_T_LEN;
	__IO uint8_t  UEP6_TX_CTRL;
	__IO uint8_t  UEP6_RX_CTRL;
	__IO uint16_t UEP7_T_LEN;
	__IO uint8_t  UEP7_TX_CTRL;
	__IO uint8_t  UEP7_RX_CTRL;
	__IO uint16_t UEP_T_ISO;
	__IO uint16_t UEP_R_ISO;
} USBHS_TypeDef;

#define USBHS_CHECK_NAK_RX (USBHS->UEP0_RX_CTRL & (1<<6))

#define UEP_RX_LEN(n)   (((volatile uint16_t*)&USBHS->UEP1_RX_LEN)[(n-1)*2])
#define UEP_TX_EN(n)    USBHS->UEP_TX_EN |= ((uint16_t)(1<<n))
#define UEP_RX_EN(n)    USBHS->UEP_RX_EN |= ((uint16_t)(1<<n))

#define DEBUG_PIN PB17

#define TEST_ENABLE  0x01
#define TEST_MASK    0x0F

#define USBHS_UMS_SUSPEND  (1<<1)

#define USBHS_UC_RESET_SIE (1<<1)
#define USBHS_UC_CLR_ALL   (1<<2)
#define USBHS_UC_DMA_EN    (1<<4)
#define USBHS_UC_PORT_EN   (1<<5)

#define USBHS_UIE_SUSPEND  (1<<1)
#define USBHS_UIE_TRANSFER (1<<4)
#define USBHS_UIE_BUS_RST  (1<<0)

// Mask for the combined USBHS->INT_FG + USBHS->INT_ST
#define CRB_UIF_FIFO_OV   (1<<7)
#define CRB_UIF_LINK_RDY  (1<<6)
#define CRB_UIF_RX_SOF    (1<<5)
#define CRB_UIF_TRANSFER  (1<<4)
#define CRB_UIF_LPM_ACT   (1<<3)
#define CRB_UIF_BUS_SLEEP (1<<2)
#define CRB_UIF_SUSPEND   (1<<1)
#define CRB_UIF_BUS_RST   (1<<0)

#define CMASK_UIS_TOKEN   (1<<12)
#define CMASK_UIS_ENDP    (7<<8)

#define CUIS_TOKEN_OUT     0x0
#define CUIS_TOKEN_IN      0x1

#define USBHS_DONE_TX(n)   UEP_CTRL_TX(n) &= ~(USBHS_UEP_T_DONE)
#define USBHS_DONE_RX(n)   UEP_CTRL_RX(n) &= ~(USBHS_UEP_R_DONE)

#endif

#define USBHS_SPEED_TYPE_MASK ((uint8_t)(0x03))

#define USBHS_DEF_UEP_IN   0x80
#define USBHS_DEF_UEP_OUT  0x00
#define USBHS_DEF_UEP_BUSY 0x01
#define USBHS_DEF_UEP_FREE 0x00

#define USBHS_NUM_EP       8
#define USBHS_DEF_UEP0     0
#define USBHS_DEF_UEP1     1
#define USBHS_DEF_UEP2     2
#define USBHS_DEF_UEP3     3
#define USBHS_DEF_UEP4     4
#define USBHS_DEF_UEP5     5
#define USBHS_DEF_UEP6     6
#define USBHS_DEF_UEP7     7
#define USBHS_DEF_UEP8     8
#define USBHS_DEF_UEP9     9
#define USBHS_DEF_UEP10    10
#define USBHS_DEF_UEP11    11
#define USBHS_DEF_UEP12    12
#define USBHS_DEF_UEP13    13
#define USBHS_DEF_UEP14    14
#define USBHS_DEF_UEP15    15

#define USBHS_DEF_UEP0_SIZE 64

#ifndef FUSB_EP_SIZE
#define USBHS_UEP_SIZE 64
#else
#define USBHS_UEP_SIZE FUSB_EP_SIZE
#endif

#define UEP_CTRL_LEN(n) (((volatile uint16_t*)&USBHS->UEP0_T_LEN)[n*2])
#define UEP_CTRL_TX(n)  (((volatile uint8_t*)&USBHS->UEP0_TX_CTRL)[n*4])
#define UEP_CTRL_RX(n)  (((volatile uint8_t*)&USBHS->UEP0_RX_CTRL)[n*4])
#define UEP_DMA_RX(n)   (((volatile uint32_t*)&USBHS->UEP0_DMA)[n])
#define UEP_DMA_TX(n)   (((volatile uint32_t*)&USBHS->UEP1_TX_DMA)[n-1])

#define USBHS           ((USBHS_TypeDef *)USBHS_BASE)

extern uint32_t USBDEBUG0, USBDEBUG1, USBDEBUG2;

struct _USBState;

// Provided functions:
int USBHSSetup();
static inline int USBHS_SendEndpointNEW( int endp, uint8_t* data, int len, int copy);
static inline uint8_t * USBHS_GetEPBufferIfAvailable( int endp );
static inline int USBHS_SendEndpoint( int endp, int len );
static inline int USBHS_SendACK( int endp, int tx );
static inline int USBHS_SendNAK( int endp, int tx );

// Implement the following:
#if FUSB_HID_USER_REPORTS
__HIGH_CODE int HandleHidUserGetReportSetup( struct _USBState * ctx, tusb_control_request_t * req );
__HIGH_CODE int HandleHidUserSetReportSetup( struct _USBState * ctx, tusb_control_request_t * req );
void HandleHidUserReportDataOut( struct _USBState * ctx, uint8_t * data, int len );
int HandleHidUserReportDataIn( struct _USBState * ctx, uint8_t * data, int len );
void HandleHidUserReportOutComplete( struct _USBState * ctx );
#endif

#if FUSB_USER_HANDLERS
__HIGH_CODE int HandleInRequest( struct _USBState * ctx, int endp, uint8_t * data, int len );
__HIGH_CODE void HandleDataOut( struct _USBState * ctx, int endp, uint8_t * data, int len );
__HIGH_CODE int HandleSetupCustom( struct _USBState * ctx, int setup_code);
#endif

typedef enum
{
	USB_SPEED_FULL = 0,
	USB_SPEED_HIGH = 1,
	USB_SPEED_LOW = 2,
} USB_SPEED_t;

typedef enum
{
	USBHS_EP_OFF = 0,
	USBHS_EP_RX  = -1,
	USBHS_EP_TX = 1,
} USBHS_EP_mode;

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
	__attribute__ ((aligned(4))) uint8_t CTRL0BUFF[64];
	__attribute__ ((aligned(4))) uint8_t ENDPOINTS[FUSB_CONFIG_EPS-1][FUSB_EP_SIZE];
	// Setup Request
	uint8_t  USBHS_SetupReqCode;
	uint8_t  USBHS_SetupReqType;
	uint16_t USBHS_SetupReqLen; // Used for tracking place along send.
	uint32_t USBHS_IndexValue;

	// USB Device Status
	uint8_t  USBHS_DevConfig;
	uint8_t  USBHS_DevAddr;
	uint8_t  USBHS_DevSleepStatus;
	uint8_t  USBHS_DevEnumStatus;

	uint8_t* pCtrlPayloadPtr;

	
	USBHS_EP_mode endpoint_mode[FUSB_CONFIG_EPS+1]; // IN -1, OUT 1, OFF 0

	#define pUSBHS_SetupReqPak      ((tusb_control_request_t*)USBHSCTX.CTRL0BUFF)

#if FUSB_HID_INTERFACES > 0
	uint8_t USBHS_HidIdle[FUSB_HID_INTERFACES];
	uint8_t USBHS_HidProtocol[FUSB_HID_INTERFACES];
#endif
	volatile uint8_t USBHS_Endp_Busy[FUSB_CONFIG_EPS];
	volatile uint8_t USBHS_errata_dont_send_endpoint_in_window;
	volatile uint64_t USBHS_sof_timestamp;
};

extern struct _USBState USBHSCTX;

#include "hsusb.c"

#endif


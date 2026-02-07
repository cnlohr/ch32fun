#include "usbd.h"
#include <string.h>

// loosely based on https://github.com/ArcaneNibble/wch-uf2/blob/main/bootloader.c
// this would not exist if it was not for ArcaneNibble's efforts (I don't know how else to credit that)

struct _USBState USBDCTX;

#define BTABLE_OFFSET 0

static void PMA_Write(uint16_t pma_addr, const uint8_t *buffer, int len) {
	volatile uint32_t *pma = (volatile uint32_t *)(USBD_PMA_BASE + pma_addr);
	for (int i = 0; i < len; i += 2) {
		uint16_t val = buffer[i];
		if (i + 1 < len) {
			val |= (uint16_t)buffer[i + 1] << 8;
		}
		*pma++ = val;
	}
}

static void PMA_Read(uint16_t pma_addr, uint8_t *buffer, int len) {
	volatile uint32_t *pma = (volatile uint32_t *)(USBD_PMA_BASE + pma_addr);
	for (int i = 0; i < len; i += 2) {
		uint32_t val = *pma++;
		buffer[i] = val & 0xFF;
		if (i + 1 < len) {
			buffer[i + 1] = (val >> 8) & 0xFF;
		}
	}
}

static void SetPMA_TxCount(int ep, int count) {
	int pma_idx = (ep *4) + 1;
	((uint32_t*)USBD_PMA_BASE)[pma_idx] = count;
}

static int GetPMA_RxCount(int ep) {
	int pma_idx = (ep *4) + 3;
	return ((uint32_t*)USBD_PMA_BASE)[pma_idx] & 0x3FF;
}

static inline void SetEPR_Status(int ep, uint16_t mask, uint16_t value) {
	uint16_t reg = USBD->EPR[ep];
	uint16_t current_stat = reg & mask;
	uint16_t toggle = current_stat ^ value;
	
	uint16_t write_val = (reg & (USBD_EPR_EA | USBD_EPR_EP_TYPE_MASK | USBD_EPR_EP_KIND));
	write_val |= toggle;
	write_val |= (USBD_EPR_CTR_RX | USBD_EPR_CTR_TX);
	
	USBD->EPR[ep] = write_val;
}

static inline void SetEPR_TxStatus(int ep, uint16_t status) {
	SetEPR_Status(ep, USBD_EPR_STAT_TX_MASK, status);
}

static inline void SetEPR_RxStatus(int ep, uint16_t status) {
	SetEPR_Status(ep, USBD_EPR_STAT_RX_MASK, status);
}

// ISR
void USB_LP_CAN1_RX0_IRQHandler(void) __attribute__((interrupt));
void USB_LP_CAN1_RX0_IRQHandler(void) {
	uint32_t istr = USBD->ISTR;

	if (istr & USBD_ISTR_CTR) {
		// Correct Transfer
		int ep = istr & USBD_ISTR_EP_ID;
		uint32_t epr = USBD->EPR[ep];

		if (epr & USBD_EPR_CTR_RX) {
			// Setup or Out
			if (ep == 0 && (epr & USBD_EPR_SETUP)) {
				// SETUP Packet
				PMA_Read(USBDCTX.pma_offset[0][1], CTRL0BUFF, 8);

				USBDCTX.pCtrlPayloadPtr = 0;

				// Parse
				USBDCTX.USBD_SetupReqType = pUSBD_SetupReqPak->bmRequestType;
				USBDCTX.USBD_SetupReqCode = pUSBD_SetupReqPak->bRequest;
				USBDCTX.USBD_SetupReqLen = pUSBD_SetupReqPak->wLength;
				USBDCTX.USBD_IndexValue = (pUSBD_SetupReqPak->wIndex << 16) | pUSBD_SetupReqPak->wValue;

				int len = 0;

				if ((USBDCTX.USBD_SetupReqType & USB_REQ_TYP_MASK) == USB_REQ_TYP_STANDARD) {
					switch (USBDCTX.USBD_SetupReqCode) {
					case USB_GET_DESCRIPTOR:
						// Simple Descriptor Search (User must define descriptor_list and DESCRIPTOR_LIST_ENTRIES)
						const struct descriptor_list_struct *e = descriptor_list;
						const struct descriptor_list_struct *e_end = e + DESCRIPTOR_LIST_ENTRIES;
						int found = 0;
						for (; e != e_end; e++) {
							if (e->lIndexValue == USBDCTX.USBD_IndexValue) {
								USBDCTX.pCtrlPayloadPtr = (uint8_t*)e->addr;
								len = e->length;
								found = 1;
								break;
							}
						}
						if(!found) {
							goto stall;
						}
						
						if (len > USBDCTX.USBD_SetupReqLen) {
							len = USBDCTX.USBD_SetupReqLen;
						}
						USBDCTX.USBD_SetupReqLen = len;
						
						// Send first chunk
						int send_len = (len > DEF_USBD_UEP0_SIZE) ? DEF_USBD_UEP0_SIZE : len;
						PMA_Write(USBDCTX.pma_offset[0][0], USBDCTX.pCtrlPayloadPtr, send_len);
						SetPMA_TxCount(0, send_len);
						SetEPR_TxStatus(0, USBD_EPR_STAT_TX_VALID);
						USBDCTX.pCtrlPayloadPtr += send_len;
						USBDCTX.USBD_SetupReqLen -= send_len;
						break;

					case USB_SET_ADDRESS:
						USBDCTX.USBD_DevAddr = (uint8_t)(USBDCTX.USBD_IndexValue & 0xFF);
						// Zero Length Packet IN
						SetPMA_TxCount(0, 0);
						SetEPR_TxStatus(0, USBD_EPR_STAT_TX_VALID);
						break;

					case USB_SET_CONFIGURATION:
						 USBDCTX.USBD_DevConfig = (uint8_t)(USBDCTX.USBD_IndexValue & 0xFF);
						 USBDCTX.USBD_DevEnumStatus = 1;
						 SetPMA_TxCount(0, 0);
						 SetEPR_TxStatus(0, USBD_EPR_STAT_TX_VALID);
						 break;

					case USB_GET_STATUS:
						CTRL0BUFF[0] = 0x00;
						CTRL0BUFF[1] = 0x00;
						PMA_Write(USBDCTX.pma_offset[0][0], CTRL0BUFF, 2);
						SetPMA_TxCount(0, 2);
						SetEPR_TxStatus(0, USBD_EPR_STAT_TX_VALID);
						// We sent everything, set remaining len to 0 so CTR_TX enables RX for Status Stage
						USBDCTX.USBD_SetupReqLen = 0; 
						break;

					case USB_GET_INTERFACE:
						CTRL0BUFF[0] = 0x00;
						PMA_Write(USBDCTX.pma_offset[0][0], CTRL0BUFF, 1);
						SetPMA_TxCount(0, 1);
						SetEPR_TxStatus(0, USBD_EPR_STAT_TX_VALID);
						USBDCTX.USBD_SetupReqLen = 0; 
						break;

					case USB_GET_CONFIGURATION:
						CTRL0BUFF[0] = USBDCTX.USBD_DevConfig;
						PMA_Write(USBDCTX.pma_offset[0][0], CTRL0BUFF, 1);
						SetPMA_TxCount(0, 1);
						SetEPR_TxStatus(0, USBD_EPR_STAT_TX_VALID);
						USBDCTX.USBD_SetupReqLen = 0; 
						break;

					default:
						// Basic ACK for unhandled standards
						SetPMA_TxCount(0, 0);
						SetEPR_TxStatus(0, USBD_EPR_STAT_TX_VALID);
						break;
					}
				}
				else {
					// Vendor/Class
#if FUSB_USER_HANDLERS
					len = HandleSetupCustom(&USBDCTX, USBDCTX.USBD_SetupReqCode);
					if (len >= 0) {
						if (USBDCTX.USBD_SetupReqType & DEF_UEP_IN) {
							USBDCTX.USBD_SetupReqLen = len;
							int send_len = (len > DEF_USBD_UEP0_SIZE) ? DEF_USBD_UEP0_SIZE : len;
							
							if(USBDCTX.pCtrlPayloadPtr) {
								PMA_Write(USBDCTX.pma_offset[0][0], USBDCTX.pCtrlPayloadPtr, send_len);
							}
							else {
								PMA_Write(USBDCTX.pma_offset[0][0], CTRL0BUFF, send_len);
							}

							SetPMA_TxCount(0, send_len);
							SetEPR_TxStatus(0, USBD_EPR_STAT_TX_VALID);
							USBDCTX.pCtrlPayloadPtr += send_len;
							USBDCTX.USBD_SetupReqLen -= send_len;
						}
						else {
							// OUT Data expected or Just Status IN
							if (USBDCTX.USBD_SetupReqLen > 0) {
								SetEPR_RxStatus(0, USBD_EPR_STAT_RX_VALID);
								SetEPR_TxStatus(0, USBD_EPR_STAT_TX_NAK);
							}
							else {
								SetPMA_TxCount(0, 0);
								SetEPR_TxStatus(0, USBD_EPR_STAT_TX_VALID);
							}
						}
					}
					else {
						goto stall;
					}
#else
					goto stall;
#endif
				}
			}
			else {
				// OUT Packet
				int rx_len = GetPMA_RxCount(ep);
				PMA_Read(USBDCTX.pma_offset[ep][1], USBDCTX.ENDPOINTS[ep], rx_len);

				if (ep == 0) {
					if(USBDCTX.USBD_SetupReqLen > 0) {
						// Data Stage OUT

						USBDCTX.USBD_SetupReqLen -= rx_len;
						// Handle Data...
#if FUSB_USER_HANDLERS
						HandleDataOut(&USBDCTX, 0, USBDCTX.ENDPOINTS[0], rx_len);
#endif
						if (USBDCTX.USBD_SetupReqLen <= 0) {
							// Status Stage IN
							USBDCTX.USBD_SetupReqLen = 0; // Ensure clean state
							SetPMA_TxCount(0, 0);
							SetEPR_TxStatus(0, USBD_EPR_STAT_TX_VALID);
							SetEPR_RxStatus(0, USBD_EPR_STAT_RX_VALID);
						}
						else {
							SetEPR_RxStatus(0, USBD_EPR_STAT_RX_VALID);
						}
					}
					else {
						// Status Stage OUT (for IN transfer)
						// Just ACK
						SetEPR_RxStatus(0, USBD_EPR_STAT_RX_VALID);
					}
				}
				else {
#if FUSB_USER_HANDLERS
					HandleDataOut(&USBDCTX, ep, USBDCTX.ENDPOINTS[ep], rx_len);
#endif
					SetEPR_RxStatus(ep, USBD_EPR_STAT_RX_VALID);
				}
			}

			// Clear CTR_RX (Write 0 to RX, 1 to TX)
			uint16_t wVal = USBD->EPR[ep] & (USBD_EPR_EA | USBD_EPR_EP_TYPE_MASK | USBD_EPR_EP_KIND);
			USBD->EPR[ep] = wVal | USBD_EPR_CTR_TX;
		}

		if (epr & USBD_EPR_CTR_TX) {
			// IN Complete
			USBDCTX.USBD_Endp_Busy[ep] = 0;
			
			if (ep == 0) {
				if (USBDCTX.USBD_SetupReqCode == USB_SET_ADDRESS) {
					USBD->DADDR = 0x80 | USBDCTX.USBD_DevAddr;
				}

				if (USBDCTX.USBD_SetupReqLen > 0) {
					// More data to send
					int send_len = (USBDCTX.USBD_SetupReqLen > DEF_USBD_UEP0_SIZE) ? DEF_USBD_UEP0_SIZE : USBDCTX.USBD_SetupReqLen;
					PMA_Write(USBDCTX.pma_offset[0][0], USBDCTX.pCtrlPayloadPtr, send_len);
					SetPMA_TxCount(0, send_len);
					SetEPR_TxStatus(0, USBD_EPR_STAT_TX_VALID);
					USBDCTX.pCtrlPayloadPtr += send_len;
					USBDCTX.USBD_SetupReqLen -= send_len;
				}
				else {
					// End of transfer, ready for next SETUP
					SetEPR_RxStatus(0, USBD_EPR_STAT_RX_VALID);
				}
			}
			else {
#if FUSB_USER_HANDLERS
				HandleInRequest(&USBDCTX, ep, USBDCTX.ENDPOINTS[ep], 0);
#endif
			}

			// Clear CTR_TX (Write 0 to TX, 1 to RX)
			uint16_t wVal = USBD->EPR[ep] & (USBD_EPR_EA | USBD_EPR_EP_TYPE_MASK | USBD_EPR_EP_KIND);
			USBD->EPR[ep] = wVal | USBD_EPR_CTR_RX;
		}

		USBD->ISTR = ~USBD_ISTR_CTR;
		return;

	stall:
		SetEPR_TxStatus(0, USBD_EPR_STAT_TX_STALL);
		SetEPR_RxStatus(0, USBD_EPR_STAT_RX_STALL);
		// Clear CTR
		uint16_t wVal = (epr & (USBD_EPR_EA | USBD_EPR_EP_TYPE_MASK | USBD_EPR_EP_KIND)); 
		USBD->EPR[ep] = wVal; 
	}

	// Check for Reset
	if (istr & USBD_ISTR_RESET) {
		USBD->ISTR = ~USBD_ISTR_RESET;

		// Reset Logic
		USBD->BTABLE = BTABLE_OFFSET;
		USBDCTX.USBD_DevConfig = 0;
		USBDCTX.USBD_DevAddr = 0;
		USBDCTX.USBD_DevEnumStatus = 0;

		// Initialize Endpoints
		for(int i = 0; i < FUSB_CONFIG_EPS; i++) {
			USBDCTX.USBD_Endp_Busy[i] = 0;
			// Clear EPR
			USBD->EPR[i] = i;
		}

		// Configure EP0
		SetEPR_Status(0, USBD_EPR_EP_TYPE_MASK, USBD_EPR_EP_TYPE_CTRL);
		SetEPR_Status(0, USBD_EPR_STAT_RX_MASK, USBD_EPR_STAT_RX_VALID);
		SetEPR_Status(0, USBD_EPR_STAT_TX_MASK, USBD_EPR_STAT_TX_NAK);

		// Configure Other EPs based on FUSB_EPx_MODE
		for(int i = 1; i < FUSB_CONFIG_EPS; i++) {
			if (USBDCTX.endpoint_mode[i] != USBD_EP_OFF) {
				// Set type Bulk (default for now, can be changed)
				SetEPR_Status(i, USBD_EPR_EP_TYPE_MASK, USBD_EPR_EP_TYPE_BULK);
				// Set address
				USBD->EPR[i] = (USBD->EPR[i] & ~USBD_EPR_EA) | i;

				if(USBDCTX.endpoint_mode[i] == USBD_EP_RX) {
					SetEPR_RxStatus(i, USBD_EPR_STAT_RX_VALID);
					SetEPR_TxStatus(i, USBD_EPR_STAT_TX_DIS);
				}
				else if(USBDCTX.endpoint_mode[i] == USBD_EP_TX) {
					SetEPR_RxStatus(i, USBD_EPR_STAT_RX_DIS);
					SetEPR_TxStatus(i, USBD_EPR_STAT_TX_NAK);
				}
			}
		}

		USBD->DADDR = 0x80; // Enable Function
	}

	// Check for Suspend
	if (istr & USBD_ISTR_SUSP) {
		USBD->ISTR = ~USBD_ISTR_SUSP;
		USBDCTX.USBD_DevSleepStatus |= 0x02;
		// Optional: Enter Low Power Mode here if FUSB_SUPPORTS_SLEEP is set
		// USBD->CNTR |= USBD_CNTR_FSUSP;
	}

	// Check for Wakeup
	if (istr & USBD_ISTR_WKUP) {
		USBD->ISTR = ~USBD_ISTR_WKUP;
		USBDCTX.USBD_DevSleepStatus &= ~0x02;
		// USBD->CNTR &= ~USBD_CNTR_FSUSP;
	}

	// Check for ESOF (Expected Start Of Frame)
	if (istr & USBD_ISTR_ESOF) {
		USBD->ISTR = ~USBD_ISTR_ESOF;
	}

	// Check for SOF
	if (istr & USBD_ISTR_SOF) {
		USBD->ISTR = ~USBD_ISTR_SOF;
	}

	// Error!
	if (istr & USBD_ISTR_ERR) {
		USBD->ISTR = ~USBD_ISTR_ERR;
	}

	return;
}

void USBD_InternalFinishSetup() {
	// Set BTABLE Address
	USBD->BTABLE = BTABLE_OFFSET;

	USBDCTX.endpoint_mode[0] = USBD_EP_RTX;
	// Config from defines
#if FUSB_EP1_MODE
	USBDCTX.endpoint_mode[1] = FUSB_EP1_MODE;
#endif
#if FUSB_EP2_MODE
	USBDCTX.endpoint_mode[2] = FUSB_EP2_MODE;
#endif
#if FUSB_EP3_MODE
	USBDCTX.endpoint_mode[3] = FUSB_EP3_MODE;
#endif
#if FUSB_EP4_MODE
	USBDCTX.endpoint_mode[4] = FUSB_EP4_MODE;
#endif
#if FUSB_EP5_MODE
	USBDCTX.endpoint_mode[5] = FUSB_EP5_MODE;
#endif
#if FUSB_EP6_MODE
	USBDCTX.endpoint_mode[6] = FUSB_EP6_MODE;
#endif
#if FUSB_EP7_MODE
	USBDCTX.endpoint_mode[7] = FUSB_EP7_MODE;
#endif

	uint16_t pma_ptr = 128; // start packet buffers after btable (8 endpoint with 16 bytes each)
	for (int i = 0; i < FUSB_CONFIG_EPS; i++) {
		// Calculate offsets
		uint32_t* btable_entry = (uint32_t*)(USBD_PMA_BASE + (i*16)); // 4 entries = 16 bytes per ep in btable

		// USBD RAM is 512 bytes
		if (pma_ptr > 512 -DEF_USBD_UEP0_SIZE) {
			printf("CRITICAL: USBD PMA OVERFLOW (%d)\n", pma_ptr);
			pma_ptr -= DEF_USBD_UEP0_SIZE;
		}

		if (USBDCTX.endpoint_mode[i] == USBD_EP_TX || USBDCTX.endpoint_mode[i] == USBD_EP_RTX) {
			USBDCTX.pma_offset[i][0] = pma_ptr *2; // *2 because this weird uint16 writes through uint32 pointers
			btable_entry[0] = pma_ptr; // ADDRx_TX
			btable_entry[1] = 0; // COUNTx_TX
			if(USBDCTX.endpoint_mode[i] != USBD_EP_RTX) {
				// bidirectional endpoints share the buffer, pma can hold only 6
				pma_ptr += DEF_USBD_UEP0_SIZE;
			}
		}

		if (USBDCTX.endpoint_mode[i] == USBD_EP_RX || USBDCTX.endpoint_mode[i] == USBD_EP_RTX) {
			USBDCTX.pma_offset[i][1] = pma_ptr *2; // *2 because this weird uint16 writes through uint32 pointers
			btable_entry[2] = pma_ptr; // ADDRx_RX
			btable_entry[3] = 0x8400; // COUNTx_RX BL_SIZE=1 (32byte), NUM_BLOCK=1 -> 64 bytes
			pma_ptr += DEF_USBD_UEP0_SIZE;
		}
	}
}

int USBDSetup() {
	// Enable Clocks
#ifdef CH32V20x
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

	// As recommended in the manual, output low on these pins before enabling USB
	RCC->APB2PCENR |= (1 << 2);
	GPIOA->CFGHR = (GPIOA->CFGHR & ~(0xff << 12)) | (0b00100010 << 12);
	GPIOA->BSHR = (1 << 27) | (1 << 28);

	RCC->APB2PCENR |= RCC_AFIOEN | RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOA;
	RCC->AHBPCENR = RCC_AHBPeriph_SRAM;
	RCC->APB1PCENR |= RCC_USBEN;
#else
#warning USBD is only tested on CH32V20x, on anything else you are trailblazing
#endif

	// Power On
	USBD->CNTR = USBD_CNTR_FRES; // Force Reset
	USBD->CNTR = 0;

	// Wait
	for(volatile int i=0; i<1000; i++);

	USBD_InternalFinishSetup();

	EXTEN->EXTEN_CTR |= EXTEN_USBD_PU_EN;

	USBD->CNTR = USBD_CNTR_CTRM | USBD_CNTR_RESETM | USBD_CNTR_SUSPM | USBD_CNTR_WKUPM;
	USBD->ISTR = 0; // reset all interrupt flags
	NVIC_EnableIRQ(USB_LP_CAN1_RX0_IRQn);

	return 0;
}

int USBD_SendEndpoint(int endp, uint8_t *data, int len) {
	if (USBDCTX.USBD_Endp_Busy[endp]) return -1;

	USBDCTX.USBD_Endp_Busy[endp] = 1;

	PMA_Write(USBDCTX.pma_offset[endp][0], data, len);
	SetPMA_TxCount(endp, len);
	SetEPR_TxStatus(endp, USBD_EPR_STAT_TX_VALID);

	return 0;
}

#if defined( FUNCONF_USE_USBPRINTF ) && FUNCONF_USE_USBPRINTF
int HandleInRequest( struct _USBState *ctx, int endp, uint8_t *data, int len ) {
	return 0;
}

static uint8_t usb_inputbuffer[DEF_USBD_UEP0_SIZE]; // this can be extended if polling rate is low
static int usb_inbuf_idx;
void HandleDataOut( struct _USBState *ctx, int endp, uint8_t *data, int len ) {
	if ( endp == 0 ) {
		ctx->USBD_SetupReqLen = 0; // To ACK
	}
	else if( endp == 2 ) {
		// discard oldest if polling is too slow
		int headroom = (sizeof(usb_inputbuffer) - usb_inbuf_idx) - len;
		if(headroom < 0) {
			// not enough space left, free up some
			int offset = -headroom;
			for(int i = offset; i < sizeof(usb_inputbuffer); i++) {
				usb_inputbuffer[i -offset] = usb_inputbuffer[i];
			}
			usb_inbuf_idx -= offset;
		}

		for(int i = 0; i < len; i++) {
			usb_inputbuffer[usb_inbuf_idx++] = data[i];
		}
	}
}

void handle_usbd_input( int numbytes, uint8_t * data );
void poll_input() {
	if(usb_inbuf_idx) {
		handle_usbd_input(usb_inbuf_idx, usb_inputbuffer);
		usb_inbuf_idx = 0;
	}
}

int HandleSetupCustom( struct _USBState *ctx, int setup_code ) {
	int ret = -1;
	if ( ctx->USBD_SetupReqType & USB_REQ_TYP_CLASS ) {
		switch ( setup_code ) {
			case CDC_SET_LINE_CODING:
			case CDC_SET_LINE_CTLSTE:
			case CDC_SEND_BREAK: ret = ( ctx->USBD_SetupReqLen ) ? ctx->USBD_SetupReqLen : -1; break;
			case CDC_GET_LINE_CODING: ret = ctx->USBD_SetupReqLen; break;
			default: ret = 0; break;
		}
	}
	else {
		ret = 0; // Go to STALL
	}
	return ret;
}
#endif // FUNCONF_USE_USBPRINTF

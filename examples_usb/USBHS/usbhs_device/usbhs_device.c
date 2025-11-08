#include "ch32fun.h"
#include <stdio.h>
#include <string.h>
#include "hsusb.h"

#if defined(CH32V30x)
#define LED PA15
#define LED_ON 1
#else
#define LED PB8
#define LED_ON 0
#endif

#if (FUSB_SOF_HSITRIM)
uint32_t last_trim = 0;
#endif

uint32_t count;

int last = 0;
void handle_debug_input( int numbytes, uint8_t * data )
{
	last = data[0];
	count += numbytes;
}

int lrx = 0;
__attribute__ ((aligned(4))) uint8_t scratchpad[16384];

int HandleHidUserSetReportSetup( struct _USBState * ctx, tusb_control_request_t * req )
{
	int id = req->wValue & 0xff;
	if( id == 0xaa && req->wLength <= sizeof(scratchpad) )
	{
		ctx->pCtrlPayloadPtr = scratchpad;
		lrx = req->wLength;
		return req->wLength;
	}
	return 0;
}

int HandleHidUserGetReportSetup( struct _USBState * ctx, tusb_control_request_t * req )
{
	int id = req->wValue & 0xff;
	if( id == 0xaa )
	{
		ctx->pCtrlPayloadPtr = scratchpad;
		if( sizeof(scratchpad) - 1 < lrx )
			return sizeof(scratchpad) - 1;
		else
			return lrx;
	}
	return 0;
}

void HandleHidUserReportDataOut( struct _USBState * ctx, uint8_t * data, int len )
{
}

int HandleHidUserReportDataIn( struct _USBState * ctx, uint8_t * data, int len )
{
	return len;
}

void HandleHidUserReportOutComplete( struct _USBState * ctx )
{
	return;
}

int HandleInRequest( struct _USBState * ctx, int endp, uint8_t * data, int len )
{
	return 0;
}

int HandleSetupCustom( struct _USBState * ctx, int setup_code)
{
	return 0;
}

void HandleDataOut( struct _USBState * ctx, int endp, uint8_t * data, int len )
{
	if( endp == 5 )
	{
		USBHSCTX.USBHS_Endp_Busy[5] = 0;
		// Received data is written into scratchpad,
		// and USBHSD->RX_LEN

		//printf( "%d\n", USBHSD->RX_LEN );
	}
}

__HIGH_CODE
static __attribute__((noreturn)) void processLoop()
{
	int tickcount = 0;
	while(1)
	{
		//printf( "%lu %08lx %lu %d %d\n", USBDEBUG0, USBDEBUG1, USBDEBUG2, 0, 0 );

		// Send data back to PC.
		if( !( USBHSCTX.USBHS_Endp_Busy[4] & 1 ) )
		{
			USBHS_SendEndpointNEW( 4, scratchpad, 512, 0 );
		}

		int i;
		for( i = 1; i < 3; i++ )
		{
			uint32_t * buffer = (uint32_t*)USBHS_GetEPBufferIfAvailable( i );
			if( buffer )
			{
				int tickDown = ((SysTick->CNT)&0x800000);
				static int wasTickMouse, wasTickKeyboard;
				if( i == 1 )
				{
					// Keyboard  (only press "8" 3 times)
					if( tickcount <= 3 && tickDown && !wasTickKeyboard )
					{
						buffer[0] = 0x00250000;
						tickcount++;
					}
					else
					{
						buffer[0] = 0x00000000;
					}
					buffer[1] = 0x00000000;
					wasTickKeyboard = tickDown;
				}
				else
				{
					buffer[0] = (tickDown && !wasTickMouse)?0x0010100:0x00000000;
					wasTickMouse = tickDown;
				}
				USBHS_SendEndpoint( i, (i==1)?8:4 );
			}
		}
#if defined(FUSB_SOF_HSITRIM) && (FUSB_SOF_HSITRIM)
		if (last_trim != RCC->CTLR) {
			last_trim = RCC->CTLR;
			printf("New HSITRIM value = %ld\n", (last_trim&RCC_HSITRIM)>>3);
		}
#endif
	}
}

int main()
{
	SystemInit();

	funGpioInitAll();

	funPinMode( LED, GPIO_CFGLR_OUT_10Mhz_PP );
	funDigitalWrite( LED, !LED_ON );

	printf("USBHS starting...");

	USBHSSetup();

	printf("ok\n");

	funDigitalWrite( LED, LED_ON );

	// Override EP5 buffer
	UEP_DMA_RX(5) = (uintptr_t)scratchpad;

#if defined(FUSB_SOF_HSITRIM) && (FUSB_SOF_HSITRIM)
	last_trim = RCC->CTLR;
	printf("HSITRIM value = %ld\n", (last_trim&RCC_HSITRIM)>>3);
#endif

	processLoop();
}


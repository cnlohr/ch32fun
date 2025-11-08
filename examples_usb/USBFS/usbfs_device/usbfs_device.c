#include "ch32fun.h"
#include <stdio.h>
#include <string.h>
#include "fsusb.h"

#if defined(CH32V30x)
#define LED PA15
#define LED_ON 1
#elif defined(CH5xx)
#define LED PA8
#define LED_ON 0
#elif defined(CH32V10x)
#define LED PC8
#define LED_ON 0
#elif defined(CH32X03x)
#define LED PB12
#define LED_ON 0
#else
#define LED PB2
#define LED_ON 1
#endif

uint32_t count;

int last = 0;
void handle_debug_input( int numbytes, uint8_t * data )
{
	last = data[0];
	count += numbytes;
}

int lrx = 0;
uint8_t scratchpad[256];

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
//	printf( "IN %d %d %08x %08x\n", len, ctx->USBFS_SetupReqLen, data, FSUSBCTX.ENDPOINTS[0] );
//	memset( data, 0xcc, len );
	return len;
}

void HandleHidUserReportOutComplete( struct _USBState * ctx )
{
	return;
}

__USBFS_FUN_ATTRIBUTE
static __attribute__((noreturn)) void processLoop()
{
	int tickcount = 0;
	while(1)
	{
		//printf( "%lu %08lx %lu %d %d\n", USBDEBUG0, USBDEBUG1, USBDEBUG2, 0, 0 );

		int i;
		for( i = 1; i < 3; i++ )
		{
			uint32_t * buffer = (uint32_t*)USBFS_GetEPBufferIfAvailable( i );
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
				USBFS_SendEndpoint( i, (i==1)?8:4 );
			}
		}
	}
}

int main()
{
	SystemInit();

	funGpioInitAll();

	funPinMode( LED, GPIO_CFGLR_OUT_10Mhz_PP );
	funDigitalWrite( LED, !LED_ON );

	printf("USBFS starting...");

	USBFSSetup();

	printf("ok\n");

	funDigitalWrite( LED, LED_ON );

	processLoop();
}


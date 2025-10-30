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

uint32_t count;

int last = 0;
void handle_debug_input( int numbytes, uint8_t * data )
{
	last = data[0];
	count += numbytes;
}

int lrx = 0;
#define ADCSAMPBUF 4096
__attribute__ ((aligned(4))) uint16_t adcSamples[ADCSAMPBUF];
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
	while(1)
	{
		//printf( "%lu %08lx %lu %d %d\n", USBDEBUG0, USBDEBUG1, USBDEBUG2, 0, 0 );

		static unsigned int last_hindex;

		// Send data back to PC.
		static int lastdelta = 0;
		if( !( USBHSCTX.USBHS_Endp_Busy[4] & 1 ) )
		{
			unsigned int hindex = (((((uint32_t)R16_ADC_DMA_NOW)-((uint32_t)(uint8_t*)adcSamples)))/2) &  (ADCSAMPBUF-1);
			unsigned int delta = (hindex - last_hindex) & (ADCSAMPBUF-1);

			if( hindex < last_hindex )
			{
				// Only read to end.
				delta = ADCSAMPBUF - last_hindex;
			}

			delta &= 0xfc;

			if( delta > 256 ) delta = 256;
			if( delta > 0 )
			{
				USBHS_SendEndpointNEW( 4, (uint8_t*)&adcSamples[last_hindex/2], delta*2, 0 );
				lastdelta = delta;
				last_hindex = (last_hindex+delta)&(ADCSAMPBUF-1);
			}
		}

static int check;
unsigned int hindex = (((((uint32_t)R16_ADC_DMA_NOW)-((uint32_t)(uint8_t*)adcSamples)))/2) &  (ADCSAMPBUF-1);
if( check++ & 0x1fff ); else printf( "%d %d %d %d\n", last_hindex, lastdelta, hindex, USBHSCTX.USBHS_Endp_Busy[4]  );

		int i;
		for( i = 1; i < 3; i++ )
		{
			uint32_t * buffer = (uint32_t*)USBHS_GetEPBufferIfAvailable( i );
			if( buffer )
			{
				int tickDown = ((SysTick->CNT)&0x800000);
				static int wasTickMouse;
				if( i == 1 )
				{
					buffer[0] = 0x00000000;
					buffer[1] = 0x00000000;
				}
				else
				{
					buffer[0] = (tickDown && !wasTickMouse)?0x0010100:0x00000000;
					wasTickMouse = tickDown;
				}
				USBHS_SendEndpoint( i, (i==1)?8:4 );
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

	printf("Configuring ADC...\n");

	printf( "R16_CLK_SYS_CFG[9] = %x\n", (R16_CLK_SYS_CFG>>9)&1 );
	printf( "(R8_ADC_CFG) = %02x\n", (R8_ADC_CFG) );

	// Enable ADC
	//R8_ADC_CFG = 0xa3;
	R8_ADC_CFG = 0xa3; // go real fast?

	printf( "(R8_ADC_CFG) = %02x\n", (R8_ADC_CFG) );

	// Enable DMA on ADC
	R8_ADC_CTRL_DMA = 0xcd;

	R8_ADC_AUTO_CYCLE = 40;

	// Sample only ADC0 (PA4)
	R32_ADC_SCAN_CFG1 = 0x00000000;
	R32_ADC_SCAN_CFG2 = 0x00000000;

	R16_ADC_DMA_BEG = adcSamples;
	R16_ADC_DMA_END = adcSamples + (sizeof( adcSamples ) / sizeof(adcSamples[0]));

	// Start conversion
	R8_ADC_CONVERT = 1;

	USBHSSetup();

	printf("ok\n");

	funDigitalWrite( LED, LED_ON );

	// Override EP5 buffer
	UEP_DMA_RX(5) = (uintptr_t)scratchpad;

	processLoop();
}


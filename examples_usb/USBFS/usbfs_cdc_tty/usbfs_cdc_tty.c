#include "ch32fun.h"
#include <stdio.h>
#include <string.h>
#include "fsusb.h"

#if defined(CH57x) && (MCU_PACKAGE == 0 || MCU_PACKAGE == 2) // ch570/2
#define PIN_LED    PA9
#define PIN_BUTTON PA1
#define BUTTON_PRESSED funDigitalRead( PIN_BUTTON )
#else
#define PIN_LED    PA8
#define PIN_BUTTON PB22
#define BUTTON_PRESSED !funDigitalRead( PIN_BUTTON )
#endif

uint8_t run = 1; // print stuff or not

void blink(int n) {
	for(int i = n-1; i >= 0; i--) {
		funDigitalWrite( PIN_LED, FUN_LOW ); // Turn on LED
		Delay_Ms(33);
		funDigitalWrite( PIN_LED, FUN_HIGH ); // Turn off LED
		if(i) Delay_Ms(33);
	}
}

int HandleInRequest( struct _USBState * ctx, int endp, uint8_t * data, int len )
{
	return 0;
}

void HandleDataOut( struct _USBState * ctx, int endp, uint8_t * data, int len )
{
	if( endp == 0 )
	{
		ctx->USBFS_SetupReqLen = 0; // To ACK
		if( ctx->USBFS_SetupReqCode == CDC_SET_LINE_CODING )
		{
		}
	}
	if( endp == 2 )
	{
		if(len == 1) {
			switch(*data) {
			case '1':
				blink(1);
				break;
			case '2':
				blink(2);
				break;
			case '3':
				blink(3);
				break;
			case 'c':
				run = 1;
				break;
			case 'p':
				run = 0;
				break;
			default:
				break;
			}
		}
	}
}

int HandleSetupCustom( struct _USBState * ctx, int setup_code)
{
	int ret = -1;
	if( ctx->USBFS_SetupReqType & USB_REQ_TYP_CLASS )
	{
		switch( setup_code )
		{
			case CDC_SET_LINE_CODING:
			case CDC_SET_LINE_CTLSTE:
			case CDC_SEND_BREAK:
				ret = (ctx->USBFS_SetupReqLen) ? ctx->USBFS_SetupReqLen : -1;
				break;
			case CDC_GET_LINE_CODING:
				ret = ctx->USBFS_SetupReqLen;
				break;
			default:
				ret = 0;
				break;
		}
	}
	else if( ctx->USBFS_SetupReqType & USB_REQ_TYP_VENDOR )
	{
		/* Manufacturer request */
	}
	else
	{
		ret = 0; // Go to STALL
	}
	return ret;
}


int main()
{
	SystemInit();

	funGpioInitAll();

	funPinMode( PIN_LED,    GPIO_CFGLR_OUT_10Mhz_PP ); // Set PIN_LED to output
	funPinMode( PIN_BUTTON, GPIO_CFGLR_IN_PUPD ); // Set PIN_BUTTON to input

	if( BUTTON_PRESSED ) {
		blink(5);
		jump_isprom();
	}

	USBFSSetup();
	blink(1);

	int i = 0;
	while(1) {
		if(run) {
			printf("Press 'p' to pause counting, 'c' to continue, 1,2, or 3 to blink. %d\r", i++);
		}
		Delay_Ms(100);
	}
}

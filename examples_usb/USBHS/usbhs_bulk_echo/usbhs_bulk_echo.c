#include "ch32fun.h"
#include "hsusb.h"

#define LED               PA8
#define USB_DATA_BUF_SIZE 64

__attribute__((aligned(4))) static volatile uint8_t gs_usb_data_buf[USB_DATA_BUF_SIZE];

__HIGH_CODE
void blink(int n) {
	for(int i = n-1; i >= 0; i--) {
		funDigitalWrite( LED, FUN_LOW ); // Turn on LED
		Delay_Ms(33);
		funDigitalWrite( LED, FUN_HIGH ); // Turn off LED
		if(i) Delay_Ms(33);
	}
}

__INTERRUPT
void GPIOB_IRQHandler() {
	int status = R16_PB_INT_IF;
	R16_PB_INT_IF = status; // acknowledge

	if(status & PB8) { // PB22 interrupt comes in on PB8 if RB_PIN_INTX is set
		USBHSReset();
		blink(2);
		jump_isprom();
	}
}

void GPIOSetup() {
	funPinMode( LED, GPIO_CFGLR_OUT_2Mhz_PP );
	funPinMode( PB22, GPIO_CFGLR_IN_PU ); // Set PB22 to input, pullup as keypress is to gnd

	R16_PIN_ALTERNATE |= RB_PIN_INTX; // set PB8 interrupt to PB22
	R16_PB_INT_MODE |= (PB8 & ~PB); // edge mode, should go to ch32fun.h
	funDigitalWrite(PB22, FUN_LOW); // falling edge

	NVIC_EnableIRQ(GPIOB_IRQn);
	R16_PB_INT_IF = (PB8 & ~PB); // reset PB8/PB22 flag
	R16_PB_INT_EN |= (PB8 & ~PB); // enable PB8/PB22 interrupt
}

int HandleSetupCustom( struct _USBState * ctx, int setup_code) {
	return 0;
}

int HandleInRequest( struct _USBState * ctx, int endp, uint8_t * data, int len ) {
	return 0;
}

__HIGH_CODE
void HandleDataOut( struct _USBState * ctx, int endp, uint8_t * data, int len ) {
	// this is actually the data rx handler
	if( endp == USB_EP_RX ) {
		if(len == 4 && ((uint32_t*)data)[0] == 0x010001a2) {
			USBHSReset();
			blink(2);
			jump_isprom();
		}
		else if( gs_usb_data_buf[0] == 0 ) {
			gs_usb_data_buf[0] = len;
			mcpy_raw(&gs_usb_data_buf[1], data, data +len);
		}
		else {
			// previous usb buffer not consumed yet, what should we do?
		}

		ctx->USBHS_Endp_Busy[USB_EP_RX] = 0;
	}
}


int main() {
	SystemInit();

	funGpioInitAll(); // no-op on ch5xx
	GPIOSetup();

	USBHSSetup();

	blink(5);
	

	while(1) {
		if( gs_usb_data_buf[0] ) {
			// Send data back to PC.
			while( USBHSCTX.USBHS_Endp_Busy[USB_EP_TX] & 1 );
			USBHS_SendEndpointNEW( USB_EP_TX, &gs_usb_data_buf[1], gs_usb_data_buf[0], 0 );
			gs_usb_data_buf[0] = 0;
		}
	}
}

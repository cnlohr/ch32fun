#include "rf433_tx.h"
#include "ch32fun.h"
#include <stdint.h>

#define PIN_TX PD6
#define PIN_BUTTON PD4
#define BUTTON_PRESSED funDigitalRead( PIN_BUTTON )


const uint16_t te = 650;

struct Protocol princeton_protocol = { .bit_sync_high = 1 * te,
	.bit_sync_low = 31 * te,
	.bit_0_high = 1 * te,
	.bit_0_low = 2 * te,
	.bit_1_high = 2 * te,
	.bit_1_low = 1 * te,
	.repeats = 5 };


int main()
{
	SystemInit();

	funGpioInitAll();

	funPinMode( PIN_BUTTON, GPIO_CFGLR_IN_PUPD );

	// Initialize TX pin and make sure it doesn't transmit at the beginning
	funPinMode( PIN_TX, GPIO_Speed_10MHz | GPIO_CNF_OUT_PP );
	funDigitalWrite( PIN_TX, FUN_LOW );

	while ( 1 )
	{
		if ( BUTTON_PRESSED )
		{
			// transmit bits of code 010011111011000001001000 using the princeton protocol
			transmit_code( "010011111011000001001000", princeton_protocol, PIN_TX );
			Delay_Ms( 500 );
		}
	}
}

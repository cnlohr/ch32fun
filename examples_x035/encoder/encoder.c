#include "ch32fun.h"
#include "rotary_encoder.h"
#include <stdio.h>

int main()
{
	SystemInit();
	funGpioInitAll();

	printf( "rotary encoder example - turn the knob to see deltas\n" );

	encoder_t enc;
	encoder_init( &enc, ENCODER_TIM1_PC0_PC1 );

	while ( 1 )
	{
		int16_t delta = encoder_get_delta( &enc );
		if ( delta ) printf( "%d\n", delta );
		Delay_Ms( 10 );
	}
}

#include "ch32fun.h"
#include <stdint.h>
#include <stdio.h>

#include "hx711.h"

const uint8_t LOADCELL_DOUT_PIN = PD4;
const uint8_t LOADCELL_SCK_PIN = PD5;

static char input_buffer[64];
static int input_len = 0;

void calibration( void );

// Called when debug/serial input is received
void handle_debug_input( int numbytes, uint8_t *data )
{
    // Echo back received data
    int echo_bytes = 0;
    while ( echo_bytes < numbytes )
    {
        putchar( data[echo_bytes] );
        echo_bytes++;
    }

	// Accumulate input into buffer
	for ( int i = 0; i < numbytes && input_len < sizeof( input_buffer ) - 1; i++ )
	{
		char c = (char)data[i];
		if ( c == '\n' || c == '\r' )
		{
			input_buffer[input_len] = '\0';

			// Parse command
			if ( strcmp( input_buffer, "C" ) == 0 )
			{
				calibration();
			}
			else if ( strcmp( input_buffer, "V" ) == 0 )
			{
				int value_int = (int)( hx711_get_units( 10 ) * 100 );
				printf( "%d.%02d\n", value_int / 100, value_int % 100 );
			}
			input_len = 0;
		}
		else
		{
			input_buffer[input_len++] = c;
		}
	}
}

// Calibration routine
void calibration( void )
{
	int target_weight_int = 0; // grams
	int iterations = 3;
	int divider_accum = 0;

	hx711_set_scale( 1.0f ); // Reset scale
	hx711_tare( 1 );
	printf( "Ok\n" );

	for ( int i = 0; i < iterations; i++ )
	{
		printf( "Place known weight and enter its value (g, integer):\n" );
		// Wait for input
		input_len = 0;
		while ( 1 )
		{
			poll_input();
			if ( input_len > 0 && ( input_buffer[input_len - 1] == '\n' || input_buffer[input_len - 1] == '\r' ) )
{
				input_buffer[input_len - 1] = '\0';
				// Convert string to integer
				target_weight_int = 0;
				for ( int j = 0; input_buffer[j] && j < 6; j++ )
				{
					if ( input_buffer[j] >= '0' && input_buffer[j] <= '9' )
					{
						target_weight_int = target_weight_int * 10 + ( input_buffer[j] - '0' );
					}
				}
				break;
			}
		}
		float measured_weight = (int)( hx711_get_units( 10 ) );
		int divider = measured_weight / (float)target_weight_int;
		divider_accum += divider;

		printf( "Ok\n" );
		input_len = 0;
	}

	float divider_final = divider_accum / iterations;

	int divider_int = (int)(divider_final * 100);

	printf( "Calibration divider: %d.%02d\n", divider_int / 100, divider_int % 100 );
	hx711_set_scale( divider_final );
	printf( "Calibrated\n" );
}

int main( void )
{
	SystemInit();
	WaitForDebuggerToAttach( 60000 );

    printf( "Start\n" );

	hx711_init( LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN, 128 );
	hx711_tare( 1 );

	printf("Tara done, give commands\n");

	while ( 1 )
	{
		poll_input();
	}
}
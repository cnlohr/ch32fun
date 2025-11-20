#include "ch32fun.h"
#include <stdio.h>

#include "hx711.h"

const uint8_t LOADCELL_DOUT_PIN = PD4;
const uint8_t LOADCELL_SCK_PIN = PD5;

static char input_buffer[64];
static int input_len = 0;

void calibration( void );

// handle_debug_input: only buffer input
void handle_debug_input(int numbytes, uint8_t *data)
{
    for (int i = 0; i < numbytes && input_len < sizeof(input_buffer) - 1; i++)
    {
        char c = (char)data[i];
        input_buffer[input_len++] = c;
        if (c == '\n' || c == '\r')
        {
            input_buffer[input_len] = '\0';
        }
    }
}

// Calibration routine
void calibration( void )
{
	int target_weight_int = 100; // grams * 100
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
		uint32_t measured_weight = hx711_get_units( 10 );
		uint32_t divider = measured_weight / target_weight_int;
		divider_accum += divider;

		printf( "%d grams - Ok\n", target_weight_int );
		input_len = 0;
	}

	uint32_t divider_final = divider_accum / iterations;

	printf( "Calibration divider: %ld.%02ld\n", divider_final / 100, divider_final % 100 );
	hx711_set_scale( divider_final );
	printf( "Calibrated\n" );
}

// main loop: handle echo and command parsing
int main(void)
{
    SystemInit();
    while (!DebugPrintfBufferFree());

    funGpioInitD();

    printf("Start\n");
    hx711_init(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN, 128);
    hx711_tare(1);
    printf("Tara done, give commands\n");

    while (1)
    {
		poll_input();

        // If a command is ready
        if (input_len > 0 && (input_buffer[input_len - 1] == '\n' || input_buffer[input_len - 1] == '\r'))
        {
            // Echo back
            for (int i = 0; i < input_len; i++)
                putchar(input_buffer[i]);
            input_buffer[input_len] = '\0';

            printf("Received command: %s\n", input_buffer);

            // Parse command
            int b = strncmp(input_buffer, "C", 1);
            printf("Compare result: %d\n", b);
            if (strncmp(input_buffer, "C", 1) == 0)
            {
                printf("Calibration\n");
                calibration();
            }
            else if (strncmp(input_buffer, "V", 1) == 0)
            {
                printf("Value\n");
                uint32_t value = hx711_get_units(10);
                printf("%ld\n", value);
            }
            input_len = 0;
        }
    }
}
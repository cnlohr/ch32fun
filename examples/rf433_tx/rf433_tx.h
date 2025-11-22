
#ifndef RF433_TX_H
#define RF433_TX_H

#include "ch32fun.h"
#include <stdint.h>

struct Protocol
{
	uint16_t bit_sync_high;
	uint16_t bit_sync_low;
	uint16_t bit_0_high;
	uint16_t bit_0_low;
	uint16_t bit_1_high;
	uint16_t bit_1_low;
	int repeats;
};

void transmit_high_low( int pin, uint16_t high, uint16_t low )
{
	funDigitalWrite( pin, FUN_HIGH );
	Delay_Us( high );
	funDigitalWrite( pin, FUN_LOW );
	Delay_Us( low );
}

void transmit_code( char *bits_string, struct Protocol protocol, int pin )
{
	int count = 0;
	while ( count < protocol.repeats )
	{
		// transmit bit by bit
		for ( const char *c = bits_string; *c; c++ )
		{
			if ( *c != '0' )
			{
				transmit_high_low( pin, protocol.bit_1_high, protocol.bit_1_low );
			}
			else
			{
				transmit_high_low( pin, protocol.bit_0_high, protocol.bit_0_low );
			};
		}

		// transmit sync
		transmit_high_low( pin, protocol.bit_sync_high, protocol.bit_sync_low );
		count++;
	}
}

#endif

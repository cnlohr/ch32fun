#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>

// We borrow the combined hidapi.c from minichlink.
//
// This is for total perf testing.
//
// for this setup, this is typical:
//  950.274 KB/s PC->203 / 974.453 KB/s 203->PC

#include "hidapi.c"
#include "os_generic.h"

#include <stdarg.h>

int isterm;
int isgoboot;

int vid = 0x1209;
int pid = 0xd035;
wchar_t * ser = 0;

int main( int argc, char ** argv )
{
	char opt;
	while ((opt = getopt(argc, argv, "v:d:s:rt")) != -1)
	{
		switch (opt) {
		case 'v':
			vid = strtol( optarg, 0, -1 );
			if( vid < 0 ) goto badargs;
			break;
		case 'p':
			pid = strtol( optarg, 0, -1 );
			if( pid < 0 ) goto badargs;
			break;
		case 's':
		{
			int sl = strlen( optarg );
			ser = calloc( ( sl + 1 ) * 2, 0 );
			int i;
			for( i = 0; i < sl; i++ )
				ser[i] = optarg[i];
			break;
		}
		case 't': isterm = 1; break;
		case 'r': isgoboot = 1; break;
		default:
		badargs:
			fprintf( stderr, "Usage: [usbhidtool] [-v VID (optional)] [-p PID (optional)] [-s SERIAL_NUMBER (optional)] [-i device index] -r (reboot) -t (terminal)\n" );
			return -5;
		}
	}
	if( isterm + isgoboot != 1 ) goto badargs;

	hid_device * hd;

	double startTime = OGGetAbsoluteTime();

	do
	{
		hd = hid_open( vid, pid, ser); // third parameter is "serial"
		if( !hd )
		{
			if( OGGetAbsoluteTime() - startTime < 2 ) continue;
			fprintf( stderr, "Error: Failed to open device.\n" );
			return -4;
		}
	} while( !hd );

	uint8_t buffer[256] = { 0 };

	if( isgoboot )
	{
		memcpy( buffer, "\xe1\xbe\xef\x00\xc0\x01\xd0\x0d", 8 );
		int r = hid_send_feature_report( hd, buffer, 8 );
		printf( "Reboot hid_send_feature_report() = %d\n", r );
		usleep( 1000000 );
		return (r != 8)?-1:0;
	}
	if( isterm )
	{
		fprintf( stderr, "Terminal starting.\n" );
		while(1)
		{
			buffer[0] = 0xe2;
			int r = hid_get_feature_report( hd, buffer, sizeof(buffer) );

			if( r < 8 )
			{
				fprintf( stderr, "hid_get_feature_report error on terminal\n" );
				return -9;
			}

			int code = buffer[0];
			if( buffer[0] & 0x80 )
			{
				int ts = (buffer[0] & 0xf)-4;
				if( ts > 7 )
				{
					fprintf( stderr, "hid_get_feature_report reports invalid printf\n" );
					return -23;
				}
				int c;
				for( c = 1; c <= ts; c++ )
				{
					putchar( buffer[c] );
				}
				fflush( stdout );
			}
		}
		return 0;
	}

	hid_close( hd );
	return 0;
}


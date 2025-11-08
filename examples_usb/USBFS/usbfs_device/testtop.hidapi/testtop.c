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

int main()
{
	hid_device * hd = hid_open( 0x1209, 0xd035, 0); // third parameter is "serial"
	if( !hd )
	{
		fprintf( stderr, "Error: Failed to open device.\n" );
		return -4;
	}

	// Size of buffers must match the report descriptor size in the special_hid_desc
	//  NOTE: You are permitted to have multiple entries.
	uint8_t buffer0[255] = { 0 }; // NOTE: This must be ONE MORE THAN what is in the hid descriptor.
	uint8_t buffer1[255] = { 0 };
	int r;
	int i;
	int j;
	int retries = 0;
	double dStart = OGGetAbsoluteTime();
	double dSecond = dStart;
	double dStartSend = 0.0;
	double dSendTotal = 0;
	double dRecvTotal = 0;
	for( j = 0; ; j++ )
	{
    int retry = 5;
		buffer0[0] = 0xaa; // First byte must match the ID.

		// But we can fill in random for the rest.
		for( i = 1; i < sizeof( buffer0 ); i ++ )
			buffer0[i] = i;//rand(); 

		if( buffer0[1] == 0xa4 ) buffer0[1]++;

		retrysend:
		
		dStartSend = OGGetAbsoluteTime();
		r = hid_send_feature_report( hd, buffer0, sizeof(buffer0) );
		dSendTotal += OGGetAbsoluteTime() - dStartSend;
		if( r != sizeof(buffer0) )
		{
			fprintf( stderr, "Warning: HID Send fault (%d) (%s) Retrying\n", r, strerror(errno) );
			retries++;
			if( retries > 10 ) break;
			goto retrysend;
		}

		retries = 0;
		printf( "<" ); // Print this out meaning we sent the data.

		memset( buffer1, 0xff, sizeof( buffer1 ) );
		buffer1[0] = 0xaa; // First byte must be ID.
    
		double dStartRecv = OGGetAbsoluteTime();
		r = hid_get_feature_report( hd, buffer1, sizeof(buffer1) );
		dRecvTotal += OGGetAbsoluteTime() - dStartRecv;

		printf( ">" ); fflush( stdout);

    // if (r == 191 || r == -1) goto retryread;
		if( r != sizeof( buffer1 ) && r != sizeof( buffer1 ) + 1) { 
      printf( "Got %d", r );
      if( retry-- )
      {
        printf( " - Retrying\n" );
        goto retrysend;
      }
      break; 
    }

		// Validate the scratches matched.
		if( memcmp( buffer0, buffer1, sizeof( buffer0 ) ) != 0 ) 
		{
			printf( "%d: ", r );
			for( i = 0; i < r; i++ )
				printf( "[%d] %02x>%02x %s", i, buffer0[i], buffer1[i], (buffer1[i] != buffer0[i])?"MISMATCH ":""  );
			printf( "\n" );
			break;
		}
		
		if( dStartRecv - dSecond > 1.0 )
		{
			printf( "\n%2.3f KB/s PC->MCU / %2.3f KB/s MCU->PC\n", j * .249 / dSendTotal, j * .249 / dRecvTotal );
			dSecond++;
		}
	}

	hid_close( hd );
}


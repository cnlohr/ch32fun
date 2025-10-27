#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>
#include <libusb-1.0/libusb.h>
#include "os_generic.h"

#define USB_VENDOR_ID  0x1209
#define USB_PRODUCT_ID 0xD035

#define USB_TIMEOUT 1024


const int block_size = 512;
int bRx = 1;
//#define SYNCHRONOUS_TEST
//#define ASYNC_CONTROL
#define BULKTEST

#define TRANSFERS 8


static libusb_context *ctx = NULL;
static libusb_device_handle *handle;

int abortLoop = 0;

static void sighandler(int signum)
{
	printf( "\nInterrupt signal received\n" );
	abortLoop = 1;
}

int xfertotal = 0;

void xcallback (struct libusb_transfer *transfer)
{
	xfertotal += transfer->actual_length;
	libusb_submit_transfer( transfer );
	//printf( "%d\n", transfer->actual_length );
}


int main(int argc, char **argv)
{
	//Pass Interrupt Signal to our handler
	signal(SIGINT, sighandler);

	libusb_init(&ctx);
	libusb_set_option(ctx, LIBUSB_OPTION_LOG_LEVEL, 3);

	handle = libusb_open_device_with_vid_pid( ctx, USB_VENDOR_ID, USB_PRODUCT_ID );
	if ( !handle )
	{
		fprintf( stderr, "Error: couldn't find handle\n" );
		return 1;
	}

	libusb_detach_kernel_driver(handle, 3);

	int r = 1;
	r = libusb_claim_interface(handle, 3 );
	if( r < 0 )
	{
		fprintf(stderr, "usb_claim_interface error %d\n", r);
		return 2;
	}

	double dRecvTotalTime = 0;
	double dSendTotalTime = 0;
	double dLastPrint = OGGetAbsoluteTime();
	int rtotal = 0, stotal = 0;

#if defined(SYNCHRONOUS_TEST)
	// Slow (synchronous) mode.
	if( bRx )
	{
		while(!abortLoop)
		{
			uint8_t buffer[block_size];
			int nread, ret;
			ret = libusb_bulk_transfer( handle, 0x84, buffer, sizeof( buffer ), &nread, USB_TIMEOUT );

			double dNow = OGGetAbsoluteTime();
			if( ret ) nread = 0;
			rtotal += nread;

			if( dNow - dLastPrint > 1 )
			{
				dRecvTotalTime = dNow - dLastPrint;
				printf( "%f MB/s RX\n", rtotal / (dRecvTotalTime * 1024 * 1024) );
				rtotal = 0;
				dRecvTotalTime = 0;
				dLastPrint = dNow;
			}
		}
	}
	else
	{
		uint8_t buffer[block_size];
		int i;
		for( i = 0; i < block_size; i++ )
		{
			buffer[i] = i;
		}
		while (!abortLoop)
		{
			int ret;
			int nwrite = 0;
			double dStartSend = OGGetAbsoluteTime();
			ret = libusb_bulk_transfer( handle, 0x05, buffer, sizeof( buffer ), &nwrite, USB_TIMEOUT );
			double dNow = OGGetAbsoluteTime();
			dSendTotalTime += dNow - dStartSend;
			if( ret ) nwrite = 0;
			stotal += nwrite;


			if( dNow - dLastPrint > 1 )
			{
				dSendTotalTime = dNow - dLastPrint;
				printf( "%f MB/s TX\n", stotal / (dSendTotalTime * 1024 * 1024) );
				stotal = 0;
				dSendTotalTime = 0;
				dLastPrint = dNow;
			}
		}
	}

#elif defined( ASYNC_CONTROL ) // Async-control (Still slow, don't use, if you want this just use HIDAPI) but this lets you go at about 10Mbit/s
	struct libusb_transfer * transfers[TRANSFERS];
	uint8_t buffers[TRANSFERS][block_size];
	int n;
	for( n = 0; n < TRANSFERS; n++ )
	{
		int k;
		for( k = 0; k < block_size; k++ )
		{
			buffers[n][k] = k;
		}

		struct libusb_transfer * t = transfers[n] = libusb_alloc_transfer( 0 );
		if( bRx )
		{
			libusb_fill_control_setup( buffers[n],
				0xA0, //	uint8_t  	bmRequestType,
				0x01, // HID_GET_REPORT, //	uint8_t  	bRequest,
				0x00aa, //	uint16_t  	wValue,
				0x0000, //	uint16_t  	wIndex,
				block_size - 8 //	uint16_t  	wLength 
			);
		}
		else
		{
			libusb_fill_control_setup( buffers[n],
				0x20, //	uint8_t  	bmRequestType,
				0x09, // HID_SET_REPORT, //	uint8_t  	bRequest,
				0x00aa, //	uint16_t  	wValue,
				0x0000, //	uint16_t  	wIndex,
				block_size - 8 //	uint16_t  	wLength 
			);
		}
		libusb_fill_control_transfer ( t, handle, buffers[n], xcallback, (void*)(intptr_t)n, 1000 );
		libusb_submit_transfer( t );
	}

	while(!abortLoop)
	{
			double dNow = OGGetAbsoluteTime();

			libusb_handle_events(ctx);

			if( dNow - dLastPrint > 1 )
			{
				dSendTotalTime = dNow - dLastPrint;
				printf( "%f MB/s TX\n", xfertotal / (dSendTotalTime * 1024 * 1024) );
				xfertotal = 0;
				dSendTotalTime = 0;
				dLastPrint = dNow;
			}
	}

	for( n = 0; n < TRANSFERS; n++ )
	{
		libusb_cancel_transfer( transfers[n] );
	 	libusb_free_transfer( transfers[n] );
	}

#elif defined( BULKTEST )
	// Async (Fast, bulk)
	// About 260-320 Mbit/s

	struct libusb_transfer * transfers[TRANSFERS];
	uint8_t buffers[TRANSFERS][block_size];
	int n;
	for( n = 0; n < TRANSFERS; n++ )
	{
		int k;
		for( k = 0; k < block_size; k++ )
		{
			buffers[n][k] = k;
		}
		struct libusb_transfer * t = transfers[n] = libusb_alloc_transfer( 0 );
		libusb_fill_bulk_transfer( t, handle, bRx ? 0x84 : 0x05, buffers[n], block_size, xcallback, (void*)(intptr_t)n, 1000 );
		libusb_submit_transfer( t );
	}


	while(!abortLoop)
	{
			double dNow = OGGetAbsoluteTime();

			libusb_handle_events(ctx);

			if( dNow - dLastPrint > 1 )
			{
				dSendTotalTime = dNow - dLastPrint;
				printf( "%f MB/s %cX\n", xfertotal / (dSendTotalTime * 1024 * 1024), bRx?'R':'T' );
				xfertotal = 0;
				dSendTotalTime = 0;
				dLastPrint = dNow;
			}
	}

	for( n = 0; n < TRANSFERS; n++ )
	{
		libusb_cancel_transfer( transfers[n] );
	 	libusb_free_transfer( transfers[n] );
	}
#endif

	libusb_release_interface (handle, 0);
	libusb_close(handle);
	libusb_exit(NULL);

	return 0;
}


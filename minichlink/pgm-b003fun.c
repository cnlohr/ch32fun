#include <stdint.h>
#include "hidapi.h"
#include "minichlink.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "../ch32fun/ch32fun.h"

//#define DEBUG_B003

#if defined(WINDOWS) || defined(WIN32) || defined(_WIN32)
void Sleep(uint32_t dwMilliseconds);
#define usleep( x ) Sleep( x / 1000 );
#else
#include <unistd.h>
#endif


struct B003FunProgrammerStruct
{
	void * internal; // Part of struct ProgrammerStructBase 

	hid_device * hd;
	uint8_t commandbuffer[128];
	uint8_t respbuffer[128];
	int commandplace;
	int prepping_for_erase;
	int no_get_report;
};

static const unsigned char byte_wise_read_blob[] = { // No alignment restrictions.
	0x23, 0xa0, 0x05, 0x00, 0x13, 0x07, 0x45, 0x03, 0x0c, 0x43, 0x50, 0x43,
	0x2e, 0x96, 0x21, 0x07, 0x94, 0x21, 0x14, 0xa3, 0x85, 0x05, 0x05, 0x07,
	0xe3, 0xcc, 0xc5, 0xfe, 0x93, 0x06, 0xf0, 0xff, 0x14, 0xc1, 0x82, 0x80,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static const unsigned char half_wise_read_blob[] = {  // size and address must be aligned by 2.
	0x23, 0xa0, 0x05, 0x00, 0x13, 0x07, 0x45, 0x03, 0x0c, 0x43, 0x50, 0x43,
	0x2e, 0x96, 0x21, 0x07, 0x96, 0x21, 0x16, 0xa3, 0x89, 0x05, 0x09, 0x07,
	0xe3, 0xcc, 0xc5, 0xfe, 0x93, 0x06, 0xf0, 0xff, 0x14, 0xc1, 0x82, 0x80,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static const unsigned char word_wise_read_blob[] = { // size and address must be aligned by 4.
	0x23, 0xa0, 0x05, 0x00, 0x13, 0x07, 0x45, 0x03, 0x0c, 0x43, 0x50, 0x43,
	0x2e, 0x96, 0x21, 0x07, 0x94, 0x41, 0x14, 0xc3, 0x91, 0x05, 0x11, 0x07,
	0xe3, 0xcc, 0xc5, 0xfe, 0x93, 0x06, 0xf0, 0xff, 0x14, 0xc1, 0x82, 0x80,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static const unsigned char word_wise_write_blob[] = { // size and address must be aligned by 4.
	0x23, 0xa0, 0x05, 0x00, 0x13, 0x07, 0x45, 0x03, 0x0c, 0x43, 0x50, 0x43,
	0x2e, 0x96, 0x21, 0x07, 0x14, 0x43, 0x94, 0xc1, 0x91, 0x05, 0x11, 0x07,
	0xe3, 0xcc, 0xc5, 0xfe, 0x93, 0x06, 0xf0, 0xff, 0x14, 0xc1, 0x82, 0x80, // NOTE: No readback!
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
/*
	0x23, 0xa0, 0x05, 0x00, 0x13, 0x07, 0x45, 0x03, 0x0c, 0x43, 0x50, 0x43,
	0x2e, 0x96, 0x21, 0x07, 0x14, 0x43, 0x94, 0xc1, 0x94, 0x41, 0x14, 0xc3, // With readback.
	0x91, 0x05, 0x11, 0x07, 0xe3, 0xca, 0xc5, 0xfe, 0x93, 0x06, 0xf0, 0xff,
	0x14, 0xc1, 0x82, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 */
};

static const unsigned char write64_flash[] = { // size and address must be aligned by 4.
  0x13, 0x07, 0x45, 0x03, 0x0c, 0x43, 0x13, 0x86, 0x05, 0x04, 0x5c, 0x43,
  0x8c, 0xc7, 0x14, 0x47, 0x94, 0xc1, 0xb7, 0x06, 0x05, 0x00, 0xd4, 0xc3,
  0x94, 0x41, 0x91, 0x05, 0x11, 0x07, 0xe3, 0xc8, 0xc5, 0xfe, 0xc1, 0x66,
  0x93, 0x86, 0x06, 0x04, 0xd4, 0xc3, 0xfd, 0x56, 0x14, 0xc1, 0x82, 0x80
};

static const unsigned char half_wise_write_blob[] = { // size and address must be aligned by 2
	0x23, 0xa0, 0x05, 0x00, 0x13, 0x07, 0x45, 0x03, 0x0c, 0x43, 0x50, 0x43,
	0x2e, 0x96, 0x21, 0x07, 0x16, 0x23, 0x96, 0xa1, 0x96, 0x21, 0x16, 0xa3,
	0x89, 0x05, 0x09, 0x07, 0xe3, 0xca, 0xc5, 0xfe, 0x93, 0x06, 0xf0, 0xff,
	0x14, 0xc1, 0x82, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static const unsigned char byte_wise_write_blob[] = { // no division requirements.
	0x23, 0xa0, 0x05, 0x00, 0x13, 0x07, 0x45, 0x03, 0x0c, 0x43, 0x50, 0x43,
	0x2e, 0x96, 0x21, 0x07, 0x14, 0x23, 0x94, 0xa1, 0x94, 0x21, 0x14, 0xa3,
	0x85, 0x05, 0x05, 0x07, 0xe3, 0xca, 0xc5, 0xfe, 0x93, 0x06, 0xf0, 0xff,
	0x14, 0xc1, 0x82, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// Just set the countdown to 0 to avoid any issues.
//   li a3, 0; sw a3, 0(a1); li a3, -1; sw a3, 0(a0); ret;
static const unsigned char halt_wait_blob[] = {
	0x81, 0x46, 0x94, 0xc1, 0xfd, 0x56, 0x14, 0xc1, 0x82, 0x80 };

// Set the countdown to -1 to cause main system to execute.
//   li a3, -1; sw a3, 0(a1); li a3, -1; sw a3, 0(a0); ret;
//static const unsigned char run_app_blob[] = {
//	0xfd, 0x56, 0x94, 0xc1, 0xfd, 0x56, 0x14, 0xc1, 0x82, 0x80 };
//
// Alternatively, we do it ourselves.
static const unsigned char run_app_blob[] = {
	0x37, 0x07, 0x67, 0x45, 0xb7, 0x27, 0x02, 0x40, 0x13, 0x07, 0x37, 0x12,
	0x98, 0xd7, 0x37, 0x97, 0xef, 0xcd, 0x13, 0x07, 0xb7, 0x9a, 0x98, 0xd7,
	0x23, 0xa6, 0x07, 0x00, 0x13, 0x07, 0x00, 0x08, 0x98, 0xcb, 0xb7, 0xf7,
	0x00, 0xe0, 0x37, 0x07, 0x00, 0x80, 0x23, 0xa8, 0xe7, 0xd0, 0x82, 0x80,
};


static void ResetOp( struct B003FunProgrammerStruct * eps )
{
	memset( eps->commandbuffer, 0, sizeof( eps->commandbuffer ) );
	memcpy( eps->commandbuffer, "\xaa\x00\x00\x00", 4 );
	eps->commandplace = 4;
}

static void WriteOp4( struct B003FunProgrammerStruct * eps, uint32_t opsend )
{
	int place = eps->commandplace;
	int newend = place + 4;
	if( newend < sizeof( eps->commandbuffer ) )
	{
		memcpy( eps->commandbuffer + place, &opsend, 4 );
	}
	eps->commandplace = newend;
}


static void WriteOpArb( struct B003FunProgrammerStruct * eps, const uint8_t * data, int len )
{
	int place = eps->commandplace;
	int newend = place + len;
	if( newend < sizeof( eps->commandbuffer ) )
	{
		memcpy( eps->commandbuffer + place, data, len );
	}
	eps->commandplace = newend;
}

static int CommitOp( struct B003FunProgrammerStruct * eps )
{
	int retries = 0;
	int r;

	uint32_t magic_go = 0x1234abcd;
	memcpy( eps->commandbuffer + 124, &magic_go, 4 );

	#ifdef DEBUG_B003
	{
		int i;
		printf( "Commit TX: %lu bytes\n", sizeof(eps->commandbuffer)  );
		for( i = 0; i < sizeof(eps->commandbuffer) ; i++ )
		{
			printf( "%02x ", eps->commandbuffer[i] );
			if( ( i & 0xf ) == 0xf ) printf( "\n" );
		}
		if( ( i & 0xf ) != 0xf ) printf( "\n" );
	}
	#endif

resend:
	r = hid_send_feature_report( eps->hd, eps->commandbuffer, sizeof(eps->commandbuffer) );
	#ifdef DEBUG_B003
	printf( "hid_send_feature_report = %d\n", r );
	#endif
	if( r < 0 )
	{
		if( retries ) fprintf( stderr, "Warning: Issue with hid_send_feature_report. Retrying: %d\n", retries );
		if( retries++ > 10 )
		{
			return r;
		}
		else
		{
			MCF.DelayUS( eps, 5000 );
			goto resend;
		}
	}
        
	if (eps->no_get_report) return r;

	int timeout = 0;

	do
	{
		eps->respbuffer[0] = 0xaa;
		r = hid_get_feature_report( eps->hd, eps->respbuffer, sizeof(eps->respbuffer) );

		#ifdef DEBUG_B003
		{
			int i;
			printf( "Commit RX: %d bytes\n", r );
			for( i = 0; i < r; i++ )
			{
				printf( "%02x ", eps->respbuffer[i] );
				if( ( i & 0xf ) == 0xf ) printf( "\n" );
			}
			if( ( i & 0xf ) != 0xf ) printf( "\n" );
		}
		#endif

		if( r < 0 )
		{
			if( retries++ > 10 ) return r;
			continue;
		}

		if( eps->respbuffer[1] == 0xff ) break;

		if( timeout++ > 20 )
		{
			printf( "Error: Timed out waiting for stub to complete\n" );
			return -99;
		}
	} while( 1 );
	return 0;
}

static int B003FunFlushLLCommands( void * dev )
{
	// All commands are synchronous anyway.
	return 0;
}


static int B003FunWaitForDoneOp( void * dev, int ignore )
{
	// It's synchronous, so no issue here.
	return 0;
}

// static int B003FunDelayUS( void * dev, int microseconds )
// {
// 	usleep( microseconds );
// 	return 0;
// }

// Does not handle erasing
static int InternalB003FunWriteBinaryBlob( void * dev, uint32_t address_to_write_to, uint32_t write_size, const uint8_t * blob )
{
	struct B003FunProgrammerStruct * eps = (struct B003FunProgrammerStruct *)dev;

	int is_flash = IsAddressFlash( address_to_write_to );

	if( ( address_to_write_to & 0x1 ) && write_size > 0 )
	{
		// Need to do byte-wise writing in front to line up with word alignment.
		ResetOp( eps );
		WriteOpArb( eps, byte_wise_write_blob, sizeof(byte_wise_write_blob) );
		WriteOp4( eps, address_to_write_to ); // Base address to write.
		WriteOp4( eps, 1 ); // write 1 bytes.
		memcpy( &eps->commandbuffer[60], blob, 1 );
		if( CommitOp( eps ) ) return -5;
		if( is_flash && memcmp( &eps->respbuffer[60], blob, 1 ) ) goto verifyfail;
		blob++;
		write_size --;
		address_to_write_to++;
	}
	if( ( address_to_write_to & 0x2 ) && write_size > 1 )
	{
		// Need to do byte-wise writing in front to line up with word alignment.
		ResetOp( eps );
		WriteOpArb( eps, half_wise_write_blob, sizeof(half_wise_write_blob) );
		WriteOp4( eps, address_to_write_to ); // Base address to write.
		WriteOp4( eps, 2 ); // write 2 bytes.
		memcpy( &eps->commandbuffer[60], blob, 2 );
		if( CommitOp( eps ) ) return -5;
		if( is_flash && memcmp( &eps->respbuffer[60], blob, 2 ) ) goto verifyfail;
		blob += 2;
		write_size -= 2;
		address_to_write_to+=2;
	}
	while( write_size > 3 )
	{
		int to_write_this_time = write_size & (~3);
		if( to_write_this_time > 64 ) to_write_this_time = 64;

		// Need to do byte-wise writing in front to line up with word alignment.
		ResetOp( eps );
		WriteOpArb( eps, word_wise_write_blob, sizeof(word_wise_write_blob) );
		WriteOp4( eps, address_to_write_to ); // Base address to write.
		WriteOp4( eps, to_write_this_time ); // write 4 bytes.
		memcpy( &eps->commandbuffer[60], blob, to_write_this_time );
		if( CommitOp( eps ) ) return -5;
		if( is_flash && memcmp( &eps->respbuffer[60], blob, to_write_this_time ) ) goto verifyfail;
		blob += to_write_this_time;
		write_size -= to_write_this_time;
		address_to_write_to += to_write_this_time;
	}
	if( write_size > 1 )
	{
		// Need to do byte-wise writing in front to line up with word alignment.
		ResetOp( eps );
		WriteOpArb( eps, half_wise_write_blob, sizeof(half_wise_write_blob) );
		WriteOp4( eps, address_to_write_to ); // Base address to write.
		WriteOp4( eps, 2 ); // write 2 bytes.
		memcpy( &eps->commandbuffer[60], blob, 2 );
		if( CommitOp( eps ) ) return -5;
		if( is_flash && memcmp( &eps->respbuffer[60], blob, 2 ) ) goto verifyfail;
		blob += 2;
		write_size -= 2;
		address_to_write_to += 2;
	}
	if( write_size )
	{
		// Need to do byte-wise writing in front to line up with word alignment.
		ResetOp( eps );
		WriteOpArb( eps, byte_wise_write_blob, sizeof(byte_wise_write_blob) );
		WriteOp4( eps, address_to_write_to ); // Base address to write.
		WriteOp4( eps, 1 ); // write 1 byte.
		memcpy( &eps->commandbuffer[60], blob, 1 );
		if( CommitOp( eps ) ) return -5;
		if( is_flash && memcmp( &eps->respbuffer[60], blob, 1 ) ) goto verifyfail;
		blob += 1;
		write_size -= 1;
		address_to_write_to+=1;
	}
	eps->prepping_for_erase = 0;
	fprintf(stderr, ".");
	return 0;
verifyfail:
	fprintf( stderr, "Error: Write Binary Blob: %d bytes to %08x\n", write_size, address_to_write_to );
	return -6;
}

static int B003FunReadBinaryBlob( void * dev, uint32_t address_to_read_from, uint32_t read_size, uint8_t * blob )
{
	struct B003FunProgrammerStruct * eps = (struct B003FunProgrammerStruct *)dev;

#ifdef DEBUG_B003
	printf( "Read Binary Blob: %d bytes from %08x\n", read_size, address_to_read_from );
#endif

	if( ( address_to_read_from & 0x1 ) && read_size > 0 )
	{
		// Need to do byte-wise reading in front to line up with word alignment.
		ResetOp( eps );
		WriteOpArb( eps, byte_wise_read_blob, sizeof(byte_wise_read_blob) );
		WriteOp4( eps, address_to_read_from ); // Base address to read.
		WriteOp4( eps, 1 ); // Read 1 bytes.
		if( CommitOp( eps ) ) return -5;
		memcpy( blob, &eps->respbuffer[60], 1 );
		blob++;
		read_size --;
		address_to_read_from++;
	}
	if( ( address_to_read_from & 0x2 ) && read_size > 1 )
	{
		// Need to do byte-wise reading in front to line up with word alignment.
		ResetOp( eps );
		WriteOpArb( eps, half_wise_read_blob, sizeof(half_wise_read_blob) );
		WriteOp4( eps, address_to_read_from ); // Base address to read.
		WriteOp4( eps, 2 ); // Read 2 bytes.
		if( CommitOp( eps ) ) return -5;
		memcpy( blob, &eps->respbuffer[60], 2 );
		blob += 2;
		read_size -= 2;
		address_to_read_from+=2;
	}
	while( read_size > 3 )
	{
		int to_read_this_time = read_size & (~3);
		if( to_read_this_time > 64 ) to_read_this_time = 64;

		// Need to do byte-wise reading in front to line up with word alignment.
		ResetOp( eps );
		WriteOpArb( eps, word_wise_read_blob, sizeof(word_wise_read_blob) );
		WriteOp4( eps, address_to_read_from ); // Base address to read.
		WriteOp4( eps, to_read_this_time ); // Read 4 bytes.
		if( CommitOp( eps ) ) return -5;
		memcpy( blob, &eps->respbuffer[60], to_read_this_time );
		blob += to_read_this_time;
		read_size -= to_read_this_time;
		address_to_read_from += to_read_this_time;
	}
	if( read_size > 1 )
	{
		// Need to do byte-wise reading in front to line up with word alignment.
		ResetOp( eps );
		WriteOpArb( eps, half_wise_read_blob, sizeof(half_wise_read_blob) );
		WriteOp4( eps, address_to_read_from ); // Base address to read.
		WriteOp4( eps, 2 ); // Read 2 bytes.
		if( CommitOp( eps ) ) return -5;
		memcpy( blob, &eps->respbuffer[60], 2 );
		blob += 2;
		read_size -= 2;
		address_to_read_from += 2;
	}
	if( read_size )
	{
		// Need to do byte-wise reading in front to line up with word alignment.
		ResetOp( eps );
		WriteOpArb( eps, byte_wise_read_blob, sizeof(byte_wise_read_blob) );
		WriteOp4( eps, address_to_read_from ); // Base address to read.
		WriteOp4( eps, 1 ); // Read 1 byte.
		if( CommitOp( eps ) ) return -5;
		memcpy( blob, &eps->respbuffer[60], 1 );
		blob += 1;
		read_size -= 1;
		address_to_read_from+=1;
	}
	return 0;
}

static int InternalB003FunBoot( void * dev )
{
	struct B003FunProgrammerStruct * eps = (struct B003FunProgrammerStruct*) dev;

	printf( "Booting\n" );
	ResetOp( eps );
	WriteOpArb( eps, run_app_blob, sizeof(run_app_blob) );
	eps->no_get_report = 1;
	if( CommitOp( eps ) ) return -5;
	return 0;
}

static int B003FunSetupInterface( void * dev )
{
	struct B003FunProgrammerStruct * eps = (struct B003FunProgrammerStruct*) dev;
	printf( "Halting Boot Countdown\n" );
	ResetOp( eps );
	WriteOpArb( eps, halt_wait_blob, sizeof(halt_wait_blob) );
	if( CommitOp( eps ) ) return -5;
	return 0;
}

static int B003FunExit( void * dev )
{	
	return 0;
}

// MUST be 4-byte-aligned.
static int B003FunWriteWord( void * dev, uint32_t address_to_write, uint32_t data )
{
	return InternalB003FunWriteBinaryBlob( dev, address_to_write, 4, (uint8_t*)&data );
}

static int B003FunReadWord( void * dev, uint32_t address_to_read, uint32_t * data )
{
	return B003FunReadBinaryBlob( dev, address_to_read, 4, (uint8_t*)data );
}

static int B003FunBlockWrite64( void * dev, uint32_t address_to_write, const uint8_t * data )
{
	struct B003FunProgrammerStruct * eps = (struct B003FunProgrammerStruct*) dev;
	struct InternalState * iss = eps->internal;

	if( IsAddressFlash( address_to_write ) )
	{
		if( !iss->flash_unlocked )
		{
			int rw;
			if( ( rw = InternalUnlockFlash( dev, iss ) ) )
				return rw;
		}

		if( !InternalIsMemoryErased( iss, address_to_write ) )
		{
			if( MCF.Erase( dev, address_to_write, 64, 0 ) )
			{
				fprintf( stderr, "Error: Failed to erase sector at %08x\n", address_to_write );
				return -9;
			}
		}

		// Not actually needed.
		MCF.WriteWord( dev, 0x40022010, CR_PAGE_PG ); // (intptr_t)&FLASH->CTLR = 0x40022010
		MCF.WriteWord( dev, 0x40022010, CR_PAGE_PG | CR_BUF_RST); // (intptr_t)&FLASH->CTLR = 0x40022010

		ResetOp( eps );
		WriteOpArb( eps, write64_flash, sizeof(write64_flash) );
		WriteOp4( eps, address_to_write ); // Base address to write. @52
		WriteOp4( eps, 0x4002200c ); // FLASH STATR base address. @ 56
		memcpy( &eps->commandbuffer[60], data, 64 ); // @60
		if( MCF.PrepForLongOp ) MCF.PrepForLongOp( dev );  // Give the programmer a headsup this next operation could take a while.
		if( CommitOp( eps ) ) return -5;

		// This is actually built-in.
//		MCF.WriteWord( dev, 0x40022010, CR_PAGE_PG|CR_STRT_Set); // (intptr_t)&FLASH->CTLR = 0x40022010  (actually commit)
	}
	else
	{
		return InternalB003FunWriteBinaryBlob( dev, address_to_write, 64, data );
	}

	return 0;
}

static int B003FunWriteHalfWord( void * dev, uint32_t address_to_write, uint16_t data )
{
	return InternalB003FunWriteBinaryBlob( dev, address_to_write, 2, (uint8_t*)&data );
}

static int B003FunReadHalfWord( void * dev, uint32_t address_to_read, uint16_t * data )
{
	return B003FunReadBinaryBlob( dev, address_to_read, 2, (uint8_t*)data );
}

static int B003FunWriteByte( void * dev, uint32_t address_to_write, uint8_t data )
{
	return InternalB003FunWriteBinaryBlob( dev, address_to_write, 1, &data );
}

static int B003FunReadByte( void * dev, uint32_t address_to_read, uint8_t * data )
{
	return B003FunReadBinaryBlob( dev, address_to_read, 1, data );
}


static int B003FunHaltMode( void * dev, int mode )
{
	struct InternalState * iss = (struct InternalState*)(((struct ProgrammerStructBase*)dev)->internal);
	switch ( mode )
	{
	case HALT_MODE_HALT_BUT_NO_RESET: // Don't reboot.
	case HALT_MODE_HALT_AND_RESET:    // Reboot and halt
		// This programmer is always halted anyway.
		break;

	case HALT_MODE_REBOOT:            // Actually boot?
		InternalB003FunBoot( dev );
		break;

	case HALT_MODE_RESUME:
		fprintf( stderr, "Warning: this programmer cannot resume\n" );
		// We can't do this.
		break;

	case HALT_MODE_GO_TO_BOOTLOADER:
		fprintf( stderr, "Warning: this programmer is already a bootloader.  Can't go into bootloader\n" );
		break;

	default:
		fprintf( stderr, "Error: Unknown halt mode %d\n", mode );
	}

	iss->processor_in_mode = mode;
	return 0;
}


int B003FunPrepForLongOp( void * dev )
{
	struct B003FunProgrammerStruct * d = (struct B003FunProgrammerStruct*)dev;
	d->prepping_for_erase = 1;
	return 0;
}

int B003PollTerminal( struct B003FunProgrammerStruct * eps, uint8_t * buffer, int maxlen, uint32_t leaveflagA, int leaveflagB )
{
	struct InternalState * iss = (struct InternalState*)(((struct ProgrammerStructBase*)eps)->internal);
	int r;
  int ack;
	uint8_t rr;
	if( iss->statetag != STTAG( "TERM" ) )
	{
    // int dmlock[6] = {'d','m','l','o','c','k'};
    // eps->respbuffer[0] = 0xfd;
    // hid_get_feature_report( eps->hd, eps->respbuffer, 8 );
    // if (memcmp(eps->respbuffer+1, dmlock, 6)) {
    //   eps->commandbuffer[0] = 0xaa;
    //   eps->commandbuffer[1] = 0xa5;
    //   eps->commandbuffer[2] = 2;
    //   hid_send_feature_report( eps->hd, eps->commandbuffer, 8 );
    // }
		iss->statetag = STTAG( "TERM" );
	}

  eps->respbuffer[0] = 0xfd;
	r = hid_get_feature_report( eps->hd, eps->respbuffer, 8 );

  if( r < 0 ) return 0;
	if( maxlen < 8 ) return -9;

  if( (leaveflagA>>8) ) {
    memset( eps->commandbuffer, 0, 8 );
    *((uint32_t*)eps->commandbuffer) = leaveflagA;
    *((uint32_t*)eps->commandbuffer+1) = leaveflagB;
    eps->commandbuffer[0] = 0xfd;
    eps->commandbuffer[1] = (leaveflagA>>8) & 0xFF;
    ack = hid_send_feature_report( eps->hd, eps->commandbuffer, 8 );
  }

  rr = eps->respbuffer[0];
  // fprintf( stderr, "r0 %u r1 %u r2 %u r3 %u\n", eps->respbuffer[0], eps->respbuffer[1], eps->respbuffer[2], eps->respbuffer[3] );
	if( rr & 0x80 )
	{
		int num_printf_chars = (rr & 0xf)-4;
		memcpy( buffer, eps->respbuffer+1, num_printf_chars );
		return num_printf_chars;
	}
	else
	{
		return -1;
	}
}

void * TryInit_B003Fun(uint32_t id)
{
	hid_init();
	fprintf( stderr, "VID:0x%04x, PID:0x%04x\n", id>>16, id&0xFFFF );
	hid_device * hd = hid_open( id>>16, id&0xFFFF, 0); // third parameter is "serial"
	if( !hd ) return 0;

	//extern int g_hidapiSuppress;
	//g_hidapiSuppress = 1;  // Suppress errors for this device.  (don't do this yet)

	struct B003FunProgrammerStruct * eps = malloc( sizeof( struct B003FunProgrammerStruct ) );
	memset( eps, 0, sizeof( *eps ) );
	eps->hd = hd;
	eps->commandplace = 1;

	memset( &MCF, 0, sizeof( MCF ) );
	MCF.WriteReg32 = 0;
	MCF.ReadReg32 = 0;
	MCF.FlushLLCommands = B003FunFlushLLCommands;
	// MCF.DelayUS = B003FunDelayUS;
	MCF.Control3v3 = 0;
	MCF.SetupInterface = B003FunSetupInterface;
	MCF.Exit = B003FunExit;
	MCF.HaltMode = 0;
	MCF.VoidHighLevelState = 0;
	MCF.PollTerminal = B003PollTerminal;

	// These are optional. Disabling these is a good mechanism to make sure the core functions still work.
	
	MCF.WriteWord = B003FunWriteWord;
	MCF.ReadWord = B003FunReadWord;

	MCF.WriteHalfWord = B003FunWriteHalfWord;
	MCF.ReadHalfWord = B003FunReadHalfWord;

	MCF.WriteByte = B003FunWriteByte;
	MCF.ReadByte = B003FunReadByte;

	MCF.WaitForDoneOp = B003FunWaitForDoneOp;
	MCF.BlockWrite64 = B003FunBlockWrite64;
	MCF.ReadBinaryBlob = B003FunReadBinaryBlob;

	MCF.PrepForLongOp = B003FunPrepForLongOp;

	MCF.HaltMode = B003FunHaltMode;

	return eps;
}





// Utility for generating bootloader code:

// make rv003usb.bin &&  xxd -i -s 100 -l 44 rv003usb.bin

/*
// Read data, arbitrarily from memory. (byte-wise)
. =  0x66
	sw x0, 0(a1);       // Stop Countdown
	addi a4, a0, 52;    // Start reading properties, starting from scratchpad + 52.
	c.lw a1, 0(a4);     // Get starting address to read
	c.lw a2, 4(a4);     // Get length to read.
	c.add a2, a1        // a2 is now ending address.
	c.addi a4, 8		// start writing back at byte 60.
1:
	XW_C_LBU(a3, a1, 0);	//lbu a3, 0(a1)       // Read from RAM
	XW_C_SB(a3, a4, 0);		//sb a3, 0(a4)       // Store into scratchpad
	c.addi a1, 1        // Advance pointers
	c.addi a4, 1
	blt a1, a2, 1b      // Loop til all read.
	addi a3, x0, -1
	sw a3, 0(a0)		// Write -1 into 0x00 indicating all done.
	ret
	.long 0,0,0,0,0,0,0
*/

/*
// Read data, arbitrarily from memory. (half-wise)

. =  0x66
	sw x0, 0(a1);       // Stop Countdown
	addi a4, a0, 52;    // Start reading properties, starting from scratchpad + 52.
	c.lw a1, 0(a4);     // Get starting address to read
	c.lw a2, 4(a4);     // Get length to read.
	c.add a2, a1        // a2 is now ending address.
	c.addi a4, 8		// start writing back at byte 60.
1:
	XW_C_LHU(a3, a1, 0);	//lhu a3, 0(a1)       // Read from RAM
	XW_C_SH(a3, a4, 0);		//sh a3, 0(a4)       // Store into scratchpad
	c.addi a1, 2        // Advance pointers
	c.addi a4, 2
	blt a1, a2, 1b      // Loop til all read.
	addi a3, x0, -1
	sw a3, 0(a0)		// Write -1 into 0x00 indicating all done.
	ret
	.long 0,0,0,0,0,0,0
*/

/*
// Read data, arbitrarily from memory. (word-wise)
. =  0x66
	sw x0, 0(a1);       // Stop Countdown
	addi a4, a0, 52;    // Start reading properties, starting from scratchpad + 52.
	c.lw a1, 0(a4);     // Get starting address to read
	c.lw a2, 4(a4);     // Get length to read.
	c.add a2, a1        // a2 is now ending address.
	c.addi a4, 8		// start writing back at byte 60.
1:
	lw a3, 0(a1);		//lw a3, 0(a1)       // Read from RAM
	sw a3, 0(a4);		//sw a3, 0(a4)       // Store into scratchpad
	c.addi a1, 4        // Advance pointers
	c.addi a4, 4
	blt a1, a2, 1b      // Loop til all read.
	addi a3, x0, -1
	sw a3, 0(a0)		// Write -1 into 0x00 indicating all done.
	ret
	.long 0,0,0,0,0,0,0
*/
/*
// Write data, arbitrarily to memory. (word-wise)
. =  0x66
	sw x0, 0(a1);       // Stop Countdown
	addi a4, a0, 52;    // Start reading properties, starting from scratchpad + 52.
	c.lw a1, 0(a4);     // Get starting address to read
	c.lw a2, 4(a4);     // Get length to read.
	c.add a2, a1        // a2 is now ending address.
	c.addi a4, 8		// start writing back at byte 60.
1:
	lw a3, 0(a4);		//lw a3, 0(a1)       // Read from RAM
	sw a3, 0(a1);		//sw a3, 0(a4)       // Store into scratchpad
	lw a3, 0(a1);       // Read-back
	sw a3, 0(a4);
	c.addi a1, 4        // Advance pointers
	c.addi a4, 4
	blt a1, a2, 1b      // Loop til all read.
	addi a3, x0, -1
	sw a3, 0(a0)		// Write -1 into 0x00 indicating all done.
	ret
	.long 0,0,0,0,0,0
*/

/*
// Write data, arbitrarily to memory. (word-wise)
. =  0x66
	sw x0, 0(a1);       // Stop Countdown
	addi a4, a0, 52;    // Start reading properties, starting from scratchpad + 52.
	c.lw a1, 0(a4);     // Get starting address to read
	c.lw a2, 4(a4);     // Get length to read.
	c.add a2, a1        // a2 is now ending address.
	c.addi a4, 8		// start writing back at byte 60.
1:
	XW_C_LHU(a3, a4, 0);	//lbu a3, 0(a4)       // Read from scratchpad
	XW_C_SH(a3, a1, 0);		//sb a3, 0(a1)       // Store into RAM
	XW_C_LHU(a3, a1, 0);	//lbu a3, 0(a4)       //  Read back
	XW_C_SH(a3, a4, 0);		//sb a3, 0(a1) 
	c.addi a1, 2        // Advance pointers
	c.addi a4, 2
	blt a1, a2, 1b      // Loop til all read.
	addi a3, x0, -1
	sw a3, 0(a0)		// Write -1 into 0x00 indicating all done.
	ret
	.long 0,0,0,0,0,0
*/

/*
// Write data, arbitrarily to memory. (byte-wise)
. =  0x66
	sw x0, 0(a1);       // Stop Countdown
	addi a4, a0, 52;    // Start reading properties, starting from scratchpad + 52.
	c.lw a1, 0(a4);     // Get starting address to read
	c.lw a2, 4(a4);     // Get length to read.
	c.add a2, a1        // a2 is now ending address.
	c.addi a4, 8		// start writing back at byte 60.
1:
	XW_C_LBU(a3, a4, 0);	//lbu a3, 0(a4)       // Read from scratchpad
	XW_C_SB(a3, a1, 0);		//sb a3, 0(a1)       // Store into RAM
	XW_C_LBU(a3, a1, 0);	//Read back
	XW_C_SB(a3, a4, 0);
	c.addi a1, 1        // Advance pointers
	c.addi a4, 1
	blt a1, a2, 1b      // Loop til all read.
	addi a3, x0, -1
	sw a3, 0(a0)		// Write -1 into 0x00 indicating all done.
	ret
	.long 0,0,0,0,0,0
*/


/* Run app blob
				FLASH->BOOT_MODEKEYR = FLASH_KEY1;
				FLASH->BOOT_MODEKEYR = FLASH_KEY2;
				FLASH->STATR = 0; // 1<<14 is zero, so, boot user code.
				FLASH->CTLR = CR_LOCK_Set;
				PFIC->SCTLR = 1<<31;
*/


/* Write flash block 64.

. =  0x66
	addi a4, a0, 52;    // Start reading properties, starting from scratchpad + 52.
	c.lw a1, 0(a4);     // a1 = Address to write to.
	addi a2, a1, 64     // a2 = end of section to write to
	c.lw a5, 4(a4);     // a5 = Get flash address (0x40022010)

	// Must be done outside.
//	li a3, 0x00080000 | 0x00010000;
	//c.sw a3, 0(a5);  //FLASH->CTLR = CR_BUF_RST | CR_PAGE_PG
	c.sw a1, 8(a5);     //FLASH->ADDR = writing location.

	1:
		c.lw a3, 8(a4);		//lw a3, 0(a1)       // Read from RAM (Starting @60)
		c.sw a3, 0(a1);		//sw a3, 0(a4)       // Store into flash

		li a3, 0x00010000 | 0x00040000;	// CR_PAGE_PG | FLASH_CTLR_BUF_LOAD
		c.sw a3, 4(a5);     // Load into flash write buffer.

		c.lw a3, 0(a1);		//Tricky: By reading from flash here, we force it to wait for completion.
		c.addi a1, 4        // Advance pointers
		c.addi a4, 4

	//	// Wait for write to complete.
	//	2:	c.lw a3, 0(a5)   // read FLASH->STATR 
	//		c.andi a3, 1     // Mask off BUSY bit.
	//		c.bnez a3, 2b


		blt a1, a2, 1b      // Loop til all read.

	li a3, 0x00010000 | 0x00000040 //CR_PAGE_PG|CR_STRT_Set
	c.sw a3, 4(a5);     //FLASH->CTRL = CR_PAGE_PG|CR_STRT_Set
	li a3, -1
	c.sw a3, 0(a0)		// Write -1 into 0x00 indicating all done.
	ret
*/

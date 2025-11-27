// The "bootloader" blob is (C) WCH.
// Tricky: You need to use wch link to use WCH-LinkRV.
//  you can always uninstall it in device manager.  It will be under USB devices or something like that at the bottom.

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "libusb.h"
#include "minichlink.h"
#include "chips.h"

#define FORCE_EXTERNAL_CHIP_DETECTION 1

struct LinkEProgrammerStruct
{
	void * internal;
	libusb_device_handle * devh;
	int lasthaltmode; // For non-003 chips
};
#if !FORCE_EXTERNAL_CHIP_DETECTION
static int checkChip(enum RiscVChip chip) {
	switch(chip) {
		case CHIP_UNKNOWN:
		case CHIP_CH32V003:
		case CHIP_CH32X03x:
		case CHIP_CH32V00x:
		case CHIP_CH641:
		case CHIP_CH643:
		case CHIP_CH32L103:
		case CHIP_CH570:
		case CHIP_CH57x:
		case CHIP_CH58x:
		case CHIP_CH585:
		case CHIP_CH59x:
			return 0; // Use direct mode
		case CHIP_CH32V10x:
		case CHIP_CH32V20x:
		case CHIP_CH32V30x:
		case CHIP_CH32V41x
			return 1; // Use binary blob mode
		case CHIP_CH56x:
		default:
			return -1; // Not supported yet
	}
}
#endif // !FORCE_EXTERNAL_CHIP_DETECTION

// For non-ch32v003 chips.
#if !FORCE_EXTERNAL_CHIP_DETECTION
//static int LEReadBinaryBlob( void * d, uint32_t offset, uint32_t amount, uint8_t * readbuff );
static int InternalLinkEHaltMode( void * d, int mode );
static int LEWriteBinaryBlob( void * d, uint32_t address_to_write, uint32_t len, const uint8_t * blob );
#endif // !FORCE_EXTERNAL_CHIP_DETECTION

#define WCHTIMEOUT 5000
#define WCHCHECK(x) if( (status = x) ) { fprintf( stderr, "Bad USB Operation on " __FILE__ ":%d (%d)\n", __LINE__, status ); exit( status ); }

void wch_link_command( libusb_device_handle * devh, const void * command_v, int commandlen, int * transferred, uint8_t * reply, int replymax )
{
	uint8_t * command = (uint8_t*)command_v;
	uint8_t buffer[1024];
	int got_to_recv = 0;
	int status;
	int transferred_local;
	if( !transferred ) transferred = &transferred_local;
	status = libusb_bulk_transfer( devh, 0x01, command, commandlen, transferred, WCHTIMEOUT );
	if( status ) goto sendfail;
	got_to_recv = 1;
	if( !reply )
	{
		reply = buffer; replymax = sizeof( buffer );
	}

//	printf("wch_link_command send (%d)", commandlen); for(int i = 0; i< commandlen; printf(" %02x",command[i++])); printf("\n");

	status = libusb_bulk_transfer( devh, 0x81, reply, replymax, transferred, WCHTIMEOUT );

//	printf("wch_link_command reply (%d)", *transferred); for(int i = 0; i< *transferred; printf(" %02x",reply[i++])); printf("\n"); 

	if( status ) goto sendfail;
	return;
sendfail:
	fprintf( stderr, "Error sending WCH command (%s): ", got_to_recv?"on recv":"on send" );
	int i;
	for( i = 0; i < commandlen; i++ )
	{
		printf( "%02x ", command[i] );
	}
	printf( "\n" );
	exit( status );
}

static void wch_link_multicommands( libusb_device_handle * devh, int nrcommands, ... )
{
	int i;
	va_list argp;
	va_start(argp, nrcommands);
	for( i = 0; i < nrcommands; i++ )
	{
		int clen = va_arg(argp, int);
		wch_link_command( devh, va_arg(argp, char *), clen, 0, 0, 0 );
	}
	va_end( argp );
}

static inline libusb_device_handle * wch_link_base_setup( int inhibit_startup )
{
	libusb_context * ctx = 0;
	int status;
	status = libusb_init(&ctx);
	if (status < 0) {
		fprintf( stderr, "Error: libusb_init_context() returned %d\n", status );
		exit( status );
	}
	
	libusb_device **list;
	ssize_t cnt = libusb_get_device_list(ctx, &list);
	ssize_t i = 0;

	libusb_device *found = NULL;
	libusb_device * found_arm_programmer = NULL;
	libusb_device * found_programmer_in_iap = NULL;

	for (i = 0; i < cnt; i++) {
		libusb_device *device = list[i];
		struct libusb_device_descriptor desc;
		int r = libusb_get_device_descriptor(device,&desc);
		if( r == 0 && desc.idVendor == 0x1a86 && desc.idProduct == 0x8010 ) { found = device; }
		if( r == 0 && desc.idVendor == 0x1a86 && desc.idProduct == 0x8012) { found_arm_programmer = device; }
		if( r == 0 && desc.idVendor == 0x4348 && desc.idProduct == 0x55e0) { found_programmer_in_iap = device; }
	}

	if( !found )
	{
		// On a lark see if we have a programmer which got stuck in IAP mode.

		if (found_arm_programmer) {
			fprintf( stderr, "Warning: found at least one WCH-LinkE in ARM programming mode. Attempting automatic switch to RISC-V.  Will need a to re-attempt.\n" );
			fprintf( stderr, "For more information, you may need to change it to RISC-V mode as per https://github.com/cnlohr/ch32v003fun/issues/227\n" ); 

			// Just in case we got stuck in IAP mode, try sending 0x83 to eject.
			libusb_device_handle * devh = 0;
			status = libusb_open( found_arm_programmer, &devh );
			if( status )
			{
				fprintf( stderr, "Found programmer in ARM mode, but couldn't open it.\n" );
				exit( -10 );
			}

			// https://github.com/wagiminator/MCU-Flash-Tools/blob/main/rvmode.py
			uint8_t rbuff[4] = { 0x81, 0xff, 0x01, 0x52 };
			int transferred = 0;
			libusb_bulk_transfer( devh, 0x02, rbuff, 4, &transferred, 1 );
			fprintf( stderr, "RISC-V command sent (%d)\n", transferred );
			exit( -3 );
		}


		if( found_programmer_in_iap )
		{
			// Just in case we got stuck in IAP mode, try sending 0x83 to eject.
			fprintf( stderr, "Found programmer in IAP mode. Attempting to eject it out of IAP.\n" );
			libusb_device_handle * devh = 0;
			status = libusb_open( found_programmer_in_iap, &devh );
			if( status )
			{
				fprintf( stderr, "Found programmer in IAP mode, but couldn't open it.\n" );
				exit( -10 );
			}
			uint8_t rbuff[4];
			int transferred = 0;
			rbuff[0] = 0x83;
			libusb_bulk_transfer( devh, 0x02, rbuff, 1, &transferred, 1 );
			fprintf( stderr, "Eject command sent (%d)\n", transferred );
			exit( -3 );
		}

		return 0;
	}

	libusb_device_handle * devh;
	status = libusb_open( found, &devh );
	if( status )
	{
		fprintf( stderr, "Error: couldn't open wch link device (libusb_open() = %d)\n", status );
		if( status == -5 )
		{
			fprintf( stderr, "Did you install the WCH Link E Drivers described here: https://github.com/cnlohr/ch32v003fun/wiki/Installation#windows\n" );
		}
		return 0;
	}
		
	WCHCHECK( libusb_claim_interface(devh, 0) );

	uint8_t rbuff[1024];
	int transferred;
	libusb_bulk_transfer( devh, 0x81, rbuff, 1024, &transferred, 1 ); // Clear out any pending transfers.  Don't wait though.

	return devh;
}

// DMI_OP decyphered From https://github.com/karlp/openocd-hacks/blob/27af153d4a373f29ad93dab28a01baffb7894363/src/jtag/drivers/wlink.c
// Thanks, CW2 for pointing this out.  See DMI_OP for more info.
int LEWriteReg32( void * dev, uint8_t reg_7_bit, uint32_t command )
{
	libusb_device_handle * devh = ((struct LinkEProgrammerStruct*)dev)->devh;

	const uint8_t iOP = 2; // op 2 = write
	uint8_t req[] = {
		0x81, 0x08, 0x06, reg_7_bit,
        (command >> 24) & 0xff,
        (command >> 16) & 0xff,
        (command >> 8) & 0xff,
        (command >> 0) & 0xff,
        iOP };

	uint8_t resp[128];
	int resplen;
	wch_link_command( devh, req, sizeof(req), &resplen, resp, sizeof(resp) );
	if( resplen != 9 || resp[8] == 0x02 || resp[8] == 0x03 ) //|| resp[3] != reg_7_bit )
	{
		struct InternalState *iss = (struct InternalState *)( ( (struct ProgrammerStructBase *)dev )->internal );
		if ( !iss->target_chip_id && !iss->statetag )
		{
			fprintf( stderr, "Programmer wasn't initialized? Fixing\n" );
			wch_link_command( devh, "\x81\x0d\x01\x02", 4, &resplen, resp, sizeof( resp ) );
			iss->statetag = STTAG( "INIT" );
		}
		else
		{
			fprintf( stderr, "Error setting write reg. Tell cnlohr. Maybe we should allow retries here?\n" );
			fprintf( stderr, "RR: %d :", resplen );
			int i;
			for ( i = 0; i < resplen; i++ )
			{
				fprintf( stderr, "%02x ", resp[i] );
			}
			fprintf( stderr, "\n" );
		}
		fprintf( stderr, "\n" );
	}
	return 0;
}

int LEReadReg32( void * dev, uint8_t reg_7_bit, uint32_t * commandresp )
{
	libusb_device_handle * devh = ((struct LinkEProgrammerStruct*)dev)->devh;
	const uint8_t iOP = 1; // op 1 = read
	uint32_t transferred;
	uint8_t rbuff[128] = { 0 };
	uint8_t req[] = {
		  0x81, 0x08, 0x06, reg_7_bit,
      0, 0, 0, 0,
      iOP };
	wch_link_command( devh, req, sizeof( req ), (int*)&transferred, rbuff, sizeof( rbuff ) );
	*commandresp = ( rbuff[4]<<24 ) | (rbuff[5]<<16) | (rbuff[6]<<8) | (rbuff[7]<<0);
	if( transferred != 9 || rbuff[8] == 0x02 || rbuff[8] == 0x03 ) //|| rbuff[3] != reg_7_bit )
	{
		struct InternalState *iss = (struct InternalState *)( ( (struct ProgrammerStructBase *)dev )->internal );
		if ( !iss->target_chip_id && !iss->statetag )
		{
			fprintf( stderr, "Programmer wasn't initialized? Fixing\n" );
			wch_link_command( devh, "\x81\x0d\x01\x02", 4, (int *)&transferred, rbuff, sizeof( rbuff ) );
			iss->statetag = STTAG( "INIT" );
		}
		else
		{
			fprintf( stderr, "Error setting read reg. Tell cnlohr. Maybe we should allow retries here?\n" );
			fprintf( stderr, "Reg: %08x, RR: %d :", reg_7_bit, transferred );
			int i;
			for ( i = 0; i < transferred; i++ )
			{
				fprintf( stderr, "%02x ", rbuff[i] );
			}
			fprintf( stderr, "\n" );
      return -1;
		}
	}
	/*
	printf( "RR: %d :", transferred );
	int i;
	for( i = 0; i < transferred; i++ )
	{
		printf( "%02x ", rbuff[i] );
	}
	printf( "\n" );
	*/

	return 0;
}

int LEFlushLLCommands( void * dev )
{
	return 0;
}

int LEResetInterface( void * d )
{
	libusb_device_handle * dev = ((struct LinkEProgrammerStruct*)d)->devh;
	wch_link_command( dev, "\x81\x0d\x01\xff", 4, 0, 0, 0 );
	wch_link_command( dev, "\x81\x0d\x01\x01", 4, 0, 0, 0 );
	return 0;
}

static int LESetupInterface( void * d )
{
	libusb_device_handle * dev = ((struct LinkEProgrammerStruct*)d)->devh;
	struct InternalState * iss = (struct InternalState*)(((struct ProgrammerStructBase*)d)->internal);
	uint8_t rbuff[1024];
	uint32_t transferred = 0;
	int r = 0;

	if( iss->target_chip != NULL )
	{
		wch_link_command( dev, "\x81\x0d\x01\x02", 4, (int*)&transferred, rbuff, 1024 ); // Reply: Ignored, 820d050900300500
		if (transferred == 4 || (rbuff[0] == 0x81 && rbuff[1] == 0x55 && rbuff[2] == 0x01) ) // && rbuff[3] == 0x01 )
		{
			MCF.DelayUS( iss, 5000 );
			wch_link_command( dev, "\x81\x0d\x01\x02", 4, (int*)&transferred, rbuff, 1024 ); // Reply: Ignored, 820d050900300500
		}
		char cmd_buf[5] = { 0x81, 0x0c, 0x02, iss->target_chip_type, iss->target_chip->interface_speed };
		wch_link_command( dev, cmd_buf, 5, 0, 0, 0 ); // Set interface clock to suitable speed
		return 0;
	}

	//Stop programmer to avoid anything being unresponsive
	wch_link_command( dev, "\x81\x0d\x01\xff", 4, 0, 0, 0);
	// Clears programmer state and returns firmware version
	wch_link_command( dev, "\x81\x0d\x01\x01", 4, (int*)&transferred, rbuff, 1024 );	// Reply is: "\x82\x0d\x04\x02\x08\x02\x00"

	switch(rbuff[5]) {
		case 1:
			fprintf(stderr, "WCH Programmer is CH549 version %d.%d\n",rbuff[3], rbuff[4]);
			break;
		case 2:
			fprintf(stderr, "WCH Programmer is CH32V307 version %d.%d\n",rbuff[3], rbuff[4]);
			break;
		case 3:
			fprintf(stderr, "WCH Programmer is CH32V203 version %d.%d\n",rbuff[3], rbuff[4]);
			break;
		case 4:
			fprintf(stderr, "WCH Programmer is LinkB version %d.%d\n",rbuff[3], rbuff[4]);
			break;
		case 5:
			fprintf(stderr, "WCH Programmer is LinkW version %d.%d\n",rbuff[3], rbuff[4]);
			break;
		case 18:
			fprintf(stderr, "WCH Programmer is LinkE version %d.%d\n",rbuff[3], rbuff[4]);
			break;
		default:
			fprintf(stderr, "Unknown WCH Programmer %02x (Ver %d.%d)\n", rbuff[5], rbuff[3], rbuff[4]);
			break;
	}
  
	wch_link_command( dev, "\x81\x0c\x02\x01\x02", 5, 0, 0, 0 );	// By default set interface speed to "normal" (4Mhz) and change that after we detect the chip

	// This puts the processor on hold to allow the debugger to run.
	int already_tried_reset = 0;
	int is_already_connected = 0;
	do
	{
		// There's a couple situations where the older firmware on the official programmer freaks out.
		// We don't want to inconvenience our users, so just try to work through it by falling back to the minichlink functions.
		// Part connected.  But the programmer doesn't know what it is.
		if( transferred == 9 && ( ( rbuff[4] == 0x00 ) || 
			( rbuff[8] != 0x02 && rbuff[8] != 0x03 && rbuff[8] != 0x00 ) ) )
		{
			// Already connected.
			if( is_already_connected )
			{
				printf( "Already Connected\n" );
				// Still need to read in the data so we can select the correct chip.
				wch_link_command( dev, "\x81\x0d\x01\x02", 4, (int*)&transferred, rbuff, 1024 ); // ?? this seems to work?
				break;
			}
			is_already_connected = 1;
		}

		wch_link_command( dev, "\x81\x0d\x01\x02", 4, (int*)&transferred, rbuff, 1024 ); // Reply: Ignored, 820d050900300500
		if (transferred == 4 || (rbuff[0] == 0x81 && rbuff[1] == 0x55 && rbuff[2] == 0x01) ) // && rbuff[3] == 0x01 )
		{
			// The following code may try to execute a few times to get the processor to actually reset.
			// This code could likely be much better.
			if( already_tried_reset > 1)
				fprintf(stderr, "link error, nothing connected to linker (%d = [%02x %02x %02x %02x]).  Trying to put processor in hold and retrying.\n", transferred, rbuff[0], rbuff[1], rbuff[2], rbuff[3]);

			// Give up if too long
			if( already_tried_reset > 5 )
			{
				break;
			}

			wch_link_multicommands( (libusb_device_handle *)dev, 1, 4, "\x81\x0d\x01\x13" ); // Try forcing reset line low.
			wch_link_command( (libusb_device_handle *)dev, "\x81\x0d\x01\xff", 4, 0, 0, 0); //Exit programming

			if( already_tried_reset > 3 )
			{
				MCF.DelayUS( iss, 5000 );
				wch_link_command( dev, "\x81\x0d\x01\x03", 4, (int*)&transferred, rbuff, 1024 ); // Reply: Ignored, 820d050900300500
			}
			else
			{
				MCF.DelayUS( iss, 5000 );
			}

			wch_link_multicommands( (libusb_device_handle *)dev, 3, 4, "\x81\x0b\x01\x01", 4, "\x81\x0d\x01\x02", 4, "\x81\x0d\x01\xff" );
			already_tried_reset++;
		}
		else
		{
			break;
		}
	} while( 1 );

#if !FORCE_EXTERNAL_CHIP_DETECTION
	printf( "Full Chip Type Reply: [%d] %02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x\n", transferred, rbuff[0], rbuff[1], rbuff[2], rbuff[3], rbuff[4], rbuff[5], rbuff[6], rbuff[7], rbuff[8] );

	const struct RiscVChip_s* chip = FindChip( rbuff[3] << 16 | rbuff[4] << 8 | rbuff[5] );
	
	if( !chip )
	{
		fprintf( stderr, "Chip Type unknown [%02x - %04x]. Aborting...\n", rbuff[3], rbuff[4] << 8 | rbuff[5] );
		return -1;
	}
	
	fprintf( stderr, "Detected: %s\n", chip->name_str );
	iss->target_chip = chip;
	iss->target_chip_type = chip->family_id;
	iss->target_chip_id = (rbuff[4] << 24) | (rbuff[5] << 16) | (rbuff[6] << 8) | rbuff[7];
	iss->flash_size = chip->flash_size;
	iss->ram_base = chip->ram_base;
	iss->ram_size = chip->ram_size;
	iss->sector_size = chip->sector_size;

#else
	r = MCF.DetermineChipType( d );
	if( r ) return r;
	// if( iss->target_chip_type == CHIP_CH32V10x )
	// {
		// fprintf( stderr, "Using binary blob write for operation.\n" );
		// MCF.WriteBinaryBlob = LEWriteBinaryBlob;
		// wch_link_command( dev, "\x81\x11\x01\x05", 4, (int*)&transferred, rbuff, 1024 );
		// wch_link_command( dev, "\x81\x0d\x01\x03", 4, (int*)&transferred, rbuff, 1024 ); // Reply: Ignored, 820d050900300500
	// }

#endif

	// For some reason, if we don't do this sometimes the programmer starts in a hosey mode.
	MCF.WriteReg32( d, DMCONTROL, 0x80000003 ); // Reset target
	MCF.WriteReg32( d, DMCONTROL, 0x80000001 ); // Un-super-halt processor.
	MCF.WriteReg32( d, DMCONTROL, 0x80000001 );
	MCF.DelayUS( iss, 10000 ); // Delay to let some chips fully reboot
	char cmd_buf[5] = { 0x81, 0x0c, 0x02, iss->target_chip_type, iss->target_chip->interface_speed};
	// if( iss->target_chip_type == CHIP_CH32V10x ) cmd_buf[3] = 0x05; // Why have I added this?!
	wch_link_command( dev, cmd_buf, 5, 0, 0, 0 ); // Set interface clock to suitable speed

	#if !FORCE_EXTERNAL_CHIP_DETECTION
	int timeout = 0;
retry_DoneOp:
	MCF.DelayUS( iss, 4000 );
	MCF.WriteReg32( d, DMABSTRACTCS, 0x00000700 ); // Ignore any pending errors.
	MCF.WriteReg32( d, DMABSTRACTAUTO, 0 );
	MCF.WriteReg32( d, DMCOMMAND, 0x00221000 ); // Read x0 (Null command) with nopostexec (to fix v307 read issues)
	r = MCF.WaitForDoneOp( d, 0 );
	if( r )
	{
		fprintf( stderr, "Retrying\n" );
		if( timeout++ < 10 ) goto retry_DoneOp;
		fprintf( stderr, "Fault on setup %d\n", r );
		return -4;
	}
	else
	{
		fprintf( stderr, "Setup success\n" );
	}

	// Skipping extended Chip ID for chips that doesn't have it
	if( iss->target_chip_type == CHIP_CH59x || iss->target_chip_type == CHIP_CH58x || iss->target_chip_type == CHIP_CH57x )
	{
		// MCF.WriteBinaryBlob = LEWriteBinaryBlob;
		return 0;
	}
	// This puts the processor on hold to allow the debugger to run.
	// Recommended to switch to 05 from 09 by Alexander M
	//	wch_link_command( dev, "\x81\x11\x01\x09", 4, (int*)&transferred, rbuff, 1024 ); // Reply: Chip ID + Other data (see below)
retry_ID:
		wch_link_command( dev, "\x81\x11\x01\x05", 4, (int*)&transferred, rbuff, 1024 ); // Reply: Chip ID + Other data (see below)

		if( rbuff[0] == 0x00 )
		{
			if( timeout++ < 10 ) goto retry_ID;
			fprintf( stderr, "Failed to get chip ID\n" );
			return -4;
		}

		if( transferred != 20 )
		{
			fprintf( stderr, "Error: could not get part status\n" );
			return -1;
		}
		int flash_size = (rbuff[2]<<8) | rbuff[3];
		fprintf( stderr, "Flash Storage: %d kB\n", flash_size );
		fprintf( stderr, "Part UUID    : %02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x\n", rbuff[4], rbuff[5], rbuff[6], rbuff[7], rbuff[8], rbuff[9], rbuff[10], rbuff[11] );
		fprintf( stderr, "PFlags       : %02x-%02x-%02x-%02x\n", rbuff[12], rbuff[13], rbuff[14], rbuff[15] );
		fprintf( stderr, "Part Type (B): %02x-%02x-%02x-%02x\n", rbuff[16], rbuff[17], rbuff[18], rbuff[19] );
		for (int n = 0; n < transferred; n++) {
			fprintf(stderr, "%02x ", rbuff[n]);
		}
		fprintf(stderr, "\n");

		// Quirk, was fixed in LinkE version 2.12.
		if( iss->target_chip_type == CHIP_CH32V10x && flash_size == 62 )
		{
			fprintf( stderr, "While the debugger reports this as a CH32V10x, it's probably a CH32X03x\n" );
			iss->target_chip_type = CHIP_CH32X03x;
			iss->target_chip = &ch32x035;
		}
		int result = checkChip(iss->target_chip_type);
		if( result == 1 ) // Using blob write
		{
			fprintf( stderr, "Using binary blob write for operation.\n" );
			MCF.WriteBinaryBlob = LEWriteBinaryBlob;

			// iss->sector_size = 256;
			// Why do we need this exactly? For blobed chips only?
			wch_link_command( dev, "\x81\x0d\x01\x03", 4, (int*)&transferred, rbuff, 1024 ); // Reply: Ignored, 820d050900300500

		} else if( result < 0 ) {
			fprintf( stderr, "Chip type not supported. Aborting...\n" );
			return -1;
		}

		// Check for read protection
		wch_link_command( dev, "\x81\x06\x01\x01", 4, (int*)&transferred, rbuff, 1024 );
		if(transferred != 4) {
			fprintf(stderr, "Error: could not get read protection status\n");
			return -1;
		}

		if(rbuff[3] == 0x01) {
			fprintf(stderr, "Read protection: enabled\n");
		} else {
			fprintf(stderr, "Read protection: disabled\n");
		}
		if(flash_size) iss->flash_size = flash_size*1024;
#endif
	return 0;
}

static int LEControl3v3( void * d, int bOn )
{
	libusb_device_handle * dev = ((struct LinkEProgrammerStruct*)d)->devh;

	if( bOn )
		wch_link_command( (libusb_device_handle *)dev, "\x81\x0d\x01\x09", 4, 0, 0, 0 );
	else
		wch_link_command( (libusb_device_handle *)dev, "\x81\x0d\x01\x0a", 4, 0, 0, 0 );
	return 0;
}

static int LEControl5v( void * d, int bOn )
{
	libusb_device_handle * dev = ((struct LinkEProgrammerStruct*)d)->devh;

	if( bOn )
		wch_link_command( (libusb_device_handle *)dev, "\x81\x0d\x01\x0b", 4, 0, 0, 0 );
	else
		wch_link_command( (libusb_device_handle *)dev, "\x81\x0d\x01\x0c", 4, 0, 0, 0 );
	return 0;
}

// Official unbrick unreliable on x-series devices.
/*
static int LEUnbrick( void * d )
{
	printf( "Sending unbrick\n" );
	libusb_device_handle * dev = ((struct LinkEProgrammerStruct*)d)->devh;
	wch_link_command( (libusb_device_handle *)dev, "\x81\x0d\x01\x0f\x09", 5, 0, 0, 0 );
	printf( "Done unbrick\n" );
	return 0;
}
*/

static int LEConfigureNRSTAsGPIO( void * d, int one_if_yes_gpio )
{
	libusb_device_handle * dev = ((struct LinkEProgrammerStruct*)d)->devh;

	if( one_if_yes_gpio )
	{
		wch_link_multicommands( (libusb_device_handle *)dev, 2, 11, "\x81\x06\x08\x02\xff\xff\xff\xff\xff\xff\xff", 4, "\x81\x0b\x01\x01" );
	}
	else
	{
		wch_link_multicommands( (libusb_device_handle *)dev, 2, 11, "\x81\x06\x08\x02\xf7\xff\xff\xff\xff\xff\xff", 4, "\x81\x0b\x01\x01" );
	}
	return 0;
}

static int LEConfigureReadProtection( void * d, int one_if_yes_protect )
{
	libusb_device_handle * dev = ((struct LinkEProgrammerStruct*)d)->devh;
	struct InternalState * iss = (struct InternalState*)(((struct ProgrammerStructBase*)d)->internal);

	
	if(	iss->target_chip_type == CHIP_CH57x ||
		  iss->target_chip_type == CHIP_CH58x ||
		  iss->target_chip_type == CHIP_CH585 ||
		  iss->target_chip_type == CHIP_CH59x )
		{
			fprintf( stderr, "This MCU doesn't support %s read-protection via LinkE\n", one_if_yes_protect?"enabling":"disabling");
			return -1;
		}

	wch_link_command( dev, "\x81\x11\x01\x09", 4, 0, 0, 0 );

	if( one_if_yes_protect )
	{
		if( iss->target_chip_type == CHIP_CH570 ) wch_link_multicommands( (libusb_device_handle *)dev, 2, 11, "\x81\x06\x07\x03\xff\xff\xff\xff\xff\xff\xff", 4, "\x81\x0b\x01\x01" );
		else wch_link_multicommands( (libusb_device_handle *)dev, 2, 11, "\x81\x06\x08\x03\xf7\xff\xff\xff\xff\xff\xff", 4, "\x81\x0b\x01\x01" );
	}
	else
	{
		fprintf( stderr, "Disabling read protection\n" );
		if( iss->target_chip_type == CHIP_CH570 ) wch_link_multicommands( (libusb_device_handle *)dev, 2, 11, "\x81\x06\x07\x02\xff\xff\xff\xff\xff\xff\xff", 4, "\x81\x0b\x01\x01" );
		else wch_link_multicommands( (libusb_device_handle *)dev, 2, 11, "\x81\x06\x08\x02\xf7\xff\xff\xff\xff\xff\xff", 4, "\x81\x0b\x01\x01" );
	}
	return 0;
}

int LEExit( void * d )
{
	libusb_device_handle * dev = ((struct LinkEProgrammerStruct*)d)->devh;
	wch_link_command( (libusb_device_handle *)dev, "\x81\x0d\x01\xff", 4, 0, 0, 0);
	return 0;
}

void * TryInit_WCHLinkE()
{
	libusb_device_handle * wch_linke_devh;
	wch_linke_devh = wch_link_base_setup(0);
	if( !wch_linke_devh ) return 0;

	struct LinkEProgrammerStruct * ret = malloc( sizeof( struct LinkEProgrammerStruct ) );
	memset( ret, 0, sizeof( *ret ) );
	ret->devh = wch_linke_devh;
	ret->lasthaltmode = 0;

	MCF.ReadReg32 = LEReadReg32;
	MCF.WriteReg32 = LEWriteReg32;
	MCF.FlushLLCommands = LEFlushLLCommands;

	MCF.ResetInterface = LEResetInterface;
	MCF.SetupInterface = LESetupInterface;
	MCF.Control3v3 = LEControl3v3;
	MCF.Control5v = LEControl5v;
	//MCF.Unbrick = LEUnbrick; // 
	MCF.ConfigureNRSTAsGPIO = LEConfigureNRSTAsGPIO;
	MCF.ConfigureReadProtection = LEConfigureReadProtection;

	MCF.Exit = LEExit;
	return ret;
};


#if 1

struct BootloaderBlob
{
	const uint8_t * blob;
	int len;
};

// Flash Bootloader for V20x and V30x series MCUs
struct BootloaderBlob bootloader_v1 = {
	.blob = (const uint8_t*)
	"\x93\x77\x15\x00\x41\x11\x99\xCF\xB7\x06\x67\x45\xB7\x27\x02\x40" \
	"\x93\x86\x36\x12\x37\x97\xEF\xCD\xD4\xC3\x13\x07\xB7\x9A\xD8\xC3" \
	"\xD4\xD3\xD8\xD3\x93\x77\x25\x00\x9D\xC7\xB7\x27\x02\x40\x98\x4B" \
	"\xAD\x66\x37\x38\x00\x40\x13\x67\x47\x00\x98\xCB\x98\x4B\x93\x86" \
	"\xA6\xAA\x13\x67\x07\x04\x98\xCB\xD8\x47\x05\x8B\x63\x1F\x07\x10" \
	"\x98\x4B\x6D\x9B\x98\xCB\x93\x77\x45\x00\xA9\xCB\x93\x07\xF6\x07" \
	"\x9D\x83\x2E\xC0\x2D\x68\x81\x76\x3E\xC4\xB7\x08\x02\x00\xB7\x27" \
	"\x02\x40\x37\x33\x00\x40\x13\x08\xA8\xAA\xFD\x16\x98\x4B\x33\x67" \
	"\x17\x01\x98\xCB\x02\x47\xD8\xCB\x98\x4B\x13\x67\x07\x04\x98\xCB" \
	"\xD8\x47\x05\x8B\x71\xEF\x98\x4B\x75\x8F\x98\xCB\x02\x47\x13\x07" \
	"\x07\x08\x3A\xC0\x22\x47\x7D\x17\x3A\xC4\x69\xFB\x93\x77\x85\x00" \
	"\xED\xC3\x93\x07\xF6\x07\x2E\xC0\x9D\x83\x37\x27\x02\x40\x3E\xC4" \
	"\x1C\x4B\xC1\x66\x37\x08\x08\x00\xD5\x8F\x1C\xCB\xA1\x48\x37\x17" \
	"\x00\x20\xB7\x27\x02\x40\x37\x03\x04\x00\x94\x4B\xB3\xE6\x06\x01" \
	"\x94\xCB\xD4\x47\x85\x8A\xF5\xFE\x82\x46\x3A\x8E\x36\xC2\x46\xC6" \
	"\x92\x46\x83\x2E\x07\x00\x41\x07\x23\xA0\xD6\x01\x92\x46\x83\x2E" \
	"\x47\xFF\x23\xA2\xD6\x01\x92\x46\x83\x2E\x87\xFF\x23\xA4\xD6\x01" \
	"\x92\x46\x03\x2E\xCE\x00\x23\xA6\xC6\x01\x94\x4B\xB3\xE6\x66\x00" \
	"\x94\xCB\xD4\x47\x85\x8A\xF5\xFE\x92\x46\x3A\x8E\xC1\x06\x36\xC2" \
	"\xB2\x46\xFD\x16\x36\xC6\xCD\xFE\x82\x46\xD4\xCB\x94\x4B\x93\xE6" \
	"\x06\x04\x94\xCB\xD4\x47\x85\x8A\xF5\xFE\xD4\x47\xD1\x8A\x85\xC6" \
	"\xD8\x47\xB7\x06\xF3\xFF\xFD\x16\x13\x67\x47\x01\xD8\xC7\x98\x4B" \
	"\x21\x45\x75\x8F\x98\xCB\x41\x01\x02\x90\x23\x20\xD8\x00\xE9\xBD" \
	"\x23\x20\x03\x01\x31\xBF\x82\x46\x93\x86\x06\x08\x36\xC0\xA2\x46" \
	"\xFD\x16\x36\xC4\xB9\xFA\x98\x4B\xB7\x06\xF3\xFF\xFD\x16\x75\x8F" \
	"\x98\xCB\x41\x89\x15\xC9\x2E\xC0\x0D\x06\x02\xC4\x09\x82\x32\xC6" \
	"\xB7\x17\x00\x20\x98\x43\x13\x86\x47\x00\xA2\x47\x82\x46\x8A\x07" \
	"\xB6\x97\x9C\x43\x63\x1C\xF7\x00\xA2\x47\x85\x07\x3E\xC4\xA2\x46" \
	"\x32\x47\xB2\x87\xE3\xE0\xE6\xFE\x01\x45\x71\xBF\x41\x45\x61\xBF" \
	"\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff" \
	"\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff" \
	"\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff",
	.len = 512
};

struct BootloaderBlob bootloader_v2 = {
	.blob = (const uint8_t*)
	"\x93\x77\x15\x00\x41\x11\x99\xcf\xb7\x06\x67\x45\xb7\x27\x02\x40" \
	"\x93\x86\x36\x12\x37\x97\xef\xcd\xd4\xc3\x13\x07\xb7\x9a\xd8\xc3" \
	"\xd4\xd3\xd8\xd3\x93\x77\x25\x00\x95\xc7\xb7\x27\x02\x40\x98\x4b" \
	"\xad\x66\x37\x38\x00\x40\x13\x67\x47\x00\x98\xcb\x98\x4b\x93\x86" \
	"\xa6\xaa\x13\x67\x07\x04\x98\xcb\xd8\x47\x05\x8b\x61\xeb\x98\x4b" \
	"\x6d\x9b\x98\xcb\x93\x77\x45\x00\xa9\xcb\x93\x07\xf6\x0f\xa1\x83" \
	"\x2e\xc0\x2d\x68\x81\x76\x3e\xc4\xb7\x08\x02\x00\xb7\x27\x02\x40" \
	"\x37\x33\x00\x40\x13\x08\xa8\xaa\xfd\x16\x98\x4b\x33\x67\x17\x01" \
	"\x98\xcb\x02\x47\xd8\xcb\x98\x4b\x13\x67\x07\x04\x98\xcb\xd8\x47" \
	"\x05\x8b\x41\xeb\x98\x4b\x75\x8f\x98\xcb\x02\x47\x13\x07\x07\x10" \
	"\x3a\xc0\x22\x47\x7d\x17\x3a\xc4\x69\xfb\x93\x77\x85\x00\xd5\xcb" \
	"\x93\x07\xf6\x0f\x2e\xc0\xa1\x83\x3e\xc4\x37\x27\x02\x40\x1c\x4b" \
	"\xc1\x66\x41\x68\xd5\x8f\x1c\xcb\xb7\x16\x00\x20\xb7\x27\x02\x40" \
	"\x93\x08\x00\x04\x37\x03\x20\x00\x98\x4b\x33\x67\x07\x01\x98\xcb" \
	"\xd8\x47\x05\x8b\x75\xff\x02\x47\x3a\xc2\x46\xc6\x32\x47\x0d\xef" \
	"\x98\x4b\x33\x67\x67\x00\x98\xcb\xd8\x47\x05\x8b\x75\xff\xd8\x47" \
	"\x41\x8b\x39\xc3\xd8\x47\xc1\x76\xfd\x16\x13\x67\x07\x01\xd8\xc7" \
	"\x98\x4b\x21\x45\x75\x8f\x98\xcb\x41\x01\x02\x90\x23\x20\xd8\x00" \
	"\x25\xb7\x23\x20\x03\x01\xa5\xb7\x12\x47\x13\x8e\x46\x00\x94\x42" \
	"\x14\xc3\x12\x47\x11\x07\x3a\xc2\x32\x47\x7d\x17\x3a\xc6\xd8\x47" \
	"\x09\x8b\x75\xff\xf2\x86\x5d\xb7\x02\x47\x13\x07\x07\x10\x3a\xc0" \
	"\x22\x47\x7d\x17\x3a\xc4\x49\xf3\x98\x4b\xc1\x76\xfd\x16\x75\x8f" \
	"\x98\xcb\x41\x89\x15\xc9\x2e\xc0\x0d\x06\x02\xc4\x09\x82\x32\xc6" \
	"\xb7\x17\x00\x20\x98\x43\x13\x86\x47\x00\xa2\x47\x82\x46\x8a\x07" \
	"\xb6\x97\x9c\x43\x63\x1c\xf7\x00\xa2\x47\x85\x07\x3e\xc4\xa2\x46" \
	"\x32\x47\xb2\x87\xe3\xe0\xe6\xfe\x01\x45\xbd\xbf\x41\x45\xad\xbf" \
	"\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff" \
	"\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff" \
	"\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff" \
	"\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff" \
	"\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff" \
	"\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff",
	.len = 512
};

// Flashloader for CH58x, CH59x and CH570/2(?)
struct BootloaderBlob bootloader_v3 = {
	.blob = (const uint8_t*)
	"\x79\x71\x22\xd4\x4a\xd0\x56\xca\x06\xd6\x26\xd2\x4e\xce\x52\xcc" \
	"\x5a\xc8\x5e\xc6\x62\xc4\x93\x77\x15\x00\x2a\x84\xae\x8a\x32\x89" \
	"\xc1\xe3\x93\x77\x24\x00\x99\xcb\xb7\x86\x07\x00\x01\x46\x81\x45" \
	"\x05\x45\x49\x2a\x93\x77\xf5\x0f\x09\x45\xa5\xef\x93\x77\x44\x00" \
	"\x91\xcb\x85\x66\x01\x46\xd6\x85\x05\x45\xad\x2a\x93\x77\xf5\x0f" \
	"\x11\x45\xa5\xe3\x93\x77\x84\x01\x01\x4a\xd9\xcf\xb7\x54\x00\x20" \
	"\x13\x09\xf9\x0f\x93\x84\x04\x10\x13\x59\x89\x00\x01\x4a\xb3\x8a" \
	"\x9a\x40\x93\x7b\x84\x00\x13\x7c\x04\x01\x33\x8b\x9a\x00\x63\x96" \
	"\x0b\x04\x63\x06\x0c\x06\x93\x89\x04\xf0\x93\x06\x00\x10\x4e\x86" \
	"\xda\x85\x0d\x45\x05\x2a\x13\x75\xf5\x0f\x21\xc5\x41\x45\x11\xa8" \
	"\x81\x46\x01\x46\x81\x45\x21\x45\x31\x2a\x93\x77\xf5\x0f\x05\x45" \
	"\xad\xdb\xb2\x50\x22\x54\x92\x54\x02\x59\xf2\x49\x62\x4a\xd2\x4a" \
	"\x42\x4b\xb2\x4b\x22\x4c\x45\x61\x02\x90\x93\x06\x00\x10\x13\x86" \
	"\x04\xf0\xda\x85\x09\x45\xfd\x20\x13\x75\xf5\x0f\x5d\xd1\x21\x45" \
	"\xc9\xbf\x83\xa7\x09\x00\x91\x09\x3e\x9a\xe3\x9c\x34\xff\x7d\x19" \
	"\x93\x84\x04\x10\xe3\x13\x09\xf8\x41\x88\x01\x45\x5d\xd8\xb7\x67" \
	"\x00\x20\x9c\x4b\xe3\x87\x47\xfb\x51\xbf\x23\x03\x04\x80\x95\x47" \
	"\x23\x03\xf4\x80\x23\x02\xa4\x80\x82\x80\x83\x07\x64\x80\xe3\xce" \
	"\x07\xfe\x23\x03\x04\x80\x82\x80\x83\x07\x64\x80\xe3\xce\x07\xfe" \
	"\x03\x45\x44\x80\x82\x80\x83\x07\x64\x80\xe3\xce\x07\xfe\x23\x02" \
	"\xa4\x80\x82\x80\x41\x11\x26\xc4\x4a\xc2\x4e\xc0\x06\xc6\x13\x77" \
	"\xf5\x0b\xad\x47\xaa\x89\x2e\x89\x95\x44\x63\x06\xf7\x00\x19\x45" \
	"\x6d\x37\x65\x3f\x8d\x44\x4e\x85\x4d\x37\xfd\x59\xfd\x14\x63\x98" \
	"\x34\x01\xb2\x40\xa2\x44\x12\x49\x82\x49\x41\x01\x82\x80\x13\x55" \
	"\x09\x01\x13\x75\xf5\x0f\x45\x3f\x22\x09\xcd\xb7\x01\x11\x26\xcc" \
	"\x06\xce\xb7\x04\x08\x00\x51\x37\x15\x45\x85\x3f\x71\x37\x69\x37" \
	"\x2a\xc6\xa5\x3f\x32\x45\x93\x77\x15\x00\x89\xeb\x13\x65\x15\x00" \
	"\x13\x75\xf5\x0f\xf2\x40\xe2\x44\x05\x61\x82\x80\xfd\x14\xe9\xfc" \
	"\x01\x45\xcd\xbf\x39\x71\x26\xdc\x4a\xda\x4e\xd8\x52\xd6\x56\xd4" \
	"\x5a\xd2\x5e\xd0\x06\xde\x62\xce\x66\xcc\xb7\xe7\x00\xe0\x7d\x57" \
	"\x83\xaa\x07\x00\x22\xc6\x03\xaa\x47\x00\x23\xa0\xe7\x18\x23\xa2" \
	"\xe7\x18\xb7\x17\x00\x40\x13\x07\x70\x05\x23\x80\xe7\x04\x13\x07" \
	"\x80\xfa\x23\x80\xe7\x04\x83\xc7\x47\x04\x93\x0b\x75\xff\xb6\x84" \
	"\xe2\x07\x93\xfb\xfb\x0f\x85\x46\xaa\x89\x2e\x8b\x32\x89\x37\x24" \
	"\x00\x40\xe1\x87\x01\x57\x63\xfa\x76\x01\x63\x08\xd5\x00\x89\x46" \
	"\x13\x07\x00\x02\x63\x13\xd5\x00\x01\x57\xd9\x8f\x93\xf7\xf7\x0f" \
	"\xb7\x1c\x00\x40\x23\x82\xfc\x04\x11\x47\x23\x03\xe4\x80\x13\x05" \
	"\xf0\x0f\x65\x3d\x09\x4c\xd1\x35\x63\x6d\x7c\x11\xb7\x05\x07\x00" \
	"\xda\x95\x37\x87\x07\x00\x79\x55\x63\xf2\xe5\x04\xb3\x87\x95\x00" \
	"\x63\x6e\xf7\x02\x37\x0b\x08\x00\xa9\x47\x33\xeb\x65\x01\x63\x99" \
	"\xf9\x06\x89\xe4\x81\x44\x51\x3d\x26\x85\x0d\xa0\xda\x85\x09\x45" \
	"\x55\x3d\x05\x09\x03\x45\xf9\xff\xfd\x14\x05\x0b\x69\x3d\x81\xc4" \
	"\x93\x77\xfb\x0f\xfd\xf7\xdd\x35\x69\xfd\x7d\x55\xb7\x17\x00\x40" \
	"\x13\x07\x70\x05\x23\x80\xe7\x04\x13\x07\x80\xfa\x23\x80\xe7\x04" \
	"\x03\xc7\x47\x04\x41\x8b\x23\x82\xe7\x04\xf2\x50\xb7\xe7\x00\xe0" \
	"\x23\xa0\x57\x11\x23\xa2\x47\x11\xe2\x54\x32\x44\x52\x59\xc2\x59" \
	"\x32\x5a\xa2\x5a\x12\x5b\x82\x5b\x72\x4c\xe2\x4c\x21\x61\x82\x80" \
	"\xa5\x47\x63\x95\xf9\x06\x85\x69\x13\x09\xf0\x0f\xb3\x06\x99\x00" \
	"\xb3\x74\x2b\x01\xb6\x94\x13\x49\xf9\xff\xb3\x74\x99\x00\x85\x6b" \
	"\x33\x79\x69\x01\x41\x6b\x93\x87\xf9\xff\xb3\xf7\x27\x01\x99\xe3" \
	"\x63\xfc\x34\x01\x93\xd9\x49\x00\xc1\x47\xe3\xe6\x37\xff\x99\xbf" \
	"\x05\x69\xc1\x69\x7d\x19\xd9\xb7\x13\x05\x80\x0d\x63\x88\x69\x01" \
	"\x13\x05\x00\x02\x63\x84\x79\x01\x13\x05\x10\x08\xca\x85\xdd\x3b" \
	"\x35\x3d\x21\xdd\x4e\x99\xb3\x84\x34\x41\xd9\xb7\xda\x85\x2d\x45" \
	"\xd5\x33\xca\x94\xe3\x00\x99\xf2\x05\x09\x7d\x3b\xa3\x0f\xa9\xfe" \
	"\xd5\xbf\x93\x87\xf9\xff\x93\xf7\xf7\x0f\x63\x61\xfc\x0c\x83\xc7" \
	"\x1c\x04\x13\x07\x30\x08\x63\x1f\xf7\x04\x37\x07\x08\x00\x63\x6b" \
	"\xeb\x04\xb3\x07\x9b\x00\xb7\x06\x10\x00\x63\xf5\xd7\x04\x33\x4b" \
	"\xeb\x00\x89\x47\x63\x93\xf9\x06\x89\x80\xd5\x49\xe3\x8c\x04\xec" \
	"\xda\x85\x09\x45\x41\x3b\x11\x09\x03\x27\xc9\xff\x91\x47\x23\x20" \
	"\xe4\x80\x03\x07\x64\x80\xe3\x4e\x07\xfe\x23\x03\x34\x81\xfd\x17" \
	"\xed\xfb\xfd\x14\x11\x0b\x81\xc4\x93\x77\xfb\x0f\xe9\xff\x7d\x33" \
	"\x71\xf5\xe1\xb5\xb7\x17\x00\x40\x03\xc7\x57\x04\xb7\x07\x08\x00" \
	"\x13\x77\x07\x02\x19\xe3\xb7\x87\x07\x00\x79\x55\xe3\x78\xfb\xea" \
	"\x33\x07\x9b\x00\xe3\xff\xe7\xf8\x55\xb5\x85\x47\xe3\x82\xf9\xf2" \
	"\xda\x85\x2d\x45\x05\x3b\x93\x89\xf4\xff\xe3\x85\x04\xe6\x29\x33" \
	"\x93\xf7\x39\x00\x91\xeb\x83\x26\x04\x80\x03\x27\x09\x00\x93\x07" \
	"\x49\x00\xe3\x9a\xe6\xe4\x3e\x89\xce\x84\xf1\xbf\xb5\x47\x63\x96" \
	"\xf9\x00\x13\x05\x90\x0b\xd1\x31\x35\xbd\xb1\x47\x13\x05\xb0\x0a" \
	"\xe3\x8b\xf9\xfe\x99\x47\x63\x91\xf9\x04\xb7\x05\x08\x00\xb3\x65" \
	"\xbb\x00\x2d\x45\xc5\x31\x81\x44\x8d\x4b\xa1\x49\x75\x39\x63\x96" \
	"\x74\x01\x83\x27\x04\x80\x23\x20\xf9\x00\x85\x04\xe3\x98\x34\xff" \
	"\x83\x27\x04\x80\x13\x17\x2b\x01\x63\x55\x07\x00\x23\x12\xf9\x00" \
	"\xd5\xbb\x23\x22\xf9\x00\xfd\xb3\x9d\x47\x63\x99\xf9\x02\x81\x45" \
	"\x13\x05\xb0\x04\x45\x31\xbd\x44\x23\x20\x09\x00\x23\x22\x09\x00" \
	"\xfd\x59\x9d\x39\x93\xf7\x74\x00\xca\x97\x03\xc7\x07\x00\xfd\x14" \
	"\x39\x8d\x23\x80\xa7\x00\xe3\x96\x34\xff\x6d\xbb\xa1\x47\x63\x92" \
	"\xf9\x04\x6d\x39\x81\x44\x63\x0d\x0b\x00\x8d\x47\x93\x04\xc0\x03" \
	"\x63\x08\xfb\x00\x93\x04\x00\x05\x63\x04\x8b\x01\x93\x04\x40\x04" \
	"\x13\x75\xc5\x07\xe3\x08\x95\xd8\x19\x45\x01\x39\x39\x39\x05\x45" \
	"\x29\x31\x26\x85\x0d\x39\x09\x45\x3d\x31\x49\x31\xe3\x1c\x05\xd6" \
	"\x69\xbb\x91\x47\x63\x99\xf9\x00\x13\x05\x60\x06\xfd\x36\xf5\x3e" \
	"\x13\x05\x90\x09\x0d\xb7\xe3\x8f\x09\xd4\xf1\x54\xa9\xbb\xff\xff" \
	"\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff" \
	"\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff" \
	"\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff" \
	"\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff" \
	"\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff" \
	"\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff" \
	"\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff" \
	"\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff" \
	"\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff" \
	"\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff" \
	"\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff" \
	"\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff" \
	"\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff",
	.len = 1536
};

// Flashloader for CH57x
struct BootloaderBlob bootloader_v4 = {
	.blob = (const uint8_t*)
	"\x79\x71\x22\xd4\x4a\xd0\x52\xcc\x06\xd6\x26\xd2\x4e\xce\x56\xca" \
	"\x5a\xc8\x5e\xc6\x93\x77\x15\x00\x2a\x84\x2e\x8a\x32\x89\x9d\xef" \
	"\x93\x77\x24\x00\x99\xcb\xb7\x86\x07\x00\x01\x46\x81\x45\x05\x45" \
	"\x69\x22\x93\x77\xf5\x0f\x09\x45\x9d\xeb\x93\x77\x44\x00\x91\xcb" \
	"\x85\x66\x01\x46\xd2\x85\x05\x45\x8d\x2a\x93\x77\xf5\x0f\x11\x45" \
	"\x99\xef\x93\x77\x84\x01\x9d\xe7\x01\x45\x11\xa8\x81\x46\x01\x46" \
	"\x81\x45\x21\x45\x99\x2a\x93\x77\xf5\x0f\x05\x45\xd5\xdb\xb2\x50" \
	"\x22\x54\x92\x54\x02\x59\xf2\x49\x62\x4a\xd2\x4a\x42\x4b\xb2\x4b" \
	"\x45\x61\x02\x90\xb7\x54\x00\x20\x13\x09\xf9\x0f\x93\x84\x04\x10" \
	"\x93\x7b\x84\x00\x13\x59\x89\x00\x81\x4a\x33\x0a\x9a\x40\x41\x88" \
	"\x33\x0b\x9a\x00\x63\x90\x0b\x02\x21\xc0\x93\x89\x04\xf0\x93\x06" \
	"\x00\x10\x4e\x86\xda\x85\x0d\x45\x09\x22\x13\x75\xf5\x0f\x19\xcd" \
	"\x41\x45\x75\xb7\x93\x06\x00\x10\x13\x86\x04\xf0\xda\x85\x09\x45" \
	"\xed\x20\x13\x75\xf5\x0f\x69\xd9\x21\x45\x51\xbf\x83\xa7\x09\x00" \
	"\x91\x09\xbe\x9a\xe3\x9c\x99\xfe\x7d\x19\x93\x84\x04\x10\xe3\x19" \
	"\x09\xfa\x3d\xd0\xb7\x67\x00\x20\x9c\x4b\xe3\x8f\x57\xf5\xc9\xb7" \
	"\x23\x03\x04\x80\x95\x47\x23\x03\xf4\x80\x23\x02\xa4\x80\x82\x80" \
	"\x83\x07\x64\x80\xe3\xce\x07\xfe\x23\x03\x04\x80\x82\x80\x83\x07" \
	"\x64\x80\xe3\xce\x07\xfe\x03\x45\x44\x80\x82\x80\x83\x07\x64\x80" \
	"\xe3\xce\x07\xfe\x23\x02\xa4\x80\x82\x80\x41\x11\x26\xc4\x4a\xc2" \
	"\x4e\xc0\x06\xc6\x13\x77\xf5\x0b\xad\x47\xaa\x89\x2e\x89\x95\x44" \
	"\x63\x06\xf7\x00\x19\x45\x6d\x37\x65\x3f\x8d\x44\x4e\x85\x4d\x37" \
	"\xfd\x59\xfd\x14\x63\x98\x34\x01\xb2\x40\xa2\x44\x12\x49\x82\x49" \
	"\x41\x01\x82\x80\x13\x55\x09\x01\x13\x75\xf5\x0f\x45\x3f\x22\x09" \
	"\xcd\xb7\x01\x11\x26\xcc\x06\xce\xb7\x04\x08\x00\x51\x37\x15\x45" \
	"\x85\x3f\x71\x37\x69\x37\x2a\xc6\xa5\x3f\x32\x45\x93\x77\x15\x00" \
	"\x89\xeb\x13\x65\x15\x00\x13\x75\xf5\x0f\xf2\x40\xe2\x44\x05\x61" \
	"\x82\x80\xfd\x14\xe9\xfc\x01\x45\xcd\xbf\x39\x71\x26\xdc\x4a\xda" \
	"\x4e\xd8\x52\xd6\x56\xd4\x5a\xd2\x5e\xd0\x06\xde\x62\xce\x66\xcc" \
	"\xb7\xe7\x00\xe0\x7d\x57\x83\xaa\x07\x00\x22\xc6\x03\xaa\x47\x00" \
	"\x23\xa0\xe7\x18\x23\xa2\xe7\x18\xb7\x17\x00\x40\x13\x07\x70\x05" \
	"\x23\x80\xe7\x04\x13\x07\x80\xfa\x23\x80\xe7\x04\x83\xc7\x47\x04" \
	"\x13\x09\x75\xff\xb6\x84\xe2\x07\x13\x79\xf9\x0f\x85\x46\xaa\x89" \
	"\xae\x8b\x32\x8b\x37\x24\x00\x40\xe1\x87\x01\x57\x63\xfa\x26\x01" \
	"\x63\x08\xd5\x00\x89\x46\x13\x07\x00\x02\x63\x13\xd5\x00\x01\x57" \
	"\xd9\x8f\x93\xf7\xf7\x0f\xb7\x1c\x00\x40\x23\x82\xfc\x04\x11\x47" \
	"\x23\x03\xe4\x80\x13\x05\xf0\x0f\x65\x3d\x09\x4c\xd1\x35\x63\x69" \
	"\x2c\x11\xb7\x07\x07\x00\xbe\x9b\x37\x87\x07\x00\x79\x55\x63\xfe" \
	"\xeb\x02\xb3\x87\x9b\x00\x63\x6a\xf7\x02\xa9\x47\x63\x99\xf9\x06" \
	"\x89\xe4\x81\x44\x71\x3d\x26\x85\x0d\xa0\xde\x85\x09\x45\x75\x3d" \
	"\x05\x0b\x03\x45\xfb\xff\xfd\x14\x85\x0b\x4d\x35\x81\xc4\x93\xf7" \
	"\xfb\x0f\xfd\xf7\xfd\x35\x69\xfd\x7d\x55\xb7\x17\x00\x40\x13\x07" \
	"\x70\x05\x23\x80\xe7\x04\x13\x07\x80\xfa\x23\x80\xe7\x04\x03\xc7" \
	"\x47\x04\x41\x8b\x23\x82\xe7\x04\xf2\x50\xb7\xe7\x00\xe0\x23\xa0" \
	"\x57\x11\x23\xa2\x47\x11\xe2\x54\x32\x44\x52\x59\xc2\x59\x32\x5a" \
	"\xa2\x5a\x12\x5b\x82\x5b\x72\x4c\xe2\x4c\x21\x61\x82\x80\xa5\x47" \
	"\x63\x95\xf9\x06\x85\x69\x13\x09\xf0\x0f\xb3\x06\x99\x00\xb3\xf4" \
	"\x2b\x01\xb6\x94\x13\x49\xf9\xff\xb3\x74\x99\x00\x41\x6b\x33\x79" \
	"\x79\x01\x85\x6b\x93\x87\xf9\xff\xb3\xf7\x27\x01\x99\xe3\x63\xfc" \
	"\x34\x01\x93\xd9\x49\x00\xc1\x47\xe3\xe6\x37\xff\x99\xbf\x05\x69" \
	"\xc1\x69\x7d\x19\xd9\xb7\x13\x05\x80\x0d\x63\x88\x69\x01\x13\x05" \
	"\x00\x02\x63\x84\x79\x01\x13\x05\x10\x08\xca\x85\xfd\x3b\x91\x35" \
	"\x21\xdd\x4e\x99\xb3\x84\x34\x41\xd9\xb7\xde\x85\x2d\x45\xf5\x33" \
	"\xda\x94\xe3\x00\x9b\xf2\x05\x0b\xd9\x33\xa3\x0f\xab\xfe\xd5\xbf" \
	"\x93\x87\xf9\xff\x93\xf7\xf7\x0f\x63\x6c\xfc\x08\x03\xc7\x5c\x04" \
	"\xb7\x07\x08\x00\x13\x77\x07\x02\x19\xe3\xb7\x87\x07\x00\x79\x55" \
	"\xe3\xfd\xfb\xf0\x33\x87\x9b\x00\xe3\xe9\xe7\xf0\x89\x47\x63\x90" \
	"\xf9\x04\x89\x80\x55\x49\xe3\x8e\x04\xec\xde\x85\x09\x45\x71\x3b" \
	"\x11\x0b\x03\x27\xcb\xff\x91\x47\x23\x20\xe4\x80\x03\x07\x64\x80" \
	"\xe3\x4e\x07\xfe\x23\x03\x24\x81\xfd\x17\xed\xfb\xfd\x14\x91\x0b" \
	"\x81\xc4\x93\xf7\xfb\x0f\xe9\xff\x6d\x3b\x71\xf5\xf1\xb5\x85\x47" \
	"\xe3\x87\xf9\xf4\xde\x85\x2d\x45\x8d\x33\x13\x89\xf4\xff\xe3\x8a" \
	"\x04\xe8\x35\x3b\x93\x77\x39\x00\x91\xeb\x83\x26\x04\x80\x03\x27" \
	"\x0b\x00\x93\x07\x4b\x00\xe3\x9f\xe6\xe6\x3e\x8b\xca\x84\xf1\xbf" \
	"\xa1\x47\x63\x92\xf9\x04\xb5\x3b\x81\x44\x63\x8d\x0b\x00\x8d\x47" \
	"\x93\x04\xc0\x03\x63\x88\xfb\x00\x93\x04\x00\x05\x63\x84\x8b\x01" \
	"\x93\x04\x40\x04\x13\x75\xc5\x07\xe3\x05\x95\xe4\x19\x45\xc9\x39" \
	"\xc5\x31\x05\x45\xf1\x31\x26\x85\xd5\x39\x09\x45\xc5\x39\x91\x33" \
	"\xe3\x19\x05\xe2\x91\xbd\xe3\x86\x09\xe2\xf1\x54\x25\xb5\xff\xff" \
	"\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff" \
	"\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff" \
	"\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff" \
	"\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff" \
	"\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff" \
	"\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff" \
	"\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff" \
	"\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff" \
	"\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff" \
	"\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff" \
	"\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff",
	.len = 1280
};

#endif

struct BootloaderBlob * GetFlashLoader( enum RiscVChip chip )
{
	switch(chip) {
		case CHIP_CH32V10x:
			fprintf( stderr, "Using bootloader v1\n" );
			return &bootloader_v1;
		case CHIP_CH57x:
			fprintf( stderr, "Using bootloader v4\n" );
			return &bootloader_v4;
		case CHIP_CH58x:
		case CHIP_CH59x:
			fprintf( stderr, "Using bootloader v3\n" );
			return &bootloader_v3;
		case CHIP_CH32V20x:
		case CHIP_CH32V30x:
		default:
			fprintf( stderr, "Using bootloader v2\n" );
			return &bootloader_v2;
	}
}

#if !FORCE_EXTERNAL_CHIP_DETECTION
static int InternalLinkEHaltMode( void * d, int mode )
{
	libusb_device_handle * dev = ((struct LinkEProgrammerStruct*)d)->devh;
	if( mode == ((struct LinkEProgrammerStruct*)d)->lasthaltmode )
		return 0;
	((struct LinkEProgrammerStruct*)d)->lasthaltmode = mode;
	
	if( mode == 0 )
	{
		printf( "Holding in reset\n" );
		// Part one "immediately" places the part into reset.  Part 2 says when we're done, leave part in reset.
    // Second command is not needed?
		wch_link_multicommands( (libusb_device_handle *)dev, 2, 4, "\x81\x0d\x01\x02", 4, "\x81\x0d\x01\x01" );
	}
	else if( mode == 1 )
	{
		// This is clearly not the "best" method to exit reset.  I don't know why this combination works.
		wch_link_multicommands( (libusb_device_handle *)dev, 3, 4, "\x81\x0b\x01\x01", 4, "\x81\x0d\x01\x02", 4, "\x81\x0d\x01\xff" );
	}
	else
	{
		return -999;
	}
	return 0;
}

static int LEWriteBinaryBlob( void * d, uint32_t address_to_write, uint32_t len, const uint8_t * blob )
{
	libusb_device_handle * dev = ((struct LinkEProgrammerStruct*)d)->devh;
	struct InternalState * iss = (struct InternalState*)(((struct LinkEProgrammerStruct*)d)->internal);

	InternalLinkEHaltMode( d, 0 );

	int i;
	int status;
	uint8_t rbuff[1024];
	int transferred;

	int padlen = ((len-1) & (~(iss->sector_size-1))) + iss->sector_size;

	fprintf( stderr, "len = %d, sector_size = %d padlen = %d chip_name = %s\n", len, iss->sector_size, padlen, iss->target_chip->name_str );

	if( iss->target_chip_type == CHIP_CH59x || iss->target_chip_type == CHIP_CH58x || iss->target_chip_type == CHIP_CH57x )
	{
		wch_link_command( (libusb_device_handle *)dev, "\x81\x0c\x02\x01\03", 5, 0, 0, 0 );
	}
	else
	{
		wch_link_command( (libusb_device_handle *)dev, "\x81\x06\x01\x01", 4, 0, 0, 0 );
		wch_link_command( (libusb_device_handle *)dev, "\x81\x06\x01\x01", 4, 0, 0, 0 ); // Not sure why but it seems to work better when we request twice.
	}
	
	// This contains the write data quantity, in bytes.  (The last 2 octets)
	// Then it just rollllls on in.
	char rksbuff[11] = { 0x81, 0x01, 0x08,
	                    // Address to write
	                    (uint8_t)(address_to_write >> 24), (uint8_t)(address_to_write >> 16),
	                    (uint8_t)(address_to_write >> 8), (uint8_t)(address_to_write & 0xff),
	                    // Length to write
	                    (uint8_t)(len >> 24), (uint8_t)(len >> 16),
	                    (uint8_t)(len >> 8), (uint8_t)(len & 0xff) };
	wch_link_command( (libusb_device_handle *)dev, rksbuff, 11, 0, 0, 0 );
	wch_link_command( (libusb_device_handle *)dev, "\x81\x02\x01\x05", 4, 0, 0, 0 );

	struct BootloaderBlob *bootloader = GetFlashLoader(iss->target_chip_type);
	int pplace = 0;
	for( pplace = 0; pplace < bootloader->len; pplace += iss->sector_size )
	{
		WCHCHECK( libusb_bulk_transfer( (libusb_device_handle *)dev, 0x02, (uint8_t*)(bootloader->blob+pplace), iss->sector_size, &transferred, WCHTIMEOUT ) );
	}
	
	for( i = 0; i < 10; i++ )
	{
		wch_link_command( (libusb_device_handle *)dev, "\x81\x02\x01\x07", 4, &transferred, rbuff, 1024 );
		if( transferred == 4 && rbuff[0] == 0x82 && rbuff[1] == 0x02 && rbuff[2] == 0x01 && rbuff[3] == 0x07 )
		{
			break;
		}
	} 
	if( i == 10 )
	{
		fprintf( stderr, "Error, confusing responses to 02/01/07\n" );
		exit( -109 );
	}

	wch_link_command( (libusb_device_handle *)dev, "\x81\x02\x01\x02", 4, 0, 0, 0 );

	for( pplace = 0; pplace < padlen; pplace += iss->sector_size )
	{
		if( pplace + iss->sector_size > len )
		{
			uint8_t paddeddata[iss->sector_size];
			int gap = pplace + iss->sector_size - len;
			int okcopy = len - pplace;
			memcpy( paddeddata, blob + pplace, okcopy );
			memset( paddeddata + okcopy, 0xff, gap );
			WCHCHECK( libusb_bulk_transfer( (libusb_device_handle *)dev, 0x02, paddeddata, iss->sector_size, &transferred, WCHTIMEOUT ) );
		}
		else
		{
			WCHCHECK( libusb_bulk_transfer( (libusb_device_handle *)dev, 0x02, ((uint8_t*)blob)+pplace, iss->sector_size, &transferred, WCHTIMEOUT ) );
		}
	}
  // wch_link_command( (libusb_device_handle *)dev, "\x81\x02\x01\x08", 4, 0, rbuff, 1024 );

	return 0;
}
#endif // !FORCE_EXTERNAL_CHIP_DETECTION

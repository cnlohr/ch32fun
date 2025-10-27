
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "libusb.h"
#include "minichlink.h"
#include "chips.h"

/*************************
 * All commands here are "inspired" by https://github.com/wagiminator/MCU-Flash-Tools/blob/main/chprog.py
 * but Wagiminator is awesome so it's ok. (To the best of my knowledge that's the license)
 *************************/
#define WCH_XOR_KEY_LEN_8   8
#define WCH_UID_LEN_8       8
#define WCH_USB_EP_OUT      0x02
#define WCH_USB_EP_IN       0x82
#define WCH_USB_TIMEOUT     5000
#define WCHCHECK(x) if( (status = x) ) { fprintf( stderr, "Bad USB Operation on " __FILE__ ":%d (%d)\n", __LINE__, status ); exit( status ); }

struct ISPProgrammerStruct
{
	void * internal;
	libusb_device_handle * devh;
	int lasthaltmode; // For non-003 chips
};

static inline libusb_device_handle * isp_base_setup( int inhibit_startup )
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

	for (i = 0; i < cnt; i++) {
		libusb_device *device = list[i];
		struct libusb_device_descriptor desc;
		int r = libusb_get_device_descriptor(device,&desc);
		if( r == 0 && (desc.idVendor == 0x4348 || desc.idVendor == 0x1a86) && desc.idProduct == 0x55e0) { found = device; }
	}

	if( !found )
	{
		return 0;
	}

	libusb_device_handle * devh;
	status = libusb_open( found, &devh );
	if( status )
	{
		fprintf( stderr, "Error: couldn't access bootloader (libusb_open() = %d)\n", status );
		return 0;
	}
		
	WCHCHECK( libusb_claim_interface(devh, 0) );

	return devh;
}

void wch_isp_command( libusb_device_handle * devh, const void * command_v, int commandlen, int * transferred, uint8_t * reply, int replymax )
{
	uint8_t * command = (uint8_t*)command_v;
	uint8_t buffer[1024];
	int got_to_recv = 0;
	int status;
	int transferred_local;
	if( !transferred ) transferred = &transferred_local;
	status = libusb_bulk_transfer( devh, WCH_USB_EP_OUT, command, commandlen, transferred, WCH_USB_TIMEOUT );
	if( status ) goto sendfail;
	got_to_recv = 1;
	if( !reply )
	{
		reply = buffer; replymax = sizeof( buffer );
	}

//	printf("wch_isp_command send (%d)", commandlen); for(int i = 0; i< commandlen; printf(" %02x",command[i++])); printf("\n");

	status = libusb_bulk_transfer( devh, WCH_USB_EP_IN, reply, replymax, transferred, WCH_USB_TIMEOUT );

//	printf("wch_isp_command reply (%d)", *transferred); for(int i = 0; i< *transferred; printf(" %02x",reply[i++])); printf("\n"); 

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

int ISPWriteReg32( void * dev, uint8_t reg_7_bit, uint32_t command ) { fprintf( stderr, "ISPWriteReg32\n" ); return -100; }
int ISPReadReg32( void * dev, uint8_t reg_7_bit, uint32_t * commandresp ) { fprintf( stderr, "ISPReadReg32\n" ); return -100; }
int ISPWriteWord( void * dev, uint32_t address_to_write, uint32_t data ) { fprintf( stderr, "ISPWriteWord\n" ); return -100; }

int ISPSetupInterface( void * d ) {
	libusb_device_handle * dev = ((struct ISPProgrammerStruct*)d)->devh;
	struct InternalState * iss = (struct InternalState*)(((struct ProgrammerStructBase*)d)->internal);
	uint8_t rbuff[1024];
	uint16_t chip_type = 0;
	uint8_t uid[WCH_UID_LEN_8];
	uint8_t xor_key[WCH_XOR_KEY_LEN_8];
	uint32_t transferred = 0;
	const struct RiscVChip_s* chip = NULL;

	// request chip id
	wch_isp_command( dev, "\xa1\x12\x00\x52\x11MCU ISP & WCH.CN", 21, (int*)&transferred, rbuff, 1024 );
	if(transferred == 6) {
		// printf("id response: %02x %02x %02x %02x %02x %02x\n", rbuff[0], rbuff[1], rbuff[2], rbuff[3], rbuff[4], rbuff[5]);
		chip_type = rbuff[4] | rbuff[5] << 8;
		
		if( (chip = FindChipISP( chip_type )) != NULL )
		{
			iss->target_chip = chip;
			if( iss->target_chip == &ch32x035 ||
			    iss->target_chip == &ch32l103 ||
			    iss->target_chip == &ch32v103 ||
			    iss->target_chip == &ch32v203 ||
			    iss->target_chip == &ch32v303 )
			{
				if( iss->target_chip == &ch32v103 )
				{
					if( rbuff[4] == 0x32 ) iss->flash_size = 32;
					else iss->flash_size = 64;
				}
				else if( iss->target_chip == &ch32v203 )
				{
					switch ( rbuff[4] )
					{
					case 0x33:
					case 0x35:
					case 0x36:
					case 0x37:
						iss->flash_size = 32;
						break;
					
					case 0x30:
					case 0x31:
					case 0x32:
					case 0x3a:
					case 0x3b:
					case 0x3e:
						iss->flash_size = 64;
						break;

					default:
						iss->flash_size = 128;
						break;
					}
				}
				else if ( iss->target_chip == &ch32v303 )
				{
					if( rbuff[4] == 0x32 || rbuff[4] == 0x33 ) iss->flash_size = 128;
					else iss->flash_size = 128;
				}
				else // If CH32X035 of CH32L103
				{
					if ( (rbuff[4] & 0xf) == 7 ) iss->flash_size = 48;
					else iss->flash_size = 64;
				}
			}
			else
			{
				iss->flash_size = iss->target_chip->flash_size / 1024;
			}
		}
		else
		{
			printf( "ERROR: This chip is upsupported: %04x.\n", chip_type );
			return -1;
		}
		// printf("chip type: %04x\n", chip_type);
		// if(chip_type != 0x70 && chip_type != 0x72 && chip_type != 0x82 && chip_type != 0x91 && chip_type != 0x92) {
	}
	else 
	{
		printf("ERROR: Request Chip ID failed.\n");
		return -1;
	}
	printf( "Detected %s\n", iss->target_chip->name_str );
	uint8_t read_protection = 0;
	
	// read config
	wch_isp_command( dev, "\xa7\x02\x00\x1f\x00", 5, (int*)&transferred, rbuff, 1024 );
	if( transferred == 30 ) {
		if( iss->target_chip->protocol == PROTOCOL_CH5xx && iss->target_chip_type != CHIP_CH570) read_protection = (rbuff[14] & 0x80);
		else if( iss->target_chip->protocol == PROTOCOL_CH5xx && iss->target_chip_type == CHIP_CH570 ) read_protection = (rbuff[16] == 0x3a);
		else read_protection = (rbuff[6] == 0xa5);
		printf( "Bootloader version: %d.%d%d\n", rbuff[19], rbuff[20], rbuff[21] );
		printf( "Flash Storage: %d kB\n", iss->flash_size );
		iss->target_chip_type = iss->target_chip->family_id;
		// printf("bootloader: v%d.%d%d\n", rbuff[19], rbuff[20], rbuff[21]);
		printf( "Part UUID: %02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x\n", rbuff[22], rbuff[23], rbuff[24], rbuff[25], rbuff[26], rbuff[27], rbuff[28], rbuff[29] );
		printf( "Read protection: %s\n", (read_protection)?"disabled":"enabled" );
		// printf("chip uid: %02x %02x %02x %02x %02x %02x %02x %02x\n", rbuff[22], rbuff[23], rbuff[24], rbuff[25], rbuff[26], rbuff[27], rbuff[28], rbuff[29]);
		memcpy(uid, &rbuff[22], WCH_UID_LEN_8);
	}
	else {
		printf("ERROR: Read config failed.\n");
		return -2;
	}

	// create local encryption key
	uint8_t sum = 0;
	for(int i = 0; i < WCH_UID_LEN_8; i++) {
		sum += uid[i];
	}
	for(int i = 0; i < WCH_XOR_KEY_LEN_8 -1; i++) {
		xor_key[i] = sum;
	}
	xor_key[WCH_XOR_KEY_LEN_8 -1] = sum + chip_type;
	// printf("encryption key: %02x %02x %02x %02x %02x %02x %02x %02x\n", xor_key[0], xor_key[1], xor_key[2], xor_key[3], xor_key[4], xor_key[5], xor_key[6], xor_key[7]);

	// send encryption key
	sum = 0;
	for(int i = 0; i < WCH_XOR_KEY_LEN_8; i++) {
		sum += xor_key[i];
	}
	wch_isp_command( dev, "\xa3\x1e\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 33, (int*)&transferred, rbuff, 1024 );
	if(rbuff[4] != sum) {
		printf("ERROR: Failed to set encryption key.\n");
		return -3;
	}
	memcpy(iss->isp_xor_key, xor_key, WCH_XOR_KEY_LEN_8);

	return 0;
}

int ISPPrintChipInfo( void * d )
{
	libusb_device_handle * dev = ((struct ISPProgrammerStruct*)d)->devh;
	struct InternalState * iss = (struct InternalState*)(((struct ProgrammerStructBase*)d)->internal); 

	uint8_t rbuff[1024];
	uint32_t transferred = 0;

	wch_isp_command( dev, "\xa7\x02\x00\x1f\x00", 5, (int*)&transferred, rbuff, 1024 );

	// printf("config: %02x%02x%02x%02x%02x%02x%02x%02x %02x%02x%02x%02x\n", rbuff[6], rbuff[7], rbuff[8], rbuff[9], rbuff[10], rbuff[11], rbuff[12], rbuff[13], rbuff[14], rbuff[15], rbuff[16], rbuff[17]);
	switch (iss->target_chip_type)
	{
		case CHIP_CH32X03x:
		case CHIP_CH32L103:
		case CHIP_CH32V10x:
		case CHIP_CH32V20x:
		case CHIP_CH32V30x:
			printf("IWDG - %s\n", (rbuff[8]&1)?"enabled":"disabled");
			//System reset control under the stop mode
			printf("STOP_RST - %s\n", (rbuff[8]&2)?"disabled":"enabled");
			printf("STANDY_RST - %s\n", (rbuff[8]&4)?"disabled":"enabled");
			if( iss->target_chip_type == CHIP_CH32X03x ) printf("Reset pin - %s\n", ((rbuff[8]&0x18) == 0x18 )?"disabled":"enabled");
			printf("DATA0 - 0x%02x\n", rbuff[10]);
			printf("DATA1 - 0x%02x\n", rbuff[12]);
			printf("WRPR - 0x%02x%02x%02x%02x\n", rbuff[16], rbuff[15], rbuff[14], rbuff[13]);
			break;

		case CHIP_CH570:
		case CHIP_CH57x:
		case CHIP_CH58x:
		case CHIP_CH585:
		case CHIP_CH59x:
			{
				uint32_t option_bytes = *((uint32_t*)&rbuff[14]);
				// fprintf( stderr, "%08x\n", option_bytes );
				if (iss->target_chip_type == CHIP_CH570 || iss->target_chip_type == CHIP_CH585)
				{
					printf("Reset - %s\n", (option_bytes&0x8)?"enabled":"disabled");
					if (iss->target_chip_type == CHIP_CH570) printf("Reset pin - PA%d\n", (option_bytes&0x10)?7:8);
					else printf("Debug - %s\n", (option_bytes&0x10)?"enabled":"disabled");
					printf("IWDG - %s\n", (option_bytes&0x20)?"enabled":"disabled");
					printf("Bootloader - %s\n", (option_bytes&0x40)?"enabled":"disabled");
					// if (iss->target_chip_type == CHIP_CH570) printf("Readout protection - %s\n", ((option_bytes&0x00FF0000)==0x3a0000)?"disabled":"enabled");
					// else printf("Readout protection - %s\n", (option_bytes&80)?"disabled":"enabled");
				} 
				else 
				{
					printf("Reset - %s\n", (option_bytes&0x8)?"enabled":"disabled");
					printf("Bootloader pin - PB%d\n", (rbuff[12]&2)?22:11);
					printf("Debug - %s\n", (option_bytes&0x10)?"enabled":"disabled");
					printf("UART_NO_KEY - %s\n", (rbuff[12]&1)?"enabled":"disabled");
					// printf("Readout protection - %s\n", (option_bytes&80)?"disabled":"enabled");
				}
			}
		default:
			break;
	}
	return 0;
}

int ISPErase( void * d, uint32_t address, uint32_t length, int type ) {
	libusb_device_handle * dev = ((struct ISPProgrammerStruct*)d)->devh;
	struct InternalState * iss = (struct InternalState*)(((struct ProgrammerStructBase*)d)->internal);
	uint8_t cmd[7] = {0};
	uint8_t rbuff[1024];
	uint32_t transferred = 0;

	if(type == 1) {
		// whole flash erase, not implemented yet (it does a minimal erase currently)
		address = 0;
		length = iss->flash_size; // NOTE: this is the part that is not implemented yet, should be done in ISPSetupInterface
	}

	uint32_t size = (length +1023) / 1024;
	size = (size < 8) ? 8 : size;

	cmd[0] = '\xa4';
	cmd[1] = '\x04';
	cmd[3] = (uint8_t)size;
	cmd[4] = (uint8_t)(size >> 8);
	// printf("erase cmd: %02x%02x%02x%02x%02x%02x%02x\n", cmd[0], cmd[1], cmd[2], cmd[3], cmd[4], cmd[5], cmd[6]);
	wch_isp_command( dev, cmd, sizeof(cmd), (int*)&transferred, rbuff, 1024 );
	if(rbuff[4]) {
		printf("ERROR: Failed to erase chip.\n");
		return -2;
	}

	return 0;
}

int ISPWriteBinaryBlob( void * d, uint32_t address_to_write, uint32_t blob_size, const uint8_t * blob ) {
	libusb_device_handle * dev = ((struct ISPProgrammerStruct*)d)->devh;
	struct InternalState * iss = (struct InternalState*)(((struct ProgrammerStructBase*)d)->internal);
	uint8_t stream[3+5+56] = {0}; // 3 byte cmd, 5 byte header, 56 byte chunk
	uint32_t offset = 0;
	uint8_t rbuff[1024];
	uint32_t transferred = 0;

	if(WCH_XOR_KEY_LEN_8 != 8) {
		printf("ERROR: Only XOR key length of 8 is implemented.\n");
		return -1;
	}

	address_to_write = (address_to_write == 0x08000000) ? 0 : address_to_write;

	// erase flash
	ISPErase(d, address_to_write, blob_size, /*type=*/0);

	// write new blob
	stream[0] = '\xa5';
	stream[1] = 56 + 5; // packet length, 56 byte chunk + 5 byte header
	stream[2] = '\x00';

	uint32_t write_size = (blob_size%256)?(((blob_size/256) + 1) * 256):blob_size;

	for(int i = 0; i < ((write_size / 56) +1); i++) {
		for(int j = 0; j < 7; j++) { // xor key length means 7 blocks per 56 byte chunk
			for(int k = 0; k < WCH_XOR_KEY_LEN_8; k++) { // i,j,k for loop I feel like I'm back in Java 101
				uint32_t blob_idx = (i*56) + (j*8) + k;
				stream[(3+5) + (j*8) + k] = (blob_idx < blob_size ? blob[blob_idx] : 0xff) ^ iss->isp_xor_key[k];
			}
		}
		offset = address_to_write + (i * 56);
		memcpy(&stream[3], &offset, 4);
		stream[7] = (uint8_t)(blob_size -(i*56));
		wch_isp_command( dev, stream, sizeof(stream), (int*)&transferred, rbuff, 1024 );
		if(rbuff[4] != 0x00 && rbuff[4] != 0xfe && rbuff[4] != 0xf5) {
			printf("ERROR: Failed to write at offset %d.\n", offset);
			return -1;
		}
	}

	return 0;
}

int ISPHaltMode( void * d, int mode ) {
	libusb_device_handle * dev = ((struct ISPProgrammerStruct*)d)->devh;
	uint8_t rbuff[1024];
	uint32_t transferred = 0;

	if(mode == HALT_MODE_REBOOT) {
		wch_isp_command( dev, "\xa2\x01\x00\x01", 4, (int*)&transferred, rbuff, 1024 );
		printf("Rebooting...\n");
	}
	return 0;
}

int ISPConfigureNRSTAsGPIO( void * d, int one_if_yes_gpio )
{
	libusb_device_handle * dev = ((struct ISPProgrammerStruct*)d)->devh;
	struct InternalState * iss = (struct InternalState*)(((struct ProgrammerStructBase*)d)->internal);

	if( (iss->target_chip->protocol != PROTOCOL_CH5xx && iss->target_chip_type != CHIP_CH32X03x) )
	{
		fprintf( stderr, "ERROR: You can't change function of a reset pin on this chip\n" );
		return -1;	
	}

	uint8_t rbuff[1024];
	uint32_t transferred = 0;
	
	wch_isp_command( dev, "\xa7\x02\x00\x1f\x00", 5, (int*)&transferred, rbuff, 1024 );
	printf("Current config: %02x%02x%02x%02x%02x%02x%02x%02x %02x%02x%02x%02x\n", rbuff[6], rbuff[7], rbuff[8], rbuff[9], rbuff[10], rbuff[11], rbuff[12], rbuff[13], rbuff[14], rbuff[15], rbuff[16], rbuff[17]);

	if( iss->target_chip_type == CHIP_CH32X03x )
	{
		if( (rbuff[8]&0x18) == 0x18 && one_if_yes_gpio > 0 )
		{
			printf( "Reset pin is already set as GPIO!\n" );
			return 0;
		}
		else if( (rbuff[8]&0x18) != 0x18 && one_if_yes_gpio == 0 )
		{
			printf( "Reset pin is already set as reset!\n" );
			return 0;
		}
		else
		{
			if( one_if_yes_gpio )
			{
				rbuff[8] |= 0x18;
			}
			else
			{
				rbuff[8] &= ~(0x18);
			}

			uint8_t conf_buffer[17] = { 0xa8, 0x0e, 0x00, 0x07, 0x00, 
			                            rbuff[6], rbuff[7], rbuff[8], rbuff[9], rbuff[10], rbuff[11], rbuff[12], rbuff[13],
			                            rbuff[14], rbuff[15], rbuff[16], rbuff[17] };
			printf("New config: %02x%02x%02x%02x%02x%02x%02x%02x %02x%02x%02x%02x\n", conf_buffer[5], conf_buffer[6], conf_buffer[7], conf_buffer[8], conf_buffer[9], conf_buffer[10], conf_buffer[11], conf_buffer[12], conf_buffer[13], conf_buffer[14], conf_buffer[15], conf_buffer[16]);
			wch_isp_command( dev, conf_buffer, 17, (int*)&transferred, rbuff, 1024 );	
		}
	}
	else
	{
		if( !(rbuff[14]&0x8) && one_if_yes_gpio > 0 )
		{
			printf( "Reset pin is already set as GPIO!\n" );
			return 0;
		}
		else if( (rbuff[14]&0x8) && one_if_yes_gpio == 0 )
		{
			printf( "Reset pin is already set as reset!\n" );
			return 0;
		}
		else
		{
			if( one_if_yes_gpio )
			{
				rbuff[14] &= ~(0x8);
			}
			else
			{
				rbuff[14] |= 0x8;
			}

			uint8_t conf_buffer[17] = { 0xa8, 0x0e, 0x00, 0x07, 0x00, 
			                            rbuff[6], rbuff[7], rbuff[8], rbuff[9], rbuff[10], rbuff[11], rbuff[12], rbuff[13],
			                            rbuff[14], rbuff[15], rbuff[16], rbuff[17] };
			printf("New config: %02x%02x%02x%02x%02x%02x%02x%02x %02x%02x%02x%02x\n", conf_buffer[5], conf_buffer[6], conf_buffer[7], conf_buffer[8], conf_buffer[9], conf_buffer[10], conf_buffer[11], conf_buffer[12], conf_buffer[13], conf_buffer[14], conf_buffer[15], conf_buffer[16]);
			wch_isp_command( dev, conf_buffer, 17, (int*)&transferred, rbuff, 1024 );	
		}	
	}
return 0;
}

int ISPConfigureReadProtection( void * d, int one_if_yes_protect )
{
	libusb_device_handle * dev = ((struct ISPProgrammerStruct*)d)->devh;
	struct InternalState * iss = (struct InternalState*)(((struct ProgrammerStructBase*)d)->internal);

	if( iss->target_chip->protocol == PROTOCOL_CH5xx )
	{
		if( iss->target_chip->family_id != CHIP_CH570 )
		{
			fprintf( stderr, "ERROR: There is no true read protection on most of CH5xx chips\nUse \"Enable/Disable debug\" function instead\n" );
			return -1;
		}
	}

	uint8_t rbuff[1024];
	uint32_t transferred = 0;
	
	wch_isp_command( dev, "\xa7\x02\x00\x1f\x00", 5, (int*)&transferred, rbuff, 1024 );
	printf("Current config: %02x%02x%02x%02x%02x%02x%02x%02x %02x%02x%02x%02x\n", rbuff[6], rbuff[7], rbuff[8], rbuff[9], rbuff[10], rbuff[11], rbuff[12], rbuff[13], rbuff[14], rbuff[15], rbuff[16], rbuff[17]);

	if( iss->target_chip_type == CHIP_CH570 )
	{
		if( rbuff[16] == 0x3a && one_if_yes_protect == 0 )
		{
			printf( "Flash is already unprotected!\n" );
			return 0;
		}
		else if ( rbuff[16] != 0x3a && one_if_yes_protect > 0 )
		{
			printf( "Flash is already protected!\n" );
			return 0;
		}
		else
		{
			if( one_if_yes_protect )
			{
				rbuff[16] = 0;
			}
			else
			{
				rbuff[16] = 0x3a;
			}

			ISPErase( d, 0, 0, 1 );

			uint8_t conf_buffer[17] = { 0xa8, 0x0e, 0x00, 0x07, 0x00, 
			                            rbuff[6], rbuff[7], rbuff[8], rbuff[9], rbuff[10], rbuff[11], rbuff[12], rbuff[13],
			                            rbuff[14], rbuff[15], rbuff[16], rbuff[17] };
			printf("New config: %02x%02x%02x%02x%02x%02x%02x%02x %02x%02x%02x%02x\n", conf_buffer[5], conf_buffer[6], conf_buffer[7], conf_buffer[8], conf_buffer[9], conf_buffer[10], conf_buffer[11], conf_buffer[12], conf_buffer[13], conf_buffer[14], conf_buffer[15], conf_buffer[16]);
			wch_isp_command( dev, conf_buffer, 17, (int*)&transferred, rbuff, 1024 );
		}
	}
	else
	{
		if( rbuff[6] == 0xa5 && one_if_yes_protect == 0 )
		{
			printf( "Flash is already unprotected!\n" );
			return 0;
		}
		else if ( rbuff[6] != 0xa5 && one_if_yes_protect > 0 )
		{
			printf( "Flash is already protected!\n" );
			return 0;
		}
		else
		{
			if( one_if_yes_protect )
			{
				rbuff[6] = 0;
				rbuff[7] = 0;
			}
			else
			{
				rbuff[6] = 0xa5;
				rbuff[7] = 0x5a;
				rbuff[14] = 0xff;
				rbuff[15] = 0xff;
				rbuff[16] = 0xff;
				rbuff[17] = 0xff;
			}

			uint8_t conf_buffer[17] = { 0xa8, 0x0e, 0x00, 0x07, 0x00, 
			                            rbuff[6], rbuff[7], rbuff[8], rbuff[9], rbuff[10], rbuff[11], rbuff[12], rbuff[13],
			                            rbuff[14], rbuff[15], rbuff[16], rbuff[17] };
			printf("New config: %02x%02x%02x%02x%02x%02x%02x%02x %02x%02x%02x%02x\n", conf_buffer[5], conf_buffer[6], conf_buffer[7], conf_buffer[8], conf_buffer[9], conf_buffer[10], conf_buffer[11], conf_buffer[12], conf_buffer[13], conf_buffer[14], conf_buffer[15], conf_buffer[16]);
			wch_isp_command( dev, conf_buffer, 17, (int*)&transferred, rbuff, 1024 );
		}
	}
return 0;
}

int ISPCH5xxEnableDebug( void * d, uint8_t disable )
{
	libusb_device_handle * dev = ((struct ISPProgrammerStruct*)d)->devh;
	struct InternalState * iss = (struct InternalState*)(((struct ProgrammerStructBase*)d)->internal);

	if( iss->target_chip->protocol != PROTOCOL_CH5xx )
	{
		fprintf( stderr, "ERROR: This function is only possible on CH5xx chips.\n" );
		return -1;
	}
	if( iss->target_chip->family_id == CHIP_CH570 )
	{
		fprintf( stderr, "ERROR: This function can't be used on CH570/2 chips.\n" );
		return -2;
	}

	uint8_t rbuff[1024];
	uint32_t transferred = 0;

	wch_isp_command( dev, "\xa7\x02\x00\x1f\x00", 5, (int*)&transferred, rbuff, 1024 );
	printf("Current config: %02x%02x%02x%02x%02x%02x%02x%02x %02x%02x%02x%02x\n", rbuff[6], rbuff[7], rbuff[8], rbuff[9], rbuff[10], rbuff[11], rbuff[12], rbuff[13], rbuff[14], rbuff[15], rbuff[16], rbuff[17]);

	if( ((rbuff[14] & 0x10) && (rbuff[14] & 0x80))  && disable == 0 )
	{
		printf( "Debug module is already enabled!\n" );
		return 0;
	}
	else if( (!(rbuff[14] & 0x10) && !(rbuff[14] & 0x80)) && disable > 0 )
	{
		printf( "Debug module is already disabled!\n" );
		return 0;	
	}
	else
	{
		if( disable ) rbuff[14] &= ~(0x90);
		else rbuff[14] |= 0x90;

		// Set valid signature
		rbuff[17] &= ~(0xf0);
		rbuff[17] |= 0x40;
		
		uint8_t conf_buffer[17] = { 0xa8, 0x0e, 0x00, 0x07, 0x00, 
		                            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
		                            rbuff[14], rbuff[15], rbuff[16], rbuff[17] };
		printf("New config: %02x%02x%02x%02x%02x%02x%02x%02x %02x%02x%02x%02x\n", conf_buffer[5], conf_buffer[6], conf_buffer[7], conf_buffer[8], conf_buffer[9], conf_buffer[10], conf_buffer[11], conf_buffer[12], conf_buffer[13], conf_buffer[14], conf_buffer[15], conf_buffer[16]);
		wch_isp_command( dev, conf_buffer, 17, (int*)&transferred, rbuff, 1024 );
	}
	return 0;
}

void * TryInit_WCHISP() {
	libusb_device_handle * wch_isp_devh;
	wch_isp_devh = isp_base_setup(0);
	if( !wch_isp_devh ) return 0;

	struct ISPProgrammerStruct * ret = malloc( sizeof( struct ISPProgrammerStruct ) );
	memset( ret, 0, sizeof( *ret ) );
	ret->devh = wch_isp_devh;
	ret->lasthaltmode = 0;

	MCF.WriteReg32 = ISPWriteReg32;
	MCF.ReadReg32 = ISPReadReg32;
	MCF.WriteWord = ISPWriteWord;

	MCF.SetupInterface = ISPSetupInterface; // no need
	MCF.Erase = ISPErase;
	MCF.WriteBinaryBlob = ISPWriteBinaryBlob;
	MCF.HaltMode = ISPHaltMode;
	MCF.PrintChipInfo = ISPPrintChipInfo;
	MCF.ConfigureNRSTAsGPIO = ISPConfigureNRSTAsGPIO;
	MCF.ConfigureReadProtection = ISPConfigureReadProtection;
	MCF.EnableDebug = ISPCH5xxEnableDebug;

	return ret;
};

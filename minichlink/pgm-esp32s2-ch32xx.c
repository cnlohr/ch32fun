#include <stdint.h>
#include "hidapi.c"
#include "minichlink.h"
#include "ch5xx.h"
#include "chips.h"

#define PROGRAMMER_TYPE_ESP32S2 0
#define PROGRAMMER_TYPE_CH32V003 1

#define DETAILED_DEBUG 0

struct ESP32ProgrammerStruct
{
	void * internal;

	hid_device * hd;
	uint32_t state;
	uint8_t commandbuffer[512];
	uint32_t commandbuffersize; // Maximum possible request size
	int programmer_type;
	int commandplace; // Place in a buffer where next command/data will be written
	uint8_t replybuffer[512];
	int replybuffersize; // Maximum possible reply size
	int replysize; // Reply size for next flush
	int replylen; // Reply length of received report

	int dev_version;
};

int ESPFlushLLCommands( void * dev );

static inline int SRemain( struct ESP32ProgrammerStruct * e )
{
	return e->commandbuffersize - e->commandplace; // We will need room for EOF, but we can use this to detect overflows.
}

static inline void Write4LE( struct ESP32ProgrammerStruct * e, uint32_t val )
{
	if( SRemain( e ) < 4 ) return;
	uint8_t * d = e->commandbuffer + e->commandplace;
	d[0] = val & 0xff;
	d[1] = (val>>8) & 0xff;
	d[2] = (val>>16) & 0xff;
	d[3] = (val>>24) & 0xff;
	e->commandplace += 4;
}

static inline void Write2LE( struct ESP32ProgrammerStruct * e, uint16_t val )
{
	if( SRemain( e ) < 2 ) return;
	uint8_t * d = e->commandbuffer + e->commandplace;
	d[0] = val & 0xff;
	d[1] = (val>>8) & 0xff;
	e->commandplace += 2;
}

static inline void Write1( struct ESP32ProgrammerStruct * e, uint8_t val )
{
	if( SRemain( e ) < 1 ) return;
	uint8_t * d = e->commandbuffer + e->commandplace;
	d[0] = val & 0xff;
	e->commandplace ++;
}


static int ESPWriteReg32( void * dev, uint8_t reg_7_bit, uint32_t value )
{
	struct ESP32ProgrammerStruct * eps = (struct ESP32ProgrammerStruct *)dev;

	if( SRemain( eps ) < 5 ) ESPFlushLLCommands( eps );

	if( eps->dev_version > 4 ) reg_7_bit -= 1;
	Write1( eps, (reg_7_bit<<1) | 1 );
	Write4LE( eps, value );
	return 0;
}

int ESPReadReg32( void * dev, uint8_t reg_7_bit, uint32_t * commandresp )
{
	struct ESP32ProgrammerStruct * eps = (struct ESP32ProgrammerStruct *)dev;

	if( eps->dev_version > 4 ) reg_7_bit -= 1;
	ESPFlushLLCommands( eps );
	Write1( eps, (reg_7_bit<<1) | 0 );

	eps->replysize = 6;
	ESPFlushLLCommands( eps );


	if( eps->replylen < 6 )
	{
		return -9;
	}
	else
	{
		memcpy( commandresp, eps->replybuffer+2, 4 );
		return 0;
	}
}

int ESPReadAllCPURegisters( void * dev, uint32_t * regret )
{
	struct InternalState * iss = (struct InternalState*)(((struct ProgrammerStructBase*)dev)->internal);
	struct ESP32ProgrammerStruct * eps = (struct ESP32ProgrammerStruct *)dev;
	ESPFlushLLCommands( dev );

	Write2LE( eps, 0x05fe ); // Void ESP's internal high level state.
	iss->statetag = STTAG( "RER2" ); // Void local high level state.

	ESPWriteReg32( dev, DMABSTRACTAUTO, 0x00000000 ); // Disable Autoexec.
	MCF.DetermineChipType( dev );
	int i;
	for( i = 0; i < iss->nr_registers_for_debug; i++ )
	{
		ESPWriteReg32( dev, DMCOMMAND, 0x00220000 | 0x1000 | i ); // Read xN into DATA0.
		Write1( eps, (DMDATA0<<1) | 0 );
	}
	ESPWriteReg32( dev, DMCOMMAND, 0x00220000 | 0x7b1 ); // Read xN into DATA0.
	Write1( eps, (DMDATA0<<1) | 0 );
  eps->replysize = iss->nr_registers_for_debug;
	ESPFlushLLCommands( eps );
	if( eps->replylen - 1 != (iss->nr_registers_for_debug+1)*5 )
	{
		fprintf( stderr, "Error: Weird reply trying to read all CPU registers (%d/%d)\n", eps->replylen - 1, (iss->nr_registers_for_debug+1)*5 );
		return -1;
	}
	uint8_t * e = eps->replybuffer + 1;
	for( i = 0; i < iss->nr_registers_for_debug + 1; i++ )
	{
		if( *e )
		{
			fprintf( stderr, "Error reading reg at %d %d\n", i, *e );
			return -2;
		}
		e++;
		memcpy( regret + i, e, 4 );
		e += 4;
	}
	return 0;
}

int DefaultReadBinaryBlob( void * dev, uint32_t address_to_read_from, uint32_t read_size, uint8_t * blob );
int ESPReadBinaryBlob( void * dev, uint32_t address_to_read_from, uint32_t read_size_in, uint8_t * blob )
{
	int read_size = read_size_in;
	struct ESP32ProgrammerStruct * eps = (struct ESP32ProgrammerStruct *)dev;
	uint32_t address_to_read_from_2 = address_to_read_from;
	uint8_t * blob_2 = blob;
	int r = 0;
	
	if( read_size == 0 )
	{
		return 0;
	}

	if( address_to_read_from_2 & 3 )
	{
		// We have to read out the first few bits.
		int nrb2r = 4 - (address_to_read_from_2 & 3);
		if( nrb2r > read_size ) nrb2r = read_size;
		r = DefaultReadBinaryBlob( dev, address_to_read_from_2, nrb2r, blob_2 ); // Correct me if I'm wrong, but before the change id didn't make sense
		if( r )	return r;
		address_to_read_from_2 += nrb2r;
		blob_2 += nrb2r;
		read_size -= nrb2r;
	}

	if( read_size <= 0 )
	{
		return 0;
	}

	int words = read_size / 4;

	ESPFlushLLCommands( dev );
	int w = 0;
	int words_this_group = 0;
	while( w <= words )
	{
		if( w < words )
		{
			Write2LE( eps, 0x09fe );
			Write4LE( eps, address_to_read_from_2 );
			address_to_read_from_2 += 4;
			read_size -= 4;
			words_this_group++;
		}
		if( ( SRemain( eps ) < 8 ) || ( words_this_group * 5 > eps->replybuffersize - 4 ) || w == words )
		{
			eps->replysize = words_this_group * 5;
			ESPFlushLLCommands( dev );
			uint8_t * resp = eps->replybuffer + 1;
			int i;
			for( i = 0; i < words_this_group; i++ )
			{
				if( resp[0] ) return resp[0];
				memcpy( blob_2, resp + 1, 4 );
				resp += 5;
				blob_2 += 4;
			}
			words_this_group = 0;
		}
		w++;
	}

	if( read_size > 0 )
	{
		r = DefaultReadBinaryBlob( dev, address_to_read_from_2, read_size, blob_2 );
		if( r )	return r;
	}

	return 0;
}


int ESPFlushLLCommands( void * dev )
{
	struct ESP32ProgrammerStruct * eps = (struct ESP32ProgrammerStruct *)dev;

	if( eps->commandplace > eps->commandbuffersize )
	{
		fprintf( stderr, "Error: Command buffer overflow: %d\n", eps->commandplace );
		return -5; 
	}

	if( eps->commandplace == 1 ) return 0;

	int r;
	uint8_t descriptor = 0xad;
	int buffer_size = eps->commandbuffersize;

	if( eps->dev_version > 4 )
	{
		if( eps->commandplace <= 7 )
		{
			descriptor = 0xaa;
			buffer_size = 8;
		}
		else if ( eps->commandplace <= 63 )
		{
			descriptor = 0xab;
			buffer_size = 64;
		}
		else if ( eps->commandplace <= 127 )
		{
			descriptor = 0xac;
			buffer_size = 128;
		}
		else if ( eps->commandplace <= 78 )
		{
			descriptor = 0xad;
			buffer_size = 79;
		}
		else
		{
			descriptor = 0xae;
			buffer_size = 264;
		}
	}

	eps->commandbuffer[0] = descriptor; // Key report ID
	memset( eps->commandbuffer + eps->commandplace, 0xff, buffer_size - eps->commandplace );

#if DETAILED_DEBUG
	int i;
	printf( "CHAL: %d", eps->commandplace );
	for( i = 0; i < eps->commandplace+1; i++ )
	{
		if( ( i & 0xff ) == 0 ) printf( "\n" );
		printf( "%02x ", eps->commandbuffer[i] );
	}
	printf("\n" );
#endif

	// fprintf( stderr, "Flushing commands: 0x%02x, %d\n", descriptor, eps->commandplace );
	r = hid_send_feature_report( eps->hd, eps->commandbuffer, buffer_size );
	eps->commandplace = 1;
	if( r < 0 )
	{
		fprintf( stderr, "Error: Got error %d when sending hid feature report.\n", r );
		exit( -9 );
	}
retry:
	// fprintf( stderr, "replysize = %d\n", eps->replysize );
	if( eps->replysize < 7 ) eps->replysize = 7;
	eps->replybuffer[0] = descriptor; // Key report ID
	if( eps->replysize > buffer_size ) buffer_size = eps->replysize + 1;
	// r = hid_get_feature_report( eps->hd, eps->replybuffer, eps->replysize + 1 ); // This is small optimization, but it doesn't gain us much. Will try it with bigger read/write amounts and remove it if it proves insignificant.
	r = hid_get_feature_report( eps->hd, eps->replybuffer, buffer_size );
	#if DETAILED_DEBUG
	printf( "RESP: %d %d", (int)r, (int)eps->replybuffer[0] );
	for( int i = 0; i < eps->replybuffer[0]; i++ )
	{
		if( (i % 16) == 0 ) printf( "\n" );
		printf( "%02x ", eps->replybuffer[i+1] );
	}
	printf( "\n" );
	#endif

	if( eps->replybuffer[0] == 0xff ) goto retry;
	//printf( ">:::%d: %02x %02x %02x %02x %02x %02x\n", eps->replylen, eps->reply[0], eps->reply[1], eps->reply[2], eps->reply[3], eps->reply[4], eps->reply[5] );
	if( r < 0 )
	{
		fprintf( stderr, "Error: Got error %d when getting hid feature report. (Size %d/%d)\n", r, eps->commandbuffersize, eps->replybuffersize );
		return r;
	}
	eps->replysize = 0;
	eps->replylen = eps->replybuffer[0] + 1; // Include the header byte.
	return r;
}

int ESPControl3v3( void * dev, int bOn )
{
	struct ESP32ProgrammerStruct * eps = (struct ESP32ProgrammerStruct *)dev;

	if( SRemain( eps ) < 2 )
		ESPFlushLLCommands( eps );

	if( bOn )
		Write2LE( eps, 0x03fe );
	else
		Write2LE( eps, 0x02fe );
	return 0;
}

int ESPReadWord( void * dev, uint32_t address_to_read, uint32_t * data )
{
	struct ESP32ProgrammerStruct * eps = (struct ESP32ProgrammerStruct *)dev;

	if( SRemain( eps ) < 6 )
		ESPFlushLLCommands( eps );

	Write2LE( eps, 0x09fe );
	Write4LE( eps, address_to_read );
  eps->replysize += 5;
	ESPFlushLLCommands( eps );

	if( eps->replylen < 5 )
	{
		return -9;
	}
	int tail = eps->replylen-5;
	memcpy( data, eps->replybuffer + tail + 1, 4 );
	return (int8_t)eps->replybuffer[tail];
}

int ESPWriteWord( void * dev, uint32_t address_to_write, uint32_t data )
{
	struct ESP32ProgrammerStruct * eps = (struct ESP32ProgrammerStruct *)dev;

	if( SRemain( eps ) < 10 )
		ESPFlushLLCommands( eps );
  
	Write2LE( eps, 0x08fe );
	Write4LE( eps, address_to_write );
	Write4LE( eps, data );
  eps->replysize += 2;
	return 0;
}

int DefaultDelayUS( void * dev, int us );
static int ESPDelayUS( void * dev, int microseconds )
{
	struct ESP32ProgrammerStruct * eps = (struct ESP32ProgrammerStruct *)dev;
	if( SRemain( eps ) < 6 )
		ESPFlushLLCommands( eps );

	Write2LE( eps, 0x04fe );
	Write2LE( eps, microseconds );
	return 0;
}


static int ESPWaitForFlash( void * dev )
{
	struct ESP32ProgrammerStruct * eps = (struct ESP32ProgrammerStruct *)dev;
	if( SRemain( eps ) < 2 )
		ESPFlushLLCommands( eps );
	Write2LE( eps, 0x06fe );
	return 0;
}

static int ESPWaitForDoneOp( void * dev, int ignore )
{
	struct ESP32ProgrammerStruct * eps = (struct ESP32ProgrammerStruct *)dev;
	if( SRemain( eps ) < 2 )
		ESPFlushLLCommands( dev );
	Write2LE( eps, 0x07fe );
	return 0;
}


int ESPExit( void * dev )
{
	struct ESP32ProgrammerStruct * eps = (struct ESP32ProgrammerStruct *)dev;
	hid_close( eps->hd );
	free( eps );
	return 0;
}

int ESPBlockWrite64( void * dev, uint32_t address_to_write, const uint8_t * data )
{
	int writeretry = 0;
	struct ESP32ProgrammerStruct * eps = (struct ESP32ProgrammerStruct *)dev;
	ESPFlushLLCommands( dev );

retry:

	if( eps->dev_version >= 2 && InternalIsMemoryErased( (struct InternalState*)eps->internal, address_to_write ) )
		Write2LE( eps, 0x0efe );
	else
		Write2LE( eps, 0x0bfe );
	Write4LE( eps, address_to_write );

	int i;
	int timeout = 0;
	for( i = 0; i < 64; i++ ) Write1( eps, data[i] );

	InternalMarkMemoryNotErased( (struct InternalState*)eps->internal, address_to_write );

	do
	{
		eps->replysize = 2;
		ESPFlushLLCommands( dev );
		timeout++;
		if( timeout > 1000 )
		{
			fprintf( stderr, "Error: Timed out block-writing 64\n" );
			return -49;
		}
	} while( eps->replylen < 2 );

	if( eps->replybuffer[1] )
	{
		fprintf( stderr, "Error: Got code %d from ESP write algo. %d [%02x %02x %02x]\n", (char)eps->replybuffer[1], eps->replylen, eps->replybuffer[0], eps->replybuffer[1], eps->replybuffer[2] );
		if( writeretry < 10 )
		{
			writeretry++;
			goto retry;
		}
	}

	return (char)eps->replybuffer[1];
}

int ESPVoidHighLevelState( void * dev )
{
	struct ESP32ProgrammerStruct * eps = (struct ESP32ProgrammerStruct *)dev;
	Write2LE( eps, 0x05fe );
	ESPFlushLLCommands( dev );
	DefaultVoidHighLevelState( dev );
	return 0;
}

int ESPVendorCommand( void * dev, const char * cmd )
{
	char command[10] = { 0 };
	char tbuf[10] = { 0 };
	int fields[10];
	char c;
	int i = 0;
	int f = 0;
	while( (c = *cmd++) )
	{
		if( c == ':' ) break;
		if( c == '\0' ) break;
		if( i + 1 >= sizeof( command )) break;
		command[i++] = c;
		command[i] = 0;
	}
	i = 0;
	f = 0;
	while( 1 )
	{
		c = *cmd++;
		if( c == ':' || c == '\0' )
		{
			fields[f++] = SimpleReadNumberInt( tbuf,  0 );
			puts( tbuf );
			if( f == 10 ) break; 
			tbuf[0] = 0;
			i = 0;
			if( c == '\0' ) break;
			continue;
		}
		if( i + 1 >= sizeof( tbuf )) break;
		tbuf[i++] = c;
		tbuf[i] = 0;
	}
	printf( "Got Vendor Command \"%s\"\n", command );
	ESPFlushLLCommands( dev );
	if( strcasecmp( command, "ECLK" ) == 0 )
	{
		printf( "Setting up external clock on pin.\n" );
		if( f < 5 )
		{
			fprintf( stderr, "Error: Need fields :use_apll:sdm0:sdm1:sdm2:odiv try 1:0:0:8:3 for 24MHz\n" );
			fprintf( stderr, "Definition:\n\
	use_apll = Configures APLL = 480 / 4 = 120\n\
	40 * (SDM2 + SDM1/(2^8) + SDM0/(2^16) + 4) / ( 2 * (ODIV+2) );\n\
	Datasheet recommends that numerator is between 300 and 500MHz.\n ");
			return -9;
		}
		Write2LE( dev, 0x0cfe );
		Write1( dev, fields[0] );
		Write1( dev, fields[1] );
		Write1( dev, fields[2] );
		Write1( dev, fields[3] );
		Write1( dev, fields[4] );
		Write1( dev, 0 );
		Write1( dev, 0 );
		Write1( dev, 0 );
		ESPFlushLLCommands( dev );
	}
	else
	{
		fprintf( stderr, "Error: Unknown vendor command %s\n", command );
	}
	return 0;
}

int ESPPollTerminal( void * dev, uint8_t * buffer, int maxlen, uint32_t leaveflagA, int leaveflagB )
{
	struct ESP32ProgrammerStruct * eps = (struct ESP32ProgrammerStruct *)dev;
	ESPFlushLLCommands( dev );
	Write1( dev, 0xfe );
	Write1( dev, 0x0d );
	Write4LE( dev, leaveflagA );
	Write4LE( dev, leaveflagB );
	Write1( dev, 0xff );

  eps->replysize = eps->replybuffersize;
	ESPFlushLLCommands( dev );

	int rlen = eps->replybuffer[0];
	if( rlen < 1 ) return -8;

#if 0
	int i;

	printf( "RESP (ML %d): %d\n", maxlen,eps->replybuffer[0] );

	for( i = 0; i < eps->replybuffer[0]; i++ )
	{
		printf( "%02x ", eps->replybuffer[i+1] );
		if( (i % 16) == 15 ) printf( "\n" );
	}
	printf( "\n" );
#endif

	int errc = eps->replybuffer[1];
	if( errc > 7 ) return -7;

	if( rlen - 1 >= maxlen ) return -6; 

	memcpy( buffer, eps->replybuffer + 2, rlen - 1 );

	return rlen - 1;
}

void esp_ch570_disable_read_protection( void * dev )
{
	struct ESP32ProgrammerStruct * eps = (struct ESP32ProgrammerStruct *)dev;
	ESPFlushLLCommands( eps );
	Write2LE( eps, 0x27fe );
	ESPFlushLLCommands( eps );
	return;
}

int esp_ch5xx_read_eeprom( void * dev, uint32_t addr, uint8_t* buffer, uint8_t len )
{
	struct ESP32ProgrammerStruct * eps = (struct ESP32ProgrammerStruct *)dev;

	if( SRemain( eps ) < 7 )
		ESPFlushLLCommands( eps );

	Write2LE( eps, 0x23fe );
	Write4LE( eps, addr );
	Write1( eps, len );
	eps->replysize = len + 2;
	ESPFlushLLCommands( eps );

	if( eps->replylen < len + 2 )
	{
		return -9;
	}
	int tail = eps->replylen - len + 1;
	memcpy( buffer, eps->replybuffer + tail + 1, len );
	return (int8_t)eps->replybuffer[tail];
}

int esp_ch5xx_read_options_bulk( void * dev, uint32_t addr, uint8_t* buffer, uint8_t len )
{
	struct ESP32ProgrammerStruct * eps = (struct ESP32ProgrammerStruct *)dev;

	if( SRemain( eps ) < 7 )
		ESPFlushLLCommands( eps );

	Write2LE( eps, 0x24fe );
	Write4LE( eps, addr );
	Write1( eps, len );
  eps->replysize = len + 2;
	ESPFlushLLCommands( eps );

	if( eps->replylen < len + 2 )
	{
		return -9;
	}

	int tail = eps->replylen - len + 1;
	memcpy( buffer, eps->replybuffer + tail + 1, len );
	return (int8_t)eps->replybuffer[tail];
}

int ESPCH5xxReadUUID( void * dev, uint8_t* buffer )
{
	struct ESP32ProgrammerStruct * eps = (struct ESP32ProgrammerStruct *)dev;

	if( SRemain( eps ) < 2 )
		ESPFlushLLCommands( eps );

	Write2LE( eps, 0x26fe );
	eps->replysize = 10;
	ESPFlushLLCommands( eps );

	if( eps->replylen < 10 )
	{
		return -9;
	}

	int tail = eps->replylen-9;
	// fprintf( stderr, "len = %d, %02x %02x %02x %02x %02x %02x %02x %02x\n", eps->replylen, eps->reply[tail], eps->reply[tail+1], eps->reply[tail+2], eps->reply[tail+3], eps->reply[tail+4], eps->reply[tail+5], eps->reply[tail+6], eps->reply[tail+7] );
	
	memcpy( buffer, eps->replybuffer + tail + 1, 8 );
	return (int8_t)eps->replybuffer[tail];
}

int esp_ch5xx_write_flash(void * dev, uint32_t start_addr, uint8_t* data, uint32_t len, uint8_t mode)
{
	int writeretry = 0;
	struct ESP32ProgrammerStruct * eps = (struct ESP32ProgrammerStruct *)dev;
	ESPFlushLLCommands( dev );

	uint32_t pos = 0;
	while( pos < len )
	{
retry:

		if( mode == 0 )
			Write2LE( eps, 0x20fe );
		else
			Write2LE( eps, 0x21fe );
		Write4LE( eps, start_addr + pos );
		Write1( eps, 0 ); // Optional byte for specifying write length

		int i;
		int timeout = 0;
		for( i = 0; i < 256; i++ ) Write1( eps, data[i + pos] );
		// fprintf( stderr, "%d\n", eps->commandplace);
		// fprintf( stderr, "%02x %02x %02x %02x %02x %02x %02x %02x\n", eps->commandbuffer[0], eps->commandbuffer[1], eps->commandbuffer[2], eps->commandbuffer[3], eps->commandbuffer[4], eps->commandbuffer[5], eps->commandbuffer[6], eps->commandbuffer[7] );

		do
		{
			eps->replysize = 2;
			ESPFlushLLCommands( dev );
			timeout++;
			if( timeout > 1000 )
			{
				fprintf( stderr, "Error: Timed out writhing flash to ch5xx\n" );
				return -49;
			}
			// fprintf( stderr, "len = %d, %02x %02x %02x %02x %02x %02x %02x %02x\n", eps->replylen, eps->reply[0], eps->reply[1], eps->reply[2], eps->reply[3], eps->reply[4], eps->reply[5], eps->reply[6], eps->reply[7] );
		} while( eps->replylen < 2 );

		if( eps->replybuffer[1] )
		{
			fprintf( stderr, "Error: Got code %d from ESP write algo. %d [%02x %02x %02x]\n", (char)eps->replybuffer[1], eps->replylen, eps->replybuffer[0], eps->replybuffer[1], eps->replybuffer[2] );
			if( writeretry < 10 )
			{
				writeretry++;
				goto retry;
			}
		}
		fprintf( stderr, "." );
		pos += 256;
	}

	return (char)eps->replybuffer[1];
}

void esp_ch5xx_end_write( void * dev )
{
	struct ESP32ProgrammerStruct * eps = (struct ESP32ProgrammerStruct *)dev;
	ESPFlushLLCommands( eps );
	Write2LE( eps, 0x22fe );
	ESPFlushLLCommands( eps );
}

int ESPSetClock( void * dev, uint32_t clock)
{
	struct InternalState * iss = (struct InternalState*)(((struct ProgrammerStructBase*)dev)->internal);
	struct ESP32ProgrammerStruct * eps = (struct ESP32ProgrammerStruct *)dev;
	if( iss->target_chip->protocol == PROTOCOL_CH5xx )
	{
		if( SRemain( eps ) < 2 )
			ESPFlushLLCommands( eps );

		Write2LE( eps, 0x28fe );
		eps->replysize += 2;
		ESPFlushLLCommands( eps );
		return (int8_t)eps->replybuffer[eps->replylen];
	}
	return 0;
}

int ESPCH5xxErase( void * dev, uint32_t addr, uint32_t len, int type )
{
	struct InternalState * iss = (struct InternalState*)(((struct ProgrammerStructBase*)dev)->internal);
	struct ESP32ProgrammerStruct * eps = (struct ESP32ProgrammerStruct *)dev;
	
	enum MemoryArea area = DEFAULT_AREA;
	fprintf( stderr, "Erasing\n" );
	if( type == 1 )
	{
		printf("Whole-chip erase\n");
		addr = iss->target_chip->flash_offset;
		len = iss->target_chip->flash_size;
	}
	else
	{
		if( iss->current_area == 0 ) DetectMemoryArea( dev, addr );
		area = iss->current_area;
	}

	if( SRemain( eps ) < 11 )
			ESPFlushLLCommands( eps );

	Write2LE( eps, 0x25fe );
	Write4LE( eps, addr );
	Write4LE( eps, len );
	Write1( eps, (uint8_t)area );
	eps->replysize += 2;
	ESPFlushLLCommands( eps );

	return (int8_t)eps->replybuffer[1];
}

int ESPCH5xxWriteBinaryBlob(void * dev, uint32_t address_to_write, uint32_t blob_size, const uint8_t * blob) {
	struct InternalState * iss = (struct InternalState*)(((struct ProgrammerStructBase*)dev)->internal);

	int ret = 0;

	if (blob_size == 0) return 0;

	if (!iss->current_area) DetectMemoryArea(dev, address_to_write);
	if (!CheckMemoryLocation(dev, 0, address_to_write, blob_size)) {
		fprintf(stderr, "Data doesn't fit into memory location\n");
		return -1;
	}

	MCF.SetClock(dev, 0);

	uint8_t write_mode = 0;

	uint32_t sector_size = iss->target_chip->sector_size;
	if (iss->current_area == RAM_AREA) sector_size = 4;
	uint8_t* start_pad = malloc(sector_size);
	uint8_t* end_pad = malloc(sector_size);
	
	uint32_t spad = address_to_write - ((address_to_write / sector_size) * sector_size);
	uint32_t epad = (address_to_write + blob_size) - ((address_to_write + blob_size) / sector_size) * sector_size;
	uint32_t new_blob_size = blob_size;

	if (iss->target_chip_type == CHIP_CH570) {
		uint32_t options;
		MCF.ReadWord(dev, 0x40001058, &options);
		if ((options&0x800000) || (options&0x200000)) {
			printf("Flash is write/read protected. You may need to remove protection using 'minichlink -p'\n"); 
		}
	}

	if (iss->current_area == PROGRAM_AREA) {
		// if (blob_size > 4096)
		write_mode = 1; // Always use microblob for program flash for now
		
		if (spad) {
			if (spad + blob_size <= sector_size) {
				MCF.ReadBinaryBlob(dev, (address_to_write - spad), sector_size, start_pad);
				memcpy(start_pad + spad, blob, blob_size);
				epad = 0;
			} else {
				MCF.ReadBinaryBlob(dev, (address_to_write - spad), spad, start_pad);
				memcpy(start_pad + spad, blob, sector_size - spad);
			}
			if (new_blob_size >= sector_size - spad) new_blob_size -= sector_size - spad;
			else new_blob_size = 0;
		}
		if (epad) {
			if (new_blob_size) memcpy(end_pad, blob + (blob_size - epad), epad);
			MCF.ReadBinaryBlob(dev, (address_to_write + blob_size), sector_size - epad, end_pad + epad);
			if (new_blob_size >= epad) new_blob_size -= epad;
			else new_blob_size = 0;
		}

		if (spad) {
			ESPCH5xxErase(dev, ((address_to_write + spad) - sector_size), new_blob_size + spad + epad, 0);
			esp_ch5xx_write_flash(dev, address_to_write - spad, start_pad, sector_size, write_mode);
			if (spad + blob_size > sector_size)
				esp_ch5xx_write_flash(dev, address_to_write + (sector_size - spad), (uint8_t*)(blob + (sector_size - spad)), new_blob_size, write_mode);
		} else {
			ESPCH5xxErase(dev, address_to_write, new_blob_size + epad, 0);
			esp_ch5xx_write_flash(dev, address_to_write, (uint8_t*)(blob), new_blob_size, write_mode);
		}
		if (epad) esp_ch5xx_write_flash(dev, (address_to_write + blob_size) - epad, end_pad, sector_size, write_mode);
		
		// ch5xx_verify_data(dev, address_to_write, blob, blob_size);
	} else if (iss->current_area == BOOTLOADER_AREA) {
		fprintf(stderr, "Can't write to Bootloader area on these chips (yet?)\n");
		ret = -2;
		goto end;
	} else if (iss->current_area == OPTIONS_AREA) {
		
		if (spad) {
			if (spad + blob_size <= sector_size) {
				esp_ch5xx_read_options_bulk(dev, (address_to_write - spad), start_pad, sector_size);
				memcpy(start_pad + spad, blob, blob_size);
				epad = 0;
			} else {
				esp_ch5xx_read_options_bulk(dev, (address_to_write - spad), start_pad, spad);
				memcpy(start_pad + spad, blob, sector_size - spad);
			}
			if (new_blob_size >= sector_size - spad) new_blob_size -= sector_size - spad;
			else new_blob_size = 0;
		}
		if (epad) {
			if (new_blob_size) memcpy(end_pad, blob + (blob_size - epad), epad);
			esp_ch5xx_read_options_bulk(dev, (address_to_write + blob_size), end_pad + epad, sector_size - epad);
			if (new_blob_size >= epad) new_blob_size -= epad;
			else new_blob_size = 0;
		}

		if (spad) {
			ESPCH5xxErase(dev, ((address_to_write + spad) - sector_size), new_blob_size + spad + epad, 0);
			esp_ch5xx_write_flash(dev, address_to_write - spad, start_pad, sector_size, write_mode);
			if (spad + blob_size > sector_size)
				esp_ch5xx_write_flash(dev, address_to_write + (sector_size - spad), (uint8_t*)(blob + (sector_size - spad)), new_blob_size, write_mode);
		} else {
			ESPCH5xxErase(dev, address_to_write, new_blob_size + epad, 0);
			esp_ch5xx_write_flash(dev, address_to_write, (uint8_t*)(blob), new_blob_size, write_mode);
		}
		if (epad) esp_ch5xx_write_flash(dev, (address_to_write + blob_size) - epad, end_pad, sector_size, write_mode);
		
	} else if (iss->current_area == EEPROM_AREA) {

		if (spad) {
			if (spad + blob_size <= sector_size) {
				esp_ch5xx_read_eeprom(dev, (address_to_write - spad), start_pad, sector_size);
				memcpy(start_pad + spad, blob, blob_size);
				epad = 0;
			} else {
				esp_ch5xx_read_eeprom(dev, (address_to_write - spad), start_pad, spad);
				memcpy(start_pad + spad, blob, sector_size - spad);
			}
			if (new_blob_size >= sector_size - spad) new_blob_size -= sector_size - spad;
			else new_blob_size = 0;
		}
		if (epad) {
			if (new_blob_size) memcpy(end_pad, blob + (blob_size - epad), epad);
			esp_ch5xx_read_eeprom(dev, (address_to_write + blob_size), end_pad + epad, sector_size - epad);
			if (new_blob_size >= epad) new_blob_size -= epad;
			else new_blob_size = 0;
		}

		if (spad) {
			ESPCH5xxErase(dev, ((address_to_write + spad) - sector_size), new_blob_size + spad + epad, 0);
			esp_ch5xx_write_flash(dev, address_to_write - spad, start_pad, sector_size, write_mode);
			if (spad + blob_size > sector_size)
				esp_ch5xx_write_flash(dev, address_to_write + (sector_size - spad), (uint8_t*)(blob + (sector_size - spad)), new_blob_size, write_mode);
		} else {
			ESPCH5xxErase(dev, address_to_write, new_blob_size + epad, 0);
			esp_ch5xx_write_flash(dev, address_to_write, (uint8_t*)(blob), new_blob_size, write_mode);
		}
		if (epad) esp_ch5xx_write_flash(dev, (address_to_write + blob_size) - epad, end_pad, sector_size, write_mode);

	} else if (iss->current_area == RAM_AREA) {
		if (spad) {
			for (int i = 0; i < spad; i++) {
				ret = MCF.WriteByte(dev, address_to_write+i, *((uint8_t*)(blob+i)));
				if (ret) {
					fprintf(stderr, "Error on WriteByte while writing to RAM.\n");
					goto end;
				}
			}
		} 
		for (int i = 0; i < blob_size-spad-epad; i += 4) {
			ret = MCF.WriteWord(dev, address_to_write+spad+i, *((uint32_t*)(blob+spad+i)));
			if (ret) {
				fprintf(stderr, "Error on WriteWord while writing to RAM.\n");
				goto end;
			}
		}
		if (epad) {
			uint32_t epad_address = address_to_write + blob_size - epad;
			for (int i = 0; i < spad; i++) {
				ret = MCF.WriteByte(dev, epad_address + i, *((uint8_t*)(blob + epad_address + i)));
				if (ret) {
					fprintf(stderr, "Error on WriteByte while writing to RAM.\n");
					goto end;
				}
			}
		}
	} else {
		fprintf(stderr, "Unknown memory region. Not writing.\n");
		ret = -2;
		goto end;
	}

	fprintf(stderr, "\nDone writing\n");
end:
	free(start_pad);
	free(end_pad);

	esp_ch5xx_end_write( dev );

	return ret;
}

int ESPCH5xxReadBinaryBlob( void * dev, uint32_t address_to_read_from, uint32_t read_size, uint8_t* blob )
{
	struct InternalState * iss = (struct InternalState*)(((struct ProgrammerStructBase*)dev)->internal);
	int ret = 0;
	
	if (!iss->current_area) DetectMemoryArea(dev, address_to_read_from);
	if (!CheckMemoryLocation(dev, 0, address_to_read_from, read_size)) {
		fprintf(stderr, "Requested location should be within one memory area. Aborting\n");
		ret = -1;
		goto end;
	}
	if (iss->current_area == OPTIONS_AREA) {
		esp_ch5xx_read_options_bulk(dev, address_to_read_from, blob, read_size);
	} else if (iss->current_area == EEPROM_AREA) {
		esp_ch5xx_read_eeprom(dev, address_to_read_from, blob, read_size);
	} else if (iss->current_area == BOOTLOADER_AREA) {
		esp_ch5xx_read_eeprom(dev, address_to_read_from, blob, read_size);
	} else if (iss->current_area == PROGRAM_AREA || iss->current_area == RAM_AREA) {
			ret = ESPReadBinaryBlob( dev, address_to_read_from, read_size, blob );
	} else {
		fprintf(stderr, "Unknown memory region. Not reading.\n");
		ret = -2;
		goto end;
	}

	fprintf(stderr, "Done reading\n");
end:
	return ret;
}

int ESPDetermineChipTypeOLD( void * dev )
{
	struct InternalState * iss = (struct InternalState*)(((struct ProgrammerStructBase*)dev)->internal);
	if( iss->target_chip == NULL )
	{
		uint32_t rr;
		if( MCF.ReadReg32( dev, DMHARTINFO, &rr ) )
		{
			fprintf( stderr, "Error: Could not get hart info.\n" );
			return -1;
		}

		uint32_t data0offset = 0xe0000000 | ( rr & 0x7ff );

		MCF.WriteReg32( dev, DMCONTROL, 0x80000001 ); // Make the debug module work properly.
		MCF.WriteReg32( dev, DMCONTROL, 0x80000001 ); // Initiate halt request.

		// Tricky, this function needs to clean everything up because it may be used entering debugger.
		uint32_t old_data0;
		MCF.ReadReg32( dev, DMDATA0, &old_data0 );
		MCF.WriteReg32( dev, DMCOMMAND, 0x00221008 ); // Copy data from x8.
		uint32_t old_x8;
		MCF.ReadReg32( dev, DMDATA0, &old_x8 );

		uint32_t vendorid = 0;
		uint32_t marchid = 0;

		MCF.WriteReg32( dev, DMABSTRACTCS, 0x08000700 ); // Clear out any dmabstractcs errors.

		MCF.WriteReg32( dev, DMABSTRACTAUTO, 0x00000000 );
		MCF.WriteReg32( dev, DMCOMMAND, 0x00220000 | 0xf12 );
		MCF.WriteReg32( dev, DMCOMMAND, 0x00220000 | 0xf12 ); // Need to double-read, not sure why.
		MCF.ReadReg32( dev, DMDATA0, &marchid );

		MCF.WriteReg32( dev, DMPROGBUF0, 0x90024000 ); // c.ebreak <<== c.lw x8, 0(x8)
		MCF.WriteReg32( dev, DMDATA0, 0x1ffff704 ); // Special chip ID location.
		MCF.WriteReg32( dev, DMCOMMAND, 0x00271008 ); // Copy data to x8, and execute.
		MCF.WaitForDoneOp( dev, 0 );

		MCF.WriteReg32( dev, DMCOMMAND, 0x00221008 ); // Copy data from x8.
		MCF.ReadReg32( dev, DMDATA0, &vendorid );

		uint32_t chip_type = (vendorid & 0xfff00000)>>20;
		uint32_t flash_size_address = 0x1ffff7e0;
		uint8_t read_protection = 0;
		if( data0offset == 0xe00000f4 )
		{
			// Only known processor with this signature = 0 is qingke-v2.
			switch( chip_type )
			{
			case 0x002: iss->target_chip = &ch32v002; break;
			case 0x004: iss->target_chip = &ch32v004; break;
			case 0x005: iss->target_chip = &ch32v005; break;
			case 0x006: iss->target_chip = &ch32v006; break;
			case 0x641: iss->target_chip = &ch641; break;
			default:    iss->target_chip = &ch32v003; break; // not usually 003
			}
			// Examples:
			// 00000012 = CHIP_CH32V003
			// 00620620 = CHIP_CH32V006
		}
		else if( data0offset == 0xe0000380 )
		{
			// All other known chips.
			switch( chip_type )
			{
				case 0x103:
					// V103 Special chip ID location: 0x1ffff884
					// CH32V103C8T6-0x25004102
					// CH32V103R8T6-0x2500410F
					// L103 Special chip ID location: 0x1ffff704
					// CH32L103C8T6-0x103107x0
					// CH32L103F8P6-0x103A07x0
					// CH32L103G8R6-0x103B07x0
					// CH32L103K8U6-0x103207x0
					// CH32L103F8U6-0x103D07x0
					// can be find in DBGMCU_GetCHIPID in \EVT\EXAM\SRC\Peripheral\src\chxxx_dbgmcu.c
					iss->target_chip = &ch32v103;
					break;
				case 0x035: case 0x033:
					iss->target_chip = &ch32x035;
					break;
				case 0x203:
					iss->target_chip = &ch32v203;
					break;
				case 0x208:
					iss->target_chip = &ch32v208;
					break;
				case 0x303:
					iss->target_chip = &ch32v303;
					break;
				case 0x305:
					iss->target_chip = &ch32v305;
					break;
				case 0x307:
					iss->target_chip = &ch32v307;
					break;
				case 0x643:
					iss->target_chip = &ch643;
					break;
			}
		}

		if( iss->target_chip == NULL )
		{
			fprintf( stderr, "Unknown chip type.  Report as bug with picture of chip.\n" );
			fprintf( stderr, "Vendorid: %08x\n", vendorid );
			fprintf( stderr, "marchid : %08x\n", marchid );
			fprintf( stderr, "HARTINFO: %08x\n", rr );
			return -2;
		}
		else
		{
			if( read_protection == 0 )
			{
				uint32_t one;
				int two;
				MCF.WriteReg32( dev, DMDATA0, 0x4002201c );
				MCF.WriteReg32( dev, DMCOMMAND, 0x00271008 ); // Copy data to x8, and execute.
				MCF.WaitForDoneOp( dev, 1 );
				MCF.WriteReg32( dev, DMCOMMAND, 0x00221008 ); // Copy data from x8.
				MCF.ReadReg32( dev, DMDATA0, &one );
				MCF.WriteReg32( dev, DMDATA0, 0x40022020 );
				MCF.WriteReg32( dev, DMCOMMAND, 0x00271008 ); // Copy data to x8, and execute.
				MCF.WaitForDoneOp( dev, 1 );
				MCF.WriteReg32( dev, DMCOMMAND, 0x00221008 ); // Copy data from x8.
				MCF.ReadReg32( dev, DMDATA0, (uint32_t*)&two );
				
				if( (one & 2) || two != -1 ) read_protection = 1;
			}

			uint32_t rr = 0;
			if( iss-> target_chip && flash_size_address )
			{
				MCF.WriteReg32( dev, DMDATA0, flash_size_address );
				MCF.WriteReg32( dev, DMCOMMAND, 0x00271008 ); // Copy data to x8, and execute.
				MCF.WaitForDoneOp( dev, 1 );
				MCF.WriteReg32( dev, DMCOMMAND, 0x00221008 ); // Copy data from x8.
				MCF.ReadReg32( dev, DMDATA0, &rr );

				iss->flash_size = rr & 0xFFFF;
			}

			iss->target_chip_type = iss->target_chip->family_id;
			if( iss->flash_size == 0 ) iss->flash_size = iss->target_chip->flash_size/1024;
			iss->ram_base = iss->target_chip->ram_base;
			iss->ram_size = iss->target_chip->ram_size;
			iss->sector_size = iss->target_chip->sector_size;
			iss->nr_registers_for_debug = 32; // Maybe add a core type variable to RiscVChip_s?
			if( iss->target_chip_type == CHIP_CH32V00x ||
				iss->target_chip_type == CHIP_CH32V003 ||
				iss->target_chip_type == CHIP_CH641 )
			{
				iss->nr_registers_for_debug = 16;
			}
			
			uint8_t uuid[8];
			if( MCF.GetUUID( dev, uuid ) ) fprintf( stderr, "Couldn't read UUID\n" );
			fprintf( stderr, "Detected %s\n", iss->target_chip->name_str );
			fprintf( stderr, "Flash Storage: %d kB\n", iss->flash_size );
			fprintf( stderr, "Part UUID: %02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x\n", uuid[0], uuid[1], uuid[2], uuid[3], uuid[4], uuid[5], uuid[6], uuid[7] );
			fprintf( stderr, "Read protection: %s\n", (read_protection > 0)?"enabled":"disabled" );
		}

		// Cleanup
		MCF.WriteReg32( dev, DMDATA0, old_x8 );
		MCF.WriteReg32( dev, DMCOMMAND, 0x00231008 ); // Copy data to x8
		MCF.WriteReg32( dev, DMDATA0, old_data0 );
		iss->statetag = STTAG( "XXXX" );
	}
	return 0;
}

int ESPDetermineChipType( void * dev )
{
	struct ESP32ProgrammerStruct * eps = (struct ESP32ProgrammerStruct *)dev;
	struct InternalState * iss = (struct InternalState*)(((struct ProgrammerStructBase*)dev)->internal);
	if( iss->target_chip == NULL )
	{
		if( SRemain( dev ) < 6 )
			ESPFlushLLCommands( dev );

		Write2LE( dev, 0x10fe );
		ESPFlushLLCommands( dev );

		if( eps->replylen < 7 )
		{
			return -9;
		}

		uint16_t reply = *((uint16_t*)&eps->replybuffer[6]);
		uint32_t sevenf_id = *((uint32_t*)&eps->replybuffer[2]);
		uint32_t read_protection = reply & 0x8000;
		uint32_t chip_id_address = 0x1ffff7c4;
		uint32_t flash_size_address = 0x1ffff7e0;

		switch ( reply & 0x7FFF )
		{
			case 0x564:
				iss->target_chip = &ch564;
				iss->target_chip_id = sevenf_id;
				flash_size_address = 0;
				break;
			case 0x565:
				iss->target_chip = &ch565;
				iss->target_chip_id = 0x65 << 24;
				flash_size_address = 0;
				break;
			case 0x569:
				iss->target_chip = &ch569;
				iss->target_chip_id = 0x69 << 24;
				flash_size_address = 0;
				break;
			case 0x570:
				iss->target_chip = &ch570;
				iss->target_chip_id = 0x70 << 24;
				flash_size_address = 0;
				break;
			case 0x571:
				iss->target_chip = &ch571;
				iss->target_chip_id = 0x71 << 24;
				flash_size_address = 0;
				break;
			case 0x572:
				iss->target_chip = &ch572;
				iss->target_chip_id = 0x72 << 24;
				flash_size_address = 0;
				break;
			case 0x573:
				{
					uint32_t ch573_variant = 0;
					esp_ch5xx_read_options_bulk( dev, 0x7f00c, (uint8_t*)&ch573_variant, 4 );
					if( (int)(ch573_variant << 18) < 0 )
					{
						iss->target_chip = &ch573q;
						iss->target_chip_id = 0x73550000;
					}
					else
					{
						iss->target_chip = &ch573;
						iss->target_chip_id = 0x73 << 24;
					}
					flash_size_address = 0;
				}
				break;
			case 0x581:
				iss->target_chip = &ch581;
				iss->target_chip_id = 0x81 << 24;
				flash_size_address = 0;
				break;
			case 0x582:
				iss->target_chip = &ch582;
				iss->target_chip_id = 0x82 << 24;
				flash_size_address = 0;
				break;
			case 0x583:
				iss->target_chip = &ch583;
				iss->target_chip_id = 0x83 << 24;
				flash_size_address = 0;
				break;
			case 0x585:
				iss->target_chip = &ch585;
				iss->target_chip_id = 0x93 << 24;
				flash_size_address = 0;
				break;
			case 0x592:
				iss->target_chip = &ch592;
				iss->target_chip_id = 0x92 << 24;
				flash_size_address = 0;
				break;
			case 0x641:
				iss->target_chip = &ch641;
				break;
			case 0x643:
				iss->target_chip = &ch643;
				break;
			case 0x645:
				iss->target_chip = &ch645;
				break;
			case ((2 << 12) | 0x30):
				iss->target_chip = &ch32m030;
				iss->target_chip_id = sevenf_id;
				flash_size_address = 0x1ffff3a0;
				break;
			case ((4 << 12) | 0x35):
				iss->target_chip = &ch32x035;
				chip_id_address = sevenf_id;
				break;
			case ((1 << 12) | 0x103):
				iss->target_chip = &ch32l103;
				chip_id_address = sevenf_id;
				break;
			case ((3 << 12) | 0x2):
				iss->target_chip = &ch32v002;
				chip_id_address = 0x1ffff704;
				break;
			case ((3 << 12) | 0x3):
				iss->target_chip = &ch32v003;
				break;
			case ((3 << 12) | 0x4):
				iss->target_chip = &ch32v004;
				chip_id_address = 0x1ffff704;
				break;
			case ((3 << 12) | 0x6):
				iss->target_chip = &ch32v006;
				chip_id_address = 0x1ffff704;
				break;
			case ((3 << 12) | 0x7):
				iss->target_chip = &ch32v007;
				chip_id_address = 0x1ffff704;
				break;
			case ((3 << 12) | 0x103):
				iss->target_chip = &ch32v103;
				chip_id_address = 0x1ffff884;
				break;
			case ((3 << 12) | 0x203):
				iss->target_chip = &ch32v203;
				chip_id_address = 0x1ffff704;
				break;
			case ((3 << 12) | 0x208):
				iss->target_chip = &ch32v208;
				chip_id_address = 0x1ffff704;
				break;
			case ((3 << 12) | 0x303):
				iss->target_chip = &ch32v303;
				chip_id_address = 0x1ffff704;
				break;
			case ((3 << 12) | 0x305):
				iss->target_chip = &ch32v305;
				chip_id_address = 0x1ffff704;
				break;
			case ((3 << 12) | 0x307):
				iss->target_chip = &ch32v307;
				chip_id_address = 0x1ffff704;
				break;
			case ((3 << 12) | 0x317):
				iss->target_chip = &ch32v317;
				chip_id_address = 0x1ffff704;
				break;
			default:
				iss->target_chip = NULL;
				break;
		}
		if( iss->target_chip == NULL)
		{
			uint32_t hartinfo = 0;
			ESPReadReg32( dev, DMHARTINFO, &hartinfo );
			fprintf( stderr, "Unknown chip type.  Report as bug with picture of chip.\n" );
			fprintf( stderr, "marchid : %08x\n", reply );
			fprintf( stderr, "HARTINFO: %08x\n", hartinfo );
			return -2;
		}
		else
		{
			if( iss->target_chip->protocol == PROTOCOL_UNSUPPORTED )
			{
				fprintf( stderr, "Detected %s chip, but it's not supported yet. Aborting\n", iss->target_chip->name_str );
				return -3;
			}

			uint32_t rr = 0;
			if( !iss->target_chip_id )
			{
				ESPReadWord( dev, chip_id_address, &rr);
				iss->target_chip_id = rr;
			}
		
			if( flash_size_address )
			{
				ESPReadWord( dev, flash_size_address, &rr);
				iss->flash_size = rr & 0xFFFF;
			}
			
			iss->target_chip_type = iss->target_chip->family_id;
			if( iss->flash_size == 0 ) iss->flash_size = iss->target_chip->flash_size/1024;
			iss->ram_base = iss->target_chip->ram_base;
			iss->ram_size = iss->target_chip->ram_size;
			iss->sector_size = iss->target_chip->sector_size;
			iss->nr_registers_for_debug = 32; // Maybe add a core type variable to RiscVChip_s?
			if( iss->target_chip_type == CHIP_CH32V00x || 
				iss->target_chip_type == CHIP_CH32V003 ||  
				iss->target_chip_type == CHIP_CH641 )
			{
				iss->nr_registers_for_debug = 16;
			}
			if( iss->target_chip->protocol == PROTOCOL_CH5xx )
			{
				MCF.WriteBinaryBlob = ESPCH5xxWriteBinaryBlob;
				MCF.Erase = ESPCH5xxErase;
				MCF.ReadBinaryBlob = ESPCH5xxReadBinaryBlob;
				MCF.GetUUID = ESPCH5xxReadUUID;
        MCF.ConfigureNRSTAsGPIO = CH5xxConfigureNRSTAsGPIO;
			}

			uint8_t * part_type = (uint8_t*)&iss->target_chip_id;
			uint8_t uuid[8];
			if( MCF.GetUUID( dev, uuid ) ) fprintf( stderr, "Couldn't read UUID\n" );
			fprintf( stderr, "Detected %s\n", iss->target_chip->name_str );
			fprintf( stderr, "Flash Storage: %d kB\n", iss->flash_size );
			fprintf( stderr, "Part UUID: %02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x\n", uuid[0], uuid[1], uuid[2], uuid[3], uuid[4], uuid[5], uuid[6], uuid[7] );
			fprintf( stderr, "Part Type: %02x-%02x-%02x-%02x\n", part_type[3], part_type[2], part_type[1], part_type[0] );
			fprintf( stderr, "Read protection: %s\n", (read_protection > 0)?"enabled":"disabled" );
		}
	}
	return 0;
}

static int ESPResetInterface( void * dev )
{
	Write2LE( dev, 0x0efe ); // Trigger Init.
	ESPFlushLLCommands( dev );
	Write2LE( dev, 0x0afe ); // Reset programmer internals
	ESPFlushLLCommands( dev );
	return 0;
}

void * TryInit_ESP32S2CHFUN()
{
	hid_init();

	struct ESP32ProgrammerStruct * eps = malloc( sizeof( struct ESP32ProgrammerStruct ) );
	memset( eps, 0, sizeof( *eps ) );
	hid_device * hd = hid_open( 0x303a, 0x4004, L"s2-ch32xx-pgm-v0"); // third parameter is "serial"
	if( hd )
	{
		eps->commandbuffersize = 255;
		eps->replybuffersize = 255;
		eps->programmer_type = PROGRAMMER_TYPE_ESP32S2;
	}
	else if( !!( hd = hid_open( 0x1206, 0x5D10, L"RVSWDIO003-01") ) )
	{
		eps->commandbuffersize = 79;
		eps->replybuffersize = 79;
		eps->programmer_type = PROGRAMMER_TYPE_CH32V003;
	}
	else
	{
		free( eps );
		return 0;
	}

	eps->hd = hd;
	eps->commandplace = 1;
	eps->dev_version = 0;

	memset( &MCF, 0, sizeof( MCF ) );
	MCF.WriteReg32 = ESPWriteReg32;
	MCF.ReadReg32 = ESPReadReg32;
	MCF.FlushLLCommands = ESPFlushLLCommands;
	MCF.DelayUS = ESPDelayUS;
	MCF.Control3v3 = ESPControl3v3;
	MCF.Exit = ESPExit;
	MCF.VoidHighLevelState = ESPVoidHighLevelState;
	MCF.VendorCommand = ESPVendorCommand;
	MCF.ResetInterface = ESPResetInterface;

#if 1
	// These are optional. Disabling these is a good mechanismto make sure the core functions still work.
	// Comment these out to test the reference algorithm.
	// DO NOT Comment them out piecemeal because there are state assumptions built into these functions.
	MCF.PollTerminal = ESPPollTerminal;
	MCF.WriteWord = ESPWriteWord;
	MCF.ReadWord = ESPReadWord;
	MCF.WaitForFlash = ESPWaitForFlash;
	MCF.WaitForDoneOp = ESPWaitForDoneOp;
	MCF.BlockWrite64 = ESPBlockWrite64;
	MCF.ReadBinaryBlob = ESPReadBinaryBlob;
	MCF.ReadAllCPURegisters = ESPReadAllCPURegisters;
#endif

	ESPFlushLLCommands( eps );

	// Force programmer mode.
	Write1( eps, 0xfe );
	Write1( eps, 0xfd );
	Write1( eps, 0x00 );
  eps->replysize = 2;
	ESPFlushLLCommands( eps );
	if( eps->replylen > 1 )
	{
		eps->dev_version = eps->replybuffer[1];
		printf( "Dev Version: %d\n", eps->dev_version );
	}
	if( eps->dev_version > 4 )
	{
		eps->commandbuffersize = 264;
		eps->replybuffersize = 264;
		MCF.DetermineChipType = ESPDetermineChipType;
		MCF.SetClock = ESPSetClock;
	}
	else
	{
		MCF.DetermineChipType = ESPDetermineChipTypeOLD;
	}
	Write2LE( eps, 0x0efe ); // Trigger Init.
	ESPFlushLLCommands( eps );
	Write2LE( eps, 0x0afe ); 	// Reset programmer internals
	ESPFlushLLCommands( eps );

	return eps;
}

/* 
 * Micro GDBStub Driver, for implementing a gdbserver.
 * Copyright (C) Charles Lohr 2023
 *  You may freely license this file under the MIT-x11, or the 2- or 3- or New BSD Licenses.
 *  You may also use this as though it is public domain.
 *
 * This project is based off of picorvd. https://github.com/aappleby/PicoRVD/
 *
 * Simply:
 *   1: define the RV_ Functions
 *   2: Call the MicroGDB* functions needed.
 *   3: Define MICROGDBSTUB_IMPLEMENTATION at least in one place this is included in your program.
 *   4: If you want to let this manage the server as a network device, simply #define MICROGDBSTUB_SOCKETS
 *
 * To connect to your GDBStub running, you can:
 *  gdb-multiarch -ex 'target remote :3333' ./blink.elf 
 *
 */

#ifndef _MICROGDBSTUB_H
#define _MICROGDBSTUB_H

enum HaltResetResumeType
{
	HALT_TYPE_SINGLE_STEP = 9,
	HALT_TYPE_CONTINUE = 2,
	HALT_TYPE_CONTINUE_WITH_SIGNAL = 4,
};

// You must write these for your processor.
void RVNetPoll(void * dev );
int RVSendGDBHaltReason( void * dev );
void RVNetConnect( void * dev );
int RVGetNumRegisters( void * dev );
int RVReadCPURegister( void * dev, int regno, uint32_t * regret );
int RVWriteCPURegister( void * dev, int regno, uint32_t value );
int RVDebugExec( void * dev, enum HaltResetResumeType halt_reset_or_resume, int resume_from_other_address, uint32_t address );
int RVReadMem( void * dev, uint32_t memaddy, uint8_t * payload, int len );
int RVHandleBreakpoint( void * dev, int set, uint32_t address );
int RVWriteRAM(void * dev, uint32_t memaddy, uint32_t length, uint8_t * payload );
void RVCommandResetPart( void * dev, int mode );
void RVHandleDisconnect( void * dev );
void RVHandleGDBBreakRequest( void * dev );
void RVHandleUnsolicitedGDBBreakRequest( void * dev );
void RVHandleKillRequest( void * dev );
int RVErase( void * dev, uint32_t memaddy, uint32_t length );
int RVWriteFlash( void * dev, uint32_t memaddy, uint32_t length, uint8_t * payload );

#ifdef MICROGDBSTUB_SOCKETS
int MicroGDBPollServer( void * dev );
int MicroGDBStubStartup( void * dev );
void MicroGDBExitServer( void * dev );
#endif

// If you are not a network socket, you can pass in this data.
void MicroGDBStubSendReply( const void * data, int len, int docs );
void MicroGDBStubHandleClientData( void * dev, const uint8_t * rxdata, int len );


#ifdef MICROGDBSTUB_IMPLEMENTATION

///////////////////////////////////////////////////////////////////////////////
// Protocol Stuff

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef MICROGDBSTUB_SOCKETS
#if defined( WIN32 ) || defined( _WIN32 )
#include <winsock2.h>
#if !defined( POLLIN ) 
typedef struct pollfd { SOCKET fd; SHORT  events; SHORT  revents; };
#define POLLIN 0x0001
#define POLLERR 0x008
#define POLLHUP 0x010
int WSAAPI WSAPoll(struct pollfd * fdArray, ULONG	   fds, INT		 timeout );
#endif
#define poll WSAPoll
#define socklen_t uint32_t
#define SHUT_RDWR SD_BOTH
#define MSG_NOSIGNAL 0
#else
#define closesocket close
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <poll.h>
#endif
#endif

#ifdef __linux__
#include <linux/in.h>
#elif defined(__APPLE__) || defined(__FreeBSD__)
#include <netinet/in.h>
#endif

char gdbbuffer[65536];
uint8_t gdbchecksum = 0;
int gdbbufferplace = 0;
int gdbbufferstate = 0;
int gdbrunningcsum = 0;
int gdbqnoackmode = 0;

static inline char ToHEXNibble( int i )
{
	i &= 0xf;
	return ( i < 10 )?('0' + i):('a' - 10 + i);
}

static int fromhex( char c )
{
	if( c >= '0' && c <= '9' ) c = c - '0';
	else if( c >= 'A' && c <= 'F' ) c = c - 'A' + 10;
	else if( c >= 'a' && c <= 'f' ) c = c - 'a' + 10;
	else return -1;
	return c;
}

// output must have length of len / 2.
static int DecodeHexToBytes(const char* hexstr, size_t string_len, void* output, size_t out_len) {
	// 2 hex chars make up one byte. out buffer needs to have >= slen/2 bytes.
	// further, we only want to decode even-length strings
	if (out_len < (string_len / 2) || (string_len % 2) != 0)
		return -1;
	uint8_t* out = (uint8_t*) output;
	for(size_t i = 0; i < string_len; i += 2) {
		int nibble1, nibble2;
		if((nibble1 = fromhex(hexstr[i])) < 0)
			return nibble1; // error
		if((nibble2 = fromhex(hexstr[i + 1])) < 0)
			return nibble2; // error
		out[i / 2] = ((uint8_t)nibble1 << 4u) | ((uint8_t)nibble2);
	}
	return string_len / 2; // number of output bytes written
}

// if (numhex < 0) 
static int ReadHex( char ** instr, int numhex, uint32_t * outwrite )
{
	if( !instr ) return -1;
	char * str = *instr;
	// If negative - error.
	// If positive - number of bytes read.

	*outwrite = 0;

	int scanhex = numhex;
	if( scanhex < 0 )
		scanhex = strlen( str );

	int i;
	for( i = 0; i < scanhex; i++ )
	{
		int v = fromhex( *(str++) );
		if( v < 0 )
		{
			if( numhex < 0 )
			{
				str--;
				*instr = str;
				return i;
			}
			else
			{
				*instr = str;
				return - i - 1;
			}
		}
		(*outwrite) = ((*outwrite) << 4) | v;
	}
	*instr = str;
	return i;
}

static int StringMatch( const char * haystack, const char * mat )
{
	int i;
	for( i = 0; mat[i] && haystack[i]; i++ )
		if( mat[i] != haystack[i] || haystack[i] == 0 ) break;
	return mat[i] == 0;
}

void SendReplyFull( const char * replyMessage )
{
	MicroGDBStubSendReply( replyMessage, -1, '$' );
}

void MakeGDBPrintText(const char* msg) {
	// ASCII to hex conversion doubles size, plus 'O', plus NUL
	size_t buf_len = 2 * strlen(msg) + 2;
	char* buf = alloca(buf_len);
	memset(buf, 0, buf_len);
	buf[0] = 'O'; // for "Output"
	char* target = buf + 1; 
	for(size_t i = 0; i < strlen(msg); i++) {
		target[2*i] = ToHEXNibble((msg[i] & 0xf0u) >> 4u);
		target[2*i + 1] = ToHEXNibble(msg[i] & 0x0fu);
	}
	SendReplyFull(buf);
}
///////////////////////////////////////////////////////////////////////////////
// General Protocol

void HandleGDBPacket( void * dev, char * data, int len )
{
	int i;
	char * odata = data;

	// Got a packet?
	if( data[0] != '$' ) return;

	data++;

	char cmd = *(data++);
	//printf( "DATA: [%c] %c%c%c%c%c%c%c%c%c%c%c%c\n",cmd, data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7], data[8], data[9], data[10], data[11] );
	
	switch( cmd )
	{
	case 'q':
	case 'Q':
		if( StringMatch( data, "Attached" ) )
			SendReplyFull( "1" ); //Attached to an existing process.
		else if( StringMatch( data, "Supported" ) ) // qXfer:threads:
			SendReplyFull( "PacketSize=f000;hwbreak+;vContSupported+;qXfer:memory-map:read+;read+;QStartNoAckMode+" );
		else if( StringMatch( data, "C") ) // Get Current Thread ID. (Can't be -1 or 0.  Those are special)
			SendReplyFull( "QC1" );
		else if( StringMatch( data, "fThreadInfo" ) )  // Query all active thread IDs (Can't be 0 or 1)
			SendReplyFull( "m1" );
		else if( StringMatch( data, "sThreadInfo" ) )  // Query all active thread IDs, continued
			SendReplyFull( "l" );
		// Unimplemented commands.
		else if( StringMatch( data, "Offsets" ) )  // Trace-Status
			SendReplyFull( "" );
		else if( StringMatch( data, "Symbol" ) )   // Trace-Status
			SendReplyFull( "" );
		else if( StringMatch( data, "StartNoAckMode" ) )
		{
			gdbqnoackmode = 1;
			SendReplyFull( "OK" );
		}
		else if( StringMatch( data, "TStatus" ) )  // Trace-Status
			SendReplyFull( "" );
		else if( StringMatch( data, "Rcmd," ) )  // "monitor <command>"
		{
			// will e.g. be "Rcmd,7265736574#"
			// hex-decode the rest of it back to ASCII
			char* cmdHex = data + strlen("Rcmd,");
			//  check if cmd is empty (no character or only '#' following)
			if(strlen(cmdHex) > 1) {
				char cmd[128];
				memset(cmd, 0, sizeof(cmd));
				if ( DecodeHexToBytes(cmdHex, strlen(cmdHex) - 1, cmd, sizeof(cmd) - 1) < 0 ) {
					// decoding failed
					SendReplyFull( "" );
					break;
				}
				//printf("Got monitor command: %s\n", cmd);
				// Support commands that OpenOCD also does:
				// https://openocd.org/doc/html/General-Commands.html
				if(StringMatch(cmd, "halt")) {
					// only halt
					RVCommandResetPart( dev, HALT_MODE_HALT_BUT_NO_RESET);
					SendReplyFull( "+" );
				}
				else if(StringMatch(cmd, "reset halt")) {
					// reset and keep halted after reset
					RVCommandResetPart( dev, HALT_MODE_HALT_AND_RESET);
					SendReplyFull( "+" );
				}
				else if(StringMatch(cmd, "reset run")) {
					// reset and run (i.e., reboot)
					RVCommandResetPart( dev, HALT_MODE_REBOOT);
					SendReplyFull( "+" );
				}
				else if(StringMatch(cmd, "reset")) {
					// same as reset run per OpenOCD
					RVCommandResetPart( dev, HALT_MODE_REBOOT);
					SendReplyFull( "+" );
				} else if(StringMatch(cmd, "resume")) {
					// just resume
					RVCommandResetPart( dev, HALT_MODE_RESUME);
					SendReplyFull( "+" );
				} else if(StringMatch(cmd, "help")) {
					static const char helptext[] = 
						"minichlink GDB monitor help:\n"
						"- halt: Halt execution\n"
						"- resume: Resume execution\n"
						"- reset [halt, run]: Reset and optionally halt or run\n";
					MakeGDBPrintText(helptext);
					SendReplyFull( "+" );
				}
				else {
					fprintf( stderr, "Unknown monitor command '%s', use 'monitor help'.\n", cmd );
					MakeGDBPrintText( "Unknown monitor command, use 'monitor help'\n" );
					SendReplyFull( "-" );
				}
			} else {
				MakeGDBPrintText("No monitor command given, use 'monitor help'\n");
				SendReplyFull( "-" );
			}
		}
		else if( StringMatch( data, "Xfer:memory-map" ) )
		{
			int mslen = strlen( MICROGDBSTUB_MEMORY_MAP ) + 32;
			char map[mslen];
			struct InternalState * iss = (struct InternalState*)(((struct ProgrammerStructBase*)dev)->internal);
			snprintf( map, mslen, MICROGDBSTUB_MEMORY_MAP, iss->flash_size, iss->sector_size, iss->ram_size );
			SendReplyFull( map );
		}
		else if( StringMatch( data, "Xfer:threads" ) )
		{
			static const char * emptyxml = "l<?xml version=\"1.0\"?>\n<threads>\n</threads>";
			SendReplyFull( emptyxml );
		}
		else if( StringMatch( data, "ThreadExtraInfo" ) )
			SendReplyFull( "4E2F41" );
		else if( data[0] == 'P' )
			SendReplyFull( "m1" );			// Archaic threadid.
		else
		{
			fprintf( stderr, "Unknown q command: q%s\n", data );
			SendReplyFull( "" );
		}
		break;
	case 'c':
	case 'C':
		// TODO: Support continue-from-another-address
		RVDebugExec( dev, (cmd == 'C')?HALT_TYPE_CONTINUE_WITH_SIGNAL:HALT_TYPE_CONTINUE, 0, 0 );
		//The real reply will be sent from RVNetPoll 
		break;
	case 's':
	case 'S':
		// TODO: Support step-with-signal.
		RVDebugExec( dev, HALT_TYPE_SINGLE_STEP, 0, 0 );
		//SendReplyFull( "T05" );
		//SendReplyFull( "OK" ); // Will be sent from RVNetPoll
		RVHandleGDBBreakRequest( dev );
		RVSendGDBHaltReason( dev );
		break;
	case 'D':
		// Handle disconnect.
		RVHandleDisconnect( dev );
		//SendReplyFull( "OK" );
		break;
	case 'k':
		RVHandleKillRequest( dev ); // no reply.
		break;
	case 'P': // Set register
	{
		uint32_t reg, value;
		if( ReadHex( &data, -1, &reg ) < 0 ) goto err;
		if( *(data++) != ',' ) goto err;
		if( ReadHex( &data, -1, &value ) < 0 ) goto err;
		RVWriteCPURegister( dev, reg, value );
		break;
	}
	case 'Z': // set
	case 'z': // unset
	{
		uint32_t type = 0;
		uint32_t addr = 0;
		uint32_t time = 0;
		if( ReadHex( &data, -1, &type ) < 0 ) goto err;
		if( *(data++) != ',' ) goto err;
		if( ReadHex( &data, -1, &addr ) < 0 ) goto err;
		if( *(data++) != ',' ) goto err;
		if( ReadHex( &data, -1, &time ) < 0 ) goto err;
		if( RVHandleBreakpoint( dev, cmd == 'Z', addr ) == 0 )
		{
			SendReplyFull( "OK" );
		}
		else
			goto err;
		break;
	}
	case 'm':
	{
		// Read memory (Binary)
		uint32_t address_to_read = 0;
		uint32_t length_to_read = 0;
		if( ReadHex( &data, -1, &address_to_read ) < 0 ) goto err;
		if( *(data++) != ',' ) goto err;
		if( ReadHex( &data, -1, &length_to_read ) < 0 ) goto err;

		uint8_t * pl = alloca( length_to_read * 2 );
		if( RVReadMem( dev, address_to_read, pl, length_to_read ) < 0 )
			goto err;
		char * repl = alloca( length_to_read * 2 );
		int i;
		for( i = 0; i < length_to_read; i++ )
		{
			sprintf( repl + i * 2, "%02x", pl[i] );
		}
		SendReplyFull( repl );
		break;
	}
	case 'M':
	{
		uint32_t address_to_write = 0;
		uint32_t length_to_write = 0;
		if( ReadHex( &data, -1, &address_to_write ) < 0 ) goto err;
		if( *(data++) != ',' ) goto err;
		if( ReadHex( &data, -1, &length_to_write ) < 0 ) goto err;
		if( *(data++) != ':' ) goto err;

		uint8_t * meml = alloca( length_to_write );
		int i;
		for( i = 0; i < length_to_write; i++ )
		{
			uint32_t rv;
			if( ReadHex( &data, 2, &rv ) < 0 ) goto err;
			meml[i] = rv;
		}
		if( RVWriteRAM( dev, address_to_write, length_to_write, meml ) < 0 ) goto err;
		SendReplyFull( "OK" );
		break;
	}
	case 'X':
	{
		// Write memory, binary.
		uint32_t address_to_write = 0;
		uint32_t length_to_write = 0;
		if( ReadHex( &data, -1, &address_to_write ) < 0 ) goto err;
		if( *(data++) != ',' ) goto err;
		if( ReadHex( &data, -1, &length_to_write ) < 0 ) goto err;
		if( *(data++) != ':' ) goto err;
		if( RVWriteRAM( dev, address_to_write, length_to_write, (uint8_t*)data ) < 0 ) goto err;
		SendReplyFull( "OK" );
		break;
	}
	case 'v':
		if( StringMatch( data, "Cont" ) ) // vCont?
		{
			//printf( "CONT: %s\n", data );
			char * de = data + 4;
			char de0;
			if( (de0 = *(de++)) ) // was while?
			{
				//printf( "DE0: %c\n", de0 );
				if( de0 == '?' )
				{
					// Request a list of actions supported by the ‘vCont’ packet. 
					// We don't support vCont
					SendReplyFull( "vCont;c;C;s;S" ); //no ;t because we don't implement them.
					break;
				}
				else if( de0 == ';' )
				{
					//printf( "de[0] = %c\n", de[0] );
					switch( de[0] )
					{
						case 'c':
						case 'C':
							// TODO: Support continue-from-another-address
							RVDebugExec( dev, (cmd == 'C')?HALT_TYPE_CONTINUE_WITH_SIGNAL:HALT_TYPE_CONTINUE, 0, 0 );
							//The real reply will be sent from RVNetPoll 
							break;
						case 's':
						case 'S':
							// TODO: Support step-with-signal.
							RVDebugExec( dev, HALT_TYPE_SINGLE_STEP, 0, 0 );
							//SendReplyFull( "T05" );
							//SendReplyFull( "OK" ); // Will be sent from RVNetPoll
							RVHandleGDBBreakRequest( dev );
							RVSendGDBHaltReason( dev );
							fprintf( stderr, "Step.\n" );
							break;
						default:
							SendReplyFull( "E 98" );
							break;
					}
					de++;
				}
				else if( de0 == ':' )
				{
					// Parse off thread number and throw it away.
					uint32_t signalnumber;
					if( ReadHex( &de, -1, &signalnumber ) < 0 ) goto err;
				}
				else if( de0 == '#' )
				{
					// We're done.
					break;
				}
				else
				{
					SendReplyFull( "E 99" );
				}
			}
		}
		else if( StringMatch( data, "MustReplyEmpty" ) ) //vMustReplyEmpty
		{
			SendReplyFull( "" );
		}
		else if( StringMatch( data, "FlashDone" ) )   //vFlashDone
		{
			SendReplyFull( "OK" );
		}
		else if( StringMatch( data, "Kill" ) )   //vKill
		{
			SendReplyFull( "OK" );
			fprintf( stderr, "Received Kill command. Exiting\n" );
			exit( 0 );
		}
		else if( StringMatch( data, "FlashErase" ) ) //vFlashErase
		{
			data += 10; // FlashErase
			if( *(data++) != ':' ) goto err;
			uint32_t address_to_erase = 0;
			uint32_t length_to_erase = 0;
			if( ReadHex( &data, -1, &address_to_erase ) < 0 ) goto err;
			if( *(data++) != ',' ) goto err;
			if( ReadHex( &data, -1, &length_to_erase ) < 0 ) goto err;

			if( RVErase( dev, address_to_erase, length_to_erase ) == 0 )
				SendReplyFull( "OK" ); 
			else
				SendReplyFull( "E 93" );
		}
		else if( StringMatch( data, "FlashWrite" ) ) //vFlashWrite
		{
			data += 10; // FlashWrite

			if( *(data++) != ':' ) goto err;
			uint32_t address_to_write = 0;
			if( ReadHex( &data, -1, &address_to_write ) < 0 ) goto err;
			if( *(data++) != ':' ) goto err;
			int toflash = len - (data - odata) - 1;
			fprintf( stderr, "Writing flash: %08x len %d\n", address_to_write, toflash );
			if( RVWriteFlash( dev, address_to_write, toflash, (uint8_t*)data ) == 0 )
			{
				fprintf( stderr, "Write OK\n" );
				SendReplyFull( "OK" ); 
			}
			else
				SendReplyFull( "E 93" );
		}
		else
		{
			fprintf( stderr, "Warning: Unknown v command %s\n", data );
			SendReplyFull( "E 01" );
		}
		break;
	case 'g':
	{
		// Register Read (All regs)
		int num_regs = RVGetNumRegisters( dev );

		char cts[num_regs*8+1];
		for( i = 0; i < num_regs+1; i++ )
		{
			uint32_t regret;
			if( RVReadCPURegister( dev, i, &regret ) ) goto err;
			sprintf( cts + i * 8, "%08x", (uint32_t)htonl( regret ) );
		}
		SendReplyFull( cts );
		break;
	}
	case 'p':
	{
		uint32_t regno;
		// Register Read (Specific Reg)
		if( ReadHex( &data, 2, &regno ) < 0 )
			SendReplyFull( "E 10" );
		else
		{
			char cts[9];
			uint32_t regret;
			if( RVReadCPURegister( dev, regno, &regret ) ) goto err;
			sprintf( cts, "%08x", (uint32_t)htonl( regret ) );
			SendReplyFull( cts );
		}
		break;
	}
	case '?': // Query reason for target halt.
		RVSendGDBHaltReason( dev );
		break;
	case 'H':
		// This is for things like selecting threads.
		// I am going to leave this stubbed out.
		SendReplyFull( "" );
		break;
	default:
		fprintf( stderr, "UNKNOWN PACKET: %d (%s)\n", len, data-1 );
		for( i = 0; i < len; i++ )
		{
			fprintf( stderr, "%02x ", data[i] );
		}
		fprintf( stderr, "\n" );
		goto err;
		break;
	}
	return;
err:
	SendReplyFull( "E 00" );
}

void MicroGDBStubHandleClientData( void * dev, const uint8_t * rxdata, int len )
{
	int pl = 0;
	for( pl = 0; pl < len; pl++ )
	{
		int c = rxdata[pl];
		if( c == '$' && gdbbufferstate == 0 )
		{
			gdbrunningcsum = 0;
			gdbbufferplace = 0;
			gdbbufferstate = 1;
		}
		if( c == 3 && gdbbufferstate == 0 )
		{
			RVHandleUnsolicitedGDBBreakRequest( dev );
			continue;
		}
		switch( gdbbufferstate )
		{
		default:
			break;
		case 1:
			if( c != '#' ) gdbrunningcsum += (uint8_t)c;
			if( gdbbufferplace < sizeof( gdbbuffer ) - 2 )
			{
				if( c == '}' ) { gdbbufferstate = 9; break; }
				gdbbuffer[gdbbufferplace++] = c;
			}
			if( c == '#' ) gdbbufferstate = 2;
			break;
		case 9: // escape
			gdbrunningcsum += (uint8_t)c;
			if( gdbbufferplace < sizeof( gdbbuffer ) - 2 )
			{
				char escaped = c ^ 0x20;
				gdbbuffer[gdbbufferplace++] = escaped;
				fprintf( stderr, "ESCAPED @ %02x -> %c [%d]\n", gdbbufferplace, escaped, escaped );
				gdbbufferstate = 1;
			}
			break;
		case 2:
		case 3:
		{
			c = fromhex( c );
			if( gdbbufferstate == 2 )
			{
				gdbchecksum = c << 4;
				gdbbufferstate++;
				break; 
			}
			if( gdbbufferstate == 3 ) gdbchecksum |= c;
			
			gdbbuffer[gdbbufferplace] = 0;

			gdbrunningcsum = (gdbrunningcsum - '$')&0xff;

			if( gdbrunningcsum == gdbchecksum )
			{
				MicroGDBStubSendReply( "+", -1, 0 );
				HandleGDBPacket( dev, (char*)gdbbuffer, gdbbufferplace );
			}
			else
			{
				fprintf( stderr, "Checksum Error: Got %02x expected %02x / len: %d\n", gdbrunningcsum, gdbchecksum, gdbbufferplace );
				int i;
				for( i = 0; i < gdbbufferplace; i++ )
				{
					int c = ((uint8_t*)gdbbuffer)[i];
					fprintf( stderr, "%02x [%c] ", c, (c>=32 && c < 128)?c:' ');
					if( ( i & 0xf ) == 0xf ) fprintf( stderr, "\n" );
				}
				fprintf( stderr, "\n" );
				MicroGDBStubSendReply( "-", -1, 0 );
			}

			gdbbufferplace = 0;
			gdbbufferstate = 0;
			break;
		}
		}
	}
}




#ifdef MICROGDBSTUB_SOCKETS

#include <fcntl.h>
#include <sys/time.h>

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>


int listenMode; // 0 for uninit.  1 for server, 2 for client.
int serverSocket;

///////////////////////////////////////////////////////////////////////////////
// Network layer.


void MicroGDBStubHandleDisconnect( void * dev )
{
	RVHandleDisconnect( dev );
}

static int GDBListen( void * dev )
{
	struct	sockaddr_in sin;
	serverSocket = socket(AF_INET, SOCK_STREAM, 0);

	//Make sure the socket worked.
	if( serverSocket == -1 )
	{
		fprintf( stderr, "Error: Cannot create socket.\n" );
		return -1;
	}

	//Disable SO_LINGER (Well, enable it but turn it way down)
#if defined( WIN32 ) || defined( _WIN32 )
	struct linger lx;
	lx.l_onoff = 1;
	lx.l_linger = 0;
	setsockopt( serverSocket, SOL_SOCKET, SO_LINGER, (const char *)&lx, sizeof( lx ) );

	//Enable SO_REUSEADDR
	int reusevar = 1;
	setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&reusevar, sizeof(reusevar));
#else
	struct linger lx;
	lx.l_onoff = 1;
	lx.l_linger = 0;
	setsockopt( serverSocket, SOL_SOCKET, SO_LINGER, &lx, sizeof( lx ) );

	//Enable SO_REUSEADDR
	int reusevar = 1;
	setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &reusevar, sizeof(reusevar));
#endif
	//Setup socket for listening address.
	memset( &sin, 0, sizeof( sin ) );
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;
	sin.sin_port = htons( MICROGDBSTUB_PORT );

	//Actually bind to the socket
	if( bind( serverSocket, (struct sockaddr *) &sin, sizeof( sin ) ) == -1 )
	{
		fprintf( stderr, "Could not bind to socket: %d\n", MICROGDBSTUB_PORT );
		closesocket( serverSocket );
		serverSocket = 0;
		return -1;
	}

	//Finally listen.
	if( listen( serverSocket, 5 ) == -1 )
	{
		fprintf(stderr, "Could not lieten to socket.");
		closesocket( serverSocket );
		serverSocket = 0;
		return -1;
	}

	fprintf( stderr, "gdbserver running on port %d\n", MICROGDBSTUB_PORT );
	
	return 0;
}

int MicroGDBPollServer( void * dev )
{
	if( !serverSocket ) return -4;

	int pollct = 1;
	struct pollfd allpolls[1] = { 0 };
	allpolls[0].fd = serverSocket;
#if defined( WIN32 ) || defined( _WIN32 )
	allpolls[0].events = 0x00000100; //POLLRDNORM;
#else
	allpolls[0].events = POLLIN;
#endif
	int r = poll( allpolls, pollct, 0 );

	if( r < 0 )
	{
		fprintf( stderr, "R poll(...): %d\n", r );
	}

	//If there's faults, bail.
	if( allpolls[0].revents & (POLLERR|POLLHUP) )
	{
		closesocket( serverSocket );
		if( listenMode == 1 )
		{
			// Some sort of weird fatal close?  Is this even possible?
			fprintf( stderr, "Error: serverSocke was forcibly closed\n" );
			exit( -4 );
		}
		else if( listenMode == 2 )
		{
			MicroGDBStubHandleDisconnect( dev );
			if( serverSocket ) 	close( serverSocket );
			serverSocket = 0;
			listenMode = 1;
			GDBListen( dev );
		}
	}
	if( allpolls[0].revents & POLLIN )
	{
		if( listenMode == 1 )
		{
			struct   sockaddr_in tin;
			socklen_t addrlen  = sizeof(tin);
			memset( &tin, 0, addrlen );
			int tsocket = accept( serverSocket, (struct sockaddr *)&tin, (void*)&addrlen );
			closesocket( serverSocket );
			serverSocket = tsocket;
			listenMode = 2;
			gdbbufferstate = 0;
			RVNetConnect( dev );
			fprintf( stderr, "Connection established to gdbserver backend\n" );	
			// Established.
		}
		else if( listenMode == 2 )
		{
			// Got data from a peer.
			uint8_t buffer[16384];
			ssize_t rx = recv( serverSocket, (char*)buffer, sizeof( buffer ), MSG_NOSIGNAL );
			if( rx == 0 )
			{
				MicroGDBStubHandleDisconnect( dev );
				close( serverSocket );
				serverSocket = 0;
				listenMode = 1;
				GDBListen( dev );
			}
			else
				MicroGDBStubHandleClientData( dev, buffer, (int)rx );
		}
	}

	if( listenMode == 2 )
	{
		RVNetPoll( dev );
	}

	return 0;
}

void MicroGDBExitServer( void * dev )
{
	shutdown( serverSocket, SHUT_RDWR );
	if( listenMode == 2 )
	{
		MicroGDBStubHandleDisconnect( dev );
	}
}


void MicroGDBStubSendReply( const void * data, int len, int docs )
{
	if( len < 0 ) len = strlen( data );
	if( docs )
	{
		uint8_t * localbuffer = alloca( len + 5 );
		localbuffer[0] = '$';
		uint8_t checksum = 0;
		int i;
		for( i = 0; i < len; i++ )
		{
			uint8_t v = ((const uint8_t*)data)[i];
			checksum += v;
			localbuffer[i+1] = v;
		}
		localbuffer[len+1] = '#';
		localbuffer[len+2] = ToHEXNibble( checksum >> 4 );
		localbuffer[len+3] = ToHEXNibble( checksum );
		localbuffer[len+4] = 0;
		data = (void*)localbuffer;
		len += 4;
	}
	
	if( listenMode == 2 )
	{
		//printf( ">>>>%s<<<<\n", data );
		send( serverSocket, data, len, MSG_NOSIGNAL );
	}
}


int MicroGDBStubStartup( void * dev )
{
#if defined( WIN32 ) || defined( _WIN32 )
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	wVersionRequested = MAKEWORD(2, 2);

	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) {
		fprintf( stderr, "WSAStartup failed with error: %d\n", err);
		return 1;
	}
}
#endif

	listenMode = 1;

	return GDBListen( dev );
}


#endif
#endif

#endif


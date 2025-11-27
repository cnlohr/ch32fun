// The rest of the code, Copyright 2023 Charles Lohr
// Freely licensable under the MIT/x11, NewBSD Licenses, or
// public domain where applicable. 

// TODO: Can we make a unified DMPROG for reading + writing?

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include "cmdserver.h"
#include "terminalhelp.h"
#include "minichlink.h"
#include "../ch32fun/ch32fun.h"
#include "chips.h"
#include "ch5xx.h"

#if defined(WINDOWS) || defined(WIN32) || defined(_WIN32)
extern int isatty(int);
#if !defined(_SYNCHAPI_H_) && !defined(__TINYC__)
void Sleep(uint32_t dwMilliseconds);
#endif
#else
#include <pwd.h>
#include <unistd.h>
#include <grp.h>
#endif

static int64_t StringToMemoryAddress( void * dev, const char * number ) __attribute__((used));
static void StaticUpdatePROGBUFRegs( void * dev ) __attribute__((used));
int DefaultReadBinaryBlob( void * dev, uint32_t address_to_read_from, uint32_t read_size, uint8_t * blob );
int DefaultDelayUS( void * dev, int us );
void PostSetupConfigureInterface( void * dev );
int DefaultConfigureReadProtection( void * dev, int one_if_yes_protect );
void TestFunction(void * v );
static void readCSR( void * dev, uint32_t csr );
static int DefaultRebootIntoBootloader( void * dev );
struct MiniChlinkFunctions MCF;

void * MiniCHLinkInitAsDLL( struct MiniChlinkFunctions ** MCFO, const init_hints_t* init_hints )
{
	void * dev = 0;
	
	const char * specpgm = init_hints->specific_programmer;
	if( specpgm )
	{
		if( strcmp( specpgm, "linke" ) == 0 )
			dev = TryInit_WCHLinkE();
		else if( strcmp( specpgm, "isp" ) == 0 )
			dev = TryInit_WCHISP();
		else if( strcmp( specpgm, "esp32s2chfun" ) == 0 )
			dev = TryInit_ESP32S2CHFUN();
		else if( strcmp( specpgm, "nchlink" ) == 0 )
			dev = TryInit_NHCLink042();
		else if( strcmp( specpgm, "b003boot" ) == 0 )
			dev = TryInit_B003Fun(SimpleReadNumberInt(init_hints->serial_port, 0x1209b003));
		else if( strcmp( specpgm, "ardulink" ) == 0 )
			dev = TryInit_Ardulink(init_hints);
	}
	else
	{
		if( (dev = TryInit_WCHISP()) )
		{
			fprintf( stderr, "Found MCU in bootloader mode\n" );
		}
		else if( (dev = TryInit_WCHLinkE()) )
		{
			fprintf( stderr, "Found WCH Link\n" );
		}
		else if( (dev = TryInit_ESP32S2CHFUN()) )
		{
			fprintf( stderr, "Found ESP32S2-Style Programmer\n" );
		}
		else if ((dev = TryInit_NHCLink042()))
		{
			fprintf( stderr, "Found NHC-Link042 Programmer\n" );
		}
		else if ((dev = TryInit_B003Fun(SimpleReadNumberInt(init_hints->serial_port, 0x1209b003))))
		{
			fprintf( stderr, "Found B003Fun Bootloader\n" );
		}
		else if ( init_hints->serial_port && strncmp( init_hints->serial_port, "0x", 2 ) && (dev = TryInit_Ardulink(init_hints)))
		{
			fprintf( stderr, "Found Ardulink Programmer\n" );
		}
	}

	if( !dev )
	{
		if ( specpgm )
		{
			fprintf( stderr, "Error: Could not initialize %s programmer\n", specpgm );	
		} else
		{
			fprintf( stderr, "Error: Could not initialize any supported programmers\n" );
		}
		return 0;
	}

	struct InternalState * iss = calloc( 1, sizeof( struct InternalState ) );
	((struct ProgrammerStructBase*)dev)->internal = iss;
	iss->ram_base = 0x20000000;
	iss->ram_size = 2048;
	iss->sector_size = 64;
	iss->target_chip = NULL;
	iss->target_chip_type = 0;

	SetupAutomaticHighLevelFunctions( dev );

	if( MCFO )
	{
		*MCFO = &MCF;
	}
	return dev;
}

#if !defined( MINICHLINK_AS_LIBRARY ) && !defined( MINICHLINK_IMPORT )
int main( int argc, char ** argv )
{
	int i;

	if( argc > 1 && argv[1][0] == '-' && argv[1][1] == 'h' )
	{
		goto help;
	}
	init_hints_t hints;
	memset(&hints, 0, sizeof(hints));

	// Scan for possible hints.
	for( i = 0; i < argc; i++ )
	{
		char * v = argv[i];
		if( strncmp( v, "-c", 2 ) == 0 )
		{
			i++;
			if( i < argc )
			{
				hints.serial_port = argv[i];
				if( strncmp( hints.serial_port, "0x", 2 ) == 0 )
				{
					hints.specific_programmer = "b003boot";
				}
			}
		}
		else if( strncmp( v, "-C", 2 ) == 0 )
		{
			i++;
			if( i < argc )
				hints.specific_programmer = argv[i];
		}
	}

#if !defined(WINDOWS) && !defined(WIN32) && !defined(_WIN32) && !defined(__APPLE__)
	{
		uid_t uid = getuid();
		struct passwd* pw = getpwuid(uid);
		if( pw )
		{
			gid_t groups[512];
			int ngroups = sizeof( groups ) / sizeof( groups[0] );
			int gl = getgrouplist( pw->pw_name, pw->pw_gid, groups, &ngroups );
			int i;
			for( i = 0; i < gl; i++ )
			{
				struct group * gr = getgrgid( groups[i] );
				if( strcmp( gr->gr_name, "plugdev" ) == 0 || strcmp( gr->gr_name, "dialout" ) == 0 )
					break;
			}
			if( i == gl )
			{
				printf( "WARNING: You are not in the plugdev/dialout group, the canned udev rules will not work on your system.\n" );
			}
		}
	}
#endif

	void * dev = MiniCHLinkInitAsDLL( 0, &hints );
	if( !dev )
	{
		// fprintf( stderr, "Error: Could not initialize any supported programmers\n" );
		return -32;
	}

	int status;
	int must_be_end = 0;

	int skip_startup = 
		(argc > 1 && argv[1][0] == '-' && argv[1][1] == 'k' ) |
		(argc > 1 && argv[1][0] == '-' && argv[1][1] == 'e' ) |
		(argc > 1 && argv[1][0] == '-' && argv[1][1] == 'A' ) |
		(argc > 1 && argv[1][0] == '-' && argv[1][1] == 'u' ) |
		(argc > 1 && argv[1][0] == '-' && argv[1][1] == 'h' ) |
		(argc > 1 && argv[1][0] == '-' && argv[1][1] == 't' ) |
		(argc > 1 && argv[1][0] == '-' && argv[1][1] == 'f' ) |
		(argc > 1 && argv[1][0] == '-' && argv[1][1] == 'X' );

	if( !skip_startup && MCF.SetupInterface )
	{
		if( MCF.SetupInterface( dev ) < 0 )
		{
			fprintf( stderr, "Could not setup interface.\n" );
			return -33;
		}
		printf( "Interface Setup\n" );
	}

	struct InternalState * iss = (struct InternalState*)(((struct ProgrammerStructBase*)dev)->internal);
	if( iss->target_chip == 0 && !skip_startup )
	{
		int ret = 1;
		if( MCF.DetermineChipType ) ret = MCF.DetermineChipType( dev );
		if( ret ) return ret;
	}

	// PostSetupConfigureInterface( dev );

	int iarg = 1;
	const char * lastcommand = 0;
	for( ; iarg < argc; iarg++ )
	{
		char * argchar = argv[iarg];

		lastcommand = argchar;
		if( argchar[0] != '-' )
		{
			fprintf( stderr, "Error: Need prefixing - before commands\n" );
			goto help;
		}
		if( must_be_end )
		{
			fprintf( stderr, "Error: the command '%c' cannot be followed by other commands.\n", must_be_end );
			return -1;
		}
		
keep_going:
		switch( argchar[1] )
		{
			default:
				fprintf( stderr, "Error: Unknown command %c\n", argchar[1] );
			case 'h':
				goto help;
			case 'k':
				printf( "Skipping programmer initialization\n" );
				iss->init_skip++;
				argchar++;
				goto keep_going;
			case '3':
				if( MCF.Control3v3 )
					MCF.Control3v3( dev, 1 );
				else
					goto unimplemented;
				break;
			case '5':
				if( MCF.Control5v )
					MCF.Control5v( dev, 1 );
				else
					goto unimplemented;
				break;
			case 't':
				if( MCF.Control3v3 )
					MCF.Control3v3( dev, 0 );
				else
					goto unimplemented;
				break;
			case 'f':
				if( MCF.Control5v )
					MCF.Control5v( dev, 0 );
				else
					goto unimplemented;
				break;
			case 'C': // For specifying programmer
			case 'c':
				// COM port or programmer argument already parsed previously
				// we still need to skip the next argument
				iarg+=1;
				if( iarg >= argc )
				{
					fprintf( stderr, "-c/C argument required 2 arguments\n" );
					goto unimplemented;
				}
				break;
			case 'u':
				if( MCF.Unbrick )
					MCF.Unbrick( dev );
				else
					goto unimplemented;
				break;
			case 'U':
				// Unlock Bootloader
				if( InternalUnlockBootloader( dev ) )
					goto unimplemented;
				break;
			case 'b':  //reBoot
				if( !MCF.HaltMode || MCF.HaltMode( dev, HALT_MODE_REBOOT ) )
					goto unimplemented;
				break;
			case 'B':  //reBoot into Bootloader
				if( iss->target_chip_type != CHIP_CH32V003
				  && iss->target_chip_type != CHIP_CH32V00x
				  && iss->target_chip_type != CHIP_CH641
				  && iss->target_chip_type != CHIP_CH643
				  && iss->target_chip_type != CHIP_CH32X03x ) DefaultRebootIntoBootloader( dev );
				else if( !MCF.HaltMode || MCF.HaltMode( dev, HALT_MODE_GO_TO_BOOTLOADER ) )
					goto unimplemented;
				break;
			case 'e':  //rEsume
				if( !MCF.HaltMode || MCF.HaltMode( dev, HALT_MODE_RESUME ) )
					goto unimplemented;
				break;
			case 'E':  //Erase whole chip.
				if( MCF.HaltMode ) MCF.HaltMode( dev, HALT_MODE_HALT_AND_RESET );
				if( !MCF.Erase || MCF.Erase( dev, 0, 0, 1 ) )
					goto unimplemented;
				break;
			case 'a':
				if( !MCF.HaltMode || MCF.HaltMode( dev, HALT_MODE_HALT_AND_RESET ) )
					goto unimplemented;
				break;
			case 'A':  // Halt without reboot
				if( !MCF.HaltMode || MCF.HaltMode( dev, HALT_MODE_HALT_BUT_NO_RESET ) )
					goto unimplemented;
				break;

			// disable NRST pin (turn it into a GPIO)
			case 'd':  // see "RSTMODE" in datasheet
				if( MCF.HaltMode ) MCF.HaltMode( dev, HALT_MODE_HALT_AND_RESET );
				if( iss->target_chip_type == CHIP_CH32L103 ||
						iss->target_chip_type == CHIP_CH32V10x ||
						iss->target_chip_type == CHIP_CH32V20x ||
						iss->target_chip_type == CHIP_CH32V205 ||
						iss->target_chip_type == CHIP_CH32V30x ||
						iss->target_chip_type == CHIP_CH32H41x )
				{
					fprintf( stderr, "Warning. This chip can't use NRST as GPIO.\n" );
					break;
				}
				if( MCF.ConfigureNRSTAsGPIO )
					MCF.ConfigureNRSTAsGPIO( dev, 0 );
				else
					goto unimplemented;
				break;
			case 'D': // see "RSTMODE" in datasheet
				if( MCF.HaltMode ) MCF.HaltMode( dev, HALT_MODE_HALT_AND_RESET );
				if( iss->target_chip_type == CHIP_CH32L103 ||
						iss->target_chip_type == CHIP_CH32V10x ||
						iss->target_chip_type == CHIP_CH32V20x ||
						iss->target_chip_type == CHIP_CH32V205 ||
						iss->target_chip_type == CHIP_CH32V30x ||
						iss->target_chip_type == CHIP_CH32H41x )
				{
					fprintf( stderr, "ERROR. This chip can't use NRST as GPIO.\n" );
					break;
				}
				if( MCF.ConfigureNRSTAsGPIO )
					MCF.ConfigureNRSTAsGPIO( dev, 1 );
				else
					goto unimplemented;
				break;
			case 'p': 
				if( MCF.HaltMode ) MCF.HaltMode( dev, HALT_MODE_HALT_AND_RESET );
				if( MCF.ConfigureReadProtection )
				{
					fprintf(stderr, "This will erase the flash entirely!\n");
					fprintf(stderr, "Press Enter to proceed, Ctrl+C to abort.");
					while(!IsKBHit());
					// TODO: revert after testing
					MCF.ConfigureReadProtection( dev, 0 );
				}
				else
					goto unimplemented;

				break;
			case 'P':
				if( MCF.HaltMode ) MCF.HaltMode( dev, HALT_MODE_HALT_AND_RESET );
				if( MCF.ConfigureReadProtection )
					MCF.ConfigureReadProtection( dev, 1 );
				else
					goto unimplemented;
				break;
			case 'S':  // Set FLASH/RAM split in option bytes
			{	
				if( !MCF.SetSplit )
					goto unimplemented;
				enum RAMSplit split = FLASH_DEFAULT;

				iarg+=2;
				if( iarg >= argc )
				{
					fprintf( stderr, "FLASH/RAM split requires two values: flash size and RAM size (in kb)\n" );
					goto unimplemented;
				}

				uint32_t flash_size = SimpleReadNumberInt( argv[iarg-1], 0);
				uint32_t sram_size = SimpleReadNumberInt( argv[iarg], 0 );
				
				if (flash_size == 192 && sram_size == 128) {
					split = FLASH_192_RAM_128;
				} else if (flash_size == 224 && sram_size == 96) {
					split = FLASH_224_RAM_96;
				} else if (flash_size == 256 && sram_size == 64) {
					split = FLASH_256_RAM_64;
				} else if (flash_size == 288 && sram_size == 32) {
					split = FLASH_288_RAM_32;
				}else if (flash_size == 128 && sram_size == 64) {
					split = FLASH_128_RAM_64;
				} else if (flash_size == 144 && sram_size == 48) {
					split = FLASH_144_RAM_48;
				} else if (flash_size == 160 && sram_size == 32) {
					split = FLASH_160_RAM_32;
				} else {
					fprintf( stderr, "Unknown split: %dk FLASH / %dk RAM\n", flash_size, sram_size );
					goto unimplemented;
				}

				MCF.SetSplit(dev, split);
				break;
			}
			case 'G':
			case 'T':
			{
				if( !MCF.PollTerminal )
					goto unimplemented;

				if( argchar[1] == 'G' && SetupGDBServer( dev ) )
				{
					fprintf( stderr, "Error: can't start GDB server\n" );
					return -1;
				}
				if( argchar[1] == 'G' )
				{
					fprintf( stderr, "GDBServer Running\n" );
				}
				else if( argchar[1] == 'T' )
				{
					// In case we aren't running already.
					if( iss->target_chip_type == CHIP_CH57x || iss->target_chip_type == CHIP_CH58x || iss->target_chip_type == CHIP_CH32V10x || iss->init_skip ) MCF.HaltMode( dev, HALT_MODE_RESUME );
					else MCF.HaltMode( dev, HALT_MODE_REBOOT );
				}

				CaptureKeyboardInput();
				printf( "Terminal started\n\n" );

				CMDInit();

#if TERMINAL_INPUT_BUFFER
				char pline_buf[256]; // Buffer that contains current line that is being printed to
				char input_buf[128]; // Buffer that contains user input until it is sent out
				memset( pline_buf, 0, sizeof(pline_buf) );
				memset( input_buf, 0, sizeof(input_buf) );
				uint8_t input_pos = 0;
				uint8_t to_send = 0;
				uint8_t nice_terminal = isatty( fileno(stdout) );
#if defined(WINDOWS) || defined(WIN32) || defined(_WIN32)
				unsigned long console_mode;
				void* handle_output = GetStdHandle(STD_OUTPUT_HANDLE);
				GetConsoleMode(handle_output, &console_mode);
				console_mode |= ENABLE_PROCESSED_OUTPUT | ENABLE_VIRTUAL_TERMINAL_PROCESSING;
				uint8_t set_result = SetConsoleMode(handle_output, console_mode);
				if ( set_result == 0 ) nice_terminal = 0;
#else
				if( nice_terminal > 0 )
				{
					fflush( stdin );
					fprintf( stdout, "\x1b[6n" );
					fflush( stdout );
					read( fileno(stdin), input_buf, 10 );
					if (input_buf[0] != 27)
					{
						nice_terminal = 0;
					}
					else
					{
						printf( TERMINAL_SEND_LABEL );
						fflush( stdout );
					}
					memset( input_buf, 0, sizeof(input_buf) );
				}
#endif
#endif
				uint32_t appendword = 0;
				do
				{
					uint8_t buffer[256];
#if TERMINAL_INPUT_BUFFER
					char print_buf[TERMINAL_BUFFER_SIZE]; // Buffer that is filled with everything and will be written to stdout (basically it's for formatting)
					uint8_t update = 0;
#endif
					if( !IsGDBServerInShadowHaltState( dev ) )
					{
						// Handle keyboard input.
#if TERMINAL_INPUT_BUFFER
						if ( nice_terminal > 0 ) 
						{
							if( IsKBHit() && to_send == 0 )
							{
								uint8_t c = ReadKBByte();
								if ( c == 8 || c == 127 )
								{
									input_buf[input_pos - 1] = 0;
									if ( input_pos > 0 ) input_pos--;
								}
								else if ( c > 31 && c < 127 )
								{
									input_buf[input_pos] = c;
									input_pos++;
								}
								else if ( c == '\n' || c == 10 )
								{
									to_send = input_pos;
								}
								update = 1;
							}
							// Process incomming buffer during sending
							if( to_send > 0 && appendword == 0 )
							{
								for( int i = 0; i < 3; i++ )
								{
									appendword |= input_buf[input_pos - to_send] << ( i * 8 + 8 );
									to_send--;
									if ( to_send == 0 ) break;
								}
								if( to_send == 0 )
								{
									snprintf(print_buf, TERMINAL_BUFFER_SIZE - 1, "%s%s%s\n%s%s", TERMINAL_CLEAR_CUR, TERMIANL_INPUT_SENT, input_buf, pline_buf, TERMINAL_SEND_LABEL);
									fwrite( print_buf, strlen( print_buf ), 1, stdout );
									fflush( stdout );
									input_pos = 0;
									memset( input_buf, 0, sizeof( input_buf ) );
								}
								appendword |= i + 4;
							}
						}
						else
						{
							if( appendword == 0 )
							{
								int i;
								for( i = 0; i < 3; i++ )
								{
									if( !IsKBHit() ) break;
									appendword |= ReadKBByte() << (i*8+8);
								}
								appendword |= i+4; // Will go into DATA0.
							}
						}
#else
						if( appendword == 0 )
						{
							int i;
							for( i = 0; i < 3; i++ )
							{
								if( !IsKBHit() ) break;
								appendword |= ReadKBByte() << (i*8+8);
							}


							// So, previously, we would always use 0x04 as the code for
							// no bytes to send/receive remaining.  But, it looks like 0x00
							// is a valid code too.  So if there's no bytes to send, we can
							// just let the programmer get more data.
							//
							// If we find out in the future there is some reason not to do
							// this, we can undo this and push the responsibility onto the
							// programmers to speed along.
							if( i )
								appendword |= i+4; // Will go into DATA0.
						}
#endif
						int r = MCF.PollTerminal( dev, buffer, sizeof( buffer ), appendword, 0 );
#if TERMINAL_INPUT_BUFFER
						if( (nice_terminal > 0) && ( r == -1 || r == 0 ) && update > 0 )
						{
							strncpy( print_buf, TERMINAL_CLEAR_CUR, TERMINAL_BUFFER_SIZE - 1 );
							if ( to_send > 0 ) strncat( print_buf, TERMINAL_DIM, TERMINAL_BUFFER_SIZE - 1 - strlen(print_buf) );
							strncat( print_buf, TERMINAL_SEND_LABEL, TERMINAL_BUFFER_SIZE - 1 - strlen(print_buf) );
							strncat( print_buf, input_buf, TERMINAL_BUFFER_SIZE - 1 - strlen(print_buf) );
							fwrite( print_buf, strlen( print_buf ), 1, stdout );
							fflush( stdout );
						}
#endif
						if( r < -5 )
						{
							fprintf( stderr, "Terminal dead.  code %d\n", r );
							return -32;
						}
						else if( r < 0 )
						{
							// Other end ack'd without printf. (Or there is another situation)
							appendword = 0;
						}
						else if( r > 0 )
						{
#if TERMINAL_INPUT_BUFFER
							if ( nice_terminal )
							{
								int new_line = -1;
								for( int i = r; i > 0; i-- )
								{
									if( buffer[i-1] == '\n' )
									{
										new_line = r - i;
										break;
									}
								}
								if( new_line < 0 )
								{
									strncpy( print_buf, TERMINAL_CLEAR_PREV, TERMINAL_BUFFER_SIZE - 1 ); //  Go one line up and erase it
									strncat( pline_buf, (char *)buffer, r); // Add newly received chars to line buffer
								}
								else
								{
									strncpy( print_buf, TERMINAL_CLEAR_CUR, TERMINAL_BUFFER_SIZE - 1 ); // Go to the start of the line and erase it
									strncat( pline_buf, (char *)buffer, r - new_line ); // Add newly received chars to line buffer
								}
								strncat( print_buf, pline_buf, TERMINAL_BUFFER_SIZE - 1 - strlen(print_buf) ); // Add line to buffer
								if( new_line >= 0 )
								{
									memset( pline_buf, 0, sizeof( pline_buf ) );
								}
								if( new_line > 0)
								{
									strncat( pline_buf, (char *)buffer+r-new_line, new_line );
									strncat( print_buf, pline_buf, TERMINAL_BUFFER_SIZE - 1 - strlen(print_buf) ); // Add line to buffer
								}
								
								if( to_send > 0 ) strncat( print_buf, TERMINAL_DIM, TERMINAL_BUFFER_SIZE - 1 - strlen(print_buf) );
								strncat( print_buf, TERMINAL_SEND_LABEL, TERMINAL_BUFFER_SIZE - 1 - strlen(print_buf) ); // Print styled "Send" label
								strncat( print_buf, input_buf, TERMINAL_BUFFER_SIZE - 1 - strlen(print_buf) ); // Print current input
								fwrite( print_buf, strlen( print_buf ), 1, stdout );
								print_buf[0] = 0;
								// memset( print_buf, 0, sizeof( print_buf ) );
								
							}
							else
							{
								fwrite( buffer, r, 1, stdout );
							}
#else
							fwrite( buffer, r, 1, stdout );
#endif
							fflush( stdout );
							// Otherwise it's basically just an ack for appendword.
							appendword = 0;
						}
					}

					if( argchar[1] == 'G' )
					{
						PollGDBServer( dev );
					}

					if(1 == CMDPollServer( dev ))
					{
						// TODO: signal to GDB server that it should resume.
					}

				} while( 1 );

				// Currently unreachable - consider reachable-ing
				if( argchar[1] == 'G' )
					ExitGDBServer( dev );
				break;
			}
			case 's':
			{
				iarg+=2;
				if( iarg >= argc )
				{
					fprintf( stderr, "Debug set commands require 2 parameters, a register and a value.\n" );
					goto unimplemented;
				}

				uint32_t datareg = SimpleReadNumberInt( argv[iarg-1], DMDATA0 );
				uint32_t value = SimpleReadNumberInt( argv[iarg], 0 ); 

				if( MCF.WriteReg32 && MCF.FlushLLCommands )
				{
					MCF.FlushLLCommands( dev );	
					MCF.WriteReg32( dev, datareg, value );
					MCF.FlushLLCommands( dev );
				}
				else
					goto unimplemented;
				break;
			}
			case 'm':
			{
				iarg+=1;
				if( iarg >= argc )
				{
					fprintf( stderr, "Debug get commands require 1 parameter, a register.\n" );
					fprintf( stderr, "One of the following:\n"
						"	DMDATA0        0x04\n"
						"	DMDATA1        0x05\n"
						"	DMCONTROL      0x10\n"
						"	DMSTATUS       0x11\n"
						"	DMHARTINFO     0x12\n"
						"	DMABSTRACTCS   0x16\n"
						"	DMCOMMAND      0x17\n"
						"	DMABSTRACTAUTO 0x18\n"
						"	DMPROGBUF0     0x20\n"
						"	DMPROGBUF1     0x21\n"
						"	DMPROGBUF2     0x22\n"
						"	DMPROGBUF3     0x23\n"
						"	DMPROGBUF4     0x24\n"
						"	DMPROGBUF5     0x25\n"
						"	DMPROGBUF6     0x26\n"
						"	DMPROGBUF7     0x27\n"
						"	DMCPBR       0x7C\n"
						"	DMCFGR       0x7D\n"
						"	DMSHDWCFGR   0x7E\n" );

					goto unimplemented;
				}

				uint32_t datareg = SimpleReadNumberInt( argv[iarg], DMDATA0 );

				if( MCF.ReadReg32 && MCF.FlushLLCommands )
				{
					uint32_t value = 0;
					int ret = MCF.ReadReg32( dev, datareg, &value );
					printf( "REGISTER %02x: %08x, %d\n", datareg, value, ret );
					ret = MCF.DetermineChipType( dev );
					if( ret ) return ret;
				}
				else
					goto unimplemented;
				break;
			}
			case 'i':
			{
				if( MCF.PrintChipInfo )
					MCF.PrintChipInfo( dev ); 
				else
					goto unimplemented;
				break;
			}
			case 'X':
			{
				iarg++;
				if( iarg >= argc )
				{
					fprintf( stderr, "Vendor command requires an actual command\n" );
					goto unimplemented;
				}
				if( MCF.VendorCommand )
					if( MCF.VendorCommand( dev, argv[iarg++] ) )
						goto unimplemented;
				break;
			}
			case 'r':
			{
				if( MCF.HaltMode ) MCF.HaltMode( dev, HALT_MODE_HALT_BUT_NO_RESET ); //No need to reboot.

				if( argchar[2] != 0 )
				{
					fprintf( stderr, "Error: can't have char after paramter field\n" ); 
					goto help;
				}
				iarg++;
				argchar = 0; // Stop advancing
				if( iarg + 2 >= argc )
				{
					fprintf( stderr, "Error: missing file for -o.\n" ); 
					goto help;
				}
				const char * fname = argv[iarg++];
				uint64_t offset = StringToMemoryAddress( dev, argv[iarg++] );

				uint64_t amount = SimpleReadNumberInt( argv[iarg], -1 );

				if( !CheckMemoryLocation( dev, DEFAULT_AREA, offset, amount ) )
				{
					if( iss->target_chip_type != CHIP_CH570 &&
						  iss->target_chip_type != CHIP_CH57x &&
						  iss->target_chip_type != CHIP_CH58x &&
						  iss->target_chip_type != CHIP_CH585 &&
						  iss->target_chip_type != CHIP_CH59x )
					{
						iss->current_area = PROGRAM_AREA;
					}
					else
					{
						fprintf( stderr, "Error: memory address is out of range\n" );
						return -9;
					}
					
				}

				FILE * f = 0;
				int hex = 0;
				if( strcmp( fname, "-" ) == 0 )
					f = stdout;
				else if( strcmp( fname, "+" ) == 0 )
					f = stdout, hex = 1;
				else
					f = fopen( fname, "wb" );
				if( !f )
				{
					fprintf( stderr, "Error: can't open write file \"%s\"\n", fname );
					return -9;
				}
				uint8_t * readbuff = malloc( amount );

				if( MCF.ReadBinaryBlob )
				{
					if( MCF.ReadBinaryBlob( dev, offset, amount, readbuff ) < 0 )
					{
						fprintf( stderr, "Fault reading device\n" );
						return -12;
					}
				}				
				else
				{
					goto unimplemented;
				}

				printf( "Read %d bytes\n", (int)amount );

				if( hex )
				{
					int i;
					for( i = 0; i < amount; i++ )
					{
						if( ( i & 0xf ) == 0 )
						{
							if( i != 0 ) printf( "\n" );
							printf( "%08x: ", (uint32_t)(offset + i) );
						}
						printf( "%02x ", readbuff[i] );
					}
					printf( "\n" );
				}
				else
					fwrite( readbuff, amount, 1, f );

				free( readbuff );

				if( f != stdout ) fclose( f );
				break;
			}
			case 'w':
			{
				//struct InternalState * iss = (struct InternalState*)(((struct ProgrammerStructBase*)dev)->internal);
				if( argchar[2] != 0 ) goto help;
				iarg++;
				argchar = 0; // Stop advancing
				if( iarg + 1 >= argc ) goto help;

				// Write binary.
				int len = 0;
				uint8_t * image = 0;
				const char * fname = argv[iarg++];

				if( fname[0] == '-' )
				{
					len = strlen( fname + 1 );
					image = (uint8_t*)strdup( fname + 1 );
					status = 1;
				}
				else if( fname[0] == '+' )
				{
					int hl = strlen( fname+1 );
					if( hl & 1 )
					{
						fprintf( stderr, "Error: hex input doesn't align to chars correctly.\n" );
						return -32;
					}
					len = hl/2;
					image = malloc( len );
					int i;
					for( i = 0; i < len; i ++ )
					{
						char c1 = fname[i*2+1];
						char c2 = fname[i*2+2];
						int v1, v2;
						if( c1 >= '0' && c1 <= '9' ) v1 = c1 - '0';
						else if( c1 >= 'a' && c1 <= 'f' ) v1 = c1 - 'a' + 10;
						else if( c1 >= 'A' && c1 <= 'F' ) v1 = c1 - 'A' + 10;
						else
						{
							fprintf( stderr, "Error: Bad hex\n" );
							return -32;
						}

						if( c2 >= '0' && c2 <= '9' ) v2 = c2 - '0';
						else if( c2 >= 'a' && c2 <= 'f' ) v2 = c2 - 'a' + 10;
						else if( c2 >= 'A' && c2 <= 'F' ) v2 = c2 - 'A' + 10;
						else
						{
							fprintf( stderr, "Error: Bad hex\n" );
							return -32;
						}
						image[i] = (v1<<4) | v2;
					}
					status = 1;
				}
				else
				{
					FILE * f = fopen( fname, "rb" );
					if( !f )
					{
						fprintf( stderr, "Error: Could not open %s\n", fname );
						return -55;
					}
					fseek( f, 0, SEEK_END );
					len = ftell( f );
					fseek( f, 0, SEEK_SET );
					image = malloc( len );
					status = fread( image, len, 1, f );
					fclose( f );
				}
				if( strlen(fname) < 5 || strncmp((char*)(fname+strlen(fname)-4), ".bin", 4))
				{
					fprintf(stderr, "Warning: Make sure you're using a raw binary file. Minichlink can't flash elf or hex files.\n");
				}

				uint64_t offset = StringToMemoryAddress( dev, argv[iarg] );
				if( offset > 0x2fffffff || (!iss->init_skip && offset < iss->target_chip->flash_offset) )
				{
					fprintf( stderr, "Error: Invalid memory offset (%s)\n", argv[iarg] );
					exit( -45 );
				}
				if( !CheckMemoryLocation( dev, DEFAULT_AREA, offset, len ) )
				{
					fprintf( stderr, "Error: binary doesn't fit into this memory area" );
					exit( -44 );
				}
				if( status != 1 )
				{
					fprintf( stderr, "Error: File I/O Fault.\n" );
					exit( -10 );
				}


				int is_flash = IsAddressFlash( offset );
				
				if( MCF.HaltMode && is_flash )
				{
					if( offset == 0x1ffff000 || iss->current_area == BOOTLOADER_AREA )
					{
						MCF.HaltMode( dev, HALT_MODE_HALT_BUT_NO_RESET ); // do not reset if writing bootloader, even if it is considered flash memory
					}
					else MCF.HaltMode( dev, HALT_MODE_HALT_AND_RESET );
				}
				else if( MCF.HaltMode )
				{
					MCF.HaltMode( dev, HALT_MODE_HALT_BUT_NO_RESET );
				}
				
				if( MCF.WriteBinaryBlob )
				{
					printf("Writing image\n");
					if( MCF.WriteBinaryBlob( dev, offset, len, image ) )
					{
						fprintf( stderr, "Error: Fault writing image.\n" );
						return -13;
					}
				}
				else
				{
					goto unimplemented;
				}

				printf( "\nImage written.\n" );

				free( image );
				break;
			}
			case 'N':
			{
				if( MCF.EnableDebug )
				{
					printf("Enabling debug\n");
					if( MCF.EnableDebug( dev, 0 ) )
					{
						return -13;
					}
				}
				break;
			}
			case 'n':
			{
				if( MCF.EnableDebug )
				{
					fprintf( stderr, "This will disable debug module. And you will be only able to program the chip using the bootloader.\nPress Enter to continue\n");
					while(!IsKBHit());
					ReadKBByte();
					fprintf( stderr, "Are you absolutely sure about it?\nPress Enter to continue, or Ctrl+C to cancel\n");
					while(!IsKBHit());
					if( MCF.EnableDebug( dev, 1 ) )
					{
						return -13;
					}
				}
				break;
			}
			case 'Y':
			{
				if( iss->target_chip_type == CHIP_CH570 ||
					  iss->target_chip_type == CHIP_CH57x ||
					  iss->target_chip_type == CHIP_CH58x ||
					  iss->target_chip_type == CHIP_CH585 ||
					  iss->target_chip_type == CHIP_CH59x) CH5xxBlink(dev, 0, 8, 0);
				break;
			}
				
			case 'y':
			{
				readCSR( dev, 0x300 );
				break;
			}
			
		}
		if( argchar && argchar[2] != 0 ) { argchar++; goto keep_going; }
	}

	if( MCF.FlushLLCommands )
		MCF.FlushLLCommands( dev );

	if( MCF.Exit && !skip_startup )
		MCF.Exit( dev );

	return 0;

help:
	fprintf( stderr, "Usage: minichlink [args]\n" );
	fprintf( stderr, " single-letter args may be combined, i.e. -3r\n" );
	fprintf( stderr, " multi-part args cannot.\n" );
	fprintf( stderr, " -3 Enable 3.3V\n" );
	fprintf( stderr, " -5 Enable 5V\n" );
	fprintf( stderr, " -t Disable 3.3V\n" );
	fprintf( stderr, " -f Disable 5V\n" );
	fprintf( stderr, " -k Skip programmer initialization\n" );
	fprintf( stderr, " -c [serial port for Ardulink, try /dev/ttyACM0 or COM11 etc] or [VID+PID of USB for b003boot, try 0x1209b003]\n" );
	fprintf( stderr, " -C [specified programmer, eg. b003boot, ardulink, esp32s2chfun, isp]\n" );
	fprintf( stderr, " -u Clear all code flash - by power off (also can unbrick)\n" );
	fprintf( stderr, " -E Erase chip\n" );
	fprintf( stderr, " -b Reboot out of Halt\n" );
	fprintf( stderr, " -e Resume from halt\n" );
	fprintf( stderr, " -a Reboot into Halt\n" );
	fprintf( stderr, " -A Go into Halt without reboot\n" );
	fprintf( stderr, " -D Configure NRST as GPIO\n" );
	fprintf( stderr, " -d Configure NRST as NRST\n" );
	fprintf( stderr, " -i Show chip info\n" );
	fprintf( stderr, " -s [debug register] [value]\n" );
	fprintf( stderr, " -m [debug register]\n" );
	fprintf( stderr, " -T Terminal Only (must be last arg)\n" );
	fprintf( stderr, " -G Terminal + GDB (must be last arg)\n" );
	fprintf( stderr, " -P Enable Read Protection\n" );
	fprintf( stderr, " -p Disable Read Protection\n" );
	fprintf( stderr, " -N Enable Debug Module\n" );
	fprintf( stderr, " -n Disable Debug Module\n" );
	fprintf( stderr, " -S set FLASH/SRAM split [FLASH kbytes] [SRAM kbytes]\n" );
	fprintf( stderr, " -w [binary image to write] [address, decimal or 0x, try0x08000000]\n" );
	fprintf( stderr, " -r [output binary image] [memory address, decimal or 0x, try 0x08000000] [size, decimal or 0x, try 16384]\n" );
	fprintf( stderr, "   Note: for memory addresses, you can use 'flash' 'launcher' 'bootloader' 'option' 'ram' and say \"ram+0x10\" for instance\n" );
	fprintf( stderr, "   For filename, you can use - for raw (terminal) or + for hex (inline).\n" );
	fprintf( stderr, " -X [programmer-specific command, for esp32-s2 programmer, -X ECLK:1:0:0:8:3 for 24MHz clock out]\n" );

	return -1;	

unimplemented:
	fprintf( stderr, "Error: Command '%s' unimplemented on this programmer.\n", lastcommand );
	return -1;
}
#endif

#if defined(WINDOWS) || defined(WIN32) || defined(_WIN32)
#define strtoll _strtoi64
#endif

int64_t SimpleReadNumberInt( const char * number, int64_t defaultNumber )
{
	if( !number || !number[0] ) return defaultNumber;
	int radix = 10;
	if( number[0] == '0' )
	{
		char nc = number[1];
		number+=2;
		if( nc == 0 ) return 0;
		else if( nc == 'x' ) radix = 16;
		else if( nc == 'b' ) radix = 2;
		else { number--; radix = 8; }
	}
	char * endptr;
	uint64_t ret = strtoll( number, &endptr, radix );
	if( endptr == number )
	{
		return defaultNumber;
	}
	else
	{
		return ret;
	}
}

static int64_t StringToMemoryAddress( void * dev, const char * number )
{
	struct InternalState * iss = (struct InternalState*)(((struct ProgrammerStructBase*)dev)->internal);
	uint32_t base = 0xFFFFFFFF;

	if( strncmp( number, "flash", 5 ) == 0 )
	{
		base = iss->target_chip->flash_offset;
		number += 5;
		iss->current_area = PROGRAM_AREA;
	}
	if( strncmp( number, "launcher", 8 ) == 0 )
	{
		base = iss->target_chip->bootloader_offset;
		number += 8;
		iss->current_area = BOOTLOADER_AREA;
	}    
	if( strncmp( number, "bootloader", 10 ) == 0 )
	{
		base = iss->target_chip->bootloader_offset;
		number += 10;
		iss->current_area = BOOTLOADER_AREA;
	}
	if( strncmp( number, "option", 6 ) == 0 )
	{
		base = iss->target_chip->options_offset;
		number += 6;
		iss->current_area = OPTIONS_AREA;
	}
	if( strncmp( number, "user", 4 ) == 0 )
	{
		base = iss->target_chip->options_offset;
		number += 4;
		iss->current_area = OPTIONS_AREA;
	}
	if( strncmp( number, "ram", 3 ) == 0 )
	{
		base = iss->target_chip->ram_base;
		number += 3;
		iss->current_area = RAM_AREA;
	}

	if( base != 0xFFFFFFFF )
	{
		if( *number != '+' )
			return base;
		number++;
		return base + SimpleReadNumberInt( number, 0 );
	}
	return SimpleReadNumberInt( number, -1 );
}

static int DefaultWaitForFlash( void * dev )
{
	uint32_t rw, timeout = 0;
	do
	{
		rw = 0;
		MCF.ReadWord( dev, (intptr_t)&FLASH->STATR, &rw ); // FLASH_STATR => 0x4002200C
		if( timeout++ > 1000 )
		{
			fprintf( stderr, "Warning: Flash timed out. STATR = %08x\n", rw );
			return -1;
		}
	} while(rw & 3);  // BSY flag for 003, or WRBSY for other processors.

	// This was set at some point for non-003 processors.
	// but, it seems not to be needed.
	// if( rw & 0x20 )
	// {
	// 	// On non-003-processors, clear done op.
	// 	MCF.WriteWord( dev, (intptr_t)&FLASH->STATR, 0x20 );
	// }

	if( rw & FLASH_STATR_WRPRTERR )
	{
		fprintf( stderr, "Memory Protection Error\n" );
		return -44;
	}

	return 0;
}

static int DefaultWaitForDoneOp( void * dev, int ignore )
{
	int r;
	uint32_t rrv;
	int timeout = 100;

	do
	{
		r = MCF.ReadReg32( dev, DMABSTRACTCS, &rrv );
		if( r ) return r;
	}
	while( (rrv & (1<<12)) && timeout-- );

	if( ((rrv >> 8 ) & 7) || (rrv & (1<<12)) )
	{
		if( !ignore )
		{
			const char * errortext = 0;
			switch( (rrv>>8)&7 )
			{
			case 1: errortext = "Command in execution"; break;
			case 2: errortext = "Abstract Command Unsupported"; break;
			case 3: errortext = "Exception executing Abstract Command"; break;
			case 4: errortext = "Processor not halted."; break;
			case 5: errortext = "Bus Error"; break;
			case 6: errortext = "Parity Bit"; break;
			default: errortext = "Other Error"; break;
			}

			uint32_t temp;
			MCF.ReadReg32( dev, DMSTATUS, &temp );
			fprintf( stderr, "Fault on op (DMABSTRACTS = %08x) (%d) (%s) DMSTATUS: %08x\n", rrv, timeout, errortext, temp );
		}
		MCF.WriteReg32( dev, DMABSTRACTCS, 0x00000700 );
		return -9;
	}
	return 0;
}

int DefaultSetupInterface( void * dev )
{
	struct InternalState * iss = (struct InternalState*)(((struct ProgrammerStructBase*)dev)->internal);

	if( MCF.Control3v3 ) MCF.Control3v3( dev, 1 );
	MCF.DelayUS( dev, 16000 );
	MCF.WriteReg32( dev, DMSHDWCFGR, 0x5aa50000 | (1<<10) ); // Shadow Config Reg
	MCF.WriteReg32( dev, DMCFGR, 0x5aa50000 | (1<<10) ); // CFGR (1<<10 == Allow output from slave)
	MCF.WriteReg32( dev, DMSHDWCFGR, 0x5aa50000 | (1<<10) ); // sometimes doing this just once isn't enough
	MCF.WriteReg32( dev, DMCFGR, 0x5aa50000 | (1<<10) ); // And this is about as fast as checking, so why not.  
	MCF.WriteReg32( dev, DMCONTROL, 0x80000001 ); // Make the debug module work properly.
	MCF.WriteReg32( dev, DMCONTROL, 0x80000001 );
	MCF.WriteReg32( dev, DMCONTROL, 0x80000001 );
	// Why do we repeat this commands? Because in some cases they can be lost, if debug module is still starting
	// and it's better to be send these crucial commands twice fot better chance for success.

	// Read back chip status.  This is really basic.
	uint32_t reg = 0;
	int r = MCF.ReadReg32( dev, DMSTATUS, &reg );
	if( r >= 0 )
	{
		// Valid R.
		if( reg == 0x00000000 || reg == 0xffffffff )
		{
			fprintf( stderr, "Error: Setup chip failed. Got code %08x\n", reg );
			return -9;
		}
	}
	else
	{
		fprintf( stderr, "Error: Could not read dmstatus. r = %d\n", r );
		return r;
	}

	iss->statetag = STTAG( "STRT" );
	return 0;
}

static int DefaultGetUUID( void * dev, uint8_t * buffer )
{
	struct InternalState * iss = (struct InternalState*)(((struct ProgrammerStructBase*)dev)->internal);
	int ret = 0;
	if( iss->target_chip == NULL)
	{
		ret = MCF.DetermineChipType( dev );
		if( ret ) return ret;
	}
	uint8_t local_buffer[8] = {0, 0, 0, 0, 0, 0, 0, 0};
	enum RiscVChip chip = iss->target_chip_type;

	if( iss->target_chip->protocol == PROTOCOL_DEFAULT )
	{
		MCF.WriteReg32( dev, DMABSTRACTAUTO, 0 );
		MCF.WriteReg32( dev, DMPROGBUF0, 0x90024000 ); // c.ebreak <<== c.lw x8, 0(x8)
		if( chip == CHIP_CH32M030 ) MCF.WriteReg32( dev, DMDATA0, 0x1ffff3a8 );
		else MCF.WriteReg32( dev, DMDATA0, 0x1ffff7e8 );
		MCF.WriteReg32( dev, DMCOMMAND, 0x00271008 ); // Copy data to x8, and execute.
		MCF.WaitForDoneOp( dev, 0 );
		MCF.WriteReg32( dev, DMCOMMAND, 0x00221008 ); // Copy data from x8.
		MCF.ReadReg32( dev, DMDATA0, (uint32_t*)local_buffer );
		if( chip == CHIP_CH32M030 ) MCF.WriteReg32( dev, DMDATA0, 0x1ffff3ac );
		else MCF.WriteReg32( dev, DMDATA0, 0x1ffff7ec );
		MCF.WriteReg32( dev, DMCOMMAND, 0x00271008 ); // Copy data to x8, and execute.
		MCF.WaitForDoneOp( dev, 0 );
		MCF.WriteReg32( dev, DMCOMMAND, 0x00221008 ); // Copy data from x8.
		MCF.ReadReg32( dev, DMDATA0, (uint32_t*)(local_buffer + 4) );
		*((uint32_t*)buffer) = local_buffer[0]<<24|local_buffer[1]<<16|local_buffer[2]<<8|local_buffer[3];
		*(((uint32_t*)buffer)+1) = local_buffer[4]<<24|local_buffer[5]<<16|local_buffer[6]<<8|local_buffer[7];
	}
	else if( iss->target_chip->protocol == PROTOCOL_CH5xx )
	{
		ret = CH5xxReadUUID( dev, local_buffer );
		memcpy(buffer, local_buffer, 8);
	}
	return ret;
}

static int DefaultDetermineChipType( void * dev )
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

		MCF.WriteReg32( dev, DMCONTROL, 0x80000001 ); // Make the debug module work properly.
		MCF.WriteReg32( dev, DMCONTROL, 0x80000001 ); // Initiate halt request.

		// Tricky, this function needs to clean everything up because it may be used entering debugger.
		uint32_t old_data0;
		MCF.ReadReg32( dev, DMDATA0, &old_data0 );
		MCF.WriteReg32( dev, DMCOMMAND, 0x00221008 ); // Copy data from x8.
		uint32_t old_x8;
		MCF.ReadReg32( dev, DMDATA0, &old_x8 );

		uint32_t marchid = 0;

		MCF.WriteReg32( dev, DMABSTRACTCS, 0x08000700 ); // Clear out any dmabstractcs errors.

		MCF.WriteReg32( dev, DMABSTRACTAUTO, 0x00000000 );
		MCF.WriteReg32( dev, DMCOMMAND, 0x00220000 | 0xf12 );
		MCF.WriteReg32( dev, DMCOMMAND, 0x00220000 | 0xf12 );  // Need to double-read, not sure why.
		MCF.ReadReg32( dev, DMDATA0, &marchid );
		MCF.WriteReg32( dev, DMPROGBUF0, 0x90024000 ); // c.ebreak <<== c.lw x8, 0(x8)
		MCF.FlushLLCommands(dev);

		uint32_t chip_id = 0;
		uint32_t vendor_bytes = 0;
		uint32_t sevenf_id = 0;
		int read_protection = 0;
		MCF.ReadReg32( dev, 0x7f, &sevenf_id );

		if( sevenf_id == 0 )
		{
			// Need to load new progbuf because we're reading 1 byte now
			MCF.WriteReg32( dev, DMPROGBUF0, 0x00040403 ); // lb x8, 0(x8)
			MCF.WriteReg32( dev, DMPROGBUF1, 0x00100073 ); // c.ebreak
			MCF.WriteReg32( dev, DMDATA0, 0x40001041 ); // Special chip ID location.
			MCF.WriteReg32( dev, DMCOMMAND, 0x00271008 ); // Copy data to x8, and execute.
			MCF.WaitForDoneOp( dev, 0 );
			MCF.WriteReg32( dev, DMCOMMAND, 0x00221008 ); // Copy data from x8.
			MCF.ReadReg32( dev, DMDATA0, &chip_id );
			chip_id = chip_id & 0xff;

			// Looks like a CH32V103 or a CH56x
			if( chip_id == 0 )
			{
				// First check for CH56x
				MCF.WriteReg32( dev, DMDATA0, 0x40001001 );			
				MCF.WriteReg32( dev, DMCOMMAND, 0x00271008 ); // Copy data to x8, and execute.
				MCF.WaitForDoneOp( dev, 1 );
				MCF.WriteReg32( dev, DMCOMMAND, 0x00221008 ); // Copy data from x8.
				MCF.ReadReg32( dev, DMDATA0, &chip_id );
				MCF.WriteReg32( dev, DMDATA0, 0x40001002 ); // Special chip ID location.
				MCF.WriteReg32( dev, DMCOMMAND, 0x00271008 ); // Copy data to x8, and execute.
				MCF.WaitForDoneOp( dev, 1 );
				MCF.WriteReg32( dev, DMCOMMAND, 0x00221008 ); // Copy data from x8.
				MCF.ReadReg32( dev, DMDATA0, &vendor_bytes );

				if( (vendor_bytes & 0xff) == 2 && ((chip_id & 0xff) == 65 || (chip_id & 0xff) == 69) )
				{
					if( (chip_id & 0xff) == 0x65 ) iss->target_chip = &ch565;
					if( (chip_id & 0xff) == 0x69 ) iss->target_chip = &ch569;
					iss->target_chip_id = chip_id << 24;
					fprintf( stderr, "Found interesting specimen, please tell us about it in Discord or on GitHub, and provide this value: %02x\n", chip_id);
					goto chip_identified;
				}

				// Now actually check for CH32V103
				MCF.WriteReg32( dev, DMPROGBUF0, 0x90024000 ); // c.ebreak <<== c.lw x8, 0(x8)
				MCF.WriteReg32( dev, DMDATA0, 0x1ffff880 ); // Special chip ID location.
				MCF.WriteReg32( dev, DMCOMMAND, 0x00271008 ); // Copy data to x8, and execute.
				MCF.WaitForDoneOp( dev, 1 );
				MCF.WriteReg32( dev, DMCOMMAND, 0x00221008 ); // Copy data from x8.
				MCF.ReadReg32( dev, DMDATA0, &chip_id );
				MCF.WriteReg32( dev, DMDATA0, 0x1ffff884 ); // Special chip ID location.
				MCF.WriteReg32( dev, DMCOMMAND, 0x00271008 ); // Copy data to x8, and execute.
				MCF.WaitForDoneOp( dev, 1 );
				MCF.WriteReg32( dev, DMCOMMAND, 0x00221008 ); // Copy data from x8.
				MCF.ReadReg32( dev, DMDATA0, &vendor_bytes );
				
				if( ((((vendor_bytes >> 16) & 0xff00) != 0x2500) && (((vendor_bytes >> 16) & 0xdf00) != 0x1500)) || chip_id != 0xdc78fe34 )
				{
					uint32_t flash_obr = 0;
					MCF.WriteReg32( dev, DMDATA0, 0x4002201c ); // Special chip ID location.
					MCF.WriteReg32( dev, DMCOMMAND, 0x00271008 ); // Copy data to x8, and execute.
					MCF.WaitForDoneOp( dev, 1 );
					MCF.WriteReg32( dev, DMCOMMAND, 0x00221008 ); // Copy data from x8.
					MCF.ReadReg32( dev, DMDATA0, &flash_obr );
						
					if( (flash_obr & 3) == 2 )
					{
						iss->target_chip = &ch32v103;
						read_protection = 1;
					}
				}
				else
				{
					iss->target_chip = &ch32v103;
					iss->target_chip_id = vendor_bytes;
					read_protection = -1;
				}
			}
			else
			{
				// Check for CH5xx
				read_protection = -1;
				if( (chip_id & 0xf0) == 0x90 )
				{
					uint32_t sevenc = 0;
					MCF.ReadReg32( dev, DMCPBR, &sevenc );
					if((sevenc & 0x30000) == 0)
					{
						if( chip_id == 0x91 )iss->target_chip = &ch591;
						if( chip_id == 0x92 )iss->target_chip = &ch592;
						uint32_t some_option;
						ch5xx_read_options_bulk(dev, 0x7f010, (uint8_t*)(&some_option), 4);
						if (some_option == 9) {
							iss->target_chip_id = chip_id << 24 | 9;
						}
						else
						{
							iss->target_chip_id = chip_id << 24;
						}
					}
					else
					{
						iss->target_chip = &ch585;
						iss->target_chip_id = chip_id << 24;
					}
				}
				else
				{
					if( chip_id == 0x70 )
					{
						iss->target_chip = &ch570;
						iss->target_chip_id = chip_id << 24;
					}
					else if( chip_id == 0x71 )
					{
						iss->target_chip = &ch571;
						iss->target_chip_id = chip_id << 24;
					}
					else if( chip_id == 0x72 )
					{
						iss->target_chip = &ch572;
						iss->target_chip_id = chip_id << 24;
					}
					else if( chip_id == 0x73 )
					{
						uint32_t ch573_variant = 0;
						ch5xx_read_options( dev, 0x7f00c, (uint8_t*)&ch573_variant );
						if( (int)(ch573_variant << 18) < 0 )
						{
							iss->target_chip = &ch573q;
							iss->target_chip_id = 0x73550000;
						}
						else
						{
							iss->target_chip = &ch573;
							iss->target_chip_id = chip_id << 24;
						}
					}
					else if( chip_id == 0x82 )
					{
						iss->target_chip = &ch582;
						iss->target_chip_id = chip_id << 24;
					}
					else if( chip_id == 0x83 )
					{
						iss->target_chip = &ch583;
						iss->target_chip_id = chip_id << 24;
					}
				}
			}
		}
		else
		{
			uint32_t chip_id_address = 0x1ffff7c4;
			uint32_t flash_size_address = 0x1ffff7e0;
			uint32_t masked_id = sevenf_id & 0xfff00000;
			uint32_t masked_id2 = sevenf_id & 0xfff00f00;

			if( masked_id == 0x3b00000 )
			{
				iss->target_chip = &ch32m030;
				iss->target_chip_id = sevenf_id;
				flash_size_address = 0x1ffff3a0;
			}
			else if( masked_id == 0x56400000 )
			{
				iss->target_chip = &ch564;
				iss->target_chip_id = sevenf_id;
			}
			else if( masked_id == 0x31700000 )
			{
				iss->target_chip = &ch32v317;
				chip_id_address = 0x1ffff704;
			}
			else if( masked_id == 0x00200000 )
			{
				iss->target_chip = &ch32v002;
				chip_id_address = 0x1ffff704;
			}
			else if( masked_id == 0x00400000 )
			{
				iss->target_chip = &ch32v004;
				chip_id_address = 0x1ffff704;
			}
			else if( masked_id == 0x00500000 )
			{
				iss->target_chip = &ch32v005;
				chip_id_address = 0x1ffff704;
			}
			else if( masked_id == 0x00600000 )
			{
				iss->target_chip = &ch32v006;
				chip_id_address = 0x1ffff704;
			}
			else if( masked_id == 0x00700000 )
			{
				iss->target_chip = &ch32v007;
				chip_id_address = 0x1ffff704;
			}
			else if( (masked_id & 0xff000000) == 0x41000000 )
			{
				switch ((sevenf_id & 0xfff00000) >> 20)
				{
					case 0x415:
						iss->target_chip = &ch32h415;
						break;
					case 0x416:
						iss->target_chip = &ch32h416;
						break;
					case 0x417:
						iss->target_chip = &ch32h417;
						break;
				}
				chip_id_address = 0x1ffff704; // Maybe wrong, will have to check
			}

			if( masked_id2 == 0x64100500 )
			{
				iss->target_chip = &ch641;
			}
			else if( masked_id2 == 0x64300600 )
			{
				iss->target_chip = &ch643;
			}
			else if( masked_id == 0x64500000 )
			{
				iss->target_chip = &ch645;
			}
			else if( masked_id2 == 0x3500600 )
			{
				iss->target_chip = &ch32x035;
				iss->target_chip_id = sevenf_id;
			}
			else if( masked_id2 == 0x10300700 )
			{
				iss->target_chip = &ch32l103;
				iss->target_chip_id = sevenf_id;
			}
			else if( masked_id2 == 0x300500 )
			{
				iss->target_chip = &ch32v003;
			}

			if( (sevenf_id & 0x20000500) == 0x20000500 || (sevenf_id & 0x30000500) == 0x30000500 )
			{
				switch ((sevenf_id & 0xfff00000) >> 20)
				{
					case 0x203:
						iss->target_chip = &ch32v203;
						break;
					case 0x205:
						iss->target_chip = &ch32v205;	// Need to confirm that it actually appears here
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
				}
				chip_id_address = 0x1ffff704;
			}
			if( iss-> target_chip  && !iss->target_chip_id )
			{
				MCF.WriteReg32( dev, DMDATA0, chip_id_address );
				MCF.WriteReg32( dev, DMCOMMAND, 0x00271008 ); // Copy data to x8, and execute.
				MCF.WaitForDoneOp( dev, 1 );
				MCF.WriteReg32( dev, DMCOMMAND, 0x00221008 ); // Copy data from x8.
				MCF.ReadReg32( dev, DMDATA0, &chip_id );

				iss->target_chip_id = chip_id;
			}
			if( iss-> target_chip && flash_size_address )
			{
				MCF.WriteReg32( dev, DMDATA0, flash_size_address );
				MCF.WriteReg32( dev, DMCOMMAND, 0x00271008 ); // Copy data to x8, and execute.
				MCF.WaitForDoneOp( dev, 1 );
				MCF.WriteReg32( dev, DMCOMMAND, 0x00221008 ); // Copy data from x8.
				MCF.ReadReg32( dev, DMDATA0, &vendor_bytes );

				iss->flash_size = vendor_bytes & 0xFFFF;
			}
		}

chip_identified:

		if( iss->target_chip == NULL)
		{
			fprintf( stderr, "Unknown chip type.  Report as bug with picture of chip.\n" );
			fprintf( stderr, "marchid : %08x\n", marchid );
			fprintf( stderr, "HARTINFO: %08x\n", rr );
			return -2;
		}
		else
		{
			if( iss->target_chip->protocol == PROTOCOL_UNSUPPORTED )
			{
				fprintf( stderr, "Detected %s chip, but it's not supported yet. Aborting\n", iss->target_chip->name_str );
				return -3;
			}

			if( iss->target_chip_type == CHIP_CH570 )
			{
				uint32_t options;
				MCF.ReadWord( dev, 0x40001058, &options );
				if( (options&0x800000) || (options&0x200000) ) read_protection = 1;
			}
			else if( read_protection == 0 )
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
				MCF.ReadBinaryBlob = CH5xxReadBinaryBlob;
				MCF.WriteBinaryBlob = CH5xxWriteBinaryBlob;
				MCF.Erase = CH5xxErase;
				MCF.SetClock = CH5xxSetClock;
				MCF.GetUUID = CH5xxReadUUID;
				MCF.ConfigureNRSTAsGPIO = CH5xxConfigureNRSTAsGPIO;
				if( iss->target_chip_type == CHIP_CH570 ) MCF.ConfigureReadProtection = DefaultConfigureReadProtection;
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
		// Cleanup
		MCF.WriteReg32( dev, DMDATA0, old_x8 );
		MCF.WriteReg32( dev, DMCOMMAND, 0x00231008 ); // Copy data to x8
		MCF.WriteReg32( dev, DMDATA0, old_data0 );

		iss->statetag = STTAG( "XXXX" );
	}

	return 0;
}

static void StaticUpdatePROGBUFRegs( void * dev )
{
	struct InternalState * iss = (struct InternalState*)(((struct ProgrammerStructBase*)dev)->internal);
	uint32_t rr;
	if( MCF.ReadReg32( dev, DMHARTINFO, &rr ) )
	{
		fprintf( stderr, "Error: Could not get hart info.\n" );
		return;
	}

		int ret = 0;
		if( iss->target_chip == NULL )
		{
			ret = MCF.DetermineChipType( dev );
			if( ret ) return;
		}

	uint32_t data0offset = 0xe0000000 | ( rr & 0x7ff );

	// Putting DATA0's location into x10, and DATA1's location into x11 is universal for all continued code.
	MCF.WriteReg32( dev, DMABSTRACTAUTO, 0x00000000 ); // Disable Autoexec.
	MCF.WriteReg32( dev, DMDATA0, data0offset );       // DATA0's location in memory.
	MCF.WriteReg32( dev, DMCOMMAND, 0x0023100a );      // Copy data to x10
	MCF.WriteReg32( dev, DMDATA0, data0offset + 4 );   // DATA1's location in memory.
	MCF.WriteReg32( dev, DMCOMMAND, 0x0023100b );      // Copy data to x11
	MCF.WriteReg32( dev, DMDATA0, 0x4002200c );        // FLASH->STATR, add 4 to get FLASH->CTLR
	MCF.WriteReg32( dev, DMCOMMAND, 0x0023100c );      // Copy data to x12

	// v003 requires bufload every word.
	// x035 requires bufload every word in spite of what the datasheet says.
	// CR_PAGE_PG = FTPG = 0x00010000 | CR_BUF_LOAD = 0x00040000
	// We just don't do the write on the v20x/v30x.
	MCF.WriteReg32( dev, DMDATA0, 0x00010000|0x00040000 );
	MCF.WriteReg32( dev, DMCOMMAND, 0x0023100d );      // Copy data to x13
}

int InternalUnlockBootloader( void * dev )
{
	if( !MCF.WriteWord ) return -99;
	int ret = 0;
	uint32_t STATR;
	ret |= MCF.WriteWord( dev, 0x40022028, 0x45670123 ); //(FLASH_BOOT_MODEKEYP)
	ret |= MCF.WriteWord( dev, 0x40022028, 0xCDEF89AB ); //(FLASH_BOOT_MODEKEYP)
	ret |= MCF.ReadWord( dev, 0x4002200C, &STATR ); //(FLASH_OBTKEYR)
	if( ret )
	{
		fprintf( stderr, "Error operating with OBTKEYR\n" );
		return -1;
	}
	if( STATR & (1<<15) )
	{
		fprintf( stderr, "Error: Could not unlock boot section (%08x)\n", STATR );
	}
	STATR |= (1<<14); // Configure for boot-to-bootload.
	ret |= MCF.WriteWord( dev, 0x4002200C, STATR );
	ret |= MCF.ReadWord( dev, 0x4002200C, &STATR ); //(FLASH_OBTKEYR)

	// Need to flush state.
	struct InternalState * iss = (struct InternalState*)(((struct ProgrammerStructBase*)dev)->internal);
	iss->statetag = STTAG( "XXXX" );

	return ret;
}


int InternalIsMemoryErased( struct InternalState * iss, uint32_t address )
{
	if(( address & 0xff000000 ) != 0x08000000 ) return 0;
	int sector = (address & 0xffffff) / iss->sector_size;
	if( sector >= MAX_FLASH_SECTORS )
		return 0;
	else
		return iss->flash_sector_status[sector];
}

void InternalMarkMemoryNotErased( struct InternalState * iss, uint32_t address )
{
	if(( address & 0xff000000 ) != 0x08000000 ) return;
	int sector = (address & 0xffffff) / iss->sector_size;
	if( sector < MAX_FLASH_SECTORS )
		iss->flash_sector_status[sector] = 0;
}

static int DefaultWriteHalfWord( void * dev, uint32_t address_to_write, uint16_t data )
{
	int ret = 0;
	struct InternalState * iss = (struct InternalState*)(((struct ProgrammerStructBase*)dev)->internal);
	if( MCF.VoidHighLevelState ) MCF.VoidHighLevelState( dev );
	iss->statetag = STTAG( "XXXX" );

	MCF.WriteReg32( dev, DMABSTRACTAUTO, 0x00000000 ); // Disable Autoexec.

	// Different address, so we don't need to re-write all the program regs.
	// sh x8,0(x9)  // Write to the address.
	MCF.WriteReg32( dev, DMPROGBUF0, 0x00849023 );
	MCF.WriteReg32( dev, DMPROGBUF1, 0x00100073 ); // c.ebreak

	MCF.WriteReg32( dev, DMDATA0, address_to_write );
	MCF.WriteReg32( dev, DMCOMMAND, 0x00231009 ); // Copy data to x9
	MCF.WriteReg32( dev, DMDATA0, data );
	MCF.WriteReg32( dev, DMCOMMAND, 0x00271008 ); // Copy data to x8, and execute program.

	ret |= MCF.WaitForDoneOp( dev, 0 );
	iss->currentstateval = -1;

	if( ret ) fprintf( stderr, "Fault on DefaultWriteHalfWord\n" );
	return ret;
}

static int DefaultReadHalfWord( void * dev, uint32_t address_to_write, uint16_t * data )
{
	int ret = 0;
	struct InternalState * iss = (struct InternalState*)(((struct ProgrammerStructBase*)dev)->internal);
	if( MCF.VoidHighLevelState ) MCF.VoidHighLevelState( dev );
	iss->statetag = STTAG( "XXXX" );

	MCF.WriteReg32( dev, DMABSTRACTAUTO, 0x00000000 ); // Disable Autoexec.

	// Different address, so we don't need to re-write all the program regs.
	// lh x8,0(x9)  // Write to the address.
	MCF.WriteReg32( dev, DMPROGBUF0, 0x00049403 ); // lh x8, 0(x9)
	MCF.WriteReg32( dev, DMPROGBUF1, 0x00100073 ); // c.ebreak

	MCF.WriteReg32( dev, DMDATA0, address_to_write );
	MCF.WriteReg32( dev, DMCOMMAND, 0x00231009 ); // Copy data to x9
	MCF.WriteReg32( dev, DMCOMMAND, 0x00241000 ); // Only execute.
	MCF.WriteReg32( dev, DMCOMMAND, 0x00221008 ); // Read x8 into DATA0.

	ret |= MCF.WaitForDoneOp( dev, 0 );
	iss->currentstateval = -1;

	if( ret ) fprintf( stderr, "Fault on DefaultReadHalfWord\n" );

	uint32_t rr;
	ret |= MCF.ReadReg32( dev, DMDATA0, &rr );
	*data = rr;
	return ret;
}

static int DefaultWriteByte( void * dev, uint32_t address_to_write, uint8_t data )
{
	int ret = 0;
	struct InternalState * iss = (struct InternalState*)(((struct ProgrammerStructBase*)dev)->internal);
	if( MCF.VoidHighLevelState ) MCF.VoidHighLevelState( dev );
	iss->statetag = STTAG( "XXXX" );

	MCF.WriteReg32( dev, DMABSTRACTAUTO, 0x00000000 ); // Disable Autoexec.

	// Different address, so we don't need to re-write all the program regs.
	// sh x8,0(x9)  // Write to the address.
	MCF.WriteReg32( dev, DMPROGBUF0, 0x00848023 ); // sb x8, 0(x9)
	MCF.WriteReg32( dev, DMPROGBUF1, 0x00100073 ); // c.ebreak

	MCF.WriteReg32( dev, DMDATA0, address_to_write );
	MCF.WriteReg32( dev, DMCOMMAND, 0x00231009 ); // Copy data to x9
	MCF.WriteReg32( dev, DMDATA0, data );
	MCF.WriteReg32( dev, DMCOMMAND, 0x00271008 ); // Copy data to x8, and execute program.

	ret |= MCF.WaitForDoneOp( dev, 0 );
	if( ret ) fprintf( stderr, "Fault on DefaultWriteByte\n" );
	iss->currentstateval = -1;
	return ret;
}

static int DefaultReadByte( void * dev, uint32_t address_to_read, uint8_t * data )
{
	int ret = 0;
	struct InternalState * iss = (struct InternalState*)(((struct ProgrammerStructBase*)dev)->internal);
	if( MCF.VoidHighLevelState ) MCF.VoidHighLevelState( dev );
	iss->statetag = STTAG( "XXXX" );

	MCF.WriteReg32( dev, DMABSTRACTAUTO, 0x00000000 ); // Disable Autoexec.

	// Different address, so we don't need to re-write all the program regs.
	// lb x8,0(x9)  // Read from the address.
	MCF.WriteReg32( dev, DMPROGBUF0, 0x00048403 ); // lb x8, 0(x9)
	MCF.WriteReg32( dev, DMPROGBUF1, 0x00100073 ); // c.ebreak

	MCF.WriteReg32( dev, DMDATA0, address_to_read );
	MCF.WriteReg32( dev, DMCOMMAND, 0x00231009 ); // Copy data to x9
	MCF.WriteReg32( dev, DMCOMMAND, 0x00241000 ); // Only execute.
	MCF.WriteReg32( dev, DMCOMMAND, 0x00221008 ); // Read x8 into DATA0.

	ret |= MCF.WaitForDoneOp( dev, 0 );
	if( ret ) fprintf( stderr, "Fault on DefaultReadByte\n" );
	iss->currentstateval = -1;

	uint32_t rr;
	ret |= MCF.ReadReg32( dev, DMDATA0, &rr );
	*data = rr;
	return ret;
}

static int DefaultWriteWord( void * dev, uint32_t address_to_write, uint32_t data )
{
	struct InternalState * iss = (struct InternalState*)(((struct ProgrammerStructBase*)dev)->internal);
	int ret = 0;

	int is_flash = IsAddressFlash( address_to_write );
	// int is_flash = 0;

	if( iss->statetag != STTAG( "WRSQ" ) || is_flash != iss->lastwriteflags )
	{
		int did_disable_req = 0;
		if( iss->statetag != STTAG( "WRSQ" ) )
		{
			MCF.WriteReg32( dev, DMABSTRACTAUTO, 0x00000000 ); // Disable Autoexec.
			did_disable_req = 1;

			if( iss->statetag != STTAG( "RDSQ" ) )
			{
				StaticUpdatePROGBUFRegs( dev );
			}

			// Different address, so we don't need to re-write all the program regs.
			// c.lw x8,0(x10) // Get the value to write.
			// c.lw x9,0(x11) // Get the address to write to. 
			MCF.WriteReg32( dev, DMPROGBUF0, 0x41844100 );
			// c.sw x8,0(x9)  // Write to the address.
			// c.addi x9, 4
			MCF.WriteReg32( dev, DMPROGBUF1, 0x0491c080 );
		}

		if( is_flash && iss->target_chip_type == CHIP_CH32V10x)
		{
			// Special 16 bytes buffer write sequence for CH32V103
			MCF.WriteReg32( dev, DMPROGBUF2, 0x0001c184 ); // c.sw x9,0(x11); c.nop;
			MCF.WriteReg32( dev, DMPROGBUF3, 0x9002e391 ); // c.bnez x15, 4; c.ebreak;
			MCF.WriteReg32( dev, DMPROGBUF4, 0x4200c254 ); // c.sw x13,4(x12); c.lw x8,0(x12);
			MCF.WriteReg32( dev, DMPROGBUF5, 0xfc758805 ); // c.andi x8, 1; c.bnez x8, -4;
			MCF.WriteReg32( dev, DMPROGBUF6, 0x90024781 ); // c.li x15, 0; c.ebreak;
		}
		else if( is_flash )
		{
			// A little weird - we need to wait until the buf load is done here to continue.
			// x12 = 0x4002200C (FLASH_STATR)
			//
			// c254 c.sw x13,4(x12) // Acknowledge the page write.  (BUT ONLY ON x035 / v003)
			// /otherwise c.nop
			// 4200 c.lw x8,0(x12)  // Start checking to see when buf load is done.
			// 8809 c.andi x8, 2    // Only look at WR_BSY (seems to be rather undocumented)
			// /8805 c.andi x8, 1    // Only look at BSY if we're not on a v30x / v20x
			// fc75 c.bnez x8, -4
			// c.ebreak
			MCF.WriteReg32( dev, DMPROGBUF2, 0x0001c184 );
			MCF.WriteReg32( dev, DMPROGBUF3, 
				(iss->target_chip_type == CHIP_CH32V003 || iss->target_chip_type == CHIP_CH32V00x
				 || iss->target_chip_type == CHIP_CH32X03x || iss->target_chip_type == CHIP_CH32L103
				 || iss->target_chip_type == CHIP_CH641 || iss->target_chip_type == CHIP_CH643) ?
				0x4200c254 : 0x42000001  );

			MCF.WriteReg32( dev, DMPROGBUF4,
				(iss->target_chip_type == CHIP_CH32V20x || iss->target_chip_type == CHIP_CH32V30x  || iss->target_chip_type == CHIP_CH32H41x) ?
				0xfc758809 : 0xfc758805 );

			MCF.WriteReg32( dev, DMPROGBUF5, 0x90029002 );
		}
		else
		{
			// c.sw x9,0(x11)
			// c.ebreak
			MCF.WriteReg32( dev, DMPROGBUF2, 0x9002c184 );
			// MCF.WriteReg32( dev, DMPROGBUF3, 0x90029002 ); // c.ebreak (nothing needs to be done if not flash)
		}

		MCF.WriteReg32( dev, DMDATA1, address_to_write );
		MCF.WriteReg32( dev, DMDATA0, data );

		if( iss->target_chip->no_autoexec )
		{
			ret |= MCF.WriteReg32( dev, DMCOMMAND, 0x00240000 ); // Execute.
		}
		else if( did_disable_req )
		{
			MCF.WriteReg32( dev, DMCOMMAND, 0x00240000 ); // Execute.
			MCF.WriteReg32( dev, DMABSTRACTAUTO, 1 ); // Enable Autoexec.
		}

		iss->lastwriteflags = is_flash;

		iss->statetag = STTAG( "WRSQ" );
		iss->currentstateval = address_to_write;
	}
	else
	{
		if( address_to_write != iss->currentstateval )
		{
			MCF.WriteReg32( dev, DMDATA1, address_to_write );
		}

		MCF.WriteReg32( dev, DMDATA0, data );

		if( iss->target_chip->no_autoexec )
		{
			ret |= MCF.WriteReg32( dev, DMCOMMAND, 0x00240000 ); // Execute.
		}
	}

	if( is_flash )
		ret |= MCF.WaitForDoneOp( dev, 0 );


	iss->currentstateval += 4;

	return ret;
}

int DefaultWriteBinaryBlob( void * dev, uint32_t address_to_write, uint32_t blob_size, const uint8_t * blob )
{
	// NOTE IF YOU FIX SOMETHING IN THIS FUNCTION PLEASE ALSO UPDATE THE PROGRAMMERS.
	//  this is only fallback functionality for really realy basic programmers.
	//  it is also used in unbrick.

	uint32_t rw;
	struct InternalState * iss = (struct InternalState*)(((struct ProgrammerStructBase*)dev)->internal);

	// We can't write into flash when mapped to 0x00000000
	if( address_to_write < 0x01000000 )
		address_to_write |= 0x08000000;

	int is_flash = IsAddressFlash( address_to_write );

	if( blob_size == 0 ) return 0;

	if( is_flash && !iss->flash_unlocked )
	{
		if( ( rw = InternalUnlockFlash( dev, iss ) ) )
			return rw;
	}

	// Special: For user data, need to write to it very carefully.
	if( address_to_write > 0x1ffff7c0 && address_to_write < 0x20000000 )
	{
		if( !MCF.WriteHalfWord )
		{
			fprintf( stderr, "Error: to write this type of memory, half-word-writing is required\n" );
			return -5;
		}

		uint32_t base = address_to_write & 0xffffffc0;
		uint8_t block[64];

		if( base != ((address_to_write+blob_size-1) & 0xffffffc0) )
		{
			fprintf( stderr, "Error: You cannot write across a 64-byte boundary when writing to option bytes\n" );
			return -9;
		}

		MCF.ReadBinaryBlob( dev, base, 64, block );

		uint32_t offset = address_to_write - base;
		memcpy( block + offset, blob, blob_size );

		uint32_t temp;
		MCF.ReadWord( dev, 0x4002200c, &temp );
		//STATR & BOOT only exists on the 003 and x03x
		// No issue if we force an unlock anyway.
		//if( temp & 0x8000 )
		{
			MCF.WriteWord( dev, 0x40022004, 0x45670123 ); // KEYR
			MCF.WriteWord( dev, 0x40022004, 0xCDEF89AB );

			// These registers are not on or required on the v20x / v30x, but no harm in writing.
			MCF.WriteWord( dev, 0x40022008, 0x45670123 ); // OBWRE
			MCF.WriteWord( dev, 0x40022008, 0xCDEF89AB );
			MCF.WriteWord( dev, 0x40022028, 0x45670123 ); //(FLASH_BOOT_MODEKEYP)
			MCF.WriteWord( dev, 0x40022028, 0xCDEF89AB ); //(FLASH_BOOT_MODEKEYP)
		}

		MCF.ReadWord( dev, 0x4002200c, &temp );
		if( temp & 0x8000 )
		{
			fprintf( stderr, "Error: Critical memory zone is still locked out\n" );
		}
		if( MCF.WaitForFlash ) MCF.WaitForFlash( dev );

		MCF.ReadWord( dev, 0x40022010, &temp );

		if( !(temp & (1<<9)) ) // Check OBWRE
		{
			fprintf( stderr, "Error: Option Byte Unlock Failed (FLASH_CTRL=%08x)\n", temp );
		}

		// Perform erase.
		MCF.WriteWord( dev, 0x40022010, FLASH_CTLR_OPTER | FLASH_CTLR_OPTWRE );
		MCF.WriteWord( dev, 0x40022010, FLASH_CTLR_OPTER | FLASH_CTLR_OPTWRE | FLASH_CTLR_STRT );

		if( MCF.WaitForFlash ) MCF.WaitForFlash( dev );

		MCF.ReadWord( dev, 0x4002200c, &temp );
		if( temp & 0x10 )
		{
			fprintf( stderr, "WRPTRERR is set.  Write failed\n" );
			return -9;
		}

		int i;
		for( i = 0; i < 8; i++ )
		{
			// OBPG = FLASH_CTLR_OPTPG
			MCF.WriteWord( dev, 0x40022010, FLASH_CTLR_OPTPG | FLASH_CTLR_OPTWRE );
			MCF.WriteWord( dev, 0x40022010, FLASH_CTLR_OPTPG | FLASH_CTLR_STRT | FLASH_CTLR_OPTWRE );
			uint32_t writeaddy = i*2+base;
			uint16_t writeword = block[i*2+0] | (block[i*2+1]<<8);
			MCF.WriteHalfWord( dev, writeaddy, writeword );
			if( MCF.WaitForFlash ) MCF.WaitForFlash( dev );
			uint16_t verify = 0;
			MCF.ReadHalfWord( dev, writeaddy, &verify );
			if( verify != writeword )
			{
				fprintf( stderr, "Warning when writing option bytes at %08x, %04x != %04x\n", writeaddy, writeword, verify );
			}
			MCF.ReadWord( dev, 0x4002200c, &temp );
			if( temp & 0x10 )
			{
				fprintf( stderr, "WRPTRERR is set.  Write failed\n" );
				return -9;
			}
		}
		// Turn off OPTPG, OPTWRE.
		MCF.WriteWord( dev, 0x40022010, 0 );
		if( MCF.WaitForFlash ) MCF.WaitForFlash( dev );

		return 0;
	}

	int sectorsize = iss->sector_size;
	int blocks_per_sector = sectorsize / 64;
	int sectorsizemask = sectorsize-1;

	// Regardless of sector size, allow block write to do its thing if it can.
	if( is_flash && MCF.BlockWrite64 && ( address_to_write & sectorsizemask ) == 0 &&
	    ( blob_size & sectorsizemask ) == 0  && iss->target_chip_type != CHIP_CH32V10x )
	{
		int i, j;
		for( i = 0; i < blob_size; )
		{
			for( j = 0; j < blocks_per_sector; j++ )
			{
				// When doing block writes, you MUST write a full sector.
				int r = MCF.BlockWrite64( dev, address_to_write + i, blob + i );
				i += 64;
				if( r )
				{
					fprintf( stderr, "Error writing block at memory %08x / Error: %d\n", address_to_write, r );
					return r;
				}
			}
		}
		return 0;
	}

	uint8_t tempblock[sectorsize];
	int sblock =  address_to_write / sectorsize;
	int eblock = ( address_to_write + blob_size + (sectorsize-1) ) / sectorsize;
	int b;
	int rsofar = 0;

	for( b = sblock; b < eblock; b++ )
	{
		int offset_in_block = address_to_write - (b * sectorsize);
		if( offset_in_block < 0 ) offset_in_block = 0;
		int end_o_plus_one_in_block = ( address_to_write + blob_size ) - (b*sectorsize);
		if( end_o_plus_one_in_block > sectorsize ) end_o_plus_one_in_block = sectorsize;
		int	base = b * sectorsize;

		if( offset_in_block == 0 && end_o_plus_one_in_block == sectorsize )
		{
			if( MCF.BlockWrite64 && iss->target_chip_type != CHIP_CH32V10x)
			{
				int i;
				for( i = 0; i < sectorsize/64; i++ )
				{
					int r = MCF.BlockWrite64( dev, base + i*64, blob + rsofar+i*64 );
					if( r )
					{
						fprintf( stderr, "Error writing block at memory %08x (error = %d)\n", base, r );
						return r;
					}
				}
				rsofar += sectorsize;
			}
			else 					// Block Write not avaialble
			{
				if( is_flash )
				{
					if( !InternalIsMemoryErased( iss, base ) )
						MCF.Erase( dev, base, sectorsize, 0 );
					if( iss->target_chip_type != CHIP_CH32V20x && iss->target_chip_type != CHIP_CH32V30x && iss->target_chip_type != CHIP_CH32H41x )
					{
						// V003, x035, maybe more.
						MCF.WriteWord( dev, 0x40022010, CR_PAGE_PG ); // THIS IS REQUIRED, (intptr_t)&FLASH->CTLR = 0x40022010
						MCF.WriteWord( dev, 0x40022010, CR_BUF_RST | CR_PAGE_PG );  // (intptr_t)&FLASH->CTLR = 0x40022010
					}
					else
					{
						// No bufrst on v20x, v30x
						if( MCF.WaitForFlash ) MCF.WaitForFlash( dev );
						MCF.WriteWord( dev, 0x40022010, CR_PAGE_PG ); // THIS IS REQUIRED, (intptr_t)&FLASH->CTLR = 0x40022010
						//FTPG ==  CR_PAGE_PG   == ((uint32_t)0x00010000)
					}
					if( MCF.WaitForFlash ) MCF.WaitForFlash( dev );
				}

				int j;

				for( j = 0; j < sectorsize/4; j++ )
				{
					uint32_t writeword;
					memcpy( &writeword, blob + rsofar, 4 );
					// WARNING: Just so you know, this is ACTUALLY doing the write AND if writing to flash, doing the following:
					// FLASH->CTLR = CR_PAGE_PG | FLASH_CTLR_BUF_LOAD AFTER it does the write.  THIS IS REQUIRED on the 003.
					if( is_flash && iss->target_chip_type == CHIP_CH32V10x && !((j+1)&3) )
					{
						// Signal to WriteWord that we need to do a buffer load
						MCF.WriteReg32( dev, DMDATA0, 1 );
						MCF.WriteReg32( dev, DMCOMMAND, 0x0023100f );
					}
					MCF.WriteWord( dev, j*4+base, writeword );
					// On the v2xx, v3xx, you also need to make sure FLASH->STATR & 2 is not set.  This is only an issue when running locally.

					rsofar += 4;
				}

				if( is_flash )
				{
					if( iss->target_chip_type == CHIP_CH32V20x || iss->target_chip_type == CHIP_CH32V30x || iss->target_chip_type == CHIP_CH32H41x )
					{
						MCF.WriteWord( dev, 0x40022010, 1<<21 ); // Page Start
					}
					else
					{
						MCF.WriteWord( dev, 0x40022014, base );  //0x40022014 -> FLASH->ADDR
						if( MCF.PrepForLongOp ) MCF.PrepForLongOp( dev );  // Give the programmer a headsup this next operation could take a while.
						MCF.WriteWord( dev, 0x40022010, CR_PAGE_PG|CR_STRT_Set ); // 0x40022010 -> FLASH->CTLR
					}
					if( MCF.WaitForFlash ) MCF.WaitForFlash( dev );
					InternalMarkMemoryNotErased( iss, base );
				}
			}
		}
		else
		{
			//Ok, we have to do something wacky.
			if( is_flash )
			{
				MCF.ReadBinaryBlob( dev, base, sectorsize, tempblock );

				// Permute tempblock
				int tocopy = end_o_plus_one_in_block - offset_in_block;
				memcpy( tempblock + offset_in_block, blob + rsofar, tocopy );
				rsofar += tocopy;

				if( MCF.BlockWrite64 ) 
				{
					int i;
					for( i = 0; i < sectorsize/64; i++ )
					{
						int r = MCF.BlockWrite64( dev, base+i*64, tempblock+i*64 );
						if( r ) return r;
					}
				}
				else
				{
					if( !InternalIsMemoryErased( iss, base ) )
						MCF.Erase( dev, base, sectorsize, 0 );
					if( iss->target_chip_type != CHIP_CH32V20x && iss->target_chip_type != CHIP_CH32V30x && iss->target_chip_type != CHIP_CH32H41x )
					{
						// V003, x035, maybe more.
						MCF.WriteWord( dev, 0x40022010, CR_PAGE_PG ); // THIS IS REQUIRED, (intptr_t)&FLASH->CTLR = 0x40022010
						MCF.WriteWord( dev, 0x40022010, CR_BUF_RST | CR_PAGE_PG );  // (intptr_t)&FLASH->CTLR = 0x40022010
					}
					else
					{
						// No bufrst on v20x, v30x
						if( MCF.WaitForFlash ) MCF.WaitForFlash( dev );
						MCF.WriteWord( dev, 0x40022010, CR_PAGE_PG ); // THIS IS REQUIRED, (intptr_t)&FLASH->CTLR = 0x40022010
						//FTPG ==  CR_PAGE_PG   == ((uint32_t)0x00010000)
					}
					if( MCF.WaitForFlash ) MCF.WaitForFlash( dev );

					int j;
					for( j = 0; j < sectorsize/4; j++ )
					{
						// WARNING: Just so you know, this is ACTUALLY doing the write AND if writing to flash, doing the following:
						// FLASH->CTLR = CR_PAGE_PG | FLASH_CTLR_BUF_LOAD AFTER it does the write.  THIS IS REQUIRED on the 003
						if( iss->target_chip_type == CHIP_CH32V10x && !((j+1)&3) )
						{
							// Signal to WriteWord that we need to do a buffer load
							MCF.WriteReg32( dev, DMDATA0, 1 );
							MCF.WriteReg32( dev, DMCOMMAND, 0x0023100f );
						}
						MCF.WriteWord( dev, j*4+base, *(uint32_t*)(tempblock + j * 4) );
						// On the v2xx, v3xx, you also need to make sure FLASH->STATR & 2 is not set.  This is only an issue when running locally.
					}

					if( iss->target_chip_type == CHIP_CH32V20x || iss->target_chip_type == CHIP_CH32V30x || iss->target_chip_type == CHIP_CH32H41x )
					{
						MCF.WriteWord( dev, 0x40022010, 1<<21 ); // Page Start
					}
					else
					{
						MCF.WriteWord( dev, 0x40022014, base );  //0x40022014 -> FLASH->ADDR
						MCF.WriteWord( dev, 0x40022010, CR_PAGE_PG|CR_STRT_Set ); // 0x40022010 -> FLASH->CTLR
					}
					if( MCF.WaitForFlash ) MCF.WaitForFlash( dev );
					InternalMarkMemoryNotErased( iss, base );
				}
				if( MCF.WaitForFlash && MCF.WaitForFlash( dev ) ) goto timedout;
			}
			else
			{
				// Accessing RAM.  Be careful to only do the needed operations.
				int j;
				for( j = 0; j < sectorsize; j++ )
				{
					uint32_t taddy = j*4;
					if( offset_in_block <= taddy && end_o_plus_one_in_block >= taddy + 4 )
					{
						MCF.WriteWord( dev, taddy + base, *(uint32_t*)(blob + rsofar) );
						rsofar += 4;
					}
					else if( ( offset_in_block & 1 ) || ( end_o_plus_one_in_block & 1 ) )
					{
						// Bytes only.
						int j;
						for( j = 0; j < 4; j++ )
						{
							if( taddy >= offset_in_block && taddy < end_o_plus_one_in_block )
							{
								MCF.WriteByte( dev, taddy + base, *(uint32_t*)(blob + rsofar) );
								rsofar ++;
							}
							taddy++;
						}
					}
					else
					{
						// Half-words
						int j;
						for( j = 0; j < 4; j+=2 )
						{
							if( taddy >= offset_in_block && taddy < end_o_plus_one_in_block )
							{
								MCF.WriteHalfWord( dev, taddy + base, *(uint32_t*)(blob + rsofar) );
								rsofar +=2;
							}
							taddy+=2;
						}
					}
				}
			}
		}
	}

	MCF.FlushLLCommands( dev );

#if 0
	{
		uint8_t scratch[blob_size];
		int rrr = DefaultReadBinaryBlob( dev, address_to_write, blob_size, scratch );
		int i;
		printf( "Read op: %d\n", rrr );
		for( i = 0; i < blob_size; i++ )
		{
			if( scratch[i] != blob[i] )
			{
				printf( "DISAGREE: %04x\n", i );
				i = (i & ~0x3f) + 0x40-1;
			}
		}
	}
#endif

	DefaultDelayUS( dev, 100 ); // Why do we need this? (We seem to need this on the WCH programmers?)
	return 0;
timedout:
	fprintf( stderr, "Timed out\n" );
	return -5;
}

static int DefaultReadWord( void * dev, uint32_t address_to_read, uint32_t * data )
{
	int r = 0;
	struct InternalState * iss = (struct InternalState*)(((struct ProgrammerStructBase*)dev)->internal);

	int autoincrement = 1;
	if( address_to_read == 0x40022010 ||
      address_to_read == 0x4002200C ||
      address_to_read + 4 >= getMemoryEnd(iss->target_chip, iss->current_area))
	{
    // Don't autoincrement when checking flash flag. Or on last byte of memory area.
		autoincrement = 0;
	}

	if( iss->statetag != STTAG( "RDSQ" ) || address_to_read != iss->currentstateval || autoincrement != iss->autoincrement )
	{
		if( iss->statetag != STTAG( "RDSQ" ) || !autoincrement )
		{
			if( iss->statetag != STTAG( "WRSQ" ) )
			{
				StaticUpdatePROGBUFRegs( dev );
			}

			MCF.WriteReg32( dev, DMABSTRACTAUTO, 0 ); // Disable Autoexec.

			// c.lw x8,0(x11) // Pull the address from DATA1
			// c.lw x9,0(x8)  // Read the data at that location.
			MCF.WriteReg32( dev, DMPROGBUF0, 0x40044180 );

			if( autoincrement )
			{
				// c.addi x8, 4
				// c.sw x9, 0(x10) // Write back to DATA0

				MCF.WriteReg32( dev, DMPROGBUF1, 0xc1040411 );
			}
			else
			{
				// c.nop
				// c.sw x9, 0(x10) // Write back to DATA0

				MCF.WriteReg32( dev, DMPROGBUF1, 0xc1040001 );
			}
			// c.sw x8, 0(x11) // Write addy to DATA1
			// c.ebreak
			MCF.WriteReg32( dev, DMPROGBUF2, 0x9002c180 );

			if( !iss->target_chip->no_autoexec )
				MCF.WriteReg32( dev, DMABSTRACTAUTO, 1 ); // Enable Autoexec (different kind of autoinc than outer autoinc)

			iss->autoincrement = autoincrement;
		}

		MCF.WriteReg32( dev, DMDATA1, address_to_read );
		if( !iss->target_chip->no_autoexec )
			MCF.WriteReg32( dev, DMCOMMAND, 0x00240000 );

		iss->statetag = STTAG( "RDSQ" );
		iss->currentstateval = address_to_read;

		MCF.WaitForDoneOp( dev, 1 );

		if( r ) fprintf( stderr, "Fault on DefaultReadWord Part 1\n" );
	}

	if( iss->autoincrement )
		iss->currentstateval += 4;

	// If you were running locally, you might need to do this.
	//MCF.WaitForDoneOp( dev, 1 );

	if( iss->target_chip->no_autoexec ) {
		MCF.WriteReg32( dev, DMCOMMAND, 0x00240000 );
	}
	r |= MCF.ReadReg32( dev, DMDATA0, data );

	if( iss->currentstateval == iss->ram_base + iss->ram_size )
		MCF.WaitForDoneOp( dev, 1 ); // Ignore any post-errors. 
	return r;
}

int InternalUnlockFlash( void * dev, struct InternalState * iss )
{
	int ret = 0;
	uint32_t rw;

	ret = MCF.ReadWord( dev, 0x40022010, &rw );  // FLASH->CTLR = 0x40022010
	if( rw & 0x8080 ) 
	{
		ret = MCF.WriteWord( dev, 0x40022004, 0x45670123 ); // FLASH->KEYR = 0x40022004
		if( ret ) goto reterr;
		ret = MCF.WriteWord( dev, 0x40022004, 0xCDEF89AB );
		if( ret ) goto reterr;
		ret = MCF.WriteWord( dev, 0x40022008, 0x45670123 ); // OBKEYR = 0x40022008  // For user word unlocking
		if( ret ) goto reterr;
		ret = MCF.WriteWord( dev, 0x40022008, 0xCDEF89AB );
		if( ret ) goto reterr;
		ret = MCF.WriteWord( dev, 0x40022024, 0x45670123 ); // MODEKEYR = 0x40022024
		if( ret ) goto reterr;
		ret = MCF.WriteWord( dev, 0x40022024, 0xCDEF89AB );
		if( ret ) goto reterr;

		ret = MCF.ReadWord( dev, 0x40022010, &rw ); // FLASH->CTLR = 0x40022010
		if( ret ) goto reterr;

		if( rw & 0x8080 ) 
		{
			fprintf( stderr, "Error: Flash is not unlocked (CTLR = %08x)\n", rw );
			return -9;
		}
	}

	MCF.ReadWord( dev, 0x4002201c, &rw ); //(FLASH_OBTKEYR)
	if( rw & 2 )
	{
		fprintf( stderr, "WARNING: Your part appears to have flash [read] locked.  Cannot program unless unlocked.\n" );
	}

	iss->flash_unlocked = 1;
	return 0;
reterr:
	fprintf( stderr, "Error unlocking flash, got code %d from underlying system\n", ret );
	return ret;
}

int DefaultErase( void * dev, uint32_t address, uint32_t length, int type )
{
	struct InternalState * iss = (struct InternalState*)(((struct ProgrammerStructBase*)dev)->internal);
	uint32_t rw;

	if( !iss->flash_unlocked )
	{
		if( ( rw = InternalUnlockFlash( dev, iss ) ) )
			return rw;
	}

	/* Workaround for Ch32H41x differently sized (but very fast) erase */
	if( type == 1 || iss->target_chip_type == CHIP_CH32H41x )
	{
		// Whole-chip flash
		iss->statetag = STTAG( "XXXX" );
		printf( "Whole-chip erase\n" );
		if( MCF.WriteWord( dev, (intptr_t)&FLASH->CTLR, 0 ) ) goto flashoperr;
		if( MCF.WriteWord( dev, (intptr_t)&FLASH->CTLR, FLASH_CTLR_MER  ) ) goto flashoperr;
		if( MCF.PrepForLongOp ) MCF.PrepForLongOp( dev );  // Give the programmer a headsup this next operation could take a while.
		if( MCF.WriteWord( dev, (intptr_t)&FLASH->CTLR, CR_STRT_Set|FLASH_CTLR_MER ) ) goto flashoperr;
		rw = MCF.WaitForDoneOp( dev, 0 );
		if( MCF.WaitForFlash && MCF.WaitForFlash( dev ) ) { fprintf( stderr, "Error: Wait for flash error.\n" ); return -11; }
		MCF.VoidHighLevelState( dev );
		memset( iss->flash_sector_status, 1, sizeof( iss->flash_sector_status ) );
	}
	else
	{
		// 16.4.7, Step 3: Check the BSY bit of the FLASH_STATR register to confirm that there are no other programming operations in progress.
		// skip (we make sure at the end)
		int chunk_to_erase = address;

		chunk_to_erase = chunk_to_erase & ~(iss->sector_size-1);
		while( chunk_to_erase < address + length )
		{
			if( ( chunk_to_erase & 0xff000000 ) == 0x08000000 )
			{
				int sector = ( chunk_to_erase & 0x00ffffff ) / iss->sector_size;
				if( sector < MAX_FLASH_SECTORS )
				{
					iss->flash_sector_status[sector] = 1;
				}
			}

			// Step 4:  set PAGE_ER of FLASH_CTLR(0x40022010)
			if( MCF.WriteWord( dev, (intptr_t)&FLASH->CTLR, CR_PAGE_ER ) ) goto flashoperr; // CR_PAGE_ER is FTER
			// Step 5: Write the first address of the fast erase page to the FLASH_ADDR register.
			if( MCF.WriteWord( dev, (intptr_t)&FLASH->ADDR, chunk_to_erase ) ) goto flashoperr;
			if( MCF.PrepForLongOp ) MCF.PrepForLongOp( dev );  // Give the programmer a headsup this next operation could take a while.

			// Step 6: Set the STAT/STRT bit of FLASH_CTLR register to '1' to initiate a fast page erase (64 bytes) action.
			if( MCF.WriteWord( dev, (intptr_t)&FLASH->CTLR, (1<<6) | CR_PAGE_ER ) ) goto flashoperr;

			if( MCF.WaitForFlash && MCF.WaitForFlash( dev ) ) return -99;

			chunk_to_erase+=iss->sector_size;
		}
	}

	return 0;
flashoperr:
	fprintf( stderr, "Error: Flash operation error\n" );
	return -93;
}

static int DefaultSetSplit(void * dev, enum RAMSplit split) {

	uint8_t split_code = 0;
	uint16_t option_bytes = 0;
	uint32_t flash_ctlr = 0;
	struct InternalState * iss = (struct InternalState*)(((struct ProgrammerStructBase*)dev)->internal);


	// If the progammer already read the chip_id, use it
	// Otherwise read it off the chip
	uint32_t chip_id = iss->target_chip_id;
	if (!chip_id) {
		if( MCF.ReadWord( dev, (intptr_t)&INFO->CHIPID, &chip_id ) ) goto flashoperr;
	}

	uint32_t chip = chip_id & 0xFFFFFF0F;

	/* List of ChipIDs
	* Values taken from: ch32v307/EVT/EXAM/SRC/Peripheral/src/ch32v30x_dbgmcu.c:108
	* CH32V303CBT6: 0x303305x4
	* CH32V303RBT6: 0x303205x4
	* CH32V303RCT6: 0x303105x4
	* CH32V303VCT6: 0x303005x4
	* CH32V305FBP6: 0x305205x8
	* CH32V305RBT6: 0x305005x8
	* CH32V305GBU6: 0x305B05x8
	* CH32V307WCU6: 0x307305x8
	* CH32V307FBP6: 0x307205x8
	* CH32V307RCT6: 0x307105x8
	* CH32V307VCT6: 0x307005x8
	* CH32V317VCT6: 0x3170B5X8
	* CH32V317WCU6: 0x3173B5X8
	* CH32V317TCU6: 0x3175B5X8
	*/

	switch (split)
	{
		case FLASH_192_RAM_128:
			if (chip == 0x30700508 
			 || chip == 0x30710508 
			 || chip == 0x30730508
			 || chip == 0x30300504
			 || chip == 0x30310504
			 || chip == 0x30720508
			 || chip == 0x30740508) {
				split_code = 0;
			} else {
				fprintf( stderr, "Error, 192k/128k split not supported for chip 0x%08x\n", chip);
				exit( -110 );
			}
			break;
		
		case FLASH_224_RAM_96:
			if (chip == 0x30700508 
			 || chip == 0x30710508 
			 || chip == 0x30730508
			 || chip == 0x30300504
			 || chip == 0x30310504
			 || chip == 0x30720508
			 || chip == 0x30740508) {
				split_code = 1;
			} else {
				fprintf( stderr, "Error, 224k/96k split not supported for chip 0x%08x\n", chip);
				exit( -110 );
			}
			break;
		
		case FLASH_256_RAM_64:
			if (chip == 0x30700508 
			 || chip == 0x30710508 
			 || chip == 0x30730508
			 || chip == 0x30300504
			 || chip == 0x30310504) {
				split_code = 2;
			} else {
				fprintf( stderr, "Error, 256k/64k split not supported for chip 0x%08x\n", chip);
				exit( -110 );
			}
			break;

		case FLASH_288_RAM_32:
			if (chip == 0x30700508 
			 || chip == 0x30710508 
			 || chip == 0x30730508
			 || chip == 0x30300504
			 || chip == 0x30310504) {
				split_code = 3;
			} else {
				fprintf( stderr, "Error, 288k/32k split not supported for chip 0x%08x\n", chip);
				exit( -110 );
			}
			break;

		case FLASH_128_RAM_64:
			if (chip == 0x2034050c
			 || chip == 0x2080050c
			 || chip == 0x2081050c
			 || chip == 0x2082050c
			 || chip == 0x2083050c) {
				split_code = 0;
			} else {
				fprintf( stderr, "Error, 128k/64k split not supported for chip 0x%08x\n", chip);
				exit( -110 );
			}
			break;

		case FLASH_144_RAM_48:
			if (chip == 0x2034050c
			 || chip == 0x2080050c
			 || chip == 0x2081050c
			 || chip == 0x2082050c
			 || chip == 0x2083050c) {
				split_code = 1;
			} else {
				fprintf( stderr, "Error, 144k/48k split not supported for chip 0x%08x\n", chip);
				exit( -110 );
			}
			break;

		case FLASH_160_RAM_32:
			if (chip == 0x2034050c
			 || chip == 0x2080050c
			 || chip == 0x2081050c
			 || chip == 0x2082050c
			 || chip == 0x2083050c) {
				split_code = 2;
			} else {
				fprintf( stderr, "Error, 160k/32k split not supported for chip 0x%08x\n", chip);
				exit( -110 );
			}
			break;

		default:
			fprintf( stderr, "Error: chip 0x%08x does not support a configurable RAM split\n", chip);
			return -110;
			
	}

	if( !MCF.WriteHalfWord || !MCF.ReadHalfWord)
	{
		fprintf( stderr, "Error: for setting ram split option bytes, half-word read and write is required\n" );
		return -5;
	}

	if( MCF.ReadHalfWord( dev, (intptr_t)&OB->USER, &option_bytes ) ) goto flashoperr;
	printf("initial option_bytes = %04x\n", option_bytes);


	// Option byte b is stored as 16 bits (~b << 8)|b
	// Mask off upper copy and clear split bits
	option_bytes &= 0x003F;
	// Set split code at [7:6]
	option_bytes |= split_code << 6;
	// Add inverted copy back as high byte
	option_bytes |= (~option_bytes) << 8;

	InternalUnlockFlash(dev, iss);

	if( MCF.ReadWord( dev, (intptr_t)&FLASH->CTLR, &flash_ctlr ) ) goto flashoperr;
	flash_ctlr |= CR_OPTER_Set;
	if( MCF.WriteWord( dev, (intptr_t)&FLASH->CTLR, flash_ctlr ) ) goto flashoperr;
	flash_ctlr |= CR_STRT_Set;
	if( MCF.WriteWord( dev, (intptr_t)&FLASH->CTLR, flash_ctlr ) ) goto flashoperr;
	if( MCF.WaitForFlash(dev) ) goto flashoperr;

	if( MCF.ReadWord( dev, (intptr_t)&FLASH->CTLR, &flash_ctlr ) ) goto flashoperr;
	flash_ctlr &= CR_OPTER_Reset;
	flash_ctlr |= CR_OPTPG_Set;
	if( MCF.WriteWord( dev, (intptr_t)&FLASH->CTLR, flash_ctlr ) ) goto flashoperr;
	if( MCF.WriteWord( dev, (intptr_t)&OB->RDPR, RDP_Key ) ) goto flashoperr;
	if( MCF.WaitForFlash(dev) ) goto flashoperr;

	if( MCF.WriteWord( dev, (intptr_t)&FLASH->OBKEYR, FLASH_KEY1 ) ) goto flashoperr;
	if( MCF.WriteWord( dev, (intptr_t)&FLASH->OBKEYR, FLASH_KEY2 ) ) goto flashoperr;
	if( MCF.WaitForFlash(dev) ) goto flashoperr;

	if( MCF.ReadWord( dev, (intptr_t)&FLASH->CTLR, &flash_ctlr ) ) goto flashoperr;
	flash_ctlr |= CR_OPTPG_Set;
	if( MCF.WriteWord( dev, (intptr_t)&FLASH->CTLR, flash_ctlr ) ) goto flashoperr;
	if( MCF.WriteHalfWord( dev, (intptr_t)&OB->USER, option_bytes ) ) goto flashoperr;
	if( MCF.WaitForFlash(dev) ) goto flashoperr;

	flash_ctlr &= CR_OPTPG_Reset;
	if( MCF.WriteWord( dev, (intptr_t)&FLASH->CTLR, flash_ctlr ) ) goto flashoperr;
	if( MCF.WaitForFlash(dev) ) goto flashoperr;

	return 0;
flashoperr:
	fprintf( stderr, "Error: Flash operation error\n" );
	return -93;
}

int DefaultReadBinaryBlob( void * dev, uint32_t address_to_read_from, uint32_t read_size, uint8_t * blob )
{
	struct InternalState * iss = (struct InternalState*)(((struct ProgrammerStructBase*)dev)->internal);
	int ret = 0;
	if( iss->target_chip == NULL)
	{
		ret = MCF.DetermineChipType( dev );
		if( ret ) return ret;
	}
	if( iss->current_area == 0 ) DetectMemoryArea( dev, address_to_read_from );

	uint32_t rpos = address_to_read_from;
	uint32_t rend = address_to_read_from + read_size;

	while( rpos < rend )
	{
		int r;
		int remain = rend - rpos;
		if( ( rpos & 3 ) == 0 && remain >= 4 )
		{
			uint32_t rw;
			r = MCF.ReadWord( dev, rpos, &rw );
			if( r ) return r;
			int rem = remain;
			if( rem > 4 ) rem = 4;
			memcpy( blob, &rw, rem);
			blob += 4;
			rpos += 4;
		}
		else
		{
			if( remain >= 1 )
			{
				uint8_t rw;
				r = MCF.ReadByte( dev, rpos, &rw );
				if( r ) return r;
				memcpy( blob, &rw, 1 );
				blob += 1;
				rpos += 1;
				remain -= 1;
			}
			if( ( rpos & 1 ) && remain >= 1 )
			{
				uint8_t rw;
				r = MCF.ReadByte( dev, rpos, &rw );
				if( r ) return r;
				memcpy( blob, &rw, 1 );
				blob += 1;
				rpos += 1;
				remain -= 1;
			}
			if( ( rpos & 2 ) && remain >= 2 )
			{
				uint16_t rw;
				r = MCF.ReadHalfWord( dev, rpos, &rw );
				if( r ) return r;
				memcpy( blob, &rw, 2 );
				blob += 2;
				rpos += 2;
				remain -= 2;
			}
		}
	}

	int r = MCF.WaitForDoneOp( dev, 0 );
	if( r ) fprintf( stderr, "Fault on DefaultReadBinaryBlob, on byte %08x\n", rpos );
	return r;
}

int DefaultReadCPURegister( void * dev, uint32_t regno, uint32_t * regret )
{
	if( !MCF.WriteReg32 || !MCF.ReadReg32 )
	{
		fprintf( stderr, "Error: Can't read CPU register on this programmer because it is missing read/writereg32\n" );
		return -5;
	}

	struct InternalState * iss = (struct InternalState*)(((struct ProgrammerStructBase*)dev)->internal);
	MCF.WriteReg32( dev, DMABSTRACTAUTO, 0x00000000 ); // Disable Autoexec.
	iss->statetag = STTAG( "REGR" );

	MCF.WriteReg32( dev, DMCOMMAND, 0x00220000 | regno ); // Read xN into DATA0.
	int r = MCF.ReadReg32( dev, DMDATA0, regret );

	return r;
}

int DefaultReadAllCPURegisters( void * dev, uint32_t * regret )
{
	struct InternalState * iss = (struct InternalState*)(((struct ProgrammerStructBase*)dev)->internal);
	MCF.WriteReg32( dev, DMABSTRACTAUTO, 0x00000000 ); // Disable Autoexec.
	iss->statetag = STTAG( "RER2" );
	int i;
	for( i = 0; i < iss->nr_registers_for_debug; i++ )
	{
		MCF.WriteReg32( dev, DMCOMMAND, 0x00220000 | 0x1000 | i ); // Read xN into DATA0.
		if( MCF.ReadReg32( dev, DMDATA0, regret + i ) )
		{
			return -5;
		}
	}
	MCF.WriteReg32( dev, DMCOMMAND, 0x00220000 | 0x7b1 ); // Read xN into DATA0.
	int r = MCF.ReadReg32( dev, DMDATA0, regret + i );
	return r;
}

int DefaultWriteAllCPURegisters( void * dev, uint32_t * regret )
{
	struct InternalState * iss = (struct InternalState*)(((struct ProgrammerStructBase*)dev)->internal);
	MCF.WriteReg32( dev, DMABSTRACTAUTO, 0x00000000 ); // Disable Autoexec.
	iss->statetag = STTAG( "WER2" );
	int i;
	for( i = 0; i < iss->nr_registers_for_debug; i++ )
	{
		if( MCF.WriteReg32( dev, DMDATA0, regret[i] ) )
		{
			return -5;
		}
		MCF.WriteReg32( dev, DMCOMMAND, 0x00230000 | 0x1000 | i ); // Read xN into DATA0.
	}
	int r = MCF.WriteReg32( dev, DMDATA0, regret[i] );
	MCF.WriteReg32( dev, DMCOMMAND, 0x00230000 | 0x7b1 ); // Read xN into DATA0.
	return r;
}


int DefaultWriteCPURegister( void * dev, uint32_t regno, uint32_t value )
{
	if( !MCF.WriteReg32 || !MCF.ReadReg32 )
	{
		fprintf( stderr, "Error: Can't read CPU register on this programmer because it is missing read/writereg32\n" );
		return -5;
	}

	struct InternalState * iss = (struct InternalState*)(((struct ProgrammerStructBase*)dev)->internal);
	MCF.WriteReg32( dev, DMABSTRACTAUTO, 0x00000000 ); // Disable Autoexec.
	iss->statetag = STTAG( "REGW" );
	MCF.WriteReg32( dev, DMDATA0, value );
	return MCF.WriteReg32( dev, DMCOMMAND, 0x00230000 | regno ); // Write xN from DATA0.
}

int DefaultSetEnableBreakpoints( void * dev, int is_enabled, int single_step )
{
	if( !MCF.ReadCPURegister || !MCF.WriteCPURegister )
	{
		fprintf( stderr, "Error: Can't set breakpoints on this programmer because it is missing read/writereg32\n" );
		return -5;
	}
	uint32_t DCSR;
	if( MCF.ReadCPURegister( dev, 0x7b0, &DCSR ) )
		fprintf( stderr, "Error: DCSR could not be read\n" );
	DCSR |= 0xb600;
	if( single_step )
		DCSR |= 4;
	else
		DCSR &=~4;

	if( MCF.WriteCPURegister( dev, 0x7b0, DCSR ) )
		fprintf( stderr, "Error: DCSR could not be read\n" );

	return 0;
}

static int DefaultHaltMode( void * dev, int mode )
{
	struct InternalState * iss = (struct InternalState*)(((struct ProgrammerStructBase*)dev)->internal);
	switch ( mode )
	{
	case HALT_MODE_HALT_BUT_NO_RESET: // Don't reboot.
	case HALT_MODE_HALT_AND_RESET:
		MCF.WriteReg32( dev, DMSHDWCFGR, 0x5aa50000 | (1<<10) ); // Shadow Config Reg
		MCF.WriteReg32( dev, DMCFGR, 0x5aa50000 | (1<<10) ); // CFGR (1<<10 == Allow output from slave)
		MCF.WriteReg32( dev, DMCONTROL, 0x80000001 ); // Make the debug module work properly.
		if( mode == HALT_MODE_HALT_AND_RESET ) MCF.WriteReg32( dev, DMCONTROL, 0x80000003 ); // Reboot.
		MCF.WriteReg32( dev, DMCONTROL, 0x80000001 ); // Re-initiate a halt request.
		MCF.WriteReg32( dev, DMCONTROL, 0x80000001 ); // Re-initiate a halt request.
		// MCF.WriteReg32( dev, DMCONTROL, 0x00000001 ); // Clear Halt Request.  This is recommended, but not doing it seems more stable.
		// Sometimes, even if the processor is halted but the MSB is clear, it will spuriously start?
		MCF.FlushLLCommands( dev );
		iss->clock_set = 0;
		break;
	case HALT_MODE_REBOOT:
		MCF.WriteReg32( dev, DMCONTROL, 0x80000001 ); // Make the debug module work properly.
		MCF.WriteReg32( dev, DMCONTROL, 0x80000001 ); // Initiate a halt request.
		MCF.WriteReg32( dev, DMCONTROL, 0x80000003 ); // Reboot.
		MCF.WriteReg32( dev, DMCONTROL, 0x40000001 ); // resumereq
		MCF.FlushLLCommands( dev );
		iss->clock_set = 0;
		break;
	case HALT_MODE_RESUME:
		MCF.WriteReg32( dev, DMSHDWCFGR, 0x5aa50000 | (1<<10) ); // Shadow Config Reg
		MCF.WriteReg32( dev, DMCFGR, 0x5aa50000 | (1<<10) ); // CFGR (1<<10 == Allow output from slave)

		MCF.WriteReg32( dev, DMCONTROL, 0x40000001 ); // resumereq
		MCF.FlushLLCommands( dev );
		break;
	case HALT_MODE_GO_TO_BOOTLOADER:
		MCF.WriteReg32( dev, DMCONTROL, 0x80000001 ); // Make the debug module work properly.
		MCF.WriteReg32( dev, DMCONTROL, 0x80000001 ); // Initiate a halt request.

		MCF.WriteWord( dev, (intptr_t)&FLASH->KEYR, FLASH_KEY1 );
		MCF.WriteWord( dev, (intptr_t)&FLASH->KEYR, FLASH_KEY2 );
		MCF.WriteWord( dev, (intptr_t)&FLASH->BOOT_MODEKEYR, FLASH_KEY1 );
		MCF.WriteWord( dev, (intptr_t)&FLASH->BOOT_MODEKEYR, FLASH_KEY2 );
		MCF.WriteWord( dev, (intptr_t)&FLASH->STATR, 1<<14 );
		MCF.WriteWord( dev, (intptr_t)&FLASH->CTLR, CR_LOCK_Set );

		MCF.WriteReg32( dev, DMCONTROL, 0x80000003 ); // Reboot.
		MCF.WriteReg32( dev, DMCONTROL, 0x40000001 ); // resumereq
		MCF.FlushLLCommands( dev );
		break;
	default:
		fprintf( stderr, "Error: Unknown halt mode %d\n", mode );
	}

	iss->flash_unlocked = 0;
	iss->processor_in_mode = mode;

	// In case processor halt process needs to complete, i.e. if it was in the middle of a flash op.
  DefaultDelayUS( dev, 10000 );

	return 0;
}

// Returns positive if received text, or request for input.
// Returns -1 if nothing was printed but received data.
// Returns negative if error.
// Returns 0 if no text waiting.
// maxlen MUST be at least 8 characters.  We null terminate.
int DefaultPollTerminal( void * dev, uint8_t * buffer, int maxlen, uint32_t leaveflagA, int leaveflagB )
{
	struct InternalState * iss = (struct InternalState*)(((struct ProgrammerStructBase*)dev)->internal);
	int r;
	uint32_t rr;
	if( iss->statetag != STTAG( "TERM" ) )
	{
		MCF.WriteReg32( dev, DMABSTRACTAUTO, 0x00000000 ); // Disable Autoexec.
		iss->statetag = STTAG( "TERM" );
	}
	r = MCF.ReadReg32( dev, DMDATA0, &rr );

	if( r < 0 ) return r;
	if( maxlen < 8 ) return -9;

	// DMDATA1:
	//  bit  7 = host-acknowledge.
	if( rr & 0x80 )
	{
		int num_printf_chars = (rr & 0xf)-4;
		if( num_printf_chars > 0 && num_printf_chars <= 7)
		{
			if( num_printf_chars > 3 )
			{
				uint32_t r2;
				r = MCF.ReadReg32( dev, DMDATA1, &r2 );
				memcpy( buffer+3, &r2, num_printf_chars - 3 );
			}
			int firstrem = num_printf_chars;
			if( firstrem > 3 ) firstrem = 3;
			memcpy( buffer, ((uint8_t*)&rr)+1, firstrem );
			buffer[num_printf_chars] = 0;
		}
		if( leaveflagA ) MCF.WriteReg32( dev, DMDATA1, leaveflagB );
		MCF.WriteReg32( dev, DMDATA0, leaveflagA ); // Write that we acknowledge the data.
		if( num_printf_chars <= 0 ) return num_printf_chars-1;      // was acked (or other error code)
		return num_printf_chars;
	}
	else
	{
		return 0;
	}
}

int DefaultUnbrick( void * dev )
{
	printf( "Entering Unbrick Mode\n" );
	if( MCF.Control5v ) MCF.Control5v( dev, 0 );
	MCF.Control3v3( dev, 0 );

	MCF.DelayUS( dev, 60000 );
	MCF.DelayUS( dev, 60000 );
	MCF.DelayUS( dev, 60000 );
	MCF.DelayUS( dev, 60000 );
	MCF.DelayUS( dev, 60000 );
	MCF.FlushLLCommands( dev );
	if( MCF.ResetInterface ) MCF.ResetInterface( dev );
	if( MCF.Control5v ) MCF.Control5v( dev, 1 );
	MCF.Control3v3( dev, 1 );
	printf( "Connection starting\n" );

	int timeout = 0;
	int max_timeout = 50000; // An absurdly long time.
	uint32_t ds = 0;
	for( timeout = 0; timeout < max_timeout; timeout++ )
	{
		MCF.DelayUS( dev, 10 );
		MCF.WriteReg32( dev, DMSHDWCFGR, 0x5aa50000 | (1<<10) ); // Shadow Config Reg
		MCF.WriteReg32( dev, DMCFGR, 0x5aa50000 | (1<<10) ); // CFGR (1<<10 == Allow output from slave)
		MCF.WriteReg32( dev, DMCONTROL, 0x80000001 ); // Make the debug module work properly.
		MCF.WriteReg32( dev, DMCONTROL, 0x80000001 ); // Initiate a halt request.
		MCF.WriteReg32( dev, DMCONTROL, 0x80000001 ); // No, really make sure.
		MCF.WriteReg32( dev, DMCONTROL, 0x80000001 );
		MCF.FlushLLCommands( dev );
		int r = MCF.ReadReg32( dev, DMSTATUS, &ds );
		if( r )
		{
			fprintf( stderr, "Error: Could not read DMSTATUS from programmers (%d)\n", r );
			return -99;
		}
		MCF.FlushLLCommands( dev );
		if( ds != 0xffffffff && ds != 0x00000000 ) break;
		else fprintf( stderr, "%08x\n", ds );
	}

	if( timeout == max_timeout ) 
	{
		fprintf( stderr, "Timed out trying to unbrick\n" );
		return -5;
	}
	
	int i;
	for( i = 0; i < 10; i++ )
	{
		// Make sure we are in halt.
		MCF.WriteReg32( dev, DMCONTROL, 0x80000001 ); // Make the debug module work properly.
		MCF.WriteReg32( dev, DMCONTROL, 0x80000001 ); // Initiate a halt request.
		MCF.WriteReg32( dev, DMCONTROL, 0x80000001 ); // No, really make sure.
		MCF.WriteReg32( dev, DMCONTROL, 0x80000001 );
		
		// After more experimentation, it appaers to work best by not clearing the halt request.
		MCF.FlushLLCommands( dev );
	}

	MCF.WriteReg32( dev, DMABSTRACTCS, 0x00000700 ); // Clear out possible abstractcs errors.

	int r = MCF.ReadReg32( dev, DMSTATUS, &ds );
	printf( "DMStatus After Halt: /%d/%08x\n", r, ds );

	r = DefaultDetermineChipType( dev );
	if( r ) return r;
	struct InternalState * iss = (struct InternalState*)(((struct ProgrammerStructBase*)dev)->internal);
	printf( "Chip Type: %s\n", iss->target_chip->name_str );

	if( iss->target_chip->protocol == PROTOCOL_DEFAULT )
	{
		// Override all option bytes and reset to factory settings, unlocking all flash sections.
		static const uint8_t option_data_003_x03x[] = { 0xa5, 0x5a, 0x97, 0x68, 0x00, 0xff, 0x00, 0xff, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00 };
		static const uint8_t option_data_20x_30x[]  = { 0xa5, 0x5a, 0x3f, 0xc0, 0x00, 0xff, 0x00, 0xff, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00 };

		InternalUnlockFlash(dev, iss);

		const uint8_t * option_data = 
			(iss->target_chip_type == CHIP_CH32V003 || iss->target_chip_type == CHIP_CH32V00x
			|| iss->target_chip_type == CHIP_CH32X03x || iss->target_chip_type == CHIP_CH32L103
			|| iss->target_chip_type == CHIP_CH641 || iss->target_chip_type == CHIP_CH643) ?
			option_data_003_x03x : option_data_20x_30x;

		DefaultWriteBinaryBlob(dev, 0x1ffff800, 16, option_data );

		MCF.DelayUS( dev, 20000 );

		MCF.Erase( dev, 0, 0, 1);
		MCF.FlushLLCommands( dev );
	}
	else if ( iss->target_chip->protocol == PROTOCOL_CH5xx )
	{
		CH5xxErase( dev, 0, 0, 1 );
	}
	return -5;
}

int DefaultConfigureNRSTAsGPIO( void * dev, int one_if_yes_gpio )
{
	fprintf( stderr, "Error: DefaultConfigureNRSTAsGPIO does not work via the programmer here.  Please see the demo \"optionbytes\"\n" );
	return -5;
}

int DefaultConfigureReadProtection( void * dev, int one_if_yes_protect )
{
	struct InternalState * iss = (struct InternalState*)(((struct ProgrammerStructBase*)dev)->internal);
	if( iss->target_chip_type == CHIP_CH570 && one_if_yes_protect == 0 )
	{
		fprintf( stderr, "Disabling protection on CH570/2\n" );
		ch570_disable_read_protection( dev );
	}
	else
	{
		fprintf( stderr, "Error: DefaultConfigureReadProtection does not work via the programmer here.  Please see the demo \"optionbytes\"\n" );
		return -5;
	}
	return 0;
}

int DefaultEnableDebug( void * dev, uint8_t disable )
{
	fprintf( stderr, "Error: This is only possibe using ISP mode on some CH5xx chips.\n" );
	return -5;
}

int DefaultPrintChipInfo( void * dev )
{
	struct InternalState * iss = (struct InternalState*)(((struct ProgrammerStructBase*)dev)->internal);
	int ret = 0;
	if( iss->target_chip == NULL)
	{
		ret = MCF.DetermineChipType( dev );
		if( ret ) return ret;
	}

	uint32_t reg;
	MCF.HaltMode( dev, HALT_MODE_HALT_BUT_NO_RESET );
	if( iss->target_chip->protocol == PROTOCOL_DEFAULT )
	{
		if( MCF.ReadWord( dev, 0x1FFFF800, &reg ) ) goto fail;
		printf( "USER/RDPR  : %04x/%04x\n", reg>>16, reg&0xFFFF );
		if( MCF.ReadWord( dev, 0x1FFFF804, &reg ) ) goto fail;
		printf( "DATA1/DATA0: %04x/%04x\n", reg>>16, reg&0xFFFF );
		if( MCF.ReadWord( dev, 0x1FFFF808, &reg ) ) goto fail;
		printf( "WRPR1/WRPR0: %04x/%04x\n", reg>>16, reg&0xFFFF );
		if( MCF.ReadWord( dev, 0x1FFFF80c, &reg ) ) goto fail;
		printf( "WRPR3/WRPR2: %04x/%04x\n", reg>>16, reg&0xFFFF );
		// if( MCF.ReadWord( dev, 0x1FFFF7E0, &reg ) ) goto fail;
		// printf( "Flash Size: %d kB\n", (reg&0xffff) );
		if( MCF.ReadWord( dev, 0x1FFFF7E8, &reg ) ) goto fail;
		printf( "R32_ESIG_UNIID1: %08x\n", reg );
		if( MCF.ReadWord( dev, 0x1FFFF7EC, &reg ) ) goto fail;
		printf( "R32_ESIG_UNIID2: %08x\n", reg );
		if( MCF.ReadWord( dev, 0x1FFFF7F0, &reg ) ) goto fail;
		printf( "R32_ESIG_UNIID3: %08x\n", reg );
		return 0;
	}
	else if( iss->target_chip->protocol == PROTOCOL_CH5xx )
	{
		if ( CH5xxPrintInfo( dev ) ) goto fail;
		return 0;
	}
fail:
	fprintf( stderr, "Error: Failed to get chip details\n");
	return -11;
}

int DefaultVoidHighLevelState( void * dev )
{
	struct InternalState * iss = (struct InternalState*)(((struct ProgrammerStructBase*)dev)->internal);
	iss->statetag = STTAG( "VOID" );
	return 0;
}

int DefaultDelayUS( void * dev, int us )
{
#if defined(WINDOWS) || defined(WIN32) || defined(_WIN32)
	Sleep( (us+9999) / 1000 );
#else
	usleep( us );
#endif
	return 0;
}

static int DefaultSetClock( void * dev, uint32_t clock )
{
  fprintf( stderr, "Will set clock here, when implemented\n" );
  return 0;
}

int SetupAutomaticHighLevelFunctions( void * dev )
{
	// Will populate high-level functions from low-level functions.
	if( MCF.WriteReg32 == 0 && MCF.ReadReg32 == 0 && MCF.WriteWord == 0 ) return -5;

	// Else, TODO: Build the high level functions from low level functions.
	// If a high-level function alrady exists, don't override.

	if( !MCF.SetupInterface )
		MCF.SetupInterface = DefaultSetupInterface;
	if( !MCF.DetermineChipType )
		MCF.DetermineChipType = DefaultDetermineChipType;
	if( !MCF.WriteBinaryBlob )
		MCF.WriteBinaryBlob = DefaultWriteBinaryBlob;
	if( !MCF.ReadBinaryBlob )
		MCF.ReadBinaryBlob = DefaultReadBinaryBlob;
	if( !MCF.WriteWord )
		MCF.WriteWord = DefaultWriteWord;
	if( !MCF.WriteHalfWord )
		MCF.WriteHalfWord = DefaultWriteHalfWord;
	if( !MCF.WriteByte )
		MCF.WriteByte = DefaultWriteByte;
	if( !MCF.ReadCPURegister )
		MCF.ReadCPURegister = DefaultReadCPURegister;
	if( !MCF.WriteCPURegister )
		MCF.WriteCPURegister = DefaultWriteCPURegister;
	if( !MCF.WriteAllCPURegisters )
		MCF.WriteAllCPURegisters = DefaultWriteAllCPURegisters;
	if( !MCF.ReadAllCPURegisters )
		MCF.ReadAllCPURegisters = DefaultReadAllCPURegisters;
	if( !MCF.SetEnableBreakpoints )
		MCF.SetEnableBreakpoints = DefaultSetEnableBreakpoints;
	if( !MCF.ReadWord )
		MCF.ReadWord = DefaultReadWord;
	if( !MCF.ReadHalfWord )
		MCF.ReadHalfWord = DefaultReadHalfWord;
	if( !MCF.ReadByte )
		MCF.ReadByte = DefaultReadByte;
	if( !MCF.Erase )
		MCF.Erase = DefaultErase;
	if( !MCF.HaltMode )
		MCF.HaltMode = DefaultHaltMode;
	if( !MCF.SetSplit )
		MCF.SetSplit = DefaultSetSplit;
	if( !MCF.PollTerminal )
		MCF.PollTerminal = DefaultPollTerminal;
	if( !MCF.WaitForFlash )
		MCF.WaitForFlash = DefaultWaitForFlash;
	if( !MCF.WaitForDoneOp )
		MCF.WaitForDoneOp = DefaultWaitForDoneOp;
	if( !MCF.PrintChipInfo )
		MCF.PrintChipInfo = DefaultPrintChipInfo;
	if( !MCF.Unbrick )
		MCF.Unbrick = DefaultUnbrick;
	if( !MCF.ConfigureNRSTAsGPIO )
		MCF.ConfigureNRSTAsGPIO = DefaultConfigureNRSTAsGPIO;
  if( !MCF.ConfigureReadProtection )
    MCF.ConfigureReadProtection = DefaultConfigureReadProtection;
	if( !MCF.VoidHighLevelState )
		MCF.VoidHighLevelState = DefaultVoidHighLevelState;
	if( !MCF.DelayUS )
		MCF.DelayUS = DefaultDelayUS;
	if( !MCF.GetUUID )
		MCF.GetUUID = DefaultGetUUID;
  if( !MCF.SetClock )
    MCF.SetClock = DefaultSetClock;
  if( !MCF.EnableDebug )
    MCF.EnableDebug = DefaultEnableDebug;

	return 0;
}




void TestFunction(void * dev )
{
	uint32_t rv;
	int r;
	MCF.WriteReg32( dev, DMCONTROL, 0x80000001 ); // Make the debug module work properly.
	MCF.WriteReg32( dev, DMCONTROL, 0x80000001 ); // Initiate a halt request.
	MCF.WriteReg32( dev, DMCONTROL, 0x00000001 ); // Clear Halt Request.

	r = MCF.WriteWord( dev, 0x20000100, 0xdeadbeef );
	r = MCF.WriteWord( dev, 0x20000104, 0xcafed0de );
	r = MCF.WriteWord( dev, 0x20000108, 0x12345678 );
	r = MCF.WriteWord( dev, 0x20000108, 0x00b00d00 );
	r = MCF.WriteWord( dev, 0x20000104, 0x33334444 );

	r = MCF.ReadWord( dev, 0x20000100, &rv );
	printf( "**>>> %d %08x\n", r, rv );
	r = MCF.ReadWord( dev, 0x20000104, &rv );
	printf( "**>>> %d %08x\n", r, rv );
	r = MCF.ReadWord( dev, 0x20000108, &rv );
	printf( "**>>> %d %08x\n", r, rv );


	r = MCF.ReadWord( dev, 0x00000300, &rv );
	printf( "F %d %08x\n", r, rv );
	r = MCF.ReadWord( dev, 0x00000304, &rv );
	printf( "F %d %08x\n", r, rv );
	r = MCF.ReadWord( dev, 0x00000308, &rv );
	printf( "F %d %08x\n", r, rv );

	uint8_t buffer[256];
	int i;
	for( i = 0; i < 256; i++ ) buffer[i] = 0;
	MCF.WriteBinaryBlob( dev, 0x08000300, 256, buffer );
	MCF.ReadBinaryBlob( dev, 0x08000300, 256, buffer );
	for( i = 0; i < 256; i++ )
	{
		printf( "%02x ", buffer[i] );
		if( (i & 0xf) == 0xf ) printf( "\n" );
	}

	for( i = 0; i < 256; i++ ) buffer[i] = i;
	MCF.WriteBinaryBlob( dev, 0x08000300, 256, buffer );
	MCF.ReadBinaryBlob( dev, 0x08000300, 256, buffer );
	for( i = 0; i < 256; i++ )
	{
		printf( "%02x ", buffer[i] );
		if( (i & 0xf) == 0xf ) printf( "\n" );
	}
}

int DetectMemoryArea( void * dev, uint32_t address )
{
	struct InternalState * iss = (struct InternalState*)(((struct ProgrammerStructBase*)dev)->internal);
	int ret = 0;
	if( iss->target_chip == NULL)
	{
		ret = MCF.DetermineChipType( dev );
		if( ret ) return ret;
	}
	const struct RiscVChip_s * chip = iss->target_chip;
	fprintf( stderr, "Detecting Memory Area\n" );
	if( address < chip->flash_offset)
	{
		fprintf( stderr, "The starting address is lower than FLASH start. Aborting\n" );
		return -1;
	}
	if( address < chip->flash_offset + chip->flash_size )
	{
		iss->current_area = PROGRAM_AREA;
		return 0;
	}
	else if( chip->bootloader_size > 0 && 
					address >= chip->bootloader_offset &&
					address < chip->bootloader_offset + chip->bootloader_size )
	{
		iss->current_area = BOOTLOADER_AREA;
		return 0;
	}
	else if( chip->options_size > 0 && 
					address >= chip->options_offset &&
					address < chip->options_offset + chip->options_size )
	{
		iss->current_area = OPTIONS_AREA;
		return 0;
	}
	else if( chip->eeprom_size > 0 && 
					address >= chip->eeprom_offset &&
					address < chip->eeprom_offset + chip->eeprom_size )
	{
		iss->current_area = EEPROM_AREA;
		return 0;
	}
	else if( address >= chip->ram_base )
	{
		iss->current_area = RAM_AREA;
		return 0;
	}

	fprintf( stderr, "Something went wrong and we couldn't detect memory region\n" );
	return -2;
}

int CheckMemoryLocation( void * dev, enum MemoryArea area, uint32_t address, uint32_t length )
{
	struct InternalState * iss = (struct InternalState*)(((struct ProgrammerStructBase*)dev)->internal);
	int ret = 0;
	if( iss->target_chip == NULL)
	{
		ret = MCF.DetermineChipType( dev );
		if( ret ) return ret;
	}
	const struct RiscVChip_s * chip = iss->target_chip;

	if( area == DEFAULT_AREA )
	{
		if( iss->current_area == 0 ) DetectMemoryArea( dev, address );
		area = iss->current_area;
	}

	if( chip->protocol == PROTOCOL_UNSUPPORTED )
	{
		fprintf(stderr, "Flashing this MCU model is not yet supported in minichlink.\n");
		fprintf(stderr, "But you can still use other functions, like working with RAM, or debug registers.\n");
		return 0;
	}

	switch (area)
	{
	case DEFAULT_AREA:
		return 0;
	case PROGRAM_AREA:
		if( address >= chip->flash_offset &&
			  address < (chip->flash_offset + chip->flash_size) &&
			  (address + length) <= (chip->flash_offset + chip->flash_size) )
		{
			return 1;
		}
		break;
	case BOOTLOADER_AREA:
		if( chip->bootloader_size > 0 &&
			  address >= chip->bootloader_offset &&
			  address < chip->bootloader_offset + chip->bootloader_size &&
			  address + length <= chip->bootloader_offset + chip->bootloader_size )
		{
			return 1;
		}
		break;
	case OPTIONS_AREA:
		if( chip->options_size > 0 &&
			  address >= chip->options_offset &&
			  address < chip->options_offset + chip->options_size &&
			  address + length <= chip->options_offset + chip->options_size )
		{
			return 1;
		}
		break;
	case EEPROM_AREA:
		if( chip->eeprom_size > 0 &&
			  address >= chip->eeprom_offset &&
			  address < chip->eeprom_offset + chip->eeprom_size &&
			  address + length <= chip->eeprom_offset + chip->eeprom_size )
		{
			return 1;
		}
		break;
	case RAM_AREA:
		if( address >= chip->ram_base)
		{
			return 1;
		}
		break;
	}

	return 0;
}

static void readCSR( void * dev, uint32_t csr )
{
	MCF.HaltMode( dev, HALT_MODE_HALT_BUT_NO_RESET );

	// MCF.WriteReg32(dev, DMABSTRACTAUTO, 0x00000000); // Disable Autoexec.
	// MCF.WriteReg32(dev, DMPROGBUF0, 0x305022f3); // csrrs t0, mtvec, zero
	// MCF.WriteReg32(dev, DMPROGBUF1, 0x00100073); // ebreak

	// MCF.WriteReg32(dev, DMCOMMAND, 0x00271000); // Write execute.
	MCF.WriteReg32( dev, DMCOMMAND, 0x00220000 | csr ); // Read a0 into DATA0.
	uint32_t dmdata = 0;
	MCF.ReadReg32( dev, DMDATA0, &dmdata );
	fprintf( stderr, "Requested CSR: %08x = %08x\n", csr, dmdata );

	MCF.WriteReg32( dev, DMCONTROL, 0x40000001 );
	MCF.WriteReg32( dev, DMCONTROL, 0x40000001 );
}

static int DefaultRebootIntoBootloader( void * dev )
{
	struct InternalState * iss = (struct InternalState*)(((struct ProgrammerStructBase*)dev)->internal);
	int r;
	int max_timeout = 0;

	if( !MCF.Control3v3 && !MCF.Control5v )
	{
		fprintf( stderr, "This programming method doesn't have power controls.\n" );
		return -100;
	}

	if( iss->target_chip_type == CHIP_CH570 || iss->target_chip_type == CHIP_CH59x )
	{
		max_timeout = 500;
	}

	if( iss->target_chip_type != CHIP_CH570
		&& iss->target_chip_type != CHIP_CH585
		&& iss->target_chip_type != CHIP_CH59x )
	{
		printf( "Press and hold bootloader button. Then press enter to proceed\n");
		while( !IsKBHit() );
		ReadKBByte();
	}

	fprintf( stderr, "Entering Bootloader\n" );
	if( MCF.Control5v ) MCF.Control5v( dev, 0 );
	if( MCF.Control3v3 ) MCF.Control3v3( dev, 0 );

	MCF.DelayUS( dev, 60000 );
	MCF.DelayUS( dev, 60000 );
	MCF.DelayUS( dev, 60000 );
	MCF.DelayUS( dev, 60000 );
	MCF.DelayUS( dev, 60000 );
	MCF.FlushLLCommands( dev );
	if( MCF.ResetInterface && max_timeout == 0 ) MCF.ResetInterface( dev );
	if( MCF.Control5v ) MCF.Control5v( dev, 1 );
	if( MCF.Control3v3 ) MCF.Control3v3( dev, 1 );
	fprintf(stderr, "Connecting to the DM\n" );

	int timeout = 0;
	uint32_t ds = 0;
	for( timeout = 0; timeout < max_timeout; timeout++ )
	{
		MCF.DelayUS( dev, 10 );
		MCF.WriteReg32( dev, DMSHDWCFGR, 0x5aa50000 | (1<<10) ); // Shadow Config Reg
		MCF.WriteReg32( dev, DMCFGR, 0x5aa50000 | (1<<10) ); // CFGR (1<<10 == Allow output from slave)
		MCF.WriteReg32( dev, DMCONTROL, 0x80000001 ); // Make the debug module work properly.
		MCF.WriteReg32( dev, DMCONTROL, 0x80000001 ); // Initiate a halt request.
		MCF.WriteReg32( dev, DMCONTROL, 0x80000001 ); // No, really make sure.
		MCF.FlushLLCommands( dev );
		r = MCF.ReadReg32( dev, DMSTATUS, &ds );
		if( r )
		{
			fprintf( stderr, "\nError: Could not read DMSTATUS from programmers (%d)\n", r );
			continue;
		}
		MCF.FlushLLCommands( dev );
		if( ds != 0xffffffff && ds != 0x00000000 ) break;
		else fprintf( stderr, "." );
	}
	fprintf(stderr, "\n" );
	MCF.SetupInterface(dev);
	fprintf(stderr, "Connected\n" );
	if( iss->target_chip->protocol == PROTOCOL_CH5xx )
	{
		uint8_t info_reg = 0;
		r = MCF.ReadByte( dev, 0x40001045, &info_reg );
		// printf("R8_GLOB_CFG_INFO = %02x, BOOTLOADER_BIT = %d\n", info_reg, (info_reg & (1<<5)));
		if( !(info_reg & (1<<5)) || r ) {
			fprintf(stderr, "Failed to switch to bootloader\n");
			return -1;
		}
	}
	MCF.HaltMode = 0;
	printf( "Chip is halted in bootloader mode. Press enter to proceed\n");
	while(!IsKBHit());
	ReadKBByte();
  return 0;
}

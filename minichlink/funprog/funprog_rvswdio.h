// CH32V003 SWIO / CH32Vxxx / CH32Xxxx bit banging minimal reference
// implementation for bit banged IO or primitive IO.
//
// You will need to implement some of the basic functions. Example
// implementations for the functions on the ESP32-S2 are included.
//
// Copyright 2023-2024 <>< Charles Lohr, May be licensed under the MIT/x11 or
// NewBSD licenses. You choose. (Can be included in commercial and copyleft work)
//
// This file is original work.
//
// Mostly tested, though, not may not be perfect. Expect to tweak some things.
// The 003 code is pretty hardened, the other code may not be.
//
// Originally the only resources used to produce this file were publicly available docs
// and playing with the chips.  The only leveraged tool was this:
//   https://github.com/perigoso/sigrok-rvswd

#ifndef FUNPROG_RVSWDIO_H
#define FUNPROG_RVSWDIO_H

enum RiscVChip {
	CHIP_UNKNOWN = 0x00,
	CHIP_CH32V10x = 0x01,
	CHIP_CH57x = 0x02,
	CHIP_CH56x = 0x03,
	CHIP_CH32V20x = 0x05,
	CHIP_CH32V30x = 0x06,
	CHIP_CH58x = 0x07,
	CHIP_CH32V003 = 0x09,
	CHIP_CH59x = 0x0b,
	CHIP_CH643 = 0x0c,
	CHIP_CH32X03x = 0x0d,
	CHIP_CH32L10x = 0x0e,
	CHIP_CH564 = 0x0f,

	CHIP_CH32V00x = 0x4e,

	CHIP_CH570 = 0x8b,
	CHIP_CH585 = 0x4b,
	CHIP_CH645 = 0x46,
	CHIP_CH641 = 0x49,
	CHIP_CH32V317 = 0x86,
	CHIP_CH32M030 = 0x8e,
};

enum MemoryArea {
	DEFAULT_AREA = 0,
	PROGRAM_AREA = 1,
	BOOTLOADER_AREA = 2,
	OPTIONS_AREA = 3,
	EEPROM_AREA = 4,
	RAM_AREA = 5,
};

struct SWIOState
{
	// Set these before calling any functions
	int t1coeff;
	int pinmaskD;
	int pinmaskC;
	int opmode; // 0 for undefined, 1 for SWIO, 2 for SWD
	enum RiscVChip target_chip_type;
	int sectorsize;

	// Zero the rest of the structure.
	uint32_t statetag;
	uint32_t lastwriteflags;
	uint32_t currentstateval;
	uint32_t flash_unlocked;
	uint32_t autoincrement;
	uint32_t clock_set;
	uint32_t no_autoexec;
#if CH5xx_SUPPORT
	int microblob_running; // < 0 - not running, 0+ - current address in flash microblob is ready to write data to
	int writing_flash;
#endif
};

#define STTAG( x ) (*((uint32_t*)(x)))

// You should interface to this file via these functions
static inline void Send1BitSWIO( struct SWIOState * state ) IRAM;
static inline void Send0BitSWIO( struct SWIOState * state ) IRAM;
static inline int ReadBitSWIO( struct SWIOState * state ) IRAM;
static inline void SendBitRVSWD( struct SWIOState * state, int val ) IRAM;
static inline int ReadBitRVSWD( struct SWIOState * state ) IRAM;
static inline void ConfigureIOForRVSWIO(struct SWIOState * state);
static inline void ConfigureIOForRVSWD(struct SWIOState * state);

// Provided Basic functions
static void MCFWriteReg32( struct SWIOState * state, uint8_t command, uint32_t value ) IRAM;
static int MCFReadReg32( struct SWIOState * state, uint8_t command, uint32_t * value ) IRAM;

// More advanced functions built on lower level PHY.
static int InitializeSWDSWIO( struct SWIOState * state );
#if CH5xx_SUPPORT
static int ReadByte( struct SWIOState * iss, uint32_t address_to_read, uint8_t * data );
static int WriteByte( struct SWIOState * iss, uint32_t address_to_write, uint8_t data );
#endif
static int ReadWord( struct SWIOState * state, uint32_t word, uint32_t * ret );
static int WriteWord( struct SWIOState * state, uint32_t word, uint32_t val );
static int WaitForFlash( struct SWIOState * state );
static int WaitForDoneOp( struct SWIOState * state );
static int WriteBlock( struct SWIOState * iss, uint32_t address_to_write, uint8_t * data, uint8_t len, uint8_t erase );
static int UnlockFlash( struct SWIOState * iss );
static int EraseFlash( struct SWIOState * iss, uint32_t address, uint32_t length, int type );
static void ResetInternalProgrammingState( struct SWIOState * iss );
static int PollTerminal( struct SWIOState * iss, uint8_t * buffer, int maxlen, uint32_t leavevalA, uint32_t leavevalB );

#if CH5xx_SUPPORT
extern int ch5xx_set_clock(struct SWIOState * iss, uint32_t clock);
extern int ch5xx_write_flash_using_microblob(struct SWIOState * iss, uint32_t start_addr, uint8_t* data, uint32_t len);
#endif

#define BDMDATA0       0x04
#define BDMDATA1       0x05
#define DMCONTROL      0x10
#define DMSTATUS       0x11
#define DMHARTINFO     0x12
#define DMABSTRACTCS   0x16
#define DMCOMMAND      0x17
#define DMABSTRACTAUTO 0x18
#define DMPROGBUF0     0x20
#define DMPROGBUF1     0x21
#define DMPROGBUF2     0x22
#define DMPROGBUF3     0x23
#define DMPROGBUF4     0x24
#define DMPROGBUF5     0x25
#define DMPROGBUF6     0x26
#define DMPROGBUF7     0x27

#define DMCPBR         0x7C
#define DMCFGR         0x7D
#define DMSHDWCFGR     0x7E

#ifndef __CH32V00x_H
#define FLASH_STATR_WRPRTERR       ((uint32_t)0x00000010)
#define CR_PAGE_PG                 ((uint32_t)0x00010000)
#define CR_BUF_LOAD                ((uint32_t)0x00040000)
#ifndef FLASH_CTLR_MER
#define FLASH_CTLR_MER             ((uint16_t)0x0004)     /* Mass Erase */
#endif
#define CR_STRT_Set                ((uint32_t)0x00000040)
#define CR_PAGE_ER                 ((uint32_t)0x00020000)
#define CR_BUF_RST                 ((uint32_t)0x00080000)
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// High level functions
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int InitializeSWDSWIO( struct SWIOState * state )
{
	for( int timeout = 20; timeout; timeout-- )
	{
		FUN_PRINTF_DEBUG( "CFG FOR RV\n" );
		ConfigureIOForRVSWIO(state);
		FUN_PRINTF_DEBUG( "DONE CFG FOR RV\n" );
		// Careful - don't halt the part, we might just want to attach for a debug printf or something.
		state->target_chip_type = CHIP_UNKNOWN;
		state->sectorsize = 64;

		state->opmode = 1; // Try SWIO first
		// First try to see if there is an 003.
		MCFWriteReg32( state, DMSHDWCFGR, 0x5aa50000 | (1<<10) ); // Shadow Config Reg
		MCFWriteReg32( state, DMCFGR, 0x5aa50000 | (1<<10) ); // CFGR (1<<10 == Allow output from slave)
		MCFWriteReg32( state, DMSHDWCFGR, 0x5aa50000 | (1<<10) ); // Try twice just in case
		MCFWriteReg32( state, DMCFGR, 0x5aa50000 | (1<<10) );

		MCFWriteReg32( state, DMCONTROL, 0x00000001 );
		MCFWriteReg32( state, DMCONTROL, 0x00000001 );

		// See if we can see a chip here...
		uint32_t value = 0;
		int readdm = MCFReadReg32( state, DMCFGR, &value );
		if( readdm == 0 && ( value & 0xffff0000 ) == ( 0x5aa50000 ) )
		{
			FUN_PRINTF_DEBUG( "Found RVSWIO interface.\n" );
			return 0;
		}

		FUN_PRINTF_DEBUG( "CFG FOR SWD\n" );
		ConfigureIOForRVSWD(state);
		FUN_PRINTF_DEBUG( "DONE CFG FOR SWD\n" );

		//Otherwise Maybe it's SWD?
		state->opmode = 2;

		MCFWriteReg32( state, DMCONTROL, 0x00000001 );
		MCFWriteReg32( state, DMCONTROL, 0x00000001 );

		uint32_t dmstatus, dmcontrol;
		if( MCFReadReg32( state, DMSTATUS, &dmstatus ) != 0 ||
		  MCFReadReg32( state, DMCONTROL, &dmcontrol ) != 0 )
		{
			FUN_PRINTF_DEBUG( "Could not read from RVSWD connection\n" );
			state->opmode = 0;
			continue;
		}

		// See if we can see a chip here...
		if( ( ( ( dmstatus >> 8 ) & 0xf ) != 0x0c &&
		  ( ( dmstatus >> 8 ) & 0xf ) != 0x03 ) ||
		  dmcontrol != 1 )
		{
			FUN_PRINTF_DEBUG( "DMSTATUS invalid (Probably no RVSWD chip)\n" );
			state->opmode = 0;
			continue;
		}

		MCFWriteReg32( state, DMABSTRACTCS, 0x08000302 ); // Clear out abstractcs register.
		FUN_PRINTF_DEBUG( "Found RVSWD interface\n" );
		return 0;
	}
	FUN_PRINTF_DEBUG( "TIMEOUT\n" );
	return -55;
}

static int DetermineChipTypeAndSectorInfo( struct SWIOState * iss, uint8_t * reply)
{
	struct SWIOState * dev = iss;
	if( iss->target_chip_type == CHIP_UNKNOWN || reply != NULL )
	{
		if( iss->opmode == 0 )
		{
			int r = InitializeSWDSWIO( iss );
			if( r ) return 0x11;
		}
		uint32_t rr;
		if( MCFReadReg32( dev, DMHARTINFO, &rr ) )
		{
			FUN_PRINTF_DEBUG( "Error: Could not get hart info.\n" );
			return 0x12;
		}

		MCFWriteReg32( dev, DMCONTROL, 0x80000001 ); // Make the debug module work properly.
		MCFWriteReg32( dev, DMCONTROL, 0x80000001 ); // Initiate halt request.

		// Tricky, this function needs to clean everything up because it may be used entering debugger.
		uint32_t old_data0;
		MCFReadReg32( dev, BDMDATA0, &old_data0 );
		MCFWriteReg32( dev, DMCOMMAND, 0x00221008 );		// Copy data from x8.
		uint32_t old_x8;
		MCFReadReg32( dev, BDMDATA0, &old_x8 );

		uint32_t marchid = 0;

		MCFWriteReg32( dev, DMABSTRACTCS, 0x08000700 ); // Clear out any dmabstractcs errors.

		MCFWriteReg32( dev, DMABSTRACTAUTO, 0x00000000 );
		MCFWriteReg32( dev, DMCOMMAND, 0x00220000 | 0xf12 );
		MCFWriteReg32( dev, DMCOMMAND, 0x00220000 | 0xf12 );  // Need to double-read, not sure why.
		MCFReadReg32( dev, BDMDATA0, &marchid );
		MCFWriteReg32( dev, DMPROGBUF0, 0x90024000 );		// c.ebreak <<== c.lw x8, 0(x8)

		uint32_t chip_id = 0;
		uint32_t vendor_bytes = 0;
		uint32_t sevenf_id = 0;
		int read_protection = 0;
		uint8_t ch5xx = 0;
		MCFReadReg32( dev, 0x7f, &sevenf_id );

		if( sevenf_id == 0 )
		{
			// Need to load new progbuf because we're reading 1 byte now
			MCFWriteReg32( dev, DMPROGBUF0, 0x00040403 ); // lb x8, 0(x8)
			MCFWriteReg32( dev, DMPROGBUF1, 0x00100073 ); // c.ebreak
			MCFWriteReg32( dev, BDMDATA0, 0x40001041 );			// Special chip ID location.
			MCFWriteReg32( dev, DMCOMMAND, 0x00271008 );		// Copy data to x8, and execute.
			WaitForDoneOp( dev );
			MCFWriteReg32( dev, DMCOMMAND, 0x00221008 );		// Copy data from x8.
			MCFReadReg32( dev, BDMDATA0, &chip_id );
			chip_id = chip_id & 0xff;

			// Looks like a CH32V103 or a CH56x
			if( chip_id == 0 )
			{
				// First check for CH56x
				MCFWriteReg32( dev, BDMDATA0, 0x40001001 );			
				MCFWriteReg32( dev, DMCOMMAND, 0x00271008 );		// Copy data to x8, and execute.
				WaitForDoneOp( dev );
				MCFWriteReg32( dev, DMCOMMAND, 0x00221008 );		// Copy data from x8.
				MCFReadReg32( dev, BDMDATA0, &chip_id );
				MCFWriteReg32( dev, BDMDATA0, 0x40001002 );			// Special chip ID location.
				MCFWriteReg32( dev, DMCOMMAND, 0x00271008 );		// Copy data to x8, and execute.
				WaitForDoneOp( dev );
				MCFWriteReg32( dev, DMCOMMAND, 0x00221008 );		// Copy data from x8.
				MCFReadReg32( dev, BDMDATA0, &vendor_bytes );

				if( (vendor_bytes & 0xff) == 2 && ((chip_id & 0xff) == 65 || (chip_id & 0xff) == 69) )
				{
					iss->target_chip_type = CHIP_CH56x;
					chip_id = 0x500 | (chip_id & 0xff);
					goto chip_identified;
				}

				// Now actually check for CH32V103
				MCFWriteReg32( dev, DMPROGBUF0, 0x90024000 );	// c.ebreak <<== c.lw x8, 0(x8)
				MCFWriteReg32( dev, BDMDATA0, 0x1ffff880 );			// Special chip ID location.
				MCFWriteReg32( dev, DMCOMMAND, 0x00271008 );		// Copy data to x8, and execute.
				WaitForDoneOp( dev );
				MCFWriteReg32( dev, DMCOMMAND, 0x00221008 );		// Copy data from x8.
				MCFReadReg32( dev, BDMDATA0, &chip_id );
				MCFWriteReg32( dev, BDMDATA0, 0x1ffff884 );			// Special chip ID location.
				MCFWriteReg32( dev, DMCOMMAND, 0x00271008 );		// Copy data to x8, and execute.
				WaitForDoneOp( dev );
				MCFWriteReg32( dev, DMCOMMAND, 0x00221008 );		// Copy data from x8.
				MCFReadReg32( dev, BDMDATA0, &vendor_bytes );
				
				if( ((((vendor_bytes >> 16) & 0xff00) != 0x2500) && (((vendor_bytes >> 16) & 0xdf00) != 0x1500)) || chip_id != 0xdc78fe34 )
				{
					uint32_t flash_obr = 0;
					MCFWriteReg32( dev, BDMDATA0, 0x4002201c );			// Special chip ID location.
					MCFWriteReg32( dev, DMCOMMAND, 0x00271008 );		// Copy data to x8, and execute.
					WaitForDoneOp( dev );
					MCFWriteReg32( dev, DMCOMMAND, 0x00221008 );		// Copy data from x8.
					MCFReadReg32( dev, BDMDATA0, &flash_obr );
						
					if( (flash_obr & 3) == 2 )
					{
						iss->target_chip_type = CHIP_CH32V10x;
						iss->no_autoexec = 1;
						iss->sectorsize = 128;
						read_protection = 1;
						chip_id = (3 << 12) | 0x103;
					}
				}
				else
				{
					iss->target_chip_type = CHIP_CH32V10x;
					iss->no_autoexec = 1;
					iss->sectorsize = 128;
					read_protection = -1;
					chip_id = (3 << 12) | 0x103;
				}
			}
			else
			{
				// Check for CH5xx        
				read_protection = -1;
				if( (chip_id & 0xf0) == 0x90 )
				{
					uint32_t sevenc = 0;
					MCFReadReg32( dev, DMCPBR, &sevenc );
					if((sevenc & 0x30000) == 0)
					{
						iss->target_chip_type = CHIP_CH59x;
						iss->sectorsize = 256;
						ch5xx = 1;
						chip_id = 0x500 | chip_id;
					}
					else
					{
						iss->target_chip_type = CHIP_CH585;
						iss->no_autoexec = 1;
						iss->sectorsize = 256;
						ch5xx = 1;
						chip_id = 0x500 | 0x85;
					}
				}
				else
				{
					if( chip_id == 0x70 || chip_id == 0x72 )
					{
						iss->target_chip_type = CHIP_CH570;
						iss->sectorsize = 4096;
						ch5xx = 1;
						chip_id = 0x500 | chip_id;
					}
					else if( chip_id == 0x71 || chip_id == 0x73 )
					{
						iss->target_chip_type = CHIP_CH57x;
						iss->no_autoexec = 1;
						iss->sectorsize = 256;
						ch5xx = 1;
						chip_id = 0x500 | chip_id;
					}
					else if( chip_id == 0x82 || chip_id == 0x83 )
					{
						iss->target_chip_type = CHIP_CH58x;
						iss->no_autoexec = 1;
						iss->sectorsize = 256;
						ch5xx = 1;
						chip_id = 0x500 | chip_id;
					}
				}
			}
		}
		else
		{
			uint32_t masked_id = sevenf_id & 0xfff00000;
			uint32_t masked_id2 = sevenf_id & 0xfff00f00;
			if( masked_id == 0x3b00000 )
			{
				iss->target_chip_type = CHIP_CH32M030;
				iss->sectorsize = 256;
				chip_id = (2 << 12) | 0x30;
			}
			else if( masked_id == 0x56400000 )
			{
				iss->target_chip_type = CHIP_CH564;
				iss->sectorsize = 256;
				chip_id = 0x564;
			}
			else if( masked_id == 0x31700000 )
			{
				iss->target_chip_type = CHIP_CH32V317;
				iss->sectorsize = 256;
				chip_id = (3 << 12) | 0x317;
			}
			else if( masked_id == 0x00200000 || masked_id == 0x00400000 || masked_id == 0x00600000 || masked_id == 0x00700000 )
			{
				iss->target_chip_type = CHIP_CH32V00x;
				iss->sectorsize = 256;
				chip_id = (3 << 12) | (masked_id >> 20);
			}

			if( masked_id2 == 0x64100500 )
			{
				iss->target_chip_type = CHIP_CH641;
				iss->sectorsize = 64;
				chip_id = 0x641;
			}
			else if( masked_id2 == 0x64300600 )
			{
				iss->target_chip_type = CHIP_CH643;
				iss->sectorsize = 256;
				chip_id = 0x643;
			}
			else if( masked_id == 0x64500000 )
			{
				iss->target_chip_type = CHIP_CH645;
				iss->sectorsize = 256;
				chip_id = 0x645;
			}
			else if( masked_id2 == 0x3500600 )
			{
				iss->target_chip_type = CHIP_CH32X03x;
				iss->sectorsize = 256;
				chip_id = (4 << 12) | 0x35;
			}
			else if( masked_id2 == 0x10300700 )
			{
				iss->target_chip_type = CHIP_CH32L10x;
				iss->sectorsize = 256;
				chip_id = (1 << 12) | 0x103;
			}
			else if( masked_id2 == 0x300500 )
			{
				iss->target_chip_type = CHIP_CH32V003;
				iss->sectorsize = 64;
				chip_id = (3 << 12) | 0x3;
			}

			if( (sevenf_id & 0x20000500) == 0x20000500 || (sevenf_id & 0x30000500) == 0x30000500 )
			{
				switch ((sevenf_id & 0xfff00000) >> 20)
				{
				case 0x203:
				case 0x208:
					iss->target_chip_type = CHIP_CH32V20x;
					iss->sectorsize = 256;
					break;
				case 0x303:
				case 0x305:
				case 0x307:
					iss->target_chip_type = CHIP_CH32V30x;
					iss->sectorsize = 256;
					break;
				}
				chip_id = (3 << 12) | (masked_id >> 20);
			}
		}

chip_identified:

    if( iss->target_chip_type == CHIP_CH570 )
    {
      uint32_t options;
      ReadWord( dev, 0x40001058, &options );
      if( (options&0x800000) || (options&0x200000) ) read_protection = 1;
    }
		else if( read_protection == 0 )
		{
			uint32_t one;
			int two;
			MCFWriteReg32( dev, BDMDATA0, 0x4002201c );
			MCFWriteReg32( dev, DMCOMMAND, 0x00271008 ); // Copy data to x8, and execute.
			WaitForDoneOp( dev );
			MCFWriteReg32( dev, DMCOMMAND, 0x00221008 ); // Copy data from x8.
			MCFReadReg32( dev, BDMDATA0, &one );
			MCFWriteReg32( dev, BDMDATA0, 0x40022020 );
			MCFWriteReg32( dev, DMCOMMAND, 0x00271008 ); // Copy data to x8, and execute.
			WaitForDoneOp( dev );
			MCFWriteReg32( dev, DMCOMMAND, 0x00221008 ); // Copy data from x8.
			MCFReadReg32( dev, BDMDATA0, (uint32_t*)&two );
			
			if( (one & 2) || two != -1 ) read_protection = 1;
		}

		if( reply != NULL )
		{
			reply[3] = sevenf_id >> 24;
			reply[2] = sevenf_id >> 16;
			reply[1] = sevenf_id >> 8;
			reply[0] = sevenf_id;
			reply[5] = chip_id >> 8;
			reply[4] = chip_id;
			if( read_protection == 1 ) reply[5] |= 0x80;
		}
		
		// Cleanup
		MCFWriteReg32( dev, BDMDATA0, old_x8 );
		MCFWriteReg32( dev, DMCOMMAND, 0x00231008 ); // Copy data to x8
		MCFWriteReg32( dev, BDMDATA0, old_data0 );
		iss->statetag = STTAG( "XXXX" );
#if CH5xx_SUPPORT
		if( ch5xx ) ch5xx_set_clock( dev, 0 );
#endif
	}
	return 0;
}

static int WaitForFlash( struct SWIOState * iss )
{
	struct SWIOState * dev = iss;

	if( DetermineChipTypeAndSectorInfo( iss, NULL ) ) return -9;

	uint32_t rw, timeout = 0;
	do
	{
		rw = 0;
		ReadWord( dev, 0x4002200C, &rw ); // FLASH_STATR => 0x4002200C
	} while( (rw & 1) && timeout++ < 2000);  // BSY flag.

	WriteWord( dev, 0x4002200C, 0 );

	if( rw & FLASH_STATR_WRPRTERR )
		return -44;

	if( rw & 1 )
		return -5;

	return 0;
}

static int WaitForDoneOp( struct SWIOState * iss )
{
	int r;
	uint32_t rrv;
	int ret = 0;
	int timeout = 1000;
	struct SWIOState * dev = iss;
	do
	{
		r = MCFReadReg32( dev, DMABSTRACTCS, &rrv );
		if( r ) return r;
		if( timeout-- == 0 ) return -8;
	}
	while( rrv & (1<<12) );
	if( (rrv >> 8 ) & 7 )
	{
		MCFWriteReg32( dev, DMABSTRACTCS, 0x00000700 );
		ret = -33;
	}
	return ret;
}

static int StaticUpdatePROGBUFRegs( struct SWIOState * dev )
{
	if( DetermineChipTypeAndSectorInfo( dev, NULL ) ) return -9;

	MCFWriteReg32( dev, DMABSTRACTAUTO, 0 ); // Disable Autoexec.
	uint32_t rr;
	MCFReadReg32( dev, DMHARTINFO, &rr );
	uint32_t data0offset = 0xe0000000 | ( rr & 0x7ff );
	MCFWriteReg32( dev, BDMDATA0, data0offset ); // DATA0's location in memory. (hard code to 0xe00000f4 if only working on the 003)
	MCFWriteReg32( dev, DMCOMMAND, 0x0023100a ); // Copy data to x10
	MCFWriteReg32( dev, BDMDATA0, data0offset + 4 ); // DATA1's location in memory. (hard code to 0xe00000f8 if only working on the 003)
	MCFWriteReg32( dev, DMCOMMAND, 0x0023100b ); // Copy data to x11
	MCFWriteReg32( dev, BDMDATA0, 0x4002200c ); //FLASH->STATR (note add 4 to FLASH->CTLR)
	MCFWriteReg32( dev, DMCOMMAND, 0x0023100c ); // Copy data to x12

	// This is not even needed on the v20x/v30x chips.  But it won't harm us to set the register for simplicity.
	// v003 requires bufload every word.
	// x035 requires bufload every word in spite of what the datasheet says.
	// CR_PAGE_PG = FTPG = 0x00010000 | CR_BUF_LOAD = 0x00040000
	MCFWriteReg32( dev, BDMDATA0, 0x00010000|0x00040000 );

	MCFWriteReg32( dev, DMCOMMAND, 0x0023100d ); // Copy data to x13
	return 0;
}

static void ResetInternalProgrammingState( struct SWIOState * iss )
{
	iss->statetag = 0;
	iss->lastwriteflags = 0;
	iss->currentstateval = 0;
	iss->flash_unlocked = 0;
	iss->autoincrement = 0;
	iss->clock_set = 0;
#if CH5xx_SUPPORT
	iss->microblob_running = -2;
	iss->writing_flash = 0;
#endif
}

static int ReadByte( struct SWIOState * iss, uint32_t address_to_read, uint8_t * data )
{
	struct SWIOState * dev = iss;
	int ret = 0;
	iss->statetag = STTAG( "XXXX" );

	MCFWriteReg32( dev, DMABSTRACTAUTO, 0x00000000 ); // Disable Autoexec.

	// Different address, so we don't need to re-write all the program regs.
	// lb x8,0(x9)  // Read from the address.
	MCFWriteReg32( dev, DMPROGBUF0, 0x00048403 ); // lb x8, 0(x9)
	MCFWriteReg32( dev, DMPROGBUF1, 0x00100073 ); // c.ebreak

	MCFWriteReg32( dev, BDMDATA0, address_to_read );
	MCFWriteReg32( dev, DMCOMMAND, 0x00231009 ); // Copy data to x9
	MCFWriteReg32( dev, DMCOMMAND, 0x00241000 ); // Only execute.
	MCFWriteReg32( dev, DMCOMMAND, 0x00221008 ); // Read x8 into DATA0.

	ret |= WaitForDoneOp( dev );
	iss->currentstateval = -1;

	uint32_t rr;
	ret |= MCFReadReg32( dev, BDMDATA0, &rr );
	*data = rr;
	return ret;
}

static int ReadWord( struct SWIOState * iss, uint32_t address_to_read, uint32_t * data )
{
	struct SWIOState * dev = iss;
	int autoincrement = 1;

	if( address_to_read == 0x40022010 || address_to_read == 0x4002200C ) // Don't autoincrement when checking flash flag. 
		autoincrement = 0;

	if( iss->statetag != STTAG( "RDSQ" ) || address_to_read != iss->currentstateval || autoincrement != iss->autoincrement )
	{
		if( iss->statetag != STTAG( "RDSQ" ) || autoincrement != iss->autoincrement )
		{
			if( iss->statetag != STTAG( "WRSQ" ) )
			{
				int r = StaticUpdatePROGBUFRegs( dev );
				if( r ) return r;
			}

			MCFWriteReg32( dev, DMABSTRACTAUTO, 0 ); // Disable Autoexec.

			// c.lw x8,0(x11) // Pull the address from DATA1
			// c.lw x9,0(x8)  // Read the data at that location.
			MCFWriteReg32( dev, DMPROGBUF0, 0x40044180 );
			if( autoincrement )
			{
				// c.addi x8, 4
				// c.sw x9, 0(x10) // Write back to DATA0

				MCFWriteReg32( dev, DMPROGBUF1, 0xc1040411 );
			}
			else
			{
				// c.nop
				// c.sw x9, 0(x10) // Write back to DATA0

				MCFWriteReg32( dev, DMPROGBUF1, 0xc1040001 );
			}
			// c.sw x8, 0(x11) // Write addy to DATA1
			// c.ebreak
			MCFWriteReg32( dev, DMPROGBUF2, 0x9002c180 );
			if( !iss->no_autoexec )
				MCFWriteReg32( dev, DMABSTRACTAUTO, 1 ); // Enable Autoexec (not autoincrement)
			iss->autoincrement = autoincrement;
		}

		MCFWriteReg32( dev, BDMDATA1, address_to_read );
		if( !iss->no_autoexec )
			MCFWriteReg32( dev, DMCOMMAND, 0x00240000 ); // Execute.

		iss->statetag = STTAG( "RDSQ" );
		iss->currentstateval = address_to_read;
	}

	if( iss->autoincrement )
		iss->currentstateval += 4;
    
	if( iss->no_autoexec ) {
		MCFWriteReg32( dev, DMCOMMAND, 0x00240000 );
	}

	// Only an issue if we are curising along very fast.
	int r = WaitForDoneOp( dev );
	if( r ) return r;

	r = MCFReadReg32( dev, BDMDATA0, data );
	return r;
}

static int WriteByte( struct SWIOState * iss, uint32_t address_to_write, uint8_t data )
{
	struct SWIOState * dev = iss;
	int ret = 0;
	iss->statetag = STTAG( "XXXX" );

	MCFWriteReg32( dev, DMABSTRACTAUTO, 0x00000000 ); // Disable Autoexec.

	// Different address, so we don't need to re-write all the program regs.
	// sh x8,0(x9)  // Write to the address.
	MCFWriteReg32( dev, DMPROGBUF0, 0x00848023 ); // sb x8, 0(x9)
	MCFWriteReg32( dev, DMPROGBUF1, 0x00100073 ); // c.ebreak

	MCFWriteReg32( dev, BDMDATA0, address_to_write );
	MCFWriteReg32( dev, DMCOMMAND, 0x00231009 ); // Copy data to x9
	MCFWriteReg32( dev, BDMDATA0, data );
	MCFWriteReg32( dev, DMCOMMAND, 0x00271008 ); // Copy data to x8, and execute program.

	ret |= WaitForDoneOp( dev );
	iss->currentstateval = -1;
	return ret;
}

static int WriteWord( struct SWIOState * iss, uint32_t address_to_write, uint32_t data )
{
	struct SWIOState * dev = iss;

	int ret = 0;

	int is_flash = 0;
	if( ( address_to_write & 0xff000000 ) == 0x08000000 || ( address_to_write & 0x1FFF0000 ) == 0x1FFF0000 )
	{
		// Is flash.
		is_flash = 1;
	}

	if( iss->statetag != STTAG( "WRSQ" ) || is_flash != iss->lastwriteflags )
	{
		int did_disable_req = 0;

		if( iss->statetag != STTAG( "WRSQ" ) )
		{
			MCFWriteReg32( dev, DMABSTRACTAUTO, 0x00000000 ); // Disable Autoexec.
			did_disable_req = 1;

			if( iss->statetag != STTAG( "RDSQ" ) )
			{
				int r = StaticUpdatePROGBUFRegs( dev );
				if( r ) return r;
			}

			// Different address, so we don't need to re-write all the program regs.
			// c.lw x8,0(x10) // Get the value to write.
			// c.lw x9,0(x11) // Get the address to write to. 
			MCFWriteReg32( dev, DMPROGBUF0, 0x41844100 );
			// c.sw x8,0(x9)  // Write to the address.
			// c.addi x9, 4
			MCFWriteReg32( dev, DMPROGBUF1, 0x0491c080 );
		}

		if( is_flash && iss->target_chip_type == CHIP_CH32V10x)
		{
			// Special 16 bytes buffer write sequence for CH32V103
			MCFWriteReg32( dev, DMPROGBUF2, 0x0001c184 ); // c.sw x9,0(x11); c.nop;
			MCFWriteReg32( dev, DMPROGBUF3, 0x9002e391 ); // c.bnez x15, 4; c.ebreak;
			MCFWriteReg32( dev, DMPROGBUF4, 0x4200c254 ); // c.sw x13,4(x12); c.lw x8,0(x12);
			MCFWriteReg32( dev, DMPROGBUF5, 0xfc758805 ); // c.andi x8, 1; c.bnez x8, -4;
			MCFWriteReg32( dev, DMPROGBUF6, 0x90024781 ); // c.li x15, 0; c.ebreak;
		}
		else if( is_flash )
		{
			// A little weird - we need to wait until the buf load is done here to continue.
			// x12 = 0x4002200C (FLASH_STATR)
			//
			// c254 c.sw x13,4(x12) // Acknowledge the page write.  (BUT ONLY ON x035 / v003)
			//  otherwise c.nop
			// 4200 c.lw x8,0(x12)  // Start checking to see when buf load is done.
			// 8809 c.andi x8, 2    // Only look at WR_BSY (seems to be rather undocumented)
			//  8805 c.andi x8, 1    // Only look at BSY if we're not on a v30x / v20x
			// fc75 c.bnez x8, -4
			// c.ebreak
			MCFWriteReg32( dev, DMPROGBUF2, 0x0001c184 );
			MCFWriteReg32( dev, DMPROGBUF3,
			  (iss->target_chip_type == CHIP_CH32V003 || iss->target_chip_type == CHIP_CH32V00x
			  || iss->target_chip_type == CHIP_CH32X03x || iss->target_chip_type == CHIP_CH32L10x
			  || iss->target_chip_type == CHIP_CH641 || iss->target_chip_type == CHIP_CH643) ?
			  0x4200c254 : 0x42000001 );

			MCFWriteReg32( dev, DMPROGBUF4,
			  (iss->target_chip_type == CHIP_CH32V20x || iss->target_chip_type == CHIP_CH32V30x ) ?
			  0xfc758809 : 0xfc758805 );

			MCFWriteReg32( dev, DMPROGBUF5, 0x90029002 );
		}
		else
		{
			MCFWriteReg32( dev, DMPROGBUF2, 0x9002c184 ); // c.sw x9,0(x11); c.ebreak;
		}

		MCFWriteReg32( dev, BDMDATA1, address_to_write );
		MCFWriteReg32( dev, BDMDATA0, data );

		if( iss->no_autoexec )
		{
			MCFWriteReg32( dev, DMCOMMAND, 0x00240000 ); // Execute.
		}
		else if( did_disable_req )
		{
			MCFWriteReg32( dev, DMCOMMAND, 0x00240000 ); // Execute.
			MCFWriteReg32( dev, DMABSTRACTAUTO, 1 ); // Enable Autoexec.
		}

		iss->lastwriteflags = is_flash;

		iss->statetag = STTAG( "WRSQ" );
		iss->currentstateval = address_to_write;
	}
	else
	{
		if( address_to_write != iss->currentstateval )
		{
			MCFWriteReg32( dev, BDMDATA1, address_to_write );
		}
		MCFWriteReg32( dev, BDMDATA0, data );
		if( iss->no_autoexec )
		{
			MCFWriteReg32( dev, DMCOMMAND, 0x00240000 ); // Execute.
		}
	}
	if( is_flash )
		ret |= WaitForDoneOp( dev );

	iss->currentstateval += 4;

	return ret;
}

static int UnlockFlash( struct SWIOState * iss )
{
	struct SWIOState * dev = iss;

	if( DetermineChipTypeAndSectorInfo( iss, NULL ) ) return -9;

	uint32_t rw;
	ReadWord( dev, 0x40022010, &rw );  // FLASH->CTLR = 0x40022010
	if( rw & 0x8080 ) 
	{

		WriteWord( dev, 0x40022004, 0x45670123 ); // FLASH->KEYR = 0x40022004
		WriteWord( dev, 0x40022004, 0xCDEF89AB );
		WriteWord( dev, 0x40022008, 0x45670123 ); // OBKEYR = 0x40022008
		WriteWord( dev, 0x40022008, 0xCDEF89AB );
		WriteWord( dev, 0x40022024, 0x45670123 ); // MODEKEYR = 0x40022024
		WriteWord( dev, 0x40022024, 0xCDEF89AB );

		ReadWord( dev, 0x40022010, &rw ); // FLASH->CTLR = 0x40022010
		if( rw & 0x8080 ) 
		{
			return -9;
		}
	}
	iss->statetag = STTAG( "XXXX" );
	iss->flash_unlocked = 1;
	return 0;
}

static int EraseFlash( struct SWIOState * iss, uint32_t address, uint32_t length, int type )
{
	struct SWIOState * dev = iss;

	uint32_t rw;

	if( !iss->flash_unlocked )
	{
		if( ( rw = UnlockFlash( iss ) ) )
			return rw;
	}

	if( type == 1 )
	{
		// Whole-chip flash
		iss->statetag = STTAG( "XXXX" );
		FUN_PRINTF_DEBUG( "Whole-chip erase\n" );
		WriteWord( dev, 0x40022010, 0 ); //  FLASH->CTLR = 0x40022010
		WriteWord( dev, 0x40022010, FLASH_CTLR_MER  );
		WriteWord( dev, 0x40022010, CR_STRT_Set|FLASH_CTLR_MER );
		if( WaitForFlash( dev ) ) return -13;
		WriteWord( dev, 0x40022010, 0 ); //  FLASH->CTLR = 0x40022010
	}
	else
	{
		// 16.4.7, Step 3: Check the BSY bit of the FLASH_STATR register to confirm that there are no other programming operations in progress.
		// skip (we make sure at the end)

		int chunk_to_erase = address;

		while( chunk_to_erase < address + length )
		{
			if( WaitForFlash( dev ) ) return -14;

			// Step 4:  set PAGE_ER of FLASH_CTLR(0x40022010)
			WriteWord( dev, 0x40022010, CR_PAGE_ER ); // Actually FTER //  FLASH->CTLR = 0x40022010

			// Step 5: Write the first address of the fast erase page to the FLASH_ADDR register.
			WriteWord( dev, 0x40022014, chunk_to_erase  ); // FLASH->ADDR = 0x40022014

			// Step 6: Set the STAT bit of FLASH_CTLR register to '1' to initiate a fast page erase (64 bytes) action.
			WriteWord( dev, 0x40022010, CR_STRT_Set|CR_PAGE_ER );  // FLASH->CTLR = 0x40022010
			if( WaitForFlash( dev ) ) return -15;

			WriteWord( dev, 0x40022010, 0 ); //  FLASH->CTLR = 0x40022010 (Disable any pending ops)
			chunk_to_erase+=64;
		}
	}
	return 0;
}

static int WriteBlock( struct SWIOState * iss, uint32_t address_to_write, uint8_t * blob, uint8_t len, uint8_t erase )
{
	struct SWIOState * dev = iss;

	if( DetermineChipTypeAndSectorInfo( iss, NULL ) ) return -9;

	const int blob_size = len;
	uint32_t wp = address_to_write;
	uint32_t ew = wp + blob_size;
	int group = -1;
	int is_flash = 0;
	int rw = 0;

	if( (address_to_write & 0xff000000) == 0x08000000 || (address_to_write & 0xff000000) == 0x00000000  || (address_to_write & 0x1FFF0000) == 0x1FFF0000  )
	{
		// Need to unlock flash.
		// Flash reg base = 0x40022000,
		// FLASH_MODEKEYR => 0x40022024
		// FLASH_KEYR => 0x40022004

		if( !iss->flash_unlocked )
		{
			if( ( rw = UnlockFlash( dev ) ) )
				return rw;
		}

		is_flash = 1;

		if( erase )
		{
			// Only erase on first block in sector.
			int block_in_sector = (wp & ( iss->sectorsize - 1 )) / blob_size;
			int is_first_block = block_in_sector == 0;

			if( is_first_block )
			{
				rw = EraseFlash( dev, address_to_write, blob_size, 0 );
				if( rw ) return rw;
				// 16.4.6 Main memory fast programming, Step 5
				//if( WaitForFlash( dev ) ) return -11;
				//WriteWord( dev, 0x40022010, FLASH_CTLR_BUF_RST );
				//if( WaitForFlash( dev ) ) return -11;
			}
		}
	}

	/* General Note:
		Most flash operations take about 3ms to complete :(
	*/

	while( wp < ew )
	{
		if( is_flash )
		{
			group = (wp & 0xffffffc0);

			int block_in_sector = (group & ( iss->sectorsize - 1 )) / blob_size;
			int is_first_block = block_in_sector == 0;
			int is_last_block = block_in_sector == (iss->sectorsize / blob_size - 1 );

			if( is_first_block )
			{
				if( dev->target_chip_type == CHIP_CH32V20x || dev->target_chip_type == CHIP_CH32V30x )
				{
					// No bufrst on v20x, v30x
					WaitForFlash( dev );
					WriteWord( dev, 0x40022010, CR_PAGE_PG ); // THIS IS REQUIRED, (intptr_t)&FLASH->CTLR = 0x40022010
					//FTPG ==  CR_PAGE_PG   == ((uint32_t)0x00010000)
				}
				else
				{
					// V003, x035, maybe more.
					WriteWord( dev, 0x40022010, CR_PAGE_PG ); // THIS IS REQUIRED, (intptr_t)&FLASH->CTLR = 0x40022010
					WriteWord( dev, 0x40022010, CR_BUF_RST | CR_PAGE_PG );  // (intptr_t)&FLASH->CTLR = 0x40022010
				}
				WaitForFlash( dev );
			}

			int j;
			for( j = 0; j < blob_size/4; j++ )
			{
				int index = (wp-address_to_write);
				uint32_t data = 0xffffffff;
				if( index + 3 < blob_size )
				{
					memcpy( &data, &blob[index], 4 );
					// data = ((uint32_t*)blob)[index/4]; // CAN'T DO THAT - results in misaligned memory read and crashes the programmer
				}
				else if( (int32_t)(blob_size - index) > 0 )
				{
					memcpy( &data, &blob[index], blob_size - index );
				}
				if( iss->target_chip_type == CHIP_CH32V10x && !((j+1)&3) )
				{
					// Signal to WriteWord that we need to do a buffer load
					MCFWriteReg32( dev, BDMDATA0, 1 );
					MCFWriteReg32( dev, DMCOMMAND, 0x0023100f );
				}
				WriteWord( dev, wp, data );
				wp += 4;
			}


			if( ( iss->target_chip_type == CHIP_CH32V20x || iss->target_chip_type == CHIP_CH32V30x ) )
			{
				if( is_last_block )
				{
					WriteWord( dev, 0x40022010, CR_PAGE_PG | (1<<21) ); // Page Start
					if( WaitForFlash( dev ) ) return -13;
				}
			}
			else
			{
				// Datasheet says the x03x needs to have this called every group-of-16, but that's not true, it should be every 16-words.
				WriteWord( dev, 0x40022014, group );  //0x40022014 -> FLASH->ADDR
				WriteWord( dev, 0x40022010, CR_PAGE_PG|CR_STRT_Set ); // 0x40022010 -> FLASH->CTLR
				if( WaitForFlash( dev ) ) return -16;
			}
		}
		else
		{
			int index = (wp-address_to_write);
			uint32_t data = 0xffffffff;
			if( index + 3 < blob_size )
			{
				memcpy( &data, &blob[index], 4 );
			}
			else if( (int32_t)(blob_size - index) > 0 )
			{
				memcpy( &data, &blob[index], blob_size - index );
			}
			WriteWord( dev, wp, data );
			wp += 4;
		}
	}

	return 0;
}

// Polls up to 7 bytes of printf, and can leave a 7-bit flag for the CH32V003.
static int PollTerminal( struct SWIOState * iss, uint8_t * buffer, int maxlen, uint32_t leavevalA, uint32_t leavevalB )
{
	struct SWIOState * dev = iss;

	int r;
	uint32_t rr;
	if( iss->statetag != STTAG( "TERM" ) )
	{
		MCFWriteReg32( dev, DMABSTRACTAUTO, 0x00000000 ); // Disable Autoexec.
		iss->statetag = STTAG( "TERM" );
	}
	r = MCFReadReg32( dev, BDMDATA0, &rr );
	if( r < 0 ) return r;

	if( maxlen < 8 ) return -9;

	// BDMDATA1:
	//  bit  7 = host-acknowledge.
	if( rr & 0x80 )
	{
		int ret = 0;
		int num_printf_chars = (rr & 0xf)-4;

		if( num_printf_chars > 0 && num_printf_chars <= 7)
		{
			if( num_printf_chars > 3 )
			{
				uint32_t r2;
				r = MCFReadReg32( dev, BDMDATA1, &r2 );
				memcpy( buffer+3, &r2, num_printf_chars - 3 );
			}
			int firstrem = num_printf_chars;
			if( firstrem > 3 ) firstrem = 3;
			memcpy( buffer, ((uint8_t*)&rr)+1, firstrem );
			buffer[num_printf_chars] = 0;
			ret = num_printf_chars;
		}
		if( leavevalA )
		{
			MCFWriteReg32( dev, BDMDATA1, leavevalB );
		}
		MCFWriteReg32( dev, BDMDATA0, leavevalA ); // Write that we acknowledge the data.
		return ret;
	}
	else
	{
		return 0;
	}
}

#endif

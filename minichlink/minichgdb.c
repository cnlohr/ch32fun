// This file is loosely based on aappleby's GDBServer.

// Connect in with:
//   gdb-multiarch -ex "set debug remote 1" -ex "target extended-remote :3333" ./blink.elf 
//   gdb-multiarch -ex "target extended-remote :3333" ./blink.elf 

#include "minichlink.h"
#include "chips.h"

#define MICROGDBSTUB_IMPLEMENTATION
#define MICROGDBSTUB_SOCKETS
#define MICROGDBSTUB_PORT 3333

const char* MICROGDBSTUB_MEMORY_MAP = "l<?xml version=\"1.0\"?>"
"<!DOCTYPE memory-map PUBLIC \"+//IDN gnu.org//DTD GDB Memory Map V1.0//EN\" \"http://sourceware.org/gdb/gdb-memory-map.dtd\">"
"<memory-map>"
"  <memory type=\"flash\" start=\"0x00000000\" length=\"0x%x\">"
"    <property name=\"blocksize\">%d</property>"
"  </memory>"
"  <memory type=\"ram\" start=\"0x20000000\" length=\"0x%x\">"
"    <property name=\"blocksize\">1</property>"
"  </memory>"
"  <memory type=\"ram\" start=\"0x40000000\" length=\"0x10000000\">"
"    <property name=\"blocksize\">4</property>"
"  </memory>"
"  <memory type=\"ram\" start=\"0xe0000000\" length=\"0x10000000\">"
"    <property name=\"blocksize\">4</property>"
"  </memory>"
"</memory-map>";

#include "microgdbstub.h"

void SendReplyFull( const char * replyMessage );

///////////////////////////////////////////////////////////////////////////////
// Actual Chip Operations

// Several pieces from picorvd. https://github.com/aappleby/PicoRVD/
int shadow_running_state = 1;
int last_halt_reason = 5;
uint32_t backup_regs[33]; //0..15 + PC, or 0..32 + PC
int gdbasserting_break = 0;

#define MAX_SOFTWARE_BREAKPOINTS 128
int num_software_breakpoints = 0;
uint8_t  software_breakpoint_type[MAX_SOFTWARE_BREAKPOINTS]; // 0 = not in use, 1 = 32-bit, 2 = 16-bit.
uint32_t software_breakpoint_addy[MAX_SOFTWARE_BREAKPOINTS];
uint32_t previous_word_at_breakpoint_address[MAX_SOFTWARE_BREAKPOINTS];

int IsGDBServerInShadowHaltState( void * dev ) { return !shadow_running_state; }

static int InternalClearFlashOfSoftwareBreakpoint( void * dev, int i );
static int InternalWriteBreakpointIntoAddress( void * v, int i );


void RVCommandPrologue( void * dev )
{
	if( !MCF.ReadCPURegister )
	{
		fprintf( stderr, "Error: Programmer does not support register reading\n" );
		exit( -5 );
	}

	MCF.WriteReg32( dev, DMABSTRACTAUTO, 0 );     // Disable autoexec.
	if( MCF.ReadAllCPURegisters( dev, backup_regs ) )
	{
		fprintf( stderr, "WARNING: failed to preserve registers\n" );
	}
	MCF.VoidHighLevelState( dev );
}

void RVCommandEpilogue( void * dev )
{
	MCF.WriteReg32( dev, DMABSTRACTAUTO, 0 );   // Disable autoexec.
	MCF.WriteAllCPURegisters( dev, backup_regs );
	MCF.VoidHighLevelState( dev );
	MCF.WriteReg32( dev, DMDATA0, 0 );
}

void RVCommandResetPart( void * dev , int mode)
{
	MCF.HaltMode( dev, mode );
	RVCommandPrologue( dev );
}

void RVNetConnect( void * dev )
{
	// ??? Should we actually halt?
	MCF.HaltMode( dev, 5 );
	MCF.SetEnableBreakpoints( dev, 1, 0 );
	RVCommandPrologue( dev );
	shadow_running_state = 0;
}

int RVSendGDBHaltReason( void * dev )
{ 
	char st[5];
	if( gdbasserting_break )
	{
		gdbasserting_break = 0;
		sprintf( st, "T%02x", 2 );
		SendReplyFull( st );
		return 0;
	}
	sprintf( st, "T%02x", last_halt_reason );
	SendReplyFull( st );
	return 0;
}

void RVNetPoll(void * dev )
{
	if( !MCF.ReadReg32 )
	{
		fprintf( stderr, "Error: Can't poll GDB because no ReadReg32 supported on this programmer\n" );
		return;
	}

	uint32_t status;
	if( MCF.ReadReg32( dev, DMSTATUS, &status ) )
	{
		fprintf( stderr, "Error: Could not get part status\n" );
		return;
	}
	int statusrunning = ((status & (1<<10)));
	
	static int laststatus;
	if( status != laststatus )
	{
		//printf( "DMSTATUS: %08x => %08x\n", laststatus, status );
		laststatus = status;
	}
	if( statusrunning != shadow_running_state )
	{
		// If was running but now is halted.
		if( statusrunning == 0 )
		{
			RVCommandPrologue( dev );
			last_halt_reason = 5;//((dscr>>6)&3)+5;
			RVSendGDBHaltReason( dev );
		}
		else
		{
			// this is the reply to 's' or 'c' packets.
			SendReplyFull( "OK" );
		}
		shadow_running_state = statusrunning;
	}
}

int RVGetNumRegisters( void * dev )
{
	struct InternalState * iss = (struct InternalState*)(((struct ProgrammerStructBase*)dev)->internal);
	return iss->nr_registers_for_debug;
}

int RVReadCPURegister( void * dev, int regno, uint32_t * regret )
{
	struct InternalState * iss = (struct InternalState*)(((struct ProgrammerStructBase*)dev)->internal);
	int nrregs = iss->nr_registers_for_debug;

	if( shadow_running_state )
	{
		MCF.HaltMode( dev, 5 );
		RVCommandPrologue( dev );
		shadow_running_state = 0;
	}

	if( nrregs == 16 )
	{
		if( regno == 32 ) regno = 16; // Hack - Make 32 also 16 for old GDBs.
		if( regno > 16 ) return 0; // Invalid register.
	}
	else
	{
		if( regno > nrregs ) return 0;
	}

	*regret = backup_regs[regno];
	return 0;
}


int RVWriteCPURegister( void * dev, int regno, uint32_t value )
{
	struct InternalState * iss = (struct InternalState*)(((struct ProgrammerStructBase*)dev)->internal);
	int nrregs = iss->nr_registers_for_debug;

	if( shadow_running_state )
	{
		MCF.HaltMode( dev, 5 );
		RVCommandPrologue( dev );
		shadow_running_state = 0;
	}

	if( nrregs == 16 )
	{
		if( regno == 32 ) regno = 16; // Hack - Make 32 also 16 for old GDBs.
		if( regno > 16 ) return 0; // Invalid register.
	}
	else
	{
		if( regno > nrregs ) return 0;
	}

	backup_regs[regno] = value;

	if( !MCF.WriteAllCPURegisters )
	{
		fprintf( stderr, "ERROR: MCF.WriteAllCPURegisters is not implemented on this platform\n" );
		return -99;
	}

	int r;
	if( ( r = MCF.WriteAllCPURegisters( dev, backup_regs ) ) )
	{
		fprintf( stderr, "Error: WriteAllCPURegisters failed (%d)\n", r );
		return r;
	}
	return 0;
}

int RVDebugExec( void * dev, enum HaltResetResumeType halt_reset_or_resume, int resume_from_other_address, uint32_t address )
{
	struct InternalState * iss = (struct InternalState*)(((struct ProgrammerStructBase*)dev)->internal);
	int nrregs = iss->nr_registers_for_debug;

	if( !MCF.HaltMode )
	{
		fprintf( stderr, "Error: Can't alter halt mode with this programmer.\n" );
		exit( -6 );
	}
	
	if( halt_reset_or_resume == HALT_TYPE_SINGLE_STEP )
	{
		MCF.SetEnableBreakpoints( dev, 1, 1 );
		RVCommandEpilogue( dev );
		MCF.HaltMode( dev, HALT_MODE_RESUME );
		MCF.HaltMode( dev, HALT_MODE_HALT_BUT_NO_RESET );
		RVCommandPrologue( dev );
		MCF.SetEnableBreakpoints( dev, 1, 0 );
		//printf( "STEP PC: %08x\n", backup_regs[iss->nr_registers_for_debug] );
		return 0;
	}

	// Special case halt_reset_or_resume = 4: Skip instruction and resume.
	if( halt_reset_or_resume == HALT_TYPE_CONTINUE_WITH_SIGNAL || halt_reset_or_resume == HALT_TYPE_CONTINUE )
	{
		// First see if we already know about this breakpoint
		int matchingbreakpoint = -1;
		// For this we want to advance PC.
		uint32_t exceptionptr = backup_regs[nrregs];
		uint32_t instruction = 0;

		int i;
		for( i = 0; i < MAX_SOFTWARE_BREAKPOINTS; i++ )
		{
			if( exceptionptr == software_breakpoint_addy[i] && software_breakpoint_type[i] )
			{
				matchingbreakpoint = i;
			}
		}

		if( matchingbreakpoint >= 0 )
		{
			// This is a known breakpoint.  Need to set it back.  Single Step.  Then continue.
			InternalClearFlashOfSoftwareBreakpoint( dev, matchingbreakpoint );
			MCF.SetEnableBreakpoints( dev, 1, 1 );
			InternalWriteBreakpointIntoAddress( dev, matchingbreakpoint );
		}
		else
		{
			// Unknown breakpoint (was originally in the firmware)
			// Just proceed past it.
			if( exceptionptr & 2 )
			{
				uint32_t part1, part2;
				MCF.ReadWord( dev, exceptionptr & ~3, &part1 );
				MCF.ReadWord( dev, (exceptionptr & ~3)+4, &part2 );
				instruction = (part1 >> 16) | (part2 << 16);
			}
			else
			{
				MCF.ReadWord( dev, exceptionptr, &instruction );
			}
			if( instruction == 0x00100073 )
				backup_regs[nrregs]+=4;
			else if( ( instruction & 0xffff ) == 0x9002 )
				backup_regs[nrregs]+=2;
			else
				; //No change, it is a normal instruction.

			if( halt_reset_or_resume == HALT_TYPE_CONTINUE_WITH_SIGNAL )
			{
				MCF.SetEnableBreakpoints( dev, 1, 1 );
			}
		}

		halt_reset_or_resume = HALT_MODE_RESUME;
	}

	if( shadow_running_state != ( halt_reset_or_resume >= HALT_TYPE_CONTINUE ) )
	{
		if( halt_reset_or_resume < HALT_TYPE_CONTINUE )
		{
			RVCommandPrologue( dev );
		}
		else
		{
			RVCommandEpilogue( dev );
		}

		MCF.HaltMode( dev, halt_reset_or_resume );
	}

	shadow_running_state = halt_reset_or_resume >= HALT_TYPE_CONTINUE;
	return 0;
}

int RVReadMem( void * dev, uint32_t memaddy, uint8_t * payload, int len )
{
	if( !MCF.ReadBinaryBlob )
	{
		fprintf( stderr, "Error: Can't alter halt mode with this programmer.\n" );
		exit( -6 );
	}
	int ret = MCF.ReadBinaryBlob( dev, memaddy, len, payload );
	//printf( "Read Mem: %08x %d\n", memaddy, len );
	//int i;
	//for( i = 0; i < len; i++ )
	//{
	//	printf( "%02x%c", payload[i], ((i&15)==15)?'\n':' ' );
	//}
	//printf( "\n" );
	if( ret < 0 )
	{
		fprintf( stderr, "Error reading binary blob at %08x\n", memaddy );
	}
	return ret;
}

static int InternalClearFlashOfSoftwareBreakpoint( void * dev, int i )
{
	int r;
	if( software_breakpoint_type[i] == 1 )
	{
		//32-bit instruction
		r = MCF.WriteBinaryBlob( dev, software_breakpoint_addy[i], 4, (uint8_t*)&previous_word_at_breakpoint_address[i] );
	}
	else
	{
		//16-bit instruction
		r = MCF.WriteBinaryBlob( dev, software_breakpoint_addy[i], 2, (uint8_t*)&previous_word_at_breakpoint_address[i] );
	}

	return r;
}

static int InternalWriteBreakpointIntoAddress( void * dev, int i )
{
	int r;
	uint32_t address = software_breakpoint_addy[i];
	if( software_breakpoint_type[i] == 1 )
	{
		//32-bit instruction
		uint32_t ebreak = 0x00100073; // ebreak
		r = MCF.WriteBinaryBlob( dev, address, 4, (uint8_t*)&ebreak );
	}
	else
	{
		//16-bit instruction
		uint32_t ebreak = 0x9002; // c.ebreak
		r = MCF.WriteBinaryBlob( dev, address, 2, (uint8_t*)&ebreak );
	}
	return r;
}

static int InternalDisableBreakpoint( void * dev, int i )
{
	int r;
	r = InternalClearFlashOfSoftwareBreakpoint( dev, i );
	previous_word_at_breakpoint_address[i] = 0;
	software_breakpoint_type[i] = 0;
	software_breakpoint_addy[i] = 0;
	return r;
}

int RVHandleBreakpoint( void * dev, int set, uint32_t address )
{
	int i;
	int first_free = -1;
	for( i = 0; i < MAX_SOFTWARE_BREAKPOINTS; i++ )
	{
		if( software_breakpoint_type[i] && software_breakpoint_addy[i] == address )
			break;
		if( first_free < 0 && software_breakpoint_type[i] == 0 )
			first_free = i;
	}

	if( i != MAX_SOFTWARE_BREAKPOINTS )
	{
		// There is already a break slot here.
		if( !set )
		{
			InternalDisableBreakpoint( dev, i );
		}
		else
		{
			// Already set.
		}
	}
	else
	{
		if( first_free == -1 )
		{
			fprintf( stderr, "Error: Too many breakpoints\n" );
			return -1;
		}
		if( set )
		{
			i = first_free;
			uint32_t readval_at_addy;
			int r = MCF.ReadBinaryBlob( dev, address, 4, (uint8_t*)&readval_at_addy );
			if( r ) return -5;
			if( ( readval_at_addy & 3 ) == 3 ) // Check opcode LSB's.
			{
				// 32-bit instruction.
				software_breakpoint_type[i] = 1;
				software_breakpoint_addy[i] = address;
				previous_word_at_breakpoint_address[i] = readval_at_addy;
			}
			else
			{
				// 16-bit instructions
				software_breakpoint_type[i] = 2;
				software_breakpoint_addy[i] = address;
				previous_word_at_breakpoint_address[i] = readval_at_addy & 0xffff;
			}
			InternalWriteBreakpointIntoAddress( dev, i );
		}
		else
		{
			// Already unset.
		}
	}

	return 0;
}

int RVWriteRAM(void * dev, uint32_t memaddy, uint32_t length, uint8_t * payload )
{
	if( !MCF.WriteBinaryBlob )
	{
		fprintf( stderr, "Error: Can't alter halt mode with this programmer.\n" );
		exit( -6 );
	}

	int r = MCF.WriteBinaryBlob( dev, memaddy, length, payload );

	return r;
}

int RVWriteFlash(void * dev, uint32_t memaddy, uint32_t length, uint8_t * payload )
{
	struct InternalState * iss = (struct InternalState*)(((struct ProgrammerStructBase*)dev)->internal);

	if( iss->target_chip && (iss->target_chip->protocol == PROTOCOL_DEFAULT) && (( memaddy & 0xff000000 ) == 0) )
	{
		memaddy |= 0x08000000; // Only applies to CH32 chips
	}
	return RVWriteRAM( dev, memaddy, length, payload );
}

int RVErase( void * dev, uint32_t memaddy, uint32_t length )
{
	if( !MCF.Erase )
	{
		fprintf( stderr, "Error: Can't alter halt mode with this programmer.\n" );
		exit( -6 );
	}

	int r = MCF.Erase( dev, memaddy, length, 0 ); // 0 = not whole chip.
	return r;
}

void RVHandleDisconnect( void * dev )
{
	MCF.HaltMode( dev, 5 );
	MCF.SetEnableBreakpoints( dev, 0, 0 );

	int i;
	for( i = 0; i < MAX_SOFTWARE_BREAKPOINTS; i++ )
	{
		if( software_breakpoint_type[i]  )
		{
			InternalDisableBreakpoint( dev, i );
		}
	}

	if( shadow_running_state == 0 )
	{
		RVCommandEpilogue( dev );
	}
	MCF.HaltMode( dev, 2 );
	shadow_running_state = 1;
}

void RVHandleGDBBreakRequest( void * dev )
{
	MCF.HaltMode( dev, 5 );
}

void RVHandleUnsolicitedGDBBreakRequest( void * dev )
{
	fprintf( stderr, "Invoke Unsolicited Break\n" );
	MCF.HaltMode( dev, 5 );
	gdbasserting_break = 1;
}

int PollGDBServer( void * dev )
{
	return MicroGDBPollServer( dev );
}

void ExitGDBServer( void * dev )
{
	MicroGDBExitServer( dev );
}


int SetupGDBServer( void * dev )
{
	return MicroGDBStubStartup( dev );
}

void RVHandleKillRequest( void * dev )
{
	// Do nothing.
}



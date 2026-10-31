// UPDI protocol implementation for use with funprog
//
// Copyright 2024 Charles Lohr, May be licensed under the MIT/x11 or NewBSD
// licenses.  You choose.  (Can be included in commercial and copyleft work)
//
// This file is original work.
//

// General notes:
//   Default UPDI Setting is UPDI @ 4MHz
//     -> 0.75 - 225kbps
//   Let's target 100 kbps.

#ifndef FUNPROG_UPDI_H
#define FUNPROG_UPDI_H

#define UPDI_WORD_ACK   0xe80
#define UPDI_WORD_REST  0x000 // RESET or like a RESET BREAK

struct UPDIState
{
	int pinmask;
	int pinmaskpower;
	int clocks_per_bit;
	int baudrate;
	int cpumhz;
};

static int UPDIComputeClocksPerBit( int cpumhz, int baudrate );
static int UPDISetup( struct UPDIState* updi, uint8_t sib[16] );
static inline void UPDIPowerOff( struct UPDIState* updi );
static inline void UPDIPowerOn( struct UPDIState* updi );
static int UPDIErase( struct UPDIState* updi );
static int UPDIReset( struct UPDIState* updi );
static int UPDIFlash( struct UPDIState* updi, int memory_address, uint8_t * program, int len, int flash_bootloader );
static int UPDIReadMemoryArea( struct UPDIState* updi, int memory_location, uint8_t * program, int len );

// Returns 0 if device detected.  Returns nonzero if failure.
static inline int UPDISyncStart( struct UPDIState* updi );

static inline void UPDISendWord( struct UPDIState* updi, uint32_t message )
{
	int parity = (message ^ (message>>4)) & 0xf;
	message <<= 1;
	message |= 0xc00;
	message |= (0xD32C00 >> parity) & 0x200;
	UPDIWordInternal( updi, message );
}

static inline void UPDISendString( struct UPDIState* updi, const uint8_t * str, int len )
{
	int i;
	for( i = 0; i < len; i++ )
		UPDISendWord( updi, str[i] );
}

static int UPDITransact( struct UPDIState* updi, uint8_t command, int expect_return, uint8_t * ret )
{
	UPDISendWord( updi, 0x55 );
	UPDISendWord( updi, command );

	int i;
	for( i = 0; i < expect_return; i++ )
	{
		int r = UPDIReceive( updi );
		if( r < 0 )
		{
			return r;
		}
		ret[i] = r;
	}

	DELAY_US(100);

	return i;
}

#define UPDI_MEM_INCREMENT 0x4

static int UPDICheckAddress( struct UPDIState* updi, int * updiptr, int flags, int address )
{
	int r = 0x40;
	if( !updiptr || address != *updiptr )
	{
		UPDISendWord( updi, 0x55 );
		UPDISendWord( updi, 0x69 );
		UPDISendWord( updi, (address)&0xff );
		UPDISendWord( updi, (address>>8) );
		r = UPDIReceive( updi );
		DELAY_US(100);
	}

	if( updiptr )
	{
		*updiptr = address  + !!( flags & UPDI_MEM_INCREMENT );
	}

	return r != 0x40;
}

static int UPDIReadMemory( struct UPDIState* updi, int * updiptr, int flags, int address  )
{
	UPDICheckAddress( updi, updiptr, flags, address );

	UPDISendWord( updi, 0x55 );
	UPDISendWord( updi, 0x20 + flags );
	int r = UPDIReceive( updi );
	DELAY_US(100);
	return r;
}

static int UPDIWriteMemory( struct UPDIState* updi, int * updiptr, int flags, int address, int data )
{
	UPDICheckAddress( updi, updiptr, flags, address );

	UPDISendWord( updi, 0x55 );
	UPDISendWord( updi, 0x60 + flags );
	UPDISendWord( updi, data & 0xff );
	int r = UPDIReceive( updi );
	DELAY_US(100);
	return r != 0x40;
}

static int UPDIComputeClocksPerBit( int cpumhz, int baudrate )
{
	return ( cpumhz * 1000000 + (baudrate/2) ) / baudrate;
}

static int UPDISetup( struct UPDIState* updi, uint8_t sib[16] )
{
	int r;
	int tries = 0;

retry:
	r = UPDISyncStart( updi );
	if( r )
		return r;

	UPDISendBreak( updi );

	// Receive SIB
	r = UPDITransact( updi, 0xe5, 16, sib );
	if( r != 16 )
	{
		if( tries++ < 10 ) goto retry;
		return r;
	}

	UPDISendString( updi, (const uint8_t*)"\x55\xc2\x05", 3 ); // STCS, UPDI.CTRLA = 0x6 (very low guard time)

retry2:
	// Unlock flash
	UPDISendWord( updi, 0x55 );
	UPDISendWord( updi, 0xe0 );
	UPDISendString( updi, (const uint8_t*)" gorPMVN", 8 ); //NVMErase", backwards.

	uint8_t csret = 0;
	r = UPDITransact( updi, 0x87, 1, &csret );  //LDCS, UPDI.ASI_KEY_STATUS -> Look for NVMPROG

	if( r != 1 )
		return -12;

	if( ! (csret & 0x10) ) // NVMPROG
	{
		if( tries++ < 10 )
		{
			UPDISendString( updi, (const uint8_t*)"\x55\xc8\x59", 3 ); // STCS, UPDI.ASI_RESET_REQ = 0x59
			UPDISendString( updi, (const uint8_t*)"\x55\xc8\x00", 3 ); // STCS, UPDI.ASI_RESET_REQ = 0x00 Clear reset request.
			goto retry2;
		}
		return -13;
	}

retry3:
	UPDISendString( updi, (const uint8_t*)"\x55\xc8\x59", 3 ); // STCS, UPDI.ASI_RESET_REQ = 0x59
	UPDISendString( updi, (const uint8_t*)"\x55\xc8\x00", 3 ); // STCS, UPDI.ASI_RESET_REQ = 0x00 Clear reset request.

	int retry;
	const int max_retry = 200;
	for( retry = 0; retry < max_retry; retry++ )
	{
		r = UPDITransact( updi, 0x8B, 1, &csret );  //LDCS, UPDI.ASI_SYS_STATUS -> Look for NVMPROG
		if( r != 1 )
			return r;
		if( (csret & 0x08) ) //NVMPROG
			break;
	}

	if( retry == max_retry )
	{
		if( tries++ < 10 ) goto retry3;
		return -15;
	}

#if 0
	int to_max = 10;
	int to;
	int memory_offset = 0x8000;
	int updi_ptr = -1;

	// First, check BOOTEND.
	if( !flash_bootloader )
	{
		r = UPDIReadMemory( updi, &updi_ptr, 0, 0x1280 /*FUSE*/ + 8 /* BOOTEND */ );
		if( r < 0 )
			return r;
		memory_offset += r<<8;
	}
#endif

	return 0;
}

static int UPDIErase( struct UPDIState* updi )
{
	int tries = 0;

retrya:
	UPDISendWord( updi, 0x55 );
	UPDISendWord( updi, 0xe0 );
	UPDISendString( updi, (const uint8_t*)"esarEMVN", 8 ); //NVMErase", backwards.

	uint8_t csret = 0;
	int r = UPDITransact( updi, 0x87, 1, &csret );  //LDCS, UPDI.ASI_KEY_STATUS -> Look for CHIPERASE

	if( r != 1 )
	{
		if( tries++ < 10 ) goto retrya;
		return -12;
	}

	if( !(csret & 0x08) )
		return -13;

	UPDISendString( updi, (const uint8_t*)"\x55\xc8\x59", 3 ); // STCS, UPDI.ASI_RESET_REQ = 0x59
	UPDISendString( updi, (const uint8_t*)"\x55\xc8\x00", 3 ); // STCS, UPDI.ASI_RESET_REQ = 0x00 Clear reset request.

	int retry;
	const int max_retry = 200;
	for( retry = 0; retry < max_retry; retry++ )
	{
		DELAY_US(10000);	
		r = UPDITransact( updi, 0x8B, 1, &csret );  //LDCS, UPDI.ASI_SYS_STATUS -> Look for LOCKSTATUS
		if( r != 1 )
			return r;
		if( !(csret & 0x1) )
			break;
	}

	if( retry == max_retry )
		return -13;

	return 0;
}

static int UPDIFlash( struct UPDIState* updi, int memory_address, uint8_t * program, int len, int flash_bootloader )
{
	int r;
	const int blocksize = 64;
	int updi_ptr = -1;
	int to_max = 10;
	int to;
	int offset = 0;
	
	int block = 0;
	for( block = 0; block < ( len + blocksize - 1 ) / blocksize; block++ )
	{
		r = UPDIWriteMemory( updi, &updi_ptr, UPDI_MEM_INCREMENT, 0x1000 + 8 /*NVMCTRL.ADDR*/, memory_address & 0xff );
		r = UPDIWriteMemory( updi, &updi_ptr, UPDI_MEM_INCREMENT, 0x1000 + 9 /*NVMCTRL.ADDR*/, memory_address >> 8 );

		// Page size is 64 bytes.
		r = UPDIWriteMemory( updi, &updi_ptr, UPDI_MEM_INCREMENT, 0x1000 + 0 /*NVMCTRL.CTRLA*/, 0x04 ); // Page Buffer Clear

		int bo;
		for( bo = 0; bo < blocksize; bo++ )
			r = UPDIWriteMemory( updi, &updi_ptr, UPDI_MEM_INCREMENT, memory_address + bo, ((bo+offset)<len)?program[bo + offset]:0 );

		r = UPDIWriteMemory( updi, &updi_ptr, UPDI_MEM_INCREMENT, 0x1000 + 0 /*NVMCTRL.CTRLA*/, 0x03 ); // erase + write page

		for( to = 0; to < to_max; to++ )
		{
			r = UPDIReadMemory( updi, &updi_ptr, 0, 0x1000 + 2 );
			if( r & 0x40 ) continue; // Invalid Read
			if( r & 0x01 ) continue; // FBUSY
			break;
		}

		r = UPDIReadMemory( updi, &updi_ptr, 0, 0x1000 + 2 );
		if( r != 0x00 || to == to_max )
			return -19;

		offset += blocksize;
		memory_address += blocksize;
	}

	return 0;
}

static int UPDIReadMemoryArea( struct UPDIState* updi, int memory_location, uint8_t * program, int len )
{
	int i, r;
	int updi_ptr = -1;
	for( i = 0; i < len; i++ )
	{
		r = UPDIReadMemory( updi, &updi_ptr, UPDI_MEM_INCREMENT, memory_location + i );
		if( r < 0 ) return r;
		program[i] = r;
	}
	return 0;
}

static int UPDIReset( struct UPDIState* updi )
{
	// Reset chip.
	UPDISendString( updi, (const uint8_t*)"\x55\xc8\x59", 3 ); // STCS, UPDI.ASI_RESET_REQ = 0x59
	UPDISendString( updi, (const uint8_t*)"\x55\xc8\x00", 3 ); // STCS, UPDI.ASI_RESET_REQ = 0x00 Clear reset request.

	int retry;
	const int max_retry = 100;
	uint8_t csret = 0;
	int r = 0;

	for( retry = 0; retry < max_retry; retry++ )
	{
		r = UPDITransact( updi, 0x8B, 1, &csret );  //LDCS, UPDI.ASI_SYS_STATUS -> Look for NVMPROG
		if( r != 1 )
			return r;
		if( !(csret & 0x20) ) //RSTSYS
			break;
	}
	return 0;
}

#endif

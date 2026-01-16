#include "ch32fun.h"
#include <stdio.h>

#define RANDOM_STRENGTH 2
#include "lib_rand.h"

#define CANARY 0xF0CACC1A

// stack start and end defined in the linker script
extern uint32_t _eusrstack;
extern uint32_t end;

static inline void *get_stack_pointer()
{
	register void *sp asm( "sp" );
	return sp;
}


void print_stacktrace()
{
	volatile uintptr_t *fp;
	asm volatile( "mv %0, s0" : "=r"( fp ) );

	int depth = 0;

	printf( "Stack Trace:\n" );
	while ( fp )
	{
		uintptr_t ra = *( fp - 1 );
		uintptr_t *nextfp = (void *)*( fp - 2 );

		printf( "  Frame %d: FP=0x%x, RA=0x%x\n", depth, (uintptr_t)nextfp, ra );

		fp = (void *)nextfp;
		if ( fp == (uintptr_t *)&_eusrstack )
		{
			printf( "Done\n" );
			break;
		}

		if ( ++depth > 20 )
		{
			printf( "Stack trace depth limit reached.\n" );
			break;
		}
	}
}

void fill_canary( uint32_t canary )
{
	uint32_t *p = get_stack_pointer();
	printf( "Filling canary from 0x%lx to 0x%lx\n", (uint32_t)p, (uint32_t)&end );

	while ( --p > &end )
	{
		*p = canary;
	}
}

void *get_stack_watermark( uint32_t canary )
{
	const uint32_t *const top = get_stack_pointer();
	uint32_t *p = &end;
	while ( ++p < top )
	{
		if ( *p != canary )
		{
			break;
		}
	}
	return p;
}

void test_function( int depth )
{
	if ( ( rand() & 0xf ) != 0 )
	{
		test_function( depth + 1 );
	}
	else
	{
		printf( "Reached depth %d, printing stack trace:\n", depth );
		print_stacktrace();
	}
}

int main()
{
	SystemInit();
	funAnalogInit();
	seed( funAnalogRead( 1 ) );

	void *const initial_sp = get_stack_pointer();

	printf( "Initial SP:  0x%lx\n", (uint32_t)initial_sp );

	printf( "Stack Start: 0x%lx\n", (uint32_t)&_eusrstack );
	printf( "Stack End:   0x%lx\n", (uint32_t)&end );

	fill_canary( CANARY );

	printf( "Hello, Stack Usage!\n" );

	const void *const used_stack = get_stack_watermark( CANARY );
	const size_t used_bytes = (size_t)initial_sp - (size_t)used_stack;
	printf( "Used Stack: %u bytes\n", (unsigned int)used_bytes );

	test_function( 0 );

	while ( 1 )
		;
}

#include "ch32fun.h"
#include <stdio.h>

typedef size_t ( *TestFunc_t )( size_t );

size_t Benchmark( const char *tag, TestFunc_t func, size_t param )
{
	const size_t start = SysTick->CNT;
	const size_t result = func( param );
	const size_t end = SysTick->CNT;

	const size_t ticks = end - start;

	printf( "%s@%08X: took %u ticks, result %u\n", tag, (size_t *)func, ticks, result );

	return ticks;
}

// function placed in slower memory section
__attribute__( ( section( ".external" ) ) ) size_t Slow( size_t b )
{

	size_t sum = 0;
	for ( size_t i = 0; i < b; i++ )
	{
		sum += i & 1 ? i : b;
	}

	return sum;
}

size_t Fast( size_t b )
{

	size_t sum = 0;
	for ( size_t i = 0; i < b; i++ )
	{
		sum += i & 1 ? i : b;
	}

	return sum;
}

int main()
{
	SystemInit();

	WaitForDebuggerToAttach( 10000 );

	const size_t slowTicks = Benchmark( "Slow", Slow, 100000 );
	const size_t fastTicks = Benchmark( "Fast", Fast, 100000 );

	const size_t ratio = slowTicks * 100 / fastTicks;
	const size_t inverse = fastTicks * 100 / slowTicks;

	printf( "Slow flash read is %u%% slower or %u%% the speed of fast flash\n", ratio, inverse );

	while ( 1 )
	{
		Delay_Ms( 100 );
	}
}


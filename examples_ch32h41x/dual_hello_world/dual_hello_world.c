/* V3F uses UART1 and outputs on PA9
 * V5F uses UART6 and outputs on PA12
 */

#include "ch32fun.h"
#include <stdio.h>

void informations()
{
	printf( "HART ID: %lu\r\n", __get_MHARTID() );
	printf( "Stack Pointer: %lx\r\n", __get_SP() );
}

void __ITCM informations_itcm()
{
	printf( "ITCM function at: %lx\r\n", (uint32_t)informations_itcm );
}

int main_V5F()
{
	printf( "Hello World from V5F\r\n" );
	informations();
	informations_itcm();

	while(1)
	{
	}
}

int main()
{
	SystemInit();

	printf( "Hello World from V3F\r\n" );
	informations();

	StartV5F(main_V5F);

	while(1)
	{
	}
}

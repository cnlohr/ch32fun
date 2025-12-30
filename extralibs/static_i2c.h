/*
	Copyright 2012-2025 <>< Charles Lohr
	This file may be used for any purposes (commercial or private) just please
	leave this copyright notice in there	somewhere.  You may cross license
	this as MIT or NewBSD.

	Now, generic I2C library for any GPIO-based system.

	Include this in your .c file only!!!
	Include it after the following are defined:

	I2CDELAY_FUNC -> If you wish to override the internal delay functions.
	I2CSPEEDBASE -> define speed base multiplier 1 = normal, 10 = slow, .1 =
		fast; failure to define this will result in the clock being as fast as
		possible. I2CNEEDGETBYTE -> Do we need to be able to read data?

	I2CPREFIX   -> #define to be the prefix, i.e. BOB will cause BOBConfigI2C
		to be generated.
	I2CCODE -> decorate functions with this, otherwise assume it's static.

	NOTE: You must initially configure the port to be outputs on both DSDA
		and DSCL and set them both to be driven high.
*/
/* Example:

	#define DELAY1 Delay_Us(1);
	#define DELAY2 Delay_Us(2);
	#define DSCL_IHIGH  { funPinMode( PIN_SCL, GPIO_CFGLR_IN_PUPD ); funDigitalWrite( PIN_SCL, 1 ); } 
	#define DSDA_IHIGH  { funPinMode( PIN_SDA, GPIO_CFGLR_IN_PUPD ); funDigitalWrite( PIN_SDA, 1 ); } 
	#define DSDA_INPUT  { funPinMode( PIN_SDA, GPIO_CFGLR_IN_PUPD ); funDigitalWrite( PIN_SDA, 1 ); } 
	#define DSCL_OUTPUT { funDigitalWrite( PIN_SCL, 0 ); funPinMode( PIN_SCL, GPIO_CFGLR_OUT_2Mhz_PP );  } 
	#define DSDA_OUTPUT { funDigitalWrite( PIN_SDA, 0 ); funPinMode( PIN_SDA, GPIO_CFGLR_OUT_2Mhz_PP );  } 
	#define READ_DSDA    funDigitalRead( PIN_SDA )
	#define I2CNEEDGETBYTE 1
	#define I2CNEEDSCAN    1

	#include "static_i2c.h"

	...
*/



#ifndef I2CPREFIX
    #define I2CPREFIX
#endif

#ifndef I2CCODE
    #define I2CCODE static
#endif

#ifndef I2CFNCOLLAPSE
    #define INTI2CFNCOLLAPSE(PFX, name) PFX##name
    #define I2CFNCOLLAPSE(PFX, name)    INTI2CFNCOLLAPSE(PFX, name)
#endif

#ifndef I2CNEEDGETBYTE
    #define I2CNEEDGETBYTE 1
#endif

#ifndef DSCL_IHIGH
    #define DSCL_IHIGH DSCL_INPUT
#endif

#ifndef DSDA_IHIGH
    #define DSDA_IHIGH DSDA_INPUT
#endif

I2CCODE void I2CFNCOLLAPSE(I2CPREFIX, ConfigI2C)()
{
	DSDA_IHIGH
	DSCL_IHIGH
}

I2CCODE void I2CFNCOLLAPSE(I2CPREFIX, SendStart)()
{
	DELAY1
	DSCL_IHIGH
	DELAY1
	DSDA_OUTPUT
	DELAY1
	DSCL_OUTPUT
	DELAY1
}

I2CCODE void I2CFNCOLLAPSE(I2CPREFIX, SendStop)()
{
	DELAY1
	DSDA_OUTPUT
	DELAY1
	DSCL_IHIGH
	DELAY1
	DSDA_IHIGH
	DELAY1
}

// Return nonzero on failure.
I2CCODE unsigned char I2CFNCOLLAPSE(I2CPREFIX, SendByte)(unsigned char data)
{
    unsigned int i;
    // Assume we are in a started state (DSCL = 0 & DSDA = 0)

    for (i = 0; i < 8; i++)
    {
        DELAY1
        if (data & 0x80)
        {
            DSDA_IHIGH
        }
        else
        {
            DSDA_OUTPUT
        }
        data <<= 1;
        DELAY2
        DSCL_IHIGH
        DELAY2
        DSCL_OUTPUT
    }

    // Immediately after sending last bit, open up DDDR for control.
    DELAY1
    DSDA_INPUT
    DELAY1
    DSCL_IHIGH
    DELAY1
    i = READ_DSDA;
    DELAY1
    DSCL_OUTPUT
    DELAY1
    return !!i;
}

#if I2CNEEDGETBYTE

I2CCODE unsigned char I2CFNCOLLAPSE(I2CPREFIX, GetByte)(uint8_t send_nak)
{
    unsigned char i;
    unsigned char ret = 0;

    DSDA_INPUT

    for (i = 0; i < 8; i++)
    {
        DELAY1
        DSCL_IHIGH
        DELAY2
        ret <<= 1;
        if (READ_DSDA)
            ret |= 1;
        DSCL_OUTPUT
    }

    // Send ack.
    if (send_nak)
    {
    }
    else
    {
        DSDA_OUTPUT
    }

    DELAY1
    DSCL_IHIGH
    DELAY2
    DSCL_OUTPUT
    DELAY1
    DSDA_IHIGH

    return ret;
}

#endif

// In case you want SCAN code

#if I2CNEEDSCAN

I2CCODE void I2CFNCOLLAPSE(I2CPREFIX, Scan)()
{
	int i;
	printf( "  " );
	for( i = 0; i < 16; i++ )
	{
		printf( " %x", i );
	}
	for( i = 0; i < 128; i++ )
	{
		if( ( i & 0xf ) == 0 )
		{
			printf( "\n%02x ", i );
		}
		SendStart();
		int b = SendByte( i<<1 );
		SendStop();
		printf( "%c ", b?'.':'#' );
	}
	printf( "\n" );
}
#endif

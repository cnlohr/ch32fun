/* Single-File-Header for using asynchronous LEDs with the CH32V003 using DMA to the SPI port.
   I may write another version of this to use DMA to timer ports, but, the SPI port can be used
   to generate outputs very efficiently. So, for now, SPI Port.  Additionally, it uses FAR less
   internal bus resources than to do the same thing with timers.
   
   **For the CH32V003 this means output will be on PORTC Pin 6**
   **For the CH570/2 - PORTA Pin 7**
   **For the CH582/3 or CH591/2 PORTA Pin 14**

   Copyright 2023 <>< Charles Lohr, under the MIT-x11 or NewBSD License, you choose!

   If you are including this in main, simply 
	#define WS2812DMA_IMPLEMENTATION

   Other defines inclue:
	#define WSRAW
	#define WSRBG
	#define WSGRB
	#define WS2812B_ALLOW_INTERRUPT_NESTING

   You will need to implement the following two functions, as callbacks from the ISR.
	uint32_t WS2812BLEDCallback( int ledno );

   You willalso need to call
	WS2812BDMAInit();

   Then, whenyou want to update the LEDs, call:
	WS2812BDMAStart( int num_leds );
*/

#ifndef _WS2812_LED_DRIVER_H
#define _WS2812_LED_DRIVER_H

#include <stdint.h>

// Use DMA and SPI to stream out WS2812B LED Data via the MOSI pin.
void WS2812BDMAInit( );
void WS2812BDMAStart( int leds );

// Callbacks that you must implement.
uint32_t WS2812BLEDCallback( int ledno );

#ifdef WS2812DMA_IMPLEMENTATION

// Must be divisble by 4.
#ifndef DMALEDS
#define DMALEDS 16
#endif

// Note first n LEDs of DMA Buffer are 0's as a "break"
// Need one extra LED at end to leave line high. 
// This must be greater than WS2812B_RESET_PERIOD.
#define WS2812B_RESET_PERIOD 2

#ifdef WSRAW
#define DMA_BUFFER_LEN (((DMALEDS)/2)*8)
#else
#define DMA_BUFFER_LEN (((DMALEDS)/2)*6)
#endif

static uint16_t WS2812dmabuff[DMA_BUFFER_LEN];
static volatile int WS2812LEDs;
static volatile int WS2812LEDPlace;
static volatile int WS2812BLEDInUse;

#ifdef CH5xx
#ifdef CH570_CH572
#define bMOSI PA7
#else
#define bMOSI PA14
#endif
#endif
// This is the code that updates a portion of the WS2812dmabuff with new data.
// This effectively creates the bitstream that outputs to the LEDs.
static void WS2812FillBuffSec( uint16_t * ptr, int numhalfwords, int tce )
{
#ifdef CH5xx
	// Reversing bit order because CH5xx SPI FIFO is only half of what CH32 have
	const static uint16_t bitquartets[16] = {
		0b0001000100010001, 0b0111000100010001, 0b0001011100010001, 0b0111011100010001,
		0b0001000101110001, 0b0111000101110001, 0b0001011101110001, 0b0111011101110001,
		0b0001000100010111, 0b0111000100010111, 0b0001011100010111, 0b0111011100010111,
		0b0001000101110111, 0b0111000101110111, 0b0001011101110111, 0b0111011101110111, };
#else
	const static uint16_t bitquartets[16] = {
		0b1000100010001000, 0b1000100010001110, 0b1000100011101000, 0b1000100011101110,
		0b1000111010001000, 0b1000111010001110, 0b1000111011101000, 0b1000111011101110,
		0b1110100010001000, 0b1110100010001110, 0b1110100011101000, 0b1110100011101110,
		0b1110111010001000, 0b1110111010001110, 0b1110111011101000, 0b1110111011101110, };
#endif

	int i;
	uint16_t * end = ptr + numhalfwords;
	int ledcount = WS2812LEDs;
	int place = WS2812LEDPlace;

#ifdef WSRAW
	while( place < 0 && ptr != end )
	{
		uint32_t * lptr = (uint32_t *)ptr;
		lptr[0] = 0;
		lptr[1] = 0;
		lptr[2] = 0;
		lptr[3] = 0;
		ptr += 8;
		place++;
	}

#else
	while( place < 0 && ptr != end )
	{
		(*ptr++) = 0;
		(*ptr++) = 0;
		(*ptr++) = 0;
		(*ptr++) = 0;
		(*ptr++) = 0;
		(*ptr++) = 0;
		place++;
	}
#endif

	while( ptr != end )
	{
		if( place >= ledcount )
		{
			// Optionally, leave line high.
			while( ptr != end )
				(*ptr++) = 0;//0xffff;

			// Only safe to do this when we're on the second leg.
			if( tce )
			{
				if( place == ledcount )
				{
					// Take the DMA out of circular mode and let it expire.
#ifdef CH5xx
					R8_SPI0_INTER_EN &= ~RB_SPI_IE_DMA_END;  // Disable DMA end interrupt
#else
					DMA1_Channel3->CFGR &= ~DMA_Mode_Circular;
#endif
					WS2812BLEDInUse = 0;
				}
				place++;
			}

			break;
		}

#ifdef WSRAW
		uint32_t ledval32bit = WS2812BLEDCallback( place++ );

		ptr[6] = bitquartets[(ledval32bit>>28)&0xf];
		ptr[7] = bitquartets[(ledval32bit>>24)&0xf];
		ptr[4] = bitquartets[(ledval32bit>>20)&0xf];
		ptr[5] = bitquartets[(ledval32bit>>16)&0xf];
		ptr[2] = bitquartets[(ledval32bit>>12)&0xf];
		ptr[3] = bitquartets[(ledval32bit>>8)&0xf];
		ptr[0] = bitquartets[(ledval32bit>>4)&0xf];
		ptr[1] = bitquartets[(ledval32bit>>0)&0xf];

		ptr += 8;
		i += 8;

#else
		// Use a LUT to figure out how we should set the SPI line.
		uint32_t ledval24bit = WS2812BLEDCallback( place++ );

#ifdef WSRBG
		ptr[0] = bitquartets[(ledval24bit>>12)&0xf];
		ptr[1] = bitquartets[(ledval24bit>>8)&0xf];
		ptr[2] = bitquartets[(ledval24bit>>20)&0xf];
		ptr[3] = bitquartets[(ledval24bit>>16)&0xf];
		ptr[4] = bitquartets[(ledval24bit>>4)&0xf];
		ptr[5] = bitquartets[(ledval24bit>>0)&0xf];
#elif defined( WSGRB )
		ptr[0] = bitquartets[(ledval24bit>>12)&0xf];
		ptr[1] = bitquartets[(ledval24bit>>8)&0xf];
		ptr[2] = bitquartets[(ledval24bit>>4)&0xf];
		ptr[3] = bitquartets[(ledval24bit>>0)&0xf];
		ptr[4] = bitquartets[(ledval24bit>>20)&0xf];
		ptr[5] = bitquartets[(ledval24bit>>16)&0xf];
#else
		ptr[0] = bitquartets[(ledval24bit>>20)&0xf];
		ptr[1] = bitquartets[(ledval24bit>>16)&0xf];
		ptr[2] = bitquartets[(ledval24bit>>12)&0xf];
		ptr[3] = bitquartets[(ledval24bit>>8)&0xf];
		ptr[4] = bitquartets[(ledval24bit>>4)&0xf];
		ptr[5] = bitquartets[(ledval24bit>>0)&0xf];
#endif
		ptr += 6;
		i += 6;
#endif

	}
	WS2812LEDPlace = place;
}

#ifdef CH5xx
void SPI0_IRQHandler( void ) __attribute__((interrupt));
void SPI0_IRQHandler( void )
{
	uint8_t intf = R8_SPI0_INT_FLAG;
	if( (intf & RB_SPI_IF_DMA_END) )
	{
		WS2812FillBuffSec( WS2812dmabuff, DMA_BUFFER_LEN, 1 );
		R16_SPI0_TOTAL_CNT = DMA_BUFFER_LEN * 2;
	}
}
#else
void DMA1_Channel3_IRQHandler( void ) __attribute__((interrupt));
void DMA1_Channel3_IRQHandler( void ) 
{
	//GPIOD->BSHR = 1;	 // Turn on GPIOD0 for profiling

	// Backup flags.
	volatile int intfr = DMA1->INTFR;
	do
	{
		// Clear all possible flags.
		DMA1->INTFCR = DMA1_IT_GL3;

		// Strange note: These are backwards.  DMA1_IT_HT3 should be HALF and
		// DMA1_IT_TC3 should be COMPLETE.  But for some reason, doing this causes
		// LED jitter.  I am henseforth flipping the order.

		if( intfr & DMA1_IT_HT3 )
		{
			// Halfwaay (Fill in first part)
			WS2812FillBuffSec( WS2812dmabuff, DMA_BUFFER_LEN / 2, 1 );
		}
		if( intfr & DMA1_IT_TC3 )
		{
			// Complete (Fill in second part)
			WS2812FillBuffSec( WS2812dmabuff + DMA_BUFFER_LEN / 2, DMA_BUFFER_LEN / 2, 0 );
		}
		intfr = DMA1->INTFR;
	} while( intfr & DMA1_IT_GL3 );

	//GPIOD->BSHR = 1<<16; // Turn off GPIOD0 for profiling
}
#endif

void WS2812BDMAStart( int leds )
{
	// Enter critical section.
	__disable_irq();
	WS2812BLEDInUse = 1;
#ifdef CH5xx
	R8_SPI0_INTER_EN &= ~RB_SPI_IE_DMA_END;
	R8_SPI0_CTRL_CFG &= ~RB_SPI_DMA_ENABLE;
	R16_SPI0_TOTAL_CNT = 0;
#else
	DMA1_Channel3->CFGR &= ~DMA_Mode_Circular;
	DMA1_Channel3->CNTR  = 0;
	DMA1_Channel3->MADDR = (uint32_t)WS2812dmabuff;
#endif
  __enable_irq();
	WS2812LEDs = leds;
	WS2812LEDPlace = -WS2812B_RESET_PERIOD;
	

#ifdef CH5xx
	WS2812FillBuffSec( WS2812dmabuff, DMA_BUFFER_LEN, 0 );
	R16_SPI0_TOTAL_CNT = DMA_BUFFER_LEN * 2;
	R16_SPI0_DMA_BEG = (uint32_t)WS2812dmabuff;
	R8_SPI0_INT_FLAG = RB_SPI_IF_CNT_END | RB_SPI_IF_DMA_END;
	R8_SPI0_INTER_EN = RB_SPI_IE_DMA_END;
	R8_SPI0_CTRL_CFG |= RB_SPI_DMA_ENABLE;
#else
	WS2812FillBuffSec( WS2812dmabuff, DMA_BUFFER_LEN, 0 );
	DMA1_Channel3->CNTR = DMA_BUFFER_LEN; // Number of unique uint16_t entries.
	DMA1_Channel3->CFGR |= DMA_Mode_Circular;
#endif
}

void WS2812BDMAInit( )
{
	// Enable DMA + Peripherals
#ifdef CH5xx
	funPinMode( bMOSI, GPIO_CFGLR_OUT_2Mhz_PP );
	R8_SPI0_CLOCK_DIV = FUNCONF_SYSTEM_CORE_CLOCK / 3000000; // div = Fsys/3MHz
	R8_SPI0_CTRL_MOD = RB_SPI_ALL_CLEAR;
	R8_SPI0_CTRL_MOD = RB_SPI_MOSI_OE | RB_SPI_2WIRE_MOD;
	R16_SPI0_DMA_END = ( (uint32_t)WS2812dmabuff + (DMA_BUFFER_LEN * 2) );
	R8_SPI0_CTRL_CFG |= RB_SPI_BIT_ORDER;

	NVIC_EnableIRQ( SPI0_IRQn );
#else
	RCC->AHBPCENR |= RCC_AHBPeriph_DMA1;
	RCC->APB2PCENR |= RCC_APB2Periph_GPIOC | RCC_APB2Periph_SPI1;

	// MOSI, Configure GPIO Pin
	GPIOC->CFGLR &= ~(0xf<<(4*6));
	GPIOC->CFGLR |= (GPIO_Speed_10MHz | GPIO_CNF_OUT_PP_AF)<<(4*6);

	// Configure SPI 
	SPI1->CTLR1 = 
		SPI_NSS_Soft | SPI_CPHA_1Edge | SPI_CPOL_Low | SPI_DataSize_16b |
		SPI_Mode_Master | SPI_Direction_1Line_Tx |
		3<<3; // Divisior = 16 (48/16 = 3MHz)

	SPI1->CTLR2 = SPI_CTLR2_TXDMAEN;  // Enable Tx buffer DMA

#if defined(CH32V003)
	SPI1->HSCR = 1; // Enable high-speed read mode
#endif

	SPI1->CTLR1 |= CTLR1_SPE_Set; // Enable SPI

	SPI1->DATAR = 0; // Set SPI line Low.

	//DMA1_Channel3 is for SPI1TX
	DMA1_Channel3->PADDR = (uint32_t)&SPI1->DATAR;
	DMA1_Channel3->MADDR = (uint32_t)WS2812dmabuff;
	DMA1_Channel3->CNTR  = 0;// sizeof( bufferset )/2; // Number of unique copies.  (Don't start, yet!)
	DMA1_Channel3->CFGR  =
		DMA_M2M_Disable |		 
		DMA_Priority_VeryHigh |
		DMA_MemoryDataSize_HalfWord |
		DMA_PeripheralDataSize_HalfWord |
		DMA_MemoryInc_Enable |
		DMA_Mode_Normal | // OR DMA_Mode_Circular or DMA_Mode_Normal
		DMA_DIR_PeripheralDST |
		DMA_IT_TC | DMA_IT_HT; // Transmission Complete + Half Empty Interrupts. 

//	NVIC_SetPriority( DMA1_Channel3_IRQn, 0<<4 ); //We don't need to tweak priority.
	NVIC_EnableIRQ( DMA1_Channel3_IRQn );
	DMA1_Channel3->CFGR |= DMA_CFGR1_EN;

#ifdef WS2812B_ALLOW_INTERRUPT_NESTING
	__set_INTSYSCR( __get_INTSYSCR() | 2 ); // Enable interrupt nesting.
	PFIC->IPRIOR[24] = 0b10000000; // Turn on preemption for DMA1Ch3
#endif
#endif
}

#endif

#endif


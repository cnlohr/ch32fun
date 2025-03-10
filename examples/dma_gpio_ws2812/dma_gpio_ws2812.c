// DMA GPIO Output Example - this one showing output
// of a bitsream to a ws2812b LED on port PD0 -- note
// that this could be any port.  Also note, that this
// has much higher overhead than using the SPI port
// for WS2812B output.

// While this does work to output via DMA ws2812 LEDs
// on any port, think of this as more of an example
// of how you would do arbitrary things on various
// ports.

#include "ch32fun.h"
#include <stdio.h>


#define NR_LEDS 160
const int pin_number = 0;
#define BITS_PER_LED 24
#define TIME_SLICES_PER_BIT 4 // 4 is more reliable, 3 has less overhead.
const int gpio_pin = PD0;
GPIO_TypeDef * const gpio_port = GPIOD;

static int frame = 0;
uint16_t phases[NR_LEDS];

// return the color (GRB) for each LED
uint32_t ComputeLED( uint32_t ledno )
{
	static const unsigned char sintable[] = {
		0x80, 0x83, 0x86, 0x89, 0x8c, 0x8f, 0x92, 0x95, 0x99, 0x9c, 0x9f, 0xa2, 0xa5, 0xa8, 0xab, 0xad, 
		0xb0, 0xb3, 0xb6, 0xb9, 0xbc, 0xbe, 0xc1, 0xc4, 0xc6, 0xc9, 0xcb, 0xce, 0xd0, 0xd3, 0xd5, 0xd7, 
		0xda, 0xdc, 0xde, 0xe0, 0xe2, 0xe4, 0xe6, 0xe8, 0xe9, 0xeb, 0xed, 0xee, 0xf0, 0xf1, 0xf3, 0xf4, 
		0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfc, 0xfd, 0xfe, 0xfe, 0xfe, 0xff, 0xff, 0xff, 
		0xff, 0xff, 0xff, 0xff, 0xfe, 0xfe, 0xfe, 0xfd, 0xfc, 0xfc, 0xfb, 0xfa, 0xf9, 0xf8, 0xf7, 0xf6, 
		0xf5, 0xf4, 0xf3, 0xf1, 0xf0, 0xee, 0xed, 0xeb, 0xe9, 0xe8, 0xe6, 0xe4, 0xe2, 0xe0, 0xde, 0xdc, 
		0xda, 0xd7, 0xd5, 0xd3, 0xd0, 0xce, 0xcb, 0xc9, 0xc6, 0xc4, 0xc1, 0xbe, 0xbc, 0xb9, 0xb6, 0xb3, 
		0xb0, 0xad, 0xab, 0xa8, 0xa5, 0xa2, 0x9f, 0x9c, 0x99, 0x95, 0x92, 0x8f, 0x8c, 0x89, 0x86, 0x83, 
		0x80, 0x7d, 0x79, 0x76, 0x73, 0x70, 0x6d, 0x6a, 0x67, 0x64, 0x61, 0x5e, 0x5b, 0x58, 0x55, 0x52, 
		0x4f, 0x4c, 0x49, 0x47, 0x44, 0x41, 0x3e, 0x3c, 0x39, 0x36, 0x34, 0x31, 0x2f, 0x2d, 0x2a, 0x28, 
		0x26, 0x24, 0x21, 0x1f, 0x1d, 0x1b, 0x1a, 0x18, 0x16, 0x14, 0x13, 0x11, 0x10, 0x0e, 0x0d, 0x0b, 
		0x0a, 0x09, 0x08, 0x07, 0x06, 0x05, 0x04, 0x04, 0x03, 0x02, 0x02, 0x01, 0x01, 0x01, 0x01, 0x01, 
		0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x02, 0x02, 0x03, 0x04, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 
		0x0a, 0x0b, 0x0d, 0x0e, 0x10, 0x11, 0x13, 0x14, 0x16, 0x18, 0x1a, 0x1b, 0x1d, 0x1f, 0x21, 0x24, 
		0x26, 0x28, 0x2a, 0x2d, 0x2f, 0x31, 0x34, 0x36, 0x39, 0x3c, 0x3e, 0x41, 0x44, 0x47, 0x49, 0x4c, 
		0x4f, 0x52, 0x55, 0x58, 0x5b, 0x5e, 0x61, 0x64, 0x67, 0x6a, 0x6d, 0x70, 0x73, 0x76, 0x79, 0x7d, };


	static const unsigned char rands[] = {
		0x67, 0xc6, 0x69, 0x73, 0x51, 0xff, 0x4a, 0xec, 0x29, 0xcd, 0xba, 0xab, 0xf2, 0xfb, 0xe3, 0x46, 
		0x7c, 0xc2, 0x54, 0xf8, 0x1b, 0xe8, 0xe7, 0x8d, 0x76, 0x5a, 0x2e, 0x63, 0x33, 0x9f, 0xc9, 0x9a, 
		0x66, 0x32, 0x0d, 0xb7, 0x31, 0x58, 0xa3, 0x5a, 0x25, 0x5d, 0x05, 0x17, 0x58, 0xe9, 0x5e, 0xd4, 
		0xab, 0xb2, 0xcd, 0xc6, 0x9b, 0xb4, 0x54, 0x11, 0x0e, 0x82, 0x74, 0x41, 0x21, 0x3d, 0xdc, 0x87, 
		0x70, 0xe9, 0x3e, 0xa1, 0x41, 0xe1, 0xfc, 0x67, 0x3e, 0x01, 0x7e, 0x97, 0xea, 0xdc, 0x6b, 0x96, 
		0x8f, 0x38, 0x5c, 0x2a, 0xec, 0xb0, 0x3b, 0xfb, 0x32, 0xaf, 0x3c, 0x54, 0xec, 0x18, 0xdb, 0x5c, 
		0x02, 0x1a, 0xfe, 0x43, 0xfb, 0xfa, 0xaa, 0x3a, 0xfb, 0x29, 0xd1, 0xe6, 0x05, 0x3c, 0x7c, 0x94, 
		0x75, 0xd8, 0xbe, 0x61, 0x89, 0xf9, 0x5c, 0xbb, 0xa8, 0x99, 0x0f, 0x95, 0xb1, 0xeb, 0xf1, 0xb3, 
		0x05, 0xef, 0xf7, 0x00, 0xe9, 0xa1, 0x3a, 0xe5, 0xca, 0x0b, 0xcb, 0xd0, 0x48, 0x47, 0x64, 0xbd, 
		0x1f, 0x23, 0x1e, 0xa8, 0x1c, 0x7b, 0x64, 0xc5, 0x14, 0x73, 0x5a, 0xc5, 0x5e, 0x4b, 0x79, 0x63, 
		0x3b, 0x70, 0x64, 0x24, 0x11, 0x9e, 0x09, 0xdc, 0xaa, 0xd4, 0xac, 0xf2, 0x1b, 0x10, 0xaf, 0x3b, 
		0x33, 0xcd, 0xe3, 0x50, 0x48, 0x47, 0x15, 0x5c, 0xbb, 0x6f, 0x22, 0x19, 0xba, 0x9b, 0x7d, 0xf5, 
		0x0b, 0xe1, 0x1a, 0x1c, 0x7f, 0x23, 0xf8, 0x29, 0xf8, 0xa4, 0x1b, 0x13, 0xb5, 0xca, 0x4e, 0xe8, 
		0x98, 0x32, 0x38, 0xe0, 0x79, 0x4d, 0x3d, 0x34, 0xbc, 0x5f, 0x4e, 0x77, 0xfa, 0xcb, 0x6c, 0x05, 
		0xac, 0x86, 0x21, 0x2b, 0xaa, 0x1a, 0x55, 0xa2, 0xbe, 0x70, 0xb5, 0x73, 0x3b, 0x04, 0x5c, 0xd3, 
		0x36, 0x94, 0xb3, 0xaf, 0xe2, 0xf0, 0xe4, 0x9e, 0x4f, 0x32, 0x15, 0x49, 0xfd, 0x82, 0x4e, 0xa9, };

	static const unsigned char huetable[] = {
		0x00, 0x06, 0x0c, 0x12, 0x18, 0x1e, 0x24, 0x2a, 0x30, 0x36, 0x3c, 0x42, 0x48, 0x4e, 0x54, 0x5a, 
		0x60, 0x66, 0x6c, 0x72, 0x78, 0x7e, 0x84, 0x8a, 0x90, 0x96, 0x9c, 0xa2, 0xa8, 0xae, 0xb4, 0xba, 
		0xc0, 0xc6, 0xcc, 0xd2, 0xd8, 0xde, 0xe4, 0xea, 0xf0, 0xf6, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
		0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
		0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
		0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
		0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
		0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
		0xff, 0xf9, 0xf3, 0xed, 0xe7, 0xe1, 0xdb, 0xd5, 0xcf, 0xc9, 0xc3, 0xbd, 0xb7, 0xb1, 0xab, 0xa5, 
		0x9f, 0x99, 0x93, 0x8d, 0x87, 0x81, 0x7b, 0x75, 0x6f, 0x69, 0x63, 0x5d, 0x57, 0x51, 0x4b, 0x45, 
		0x3f, 0x39, 0x33, 0x2d, 0x27, 0x21, 0x1b, 0x15, 0x0f, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, };

	phases[ledno] += ((((rands[ledno&0xff])+0xf)<<2) + (((rands[ledno&0xff])+0xf)<<1))>>1;
	uint8_t index = (phases[ledno])>>8;
	uint8_t rsbase = sintable[index];
	uint8_t rs = rsbase>>3;
	uint32_t fire = ((huetable[(rs+190)&0xff]>>1)<<0) | ((huetable[(rs+30)&0xff])<<8) | ((huetable[(rs+0)]>>1)<<16);
	uint32_t ice  = 0x00007f | ((rsbase>>1)<<8) | ((rsbase>>1)<<16);
	if( ( ( ledno + frame ) & 0xfff ) > 0x800 )
		return ice;
	else
		return fire;
}

// We fill the BSHR value associated with each timeslice.
void FillLED( uint32_t * buffer )
{
	static int ledno;
	uint32_t hi = 1<<(pin_number);
	uint32_t lo = 1<<(pin_number+16);

	if( ledno >= NR_LEDS )
	{
		uint32_t * be = buffer + (BITS_PER_LED * TIME_SLICES_PER_BIT);
		do
		{
			buffer[0] = lo;
			buffer++;
		} while( buffer != be );
		ledno++;
		if( ledno > NR_LEDS + 3 ) ledno = 0;
	}
	else
	{
		int32_t led_color = ComputeLED( ledno );

		// Force into signed bit.
		led_color <<= 8;

		uint32_t * be = buffer + (BITS_PER_LED * TIME_SLICES_PER_BIT);
		do
		{
			uint32_t val = ( led_color < 0 ) ? hi : lo;
			led_color<<=1;

			// This code here is what actually sets the state of the pin over time.
#if TIME_SLICES_PER_BIT == 4
			buffer[0] = hi;
			buffer[1] = val;
			buffer[2] = val;
			buffer[3] = lo;
			buffer+=4;
#elif TIME_SLICES_PER_BIT == 3
			buffer[0] = hi;
			buffer[1] = val;
			buffer[2] = lo;
			buffer+=3;
#else
			#error Not sure how to use this number of bits.
#endif
		} while( buffer != be );
		if( ledno == 0 ) frame++;
		ledno++;
	}
}


static uint32_t memory_buffer[BITS_PER_LED * TIME_SLICES_PER_BIT * 2]; //2 LEDs worth.

// The DMA has an interrupt when the buffer is half full, or when it's done. 
// That way we can choose to fill the part of the buffer that is not currently being output.
void DMA1_Channel2_IRQHandler( void ) __attribute__((interrupt)) __attribute__((section(".srodata")));
void DMA1_Channel2_IRQHandler( void ) 
{
	const int halfsamps = sizeof(memory_buffer)/sizeof(memory_buffer[0])/2;

	volatile int intfr = DMA1->INTFR;
	do
	{
		DMA1->INTFCR = DMA1_IT_GL2;

		// Gets called at the end-of-a frame.
		if( intfr & DMA1_IT_TC2 )
		{
			uint32_t * mbb = (uint32_t*)( memory_buffer + halfsamps );
			FillLED( mbb );
		}
		
		// Gets called halfway through the frame
		if( intfr & DMA1_IT_HT2 )
		{
			uint32_t * mbb = (uint32_t*)( memory_buffer );
			FillLED( mbb );
		}
		intfr = DMA1->INTFR;
	} while( intfr );
}

int main()
{
	int i;

	SystemInit();
	funGpioInitAll();

	// Enable DMA
	RCC->AHBPCENR = RCC_AHBPeriph_SRAM | RCC_AHBPeriph_DMA1;

	// Enable Timer 1
	RCC->APB2PCENR |= RCC_APB2Periph_TIM1;


	// GPIO D0 Output (where we are connecting our LED)
	funPinMode( gpio_pin, GPIO_CFGLR_OUT_10Mhz_PP );

	// Setup visual effect
	for( i = 0; i < NR_LEDS; i++ ) phases[i] = i<<8;

	// DMA2 can be configured to attach to T1CH1
	// The system can only DMA out at ~2.2MSPS.  2MHz is stable.
	// The idea here is that this copies, byte-at-a-time from the memory
	// into the peripheral addres.
	DMA1_Channel2->CNTR = sizeof(memory_buffer) / sizeof(memory_buffer[0]);
	DMA1_Channel2->MADDR = (uint32_t)memory_buffer;
	DMA1_Channel2->PADDR = (uint32_t)&gpio_port->BSHR;
	DMA1_Channel2->CFGR = 
		DMA_CFGR1_DIR |                      // MEM2PERIPHERAL
		DMA_CFGR1_PL |                       // High priority.
		DMA_CFGR1_MSIZE_1 |                  // 32-bit memory
		DMA_CFGR1_PSIZE_1 |                  // 32-bit peripheral
		DMA_CFGR1_MINC |                     // Increase memory.
		DMA_CFGR1_CIRC |                     // Circular mode.
		DMA_CFGR1_HTIE |                     // Half-trigger
		DMA_CFGR1_TCIE |                     // Whole-trigger
		DMA_CFGR1_EN;                        // Enable

	NVIC_EnableIRQ( DMA1_Channel2_IRQn );
	DMA1_Channel2->CFGR |= DMA_CFGR1_EN;

	// NOTE: You can also hook up DMA1 Channel 3 to T1C2,
	// if you want to output to multiple IO ports at
	// at the same time.  Just be aware you have to offset
	// the time they read at by at least 1/8Mth of a second.

	// Setup Timer1.
	RCC->APB2PRSTR = RCC_APB2Periph_TIM1;    // Reset Timer
	RCC->APB2PRSTR = 0;

	// Timer 1 setup.
	// Timer 1 is what will trigger the DMA, Channel 2 engine.
	TIM1->PSC = 0x0000;                      // Prescaler 
#if TIME_SLICES_PER_BIT == 4
	TIM1->ATRLR = 15;                        // Auto Reload - sets period (48MHz / (15+1) = 3MHz) valid divisors = 11-20
#elif TIME_SLICES_PER_BIT == 3
	TIM1->ATRLR = 17;                        // Auto Reload - sets period (48MHz / (17+1) = 2.66MHz) valid divisors = 14-20
#endif
	TIM1->SWEVGR = TIM_UG | TIM_TG;          // Reload immediately + Trigger DMA
	TIM1->CCER = TIM_CC1E | TIM_CC1P;        // Enable CH1 output, positive pol
	TIM1->CHCTLR1 = TIM_OC1M_2 | TIM_OC1M_1; // CH1 Mode is output, PWM1 (CC1S = 00, OC1M = 110)
	TIM1->CH1CVR = 6;                        // Set the Capture Compare Register value to 50% initially
	TIM1->BDTR = TIM_MOE;                    // Enable TIM1 outputs
	TIM1->CTLR1 = TIM_CEN;                   // Enable TIM1
	TIM1->DMAINTENR = TIM_UDE | TIM_CC1DE;   // Trigger DMA on TC match 1 (DMA Ch2) and TC match 2 (DMA Ch3)


	// Just debug stuff.
	printf( "Setup OK\n" );

	while(1)
	{
	}
}


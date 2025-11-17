//######## necessities

// include guards
#ifndef CH32V003_SPI_H
#define CH32V003_SPI_H

// includes
#include<stdint.h>								//uintN_t support
#include "ch32fun.h"

#ifndef APB_CLOCK
	#define APB_CLOCK FUNCONF_SYSTEM_CORE_CLOCK
#endif

/*######## library usage and configuration

in the .c files that use this library, you'll need to #define some configuration options _before_ the #include "ch32v003_SPI.h"

SYSTEM_CORE_CLOCK and APB_CLOCK should be defined already as APB_CLOCK is used by this library


#ifndef APB_CLOCK
	#define APB_CLOCK FUNCONF_SYSTEM_CORE_CLOCK
#endif

to enable using the functions of this library:
#define CH32V003_SPI_IMPLEMENTATION

to configure the settings of the SPI bus, first, declare the desired bus speed

#define CH32V003_SPI_SPEED_HZ 1000000

then pick the desired setting of each group:

#define CH32V003_SPI_DIRECTION_2LINE_TXRX
#define CH32V003_SPI_DIRECTION_1LINE_TX

#define CH32V003_SPI_CLK_MODE_POL0_PHA0			//leading = rising		trailing = falling		sample on leading		default if you're unsure
#define CH32V003_SPI_CLK_MODE_POL0_PHA1			//leading = rising		trailing = falling		sample on trailing
#define CH32V003_SPI_CLK_MODE_POL1_PHA0			//leading = falling		trailing = rising		sample on leading
#define CH32V003_SPI_CLK_MODE_POL1_PHA1			//leading = falling		trailing = rising		sample on trailing

#define CH32V003_SPI_NSS_HARDWARE_PC0			// _NSS	toggled by hardware, automatic
#define CH32V003_SPI_NSS_HARDWARE_PC1			// NSS	toggled by hardware, automatic
#define CH32V003_SPI_NSS_SOFTWARE_PC3			// PC3	toggled by software, automatic, manual setters available
#define CH32V003_SPI_NSS_SOFTWARE_PC4			// PC4	toggled by software, automatic, manual setters available
#define CH32V003_SPI_NSS_SOFTWARE_ANY_MANUAL	// toggle manually!
*/



//######## function overview (declarations): use these!
// initialize and configure the SPI peripheral
static inline void SPI_init();

// establish / end a connection to the SPI device
static inline void SPI_begin_8();
static inline void SPI_begin_16();
static inline void SPI_end();

// manually set the NSS (chip select) pin high / low
// "SPI_NSS_HIGH_FN" and "SPI_NSS_LOW_FN" only become available functions if the selected NSS is software PC3 or PC4
#if defined(CH32V003_SPI_NSS_SOFTWARE_PC3) || defined(CH32V003_SPI_NSS_SOFTWARE_PC4)
static inline void SPI_NSS_software_low();
static inline void SPI_NSS_software_high();
#endif

// read / write the SPI device
// these commands are raw, you'll have to consider all other steps in SPI_transfer!
static inline uint8_t SPI_read_8();
static inline uint16_t SPI_read_16();
static inline void SPI_write_8(uint8_t data);
static inline void SPI_write_16(uint16_t data);

// send a command and get a response from the SPI device
// you'll use this for most devices
static inline uint8_t SPI_transfer_8(uint8_t data);
static inline uint16_t SPI_transfer_16(uint16_t data);

// SPI peripheral power enable / disable (default off, init() automatically enables)
// send SPI peripheral to sleep
static inline void SPI_poweroff();
// wake SPI peripheral from sleep
static inline void SPI_poweron();

// helper: kill / restore all interrupts on the CH32V003
static inline void kill_interrrupts();
static inline void restore_interrupts();



//######## internal function declarations
static inline void SPI_wait_TX_complete();
static inline uint8_t SPI_is_RX_empty();
static inline void SPI_wait_RX_available();



//######## internal variables
static uint16_t EXT1_INTENR_backup;



//######## preprocessor macros
// min and max helper macros
#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

// stringify for displaying what #defines evaluated to at preprocessor stage
#define VALUE_TO_STRING(x) #x
#define VALUE(x) VALUE_TO_STRING(x)
#define VAR_NAME_VALUE(var) #var "="  VALUE(var)

//compile-time log2
#define LOG2(x) ((x) == 0 ? -1 : __builtin_ctz(x))

// compile-time clock prescaler calculation: log2(APB_CLOCK/SPEED_BUS)
#define SPI_CLK_RATIO (APB_CLOCK / CH32V003_SPI_SPEED_HZ)
#define SPI_CLK_PRESCALER LOG2(SPI_CLK_RATIO)

// ensure that CLOCK_PRESCALER_VALUE is within the range of 0..7
_Static_assert(SPI_CLK_PRESCALER >= 0 && SPI_CLK_PRESCALER <= 7, "SPI_CLK_PRESCALER is out of range (0..7). Please set a different SPI bus speed. prescaler = log2(f_CPU/f_SPI)");
#pragma message(VAR_NAME_VALUE(SPI_CLK_PRESCALER))



//######## preprocessor #define requirements

#if !defined(CH32V003_SPI_DIRECTION_2LINE_TXRX) && !defined(CH32V003_SPI_DIRECTION_1LINE_TX)
	#warning "none of the CH32V003_SPI_DIRECTION_ options were defined!"
#endif
#if defined(CH32V003_SPI_DIRECTION_2LINE_TXRX) && defined(CH32V003_SPI_DIRECTION_1LINE_TX)
	#warning "both CH32V003_SPI_DIRECTION_ options were defined!"
#endif

#if ((defined(CH32V003_SPI_CLK_MODE_POL0_PHA0) ? 1 : 0) + \
	(defined(CH32V003_SPI_CLK_MODE_POL0_PHA1) ? 1 : 0) + \
	(defined(CH32V003_SPI_CLK_MODE_POL1_PHA0) ? 1 : 0) + \
	(defined(CH32V003_SPI_CLK_MODE_POL1_PHA1) ? 1 : 0)) > 1
	#warning "more than one of the CH32V003_SPI_CLK_MODE_ options were defined!"
#endif
#if ((defined(CH32V003_SPI_CLK_MODE_POL0_PHA0) ? 1 : 0) + \
	(defined(CH32V003_SPI_CLK_MODE_POL0_PHA1) ? 1 : 0) + \
	(defined(CH32V003_SPI_CLK_MODE_POL1_PHA0) ? 1 : 0) + \
	(defined(CH32V003_SPI_CLK_MODE_POL1_PHA1) ? 1 : 0)) == 0
	#warning "none of the CH32V003_SPI_CLK_MODE_ options were defined!"
#endif

#if ((defined(CH32V003_SPI_NSS_HARDWARE_PC0) ? 1 : 0) + \
	(defined(CH32V003_SPI_NSS_HARDWARE_PC1) ? 1 : 0) + \
	(defined(CH32V003_SPI_NSS_SOFTWARE_PC3) ? 1 : 0) + \
	(defined(CH32V003_SPI_NSS_SOFTWARE_PC4) ? 1 : 0) + \
	(defined(CH32V003_SPI_NSS_SOFTWARE_ANY_MANUAL) ? 1 : 0)) > 1
	#warning "more than one of the CH32V003_SPI_NSS_ options were defined!"
#endif
#if ((defined(CH32V003_SPI_NSS_HARDWARE_PC0) ? 1 : 0) + \
	(defined(CH32V003_SPI_NSS_HARDWARE_PC1) ? 1 : 0) + \
	(defined(CH32V003_SPI_NSS_SOFTWARE_PC3) ? 1 : 0) + \
	(defined(CH32V003_SPI_NSS_SOFTWARE_PC4) ? 1 : 0) + \
	(defined(CH32V003_SPI_NSS_SOFTWARE_ANY_MANUAL) ? 1 : 0)) == 0
	#warning "none of the CH32V003_SPI_NSS_ options were defined!"
#endif



//########  small function definitions, static inline
static inline void SPI_init() {
	SPI_poweron();
	
	// reset control register
	SPI1->CTLR1 = 0;

	// set prescaler
	SPI1->CTLR1 |= SPI_CTLR1_BR & (SPI_CLK_PRESCALER<<3);

	// set clock polarity and phase 
	#if defined(CH32V003_SPI_CLK_MODE_POL0_PHA0)
		SPI1->CTLR1 |= (SPI_CPOL_Low | SPI_CPHA_1Edge);
	#elif defined (CH32V003_SPI_CLK_MODE_POL0_PHA1)
		SPI1->CTLR1 |= (SPI_CPOL_Low | SPI_CPHA_2Edge);
	#elif defined (CH32V003_SPI_CLK_MODE_POL1_PHA0)
		SPI1->CTLR1 |= (SPI_CPOL_High | SPI_CPHA_1Edge);
	#elif defined (CH32V003_SPI_CLK_MODE_POL1_PHA1)
		SPI1->CTLR1 |= (SPI_CPOL_High | SPI_CPHA_2Edge);
	#endif
	
	// configure NSS pin, master mode
	#if defined(CH32V003_SPI_NSS_HARDWARE_PC0)
		// _NSS (negative slave select) on PC0, 10MHz Output, alt func, push-pull1
		SPI1->CTLR1 |= SPI_NSS_Hard;					// NSS hardware control mode
		GPIOC->CFGLR &= ~(0xf<<(4*0));
		GPIOC->CFGLR |= (GPIO_Speed_10MHz | GPIO_CNF_OUT_PP_AF)<<(4*0);
		AFIO->PCFR1 |= GPIO_Remap_SPI1;					// remap NSS (C1) to _NSS (C0)
		SPI1->CTLR2 |= SPI_CTLR2_SSOE;					// pull _NSS high
	#elif defined(CH32V003_SPI_NSS_HARDWARE_PC1)
		// NSS (negative slave select) on PC1, 10MHz Output, alt func, push-pull1
		SPI1->CTLR1 |= SPI_NSS_Hard;					// NSS hardware control mode
		GPIOC->CFGLR &= ~(0xf<<(4*1));
		GPIOC->CFGLR |= (GPIO_Speed_10MHz | GPIO_CNF_OUT_PP_AF)<<(4*1);
		SPI1->CTLR2 |= SPI_CTLR2_SSOE;					// pull _NSS high
	#elif defined(CH32V003_SPI_NSS_SOFTWARE_PC3)
		SPI1->CTLR1 |= SPI_NSS_Soft;					// SSM NSS software control mode
		GPIOC->CFGLR &= ~(0xf<<(4*3));
		GPIOC->CFGLR |= (GPIO_Speed_10MHz | GPIO_CNF_OUT_PP_AF)<<(4*3);
	#elif defined(CH32V003_SPI_NSS_SOFTWARE_PC4)
		SPI1->CTLR1 |= SPI_NSS_Soft;					// SSM NSS software control mode
		GPIOC->CFGLR &= ~(0xf<<(4*4));
		GPIOC->CFGLR |= (GPIO_Speed_10MHz | GPIO_CNF_OUT_PP_AF)<<(4*4);
	#elif defined(CH32V003_SPI_NSS_SOFTWARE_ANY_MANUAL)
		SPI1->CTLR1 |= SPI_NSS_Soft;					// SSM NSS software control mode
	#endif

	// SCK on PC5, 10MHz Output, alt func, push-pull
	GPIOC->CFGLR &= ~(0xf<<(4*5));
	GPIOC->CFGLR |= (GPIO_Speed_50MHz | GPIO_CNF_OUT_PP_AF)<<(4*5);

	// CH32V003 is master
	SPI1->CTLR1 |= SPI_Mode_Master;
	
	// set data direction and configure data pins
	#if defined(CH32V003_SPI_DIRECTION_2LINE_TXRX)
		SPI1->CTLR1 |= SPI_Direction_2Lines_FullDuplex;

		// MOSI on PC6, 10MHz Output, alt func, push-pull
		GPIOC->CFGLR &= ~(0xf<<(4*6));
		GPIOC->CFGLR |= (GPIO_Speed_50MHz | GPIO_CNF_OUT_PP_AF)<<(4*6);
		
		// MISO on PC7, 10MHz input, floating
		GPIOC->CFGLR &= ~(0xf<<(4*7));
		GPIOC->CFGLR |= GPIO_CNF_IN_FLOATING<<(4*7);
	#elif defined(CH32V003_SPI_DIRECTION_1LINE_TX)
		SPI1->CTLR1 |= SPI_Direction_1Line_Tx;

		// MOSI on PC6, 10MHz Output, alt func, push-pull
		GPIOC->CFGLR &= ~(0xf<<(4*6));
		GPIOC->CFGLR |= (GPIO_Speed_50MHz | GPIO_CNF_OUT_PP_AF)<<(4*6);
	#endif
}

static inline void SPI_begin_8() {
	SPI1->CTLR1 &= ~(SPI_CTLR1_DFF);			// DFF 16bit data-length enable, writable only when SPE is 0
	SPI1->CTLR1 |= SPI_CTLR1_SPE;
}
static inline void SPI_begin_16() {
	SPI1->CTLR1 |= SPI_CTLR1_DFF;				// DFF 16bit data-length enable, writable only when SPE is 0
	SPI1->CTLR1 |= SPI_CTLR1_SPE;
}
static inline void SPI_end() {
	SPI1->CTLR1 &= ~(SPI_CTLR1_SPE);
}

#if defined(CH32V003_SPI_NSS_SOFTWARE_PC3)
static inline void SPI_NSS_software_high() {
	GPIOC->BSHR = (1<<3);
}
static inline void SPI_NSS_software_low() {
	GPIOC->BSHR = (1<<(16+3));
}
#elif defined(CH32V003_SPI_NSS_SOFTWARE_PC4) 
static inline void SPI_NSS_software_high() {
	GPIOC->BSHR = (1<<4);
}
static inline void SPI_NSS_software_low() {
	GPIOC->BSHR = (1<<(16+4));
}
#endif

static inline uint8_t SPI_read_8() {
	return SPI1->DATAR;
}
static inline uint16_t SPI_read_16() {
	return SPI1->DATAR;
}
static inline void SPI_write_8(uint8_t data) {
	SPI1->DATAR = data;
}
static inline void SPI_write_16(uint16_t data) {
	SPI1->DATAR = data;
}
static inline uint8_t SPI_transfer_8(uint8_t data) {
	#if defined(CH32V003_SPI_NSS_SOFTWARE_PC3) || defined(CH32V003_SPI_NSS_SOFTWARE_PC4) 
		SPI_NSS_software_high();
	#endif
	SPI_write_8(data);
	SPI_wait_TX_complete();
	asm volatile("nop");
	SPI_wait_RX_available();
	#if defined(CH32V003_SPI_NSS_SOFTWARE_PC3) || defined(CH32V003_SPI_NSS_SOFTWARE_PC4) 
		SPI_NSS_software_low();
	#endif
	return SPI_read_8();
}
static inline uint16_t SPI_transfer_16(uint16_t data) {
	#if defined(CH32V003_SPI_NSS_SOFTWARE_PC3) || defined(CH32V003_SPI_NSS_SOFTWARE_PC4) 
		SPI_NSS_software_high();
	#endif
	SPI_write_16(data);
	SPI_wait_TX_complete();
	asm volatile("nop");
	SPI_wait_RX_available();
	#if defined(CH32V003_SPI_NSS_SOFTWARE_PC3) || defined(CH32V003_SPI_NSS_SOFTWARE_PC4) 
		SPI_NSS_software_low();
	#endif
	return SPI_read_16();
}

static inline void SPI_poweroff() {
	SPI_end();
	RCC->APB2PCENR &= ~RCC_APB2Periph_SPI1;
}
static inline void SPI_poweron() {
	RCC->APB2PCENR |= RCC_APB2Periph_GPIOC | RCC_APB2Periph_SPI1;
}

static inline void kill_interrrupts() {
	EXT1_INTENR_backup = EXTI->INTENR;
	// zero the interrupt enable register to disable all interrupts
	EXTI->INTENR = 0;
}
static inline void restore_interrupts() {
	EXTI->INTENR = EXT1_INTENR_backup;
}



//########  small internal function definitions, static inline
static inline void SPI_wait_TX_complete() {
	while(!(SPI1->STATR & SPI_STATR_TXE)) {}
}
static inline uint8_t SPI_is_RX_empty() {
	return SPI1->STATR & SPI_STATR_RXNE;
}
static inline void SPI_wait_RX_available() {
	while(!(SPI1->STATR & SPI_STATR_RXNE)) {}
}
static inline void SPI_wait_not_busy() {
	while((SPI1->STATR & SPI_STATR_BSY) != 0) {}
}
static inline void SPI_wait_transmit_finished() {
	SPI_wait_TX_complete();
	SPI_wait_not_busy();
}


//######## implementation block
//#define CH32V003_SPI_IMPLEMENTATION //enable so LSP can give you text colors while working on the implementation block, disable for normal use of the library
#if defined(CH32V003_SPI_IMPLEMENTATION)

//no functions here because I think all of the functions are small enough to static inline

#endif // CH32V003_SPI_IMPLEMENTATION
#endif // CH32V003_SPI_H

// Examples of using PWM, TMR, DMA & UART0 to drive WS2812B RGB LEDs

#include "ch32fun.h"
#include "../ws2812bdemo/color_utilities.h"

#define NR_LEDS 4

#define WS2812B_CLOCK_HZ (800 * 1000)
#define WS2812B_CYCLES (FUNCONF_SYSTEM_CORE_CLOCK / WS2812B_CLOCK_HZ)
#define WS2812B_DUTYCYCLE_NS(NANOSECS) ((WS2812B_CYCLES * NANOSECS + 625) / 1250)
#define WS2812B_0 WS2812B_DUTYCYCLE_NS(350)
#define WS2812B_1 WS2812B_DUTYCYCLE_NS(900)

void WS2812B_TMR_SetPixelRGB(uint32_t* buffer, int pixelNumber, int32_t rgb)
{
	uint32_t* dst = &buffer[pixelNumber * 24];
	rgb <<= 8;
	for(int bit = 0; bit != 24; ++bit, rgb <<= 1)
	{
		*dst++ = WS2812B_0 + ((rgb >> 31) & (WS2812B_1 - WS2812B_0));
	}
}
#ifdef CH57x
void WS2812B_PWM_Init(int channel)
{
	const int pins[] = {PA7, PA2, PA3, PA4, PA8};
	funPinMode(pins[channel - 1], GPIO_CFGLR_OUT_10Mhz_PP);
	R16_PWM_CLOCK_DIV = 1;
	R8_PWM_POLAR = 0;
	R8_PWM_CONFIG = RB_PWM_CYC_MOD; // 16 bit data mode
	R16_PWM_CYC_VALUE = WS2812B_CYCLES - 1;
	if (channel < 3)
		(&R16_PWM1_DATA)[channel - 1] = 0;
	else
		(&R16_PWM4_DATA)[channel - 4] = 0;
	R8_PWM_OUT_EN |= RB_PWM1_OUT_EN << (channel - 1);
}

void WS2812B_PWM123_SetPixelRGB(uint16_t* buffer, int pixelNumber, int pwmChannel, int32_t rgb)
{
	// each DMA entry contains data for PWM1, 2 & 3, i.e. 6 bytes
	uint16_t* dst = &buffer[(pixelNumber * 24 + 1) * 3 + (pwmChannel - 1)];
	rgb <<= 8;
	for(int bit = 0; bit != 24; ++bit, dst += 3, rgb <<= 1)
	{
		*dst = WS2812B_0 + ((rgb >> 31) & (WS2812B_1 - WS2812B_0));
	}
}
void WS2812B_PWM123_StartDMA(const uint16_t* buffer, int numLeds)
{
	R8_PWM_DMA_CTRL = 0;
	R32_PWM_DMA_BEG = (uintptr_t)&buffer[0];
	R32_PWM_DMA_END = (uintptr_t)&buffer[(numLeds * 24 + 2) * 3];
	R8_PWM_DMA_CTRL = RB_DMA_SEL|RB_DMA_ENABLE;	
}
void WS2812B_PWM45_SetPixelRGB(uint16_t* buffer, int pixelNumber, int pwmChannel, int32_t rgb)
{
	// each DMA entry contains data for PWM4 & 5, i.e. 4 bytes
	uint16_t* dst = &buffer[(pixelNumber * 24 + 1) * 2 + (pwmChannel - 4)];
	rgb <<= 8;
	for(int bit = 0; bit != 24; ++bit, dst += 2, rgb += rgb)
	{
		*dst = WS2812B_0 + ((rgb >> 31) & (WS2812B_1 - WS2812B_0));
	}
}
void WS2812B_PWM45_StartDMA(const uint16_t* buffer, int numLeds)
{
	R8_PWM_DMA_CTRL = 0;
	R32_PWM_DMA_BEG = (uintptr_t)&buffer[0];
	R32_PWM_DMA_END = (uintptr_t)&buffer[(numLeds * 24 + 2) * 2];
	R8_PWM_DMA_CTRL = RB_DMA_ENABLE;
}
void WS2812B_TMR_StartDMA(const uint32_t* buffer, int numLeds)
{
	// turn off DMA & timer
	R8_TMR_CTRL_DMA = 0;
	R8_TMR_CTRL_MOD = RB_TMR_ALL_CLEAR;
	// set up DMA
	R32_TMR_DMA_BEG = (uintptr_t)&buffer[0];
	R32_TMR_DMA_END = (uintptr_t)&buffer[numLeds * 24 + 1];
	R32_TMR_CNT_END = WS2812B_CYCLES;
	// enable timer and then start the DMA
	R8_TMR_CTRL_MOD = RB_TMR_OUT_EN|RB_TMR_COUNT_EN;
	R8_TMR_CTRL_DMA = RB_TMR_DMA_ENABLE;
}
#else
void WS2812B_TMR1_StartDMA(const uint32_t* buffer, int numLeds)
{
	// turn off DMA & timer
	R8_TMR1_CTRL_DMA = 0;
	R8_TMR1_CTRL_MOD = RB_TMR_ALL_CLEAR;
	// set up DMA
	R32_TMR1_DMA_BEG = (uintptr_t)&buffer[0];
	R32_TMR1_DMA_END = (uintptr_t)&buffer[numLeds * 24 + 1];
	R32_TMR1_CNT_END = WS2812B_CYCLES;
	// enable timer and then start the DMA
	R8_TMR1_CTRL_MOD = RB_TMR_OUT_EN|RB_TMR_COUNT_EN;
	R8_TMR1_CTRL_DMA = RB_TMR_DMA_ENABLE;
}
void WS2812B_TMR2_StartDMA(const uint32_t* buffer, int numLeds)
{
	// turn off DMA & timer
	R8_TMR2_CTRL_DMA = 0;
	R8_TMR2_CTRL_MOD = RB_TMR_ALL_CLEAR;
	// set up DMA
	R32_TMR2_DMA_BEG = (uintptr_t)&buffer[0];
	R32_TMR2_DMA_END = (uintptr_t)&buffer[numLeds * 24 + 1];
	R32_TMR2_CNT_END = WS2812B_CYCLES;
	// enable timer and then start the DMA
	R8_TMR2_CTRL_MOD = RB_TMR_OUT_EN|RB_TMR_COUNT_EN;
	R8_TMR2_CTRL_DMA = RB_TMR_DMA_ENABLE;
}
#endif

#ifdef CH57x
#define TMR_PA9 1
// the PWM buffers start and end with zero entry (duty cycle of 0 = output off). 
// I'm not sure why the start one is needed but you get flickering without it.
uint16_t __attribute__((aligned(4))) WS2812B_PWM123_LedBuffer[(NR_LEDS * 24 + 2) * 3] = {0};
uint16_t __attribute__((aligned(4))) WS2812B_PWM45_LedBuffer[(NR_LEDS * 24 + 2) * 2] = {0};
#endif
// the TMR buffer just needs one zero at the end to turn off the output
uint32_t WS2812B_TMR_LedBuffer[NR_LEDS * 24 + 1] = {0};

#ifdef RB_PIN_U0_INV // CH584/5 support inverted UART output
uint32_t WS2812B_UART_LedBuffer[NR_LEDS];

static const uint8_t WS2812B_UART_LUT[8] = 
{
	~0b0100100, // 9 bits are used to encode 3 bits of WS2812B data.
	~0b1100100, // Each of these 7 bit payloads is framed by the
	~0b0101100, // UART high-start and low-stop bits. Entries are
	~0b1101100, // inverted to compensate for the output inversion.
	~0b0100101,
	~0b1100101,
	~0b0101101,
	~0b1101101,
};
void WS2812B_UART0_Send(const uint32_t* ledData, int numLeds)
{
	UART0->IER = RB_IER_RESET;
	R16_PIN_ALTERNATE |= RB_PIN_U0_INV;
	UART0->FCR = RB_FCR_FIFO_EN | RB_FCR_TX_FIFO_CLR;
	UART0->LCR = 2; // word length: 7 bits
	UART0->DL = (FUNCONF_SYSTEM_CORE_CLOCK / 2400000 + 4) / 8;
	UART0->DIV = 1;
	UART0->IER = RB_IER_TXD_EN;
	for(int i = 0; i < numLeds; ++i)
	{
		uint32_t rgb = ledData[i];
		for(int j = 0; j < 8; ++j)
		{
			uint8_t nextByte = WS2812B_UART_LUT[rgb >> 29];
			while (UART0->TFC == 8)
			{
				// wait for free space in TX FIFO
			}
			UART0->THR_RBR = nextByte;
			rgb <<= 3;
		}
	}
}
#endif

int main()
{
	SystemInit();

#ifdef CH57x
#if TMR_PA9
 	R16_PIN_ALTERNATE_H |= RB_TMR_PIN; // map TMR to PA9
 	funPinMode(PA9, GPIO_CFGLR_OUT_10Mhz_PP); 
#else
	funPinMode(PA7, GPIO_CFGLR_OUT_10Mhz_PP); 
#endif

	WS2812B_PWM_Init(1);
	WS2812B_PWM_Init(2);
	WS2812B_PWM_Init(3);
	WS2812B_PWM_Init(4);
	WS2812B_PWM_Init(5); // TODO: PWM5 doesn't seem to work
#else
	funPinMode(PA10, GPIO_CFGLR_OUT_10Mhz_PP); // TMR1
	funPinMode(PA11, GPIO_CFGLR_OUT_10Mhz_PP); // TMR2

#ifdef RB_PIN_U0_INV	
	funPinMode(PB7, GPIO_CFGLR_OUT_10Mhz_PP); // UART
	funDigitalWrite(PB7, FUN_LOW);
#endif
#endif


	uint8_t hue = 0;
	uint8_t sat = 255;
	uint8_t val = 64; // brightness control
	for(;;)
	{
		uint8_t hue1 = hue++;
		for(int i = 0; i < NR_LEDS; ++i)
		{
#ifdef CH57x
			// up to 5 strips of LEDs can be driven by PWM DMA
			WS2812B_PWM123_SetPixelRGB(WS2812B_PWM123_LedBuffer, i, 1, EHSVtoHEX(hue1, sat, val));
			WS2812B_PWM123_SetPixelRGB(WS2812B_PWM123_LedBuffer, i, 2, EHSVtoHEX(hue1 + 64, sat, val));
			WS2812B_PWM123_SetPixelRGB(WS2812B_PWM123_LedBuffer, i, 3, EHSVtoHEX(hue1 + 128, sat, val));
			WS2812B_PWM45_SetPixelRGB(WS2812B_PWM45_LedBuffer, i, 4, EHSVtoHEX(hue1 + 160, sat, val));
			WS2812B_PWM45_SetPixelRGB(WS2812B_PWM45_LedBuffer, i, 5, EHSVtoHEX(hue1 + 192, sat, val));
#endif
#ifdef RB_PIN_U0_INV
			WS2812B_UART_LedBuffer[i] = EHSVtoHEX(hue1, sat, val) << 8;
#endif
			WS2812B_TMR_SetPixelRGB(WS2812B_TMR_LedBuffer, i, EHSVtoHEX(hue1 + 224, sat, val));
			hue1 += 256 / NR_LEDS;
		}
#ifdef CH57x
		WS2812B_TMR_StartDMA(WS2812B_TMR_LedBuffer, NR_LEDS);
		WS2812B_PWM123_StartDMA(WS2812B_PWM123_LedBuffer, NR_LEDS);		
		Delay_Ms(5); // can't run PWM123 & PWM45 DMAs simultaneously
		WS2812B_PWM45_StartDMA(WS2812B_PWM45_LedBuffer, NR_LEDS);
		Delay_Ms(10);
#else
		// two strips can be driven simultaneously by TMR PWM DMA
		WS2812B_TMR1_StartDMA(WS2812B_TMR_LedBuffer, NR_LEDS);
		WS2812B_TMR2_StartDMA(WS2812B_TMR_LedBuffer, NR_LEDS);
#ifdef RB_PIN_U0_INV
		WS2812B_UART0_Send(WS2812B_UART_LedBuffer, NR_LEDS);
#endif
		Delay_Ms(15);
#endif
	}
}

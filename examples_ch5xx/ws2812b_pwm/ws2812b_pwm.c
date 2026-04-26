// Simple example of using PWM, TMR and DMA to drive WS2812B RGB LEDs
// NOTE: CONNECT WS2812B's to PA4 and/or PA7 (CH570/2)

#include "ch32fun.h"
#include "../ws2812bdemo/color_utilities.h"

#define WS2812B_CLOCK_HZ (800 * 1000)
#define WS2812B_CYCLES (FUNCONF_SYSTEM_CORE_CLOCK / WS2812B_CLOCK_HZ)
#define WS2812B_DUTYCYCLE_NS(NANOSECS) ((WS2812B_CYCLES * NANOSECS + 625) / 1250)
#define WS2812B_0 WS2812B_DUTYCYCLE_NS(350)
#define WS2812B_1 WS2812B_DUTYCYCLE_NS(900)

#ifdef CH57x
void WriteWS2812B_TMR_SetPixelRGB(uint32_t* buffer, int pixelNumber, int32_t rgb)
{
	uint32_t* dst = &buffer[pixelNumber * 24];
	rgb <<= 8;
	for(int bit = 0; bit != 24; ++bit, rgb <<= 1)
	{
		*dst++ = WS2812B_0 + ((rgb >> 31) & (WS2812B_1 - WS2812B_0));
	}
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

void WS2812B_PWM_Init(int channel)
{
	const int pins[] = {PA7, PA2, PA3, PA4, PA8};
	funPinMode(pins[channel - 1], GPIO_CFGLR_OUT_10Mhz_PP);
	R16_PWM_CLOCK_DIV = 1;
	R8_PWM_POLAR = 0;
	R8_PWM_CONFIG = RB_PWM_CYC_MOD; // 16 bit data mode
	R16_PWM_CYC_VALUE = WS2812B_CYCLES - 1;
	(&R16_PWM1_DATA)[channel - 1] = 0;
	R8_PWM_OUT_EN |= RB_PWM1_OUT_EN << (channel - 1);
}

void WriteWS2812B_PWM123_SetPixelRGB(uint16_t* buffer, int pixelNumber, int pwmChannel, int32_t rgb)
{
	// each DMA entry contains data for PWM1, 2 & 3, i.e. 6 bytes
	uint16_t* dst = &buffer[pixelNumber * 24 * 3 + (pwmChannel - 1)];
	rgb <<= 8;
	for(int bit = 0; bit != 24; ++bit, dst += 3, rgb <<= 1)
	{
		*dst = WS2812B_0 + ((rgb >> 31) & (WS2812B_1 - WS2812B_0));
	}
}
void WS2812B_PWM123_StartDMA(const uint16_t* buffer, int numLeds)
{
	R8_PWM_DMA_CTRL = 0;
	Delay_Us(10); // todo: how much delay (if any) is needed
	R32_PWM_DMA_BEG = (uintptr_t)&buffer[0];
	R32_PWM_DMA_END = (uintptr_t)&buffer[(numLeds * 24 + 2) * 3];
	Delay_Us(10); // todo: how much delay (if any) is needed
	R8_PWM_DMA_CTRL = RB_DMA_SEL|RB_DMA_ENABLE;	
}
void WriteWS2812B_PWM45_SetPixelRGB(uint16_t* buffer, int pixelNumber, int pwmChannel, int32_t rgb)
{
	// each DMA entry contains data for PWM4 & 5, i.e. 4 bytes
	uint16_t* dst = &buffer[pixelNumber * 24 * 2 + (pwmChannel - 4)];
	rgb <<= 8;
	for(int bit = 0; bit != 24; ++bit, dst += 2, rgb <<= 1)
	{
		*dst = WS2812B_0 + ((rgb >> 31) & (WS2812B_1 - WS2812B_0));
	}
}
void WS2812B_PWM45_StartDMA(const uint16_t* buffer, int numLeds)
{
	R8_PWM_DMA_CTRL = 0;
	Delay_Us(10); // todo: how much delay (if any) is needed
	R32_PWM_DMA_BEG = (uintptr_t)&buffer[0];
	R32_PWM_DMA_END = (uintptr_t)&buffer[(numLeds * 24 + 1) * 2];
	Delay_Us(10); // todo: how much delay (if any) is needed
	R8_PWM_DMA_CTRL = RB_DMA_ENABLE;
}
#else
#error Sorry, not implemented yet!
#endif

#define NR_LEDS 4
#define TMR_PA9 1

uint16_t __attribute__((aligned(4))) WS2812B_PWM123_LedBuffer[(NR_LEDS * 24 + 2) * 3] = {0};
uint16_t __attribute__((aligned(4))) WS2812B_PWM45_LedBuffer[(NR_LEDS * 24 + 1) * 2] = {0};
uint32_t WS2812B_TMR_LedBuffer[NR_LEDS * 24 + 1] = {0};

int main()
{
	SystemInit();

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

	uint8_t hue = 0;
	uint8_t sat = 255;
	uint8_t val = 64; // brightness control
	for(;;)
	{
		uint8_t hue1 = hue++;
		for(int i = 0; i < NR_LEDS; ++i)
		{
			WriteWS2812B_PWM123_SetPixelRGB(WS2812B_PWM123_LedBuffer, i, 1, EHSVtoHEX(hue1, sat, val));
			WriteWS2812B_PWM123_SetPixelRGB(WS2812B_PWM123_LedBuffer, i, 2, EHSVtoHEX(hue1 + 64, sat, val));
			WriteWS2812B_PWM123_SetPixelRGB(WS2812B_PWM123_LedBuffer, i, 3, EHSVtoHEX(hue1 + 128, sat, val));
			WriteWS2812B_PWM45_SetPixelRGB(WS2812B_PWM45_LedBuffer, i, 4, EHSVtoHEX(hue1 + 160, sat, val));
			WriteWS2812B_PWM45_SetPixelRGB(WS2812B_PWM45_LedBuffer, i, 5, EHSVtoHEX(hue1 + 192, sat, val));
			WriteWS2812B_TMR_SetPixelRGB(WS2812B_TMR_LedBuffer, i, EHSVtoHEX(hue1 + 224, sat, val));
			hue1 += 256 / NR_LEDS;
		}
		WS2812B_TMR_StartDMA(WS2812B_TMR_LedBuffer, NR_LEDS);
		WS2812B_PWM123_StartDMA(WS2812B_PWM123_LedBuffer, NR_LEDS);		
		Delay_Ms(5); // can't run PWM123 & PWM45 DMAs simultaneously
		WS2812B_PWM45_StartDMA(WS2812B_PWM45_LedBuffer, NR_LEDS);
		Delay_Ms(10);
	}
}

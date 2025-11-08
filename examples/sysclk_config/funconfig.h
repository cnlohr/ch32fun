#ifndef _FUNCONFIG_H
#define _FUNCONFIG_H

// Place configuration items here, you can see a full list in ch32fun/ch32fun.h
// To reconfigure to a different processor, update TARGET_MCU in the  Makefile

#define FUNCONF_USE_HSE 1  			// external crystal on PA1 PA2
#define FUNCONF_USE_HSI 0    		// internal 24MHz clock oscillator
#define FUNCONF_USE_PLL 1			// use PLL x2
#define FUNCONF_HSE_BYPASS 0 		// bypass the HSE when using an external clock source
									// requires enabled HSE
#define FUNCONF_USE_CLK_SEC	1		// clock security system

#endif // _FUNCONFIG_Hma

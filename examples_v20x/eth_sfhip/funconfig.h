#ifndef _FUNCONFIG_H
#define _FUNCONFIG_H

// Place configuration items here, you can see a full list in ch32fun/ch32fun.h
// To reconfigure to a different processor, update TARGET_MCU in the  Makefile

// #define CH32V20x 1
#define FUNCONF_USE_HSE 1
#define FUNCONF_SYSTEM_CORE_CLOCK 120000000
#define FUNCONF_PLL_MULTIPLIER 15
#define FUNCONF_SYSTICK_USE_HCLK 1

#endif

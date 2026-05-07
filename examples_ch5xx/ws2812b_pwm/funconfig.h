#ifndef _FUNCONFIG_H
#define _FUNCONFIG_H

#if !defined(CH585) && !defined(CH584)
#define FUNCONF_USE_HSI           0 // CH5xx does not have HSI
#define FUNCONF_USE_HSE           1
#define CLK_SOURCE_CH5XX          CLK_SOURCE_PLL_48MHz // default so not really needed
#define FUNCONF_SYSTEM_CORE_CLOCK (48 * 1000 * 1000)   // keep in line with CLK_SOURCE_CH5XX
#else
#define FUNCONF_USE_HSI           0 // CH585/584 has HSI
#define FUNCONF_USE_HSE           1
#define CLK_SOURCE_CH5XX          CLK_SOURCE_HSE_PLL_78MHz
#define FUNCONF_SYSTEM_CORE_CLOCK (78 * 1000 * 1000)   // keep in line with CLK_SOURCE_CH5XX
#endif

#define FUNCONF_DEBUG_HARDFAULT   0
#define FUNCONF_USE_CLK_SEC       0
#define FUNCONF_USE_DEBUGPRINTF   1 // saves 16 bytes, enable / remove if you want printf over swio

#endif

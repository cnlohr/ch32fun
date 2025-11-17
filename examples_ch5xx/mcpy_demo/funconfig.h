#ifndef _FUNCONFIG_H
#define _FUNCONFIG_H

#define FUNCONF_USE_HSI           0 // CH5xx does not have HSI
#define FUNCONF_USE_HSE           1
#define CLK_SOURCE_CH5XX          CLK_SOURCE_PLL_60MHz // default so not really needed
#define FUNCONF_SYSTEM_CORE_CLOCK 60 * 1000 * 1000     // keep in line with CLK_SOURCE_CH5XX

#define FUNCONF_DEBUG_HARDFAULT   1
#define FUNCONF_USE_CLK_SEC       0

#endif

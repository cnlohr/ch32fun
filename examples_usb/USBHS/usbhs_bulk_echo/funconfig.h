#ifndef _FUNCONFIG_H
#define _FUNCONFIG_H

#define FUNCONF_USE_HSI           0
#define FUNCONF_USE_HSE           1
#define CLK_SOURCE_CH5XX          CLK_SOURCE_HSE_PLL_62_4MHz
#define FUNCONF_SYSTEM_CORE_CLOCK 624 * 100 * 1000     // keep in line with CLK_SOURCE_CH5XX

#define FUNCONF_DEBUG_HARDFAULT   0
#define FUNCONF_USE_CLK_SEC       0
#define FUNCONF_USE_DEBUGPRINTF   0

#endif

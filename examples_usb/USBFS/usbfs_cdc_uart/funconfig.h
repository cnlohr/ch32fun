#ifndef _FUNCONFIG_H
#define _FUNCONFIG_H

// Though this should be on by default we can extra force it on.
#define FUNCONF_USE_DEBUGPRINTF   1
// #define FUNCONF_USE_HSE           1
#define FUNCONF_DEBUGPRINTF_TIMEOUT 0x800000
#define FUNCONF_ENABLE_HPE        0
#define FUNCONF_SYSTICK_USE_HCLK  1

// #define FUNCONF_USE_HSI           0 // CH5xx does not have HSI
// #define CLK_SOURCE_CH5XX          CLK_SOURCE_PLL_60MHz // default so not really needed
// #define FUNCONF_SYSTEM_CORE_CLOCK 60 * 1000 * 1000     // keep in line with CLK_SOURCE_CH5XX
// #define FUNCONF_DEBUG_HARDFAULT   0
// #define FUNCONF_USE_CLK_SEC       0
#define FUNCONF_INIT_ANALOG       0 // ADC is not implemented yet

#endif


#ifndef _FUNCONFIG_H
#define _FUNCONFIG_H

// Though this should be on by default we can extra force it on.
#define FUNCONF_USE_DEBUGPRINTF     1
#define FUNCONF_ENABLE_HPE          0
#define FUNCONF_SYSTICK_USE_HCLK    1

#if defined(CH32V10x)
#define FUNCONF_SYSTEM_CORE_CLOCK   72000000
#define FUNCONF_PLL_MULTIPLIER      9
#define FUNCONF_USE_HSE             1
#define FUNCONF_USE_5V_VDD          1
#elif defined(CH32X03x)
#define FUNCONF_USE_HSI             1
#elif defined(CH5xx)
#define FUNCONF_USE_HSI             0 // CH5xx does not have HSI
#define CLK_SOURCE_CH5XX            CLK_SOURCE_PLL_60MHz // default so not really needed
#define FUNCONF_SYSTEM_CORE_CLOCK   60 * 1000 * 1000     // keep in line with CLK_SOURCE_CH5XX
#define FUNCONF_USE_CLK_SEC         0
#else
#define FUNCONF_USE_HSI             0
#define FUNCONF_USE_HSE             1
#endif

#define FUNCONF_DEBUG_HARDFAULT     1

#endif


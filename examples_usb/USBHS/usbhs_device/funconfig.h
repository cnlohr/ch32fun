#ifndef _FUNCONFIG_H
#define _FUNCONFIG_H

// Though this should be on by default we can extra force it on.
#define FUNCONF_USE_DEBUGPRINTF     1
#define FUNCONF_ENABLE_HPE          0
#define FUNCONF_SYSTICK_USE_HCLK    1

#if defined(CH58x)
#define CLK_SOURCE_CH5XX            CLK_SOURCE_HSE_PLL_78MHz // default so not really needed
#define FUNCONF_SYSTEM_CORE_CLOCK   78 * 1000 * 1000     // keep in line with CLK_SOURCE_CH5XX
#define FUNCONF_USE_CLK_SEC         0
#define FUNCONF_USE_HSE             1
#else
#define FUNCONF_USE_HSE             1
#define FUNCONF_USE_HSI             0 // HSI is perfectly usable but requires active clock tuning with "FUSB_SOF_HSITRIM"
#endif

#define FUNCONF_DEBUG_HARDFAULT     1

#endif


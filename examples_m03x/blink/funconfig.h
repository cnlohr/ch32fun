#ifndef _FUNCONFIG_H
#define _FUNCONFIG_H

// CH32M030 configuration
// HSI 8 MHz -> PLL x18 / HPRE/2 -> SYSCLK 72 MHz (PLL multiplier is hardware-fixed on M030)
#define FUNCONF_USE_HSI                  1
#define FUNCONF_USE_HSE                  0
#define FUNCONF_USE_PLL                  1
#define FUNCONF_SYSTEM_CORE_CLOCK        72000000

// Disable hardware-prologue/epilogue extension for safety on first bring-up
// (QingKeV3B core; can be enabled later once confirmed stable)
#define FUNCONF_ENABLE_HPE               0

#endif
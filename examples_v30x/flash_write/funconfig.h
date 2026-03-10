#ifndef _FUNCONFIG_H
#define _FUNCONFIG_H

// For flash to work on v2x and v3x, HB clock must be at most 120 MHz. So you can
// either run the system at 120 MHz max, or set HB prescaler to /2 or greater.
// In this example we run at 112 MHz.
#define FUNCONF_SYSTEM_CORE_CLOCK 112000000
#define FUNCONF_PLL_MULTIPLIER 14

// Though this should be on by default we can extra force it on.
#define FUNCONF_USE_DEBUGPRINTF 1
#define FUNCONF_DEBUGPRINTF_TIMEOUT (1<<31) // Wait for a very very long time.

#define CH32V30x           1

#endif


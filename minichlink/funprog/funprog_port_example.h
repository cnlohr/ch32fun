#ifndef FUNPROG_PORT_H
#define FUNPROG_PORT_H

// Define your debug print method here
#if DEBUG_PRINT
#define FUN_PRINTF_DEBUG(format, args...) printf(format, ##args)
#else
#define FUN_PRINTF_DEBUG(format, args...)
#endif

// Macros to control built-in LEDs
#define STATUS_LED_ON
#define STATUS_LED_OFF

// Macros for setting output power pins and toggling them on and off
#define POWER_SETUP
#define POWER_ON
#define POWER_OFF

// Define your delay macros here
#define DELAY_US(n)
#define DELAY_MS(n)

// Some chips need to slow down the protocol speed to work stable. Check available implementations
#define SLOW_DOWN_PROTOCOL

// Platform dependent attribute to put functions in RAM
#define IRAM

// Implement the function to enable clock output on a dedicated pin
#if defined(MCO_SUPPORT) && MCO_SUPPORT
static void EnableMCO( int use_apll, int sdm0, int sdm1, int sdm2, int odiv )
{
}
#endif

#endif

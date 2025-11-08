#ifndef _FUNCONFIG_H
#define _FUNCONFIG_H

#define SYSTICK_USE_HCLK 1

// Place configuration items here, you can see a full list in ch32fun/ch32fun.h
// To reconfigure to a different processor, update TARGET_MCU in the  Makefile

// This tool does require more package specifity.

// Package definition
#define CH32V003F4U6 (1)  // QFN-20
#define CH32V003A4M6 (0)  // SOP-16
#define CH32V003J4M6 (0)  // SOP-8

// also requires PIO change to monitor
// #define FUNCONF_USE_UARTPRINTF 1
// disable printf
// #define FUNCONF_NULL_PRINTF 1

#endif

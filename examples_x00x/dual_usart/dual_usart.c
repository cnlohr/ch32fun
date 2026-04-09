/*
 * Example for using USART1 and USART2 on CH32V00x parts.
 * Default configuration uses:
 *   USART1: PD5 TX, PD6 RX, remap value 0, 115200 8N1
 *   USART2: PD2 TX, PD3 RX, remap value 3, 115200 8N1
 *
 * Intended user-tunable defines in this file are:
 *   USE_USART1 / USE_USART2
 *   USART1_TX_PIN / USART1_RX_PIN
 *   USART2_TX_PIN / USART2_RX_PIN
 *   USART1_BAUD / USART2_BAUD
 *   USART1_CTLR1_FORMAT / USART1_CTLR2_FORMAT
 *   USART2_CTLR1_FORMAT / USART2_CTLR2_FORMAT
 *   USART1_RM_VALUE / USART2_RM_VALUE
 *
 * Do not change USART1_RM_SHIFT or USART2_RM_SHIFT. Those are fixed bit
 * positions of the remap fields inside AFIO->PCFR1 on CH32V00x hardware.
 */

#include "ch32fun.h"
#include <stdio.h>

#ifndef USE_USART1
#define USE_USART1         1
#endif

#ifndef USE_USART2
#define USE_USART2         1
#endif

#if !USE_USART1 && !USE_USART2
#error At least one of USE_USART1 or USE_USART2 must be enabled.
#endif

#ifndef USART1_TX_PIN
#define USART1_TX_PIN       PD5
#endif

#ifndef USART1_RX_PIN
#define USART1_RX_PIN       PD6
#endif

#ifndef USART2_TX_PIN
#define USART2_TX_PIN       PD2
#endif

#ifndef USART2_RX_PIN
#define USART2_RX_PIN       PD3
#endif

#ifndef USART1_BAUD
#define USART1_BAUD         115200UL
#endif

#ifndef USART2_BAUD
#define USART2_BAUD         115200UL
#endif

// These CTLR1/CTLR2 defaults implement 8N1 on each port:
// - 8 data bits: no USART_CTLR1_M bit
// - no parity: no USART_CTLR1_PCE bit
// - 1 stop bit: USART_StopBits_1
// Override these if you want a different framing such as 8E1.
#ifndef USART1_CTLR1_FORMAT
#define USART1_CTLR1_FORMAT (USART_Mode_Tx | USART_Mode_Rx)
#endif

#ifndef USART1_CTLR2_FORMAT
#define USART1_CTLR2_FORMAT USART_StopBits_1
#endif

#ifndef USART2_CTLR1_FORMAT
#define USART2_CTLR1_FORMAT (USART_Mode_Tx | USART_Mode_Rx)
#endif

#ifndef USART2_CTLR2_FORMAT
#define USART2_CTLR2_FORMAT USART_StopBits_1
#endif

// Do not edit these shift constants. They are derived from the CH32V00x AFIO
// register layout: USART1 remap lives in PCFR1 bits 9:6 and USART2 remap
// lives in bits 22:20, so the remap values must be shifted into those exact
// positions before writing the register.
#define USART1_RM_SHIFT     6u
#define USART2_RM_SHIFT     20u

#ifndef USART1_RM_VALUE
// This is user-configurable. Remap value 0 keeps USART1 on its default
// routing, which for this example is TX on PD5 and RX on PD6.
#define USART1_RM_VALUE     0u
#endif

#ifndef USART2_RM_VALUE
// This is user-configurable. Remap value 3 selects the routing used by the
// working CH32V006 dual-UART setup in this repository: USART2 TX on PD2 and
// RX on PD3.
#define USART2_RM_VALUE     3u
#endif

static void usart_write_byte(USART_TypeDef *usart, uint8_t byte)
{
	// TXE indicates that the transmit data register is empty and can accept
	// the next byte.
	while((usart->STATR & USART_STATR_TXE) == 0u)
		;
	usart->DATAR = byte;
}

static void usart_write_string(USART_TypeDef *usart, const char *text)
{
	// Keep the helper simple: send characters until the terminating '\0'.
	while(*text != '\0')
		usart_write_byte(usart, (uint8_t)*text++);
}

static void usart_init(USART_TypeDef *usart, uint32_t baud, uint32_t ctlr1_format, uint32_t ctlr2_format)
{
	// Reset the control registers to a known state before programming the
	// framing and baud rate.
	usart->CTLR1 = 0;
	usart->CTLR2 = ctlr2_format;
	usart->CTLR3 = 0;
	usart->BRR = (FUNCONF_SYSTEM_CORE_CLOCK + (baud / 2u)) / baud;

	// The caller provides the frame format bits for this port. UE is added
	// here so every configuration path still enables the peripheral.
	usart->CTLR1 = ctlr1_format | USART_CTLR1_UE;
}

static void configure_dual_usart_remap(void)
{
	uint32_t pcfr1 = AFIO->PCFR1;

	// Clear the existing USART remap fields first so the example can be
	// retargeted by changing only USART1_RM_VALUE / USART2_RM_VALUE.
	pcfr1 &= ~(AFIO_PCFR1_USART1_RM | AFIO_PCFR1_USART2_RM);
#if USE_USART1
	pcfr1 |= (USART1_RM_VALUE << USART1_RM_SHIFT);
#endif
#if USE_USART2
	pcfr1 |= (USART2_RM_VALUE << USART2_RM_SHIFT);
#endif
	AFIO->PCFR1 = pcfr1;
}

static void gpio_setup(void)
{
	// AFIO must be enabled before changing remap settings. GPIOD carries the
	// default example UART pins for both serial ports.
	RCC->APB2PCENR |= RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOD;
	configure_dual_usart_remap();

	// TX pins use alternate-function push-pull. RX pins are plain inputs.
#if USE_USART1
	funPinMode(USART1_TX_PIN, GPIO_CFGLR_OUT_10Mhz_AF_PP);
	funPinMode(USART1_RX_PIN, GPIO_CFGLR_IN_FLOAT);
#endif
#if USE_USART2
	funPinMode(USART2_TX_PIN, GPIO_CFGLR_OUT_10Mhz_AF_PP);
	funPinMode(USART2_RX_PIN, GPIO_CFGLR_IN_FLOAT);
#endif
}

static void usart_setup(void)
{
	// Enable only the UART clocks that are actually used by this build.
#if USE_USART1
	RCC->APB2PCENR |= RCC_APB2Periph_USART1;
	usart_init(USART1, USART1_BAUD, USART1_CTLR1_FORMAT, USART1_CTLR2_FORMAT);
#endif
#if USE_USART2
	RCC->APB2PCENR |= RCC_APB2Periph_USART2;
	usart_init(USART2, USART2_BAUD, USART2_CTLR1_FORMAT, USART2_CTLR2_FORMAT);
#endif
}

int main(void)
{
	uint32_t counter = 0;
	char message[80];

	SystemInit();
	gpio_setup();
	usart_setup();

	// Send a startup banner on each enabled port so it is immediately obvious
	// which UARTs are active in the current build.
#if USE_USART1
	usart_write_string(USART1, "\r\nCH32V00x dual_usart example\r\n");
	usart_write_string(USART1, "USART1 ready\r\n");
#endif
#if USE_USART2
	usart_write_string(USART2, "\r\nCH32V00x dual_usart example\r\n");
	usart_write_string(USART2, "USART2 ready\r\n");
#endif

	while(1)
	{
#if USE_USART1
		snprintf(message, sizeof(message), "tick %lu via USART1\r\n", counter);
		usart_write_string(USART1, message);
#endif

#if USE_USART2
		snprintf(message, sizeof(message), "tick %lu via USART2\r\n", counter);
		usart_write_string(USART2, message);
#endif

		counter++;
		Delay_Ms(1000);
	}
}

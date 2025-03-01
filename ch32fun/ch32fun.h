/* SPDX-License-Identifier: MIT */
// This contains a copy of ch32v00x.h and core_riscv.h ch32v00x_conf.h and other misc functions See copyright explanation at the end of the file.

#ifndef __CH32FUN_H
#define __CH32FUN_H

#include "funconfig.h"

/*****************************************************************************
	CH32V003 BASICS

	1. Be sure to see configuration section below!

	2. Backend Initialization
		SystemInit();

	3. Arduino-like I/O
		funGpioInitAll();
		funPinMode( PA2, GPIO_CFGLR_OUT_10Mhz_PP );
		funDigitalWrite( PA2, FUN_HIGH );
		funDigitalWrite( PA2, FUN_LOW );
		funAnalogRead( 0 ); // Not Pin number, but rather analog number.

	4. Delays
		Delay_Us(n)
		Delay_Ms(n)
		DelaySysTick( uint32_t n );

	5. printf
		printf, _write may be semihosted, or printed to UART.

		poll_input, handle_debug_input may be used with semihsoting to accept input from host.

		For UART printf, on:
			CH32V003, Port D5, 115200 8n1
			CH32V203, Port A9, 115200 8n1

		Modifications can be made to SetupUart, or your own version as desired.

    6. ISR Control Routines
		__enable_irq();    // For global interrupt enable
		__disable_irq();   // For global interrupt disable
		__isenabled_irq(); // For seeing if interrupts are enabled.
		NVIC_EnableIRQ(IRQn_Type IRQn) // To enable a specific interrupt

	7. Hardware MMIO structs, i.e.
		SysTick->CNT = current system tick counter (can be Hclk or Hclk/8)
		TIM2->CH1CVR = direct control over a PWM output

    8. Default debug behavior, when semihosting:
        a. You get access to DidDebuggerAttach() - so you can see if a debugger has attached.
        b. WaitForDebuggerToAttach( int timeout_ms ) - if timeout_ms == 0, will wait for forever.
        c. printf will wait 120ms (configurable) to make sure it doesn't drop data.  Otherwise,
           printf will fast-path to exit after the first timeout. It will still do the string
           formatting, but will not wait on output. Timeout is configured with
           FUNCONF_DEBUGPRINTF_TIMEOUT.
        d. If you hard fault, it will wait indefinitely for a debugger to attach, once attached,
           will printf the fault cause, and the memory address of the fault. Space can be saved
           by setting FUNCONF_DEBUG_HARDFAULT to 0.
*/



/******************************************************************************
 * CH32V003 Fun Configs; please define any non-default options in funconfig.h *

#define FUNCONF_USE_PLL 1               // Use built-in 2x PLL 
#define FUNCONF_USE_HSI 1               // Use HSI Internal Oscillator
#define FUNCONF_USE_HSE 0               // Use External Oscillator
#define FUNCONF_HSITRIM 0x10            // Use factory calibration on HSI Trim.
#define FUNCONF_SYSTEM_CORE_CLOCK 48000000  // Computed Clock in Hz (Default only for 003, other chips have other defaults)
#define FUNCONF_HSE_BYPASS 0            // Use HSE Bypass feature (for oscillator input)
#define FUNCONF_USE_CLK_SEC	1			// Use clock security system, enabled by default
#define FUNCONF_USE_DEBUGPRINTF 1
#define FUNCONF_USE_UARTPRINTF  0
#define FUNCONF_NULL_PRINTF 0           // Have printf but direct it "nowhere"
#define FUNCONF_SYSTICK_USE_HCLK 0      // Should systick be at 48 MHz (1) or 6MHz (0) on an '003.  Typically set to 0 to divide HCLK by 8.
#define FUNCONF_TINYVECTOR 0            // If enabled, Does not allow normal interrupts.
#define FUNCONF_UART_PRINTF_BAUD 115200 // Only used if FUNCONF_USE_UARTPRINTF is set.
#define FUNCONF_DEBUGPRINTF_TIMEOUT 0x80000 // Arbitrary time units, this is around 120ms.
#define FUNCONF_ENABLE_HPE 1            // Enable hardware interrupt stack.  Very good on QingKeV4, i.e. x035, v10x, v20x, v30x, but questionable on 003. 
                                        // If you are using that, consider using INTERRUPT_DECORATOR as an attribute to your interrupt handlers.
#define FUNCONF_USE_5V_VDD 0            // Enable this if you plan to use your part at 5V - affects USB and PD configration on the x035.
#define FUNCONF_DEBUG_HARDFAULT    1    // Log fatal errors with "printf"
*/

// Sanity check for when porting old code.
#if defined(CH32V10x) || defined(CH32V20x) || defined(CH32V30x) || defined(CH32X03x)
	#if defined(CH32V003)
		#error Cannot define CH32V003 and another arch.
	#endif
#endif

#if !defined(FUNCONF_USE_DEBUGPRINTF) && !defined(FUNCONF_USE_UARTPRINTF)
	#define FUNCONF_USE_DEBUGPRINTF 1
#endif

#if defined(FUNCONF_USE_UARTPRINTF) && FUNCONF_USE_UARTPRINTF && !defined(FUNCONF_UART_PRINTF_BAUD)
	#define FUNCONF_UART_PRINTF_BAUD 115200
#endif

#if defined(FUNCONF_USE_DEBUGPRINTF) && FUNCONF_USE_DEBUGPRINTF && !defined(FUNCONF_DEBUGPRINTF_TIMEOUT)
	#define FUNCONF_DEBUGPRINTF_TIMEOUT 0x80000
#endif

#if defined(FUNCONF_USE_HSI) && defined(FUNCONF_USE_HSE) && FUNCONF_USE_HSI && FUNCONF_USE_HSE
       #error FUNCONF_USE_HSI and FUNCONF_USE_HSE cannot both be set
#endif

#if !defined( FUNCONF_USE_HSI ) && !defined( FUNCONF_USE_HSE )
	#define FUNCONF_USE_HSI 1 // Default to use HSI
	#define FUNCONF_USE_HSE 0
#endif

#if defined( CH32X03x ) && FUNCONF_USE_HSE
	#error No HSE in CH32X03x
#endif

#if !defined( FUNCONF_USE_PLL )
	#if defined( CH32X03x )
		#define FUNCONF_USE_PLL 0 // No PLL on X03x
	#else
		#define FUNCONF_USE_PLL 1 // Default to use PLL
	#endif
#endif

#if !defined( FUNCONF_DEBUG_HARDFAULT )
	#define FUNCONF_DEBUG_HARDFAULT 1
#endif

#if defined( CH32X03x ) && FUNCONF_USE_PLL
	#error No PLL on the X03x
#endif

#ifndef FUNCONF_ENABLE_HPE
	#define FUNCONF_ENABLE_HPE 0
#endif

#if FUNCONF_ENABLE_HPE == 1
	#define INTERRUPT_DECORATOR  __attribute__((interrupt("WCH-Interrupt-fast")))
#else
	#define INTERRUPT_DECORATOR  __attribute__((interrupt))
#endif


#if !defined( FUNCONF_USE_CLK_SEC )
	#define FUNCONF_USE_CLK_SEC  1// use clock security system by default
#endif

#ifndef HSE_VALUE
	#if defined(CH32V003)
		#define HSE_VALUE                 (24000000) // Value of the External oscillator in Hz, default
	#elif defined(CH32V10x)
		#define HSE_VALUE				  (8000000)
	#elif defined(CH32V20x)
		#if defined(CH32V20x_D8) || defined(CH32V20x_D8W)
		#define HSE_VALUE    			  (32000000)
		#else
		#define HSE_VALUE    			  (8000000)
		#endif
	#elif defined(CH32V30x)
		#define HSE_VALUE				  (8000000)
	#endif
#endif

#ifndef HSI_VALUE
	#if defined(CH32V003)
		#define HSI_VALUE                 (24000000) // Value of the Internal oscillator in Hz, default.
	#elif defined(CH32X03x)
		#define HSI_VALUE				  (48000000)
	#elif defined(CH32V10x)
		#define HSI_VALUE				  (8000000)
	#elif defined(CH32V20x)
		#define HSI_VALUE    			  (8000000)
	#elif defined(CH32V30x)
		#define HSI_VALUE				  (8000000)
	#endif
#endif

#ifndef FUNCONF_HSITRIM
	#define FUNCONF_HSITRIM 0x10  // Default (Chip default)
#endif

#ifndef FUNCONF_USE_PLL
	#define FUNCONF_USE_PLL 1     // Default, Use PLL.
#endif

#if !defined( FUNCONF_PLL_MULTIPLIER )
	#if defined(FUNCONF_USE_PLL) && FUNCONF_USE_PLL
		#if defined(CH32V10x)
			#define FUNCONF_PLL_MULTIPLIER 10	// Default: 8 * 10 = 80 MHz
		#elif defined(CH32V20x)
			#define FUNCONF_PLL_MULTIPLIER 18	// Default: 8 * 18 = 144 MHz
		#elif defined(CH32V30x)
			#define FUNCONF_PLL_MULTIPLIER 18	// Default: 8 * 18 = 144 MHz
		#else // CH32V003
			#define FUNCONF_PLL_MULTIPLIER 2	// Default: 24 * 2 = 48 MHz
		#endif
	#else
		#define FUNCONF_PLL_MULTIPLIER 1
	#endif
#endif

#ifndef FUNCONF_SYSTEM_CORE_CLOCK
	#if defined(FUNCONF_USE_HSI) && FUNCONF_USE_HSI
		#define FUNCONF_SYSTEM_CORE_CLOCK ((HSI_VALUE)*(FUNCONF_PLL_MULTIPLIER))
	#elif defined(FUNCONF_USE_HSE) && FUNCONF_USE_HSE
		#define FUNCONF_SYSTEM_CORE_CLOCK ((HSE_VALUE)*(FUNCONF_PLL_MULTIPLIER))
	#else
		#error Must define either FUNCONF_USE_HSI or FUNCONF_USE_HSE to be 1.
	#endif
#endif

#ifndef FUNCONF_USE_5V_VDD
	#define FUNCONF_USE_5V_VDD 0
#endif

// Default package for CH32V20x
#if defined(CH32V20x)
#if !defined(CH32V20x_D8W) && !defined(CH32V20x_D8) && !defined(CH32V20x_D6)
	#define CH32V20x_D6              /* CH32V203F6-CH32V203F8-CH32V203G6-CH32V203G8-CH32V203K6-CH32V203K8-CH32V203C6-CH32V203C8 */
	//#define CH32V20x_D8              /* CH32V203RBT6 */
	//#define CH32V20x_D8W             /* CH32V208 */
	#endif
#endif

// Default package for CH32V30x
#if defined(CH32V30x)
#if !defined(CH32V30x_D8) && !defined(CH32V30x_D8C)
	//#define CH32V30x_D8              /* CH32V303x */
	#define CH32V30x_D8C             /* CH32V307x-CH32V305x */
	#endif
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////
// Legacy, for EVT, CMSIS

#define __MPU_PRESENT             0  /* Other CH32 devices does not provide an MPU */
#define __Vendor_SysTickConfig    0  /* Set to 1 if different SysTick Config is used */

#ifndef __ASSEMBLER__  // Things before this can be used in assembly.

#include <stdint.h>

#ifdef __cplusplus
  #define     __I     volatile                /*!< defines 'read only' permissions      */
#else
  #define     __I     volatile const          /*!< defines 'read only' permissions     */
#endif
#define     __O     volatile                  /*!< defines 'write only' permissions     */
#define     __IO    volatile                  /*!< defines 'read / write' permissions   */

#endif // __ASSEMBLER__
	
///////////////////////////////////////////////////////////////////////////////////////////////	
///////////////////////////////////////////////////////////////////////////////////////////////
// Code in this section was originally from __CORE_RISCV_H__

#ifndef __CORE_RISCV_H__
#define __CORE_RISCV_H__

/* define compiler specific symbols */
#if defined(__CC_ARM)
  #define __ASM       __asm     /*!< asm keyword for ARM Compiler          */
  #define __INLINE    __inline  /*!< inline keyword for ARM Compiler       */

#elif defined(__ICCARM__)
  #define __ASM       __asm   /*!< asm keyword for IAR Compiler          */
  #define __INLINE    inline  /*!< inline keyword for IAR Compiler. Only avaiable in High optimization mode! */

#elif defined(__GNUC__)
  #define __ASM       __asm   /*!< asm keyword for GNU Compiler          */
  #define __INLINE    inline  /*!< inline keyword for GNU Compiler       */

#elif defined(__TASKING__)
  #define __ASM       __asm   /*!< asm keyword for TASKING Compiler      */
  #define __INLINE    inline  /*!< inline keyword for TASKING Compiler   */

#endif


#ifdef __cplusplus
 extern "C" {
#endif
	
#ifndef __ASSEMBLER__

/* Standard Peripheral Library old types (maintained for legacy purpose) */
typedef __I uint32_t vuc32;  /* Read Only */
typedef __I uint16_t vuc16;  /* Read Only */
typedef __I uint8_t vuc8;   /* Read Only */

typedef const uint32_t uc32;  /* Read Only */
typedef const uint16_t uc16;  /* Read Only */
typedef const uint8_t uc8;   /* Read Only */

typedef __I int32_t vsc32;  /* Read Only */
typedef __I int16_t vsc16;  /* Read Only */
typedef __I int8_t vsc8;   /* Read Only */

typedef const int32_t sc32;  /* Read Only */
typedef const int16_t sc16;  /* Read Only */
typedef const int8_t sc8;   /* Read Only */

typedef __IO uint32_t  vu32;
typedef __IO uint16_t vu16;
typedef __IO uint8_t  vu8;

typedef uint32_t  u32;
typedef uint16_t u16;
typedef uint8_t  u8;

typedef __IO int32_t  vs32;
typedef __IO int16_t  vs16;
typedef __IO int8_t   vs8;

typedef int32_t  s32;
typedef int16_t s16;
typedef int8_t  s8;

typedef __I uint64_t vuc64;  /* Read Only */
typedef const uint64_t uc64;  /* Read Only */
typedef __I int64_t vsc64;  /* Read Only */
typedef const int64_t sc64;  /* Read Only */
typedef __IO uint64_t  vu64;
typedef uint64_t  u64;
typedef __IO int64_t  vs64;
typedef int64_t  s64;

typedef enum {NoREADY = 0, READY = !NoREADY} ErrorStatus;

typedef enum {DISABLE = 0, ENABLE = !DISABLE} FunctionalState;

typedef enum {RESET = 0, SET = !RESET} FlagStatus, ITStatus;

#define   RV_STATIC_INLINE  static  inline
#endif // __ASSEMBLER__

#ifdef CH32V003
	#include "ch32v003hw.h"
#elif defined( CH32X03x )
	#include "ch32x03xhw.h"
#elif defined( CH32X03x )
	#include "ch32x03xhw.h"
#elif defined( CH32V10x )
	#include "ch32v10xhw.h"
#elif defined( CH32V20x )
	#include "ch32v20xhw.h"
#elif defined( CH32V30x )
	#include "ch32v30xhw.h"
#endif

#if defined(__riscv) || defined(__riscv__) || defined( CH32V003FUN_BASE )

#if __GNUC__ > 10
	#define ADD_ARCH_ZICSR ".option arch, +zicsr\n"
#else
	#define ADD_ARCH_ZICSR 
#endif

#ifndef __ASSEMBLER__

// Enable Global Interrupt
RV_STATIC_INLINE void __enable_irq()
{
	uint32_t result; __ASM volatile( ADD_ARCH_ZICSR "csrr %0," "mstatus": "=r"(result) );
	result |= 0x88;  __ASM volatile( ADD_ARCH_ZICSR "csrw mstatus, %0" : : "r" (result) );
}

// Disable Global Interrupt
RV_STATIC_INLINE void __disable_irq()
{
	uint32_t result; __ASM volatile( ADD_ARCH_ZICSR "csrr %0," "mstatus": "=r"(result) );
	result &= ~0x88; __ASM volatile( ADD_ARCH_ZICSR "csrw mstatus, %0" : : "r" (result) );
}

// Is Global Interrupt enabled (1 = yes, 0 = no)
RV_STATIC_INLINE uint8_t __isenabled_irq(void)
{
    uint32_t result; __ASM volatile( ADD_ARCH_ZICSR "csrr %0," "mstatus": "=r"(result) );
    return (result & 0x08) != 0u;
}

// Get stack pointer (returns the stack pointer)
RV_STATIC_INLINE uint32_t __get_cpu_sp(void)
{
	uint32_t result; __ASM volatile( ADD_ARCH_ZICSR "mv %0, sp" : "=r"(result));
	return result;
}

// nop
RV_STATIC_INLINE void __NOP()
{
	__ASM volatile( "nop" );
}

// Enable Interrupt (by interrupt number)
RV_STATIC_INLINE void NVIC_EnableIRQ(IRQn_Type IRQn)
{
	NVIC->IENR[((uint32_t)(IRQn) >> 5)] = (1 << ((uint32_t)(IRQn) & 0x1F));
}

// Disable Interrupt (by interrupt number)
RV_STATIC_INLINE void NVIC_DisableIRQ(IRQn_Type IRQn)
{
	NVIC->IRER[((uint32_t)(IRQn) >> 5)] = (1 << ((uint32_t)(IRQn) & 0x1F));
}

// Get Interrupt Enable State, (by number), 1 = Triggered 0 = Not triggered
RV_STATIC_INLINE uint32_t NVIC_GetStatusIRQ(IRQn_Type IRQn)
{
	return((uint32_t) ((NVIC->ISR[(uint32_t)(IRQn) >> 5] & (1 << ((uint32_t)(IRQn) & 0x1F)))?1:0));
}

// Get Interrupt Pending State, (by number), 1 = Pending 0 = Not pending
RV_STATIC_INLINE uint32_t NVIC_GetPendingIRQ(IRQn_Type IRQn)
{
	return((uint32_t) ((NVIC->IPR[(uint32_t)(IRQn) >> 5] & (1 << ((uint32_t)(IRQn) & 0x1F)))?1:0));
}

// "current number break hang"
RV_STATIC_INLINE void NVIC_SetPendingIRQ(IRQn_Type IRQn)
{
	NVIC->IPSR[((uint32_t)(IRQn) >> 5)] = (1 << ((uint32_t)(IRQn) & 0x1F));
}

// Clear Interrupt Pending
RV_STATIC_INLINE void NVIC_ClearPendingIRQ(IRQn_Type IRQn)
{
	NVIC->IPRR[((uint32_t)(IRQn) >> 5)] = (1 << ((uint32_t)(IRQn) & 0x1F));
}

// Get Interrupt Active State (returns 1 if active)
RV_STATIC_INLINE uint32_t NVIC_GetActive(IRQn_Type IRQn)
{
	return((uint32_t)((NVIC->IACTR[(uint32_t)(IRQn) >> 5] & (1 << ((uint32_t)(IRQn) & 0x1F)))?1:0));
}

// Set Interrupt Priority (priority: bit7: pre-emption priority, bit6: subpriority, bit[5-0]: reserved
RV_STATIC_INLINE void NVIC_SetPriority(IRQn_Type IRQn, uint8_t priority)
{
	NVIC->IPRIOR[(uint32_t)(IRQn)] = priority;
}

/*********************************************************************
 * SUSPEND ALL INTERRUPTS EXCEPT
 * The following 3 functions serve to suspend all interrupts, except for the one you momentarily need.
 * The purpose of this is to not disturb the one interrupt of interest and let it run unimpeded.
 * procedure:
 * 1. save the enabled IRQs: uint32_t IRQ_backup = NVIC_get_enabled_IRQs();
 * 2. disable all IRQs: NVIC_clear_all_IRQs_except(IRQ_of_interest);
 * 3. restore the previously enabled IRQs: NVIC_restore_IRQs(IRQ_backup);
 * 
 * bit layout of the IRQ backup
 * bit		0 | 1 | 2  |  3  | 4  |  5  | 6  .. 22 | 23 .. 28
 * IRQn		2 | 3 | 12 | res | 14 | res | 16 .. 31 | 32 .. 38
 * IRQn 2 and 3 aren't actually user-settable (see RM).
 * 
 * Specifying an invalid IRQn_to_keep like 0 will disable all interrupts.
 */

RV_STATIC_INLINE uint32_t NVIC_get_enabled_IRQs()
{
	return ( ((NVIC->ISR[0] >> 2) & 0b11) | ((NVIC->ISR[0] >> 12) << 2) | ((NVIC->ISR[1] & 0b1111111) << 23) );
}

RV_STATIC_INLINE void NVIC_clear_all_IRQs_except(uint8_t IRQn_to_keep)
{
	if (!(IRQn_to_keep >> 5)) {		// IRQn_to_keep < 32
		NVIC->IRER[0] = (~0) & (~(1 << IRQn_to_keep));
		NVIC->IRER[1] = (~0);
	}
	else {
		IRQn_to_keep = IRQn_to_keep >> 5;
		NVIC->IRER[0] = (~0);
		NVIC->IRER[1] = (~0) & (~(1 << IRQn_to_keep));
	}
}

RV_STATIC_INLINE void NVIC_restore_IRQs(uint32_t old_state)
{
	NVIC->IENR[0] = (old_state >> 2) << 12;
	NVIC->IENR[1] = old_state >> 23;
}

// WFI - wait for interrupt (like a light sleep)
__attribute__( ( always_inline ) ) RV_STATIC_INLINE void __WFI(void)
{
	NVIC->SCTLR &= ~(1<<3);   // wfi
	__ASM volatile ("wfi");
}

// WFE - wait for events (more like a deeper sleep)
__attribute__( ( always_inline ) ) RV_STATIC_INLINE void __WFE(void)
{
  uint32_t t;

  t = NVIC->SCTLR;
  NVIC->SCTLR |= (1<<3)|(1<<5);     // (wfi->wfe)+(__sev)
  NVIC->SCTLR = (NVIC->SCTLR & ~(1<<5)) | ( t & (1<<5));
  __ASM volatile ("wfi");
  __ASM volatile ("wfi");
}

/*********************************************************************
 * @fn      SetVTFIRQ
 * @brief   Set VTF Interrupt
 * @param   addr - VTF interrupt service function base address.
 *                  IRQn - Interrupt Numbers
 *                  num - VTF Interrupt Numbers
 *                  NewState -  DISABLE or ENABLE
 *
 * @return  none
 */
RV_STATIC_INLINE void SetVTFIRQ(uint32_t addr, IRQn_Type IRQn, uint8_t num, FunctionalState NewState){
	if(num > 1)  return ;

	if (NewState != DISABLE)
	{
		NVIC->VTFIDR[num] = IRQn;
		NVIC->VTFADDR[num] = ((addr&0xFFFFFFFE)|0x1);
	}
	else{
		NVIC->VTFIDR[num] = IRQn;
		NVIC->VTFADDR[num] = ((addr&0xFFFFFFFE)&(~0x1));
	}
}

// Initiate a system reset request
RV_STATIC_INLINE void NVIC_SystemReset(void)
{
	NVIC->CFGR = NVIC_KEY3|(1<<7);
}

// For reading INTSYSCR, for interrupt nesting + hardware stack enable.
static inline uint32_t __get_INTSYSCR(void)
{
	uint32_t result;
	__ASM volatile(	ADD_ARCH_ZICSR "csrr %0, 0x804": "=r"(result));
	return result;
}

// For setting INTSYSCR, for interrupt nesting + hardware stack enable.
static inline void __set_INTSYSCR( uint32_t value )
{
	__ASM volatile( ADD_ARCH_ZICSR "csrw 0x804, %0" : : "r"(value));
}

#if defined(CH32V30x)

// Return the Floating-Point Accrued Exceptions
static inline uint32_t __get_FFLAGS(void)
{
	uint32_t result;
	__ASM volatile( ADD_ARCH_ZICSR "csrr %0," "fflags" : "=r" (result) );
	return (result);
}

// Set the Floating-Point Accrued Exceptions
static inline void __set_FFLAGS(uint32_t value)
{
	__ASM volatile( ADD_ARCH_ZICSR "csrw fflags, %0" : : "r" (value) );
}

// Return the Floating-Point Dynamic Rounding Mode
static inline uint32_t __get_FRM(void)
{
	uint32_t result;
	__ASM volatile( ADD_ARCH_ZICSR "csrr %0," "frm" : "=r" (result) );
	return (result);
}

// Set the Floating-Point Dynamic Rounding Mode
static inline void __set_FRM(uint32_t value)
{
	__ASM volatile( ADD_ARCH_ZICSR "csrw frm, %0" : : "r" (value) );
}

// Return the Floating-Point Control and Status Register
static inline uint32_t __get_FCSR(void)
{
	uint32_t result;
	__ASM volatile( ADD_ARCH_ZICSR "csrr %0," "fcsr" : "=r" (result) );
	return (result);
}

// Set the Floating-Point Dynamic Rounding Mode
static inline void __set_FCSR(uint32_t value)
{
	__ASM volatile( ADD_ARCH_ZICSR "csrw fcsr, %0" : : "r" (value) );
}

#endif // CH32V30x

// Return the Machine Status Register (MSTATUS)
static inline uint32_t __get_MSTATUS(void)
{
	uint32_t result;
	__ASM volatile( ADD_ARCH_ZICSR "csrr %0, mstatus": "=r"(result) );
	return (result);
}

// Set the Machine Status Register (MSTATUS)
static inline void __set_MSTATUS(uint32_t value)
{
	__ASM volatile( ADD_ARCH_ZICSR "csrw mstatus, %0" : : "r"(value) );
}

// Return the Machine ISA Register (MISA)
static inline uint32_t __get_MISA(void)
{
	uint32_t result;
	__ASM volatile( ADD_ARCH_ZICSR "csrr %0, misa" : "=r"(result));
	return (result);
}

// Set the Machine ISA Register (MISA)
static inline void __set_MISA(uint32_t value)
{
	__ASM volatile( ADD_ARCH_ZICSR "csrw misa, %0" : : "r"(value));
}

// Return the Machine Trap-Vector Base-Address Register (MTVEC)
static inline uint32_t __get_MTVEC(void)
{
	uint32_t result;
	__ASM volatile( ADD_ARCH_ZICSR "csrr %0," "mtvec": "=r"(result));
	return (result);
}

//  * @brief   Set the Machine Trap-Vector Base-Address Register (MTVEC)
static inline void __set_MTVEC(uint32_t value)
{
	__ASM volatile( ADD_ARCH_ZICSR "csrw mtvec, %0":: "r"(value));
}

// Return the Machine Seratch Register (MSCRATCH)
static inline uint32_t __get_MSCRATCH(void)
{
	uint32_t result;
	__ASM volatile( ADD_ARCH_ZICSR "csrr %0," "mscratch" : "=r"(result));
	return (result);
}

// Set the Machine Seratch Register (MSRATCH)
static inline void __set_MSCRATCH(uint32_t value)
{
	__ASM volatile( ADD_ARCH_ZICSR "csrw mscratch, %0" : : "r"(value));
}

// Return the Machine Exception Program Register (MEPC)
static inline uint32_t __get_MEPC(void)
{
	uint32_t result;
	__ASM volatile( ADD_ARCH_ZICSR "csrr %0," "mepc" : "=r"(result));
	return (result);
}

// Set the Machine Exception Program Register (MEPC)
static inline void __set_MEPC(uint32_t value)
{
	__ASM volatile( ADD_ARCH_ZICSR "csrw mepc, %0" : : "r"(value));
}

// Return the Machine Cause Register (MCAUSE)
static inline uint32_t __get_MCAUSE(void)
{
	uint32_t result;
	__ASM volatile( ADD_ARCH_ZICSR "csrr %0," "mcause": "=r"(result));
	return (result);
}

// Set the Machine Cause Register (MCAUSE)
static inline void __set_MCAUSE(uint32_t value)
{
	__ASM volatile( ADD_ARCH_ZICSR "csrw mcause, %0":: "r"(value));
}

// Return the Machine Trap Value Register (MTVAL)
static inline uint32_t __get_MTVAL(void)
{
	uint32_t result;
	__ASM volatile( ADD_ARCH_ZICSR "csrr %0," "mtval" : "=r" (result) );
	return (result);
}

// Set the Machine Trap Value Register (MTVAL)
static inline void __set_MTVAL(uint32_t value)
{
	__ASM volatile ( ADD_ARCH_ZICSR "csrw mtval, %0" : : "r" (value) );
}

// Return Vendor ID Register (MVENDORID)
static inline uint32_t __get_MVENDORID(void)
{
	uint32_t result;
	__ASM volatile( ADD_ARCH_ZICSR "csrr %0, mvendorid": "=r"(result));
	return (result);
}

// Return Machine Architecture ID Register (MARCHID)
static inline uint32_t __get_MARCHID(void)
{
	uint32_t result;
	__ASM volatile( ADD_ARCH_ZICSR "csrr %0, marchid": "=r"(result));
	return (result);
}

// Return Machine Implementation ID Register (MIPID)
static inline uint32_t __get_MIMPID(void)
{
	uint32_t result;
	__ASM volatile( ADD_ARCH_ZICSR "csrr %0, mimpid": "=r"(result));
	return (result);
}

// Return Hart ID Register MHARTID
static inline uint32_t __get_MHARTID(void)
{
	uint32_t result;
	__ASM volatile( ADD_ARCH_ZICSR "csrr %0, mhartid": "=r"(result));
	return (result);
}

#if defined(CH32V003) && CH32V003

// Return DBGMCU_CR Register value
static inline uint32_t __get_DEBUG_CR(void)
{
	uint32_t result;
	__ASM volatile( ADD_ARCH_ZICSR "csrr %0, 0x7C0" : "=r" (result) );
	return (result);
}


// Set the DBGMCU_CR Register value
static inline void __set_DEBUG_CR(uint32_t value)
{
	__ASM volatile( ADD_ARCH_ZICSR "csrw 0x7C0, %0" : : "r" (value) );
}

// Return stack pointer register (SP)
static inline uint32_t __get_SP(void)
{
	uint32_t result;
	__ASM volatile( "mv %0,""sp": "=r"(result):);
	return (result);
}
#endif // CH32V003

#endif // !assembler

// _JBTYPE using long long to make sure the alignment is align to 8 byte,
// otherwise in rv32imafd, store/restore FPR may mis-align.
#define _JBTYPE long long
#if defined( __riscv_abi_rve )
#define _JBLEN ((4*sizeof(long))/sizeof(long))
#elif defined( __riscv_float_abi_double )
#define _JBLEN ((14*sizeof(long) + 12*sizeof(double))/sizeof(long))
#elif defined( __riscv_float_abi_single )
#define _JBLEN ((14*sizeof(long) + 12*sizeof(float))/sizeof(long))
#else
#define _JBLEN ((14*sizeof(long))/sizeof(long))
#endif

#ifndef __ASSEMBLER__
#ifdef _JBLEN
#ifdef _JBTYPE
typedef _JBTYPE jmp_buf[_JBLEN];
#else
typedef int jmp_buf[_JBLEN];
#endif // _JBTYPE
#endif // _JBLEN

int setjmp( jmp_buf env );
void longjmp( jmp_buf env, int val );
#endif

#endif // defined(__riscv) || defined(__riscv__) || defined( CH32V003FUN_BASE )

#ifdef __cplusplus
}
#endif

#endif/* __CORE_RISCV_H__ */


///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
extern "C" {
#endif

/* SYSTICK info
 * time on the ch32v003 is kept by the SysTick counter (32bit)
 * by default, it will operate at (FUNCONF_SYSTEM_CORE_CLOCK / 8) = 6MHz
 * more info at https://github.com/cnlohr/ch32v003fun/wiki/Time
*/

#if defined( FUNCONF_SYSTICK_USE_HCLK ) && FUNCONF_SYSTICK_USE_HCLK && !defined(CH32V10x)
#define DELAY_US_TIME ((FUNCONF_SYSTEM_CORE_CLOCK)/1000000)
#define DELAY_MS_TIME ((FUNCONF_SYSTEM_CORE_CLOCK)/1000)
#else // Use systick = hclk/8
#define DELAY_US_TIME ((FUNCONF_SYSTEM_CORE_CLOCK)/8000000)
#define DELAY_MS_TIME ((FUNCONF_SYSTEM_CORE_CLOCK)/8000)
#endif

#define Delay_Us(n) DelaySysTick( (n) * DELAY_US_TIME )
#define Delay_Ms(n) DelaySysTick( (n) * DELAY_MS_TIME )

#define Ticks_from_Us(n)	(n * DELAY_US_TIME)
#define Ticks_from_Ms(n)	(n * DELAY_MS_TIME)

// Add a certain number of nops.  Note: These are usually executed in pairs
// and take two cycles, so you typically would use 0, 2, 4, etc.
#define ADD_N_NOPS( n ) asm volatile( ".rept " #n "\nc.nop\n.endr" );

// Arduino-like GPIO Functionality
#define GpioOf( pin ) ((GPIO_TypeDef *)(GPIOA_BASE + 0x400 * ((pin)>>4)))

#define FUN_HIGH 0x1
#define FUN_LOW 0x0
#define FUN_OUTPUT (GPIO_Speed_10MHz | GPIO_CNF_OUT_PP)
#define FUN_INPUT (GPIO_CNF_IN_FLOATING)

// For pins, use things like PA8, PB15
// For configuration, use things like GPIO_CFGLR_OUT_10Mhz_PP

#define funDigitalWrite( pin, value ) { GpioOf( pin )->BSHR = 1<<((!(value))*16 + ((pin) & 0xf)); }

#if defined(CH32X03x)
#define funGpioInitAll() { RCC->APB2PCENR |= ( RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC ); }
#define funPinMode( pin, mode ) { *((&GpioOf(pin)->CFGLR)+((pin&0x8)>>3)) = ( (*((&GpioOf(pin)->CFGLR)+((pin&0x8)>>3))) & (~(0xf<<(4*((pin)&0x7))))) | ((mode)<<(4*((pin)&0x7))); }
#elif defined(CH32V10x) || defined(CH32V20x) || defined(CH32V30x)
#define funGpioInitAll() { RCC->APB2PCENR |= ( RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD ); }
#define funPinMode( pin, mode ) { *((&GpioOf(pin)->CFGLR)+((pin&0x8)>>3)) = ( (*((&GpioOf(pin)->CFGLR)+((pin&0x8)>>3))) & (~(0xf<<(4*((pin)&0x7))))) | ((mode)<<(4*((pin)&0x7))); }
#define funGpioInitB() { RCC->APB2PCENR |= ( RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOB ); }
#else
#define funGpioInitAll() { RCC->APB2PCENR |= ( RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD ); }
#define funPinMode( pin, mode ) { GpioOf(pin)->CFGLR = (GpioOf(pin)->CFGLR & (~(0xf<<(4*((pin)&0xf))))) | ((mode)<<(4*((pin)&0xf))); }
#endif

#define funGpioInitA() { RCC->APB2PCENR |= ( RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOA ); }
#define funGpioInitC() { RCC->APB2PCENR |= ( RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOC ); }
#define funGpioInitD() { RCC->APB2PCENR |= ( RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOD ); }
#define funDigitalRead( pin ) ((int)((GpioOf(pin)->INDR >> ((pin)&0xf)) & 1))


#define ANALOG_0 0
#define ANALOG_1 1
#define ANALOG_2 2
#define ANALOG_3 3
#define ANALOG_4 4
#define ANALOG_5 5
#define ANALOG_6 6
#define ANALOG_7 7
#define ANALOG_8 8
#define ANALOG_9 9
#define ANALOG_10 10
#define ANALOG_11 11

#ifndef __ASSEMBLER__

#if defined(__riscv) || defined(__riscv__) || defined( CH32V003FUN_BASE )


// Stuff that can only be compiled on device (not for the programmer, or other host programs)

// Initialize the ADC calibrate it and set some sane defaults.
void funAnalogInit( void );

// Read an analog input (not a GPIO pin number)
// Be sure to call funAnalogInit first.
int funAnalogRead( int nAnalogNumber );

void handle_reset()            __attribute__((naked)) __attribute((section(".text.handle_reset"))) __attribute__((used));
void DefaultIRQHandler( void ) __attribute__((section(".text.vector_handler"))) __attribute__((naked)) __attribute__((used));
// used to clear the CSS flag in case of clock fail switch
#if defined(FUNCONF_USE_CLK_SEC) && FUNCONF_USE_CLK_SEC
	void NMI_RCC_CSS_IRQHandler( void ) __attribute__((section(".text.vector_handler"))) __attribute__((naked)) __attribute__((used));
#endif

void DelaySysTick( uint32_t n );


// Depending on a LOT of factors, it's about 6 cycles per n.
// **DO NOT send it zero or less.**
#ifndef __MACOSX__
#ifndef __DELAY_TINY_DEFINED__
#define __DELAY_TINY_DEFINED__
static inline void Delay_Tiny( int n ) {
	__ASM volatile( "\
		mv a5, %[n]\n\
		1: \
		c.addi a5, -1\n\
		c.bnez a5, 1b" : : [n]"r"(n) : "a5" );
}
#endif
#endif
#endif //defined(__riscv) || defined(__riscv__) || defined( CH32V003FUN_BASE )

// Tricky: We need to make sure main and SystemInit() are preserved.
int main() __attribute__((used));
void SystemInit(void);

#ifdef FUNCONF_UART_PRINTF_BAUD
	#define UART_BAUD_RATE FUNCONF_UART_PRINTF_BAUD
#else
	#define UART_BAUD_RATE 115200
#endif
// Debug UART baud rate register calculation. Works assuming HCLK prescaler is off.
// Computes UART_BRR = CORE_CLOCK / BAUD_RATE with rounding to closest integer
#define UART_BRR (((FUNCONF_SYSTEM_CORE_CLOCK) + (UART_BAUD_RATE)/2) / (UART_BAUD_RATE))
// Put an output debug UART on Pin D5.
// You can write to this with printf(...) or puts(...)

void SetupUART( int uartBRR );

// Returns 1 if timeout reached, 0 otherwise.
// If timeout_ms == 0, wait indefinitely.
// Use DidDebuggerAttach() For a zero-wait way of seeing if it attached.
int WaitForDebuggerToAttach( int timeout_ms );

// Returns 1 if a debugger has activated the debug module.
#define DidDebuggerAttach() (!*DMSTATUS_SENTINEL)

// Returns 1 if a debugger has activated the debug module.
#define DebugPrintfBufferFree() (!(*DMDATA0 & 0x80))

// Just a definition to the internal _write function.
int _write(int fd, const char *buf, int size);

// Print a hexadecimal value to the debug (or UART) depending on configuration.
void PrintHex( uint32_t n );

// Call this to busy-wait the polling of input.
void poll_input( void );

// Receiving bytes from host.  Override if you wish.
void handle_debug_input( int numbytes, uint8_t * data );


// Functions from ch32fun.c
#include <stdarg.h>

int mini_vsnprintf( char *buffer, unsigned int buffer_len, const char *fmt, va_list va );
int mini_vpprintf( int (*puts)(char* s, int len, void* buf), void* buf, const char *fmt, va_list va );
int mini_snprintf(char* buffer, unsigned int buffer_len, const char *fmt, ...);
int mini_pprintf(int (*puts)(char*s, int len, void* buf), void* buf, const char *fmt, ...);
int mini_itoa(long value, unsigned int radix, int uppercase, int unsig,	char *buffer);

#endif // __ASSEMBLER__


/*
 * This file contains various parts of the official WCH EVT Headers which
 * were originally under a restrictive license.
 * 
 * The collection of this file was generated by 
 * cnlohr, 2023-02-18 and
 * AlexanderMandera, 2023-06-23
 * It was significantly reworked into several files cnlohr, 2025-01-29
 *
 * While originally under a restrictive copyright, WCH has approved use
 * under MIT-licensed use, because of inclusion in Zephyr, as well as other
 * open-source licensed projects.
 *
 * These copies of the headers from WCH are available now under:
 *
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the “Software”), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#ifdef __cplusplus
};
#endif


#endif // __CH32FUN_H


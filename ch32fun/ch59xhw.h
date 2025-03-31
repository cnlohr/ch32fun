#ifndef TODO_HARDWARE_H
#define TODO_HARDWARE_H

#include "ch32fun.h"

#ifndef __ASSEMBLER__  // Things before this can be used in assembly.

#ifdef __cplusplus
extern "C" {
#endif

/* Interrupt Number Definition, according to the selected device */
typedef enum IRQn
{
	/******  RISC-V Processor Exceptions Numbers *******************************************************/
	NonMaskableInt_IRQn = 2, /* 2 Non Maskable Interrupt                             */
	EXC_IRQn = 3,            /* 3 Exception Interrupt                                */
	Ecall_M_Mode_IRQn = 5,   /* 5 Ecall M Mode Interrupt                             */
	Ecall_U_Mode_IRQn = 8,   /* 8 Ecall U Mode Interrupt                             */
	Break_Point_IRQn = 9,    /* 9 Break Point Interrupt                              */
	SysTicK_IRQn = 12,       /* 12 System timer Interrupt                            */
	Software_IRQn = 14,      /* 14 software Interrupt                                */

	/******  RISC-V specific Interrupt Numbers *********************************************************/
	TMR0_IRQn = 16,          /* 0:  TMR0 */
	GPIOA_IRQn = 17,         /* GPIOA */
	GPIOB_IRQn = 18,         /* GPIOB */
	SPI0_IRQn = 19,          /* SPI0 */
	BB_IRQn = 20,            /* BLEB */
	LLE_IRQn = 21,           /* BLEL */
	USB_IRQn = 22,           /* USB */
	TMR1_IRQn = 24,          /* TMR1 */
	TMR2_IRQn = 25,          /* TMR2 */
	UART0_IRQn = 26,         /* UART0 */
	UART1_IRQn = 27,         /* UART1 */
	RTC_IRQn = 28,           /* RTC */
	ADC_IRQn = 29,           /* ADC */
	I2C_IRQn = 30,           /* I2C */
	PWMX_IRQn = 31,          /* PWMX */
	TMR3_IRQn = 32,          /* TMR3 */
	UART2_IRQn = 33,         /* UART2 */
	UART3_IRQn = 34,         /* UART3 */
	WDOG_BAT_IRQn = 35,      /* WDOG_BAT */
} IRQn_Type;

#define BASE_VECTOR "\n\
	.balign  2\n\
	.option   push;\n\
	.option   norvc;\n\
	j handle_reset\n\
	.word   0\n\
	.word   NMI_Handler                 /* NMI Handler */\n\
	.word   HardFault_Handler           /* Hard Fault Handler */\n\
	.word   0xF5F9BDA9\n\
	.word   Ecall_M_Mode_Handler        /* 5 */\n\
	.word   0\n\
	.word   0\n\
	.word   Ecall_U_Mode_Handler		/* 8 */\n\
	.word   Break_Point_Handler			/* 9 */\n\
	.word   0\n\
	.word   0\n\
	.word   SysTick_Handler            /* SysTick Handler */\n\
	.word   0\n\
	.word   SW_Handler                 /* SW Handler */\n\
	.word   0\n\
	/* External Interrupts */\n\
	.word   TMR0_IRQHandler            /* 0:  TMR0 */\n\
	.word   GPIOA_IRQHandler           /* GPIOA */\n\
	.word   GPIOB_IRQHandler           /* GPIOB */\n\
	.word   SPI0_IRQHandler            /* SPI0 */\n\
	.word   BB_IRQHandler              /* BLEB */\n\
	.word   LLE_IRQHandler             /* BLEL */\n\
	.word   USB_IRQHandler             /* USB */\n\
	.word   0\n\
	.word   TMR1_IRQHandler            /* TMR1 */\n\
	.word   TMR2_IRQHandler            /* TMR2 */\n\
	.word   UART0_IRQHandler           /* UART0 */\n\
	.word   UART1_IRQHandler           /* UART1 */\n\
	.word   RTC_IRQHandler             /* RTC */\n\
	.word   ADC_IRQHandler             /* ADC */\n\
	.word   I2C_IRQHandler             /* I2C */\n\
	.word   PWMX_IRQHandler            /* PWMX */\n\
	.word   TMR3_IRQHandler            /* TMR3 */\n\
	.word   UART2_IRQHandler           /* UART2 */\n\
	.word   UART3_IRQHandler           /* UART3 */\n\
	.word   WDOG_BAT_IRQHandler        /* WDOG_BAT */\n"

#define DEFAULT_INTERRUPT_VECTOR_CONTENTS BASE_VECTOR "\n.option pop;\n"

#define __HIGH_CODE __attribute__((section(".highcode"), used))
#define __INTERRUPT __attribute__((interrupt))

/* memory mapped structure for SysTick */
typedef struct
{
	__IO uint32_t CTLR;
	__IO uint32_t SR;
	__IO uint64_t CNT;
	__IO uint64_t CMP;
} SysTick_Type;


/* memory mapped structure for Program Fast Interrupt Controller (PFIC) */
typedef struct
{
	__I uint32_t  ISR[8];           // 0
	__I uint32_t  IPR[8];           // 20H
	__IO uint32_t ITHRESDR;         // 40H
	uint8_t       RESERVED[4];      // 44H
	__O uint32_t  CFGR;             // 48H
	__I uint32_t  GISR;             // 4CH
	__IO uint8_t  VTFIDR[4];        // 50H
	uint8_t       RESERVED0[0x0C];  // 54H
	__IO uint32_t VTFADDR[4];       // 60H
	uint8_t       RESERVED1[0x90];  // 70H
	__O uint32_t  IENR[8];          // 100H
	uint8_t       RESERVED2[0x60];  // 120H
	__O uint32_t  IRER[8];          // 180H
	uint8_t       RESERVED3[0x60];  // 1A0H
	__O uint32_t  IPSR[8];          // 200H
	uint8_t       RESERVED4[0x60];  // 220H
	__O uint32_t  IPRR[8];          // 280H
	uint8_t       RESERVED5[0x60];  // 2A0H
	__IO uint32_t IACTR[8];         // 300H
	uint8_t       RESERVED6[0xE0];  // 320H
	__IO uint8_t  IPRIOR[256];      // 400H
	uint8_t       RESERVED7[0x810]; // 500H
	__IO uint32_t SCTLR;            // D10H
} PFIC_Type;
#endif /* __ASSEMBLER__*/

#ifdef __ASSEMBLER__
#define CORE_PERIPH_BASE           (0xE0000000) /* System peripherals base address in the alias region */
#else
#define CORE_PERIPH_BASE           ((uint32_t)(0xE0000000))
#endif /* __ASSEMBLER__*/

#define PFIC_BASE                  (CORE_PERIPH_BASE + 0xE000)
#define SysTick_BASE               (CORE_PERIPH_BASE + 0xF000)

#define PFIC                       ((PFIC_Type *) PFIC_BASE)
#define NVIC                       PFIC
#define NVIC_KEY1                  ((uint32_t)0xFA050000)
#define	NVIC_KEY2	               ((uint32_t)0xBCAF0000)
#define	NVIC_KEY3		           ((uint32_t)0xBEEF0000)

#define SysTick                    ((SysTick_Type *) SysTick_BASE)
#define SYSTICK_LOAD_RELOAD_MSK    (0xFFFFFFFFFFFFFFFF)
#define SYSTICK_CTLR_SWIE          (1 << 31)
#define SYSTICK_CTLR_INIT          (1 << 5)
#define SYSTICK_CTLR_MODE          (1 << 4)
#define SYSTICK_CTLR_STRE          (1 << 3)
#define SYSTICK_CTLR_STCLK         (1 << 2)
#define SYSTICK_CTLR_STIE          (1 << 1)
#define SYSTICK_CTLR_STE           (1 << 0)
#define SYSTICK_SR_CNTIF           (1 << 0)

typedef enum
{
	CLK_SOURCE_LSI = 0x00,
	CLK_SOURCE_LSE,

	CLK_SOURCE_HSE_16MHz =  (0x20 |  2),
	CLK_SOURCE_HSE_8MHz =   (0x20 |  4),
	CLK_SOURCE_HSE_6_4MHz = (0x20 |  5),
	CLK_SOURCE_HSE_4MHz =   (0x20 |  8),

	CLK_SOURCE_PLL_60MHz =  (0x40 |  8),
	CLK_SOURCE_PLL_48MHz =  (0x40 | 10),
	CLK_SOURCE_PLL_32MHz =  (0x40 | 15),
	CLK_SOURCE_PLL_24MHz =  (0x40 | 20),
} SYS_CLKTypeDef;

typedef enum
{
	LSE_RCur_70 = 0,
	LSE_RCur_100,
	LSE_RCur_140,
	LSE_RCur_200
} LSECurrentTypeDef;

typedef enum
{
	HSE_RCur_75 = 0,
	HSE_RCur_100,
	HSE_RCur_125,
	HSE_RCur_150
} HSECurrentTypeDef;

// For debug writing to the debug interface.
#define DMDATA0 			((vu32*)0xe0000380)
#define DMDATA1 			((vu32*)0xe0000384)
#define DMSTATUS_SENTINEL	((vu32*)0xe0000388)// Reads as 0x00000000 if debugger is attached.

/* System: clock configuration register */
#define R32_CLK_SYS_CFG     (*((vu32*)0x40001008)) // RWA, system clock configuration, SAM
#define  RB_CLK_PLL_DIV     0x1F                      // RWA, output clock divider from PLL or CK32M
#define  RB_CLK_SYS_MOD     0xC0                      // RWA, system clock source mode: 00=divided from 32MHz, 01=divided from PLL-480MHz, 10=directly from 32MHz, 11=directly from 32KHz
#define  RB_TX_32M_PWR_EN   0x40000                   // RWA, extern 32MHz HSE power contorl
#define  RB_XT_FORCE_EN     0x80000                   // RWA, system clock control in Halt mode
#define  RB_PLL_PWR_EN      0x100000                  // RWA, PLL power control

/* System: sleep control register */
#define R32_SLEEP_CONTROL   (*((vu32*)0x4000100C)) // RWA, sleep control, SAM
#define R8_SLP_CLK_OFF0     (*((vu8*)0x4000100C))  // RWA, sleep clock off control byte 0, SAM
#define  RB_SLP_CLK_TMR0    0x01                      // RWA, close TMR0 clock
#define  RB_SLP_CLK_TMR1    0x02                      // RWA, close TMR1 clock
#define  RB_SLP_CLK_TMR2    0x04                      // RWA, close TMR2 clock
#define  RB_SLP_CLK_TMR3    0x08                      // RWA, close TMR3 clock
#define  RB_SLP_CLK_UART0   0x10                      // RWA, close UART0 clock
#define  RB_SLP_CLK_UART1   0x20                      // RWA, close UART1 clock
#define  RB_SLP_CLK_UART2   0x40                      // RWA, close UART2 clock
#define  RB_SLP_CLK_UART3   0x80                      // RWA, close UART3 clock
#define R8_SLP_CLK_OFF1     (*((vu8*)0x4000100D))  // RWA, sleep clock off control byte 1, SAM
#define  RB_SLP_CLK_SPI0    0x01                      // RWA, close SPI0 clock
#define  RB_SLP_CLK_PWMX    0x04                      // RWA, close PWMx clock
#define  RB_SLP_CLK_I2C     0x08                      // RWA, close I2C clock
#define  RB_SLP_CLK_USB     0x10                      // RWA, close USB clock
#define  RB_SLP_CLK_LCD     0x40                      // RWA, close LCD clock
#define  RB_SLP_CLK_BLE     0x80                      // RWA, close BLE clock
#define R8_SLP_WAKE_CTRL    (*((vu8*)0x4000100E))  // RWA, wake control, SAM
#define  RB_SLP_USB_WAKE    0x01                      // RWA, enable USB waking
//#define  RB_SLP_BLE_WAKE    0x04                      // RWA, enable BLE waking
#define  RB_SLP_RTC_WAKE    0x08                      // RWA, enable RTC waking
#define  RB_SLP_GPIO_WAKE   0x10                      // RWA, enable GPIO waking
#define  RB_SLP_BAT_WAKE    0x20                      // RWA, enable BAT waking
#define  RB_WAKE_EV_MODE    0x40                      // RWA, event wakeup mode: 0=event keep valid for long time, 1=short pulse event
#define  RB_GPIO_WAKE_MODE  0x80                      // RWA,
#define R8_SLP_POWER_CTRL   (*((vu8*)0x4000100F))  // RWA, peripherals power down control, SAM
#define  RB_WAKE_DLY_MOD    0x03                      // RWA, wakeup delay time selection
// RB_WAKE_DLY_MOD select wakeup delay
//   00: long time, 3590 cycles+TSUHSE
//   01: short time, 520 cycles+TSUHSE
//   10: shorter time, 70 cycles+TSUHSE
//   11: no delay, 8 cycles+TSUHSE
//#define  RB_SLP_USB_PWR_DN  0x01                      // RWA, enable USB power down
//#define  RB_SLP_BLE_PWR_DN  0x04                      // RWA, enable BLE power down
#define  RB_SLP_CLK_RAMX    0x10                      // RWA, close main SRAM clock
#define  RB_SLP_CLK_RAM2K   0x20                      // RWA, close retention 2KB SRAM clock
#define  RB_RAM_RET_LV      0x40                      // RWA, SRAM retention voltage selection: 0=normal, 1=low voltage for low power
#define R32_SLEEP_CTRL2     (*((vu32*)0x40001010)) // RWA, touchkey wake up enable
#define  RB_TKEY0_5_WAKE_EN    0x003F                 // RWA, touchkey wake up enable channel 0-5
#define  RB_TKEY8_13_WAKE_EN   0x3F00                 // RWA, touchkey wake up enable channel 8-13

/* System: I/O pin configuration register */
#define R32_PIN_CONFIG      (*((vu32*)0x40001018)) // RW, I/O pin configuration
#define R16_PIN_ALTERNATE   (*((vu16*)0x40001018)) // RW, function pin alternate configuration
#define  RB_PIN_TMR0        0x01                      // RW, TMR0 alternate pin enable: 0=TMR0/PWM0/CAP0 on PA[9], 1=TMR0_/PWM0_/CAP0_ on PB[23]
#define  RB_PIN_TMR1        0x02                      // RW, TMR1 alternate pin enable: 0=TMR1/PWM1/CAP1 on PA[10], 1=TMR1_/PWM1_/CAP1_ on PB[10]
#define  RB_PIN_TMR2        0x04                      // RW, TMR2 alternate pin enable: 0=TMR2/PWM2/CAP2 on PA[11], 1=TMR2_/PWM2_/CAP2_ on PB[11]
#define  RB_PIN_TMR3        0x08                      // RW, TMR3 alternate pin enable: 0=TMR3/PWM3/CAP3 on PA[2], 1=TMR3_/PWM3_/CAP3_ on PB[22]
#define  RB_PIN_UART0       0x10                      // RW, RXD0/TXD0 alternate pin enable: 0=RXD0/TXD0 on PB[4]/PB[7], 1=RXD0_/TXD0_ on PA[15]/PA[14]
#define  RB_PIN_UART1       0x20                      // RW, RXD1/TXD1 alternate pin enable: 0=RXD1/TXD1 on PA[8]/PA[9], 1=RXD1_/TXD1_ on PB[12]/PB[13]
#define  RB_PIN_UART2       0x40                      // RW, RXD2/TXD2 alternate pin enable: 0=RXD2/TXD2 on PA[6]/PA[7], 1=RXD2_/TXD2_ on PB[22]/PB[23]
#define  RB_PIN_UART3       0x80                      // RW, RXD3/TXD3 alternate pin enable: 0=RXD3/TXD3 on PA[4]/PA[5], 1=RXD3_/TXD3_ on PB[20]/PB[21]
#define  RB_PIN_SPI0        0x100                     // RW, SCS/SCK0/MOSI/MISO alternate pin enable: 0=SCS/SCK0/MOSI/MISO on PA[12]/PA[13]/PA[14]/PA[15], 1=SCS_/SCK0_/MOSI_/MISO_ on PB[12]/PB[13]/PB[14]/PB[15]
#define  RB_PIN_PWMX        0x400                     // RW, PWM4/PWM5/PWM7/PWM8/PWM9 alternate pin enable: 0=PWM4/5/7/8/9 on PA[12]/PA[13]/PB[4]/PB[6]/PB[7], 1=PWM4/5/7/8/9 on PA[6]/PA[7]/PB[1]/PB[2]/P[3]
#define  RB_PIN_I2C         0x800                     // RW, SCL/SDA alternate pin enable: 0=SCL/SDA on PB[13]/PB[12], 1=SCL_/SDA_ on PB[21]/PB[20]
#define  RB_PIN_MODEM       0x1000                    // RW, DSR/DTR alternate pin enable: 0=DSR/DTR on PB[1]/PB[5], 1=DSR_/DTR_ on PB[14]/PB[15]
#define  RB_RF_ANT_SW_EN    0x8000                    // RW, RF antenna switch control output enable: 0=disable output, 1=output on PB[16]/PB[17]/PB[18]/PB[19]/PB[20]/PB[21]
#define R16_PIN_ANALOG_IE   (*((vu16*)0x4000101A)) // RW, analog pin enable and digital input disable
#define  RB_PIN_USB_DP_PU   0x40                      // RW, USB UDP internal pullup resistance enable: 0=enable/disable by RB_UC_DEV_PU_EN, 1=enable pullup, replace RB_UC_DEV_PU_EN under sleep mode
#define  RB_PIN_USB_IE      0x80                      // RW, USB analog I/O enable: 0=analog I/O disable, 1=analog I/O enable
#define R32_PIN_CONFIG2     (*((vu32*)0x4000101C)) // RW, I/O pin configuration
#define  RB_PIN_PA4_15_DIS  0xFFF0                    // RW, PA4-PA15 digital input disable
#define  RB_PIN_PB0_DIS     0x10000                   // RW, PB0 digital input disable
#define  RB_PIN_PB4_DIS     0x100000                  // RW, PB4 digital input disable
#define  RB_PIN_PB6_7_DIS   0xC00000                  // RW, PB6-PB7 digital input disable
#define  RB_PIN_PB22_23_DIS 0x3000000                 // RW, PB22-PB23 digital input disable
#define  RB_PIN_PB10_15_DIS 0xFC000000                // RW, PB10-PB15 digital input disable

/* System: power management register */
#define R32_POWER_MANAG     (*((vu32*)0x40001020)) // RWA, power management register, SAM
#define R16_POWER_PLAN      (*((vu16*)0x40001020)) // RWA, power plan before sleep instruction, SAM
#define  RB_PWR_XROM        0x01                      // RWA, power for flash ROM
#define  RB_PWR_RAM2K       0x02                      // RWA, power for retention 2KB SRAM
#define  RB_PWR_CORE        0x04                      // RWA, power retention for core and base peripherals
#define  RB_PWR_EXTEND      0x08                      // RWA, power retention for USB and BLE
#define  RB_PWR_RAM24K      0x10                      // RWA, power for main SRAM
#define  RB_MAIN_ACT        0x40                      // RWA, main power chose
#define  RB_PWR_SYS_EN      0x80                      // RWA, power for system
#define  RB_PWR_LDO_EN      0x0100                    // RWA, LDO enable
#define  RB_PWR_DCDC_EN     0x0200                    // RWA, DC/DC converter enable: 0=DC/DC disable and bypass, 1=DC/DC enable
#define  RB_PWR_DCDC_PRE    0x0400                    // RWA, DC/DC converter pre-enable
#define  RB_XT_PRE_CFG      0x1800                    // RWA, extern 32MHz HSE early wake up time configuration
#define  RB_PWR_MUST_0      0x2000                    // RWA, must write 0
#define  RB_XT_PRE_EN       0x4000                    // RWA, extern 32MHz HSE early wake up enable, must be used with LSI/LSE
#define  RB_PWR_PLAN_EN     0x8000                    // RWA/WZ, power plan enable, auto clear after sleep executed
#define R16_AUX_POWER_ADJ   (*((vu16*)0x40001022))  // RWA, aux power adjust control, SAM
#define  RB_ULPLDO_ADJ      0x0007                    // RWA, Ultra-Low-Power LDO voltage adjust
#define  RB_DCDC_CHARGE     0x0080                    // RWA, DC/DC aux charge enable
#define  RB_IPU_TKEY_SEL    0xC000                    // RWA, TouchKey wakeup

/* System: battery detector register */
#define R32_BATTERY_CTRL    (*((vu32*)0x40001024)) // RWA, battery voltage detector, SAM
#define R8_BAT_DET_CTRL     (*((vu8*)0x40001024))  // RWA, battery voltage detector control, SAM
#define  RB_BAT_DET_EN      0x01                      // RWA, battery voltage detector enable if RB_BAT_MON_EN=0
#define  RB_BAT_LOW_VTHX    0x01                      // RWA, select monitor threshold voltage if RB_BAT_MON_EN=1
#define  RB_BAT_MON_EN      0x02                      // RWA, battery voltage monitor enable under sleep mode
#define  RB_BAT_LOWER_IE    0x04                      // RWA, interrupt enable for battery lower voltage
#define  RB_BAT_LOW_IE      0x08                      // RWA, interrupt enable for battery low voltage
// request NMI interrupt if both RB_BAT_LOWER_IE and RB_BAT_LOW_IE enabled
#define R8_BAT_DET_CFG      (*((vu8*)0x40001025))  // RWA, battery voltage detector configuration, SAM
#define  RB_BAT_LOW_VTH     0x03                      // RWA, select detector/monitor threshold voltage of battery voltage low
#define R8_BAT_STATUS       (*((vu8*)0x40001026))  // RO, battery status
#define  RB_BAT_STAT_LOWER  0x01                      // RO, battery lower voltage status for detector, high action
#define  RB_BAT_STAT_LOW    0x02                      // RO, battery low voltage status for detector/monitor, high action

/* System: 32KHz oscillator control register */
#define R32_OSC32K_CTRL     (*((vu32*)0x4000102C)) // RWA, 32KHz oscillator control, SAM
#define R16_INT32K_TUNE     (*((vu16*)0x4000102C)) // RWA, internal 32KHz oscillator tune control, SAM
#define  RB_INT32K_TUNE     0x1FFF                    // RWA, internal 32KHz oscillator frequency tune
#define R8_XT32K_TUNE       (*((vu8*)0x4000102E))  // RWA, external 32KHz oscillator tune control, SAM
#define  RB_XT32K_I_TUNE    0x03                      // RWA, external 32KHz oscillator current tune: 00=75% current, 01=standard current, 10=150% current, 11=200% current for startup
#define  RB_XT32K_C_LOAD    0xF0                      // RWA, external 32KHz oscillator load capacitor tune: Cap = RB_XT32K_C_LOAD + 12pF
#define R8_CK32K_CONFIG     (*((vu8*)0x4000102F))  // RWA, 32KHz oscillator configure
#define  RB_CLK_XT32K_PON   0x01                      // RWA, external 32KHz oscillator power on
#define  RB_CLK_INT32K_PON  0x02                      // RWA, internal 32KHz oscillator power on
#define  RB_CLK_OSC32K_XT   0x04                      // RWA, 32KHz oscillator source selection: 0=RC, 1=XT
#define  RB_CLK_OSC32K_FILT 0x08                      // RWA, internal 32KHz oscillator low noise mode disable: 1=enable, 0=disable
#define  RB_32K_CLK_PIN     0x80                      // RO, 32KHz oscillator clock pin status

/* System: real-time clock register */
#define R32_RTC_CTRL        (*((vu32*)0x40001030)) // RWA, RTC control, SAM
#define R8_RTC_FLAG_CTRL    (*((vu8*)0x40001030))  // RW, RTC flag and clear control
#define  RB_RTC_TMR_CLR     0x10                      // RW, set 1 to clear RTC timer action flag, auto clear
#define  RB_RTC_TRIG_CLR    0x20                      // RW, set 1 to clear RTC trigger action flag, auto clear
#define  RB_RTC_TMR_FLAG    0x40                      // RO, RTC timer action flag
#define  RB_RTC_TRIG_FLAG   0x80                      // RO, RTC trigger action flag
#define R8_RTC_MODE_CTRL    (*((vu8*)0x40001031))  // RWA, RTC mode control, SAM
#define  RB_RTC_TMR_MODE    0x07                      // RWA, RTC timer mode: 000=0.125S, 001=0.25S, 010=0.5S, 011=1S, 100=2S, 101=4S, 110=8S, 111=16S
#define  RB_RTC_IGNORE_B0   0x08                      // RWA, force ignore bit0 for trigger mode: 0=compare bit0, 1=ignore bit0
#define  RB_RTC_TMR_EN      0x10                      // RWA, RTC timer mode enable
#define  RB_RTC_TRIG_EN     0x20                      // RWA, RTC trigger mode enable
#define  RB_RTC_LOAD_LO     0x40                      // RWA, set 1 to load RTC count low word R32_RTC_CNT_32K, auto clear after loaded
#define  RB_RTC_LOAD_HI     0x80                      // RWA, set 1 to load RTC count high word R32_RTC_CNT_DAY, auto clear after loaded
#define R32_RTC_TRIG        (*((vu32*)0x40001034)) // RWA, RTC trigger value, SAM
#define R32_RTC_CNT_32K     (*((vu32*)0x40001038)) // RO, RTC count based 32KHz
#define R16_RTC_CNT_32K     (*((vu16*)0x40001038)) // RO, RTC count based 32KHz
#define R16_RTC_CNT_2S      (*((vu16*)0x4000103A)) // RO, RTC count based 2 second
#define R32_RTC_CNT_DAY     (*((vu32*)0x4000103C)) // RO, RTC count based one day, only low 14 bit

#define RTC_MAX_COUNT       0xA8C00000
#define RTC_FREQ            32000 // LSI
// #define RTC_FREQ            32768 // LSE
#define CLK_PER_US          (1.0 / ((1.0 / RTC_FREQ) * 1000 * 1000))
#define CLK_PER_MS          (CLK_PER_US * 1000)
#define US_TO_RTC(us)       ((uint32_t)((us) * CLK_PER_US + 0.5))
#define MS_TO_RTC(ms)       ((uint32_t)((ms) * CLK_PER_MS + 0.5))
#define RTC_WAIT_TICKS(t)   uint32_t rtcset = R32_RTC_CNT_32K +(t); while(R32_RTC_CNT_32K <= rtcset)
#define SLEEP_RTC_MIN_TIME  US_TO_RTC(1000)
#define SLEEP_RTC_MAX_TIME  (RTC_MAX_COUNT - 1000 * 1000 * 30)
#define WAKE_UP_RTC_MAX_TIME US_TO_RTC(1600)

/* System: safe accessing register */
#define R32_SAFE_ACCESS     (*((vu32*)0x40001040)) // RW, safe accessing
#define R8_SAFE_ACCESS_SIG  (*((vu8*)0x40001040))  // WO, safe accessing sign register, must write SAFE_ACCESS_SIG1 then SAFE_ACCESS_SIG2 to enter safe accessing mode
#define  RB_SAFE_ACC_MODE   0x03                      // RO, current safe accessing mode: 11=safe/unlocked (SAM), other=locked (00..01..10..11)
#define  RB_SAFE_ACC_ACT    0x08                      // RO, indicate safe accessing status now: 0=locked, read only, 1=safe/unlocked (SAM), write enabled
#define  RB_SAFE_ACC_TIMER  0x70                      // RO, safe accessing timer bit mask (16*clock number)
#define SAFE_ACCESS_SIG1    0x57                      // WO: safe accessing sign value step 1
#define SAFE_ACCESS_SIG2    0xA8                      // WO: safe accessing sign value step 2
#define SAFE_ACCESS_SIG0    0x00                      // WO: safe accessing sign value for disable
#define R8_CHIP_ID          (*((vu8*)0x40001041))  // RF, chip ID register, always is ID_CH59*
#define R8_SAFE_ACCESS_ID   (*((vu8*)0x40001042))  // RF, safe accessing ID register, always 0x0C
#define R8_WDOG_COUNT       (*((vu8*)0x40001043))  // RW, watch-dog count, count by clock frequency Fsys/131072
#define SYS_SAFE_ACCESS(a)  do { R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1; \
								 R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2; \
								 asm volatile ("nop\nnop"); \
								 {a} \
								 R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG0; \
								 asm volatile ("nop\nnop"); } while(0)

/* System: global configuration register */
#define R32_GLOBAL_CONFIG   (*((vu32*)0x40001044)) // RW, global configuration
#define R8_RESET_STATUS     (*((vu8*)0x40001044))  // RO, reset status
#define  RB_RESET_FLAG      0x07                      // RO: recent reset flag
#define  RST_FLAG_SW        0x00
#define  RST_FLAG_RPOR      0x01
#define  RST_FLAG_WTR       0x02
#define  RST_FLAG_MR        0x03
//#define  RST_FLAG_GPWSM     0x04                      // RO, power on reset flag during sleep/shutdown: 0=no power on reset during sleep/shutdown, 1=power on reset occurred during sleep/shutdown
#define  RST_FLAG_GPWSM     0x05
// RB_RESET_FLAG: recent reset flag
//   000 - SR, software reset, by RB_SOFTWARE_RESET=1 @RB_WDOG_RST_EN=0
//   001 - RPOR, real power on reset
//   010 - WTR, watch-dog timer-out reset
//   011 - MR, external manual reset by RST pin input low
//   101 - GRWSM, global reset by waking under shutdown mode
//   1?? - LRW, power on reset occurred during sleep
#define R8_GLOB_ROM_CFG     R8_RESET_STATUS           // RWA, flash ROM configuration, SAM
#define  RB_ROM_CODE_OFS    0x10                      // RWA, code offset address selection in Flash ROM: 0=start address 0x000000, 1=start address 0x040000
#define  RB_ROM_CTRL_EN     0x20                      // RWA, enable flash ROM control interface enable: 0=disable access, 1=enable access control register
#define  RB_ROM_DATA_WE     0x40                      // RWA, enable flash ROM data & code area being erase/write: 0=all writing protect, 1=enable data area program and erase
#define  RB_ROM_CODE_WE     0x80                      // RWA, enable flash ROM code area being erase/write: 0=code writing protect, 1=enable code area program and erase
#define R8_GLOB_CFG_INFO    (*((vu8*)0x40001045))  // RO, global configuration information and status
#define  RB_CFG_ROM_READ    0x01                      // RO, indicate protected status of Flash ROM code and data: 0=reading protect, 1=enable read by external programmer
#define  RB_CFG_RESET_EN    0x04                      // RO, manual reset input enable status
#define  RB_CFG_BOOT_EN     0x08                      // RO, boot-loader enable status
#define  RB_CFG_DEBUG_EN    0x10                      // RO, debug enable status
#define  RB_BOOT_LOADER     0x20                      // RO, indicate boot loader status: 0=application status (by software reset), 1=boot loader status
#define R8_RST_WDOG_CTRL    (*((vu8*)0x40001046))  // RWA, reset and watch-dog control, SAM
#define  RB_SOFTWARE_RESET  0x01                      // WA/WZ, global software reset, high action, auto clear
#define  RB_WDOG_RST_EN     0x02                      // RWA, enable watch-dog reset if watch-dog timer overflow: 0=as timer only, 1=enable reset if timer overflow
#define  RB_WDOG_INT_EN     0x04                      // RWA, watch-dog timer overflow interrupt enable: 0=disable, 1=enable
#define  RB_WDOG_INT_FLAG   0x10                      // RW1, watch-dog timer overflow interrupt flag, cleared by RW1 or reload watch-dog count or __SEV(Send-Event)
#define R8_GLOB_RESET_KEEP  (*((vu8*)0x40001047))  // RW, value keeper during global reset

/*System: Miscellaneous Control register */
#define R32_MISC_CTRL       (*((vu32*)0x40001048)) // RWA, miscellaneous control register
#define R8_PLL_CONFIG       (*((vu8*)0x4000104B))  // RWA, PLL configuration control, SAM
#define  RB_PLL_CFG_DAT     0x7F                      // RWA, PLL configuration control, SAM

/* System: 32MHz oscillator control register */
#define R8_XT32M_TUNE       (*((vu8*)0x4000104E))  // RWA, external 32MHz oscillator tune control, SAM
#define  RB_XT32M_I_BIAS    0x03                      // RWA, external 32MHz oscillator bias current tune: 00=75% current, 01=standard current, 10=125% current, 11=150% current
#define  RB_XT32M_C_LOAD    0x70                      // RWA, external 32MHz oscillator load capacitor tune: Cap = RB_XT32M_C_LOAD * 2 + 10pF

/* System: oscillator frequency calibration register */
#define R32_OSC_CALIB       (*((vu32*)0x40001050)) // RWA, oscillator frequency calibration, SAM
#define R16_OSC_CAL_CNT     (*((vu16*)0x40001050)) // RO, system clock count value for 32KHz multi-cycles
#define  RB_OSC_CAL_CNT     0x3FFF                    // RO, system clock count value for 32KHz multi-cycles
#define  RB_OSC_CAL_OV_CLR  0x4000                    // RW1, indicate R8_OSC_CAL_OV_CNT not zero, set 1 to clear R8_OSC_CAL_OV_CNT
#define  RB_OSC_CAL_IF      0x8000                    // RW1, interrupt flag for oscillator capture end, set 1 to clear
#define R8_OSC_CAL_OV_CNT   (*((vu8*)0x40001052))  // RO, oscillator frequency calibration overflow times
#define R8_OSC_CAL_CTRL     (*((vu8*)0x40001053))  // RWA, oscillator frequency calibration control, SAM
#define  RB_OSC_CNT_TOTAL   0x07                      // RWA, total cycles mode for oscillator capture
// RB_OSC_CNT_TOTAL: select total cycles for oscillator capture
//    000: 1
//    001: 2
//    010: 4
//    011: 32
//    100: 64
//    101: 128
//    110: 1024
//    111: 2047
#define  RB_OSC_CNT_HALT    0x08                      // RO, calibration counter halt status: 0=counting, 1=halt for reading count value
#define  RB_OSC_CAL_IE      0x10                      // RWA, interrupt enable for oscillator capture end
#define  RB_OSC_CNT_EN      0x20                      // RWA, calibration counter enable
#define  RB_OSC_CNT_END     0x40                      // RWA, select oscillator capture end mode: 0=normal, 1=append 2 cycles

/* System: Flash ROM control register */
#define R32_FLASH_DATA      (*((vu32*)0x40001800)) // RO/WO, flash ROM data
#define R32_FLASH_CONTROL   (*((vu32*)0x40001804)) // RW, flash ROM control
#define R8_FLASH_DATA       (*((vu8*)0x40001804))  // RO/WO, flash ROM data buffer
#define R8_FLASH_CTRL       (*((vu8*)0x40001806))  // RW, flash ROM access control
#define R8_FLASH_CFG        (*((vu8*)0x40001807))  // RW, flash ROM access config, SAM


/* GPIO PA register */
#define R32_PA_DIR          (*((vu32*)0x400010A0)) // RW, GPIO PA I/O direction: 0=in, 1=out
#define R32_PA_PIN          (*((vu32*)0x400010A4)) // RO, GPIO PA input
#define R32_PA_OUT          (*((vu32*)0x400010A8)) // RW, GPIO PA output
#define R32_PA_CLR          (*((vu32*)0x400010AC)) // WZ, GPIO PA clear output: 0=keep, 1=clear
#define R32_PA_PU           (*((vu32*)0x400010B0)) // RW, GPIO PA pullup resistance enable
#define R32_PA_PD_DRV       (*((vu32*)0x400010B4)) // RW, PA pulldown for input or PA driving capability for output

/* GPIO PB register */
#define R32_PB_DIR          (*((vu32*)0x400010C0)) // RW, GPIO PB I/O direction: 0=in, 1=out
#define R32_PB_PIN          (*((vu32*)0x400010C4)) // RO, GPIO PB input
#define R32_PB_OUT          (*((vu32*)0x400010C8)) // RW, GPIO PB output
#define R32_PB_CLR          (*((vu32*)0x400010CC)) // WZ, GPIO PB clear output: 0=keep, 1=clear
#define R32_PB_PU           (*((vu32*)0x400010D0)) // RW, GPIO PB pullup resistance enable
#define R32_PB_PD_DRV       (*((vu32*)0x400010D4)) // RW, PB pulldown for input or PB driving capability for output

#define PA0      (0x00000001) /*!< Pin 0 selected */
#define PA1      (0x00000002) /*!< Pin 1 selected */
#define PA2      (0x00000004) /*!< Pin 2 selected */
#define PA3      (0x00000008) /*!< Pin 3 selected */
#define PA4      (0x00000010) /*!< Pin 4 selected */
#define PA5      (0x00000020) /*!< Pin 5 selected */
#define PA6      (0x00000040) /*!< Pin 6 selected */
#define PA7      (0x00000080) /*!< Pin 7 selected */
#define PA8      (0x00000100) /*!< Pin 8 selected */
#define PA9      (0x00000200) /*!< Pin 9 selected */
#define PA10     (0x00000400) /*!< Pin 10 selected */
#define PA11     (0x00000800) /*!< Pin 11 selected */
#define PA12     (0x00001000) /*!< Pin 12 selected */
#define PA13     (0x00002000) /*!< Pin 13 selected */
#define PA14     (0x00004000) /*!< Pin 14 selected */
#define PA15     (0x00008000) /*!< Pin 15 selected */

#define PB       (0x80000000) /* Bit mask to indicate bank B */
#define PB0      (0x80000001) /*!< Pin 0 selected */
#define PB1      (0x80000002) /*!< Pin 1 selected */
#define PB2      (0x80000004) /*!< Pin 2 selected */
#define PB3      (0x80000008) /*!< Pin 3 selected */
#define PB4      (0x80000010) /*!< Pin 4 selected */
#define PB5      (0x80000020) /*!< Pin 5 selected */
#define PB6      (0x80000040) /*!< Pin 6 selected */
#define PB7      (0x80000080) /*!< Pin 7 selected */
#define PB8      (0x80000100) /*!< Pin 8 selected */
#define PB9      (0x80000200) /*!< Pin 9 selected */
#define PB10     (0x80000400) /*!< Pin 10 selected */
#define PB11     (0x80000800) /*!< Pin 11 selected */
#define PB12     (0x80001000) /*!< Pin 12 selected */
#define PB13     (0x80002000) /*!< Pin 13 selected */
#define PB14     (0x80004000) /*!< Pin 14 selected */
#define PB15     (0x80008000) /*!< Pin 15 selected */
#define PB16     (0x80010000) /*!< Pin 16 selected */
#define PB17     (0x80020000) /*!< Pin 17 selected */
#define PB18     (0x80040000) /*!< Pin 18 selected */
#define PB19     (0x80080000) /*!< Pin 19 selected */
#define PB20     (0x80100000) /*!< Pin 20 selected */
#define PB21     (0x80200000) /*!< Pin 21 selected */
#define PB22     (0x80400000) /*!< Pin 22 selected */
#define PB23     (0x80800000) /*!< Pin 23 selected */
#define P_All    (0xFFFFFFFF) /*!< All pins selected */

typedef enum
{
	GPIO_ModeIN_Floating,
	GPIO_ModeIN_PU,
	GPIO_ModeIN_PD,
	GPIO_ModeOut_PP_5mA,
	GPIO_ModeOut_PP_20mA,
} GPIOModeTypeDef;

/* General Purpose I/O */
typedef enum
{
	GPIO_CFGLR_IN_FLOAT = GPIO_ModeIN_Floating,
	GPIO_CFGLR_IN_PUPD = GPIO_ModeIN_PU, // is most common
	GPIO_CFGLR_IN_PU = GPIO_ModeIN_PU,
	GPIO_CFGLR_IN_PD = GPIO_ModeIN_PD, // to suppress the -Wswitch warning
	GPIO_CFGLR_OUT_10Mhz_PP = GPIO_ModeOut_PP_20mA,
	GPIO_CFGLR_OUT_2Mhz_PP = GPIO_ModeOut_PP_5mA,
	GPIO_CFGLR_OUT_50Mhz_PP = GPIO_ModeOut_PP_20mA,
} GPIO_CFGLR_PIN_MODE_Typedef;

#define EEPROM_PAGE_SIZE    256                       // Flash-ROM & Data-Flash page size for writing
#define EEPROM_BLOCK_SIZE   4096                      // Flash-ROM & Data-Flash block size for erasing
#define EEPROM_MIN_ER_SIZE  EEPROM_PAGE_SIZE          // Data-Flash minimal size for erasing
//#define EEPROM_MIN_ER_SIZE  EEPROM_BLOCK_SIZE         // Flash-ROM  minimal size for erasing
#define EEPROM_MIN_WR_SIZE  1                         // Data-Flash minimal size for writing
#define EEPROM_MAX_SIZE     0x8000                    // Data-Flash maximum size, 32KB
#define FLASH_MIN_WR_SIZE   4                         // Flash-ROM minimal size for writing
#define FLASH_ROM_MAX_SIZE  0x070000                  // Flash-ROM maximum program size, 448KB

#define CMD_FLASH_ROM_START_IO	0x00		// start FlashROM I/O, without parameter
#define CMD_FLASH_ROM_SW_RESET	0x04		// software reset FlashROM, without parameter
#define CMD_GET_ROM_INFO		0x06		// get information from FlashROM, parameter @Address,Buffer
#define CMD_GET_UNIQUE_ID		0x07		// get 64 bit unique ID, parameter @Buffer
#define CMD_FLASH_ROM_PWR_DOWN	0x0D		// power-down FlashROM, without parameter
#define CMD_FLASH_ROM_PWR_UP	0x0C		// power-up FlashROM, without parameter
#define CMD_FLASH_ROM_LOCK		0x08		// lock(protect)/unlock FlashROM data block, return 0 if success, parameter @StartAddr
// StartAddr: 0=unlock all, 1=lock boot code, 3=lock all code and data

#define CMD_EEPROM_ERASE		0x09		// erase Data-Flash block, return 0 if success, parameter @StartAddr,Length
#define CMD_EEPROM_WRITE		0x0A		// write Data-Flash data block, return 0 if success, parameter @StartAddr,Buffer,Length
#define CMD_EEPROM_READ			0x0B		// read Data-Flash data block, parameter @StartAddr,Buffer,Length
#define CMD_FLASH_ROM_ERASE		0x01		// erase FlashROM block, return 0 if success, parameter @StartAddr,Length
#define CMD_FLASH_ROM_WRITE		0x02		// write FlashROM data block, minimal block is dword, return 0 if success, parameter @StartAddr,Buffer,Length
#define CMD_FLASH_ROM_VERIFY	0x03		// read FlashROM data block, minimal block is dword, return 0 if success, parameter @StartAddr,Buffer,Length

#define ROM_CFG_MAC_ADDR	0x7F018			// address for MAC address information
#define ROM_CFG_BOOT_INFO	0x7DFF8			// address for BOOT information

#define flash_rom_start_io( )                       flash_eeprom_cmd( CMD_FLASH_ROM_START_IO, 0, 0, 0 )
#define flash_rom_sw_reset( )                       flash_eeprom_cmd( CMD_FLASH_ROM_SW_RESET, 0, 0, 0 )
#define get_mac_address(Buffer)                     flash_eeprom_cmd( CMD_GET_ROM_INFO, ROM_CFG_MAC_ADDR, Buffer, 0 )
#define get_boot_info(Buffer)                       flash_eeprom_cmd( CMD_GET_ROM_INFO, ROM_CFG_BOOT_INFO, Buffer, 0 )
#define get_unique_id(Buffer)                       flash_eeprom_cmd( CMD_GET_UNIQUE_ID, 0, Buffer, 0 )
#define flash_rom_pwr_down( )                       flash_eeprom_cmd( CMD_FLASH_ROM_PWR_DOWN, 0, 0, 0 )
#define flash_rom_pwr_up( )                         flash_eeprom_cmd( CMD_FLASH_ROM_PWR_UP, 0, 0, 0 )
#define eeprom_read(StartAddr,Buffer,Length)        flash_eeprom_cmd( CMD_EEPROM_READ, StartAddr, Buffer, Length )
#define eeprom_erase(StartAddr,Length)              flash_eeprom_cmd( CMD_EEPROM_ERASE, StartAddr, 0, Length )
#define eeprom_write(StartAddr,Buffer,Length)       flash_eeprom_cmd( CMD_EEPROM_WRITE, StartAddr, Buffer, Length )
#define flash_rom_erase(StartAddr,Length)           flash_eeprom_cmd( CMD_FLASH_ROM_ERASE, StartAddr, 0, Length )
#define flash_rom_write(StartAddr,Buffer,Length)    flash_eeprom_cmd( CMD_FLASH_ROM_WRITE, StartAddr, Buffer, Length )
#define flash_rom_verify(StartAddr,Buffer,Length)   flash_eeprom_cmd( CMD_FLASH_ROM_VERIFY, StartAddr, Buffer, Length )
#define flash_rom_read(StartAddr,Buffer,Length)     do { int k,l = (Length +3)>>2; for(k=0; k<l; k++) ((uint32_t*)(Buffer))[k] = ((uint32_t*)(StartAddr))[k]; } while(0)

/* UART0 register */
#define R32_UART0_CTRL      (*((vu32*)0x40003000)) // RW, UART0 control
#define R8_UART0_MCR        (*((vu8*)0x40003000))  // RW, UART0 modem control
#define R8_UART0_IER        (*((vu8*)0x40003001))  // RW, UART0 interrupt enable
#define R8_UART0_FCR        (*((vu8*)0x40003002))  // RW, UART0 FIFO control
#define R8_UART0_LCR        (*((vu8*)0x40003003))  // RW, UART0 line control
#define R32_UART0_STAT      (*((vu32*)0x40003004)) // RO, UART0 status
#define R8_UART0_IIR        (*((vu8*)0x40003004))  // RO, UART0 interrupt identification
#define R8_UART0_LSR        (*((vu8*)0x40003005))  // RO, UART0 line status
#define R8_UART0_MSR        (*((vu8*)0x40003006))  // RO, UART0 modem status
#define R32_UART0_FIFO      (*((vu32*)0x40003008)) // RW, UART0 data or FIFO port
#define R8_UART0_RBR        (*((vu8*)0x40003008))  // RO, UART0 receiver buffer, receiving byte
#define R8_UART0_THR        (*((vu8*)0x40003008))  // WO, UART0 transmitter holding, transmittal byte
#define R8_UART0_RFC        (*((vu8*)0x4000300A))  // RO, UART0 receiver FIFO count
#define R8_UART0_TFC        (*((vu8*)0x4000300B))  // RO, UART0 transmitter FIFO count
#define R32_UART0_SETUP     (*((vu32*)0x4000300C)) // RW, UART0 setup
#define R16_UART0_DL        (*((vu16*)0x4000300C)) // RW, UART0 divisor latch
#define R8_UART0_DLL        (*((vu8*)0x4000300C))  // RW, UART0 divisor latch LSB byte
// #define R8_UART0_DLM        (*((vu8*)0x4000300D))  // RW, UART0 divisor latch MSB byte
#define R8_UART0_DIV        (*((vu8*)0x4000300E))  // RW, UART0 pre-divisor latch byte, only low 7 bit, from 1 to 0/128
#define R8_UART0_ADR        (*((vu8*)0x4000300F))  // RW, UART0 slave address: 0xFF=disable, other=enable

/* UART1 register */
#define R32_UART1_CTRL      (*((vu32*)0x40003400)) // RW, UART1 control
#define R8_UART1_MCR        (*((vu8*)0x40003400))  // RW, UART1 modem control
#define R8_UART1_IER        (*((vu8*)0x40003401))  // RW, UART1 interrupt enable
#define R8_UART1_FCR        (*((vu8*)0x40003402))  // RW, UART1 FIFO control
#define R8_UART1_LCR        (*((vu8*)0x40003403))  // RW, UART1 line control
#define R32_UART1_STAT      (*((vu32*)0x40003404)) // RO, UART1 status
#define R8_UART1_IIR        (*((vu8*)0x40003404))  // RO, UART1 interrupt identification
#define R8_UART1_LSR        (*((vu8*)0x40003405))  // RO, UART1 line status
#define R32_UART1_FIFO      (*((vu32*)0x40003408)) // RW, UART1 data or FIFO port
#define R8_UART1_RBR        (*((vu8*)0x40003408))  // RO, UART1 receiver buffer, receiving byte
#define R8_UART1_THR        (*((vu8*)0x40003408))  // WO, UART1 transmitter holding, transmittal byte
#define R8_UART1_RFC        (*((vu8*)0x4000340A))  // RO, UART1 receiver FIFO count
#define R8_UART1_TFC        (*((vu8*)0x4000340B))  // RO, UART1 transmitter FIFO count
#define R32_UART1_SETUP     (*((vu32*)0x4000340C)) // RW, UART1 setup
#define R16_UART1_DL        (*((vu16*)0x4000340C)) // RW, UART1 divisor latch
#define R8_UART1_DLL        (*((vu8*)0x4000340C))  // RW, UART1 divisor latch LSB byte
// #define R8_UART1_DLM        (*((vu8*)0x4000340D))  // RW, UART1 divisor latch MSB byte
#define R8_UART1_DIV        (*((vu8*)0x4000340E))  // RW, UART1 pre-divisor latch byte, only low 7 bit, from 1 to 0/128

/* UART2 register */
#define R32_UART2_CTRL      (*((vu32*)0x40003800)) // RW, UART2 control
#define R8_UART2_MCR        (*((vu8*)0x40003800))  // RW, UART2 modem control
#define R8_UART2_IER        (*((vu8*)0x40003801))  // RW, UART2 interrupt enable
#define R8_UART2_FCR        (*((vu8*)0x40003802))  // RW, UART2 FIFO control
#define R8_UART2_LCR        (*((vu8*)0x40003803))  // RW, UART2 line control
#define R32_UART2_STAT      (*((vu32*)0x40003804)) // RO, UART2 status
#define R8_UART2_IIR        (*((vu8*)0x40003804))  // RO, UART2 interrupt identification
#define R8_UART2_LSR        (*((vu8*)0x40003805))  // RO, UART2 line status
#define R32_UART2_FIFO      (*((vu32*)0x40003808)) // RW, UART2 data or FIFO port
#define R8_UART2_RBR        (*((vu8*)0x40003808))  // RO, UART2 receiver buffer, receiving byte
#define R8_UART2_THR        (*((vu8*)0x40003808))  // WO, UART2 transmitter holding, transmittal byte
#define R8_UART2_RFC        (*((vu8*)0x4000380A))  // RO, UART2 receiver FIFO count
#define R8_UART2_TFC        (*((vu8*)0x4000380B))  // RO, UART2 transmitter FIFO count
#define R32_UART2_SETUP     (*((vu32*)0x4000380C)) // RW, UART2 setup
#define R16_UART2_DL        (*((vu16*)0x4000380C)) // RW, UART2 divisor latch
#define R8_UART2_DLL        (*((vu8*)0x4000380C))  // RW, UART2 divisor latch LSB byte
// #define R8_UART2_DLM        (*((vu8*)0x4000380D))  // RW, UART2 divisor latch MSB byte
#define R8_UART2_DIV        (*((vu8*)0x4000380E))  // RW, UART2 pre-divisor latch byte, only low 7 bit, from 1 to 0/128

/* UART3 register */
#define R32_UART3_CTRL      (*((vu32*)0x40003C00)) // RW, UART3 control
#define R8_UART3_MCR        (*((vu8*)0x40003C00))  // RW, UART3 modem control
#define R8_UART3_IER        (*((vu8*)0x40003C01))  // RW, UART3 interrupt enable
#define R8_UART3_FCR        (*((vu8*)0x40003C02))  // RW, UART3 FIFO control
#define R8_UART3_LCR        (*((vu8*)0x40003C03))  // RW, UART3 line control
#define R32_UART3_STAT      (*((vu32*)0x40003C04)) // RO, UART3 status
#define R8_UART3_IIR        (*((vu8*)0x40003C04))  // RO, UART3 interrupt identification
#define R8_UART3_LSR        (*((vu8*)0x40003C05))  // RO, UART3 line status
#define R32_UART3_FIFO      (*((vu32*)0x40003C08)) // RW, UART3 data or FIFO port
#define R8_UART3_RBR        (*((vu8*)0x40003C08))  // RO, UART3 receiver buffer, receiving byte
#define R8_UART3_THR        (*((vu8*)0x40003C08))  // WO, UART3 transmitter holding, transmittal byte
#define R8_UART3_RFC        (*((vu8*)0x40003C0A))  // RO, UART3 receiver FIFO count
#define R8_UART3_TFC        (*((vu8*)0x40003C0B))  // RO, UART3 transmitter FIFO count
#define R32_UART3_SETUP     (*((vu32*)0x40003C0C)) // RW, UART3 setup
#define R16_UART3_DL        (*((vu16*)0x40003C0C)) // RW, UART3 divisor latch
#define R8_UART3_DLL        (*((vu8*)0x40003C0C))  // RW, UART3 divisor latch LSB byte
// #define R8_UART3_DLM        (*((vu8*)0x40003C0D))  // RW, UART3 divisor latch MSB byte
#define R8_UART3_DIV        (*((vu8*)0x40003C0E))  // RW, UART3 pre-divisor latch byte, only low 7 bit, from 1 to 0/128

/* UART register address offset and bit define */
#define UART_FIFO_SIZE      8                         // UART FIFO size (depth)
#define UART_RECV_RDY_SZ    7                         // the max FIFO trigger level for UART receiver data available
#define BA_UART0            ((vu8*)0x40003000)     // point UART0 base address
#define BA_UART1            ((vu8*)0x40003400)     // point UART1 base address
#define BA_UART2            ((vu8*)0x40003800)     // point UART2 base address
#define BA_UART3            ((vu8*)0x40003C00)     // point UART3 base address
#define UART_MCR            0
#define  RB_MCR_DTR         0x01                      // RW, UART0 control DTR
#define  RB_MCR_RTS         0x02                      // RW, UART0 control RTS
#define  RB_MCR_OUT1        0x04                      // RW, UART0 control OUT1
#define  RB_MCR_OUT2        0x08                      // RW, UART control OUT2
#define  RB_MCR_INT_OE      0x08                      // RW, UART interrupt output enable
#define  RB_MCR_LOOP        0x10                      // RW, UART0 enable local loop back
#define  RB_MCR_AU_FLOW_EN  0x20                      // RW, UART0 enable autoflow control
#define  RB_MCR_TNOW        0x40                      // RW, UART0 enable TNOW output on DTR pin
#define  RB_MCR_HALF        0x80                      // RW, UART0 enable half-duplex
#define UART_IER            1
#define  RB_IER_RECV_RDY    0x01                      // RW, UART interrupt enable for receiver data ready
#define  RB_IER_THR_EMPTY   0x02                      // RW, UART interrupt enable for THR empty
#define  RB_IER_LINE_STAT   0x04                      // RW, UART interrupt enable for receiver line status
#define  RB_IER_MODEM_CHG   0x08                      // RW, UART0 interrupt enable for modem status change
#define  RB_IER_DTR_EN      0x10                      // RW, UART0 DTR/TNOW output pin enable
#define  RB_IER_RTS_EN      0x20                      // RW, UART0 RTS output pin enable
#define  RB_IER_TXD_EN      0x40                      // RW, UART TXD pin enable
#define  RB_IER_RESET       0x80                      // WZ, UART software reset control, high action, auto clear
#define UART_FCR            2
#define  RB_FCR_FIFO_EN     0x01                      // RW, UART FIFO enable
#define  RB_FCR_RX_FIFO_CLR 0x02                      // WZ, clear UART receiver FIFO, high action, auto clear
#define  RB_FCR_TX_FIFO_CLR 0x04                      // WZ, clear UART transmitter FIFO, high action, auto clear
#define  RB_FCR_FIFO_TRIG   0xC0                      // RW, UART receiver FIFO trigger level: 00-1byte, 01-2bytes, 10-4bytes, 11-7bytes
#define UART_LCR            3
#define  RB_LCR_WORD_SZ     0x03                      // RW, UART word bit length: 00-5bit, 01-6bit, 10-7bit, 11-8bit
#define  RB_LCR_STOP_BIT    0x04                      // RW, UART stop bit length: 0-1bit, 1-2bit
#define  RB_LCR_PAR_EN      0x08                      // RW, UART parity enable
#define  RB_LCR_PAR_MOD     0x30                      // RW, UART parity mode: 00-odd, 01-even, 10-mark, 11-space
#define  RB_LCR_BREAK_EN    0x40                      // RW, UART break control enable
#define  RB_LCR_DLAB        0x80                      // RW, UART reserved bit
#define  RB_LCR_GP_BIT      0x80                      // RW, UART general purpose bit
#define UART_IIR            4
#define  RB_IIR_NO_INT      0x01                      // RO, UART no interrupt flag: 0=interrupt action, 1=no interrupt
#define  RB_IIR_INT_MASK    0x0F                      // RO, UART interrupt flag bit mask
#define  RB_IIR_FIFO_ID     0xC0                      // RO, UART FIFO enabled flag
#define UART_LSR            5
#define  RB_LSR_DATA_RDY    0x01                      // RO, UART receiver fifo data ready status
#define  RB_LSR_OVER_ERR    0x02                      // RZ, UART receiver overrun error
#define  RB_LSR_PAR_ERR     0x04                      // RZ, UART receiver parity error
#define  RB_LSR_FRAME_ERR   0x08                      // RZ, UART receiver frame error
#define  RB_LSR_BREAK_ERR   0x10                      // RZ, UART receiver break error
#define  RB_LSR_TX_FIFO_EMP 0x20                      // RO, UART transmitter fifo empty status
#define  RB_LSR_TX_ALL_EMP  0x40                      // RO, UART transmitter all empty status
#define  RB_LSR_ERR_RX_FIFO 0x80                      // RO, indicate error in UART receiver fifo
#define UART_MSR            6
#define  RB_MSR_CTS_CHG     0x01                      // RZ, UART0 CTS changed status, high action
#define  RB_MSR_DSR_CHG     0x02                      // RZ, UART0 DSR changed status, high action
//#define  RB_MSR_RI_CHG      0x04                      // RZ, UART0 RI changed status, high action
//#define  RB_MSR_DCD_CHG     0x08                      // RZ, UART0 DCD changed status, high action
#define  RB_MSR_CTS         0x10                      // RO, UART0 CTS action status
#define  RB_MSR_DSR         0x20                      // RO, UART0 DSR action status
//#define  RB_MSR_RI          0x40                      // RO, UART0 RI action status
//#define  RB_MSR_DCD         0x80                      // RO, UART0 DCD action status
#define UART_RBR            8
#define UART_THR            8
#define UART_RFC            0x0A
#define UART_TFC            0x0B
#define UART_DLL            0x0C
// #define UART_DLM            0x0D
#define UART_DIV            0x0E
#define UART_ADR            0x0F

/* UART interrupt identification values for IIR bits 3:0 */
#define UART_II_SLV_ADDR    0x0E                      // RO, UART0 slave address match
#define UART_II_LINE_STAT   0x06                      // RO, UART interrupt by receiver line status
#define UART_II_RECV_RDY    0x04                      // RO, UART interrupt by receiver data available
#define UART_II_RECV_TOUT   0x0C                      // RO, UART interrupt by receiver fifo timeout
#define UART_II_THR_EMPTY   0x02                      // RO, UART interrupt by THR empty
#define UART_II_MODEM_CHG   0x00                      // RO, UART0 interrupt by modem status change
#define UART_II_NO_INTER    0x01                      // RO, no UART interrupt is pending


RV_STATIC_INLINE void LSIEnable() {
	SYS_SAFE_ACCESS(
		R8_CK32K_CONFIG &= ~(RB_CLK_OSC32K_XT | RB_CLK_XT32K_PON); // turn off LSE
		R8_CK32K_CONFIG |= RB_CLK_INT32K_PON; // turn on LSI
	);
}

RV_STATIC_INLINE void DCDCEnable()
{
	SYS_SAFE_ACCESS(
		R16_AUX_POWER_ADJ |= RB_DCDC_CHARGE;
		R16_POWER_PLAN |= RB_PWR_DCDC_PRE;
	);

	RTC_WAIT_TICKS(2);

	SYS_SAFE_ACCESS(
		R16_POWER_PLAN |= RB_PWR_DCDC_EN;
	);
}

RV_STATIC_INLINE void SleepInit() {
	SYS_SAFE_ACCESS(
		R8_SLP_WAKE_CTRL |= RB_SLP_RTC_WAKE;
		R8_RTC_MODE_CTRL |= RB_RTC_TRIG_EN;
	);
	// NVIC_EnableIRQ(RTC_IRQn) is not available here yet
	NVIC->IENR[((uint32_t)(RTC_IRQn) >> 5)] = (1 << ((uint32_t)(RTC_IRQn) & 0x1F));
}

RV_STATIC_INLINE void RTCInit() {
	SYS_SAFE_ACCESS(
		R32_RTC_TRIG = 0;
		R8_RTC_MODE_CTRL |= RB_RTC_LOAD_HI;
	);
	while((R32_RTC_TRIG & 0x3FFF) != (R32_RTC_CNT_DAY & 0x3FFF));
	SYS_SAFE_ACCESS(
		R32_RTC_TRIG = 0;
		R8_RTC_MODE_CTRL |= RB_RTC_LOAD_LO;
	);
}

RV_STATIC_INLINE void RTCTrigger(uint32_t cyc) {
	uint32_t t = R32_RTC_CNT_32K + cyc;
	if(t > RTC_MAX_COUNT) {
		t -= RTC_MAX_COUNT;
	}

	SYS_SAFE_ACCESS(
		R32_RTC_TRIG = t;
	);
}

RV_STATIC_INLINE void LowPowerIdle(uint32_t cyc)
{
	RTCTrigger(cyc);

	NVIC->SCTLR &= ~(1 << 2); // sleep
	NVIC->SCTLR &= ~(1 << 3); // wfi
	asm volatile ("wfi\nnop\nnop" );
}

RV_STATIC_INLINE void LowPowerSleep(uint32_t cyc, uint16_t power_plan) {
	RTCTrigger(cyc);

	SYS_SAFE_ACCESS(
		R8_BAT_DET_CTRL = 0;
		R8_XT32K_TUNE = (R16_RTC_CNT_32K > 0x3fff) ? (R8_XT32K_TUNE & 0xfc) | 0x01 : R8_XT32K_TUNE;
		R8_XT32M_TUNE = (R8_XT32M_TUNE & 0xfc) | 0x03;
	);

	NVIC->SCTLR |= (1 << 2); //deep sleep

	SYS_SAFE_ACCESS(
		R8_SLP_POWER_CTRL |= RB_RAM_RET_LV;
		R16_POWER_PLAN = RB_PWR_PLAN_EN | RB_PWR_CORE | power_plan;
		R8_PLL_CONFIG |= (1 << 5);
	);

	NVIC->SCTLR &= ~(1 << 3); // wfi
	asm volatile ("wfi\nnop\nnop" );

	SYS_SAFE_ACCESS(
		R16_POWER_PLAN &= ~RB_XT_PRE_EN;
		R8_PLL_CONFIG &= ~(1 << 5);
		R8_XT32M_TUNE = (R8_XT32M_TUNE & 0xfc) | 0x01;
	);
}

RV_STATIC_INLINE void LowPower(uint32_t time, uint16_t power_plan) {
	uint32_t time_sleep, time_curr;
	
	if (time <= WAKE_UP_RTC_MAX_TIME) {
		time = time + (RTC_MAX_COUNT - WAKE_UP_RTC_MAX_TIME);
	}
	else {
		time = time - WAKE_UP_RTC_MAX_TIME;
	}

	time_curr = R32_RTC_CNT_32K;
	if (time < time_curr) {
		time_sleep = time + (RTC_MAX_COUNT - time_curr);
	}
	else {
		time_sleep = time - time_curr;
	}
	
	if ((SLEEP_RTC_MIN_TIME < time_sleep) && (time_sleep < SLEEP_RTC_MAX_TIME)) {
		LowPowerSleep( time, power_plan );
	}
	else {
		LowPowerIdle( time );
	}
	
	RTCInit(); // after sleeping this is necessary, but resets the RTC to 0 so don't use it to keep wall time
}

#define HardFault_IRQn        EXC_IRQn

/* Standard Peripheral Library old definitions (maintained for legacy purpose) */
#define HSI_Value             HSI_VALUE
#define HSE_Value             HSE_VALUE
#define HSEStartUp_TimeOut    HSE_STARTUP_TIMEOUT

#ifdef __cplusplus
}
#endif

#endif // Header guard

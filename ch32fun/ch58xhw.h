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
	TMR0_IRQn = 16,          /* TMR0 */
	GPIOA_IRQn = 17,         /* GPIOA */
	GPIOB_IRQn = 18,         /* GPIOB */
	SPI0_IRQn = 19,          /* SPI0 */
	BB_IRQn = 20,            /* BLEB */
	LLE_IRQn = 21,           /* BLEL */
	USB_IRQn = 22,           /* USB */
	USB2_IRQn = 23,          /* USB2 */
	TMR1_IRQn = 24,          /* TMR1 */
	TMR2_IRQn = 25,          /* TMR2 */
	UART0_IRQn = 26,         /* UART0 */
	UART1_IRQn = 27,         /* UART1 */
	RTC_IRQn = 28,           /* RTC */
	ADC_IRQn = 29,           /* CMP */
	I2C_IRQn = 30,           /* I2C */
	PWMX_IRQn = 31,          /* PWMX */
	TMR3_IRQn = 32,          /* TMR3 */
	UART2_IRQn = 33,         /* UART2 / KEYSCAN */
	UART3_IRQn = 34,         /* UART3 / ENCODER */
	WDOG_BAT_IRQn = 35,      /* WDOG_BAT */
} IRQn_Type;

#define BASE_VECTOR "\n\
	.balign  2\n\
	.option   push;\n\
	.option   norvc;\n\
	j handle_reset\n\
	.word   0\n\
	.word   NMI_Handler                /* NMI Handler */\n\
	.word   HardFault_Handler          /* Hard Fault Handler */\n\
	.word   0xF5F9BDA9\n\
	.word   Ecall_M_Mode_Handler       /* 5 */\n\
	.word   0\n\
	.word   0\n\
	.word   Ecall_U_Mode_Handler       /* 8 */\n\
	.word   Break_Point_Handler        /* 9 */\n\
	.word   0\n\
	.word   0\n\
	.word   SysTick_Handler            /* SysTick Handler */\n\
	.word   0\n\
	.word   SW_Handler                 /* SW Handler */\n\
	.word   0\n\
	/* External Interrupts */\n\
	.word   TMR0_IRQHandler            /* 16: TMR0 */\n\
	.word   GPIOA_IRQHandler           /* GPIOA */\n\
	.word   GPIOB_IRQHandler           /* GPIOB */\n\
	.word   SPI0_IRQHandler            /* SPI0 */\n\
	.word   BB_IRQHandler              /* BLEB */\n\
	.word   LLE_IRQHandler             /* BLEL */\n\
	.word   USB_IRQHandler             /* USB */\n\
	.word   USB2_IRQHandler            /* USB2 */\n\
	.word   TMR1_IRQHandler            /* TMR1 */\n\
	.word   TMR2_IRQHandler            /* TMR2 */\n\
	.word   UART0_IRQHandler           /* UART0 */\n\
	.word   UART1_IRQHandler           /* UART1 */\n\
	.word   RTC_IRQHandler             /* RTC */\n\
	.word   ADC_IRQHandler             /* ADC */\n\
	.word   I2C_IRQHandler             /* I2C */\n\
	.word   PWMX_IRQHandler            /* PWMX */\n\
	.word   TMR3_IRQHandler            /* TMR3 */\n\
	.word   UART2_IRQHandler           /* UART2 / KEYSCAN */\n\
	.word   UART3_IRQHandler           /* UART3 / ENCODER */\n\
	.word   WDOG_BAT_IRQHandler        /* WDOG_BAT */\n"

#define DEFAULT_INTERRUPT_VECTOR_CONTENTS BASE_VECTOR "\n.option pop;\n"

/* memory mapped structure for SysTick */
typedef struct __attribute__((packed))
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
	__IO uint8_t  VTFIDR[4];        // 50H (IDCFGR)
	uint8_t       RESERVED0[0x0C];  // 54H
	__IO uint32_t VTFADDR[4];       // 60H (FIADDRR)
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
#define NVIC_KEY2                  ((uint32_t)0xBCAF0000)
#define NVIC_KEY3                  ((uint32_t)0xBEEF0000)

#define SysTick                    ((SysTick_Type *) SysTick_BASE)
#define SysTick_LOAD_RELOAD_Msk    (0xFFFFFFFFFFFFFFFF)
#define SysTick_CTLR_INIT          (1 << 5)
#define SysTick_CTLR_MODE          (1 << 4)
#define SysTick_CTLR_STRE          (1 << 3)
#define SysTick_CTLR_STCLK         (1 << 2)
#define SysTick_CTLR_STIE          (1 << 1)
#define SysTick_CTLR_STE           (1 << 0)
#define SysTick_SR_CNTIF           (1 << 0)

typedef enum
{
	CLK_SOURCE_LSI = 0x00,
	CLK_SOURCE_LSE,

	CLK_SOURCE_HSE_16MHz = 0x22,
	CLK_SOURCE_HSE_8MHz = 0x24,
	CLK_SOURCE_HSE_6_4MHz = 0x25,
	CLK_SOURCE_HSE_4MHz = 0x28,
	CLK_SOURCE_HSE_2MHz = (0x20 | 16),
	CLK_SOURCE_HSE_1MHz = (0x20 | 0),

	CLK_SOURCE_PLL_80MHz = 0x46,
	CLK_SOURCE_PLL_60MHz = 0x48,
	CLK_SOURCE_PLL_48MHz = (0x40 | 10),
	CLK_SOURCE_PLL_40MHz = (0x40 | 12),
	CLK_SOURCE_PLL_36_9MHz = (0x40 | 13),
	CLK_SOURCE_PLL_32MHz = (0x40 | 15),
	CLK_SOURCE_PLL_30MHz = (0x40 | 16),
	CLK_SOURCE_PLL_24MHz = (0x40 | 20),
	CLK_SOURCE_PLL_20MHz = (0x40 | 24),
	CLK_SOURCE_PLL_15MHz = (0x40 | 0),
} SYS_CLKTypeDef;

// For debug writing to the debug interface.
#define DMDATA0 			((vu32*)0xe0000380)
#define DMDATA1 			((vu32*)0xe0000384)
#define DMSTATUS_SENTINEL	((vu32*)0xe0000388)// Reads as 0x00000000 if debugger is attached.

/* System: clock configuration register */
#define R32_CLK_SYS_CFG     (*((vu32*)0x40001008))    // RWA, system clock configuration, SAM
#define R8_CLK_SYS_CFG      (*((vu8*)0x40001008))     // RWA, system clock configuration, SAM
#define  RB_CLK_SYS_MOD     0xC0                      // RWA, system clock source mode: 00/10=divided from 32MHz, 01=divided from PLL-600MHz,11=directly from LSI
#define  RB_CLK_PLL_DIV     0x1F                      // RWA, output clock divider from PLL or CK32M
#define  RB_TX_32M_PWR_EN   0x40000                   // RWA, extern 32MHz HSE power contorl
#define  RB_PLL_PWR_EN      0x100000                  // RWA, PLL power control
#define R8_HFCK_PWR_CTRL    (*((vu8*)0x4000100A))     // RWA, power configuration for system high clock, SAM
#define  RB_CLK_PLL_PON     0x10                      // RWA, PLL power control
#define  RB_CLK_XT32M_KEEP  0x08                      // RWA, RWA, disable auto closing when in halt mode
#define  RB_CLK_XT32M_PON   0x04                      // RWA, extern 32MHz HSE power contorl


/* System: safe accessing register */
#define R32_SAFE_ACCESS     (*((vu32*)0x40001040)) // RW, safe accessing
#define R8_SAFE_ACCESS_SIG  (*((vu8*)0x40001040))  // WO, safe accessing sign register, must write SAFE_ACCESS_SIG1 then SAFE_ACCESS_SIG2 to enter safe accessing mode
#define  RB_SAFE_ACC_MODE   0x03                      // RO, current safe accessing mode: 11=safe/unlocked (SAM), other=locked (00..01..10..11)
#define  RB_SAFE_ACC_ACT    0x04                      // RO, indicate safe accessing on
#define  RB_SAFE_ACC_TIMER  0xF8                      // RO, safe accessing mode closed
#define SAFE_ACCESS_SIG1    0x57                      // WO: safe accessing sign value step 1
#define SAFE_ACCESS_SIG2    0xA8                      // WO: safe accessing sign value step 2
#define SAFE_ACCESS_SIG0    0x00                      // WO: safe accessing sign value for disable
#define R8_CHIP_ID          (*((vu8*)0x40001041))  // RF, chip ID register, always is ID_CH58*
#define SYS_SAFE_ACCESS(a)  do { R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1; \
								 R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2; \
								 asm volatile ("nop\nnop"); \
								 {a} \
								 R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG0; \
								 asm volatile ("nop\nnop"); } while(0)

/*System: Miscellaneous Control register */
#define R32_MISC_CTRL       (*((vu32*)0x40001048)) // RWA, miscellaneous control register
#define R8_PLL_CONFIG       (*((vu8*)0x4000104B))  // RWA, PLL configuration control, SAM
#define  RB_PLL_CFG_DAT     0x7F                      // RWA, PLL configuration control, SAM

/* System: 32MHz oscillator control register */
#define R8_XT32M_TUNE       (*((vu8*)0x4000104E))  // RWA, external 32MHz oscillator tune control, SAM
#define  RB_XT32M_C_LOAD    0x70                      // RWA, external 32MHz oscillator load capacitor tune: Cap = RB_XT32M_C_LOAD * 2 + 10pF
#define  RB_XT32M_I_BIAS    0x03                      // RWA, external 32MHz oscillator bias current tune: 00=75% current, 01=standard current, 10=125% current, 11=150% current

/* System: Flash ROM control register */
#define R32_FLASH_DATA      (*((vu32*)0x40001800)) // RO/WO, flash ROM data
#define R32_FLASH_CONTROL   (*((vu32*)0x40001804)) // RW, flash ROM control, byte1 and byte3 need RWA
#define R8_FLASH_DATA       (*((vu8*)0x40001804))  // RO/WO, flash ROM data buffer
#define R8_FLASH_SCK        (*((vu8*)0x40001805))  // RW, flash ROM sck time config
#define R8_FLASH_CTRL       (*((vu8*)0x40001806))  // RW, flash ROM access control
#define R8_FLASH_CFG        (*((vu8*)0x40001807))  // RWA, flash ROM access config, SAM

/* GPIO PA register */
#define R32_PA_DIR          (*((vu32*)0x400010A0)) // RW, GPIO PA I/O direction: 0=in, 1=out
#define R32_PA_PIN          (*((vu32*)0x400010A4)) // RO, GPIO PA input
#define R32_PA_OUT          (*((vu32*)0x400010A8)) // RW, GPIO PA output
#define R32_PA_CLR          (*((vu32*)0x400010AC)) // WZ, GPIO PA clear output: 0=keep, 1=clear
#define R32_PA_PU           (*((vu32*)0x400010B0)) // RW, GPIO PA pullup resistance enable
#define R32_PA_PD_DRV       (*((vu32*)0x400010B4)) // RW, PA pulldown for input or PA driving capability for output
#define R32_PA_SET          (*((vu32*)0x400010B8)) // RW, PA set high for output ,1=set output high,0=IDLE

/* GPIO PB register */
#define R32_PB_DIR          (*((vu32*)0x400010C0)) // RW, GPIO PB I/O direction: 0=in, 1=out
#define R32_PB_PIN          (*((vu32*)0x400010C4)) // RO, GPIO PB input
#define R32_PB_OUT          (*((vu32*)0x400010C8)) // RW, GPIO PB output
#define R32_PB_CLR          (*((vu32*)0x400010CC)) // WZ, GPIO PB clear output: 0=keep, 1=clear
#define R32_PB_PU           (*((vu32*)0x400010D0)) // RW, GPIO PB pullup resistance enable
#define R32_PB_PD_DRV       (*((vu32*)0x400010D4)) // RW, PB pulldown for input or PB driving capability for output
#define R32_PB_SET          (*((vu32*)0x400010D8)) // RW, PB set high for output ,1=set output high,0=IDLE

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

#define HardFault_IRQn        EXC_IRQn

/* Standard Peripheral Library old definitions (maintained for legacy purpose) */
#define HSI_Value             HSI_VALUE
#define HSE_Value             HSE_VALUE
#define HSEStartUp_TimeOut    HSE_STARTUP_TIMEOUT

#ifdef __cplusplus
}
#endif

#endif // Header guard

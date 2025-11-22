/* This file supports the CH32V00X and CH32M00X family of chips
*/

#ifndef TODO_CH32X00X_H
#define TODO_CH32X00X_H

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
    SysTick_IRQn = 12,       /* 12 System timer Interrupt                            */
    Software_IRQn = 14,      /* 14 software Interrupt                                */

    /******  RISC-V specific Interrupt Numbers *********************************************************/
    WWDG_IRQn = 16,          /* Window WatchDog Interrupt                            */
    PVD_IRQn = 17,           /* PVD through EXTI Line detection Interrupt            */
    FLASH_IRQn = 18,         /* FLASH global Interrupt                               */
    RCC_IRQn = 19,           /* RCC global Interrupt                                 */
    EXTI7_0_IRQn = 20,       /* External Line[7:0] Interrupts                        */
    AWU_IRQn = 21,           /* AWU global Interrupt                                 */
    DMA1_Channel1_IRQn = 22, /* DMA1 Channel 1 global Interrupt                      */
    DMA1_Channel2_IRQn = 23, /* DMA1 Channel 2 global Interrupt                      */
    DMA1_Channel3_IRQn = 24, /* DMA1 Channel 3 global Interrupt                      */
    DMA1_Channel4_IRQn = 25, /* DMA1 Channel 4 global Interrupt                      */
    DMA1_Channel5_IRQn = 26, /* DMA1 Channel 5 global Interrupt                      */
    DMA1_Channel6_IRQn = 27, /* DMA1 Channel 6 global Interrupt                      */
    DMA1_Channel7_IRQn = 28, /* DMA1 Channel 7 global Interrupt                      */
    ADC_IRQn = 29,           /* ADC global Interrupt                                 */
    I2C1_EV_IRQn = 30,       /* I2C1 Event Interrupt                                 */
    I2C1_ER_IRQn = 31,       /* I2C1 Error Interrupt                                 */
    USART1_IRQn = 32,        /* USART1 global Interrupt                              */
    SPI1_IRQn = 33,          /* SPI1 global Interrupt                                */
    TIM1_BRK_IRQn = 34,      /* TIM1 Break Interrupt                                 */
    TIM1_UP_IRQn = 35,       /* TIM1 Update Interrupt                                */
    TIM1_TRG_COM_IRQn = 36,  /* TIM1 Trigger and Commutation Interrupt               */
    TIM1_CC_IRQn = 37,       /* TIM1 Capture Compare Interrupt                       */
    TIM2_IRQn = 38,          /* TIM2 global Interrupt                                */

#if defined(CH32V005) || defined(CH32V006) || defined(CH32V007_M007)
    USART2_IRQn = 39,        /* USART2 global Interrupt                              */
    OPCM_IRQn = 40,          /* OPCM global Interrupt                                */
#endif
} IRQn_Type;

#define DEFAULT_INTERRUPT_VECTOR_CONTENTS "\n\
	.align  2\n\
	.option   push;\n\
	.option   norvc;\n\
	j handle_reset\n\
	.word   0\n\
	.word   NMI_Handler               /* NMI Handler */                    \n\
	.word   HardFault_Handler         /* Hard Fault Handler */             \n\
	.word   0\n\
	.word   0\n\
	.word   0\n\
	.word   0\n\
	.word   0\n\
	.word   0\n\
	.word   0\n\
	.word   0\n\
	.word   SysTick_Handler           /* SysTick Handler */                \n\
	.word   0\n\
	.word   SW_Handler                /* SW Handler */                     \n\
	.word   0\n\
	.word   WWDG_IRQHandler           /* Window Watchdog */                \n\
	.word   PVD_IRQHandler            /* PVD through EXTI Line detect */   \n\
	.word   FLASH_IRQHandler          /* Flash */                          \n\
	.word   RCC_IRQHandler            /* RCC */                            \n\
	.word   EXTI7_0_IRQHandler        /* EXTI Line 7..0 */                 \n\
	.word   AWU_IRQHandler            /* AWU */                            \n\
	.word   DMA1_Channel1_IRQHandler  /* DMA1 Channel 1 */                 \n\
	.word   DMA1_Channel2_IRQHandler  /* DMA1 Channel 2 */                 \n\
	.word   DMA1_Channel3_IRQHandler  /* DMA1 Channel 3 */                 \n\
	.word   DMA1_Channel4_IRQHandler  /* DMA1 Channel 4 */                 \n\
	.word   DMA1_Channel5_IRQHandler  /* DMA1 Channel 5 */                 \n\
	.word   DMA1_Channel6_IRQHandler  /* DMA1 Channel 6 */                 \n\
	.word   DMA1_Channel7_IRQHandler  /* DMA1 Channel 7 */                 \n\
	.word   ADC1_IRQHandler           /* ADC1 */                           \n\
	.word   I2C1_EV_IRQHandler        /* I2C1 Event */                     \n\
	.word   I2C1_ER_IRQHandler        /* I2C1 Error */                     \n\
	.word   USART1_IRQHandler         /* USART1 */                         \n\
	.word   SPI1_IRQHandler           /* SPI1 */                           \n\
	.word   TIM1_BRK_IRQHandler       /* TIM1 Break */                     \n\
	.word   TIM1_UP_IRQHandler        /* TIM1 Update */                    \n\
	.word   TIM1_TRG_COM_IRQHandler   /* TIM1 Trigger and Commutation */   \n\
	.word   TIM1_CC_IRQHandler        /* TIM1 Capture Compare */           \n\
	.word   TIM2_IRQHandler           /* TIM2 */                           \n\
	.option   pop;\n"


/* memory mapped structure for SysTick */
typedef struct
{
    __IO uint32_t CTLR;
    __IO uint32_t SR;
    __IO uint32_t CNT;
    uint32_t RESERVED0;
    __IO uint32_t CMP;
    uint32_t RESERVED1;
} SysTick_Type;


#endif /* __ASSEMBLER__*/

#define HardFault_IRQn    EXC_IRQn


/* Standard Peripheral Library old definitions (maintained for legacy purpose) */
#define HSI_Value             HSI_VALUE
#define HSE_Value             HSE_VALUE
#define HSEStartUp_TimeOut    HSE_STARTUP_TIMEOUT

// Datasheet recommends HSE of 24M
#ifndef HSE_VALUE
#define HSE_VALUE 24000000
#endif

#ifndef __ASSEMBLER__
/* Analog to Digital Converter */
typedef struct
{
    __IO uint32_t STATR;
    __IO uint32_t CTLR1;
    __IO uint32_t CTLR2;
    __IO uint32_t SAMPTR1;
    __IO uint32_t SAMPTR2;
    __IO uint32_t IOFR1;
    __IO uint32_t IOFR2;
    __IO uint32_t IOFR3;
    __IO uint32_t IOFR4;
    __IO uint32_t WDHTR;
    __IO uint32_t WDLTR;
    __IO uint32_t RSQR1;
    __IO uint32_t RSQR2;
    __IO uint32_t RSQR3;
    __IO uint32_t ISQR;
    __IO uint32_t IDATAR1;
    __IO uint32_t IDATAR2;
    __IO uint32_t IDATAR3;
    __IO uint32_t IDATAR4;
    __IO uint32_t RDATAR;
    __IO uint32_t CTLR3;
    __IO uint32_t WDTR1;
    __IO uint32_t WDTR2;
} ADC_TypeDef;

/* DMA Controller */
typedef struct
{
    __IO uint32_t CFGR;
    __IO uint32_t CNTR;
    __IO uint32_t PADDR;
    __IO uint32_t MADDR;
} DMA_Channel_TypeDef;

typedef struct
{
    __IO uint32_t INTFR;
    __IO uint32_t INTFCR;
} DMA_TypeDef;

/* External Interrupt/Event Controller */
typedef struct
{
    __IO uint32_t INTENR;
    __IO uint32_t EVENR;
    __IO uint32_t RTENR;
    __IO uint32_t FTENR;
    __IO uint32_t SWIEVR;
    __IO uint32_t INTFR;
} EXTI_TypeDef;

/* FLASH Registers */
typedef struct
{
    __IO uint32_t ACTLR;
    __IO uint32_t KEYR;
    __IO uint32_t OBKEYR;
    __IO uint32_t STATR;
    __IO uint32_t CTLR;
    __IO uint32_t ADDR;
    __IO uint32_t RESERVED;
    __IO uint32_t OBR;
    __IO uint32_t WPR;
    __IO uint32_t MODEKEYR;
    __IO uint32_t BOOT_MODEKEYR;
} FLASH_TypeDef;

/* Option Bytes Registers */
typedef struct
{
    __IO uint16_t RDPR;
    __IO uint16_t USER;
    __IO uint16_t Data0;
    __IO uint16_t Data1;
    __IO uint16_t WRPR0;
    __IO uint16_t WRPR1;
    __IO uint16_t WRPR2;
    __IO uint16_t WRPR3;
} OB_TypeDef;

typedef struct
{
	__IO uint16_t FLACAP;
	__IO uint16_t RES1;
	__IO uint32_t RES2;
	__IO uint32_t UNIID1;
	__IO uint32_t UNIID2;
	__IO uint32_t UNIID3;
} ESIG_TypeDef;

typedef struct
{
    union
    {   
        __I uint32_t CHIPID;
        struct 
        {
            __I uint16_t REVID;
	        __I uint16_t DEVID;
        };
    };
} INFO_TypeDef;


/* General Purpose I/O */
typedef enum
{
	GPIO_CFGLR_MASK      = 0b1111,

	GPIO_CFGLR_IN_ANALOG = 0b0000,
	GPIO_CFGLR_IN_FLOAT  = 0b0100,
	GPIO_CFGLR_IN_PUPD   = 0b1000,
	GPIO_CFGLR_RES1      = 0b1100,
	GPIO_CFGLR_OUT_PP    = 0b0001,
	GPIO_CFGLR_OUT_OD    = 0b0101,
	GPIO_CFGLR_OUT_AF_PP = 0b1001,
	GPIO_CFGLR_OUT_AF_OD = 0b1101,

	// For intercompatibility with 003 legacy code.
	GPIO_CFGLR_OUT_10Mhz_PP = 0b0001,
	GPIO_CFGLR_OUT_2Mhz_PP = 0b0001,
	GPIO_CFGLR_OUT_50Mhz_PP = 0b0001,
	GPIO_CFGLR_OUT_10Mhz_OD = 0b0101,
	GPIO_CFGLR_OUT_2Mhz_OD = 0b0101,
	GPIO_CFGLR_OUT_50Mhz_OD = 0b0101,
	GPIO_CFGLR_OUT_10Mhz_AF_PP = 0b1001,
	GPIO_CFGLR_OUT_2Mhz_AF_PP = 0b1001,
	GPIO_CFGLR_OUT_50Mhz_AF_PP = 0b1001,
	GPIO_CFGLR_OUT_10Mhz_AF_OD = 0b1101,
	GPIO_CFGLR_OUT_2Mhz_AF_OD = 0b1101,
	GPIO_CFGLR_OUT_50Mhz_AF_OD = 0b1101,
} GPIO_CFGLR_PIN_MODE_Typedef;

/*  This was correct in the 003, but the 00X have 0b0010 as a reserved bit in this field.  The above enum never sets this bit as directed by the RM.  But it's included in the following structure as a 4 bit field becaus changing it to two disjoint bit fields would make life too painful.  */

typedef union {
	uint32_t __FULL;
	struct {
		GPIO_CFGLR_PIN_MODE_Typedef PIN0 :4;
		GPIO_CFGLR_PIN_MODE_Typedef PIN1 :4;
		GPIO_CFGLR_PIN_MODE_Typedef PIN2 :4;
		GPIO_CFGLR_PIN_MODE_Typedef PIN3 :4;
		GPIO_CFGLR_PIN_MODE_Typedef PIN4 :4;
		GPIO_CFGLR_PIN_MODE_Typedef PIN5 :4;
		GPIO_CFGLR_PIN_MODE_Typedef PIN6 :4;
		GPIO_CFGLR_PIN_MODE_Typedef PIN7 :4;
	};
} GPIO_CFGLR_t;

typedef union {
	uint32_t __FULL;
	const struct {
		uint32_t IDR0 :1;
		uint32_t IDR1 :1;
		uint32_t IDR2 :1;
		uint32_t IDR3 :1;
		uint32_t IDR4 :1;
		uint32_t IDR5 :1;
		uint32_t IDR6 :1;
		uint32_t IDR7 :1;
		uint32_t :24;
	};
} GPIO_INDR_t;
typedef union {
	uint32_t __FULL;
	struct {
		uint32_t ODR0 :1;
		uint32_t ODR1 :1;
		uint32_t ODR2 :1;
		uint32_t ODR3 :1;
		uint32_t ODR4 :1;
		uint32_t ODR5 :1;
		uint32_t ODR6 :1;
		uint32_t ODR7 :1;
		uint32_t :24;
	};
} GPIO_OUTDR_t;
typedef union {
	uint32_t __FULL;
	struct {
		uint32_t BS0 :1;
		uint32_t BS1 :1;
		uint32_t BS2 :1;
		uint32_t BS3 :1;
		uint32_t BS4 :1;
		uint32_t BS5 :1;
		uint32_t BS6 :1;
		uint32_t BS7 :1;
		uint32_t :8;
		uint32_t BR0 :1;
		uint32_t BR1 :1;
		uint32_t BR2 :1;
		uint32_t BR3 :1;
		uint32_t BR4 :1;
		uint32_t BR5 :1;
		uint32_t BR6 :1;
		uint32_t BR7 :1;
		uint32_t :8;
	};
} GPIO_BSHR_t;
typedef union {
	uint32_t __FULL;
	struct {
		uint32_t BR0 :1;
		uint32_t BR1 :1;
		uint32_t BR2 :1;
		uint32_t BR3 :1;
		uint32_t BR4 :1;
		uint32_t BR5 :1;
		uint32_t BR6 :1;
		uint32_t BR7 :1;
		uint32_t :24;
	};
} GPIO_BCR_t;
typedef union {
	uint32_t __FULL;
	struct {
		uint32_t LCK0 :1;
		uint32_t LCK1 :1;
		uint32_t LCK2 :1;
		uint32_t LCK3 :1;
		uint32_t LCK4 :1;
		uint32_t LCK5 :1;
		uint32_t LCK6 :1;
		uint32_t LCK7 :1;
		uint32_t LCKK :1;
		uint32_t :23;
	};
} GPIO_LCKR_t;
typedef struct
{
	__IO uint32_t CFGLR;
	__IO uint32_t CFGHR;
	__I  uint32_t INDR;
	__IO uint32_t OUTDR;
	__IO uint32_t BSHR;
	__IO uint32_t BCR;
	__IO uint32_t LCKR;
} GPIO_TypeDef;

#define DYN_GPIO_READ(gpio, field) ((GPIO_##field##_t) { .__FULL = gpio->field })
#define DYN_GPIO_WRITE(gpio, field, ...) gpio->field = ((const GPIO_##field##_t) __VA_ARGS__).__FULL
#define DYN_GPIO_MOD(gpio, field, reg, val) {GPIO_##field##_t tmp; tmp.__FULL = gpio->field; tmp.reg = val; gpio->field = tmp.__FULL;}

/* Alternate Function I/O */
typedef struct
{
    uint32_t RESERVED0;
    uint32_t RESERVED1;
    __IO uint32_t EXTICR;
    __IO uint32_t PCFR1;
} AFIO_TypeDef;

/* Inter Integrated Circuit Interface */
typedef struct
{
    __IO uint16_t CTLR1;
    uint16_t      RESERVED0;
    __IO uint16_t CTLR2;
    uint16_t      RESERVED1;
    __IO uint16_t OADDR1;
    uint16_t      RESERVED2;
    __IO uint16_t OADDR2;
    uint16_t      RESERVED3;
    __IO uint16_t DATAR;
    uint16_t      RESERVED4;
    __IO uint16_t STAR1;
    uint16_t      RESERVED5;
    __IO uint16_t STAR2;
    uint16_t      RESERVED6;
    __IO uint16_t CKCFGR;
    uint16_t      RESERVED7;
} I2C_TypeDef;

/* Independent WatchDog */
typedef struct
{
    __IO uint32_t CTLR;
    __IO uint32_t PSCR;
    __IO uint32_t RLDR;
    __IO uint32_t STATR;
} IWDG_TypeDef;

/* Power Control */
typedef struct
{
    __IO uint32_t CTLR;
    __IO uint32_t CSR;
    __IO uint32_t AWUCSR;
    __IO uint32_t AWUWR;
    __IO uint32_t AWUPSC;
} PWR_TypeDef;

/* Reset and Clock Control */
typedef struct
{
    __IO uint32_t CTLR;
    __IO uint32_t CFGR0;
    __IO uint32_t INTR;
	union
	{
	    __IO uint32_t PB2PRSTR;
	    __IO uint32_t APB2PRSTR; // For 003 compatibility
	};
    __IO uint32_t PB1PRSTR;
	union
	{
	    __IO uint32_t HBPCENR;
	    __IO uint32_t AHBPCENR; // For 003 compatibility
	};
	union
	{
	    __IO uint32_t PB2PCENR;
	    __IO uint32_t APB2PCENR; // For 003 compatibility
	};
	union
	{
	    __IO uint32_t PB1PCENR;
	    __IO uint32_t APB1PCENR; // For 003 compatibility
	};
    __IO uint32_t RESERVED0;
    __IO uint32_t RSTSCKR;
} RCC_TypeDef;



/* Serial Peripheral Interface */
typedef struct
{
    __IO uint16_t CTLR1;
    uint16_t      RESERVED0;
    __IO uint16_t CTLR2;
    uint16_t      RESERVED1;
    __IO uint16_t STATR;
    uint16_t      RESERVED2;
    __IO uint16_t DATAR;
    uint16_t      RESERVED3;
    __IO uint16_t CRCR;
    uint16_t      RESERVED4;
    __IO uint16_t RCRCR;
    uint16_t      RESERVED5;
    __IO uint16_t TCRCR;
    uint16_t      RESERVED6;
    uint32_t      RESERVED7;
    uint32_t      RESERVED8;
    __IO uint16_t HSCR;
    uint16_t      RESERVED9;
} SPI_TypeDef;

/* ADTM - Advanced-Control Timer */
typedef struct
{
    __IO uint16_t CTLR1;
    uint16_t      RESERVED0;
    __IO uint16_t CTLR2;
    uint16_t      RESERVED1;
    __IO uint16_t SMCFGR;
    uint16_t      RESERVED2;
    __IO uint16_t DMAINTENR;
    uint16_t      RESERVED3;
    __IO uint16_t INTFR;
    uint16_t      RESERVED4;
    __IO uint16_t SWEVGR;
    uint16_t      RESERVED5;
    __IO uint16_t CHCTLR1;
    uint16_t      RESERVED6;
    __IO uint16_t CHCTLR2;
    uint16_t      RESERVED7;
    __IO uint16_t CCER;
    uint16_t      RESERVED8;
    __IO uint16_t CNT;
    uint16_t      RESERVED9;
    __IO uint16_t PSC;
    uint16_t      RESERVED10;
    __IO uint16_t ATRLR;
    uint16_t      RESERVED11;
    __IO uint16_t RPTCR;
    uint16_t      RESERVED12;
    __IO uint32_t CH1CVR;
    __IO uint32_t CH2CVR;
    __IO uint32_t CH3CVR;
    __IO uint32_t CH4CVR;
    __IO uint16_t BDTR;
    uint16_t      RESERVED13;
    __IO uint16_t DMACFGR;
    uint16_t      RESERVED14;
    __IO uint16_t DMAADR;
    uint16_t      RESERVED15;
} ADTM_TypeDef;

/* GPTM - General-Purpose Timer */
typedef struct
{
    __IO uint16_t CTLR1;
    uint16_t      RESERVED0;
    __IO uint16_t CTLR2;
    uint16_t      RESERVED1;
    __IO uint16_t SMCFGR;
    uint16_t      RESERVED2;
    __IO uint16_t DMAINTENR;
    uint16_t      RESERVED3;
    __IO uint16_t INTFR;
    uint16_t      RESERVED4;
    __IO uint16_t SWEVGR;
    uint16_t      RESERVED5;
    __IO uint16_t CHCTLR1;
    uint16_t      RESERVED6;
    __IO uint16_t CHCTLR2;
    uint16_t      RESERVED7;
    __IO uint16_t CCER;
    uint16_t      RESERVED8;
    __IO uint16_t CNT;
    uint16_t      RESERVED9;
    __IO uint16_t PSC;
    uint16_t      RESERVED10;
    __IO uint16_t ATRLR;
    uint16_t      RESERVED11;
    uint16_t      RESERVED12;
    uint16_t      RESERVED13;
    __IO uint32_t CH1CVR;
    __IO uint32_t CH2CVR;
    __IO uint32_t CH3CVR;
    __IO uint32_t CH4CVR;
    __IO uint16_t DTCR;
    uint16_t      RESERVED14;
    __IO uint16_t DMACFGR;
    uint16_t      RESERVED15;
    __IO uint16_t DMAADR;
    uint16_t      RESERVED16;
} GPTM_TypeDef;

/* SLTM - Streamlined Timer */
typedef struct
{
    __IO uint16_t CTLR1;
    uint16_t      RESERVED0;
    __IO uint16_t DMAINTENR;
    uint16_t      RESERVED2;
    __IO uint16_t CNT;
    uint16_t      RESERVED3;
    __IO uint16_t ATRLR;
    uint16_t      RESERVED4;
    __IO uint16_t CH1CVR;
    uint16_t      RESERVED5;
    __IO uint16_t CH2CVR;
    uint16_t      RESERVED6;
    __IO uint16_t CH3CVR;
    uint16_t      RESERVED7;
    __IO uint16_t CH4CVR;
    uint16_t      RESERVED8;
} SLTM_TypeDef;

/* Universal Synchronous Asynchronous Receiver Transmitter */
typedef struct
{
    __IO uint32_t STATR;
    __IO uint32_t DATAR;
    __IO uint32_t BRR;
    __IO uint32_t CTLR1;
    __IO uint32_t CTLR2;
    __IO uint32_t CTLR3;
    __IO uint32_t GPR;
} USART_TypeDef;

/* Window WatchDog */
typedef struct
{
    __IO uint16_t CTLR;
		uint16_t      RESERVED1;
    __IO uint16_t CFGR;
		uint16_t      RESERVED2;
    __IO uint16_t STATR;
		uint16_t      RESERVED3;
} WWDG_TypeDef;

/* Enhanced Registers */
typedef struct
{
    __IO uint32_t EXTEN_CTR;
} EXTEN_TypeDef;

/* The reference manual for the ch32v2xx/v3xx reference this as "CTR" field in the "EXTEND" register so adding an alias here. */
typedef struct
{
    __IO uint32_t CTR;
} EXTEND_TypeDef;



/* Operational Amplifier and Comparator */
typedef struct
{
    __IO uint32_t CFGR1;
    __IO uint32_t CTLR1;
    __IO uint32_t CFGR2;
    __IO uint32_t CTLR2;
    __IO uint32_t OPA_KEY;
    __IO uint32_t CMP_KEY;
    __IO uint32_t POLL_KEY;
} OPA_TypeDef;




#endif

/* Peripheral memory map */
#ifdef __ASSEMBLER__
#define FLASH_BASE                              (0x08000000) /* FLASH base address in the alias region */
#define SRAM_BASE                               (0x20000000) /* SRAM base address in the alias region */
#define PERIPH_BASE                             (0x40000000) /* Peripheral base address in the alias region */
#define CORE_PERIPH_BASE                        (0xE0000000) /* System peripherals base address in the alias region */
#else
#define FLASH_BASE                              ((uint32_t)0x08000000) /* FLASH base address in the alias region */
#define SRAM_BASE                               ((uint32_t)0x20000000) /* SRAM base address in the alias region */
#define PERIPH_BASE                             ((uint32_t)0x40000000) /* Peripheral base address in the alias region */
#define CORE_PERIPH_BASE                        ((uint32_t)0xE0000000) /* System peripherals base address in the alias region */
#endif


#define APB1PERIPH_BASE                         (PERIPH_BASE)
#define APB2PERIPH_BASE                         (PERIPH_BASE + 0x10000)
#define AHBPERIPH_BASE                          (PERIPH_BASE + 0x20000)

#define TIM2_BASE                               (APB1PERIPH_BASE + 0x0000)
#define TIM3_BASE                               (APB1PERIPH_BASE + 0x0800)
#define WWDG_BASE                               (APB1PERIPH_BASE + 0x2C00)
#define IWDG_BASE                               (APB1PERIPH_BASE + 0x3000)
#define USART2_BASE                             (APB1PERIPH_BASE + 0x4400)
#define I2C1_BASE                               (APB1PERIPH_BASE + 0x5400)
#define PWR_BASE                                (APB1PERIPH_BASE + 0x7000)

#define AFIO_BASE                               (APB2PERIPH_BASE + 0x0000)
#define EXTI_BASE                               (APB2PERIPH_BASE + 0x0400)
#define GPIOA_BASE                              (APB2PERIPH_BASE + 0x0800)
#define GPIOB_BASE                              (APB2PERIPH_BASE + 0x0C00)
#define GPIOC_BASE                              (APB2PERIPH_BASE + 0x1000)
#define GPIOD_BASE                              (APB2PERIPH_BASE + 0x1400)
#define ADC1_BASE                               (APB2PERIPH_BASE + 0x2400)
#define TIM1_BASE                               (APB2PERIPH_BASE + 0x2C00)
#define SPI1_BASE                               (APB2PERIPH_BASE + 0x3000)
#define USART1_BASE                             (APB2PERIPH_BASE + 0x3800)

#define DMA1_BASE                               (AHBPERIPH_BASE + 0x0000)
#define DMA1_Channel1_BASE                      (AHBPERIPH_BASE + 0x0008)
#define DMA1_Channel2_BASE                      (AHBPERIPH_BASE + 0x001C)
#define DMA1_Channel3_BASE                      (AHBPERIPH_BASE + 0x0030)
#define DMA1_Channel4_BASE                      (AHBPERIPH_BASE + 0x0044)
#define DMA1_Channel5_BASE                      (AHBPERIPH_BASE + 0x0058)
#define DMA1_Channel6_BASE                      (AHBPERIPH_BASE + 0x006C)
#define DMA1_Channel7_BASE                      (AHBPERIPH_BASE + 0x0080)
#define RCC_BASE                                (AHBPERIPH_BASE + 0x1000)

#define FLASH_R_BASE                            (AHBPERIPH_BASE + 0x2000) /* Flash registers base address */
#define EXTEN_BASE                              (AHBPERIPH_BASE + 0x3800)
#define OPA_BASE                                (AHBPERIPH_BASE + 0x4000)

#define OB_BASE                                 ((uint32_t)0x1FFFF800)    /* Flash Option Bytes base address */
#define ESIG_BASE                               ((uint32_t)0x1FFFF7E0)
#define INFO_BASE                               ((uint32_t)0x1FFFF704)


#define PFIC_BASE    (CORE_PERIPH_BASE + 0xE000)
#define SysTick_BASE    (CORE_PERIPH_BASE + 0xF000)

/* Peripheral declaration */
#define TIM2                                    ((GPTM_TypeDef *)TIM2_BASE)
#define TIM3                                    ((ALTM_TypeDef *)TIM3_BASE)
#define WWDG                                    ((WWDG_TypeDef *)WWDG_BASE)
#define IWDG                                    ((IWDG_TypeDef *)IWDG_BASE)
#define I2C1                                    ((I2C_TypeDef *)I2C1_BASE)
#define PWR                                     ((PWR_TypeDef *)PWR_BASE)

#define AFIO                                    ((AFIO_TypeDef *)AFIO_BASE)
#define EXTI                                    ((EXTI_TypeDef *)EXTI_BASE)
#define GPIOA                                   ((GPIO_TypeDef *)GPIOA_BASE)
#define GPIOB                                   ((GPIO_TypeDef *)GPIOB_BASE)
#define GPIOC                                   ((GPIO_TypeDef *)GPIOC_BASE)
#define GPIOD                                   ((GPIO_TypeDef *)GPIOD_BASE)
#define ADC1                                    ((ADC_TypeDef *)ADC1_BASE)
#define TIM1                                    ((ADTM_TypeDef *)TIM1_BASE)
#define SPI1                                    ((SPI_TypeDef *)SPI1_BASE)
#define USART1                                  ((USART_TypeDef *)USART1_BASE)
#define USART2                                  ((USART_TypeDef *)USART2_BASE)

#define DMA1                                    ((DMA_TypeDef *)DMA1_BASE)
#define DMA1_Channel1                           ((DMA_Channel_TypeDef *)DMA1_Channel1_BASE)
#define DMA1_Channel2                           ((DMA_Channel_TypeDef *)DMA1_Channel2_BASE)
#define DMA1_Channel3                           ((DMA_Channel_TypeDef *)DMA1_Channel3_BASE)
#define DMA1_Channel4                           ((DMA_Channel_TypeDef *)DMA1_Channel4_BASE)
#define DMA1_Channel5                           ((DMA_Channel_TypeDef *)DMA1_Channel5_BASE)
#define DMA1_Channel6                           ((DMA_Channel_TypeDef *)DMA1_Channel6_BASE)
#define DMA1_Channel7                           ((DMA_Channel_TypeDef *)DMA1_Channel7_BASE)
#define RCC                                     ((RCC_TypeDef *)RCC_BASE)
#define FLASH                                   ((FLASH_TypeDef *)FLASH_R_BASE)
#define OPA                                     ((OPA_TypeDef *)OPA_BASE)
#define OB                                      ((OB_TypeDef *)OB_BASE)
#define ESIG                                    ((ESIG_TypeDef *)ESIG_BASE)
// Mentioned in ch32v30x_dbgmcu.c, may not work on all processors.
#define INFO                                    ((INFO_TypeDef *)INFO_BASE) 
#define EXTEN                                   ((EXTEN_TypeDef *)EXTEN_BASE)
#define EXTEND                                  ((EXTEND_TypeDef *)EXTEN_BASE)  // Alias to EXTEN




/******************************************************************************/
/*                         Peripheral Registers Bits Definition               */
/******************************************************************************/

/******************************************************************************/
/*                        Analog to Digital Converter                         */
/******************************************************************************/

/********************  Bit definition for ADC_STATR register  ********************/
#define ADC_AWD                                 ((uint32_t)0x00000001) /* Analog watchdog flag */
#define ADC_EOC                                 ((uint32_t)0x00000002) /* End of conversion */
#define ADC_JEOC                                ((uint32_t)0x00000004) /* Injected channel end of conversion */
#define ADC_JSTRT                               ((uint32_t)0x00000008) /* Injected channel Start flag */
#define ADC_STRT                                ((uint32_t)0x00000010) /* Regular channel Start flag */

/*******************  Bit definition for ADC_CTLR1 register  ********************/
#define ADC_AWDCH                               ((uint32_t)0x0000001F) /* AWDCH[4:0] bits (Analog watchdog channel select bits) */
#define ADC_AWDCH_0                             ((uint32_t)0x00000001) /* Bit 0 */
#define ADC_AWDCH_1                             ((uint32_t)0x00000002) /* Bit 1 */
#define ADC_AWDCH_2                             ((uint32_t)0x00000004) /* Bit 2 */
#define ADC_AWDCH_3                             ((uint32_t)0x00000008) /* Bit 3 */
#define ADC_AWDCH_4                             ((uint32_t)0x00000010) /* Bit 4 */

#define ADC_EOCIE                               ((uint32_t)0x00000020) /* Interrupt enable for EOC */
#define ADC_AWDIE                               ((uint32_t)0x00000040) /* Analog Watchdog interrupt enable */
#define ADC_JEOCIE                              ((uint32_t)0x00000080) /* Interrupt enable for injected channels */
#define ADC_SCAN                                ((uint32_t)0x00000100) /* Scan mode */
#define ADC_AWDSGL                              ((uint32_t)0x00000200) /* Enable the watchdog on a single channel in scan mode */
#define ADC_JAUTO                               ((uint32_t)0x00000400) /* Automatic injected group conversion */
#define ADC_DISCEN                              ((uint32_t)0x00000800) /* Discontinuous mode on regular channels */
#define ADC_JDISCEN                             ((uint32_t)0x00001000) /* Discontinuous mode on injected channels */

#define ADC_DISCNUM                             ((uint32_t)0x0000E000) /* DISCNUM[2:0] bits (Discontinuous mode channel count) */
#define ADC_DISCNUM_0                           ((uint32_t)0x00002000) /* Bit 0 */
#define ADC_DISCNUM_1                           ((uint32_t)0x00004000) /* Bit 1 */
#define ADC_DISCNUM_2                           ((uint32_t)0x00008000) /* Bit 2 */

#define ADC_JAWDEN                              ((uint32_t)0x00400000) /* Analog watchdog enable on injected channels */
#define ADC_AWDEN                               ((uint32_t)0x00800000) /* Analog watchdog enable on regular channels */
#define ADC_TKENABLE                            ((uint32_t)0x01000000) /* TKEY module enable control */
#define ADC_TKITUNE                             ((uint32_t)0x02000000) /* Touchkey current adjustment */
#define ADC_BUFEN                               ((uint32_t)0x04000000) /* ADC BUF enable */

/*******************  Bit definition for ADC_CTLR2 register  ********************/
#define ADC_ADON                                ((uint32_t)0x00000001) /* A/D Converter ON / OFF */
#define ADC_CONT                                ((uint32_t)0x00000002) /* Continuous Conversion */
#define ADC_TGREGU                              ((uint32_t)0x00000010) /* External trigger event of rule channel conversion */
#define ADC_TGINJE                              ((uint32_t)0x00000020) /* External trigger event of injection channel conversion */
#define ADC_DMA                                 ((uint32_t)0x00000100) /* Direct Memory access mode */
#define ADC_ALIGN                               ((uint32_t)0x00000800) /* Data Alignment */

#define ADC_JEXTSEL                             ((uint32_t)0x00007000) /* JEXTSEL[2:0] bits (External event select for injected group) */
#define ADC_JEXTSEL_0                           ((uint32_t)0x00001000) /* Bit 0 */
#define ADC_JEXTSEL_1                           ((uint32_t)0x00002000) /* Bit 1 */
#define ADC_JEXTSEL_2                           ((uint32_t)0x00004000) /* Bit 2 */

#define ADC_JEXTTRIG                            ((uint32_t)0x00008000) /* External Trigger Conversion mode for injected channels */

#define ADC_EXTSEL                              ((uint32_t)0x000E0000) /* EXTSEL[2:0] bits (External Event Select for regular group) */
#define ADC_EXTSEL_0                            ((uint32_t)0x00020000) /* Bit 0 */
#define ADC_EXTSEL_1                            ((uint32_t)0x00040000) /* Bit 1 */
#define ADC_EXTSEL_2                            ((uint32_t)0x00080000) /* Bit 2 */
#define ADC_EXTSEL_TRGO_1						((uint32_t)0x00000000) /* TRGO event of timer 1 		*/
#define ADC_EXTSEL_CC1_1						((uint32_t)0x00000000) /* CC1 event of timer 1 			*/
#define ADC_EXTSEL_CC2_1						((uint32_t)0x00000000) /* CC2 event of timer 1 			*/
#define ADC_EXTSEL_TRGO_2						((uint32_t)0x00000000) /* TRGO event of timer 2 		*/
#define ADC_EXTSEL_CC1_2						((uint32_t)0x00000000) /* CC1 event of timer 2 			*/
#define ADC_EXTSEL_CC2_2						((uint32_t)0x00000000) /* CC2 event of timer 2 			*/
#define ADC_EXTSEL_OPA							((uint32_t)0x00000000) /* OPA trigger/(PD3/PC2) 		*/
#define ADC_EXTSEL_SWSTART						((uint32_t)0x00000000) /* SWSTART software trigger 	*/

#define ADC_EXTTRIG                             ((uint32_t)0x00100000) /* External Trigger Conversion mode for regular channels */
#define ADC_JSWSTART                            ((uint32_t)0x00200000) /* Start Conversion of injected channels */
#define ADC_SWSTART                             ((uint32_t)0x00400000) /* Start Conversion of regular channels */

/******************  Bit definition for ADC_SAMPTR2 register  *******************/
#define ADC_SMP0                                ((uint32_t)0x00000007) /* SMP0[2:0] bits (Channel 0 Sample time selection) */
#define ADC_SMP0_0                              ((uint32_t)0x00000001) /* Bit 0 */
#define ADC_SMP0_1                              ((uint32_t)0x00000002) /* Bit 1 */
#define ADC_SMP0_2                              ((uint32_t)0x00000004) /* Bit 2 */

#define ADC_SMP1                                ((uint32_t)0x00000038) /* SMP1[2:0] bits (Channel 1 Sample time selection) */
#define ADC_SMP1_0                              ((uint32_t)0x00000008) /* Bit 0 */
#define ADC_SMP1_1                              ((uint32_t)0x00000010) /* Bit 1 */
#define ADC_SMP1_2                              ((uint32_t)0x00000020) /* Bit 2 */

#define ADC_SMP2                                ((uint32_t)0x000001C0) /* SMP2[2:0] bits (Channel 2 Sample time selection) */
#define ADC_SMP2_0                              ((uint32_t)0x00000040) /* Bit 0 */
#define ADC_SMP2_1                              ((uint32_t)0x00000080) /* Bit 1 */
#define ADC_SMP2_2                              ((uint32_t)0x00000100) /* Bit 2 */

#define ADC_SMP3                                ((uint32_t)0x00000E00) /* SMP3[2:0] bits (Channel 3 Sample time selection) */
#define ADC_SMP3_0                              ((uint32_t)0x00000200) /* Bit 0 */
#define ADC_SMP3_1                              ((uint32_t)0x00000400) /* Bit 1 */
#define ADC_SMP3_2                              ((uint32_t)0x00000800) /* Bit 2 */

#define ADC_SMP4                                ((uint32_t)0x00007000) /* SMP4[2:0] bits (Channel 4 Sample time selection) */
#define ADC_SMP4_0                              ((uint32_t)0x00001000) /* Bit 0 */
#define ADC_SMP4_1                              ((uint32_t)0x00002000) /* Bit 1 */
#define ADC_SMP4_2                              ((uint32_t)0x00004000) /* Bit 2 */

#define ADC_SMP5                                ((uint32_t)0x00038000) /* SMP5[2:0] bits (Channel 5 Sample time selection) */
#define ADC_SMP5_0                              ((uint32_t)0x00008000) /* Bit 0 */
#define ADC_SMP5_1                              ((uint32_t)0x00010000) /* Bit 1 */
#define ADC_SMP5_2                              ((uint32_t)0x00020000) /* Bit 2 */

#define ADC_SMP6                                ((uint32_t)0x001C0000) /* SMP6[2:0] bits (Channel 6 Sample time selection) */
#define ADC_SMP6_0                              ((uint32_t)0x00040000) /* Bit 0 */
#define ADC_SMP6_1                              ((uint32_t)0x00080000) /* Bit 1 */
#define ADC_SMP6_2                              ((uint32_t)0x00100000) /* Bit 2 */

#define ADC_SMP7                                ((uint32_t)0x00E00000) /* SMP7[2:0] bits (Channel 7 Sample time selection) */
#define ADC_SMP7_0                              ((uint32_t)0x00200000) /* Bit 0 */
#define ADC_SMP7_1                              ((uint32_t)0x00400000) /* Bit 1 */
#define ADC_SMP7_2                              ((uint32_t)0x00800000) /* Bit 2 */

#define ADC_SMP8                                ((uint32_t)0x07000000) /* SMP8[2:0] bits (Channel 8 Sample time selection) */
#define ADC_SMP8_0                              ((uint32_t)0x01000000) /* Bit 0 */
#define ADC_SMP8_1                              ((uint32_t)0x02000000) /* Bit 1 */
#define ADC_SMP8_2                              ((uint32_t)0x04000000) /* Bit 2 */

#define ADC_SMP9                                ((uint32_t)0x38000000) /* SMP9[2:0] bits (Channel 9 Sample time selection) */
#define ADC_SMP9_0                              ((uint32_t)0x08000000) /* Bit 0 */
#define ADC_SMP9_1                              ((uint32_t)0x10000000) /* Bit 1 */
#define ADC_SMP9_2                              ((uint32_t)0x20000000) /* Bit 2 */

/******************  Bit definition for ADC_IOFR1 register  *******************/
#define ADC_JOFFSET1                            ((uint32_t)0x00000FFF) /* Data offset for injected channel 1 */

/******************  Bit definition for ADC_IOFR2 register  *******************/
#define ADC_JOFFSET2                            ((uint32_t)0x00000FFF) /* Data offset for injected channel 2 */

/******************  Bit definition for ADC_IOFR3 register  *******************/
#define ADC_JOFFSET3                            ((uint32_t)0x00000FFF) /* Data offset for injected channel 3 */

/******************  Bit definition for ADC_IOFR4 register  *******************/
#define ADC_JOFFSET4                            ((uint32_t)0x00000FFF) /* Data offset for injected channel 4 */

/*******************  Bit definition for ADC_WDHTR register  ********************/
#define ADC_HT                                  ((uint32_t)0x000001FF) /* Analog watchdog high threshold */

/*******************  Bit definition for ADC_WDLTR register  ********************/
#define ADC_LT                                  ((uint32_t)0x000001FF) /* Analog watchdog low threshold */

/*******************  Bit definition for ADC_RSQR1 register  *******************/
#define ADC_SQ13                                ((uint32_t)0x0000001F) /* SQ13[4:0] bits (13th conversion in regular sequence) */
#define ADC_SQ13_0                              ((uint32_t)0x00000001) /* Bit 0 */
#define ADC_SQ13_1                              ((uint32_t)0x00000002) /* Bit 1 */
#define ADC_SQ13_2                              ((uint32_t)0x00000004) /* Bit 2 */
#define ADC_SQ13_3                              ((uint32_t)0x00000008) /* Bit 3 */
#define ADC_SQ13_4                              ((uint32_t)0x00000010) /* Bit 4 */

#define ADC_SQ14                                ((uint32_t)0x000003E0) /* SQ14[4:0] bits (14th conversion in regular sequence) */
#define ADC_SQ14_0                              ((uint32_t)0x00000020) /* Bit 0 */
#define ADC_SQ14_1                              ((uint32_t)0x00000040) /* Bit 1 */
#define ADC_SQ14_2                              ((uint32_t)0x00000080) /* Bit 2 */
#define ADC_SQ14_3                              ((uint32_t)0x00000100) /* Bit 3 */
#define ADC_SQ14_4                              ((uint32_t)0x00000200) /* Bit 4 */

#define ADC_SQ15                                ((uint32_t)0x00007C00) /* SQ15[4:0] bits (15th conversion in regular sequence) */
#define ADC_SQ15_0                              ((uint32_t)0x00000400) /* Bit 0 */
#define ADC_SQ15_1                              ((uint32_t)0x00000800) /* Bit 1 */
#define ADC_SQ15_2                              ((uint32_t)0x00001000) /* Bit 2 */
#define ADC_SQ15_3                              ((uint32_t)0x00002000) /* Bit 3 */
#define ADC_SQ15_4                              ((uint32_t)0x00004000) /* Bit 4 */

#define ADC_SQ16                                ((uint32_t)0x000F8000) /* SQ16[4:0] bits (16th conversion in regular sequence) */
#define ADC_SQ16_0                              ((uint32_t)0x00008000) /* Bit 0 */
#define ADC_SQ16_1                              ((uint32_t)0x00010000) /* Bit 1 */
#define ADC_SQ16_2                              ((uint32_t)0x00020000) /* Bit 2 */
#define ADC_SQ16_3                              ((uint32_t)0x00040000) /* Bit 3 */
#define ADC_SQ16_4                              ((uint32_t)0x00080000) /* Bit 4 */

#define ADC_L                                   ((uint32_t)0x00F00000) /* L[3:0] bits (Regular channel sequence length) */
#define ADC_L_0                                 ((uint32_t)0x00100000) /* Bit 0 */
#define ADC_L_1                                 ((uint32_t)0x00200000) /* Bit 1 */
#define ADC_L_2                                 ((uint32_t)0x00400000) /* Bit 2 */
#define ADC_L_3                                 ((uint32_t)0x00800000) /* Bit 3 */

/*******************  Bit definition for ADC_RSQR2 register  *******************/
#define ADC_SQ7                                 ((uint32_t)0x0000001F) /* SQ7[4:0] bits (7th conversion in regular sequence) */
#define ADC_SQ7_0                               ((uint32_t)0x00000001) /* Bit 0 */
#define ADC_SQ7_1                               ((uint32_t)0x00000002) /* Bit 1 */
#define ADC_SQ7_2                               ((uint32_t)0x00000004) /* Bit 2 */
#define ADC_SQ7_3                               ((uint32_t)0x00000008) /* Bit 3 */
#define ADC_SQ7_4                               ((uint32_t)0x00000010) /* Bit 4 */

#define ADC_SQ8                                 ((uint32_t)0x000003E0) /* SQ8[4:0] bits (8th conversion in regular sequence) */
#define ADC_SQ8_0                               ((uint32_t)0x00000020) /* Bit 0 */
#define ADC_SQ8_1                               ((uint32_t)0x00000040) /* Bit 1 */
#define ADC_SQ8_2                               ((uint32_t)0x00000080) /* Bit 2 */
#define ADC_SQ8_3                               ((uint32_t)0x00000100) /* Bit 3 */
#define ADC_SQ8_4                               ((uint32_t)0x00000200) /* Bit 4 */

#define ADC_SQ9                                 ((uint32_t)0x00007C00) /* SQ9[4:0] bits (9th conversion in regular sequence) */
#define ADC_SQ9_0                               ((uint32_t)0x00000400) /* Bit 0 */
#define ADC_SQ9_1                               ((uint32_t)0x00000800) /* Bit 1 */
#define ADC_SQ9_2                               ((uint32_t)0x00001000) /* Bit 2 */
#define ADC_SQ9_3                               ((uint32_t)0x00002000) /* Bit 3 */
#define ADC_SQ9_4                               ((uint32_t)0x00004000) /* Bit 4 */

#define ADC_SQ10                                ((uint32_t)0x000F8000) /* SQ10[4:0] bits (10th conversion in regular sequence) */
#define ADC_SQ10_0                              ((uint32_t)0x00008000) /* Bit 0 */
#define ADC_SQ10_1                              ((uint32_t)0x00010000) /* Bit 1 */
#define ADC_SQ10_2                              ((uint32_t)0x00020000) /* Bit 2 */
#define ADC_SQ10_3                              ((uint32_t)0x00040000) /* Bit 3 */
#define ADC_SQ10_4                              ((uint32_t)0x00080000) /* Bit 4 */

#define ADC_SQ11                                ((uint32_t)0x01F00000) /* SQ11[4:0] bits (11th conversion in regular sequence) */
#define ADC_SQ11_0                              ((uint32_t)0x00100000) /* Bit 0 */
#define ADC_SQ11_1                              ((uint32_t)0x00200000) /* Bit 1 */
#define ADC_SQ11_2                              ((uint32_t)0x00400000) /* Bit 2 */
#define ADC_SQ11_3                              ((uint32_t)0x00800000) /* Bit 3 */
#define ADC_SQ11_4                              ((uint32_t)0x01000000) /* Bit 4 */

#define ADC_SQ12                                ((uint32_t)0x3E000000) /* SQ12[4:0] bits (12th conversion in regular sequence) */
#define ADC_SQ12_0                              ((uint32_t)0x02000000) /* Bit 0 */
#define ADC_SQ12_1                              ((uint32_t)0x04000000) /* Bit 1 */
#define ADC_SQ12_2                              ((uint32_t)0x08000000) /* Bit 2 */
#define ADC_SQ12_3                              ((uint32_t)0x10000000) /* Bit 3 */
#define ADC_SQ12_4                              ((uint32_t)0x20000000) /* Bit 4 */

/*******************  Bit definition for ADC_RSQR3 register  *******************/
#define ADC_SQ1                                 ((uint32_t)0x0000001F) /* SQ1[4:0] bits (1st conversion in regular sequence) */
#define ADC_SQ1_0                               ((uint32_t)0x00000001) /* Bit 0 */
#define ADC_SQ1_1                               ((uint32_t)0x00000002) /* Bit 1 */
#define ADC_SQ1_2                               ((uint32_t)0x00000004) /* Bit 2 */
#define ADC_SQ1_3                               ((uint32_t)0x00000008) /* Bit 3 */
#define ADC_SQ1_4                               ((uint32_t)0x00000010) /* Bit 4 */

#define ADC_SQ2                                 ((uint32_t)0x000003E0) /* SQ2[4:0] bits (2nd conversion in regular sequence) */
#define ADC_SQ2_0                               ((uint32_t)0x00000020) /* Bit 0 */
#define ADC_SQ2_1                               ((uint32_t)0x00000040) /* Bit 1 */
#define ADC_SQ2_2                               ((uint32_t)0x00000080) /* Bit 2 */
#define ADC_SQ2_3                               ((uint32_t)0x00000100) /* Bit 3 */
#define ADC_SQ2_4                               ((uint32_t)0x00000200) /* Bit 4 */

#define ADC_SQ3                                 ((uint32_t)0x00007C00) /* SQ3[4:0] bits (3rd conversion in regular sequence) */
#define ADC_SQ3_0                               ((uint32_t)0x00000400) /* Bit 0 */
#define ADC_SQ3_1                               ((uint32_t)0x00000800) /* Bit 1 */
#define ADC_SQ3_2                               ((uint32_t)0x00001000) /* Bit 2 */
#define ADC_SQ3_3                               ((uint32_t)0x00002000) /* Bit 3 */
#define ADC_SQ3_4                               ((uint32_t)0x00004000) /* Bit 4 */

#define ADC_SQ4                                 ((uint32_t)0x000F8000) /* SQ4[4:0] bits (4th conversion in regular sequence) */
#define ADC_SQ4_0                               ((uint32_t)0x00008000) /* Bit 0 */
#define ADC_SQ4_1                               ((uint32_t)0x00010000) /* Bit 1 */
#define ADC_SQ4_2                               ((uint32_t)0x00020000) /* Bit 2 */
#define ADC_SQ4_3                               ((uint32_t)0x00040000) /* Bit 3 */
#define ADC_SQ4_4                               ((uint32_t)0x00080000) /* Bit 4 */

#define ADC_SQ5                                 ((uint32_t)0x01F00000) /* SQ5[4:0] bits (5th conversion in regular sequence) */
#define ADC_SQ5_0                               ((uint32_t)0x00100000) /* Bit 0 */
#define ADC_SQ5_1                               ((uint32_t)0x00200000) /* Bit 1 */
#define ADC_SQ5_2                               ((uint32_t)0x00400000) /* Bit 2 */
#define ADC_SQ5_3                               ((uint32_t)0x00800000) /* Bit 3 */
#define ADC_SQ5_4                               ((uint32_t)0x01000000) /* Bit 4 */

#define ADC_SQ6                                 ((uint32_t)0x3E000000) /* SQ6[4:0] bits (6th conversion in regular sequence) */
#define ADC_SQ6_0                               ((uint32_t)0x02000000) /* Bit 0 */
#define ADC_SQ6_1                               ((uint32_t)0x04000000) /* Bit 1 */
#define ADC_SQ6_2                               ((uint32_t)0x08000000) /* Bit 2 */
#define ADC_SQ6_3                               ((uint32_t)0x10000000) /* Bit 3 */
#define ADC_SQ6_4                               ((uint32_t)0x20000000) /* Bit 4 */

/*******************  Bit definition for ADC_ISQR register  *******************/
#define ADC_JSQ1                                ((uint32_t)0x0000001F) /* JSQ1[4:0] bits (1st conversion in injected sequence) */
#define ADC_JSQ1_0                              ((uint32_t)0x00000001) /* Bit 0 */
#define ADC_JSQ1_1                              ((uint32_t)0x00000002) /* Bit 1 */
#define ADC_JSQ1_2                              ((uint32_t)0x00000004) /* Bit 2 */
#define ADC_JSQ1_3                              ((uint32_t)0x00000008) /* Bit 3 */
#define ADC_JSQ1_4                              ((uint32_t)0x00000010) /* Bit 4 */

#define ADC_JSQ2                                ((uint32_t)0x000003E0) /* JSQ2[4:0] bits (2nd conversion in injected sequence) */
#define ADC_JSQ2_0                              ((uint32_t)0x00000020) /* Bit 0 */
#define ADC_JSQ2_1                              ((uint32_t)0x00000040) /* Bit 1 */
#define ADC_JSQ2_2                              ((uint32_t)0x00000080) /* Bit 2 */
#define ADC_JSQ2_3                              ((uint32_t)0x00000100) /* Bit 3 */
#define ADC_JSQ2_4                              ((uint32_t)0x00000200) /* Bit 4 */

#define ADC_JSQ3                                ((uint32_t)0x00007C00) /* JSQ3[4:0] bits (3rd conversion in injected sequence) */
#define ADC_JSQ3_0                              ((uint32_t)0x00000400) /* Bit 0 */
#define ADC_JSQ3_1                              ((uint32_t)0x00000800) /* Bit 1 */
#define ADC_JSQ3_2                              ((uint32_t)0x00001000) /* Bit 2 */
#define ADC_JSQ3_3                              ((uint32_t)0x00002000) /* Bit 3 */
#define ADC_JSQ3_4                              ((uint32_t)0x00004000) /* Bit 4 */

#define ADC_JSQ4                                ((uint32_t)0x000F8000) /* JSQ4[4:0] bits (4th conversion in injected sequence) */
#define ADC_JSQ4_0                              ((uint32_t)0x00008000) /* Bit 0 */
#define ADC_JSQ4_1                              ((uint32_t)0x00010000) /* Bit 1 */
#define ADC_JSQ4_2                              ((uint32_t)0x00020000) /* Bit 2 */
#define ADC_JSQ4_3                              ((uint32_t)0x00040000) /* Bit 3 */
#define ADC_JSQ4_4                              ((uint32_t)0x00080000) /* Bit 4 */

#define ADC_JL                                  ((uint32_t)0x00300000) /* JL[1:0] bits (Injected Sequence length) */
#define ADC_JL_0                                ((uint32_t)0x00100000) /* Bit 0 */
#define ADC_JL_1                                ((uint32_t)0x00200000) /* Bit 1 */

/*******************  Bit definition for ADC_IDATAR1 register  *******************/
#define ADC_IDATAR1_JDATA                       ((uint32_t)0x0000FFFF) /* Injected data */

/*******************  Bit definition for ADC_IDATAR2 register  *******************/
#define ADC_IDATAR2_JDATA                       ((uint32_t)0x0000FFFF) /* Injected data */

/*******************  Bit definition for ADC_IDATAR3 register  *******************/
#define ADC_IDATAR3_JDATA                       ((uint32_t)0x0000FFFF) /* Injected data */

/*******************  Bit definition for ADC_IDATAR4 register  *******************/
#define ADC_IDATAR4_JDATA                       ((uint32_t)0x0000FFFF) /* Injected data */

/********************  Bit definition for ADC_RDATAR register  ********************/
#define ADC_RDATAR_DATA                         ((uint32_t)0x0000FFFF) /* Regular data */
#define ADC_RDATAR_ADC2DATA                     ((uint32_t)0xFFFF0000) /* ADC2 data */

/*******************  Bit definition for ADC_CTLR3 register  ********************/
#define ADC_ADC_LP                              ((uint32_t)0x00000001) /* ADC low power mode control bit */
#define ADC_DUTYEN                              ((uint32_t)0x00000002) /* ADC clock duty cycle control bit */
#define ADC_ADC_DRVEN                           ((uint32_t)0x00000004) /* Touchkey multi-mask enable */
#define AWD_SCAN                                ((uint32_t)0x00000008) /* Analog watchdog scan enable */
#define ADC_AWD0_RST_EN                         ((uint32_t)0x00000010) /* Analog watchdog 0 output reset enable bit */
#define ADC_AWD1_RST_EN                         ((uint32_t)0x00000020) /* Analog watchdog 1 output reset enable bit */
#define ADC_AWD2_RST_EN                         ((uint32_t)0x00000040) /* Analog watchdog 2 output reset enable bit */
#define ADC_AWD0_RES                            ((uint32_t)0x00000100) /* Analog watchdog 0 compare result */
#define ADC_AWD1_RES                            ((uint32_t)0x00000200) /* Analog watchdog 1 compare result */
#define ADC_AWD2_RES                            ((uint32_t)0x00000400) /* Analog watchdog 2 compare result */
#define ADC_DRV_OUTEN                           ((uint32_t)0x00FF0000) /* DRV_OUTEN[7:0] Touchkey multi-mask each channel enable */
#define ADC_DRV_OUTEN_0                         ((uint32_t)0x00010000) /* Bit 0 */
#define ADC_DRV_OUTEN_1                         ((uint32_t)0x00020000) /* Bit 1 */
#define ADC_DRV_OUTEN_2                         ((uint32_t)0x00040000) /* Bit 2 */
#define ADC_DRV_OUTEN_3                         ((uint32_t)0x00080000) /* Bit 3 */
#define ADC_DRV_OUTEN_4                         ((uint32_t)0x00100000) /* Bit 4 */
#define ADC_DRV_OUTEN_5                         ((uint32_t)0x00200000) /* Bit 5 */
#define ADC_DRV_OUTEN_6                         ((uint32_t)0x00400000) /* Bit 6 */
#define ADC_DRV_OUTEN_7                         ((uint32_t)0x00800000) /* Bit 7 */

/*******************  Bit definition for ADC_WDTR1 register  ********************/
#define ADC_HTR1                            ((uint32_t)0x0FFF0000) /* Analog watchdog high threshold setting */
#define ADC_LTR1                            ((uint32_t)0x00000FFF) /* Analog watchdog log threshold setting */

/*******************  Bit definition for ADC_WDTR2 register  ********************/
#define ADC_HTR2                            ((uint32_t)0x0FFF0000) /* Analog watchdog high threshold setting */
#define ADC_LTR2                            ((uint32_t)0x00000FFF) /* Analog watchdog log threshold setting */



/******************************************************************************/
/*                             DMA Controller                                 */
/******************************************************************************/

/*******************  Bit definition for DMA_INTFR register  ********************/
#define DMA_GIF1                                ((uint32_t)0x00000001) /* Channel 1 Global interrupt flag */
#define DMA_TCIF1                               ((uint32_t)0x00000002) /* Channel 1 Transfer Complete flag */
#define DMA_HTIF1                               ((uint32_t)0x00000004) /* Channel 1 Half Transfer flag */
#define DMA_TEIF1                               ((uint32_t)0x00000008) /* Channel 1 Transfer Error flag */
#define DMA_GIF2                                ((uint32_t)0x00000010) /* Channel 2 Global interrupt flag */
#define DMA_TCIF2                               ((uint32_t)0x00000020) /* Channel 2 Transfer Complete flag */
#define DMA_HTIF2                               ((uint32_t)0x00000040) /* Channel 2 Half Transfer flag */
#define DMA_TEIF2                               ((uint32_t)0x00000080) /* Channel 2 Transfer Error flag */
#define DMA_GIF3                                ((uint32_t)0x00000100) /* Channel 3 Global interrupt flag */
#define DMA_TCIF3                               ((uint32_t)0x00000200) /* Channel 3 Transfer Complete flag */
#define DMA_HTIF3                               ((uint32_t)0x00000400) /* Channel 3 Half Transfer flag */
#define DMA_TEIF3                               ((uint32_t)0x00000800) /* Channel 3 Transfer Error flag */
#define DMA_GIF4                                ((uint32_t)0x00001000) /* Channel 4 Global interrupt flag */
#define DMA_TCIF4                               ((uint32_t)0x00002000) /* Channel 4 Transfer Complete flag */
#define DMA_HTIF4                               ((uint32_t)0x00004000) /* Channel 4 Half Transfer flag */
#define DMA_TEIF4                               ((uint32_t)0x00008000) /* Channel 4 Transfer Error flag */
#define DMA_GIF5                                ((uint32_t)0x00010000) /* Channel 5 Global interrupt flag */
#define DMA_TCIF5                               ((uint32_t)0x00020000) /* Channel 5 Transfer Complete flag */
#define DMA_HTIF5                               ((uint32_t)0x00040000) /* Channel 5 Half Transfer flag */
#define DMA_TEIF5                               ((uint32_t)0x00080000) /* Channel 5 Transfer Error flag */
#define DMA_GIF6                                ((uint32_t)0x00100000) /* Channel 6 Global interrupt flag */
#define DMA_TCIF6                               ((uint32_t)0x00200000) /* Channel 6 Transfer Complete flag */
#define DMA_HTIF6                               ((uint32_t)0x00400000) /* Channel 6 Half Transfer flag */
#define DMA_TEIF6                               ((uint32_t)0x00800000) /* Channel 6 Transfer Error flag */
#define DMA_GIF7                                ((uint32_t)0x01000000) /* Channel 7 Global interrupt flag */
#define DMA_TCIF7                               ((uint32_t)0x02000000) /* Channel 7 Transfer Complete flag */
#define DMA_HTIF7                               ((uint32_t)0x04000000) /* Channel 7 Half Transfer flag */
#define DMA_TEIF7                               ((uint32_t)0x08000000) /* Channel 7 Transfer Error flag */


/*******************  Bit definition for DMA_INTFCR register  *******************/
#define DMA_CGIF1                               ((uint32_t)0x00000001) /* Channel 1 Global interrupt clear */
#define DMA_CTCIF1                              ((uint32_t)0x00000002) /* Channel 1 Transfer Complete clear */
#define DMA_CHTIF1                              ((uint32_t)0x00000004) /* Channel 1 Half Transfer clear */
#define DMA_CTEIF1                              ((uint32_t)0x00000008) /* Channel 1 Transfer Error clear */
#define DMA_CGIF2                               ((uint32_t)0x00000010) /* Channel 2 Global interrupt clear */
#define DMA_CTCIF2                              ((uint32_t)0x00000020) /* Channel 2 Transfer Complete clear */
#define DMA_CHTIF2                              ((uint32_t)0x00000040) /* Channel 2 Half Transfer clear */
#define DMA_CTEIF2                              ((uint32_t)0x00000080) /* Channel 2 Transfer Error clear */
#define DMA_CGIF3                               ((uint32_t)0x00000100) /* Channel 3 Global interrupt clear */
#define DMA_CTCIF3                              ((uint32_t)0x00000200) /* Channel 3 Transfer Complete clear */
#define DMA_CHTIF3                              ((uint32_t)0x00000400) /* Channel 3 Half Transfer clear */
#define DMA_CTEIF3                              ((uint32_t)0x00000800) /* Channel 3 Transfer Error clear */
#define DMA_CGIF4                               ((uint32_t)0x00001000) /* Channel 4 Global interrupt clear */
#define DMA_CTCIF4                              ((uint32_t)0x00002000) /* Channel 4 Transfer Complete clear */
#define DMA_CHTIF4                              ((uint32_t)0x00004000) /* Channel 4 Half Transfer clear */
#define DMA_CTEIF4                              ((uint32_t)0x00008000) /* Channel 4 Transfer Error clear */
#define DMA_CGIF5                               ((uint32_t)0x00010000) /* Channel 5 Global interrupt clear */
#define DMA_CTCIF5                              ((uint32_t)0x00020000) /* Channel 5 Transfer Complete clear */
#define DMA_CHTIF5                              ((uint32_t)0x00040000) /* Channel 5 Half Transfer clear */
#define DMA_CTEIF5                              ((uint32_t)0x00080000) /* Channel 5 Transfer Error clear */
#define DMA_CGIF6                               ((uint32_t)0x00100000) /* Channel 6 Global interrupt clear */
#define DMA_CTCIF6                              ((uint32_t)0x00200000) /* Channel 6 Transfer Complete clear */
#define DMA_CHTIF6                              ((uint32_t)0x00400000) /* Channel 6 Half Transfer clear */
#define DMA_CTEIF6                              ((uint32_t)0x00800000) /* Channel 6 Transfer Error clear */
#define DMA_CGIF7                               ((uint32_t)0x01000000) /* Channel 7 Global interrupt clear */
#define DMA_CTCIF7                              ((uint32_t)0x02000000) /* Channel 7 Transfer Complete clear */
#define DMA_CHTIF7                              ((uint32_t)0x04000000) /* Channel 7 Half Transfer clear */
#define DMA_CTEIF7                              ((uint32_t)0x08000000) /* Channel 7 Transfer Error clear */

/*******************  Bit definition for DMA_CFGR1 register  *******************/
#define DMA_CFGR1_EN                            ((uint16_t)0x0001) /* Channel enable*/
#define DMA_CFGR1_TCIE                          ((uint16_t)0x0002) /* Transfer complete interrupt enable */
#define DMA_CFGR1_HTIE                          ((uint16_t)0x0004) /* Half Transfer interrupt enable */
#define DMA_CFGR1_TEIE                          ((uint16_t)0x0008) /* Transfer error interrupt enable */
#define DMA_CFGR1_DIR                           ((uint16_t)0x0010) /* Data transfer direction (Setting = Memory -> Peripheral) */
#define DMA_CFGR1_CIRC                          ((uint16_t)0x0020) /* Circular mode */
#define DMA_CFGR1_PINC                          ((uint16_t)0x0040) /* Peripheral increment mode */
#define DMA_CFGR1_MINC                          ((uint16_t)0x0080) /* Memory increment mode */

#define DMA_CFGR1_PSIZE                         ((uint16_t)0x0300) /* PSIZE[1:0] bits (Peripheral size) */
#define DMA_CFGR1_PSIZE_0                       ((uint16_t)0x0100) /* Bit 0 */
#define DMA_CFGR1_PSIZE_1                       ((uint16_t)0x0200) /* Bit 1 */

#define DMA_CFGR1_MSIZE                         ((uint16_t)0x0C00) /* MSIZE[1:0] bits (Memory size) */
#define DMA_CFGR1_MSIZE_0                       ((uint16_t)0x0400) /* Bit 0 */
#define DMA_CFGR1_MSIZE_1                       ((uint16_t)0x0800) /* Bit 1 */

#define DMA_CFGR1_PL                            ((uint16_t)0x3000) /* PL[1:0] bits(Channel Priority level) */
#define DMA_CFGR1_PL_0                          ((uint16_t)0x1000) /* Bit 0 */
#define DMA_CFGR1_PL_1                          ((uint16_t)0x2000) /* Bit 1 */

#define DMA_CFGR1_MEM2MEM                       ((uint16_t)0x4000) /* Memory to memory mode */

/*******************  Bit definition for DMA_CFGR2 register  *******************/
#define DMA_CFGR2_EN                            ((uint16_t)0x0001) /* Channel enable */
#define DMA_CFGR2_TCIE                          ((uint16_t)0x0002) /* Transfer complete interrupt enable */
#define DMA_CFGR2_HTIE                          ((uint16_t)0x0004) /* Half Transfer interrupt enable */
#define DMA_CFGR2_TEIE                          ((uint16_t)0x0008) /* Transfer error interrupt enable */
#define DMA_CFGR2_DIR                           ((uint16_t)0x0010) /* Data transfer direction */
#define DMA_CFGR2_CIRC                          ((uint16_t)0x0020) /* Circular mode */
#define DMA_CFGR2_PINC                          ((uint16_t)0x0040) /* Peripheral increment mode */
#define DMA_CFGR2_MINC                          ((uint16_t)0x0080) /* Memory increment mode */

#define DMA_CFGR2_PSIZE                         ((uint16_t)0x0300) /* PSIZE[1:0] bits (Peripheral size) */
#define DMA_CFGR2_PSIZE_0                       ((uint16_t)0x0100) /* Bit 0 */
#define DMA_CFGR2_PSIZE_1                       ((uint16_t)0x0200) /* Bit 1 */

#define DMA_CFGR2_MSIZE                         ((uint16_t)0x0C00) /* MSIZE[1:0] bits (Memory size) */
#define DMA_CFGR2_MSIZE_0                       ((uint16_t)0x0400) /* Bit 0 */
#define DMA_CFGR2_MSIZE_1                       ((uint16_t)0x0800) /* Bit 1 */

#define DMA_CFGR2_PL                            ((uint16_t)0x3000) /* PL[1:0] bits (Channel Priority level) */
#define DMA_CFGR2_PL_0                          ((uint16_t)0x1000) /* Bit 0 */
#define DMA_CFGR2_PL_1                          ((uint16_t)0x2000) /* Bit 1 */

#define DMA_CFGR2_MEM2MEM                       ((uint16_t)0x4000) /* Memory to memory mode */

/*******************  Bit definition for DMA_CFGR3 register  *******************/
#define DMA_CFGR3_EN                            ((uint16_t)0x0001) /* Channel enable */
#define DMA_CFGR3_TCIE                          ((uint16_t)0x0002) /* Transfer complete interrupt enable */
#define DMA_CFGR3_HTIE                          ((uint16_t)0x0004) /* Half Transfer interrupt enable */
#define DMA_CFGR3_TEIE                          ((uint16_t)0x0008) /* Transfer error interrupt enable */
#define DMA_CFGR3_DIR                           ((uint16_t)0x0010) /* Data transfer direction */
#define DMA_CFGR3_CIRC                          ((uint16_t)0x0020) /* Circular mode */
#define DMA_CFGR3_PINC                          ((uint16_t)0x0040) /* Peripheral increment mode */
#define DMA_CFGR3_MINC                          ((uint16_t)0x0080) /* Memory increment mode */

#define DMA_CFGR3_PSIZE                         ((uint16_t)0x0300) /* PSIZE[1:0] bits (Peripheral size) */
#define DMA_CFGR3_PSIZE_0                       ((uint16_t)0x0100) /* Bit 0 */
#define DMA_CFGR3_PSIZE_1                       ((uint16_t)0x0200) /* Bit 1 */

#define DMA_CFGR3_MSIZE                         ((uint16_t)0x0C00) /* MSIZE[1:0] bits (Memory size) */
#define DMA_CFGR3_MSIZE_0                       ((uint16_t)0x0400) /* Bit 0 */
#define DMA_CFGR3_MSIZE_1                       ((uint16_t)0x0800) /* Bit 1 */

#define DMA_CFGR3_PL                            ((uint16_t)0x3000) /* PL[1:0] bits (Channel Priority level) */
#define DMA_CFGR3_PL_0                          ((uint16_t)0x1000) /* Bit 0 */
#define DMA_CFGR3_PL_1                          ((uint16_t)0x2000) /* Bit 1 */

#define DMA_CFGR3_MEM2MEM                       ((uint16_t)0x4000) /* Memory to memory mode */

/*******************  Bit definition for DMA_CFG4 register  *******************/
#define DMA_CFGR4_EN                             ((uint16_t)0x0001) /* Channel enable */
#define DMA_CFGR4_TCIE                           ((uint16_t)0x0002) /* Transfer complete interrupt enable */
#define DMA_CFGR4_HTIE                           ((uint16_t)0x0004) /* Half Transfer interrupt enable */
#define DMA_CFG4_TEIE                            ((uint16_t)0x0008) /* Transfer error interrupt enable */
#define DMA_CFGR4_DIR                            ((uint16_t)0x0010) /* Data transfer direction */
#define DMA_CFGR4_CIRC                           ((uint16_t)0x0020) /* Circular mode */
#define DMA_CFGR4_PINC                           ((uint16_t)0x0040) /* Peripheral increment mode */
#define DMA_CFGR4_MINC                           ((uint16_t)0x0080) /* Memory increment mode */

#define DMA_CFGR4_PSIZE                          ((uint16_t)0x0300) /* PSIZE[1:0] bits (Peripheral size) */
#define DMA_CFGR4_PSIZE_0                        ((uint16_t)0x0100) /* Bit 0 */
#define DMA_CFGR4_PSIZE_1                        ((uint16_t)0x0200) /* Bit 1 */

#define DMA_CFGR4_MSIZE                          ((uint16_t)0x0C00) /* MSIZE[1:0] bits (Memory size) */
#define DMA_CFGR4_MSIZE_0                        ((uint16_t)0x0400) /* Bit 0 */
#define DMA_CFGR4_MSIZE_1                        ((uint16_t)0x0800) /* Bit 1 */

#define DMA_CFGR4_PL                             ((uint16_t)0x3000) /* PL[1:0] bits (Channel Priority level) */
#define DMA_CFGR4_PL_0                           ((uint16_t)0x1000) /* Bit 0 */
#define DMA_CFGR4_PL_1                           ((uint16_t)0x2000) /* Bit 1 */

#define DMA_CFGR4_MEM2MEM                        ((uint16_t)0x4000) /* Memory to memory mode */

/******************  Bit definition for DMA_CFG5 register  *******************/
#define DMA_CFGR5_EN                             ((uint16_t)0x0001) /* Channel enable */
#define DMA_CFGR5_TCIE                           ((uint16_t)0x0002) /* Transfer complete interrupt enable */
#define DMA_CFGR5_HTIE                           ((uint16_t)0x0004) /* Half Transfer interrupt enable */
#define DMA_CFGR5_TEIE                           ((uint16_t)0x0008) /* Transfer error interrupt enable */
#define DMA_CFGR5_DIR                            ((uint16_t)0x0010) /* Data transfer direction */
#define DMA_CFGR5_CIRC                           ((uint16_t)0x0020) /* Circular mode */
#define DMA_CFGR5_PINC                           ((uint16_t)0x0040) /* Peripheral increment mode */
#define DMA_CFGR5_MINC                           ((uint16_t)0x0080) /* Memory increment mode */

#define DMA_CFGR5_PSIZE                          ((uint16_t)0x0300) /* PSIZE[1:0] bits (Peripheral size) */
#define DMA_CFGR5_PSIZE_0                        ((uint16_t)0x0100) /* Bit 0 */
#define DMA_CFGR5_PSIZE_1                        ((uint16_t)0x0200) /* Bit 1 */

#define DMA_CFGR5_MSIZE                          ((uint16_t)0x0C00) /* MSIZE[1:0] bits (Memory size) */
#define DMA_CFGR5_MSIZE_0                        ((uint16_t)0x0400) /* Bit 0 */
#define DMA_CFGR5_MSIZE_1                        ((uint16_t)0x0800) /* Bit 1 */

#define DMA_CFGR5_PL                             ((uint16_t)0x3000) /* PL[1:0] bits (Channel Priority level) */
#define DMA_CFGR5_PL_0                           ((uint16_t)0x1000) /* Bit 0 */
#define DMA_CFGR5_PL_1                           ((uint16_t)0x2000) /* Bit 1 */

#define DMA_CFGR5_MEM2MEM                        ((uint16_t)0x4000) /* Memory to memory mode enable */

/*******************  Bit definition for DMA_CFG6 register  *******************/
#define DMA_CFGR6_EN                             ((uint16_t)0x0001) /* Channel enable */
#define DMA_CFGR6_TCIE                           ((uint16_t)0x0002) /* Transfer complete interrupt enable */
#define DMA_CFGR6_HTIE                           ((uint16_t)0x0004) /* Half Transfer interrupt enable */
#define DMA_CFGR6_TEIE                           ((uint16_t)0x0008) /* Transfer error interrupt enable */
#define DMA_CFGR6_DIR                            ((uint16_t)0x0010) /* Data transfer direction */
#define DMA_CFGR6_CIRC                           ((uint16_t)0x0020) /* Circular mode */
#define DMA_CFGR6_PINC                           ((uint16_t)0x0040) /* Peripheral increment mode */
#define DMA_CFGR6_MINC                           ((uint16_t)0x0080) /* Memory increment mode */

#define DMA_CFGR6_PSIZE                          ((uint16_t)0x0300) /* PSIZE[1:0] bits (Peripheral size) */
#define DMA_CFGR6_PSIZE_0                        ((uint16_t)0x0100) /* Bit 0 */
#define DMA_CFGR6_PSIZE_1                        ((uint16_t)0x0200) /* Bit 1 */

#define DMA_CFGR6_MSIZE                          ((uint16_t)0x0C00) /* MSIZE[1:0] bits (Memory size) */
#define DMA_CFGR6_MSIZE_0                        ((uint16_t)0x0400) /* Bit 0 */
#define DMA_CFGR6_MSIZE_1                        ((uint16_t)0x0800) /* Bit 1 */

#define DMA_CFGR6_PL                             ((uint16_t)0x3000) /* PL[1:0] bits (Channel Priority level) */
#define DMA_CFGR6_PL_0                           ((uint16_t)0x1000) /* Bit 0 */
#define DMA_CFGR6_PL_1                           ((uint16_t)0x2000) /* Bit 1 */

#define DMA_CFGR6_MEM2MEM                        ((uint16_t)0x4000) /* Memory to memory mode */

/*******************  Bit definition for DMA_CFG7 register  *******************/
#define DMA_CFGR7_EN                             ((uint16_t)0x0001) /* Channel enable */
#define DMA_CFGR7_TCIE                           ((uint16_t)0x0002) /* Transfer complete interrupt enable */
#define DMA_CFGR7_HTIE                           ((uint16_t)0x0004) /* Half Transfer interrupt enable */
#define DMA_CFGR7_TEIE                           ((uint16_t)0x0008) /* Transfer error interrupt enable */
#define DMA_CFGR7_DIR                            ((uint16_t)0x0010) /* Data transfer direction */
#define DMA_CFGR7_CIRC                           ((uint16_t)0x0020) /* Circular mode */
#define DMA_CFGR7_PINC                           ((uint16_t)0x0040) /* Peripheral increment mode */
#define DMA_CFGR7_MINC                           ((uint16_t)0x0080) /* Memory increment mode */

#define DMA_CFGR7_PSIZE                          ((uint16_t)0x0300) /* PSIZE[1:0] bits (Peripheral size) */
#define DMA_CFGR7_PSIZE_0                        ((uint16_t)0x0100) /* Bit 0 */
#define DMA_CFGR7_PSIZE_1                        ((uint16_t)0x0200) /* Bit 1 */

#define DMA_CFGR7_MSIZE                          ((uint16_t)0x0C00) /* MSIZE[1:0] bits (Memory size) */
#define DMA_CFGR7_MSIZE_0                        ((uint16_t)0x0400) /* Bit 0 */
#define DMA_CFGR7_MSIZE_1                        ((uint16_t)0x0800) /* Bit 1 */

#define DMA_CFGR7_PL                             ((uint16_t)0x3000) /* PL[1:0] bits (Channel Priority level) */
#define DMA_CFGR7_PL_0                           ((uint16_t)0x1000) /* Bit 0 */
#define DMA_CFGR7_PL_1                           ((uint16_t)0x2000) /* Bit 1 */

#define DMA_CFGR7_MEM2MEM                        ((uint16_t)0x4000) /* Memory to memory mode enable */

/******************  Bit definition for DMA_CNTR1 register  ******************/
#define DMA_CNTR1_NDT                           ((uint16_t)0xFFFF) /* Number of data to Transfer */

/******************  Bit definition for DMA_CNTR2 register  ******************/
#define DMA_CNTR2_NDT                           ((uint16_t)0xFFFF) /* Number of data to Transfer */

/******************  Bit definition for DMA_CNTR3 register  ******************/
#define DMA_CNTR3_NDT                           ((uint16_t)0xFFFF) /* Number of data to Transfer */

/******************  Bit definition for DMA_CNTR4 register  ******************/
#define DMA_CNTR4_NDT                           ((uint16_t)0xFFFF) /* Number of data to Transfer */

/******************  Bit definition for DMA_CNTR5 register  ******************/
#define DMA_CNTR5_NDT                           ((uint16_t)0xFFFF) /* Number of data to Transfer */

/******************  Bit definition for DMA_CNTR6 register  ******************/
#define DMA_CNTR6_NDT                           ((uint16_t)0xFFFF) /* Number of data to Transfer */

/******************  Bit definition for DMA_CNTR7 register  ******************/
#define DMA_CNTR7_NDT                           ((uint16_t)0xFFFF) /* Number of data to Transfer */

/******************  Bit definition for DMA_PADDR1 register  *******************/
#define DMA_PADDR1_PA                           ((uint32_t)0xFFFFFFFF) /* Peripheral Address */

/******************  Bit definition for DMA_PADDR2 register  *******************/
#define DMA_PADDR2_PA                           ((uint32_t)0xFFFFFFFF) /* Peripheral Address */

/******************  Bit definition for DMA_PADDR3 register  *******************/
#define DMA_PADDR3_PA                           ((uint32_t)0xFFFFFFFF) /* Peripheral Address */

/******************  Bit definition for DMA_PADDR4 register  *******************/
#define DMA_PADDR4_PA                           ((uint32_t)0xFFFFFFFF) /* Peripheral Address */

/******************  Bit definition for DMA_PADDR5 register  *******************/
#define DMA_PADDR5_PA                           ((uint32_t)0xFFFFFFFF) /* Peripheral Address */

/******************  Bit definition for DMA_PADDR6 register  *******************/
#define DMA_PADDR6_PA                           ((uint32_t)0xFFFFFFFF) /* Peripheral Address */

/******************  Bit definition for DMA_PADDR7 register  *******************/
#define DMA_PADDR7_PA                           ((uint32_t)0xFFFFFFFF) /* Peripheral Address */

/******************  Bit definition for DMA_MADDR1 register  *******************/
#define DMA_MADDR1_MA                           ((uint32_t)0xFFFFFFFF) /* Memory Address */

/******************  Bit definition for DMA_MADDR2 register  *******************/
#define DMA_MADDR2_MA                           ((uint32_t)0xFFFFFFFF) /* Memory Address */

/******************  Bit definition for DMA_MADDR3 register  *******************/
#define DMA_MADDR3_MA                           ((uint32_t)0xFFFFFFFF) /* Memory Address */

/******************  Bit definition for DMA_MADDR4 register  *******************/
#define DMA_MADDR4_MA                           ((uint32_t)0xFFFFFFFF) /* Memory Address */

/******************  Bit definition for DMA_MADDR5 register  *******************/
#define DMA_MADDR5_MA                           ((uint32_t)0xFFFFFFFF) /* Memory Address */

/******************  Bit definition for DMA_MADDR6 register  *******************/
#define DMA_MADDR6_MA                           ((uint32_t)0xFFFFFFFF) /* Memory Address */

/******************  Bit definition for DMA_MADDR7 register  *******************/
#define DMA_MADDR7_MA                           ((uint32_t)0xFFFFFFFF) /* Memory Address */

/******************************************************************************/
/*                    External Interrupt/Event Controller                     */
/******************************************************************************/

/*******************  Bit definition for EXTI_INTENR register  *******************/
#define EXTI_INTENR_MR0                         ((uint32_t)0x00000001) /* Interrupt Mask on line 0 */
#define EXTI_INTENR_MR1                         ((uint32_t)0x00000002) /* Interrupt Mask on line 1 */
#define EXTI_INTENR_MR2                         ((uint32_t)0x00000004) /* Interrupt Mask on line 2 */
#define EXTI_INTENR_MR3                         ((uint32_t)0x00000008) /* Interrupt Mask on line 3 */
#define EXTI_INTENR_MR4                         ((uint32_t)0x00000010) /* Interrupt Mask on line 4 */
#define EXTI_INTENR_MR5                         ((uint32_t)0x00000020) /* Interrupt Mask on line 5 */
#define EXTI_INTENR_MR6                         ((uint32_t)0x00000040) /* Interrupt Mask on line 6 */
#define EXTI_INTENR_MR7                         ((uint32_t)0x00000080) /* Interrupt Mask on line 7 */
#define EXTI_INTENR_MR8                         ((uint32_t)0x00000100) /* Interrupt Mask on line 8 */
#define EXTI_INTENR_MR9                         ((uint32_t)0x00000200) /* Interrupt Mask on line 9 */

/*******************  Bit definition for EXTI_EVENR register  *******************/
#define EXTI_EVENR_MR0                          ((uint32_t)0x00000001) /* Event Mask on line 0 */
#define EXTI_EVENR_MR1                          ((uint32_t)0x00000002) /* Event Mask on line 1 */
#define EXTI_EVENR_MR2                          ((uint32_t)0x00000004) /* Event Mask on line 2 */
#define EXTI_EVENR_MR3                          ((uint32_t)0x00000008) /* Event Mask on line 3 */
#define EXTI_EVENR_MR4                          ((uint32_t)0x00000010) /* Event Mask on line 4 */
#define EXTI_EVENR_MR5                          ((uint32_t)0x00000020) /* Event Mask on line 5 */
#define EXTI_EVENR_MR6                          ((uint32_t)0x00000040) /* Event Mask on line 6 */
#define EXTI_EVENR_MR7                          ((uint32_t)0x00000080) /* Event Mask on line 7 */
#define EXTI_EVENR_MR8                          ((uint32_t)0x00000100) /* Event Mask on line 8 */
#define EXTI_EVENR_MR9                          ((uint32_t)0x00000200) /* Event Mask on line 9 */

/******************  Bit definition for EXTI_RTENR register  *******************/
#define EXTI_RTENR_TR0                          ((uint32_t)0x00000001) /* Rising trigger event configuration bit of line 0 */
#define EXTI_RTENR_TR1                          ((uint32_t)0x00000002) /* Rising trigger event configuration bit of line 1 */
#define EXTI_RTENR_TR2                          ((uint32_t)0x00000004) /* Rising trigger event configuration bit of line 2 */
#define EXTI_RTENR_TR3                          ((uint32_t)0x00000008) /* Rising trigger event configuration bit of line 3 */
#define EXTI_RTENR_TR4                          ((uint32_t)0x00000010) /* Rising trigger event configuration bit of line 4 */
#define EXTI_RTENR_TR5                          ((uint32_t)0x00000020) /* Rising trigger event configuration bit of line 5 */
#define EXTI_RTENR_TR6                          ((uint32_t)0x00000040) /* Rising trigger event configuration bit of line 6 */
#define EXTI_RTENR_TR7                          ((uint32_t)0x00000080) /* Rising trigger event configuration bit of line 7 */
#define EXTI_RTENR_TR8                          ((uint32_t)0x00000100) /* Rising trigger event configuration bit of line 8 */
#define EXTI_RTENR_TR9                          ((uint32_t)0x00000200) /* Rising trigger event configuration bit of line 9 */

/******************  Bit definition for EXTI_FTENR register  *******************/
#define EXTI_FTENR_TR0                          ((uint32_t)0x00000001) /* Falling trigger event configuration bit of line 0 */
#define EXTI_FTENR_TR1                          ((uint32_t)0x00000002) /* Falling trigger event configuration bit of line 1 */
#define EXTI_FTENR_TR2                          ((uint32_t)0x00000004) /* Falling trigger event configuration bit of line 2 */
#define EXTI_FTENR_TR3                          ((uint32_t)0x00000008) /* Falling trigger event configuration bit of line 3 */
#define EXTI_FTENR_TR4                          ((uint32_t)0x00000010) /* Falling trigger event configuration bit of line 4 */
#define EXTI_FTENR_TR5                          ((uint32_t)0x00000020) /* Falling trigger event configuration bit of line 5 */
#define EXTI_FTENR_TR6                          ((uint32_t)0x00000040) /* Falling trigger event configuration bit of line 6 */
#define EXTI_FTENR_TR7                          ((uint32_t)0x00000080) /* Falling trigger event configuration bit of line 7 */
#define EXTI_FTENR_TR8                          ((uint32_t)0x00000100) /* Falling trigger event configuration bit of line 8 */
#define EXTI_FTENR_TR9                          ((uint32_t)0x00000200) /* Falling trigger event configuration bit of line 9 */

/******************  Bit definition for EXTI_SWIEVR register  ******************/
#define EXTI_SWIEVR_SWIEVR0                     ((uint32_t)0x00000001) /* Software Interrupt on line 0 */
#define EXTI_SWIEVR_SWIEVR1                     ((uint32_t)0x00000002) /* Software Interrupt on line 1 */
#define EXTI_SWIEVR_SWIEVR2                     ((uint32_t)0x00000004) /* Software Interrupt on line 2 */
#define EXTI_SWIEVR_SWIEVR3                     ((uint32_t)0x00000008) /* Software Interrupt on line 3 */
#define EXTI_SWIEVR_SWIEVR4                     ((uint32_t)0x00000010) /* Software Interrupt on line 4 */
#define EXTI_SWIEVR_SWIEVR5                     ((uint32_t)0x00000020) /* Software Interrupt on line 5 */
#define EXTI_SWIEVR_SWIEVR6                     ((uint32_t)0x00000040) /* Software Interrupt on line 6 */
#define EXTI_SWIEVR_SWIEVR7                     ((uint32_t)0x00000080) /* Software Interrupt on line 7 */
#define EXTI_SWIEVR_SWIEVR8                     ((uint32_t)0x00000100) /* Software Interrupt on line 8 */
#define EXTI_SWIEVR_SWIEVR9                     ((uint32_t)0x00000200) /* Software Interrupt on line 9 */

/*******************  Bit definition for EXTI_INTFR register  ********************/
#define EXTI_INTF_INTF0                         ((uint32_t)0x00000001) /* Pending bit for line 0 */
#define EXTI_INTF_INTF1                         ((uint32_t)0x00000002) /* Pending bit for line 1 */
#define EXTI_INTF_INTF2                         ((uint32_t)0x00000004) /* Pending bit for line 2 */
#define EXTI_INTF_INTF3                         ((uint32_t)0x00000008) /* Pending bit for line 3 */
#define EXTI_INTF_INTF4                         ((uint32_t)0x00000010) /* Pending bit for line 4 */
#define EXTI_INTF_INTF5                         ((uint32_t)0x00000020) /* Pending bit for line 5 */
#define EXTI_INTF_INTF6                         ((uint32_t)0x00000040) /* Pending bit for line 6 */
#define EXTI_INTF_INTF7                         ((uint32_t)0x00000080) /* Pending bit for line 7 */
#define EXTI_INTF_INTF8                         ((uint32_t)0x00000100) /* Pending bit for line 8 */
#define EXTI_INTF_INTF9                         ((uint32_t)0x00000200) /* Pending bit for line 9 */

/******************************************************************************/
/*                      FLASH and Option Bytes Registers                      */
/******************************************************************************/

/*******************  Bit definition for FLASH_ACTLR register  ******************/
#define FLASH_ACTLR_LATENCY                     ((uint32_t)0x03) /* LATENCY[1:0] bits (Latency) */
/* unlike all other register bit fields, this one is not defined by bits, but by values! */
#define FLASH_ACTLR_LATENCY_0                   ((uint32_t)0x00) /* Value 0 */
#define FLASH_ACTLR_LATENCY_1                   ((uint32_t)0x01) /* Value 1 */
#define FLASH_ACTLR_LATENCY_2                   ((uint32_t)0x02) /* Value 2 */


/******************  Bit definition for FLASH_KEYR register  ******************/
#define FLASH_KEYR_FKEYR                        ((uint32_t)0xFFFFFFFF) /* FPEC Key */

/*****************  Bit definition for FLASH_OBKEYR register  ****************/
#define FLASH_OBKEYR_OBKEYR                     ((uint32_t)0xFFFFFFFF) /* Option Byte Key */

/******************  Bit definition for FLASH_STATR register  *******************/
#define FLASH_STATR_BSY                         ((uint32_t)0x00000001) /* Busy */
#define FLASH_STATR_WRPRTERR                    ((uint32_t)0x00000010) /* Write Protection Error */
#define FLASH_STATR_EOP                         ((uint32_t)0x00000020) /* End of operation */
#define FLASH_STATR_FWAKE_FLAG                  ((uint32_t)0x00000040) /* FLASH wakeup flag */
#define FLASH_STATR_TURBO                       ((uint32_t)0x00000080) /* TURBO mode enabled */
#define FLASH_STATR_BOOT_AVA                    ((uint32_t)0x00001000) /* Initiaslizes the configuration word status */
#define FLASH_STATR_BOOT_STATUS                 ((uint32_t)0x00002000) /* The source of the currently executed program */
#define FLASH_STATR_BOOT_MODE                   ((uint32_t)0x00004000) /* Combined with BOOT_AVA, the switch betten user area and BOOT area can be controlled */
#define FLASH_STATR_BOOT_LOCK                   ((uint32_t)0x00008000) /* BOOT lock */

/*******************  Bit definition for FLASH_CTLR register  *******************/
#define FLASH_CTLR_PER                          ((uint32_t)0x00000002) /* Page Erase 1KByte*/
#define FLASH_CTLR_MER                          ((uint32_t)0x00000004) /* Mass Erase */
#define FLASH_CTLR_OBER                         ((uint32_t)0x00000020) /* Option Byte Erase */
#define FLASH_CTLR_STRT                         ((uint32_t)0x00000040) /* Start */
#define FLASH_CTLR_LOCK                         ((uint32_t)0x00000080) /* Lock */
#define FLASH_CTLR_OBWRE                        ((uint32_t)0x00000200) /* Option Bytes Write Enable */
#define FLASH_CTLR_ERRIE                        ((uint32_t)0x00000400) /* Error Interrupt Enable */
#define FLASH_CTLR_EOPIE                        ((uint32_t)0x00001000) /* End of operation interrupt enable */
#define FLASH_CTLR_FWAKEIE                      ((uint32_t)0x00002000) /* Wakeup interrupt enable */
#define FLASH_CTLR_FLOCK                        ((uint32_t)0x00008000) /* Fast programming lock */
#define FLASH_CTLR_PAGE_FTPG                    ((uint32_t)0x00010000) /* Fast Page Programming 256 byte */
#define FLASH_CTLR_PAGE_FTER                    ((uint32_t)0x00020000) /* Fast Page Erase 256Byte */
#define FLASH_CTLR_BUF_LOAD                     ((uint32_t)0x00040000) /* Buffer Load */
#define FLASH_CTLR_BUF_RST                      ((uint32_t)0x00080000) /* Buffer Reset */
#define FLASH_CTLR_BER32                        ((uint32_t)0x00800000) /* Fast block erase 32KB 0x08000000-0x08007FFF */

/*******************  Bit definition for FLASH_ADDR register  *******************/
#define FLASH_ADDR_FAR                          ((uint32_t)0xFFFFFFFF) /* Flash Address */

/******************  Bit definition for FLASH_OBR register  *******************/
#define FLASH_OBR_OBERR                         ((uint32_t)0x00000001) /* Option Byte Error */
#define FLASH_OBR_RDPRT                         ((uint32_t)0x00000002) /* Read protection */
#define FLASH_OBR_USER                          ((uint32_t)0x000000FC) /* User Option Bits */
#define FLASH_OBR_USER_IWDG_SW                  ((uint32_t)0x00000004) /* WDG_SW */
#define FLASH_OBR_USER_STANDY_RST               ((uint32_t)0x00000010) /* The system is reset and controlled in standby mode */
#define FLASH_OBR_USER_RST_MODE                 ((uint32_t)0x00000060) /* Reset Mode bits - Configure byte reset delay time */
#define FLASH_OBR_USER_STATR_MODE               ((uint32_t)0x00000080) /* Power-on startup mode */
#define FLASH_OBR_DATA0                         ((uint32_t)0x0003FC00) /* DATA0 of user bytes */
#define FLASH_OBR_DATA1                         ((uint32_t)0x03FC0000) /* DATA1 of user bytes */

/******************  Bit definition for FLASH_WPR register  ******************/
#define FLASH_WPR_WRP                           ((uint32_t)0xFFFFFFFF) /* Write Protect */

/******************  Bit definition for FLASH_MODEKEYR register  ******************/
#define FLASH_MODEKEYR_MODEKEYR                 ((uint32_t)0xFFFFFFFF) /* Flash programming mode Key */

/******************  Bit definition for FLASH_MODEKEYP register  ******************/
#define FLASH_MODEKEYP_MODEKEYR                 ((uint32_t)0xFFFFFFFF) /* BOOT programming mode Key */

/******************  Bit definition for FLASH_RDPR register  *******************/
#define FLASH_RDPR_RDPR                         ((uint32_t)0x000000FF) /* Read protection option byte */
#define FLASH_RDPR_nRDPR                        ((uint32_t)0x0000FF00) /* Read protection complemented option byte */

/******************  Bit definition for FLASH_USER register  ******************/
#define FLASH_USER_USER                         ((uint32_t)0x00FF0000) /* User option byte */
#define FLASH_USER_nUSER                        ((uint32_t)0xFF000000) /* User complemented option byte */

/******************  Bit definition for FLASH_Data0 register  *****************/
#define FLASH_Data0_Data0                       ((uint32_t)0x000000FF) /* User data storage option byte */
#define FLASH_Data0_nData0                      ((uint32_t)0x0000FF00) /* User data storage complemented option byte */

/******************  Bit definition for FLASH_Data1 register  *****************/
#define FLASH_Data1_Data1                       ((uint32_t)0x00FF0000) /* User data storage option byte */
#define FLASH_Data1_nData1                      ((uint32_t)0xFF000000) /* User data storage complemented option byte */

/******************  Bit definition for FLASH_WRPR0 register  ******************/
#define FLASH_WRPR0_WRPR0                       ((uint32_t)0x000000FF) /* Flash memory write protection option bytes */
#define FLASH_WRPR0_nWRPR0                      ((uint32_t)0x0000FF00) /* Flash memory write protection complemented option bytes */

/******************  Bit definition for FLASH_WRPR1 register  ******************/
#define FLASH_WRPR1_WRPR1                       ((uint32_t)0x00FF0000) /* Flash memory write protection option bytes */
#define FLASH_WRPR1_nWRPR1                      ((uint32_t)0xFF000000) /* Flash memory write protection complemented option bytes */

/******************  Bit definition for FLASH_WRPR2 register  ******************/
#define FLASH_WRPR1_WRPR2                       ((uint32_t)0x00FF0000) /* Flash memory write protection option bytes */
#define FLASH_WRPR1_nWRPR2                      ((uint32_t)0xFF000000) /* Flash memory write protection complemented option bytes */

/******************  Bit definition for FLASH_WRPR3 register  ******************/
#define FLASH_WRPR1_WRPR3                       ((uint32_t)0x00FF0000) /* Flash memory write protection option bytes */
#define FLASH_WRPR1_nWRPR3                      ((uint32_t)0xFF000000) /* Flash memory write protection complemented option bytes */


/******************************************************************************/
/*                General Purpose and Alternate Function I/O                  */
/******************************************************************************/

/*******************  Bit definition for GPIO_CFGLR register  *******************/
#define GPIO_CFGLR_MODE                         ((uint32_t)0x11111111) /* Port x mode bits */

#define GPIO_CFGLR_MODE0                        ((uint32_t)0x00000003) /* MODE0[1:0] bits (Port x mode bits, pin 0) */
#define GPIO_CFGLR_MODE0_0                      ((uint32_t)0x00000001) /* Bit 0 */

#define GPIO_CFGLR_MODE1                        ((uint32_t)0x00000030) /* MODE1[1:0] bits (Port x mode bits, pin 1) */
#define GPIO_CFGLR_MODE1_0                      ((uint32_t)0x00000010) /* Bit 0 */

#define GPIO_CFGLR_MODE2                        ((uint32_t)0x00000300) /* MODE2[1:0] bits (Port x mode bits, pin 2) */
#define GPIO_CFGLR_MODE2_0                      ((uint32_t)0x00000100) /* Bit 0 */

#define GPIO_CFGLR_MODE3                        ((uint32_t)0x00003000) /* MODE3[1:0] bits (Port x mode bits, pin 3) */
#define GPIO_CFGLR_MODE3_0                      ((uint32_t)0x00001000) /* Bit 0 */

#define GPIO_CFGLR_MODE4                        ((uint32_t)0x00030000) /* MODE4[1:0] bits (Port x mode bits, pin 4) */
#define GPIO_CFGLR_MODE4_0                      ((uint32_t)0x00010000) /* Bit 0 */

#define GPIO_CFGLR_MODE5                        ((uint32_t)0x00300000) /* MODE5[1:0] bits (Port x mode bits, pin 5) */
#define GPIO_CFGLR_MODE5_0                      ((uint32_t)0x00100000) /* Bit 0 */

#define GPIO_CFGLR_MODE6                        ((uint32_t)0x03000000) /* MODE6[1:0] bits (Port x mode bits, pin 6) */
#define GPIO_CFGLR_MODE6_0                      ((uint32_t)0x01000000) /* Bit 0 */

#define GPIO_CFGLR_MODE7                        ((uint32_t)0x30000000) /* MODE7[1:0] bits (Port x mode bits, pin 7) */
#define GPIO_CFGLR_MODE7_0                      ((uint32_t)0x10000000) /* Bit 0 */

#define GPIO_CFGLR_CNF                          ((uint32_t)0xCCCCCCCC) /* Port x configuration bits */

#define GPIO_CFGLR_CNF0                         ((uint32_t)0x0000000C) /* CNF0[1:0] bits (Port x configuration bits, pin 0) */
#define GPIO_CFGLR_CNF0_0                       ((uint32_t)0x00000004) /* Bit 0 */
#define GPIO_CFGLR_CNF0_1                       ((uint32_t)0x00000008) /* Bit 1 */

#define GPIO_CFGLR_CNF1                         ((uint32_t)0x000000C0) /* CNF1[1:0] bits (Port x configuration bits, pin 1) */
#define GPIO_CFGLR_CNF1_0                       ((uint32_t)0x00000040) /* Bit 0 */
#define GPIO_CFGLR_CNF1_1                       ((uint32_t)0x00000080) /* Bit 1 */

#define GPIO_CFGLR_CNF2                         ((uint32_t)0x00000C00) /* CNF2[1:0] bits (Port x configuration bits, pin 2) */
#define GPIO_CFGLR_CNF2_0                       ((uint32_t)0x00000400) /* Bit 0 */
#define GPIO_CFGLR_CNF2_1                       ((uint32_t)0x00000800) /* Bit 1 */

#define GPIO_CFGLR_CNF3                         ((uint32_t)0x0000C000) /* CNF3[1:0] bits (Port x configuration bits, pin 3) */
#define GPIO_CFGLR_CNF3_0                       ((uint32_t)0x00004000) /* Bit 0 */
#define GPIO_CFGLR_CNF3_1                       ((uint32_t)0x00008000) /* Bit 1 */

#define GPIO_CFGLR_CNF4                         ((uint32_t)0x000C0000) /* CNF4[1:0] bits (Port x configuration bits, pin 4) */
#define GPIO_CFGLR_CNF4_0                       ((uint32_t)0x00040000) /* Bit 0 */
#define GPIO_CFGLR_CNF4_1                       ((uint32_t)0x00080000) /* Bit 1 */

#define GPIO_CFGLR_CNF5                         ((uint32_t)0x00C00000) /* CNF5[1:0] bits (Port x configuration bits, pin 5) */
#define GPIO_CFGLR_CNF5_0                       ((uint32_t)0x00400000) /* Bit 0 */
#define GPIO_CFGLR_CNF5_1                       ((uint32_t)0x00800000) /* Bit 1 */

#define GPIO_CFGLR_CNF6                         ((uint32_t)0x0C000000) /* CNF6[1:0] bits (Port x configuration bits, pin 6) */
#define GPIO_CFGLR_CNF6_0                       ((uint32_t)0x04000000) /* Bit 0 */
#define GPIO_CFGLR_CNF6_1                       ((uint32_t)0x08000000) /* Bit 1 */

#define GPIO_CFGLR_CNF7                         ((uint32_t)0xC0000000) /* CNF7[1:0] bits (Port x configuration bits, pin 7) */
#define GPIO_CFGLR_CNF7_0                       ((uint32_t)0x40000000) /* Bit 0 */
#define GPIO_CFGLR_CNF7_1                       ((uint32_t)0x80000000) /* Bit 1 */

/*******************  Bit definition for GPIO_INDR register  *******************/
#define GPIO_INDR_IDR0                          ((uint32_t)0x00000001) /* Port input data, bit 0 */
#define GPIO_INDR_IDR1                          ((uint32_t)0x00000002) /* Port input data, bit 1 */
#define GPIO_INDR_IDR2                          ((uint32_t)0x00000004) /* Port input data, bit 2 */
#define GPIO_INDR_IDR3                          ((uint32_t)0x00000008) /* Port input data, bit 3 */
#define GPIO_INDR_IDR4                          ((uint32_t)0x00000010) /* Port input data, bit 4 */
#define GPIO_INDR_IDR5                          ((uint32_t)0x00000020) /* Port input data, bit 5 */
#define GPIO_INDR_IDR6                          ((uint32_t)0x00000040) /* Port input data, bit 6 */
#define GPIO_INDR_IDR7                          ((uint32_t)0x00000080) /* Port input data, bit 7 */

/*******************  Bit definition for GPIO_OUTDR register  *******************/
#define GPIO_OUTDR_ODR0                         ((uint32_t)0x00000001) /* Port output data, bit 0 */
#define GPIO_OUTDR_ODR1                         ((uint32_t)0x00000002) /* Port output data, bit 1 */
#define GPIO_OUTDR_ODR2                         ((uint32_t)0x00000004) /* Port output data, bit 2 */
#define GPIO_OUTDR_ODR3                         ((uint32_t)0x00000008) /* Port output data, bit 3 */
#define GPIO_OUTDR_ODR4                         ((uint32_t)0x00000010) /* Port output data, bit 4 */
#define GPIO_OUTDR_ODR5                         ((uint32_t)0x00000020) /* Port output data, bit 5 */
#define GPIO_OUTDR_ODR6                         ((uint32_t)0x00000040) /* Port output data, bit 6 */
#define GPIO_OUTDR_ODR7                         ((uint32_t)0x00000080) /* Port output data, bit 7 */

/******************  Bit definition for GPIO_BSHR register  *******************/
#define GPIO_BSHR_BS0                           ((uint32_t)0x00000001) /* Port x Set bit 0 */
#define GPIO_BSHR_BS1                           ((uint32_t)0x00000002) /* Port x Set bit 1 */
#define GPIO_BSHR_BS2                           ((uint32_t)0x00000004) /* Port x Set bit 2 */
#define GPIO_BSHR_BS3                           ((uint32_t)0x00000008) /* Port x Set bit 3 */
#define GPIO_BSHR_BS4                           ((uint32_t)0x00000010) /* Port x Set bit 4 */
#define GPIO_BSHR_BS5                           ((uint32_t)0x00000020) /* Port x Set bit 5 */
#define GPIO_BSHR_BS6                           ((uint32_t)0x00000040) /* Port x Set bit 6 */
#define GPIO_BSHR_BS7                           ((uint32_t)0x00000080) /* Port x Set bit 7 */

#define GPIO_BSHR_BR0                           ((uint32_t)0x00010000) /* Port x Reset bit 0 */
#define GPIO_BSHR_BR1                           ((uint32_t)0x00020000) /* Port x Reset bit 1 */
#define GPIO_BSHR_BR2                           ((uint32_t)0x00040000) /* Port x Reset bit 2 */
#define GPIO_BSHR_BR3                           ((uint32_t)0x00080000) /* Port x Reset bit 3 */
#define GPIO_BSHR_BR4                           ((uint32_t)0x00100000) /* Port x Reset bit 4 */
#define GPIO_BSHR_BR5                           ((uint32_t)0x00200000) /* Port x Reset bit 5 */
#define GPIO_BSHR_BR6                           ((uint32_t)0x00400000) /* Port x Reset bit 6 */
#define GPIO_BSHR_BR7                           ((uint32_t)0x00800000) /* Port x Reset bit 7 */

/*******************  Bit definition for GPIO_BCR register  *******************/
#define GPIO_BCR_BR0                            ((uint32_t)0x00000001) /* Port x Reset bit 0 */
#define GPIO_BCR_BR1                            ((uint32_t)0x00000002) /* Port x Reset bit 1 */
#define GPIO_BCR_BR2                            ((uint32_t)0x00000004) /* Port x Reset bit 2 */
#define GPIO_BCR_BR3                            ((uint32_t)0x00000008) /* Port x Reset bit 3 */
#define GPIO_BCR_BR4                            ((uint32_t)0x00000010) /* Port x Reset bit 4 */
#define GPIO_BCR_BR5                            ((uint32_t)0x00000020) /* Port x Reset bit 5 */
#define GPIO_BCR_BR6                            ((uint32_t)0x00000040) /* Port x Reset bit 6 */
#define GPIO_BCR_BR7                            ((uint32_t)0x00000080) /* Port x Reset bit 7 */

/******************  Bit definition for GPIO_LCKR register  *******************/
#define GPIO_LCK0                               ((uint32_t)0x00000001) /* Port x Lock bit 0 */
#define GPIO_LCK1                               ((uint32_t)0x00000002) /* Port x Lock bit 1 */
#define GPIO_LCK2                               ((uint32_t)0x00000004) /* Port x Lock bit 2 */
#define GPIO_LCK3                               ((uint32_t)0x00000008) /* Port x Lock bit 3 */
#define GPIO_LCK4                               ((uint32_t)0x00000010) /* Port x Lock bit 4 */
#define GPIO_LCK5                               ((uint32_t)0x00000020) /* Port x Lock bit 5 */
#define GPIO_LCK6                               ((uint32_t)0x00000040) /* Port x Lock bit 6 */
#define GPIO_LCK7                               ((uint32_t)0x00000080) /* Port x Lock bit 7 */
#define GPIO_LCKK                               ((uint32_t)0x00010000) /* Lock key */

/******************  Bit definition for AFIO_PCFR1register  *******************/
#define AFIO_PCFR1_SPI1_RM                      ((uint32_t)0x00000007) /* SPI1 remapping */
#define AFIO_PCFR1_SPI1_RM_0                    ((uint32_t)0x00000001) /* SPI1 remapping */
#define AFIO_PCFR1_SPI1_RM_1                    ((uint32_t)0x00000002) /* SPI1 remapping */
#define AFIO_PCFR1_SPI1_RM_2                    ((uint32_t)0x00000004) /* SPI1 remapping */
#define AFIO_PCFR1_I2C1_RM                      ((uint32_t)0x00000038) /* I2C1 remapping */
#define AFIO_PCFR1_I2C1_RM_0                    ((uint32_t)0x00000008) /* I2C1 remapping */
#define AFIO_PCFR1_I2C1_RM_1                    ((uint32_t)0x00000010) /* I2C1 remapping */
#define AFIO_PCFR1_I2C1_RM_2                    ((uint32_t)0x00000020) /* I2C1 remapping */
#define AFIO_PCFR1_USART1_RM                    ((uint32_t)0x000003C0) /* USART1 remapping */
#define AFIO_PCFR1_USART1_RM_0                  ((uint32_t)0x00000040) /* USART1 remapping */
#define AFIO_PCFR1_USART1_RM_1                  ((uint32_t)0x00000080) /* USART1 remapping */
#define AFIO_PCFR1_USART1_RM_2                  ((uint32_t)0x00000100) /* USART1 remapping */
#define AFIO_PCFR1_USART1_RM_3                  ((uint32_t)0x00000200) /* USART1 remapping */
#define AFIO_PCFR1_TIM1_RM                      ((uint32_t)0x00003C00) /* TIM1_REMAP remapping */
#define AFIO_PCFR1_TIM1_RM_0                    ((uint32_t)0x00000400) /* TIM1_REMAP remapping */
#define AFIO_PCFR1_TIM1_RM_1                    ((uint32_t)0x00000800) /* TIM1_REMAP remapping */
#define AFIO_PCFR1_TIM1_RM_2                    ((uint32_t)0x00001000) /* TIM1_REMAP remapping */
#define AFIO_PCFR1_TIM1_RM_3                    ((uint32_t)0x00002000) /* TIM1_REMAP remapping */
#define AFIO_PCFR1_TIM2_RM                      ((uint32_t)0x0001C000) /* TIM2_REMAP remapping */
#define AFIO_PCFR1_TIM2_RM_0                    ((uint32_t)0x00004000) /* TIM2_REMAP remapping */
#define AFIO_PCFR1_TIM2_RM_1                    ((uint32_t)0x00008000) /* TIM2_REMAP remapping */
#define AFIO_PCFR1_TIM2_RM_2                    ((uint32_t)0x00010000) /* TIM2_REMAP remapping */
#define AFIO_PCFR1_PA12_RM                      ((uint32_t)0x00020000) /* PA1/2 OSC_IN/OSC_OUT remapping */
#define AFIO_PCFR1_ADC_ETRGINJ_RM               ((uint32_t)0x00040000) /* ADC external triggered injection remapping */
#define AFIO_PCFR1_ADC_ETRGREG_RM               ((uint32_t)0x00080000) /* ADC external triggered rule remapping */
#define AFIO_PCFR1_USART2_RM                    ((uint32_t)0x00700000) /* USART2 remapping */
#define AFIO_PCFR1_USART2_RM_0                  ((uint32_t)0x00100000) /* USART2 remapping */
#define AFIO_PCFR1_USART2_RM_1                  ((uint32_t)0x00200000) /* USART2 remapping */
#define AFIO_PCFR1_USART2_RM_2                  ((uint32_t)0x00400000) /* USART2 remapping */
#define AFIO_PCFR1_SWCFG                        ((uint32_t)0x07000000) /* SWCFG  SDI or GPIO configuration (Please reconsider your design if you find yourself setting this bit) */
#define AFIO_PCFR1_SWCFG_0                      ((uint32_t)0x01000000) /* SWCFG  SDI or GPIO configuration Bit 0 (Please reconsider your design if you find yourself setting this bit)*/
#define AFIO_PCFR1_SWCFG_1                      ((uint32_t)0x02000000) /* SWCFG  SDI or GPIO configuration Bit 1 (Please reconsider your design if you find yourself setting this bit)*/
#define AFIO_PCFR1_SWCFG_2                      ((uint32_t)0x04000000) /* SWCFG  SDI or GPIO configuration Bit 2 (Please reconsider your design if you find yourself setting this bit)*/

/*****************  Bit definition for AFIO_EXTICR register  *****************/
#define AFIO_EXTICR_EXTI0                       ((uint32_t)0x00000003) /* EXTI 0 configuration */
#define AFIO_EXTICR_EXTI1                       ((uint32_t)0x0000000C) /* EXTI 1 configuration */
#define AFIO_EXTICR_EXTI2                       ((uint32_t)0x00000030) /* EXTI 2 configuration */
#define AFIO_EXTICR_EXTI3                       ((uint32_t)0x000000C0) /* EXTI 3 configuration */
#define AFIO_EXTICR_EXTI4                       ((uint32_t)0x00000300) /* EXTI 4 configuration */
#define AFIO_EXTICR_EXTI5                       ((uint32_t)0x00000C00) /* EXTI 5 configuration */
#define AFIO_EXTICR_EXTI6                       ((uint32_t)0x00003000) /* EXTI 6 configuration */
#define AFIO_EXTICR_EXTI7                       ((uint32_t)0x0000C000) /* EXTI 7 configuration */

#define AFIO_EXTICR_EXTI0_PA                    ((uint32_t)0x00000000) /* PA[0] pin */
#define AFIO_EXTICR_EXTI0_PB                    ((uint32_t)0x00000001) /* PB[0] pin */
#define AFIO_EXTICR_EXTI0_PC                    ((uint32_t)0x00000002) /* PC[0] pin */
#define AFIO_EXTICR_EXTI0_PD                    ((uint32_t)0x00000003) /* PD[0] pin */
#define AFIO_EXTICR_EXTI1_PA                    ((uint32_t)0x00000000) /* PA[1] pin */
#define AFIO_EXTICR_EXTI1_PB                    ((uint32_t)0x00000004) /* PB[1] pin */
#define AFIO_EXTICR_EXTI1_PC                    ((uint32_t)0x00000008) /* PC[1] pin */
#define AFIO_EXTICR_EXTI1_PD                    ((uint32_t)0x0000000C) /* PD[1] pin */
#define AFIO_EXTICR_EXTI2_PA                    ((uint32_t)0x00000000) /* PA[2] pin */
#define AFIO_EXTICR_EXTI2_PB                    ((uint32_t)0x00000010) /* PB[2] pin */
#define AFIO_EXTICR_EXTI2_PC                    ((uint32_t)0x00000020) /* PC[2] pin */
#define AFIO_EXTICR_EXTI2_PD                    ((uint32_t)0x00000030) /* PD[2] pin */
#define AFIO_EXTICR_EXTI3_PA                    ((uint32_t)0x00000000) /* PA[3] pin */
#define AFIO_EXTICR_EXTI3_PB                    ((uint32_t)0x00000040) /* PB[3] pin */
#define AFIO_EXTICR_EXTI3_PC                    ((uint32_t)0x00000080) /* PC[3] pin */
#define AFIO_EXTICR_EXTI3_PD                    ((uint32_t)0x000000C0) /* PD[3] pin */
#define AFIO_EXTICR_EXTI4_PA                    ((uint32_t)0x00000000) /* PA[4] pin */
#define AFIO_EXTICR_EXTI4_PB                    ((uint32_t)0x00000100) /* PB[4] pin */
#define AFIO_EXTICR_EXTI4_PC                    ((uint32_t)0x00000200) /* PC[4] pin */
#define AFIO_EXTICR_EXTI4_PD                    ((uint32_t)0x00000300) /* PD[4] pin */
#define AFIO_EXTICR_EXTI5_PA                    ((uint32_t)0x00000000) /* PA[5] pin */
#define AFIO_EXTICR_EXTI5_PB                    ((uint32_t)0x00000400) /* PB[5] pin */
#define AFIO_EXTICR_EXTI5_PC                    ((uint32_t)0x00000800) /* PC[5] pin */
#define AFIO_EXTICR_EXTI5_PD                    ((uint32_t)0x00000C00) /* PD[5] pin */
#define AFIO_EXTICR_EXTI6_PA                    ((uint32_t)0x00000000) /* PA[6] pin */
#define AFIO_EXTICR_EXTI6_PB                    ((uint32_t)0x00001000) /* PB[6] pin */
#define AFIO_EXTICR_EXTI6_PC                    ((uint32_t)0x00002000) /* PC[6] pin */
#define AFIO_EXTICR_EXTI6_PD                    ((uint32_t)0x00003000) /* PD[6] pin */
#define AFIO_EXTICR_EXTI7_PA                    ((uint32_t)0x00000000) /* PA[7] pin */
#define AFIO_EXTICR_EXTI7_PB                    ((uint32_t)0x00004000) /* PB[7] pin */
#define AFIO_EXTICR_EXTI7_PC                    ((uint32_t)0x00008000) /* PC[7] pin */
#define AFIO_EXTICR_EXTI7_PD                    ((uint32_t)0x0000C000) /* PD[7] pin */


/******************************************************************************/
/*                           Independent WATCHDOG                             */
/******************************************************************************/

/*******************  Bit definition for IWDG_CTLR register  ********************/
#define IWDG_KEY                                ((uint16_t)0xFFFF) /* Key value (write only, read 0000h) */

/*******************  Bit definition for IWDG_PSCR register  ********************/
#define IWDG_PR                                 ((uint8_t)0x07) /* PR[2:0] (Prescaler divider) */
#define IWDG_PR_0                               ((uint8_t)0x01) /* Bit 0 */
#define IWDG_PR_1                               ((uint8_t)0x02) /* Bit 1 */
#define IWDG_PR_2                               ((uint8_t)0x04) /* Bit 2 */

/*******************  Bit definition for IWDG_RLDR register  *******************/
#define IWDG_RL                                 ((uint16_t)0x0FFF) /* Watchdog counter reload value */

/*******************  Bit definition for IWDG_STATR register  ********************/
#define IWDG_PVU                                ((uint8_t)0x01) /* Watchdog prescaler value update */
#define IWDG_RVU                                ((uint8_t)0x02) /* Watchdog counter reload value update */

/******************************************************************************/
/*                      Inter-integrated Circuit Interface                    */
/******************************************************************************/

/*******************  Bit definition for I2C_CTLR1 register  ********************/
#define I2C_CTLR1_PE                            ((uint16_t)0x0001) /* Peripheral Enable */
#define I2C_CTLR1_ENPEC                         ((uint16_t)0x0020) /* PEC Enable */
#define I2C_CTLR1_ENGC                          ((uint16_t)0x0040) /* General Call Enable */
#define I2C_CTLR1_NOSTRETCH                     ((uint16_t)0x0080) /* Clock Stretching Disable (Slave mode) */
#define I2C_CTLR1_START                         ((uint16_t)0x0100) /* Start Generation */
#define I2C_CTLR1_STOP                          ((uint16_t)0x0200) /* Stop Generation */
#define I2C_CTLR1_ACK                           ((uint16_t)0x0400) /* Acknowledge Enable */
#define I2C_CTLR1_POS                           ((uint16_t)0x0800) /* Acknowledge/PEC Position (for data reception) */
#define I2C_CTLR1_PEC                           ((uint16_t)0x1000) /* Packet Error Checking */
#define I2C_CTLR1_SWRST                         ((uint16_t)0x8000) /* Software Reset */

/*******************  Bit definition for I2C_CTLR2 register  ********************/
#define I2C_CTLR2_FREQ                          ((uint16_t)0x003F) /* FREQ[5:0] bits (Peripheral Clock Frequency) */
#define I2C_CTLR2_FREQ_0                        ((uint16_t)0x0001) /* Bit 0 */
#define I2C_CTLR2_FREQ_1                        ((uint16_t)0x0002) /* Bit 1 */
#define I2C_CTLR2_FREQ_2                        ((uint16_t)0x0004) /* Bit 2 */
#define I2C_CTLR2_FREQ_3                        ((uint16_t)0x0008) /* Bit 3 */
#define I2C_CTLR2_FREQ_4                        ((uint16_t)0x0010) /* Bit 4 */
#define I2C_CTLR2_FREQ_5                        ((uint16_t)0x0020) /* Bit 5 */

#define I2C_CTLR2_ITERREN                       ((uint16_t)0x0100) /* Error Interrupt Enable */
#define I2C_CTLR2_ITEVTEN                       ((uint16_t)0x0200) /* Event Interrupt Enable */
#define I2C_CTLR2_ITBUFEN                       ((uint16_t)0x0400) /* Buffer Interrupt Enable */
#define I2C_CTLR2_DMAEN                         ((uint16_t)0x0800) /* DMA Requests Enable */
#define I2C_CTLR2_LAST                          ((uint16_t)0x1000) /* DMA Last Transfer */

/*******************  Bit definition for I2C_OADDR1 register  *******************/
#define I2C_OADDR1_ADD1_7                       ((uint16_t)0x00FE) /* Interface Address */
#define I2C_OADDR1_ADD8_9                       ((uint16_t)0x0300) /* Interface Address */

#define I2C_OADDR1_ADD0                         ((uint16_t)0x0001) /* Bit 0 */
#define I2C_OADDR1_ADD1                         ((uint16_t)0x0002) /* Bit 1 */
#define I2C_OADDR1_ADD2                         ((uint16_t)0x0004) /* Bit 2 */
#define I2C_OADDR1_ADD3                         ((uint16_t)0x0008) /* Bit 3 */
#define I2C_OADDR1_ADD4                         ((uint16_t)0x0010) /* Bit 4 */
#define I2C_OADDR1_ADD5                         ((uint16_t)0x0020) /* Bit 5 */
#define I2C_OADDR1_ADD6                         ((uint16_t)0x0040) /* Bit 6 */
#define I2C_OADDR1_ADD7                         ((uint16_t)0x0080) /* Bit 7 */
#define I2C_OADDR1_ADD8                         ((uint16_t)0x0100) /* Bit 8 */
#define I2C_OADDR1_ADD9                         ((uint16_t)0x0200) /* Bit 9 */

#define I2C_OADDR1_ADDMODE                      ((uint16_t)0x8000) /* Addressing Mode (Slave mode) */

/*******************  Bit definition for I2C_OADDR2 register  *******************/
#define I2C_OADDR2_ENDUAL                       ((uint8_t)0x01) /* Dual addressing mode enable */
#define I2C_OADDR2_ADD2                         ((uint8_t)0xFE) /* Interface address */

/********************  Bit definition for I2C_DATAR register  ********************/
#define I2C_DR_DATAR                            ((uint8_t)0xFF) /* 8-bit Data Register */

/*******************  Bit definition for I2C_STAR1 register  ********************/
#define I2C_STAR1_SB                            ((uint16_t)0x0001) /* Start Bit (Master mode) */
#define I2C_STAR1_ADDR                          ((uint16_t)0x0002) /* Address sent (master mode)/matched (slave mode) */
#define I2C_STAR1_BTF                           ((uint16_t)0x0004) /* Byte Transfer Finished */
#define I2C_STAR1_ADD10                         ((uint16_t)0x0008) /* 10-bit header sent (Master mode) */
#define I2C_STAR1_STOPF                         ((uint16_t)0x0010) /* Stop detection (Slave mode) */
#define I2C_STAR1_RXNE                          ((uint16_t)0x0040) /* Data Register not Empty (receivers) */
#define I2C_STAR1_TXE                           ((uint16_t)0x0080) /* Data Register Empty (transmitters) */
#define I2C_STAR1_BERR                          ((uint16_t)0x0100) /* Bus Error */
#define I2C_STAR1_ARLO                          ((uint16_t)0x0200) /* Arbitration Lost (master mode) */
#define I2C_STAR1_AF                            ((uint16_t)0x0400) /* Acknowledge Failure */
#define I2C_STAR1_OVR                           ((uint16_t)0x0800) /* Overrun/Underrun */
#define I2C_STAR1_PECERR                        ((uint16_t)0x1000) /* PEC Error in reception */

/*******************  Bit definition for I2C_STAR2 register  ********************/
#define I2C_STAR2_MSL                           ((uint16_t)0x0001) /* Master/Slave */
#define I2C_STAR2_BUSY                          ((uint16_t)0x0002) /* Bus Busy */
#define I2C_STAR2_TRA                           ((uint16_t)0x0004) /* Transmitter/Receiver */
#define I2C_STAR2_GENCALL                       ((uint16_t)0x0010) /* General Call Address (Slave mode) */
#define I2C_STAR2_DUALF                         ((uint16_t)0x0080) /* Dual Flag (Slave mode) */
#define I2C_STAR2_PEC                           ((uint16_t)0xFF00) /* Packet Error Checking Register */

/*******************  Bit definition for I2C_CKCFGR register  ********************/
#define I2C_CKCFGR_CCR                          ((uint16_t)0x0FFF) /* Clock Control Register in Fast/Standard mode (Master mode) */
#define I2C_CKCFGR_DUTY                         ((uint16_t)0x4000) /* Fast Mode Duty Cycle */
#define I2C_CKCFGR_FS                           ((uint16_t)0x8000) /* I2C Master Mode Selection */


/******************************************************************************/
/*                             Power Control                                  */
/******************************************************************************/

/********************  Bit definition for PWR_CTLR register  ********************/
#define PWR_CTLR_PDDS                           ((uint32_t)0x00000002) /* Power Down Deepsleep */
#define PWR_CTLR_LDOMODE                        ((uint32_t)0x0000000C) /* LDO mode setting */
#define PWR_CTLR_PVDE                           ((uint32_t)0x00000010) /* Power Voltage Detector Enable */
#define PWR_CTLR_PLS                            ((uint32_t)0x00000060) /* PLS[1:0] bits (PVD Level Selection) */
#define PWR_CTLR_PLS_0                          ((uint32_t)0x00000020) /* Bit 0 */
#define PWR_CTLR_PLS_1                          ((uint32_t)0x00000040) /* Bit 1 */
#define PWR_CTLR_FLASH_LP_REG                   ((uint32_t)0x00000200) /* FLASH low power mode config */
#define PWR_CTLR_FLASH_LP                       ((uint32_t)0x00000C00) /* FLASH low power mode config */

/*******************  Bit definition for PWR_CSR register  ********************/
#define PWR_CSR_PVDO                            ((uint16_t)0x0004) /* PVD Output */
#define PVD_MAX_THRESHOLD_LVL                   3

/******************************************************************************/
/*                         Reset and Clock Control                            */
/******************************************************************************/

/********************  Bit definition for RCC_CTLR register  ********************/
#define RCC_HSION                               ((uint32_t)0x00000001) /* Internal High Speed clock enable */
#define RCC_HSIRDY                              ((uint32_t)0x00000002) /* Internal High Speed clock ready flag */
#define RCC_HSITRIM                             ((uint32_t)0x000000F8) /* Internal High Speed clock trimming */
#define RCC_HSICAL                              ((uint32_t)0x0000FF00) /* Internal High Speed clock Calibration */
#define RCC_HSEON                               ((uint32_t)0x00010000) /* External High Speed clock enable */
#define RCC_HSERDY                              ((uint32_t)0x00020000) /* External High Speed clock ready flag */
#define RCC_HSEBYP                              ((uint32_t)0x00040000) /* External High Speed clock Bypass */
#define RCC_CSSON                               ((uint32_t)0x00080000) /* Clock Security System enable */
#define RCC_HSE_LP                              ((uint32_t)0x00100000) /* External Low Speed clock enable */
#define RCC_SYSCM_EN                            ((uint32_t)0x00200000) /* Systemclock monitoring module SCM enable */
#define RCC_HSE_SI                              ((uint32_t)0x00C00000) /* HSE current supply regulation value */
#define RCC_PLLON                               ((uint32_t)0x01000000) /* PLL enable */
#define RCC_PLLRDY                              ((uint32_t)0x02000000) /* PLL clock ready flag */


/*******************  Bit definition for RCC_CFGR0 register  *******************/
#define RCC_SW                                  ((uint32_t)0x00000003) /* SW[1:0] bits (System clock Switch) */
#define RCC_SW_0                                ((uint32_t)0x00000001) /* Bit 0 */
#define RCC_SW_1                                ((uint32_t)0x00000002) /* Bit 1 */

#define RCC_SW_HSI                              ((uint32_t)0x00000000) /* HSI selected as system clock */
#define RCC_SW_HSE                              ((uint32_t)0x00000001) /* HSE selected as system clock */
#define RCC_SW_PLL                              ((uint32_t)0x00000002) /* PLL selected as system clock */

#define RCC_SWS                                 ((uint32_t)0x0000000C) /* SWS[1:0] bits (System Clock Switch Status) */
#define RCC_SWS_0                               ((uint32_t)0x00000004) /* Bit 0 */
#define RCC_SWS_1                               ((uint32_t)0x00000008) /* Bit 1 */

#define RCC_SWS_HSI                             ((uint32_t)0x00000000) /* HSI oscillator used as system clock */
#define RCC_SWS_HSE                             ((uint32_t)0x00000004) /* HSE oscillator used as system clock */
#define RCC_SWS_PLL                             ((uint32_t)0x00000008) /* PLL used as system clock */

#define RCC_HPRE                                ((uint32_t)0x000000F0) /* HPRE[3:0] bits (AHB prescaler) */
#define RCC_HPRE_0                              ((uint32_t)0x00000010) /* Bit 0 */
#define RCC_HPRE_1                              ((uint32_t)0x00000020) /* Bit 1 */
#define RCC_HPRE_2                              ((uint32_t)0x00000040) /* Bit 2 */
#define RCC_HPRE_3                              ((uint32_t)0x00000080) /* Bit 3 */

#define RCC_HPRE_DIV1                           ((uint32_t)0x00000000) /* SYSCLK not divided */
#define RCC_HPRE_DIV2                           ((uint32_t)0x00000010) /* SYSCLK divided by 2 */
#define RCC_HPRE_DIV3                           ((uint32_t)0x00000020) /* SYSCLK divided by 3 */
#define RCC_HPRE_DIV4                           ((uint32_t)0x00000030) /* SYSCLK divided by 4 */
#define RCC_HPRE_DIV5                           ((uint32_t)0x00000040) /* SYSCLK divided by 5 */
#define RCC_HPRE_DIV6                           ((uint32_t)0x00000050) /* SYSCLK divided by 6 */
#define RCC_HPRE_DIV7                           ((uint32_t)0x00000060) /* SYSCLK divided by 7 */
#define RCC_HPRE_DIV8                           ((uint32_t)0x00000070) /* SYSCLK divided by 8 */
#define RCC_HPRE_DIV16                          ((uint32_t)0x000000B0) /* SYSCLK divided by 16 */
#define RCC_HPRE_DIV32                          ((uint32_t)0x000000C0) /* SYSCLK divided by 32 */
#define RCC_HPRE_DIV64                          ((uint32_t)0x000000D0) /* SYSCLK divided by 64 */
#define RCC_HPRE_DIV128                         ((uint32_t)0x000000E0) /* SYSCLK divided by 128 */
#define RCC_HPRE_DIV256                         ((uint32_t)0x000000F0) /* SYSCLK divided by 256 */

#define RCC_ADCPRE                              ((uint32_t)0x0000F800) /* ADCPRE[4:0] bits ADC prscaler */
#define RCC_ADCPRE_0                            ((uint32_t)0x00000800) /* Bit 0 */
#define RCC_ADCPRE_1                            ((uint32_t)0x00001000) /* Bit 1 */
#define RCC_ADCPRE_2                            ((uint32_t)0x00002000) /* Bit 2 */
#define RCC_ADCPRE_3                            ((uint32_t)0x00004000) /* Bit 3 */
#define RCC_ADCPRE_4                            ((uint32_t)0x00008000) /* Bit 4 */

#define RCC_ADCPRE_DIV2                         ((uint32_t)0x00000000) /* HBCLK divided by 2 */
/* includes                                                0x00000800,
                                                           0x00001000,
																													 0x00001800 */
#define RCC_ADCPRE_DIV4_1                       ((uint32_t)0x00002000) /* HBCLK divided by 4 */
#define RCC_ADCPRE_DIV8_2                       ((uint32_t)0x00002800) /* HBCLK divided by 8 */
#define RCC_ADCPRE_DIV16_2                      ((uint32_t)0x00003000) /* HBCLK divided by 16 */
#define RCC_ADCPRE_DIV32_2                      ((uint32_t)0x00003800) /* HBCLK divided by 32 */
#define RCC_ADCPRE_DIV4                         ((uint32_t)0x00004000) /* HBCLK divided by 4 */
/* includes                                                0x00004800,
                                                           0x00005000,
																													 0x00005800 */
#define RCC_ADCPRE_DIV8_1                       ((uint32_t)0x00006000) /* HBCLK divided by 8 */
#define RCC_ADCPRE_DIV16_1                      ((uint32_t)0x00006800) /* HBCLK divided by 16 */
#define RCC_ADCPRE_DIV32_1                      ((uint32_t)0x00007000) /* HBCLK divided by 32 */
#define RCC_ADCPRE_DIV64_1                      ((uint32_t)0x00007800) /* HBCLK divided by 64 */
#define RCC_ADCPRE_DIV6                         ((uint32_t)0x00008000) /* HBCLK divided by 6 */
/* includes                                                0x00008800,
                                                           0x00009000,
																													 0x00009800 */
#define RCC_ADCPRE_DIV12                        ((uint32_t)0x0000A000) /* HBCLK divided by 12 */
#define RCC_ADCPRE_DIV24                        ((uint32_t)0x0000A800) /* HBCLK divided by 24 */
#define RCC_ADCPRE_DIV48                        ((uint32_t)0x0000B000) /* HBCLK divided by 48 */
#define RCC_ADCPRE_DIV96                        ((uint32_t)0x0000B800) /* HBCLK divided by 96 */
#define RCC_ADCPRE_DIV8                         ((uint32_t)0x0000C000) /* HBCLK divided by 8 */
/* includes                                                0x0000C800,
                                                           0x0000D000,
																													 0x0000D800 */
#define RCC_ADCPRE_DIV16                        ((uint32_t)0x0000E000) /* HBCLK divided by 16 */
#define RCC_ADCPRE_DIV32                        ((uint32_t)0x0000E800) /* HBCLK divided by 32 */
#define RCC_ADCPRE_DIV64                        ((uint32_t)0x0000F000) /* HBCLK divided by 64 */
#define RCC_ADCPRE_DIV128                       ((uint32_t)0x0000F800) /* HBCLK divided by 128 */

#define RCC_PLLSRC                              ((uint32_t)0x00010000) /* PLL entry clock source */

//For compatibility with the v003
#define RCC_PLLSRC_HSI_Mul2                     ((uint32_t)0x00000000) /* HSI clock*2 selected as PLL entry clock source */
#define RCC_PLLSRC_HSE_Mul2                     ((uint32_t)0x00010000) /* HSE clock*2 selected as PLL entry clock source */

#define RCC_CFGR0_MCO                           ((uint32_t)0x07000000) /* MCO[2:0] bits (Microcontroller Clock Output) */
#define RCC_MCO_0                               ((uint32_t)0x01000000) /* Bit 0 */
#define RCC_MCO_1                               ((uint32_t)0x02000000) /* Bit 1 */
#define RCC_MCO_2                               ((uint32_t)0x04000000) /* Bit 2 */

#define RCC_MCO_NOCLOCK                         ((uint32_t)0x00000000) /* No clock */
#define RCC_CFGR0_MCO_SYSCLK                    ((uint32_t)0x04000000) /* System clock selected as MCO source */
#define RCC_CFGR0_MCO_HSI                       ((uint32_t)0x05000000) /* HSI clock selected as MCO source */
#define RCC_CFGR0_MCO_HSE                       ((uint32_t)0x06000000) /* HSE clock selected as MCO source  */
#define RCC_CFGR0_MCO_PLL                       ((uint32_t)0x07000000) /* PLL clock divided by 2 selected as MCO source */

#define RCC_CFGR0_ADC_CLK_ADJ                   ((uint32_t)0x10000000) /* ADC clock duty cycle adjustment */
#define RCC_CFGR0_ADC_CLK_MODE                  ((uint32_t)0x80000000) /* ADC clock mode */

/*******************  Bit definition for RCC_INTR register  ********************/
#define RCC_LSIRDYF                             ((uint32_t)0x00000001) /* LSI Ready Interrupt flag */
#define RCC_HSIRDYF                             ((uint32_t)0x00000004) /* HSI Ready Interrupt flag */
#define RCC_HSERDYF                             ((uint32_t)0x00000008) /* HSE Ready Interrupt flag */
#define RCC_PLLRDYF                             ((uint32_t)0x00000010) /* PLL Ready Interrupt flag */
#define RCC_CSSF                                ((uint32_t)0x00000080) /* Clock Security System Interrupt flag */
#define RCC_LSIRDYIE                            ((uint32_t)0x00000100) /* LSI Ready Interrupt Enable */
#define RCC_SYSCLK_FAILIE                       ((uint32_t)0x00000200) /* SYSCLK invalid interrupt enable bit */
#define RCC_HSIRDYIE                            ((uint32_t)0x00000400) /* HSI Ready Interrupt Enable */
#define RCC_HSERDYIE                            ((uint32_t)0x00000800) /* HSE Ready Interrupt Enable */
#define RCC_PLLRDYIE                            ((uint32_t)0x00001000) /* PLL Ready Interrupt Enable */
#define RCC_LSIRDYC                             ((uint32_t)0x00010000) /* LSI Ready Interrupt Clear */
#define RCC_HSIRDYC                             ((uint32_t)0x00040000) /* HSI Ready Interrupt Clear */
#define RCC_HSERDYC                             ((uint32_t)0x00080000) /* HSE Ready Interrupt Clear */
#define RCC_PLLRDYC                             ((uint32_t)0x00100000) /* PLL Ready Interrupt Clear */
#define RCC_CSSC                                ((uint32_t)0x00800000) /* Clock Security System Interrupt Clear */

/*****************  Bit definition for RCC_PB2PRSTR register  *****************/
#define RCC_AFIORST                             ((uint32_t)0x00000001) /* Alternate Function I/O reset */
#define RCC_IOPARST                             ((uint32_t)0x00000004) /* I/O port A reset */
#define RCC_IOPBRST                             ((uint32_t)0x00000008) /* I/O port B reset */
#define RCC_IOPCRST                             ((uint32_t)0x00000010) /* I/O port C reset */
#define RCC_IOPDRST                             ((uint32_t)0x00000020) /* I/O port D reset */
#define RCC_ADC1RST                             ((uint32_t)0x00000200) /* ADC 1 interface reset */
#define RCC_TIM1RST                             ((uint32_t)0x00000800) /* TIM1 Timer reset */
#define RCC_SPI1RST                             ((uint32_t)0x00001000) /* SPI 1 reset */
#define RCC_USART2RST                           ((uint32_t)0x00002000) /* USART2 reset */
#define RCC_USART1RST                           ((uint32_t)0x00004000) /* USART1 reset */

/*****************  Bit definition for RCC_PB1PRSTR register  *****************/
#define RCC_TIM2RST                             ((uint32_t)0x00000001) /* Timer 2 reset */
#define RCC_TIM3RST                             ((uint32_t)0x00000004) /* Timer 3 reset */
#define RCC_WWDGRST                             ((uint32_t)0x00000800) /* Window Watchdog reset */
#define RCC_I2C1RST                             ((uint32_t)0x00200000) /* I2C 1 reset */
#define RCC_PWRRST                              ((uint32_t)0x10000000) /* Power interface module reset */

/******************  Bit definition for RCC_HBPCENR register  ******************/
#define RCC_DMA1EN                              ((uint32_t)0x0001) /* DMA1 clock enable */
#define RCC_SRAMEN                              ((uint32_t)0x0004) /* SRAM interface clock enable */

/******************  Bit definition for RCC_PB2PCENR register  *****************/
#define RCC_AFIOEN                              ((uint32_t)0x00000001) /* Alternate Function I/O clock enable */
#define RCC_IOPAEN                              ((uint32_t)0x00000004) /* I/O port A clock enable */
#define RCC_IOPBEN                              ((uint32_t)0x00000008) /* I/O port B clock enable */
#define RCC_IOPCEN                              ((uint32_t)0x00000010) /* I/O port C clock enable */
#define RCC_IOPDEN                              ((uint32_t)0x00000020) /* I/O port D clock enable */
#define RCC_ADCEN                               ((uint32_t)0x00000200) /* ADC 1 interface clock enable */
#define RCC_TIM1EN                              ((uint32_t)0x00000800) /* TIM1 Timer clock enable */
#define RCC_SPI1EN                              ((uint32_t)0x00001000) /* SPI 1 clock enable */
#define RCC_USART2EN                            ((uint32_t)0x00002000) /* USART2 clock enable */
#define RCC_USART1EN                            ((uint32_t)0x00004000) /* USART1 clock enable */

/*****************  Bit definition for RCC_PB1PCENR register  ******************/
#define RCC_TIM2EN                              ((uint32_t)0x00000001) /* Timer 2 clock enabled*/
#define RCC_TIM3EN                              ((uint32_t)0x00000002) /* Timer 3 clock enable */
#define RCC_WWDGEN                              ((uint32_t)0x00000800) /* Window Watchdog clock enable */
#define RCC_I2C1EN                              ((uint32_t)0x00200000) /* I2C 1 clock enable */
#define RCC_PWREN                               ((uint32_t)0x10000000) /* Power interface clock enable */

/*******************  Bit definition for RCC_RSTSCKR register  ********************/
#define RCC_LSION                               ((uint32_t)0x00000001) /* Internal Low Speed oscillator enable */
#define RCC_LSIRDY                              ((uint32_t)0x00000002) /* Internal Low Speed oscillator Ready */
#define RCC_SYSCLK_FAILIF                       ((uint32_t)0x00000100) /* System clock failure flag */
#define RCC_ADCRSTF                             ((uint32_t)0x00800000) /* ADC reset flag */
#define RCC_RMVF                                ((uint32_t)0x01000000) /* Remove reset flag */
#define RCC_OPCMRSTF                            ((uint32_t)0x02000000) /* OPA_CMP reset flag */
#define RCC_PINRSTF                             ((uint32_t)0x04000000) /* Externam manual reset pin flag */
#define RCC_PORRSTF                             ((uint32_t)0x08000000) /* POR/PDR reset flag */
#define RCC_SFTRSTF                             ((uint32_t)0x10000000) /* Software Reset flag */
#define RCC_IWDGRSTF                            ((uint32_t)0x20000000) /* Independent Watchdog reset flag */
#define RCC_WWDGRSTF                            ((uint32_t)0x40000000) /* Window watchdog reset flag */

/******************************************************************************/
/*                        Serial Peripheral Interface                         */
/******************************************************************************/

/*******************  Bit definition for SPI_CTLR1 register  ********************/
#define SPI_CTLR1_CPHA                          ((uint16_t)0x0001) /* Clock Phase */
#define SPI_CTLR1_CPOL                          ((uint16_t)0x0002) /* Clock Polarity */
#define SPI_CTLR1_MSTR                          ((uint16_t)0x0004) /* Master Selection */

#define SPI_CTLR1_BR                            ((uint16_t)0x0038) /* BR[2:0] bits (Baud Rate Control) */
#define SPI_CTLR1_BR_0                          ((uint16_t)0x0008) /* Bit 0 */
#define SPI_CTLR1_BR_1                          ((uint16_t)0x0010) /* Bit 1 */
#define SPI_CTLR1_BR_2                          ((uint16_t)0x0020) /* Bit 2 */

#define SPI_CTLR1_SPE                           ((uint16_t)0x0040) /* SPI Enable */
#define SPI_CTLR1_LSBFIRST                      ((uint16_t)0x0080) /* LSB or MSB first */
#define SPI_CTLR1_SSI                           ((uint16_t)0x0100) /* Internal slave select */
#define SPI_CTLR1_SSM                           ((uint16_t)0x0200) /* Software slave management */
#define SPI_CTLR1_RXONLY                        ((uint16_t)0x0400) /* Receive only */
#define SPI_CTLR1_DFF                           ((uint16_t)0x0800) /* Data Frame Format */
#define SPI_CTLR1_CRCNEXT                       ((uint16_t)0x1000) /* Transmit CRC next */
#define SPI_CTLR1_CRCEN                         ((uint16_t)0x2000) /* Hardware CRC calculation enable */
#define SPI_CTLR1_BIDIOE                        ((uint16_t)0x4000) /* Output enable in bidirectional mode */
#define SPI_CTLR1_BIDIMODE                      ((uint16_t)0x8000) /* Bidirectional data mode enable */

/*******************  Bit definition for SPI_CTLR2 register  ********************/
#define SPI_CTLR2_RXDMAEN                       ((uint8_t)0x01) /* Rx Buffer DMA Enable */
#define SPI_CTLR2_TXDMAEN                       ((uint8_t)0x02) /* Tx Buffer DMA Enable */
#define SPI_CTLR2_SSOE                          ((uint8_t)0x04) /* SS Output Enable */
#define SPI_CTLR2_ERRIE                         ((uint8_t)0x20) /* Error Interrupt Enable */
#define SPI_CTLR2_RXNEIE                        ((uint8_t)0x40) /* RX buffer Not Empty Interrupt Enable */
#define SPI_CTLR2_TXEIE                         ((uint8_t)0x80) /* Tx buffer Empty Interrupt Enable */

/********************  Bit definition for SPI_STATR register  ********************/
#define SPI_STATR_RXNE                          ((uint8_t)0x01) /* Receive buffer Not Empty */
#define SPI_STATR_TXE                           ((uint8_t)0x02) /* Transmit buffer Empty */
#define SPI_STATR_CRCERR                        ((uint8_t)0x10) /* CRC Error flag */
#define SPI_STATR_MODF                          ((uint8_t)0x20) /* Mode fault */
#define SPI_STATR_OVR                           ((uint8_t)0x40) /* Overrun flag */
#define SPI_STATR_BSY                           ((uint8_t)0x80) /* Busy flag */

/********************  Bit definition for SPI_DATAR register  ********************/
#define SPI_DATAR_DR                            ((uint16_t)0xFFFF) /* Data Register */

/*******************  Bit definition for SPI_CRCR register  ******************/
#define SPI_CRCR_CRCPOLY                        ((uint16_t)0xFFFF) /* CRC polynomial register */

/******************  Bit definition for SPI_RCRCR register  ******************/
#define SPI_RCRCR_RXCRC                         ((uint16_t)0xFFFF) /* Rx CRC Register */

/******************  Bit definition for SPI_TCRCR register  ******************/
#define SPI_TCRCR_TXCRC                         ((uint16_t)0xFFFF) /* Tx CRC Register */

/******************  Bit definition for SPI_HSCR register  ******************/
#define SPI_HSCR_HSRXEN                         ((uint16_t)0x0001) /* Read enable in SPI high-speed mode */

/******************************************************************************/
/*                                    TIM1                                    */
/******************************************************************************/

/*******************  Bit definition for TIM1_CTLR1 register  *****************/
#define TIM1_CTLR1_CEN                          ((uint16_t)0x0001) /* Counter enable */
#define TIM1_CTLR1_UDIS                         ((uint16_t)0x0002) /* Update disable */
#define TIM1_CTLR1_URS                          ((uint16_t)0x0004) /* Update request source */
#define TIM1_CTLR1_OPM                          ((uint16_t)0x0008) /* One pulse mode */
#define TIM1_CTLR1_DIR                          ((uint16_t)0x0010) /* Direction */
#define TIM1_CTLR1_CMS                          ((uint16_t)0x0060) /* CMS[1:0] bits (Center-aligned mode selection) */
#define TIM1_CTLR1_CMS_0                        ((uint16_t)0x0020) /* Bit 0 */
#define TIM1_CTLR1_CMS_1                        ((uint16_t)0x0040) /* Bit 1 */

#define TIM1_CTLR1_ARPE                         ((uint16_t)0x0080) /* Auto-reload preload enable */
#define TIM1_CTLR1_CKD                          ((uint16_t)0x0300) /* CKD[1:0] bits (clock division) */
#define TIM1_CTLR1_CKD_0                        ((uint16_t)0x0100) /* Bit 0 */
#define TIM1_CTLR1_CKD_1                        ((uint16_t)0x0200) /* Bit 1 */

#define TIM1_CTLR1_CAPOV                        ((uint16_t)0x4000) /* Capture value mode configuration */
#define TIM1_CTLR1_CAPLVL                       ((uint16_t)0x8000) /* Capture level indication enable in dual-edge capture */

/*******************  Bit definition for TIM1_CTLR2 register  *****************/
#define TIM1_CTLR2_CCPC                         ((uint16_t)0x0001) /* Capture/Compare Preloaded Control */
#define TIM1_CTLR2_CCUS                         ((uint16_t)0x0004) /* Capture/Compare Control Update Selection */
#define TIM1_CTLR2_CCDS                         ((uint16_t)0x0008) /* Capture/Compare DMA Selection */
#define TIM1_CTLR2_MMS                          ((uint16_t)0x0070) /* MMS[2:0] bits (Master Mode Selection) */
#define TIM1_CTLR2_MMS_0                        ((uint16_t)0x0010) /* Bit 0 */
#define TIM1_CTLR2_MMS_1                        ((uint16_t)0x0020) /* Bit 1 */
#define TIM1_CTLR2_MMS_2                        ((uint16_t)0x0040) /* Bit 2 */
#define TIM1_CTLR2_TI1S                         ((uint16_t)0x0080) /* TI1 Selection */
#define TIM1_CTLR2_OIS1                         ((uint16_t)0x0100) /* Output Idle state 1 (OC1 output) */
#define TIM1_CTLR2_OIS1N                        ((uint16_t)0x0200) /* Output Idle state 1 (OC1N output) */
#define TIM1_CTLR2_OIS2                         ((uint16_t)0x0400) /* Output Idle state 2 (OC2 output) */
#define TIM1_CTLR2_OIS2N                        ((uint16_t)0x0800) /* Output Idle state 2 (OC2N output) */
#define TIM1_CTLR2_OIS3                         ((uint16_t)0x1000) /* Output Idle state 3 (OC3 output) */
#define TIM1_CTLR2_OIS3N                        ((uint16_t)0x2000) /* Output Idle state 3 (OC3N output) */
#define TIM1_CTLR2_OIS4                         ((uint16_t)0x4000) /* Output Idle state 4 (OC4 output) */

/*******************  Bit definition for TIM1_SMCFGR register  ****************/
#define TIM1_SMCFGR_SMS                         ((uint16_t)0x0007) /* SMS[2:0] bits (Slave mode selection) */
#define TIM1_SMCFGR_SMS_0                       ((uint16_t)0x0001) /* Bit 0 */
#define TIM1_SMCFGR_SMS_1                       ((uint16_t)0x0002) /* Bit 1 */
#define TIM1_SMCFGR_SMS_2                       ((uint16_t)0x0004) /* Bit 2 */

#define TIM1_SMCFGR_TS                          ((uint16_t)0x0070) /* TS[2:0] bits (Trigger selection) */
#define TIM1_SMCFGR_TS_0                        ((uint16_t)0x0010) /* Bit 0 */
#define TIM1_SMCFGR_TS_1                        ((uint16_t)0x0020) /* Bit 1 */
#define TIM1_SMCFGR_TS_2                        ((uint16_t)0x0040) /* Bit 2 */

#define TIM1_SMCFGR_MSM                         ((uint16_t)0x0080) /* Master/slave mode */

#define TIM1_SMCFGR_ETF                         ((uint16_t)0x0F00) /* ETF[3:0] bits (External trigger filter) */
#define TIM1_SMCFGR_ETF_0                       ((uint16_t)0x0100) /* Bit 0 */
#define TIM1_SMCFGR_ETF_1                       ((uint16_t)0x0200) /* Bit 1 */
#define TIM1_SMCFGR_ETF_2                       ((uint16_t)0x0400) /* Bit 2 */
#define TIM1_SMCFGR_ETF_3                       ((uint16_t)0x0800) /* Bit 3 */

#define TIM1_SMCFGR_ETPS                        ((uint16_t)0x3000) /* ETPS[1:0] bits (External trigger prescaler) */
#define TIM1_SMCFGR_ETPS_0                      ((uint16_t)0x1000) /* Bit 0 */
#define TIM1_SMCFGR_ETPS_1                      ((uint16_t)0x2000) /* Bit 1 */

#define TIM1_SMCFGR_ECE                         ((uint16_t)0x4000) /* External clock enable */
#define TIM1_SMCFGR_ETP                         ((uint16_t)0x8000) /* External trigger polarity */

/*******************  Bit definition for TIM1_DMAINTENR register  *************/
#define TIM1_DMAINTENR_UIE                      ((uint16_t)0x0001) /* Update interrupt enable */
#define TIM1_DMAINTENR_CC1IE                    ((uint16_t)0x0002) /* Capture/Compare 1 interrupt enable */
#define TIM1_DMAINTENR_CC2IE                    ((uint16_t)0x0004) /* Capture/Compare 2 interrupt enable */
#define TIM1_DMAINTENR_CC3IE                    ((uint16_t)0x0008) /* Capture/Compare 3 interrupt enable */
#define TIM1_DMAINTENR_CC4IE                    ((uint16_t)0x0010) /* Capture/Compare 4 interrupt enable */
#define TIM1_DMAINTENR_COMIE                    ((uint16_t)0x0020) /* COM interrupt enable */
#define TIM1_DMAINTENR_TIE                      ((uint16_t)0x0040) /* Trigger interrupt enable */
#define TIM1_DMAINTENR_BIE                      ((uint16_t)0x0080) /* Break interrupt enable */
#define TIM1_DMAINTENR_UDE                      ((uint16_t)0x0100) /* Update DMA request enable */
#define TIM1_DMAINTENR_CC1DE                    ((uint16_t)0x0200) /* Capture/Compare 1 DMA request enable */
#define TIM1_DMAINTENR_CC2DE                    ((uint16_t)0x0400) /* Capture/Compare 2 DMA request enable */
#define TIM1_DMAINTENR_CC3DE                    ((uint16_t)0x0800) /* Capture/Compare 3 DMA request enable */
#define TIM1_DMAINTENR_CC4DE                    ((uint16_t)0x1000) /* Capture/Compare 4 DMA request enable */
#define TIM1_DMAINTENR_COMDE                    ((uint16_t)0x2000) /* COM DMA request enable */
#define TIM1_DMAINTENR_TDE                      ((uint16_t)0x4000) /* Trigger DMA request enable */

/*******************  Bit definition for TIM1_INTFR register  *****************/
#define TIM1_INTFR_UIF                          ((uint16_t)0x0001) /* Update interrupt Flag */
#define TIM1_INTFR_CC1IF                        ((uint16_t)0x0002) /* Capture/Compare 1 interrupt Flag */
#define TIM1_INTFR_CC2IF                        ((uint16_t)0x0004) /* Capture/Compare 2 interrupt Flag */
#define TIM1_INTFR_CC3IF                        ((uint16_t)0x0008) /* Capture/Compare 3 interrupt Flag */
#define TIM1_INTFR_CC4IF                        ((uint16_t)0x0010) /* Capture/Compare 4 interrupt Flag */
#define TIM1_INTFR_COMIF                        ((uint16_t)0x0020) /* COM interrupt Flag */
#define TIM1_INTFR_TIF                          ((uint16_t)0x0040) /* Trigger interrupt Flag */
#define TIM1_INTFR_BIF                          ((uint16_t)0x0080) /* Break interrupt Flag */
#define TIM1_INTFR_CC1OF                        ((uint16_t)0x0200) /* Capture/Compare 1 Overcapture Flag */
#define TIM1_INTFR_CC2OF                        ((uint16_t)0x0400) /* Capture/Compare 2 Overcapture Flag */
#define TIM1_INTFR_CC3OF                        ((uint16_t)0x0800) /* Capture/Compare 3 Overcapture Flag */
#define TIM1_INTFR_CC4OF                        ((uint16_t)0x1000) /* Capture/Compare 4 Overcapture Flag */

/*******************  Bit definition for TIM1_SWEVGR register  ****************/
#define TIM1_SWEVGR_UG                          ((uint16_t)0x0001) /* Update Generation */
#define TIM1_SWEVGR_CC1G                        ((uint16_t)0x0002) /* Capture/Compare 1 Generation */
#define TIM1_SWEVGR_CC2G                        ((uint16_t)0x0004) /* Capture/Compare 2 Generation */
#define TIM1_SWEVGR_CC3G                        ((uint16_t)0x0008) /* Capture/Compare 3 Generation */
#define TIM1_SWEVGR_CC4G                        ((uint16_t)0x0010) /* Capture/Compare 4 Generation */
#define TIM1_SWEVGR_COMG                        ((uint16_t)0x0020) /* Capture/Compare Control Update Generation */
#define TIM1_SWEVGR_TG                          ((uint16_t)0x0040) /* Trigger Generation */
#define TIM1_SWEVGR_BG                          ((uint16_t)0x0080) /* Break Generation */

/******************  Bit definition for TIM1_CHCTLR1 register  ****************/
#define TIM1_CHCTLR1_CC1S                       ((uint16_t)0x0003) /* CC1S[1:0] bits (Capture/Compare 1 Selection) */
#define TIM1_CHCTLR1_CC1S_0                     ((uint16_t)0x0001) /* Bit 0 */
#define TIM1_CHCTLR1_CC1S_1                     ((uint16_t)0x0002) /* Bit 1 */

#define TIM1_CHCTLR1_OC1FE                      ((uint16_t)0x0004) /* Output Compare 1 Fast enable */
#define TIM1_CHCTLR1_OC1PE                      ((uint16_t)0x0008) /* Output Compare 1 Preload enable */

#define TIM1_CHCTLR1_OC1M                       ((uint16_t)0x0070) /* OC1M[2:0] bits (Output Compare 1 Mode) */
#define TIM1_CHCTLR1_OC1M_0                     ((uint16_t)0x0010) /* Bit 0 */
#define TIM1_CHCTLR1_OC1M_1                     ((uint16_t)0x0020) /* Bit 1 */
#define TIM1_CHCTLR1_OC1M_2                     ((uint16_t)0x0040) /* Bit 2 */

#define TIM1_CHCTLR1_OC1CE                      ((uint16_t)0x0080) /* Output Compare 1Clear Enable */

#define TIM1_CHCTLR1_CC2S                       ((uint16_t)0x0300) /* CC2S[1:0] bits (Capture/Compare 2 Selection) */
#define TIM1_CHCTLR1_CC2S_0                     ((uint16_t)0x0100) /* Bit 0 */
#define TIM1_CHCTLR1_CC2S_1                     ((uint16_t)0x0200) /* Bit 1 */

#define TIM1_CHCTLR1_OC2FE                      ((uint16_t)0x0400) /* Output Compare 2 Fast enable */
#define TIM1_CHCTLR1_OC2PE                      ((uint16_t)0x0800) /* Output Compare 2 Preload enable */

#define TIM1_CHCTLR1_OC2M                       ((uint16_t)0x7000) /* OC2M[2:0] bits (Output Compare 2 Mode) */
#define TIM1_CHCTLR1_OC2M_0                     ((uint16_t)0x1000) /* Bit 0 */
#define TIM1_CHCTLR1_OC2M_1                     ((uint16_t)0x2000) /* Bit 1 */
#define TIM1_CHCTLR1_OC2M_2                     ((uint16_t)0x4000) /* Bit 2 */

#define TIM1_CHCTLR1_OC2CE                      ((uint16_t)0x8000) /* Output Compare 2 Clear Enable */

#define TIM1_CHCTLR1_IC1PSC                     ((uint16_t)0x000C) /* IC1PSC[1:0] bits (Input Capture 1 Prescaler) */
#define TIM1_CHCTLR1_IC1PSC_0                   ((uint16_t)0x0004) /* Bit 0 */
#define TIM1_CHCTLR1_IC1PSC_1                   ((uint16_t)0x0008) /* Bit 1 */

#define TIM1_CHCTLR1_IC1F                       ((uint16_t)0x00F0) /* IC1F[3:0] bits (Input Capture 1 Filter) */
#define TIM1_CHCTLR1_IC1F_0                     ((uint16_t)0x0010) /* Bit 0 */
#define TIM1_CHCTLR1_IC1F_1                     ((uint16_t)0x0020) /* Bit 1 */
#define TIM1_CHCTLR1_IC1F_2                     ((uint16_t)0x0040) /* Bit 2 */
#define TIM1_CHCTLR1_IC1F_3                     ((uint16_t)0x0080) /* Bit 3 */

#define TIM1_CHCTLR1_IC2PSC                     ((uint16_t)0x0C00) /* IC2PSC[1:0] bits (Input Capture 2 Prescaler) */
#define TIM1_CHCTLR1_IC2PSC_0                   ((uint16_t)0x0400) /* Bit 0 */
#define TIM1_CHCTLR1_IC2PSC_1                   ((uint16_t)0x0800) /* Bit 1 */

#define TIM1_CHCTLR1_IC2F                       ((uint16_t)0xF000) /* IC2F[3:0] bits (Input Capture 2 Filter) */
#define TIM1_CHCTLR1_IC2F_0                     ((uint16_t)0x1000) /* Bit 0 */
#define TIM1_CHCTLR1_IC2F_1                     ((uint16_t)0x2000) /* Bit 1 */
#define TIM1_CHCTLR1_IC2F_2                     ((uint16_t)0x4000) /* Bit 2 */
#define TIM1_CHCTLR1_IC2F_3                     ((uint16_t)0x8000) /* Bit 3 */

/******************  Bit definition for TIM1_CHCTLR2 register  ****************/
#define TIM1_CHCTLR2_CC3S                       ((uint16_t)0x0003) /* CC3S[1:0] bits (Capture/Compare 3 Selection) */
#define TIM1_CHCTLR2_CC3S_0                     ((uint16_t)0x0001) /* Bit 0 */
#define TIM1_CHCTLR2_CC3S_1                     ((uint16_t)0x0002) /* Bit 1 */

#define TIM1_CHCTLR2_OC3FE                      ((uint16_t)0x0004) /* Output Compare 3 Fast enable */
#define TIM1_CHCTLR2_OC3PE                      ((uint16_t)0x0008) /* Output Compare 3 Preload enable */

#define TIM1_CHCTLR2_OC3M                       ((uint16_t)0x0070) /* OC3M[2:0] bits (Output Compare 3 Mode) */
#define TIM1_CHCTLR2_OC3M_0                     ((uint16_t)0x0010) /* Bit 0 */
#define TIM1_CHCTLR2_OC3M_1                     ((uint16_t)0x0020) /* Bit 1 */
#define TIM1_CHCTLR2_OC3M_2                     ((uint16_t)0x0040) /* Bit 2 */

#define TIM1_CHCTLR2_OC3CE                      ((uint16_t)0x0080) /* Output Compare 3 Clear Enable */

#define TIM1_CHCTLR2_CC4S                       ((uint16_t)0x0300) /* CC4S[1:0] bits (Capture/Compare 4 Selection) */
#define TIM1_CHCTLR2_CC4S_0                     ((uint16_t)0x0100) /* Bit 0 */
#define TIM1_CHCTLR2_CC4S_1                     ((uint16_t)0x0200) /* Bit 1 */

#define TIM1_CHCTLR2_OC4FE                      ((uint16_t)0x0400) /* Output Compare 4 Fast enable */
#define TIM1_CHCTLR2_OC4PE                      ((uint16_t)0x0800) /* Output Compare 4 Preload enable */

#define TIM1_CHCTLR2_OC4M                       ((uint16_t)0x7000) /* OC4M[2:0] bits (Output Compare 4 Mode) */
#define TIM1_CHCTLR2_OC4M_0                     ((uint16_t)0x1000) /* Bit 0 */
#define TIM1_CHCTLR2_OC4M_1                     ((uint16_t)0x2000) /* Bit 1 */
#define TIM1_CHCTLR2_OC4M_2                     ((uint16_t)0x4000) /* Bit 2 */

#define TIM1_CHCTLR2_OC4CE                      ((uint16_t)0x8000) /* Output Compare 4 Clear Enable */

#define TIM1_CHCTLR2_IC3PSC                     ((uint16_t)0x000C) /* IC3PSC[1:0] bits (Input Capture 3 Prescaler) */
#define TIM1_CHCTLR2_IC3PSC_0                   ((uint16_t)0x0004) /* Bit 0 */
#define TIM1_CHCTLR2_IC3PSC_1                   ((uint16_t)0x0008) /* Bit 1 */

#define TIM1_CHCTLR2_IC3F                       ((uint16_t)0x00F0) /* IC3F[3:0] bits (Input Capture 3 Filter) */
#define TIM1_CHCTLR2_IC3F_0                     ((uint16_t)0x0010) /* Bit 0 */
#define TIM1_CHCTLR2_IC3F_1                     ((uint16_t)0x0020) /* Bit 1 */
#define TIM1_CHCTLR2_IC3F_2                     ((uint16_t)0x0040) /* Bit 2 */
#define TIM1_CHCTLR2_IC3F_3                     ((uint16_t)0x0080) /* Bit 3 */

#define TIM1_CHCTLR2_IC4PSC                     ((uint16_t)0x0C00) /* IC4PSC[1:0] bits (Input Capture 4 Prescaler) */
#define TIM1_CHCTLR2_IC4PSC_0                   ((uint16_t)0x0400) /* Bit 0 */
#define TIM1_CHCTLR2_IC4PSC_1                   ((uint16_t)0x0800) /* Bit 1 */

#define TIM1_CHCTLR2_IC4F                       ((uint16_t)0xF000) /* IC4F[3:0] bits (Input Capture 4 Filter) */
#define TIM1_CHCTLR2_IC4F_0                     ((uint16_t)0x1000) /* Bit 0 */
#define TIM1_CHCTLR2_IC4F_1                     ((uint16_t)0x2000) /* Bit 1 */
#define TIM1_CHCTLR2_IC4F_2                     ((uint16_t)0x4000) /* Bit 2 */
#define TIM1_CHCTLR2_IC4F_3                     ((uint16_t)0x8000) /* Bit 3 */

/*******************  Bit definition for TIM1_CCER register  ******************/
#define TIM1_CCER_CC1E                          ((uint16_t)0x0001) /* Capture/Compare 1 output enable */
#define TIM1_CCER_CC1P                          ((uint16_t)0x0002) /* Capture/Compare 1 output Polarity */
#define TIM1_CCER_CC1NE                         ((uint16_t)0x0004) /* Capture/Compare 1 Complementary output enable */
#define TIM1_CCER_CC1NP                         ((uint16_t)0x0008) /* Capture/Compare 1 Complementary output Polarity */
#define TIM1_CCER_CC2E                          ((uint16_t)0x0010) /* Capture/Compare 2 output enable */
#define TIM1_CCER_CC2P                          ((uint16_t)0x0020) /* Capture/Compare 2 output Polarity */
#define TIM1_CCER_CC2NE                         ((uint16_t)0x0040) /* Capture/Compare 2 Complementary output enable */
#define TIM1_CCER_CC2NP                         ((uint16_t)0x0080) /* Capture/Compare 2 Complementary output Polarity */
#define TIM1_CCER_CC3E                          ((uint16_t)0x0100) /* Capture/Compare 3 output enable */
#define TIM1_CCER_CC3P                          ((uint16_t)0x0200) /* Capture/Compare 3 output Polarity */
#define TIM1_CCER_CC3NE                         ((uint16_t)0x0400) /* Capture/Compare 3 Complementary output enable */
#define TIM1_CCER_CC3NP                         ((uint16_t)0x0800) /* Capture/Compare 3 Complementary output Polarity */
#define TIM1_CCER_CC4E                          ((uint16_t)0x1000) /* Capture/Compare 4 output enable */
#define TIM1_CCER_CC4P                          ((uint16_t)0x2000) /* Capture/Compare 4 output Polarity */

/*******************  Bit definition for TIM1_CNT register  *******************/
#define TIM1_CNT                                ((uint16_t)0xFFFF) /* Counter Value */

/*******************  Bit definition for TIM1_PSC register  *******************/
#define TIM1_PSC                                ((uint16_t)0xFFFF) /* Prescaler Value */

/*******************  Bit definition for TIM1_ATRLR register  *****************/
#define TIM1_ATRLR                              ((uint16_t)0xFFFF) /* actual auto-reload Value */

/*******************  Bit definition for TIM1_RPTCR register  *****************/
#define TIM1_REP                                ((uint16_t)0x00FF) /* Repetition Counter Value */

/*******************  Bit definition for TIM1_CH1CVR register  ****************/
#define TIM1_CH1CVR_CH1CVR                      ((uint32_t)0x0000FFFF) /* Capture/Compare 1 Value[15:0] */
#define TIM1_CH1CVR_LEVEL1                      ((uint32_t)0x00010000) /* The level indication bit corresponding to the captured value */

/*******************  Bit definition for TIM1_CH2CVR register  ****************/
#define TIM1_CH2CVR_CH2CVR                      ((uint32_t)0x0000FFFF) /* Capture/Compare 2 Value */
#define TIM1_CH2CVR_LEVEL2                      ((uint32_t)0x00010000) /* The level indication bit corresponding to the captured value */

/*******************  Bit definition for TIM1_CH3CVR register  ****************/
#define TIM1_CH3CVR_CH3CVR                      ((uint32_t)0x0000FFFF) /* Capture/Compare 3 Value */
#define TIM1_CH3CVR_LEVEL3                      ((uint32_t)0x00010000) /* The level indication bit corresponding to the captured value */

/*******************  Bit definition for TIM1_CH4CVR register  ****************/
#define TIM1_CH4CVR_CH4CVR                      ((uint32_t)0x0000FFFF) /* Capture/Compare 4 Value */
#define TIM1_CH4CVR_LEVEL4                      ((uint32_t)0x00010000) /* The level indication bit corresponding to the captured value */

/*******************  Bit definition for TIM1_BDTR register  ******************/
#define TIM1_BDTR_DTG                           ((uint16_t)0x00FF) /* DTG[0:7] bits (Dead-Time Generator set-up) */
#define TIM1_BDTR_DTG_0                         ((uint16_t)0x0001) /* Bit 0 */
#define TIM1_BDTR_DTG_1                         ((uint16_t)0x0002) /* Bit 1 */
#define TIM1_BDTR_DTG_2                         ((uint16_t)0x0004) /* Bit 2 */
#define TIM1_BDTR_DTG_3                         ((uint16_t)0x0008) /* Bit 3 */
#define TIM1_BDTR_DTG_4                         ((uint16_t)0x0010) /* Bit 4 */
#define TIM1_BDTR_DTG_5                         ((uint16_t)0x0020) /* Bit 5 */
#define TIM1_BDTR_DTG_6                         ((uint16_t)0x0040) /* Bit 6 */
#define TIM1_BDTR_DTG_7                         ((uint16_t)0x0080) /* Bit 7 */

#define TIM1_BDTR_LOCK                          ((uint16_t)0x0300) /* LOCK[1:0] bits (Lock Configuration) */
#define TIM1_BDTR_LOCK_0                        ((uint16_t)0x0100) /* Bit 0 */
#define TIM1_BDTR_LOCK_1                        ((uint16_t)0x0200) /* Bit 1 */

#define TIM1_BDTR_OSSI                          ((uint16_t)0x0400) /* Off-State Selection for Idle mode */
#define TIM1_BDTR_OSSR                          ((uint16_t)0x0800) /* Off-State Selection for Run mode */
#define TIM1_BDTR_BKE                           ((uint16_t)0x1000) /* Break enable */
#define TIM1_BDTR_BKP                           ((uint16_t)0x2000) /* Break Polarity */
#define TIM1_BDTR_AOE                           ((uint16_t)0x4000) /* Automatic Output enable */
#define TIM1_BDTR_MOE                           ((uint16_t)0x8000) /* Main Output enable */

/*******************  Bit definition for TIM1_DMACFGR register  ***************/
#define TIM1_DMACFGR_DBA                        ((uint16_t)0x001F) /* DBA[4:0] bits (DMA Base Address) */
#define TIM1_DMACFGR_DBA_0                      ((uint16_t)0x0001) /* Bit 0 */
#define TIM1_DMACFGR_DBA_1                      ((uint16_t)0x0002) /* Bit 1 */
#define TIM1_DMACFGR_DBA_2                      ((uint16_t)0x0004) /* Bit 2 */
#define TIM1_DMACFGR_DBA_3                      ((uint16_t)0x0008) /* Bit 3 */
#define TIM1_DMACFGR_DBA_4                      ((uint16_t)0x0010) /* Bit 4 */

#define TIM1_DMACFGR_DBL                        ((uint16_t)0x1F00) /* DBL[4:0] bits (DMA Burst Length) */
#define TIM1_DMACFGR_DBL_0                      ((uint16_t)0x0100) /* Bit 0 */
#define TIM1_DMACFGR_DBL_1                      ((uint16_t)0x0200) /* Bit 1 */
#define TIM1_DMACFGR_DBL_2                      ((uint16_t)0x0400) /* Bit 2 */
#define TIM1_DMACFGR_DBL_3                      ((uint16_t)0x0800) /* Bit 3 */
#define TIM1_DMACFGR_DBL_4                      ((uint16_t)0x1000) /* Bit 4 */

/*******************  Bit definition for TIM1_DMAADR register  ****************/
#define TIM1_DMAADR_DMAB                        ((uint16_t)0xFFFF) /* DMA register for burst accesses */

/******************************************************************************/
/*                                    TIM2                                    */
/******************************************************************************/

/*******************  Bit definition for TIM2_CTLR1 register  *****************/
#define TIM2_CTLR1_CEN                          ((uint16_t)0x0001) /* Counter enable */
#define TIM2_CTLR1_UDIS                         ((uint16_t)0x0002) /* Update disable */
#define TIM2_CTLR1_URS                          ((uint16_t)0x0004) /* Update request source */
#define TIM2_CTLR1_OPM                          ((uint16_t)0x0008) /* One pulse mode */
#define TIM2_CTLR1_DIR                          ((uint16_t)0x0010) /* Direction */
#define TIM2_CTLR1_CMS                          ((uint16_t)0x0060) /* CMS[1:0] bits (Center-aligned mode selection) */
#define TIM2_CTLR1_CMS_0                        ((uint16_t)0x0020) /* Bit 0 */
#define TIM2_CTLR1_CMS_1                        ((uint16_t)0x0040) /* Bit 1 */

#define TIM2_CTLR1_ARPE                         ((uint16_t)0x0080) /* Auto-reload preload enable */
#define TIM2_CTLR1_CKD                          ((uint16_t)0x0300) /* CKD[1:0] bits (clock division) */
#define TIM2_CTLR1_CKD_0                        ((uint16_t)0x0100) /* Bit 0 */
#define TIM2_CTLR1_CKD_1                        ((uint16_t)0x0200) /* Bit 1 */

#define TIM2_CTLR1_CAPOV                        ((uint16_t)0x4000) /* Capture value mode configuration */
#define TIM2_CTLR1_CAPLVL                       ((uint16_t)0x8000) /* Capture level indication enable in dual-edge capture */

/*******************  Bit definition for TIM2_CTLR2 register  *****************/
#define TIM2_CTLR2_CCDS                         ((uint16_t)0x0008) /* Capture/Compare DMA Selection */
#define TIM2_CTLR2_MMS                          ((uint16_t)0x0070) /* MMS[2:0] bits (Master Mode Selection) */
#define TIM2_CTLR2_MMS_0                        ((uint16_t)0x0010) /* Bit 0 */
#define TIM2_CTLR2_MMS_1                        ((uint16_t)0x0020) /* Bit 1 */
#define TIM2_CTLR2_MMS_2                        ((uint16_t)0x0040) /* Bit 2 */
#define TIM2_CTLR2_TI1S                         ((uint16_t)0x0080) /* TI1 Selection */

/*******************  Bit definition for TIM2_SMCFGR register  ****************/
#define TIM2_SMCFGR_SMS                         ((uint16_t)0x0007) /* SMS[2:0] bits (Slave mode selection) */
#define TIM2_SMCFGR_SMS_0                       ((uint16_t)0x0001) /* Bit 0 */
#define TIM2_SMCFGR_SMS_1                       ((uint16_t)0x0002) /* Bit 1 */
#define TIM2_SMCFGR_SMS_2                       ((uint16_t)0x0004) /* Bit 2 */

#define TIM2_SMCFGR_TS                          ((uint16_t)0x0070) /* TS[2:0] bits (Trigger selection) */
#define TIM2_SMCFGR_TS_0                        ((uint16_t)0x0010) /* Bit 0 */
#define TIM2_SMCFGR_TS_1                        ((uint16_t)0x0020) /* Bit 1 */
#define TIM2_SMCFGR_TS_2                        ((uint16_t)0x0040) /* Bit 2 */

#define TIM2_SMCFGR_MSM                         ((uint16_t)0x0080) /* Master/slave mode */

#define TIM2_SMCFGR_ETF                         ((uint16_t)0x0F00) /* ETF[3:0] bits (External trigger filter) */
#define TIM2_SMCFGR_ETF_0                       ((uint16_t)0x0100) /* Bit 0 */
#define TIM2_SMCFGR_ETF_1                       ((uint16_t)0x0200) /* Bit 1 */
#define TIM2_SMCFGR_ETF_2                       ((uint16_t)0x0400) /* Bit 2 */
#define TIM2_SMCFGR_ETF_3                       ((uint16_t)0x0800) /* Bit 3 */

#define TIM2_SMCFGR_ETPS                        ((uint16_t)0x3000) /* ETPS[1:0] bits (External trigger prescaler) */
#define TIM2_SMCFGR_ETPS_0                      ((uint16_t)0x1000) /* Bit 0 */
#define TIM2_SMCFGR_ETPS_1                      ((uint16_t)0x2000) /* Bit 1 */

#define TIM2_SMCFGR_ECE                         ((uint16_t)0x4000) /* External clock enable */
#define TIM2_SMCFGR_ETP                         ((uint16_t)0x8000) /* External trigger polarity */

/*******************  Bit definition for TIM2_DMAINTENR register  *************/
#define TIM2_DMAINTENR_UIE                      ((uint16_t)0x0001) /* Update interrupt enable */
#define TIM2_DMAINTENR_CC1IE                    ((uint16_t)0x0002) /* Capture/Compare 1 interrupt enable */
#define TIM2_DMAINTENR_CC2IE                    ((uint16_t)0x0004) /* Capture/Compare 2 interrupt enable */
#define TIM2_DMAINTENR_CC3IE                    ((uint16_t)0x0008) /* Capture/Compare 3 interrupt enable */
#define TIM2_DMAINTENR_CC4IE                    ((uint16_t)0x0010) /* Capture/Compare 4 interrupt enable */
#define TIM2_DMAINTENR_TIE                      ((uint16_t)0x0040) /* Trigger interrupt enable */
#define TIM2_DMAINTENR_UDE                      ((uint16_t)0x0100) /* Update DMA request enable */
#define TIM2_DMAINTENR_CC1DE                    ((uint16_t)0x0200) /* Capture/Compare 1 DMA request enable */
#define TIM2_DMAINTENR_CC2DE                    ((uint16_t)0x0400) /* Capture/Compare 2 DMA request enable */
#define TIM2_DMAINTENR_CC3DE                    ((uint16_t)0x0800) /* Capture/Compare 3 DMA request enable */
#define TIM2_DMAINTENR_CC4DE                    ((uint16_t)0x1000) /* Capture/Compare 4 DMA request enable */
#define TIM2_DMAINTENR_TDE                      ((uint16_t)0x4000) /* Trigger DMA request enable */

/*******************  Bit definition for TIM2_INTFR register  *****************/
#define TIM2_INTFR_UIF                          ((uint16_t)0x0001) /* Update interrupt Flag */
#define TIM2_INTFR_CC1IF                        ((uint16_t)0x0002) /* Capture/Compare 1 interrupt Flag */
#define TIM2_INTFR_CC2IF                        ((uint16_t)0x0004) /* Capture/Compare 2 interrupt Flag */
#define TIM2_INTFR_CC3IF                        ((uint16_t)0x0008) /* Capture/Compare 3 interrupt Flag */
#define TIM2_INTFR_CC4IF                        ((uint16_t)0x0010) /* Capture/Compare 4 interrupt Flag */
#define TIM2_INTFR_TIF                          ((uint16_t)0x0040) /* Trigger interrupt Flag */
#define TIM2_INTFR_CC1OF                        ((uint16_t)0x0200) /* Capture/Compare 1 Overcapture Flag */
#define TIM2_INTFR_CC2OF                        ((uint16_t)0x0400) /* Capture/Compare 2 Overcapture Flag */
#define TIM2_INTFR_CC3OF                        ((uint16_t)0x0800) /* Capture/Compare 3 Overcapture Flag */
#define TIM2_INTFR_CC4OF                        ((uint16_t)0x1000) /* Capture/Compare 4 Overcapture Flag */

/*******************  Bit definition for TIM2_SWEVGR register  ****************/
#define TIM2_SWEVGR_UG                          ((uint16_t)0x0001) /* Update Generation */
#define TIM2_SWEVGR_CC1G                        ((uint16_t)0x0002) /* Capture/Compare 1 Generation */
#define TIM2_SWEVGR_CC2G                        ((uint16_t)0x0004) /* Capture/Compare 2 Generation */
#define TIM2_SWEVGR_CC3G                        ((uint16_t)0x0008) /* Capture/Compare 3 Generation */
#define TIM2_SWEVGR_CC4G                        ((uint16_t)0x0010) /* Capture/Compare 4 Generation */
#define TIM2_SWEVGR_TG                          ((uint16_t)0x0040) /* Trigger Generation */

/******************  Bit definition for TIM2_CHCTLR1 register  ****************/
#define TIM2_CHCTLR1_CC1S                       ((uint16_t)0x0003) /* CC1S[1:0] bits (Capture/Compare 1 Selection) */
#define TIM2_CHCTLR1_CC1S_0                     ((uint16_t)0x0001) /* Bit 0 */
#define TIM2_CHCTLR1_CC1S_1                     ((uint16_t)0x0002) /* Bit 1 */

#define TIM2_CHCTLR1_OC1FE                      ((uint16_t)0x0004) /* Output Compare 1 Fast enable */
#define TIM2_CHCTLR1_OC1PE                      ((uint16_t)0x0008) /* Output Compare 1 Preload enable */

#define TIM2_CHCTLR1_OC1M                       ((uint16_t)0x0070) /* OC1M[2:0] bits (Output Compare 1 Mode) */
#define TIM2_CHCTLR1_OC1M_0                     ((uint16_t)0x0010) /* Bit 0 */
#define TIM2_CHCTLR1_OC1M_1                     ((uint16_t)0x0020) /* Bit 1 */
#define TIM2_CHCTLR1_OC1M_2                     ((uint16_t)0x0040) /* Bit 2 */

#define TIM2_CHCTLR1_OC1CE                      ((uint16_t)0x0080) /* Output Compare 1Clear Enable */

#define TIM2_CHCTLR1_CC2S                       ((uint16_t)0x0300) /* CC2S[1:0] bits (Capture/Compare 2 Selection) */
#define TIM2_CHCTLR1_CC2S_0                     ((uint16_t)0x0100) /* Bit 0 */
#define TIM2_CHCTLR1_CC2S_1                     ((uint16_t)0x0200) /* Bit 1 */

#define TIM2_CHCTLR1_OC2FE                      ((uint16_t)0x0400) /* Output Compare 2 Fast enable */
#define TIM2_CHCTLR1_OC2PE                      ((uint16_t)0x0800) /* Output Compare 2 Preload enable */

#define TIM2_CHCTLR1_OC2M                       ((uint16_t)0x7000) /* OC2M[2:0] bits (Output Compare 2 Mode) */
#define TIM2_CHCTLR1_OC2M_0                     ((uint16_t)0x1000) /* Bit 0 */
#define TIM2_CHCTLR1_OC2M_1                     ((uint16_t)0x2000) /* Bit 1 */
#define TIM2_CHCTLR1_OC2M_2                     ((uint16_t)0x4000) /* Bit 2 */

#define TIM2_CHCTLR1_OC2CE                      ((uint16_t)0x8000) /* Output Compare 2 Clear Enable */

#define TIM2_CHCTLR1_IC1PSC                     ((uint16_t)0x000C) /* IC1PSC[1:0] bits (Input Capture 1 Prescaler) */
#define TIM2_CHCTLR1_IC1PSC_0                   ((uint16_t)0x0004) /* Bit 0 */
#define TIM2_CHCTLR1_IC1PSC_1                   ((uint16_t)0x0008) /* Bit 1 */

#define TIM2_CHCTLR1_IC1F                       ((uint16_t)0x00F0) /* IC1F[3:0] bits (Input Capture 1 Filter) */
#define TIM2_CHCTLR1_IC1F_0                     ((uint16_t)0x0010) /* Bit 0 */
#define TIM2_CHCTLR1_IC1F_1                     ((uint16_t)0x0020) /* Bit 1 */
#define TIM2_CHCTLR1_IC1F_2                     ((uint16_t)0x0040) /* Bit 2 */
#define TIM2_CHCTLR1_IC1F_3                     ((uint16_t)0x0080) /* Bit 3 */

#define TIM2_CHCTLR1_IC2PSC                     ((uint16_t)0x0C00) /* IC2PSC[1:0] bits (Input Capture 2 Prescaler) */
#define TIM2_CHCTLR1_IC2PSC_0                   ((uint16_t)0x0400) /* Bit 0 */
#define TIM2_CHCTLR1_IC2PSC_1                   ((uint16_t)0x0800) /* Bit 1 */

#define TIM2_CHCTLR1_IC2F                       ((uint16_t)0xF000) /* IC2F[3:0] bits (Input Capture 2 Filter) */
#define TIM2_CHCTLR1_IC2F_0                     ((uint16_t)0x1000) /* Bit 0 */
#define TIM2_CHCTLR1_IC2F_1                     ((uint16_t)0x2000) /* Bit 1 */
#define TIM2_CHCTLR1_IC2F_2                     ((uint16_t)0x4000) /* Bit 2 */
#define TIM2_CHCTLR1_IC2F_3                     ((uint16_t)0x8000) /* Bit 3 */

/******************  Bit definition for TIM2_CHCTLR2 register  ****************/
#define TIM2_CHCTLR2_CC3S                       ((uint16_t)0x0003) /* CC3S[1:0] bits (Capture/Compare 3 Selection) */
#define TIM2_CHCTLR2_CC3S_0                     ((uint16_t)0x0001) /* Bit 0 */
#define TIM2_CHCTLR2_CC3S_1                     ((uint16_t)0x0002) /* Bit 1 */

#define TIM2_CHCTLR2_OC3FE                      ((uint16_t)0x0004) /* Output Compare 3 Fast enable */
#define TIM2_CHCTLR2_OC3PE                      ((uint16_t)0x0008) /* Output Compare 3 Preload enable */

#define TIM2_CHCTLR2_OC3M                       ((uint16_t)0x0070) /* OC3M[2:0] bits (Output Compare 3 Mode) */
#define TIM2_CHCTLR2_OC3M_0                     ((uint16_t)0x0010) /* Bit 0 */
#define TIM2_CHCTLR2_OC3M_1                     ((uint16_t)0x0020) /* Bit 1 */
#define TIM2_CHCTLR2_OC3M_2                     ((uint16_t)0x0040) /* Bit 2 */

#define TIM2_CHCTLR2_OC3CE                      ((uint16_t)0x0080) /* Output Compare 3 Clear Enable */

#define TIM2_CHCTLR2_CC4S                       ((uint16_t)0x0300) /* CC4S[1:0] bits (Capture/Compare 4 Selection) */
#define TIM2_CHCTLR2_CC4S_0                     ((uint16_t)0x0100) /* Bit 0 */
#define TIM2_CHCTLR2_CC4S_1                     ((uint16_t)0x0200) /* Bit 1 */

#define TIM2_CHCTLR2_OC4FE                      ((uint16_t)0x0400) /* Output Compare 4 Fast enable */
#define TIM2_CHCTLR2_OC4PE                      ((uint16_t)0x0800) /* Output Compare 4 Preload enable */

#define TIM2_CHCTLR2_OC4M                       ((uint16_t)0x7000) /* OC4M[2:0] bits (Output Compare 4 Mode) */
#define TIM2_CHCTLR2_OC4M_0                     ((uint16_t)0x1000) /* Bit 0 */
#define TIM2_CHCTLR2_OC4M_1                     ((uint16_t)0x2000) /* Bit 1 */
#define TIM2_CHCTLR2_OC4M_2                     ((uint16_t)0x4000) /* Bit 2 */

#define TIM2_CHCTLR2_OC4CE                      ((uint16_t)0x8000) /* Output Compare 4 Clear Enable */

#define TIM2_CHCTLR2_IC3PSC                     ((uint16_t)0x000C) /* IC3PSC[1:0] bits (Input Capture 3 Prescaler) */
#define TIM2_CHCTLR2_IC3PSC_0                   ((uint16_t)0x0004) /* Bit 0 */
#define TIM2_CHCTLR2_IC3PSC_1                   ((uint16_t)0x0008) /* Bit 1 */

#define TIM2_CHCTLR2_IC3F                       ((uint16_t)0x00F0) /* IC3F[3:0] bits (Input Capture 3 Filter) */
#define TIM2_CHCTLR2_IC3F_0                     ((uint16_t)0x0010) /* Bit 0 */
#define TIM2_CHCTLR2_IC3F_1                     ((uint16_t)0x0020) /* Bit 1 */
#define TIM2_CHCTLR2_IC3F_2                     ((uint16_t)0x0040) /* Bit 2 */
#define TIM2_CHCTLR2_IC3F_3                     ((uint16_t)0x0080) /* Bit 3 */

#define TIM2_CHCTLR2_IC4PSC                     ((uint16_t)0x0C00) /* IC4PSC[1:0] bits (Input Capture 4 Prescaler) */
#define TIM2_CHCTLR2_IC4PSC_0                   ((uint16_t)0x0400) /* Bit 0 */
#define TIM2_CHCTLR2_IC4PSC_1                   ((uint16_t)0x0800) /* Bit 1 */

#define TIM2_CHCTLR2_IC4F                       ((uint16_t)0xF000) /* IC4F[3:0] bits (Input Capture 4 Filter) */
#define TIM2_CHCTLR2_IC4F_0                     ((uint16_t)0x1000) /* Bit 0 */
#define TIM2_CHCTLR2_IC4F_1                     ((uint16_t)0x2000) /* Bit 1 */
#define TIM2_CHCTLR2_IC4F_2                     ((uint16_t)0x4000) /* Bit 2 */
#define TIM2_CHCTLR2_IC4F_3                     ((uint16_t)0x8000) /* Bit 3 */

/*******************  Bit definition for TIM2_CCER register  ******************/
#define TIM2_CCER_CC1E                          ((uint16_t)0x0001) /* Capture/Compare 1 output enable */
#define TIM2_CCER_CC1P                          ((uint16_t)0x0002) /* Capture/Compare 1 output Polarity */
#define TIM2_CCER_CC2E                          ((uint16_t)0x0010) /* Capture/Compare 2 output enable */
#define TIM2_CCER_CC2P                          ((uint16_t)0x0020) /* Capture/Compare 2 output Polarity */
#define TIM2_CCER_CC3E                          ((uint16_t)0x0100) /* Capture/Compare 3 output enable */
#define TIM2_CCER_CC3P                          ((uint16_t)0x0200) /* Capture/Compare 3 output Polarity */
#define TIM2_CCER_CC4E                          ((uint16_t)0x1000) /* Capture/Compare 4 output enable */
#define TIM2_CCER_CC4P                          ((uint16_t)0x2000) /* Capture/Compare 4 output Polarity */

/*******************  Bit definition for TIM2_CNT register  *******************/
#define TIM2_CNT                                ((uint16_t)0xFFFF) /* Counter Value */

/*******************  Bit definition for TIM2_PSC register  *******************/
#define TIM2_PSC                                ((uint16_t)0xFFFF) /* Prescaler Value */

/*******************  Bit definition for TIM2_ATRLR register  *****************/
#define TIM2_ATRLR                              ((uint16_t)0xFFFF) /* actual auto-reload Value */

/*******************  Bit definition for TIM2_CH1CVR register  ****************/
#define TIM2_CH1CVR_CH1CVR                      ((uint32_t)0x0000FFFF) /* Capture/Compare 1 Value[15:0] */
#define TIM2_CH1CVR_LEVEL1                      ((uint32_t)0x00010000) /* The level indication bit corresponding to the captured value */

/*******************  Bit definition for TIM2_CH2CVR register  ****************/
#define TIM2_CH2CVR_CH2CVR                      ((uint32_t)0x0000FFFF) /* Capture/Compare 2 Value */
#define TIM2_CH2CVR_LEVEL2                      ((uint32_t)0x00010000) /* The level indication bit corresponding to the captured value */

/*******************  Bit definition for TIM2_CH3CVR register  ****************/
#define TIM2_CH3CVR_CH3CVR                      ((uint32_t)0x0000FFFF) /* Capture/Compare 3 Value */
#define TIM2_CH3CVR_LEVEL3                      ((uint32_t)0x00010000) /* The level indication bit corresponding to the captured value */

/*******************  Bit definition for TIM2_CH4CVR register  ****************/
#define TIM2_CH4CVR_CH4CVR                      ((uint32_t)0x0000FFFF) /* Capture/Compare 4 Value */
#define TIM2_CH4CVR_LEVEL4                      ((uint32_t)0x00010000) /* The level indication bit corresponding to the captured value */

/*******************  Bit definition for TIM2_DTCR register  ******************/
#define TIM2_DTCR_OC1N_EN                       ((uint16_t)0x0001) /* Complementary output CH1 EN */
#define TIM2_DTCR_OC2N_EN                       ((uint16_t)0x0002) /* Complementary output CH2 EN */
#define TIM2_DTCR_DT1_P                         ((uint16_t)0x0004) /* Complementary output CH1 POL */
#define TIM2_DTCR_DT1N_P                        ((uint16_t)0x0008) /* Complementary output CH1 compliment POL */
#define TIM2_DTCR_DT2_P                         ((uint16_t)0x0010) /* Complementary output CH2 POL */
#define TIM2_DTCR_DT2N_P                        ((uint16_t)0x0020) /* Complementary output CH2 compliment POL */

#define TIM2_DTCR_DT1                           ((uint16_t)0x0F00) /* DT1[3:0] bits (Dead-Time Generator set-up) */
#define TIM2_DTCR_DT1_0                         ((uint16_t)0x0100) /* Bit 0 */
#define TIM2_DTCR_DT1_1                         ((uint16_t)0x0200) /* Bit 1 */
#define TIM2_DTCR_DT1_2                         ((uint16_t)0x0400) /* Bit 2 */
#define TIM2_DTCR_DT1_3                         ((uint16_t)0x0800) /* Bit 3 */

#define TIM2_DTCR_DT2                           ((uint16_t)0x0F00) /* DT2[3:0] bits (Dead-Time Generator set-up) */
#define TIM2_DTCR_DT2_0                         ((uint16_t)0x0100) /* Bit 0 */
#define TIM2_DTCR_DT2_1                         ((uint16_t)0x0200) /* Bit 1 */
#define TIM2_DTCR_DT2_2                         ((uint16_t)0x0400) /* Bit 2 */
#define TIM2_DTCR_DT2_3                         ((uint16_t)0x0800) /* Bit 3 */

/*******************  Bit definition for TIM2_DMACFGR register  ***************/
#define TIM2_DMACFGR_DBA                        ((uint16_t)0x001F) /* DBA[4:0] bits (DMA Base Address) */
#define TIM2_DMACFGR_DBA_0                      ((uint16_t)0x0001) /* Bit 0 */
#define TIM2_DMACFGR_DBA_1                      ((uint16_t)0x0002) /* Bit 1 */
#define TIM2_DMACFGR_DBA_2                      ((uint16_t)0x0004) /* Bit 2 */
#define TIM2_DMACFGR_DBA_3                      ((uint16_t)0x0008) /* Bit 3 */
#define TIM2_DMACFGR_DBA_4                      ((uint16_t)0x0010) /* Bit 4 */

#define TIM2_DMACFGR_DBL                        ((uint16_t)0x1F00) /* DBL[4:0] bits (DMA Burst Length) */
#define TIM2_DMACFGR_DBL_0                      ((uint16_t)0x0100) /* Bit 0 */
#define TIM2_DMACFGR_DBL_1                      ((uint16_t)0x0200) /* Bit 1 */
#define TIM2_DMACFGR_DBL_2                      ((uint16_t)0x0400) /* Bit 2 */
#define TIM2_DMACFGR_DBL_3                      ((uint16_t)0x0800) /* Bit 3 */
#define TIM2_DMACFGR_DBL_4                      ((uint16_t)0x1000) /* Bit 4 */

/*******************  Bit definition for TIM2_DMAADR register  ****************/
#define TIM2_DMAADR_DMAB                        ((uint16_t)0xFFFF) /* DMA register for burst accesses */

/******************************************************************************/
/*                                    TIM3                                    */
/******************************************************************************/

/*******************  Bit definition for TIM3_CTLR register  ******************/
#define TIM3_CTLR_CEN                           ((uint16_t)0x0001) /* Counter enable */
#define TIM3_CTLR_UDIS                          ((uint16_t)0x0002) /* Update disable */
#define TIM3_CTLR_DIR                           ((uint16_t)0x0010) /* Direction */
#define TIM3_CTLR_CMS                           ((uint16_t)0x0060) /* CMS[1:0] bits (Center-aligned mode selection) */
#define TIM3_CTLR_CMS_0                         ((uint16_t)0x0020) /* Bit 0 */
#define TIM3_CTLR_CMS_1                         ((uint16_t)0x0040) /* Bit 1 */

#define TIM3_CTLR_ARPE                          ((uint16_t)0x0080) /* Auto-reload preload enable */

#define TIM3_CTLR_SMS                           ((uint16_t)0x0700) /* SMS[2:0] bits clock source */
#define TIM3_CTLR_SMS_0                         ((uint16_t)0x0100) /* Bit 0 */
#define TIM3_CTLR_SMS_1                         ((uint16_t)0x0200) /* Bit 1 */
#define TIM3_CTLR_SMS_2                         ((uint16_t)0x0400) /* Bit 2 */

/*******************  Bit definition for TIM3_DMAINTENR register  *************/
#define TIM3_DMAINTENR_OC1PE                    ((uint16_t)0x0001) /* Compare 1 preload enable */
#define TIM3_DMAINTENR_OC2PE                    ((uint16_t)0x0002) /* Compare 2 preload enable */
#define TIM3_DMAINTENR_OC3PE                    ((uint16_t)0x0004) /* Compare 3 preload enable */
#define TIM3_DMAINTENR_OC4PE                    ((uint16_t)0x0008) /* Compare 4 preload enable */
#define TIM3_DMAINTENR_CC3DE                    ((uint16_t)0x0800) /* Compare 3 DMA request enable */
#define TIM3_DMAINTENR_CC4DE                    ((uint16_t)0x1000) /* Compare 4 DMA request enable */

/*******************  Bit definition for TIM3_CNT register  *******************/
#define TIM3_CNT                                ((uint16_t)0xFFFF) /* Counter Value */

/*******************  Bit definition for TIM3_ATRLR register  *****************/
#define TIM3_ATRLR                              ((uint16_t)0xFFFF) /* actual auto-reload Value */

/*******************  Bit definition for TIM3_CH1CVR register  ****************/
#define TIM3_CH1CVR_CH1CVR                      ((uint16_t)0xFFFF) /* Compare 1 Value[15:0] */

/*******************  Bit definition for TIM3_CH2CVR register  ****************/
#define TIM3_CH2CVR_CH2CVR                      ((uint16_t)0xFFFF) /* Compare 2 Value */

/*******************  Bit definition for TIM3_CH3CVR register  ****************/
#define TIM3_CH3CVR_CH3CVR                      ((uint16_t)0xFFFF) /* Compare 3 Value */

/*******************  Bit definition for TIM3_CH4CVR register  ****************/
#define TIM3_CH4CVR_CH4CVR                      ((uint16_t)0xFFFF) /* Compare 4 Value */

/******************************************************************************/
/*         Universal Synchronous Asynchronous Receiver Transmitter            */
/******************************************************************************/

/*******************  Bit definition for USART_STATR register  ****************/
#define USART_STATR_PE                          ((uint32_t)0x00000001) /* Parity Error */
#define USART_STATR_FE                          ((uint32_t)0x00000002) /* Framing Error */
#define USART_STATR_NE                          ((uint32_t)0x00000004) /* Noise Error Flag */
#define USART_STATR_ORE                         ((uint32_t)0x00000008) /* OverRun Error */
#define USART_STATR_IDLE                        ((uint32_t)0x00000010) /* IDLE line detected */
#define USART_STATR_RXNE                        ((uint32_t)0x00000020) /* Read Data Register Not Empty */
#define USART_STATR_TC                          ((uint32_t)0x00000040) /* Transmission Complete */
#define USART_STATR_TXE                         ((uint32_t)0x00000080) /* Transmit Data Register Empty */
#define USART_STATR_LBD                         ((uint32_t)0x00000100) /* LIN Break Detection Flag */
#define USART_STATR_CTS                         ((uint32_t)0x00000200) /* CTS Flag */

/*******************  Bit definition for USART_DATAR register  ****************/
#define USART_DATAR_DR                          ((uint32_t)0x000001FF) /* Data value */

/******************  Bit definition for USART_BRR register  *******************/
#define USART_BRR_DIV_Fraction                  ((uint32_t)0x0000000F) /* Fraction of USARTDIV */
#define USART_BRR_DIV_Mantissa                  ((uint32_t)0x0000FFF0) /* Mantissa of USARTDIV */

/******************  Bit definition for USART_CTLR1 register  *****************/
#define USART_CTLR1_SBK                         ((uint32_t)0x00000001) /* Send Break */
#define USART_CTLR1_RWU                         ((uint32_t)0x00000002) /* Receiver wakeup */
#define USART_CTLR1_RE                          ((uint32_t)0x00000004) /* Receiver Enable */
#define USART_CTLR1_TE                          ((uint32_t)0x00000008) /* Transmitter Enable */
#define USART_CTLR1_IDLEIE                      ((uint32_t)0x00000010) /* IDLE Interrupt Enable */
#define USART_CTLR1_RXNEIE                      ((uint32_t)0x00000020) /* RXNE Interrupt Enable */
#define USART_CTLR1_TCIE                        ((uint32_t)0x00000040) /* Transmission Complete Interrupt Enable */
#define USART_CTLR1_TXEIE                       ((uint32_t)0x00000080) /* PE Interrupt Enable */
#define USART_CTLR1_PEIE                        ((uint32_t)0x00000100) /* PE Interrupt Enable */
#define USART_CTLR1_PS                          ((uint32_t)0x00000200) /* Parity Selection */
#define USART_CTLR1_PCE                         ((uint32_t)0x00000400) /* Parity Control Enable */
#define USART_CTLR1_WAKE                        ((uint32_t)0x00000800) /* Wakeup method */
#define USART_CTLR1_M                           ((uint32_t)0x00001000) /* Word length */
#define USART_CTLR1_UE                          ((uint32_t)0x00002000) /* USART Enable */

/******************  Bit definition for USART_CTLR2 register  *****************/
#define USART_CTLR2_ADD                         ((uint32_t)0x0000000F) /* Address of the USART node */
#define USART_CTLR2_LBDL                        ((uint32_t)0x00000020) /* LIN Break Detection Length */
#define USART_CTLR2_LBDIE                       ((uint32_t)0x00000040) /* LIN Break Detection Interrupt Enable */

#define USART_CTLR2_STOP                        ((uint32_t)0x00003000) /* STOP[1:0] bits (STOP bits) */
#define USART_CTLR2_STOP_0                      ((uint32_t)0x00001000) /* Bit 0 */
#define USART_CTLR2_STOP_1                      ((uint32_t)0x00002000) /* Bit 1 */

#define USART_CTLR2_LINEN                       ((uint32_t)0x00004000) /* LIN mode enable */

/******************  Bit definition for USART_CTLR3 register  *****************/
#define USART_CTLR3_EIE                         ((uint32_t)0x00000001) /* Error Interrupt Enable */
#define USART_CTLR3_IREN                        ((uint32_t)0x00000002) /* IrDA mode Enable */
#define USART_CTLR3_IRLP                        ((uint32_t)0x00000004) /* IrDA Low-Power */
#define USART_CTLR3_HDSEL                       ((uint32_t)0x00000008) /* Half-Duplex Selection */
#define USART_CTLR3_DMAR                        ((uint32_t)0x00000040) /* DMA Enable Receiver */
#define USART_CTLR3_DMAT                        ((uint32_t)0x00000080) /* DMA Enable Transmitter */
#define USART_CTLR3_RTSE                        ((uint32_t)0x00000100) /* RTS Enable */
#define USART_CTLR3_CTSE                        ((uint32_t)0x00000200) /* CTS Enable */
#define USART_CTLR3_CTSIE                       ((uint32_t)0x00000400) /* CTS Interrupt Enable */

/******************  Bit definition for USART_GPR register  ******************/
#define USART_GPR_PSC                           ((uint32_t)0x000000FF) /* PSC[7:0] bits (Prescaler value) */
#define USART_GPR_PSC_0                         ((uint32_t)0x00000001) /* Bit 0 */
#define USART_GPR_PSC_1                         ((uint32_t)0x00000002) /* Bit 1 */
#define USART_GPR_PSC_2                         ((uint32_t)0x00000004) /* Bit 2 */
#define USART_GPR_PSC_3                         ((uint32_t)0x00000008) /* Bit 3 */
#define USART_GPR_PSC_4                         ((uint32_t)0x00000010) /* Bit 4 */
#define USART_GPR_PSC_5                         ((uint32_t)0x00000020) /* Bit 5 */
#define USART_GPR_PSC_6                         ((uint32_t)0x00000040) /* Bit 6 */
#define USART_GPR_PSC_7                         ((uint32_t)0x00000080) /* Bit 7 */


/******************************************************************************/
/*                            Window WATCHDOG                                 */
/******************************************************************************/

/*******************  Bit definition for WWDG_CTLR register  ******************/
#define WWDG_CTLR_T                             ((uint16_t)0x7F) /* T[6:0] bits (7-Bit counter (MSB to LSB)) */
#define WWDG_CTLR_T0                            ((uint16_t)0x01) /* Bit 0 */
#define WWDG_CTLR_T1                            ((uint16_t)0x0002) /* Bit 1 */
#define WWDG_CTLR_T2                            ((uint16_t)0x0004) /* Bit 2 */
#define WWDG_CTLR_T3                            ((uint16_t)0x0008) /* Bit 3 */
#define WWDG_CTLR_T4                            ((uint16_t)0x0010) /* Bit 4 */
#define WWDG_CTLR_T5                            ((uint16_t)0x0020) /* Bit 5 */
#define WWDG_CTLR_T6                            ((uint16_t)0x0040) /* Bit 6 */

#define WWDG_CTLR_WDGA                          ((uint16_t)0x0080) /* Activation bit */

/*******************  Bit definition for WWDG_CFGR register  ******************/
#define WWDG_CFGR_W                             ((uint16_t)0x007F) /* W[6:0] bits (7-bit window value) */
#define WWDG_CFGR_W0                            ((uint16_t)0x0001) /* Bit 0 */
#define WWDG_CFGR_W1                            ((uint16_t)0x0002) /* Bit 1 */
#define WWDG_CFGR_W2                            ((uint16_t)0x0004) /* Bit 2 */
#define WWDG_CFGR_W3                            ((uint16_t)0x0008) /* Bit 3 */
#define WWDG_CFGR_W4                            ((uint16_t)0x0010) /* Bit 4 */
#define WWDG_CFGR_W5                            ((uint16_t)0x0020) /* Bit 5 */
#define WWDG_CFGR_W6                            ((uint16_t)0x0040) /* Bit 6 */

#define WWDG_CFGR_WDGTB                         ((uint16_t)0x0180) /* WDGTB[1:0] bits (Timer Base) */
#define WWDG_CFGR_WDGTB0                        ((uint16_t)0x0080) /* Bit 0 */
#define WWDG_CFGR_WDGTB1                        ((uint16_t)0x0100) /* Bit 1 */

#define WWDG_CFGR_EWI                           ((uint16_t)0x0200) /* Early Wakeup Interrupt */

/*******************  Bit definition for WWDG_STATR register  ********************/
#define WWDG_STATR_EWIF                         ((uint16_t)0x0001) /* Early Wakeup Interrupt Flag */

/******************************************************************************/
/*                          Extended Configuration                            */
/******************************************************************************/

/****************************  EXTEN_CTR  *************************************/
#define EXTEN_LKUPEN                            ((uint32_t)0x00000040) /* LOCKUP monitoring function enable */
#define EXTEN_LKUPRST                           ((uint32_t)0x00000080) /* LOCKUP reset flag */
#define EXTEN_LDOTRIM                           ((uint32_t)0x00000400) /* Core voltage modes */
#define EXTEN_TIM2_DMA_REMAP                    ((uint32_t)0x00010000) /* DMA alternate of TIM2 */

/******************************************************************************/
/*                    Operational Amplifier and Comparator                    */
/******************************************************************************/

/********************  Bit definition for OPA_CFGR1 register  ********************/
#define OPA_CFGR1_POLL_EN                       ((uint32_t)0x00000001) /*  */
#define OPA_CFGR1_POLL1_NUM                     ((uint32_t)0x0000000C) /*  */
#define OPA_CFGR1_POLL1_NUM_0                   ((uint32_t)0x00000004) /*  */
#define OPA_CFGR1_POLL1_NUM_1                   ((uint32_t)0x00000008) /*  */
#define OPA_CFGR1_RST_EN1                       ((uint32_t)0x00000010) /*  */
#define OPA_CFGR1_SETUP_CFG                     ((uint32_t)0x00000060) /*  */
#define OPA_CFGR1_SETUP_CFG_0                   ((uint32_t)0x00000020) /*  */
#define OPA_CFGR1_SETUP_CFG_1                   ((uint32_t)0x00000040) /*  */
#define OPA_CFGR1_AUTO_ADC_CFG                  ((uint32_t)0x00000080) /*  */
#define OPA_CFGR1_IE_OUT1                       ((uint32_t)0x00000100) /*  */
#define OPA_CFGR1_NMI_EN                        ((uint32_t)0x00000400) /*  */
#define OPA_CFGR1_IF_OUT_POLL_CH1               ((uint32_t)0x00001000) /*  */
#define OPA_CFGR1_IF_OUT_POLL_CH2               ((uint32_t)0x00002000) /*  */
#define OPA_CFGR1_IF_OUT_POLL_CH3               ((uint32_t)0x00004000) /*  */
#define OPA_CFGR1_POLL_CH1                      ((uint32_t)0x00030000) /*  */
#define OPA_CFGR1_POLL_CH1_0                    ((uint32_t)0x00010000) /*  */
#define OPA_CFGR1_POLL_CH1_1                    ((uint32_t)0x00020000) /*  */
#define OPA_CFGR1_POLL_CH2                      ((uint32_t)0x000C0000) /*  */
#define OPA_CFGR1_POLL_CH2_0                    ((uint32_t)0x00040000) /*  */
#define OPA_CFGR1_POLL_CH2_1                    ((uint32_t)0x00080000) /*  */
#define OPA_CFGR1_POLL_CH3                      ((uint32_t)0x00300000) /*  */
#define OPA_CFGR1_POLL_CH3_0                    ((uint32_t)0x00100000) /*  */
#define OPA_CFGR1_POLL_CH3_1                    ((uint32_t)0x00200000) /*  */
#define OPA_CFGR1_POLL_SWSTRT                   ((uint32_t)0x01000000) /*  */
#define OPA_CFGR1_POLL_SEL                      ((uint32_t)0x0E000000) /*  */
#define OPA_CFGR1_POLL_SEL_1                    ((uint32_t)0x02000000) /*  */
#define OPA_CFGR1_POLL_SEL_2                    ((uint32_t)0x04000000) /*  */
#define OPA_CFGR1_POLL_SEL_3                    ((uint32_t)0x08000000) /*  */
#define OPA_CFGR1_POLL_LOCK                     ((uint32_t)0x80000000) /*  */

/*******************  Bit definition for OPA_CTLR1 register  ********************/
#define OPA_CTLR1_OPA_EN1                       ((uint32_t)0x00000001) /*  */
#define OPA_CTLR1_MODE1                         ((uint32_t)0x00000006) /*  */
#define OPA_CTLR1_MODE1_0                       ((uint32_t)0x00000002) /*  */
#define OPA_CTLR1_MODE1_1                       ((uint32_t)0x00000004) /*  */
#define OPA_CTLR1_PSEL1                         ((uint32_t)0x00000030) /*  */
#define OPA_CTLR1_PSEL1_0                       ((uint32_t)0x00000010) /*  */
#define OPA_CTLR1_PSEL1_1                       ((uint32_t)0x00000020) /*  */
#define OPA_CTLR1_PSEL1_CHP0                    ((uint32_t)0x00000000) /*  */
#define OPA_CTLR1_PSEL1_CHP1                    ((uint32_t)0x00000010) /*  */
#define OPA_CTLR1_PSEL1_CHP2                    ((uint32_t)0x00000020) /*  */
#define OPA_CTLR1_PSEL1_CHP3                    ((uint32_t)0x00000030) /*  */
#define OPA_CTLR1_NSEL1                         ((uint32_t)0x00000700) /*  */
#define OPA_CTLR1_NSEL1_0                       ((uint32_t)0x00000100) /*  */
#define OPA_CTLR1_NSEL1_1                       ((uint32_t)0x00000200) /*  */
#define OPA_CTLR1_NSEL1_2                       ((uint32_t)0x00000400) /*  */
#define OPA_CTLR1_NSEL1_CHN0                    ((uint32_t)0x00000000) /*  */
#define OPA_CTLR1_NSEL1_CHN1                    ((uint32_t)0x00000100) /*  */
#define OPA_CTLR1_NSEL1_PGA4                    ((uint32_t)0x00000300) /*  */
#define OPA_CTLR1_NSEL1_PGA8                    ((uint32_t)0x00000400) /*  */
#define OPA_CTLR1_NSEL1_PGA16                   ((uint32_t)0x00000500) /*  */
#define OPA_CTLR1_NSEL1_PGA32                   ((uint32_t)0x00000600) /*  */
#define OPA_CTLR1_FB_EN1                        ((uint32_t)0x00000800) /*  */
#define OPA_CTLR1_PGADIF                        ((uint32_t)0x00001000) /*  */
#define OPA_CTLR1_VBEN                          ((uint32_t)0x00010000) /*  */
#define OPA_CTLR1_VBSEL                         ((uint32_t)0x00020000) /*  */
#define OPA_CTLR1_VBCMPSEL                      ((uint32_t)0x000C0000) /*  */
#define OPA_CTLR1_VBCMPSEL_0                    ((uint32_t)0x00040000) /*  */
#define OPA_CTLR1_VBCMPSEL_1                    ((uint32_t)0x00080000) /*  */
#define OPA_CTLR1_OPA_HS1                       ((uint32_t)0x00100000) /*  */
#define OPA_CTLR1_OPA_LOCK                      ((uint32_t)0x80000000) /*  */

/********************  Bit definition for OPA_CFGR2 register  ********************/
#define OPA_CFGR2_POLL_EN1                      ((uint32_t)0x00000001) /*  */
#define OPA_CFGR2_POLL1_NUM                     ((uint32_t)0x0000000C) /*  */
#define OPA_CFGR2_POLL1_NUM_0                   ((uint32_t)0x00000004) /*  */
#define OPA_CFGR2_POLL1_NUM_1                   ((uint32_t)0x00000008) /*  */
#define OPA_CFGR2_RST_EN1                       ((uint32_t)0x00000010) /*  */
#define OPA_CFGR2_RST_EN2                       ((uint32_t)0x00000020) /*  */
#define OPA_CFGR2_IE_OUT1                       ((uint32_t)0x00000100) /*  */
#define OPA_CFGR2_IE_CNT                        ((uint32_t)0x00000200) /*  */
#define OPA_CFGR2_IF_OUT_POLL_CH1               ((uint32_t)0x00001000) /*  */
#define OPA_CFGR2_IF_OUT_POLL_CH2               ((uint32_t)0x00002000) /*  */
#define OPA_CFGR2_IF_OUT_POLL_CH3               ((uint32_t)0x00004000) /*  */
#define OPA_CFGR2_IF_CNT                        ((uint32_t)0x00008000) /*  */
#define OPA_CFGR2_POLL_VLU                      ((uint32_t)0x01FF0000) /*  */
#define OPA_CFGR2_POLL_CH1                      ((uint32_t)0x0C000000) /*  */
#define OPA_CFGR2_POLL_CH1_0                    ((uint32_t)0x02000000) /*  */
#define OPA_CFGR2_POLL_CH1_1                    ((uint32_t)0x04000000) /*  */
#define OPA_CFGR2_POLL_CH2                      ((uint32_t)0x18000000) /*  */
#define OPA_CFGR2_POLL_CH2_0                    ((uint32_t)0x08000000) /*  */
#define OPA_CFGR2_POLL_CH2_1                    ((uint32_t)0x10000000) /*  */
#define OPA_CFGR2_POLL_CH3                      ((uint32_t)0xC0000000) /*  */
#define OPA_CFGR2_POLL_CH3_0                    ((uint32_t)0x20000000) /*  */
#define OPA_CFGR2_POLL_CH3_1                    ((uint32_t)0x40000000) /*  */

/********************  Bit definition for OPA_CTRL2 register  ********************/
#define OPA_CTRL2_CMP_EN1                       ((uint32_t)0x00000001) /*  */
#define OPA_CTRL2_MODE1                         ((uint32_t)0x00000006) /*  */
#define OPA_CTRL2_MODE1_0                       ((uint32_t)0x00000002) /*  */
#define OPA_CTRL2_MODE1_1                       ((uint32_t)0x00000004) /*  */
#define OPA_CTRL2_NSEL1                         ((uint32_t)0x00000018) /*  */
#define OPA_CTRL2_NSEL1_0                       ((uint32_t)0x00000008) /*  */
#define OPA_CTRL2_NSEL1_1                       ((uint32_t)0x00000010) /*  */
#define OPA_CTRL2_NSEL1_CHN0                    ((uint32_t)0x00000000) /*  */
#define OPA_CTRL2_NSEL1_CHN1                    ((uint32_t)0x00000008) /*  */
#define OPA_CTRL2_NSEL1_CHN2                    ((uint32_t)0x00000010) /*  */
#define OPA_CTRL2_PSEL1                         ((uint32_t)0x00000060) /*  */
#define OPA_CTRL2_PSEL1_0                       ((uint32_t)0x00000020) /*  */
#define OPA_CTRL2_PSEL1_1                       ((uint32_t)0x00000040) /*  */
#define OPA_CTRL2_PSEL1_CHP0                    ((uint32_t)0x00000000) /*  */
#define OPA_CTRL2_PSEL1_CHP1                    ((uint32_t)0x00000020) /*  */
#define OPA_CTRL2_PSEL1_CHP2                    ((uint32_t)0x00000040) /*  */
#define OPA_CTRL2_HYEN1                         ((uint32_t)0x00000080) /*  */
#define OPA_CTRL2_RMID1                         ((uint32_t)0x00000100) /*  */
#define OPA_CTRL2_CMP_EN2                       ((uint32_t)0x00010000) /*  */
#define OPA_CTRL2_FILT_EN                       ((uint32_t)0x01000000) /*  */
#define OPA_CTRL2_FILT_SEL                      ((uint32_t)0x02000000) /*  */
#define OPA_CTRL2_BKIN_CFG                      ((uint32_t)0x0C000000) /*  */
#define OPA_CTRL2_BKIN_CFG_0                    ((uint32_t)0x04000000) /*  */
#define OPA_CTRL2_BKIN_CFG_1                    ((uint32_t)0x08000000) /*  */
#define OPA_CTRL2_BKIN_CFG_IO                   ((uint32_t)0x00000000) /*  */
#define OPA_CTRL2_BKIN_CFG_CMP1                 ((uint32_t)0x04000000) /*  */
#define OPA_CTRL2_BKIN_CFG_CMP2                 ((uint32_t)0x08000000) /*  */
#define OPA_CTRL2_BKIN_CFG_OPA                  ((uint32_t)0x0C000000) /*  */
#define OPA_CTRL2_CMP_LOCK                      ((uint32_t)0x80000000) /*  */

/*******************  Bit definition for OPA_KEY register  ********************/
/*******************  Bit definition for CMP_KEY register  ********************/
/*******************  Bit definition for POLL_KEY register  *******************/
#define OPA_KEY1                                ((uint32_t)0x45670123) /*  */
#define OPA_KEY2                                ((uint32_t)0xCDEF89AB) /*  */

#ifdef __cplusplus
}
#endif



#ifdef __cplusplus
extern "C" {
#endif



/* ch32v00x_gpio.c -----------------------------------------------------------*/
/* MASK */
#define LSB_MASK                         ((uint16_t)0xFFFF)
#define DBGAFR_POSITION_MASK             ((uint32_t)0x000F0000)
#define DBGAFR_SDI_MASK                  ((uint32_t)0xF8FFFFFF)
#define DBGAFR_LOCATION_MASK             ((uint32_t)0x00200000)
#define DBGAFR_NUMBITS_MASK              ((uint32_t)0x00100000)


/* ch32v00x_adc.c ------------------------------------------------------------*/

/* ADC DISCNUM mask */
#define CTLR1_DISCNUM_Reset              ((uint32_t)0xFFFF1FFF)

/* ADC DISCEN mask */
#define CTLR1_DISCEN_Set                 ((uint32_t)0x00000800)
#define CTLR1_DISCEN_Reset               ((uint32_t)0xFFFFF7FF)

/* ADC JAUTO mask */
#define CTLR1_JAUTO_Set                  ((uint32_t)0x00000400)
#define CTLR1_JAUTO_Reset                ((uint32_t)0xFFFFFBFF)

/* ADC JDISCEN mask */
#define CTLR1_JDISCEN_Set                ((uint32_t)0x00001000)
#define CTLR1_JDISCEN_Reset              ((uint32_t)0xFFFFEFFF)

/* ADC AWDCH mask */
#define CTLR1_AWDCH_Reset                ((uint32_t)0xFFFFFFE0)

/* ADC Analog watchdog enable mode mask */
#define CTLR1_AWDMode_Reset              ((uint32_t)0xFF3FFDFF)

///* CTLR1 register Mask */
//Editor's Note: Overloaded Definition
#define ADC_CTLR1_CLEAR_Mask             ((uint32_t)0xFFF0FEFF)

/* ADC ADON mask */
#define CTLR2_ADON_Set                   ((uint32_t)0x00000001)
#define CTLR2_ADON_Reset                 ((uint32_t)0xFFFFFFFE)

/* ADC DMA mask */
#define CTLR2_DMA_Set                    ((uint32_t)0x00000100)
#define CTLR2_DMA_Reset                  ((uint32_t)0xFFFFFEFF)

/* ADC RSTCAL mask */
#define CTLR2_RSTCAL_Set                 ((uint32_t)0x00000008)

/* ADC CAL mask */
#define CTLR2_CAL_Set                    ((uint32_t)0x00000004)

/* ADC SWSTART mask */
#define CTLR2_SWSTART_Set                ((uint32_t)0x00400000)

/* ADC EXTTRIG mask */
#define CTLR2_EXTTRIG_Set                ((uint32_t)0x00100000)
#define CTLR2_EXTTRIG_Reset              ((uint32_t)0xFFEFFFFF)

/* ADC Software start mask */
#define CTLR2_EXTTRIG_SWSTART_Set        ((uint32_t)0x00500000)
#define CTLR2_EXTTRIG_SWSTART_Reset      ((uint32_t)0xFFAFFFFF)

/* ADC JEXTSEL mask */
#define CTLR2_JEXTSEL_Reset              ((uint32_t)0xFFFF8FFF)

/* ADC JEXTTRIG mask */
#define CTLR2_JEXTTRIG_Set               ((uint32_t)0x00008000)
#define CTLR2_JEXTTRIG_Reset             ((uint32_t)0xFFFF7FFF)

/* ADC JSWSTART mask */
#define CTLR2_JSWSTART_Set               ((uint32_t)0x00200000)

/* ADC injected software start mask */
#define CTLR2_JEXTTRIG_JSWSTART_Set      ((uint32_t)0x00208000)
#define CTLR2_JEXTTRIG_JSWSTART_Reset    ((uint32_t)0xFFDF7FFF)

/* ADC TSPD mask */
#define CTLR2_TSVREFE_Set                ((uint32_t)0x00800000)
#define CTLR2_TSVREFE_Reset              ((uint32_t)0xFF7FFFFF)

/* CTLR2 register Mask */
#define CTLR2_CLEAR_Mask                 ((uint32_t)0xFFF1F7FD)

/* ADC SQx mask */
#define RSQR3_SQ_Set                     ((uint32_t)0x0000001F)
#define RSQR2_SQ_Set                     ((uint32_t)0x0000001F)
#define RSQR1_SQ_Set                     ((uint32_t)0x0000001F)

/* RSQR1 register Mask */
#define RSQR1_CLEAR_Mask                 ((uint32_t)0xFF0FFFFF)

/* ADC JSQx mask */
#define ISQR_JSQ_Set                     ((uint32_t)0x0000001F)

/* ADC JL mask */
#define ISQR_JL_Set                      ((uint32_t)0x00300000)
#define ISQR_JL_Reset                    ((uint32_t)0xFFCFFFFF)

/* ADC SMPx mask */
#define SAMPTR1_SMP_Set                  ((uint32_t)0x00000007)
#define SAMPTR2_SMP_Set                  ((uint32_t)0x00000007)

/* ADC IDATARx registers offset */
#define IDATAR_Offset                    ((uint8_t)0x28)




/* ch32v00x_dbgmcu.c ---------------------------------------------------------*/
#define IDCODE_DEVID_MASK    ((uint32_t)0x0000FFFF)


/* ch32v00x_dma.c ------------------------------------------------------------*/

/* DMA1 Channelx interrupt pending bit masks */
#define DMA1_Channel1_IT_Mask    ((uint32_t)(DMA_GIF1 | DMA_TCIF1 | DMA_HTIF1 | DMA_TEIF1))
#define DMA1_Channel2_IT_Mask    ((uint32_t)(DMA_GIF2 | DMA_TCIF2 | DMA_HTIF2 | DMA_TEIF2))
#define DMA1_Channel3_IT_Mask    ((uint32_t)(DMA_GIF3 | DMA_TCIF3 | DMA_HTIF3 | DMA_TEIF3))
#define DMA1_Channel4_IT_Mask    ((uint32_t)(DMA_GIF4 | DMA_TCIF4 | DMA_HTIF4 | DMA_TEIF4))
#define DMA1_Channel5_IT_Mask    ((uint32_t)(DMA_GIF5 | DMA_TCIF5 | DMA_HTIF5 | DMA_TEIF5))
#define DMA1_Channel6_IT_Mask    ((uint32_t)(DMA_GIF6 | DMA_TCIF6 | DMA_HTIF6 | DMA_TEIF6))
#define DMA1_Channel7_IT_Mask    ((uint32_t)(DMA_GIF7 | DMA_TCIF7 | DMA_HTIF7 | DMA_TEIF7))


/* DMA2 FLAG mask */
// Editor's note: Overloaded Definition.
#define DMA2_FLAG_Mask            ((uint32_t)0x10000000)

/* DMA registers Masks */
#define CFGR_CLEAR_Mask          ((uint32_t)0xFFFF800F)

/* ch32v00x_exti.c -----------------------------------------------------------*/


/* No interrupt selected */
#define EXTI_LINENONE    ((uint32_t)0x00000)

/* ch32v00x_flash.c ----------------------------------------------------------*/

/* Flash Access Control Register bits */
#define ACR_LATENCY_Mask           ((uint32_t)0x00000038)


/* Flash Control Register bits */
#define CR_PG_Set                  ((uint32_t)0x00000001)
#define CR_PG_Reset                ((uint32_t)0xFFFFFFFE)
#define CR_PER_Set                 ((uint32_t)0x00000002)
#define CR_PER_Reset               ((uint32_t)0xFFFFFFFD)
#define CR_MER_Set                 ((uint32_t)0x00000004)
#define CR_MER_Reset               ((uint32_t)0xFFFFFFFB)
#define CR_OPTPG_Set               ((uint32_t)0x00000010)
#define CR_OPTPG_Reset             ((uint32_t)0xFFFFFFEF)
#define CR_OPTER_Set               ((uint32_t)0x00000020)
#define CR_OPTER_Reset             ((uint32_t)0xFFFFFFDF)
#define CR_STRT_Set                ((uint32_t)0x00000040)
#define CR_LOCK_Set                ((uint32_t)0x00000080)
#define CR_PAGE_PG                 ((uint32_t)0x00010000)
#define CR_PAGE_ER                 ((uint32_t)0x00020000)
#define CR_BUF_LOAD                ((uint32_t)0x00040000)
#define CR_BUF_RST                 ((uint32_t)0x00080000)

/* FLASH Status Register bits */
#define SR_BSY                     ((uint32_t)0x00000001)
#define SR_WRPRTERR                ((uint32_t)0x00000010)
#define SR_EOP                     ((uint32_t)0x00000020)

/* FLASH Mask */
#define RDPRT_Mask                 ((uint32_t)0x00000002)
#define WRP0_Mask                  ((uint32_t)0x000000FF)
#define WRP1_Mask                  ((uint32_t)0x0000FF00)
#define WRP2_Mask                  ((uint32_t)0x00FF0000)
#define WRP3_Mask                  ((uint32_t)0xFF000000)

/* FLASH Keys */
#define RDP_Key                    ((uint16_t)0x00A5)
#define FLASH_KEY1                 ((uint32_t)0x45670123)
#define FLASH_KEY2                 ((uint32_t)0xCDEF89AB)

/* FLASH BANK address */
#define FLASH_BANK1_END_ADDRESS    ((uint32_t)0x807FFFF)


/* Delay definition */
#define EraseTimeout               ((uint32_t)0x000B0000)
#define ProgramTimeout             ((uint32_t)0x00002000)

/* Flash Program Valid Address */
#define ValidAddrStart             (FLASH_BASE)
#define ValidAddrEnd               (FLASH_BASE + 0x4000)


/* ch32v00x_i2c.c ------------------------------------------------------------*/


/* I2C SPE mask */
#define CTLR1_PE_Set             ((uint16_t)0x0001)
#define CTLR1_PE_Reset           ((uint16_t)0xFFFE)

/* I2C START mask */
#define CTLR1_START_Set          ((uint16_t)0x0100)
#define CTLR1_START_Reset        ((uint16_t)0xFEFF)

/* I2C STOP mask */
#define CTLR1_STOP_Set           ((uint16_t)0x0200)
#define CTLR1_STOP_Reset         ((uint16_t)0xFDFF)

/* I2C ACK mask */
#define CTLR1_ACK_Set            ((uint16_t)0x0400)
#define CTLR1_ACK_Reset          ((uint16_t)0xFBFF)

/* I2C ENGC mask */
#define CTLR1_ENGC_Set           ((uint16_t)0x0040)
#define CTLR1_ENGC_Reset         ((uint16_t)0xFFBF)

/* I2C SWRST mask */
#define CTLR1_SWRST_Set          ((uint16_t)0x8000)
#define CTLR1_SWRST_Reset        ((uint16_t)0x7FFF)

/* I2C PEC mask */
#define CTLR1_PEC_Set            ((uint16_t)0x1000)
#define CTLR1_PEC_Reset          ((uint16_t)0xEFFF)

/* I2C ENPEC mask */
#define CTLR1_ENPEC_Set          ((uint16_t)0x0020)
#define CTLR1_ENPEC_Reset        ((uint16_t)0xFFDF)

/* I2C ENARP mask */
#define CTLR1_ENARP_Set          ((uint16_t)0x0010)
#define CTLR1_ENARP_Reset        ((uint16_t)0xFFEF)

/* I2C NOSTRETCH mask */
#define CTLR1_NOSTRETCH_Set      ((uint16_t)0x0080)
#define CTLR1_NOSTRETCH_Reset    ((uint16_t)0xFF7F)

////* I2C registers Masks */
// Editor's note: Overloaded Definition.
#define I2C_CTLR1_CLEAR_Mask     ((uint16_t)0xFBF5)

/* I2C DMAEN mask */
#define CTLR2_DMAEN_Set          ((uint16_t)0x0800)
#define CTLR2_DMAEN_Reset        ((uint16_t)0xF7FF)

/* I2C LAST mask */
#define CTLR2_LAST_Set           ((uint16_t)0x1000)
#define CTLR2_LAST_Reset         ((uint16_t)0xEFFF)

/* I2C FREQ mask */
#define CTLR2_FREQ_Reset         ((uint16_t)0xFFC0)

/* I2C ADD0 mask */
#define OADDR1_ADD0_Set          ((uint16_t)0x0001)
#define OADDR1_ADD0_Reset        ((uint16_t)0xFFFE)

/* I2C ENDUAL mask */
#define OADDR2_ENDUAL_Set        ((uint16_t)0x0001)
#define OADDR2_ENDUAL_Reset      ((uint16_t)0xFFFE)

/* I2C ADD2 mask */
#define OADDR2_ADD2_Reset        ((uint16_t)0xFF01)

/* I2C F/S mask */
#define CKCFGR_FS_Set            ((uint16_t)0x8000)

/* I2C CCR mask */
#define CKCFGR_CCR_Set           ((uint16_t)0x0FFF)

/* I2C FLAG mask */
//Editor's Note: Overloaded Definition
#define I2c_FLAG_Mask            ((uint32_t)0x00FFFFFF)

/* I2C Interrupt Enable mask */
#define ITEN_Mask                ((uint32_t)0x07000000)

/* ch32v00x_iwdg.c -----------------------------------------------------------*/

/* CTLR register bit mask */
#define CTLR_KEY_Reload    ((uint16_t)0xAAAA)
#define CTLR_KEY_Enable    ((uint16_t)0xCCCC)


/* ch32v00x_pwr.c ------------------------------------------------------------*/


/* PWR registers bit mask */
/* CTLR register bit mask */
#define CTLR_DS_MASK     ((uint32_t)0xFFFFFFFD)
#define CTLR_PLS_MASK    ((uint32_t)0xFFFFFF1F)
#define AWUPSC_MASK      ((uint32_t)0xFFFFFFF0)
#define AWUWR_MASK       ((uint32_t)0xFFFFFFC0)

/* ch32v00x_rcc.c ------------------------------------------------------------*/

/* RCC registers bit address in the alias region */
#define RCC_OFFSET                 (RCC_BASE - PERIPH_BASE)

/* BDCTLR Register */
#define BDCTLR_OFFSET              (RCC_OFFSET + 0x20)

/* RCC registers bit mask */

/* CTLR register bit mask */
#define CTLR_HSEBYP_Reset          ((uint32_t)0xFFFBFFFF)
#define CTLR_HSEBYP_Set            ((uint32_t)0x00040000)
#define CTLR_HSEON_Reset           ((uint32_t)0xFFFEFFFF)
#define CTLR_HSEON_Set             ((uint32_t)0x00010000)
#define CTLR_HSITRIM_Mask          ((uint32_t)0xFFFFFF07)

#define CFGR0_PLL_Mask             ((uint32_t)0xFFC0FFFF)
#define CFGR0_PLLMull_Mask         ((uint32_t)0x003C0000)
#define CFGR0_PLLSRC_Mask          ((uint32_t)0x00010000)
#define CFGR0_PLLXTPRE_Mask        ((uint32_t)0x00020000)
#define CFGR0_SWS_Mask             ((uint32_t)0x0000000C)
#define CFGR0_SW_Mask              ((uint32_t)0xFFFFFFFC)
#define CFGR0_HPRE_Reset_Mask      ((uint32_t)0xFFFFFF0F)
#define CFGR0_HPRE_Set_Mask        ((uint32_t)0x000000F0)
#define CFGR0_PPRE1_Reset_Mask     ((uint32_t)0xFFFFF8FF)
#define CFGR0_PPRE1_Set_Mask       ((uint32_t)0x00000700)
#define CFGR0_PPRE2_Reset_Mask     ((uint32_t)0xFFFFC7FF)
#define CFGR0_PPRE2_Set_Mask       ((uint32_t)0x00003800)
#define CFGR0_ADCPRE_Reset_Mask    ((uint32_t)0xFFFF07FF)
#define CFGR0_ADCPRE_Set_Mask      ((uint32_t)0x0000F800)

/* RSTSCKR register bit mask */
#define RSTSCKR_RMVF_Set           ((uint32_t)0x01000000)


/* RCC Flag Mask */
//Editor's Note: Overloaded Definition
#define RCC_FLAG_Mask              ((uint8_t)0x1F)

/* INTR register byte 2 (Bits[15:8]) base address */
#define INTR_BYTE2_ADDRESS         ((uint32_t)0x40021009)

/* INTR register byte 3 (Bits[23:16]) base address */
#define INTR_BYTE3_ADDRESS         ((uint32_t)0x4002100A)

/* CFGR0 register byte 4 (Bits[31:24]) base address */
#define CFGR0_BYTE4_ADDRESS        ((uint32_t)0x40021007)

/* BDCTLR register base address */
#define BDCTLR_ADDRESS             (PERIPH_BASE + BDCTLR_OFFSET)

#ifndef __ASSEMBLER__
static __I uint8_t APBAHBPrescTable[16] = {1, 2, 3, 4, 5, 6, 7, 8, 1, 2, 3, 4, 5, 6, 7, 8};
static __I uint8_t ADCPrescTable[20] = {2, 4, 6, 8, 4, 8, 12, 16, 8, 16, 24, 32, 16, 32, 48, 64, 32, 64, 96, 128};
#endif



/* ch32v00x_spi.c ------------------------------------------------------------*/

/* SPI SPE mask */
#define CTLR1_SPE_Set         ((uint16_t)0x0040)
#define CTLR1_SPE_Reset       ((uint16_t)0xFFBF)


/* SPI CRCNext mask */
#define CTLR1_CRCNext_Set     ((uint16_t)0x1000)

/* SPI CRCEN mask */
#define CTLR1_CRCEN_Set       ((uint16_t)0x2000)
#define CTLR1_CRCEN_Reset     ((uint16_t)0xDFFF)

/* SPI SSOE mask */
#define CTLR2_SSOE_Set        ((uint16_t)0x0004)
#define CTLR2_SSOE_Reset      ((uint16_t)0xFFFB)

/* SPI registers Masks */
//Editor's Note: Overloaded Definition
#define SPI_CTLR1_CLEAR_Mask  ((uint16_t)0x3040)
#define I2SCFGR_CLEAR_Mask    ((uint16_t)0xF040)



/* ch32v00x_tim.c ------------------------------------------------------------*/

/* TIM registers bit mask */
#define SMCFGR_ETR_Mask    ((uint16_t)0x00FF)
#define CHCTLR_Offset      ((uint16_t)0x0018)
#define CCER_CCE_Set       ((uint16_t)0x0001)
#define CCER_CCNE_Set      ((uint16_t)0x0004)

/* ch32v00x_usart.c ----------------------------------------------------------*/

/* USART_Private_Defines */
#define CTLR1_UE_Set              ((uint16_t)0x2000) /* USART Enable Mask */
#define CTLR1_UE_Reset            ((uint16_t)0xDFFF) /* USART Disable Mask */

#define CTLR1_WAKE_Mask           ((uint16_t)0xF7FF) /* USART WakeUp Method Mask */

#define CTLR1_RWU_Set             ((uint16_t)0x0002) /* USART mute mode Enable Mask */
#define CTLR1_RWU_Reset           ((uint16_t)0xFFFD) /* USART mute mode Enable Mask */
#define CTLR1_SBK_Set             ((uint16_t)0x0001) /* USART Break Character send Mask */
//Editor's Note: Overloaded Definition
#define USART_CTLR1_CLEAR_Mask    ((uint16_t)0xE9F3) /* USART CR1 Mask */
#define CTLR2_Address_Mask        ((uint16_t)0xFFF0) /* USART address Mask */

#define CTLR2_LINEN_Set           ((uint16_t)0x4000) /* USART LIN Enable Mask */
#define CTLR2_LINEN_Reset         ((uint16_t)0xBFFF) /* USART LIN Disable Mask */

#define CTLR2_LBDL_Mask           ((uint16_t)0xFFDF) /* USART LIN Break detection Mask */
#define CTLR2_STOP_CLEAR_Mask     ((uint16_t)0xCFFF) /* USART CR2 STOP Bits Mask */
#define CTLR2_CLOCK_CLEAR_Mask    ((uint16_t)0xF0FF) /* USART CR2 Clock Mask */

#define CTLR3_SCEN_Set            ((uint16_t)0x0020) /* USART SC Enable Mask */
#define CTLR3_SCEN_Reset          ((uint16_t)0xFFDF) /* USART SC Disable Mask */

#define CTLR3_NACK_Set            ((uint16_t)0x0010) /* USART SC NACK Enable Mask */
#define CTLR3_NACK_Reset          ((uint16_t)0xFFEF) /* USART SC NACK Disable Mask */

#define CTLR3_HDSEL_Set           ((uint16_t)0x0008) /* USART Half-Duplex Enable Mask */
#define CTLR3_HDSEL_Reset         ((uint16_t)0xFFF7) /* USART Half-Duplex Disable Mask */

#define CTLR3_IRLP_Mask           ((uint16_t)0xFFFB) /* USART IrDA LowPower mode Mask */
#define CTLR3_CLEAR_Mask          ((uint16_t)0xFCFF) /* USART CR3 Mask */

#define CTLR3_IREN_Set            ((uint16_t)0x0002) /* USART IrDA Enable Mask */
#define CTLR3_IREN_Reset          ((uint16_t)0xFFFD) /* USART IrDA Disable Mask */
#define GPR_LSB_Mask              ((uint16_t)0x00FF) /* Guard Time Register LSB Mask */
#define GPR_MSB_Mask              ((uint16_t)0xFF00) /* Guard Time Register MSB Mask */
#define IT_Mask                   ((uint16_t)0x001F) /* USART Interrupt Mask */

/* USART OverSampling-8 Mask */
#define CTLR1_OVER8_Set           ((uint16_t)0x8000) /* USART OVER8 mode Enable Mask */
#define CTLR1_OVER8_Reset         ((uint16_t)0x7FFF) /* USART OVER8 mode Disable Mask */

/* USART One Bit Sampling Mask */
#define CTLR3_ONEBITE_Set         ((uint16_t)0x0800) /* USART ONEBITE mode Enable Mask */
#define CTLR3_ONEBITE_Reset       ((uint16_t)0xF7FF) /* USART ONEBITE mode Disable Mask */

/* ch32v00x_wwdg.c ------------------------------------------------------------*/

/* CTLR register bit mask */
#define CTLR_WDGA_Set      ((uint32_t)0x00000080)

/* CFGR register bit mask */
#define CFGR_WDGTB_Mask    ((uint32_t)0xFFFFFE7F)
#define CFGR_W_Mask        ((uint32_t)0xFFFFFF80)
#define BIT_Mask           ((uint8_t)0x7F)


/* ch32v00x_adc.h ------------------------------------------------------------*/

/* ADC_mode */
#define ADC_Mode_Independent                           ((uint32_t)0x00000000)

/* ADC_external_trigger_sources_for_regular_channels_conversion */

#define ADC_ExternalTrigConv_T1_TRGO                   ((uint32_t)0x00000000)
#define ADC_ExternalTrigConv_T1_CC1                    ((uint32_t)0x00020000)
#define ADC_ExternalTrigConv_T1_CC2                    ((uint32_t)0x00040000)
#define ADC_ExternalTrigConv_T2_TRGO                   ((uint32_t)0x00060000)
#define ADC_ExternalTrigConv_T2_CC1                    ((uint32_t)0x00080000)
#define ADC_ExternalTrigConv_T2_CC2                    ((uint32_t)0x000A0000)
#define ADC_ExternalTrigConv_Ext_PD3_PC2               ((uint32_t)0x000C0000)
#define ADC_ExternalTrigConv_None                      ((uint32_t)0x000E0000)



/* ADC_data_align */
#define ADC_DataAlign_Right                            ((uint32_t)0x00000000)
#define ADC_DataAlign_Left                             ((uint32_t)0x00000800)

/* ADC_channels */
#define ADC_Channel_0                                  ((uint8_t)0x00)
#define ADC_Channel_1                                  ((uint8_t)0x01)
#define ADC_Channel_2                                  ((uint8_t)0x02)
#define ADC_Channel_3                                  ((uint8_t)0x03)
#define ADC_Channel_4                                  ((uint8_t)0x04)
#define ADC_Channel_5                                  ((uint8_t)0x05)
#define ADC_Channel_6                                  ((uint8_t)0x06)
#define ADC_Channel_7                                  ((uint8_t)0x07)
#define ADC_Channel_8                                  ((uint8_t)0x08)
#define ADC_Channel_9                                  ((uint8_t)0x09)

#define ADC_Channel_Vrefint                            ((uint8_t)ADC_Channel_8)
#define ADC_Channel_Vcalint                            ((uint8_t)ADC_Channel_9)


/* ADC_sampling_time */
#define ADC_SampleTime_3Cycles                         ((uint8_t)0x00)
#define ADC_SampleTime_9Cycles                         ((uint8_t)0x01)
#define ADC_SampleTime_15Cycles                        ((uint8_t)0x02)
#define ADC_SampleTime_30Cycles                        ((uint8_t)0x03)
#define ADC_SampleTime_43Cycles                        ((uint8_t)0x04)
#define ADC_SampleTime_57Cycles                        ((uint8_t)0x05)
#define ADC_SampleTime_73Cycles                        ((uint8_t)0x06)
#define ADC_SampleTime_241Cycles                       ((uint8_t)0x07)

/* ADC_external_trigger_sources_for_injected_channels_conversion */
#define ADC_ExternalTrigInjecConv_T1_CC3               ((uint32_t)0x00000000)
#define ADC_ExternalTrigInjecConv_T1_CC4               ((uint32_t)0x00001000)
#define ADC_ExternalTrigInjecConv_T2_CC3               ((uint32_t)0x00002000)
#define ADC_ExternalTrigInjecConv_T2_CC4               ((uint32_t)0x00003000)
#define ADC_ExternalTrigInjecConv_Ext_PD1_PA2          ((uint32_t)0x00006000)
#define ADC_ExternalTrigInjecConv_None                 ((uint32_t)0x00007000)


/* ADC_injected_channel_selection */
#define ADC_InjectedChannel_1                          ((uint8_t)0x14)
#define ADC_InjectedChannel_2                          ((uint8_t)0x18)
#define ADC_InjectedChannel_3                          ((uint8_t)0x1C)
#define ADC_InjectedChannel_4                          ((uint8_t)0x20)

/* ADC_analog_watchdog_selection */
#define ADC_AnalogWatchdog_SingleRegEnable             ((uint32_t)0x00800200)
#define ADC_AnalogWatchdog_SingleInjecEnable           ((uint32_t)0x00400200)
#define ADC_AnalogWatchdog_SingleRegOrInjecEnable      ((uint32_t)0x00C00200)
#define ADC_AnalogWatchdog_AllRegEnable                ((uint32_t)0x00800000)
#define ADC_AnalogWatchdog_AllInjecEnable              ((uint32_t)0x00400000)
#define ADC_AnalogWatchdog_AllRegAllInjecEnable        ((uint32_t)0x00C00000)
#define ADC_AnalogWatchdog_None                        ((uint32_t)0x00000000)

/* ADC_interrupts_definition */
#define ADC_IT_EOC                                     ((uint16_t)0x0220)
#define ADC_IT_AWD                                     ((uint16_t)0x0140)
#define ADC_IT_JEOC                                    ((uint16_t)0x0480)

/* ADC_flags_definition */
#define ADC_FLAG_AWD                                   ((uint8_t)0x01)
#define ADC_FLAG_EOC                                   ((uint8_t)0x02)
#define ADC_FLAG_JEOC                                  ((uint8_t)0x04)
#define ADC_FLAG_JSTRT                                 ((uint8_t)0x08)
#define ADC_FLAG_STRT                                  ((uint8_t)0x10)

/* ADC_calibration_voltage_definition */
#define ADC_CALVOL_50PERCENT                           ((uint32_t)0x02000000)
#define ADC_CALVOL_75PERCENT                           ((uint32_t)0x04000000)

/* ADC_external_trigger_sources_delay_channels_definition */
#define ADC_ExternalTrigRegul_DLY                      ((uint32_t)0x00000000)
#define ADC_ExternalTrigInjec_DLY                      ((uint32_t)0x00020000)




/* ch32v00x_dbgmcu.h ---------------------------------------------------------*/

/* DBGMCU_CR Register */
#define DBGMCU_SLEEP                 ((uint32_t)0x00000001)
#define DBGMCU_STOP                  ((uint32_t)0x00000002)
#define DBGMCU_STANDBY               ((uint32_t)0x00000004)
#define DBGMCU_IWDG_STOP             ((uint32_t)0x00000100)
#define DBGMCU_WWDG_STOP             ((uint32_t)0x00000200)
#define DBGMCU_TIM1_STOP             ((uint32_t)0x00001000)
#define DBGMCU_TIM2_STOP             ((uint32_t)0x00002000)

/* ch32v00x_dma.h ------------------------------------------------------------*/

/* DMA_data_transfer_direction */
#define DMA_DIR_PeripheralDST              ((uint32_t)0x00000010)
#define DMA_DIR_PeripheralSRC              ((uint32_t)0x00000000)

/* DMA_peripheral_incremented_mode */
#define DMA_PeripheralInc_Enable           ((uint32_t)0x00000040)
#define DMA_PeripheralInc_Disable          ((uint32_t)0x00000000)

/* DMA_memory_incremented_mode */
#define DMA_MemoryInc_Enable               ((uint32_t)0x00000080)
#define DMA_MemoryInc_Disable              ((uint32_t)0x00000000)

/* DMA_peripheral_data_size */
#define DMA_PeripheralDataSize_Byte        ((uint32_t)0x00000000)
#define DMA_PeripheralDataSize_HalfWord    ((uint32_t)0x00000100)
#define DMA_PeripheralDataSize_Word        ((uint32_t)0x00000200)

/* DMA_memory_data_size */
#define DMA_MemoryDataSize_Byte            ((uint32_t)0x00000000)
#define DMA_MemoryDataSize_HalfWord        ((uint32_t)0x00000400)
#define DMA_MemoryDataSize_Word            ((uint32_t)0x00000800)

/* DMA_circular_normal_mode */
#define DMA_Mode_Circular                  ((uint32_t)0x00000020)
#define DMA_Mode_Normal                    ((uint32_t)0x00000000)

/* DMA_priority_level */
#define DMA_Priority_VeryHigh              ((uint32_t)0x00003000)
#define DMA_Priority_High                  ((uint32_t)0x00002000)
#define DMA_Priority_Medium                ((uint32_t)0x00001000)
#define DMA_Priority_Low                   ((uint32_t)0x00000000)

/* DMA_memory_to_memory */
#define DMA_M2M_Enable                     ((uint32_t)0x00004000)
#define DMA_M2M_Disable                    ((uint32_t)0x00000000)

/* DMA_interrupts_definition */
#define DMA_IT_TC                          ((uint32_t)0x00000002)
#define DMA_IT_HT                          ((uint32_t)0x00000004)
#define DMA_IT_TE                          ((uint32_t)0x00000008)

#define DMA1_IT_GL1                        ((uint32_t)0x00000001)
#define DMA1_IT_TC1                        ((uint32_t)0x00000002)
#define DMA1_IT_HT1                        ((uint32_t)0x00000004)
#define DMA1_IT_TE1                        ((uint32_t)0x00000008)
#define DMA1_IT_GL2                        ((uint32_t)0x00000010)
#define DMA1_IT_TC2                        ((uint32_t)0x00000020)
#define DMA1_IT_HT2                        ((uint32_t)0x00000040)
#define DMA1_IT_TE2                        ((uint32_t)0x00000080)
#define DMA1_IT_GL3                        ((uint32_t)0x00000100)
#define DMA1_IT_TC3                        ((uint32_t)0x00000200)
#define DMA1_IT_HT3                        ((uint32_t)0x00000400)
#define DMA1_IT_TE3                        ((uint32_t)0x00000800)
#define DMA1_IT_GL4                        ((uint32_t)0x00001000)
#define DMA1_IT_TC4                        ((uint32_t)0x00002000)
#define DMA1_IT_HT4                        ((uint32_t)0x00004000)
#define DMA1_IT_TE4                        ((uint32_t)0x00008000)
#define DMA1_IT_GL5                        ((uint32_t)0x00010000)
#define DMA1_IT_TC5                        ((uint32_t)0x00020000)
#define DMA1_IT_HT5                        ((uint32_t)0x00040000)
#define DMA1_IT_TE5                        ((uint32_t)0x00080000)
#define DMA1_IT_GL6                        ((uint32_t)0x00100000)
#define DMA1_IT_TC6                        ((uint32_t)0x00200000)
#define DMA1_IT_HT6                        ((uint32_t)0x00400000)
#define DMA1_IT_TE6                        ((uint32_t)0x00800000)
#define DMA1_IT_GL7                        ((uint32_t)0x01000000)
#define DMA1_IT_TC7                        ((uint32_t)0x02000000)
#define DMA1_IT_HT7                        ((uint32_t)0x04000000)
#define DMA1_IT_TE7                        ((uint32_t)0x08000000)


/* DMA_flags_definition */
#define DMA1_FLAG_GL1                      ((uint32_t)0x00000001)
#define DMA1_FLAG_TC1                      ((uint32_t)0x00000002)
#define DMA1_FLAG_HT1                      ((uint32_t)0x00000004)
#define DMA1_FLAG_TE1                      ((uint32_t)0x00000008)
#define DMA1_FLAG_GL2                      ((uint32_t)0x00000010)
#define DMA1_FLAG_TC2                      ((uint32_t)0x00000020)
#define DMA1_FLAG_HT2                      ((uint32_t)0x00000040)
#define DMA1_FLAG_TE2                      ((uint32_t)0x00000080)
#define DMA1_FLAG_GL3                      ((uint32_t)0x00000100)
#define DMA1_FLAG_TC3                      ((uint32_t)0x00000200)
#define DMA1_FLAG_HT3                      ((uint32_t)0x00000400)
#define DMA1_FLAG_TE3                      ((uint32_t)0x00000800)
#define DMA1_FLAG_GL4                      ((uint32_t)0x00001000)
#define DMA1_FLAG_TC4                      ((uint32_t)0x00002000)
#define DMA1_FLAG_HT4                      ((uint32_t)0x00004000)
#define DMA1_FLAG_TE4                      ((uint32_t)0x00008000)
#define DMA1_FLAG_GL5                      ((uint32_t)0x00010000)
#define DMA1_FLAG_TC5                      ((uint32_t)0x00020000)
#define DMA1_FLAG_HT5                      ((uint32_t)0x00040000)
#define DMA1_FLAG_TE5                      ((uint32_t)0x00080000)
#define DMA1_FLAG_GL6                      ((uint32_t)0x00100000)
#define DMA1_FLAG_TC6                      ((uint32_t)0x00200000)
#define DMA1_FLAG_HT6                      ((uint32_t)0x00400000)
#define DMA1_FLAG_TE6                      ((uint32_t)0x00800000)
#define DMA1_FLAG_GL7                      ((uint32_t)0x01000000)
#define DMA1_FLAG_TC7                      ((uint32_t)0x02000000)
#define DMA1_FLAG_HT7                      ((uint32_t)0x04000000)
#define DMA1_FLAG_TE7                      ((uint32_t)0x08000000)




/* ch32v00x_exti.h -----------------------------------------------------------*/

#ifndef __ASSEMBLER__

/* EXTI mode enumeration */
typedef enum
{
    EXTI_Mode_Interrupt = 0x00,
    EXTI_Mode_Event = 0x04
} EXTIMode_TypeDef;

/* EXTI Trigger enumeration */
typedef enum
{
    EXTI_Trigger_Rising = 0x08,
    EXTI_Trigger_Falling = 0x0C,
    EXTI_Trigger_Rising_Falling = 0x10
} EXTITrigger_TypeDef;

#endif

/* EXTI_Lines */
#define EXTI_Line0     ((uint32_t)0x00001) /* External interrupt line 0 */
#define EXTI_Line1     ((uint32_t)0x00002) /* External interrupt line 1 */
#define EXTI_Line2     ((uint32_t)0x00004) /* External interrupt line 2 */
#define EXTI_Line3     ((uint32_t)0x00008) /* External interrupt line 3 */
#define EXTI_Line4     ((uint32_t)0x00010) /* External interrupt line 4 */
#define EXTI_Line5     ((uint32_t)0x00020) /* External interrupt line 5 */
#define EXTI_Line6     ((uint32_t)0x00040) /* External interrupt line 6 */
#define EXTI_Line7     ((uint32_t)0x00080) /* External interrupt line 7 */
#define EXTI_Line8     ((uint32_t)0x00100) /* External interrupt line 8 Connected to the PVD Output */
#define EXTI_Line9     ((uint32_t)0x00200) /* External interrupt line 9 Connected to the PWR Auto Wake-up event*/

/* ch32v00x_flash.h ----------------------------------------------------------*/


#ifndef __ASSEMBLER__
/* FLASH Status */
typedef enum
{
    FLASH_BUSY = 1,
    FLASH_ERROR_PG,
    FLASH_ERROR_WRP,
    FLASH_COMPLETE,
    FLASH_TIMEOUT
} FLASH_Status;
#endif

/* Flash_Latency */
#define FLASH_Latency_0                  ((uint32_t)0x00000000) /* FLASH Zero Latency cycle */
#define FLASH_Latency_1                  ((uint32_t)0x00000001) /* FLASH One Latency cycle */
#define FLASH_Latency_2                  ((uint32_t)0x00000002) /* FLASH Two Latency cycles */


/* Values to be used with CH32V00x devices (1page = 64Byte) */
#define FLASH_WRProt_Pages0to15          ((uint32_t)0x00000001) /* CH32 Low and Medium density devices: Write protection of page 0 to 15 */
#define FLASH_WRProt_Pages16to31         ((uint32_t)0x00000002) /* CH32 Low and Medium density devices: Write protection of page 16 to 31 */
#define FLASH_WRProt_Pages32to47         ((uint32_t)0x00000004) /* CH32 Low and Medium density devices: Write protection of page 32 to 47 */
#define FLASH_WRProt_Pages48to63         ((uint32_t)0x00000008) /* CH32 Low and Medium density devices: Write protection of page 48 to 63 */
#define FLASH_WRProt_Pages64to79         ((uint32_t)0x00000010) /* CH32 Low and Medium density devices: Write protection of page 64 to 79 */
#define FLASH_WRProt_Pages80to95         ((uint32_t)0x00000020) /* CH32 Low and Medium density devices: Write protection of page 80 to 95 */
#define FLASH_WRProt_Pages96to111        ((uint32_t)0x00000040) /* CH32 Low and Medium density devices: Write protection of page 96 to 111 */
#define FLASH_WRProt_Pages112to127       ((uint32_t)0x00000080) /* CH32 Low and Medium density devices: Write protection of page 112 to 127 */
#define FLASH_WRProt_Pages128to143       ((uint32_t)0x00000100) /* CH32 Medium-density devices: Write protection of page 128 to 143 */
#define FLASH_WRProt_Pages144to159       ((uint32_t)0x00000200) /* CH32 Medium-density devices: Write protection of page 144 to 159 */
#define FLASH_WRProt_Pages160to175       ((uint32_t)0x00000400) /* CH32 Medium-density devices: Write protection of page 160 to 175 */
#define FLASH_WRProt_Pages176to191       ((uint32_t)0x00000800) /* CH32 Medium-density devices: Write protection of page 176 to 191 */
#define FLASH_WRProt_Pages192to207       ((uint32_t)0x00001000) /* CH32 Medium-density devices: Write protection of page 192 to 207 */
#define FLASH_WRProt_Pages208to223       ((uint32_t)0x00002000) /* CH32 Medium-density devices: Write protection of page 208 to 223 */
#define FLASH_WRProt_Pages224to239       ((uint32_t)0x00004000) /* CH32 Medium-density devices: Write protection of page 224 to 239 */
#define FLASH_WRProt_Pages240to255       ((uint32_t)0x00008000) /* CH32 Medium-density devices: Write protection of page 240 to 255 */

#define FLASH_WRProt_AllPages            ((uint32_t)0x0000FFFF) /* Write protection of all Pages */


/* Option_Bytes_IWatchdog */
#define OB_IWDG_SW                       ((uint16_t)0x0001) /* Software IWDG selected */
#define OB_IWDG_HW                       ((uint16_t)0x0000) /* Hardware IWDG selected */

/* Option_Bytes_nRST_STOP */
#define OB_STOP_NoRST                    ((uint16_t)0x0002) /* No reset generated when entering in STOP */
#define OB_STOP_RST                      ((uint16_t)0x0000) /* Reset generated when entering in STOP */

/* Option_Bytes_nRST_STDBY */
#define OB_STDBY_NoRST                   ((uint16_t)0x0004) /* No reset generated when entering in STANDBY */
#define OB_STDBY_RST                     ((uint16_t)0x0000) /* Reset generated when entering in STANDBY */

/* Option_Bytes_RST_ENandDT */
#define OB_RST_NoEN                      ((uint16_t)0x0018) /* Reset IO disable (PD7)*/
#define OB_RST_EN_DT12ms                 ((uint16_t)0x0010) /* Reset IO enable (PD7) and  Ignore delay time 12ms */
#define OB_RST_EN_DT1ms                  ((uint16_t)0x0008) /* Reset IO enable (PD7) and  Ignore delay time 1ms */
#define OB_RST_EN_DT128ms                ((uint16_t)0x0000) /* Reset IO enable (PD7) and  Ignore delay time 128ms */

#define OB_STARTMODE_BOOT                ((uint16_t)0x0020) /* Start in BOOT area */
#define OB_STARTMODE_USER                ((uint16_t)0x0000) /* Start in user area */

/* FLASH_Interrupts */
#define FLASH_IT_ERROR                   ((uint32_t)0x00000400) /* FPEC error interrupt source */
#define FLASH_IT_EOP                     ((uint32_t)0x00001000) /* End of FLASH Operation Interrupt source */
#define FLASH_IT_BANK1_ERROR             FLASH_IT_ERROR         /* FPEC BANK1 error interrupt source */
#define FLASH_IT_BANK1_EOP               FLASH_IT_EOP           /* End of FLASH BANK1 Operation Interrupt source */

/* FLASH_Flags */
#define FLASH_FLAG_BSY                   ((uint32_t)0x00000001) /* FLASH Busy flag */
#define FLASH_FLAG_EOP                   ((uint32_t)0x00000020) /* FLASH End of Operation flag */
#define FLASH_FLAG_WRPRTERR              ((uint32_t)0x00000010) /* FLASH Write protected error flag */
#define FLASH_FLAG_OPTERR                ((uint32_t)0x00000001) /* FLASH Option Byte error flag */

#define FLASH_FLAG_BANK1_BSY             FLASH_FLAG_BSY       /* FLASH BANK1 Busy flag*/
#define FLASH_FLAG_BANK1_EOP             FLASH_FLAG_EOP       /* FLASH BANK1 End of Operation flag */
#define FLASH_FLAG_BANK1_WRPRTERR        FLASH_FLAG_WRPRTERR  /* FLASH BANK1 Write protected error flag */


/* System_Reset_Start_Mode */
#define Start_Mode_USER                  ((uint32_t)0x00000000)
#define Start_Mode_BOOT                  ((uint32_t)0x00004000)


/* ch32v00x_gpio.h ------------------------------------------------------------*/

#ifndef __ASSEMBLER__

/* Output Maximum frequency selection */
typedef enum
{
	GPIO_Speed_In = 0,
	GPIO_Speed_10MHz,
	GPIO_Speed_2MHz,
	GPIO_Speed_50MHz
} GPIOSpeed_TypeDef;

#endif

#define GPIO_CNF_IN_ANALOG   0
#define GPIO_CNF_IN_FLOATING 4
#define GPIO_CNF_IN_PUPD     8
#define GPIO_CNF_OUT_PP      0
#define GPIO_CNF_OUT_OD      4
#define GPIO_CNF_OUT_PP_AF   8
#define GPIO_CNF_OUT_OD_AF   12

/* Configuration Mode enumeration */
/*
typedef enum
{
    GPIO_Mode_AIN = 0x0,
    GPIO_Mode_IN_FLOATING = 0x04,
    GPIO_Mode_IPD = 0x28,
    GPIO_Mode_IPU = 0x48,
    GPIO_Mode_Out_OD = 0x14,
    GPIO_Mode_Out_PP = 0x10,
    GPIO_Mode_AF_OD = 0x1C,
    GPIO_Mode_AF_PP = 0x18
} GPIOMode_TypeDef;
*/

#ifndef __ASSEMBLER__

/* Bit_SET and Bit_RESET enumeration */
typedef enum
{
    Bit_RESET = 0,
    Bit_SET
} BitAction;

#endif

/* GPIO_pins_define */
#define GPIO_Pin_0                     ((uint16_t)0x0001) /* Pin 0 selected */
#define GPIO_Pin_1                     ((uint16_t)0x0002) /* Pin 1 selected */
#define GPIO_Pin_2                     ((uint16_t)0x0004) /* Pin 2 selected */
#define GPIO_Pin_3                     ((uint16_t)0x0008) /* Pin 3 selected */
#define GPIO_Pin_4                     ((uint16_t)0x0010) /* Pin 4 selected */
#define GPIO_Pin_5                     ((uint16_t)0x0020) /* Pin 5 selected */
#define GPIO_Pin_6                     ((uint16_t)0x0040) /* Pin 6 selected */
#define GPIO_Pin_7                     ((uint16_t)0x0080) /* Pin 7 selected */
#define GPIO_Pin_All                   ((uint16_t)0xFFFF) /* All pins selected */

/* GPIO_Remap_define */

#define GPIO_Remap_SPI1                ((uint32_t)0x00000001) /* SPI1 Alternate Function mapping */
#define GPIO_PartialRemap_I2C1         ((uint32_t)0x10000002) /* I2C1 Partial Alternate Function mapping */
#define GPIO_FullRemap_I2C1            ((uint32_t)0x10400002) /* I2C1 Full Alternate Function mapping */
#define GPIO_PartialRemap1_USART1      ((uint32_t)0x80000004) /* USART1 Partial1 Alternate Function mapping */
#define GPIO_PartialRemap2_USART1      ((uint32_t)0x80200000) /* USART1 Partial2 Alternate Function mapping */
#define GPIO_FullRemap_USART1          ((uint32_t)0x80200004) /* USART1 Full Alternate Function mapping */
#define GPIO_PartialRemap1_TIM1        ((uint32_t)0x00160040) /* TIM1 Partial1 Alternate Function mapping */
#define GPIO_PartialRemap2_TIM1        ((uint32_t)0x00160080) /* TIM1 Partial2 Alternate Function mapping */
#define GPIO_FullRemap_TIM1            ((uint32_t)0x001600C0) /* TIM1 Full Alternate Function mapping */
#define GPIO_PartialRemap1_TIM2        ((uint32_t)0x00180100) /* TIM2 Partial1 Alternate Function mapping */
#define GPIO_PartialRemap2_TIM2        ((uint32_t)0x00180200) /* TIM2 Partial2 Alternate Function mapping */
#define GPIO_FullRemap_TIM2            ((uint32_t)0x00180300) /* TIM2 Full Alternate Function mapping */
#define GPIO_Remap_PA1_2               ((uint32_t)0x00008000) /* PA1 and PA2 Alternate Function mapping */
#define GPIO_Remap_ADC1_ETRGINJ        ((uint32_t)0x00200002) /* ADC1 External Trigger Injected Conversion remapping */
#define GPIO_Remap_ADC1_ETRGREG        ((uint32_t)0x00200004) /* ADC1 External Trigger Regular Conversion remapping */
#define GPIO_Remap_LSI_CAL             ((uint32_t)0x00200080) /* LSI calibration Alternate Function mapping */
#define GPIO_Remap_SDI_Disable         ((uint32_t)0x00300400) /* SDI Disabled */


/* GPIO_Port_Sources */
#define GPIO_PortSourceGPIOA           ((uint8_t)0x00)
#define GPIO_PortSourceGPIOB           ((uint8_t)0x01)
#define GPIO_PortSourceGPIOC           ((uint8_t)0x02)
#define GPIO_PortSourceGPIOD           ((uint8_t)0x03)

/* GPIO_Pin_sources */
#define GPIO_PinSource0                ((uint8_t)0x00)
#define GPIO_PinSource1                ((uint8_t)0x01)
#define GPIO_PinSource2                ((uint8_t)0x02)
#define GPIO_PinSource3                ((uint8_t)0x03)
#define GPIO_PinSource4                ((uint8_t)0x04)
#define GPIO_PinSource5                ((uint8_t)0x05)
#define GPIO_PinSource6                ((uint8_t)0x06)
#define GPIO_PinSource7                ((uint8_t)0x07)


/* ch32v00x_i2c.h ------------------------------------------------------------*/

/* I2C_mode */
#define I2C_Mode_I2C                                         ((uint16_t)0x0000)

/* I2C_duty_cycle_in_fast_mode */
#define I2C_DutyCycle_16_9                                   ((uint16_t)0x4000) /* I2C fast mode Tlow/Thigh = 16/9 */
#define I2C_DutyCycle_2                                      ((uint16_t)0xBFFF) /* I2C fast mode Tlow/Thigh = 2 */

/* I2C_acknowledgement */
#define I2C_Ack_Enable                                       ((uint16_t)0x0400)
#define I2C_Ack_Disable                                      ((uint16_t)0x0000)

/* I2C_transfer_direction */
#define I2C_Direction_Transmitter                            ((uint8_t)0x00)
#define I2C_Direction_Receiver                               ((uint8_t)0x01)

/* I2C_acknowledged_address */
#define I2C_AcknowledgedAddress_7bit                         ((uint16_t)0x4000)
#define I2C_AcknowledgedAddress_10bit                        ((uint16_t)0xC000)

/* I2C_registers */
#define I2C_Register_CTLR1                                   ((uint8_t)0x00)
#define I2C_Register_CTLR2                                   ((uint8_t)0x04)
#define I2C_Register_OADDR1                                  ((uint8_t)0x08)
#define I2C_Register_OADDR2                                  ((uint8_t)0x0C)
#define I2C_Register_DATAR                                   ((uint8_t)0x10)
#define I2C_Register_STAR1                                   ((uint8_t)0x14)
#define I2C_Register_STAR2                                   ((uint8_t)0x18)
#define I2C_Register_CKCFGR                                  ((uint8_t)0x1C)

/* I2C_PEC_position */
#define I2C_PECPosition_Next                                 ((uint16_t)0x0800)
#define I2C_PECPosition_Current                              ((uint16_t)0xF7FF)

/* I2C_NACK_position */
#define I2C_NACKPosition_Next                                ((uint16_t)0x0800)
#define I2C_NACKPosition_Current                             ((uint16_t)0xF7FF)

/* I2C_interrupts_definition */
#define I2C_IT_BUF                                           ((uint16_t)0x0400)
#define I2C_IT_EVT                                           ((uint16_t)0x0200)
#define I2C_IT_ERR                                           ((uint16_t)0x0100)

/* I2C_interrupts_definition */
#define I2C_IT_PECERR                                        ((uint32_t)0x01001000)
#define I2C_IT_OVR                                           ((uint32_t)0x01000800)
#define I2C_IT_AF                                            ((uint32_t)0x01000400)
#define I2C_IT_ARLO                                          ((uint32_t)0x01000200)
#define I2C_IT_BERR                                          ((uint32_t)0x01000100)
#define I2C_IT_TXE                                           ((uint32_t)0x06000080)
#define I2C_IT_RXNE                                          ((uint32_t)0x06000040)
#define I2C_IT_STOPF                                         ((uint32_t)0x02000010)
#define I2C_IT_ADD10                                         ((uint32_t)0x02000008)
#define I2C_IT_BTF                                           ((uint32_t)0x02000004)
#define I2C_IT_ADDR                                          ((uint32_t)0x02000002)
#define I2C_IT_SB                                            ((uint32_t)0x02000001)

/* SR2 register flags  */
#define I2C_FLAG_DUALF                                       ((uint32_t)0x00800000)
#define I2C_FLAG_GENCALL                                     ((uint32_t)0x00100000)
#define I2C_FLAG_TRA                                         ((uint32_t)0x00040000)
#define I2C_FLAG_BUSY                                        ((uint32_t)0x00020000)
#define I2C_FLAG_MSL                                         ((uint32_t)0x00010000)

/* SR1 register flags */
#define I2C_FLAG_PECERR                                      ((uint32_t)0x10001000)
#define I2C_FLAG_OVR                                         ((uint32_t)0x10000800)
#define I2C_FLAG_AF                                          ((uint32_t)0x10000400)
#define I2C_FLAG_ARLO                                        ((uint32_t)0x10000200)
#define I2C_FLAG_BERR                                        ((uint32_t)0x10000100)
#define I2C_FLAG_TXE                                         ((uint32_t)0x10000080)
#define I2C_FLAG_RXNE                                        ((uint32_t)0x10000040)
#define I2C_FLAG_STOPF                                       ((uint32_t)0x10000010)
#define I2C_FLAG_ADD10                                       ((uint32_t)0x10000008)
#define I2C_FLAG_BTF                                         ((uint32_t)0x10000004)
#define I2C_FLAG_ADDR                                        ((uint32_t)0x10000002)
#define I2C_FLAG_SB                                          ((uint32_t)0x10000001)

/****************I2C Master Events (Events grouped in order of communication)********************/

/******************************************************************************************************************** 
  * @brief  Start communicate
  * 
  * After master use I2C_GenerateSTART() function sending the START condition,the master 
  * has to wait for event 5(the Start condition has been correctly 
  * released on the I2C bus ).
  * 
  */
/* EVT5 */
#define  I2C_EVENT_MASTER_MODE_SELECT                      ((uint32_t)0x00030001)  /* BUSY, MSL and SB flag */

/********************************************************************************************************************
  * @brief  Address Acknowledge
  * 
  * When start condition correctly released on the bus(check EVT5), the 
  * master use I2C_Send7bitAddress() function sends the address of the slave(s) with which it will communicate 
  * it also determines master as transmitter or Receiver. Then the master has to wait that a slave acknowledges 
  * his address. If an acknowledge is sent on the bus, one of the following events will be set:
  * 
  *
  * 
  *  1) In case of Master Receiver (7-bit addressing): the I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED 
  *     event is set.
  *  
  *  2) In case of Master Transmitter (7-bit addressing): the I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED 
  *     is set
  *  
  *  3) In case of 10-Bit addressing mode, the master (after generating the START 
  *  and checking on EVT5) use I2C_SendData() function send the header of 10-bit addressing mode.  
  *  Then master wait EVT9. EVT9 means that the 10-bit addressing header has been correctly sent 
  *  on the bus. Then master should use the function I2C_Send7bitAddress() to send the second part 
  *  of the 10-bit address (LSB) . Then master should wait for event 6. 
  *
  *     
  */

/* EVT6 */
#define  I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED        ((uint32_t)0x00070082)  /* BUSY, MSL, ADDR, TXE and TRA flags */
#define  I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED           ((uint32_t)0x00030002)  /* BUSY, MSL and ADDR flags */
/*EVT9 */
#define  I2C_EVENT_MASTER_MODE_ADDRESS10                   ((uint32_t)0x00030008)  /* BUSY, MSL and ADD10 flags */

/******************************************************************************************************************** 
  * @brief Communication events
  * 
  * If START condition has generated and slave address 
  * been acknowledged. then the master has to check one of the following events for 
  * communication procedures:
  *  
  * 1) Master Receiver mode: The master has to wait on the event EVT7 then use  
  *   I2C_ReceiveData() function to read the data received from the slave .
  * 
  * 2) Master Transmitter mode: The master use I2C_SendData() function to send data  
  *     then to wait on event EVT8 or EVT8_2.
  *    These two events are similar: 
  *     - EVT8 means that the data has been written in the data register and is 
  *       being shifted out.
  *     - EVT8_2 means that the data has been physically shifted out and output 
  *       on the bus.
  *     In most cases, using EVT8 is sufficient for the application.
  *     Using EVT8_2  will leads to a slower communication  speed but will more reliable .
  *     EVT8_2 is also more suitable than EVT8 for testing on the last data transmission 
  *    
  *     
  *  Note:
  *  In case the  user software does not guarantee that this event EVT7 is managed before 
  *  the current byte end of transfer, then user may check on I2C_EVENT_MASTER_BYTE_RECEIVED 
  *  and I2C_FLAG_BTF flag at the same time .But in this case the communication may be slower.
  *
  * 
  */

/* Master Receive mode */ 
/* EVT7 */
#define  I2C_EVENT_MASTER_BYTE_RECEIVED                    ((uint32_t)0x00030040)  /* BUSY, MSL and RXNE flags */

/* Master Transmitter mode*/
/* EVT8 */
#define I2C_EVENT_MASTER_BYTE_TRANSMITTING                 ((uint32_t)0x00070080) /* TRA, BUSY, MSL, TXE flags */
/* EVT8_2 */
#define  I2C_EVENT_MASTER_BYTE_TRANSMITTED                 ((uint32_t)0x00070084)  /* TRA, BUSY, MSL, TXE and BTF flags */

/******************I2C Slave Events (Events grouped in order of communication)******************/

/******************************************************************************************************************** 
  * @brief  Start Communicate events
  * 
  * Wait on one of these events at the start of the communication. It means that 
  * the I2C peripheral detected a start condition of master device generate on the bus.  
  * If the acknowledge feature is enabled through function I2C_AcknowledgeConfig()),The peripheral generates an ACK condition on the bus. 
  *    
  *
  *
  * a) In normal case (only one address managed by the slave), when the address 
  *   sent by the master matches the own address of the peripheral (configured by 
  *   I2C_OwnAddress1 field) the I2C_EVENT_SLAVE_XXX_ADDRESS_MATCHED event is set 
  *   (where XXX could be TRANSMITTER or RECEIVER).
  *    
  * b) In case the address sent by the master matches the second address of the 
  *   peripheral (configured by the function I2C_OwnAddress2Config() and enabled 
  *   by the function I2C_DualAddressCmd()) the events I2C_EVENT_SLAVE_XXX_SECONDADDRESS_MATCHED 
  *   (where XXX could be TRANSMITTER or RECEIVER) are set.
  *   
  * c) In case the address sent by the master is General Call (address 0x00) and 
  *   if the General Call is enabled for the peripheral (using function I2C_GeneralCallCmd()) 
  *   the following event is set I2C_EVENT_SLAVE_GENERALCALLADDRESS_MATCHED.   
  * 
  */

/* EVT1 */   
/* a) Case of One Single Address managed by the slave */
#define  I2C_EVENT_SLAVE_RECEIVER_ADDRESS_MATCHED          ((uint32_t)0x00020002) /* BUSY and ADDR flags */
#define  I2C_EVENT_SLAVE_TRANSMITTER_ADDRESS_MATCHED       ((uint32_t)0x00060082) /* TRA, BUSY, TXE and ADDR flags */

/* b) Case of Dual address managed by the slave */
#define  I2C_EVENT_SLAVE_RECEIVER_SECONDADDRESS_MATCHED    ((uint32_t)0x00820000)  /* DUALF and BUSY flags */
#define  I2C_EVENT_SLAVE_TRANSMITTER_SECONDADDRESS_MATCHED ((uint32_t)0x00860080)  /* DUALF, TRA, BUSY and TXE flags */

/* c) Case of General Call enabled for the slave */
#define  I2C_EVENT_SLAVE_GENERALCALLADDRESS_MATCHED        ((uint32_t)0x00120000)  /* GENCALL and BUSY flags */

/******************************************************************************************************************** 
  * @brief  Communication events
  * 
  * Wait on one of these events when EVT1 has already been checked : 
  * 
  * - Slave Receiver mode:
  *     - EVT2--The device is expecting to receive a data byte . 
  *     - EVT4--The device is expecting the end of the communication: master 
  *       sends a stop condition and data transmission is stopped.
  *    
  * - Slave Transmitter mode:
  *    - EVT3--When a byte has been transmitted by the slave and the Master is expecting 
  *      the end of the byte transmission. The two events I2C_EVENT_SLAVE_BYTE_TRANSMITTED and
  *      I2C_EVENT_SLAVE_BYTE_TRANSMITTING are similar. If the user software doesn't guarantee 
  *      the EVT3 is managed before the current byte end of transfer The second one can optionally
  *      be used. 
  *    - EVT3_2--When the master sends a NACK  to tell slave device that data transmission 
  *      shall end . The slave device has to stop sending 
  *      data bytes and wait a Stop condition from bus.
  *      
  *  Note:
  *  If the  user software does not guarantee that the event 2 is 
  *  managed before the current byte end of transfer, User may check on I2C_EVENT_SLAVE_BYTE_RECEIVED 
  *  and I2C_FLAG_BTF flag at the same time .
  *  In this case the communication will be slower.
  *
  */

/* Slave Receiver mode*/ 
/* EVT2 */
#define  I2C_EVENT_SLAVE_BYTE_RECEIVED                     ((uint32_t)0x00020040)  /* BUSY and RXNE flags */
/* EVT4  */
#define  I2C_EVENT_SLAVE_STOP_DETECTED                     ((uint32_t)0x00000010)  /* STOPF flag */

/* Slave Transmitter mode -----------------------*/
/* EVT3 */
#define  I2C_EVENT_SLAVE_BYTE_TRANSMITTED                  ((uint32_t)0x00060084)  /* TRA, BUSY, TXE and BTF flags */
#define  I2C_EVENT_SLAVE_BYTE_TRANSMITTING                 ((uint32_t)0x00060080)  /* TRA, BUSY and TXE flags */
/*EVT3_2 */
#define  I2C_EVENT_SLAVE_ACK_FAILURE                       ((uint32_t)0x00000400)  /* AF flag */


/* ch32v00x_iwdg.h -----------------------------------------------------------*/

/* IWDG_WriteAccess */
#define IWDG_WriteAccess_Enable     ((uint16_t)0x5555)
#define IWDG_WriteAccess_Disable    ((uint16_t)0x0000)

/* IWDG_prescaler */
#define IWDG_Prescaler_4            ((uint8_t)0x00)
#define IWDG_Prescaler_8            ((uint8_t)0x01)
#define IWDG_Prescaler_16           ((uint8_t)0x02)
#define IWDG_Prescaler_32           ((uint8_t)0x03)
#define IWDG_Prescaler_64           ((uint8_t)0x04)
#define IWDG_Prescaler_128          ((uint8_t)0x05)
#define IWDG_Prescaler_256          ((uint8_t)0x06)

/* IWDG_Flag */
#define IWDG_FLAG_PVU               ((uint16_t)0x0001)
#define IWDG_FLAG_RVU               ((uint16_t)0x0002)


/* ch32v00x_misc.h -----------------------------------------------------------*/

/* Preemption_Priority_Group */
#define NVIC_PriorityGroup_0           ((uint32_t)0x00)
#define NVIC_PriorityGroup_1           ((uint32_t)0x01)
#define NVIC_PriorityGroup_2           ((uint32_t)0x02)
#define NVIC_PriorityGroup_3           ((uint32_t)0x03)
#define NVIC_PriorityGroup_4           ((uint32_t)0x04)

/* ch32v00x_opa.h ------------------------------------------------------------*/

/* Editor's note: I don't know if this is actually useful */
#ifndef __ASSEMBLER__


/* OPA PSEL enumeration */
typedef enum
{
    CHP0 = 0,
    CHP1
} OPA_PSEL_TypeDef;

/* OPA NSEL enumeration */
typedef enum
{
    CHN0 = 0,
    CHN1
} OPA_NSEL_TypeDef;



/* OPA Init Structure definition */
typedef struct
{
    OPA_PSEL_TypeDef PSEL;    /* Specifies the positive channel of OPA */
    OPA_NSEL_TypeDef NSEL;    /* Specifies the negative channel of OPA */
} OPA_InitTypeDef;


/* memory mapped structure for Program Fast Interrupt Controller (PFIC) */
typedef struct{
    __I  uint32_t ISR[8];
    __I  uint32_t IPR[8];
    __IO uint32_t ITHRESDR;
    __IO uint32_t RESERVED;
    __IO uint32_t CFGR;
    __I  uint32_t GISR;
    __IO uint8_t VTFIDR[4];
    uint8_t RESERVED0[12];
    __IO uint32_t VTFADDR[4];
    uint8_t RESERVED1[0x90];
    __O  uint32_t IENR[8];
    uint8_t RESERVED2[0x60];
    __O  uint32_t IRER[8];
    uint8_t RESERVED3[0x60];
    __O  uint32_t IPSR[8];
    uint8_t RESERVED4[0x60];
    __O  uint32_t IPRR[8];
    uint8_t RESERVED5[0x60];
    __IO uint32_t IACTR[8];
    uint8_t RESERVED6[0xE0];
    __IO uint8_t IPRIOR[256];
    uint8_t RESERVED7[0x810];
    __IO uint32_t SCTLR;
}PFIC_Type;

#endif

/* ch32v00x_pwr.h ------------------------------------------------------------*/


/* PVD_detection_level  */

#define PWR_PVDLevel_2V9          ((uint32_t)0x00000000)
#define PWR_PVDLevel_3V1          ((uint32_t)0x00000020)
#define PWR_PVDLevel_3V3          ((uint32_t)0x00000040)
#define PWR_PVDLevel_3V5          ((uint32_t)0x00000060)
#define PWR_PVDLevel_3V7          ((uint32_t)0x00000080)
#define PWR_PVDLevel_3V9          ((uint32_t)0x000000A0)
#define PWR_PVDLevel_4V1          ((uint32_t)0x000000C0)
#define PWR_PVDLevel_4V4          ((uint32_t)0x000000E0)

/* PWR_AWU_Prescaler */
#define PWR_AWU_Prescaler_1       ((uint32_t)0x00000000)
#define PWR_AWU_Prescaler_2       ((uint32_t)0x00000002)
#define PWR_AWU_Prescaler_4       ((uint32_t)0x00000003)
#define PWR_AWU_Prescaler_8       ((uint32_t)0x00000004)
#define PWR_AWU_Prescaler_16      ((uint32_t)0x00000005)
#define PWR_AWU_Prescaler_32      ((uint32_t)0x00000006)
#define PWR_AWU_Prescaler_64      ((uint32_t)0x00000007)
#define PWR_AWU_Prescaler_128     ((uint32_t)0x00000008)
#define PWR_AWU_Prescaler_256     ((uint32_t)0x00000009)
#define PWR_AWU_Prescaler_512     ((uint32_t)0x0000000A)
#define PWR_AWU_Prescaler_1024    ((uint32_t)0x0000000B)
#define PWR_AWU_Prescaler_2048    ((uint32_t)0x0000000C)
#define PWR_AWU_Prescaler_4096    ((uint32_t)0x0000000D)
#define PWR_AWU_Prescaler_10240   ((uint32_t)0x0000000E)
#define PWR_AWU_Prescaler_61440   ((uint32_t)0x0000000F)

/* STOP_mode_entry */
#define PWR_STANDBYEntry_WFI      ((uint8_t)0x01)
#define PWR_STANDBYEntry_WFE      ((uint8_t)0x02)

/* PWR_Flag */
#define PWR_FLAG_PVDO             ((uint32_t)0x00000004)



/* ch32v00x_rcc.h ------------------------------------------------------------*/


/* HSE_configuration */
#define RCC_HSE_OFF                      ((uint32_t)0x00000000)
#define RCC_HSE_ON                       ((uint32_t)0x00010000)
#define RCC_HSE_Bypass                   ((uint32_t)0x00040000)


/* PLL_entry_clock_source */
#define RCC_PLLSource_HSI_MUL2           ((uint32_t)0x00000000)
#define RCC_PLLSource_HSE_MUL2           ((uint32_t)0x00030000)


/* System_clock_source */
#define RCC_SYSCLKSource_HSI             ((uint32_t)0x00000000)
#define RCC_SYSCLKSource_HSE             ((uint32_t)0x00000001)
#define RCC_SYSCLKSource_PLLCLK          ((uint32_t)0x00000002)


/* AHB_clock_source */
#define RCC_SYSCLK_Div1                  ((uint32_t)0x00000000)
#define RCC_SYSCLK_Div2                  ((uint32_t)0x00000010)
#define RCC_SYSCLK_Div3                  ((uint32_t)0x00000020)
#define RCC_SYSCLK_Div4                  ((uint32_t)0x00000030)
#define RCC_SYSCLK_Div5                  ((uint32_t)0x00000040)
#define RCC_SYSCLK_Div6                  ((uint32_t)0x00000050)
#define RCC_SYSCLK_Div7                  ((uint32_t)0x00000060)
#define RCC_SYSCLK_Div8                  ((uint32_t)0x00000070)
#define RCC_SYSCLK_Div16                 ((uint32_t)0x000000B0)
#define RCC_SYSCLK_Div32                 ((uint32_t)0x000000C0)
#define RCC_SYSCLK_Div64                 ((uint32_t)0x000000D0)
#define RCC_SYSCLK_Div128                ((uint32_t)0x000000E0)
#define RCC_SYSCLK_Div256                ((uint32_t)0x000000F0)


/* RCC_Interrupt_source */
#define RCC_IT_LSIRDY                    ((uint8_t)0x01)
#define RCC_IT_HSIRDY                    ((uint8_t)0x04)
#define RCC_IT_HSERDY                    ((uint8_t)0x08)
#define RCC_IT_PLLRDY                    ((uint8_t)0x10)
#define RCC_IT_CSS                       ((uint8_t)0x80)

#ifdef CH32V30x_D8C
#define RCC_IT_PLL2RDY                   ((uint8_t)0x20)
#define RCC_IT_PLL3RDY                   ((uint8_t)0x40)
#endif





/* ADC_clock_source */
#define RCC_PCLK2_Div2                   ((uint32_t)0x00000000)
#define RCC_PCLK2_Div4                   ((uint32_t)0x00004000)
#define RCC_PCLK2_Div6                   ((uint32_t)0x00008000)
#define RCC_PCLK2_Div8                   ((uint32_t)0x0000C000)
#define RCC_PCLK2_Div12                  ((uint32_t)0x0000A000)
#define RCC_PCLK2_Div16                  ((uint32_t)0x0000E000)
#define RCC_PCLK2_Div24                  ((uint32_t)0x0000A800)
#define RCC_PCLK2_Div32                  ((uint32_t)0x0000E800)
#define RCC_PCLK2_Div48                  ((uint32_t)0x0000B000)
#define RCC_PCLK2_Div64                  ((uint32_t)0x0000F000)
#define RCC_PCLK2_Div96                  ((uint32_t)0x0000B800)
#define RCC_PCLK2_Div128                 ((uint32_t)0x0000F800)



/* AHB_peripheral */
#define RCC_AHBPeriph_DMA1               ((uint32_t)0x00000001)
#define RCC_AHBPeriph_SRAM               ((uint32_t)0x00000004)

/* APB2_peripheral */
#define RCC_APB2Periph_AFIO              ((uint32_t)0x00000001)
#define RCC_APB2Periph_GPIOA             ((uint32_t)0x00000004)
#define RCC_APB2Periph_GPIOB             ((uint32_t)0x00000008)
#define RCC_APB2Periph_GPIOC             ((uint32_t)0x00000010)
#define RCC_APB2Periph_GPIOD             ((uint32_t)0x00000020)
#define RCC_APB2Periph_ADC1              ((uint32_t)0x00000200)
#define RCC_APB2Periph_TIM1              ((uint32_t)0x00000800)
#define RCC_APB2Periph_SPI1              ((uint32_t)0x00001000)
#define RCC_APB2Periph_USART2            ((uint32_t)0x00002000)
#define RCC_APB2Periph_USART1            ((uint32_t)0x00004000)

/* APB1_peripheral */
#define RCC_APB1Periph_TIM2              ((uint32_t)0x00000001)
#define RCC_APB1Periph_TIM3              ((uint32_t)0x00000004)
#define RCC_APB1Periph_WWDG              ((uint32_t)0x00000800)
#define RCC_APB1Periph_I2C1              ((uint32_t)0x00200000)
#define RCC_APB1Periph_PWR               ((uint32_t)0x10000000)



/* Clock_source_to_output_on_MCO_pin */
#define RCC_MCO_NoClock                  ((uint8_t)0x00)
#define RCC_MCO_SYSCLK                   ((uint8_t)0x04)
#define RCC_MCO_HSI                      ((uint8_t)0x05)
#define RCC_MCO_HSE                      ((uint8_t)0x06)
#define RCC_MCO_PLLCLK                   ((uint8_t)0x07)

#ifdef CH32V30x_D8C
#define RCC_MCO_PLL2CLK                  ((uint8_t)0x08)
#define RCC_MCO_PLL3CLK_Div2             ((uint8_t)0x09)
#define RCC_MCO_XT1                      ((uint8_t)0x0A)
#define RCC_MCO_PLL3CLK                  ((uint8_t)0x0B)
#endif

/* RCC_Flag */
#define RCC_FLAG_HSIRDY                  ((uint8_t)0x21)
#define RCC_FLAG_HSERDY                  ((uint8_t)0x31)
#define RCC_FLAG_PLLRDY                  ((uint8_t)0x39)
#define RCC_FLAG_LSIRDY                  ((uint8_t)0x61)
#define RCC_FLAG_PINRST                  ((uint8_t)0x7A)
#define RCC_FLAG_PORRST                  ((uint8_t)0x7B)
#define RCC_FLAG_SFTRST                  ((uint8_t)0x7C)
#define RCC_FLAG_IWDGRST                 ((uint8_t)0x7D)
#define RCC_FLAG_WWDGRST                 ((uint8_t)0x7E)
#define RCC_FLAG_LPWRRST                 ((uint8_t)0x7F)

#ifdef CH32V30x_D8C
#define RCC_FLAG_PLL2RDY                 ((uint8_t)0x3B)
#define RCC_FLAG_PLL3RDY                 ((uint8_t)0x3D)
#endif

/* SysTick_clock_source */
#define SysTick_CLKSource_HCLK_Div8      ((uint32_t)0xFFFFFFFB)
#define SysTick_CLKSource_HCLK           ((uint32_t)0x00000004)

/* RNG_clock_source */
#ifdef CH32V30x_D8C
#define RCC_RNGCLKSource_SYSCLK          ((uint32_t)0x00)
#define RCC_RNGCLKSource_PLL3_VCO        ((uint32_t)0x01)
#endif

/* ETH1G_clock_source */
#ifdef CH32V30x_D8C
#define RCC_ETH1GCLKSource_PLL2_VCO      ((uint32_t)0x00)
#define RCC_ETH1GCLKSource_PLL3_VCO      ((uint32_t)0x01)
#define RCC_ETH1GCLKSource_PB1_IN        ((uint32_t)0x02)
#endif






/* ch32v00x_spi.h ------------------------------------------------------------*/


/* SPI_data_direction */
#define SPI_Direction_2Lines_FullDuplex    ((uint16_t)0x0000)
#define SPI_Direction_2Lines_RxOnly        ((uint16_t)0x0400)
#define SPI_Direction_1Line_Rx             ((uint16_t)0x8000)
#define SPI_Direction_1Line_Tx             ((uint16_t)0xC000)

/* SPI_mode */
#define SPI_Mode_Master                    ((uint16_t)0x0104) /* Sets MSTR, as well as SSI, which is required for Master Mode */
#define SPI_Mode_Slave                     ((uint16_t)0x0000)

/* SPI_data_size */
#define SPI_DataSize_16b                   ((uint16_t)0x0800)
#define SPI_DataSize_8b                    ((uint16_t)0x0000)

/* SPI_Clock_Polarity */
#define SPI_CPOL_Low                       ((uint16_t)0x0000)
#define SPI_CPOL_High                      ((uint16_t)0x0002)

/* SPI_Clock_Phase */
#define SPI_CPHA_1Edge                     ((uint16_t)0x0000)
#define SPI_CPHA_2Edge                     ((uint16_t)0x0001)

/* SPI_Slave_Select_management */
#define SPI_NSS_Soft                       ((uint16_t)0x0200)
#define SPI_NSS_Hard                       ((uint16_t)0x0000)

/* SPI_BaudRate_Prescaler */
#define SPI_BaudRatePrescaler_2            ((uint16_t)0x0000)
#define SPI_BaudRatePrescaler_4            ((uint16_t)0x0008)
#define SPI_BaudRatePrescaler_8            ((uint16_t)0x0010)
#define SPI_BaudRatePrescaler_16           ((uint16_t)0x0018)
#define SPI_BaudRatePrescaler_32           ((uint16_t)0x0020)
#define SPI_BaudRatePrescaler_64           ((uint16_t)0x0028)
#define SPI_BaudRatePrescaler_128          ((uint16_t)0x0030)
#define SPI_BaudRatePrescaler_256          ((uint16_t)0x0038)

/* SPI_MSB transmission */
#define SPI_FirstBit_MSB                   ((uint16_t)0x0000)


/* SPI_I2S_DMA_transfer_requests */
#define SPI_I2S_DMAReq_Tx                  ((uint16_t)0x0002)
#define SPI_I2S_DMAReq_Rx                  ((uint16_t)0x0001)

/* SPI_NSS_internal_software_management */
#define SPI_NSSInternalSoft_Set            ((uint16_t)0x0100)
#define SPI_NSSInternalSoft_Reset          ((uint16_t)0xFEFF)

/* SPI_CRC_Transmit_Receive */
#define SPI_CRC_Tx                         ((uint8_t)0x00)
#define SPI_CRC_Rx                         ((uint8_t)0x01)

/* SPI_direction_transmit_receive */
#define SPI_Direction_Rx                   ((uint16_t)0xBFFF)
#define SPI_Direction_Tx                   ((uint16_t)0x4000)

/* SPI_I2S_interrupts_definition */
#define SPI_I2S_IT_TXE                     ((uint8_t)0x71)
#define SPI_I2S_IT_RXNE                    ((uint8_t)0x60)
#define SPI_I2S_IT_ERR                     ((uint8_t)0x50)
#define SPI_I2S_IT_OVR                     ((uint8_t)0x56)
#define SPI_IT_MODF                        ((uint8_t)0x55)
#define SPI_IT_CRCERR                      ((uint8_t)0x54)
#define I2S_IT_UDR                         ((uint8_t)0x53)

/* SPI_I2S_flags_definition */
#define SPI_I2S_FLAG_RXNE                  ((uint16_t)0x0001)
#define SPI_I2S_FLAG_TXE                   ((uint16_t)0x0002)
#define I2S_FLAG_CHSIDE                    ((uint16_t)0x0004)
#define I2S_FLAG_UDR                       ((uint16_t)0x0008)
#define SPI_FLAG_CRCERR                    ((uint16_t)0x0010)
#define SPI_FLAG_MODF                      ((uint16_t)0x0020)
#define SPI_I2S_FLAG_OVR                   ((uint16_t)0x0040)
#define SPI_I2S_FLAG_BSY                   ((uint16_t)0x0080)


/* ch32v00x_tim.h ------------------------------------------------------------*/

/* TIM_Output_Compare_and_PWM_modes */
#define TIM_OCMode_Timing                  ((uint16_t)0x0000)
#define TIM_OCMode_Active                  ((uint16_t)0x0010)
#define TIM_OCMode_Inactive                ((uint16_t)0x0020)
#define TIM_OCMode_Toggle                  ((uint16_t)0x0030)
#define TIM_OCMode_PWM1                    ((uint16_t)0x0060)
#define TIM_OCMode_PWM2                    ((uint16_t)0x0070)

/* TIM_One_Pulse_Mode */
#define TIM_OPMode_Single                  ((uint16_t)0x0008)
#define TIM_OPMode_Repetitive              ((uint16_t)0x0000)

/* TIM_Channel */
#define TIM_Channel_1                      ((uint16_t)0x0000)
#define TIM_Channel_2                      ((uint16_t)0x0004)
#define TIM_Channel_3                      ((uint16_t)0x0008)
#define TIM_Channel_4                      ((uint16_t)0x000C)

/* TIM_Clock_Division_CKD */
#define TIM_CKD_DIV1                       ((uint16_t)0x0000)
#define TIM_CKD_DIV2                       ((uint16_t)0x0100)
#define TIM_CKD_DIV4                       ((uint16_t)0x0200)

/* TIM_Counter_Mode */
#define TIM_CounterMode_Up                 ((uint16_t)0x0000)
#define TIM_CounterMode_Down               ((uint16_t)0x0010)
#define TIM_CounterMode_CenterAligned1     ((uint16_t)0x0020)
#define TIM_CounterMode_CenterAligned2     ((uint16_t)0x0040)
#define TIM_CounterMode_CenterAligned3     ((uint16_t)0x0060)

/* TIM_Output_Compare_Polarity */
#define TIM_OCPolarity_High                ((uint16_t)0x0000)
#define TIM_OCPolarity_Low                 ((uint16_t)0x0002)

/* TIM_Output_Compare_N_Polarity */
#define TIM_OCNPolarity_High               ((uint16_t)0x0000)
#define TIM_OCNPolarity_Low                ((uint16_t)0x0008)

/* TIM_Output_Compare_state */
#define TIM_OutputState_Disable            ((uint16_t)0x0000)
#define TIM_OutputState_Enable             ((uint16_t)0x0001)

/* TIM_Output_Compare_N_state */
#define TIM_OutputNState_Disable           ((uint16_t)0x0000)
#define TIM_OutputNState_Enable            ((uint16_t)0x0004)

/* TIM_Capture_Compare_state */
#define TIM_CCx_Enable                     ((uint16_t)0x0001)
#define TIM_CCx_Disable                    ((uint16_t)0x0000)

/* TIM_Capture_Compare_N_state */
#define TIM_CCxN_Enable                    ((uint16_t)0x0004)
#define TIM_CCxN_Disable                   ((uint16_t)0x0000)

/* Break_Input_enable_disable */
#define TIM_Break_Enable                   ((uint16_t)0x1000)
#define TIM_Break_Disable                  ((uint16_t)0x0000)

/* Break_Polarity */
#define TIM_BreakPolarity_Low              ((uint16_t)0x0000)
#define TIM_BreakPolarity_High             ((uint16_t)0x2000)

/* TIM_AOE_Bit_Set_Reset */
#define TIM_AutomaticOutput_Enable         ((uint16_t)0x4000)
#define TIM_AutomaticOutput_Disable        ((uint16_t)0x0000)

/* Lock_level */
#define TIM_LOCKLevel_OFF                  ((uint16_t)0x0000)
#define TIM_LOCKLevel_1                    ((uint16_t)0x0100)
#define TIM_LOCKLevel_2                    ((uint16_t)0x0200)
#define TIM_LOCKLevel_3                    ((uint16_t)0x0300)

/* OSSI_Off_State_Selection_for_Idle_mode_state */
#define TIM_OSSIState_Enable               ((uint16_t)0x0400)
#define TIM_OSSIState_Disable              ((uint16_t)0x0000)

/* OSSR_Off_State_Selection_for_Run_mode_state */
#define TIM_OSSRState_Enable               ((uint16_t)0x0800)
#define TIM_OSSRState_Disable              ((uint16_t)0x0000)

/* TIM_Output_Compare_Idle_State */
#define TIM_OCIdleState_Set                ((uint16_t)0x0100)
#define TIM_OCIdleState_Reset              ((uint16_t)0x0000)

/* TIM_Output_Compare_N_Idle_State */
#define TIM_OCNIdleState_Set               ((uint16_t)0x0200)
#define TIM_OCNIdleState_Reset             ((uint16_t)0x0000)

/* TIM_Input_Capture_Polarity */
#define TIM_ICPolarity_Rising              ((uint16_t)0x0000)
#define TIM_ICPolarity_Falling             ((uint16_t)0x0002)
#define TIM_ICPolarity_BothEdge            ((uint16_t)0x000A)

/* TIM_Input_Capture_Selection */
#define TIM_ICSelection_DirectTI           ((uint16_t)0x0001) /* TIM Input 1, 2, 3 or 4 is selected to be \
                                                                 connected to IC1, IC2, IC3 or IC4, respectively */
#define TIM_ICSelection_IndirectTI         ((uint16_t)0x0002) /* TIM Input 1, 2, 3 or 4 is selected to be \
                                                                 connected to IC2, IC1, IC4 or IC3, respectively. */
#define TIM_ICSelection_TRC                ((uint16_t)0x0003) /* TIM Input 1, 2, 3 or 4 is selected to be connected to TRC. */

/* TIM_Input_Capture_Prescaler */
#define TIM_ICPSC_DIV1                     ((uint16_t)0x0000) /* Capture performed each time an edge is detected on the capture input. */
#define TIM_ICPSC_DIV2                     ((uint16_t)0x0004) /* Capture performed once every 2 events. */
#define TIM_ICPSC_DIV4                     ((uint16_t)0x0008) /* Capture performed once every 4 events. */
#define TIM_ICPSC_DIV8                     ((uint16_t)0x000C) /* Capture performed once every 8 events. */

/* TIM_interrupt_sources */
#define TIM_IT_Update                      ((uint16_t)0x0001)
#define TIM_IT_CC1                         ((uint16_t)0x0002)
#define TIM_IT_CC2                         ((uint16_t)0x0004)
#define TIM_IT_CC3                         ((uint16_t)0x0008)
#define TIM_IT_CC4                         ((uint16_t)0x0010)
#define TIM_IT_COM                         ((uint16_t)0x0020)
#define TIM_IT_Trigger                     ((uint16_t)0x0040)
#define TIM_IT_Break                       ((uint16_t)0x0080)

/* TIM_DMA_Base_address */
#define TIM_DMABase_CR1                    ((uint16_t)0x0000)
#define TIM_DMABase_CR2                    ((uint16_t)0x0001)
#define TIM_DMABase_SMCR                   ((uint16_t)0x0002)
#define TIM_DMABase_DIER                   ((uint16_t)0x0003)
#define TIM_DMABase_SR                     ((uint16_t)0x0004)
#define TIM_DMABase_EGR                    ((uint16_t)0x0005)
#define TIM_DMABase_CCMR1                  ((uint16_t)0x0006)
#define TIM_DMABase_CCMR2                  ((uint16_t)0x0007)
#define TIM_DMABase_CCER                   ((uint16_t)0x0008)
#define TIM_DMABase_CNT                    ((uint16_t)0x0009)
#define TIM_DMABase_PSC                    ((uint16_t)0x000A)
#define TIM_DMABase_ARR                    ((uint16_t)0x000B)
#define TIM_DMABase_RCR                    ((uint16_t)0x000C)
#define TIM_DMABase_CCR1                   ((uint16_t)0x000D)
#define TIM_DMABase_CCR2                   ((uint16_t)0x000E)
#define TIM_DMABase_CCR3                   ((uint16_t)0x000F)
#define TIM_DMABase_CCR4                   ((uint16_t)0x0010)
#define TIM_DMABase_BDTR                   ((uint16_t)0x0011)
#define TIM_DMABase_DCR                    ((uint16_t)0x0012)

/* TIM_DMA_Burst_Length */
#define TIM_DMABurstLength_1Transfer       ((uint16_t)0x0000)
#define TIM_DMABurstLength_2Transfers      ((uint16_t)0x0100)
#define TIM_DMABurstLength_3Transfers      ((uint16_t)0x0200)
#define TIM_DMABurstLength_4Transfers      ((uint16_t)0x0300)
#define TIM_DMABurstLength_5Transfers      ((uint16_t)0x0400)
#define TIM_DMABurstLength_6Transfers      ((uint16_t)0x0500)
#define TIM_DMABurstLength_7Transfers      ((uint16_t)0x0600)
#define TIM_DMABurstLength_8Transfers      ((uint16_t)0x0700)
#define TIM_DMABurstLength_9Transfers      ((uint16_t)0x0800)
#define TIM_DMABurstLength_10Transfers     ((uint16_t)0x0900)
#define TIM_DMABurstLength_11Transfers     ((uint16_t)0x0A00)
#define TIM_DMABurstLength_12Transfers     ((uint16_t)0x0B00)
#define TIM_DMABurstLength_13Transfers     ((uint16_t)0x0C00)
#define TIM_DMABurstLength_14Transfers     ((uint16_t)0x0D00)
#define TIM_DMABurstLength_15Transfers     ((uint16_t)0x0E00)
#define TIM_DMABurstLength_16Transfers     ((uint16_t)0x0F00)
#define TIM_DMABurstLength_17Transfers     ((uint16_t)0x1000)
#define TIM_DMABurstLength_18Transfers     ((uint16_t)0x1100)

/* TIM_DMA_sources */
#define TIM_DMA_Update                     ((uint16_t)0x0100)
#define TIM_DMA_CC1                        ((uint16_t)0x0200)
#define TIM_DMA_CC2                        ((uint16_t)0x0400)
#define TIM_DMA_CC3                        ((uint16_t)0x0800)
#define TIM_DMA_CC4                        ((uint16_t)0x1000)
#define TIM_DMA_COM                        ((uint16_t)0x2000)
#define TIM_DMA_Trigger                    ((uint16_t)0x4000)

/* TIM_External_Trigger_Prescaler */
#define TIM_ExtTRGPSC_OFF                  ((uint16_t)0x0000)
#define TIM_ExtTRGPSC_DIV2                 ((uint16_t)0x1000)
#define TIM_ExtTRGPSC_DIV4                 ((uint16_t)0x2000)
#define TIM_ExtTRGPSC_DIV8                 ((uint16_t)0x3000)

/* TIM_Internal_Trigger_Selection */
#define TIM_TS_ITR0                        ((uint16_t)0x0000)
#define TIM_TS_ITR1                        ((uint16_t)0x0010)
#define TIM_TS_ITR2                        ((uint16_t)0x0020)
#define TIM_TS_ITR3                        ((uint16_t)0x0030)
#define TIM_TS_TI1F_ED                     ((uint16_t)0x0040)
#define TIM_TS_TI1FP1                      ((uint16_t)0x0050)
#define TIM_TS_TI2FP2                      ((uint16_t)0x0060)
#define TIM_TS_ETRF                        ((uint16_t)0x0070)

/* TIM_TIx_External_Clock_Source */
#define TIM_TIxExternalCLK1Source_TI1      ((uint16_t)0x0050)
#define TIM_TIxExternalCLK1Source_TI2      ((uint16_t)0x0060)
#define TIM_TIxExternalCLK1Source_TI1ED    ((uint16_t)0x0040)

/* TIM_External_Trigger_Polarity */
#define TIM_ExtTRGPolarity_Inverted        ((uint16_t)0x8000)
#define TIM_ExtTRGPolarity_NonInverted     ((uint16_t)0x0000)

/* TIM_Prescaler_Reload_Mode */
#define TIM_PSCReloadMode_Update           ((uint16_t)0x0000)
#define TIM_PSCReloadMode_Immediate        ((uint16_t)0x0001)

/* TIM_Forced_Action */
#define TIM_ForcedAction_Active            ((uint16_t)0x0050)
#define TIM_ForcedAction_InActive          ((uint16_t)0x0040)

/* TIM_Encoder_Mode */
#define TIM_EncoderMode_TI1                ((uint16_t)0x0001)
#define TIM_EncoderMode_TI2                ((uint16_t)0x0002)
#define TIM_EncoderMode_TI12               ((uint16_t)0x0003)

/* TIM_Event_Source */
#define TIM_EventSource_Update             ((uint16_t)0x0001)
#define TIM_EventSource_CC1                ((uint16_t)0x0002)
#define TIM_EventSource_CC2                ((uint16_t)0x0004)
#define TIM_EventSource_CC3                ((uint16_t)0x0008)
#define TIM_EventSource_CC4                ((uint16_t)0x0010)
#define TIM_EventSource_COM                ((uint16_t)0x0020)
#define TIM_EventSource_Trigger            ((uint16_t)0x0040)
#define TIM_EventSource_Break              ((uint16_t)0x0080)

/* TIM_Update_Source */
#define TIM_UpdateSource_Global            ((uint16_t)0x0000) /* Source of update is the counter overflow/underflow \
                                                                 or the setting of UG bit, or an update generation  \
                                                                 through the slave mode controller. */
#define TIM_UpdateSource_Regular           ((uint16_t)0x0001) /* Source of update is counter overflow/underflow. */

/* TIM_Output_Compare_Preload_State */
#define TIM_OCPreload_Enable               ((uint16_t)0x0008)
#define TIM_OCPreload_Disable              ((uint16_t)0x0000)

/* TIM_Output_Compare_Fast_State */
#define TIM_OCFast_Enable                  ((uint16_t)0x0004)
#define TIM_OCFast_Disable                 ((uint16_t)0x0000)

/* TIM_Output_Compare_Clear_State */
#define TIM_OCClear_Enable                 ((uint16_t)0x0080)
#define TIM_OCClear_Disable                ((uint16_t)0x0000)

/* TIM_Trigger_Output_Source */
#define TIM_TRGOSource_Reset               ((uint16_t)0x0000)
#define TIM_TRGOSource_Enable              ((uint16_t)0x0010)
#define TIM_TRGOSource_Update              ((uint16_t)0x0020)
#define TIM_TRGOSource_OC1                 ((uint16_t)0x0030)
#define TIM_TRGOSource_OC1Ref              ((uint16_t)0x0040)
#define TIM_TRGOSource_OC2Ref              ((uint16_t)0x0050)
#define TIM_TRGOSource_OC3Ref              ((uint16_t)0x0060)
#define TIM_TRGOSource_OC4Ref              ((uint16_t)0x0070)

/* TIM_Slave_Mode */
#define TIM_SlaveMode_Reset                ((uint16_t)0x0004)
#define TIM_SlaveMode_Gated                ((uint16_t)0x0005)
#define TIM_SlaveMode_Trigger              ((uint16_t)0x0006)
#define TIM_SlaveMode_External1            ((uint16_t)0x0007)

/* TIM_Master_Slave_Mode */
#define TIM_MasterSlaveMode_Enable         ((uint16_t)0x0080)
#define TIM_MasterSlaveMode_Disable        ((uint16_t)0x0000)

/* TIM_Flags */
#define TIM_FLAG_Update                    ((uint16_t)0x0001)
#define TIM_FLAG_CC1                       ((uint16_t)0x0002)
#define TIM_FLAG_CC2                       ((uint16_t)0x0004)
#define TIM_FLAG_CC3                       ((uint16_t)0x0008)
#define TIM_FLAG_CC4                       ((uint16_t)0x0010)
#define TIM_FLAG_COM                       ((uint16_t)0x0020)
#define TIM_FLAG_Trigger                   ((uint16_t)0x0040)
#define TIM_FLAG_Break                     ((uint16_t)0x0080)
#define TIM_FLAG_CC1OF                     ((uint16_t)0x0200)
#define TIM_FLAG_CC2OF                     ((uint16_t)0x0400)
#define TIM_FLAG_CC3OF                     ((uint16_t)0x0800)
#define TIM_FLAG_CC4OF                     ((uint16_t)0x1000)

/* TIM_Legacy */
#define TIM_DMABurstLength_1Byte           TIM_DMABurstLength_1Transfer
#define TIM_DMABurstLength_2Bytes          TIM_DMABurstLength_2Transfers
#define TIM_DMABurstLength_3Bytes          TIM_DMABurstLength_3Transfers
#define TIM_DMABurstLength_4Bytes          TIM_DMABurstLength_4Transfers
#define TIM_DMABurstLength_5Bytes          TIM_DMABurstLength_5Transfers
#define TIM_DMABurstLength_6Bytes          TIM_DMABurstLength_6Transfers
#define TIM_DMABurstLength_7Bytes          TIM_DMABurstLength_7Transfers
#define TIM_DMABurstLength_8Bytes          TIM_DMABurstLength_8Transfers
#define TIM_DMABurstLength_9Bytes          TIM_DMABurstLength_9Transfers
#define TIM_DMABurstLength_10Bytes         TIM_DMABurstLength_10Transfers
#define TIM_DMABurstLength_11Bytes         TIM_DMABurstLength_11Transfers
#define TIM_DMABurstLength_12Bytes         TIM_DMABurstLength_12Transfers
#define TIM_DMABurstLength_13Bytes         TIM_DMABurstLength_13Transfers
#define TIM_DMABurstLength_14Bytes         TIM_DMABurstLength_14Transfers
#define TIM_DMABurstLength_15Bytes         TIM_DMABurstLength_15Transfers
#define TIM_DMABurstLength_16Bytes         TIM_DMABurstLength_16Transfers
#define TIM_DMABurstLength_17Bytes         TIM_DMABurstLength_17Transfers
#define TIM_DMABurstLength_18Bytes         TIM_DMABurstLength_18Transfers


/* ch32v00x_usart.h ----------------------------------------------------------*/

/* USART_Word_Length */
#define USART_WordLength_8b                  ((uint16_t)0x0000)
#define USART_WordLength_9b                  ((uint16_t)0x1000)

/* USART_Stop_Bits */
#define USART_StopBits_1                     ((uint16_t)0x0000)
#define USART_StopBits_0_5                   ((uint16_t)0x1000)
#define USART_StopBits_2                     ((uint16_t)0x2000)
#define USART_StopBits_1_5                   ((uint16_t)0x3000)

/* USART_Parity */
#define USART_Parity_No                      ((uint16_t)0x0000)
#define USART_Parity_Even                    ((uint16_t)0x0400)
#define USART_Parity_Odd                     ((uint16_t)0x0600)

/* USART_Mode */
#define USART_Mode_Rx                        ((uint16_t)0x0004)
#define USART_Mode_Tx                        ((uint16_t)0x0008)

/* USART_Hardware_Flow_Control */
#define USART_HardwareFlowControl_None       ((uint16_t)0x0000)
#define USART_HardwareFlowControl_RTS        ((uint16_t)0x0100)
#define USART_HardwareFlowControl_CTS        ((uint16_t)0x0200)
#define USART_HardwareFlowControl_RTS_CTS    ((uint16_t)0x0300)

/* USART_Clock */
#define USART_Clock_Disable                  ((uint16_t)0x0000)
#define USART_Clock_Enable                   ((uint16_t)0x0800)

/* USART_Clock_Polarity */
#define USART_CPOL_Low                       ((uint16_t)0x0000)
#define USART_CPOL_High                      ((uint16_t)0x0400)

/* USART_Clock_Phase */
#define USART_CPHA_1Edge                     ((uint16_t)0x0000)
#define USART_CPHA_2Edge                     ((uint16_t)0x0200)

/* USART_Last_Bit */
#define USART_LastBit_Disable                ((uint16_t)0x0000)
#define USART_LastBit_Enable                 ((uint16_t)0x0100)

/* USART_Interrupt_definition */
#define USART_IT_PE                          ((uint16_t)0x0028)
#define USART_IT_TXE                         ((uint16_t)0x0727)
#define USART_IT_TC                          ((uint16_t)0x0626)
#define USART_IT_RXNE                        ((uint16_t)0x0525)
#define USART_IT_ORE_RX                      ((uint16_t)0x0325)
#define USART_IT_IDLE                        ((uint16_t)0x0424)
#define USART_IT_LBD                         ((uint16_t)0x0846)
#define USART_IT_CTS                         ((uint16_t)0x096A)
#define USART_IT_ERR                         ((uint16_t)0x0060)
#define USART_IT_ORE_ER                      ((uint16_t)0x0360)
#define USART_IT_NE                          ((uint16_t)0x0260)
#define USART_IT_FE                          ((uint16_t)0x0160)

#define USART_IT_ORE                         USART_IT_ORE_ER

/* USART_DMA_Requests */
#define USART_DMAReq_Tx                      ((uint16_t)0x0080)
#define USART_DMAReq_Rx                      ((uint16_t)0x0040)

/* USART_WakeUp_methods */
#define USART_WakeUp_IdleLine                ((uint16_t)0x0000)
#define USART_WakeUp_AddressMark             ((uint16_t)0x0800)

/* USART_LIN_Break_Detection_Length */
#define USART_LINBreakDetectLength_10b       ((uint16_t)0x0000)
#define USART_LINBreakDetectLength_11b       ((uint16_t)0x0020)

/* USART_IrDA_Low_Power */
#define USART_IrDAMode_LowPower              ((uint16_t)0x0004)
#define USART_IrDAMode_Normal                ((uint16_t)0x0000)

/* USART_Flags */
#define USART_FLAG_CTS                       ((uint16_t)0x0200)
#define USART_FLAG_LBD                       ((uint16_t)0x0100)
#define USART_FLAG_TXE                       ((uint16_t)0x0080)
#define USART_FLAG_TC                        ((uint16_t)0x0040)
#define USART_FLAG_RXNE                      ((uint16_t)0x0020)
#define USART_FLAG_IDLE                      ((uint16_t)0x0010)
#define USART_FLAG_ORE                       ((uint16_t)0x0008)
#define USART_FLAG_NE                        ((uint16_t)0x0004)
#define USART_FLAG_FE                        ((uint16_t)0x0002)
#define USART_FLAG_PE                        ((uint16_t)0x0001)

// While not truly CH32X035, we can re-use some of the USB register defs.

/* ch32v30x_usb.h ------------------------------------------------------------*/


/* ch32v00x_wwdg.h -----------------------------------------------------------*/


/* WWDG_Prescaler */
#define WWDG_Prescaler_1    ((uint32_t)0x00000000)
#define WWDG_Prescaler_2    ((uint32_t)0x00000080)
#define WWDG_Prescaler_4    ((uint32_t)0x00000100)
#define WWDG_Prescaler_8    ((uint32_t)0x00000180)

#ifdef __cplusplus
};
#endif


// For debug writing to the debug interface.
#ifndef MINICHLINK
	#define DMDATA0 ((volatile uint32_t*)0xe00000f4)
	#define DMDATA1 ((volatile uint32_t*)0xe00000f8)
	#define DMSTATUS_SENTINEL ((volatile uint32_t*)0xe00000fc) // Reads as 0x00000000 if debugger is attached.
#endif

// Determination of PLL multiplication factor for non-V003 chips



// xw_ext.inc, thanks to @macyler, @jnk0le, @duk for this reverse engineering.

/*
Encoder for some of the proprietary 'XW' RISC-V instructions present on the QingKe RV32 processor.
Examples:
	XW_C_LBU(a3, a1, 27); // c.xw.lbu a3, 27(a1)
	XW_C_SB(a0, s0, 13);  // c.xw.sb a0, 13(s0)

	XW_C_LHU(a5, a5, 38); // c.xw.lhu a5, 38(a5)
	XW_C_SH(a2, s1, 14);  // c.xw.sh a2, 14(s1)
*/

// Let us do some compile-time error checking.
#define ASM_ASSERT(COND) .if (!(COND)); .err; .endif

// Integer encodings of the possible compressed registers.
#define C_s0 0
#define C_s1 1
#define C_a0 2
#define C_a1 3
#define C_a2 4
#define C_a3 5
#define C_a4 6
#define C_a5 7

// register to encoding
#define REG2I(X) (C_ ## X)

// XW opcodes
#define XW_OP_LBUSP 0b1000000000000000
#define XW_OP_STSP  0b1000000001000000

#define XW_OP_LHUSP 0b1000000000100000
#define XW_OP_SHSP  0b1000000001100000

#define XW_OP_LBU   0b0010000000000000
#define XW_OP_SB    0b1010000000000000

#define XW_OP_LHU   0b0010000000000010
#define XW_OP_SH    0b1010000000000010

// The two different XW encodings supported at the moment.
#define XW_ENCODE1(OP, R1, R2, IMM) ASM_ASSERT((IMM) >= 0 && (IMM) < 32); .2byte ((OP) | (REG2I(R1) << 2) | (REG2I(R2) << 7) | \
	(((IMM) & 0b1) << 12) | (((IMM) & 0b110) << (5 - 1)) | (((IMM) & 0b11000) << (10 - 3)))

#define XW_ENCODE2(OP, R1, R2, IMM) ASM_ASSERT((IMM) >= 0 && (IMM) < 32); .2byte ((OP) | (REG2I(R1) << 2) | (REG2I(R2) << 7) | \
	(((IMM) & 0b11) << 5) | (((IMM) & 0b11100) << (10 - 2))

// Compressed load byte, zero-extend result
#define XW_C_LBU(RD, RS, IMM) XW_ENCODE1(XW_OP_LBU, RD, RS, IMM)

// Compressed store byte
#define XW_C_SB(RS1, RS2, IMM) XW_ENCODE1(XW_OP_SB, RS1, RS2, IMM)

// Compressed load half, zero-extend result
#define XW_C_LHU(RD, RS, IMM) ASM_ASSERT(((IMM) & 1) == 0); XW_ENCODE2(XW_OP_LHU, RD, RS, ((IMM) >> 1)))

// Compressed store half
#define XW_C_SH(RS1, RS2, IMM)  ASM_ASSERT(((IMM) & 1) == 0); XW_ENCODE2(XW_OP_SH, RS1, RS2, ((IMM) >> 1)))



// Applies to all processors


/* some bit definitions for systick regs */
#define SYSTICK_SR_CNTIF (1<<0)
#define SYSTICK_CTLR_STE (1<<0)
#define SYSTICK_CTLR_STIE (1<<1)
#define SYSTICK_CTLR_STCLK (1<<2)
#define SYSTICK_CTLR_STRE (1<<3)
#define SYSTICK_CTLR_SWIE (1<<31)

#define PFIC            ((PFIC_Type *) PFIC_BASE )
#define NVIC            PFIC
#define NVIC_KEY1       ((uint32_t)0xFA050000)
#define	NVIC_KEY2	    ((uint32_t)0xBCAF0000)
#define	NVIC_KEY3		((uint32_t)0xBEEF0000)


#define SysTick         ((SysTick_Type *) SysTick_BASE)


#define PA0 0
#define PA1 1
#define PA2 2
#define PA3 3
#define PA4 4
#define PA5 5
#define PA6 6
#define PA7 7
#define PB0 16
#define PB1 17
#define PB2 18
#define PB3 19
#define PB4 20
#define PB5 21
#define PB6 22
// There is not PB7 in V006
#define PC0 32
#define PC1 33
#define PC2 34
#define PC3 35
#define PC4 36
#define PC5 37
#define PC6 38
#define PC7 39
#define PD0 48
#define PD1 49
#define PD2 50
#define PD3 51
#define PD4 52
#define PD5 53
#define PD6 54
#define PD7 55

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

#endif // Header guard

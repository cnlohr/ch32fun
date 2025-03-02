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
    SysTicK_IRQn = 12,       /* 12 System timer Interrupt                            */
    Software_IRQn = 14,      /* 14 software Interrupt                                */

	/******  RISC-V specific Interrupt Numbers *********************************************************/
	WWDG_IRQn = 16,            /* Window WatchDog Interrupt                            */
	PVD_IRQn = 17,             /* PVD through EXTI Line detection Interrupt            */
	TAMPER_IRQn = 18,          /* Tamper Interrupt                                     */
	RTC_IRQn = 19,             /* RTC global Interrupt                                 */
	FLASH_IRQn = 20,           /* FLASH global Interrupt                               */
	RCC_IRQn = 21,             /* RCC global Interrupt                                 */
	EXTI0_IRQn = 22,           /* EXTI Line0 Interrupt                                 */
	EXTI1_IRQn = 23,           /* EXTI Line1 Interrupt                                 */
	EXTI2_IRQn = 24,           /* EXTI Line2 Interrupt                                 */
	EXTI3_IRQn = 25,           /* EXTI Line3 Interrupt                                 */
	EXTI4_IRQn = 26,           /* EXTI Line4 Interrupt                                 */
	DMA1_Channel1_IRQn = 27,   /* DMA1 Channel 1 global Interrupt                      */
	DMA1_Channel2_IRQn = 28,   /* DMA1 Channel 2 global Interrupt                      */
	DMA1_Channel3_IRQn = 29,   /* DMA1 Channel 3 global Interrupt                      */
	DMA1_Channel4_IRQn = 30,   /* DMA1 Channel 4 global Interrupt                      */
	DMA1_Channel5_IRQn = 31,   /* DMA1 Channel 5 global Interrupt                      */
	DMA1_Channel6_IRQn = 32,   /* DMA1 Channel 6 global Interrupt                      */
	DMA1_Channel7_IRQn = 33,   /* DMA1 Channel 7 global Interrupt                      */
	ADC_IRQn = 34,             /* ADC1 and ADC2 global Interrupt                       */
	EXTI9_5_IRQn = 39,         /* External Line[9:5] Interrupts                        */
	TIM1_BRK_IRQn = 40,        /* TIM1 Break Interrupt                                 */
	TIM1_UP_IRQn = 41,         /* TIM1 Update Interrupt                                */
	TIM1_TRG_COM_IRQn = 42,    /* TIM1 Trigger and Commutation Interrupt               */
	TIM1_CC_IRQn = 43,         /* TIM1 Capture Compare Interrupt                       */
	TIM2_IRQn = 44,            /* TIM2 global Interrupt                                */
	TIM3_IRQn = 45,            /* TIM3 global Interrupt                                */
	TIM4_IRQn = 46,            /* TIM4 global Interrupt                                */
	I2C1_EV_IRQn = 47,         /* I2C1 Event Interrupt                                 */
	I2C1_ER_IRQn = 48,         /* I2C1 Error Interrupt                                 */
	I2C2_EV_IRQn = 49,         /* I2C2 Event Interrupt                                 */
	I2C2_ER_IRQn = 50,         /* I2C2 Error Interrupt                                 */
	SPI1_IRQn = 51,            /* SPI1 global Interrupt                                */
	SPI2_IRQn = 52,            /* SPI2 global Interrupt                                */
	USART1_IRQn = 53,          /* USART1 global Interrupt                              */
	USART2_IRQn = 54,          /* USART2 global Interrupt                              */
	USART3_IRQn = 55,          /* USART3 global Interrupt                              */
	EXTI15_10_IRQn = 56,       /* External Line[15:10] Interrupts                      */
	RTCAlarm_IRQn = 57,        /* RTC Alarm through EXTI Line Interrupt                */
	USBWakeUp_IRQn = 58,       /* USB Device WakeUp from suspend through EXTI Line Interrupt 	*/
	USBHD_IRQn = 59,           /* USBHD global Interrupt                               */
} IRQn_Type;

#define DEFAULT_INTERRUPT_VECTOR_CONTENTS "\n\
	.balign  2\n\
	.option   push;\n\
	.option   norvc;\n\
	j handle_reset\n\
	.word   0 \n\
	.word   NMI_Handler                /* NMI */ \n\
	.word   HardFault_Handler          /* Hard Fault */ \n\
	.word   0 \n\
	.word   0 \n\
	.word   0 \n\
	.word   0 \n\
	.word   0 \n\
	.word   0 \n\
	.word   0 \n\
	.word   0 \n\
	.word   SysTick_Handler            /* SysTick Handler */ \n \
	.word   0 \n \
	.word   SW_Handler                 /* SW Handler */ \n \
	.word   0 \n \
	.word   WWDG_IRQHandler            /* Window Watchdog */ \n \
	.word   PVD_IRQHandler             /* PVD through EXTI Line detect */ \n \
	.word   TAMPER_IRQHandler          /* TAMPER */ \n \
	.word   RTC_IRQHandler             /* RTC */ \n \
	.word   FLASH_IRQHandler           /* Flash */ \n \
	.word   RCC_IRQHandler             /* RCC */ \n \
	.word   EXTI0_IRQHandler           /* EXTI Line 0 */ \n \
	.word   EXTI1_IRQHandler           /* EXTI Line 1 */ \n \
	.word   EXTI2_IRQHandler           /* EXTI Line 2 */ \n \
	.word   EXTI3_IRQHandler           /* EXTI Line 3 */ \n \
	.word   EXTI4_IRQHandler           /* EXTI Line 4 */ \n \
	.word   DMA1_Channel1_IRQHandler   /* DMA1 Channel 1 */ \n \
	.word   DMA1_Channel2_IRQHandler   /* DMA1 Channel 2 */ \n \
	.word   DMA1_Channel3_IRQHandler   /* DMA1 Channel 3 */ \n \
	.word   DMA1_Channel4_IRQHandler   /* DMA1 Channel 4 */ \n \
	.word   DMA1_Channel5_IRQHandler   /* DMA1 Channel 5 */ \n \
	.word   DMA1_Channel6_IRQHandler   /* DMA1 Channel 6 */ \n \
	.word   DMA1_Channel7_IRQHandler   /* DMA1 Channel 7 */ \n \
	.word   ADC1_2_IRQHandler          /* ADC1_2 */ \n \
	.word   0 \n \
	.word   0 \n \
	.word   0 \n \
	.word   0 \n \
	.word   EXTI9_5_IRQHandler         /* EXTI Line 9..5 */ \n \
	.word   TIM1_BRK_IRQHandler        /* TIM1 Break */ \n \
	.word   TIM1_UP_IRQHandler         /* TIM1 Update */ \n \
	.word   TIM1_TRG_COM_IRQHandler    /* TIM1 Trigger and Commutation */ \n \
	.word   TIM1_CC_IRQHandler         /* TIM1 Capture Compare */ \n \
	.word   TIM2_IRQHandler            /* TIM2 */ \n \
	.word   TIM3_IRQHandler            /* TIM3 */ \n \
	.word   TIM4_IRQHandler            /* TIM4 */ \n \
	.word   I2C1_EV_IRQHandler         /* I2C1 Event */ \n \
	.word   I2C1_ER_IRQHandler         /* I2C1 Error */ \n \
	.word   I2C2_EV_IRQHandler         /* I2C2 Event */ \n \
	.word   I2C2_ER_IRQHandler         /* I2C2 Error */ \n \
	.word   SPI1_IRQHandler            /* SPI1 */ \n \
	.word   SPI2_IRQHandler            /* SPI2 */ \n \
	.word   USART1_IRQHandler          /* USART1 */ \n \
	.word   USART2_IRQHandler          /* USART2 */ \n \
	.word   USART3_IRQHandler          /* USART3 */ \n \
	.word   EXTI15_10_IRQHandler       /* EXTI Line 15..10 */ \n \
	.word   RTCAlarm_IRQHandler        /* RTC Alarm through EXTI Line */ \n \
	.word   USBFSWakeUp_IRQHandler     /* USB Wakeup from suspend */ \n \
	.word   USBFS_IRQHandler           /* USBFS */ \n \
	.option   pop;\n"

/* memory mapped structure for SysTick */
typedef struct
{
  __IO uint32_t CTLR;
  __IO uint32_t CNTL;
  __IO uint32_t CNTH;
  __IO uint32_t CMPL;
  __IO uint32_t CMPH;
} SysTick_Type;


#endif /* __ASSEMBLER__*/

#define HardFault_IRQn    EXC_IRQn

	#define ADC1_2_IRQn       ADC_IRQn

/* Standard Peripheral Library old definitions (maintained for legacy purpose) */
#define HSI_Value             HSI_VALUE
#define HSE_Value             HSE_VALUE
#define HSEStartUp_TimeOut    HSE_STARTUP_TIMEOUT

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
} ADC_TypeDef;

/* Backup Registers */
typedef struct
{
	uint32_t      RESERVED0;
	__IO uint16_t DATAR1;
	uint16_t      RESERVED1;
	__IO uint16_t DATAR2;
	uint16_t      RESERVED2;
	__IO uint16_t DATAR3;
	uint16_t      RESERVED3;
	__IO uint16_t DATAR4;
	uint16_t      RESERVED4;
	__IO uint16_t DATAR5;
	uint16_t      RESERVED5;
	__IO uint16_t DATAR6;
	uint16_t      RESERVED6;
	__IO uint16_t DATAR7;
	uint16_t      RESERVED7;
	__IO uint16_t DATAR8;
	uint16_t      RESERVED8;
	__IO uint16_t DATAR9;
	uint16_t      RESERVED9;
	__IO uint16_t DATAR10;
	uint16_t      RESERVED10;
	__IO uint16_t OCTLR;
	uint16_t      RESERVED11;
	__IO uint16_t TPCTLR;
	uint16_t      RESERVED12;
	__IO uint16_t TPCSR;
	uint16_t      RESERVED13[5];
	__IO uint16_t DATAR11;
	uint16_t      RESERVED14;
	__IO uint16_t DATAR12;
	uint16_t      RESERVED15;
	__IO uint16_t DATAR13;
	uint16_t      RESERVED16;
	__IO uint16_t DATAR14;
	uint16_t      RESERVED17;
	__IO uint16_t DATAR15;
	uint16_t      RESERVED18;
	__IO uint16_t DATAR16;
	uint16_t      RESERVED19;
	__IO uint16_t DATAR17;
	uint16_t      RESERVED20;
	__IO uint16_t DATAR18;
	uint16_t      RESERVED21;
	__IO uint16_t DATAR19;
	uint16_t      RESERVED22;
	__IO uint16_t DATAR20;
	uint16_t      RESERVED23;
	__IO uint16_t DATAR21;
	uint16_t      RESERVED24;
	__IO uint16_t DATAR22;
	uint16_t      RESERVED25;
	__IO uint16_t DATAR23;
	uint16_t      RESERVED26;
	__IO uint16_t DATAR24;
	uint16_t      RESERVED27;
	__IO uint16_t DATAR25;
	uint16_t      RESERVED28;
	__IO uint16_t DATAR26;
	uint16_t      RESERVED29;
	__IO uint16_t DATAR27;
	uint16_t      RESERVED30;
	__IO uint16_t DATAR28;
	uint16_t      RESERVED31;
	__IO uint16_t DATAR29;
	uint16_t      RESERVED32;
	__IO uint16_t DATAR30;
	uint16_t      RESERVED33;
	__IO uint16_t DATAR31;
	uint16_t      RESERVED34;
	__IO uint16_t DATAR32;
	uint16_t      RESERVED35;
	__IO uint16_t DATAR33;
	uint16_t      RESERVED36;
	__IO uint16_t DATAR34;
	uint16_t      RESERVED37;
	__IO uint16_t DATAR35;
	uint16_t      RESERVED38;
	__IO uint16_t DATAR36;
	uint16_t      RESERVED39;
	__IO uint16_t DATAR37;
	uint16_t      RESERVED40;
	__IO uint16_t DATAR38;
	uint16_t      RESERVED41;
	__IO uint16_t DATAR39;
	uint16_t      RESERVED42;
	__IO uint16_t DATAR40;
	uint16_t      RESERVED43;
	__IO uint16_t DATAR41;
	uint16_t      RESERVED44;
	__IO uint16_t DATAR42;
	uint16_t      RESERVED45;
} BKP_TypeDef;


/* CRC Calculation Unit */
typedef struct
{
	__IO uint32_t DATAR;
	__IO uint8_t  IDATAR;
	uint8_t       RESERVED0;
	uint16_t      RESERVED1;
	__IO uint32_t CTLR;
} CRC_TypeDef;

/* Digital to Analog Converter */
typedef struct
{
  __IO uint32_t CTLR;
  __IO uint32_t SWTR;
  __IO uint32_t R12BDHR1;
  __IO uint32_t L12BDHR1;
  __IO uint32_t R8BDHR1;
  __IO uint32_t R12BDHR2;
  __IO uint32_t L12BDHR2;
  __IO uint32_t R8BDHR2;
  __IO uint32_t RD12BDHR;
  __IO uint32_t LD12BDHR;
  __IO uint32_t RD8BDHR;
  __IO uint32_t DOR1;
  __IO uint32_t DOR2;
} DAC_TypeDef;

/* Debug MCU */
typedef struct
{
    __IO uint32_t CFGR0;
    __IO uint32_t CFGR1;
} DBGMCU_TypeDef;

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
	__IO uint16_t CAP;
	__IO uint16_t RES1;
	__IO uint32_t RES2;
	__IO uint32_t UID0;
	__IO uint32_t UID1;
	__IO uint32_t UID2;
	__IO uint32_t RES3;
} ESG_TypeDef;

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
	GPIO_CFGLR_IN_ANALOG = 0,
	GPIO_CFGLR_IN_FLOAT = 4,
	GPIO_CFGLR_IN_PUPD = 8,
	GPIO_CFGLR_OUT_10Mhz_PP = 1,
	GPIO_CFGLR_OUT_2Mhz_PP = 2,
	GPIO_CFGLR_OUT_50Mhz_PP = 3,
	GPIO_CFGLR_OUT_10Mhz_OD = 5,
	GPIO_CFGLR_OUT_2Mhz_OD = 6,
	GPIO_CFGLR_OUT_50Mhz_OD = 7,
	GPIO_CFGLR_OUT_10Mhz_AF_PP = 9,
	GPIO_CFGLR_OUT_2Mhz_AF_PP = 10,
	GPIO_CFGLR_OUT_50Mhz_AF_PP = 11,
	GPIO_CFGLR_OUT_10Mhz_AF_OD = 13,
	GPIO_CFGLR_OUT_2Mhz_AF_OD = 14,
	GPIO_CFGLR_OUT_50Mhz_AF_OD = 15,
} GPIO_CFGLR_PIN_MODE_Typedef;

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
	__IO uint32_t ECR;
	__IO uint32_t PCFR1;
	__IO uint32_t EXTICR[4];
	uint32_t      RESERVED0;
	__IO uint32_t PCFR2;
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
	__IO uint16_t RTR;
	uint16_t      RESERVED8;
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
} PWR_TypeDef;

/* Reset and Clock Control */
typedef struct
{
    __IO uint32_t CTLR;
    __IO uint32_t CFGR0;
    __IO uint32_t INTR;
    __IO uint32_t APB2PRSTR;
    __IO uint32_t APB1PRSTR;
    __IO uint32_t AHBPCENR;
    __IO uint32_t APB2PCENR;
    __IO uint32_t APB1PCENR;
	__IO uint32_t BDCTLR;
	__IO uint32_t RSTSCKR;
} RCC_TypeDef;

/* Real-Time Clock */
typedef struct
{
	__IO uint16_t CTLRH;
	uint16_t      RESERVED0;
	__IO uint16_t CTLRL;
	uint16_t      RESERVED1;
	__IO uint16_t PSCRH;
	uint16_t      RESERVED2;
	__IO uint16_t PSCRL;
	uint16_t      RESERVED3;
	__IO uint16_t DIVH;
	uint16_t      RESERVED4;
	__IO uint16_t DIVL;
	uint16_t      RESERVED5;
	__IO uint16_t CNTH;
	uint16_t      RESERVED6;
	__IO uint16_t CNTL;
	uint16_t      RESERVED7;
	__IO uint16_t ALRMH;
	uint16_t      RESERVED8;
	__IO uint16_t ALRML;
	uint16_t      RESERVED9;
} RTC_TypeDef;


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

/* TIM */
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
	__IO uint16_t CH1CVR;
	uint16_t      RESERVED13;
	__IO uint16_t CH2CVR;
	uint16_t      RESERVED14;
	__IO uint16_t CH3CVR;
	uint16_t      RESERVED15;
	__IO uint16_t CH4CVR;
	uint16_t      RESERVED16;
	__IO uint16_t BDTR;
	uint16_t      RESERVED17;
	__IO uint16_t DMACFGR;
	uint16_t      RESERVED18;
	__IO uint16_t DMAADR;
	uint16_t      RESERVED19;
} TIM_TypeDef;

/* Universal Synchronous Asynchronous Receiver Transmitter */
typedef struct
{
    __IO uint16_t STATR;
    uint16_t      RESERVED0;
    __IO uint16_t DATAR;
    uint16_t      RESERVED1;
    __IO uint16_t BRR;
    uint16_t      RESERVED2;
    __IO uint16_t CTLR1;
    uint16_t      RESERVED3;
    __IO uint16_t CTLR2;
    uint16_t      RESERVED4;
    __IO uint16_t CTLR3;
    uint16_t      RESERVED5;
    __IO uint16_t GPR;
    uint16_t      RESERVED6;
} USART_TypeDef;

/* Window WatchDog */
typedef struct
{
    __IO uint32_t CTLR;
    __IO uint32_t CFGR;
    __IO uint32_t STATR;
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
#define TIM3_BASE                               (APB1PERIPH_BASE + 0x0400)
#define TIM4_BASE                               (APB1PERIPH_BASE + 0x0800)
#define TIM5_BASE                               (APB1PERIPH_BASE + 0x0C00)
#define TIM6_BASE                               (APB1PERIPH_BASE + 0x1000)
#define TIM7_BASE                               (APB1PERIPH_BASE + 0x1400)
#define TIM12_BASE                              (APB1PERIPH_BASE + 0x1800)
#define TIM13_BASE                              (APB1PERIPH_BASE + 0x1C00)
#define TIM14_BASE                              (APB1PERIPH_BASE + 0x2000)
#define RTC_BASE                                (APB1PERIPH_BASE + 0x2800)
#define WWDG_BASE                               (APB1PERIPH_BASE + 0x2C00)
#define IWDG_BASE                               (APB1PERIPH_BASE + 0x3000)
#define SPI2_BASE                               (APB1PERIPH_BASE + 0x3800)
#define SPI3_BASE             					(APB1PERIPH_BASE + 0x3C00)
#define USART2_BASE                             (APB1PERIPH_BASE + 0x4400)
#define USART3_BASE                             (APB1PERIPH_BASE + 0x4800)
#define UART4_BASE                              (APB1PERIPH_BASE + 0x4C00)
#define UART5_BASE            					(APB1PERIPH_BASE + 0x5000)
#define I2C1_BASE                               (APB1PERIPH_BASE + 0x5400)
#define I2C2_BASE                               (APB1PERIPH_BASE + 0x5800)
#define BKP_BASE                                (APB1PERIPH_BASE + 0x6C00)
#define PWR_BASE                                (APB1PERIPH_BASE + 0x7000)
#define DAC_BASE              					(APB1PERIPH_BASE + 0x7400)

#define AFIO_BASE                               (APB2PERIPH_BASE + 0x0000)
#define EXTI_BASE                               (APB2PERIPH_BASE + 0x0400)
#define GPIOA_BASE                              (APB2PERIPH_BASE + 0x0800)
#define GPIOB_BASE                              (APB2PERIPH_BASE + 0x0C00)
#define GPIOC_BASE                              (APB2PERIPH_BASE + 0x1000)
#define GPIOD_BASE                              (APB2PERIPH_BASE + 0x1400)
#define GPIOE_BASE                              (APB2PERIPH_BASE + 0x1800)
#define GPIOF_BASE                              (APB2PERIPH_BASE + 0x1C00)
#define GPIOG_BASE                              (APB2PERIPH_BASE + 0x2000)
#define ADC1_BASE                               (APB2PERIPH_BASE + 0x2400)
#define ADC2_BASE                               (APB2PERIPH_BASE + 0x2800)
#define TIM1_BASE                               (APB2PERIPH_BASE + 0x2C00)
#define SPI1_BASE                               (APB2PERIPH_BASE + 0x3000)
#define TIM8_BASE             					(APB2PERIPH_BASE + 0x3400)
#define USART1_BASE                             (APB2PERIPH_BASE + 0x3800)
#define ADC3_BASE             					(APB2PERIPH_BASE + 0x3C00)
#define TIM15_BASE            					(APB2PERIPH_BASE + 0x4000)
#define TIM16_BASE            					(APB2PERIPH_BASE + 0x4400)
#define TIM17_BASE            					(APB2PERIPH_BASE + 0x4800)
#define TIM9_BASE             					(APB2PERIPH_BASE + 0x4C00)
#define TIM10_BASE            					(APB2PERIPH_BASE + 0x5000)
#define TIM11_BASE           					(APB2PERIPH_BASE + 0x5400)

#define DMA1_BASE                               (AHBPERIPH_BASE + 0x0000)
#define DMA1_Channel1_BASE                      (AHBPERIPH_BASE + 0x0008)
#define DMA1_Channel2_BASE                      (AHBPERIPH_BASE + 0x001C)
#define DMA1_Channel3_BASE                      (AHBPERIPH_BASE + 0x0030)
#define DMA1_Channel4_BASE                      (AHBPERIPH_BASE + 0x0044)
#define DMA1_Channel5_BASE                      (AHBPERIPH_BASE + 0x0058)
#define DMA1_Channel6_BASE                      (AHBPERIPH_BASE + 0x006C)
#define DMA1_Channel7_BASE                      (AHBPERIPH_BASE + 0x0080)
#define DMA2_BASE             					(AHBPERIPH_BASE + 0x0400)
#define DMA2_Channel1_BASE    					(AHBPERIPH_BASE + 0x0408)
#define DMA2_Channel2_BASE    					(AHBPERIPH_BASE + 0x041C)
#define DMA2_Channel3_BASE    					(AHBPERIPH_BASE + 0x0430)
#define DMA2_Channel4_BASE    					(AHBPERIPH_BASE + 0x0444)
#define DMA2_Channel5_BASE    					(AHBPERIPH_BASE + 0x0458)
#define RCC_BASE                                (AHBPERIPH_BASE + 0x1000)

#define FLASH_R_BASE                            (AHBPERIPH_BASE + 0x2000) /* Flash registers base address */

#define CRC_BASE                                (AHBPERIPH_BASE + 0x3000)
#define DBGMCU_BASE                             ((uint32_t)0xE000D000)

#define OB_BASE                                 ((uint32_t)0x1FFFF800)    /* Flash Option Bytes base address */
#define ESIG_BASE                               ((uint32_t)0x1FFFF7E0)
#define INFO_BASE                               ((uint32_t)0x1FFFF704)

#define EXTEN_BASE                              (AHBPERIPH_BASE + 0x3800)

#define PFIC_BASE    (CORE_PERIPH_BASE + 0xE000)
#define SysTick_BASE    (CORE_PERIPH_BASE + 0xF000)






/* Peripheral declaration */
#define TIM2                                    ((TIM_TypeDef *)TIM2_BASE)
#define TIM3                                    ((TIM_TypeDef *)TIM3_BASE)
#define TIM4                                    ((TIM_TypeDef *)TIM4_BASE)
#define TIM5                                    ((TIM_TypeDef *)TIM5_BASE)
#define TIM6                                    ((TIM_TypeDef *)TIM6_BASE)
#define TIM7                                    ((TIM_TypeDef *)TIM7_BASE)
#define TIM12                                   ((TIM_TypeDef *)TIM12_BASE)
#define TIM13                                   ((TIM_TypeDef *)TIM13_BASE)
#define TIM14                                   ((TIM_TypeDef *)TIM14_BASE)
#define RTC                                     ((RTC_TypeDef *)RTC_BASE)
#define WWDG                                    ((WWDG_TypeDef *)WWDG_BASE)
#define IWDG                                    ((IWDG_TypeDef *)IWDG_BASE)
#define SPI2                                    ((SPI_TypeDef *)SPI2_BASE)
#define SPI3                					((SPI_TypeDef *) SPI3_BASE)
#define USART2                                  ((USART_TypeDef *)USART2_BASE)
#define USART3                                  ((USART_TypeDef *)USART3_BASE)
#define UART4                                   ((USART_TypeDef *)UART4_BASE)
#define UART5               					((USART_TypeDef *) UART5_BASE)
#define I2C1                                    ((I2C_TypeDef *)I2C1_BASE)
#define I2C2                                    ((I2C_TypeDef *)I2C2_BASE)
#define BKP                                     ((BKP_TypeDef *)BKP_BASE)
#define PWR                                     ((PWR_TypeDef *)PWR_BASE)
#define DAC                 					((DAC_TypeDef *) DAC_BASE)

#define AFIO                                    ((AFIO_TypeDef *)AFIO_BASE)
#define EXTI                                    ((EXTI_TypeDef *)EXTI_BASE)
#define GPIOA                                   ((GPIO_TypeDef *)GPIOA_BASE)
#define GPIOB                                   ((GPIO_TypeDef *)GPIOB_BASE)
#define GPIOC                                   ((GPIO_TypeDef *)GPIOC_BASE)
#define GPIOD                                   ((GPIO_TypeDef *)GPIOD_BASE)
#define GPIOE                                   ((GPIO_TypeDef *)GPIOE_BASE)
#define GPIOF                                   ((GPIO_TypeDef *)GPIOF_BASE)
#define GPIOG                                   ((GPIO_TypeDef *)GPIOG_BASE)
#define ADC1                                    ((ADC_TypeDef *)ADC1_BASE)
#define ADC2                                    ((ADC_TypeDef *)ADC2_BASE)
#define TIM1                                    ((TIM_TypeDef *)TIM1_BASE)
#define SPI1                                    ((SPI_TypeDef *)SPI1_BASE)
#define TIM8                					((TIM_TypeDef *) TIM8_BASE)
#define USART1                                  ((USART_TypeDef *)USART1_BASE)
#define ADC3                					((ADC_TypeDef *) ADC3_BASE)
#define TIM15               					((TIM_TypeDef *) TIM15_BASE)
#define TIM16               					((TIM_TypeDef *) TIM16_BASE)
#define TIM17               					((TIM_TypeDef *) TIM17_BASE)
#define TIM9                					((TIM_TypeDef *) TIM9_BASE)
#define TIM10               					((TIM_TypeDef *) TIM10_BASE)
#define TIM11               					((TIM_TypeDef *) TIM11_BASE)

#define DMA1                                    ((DMA_TypeDef *)DMA1_BASE)
#define DMA1_Channel1                           ((DMA_Channel_TypeDef *)DMA1_Channel1_BASE)
#define DMA1_Channel2                           ((DMA_Channel_TypeDef *)DMA1_Channel2_BASE)
#define DMA1_Channel3                           ((DMA_Channel_TypeDef *)DMA1_Channel3_BASE)
#define DMA1_Channel4                           ((DMA_Channel_TypeDef *)DMA1_Channel4_BASE)
#define DMA1_Channel5                           ((DMA_Channel_TypeDef *)DMA1_Channel5_BASE)
#define DMA1_Channel6                           ((DMA_Channel_TypeDef *)DMA1_Channel6_BASE)
#define DMA1_Channel7                           ((DMA_Channel_TypeDef *)DMA1_Channel7_BASE)
#define DMA2                					((DMA_TypeDef *) DMA2_BASE)
#define DMA2_EXTEN          					((DMA_TypeDef *) DMA2_EXTEN_BASE)
#define DMA2_Channel1       					((DMA_Channel_TypeDef *) DMA2_Channel1_BASE)
#define DMA2_Channel2       					((DMA_Channel_TypeDef *) DMA2_Channel2_BASE)
#define DMA2_Channel3       					((DMA_Channel_TypeDef *) DMA2_Channel3_BASE)
#define DMA2_Channel4       					((DMA_Channel_TypeDef *) DMA2_Channel4_BASE)
#define DMA2_Channel5       					((DMA_Channel_TypeDef *) DMA2_Channel5_BASE)
#define RCC                                     ((RCC_TypeDef *)RCC_BASE)
#define FLASH                                   ((FLASH_TypeDef *)FLASH_R_BASE)
#define CRC                                     ((CRC_TypeDef *)CRC_BASE)
#define OB                                      ((OB_TypeDef *)OB_BASE)
#define ESIG                                    ((ESG_TypeDef *)ESIG_BASE)
// Mentioned in ch32v30x_dbgmcu.c, may not work on all processors.
#define INFO                                    ((INFO_TypeDef *)INFO_BASE) 
#define EXTEN                                   ((EXTEN_TypeDef *)EXTEN_BASE)
#define EXTEND                                  ((EXTEND_TypeDef *)EXTEN_BASE)  // Alias to EXTEN



#define DBGMCU                                  ((DBGMCU_TypeDef *)DBGMCU_BASE)

/******************************************************************************/
/*                         Peripheral Registers Bits Definition               */
/******************************************************************************/

/******************************************************************************/
/*                        Analog to Digital Converter                         */
/******************************************************************************/

/********************  Bit definition for ADC_STATR register  ********************/
#define ADC_AWD                                 ((uint8_t)0x01) /* Analog watchdog flag */
#define ADC_EOC                                 ((uint8_t)0x02) /* End of conversion */
#define ADC_JEOC                                ((uint8_t)0x04) /* Injected channel end of conversion */
#define ADC_JSTRT                               ((uint8_t)0x08) /* Injected channel Start flag */
#define ADC_STRT                                ((uint8_t)0x10) /* Regular channel Start flag */

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

#define ADC_DUALMOD                             ((uint32_t)0x000F0000) /* DUALMOD[3:0] bits (Dual mode selection) */
#define ADC_DUALMOD_0                           ((uint32_t)0x00010000) /* Bit 0 */
#define ADC_DUALMOD_1                           ((uint32_t)0x00020000) /* Bit 1 */
#define ADC_DUALMOD_2                           ((uint32_t)0x00040000) /* Bit 2 */
#define ADC_DUALMOD_3                           ((uint32_t)0x00080000) /* Bit 3 */

#define ADC_JAWDEN                              ((uint32_t)0x00400000) /* Analog watchdog enable on injected channels */
#define ADC_AWDEN                               ((uint32_t)0x00800000) /* Analog watchdog enable on regular channels */

/*******************  Bit definition for ADC_CTLR2 register  ********************/
#define ADC_ADON                                ((uint32_t)0x00000001) /* A/D Converter ON / OFF */
#define ADC_CONT                                ((uint32_t)0x00000002) /* Continuous Conversion */
#define ADC_CAL                                 ((uint32_t)0x00000004) /* A/D Calibration */
#define ADC_RSTCAL                              ((uint32_t)0x00000008) /* Reset Calibration */
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

#define ADC_EXTTRIG                             ((uint32_t)0x00100000) /* External Trigger Conversion mode for regular channels */
#define ADC_JSWSTART                            ((uint32_t)0x00200000) /* Start Conversion of injected channels */
#define ADC_SWSTART                             ((uint32_t)0x00400000) /* Start Conversion of regular channels */
#define ADC_TSVREFE                             ((uint32_t)0x00800000) /* Temperature Sensor and VREFINT Enable */

/******************  Bit definition for ADC_SAMPTR1 register  *******************/
#define ADC_SMP10                               ((uint32_t)0x00000007) /* SMP10[2:0] bits (Channel 10 Sample time selection) */
#define ADC_SMP10_0                             ((uint32_t)0x00000001) /* Bit 0 */
#define ADC_SMP10_1                             ((uint32_t)0x00000002) /* Bit 1 */
#define ADC_SMP10_2                             ((uint32_t)0x00000004) /* Bit 2 */

#define ADC_SMP11                               ((uint32_t)0x00000038) /* SMP11[2:0] bits (Channel 11 Sample time selection) */
#define ADC_SMP11_0                             ((uint32_t)0x00000008) /* Bit 0 */
#define ADC_SMP11_1                             ((uint32_t)0x00000010) /* Bit 1 */
#define ADC_SMP11_2                             ((uint32_t)0x00000020) /* Bit 2 */

#define ADC_SMP12                               ((uint32_t)0x000001C0) /* SMP12[2:0] bits (Channel 12 Sample time selection) */
#define ADC_SMP12_0                             ((uint32_t)0x00000040) /* Bit 0 */
#define ADC_SMP12_1                             ((uint32_t)0x00000080) /* Bit 1 */
#define ADC_SMP12_2                             ((uint32_t)0x00000100) /* Bit 2 */

#define ADC_SMP13                               ((uint32_t)0x00000E00) /* SMP13[2:0] bits (Channel 13 Sample time selection) */
#define ADC_SMP13_0                             ((uint32_t)0x00000200) /* Bit 0 */
#define ADC_SMP13_1                             ((uint32_t)0x00000400) /* Bit 1 */
#define ADC_SMP13_2                             ((uint32_t)0x00000800) /* Bit 2 */

#define ADC_SMP14                               ((uint32_t)0x00007000) /* SMP14[2:0] bits (Channel 14 Sample time selection) */
#define ADC_SMP14_0                             ((uint32_t)0x00001000) /* Bit 0 */
#define ADC_SMP14_1                             ((uint32_t)0x00002000) /* Bit 1 */
#define ADC_SMP14_2                             ((uint32_t)0x00004000) /* Bit 2 */

#define ADC_SMP15                               ((uint32_t)0x00038000) /* SMP15[2:0] bits (Channel 15 Sample time selection) */
#define ADC_SMP15_0                             ((uint32_t)0x00008000) /* Bit 0 */
#define ADC_SMP15_1                             ((uint32_t)0x00010000) /* Bit 1 */
#define ADC_SMP15_2                             ((uint32_t)0x00020000) /* Bit 2 */

#define ADC_SMP16                               ((uint32_t)0x001C0000) /* SMP16[2:0] bits (Channel 16 Sample time selection) */
#define ADC_SMP16_0                             ((uint32_t)0x00040000) /* Bit 0 */
#define ADC_SMP16_1                             ((uint32_t)0x00080000) /* Bit 1 */
#define ADC_SMP16_2                             ((uint32_t)0x00100000) /* Bit 2 */

#define ADC_SMP17                               ((uint32_t)0x00E00000) /* SMP17[2:0] bits (Channel 17 Sample time selection) */
#define ADC_SMP17_0                             ((uint32_t)0x00200000) /* Bit 0 */
#define ADC_SMP17_1                             ((uint32_t)0x00400000) /* Bit 1 */
#define ADC_SMP17_2                             ((uint32_t)0x00800000) /* Bit 2 */

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
#define ADC_JOFFSET1                            ((uint16_t)0x0FFF) /* Data offset for injected channel 1 */

/******************  Bit definition for ADC_IOFR2 register  *******************/
#define ADC_JOFFSET2                            ((uint16_t)0x0FFF) /* Data offset for injected channel 2 */

/******************  Bit definition for ADC_IOFR3 register  *******************/
#define ADC_JOFFSET3                            ((uint16_t)0x0FFF) /* Data offset for injected channel 3 */

/******************  Bit definition for ADC_IOFR4 register  *******************/
#define ADC_JOFFSET4                            ((uint16_t)0x0FFF) /* Data offset for injected channel 4 */

/*******************  Bit definition for ADC_WDHTR register  ********************/
#define ADC_HT                                  ((uint16_t)0x0FFF) /* Analog watchdog high threshold */

/*******************  Bit definition for ADC_WDLTR register  ********************/
#define ADC_LT                                  ((uint16_t)0x0FFF) /* Analog watchdog low threshold */

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
#define ADC_IDATAR1_JDATA                       ((uint16_t)0xFFFF) /* Injected data */

/*******************  Bit definition for ADC_IDATAR2 register  *******************/
#define ADC_IDATAR2_JDATA                       ((uint16_t)0xFFFF) /* Injected data */

/*******************  Bit definition for ADC_IDATAR3 register  *******************/
#define ADC_IDATAR3_JDATA                       ((uint16_t)0xFFFF) /* Injected data */

/*******************  Bit definition for ADC_IDATAR4 register  *******************/
#define ADC_IDATAR4_JDATA                       ((uint16_t)0xFFFF) /* Injected data */

/********************  Bit definition for ADC_RDATAR register  ********************/
#define ADC_RDATAR_DATA                         ((uint32_t)0x0000FFFF) /* Regular data */
#define ADC_RDATAR_ADC2DATA                     ((uint32_t)0xFFFF0000) /* ADC2 data */

/******************************************************************************/
/*                            Backup registers                                */
/******************************************************************************/

/*******************  Bit definition for BKP_DATAR1 register  ********************/
#define BKP_DATAR1_D                            ((uint16_t)0xFFFF) /* Backup data */

/*******************  Bit definition for BKP_DATAR2 register  ********************/
#define BKP_DATAR2_D                            ((uint16_t)0xFFFF) /* Backup data */

/*******************  Bit definition for BKP_DATAR3 register  ********************/
#define BKP_DATAR3_D                            ((uint16_t)0xFFFF) /* Backup data */

/*******************  Bit definition for BKP_DATAR4 register  ********************/
#define BKP_DATAR4_D                            ((uint16_t)0xFFFF) /* Backup data */

/*******************  Bit definition for BKP_DATAR5 register  ********************/
#define BKP_DATAR5_D                            ((uint16_t)0xFFFF) /* Backup data */

/*******************  Bit definition for BKP_DATAR6 register  ********************/
#define BKP_DATAR6_D                            ((uint16_t)0xFFFF) /* Backup data */

/*******************  Bit definition for BKP_DATAR7 register  ********************/
#define BKP_DATAR7_D                            ((uint16_t)0xFFFF) /* Backup data */

/*******************  Bit definition for BKP_DATAR8 register  ********************/
#define BKP_DATAR8_D                            ((uint16_t)0xFFFF) /* Backup data */

/*******************  Bit definition for BKP_DATAR9 register  ********************/
#define BKP_DATAR9_D                            ((uint16_t)0xFFFF) /* Backup data */

/*******************  Bit definition for BKP_DATAR10 register  *******************/
#define BKP_DATAR10_D                           ((uint16_t)0xFFFF) /* Backup data */

/*******************  Bit definition for BKP_DATAR11 register  *******************/
#define BKP_DATAR11_D                           ((uint16_t)0xFFFF) /* Backup data */

/*******************  Bit definition for BKP_DATAR12 register  *******************/
#define BKP_DATAR12_D                           ((uint16_t)0xFFFF) /* Backup data */

/*******************  Bit definition for BKP_DATAR13 register  *******************/
#define BKP_DATAR13_D                           ((uint16_t)0xFFFF) /* Backup data */

/*******************  Bit definition for BKP_DATAR14 register  *******************/
#define BKP_DATAR14_D                           ((uint16_t)0xFFFF) /* Backup data */

/*******************  Bit definition for BKP_DATAR15 register  *******************/
#define BKP_DATAR15_D                           ((uint16_t)0xFFFF) /* Backup data */

/*******************  Bit definition for BKP_DATAR16 register  *******************/
#define BKP_DATAR16_D                           ((uint16_t)0xFFFF) /* Backup data */

/*******************  Bit definition for BKP_DATAR17 register  *******************/
#define BKP_DATAR17_D                           ((uint16_t)0xFFFF) /* Backup data */

/******************  Bit definition for BKP_DATAR18 register  ********************/
#define BKP_DATAR18_D                           ((uint16_t)0xFFFF) /* Backup data */

/*******************  Bit definition for BKP_DATAR19 register  *******************/
#define BKP_DATAR19_D                           ((uint16_t)0xFFFF) /* Backup data */

/*******************  Bit definition for BKP_DATAR20 register  *******************/
#define BKP_DATAR20_D                           ((uint16_t)0xFFFF) /* Backup data */

/*******************  Bit definition for BKP_DATAR21 register  *******************/
#define BKP_DATAR21_D                           ((uint16_t)0xFFFF) /* Backup data */

/*******************  Bit definition for BKP_DATAR22 register  *******************/
#define BKP_DATAR22_D                           ((uint16_t)0xFFFF) /* Backup data */

/*******************  Bit definition for BKP_DATAR23 register  *******************/
#define BKP_DATAR23_D                           ((uint16_t)0xFFFF) /* Backup data */

/*******************  Bit definition for BKP_DATAR24 register  *******************/
#define BKP_DATAR24_D                           ((uint16_t)0xFFFF) /* Backup data */

/*******************  Bit definition for BKP_DATAR25 register  *******************/
#define BKP_DATAR25_D                           ((uint16_t)0xFFFF) /* Backup data */

/*******************  Bit definition for BKP_DATAR26 register  *******************/
#define BKP_DATAR26_D                           ((uint16_t)0xFFFF) /* Backup data */

/*******************  Bit definition for BKP_DATAR27 register  *******************/
#define BKP_DATAR27_D                           ((uint16_t)0xFFFF) /* Backup data */

/*******************  Bit definition for BKP_DATAR28 register  *******************/
#define BKP_DATAR28_D                           ((uint16_t)0xFFFF) /* Backup data */

/*******************  Bit definition for BKP_DATAR29 register  *******************/
#define BKP_DATAR29_D                           ((uint16_t)0xFFFF) /* Backup data */

/*******************  Bit definition for BKP_DATAR30 register  *******************/
#define BKP_DATAR30_D                           ((uint16_t)0xFFFF) /* Backup data */

/*******************  Bit definition for BKP_DATAR31 register  *******************/
#define BKP_DATAR31_D                           ((uint16_t)0xFFFF) /* Backup data */

/*******************  Bit definition for BKP_DATAR32 register  *******************/
#define BKP_DATAR32_D                           ((uint16_t)0xFFFF) /* Backup data */

/*******************  Bit definition for BKP_DATAR33 register  *******************/
#define BKP_DATAR33_D                           ((uint16_t)0xFFFF) /* Backup data */

/*******************  Bit definition for BKP_DATAR34 register  *******************/
#define BKP_DATAR34_D                           ((uint16_t)0xFFFF) /* Backup data */

/*******************  Bit definition for BKP_DATAR35 register  *******************/
#define BKP_DATAR35_D                           ((uint16_t)0xFFFF) /* Backup data */

/*******************  Bit definition for BKP_DATAR36 register  *******************/
#define BKP_DATAR36_D                           ((uint16_t)0xFFFF) /* Backup data */

/*******************  Bit definition for BKP_DATAR37 register  *******************/
#define BKP_DATAR37_D                           ((uint16_t)0xFFFF) /* Backup data */

/*******************  Bit definition for BKP_DATAR38 register  *******************/
#define BKP_DATAR38_D                           ((uint16_t)0xFFFF) /* Backup data */

/*******************  Bit definition for BKP_DATAR39 register  *******************/
#define BKP_DATAR39_D                           ((uint16_t)0xFFFF) /* Backup data */

/*******************  Bit definition for BKP_DATAR40 register  *******************/
#define BKP_DATAR40_D                           ((uint16_t)0xFFFF) /* Backup data */

/*******************  Bit definition for BKP_DATAR41 register  *******************/
#define BKP_DATAR41_D                           ((uint16_t)0xFFFF) /* Backup data */

/*******************  Bit definition for BKP_DATAR42 register  *******************/
#define BKP_DATAR42_D                           ((uint16_t)0xFFFF) /* Backup data */

/******************  Bit definition for BKP_OCTLR register  *******************/
#define BKP_CAL                                 ((uint16_t)0x007F) /* Calibration value */
#define BKP_CCO                                 ((uint16_t)0x0080) /* Calibration Clock Output */
#define BKP_ASOE                                ((uint16_t)0x0100) /* Alarm or Second Output Enable */
#define BKP_ASOS                                ((uint16_t)0x0200) /* Alarm or Second Output Selection */

/********************  Bit definition for BKP_TPCTLR register  ********************/
#define BKP_TPE                                 ((uint8_t)0x01) /* TAMPER pin enable */
#define BKP_TPAL                                ((uint8_t)0x02) /* TAMPER pin active level */

/*******************  Bit definition for BKP_TPCSR register  ********************/
#define BKP_CTE                                 ((uint16_t)0x0001) /* Clear Tamper event */
#define BKP_CTI                                 ((uint16_t)0x0002) /* Clear Tamper Interrupt */
#define BKP_TPIE                                ((uint16_t)0x0004) /* TAMPER Pin interrupt enable */
#define BKP_TEF                                 ((uint16_t)0x0100) /* Tamper Event Flag */
#define BKP_TIF                                 ((uint16_t)0x0200) /* Tamper Interrupt Flag */

/******************************************************************************/
/*                          CRC Calculation Unit                              */
/******************************************************************************/

/*******************  Bit definition for CRC_DATAR register  *********************/
#define CRC_DATAR_DR                            ((uint32_t)0xFFFFFFFF) /* Data register bits */

/*******************  Bit definition for CRC_IDATAR register  ********************/
#define CRC_IDR_IDATAR                          ((uint8_t)0xFF) /* General-purpose 8-bit data register bits */

/********************  Bit definition for CRC_CTLR register  ********************/
#define CRC_CTLR_RESET                          ((uint8_t)0x01) /* RESET bit */

/******************************************************************************/
/*                      Digital to Analog Converter                           */
/******************************************************************************/

/********************  Bit definition for DAC_CTLR register  ********************/
#define DAC_EN1                                 ((uint32_t)0x00000001) /* DAC channel1 enable */
#define DAC_BOFF1                               ((uint32_t)0x00000002) /* DAC channel1 output buffer disable */
#define DAC_TEN1                                ((uint32_t)0x00000004) /* DAC channel1 Trigger enable */

#define DAC_TSEL1                               ((uint32_t)0x00000038) /* TSEL1[2:0] (DAC channel1 Trigger selection) */
#define DAC_TSEL1_0                             ((uint32_t)0x00000008) /* Bit 0 */
#define DAC_TSEL1_1                             ((uint32_t)0x00000010) /* Bit 1 */
#define DAC_TSEL1_2                             ((uint32_t)0x00000020) /* Bit 2 */

#define DAC_WAVE1                               ((uint32_t)0x000000C0) /* WAVE1[1:0] (DAC channel1 noise/triangle wave generation enable) */
#define DAC_WAVE1_0                             ((uint32_t)0x00000040) /* Bit 0 */
#define DAC_WAVE1_1                             ((uint32_t)0x00000080) /* Bit 1 */

#define DAC_MAMP1                               ((uint32_t)0x00000F00) /* MAMP1[3:0] (DAC channel1 Mask/Amplitude selector) */
#define DAC_MAMP1_0                             ((uint32_t)0x00000100) /* Bit 0 */
#define DAC_MAMP1_1                             ((uint32_t)0x00000200) /* Bit 1 */
#define DAC_MAMP1_2                             ((uint32_t)0x00000400) /* Bit 2 */
#define DAC_MAMP1_3                             ((uint32_t)0x00000800) /* Bit 3 */

#define DAC_DMAEN1                              ((uint32_t)0x00001000) /* DAC channel1 DMA enable */
#define DAC_EN2                                 ((uint32_t)0x00010000) /* DAC channel2 enable */
#define DAC_BOFF2                               ((uint32_t)0x00020000) /* DAC channel2 output buffer disable */
#define DAC_TEN2                                ((uint32_t)0x00040000) /* DAC channel2 Trigger enable */

#define DAC_TSEL2                               ((uint32_t)0x00380000) /* TSEL2[2:0] (DAC channel2 Trigger selection) */
#define DAC_TSEL2_0                             ((uint32_t)0x00080000) /* Bit 0 */
#define DAC_TSEL2_1                             ((uint32_t)0x00100000) /* Bit 1 */
#define DAC_TSEL2_2                             ((uint32_t)0x00200000) /* Bit 2 */

#define DAC_WAVE2                               ((uint32_t)0x00C00000) /* WAVE2[1:0] (DAC channel2 noise/triangle wave generation enable) */
#define DAC_WAVE2_0                             ((uint32_t)0x00400000) /* Bit 0 */
#define DAC_WAVE2_1                             ((uint32_t)0x00800000) /* Bit 1 */

#define DAC_MAMP2                               ((uint32_t)0x0F000000) /* MAMP2[3:0] (DAC channel2 Mask/Amplitude selector) */
#define DAC_MAMP2_0                             ((uint32_t)0x01000000) /* Bit 0 */
#define DAC_MAMP2_1                             ((uint32_t)0x02000000) /* Bit 1 */
#define DAC_MAMP2_2                             ((uint32_t)0x04000000) /* Bit 2 */
#define DAC_MAMP2_3                             ((uint32_t)0x08000000) /* Bit 3 */

#define DAC_DMAEN2                              ((uint32_t)0x10000000) /* DAC channel2 DMA enabled */

/*****************  Bit definition for DAC_SWTR register  ******************/
#define DAC_SWTRIG1                             ((uint8_t)0x01) /* DAC channel1 software trigger */
#define DAC_SWTRIG2                             ((uint8_t)0x02) /* DAC channel2 software trigger */

/*****************  Bit definition for DAC_R12BDHR1 register  ******************/
#define DAC_DHR12R1                             ((uint16_t)0x0FFF) /* DAC channel1 12-bit Right aligned data */

/*****************  Bit definition for DAC_L12BDHR1 register  ******************/
#define DAC_DHR12L1                             ((uint16_t)0xFFF0) /* DAC channel1 12-bit Left aligned data */

/******************  Bit definition for DAC_R8BDHR1 register  ******************/
#define DAC_DHR8R1                              ((uint8_t)0xFF) /* DAC channel1 8-bit Right aligned data */

/*****************  Bit definition for DAC_R12BDHR2 register  ******************/
#define DAC_DHR12R2                             ((uint16_t)0x0FFF) /* DAC channel2 12-bit Right aligned data */

/*****************  Bit definition for DAC_L12BDHR2 register  ******************/
#define DAC_DHR12L2                             ((uint16_t)0xFFF0) /* DAC channel2 12-bit Left aligned data */

/******************  Bit definition for DAC_R8BDHR2 register  ******************/
#define DAC_DHR8R2                              ((uint8_t)0xFF) /* DAC channel2 8-bit Right aligned data */

/*****************  Bit definition for DAC_RD12BDHR register  ******************/
#define DAC_RD12BDHR_DACC1DHR                   ((uint32_t)0x00000FFF) /* DAC channel1 12-bit Right aligned data */
#define DAC_RD12BDHR_DACC2DHR                   ((uint32_t)0x0FFF0000) /* DAC channel2 12-bit Right aligned data */

/*****************  Bit definition for DAC_LD12BDHR register  ******************/
#define DAC_LD12BDHR_DACC1DHR                   ((uint32_t)0x0000FFF0) /* DAC channel1 12-bit Left aligned data */
#define DAC_LD12BDHR_DACC2DHR                   ((uint32_t)0xFFF00000) /* DAC channel2 12-bit Left aligned data */

/******************  Bit definition for DAC_RD8BDHR register  ******************/
#define DAC_RD8BDHR_DACC1DHR                    ((uint16_t)0x00FF) /* DAC channel1 8-bit Right aligned data */
#define DAC_RD8BDHR_DACC2DHR                    ((uint16_t)0xFF00) /* DAC channel2 8-bit Right aligned data */

/*******************  Bit definition for DAC_DOR1 register  *******************/
#define DAC_DACC1DOR                            ((uint16_t)0x0FFF) /* DAC channel1 data output */

/*******************  Bit definition for DAC_DOR2 register  *******************/
#define DAC_DACC2DOR                            ((uint16_t)0x0FFF) /* DAC channel2 data output */

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

/*******************  Bit definition for DMA_CFGR4 register  *******************/
#define DMA_CFGR4_EN                             ((uint16_t)0x0001) /* Channel enable */
#define DMA_CFGR4_TCIE                           ((uint16_t)0x0002) /* Transfer complete interrupt enable */
#define DMA_CFGR4_HTIE                           ((uint16_t)0x0004) /* Half Transfer interrupt enable */
#define DMA_CFGR4_TEIE                           ((uint16_t)0x0008) /* Transfer error interrupt enable */
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

/******************  Bit definition for DMA_CFGR5 register  *******************/
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

/*******************  Bit definition for DMA_CFGR6 register  *******************/
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

/*******************  Bit definition for DMA_CFGR7 register  *******************/
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
#define EXTI_INTENR_MR10                        ((uint32_t)0x00000400) /* Interrupt Mask on line 10 */
#define EXTI_INTENR_MR11                        ((uint32_t)0x00000800) /* Interrupt Mask on line 11 */
#define EXTI_INTENR_MR12                        ((uint32_t)0x00001000) /* Interrupt Mask on line 12 */
#define EXTI_INTENR_MR13                        ((uint32_t)0x00002000) /* Interrupt Mask on line 13 */
#define EXTI_INTENR_MR14                        ((uint32_t)0x00004000) /* Interrupt Mask on line 14 */
#define EXTI_INTENR_MR15                        ((uint32_t)0x00008000) /* Interrupt Mask on line 15 */
#define EXTI_INTENR_MR16                        ((uint32_t)0x00010000) /* Interrupt Mask on line 16 */
#define EXTI_INTENR_MR17                        ((uint32_t)0x00020000) /* Interrupt Mask on line 17 */
#define EXTI_INTENR_MR18                        ((uint32_t)0x00040000) /* Interrupt Mask on line 18 */
#define EXTI_INTENR_MR19                        ((uint32_t)0x00080000) /* Interrupt Mask on line 19 */

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
#define EXTI_EVENR_MR10                         ((uint32_t)0x00000400) /* Event Mask on line 10 */
#define EXTI_EVENR_MR11                         ((uint32_t)0x00000800) /* Event Mask on line 11 */
#define EXTI_EVENR_MR12                         ((uint32_t)0x00001000) /* Event Mask on line 12 */
#define EXTI_EVENR_MR13                         ((uint32_t)0x00002000) /* Event Mask on line 13 */
#define EXTI_EVENR_MR14                         ((uint32_t)0x00004000) /* Event Mask on line 14 */
#define EXTI_EVENR_MR15                         ((uint32_t)0x00008000) /* Event Mask on line 15 */
#define EXTI_EVENR_MR16                         ((uint32_t)0x00010000) /* Event Mask on line 16 */
#define EXTI_EVENR_MR17                         ((uint32_t)0x00020000) /* Event Mask on line 17 */
#define EXTI_EVENR_MR18                         ((uint32_t)0x00040000) /* Event Mask on line 18 */
#define EXTI_EVENR_MR19                         ((uint32_t)0x00080000) /* Event Mask on line 19 */

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
#define EXTI_RTENR_TR10                         ((uint32_t)0x00000400) /* Rising trigger event configuration bit of line 10 */
#define EXTI_RTENR_TR11                         ((uint32_t)0x00000800) /* Rising trigger event configuration bit of line 11 */
#define EXTI_RTENR_TR12                         ((uint32_t)0x00001000) /* Rising trigger event configuration bit of line 12 */
#define EXTI_RTENR_TR13                         ((uint32_t)0x00002000) /* Rising trigger event configuration bit of line 13 */
#define EXTI_RTENR_TR14                         ((uint32_t)0x00004000) /* Rising trigger event configuration bit of line 14 */
#define EXTI_RTENR_TR15                         ((uint32_t)0x00008000) /* Rising trigger event configuration bit of line 15 */
#define EXTI_RTENR_TR16                         ((uint32_t)0x00010000) /* Rising trigger event configuration bit of line 16 */
#define EXTI_RTENR_TR17                         ((uint32_t)0x00020000) /* Rising trigger event configuration bit of line 17 */
#define EXTI_RTENR_TR18                         ((uint32_t)0x00040000) /* Rising trigger event configuration bit of line 18 */
#define EXTI_RTENR_TR19                         ((uint32_t)0x00080000) /* Rising trigger event configuration bit of line 19 */

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
#define EXTI_FTENR_TR10                         ((uint32_t)0x00000400) /* Falling trigger event configuration bit of line 10 */
#define EXTI_FTENR_TR11                         ((uint32_t)0x00000800) /* Falling trigger event configuration bit of line 11 */
#define EXTI_FTENR_TR12                         ((uint32_t)0x00001000) /* Falling trigger event configuration bit of line 12 */
#define EXTI_FTENR_TR13                         ((uint32_t)0x00002000) /* Falling trigger event configuration bit of line 13 */
#define EXTI_FTENR_TR14                         ((uint32_t)0x00004000) /* Falling trigger event configuration bit of line 14 */
#define EXTI_FTENR_TR15                         ((uint32_t)0x00008000) /* Falling trigger event configuration bit of line 15 */
#define EXTI_FTENR_TR16                         ((uint32_t)0x00010000) /* Falling trigger event configuration bit of line 16 */
#define EXTI_FTENR_TR17                         ((uint32_t)0x00020000) /* Falling trigger event configuration bit of line 17 */
#define EXTI_FTENR_TR18                         ((uint32_t)0x00040000) /* Falling trigger event configuration bit of line 18 */
#define EXTI_FTENR_TR19                         ((uint32_t)0x00080000) /* Falling trigger event configuration bit of line 19 */

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
#define EXTI_SWIEVR_SWIEVR10                    ((uint32_t)0x00000400) /* Software Interrupt on line 10 */
#define EXTI_SWIEVR_SWIEVR11                    ((uint32_t)0x00000800) /* Software Interrupt on line 11 */
#define EXTI_SWIEVR_SWIEVR12                    ((uint32_t)0x00001000) /* Software Interrupt on line 12 */
#define EXTI_SWIEVR_SWIEVR13                    ((uint32_t)0x00002000) /* Software Interrupt on line 13 */
#define EXTI_SWIEVR_SWIEVR14                    ((uint32_t)0x00004000) /* Software Interrupt on line 14 */
#define EXTI_SWIEVR_SWIEVR15                    ((uint32_t)0x00008000) /* Software Interrupt on line 15 */
#define EXTI_SWIEVR_SWIEVR16                    ((uint32_t)0x00010000) /* Software Interrupt on line 16 */
#define EXTI_SWIEVR_SWIEVR17                    ((uint32_t)0x00020000) /* Software Interrupt on line 17 */
#define EXTI_SWIEVR_SWIEVR18                    ((uint32_t)0x00040000) /* Software Interrupt on line 18 */
#define EXTI_SWIEVR_SWIEVR19                    ((uint32_t)0x00080000) /* Software Interrupt on line 19 */

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
#define EXTI_INTF_INTF10                        ((uint32_t)0x00000400) /* Pending bit for line 10 */
#define EXTI_INTF_INTF11                        ((uint32_t)0x00000800) /* Pending bit for line 11 */
#define EXTI_INTF_INTF12                        ((uint32_t)0x00001000) /* Pending bit for line 12 */
#define EXTI_INTF_INTF13                        ((uint32_t)0x00002000) /* Pending bit for line 13 */
#define EXTI_INTF_INTF14                        ((uint32_t)0x00004000) /* Pending bit for line 14 */
#define EXTI_INTF_INTF15                        ((uint32_t)0x00008000) /* Pending bit for line 15 */
#define EXTI_INTF_INTF16                        ((uint32_t)0x00010000) /* Pending bit for line 16 */
#define EXTI_INTF_INTF17                        ((uint32_t)0x00020000) /* Pending bit for line 17 */
#define EXTI_INTF_INTF18                        ((uint32_t)0x00040000) /* Pending bit for line 18 */
#define EXTI_INTF_INTF19                        ((uint32_t)0x00080000) /* Pending bit for line 19 */

/******************************************************************************/
/*                      FLASH and Option Bytes Registers                      */
/******************************************************************************/

/*******************  Bit definition for FLASH_ACTLR register  ******************/
#define FLASH_ACTLR_LATENCY                     ((uint8_t)0x03) /* LATENCY[2:0] bits (Latency) */
#define FLASH_ACTLR_LATENCY_0                   ((uint8_t)0x00) /* Bit 0 */
#define FLASH_ACTLR_LATENCY_1                   ((uint8_t)0x01) /* Bit 0 */
#define FLASH_ACTLR_LATENCY_2                   ((uint8_t)0x02) /* Bit 1 */

#define FLASH_ACTLR_HLFCYA                      ((uint8_t)0x08) /* Flash Half Cycle Access Enable */
#define FLASH_ACTLR_PRFTBE                      ((uint8_t)0x10) /* Prefetch Buffer Enable */
#define FLASH_ACTLR_PRFTBS                      ((uint8_t)0x20) /* Prefetch Buffer Status */

/******************  Bit definition for FLASH_KEYR register  ******************/
#define FLASH_KEYR_FKEYR                        ((uint32_t)0xFFFFFFFF) /* FPEC Key */

/*****************  Bit definition for FLASH_OBKEYR register  ****************/
#define FLASH_OBKEYR_OBKEYR                     ((uint32_t)0xFFFFFFFF) /* Option Byte Key */

/******************  Bit definition for FLASH_STATR register  *******************/
#define FLASH_STATR_BSY                         ((uint8_t)0x01) /* Busy */
#define FLASH_STATR_PGERR                       ((uint8_t)0x04) /* Programming Error */
#define FLASH_STATR_WRPRTERR                    ((uint8_t)0x10) /* Write Protection Error */
#define FLASH_STATR_EOP                         ((uint8_t)0x20) /* End of operation */

/*******************  Bit definition for FLASH_CTLR register  *******************/
#define FLASH_CTLR_PG                           (0x0001)     /* Programming */
#define FLASH_CTLR_PER                          (0x0002)     /* Page Erase 1KByte*/
#define FLASH_CTLR_MER                          (0x0004)     /* Mass Erase */
#define FLASH_CTLR_OPTPG                        (0x0010)     /* Option Byte Programming */
#define FLASH_CTLR_OPTER                        (0x0020)     /* Option Byte Erase */
#define FLASH_CTLR_STRT                         (0x0040)     /* Start */
#define FLASH_CTLR_LOCK                         (0x0080)     /* Lock */
#define FLASH_CTLR_OPTWRE                       (0x0200)     /* Option Bytes Write Enable */
#define FLASH_CTLR_ERRIE                        (0x0400)     /* Error Interrupt Enable */
#define FLASH_CTLR_EOPIE                        (0x1000)     /* End of operation interrupt enable */
#define FLASH_CTLR_PAGE_PG                      (0x00010000) /* Page Programming 64Byte */
#define FLASH_CTLR_PAGE_ER                      (0x00020000) /* Page Erase 128Byte */
#define FLASH_CTLR_BUF_LOAD                     (0x00040000) /* Buffer Load */
#define FLASH_CTLR_BUF_RST                      (0x00080000) /* Buffer Reset */

/*******************  Bit definition for FLASH_ADDR register  *******************/
#define FLASH_ADDR_FAR                          ((uint32_t)0xFFFFFFFF) /* Flash Address */

/******************  Bit definition for FLASH_OBR register  *******************/
#define FLASH_OBR_OPTERR                        ((uint16_t)0x0001) /* Option Byte Error */
#define FLASH_OBR_RDPRT                         ((uint16_t)0x0002) /* Read protection */

#define FLASH_OBR_USER                          ((uint16_t)0x03FC) /* User Option Bytes */
#define FLASH_OBR_WDG_SW                        ((uint16_t)0x0004) /* WDG_SW */
#define FLASH_OBR_nRST_STOP                     ((uint16_t)0x0008) /* nRST_STOP */
#define FLASH_OBR_nRST_STDBY                    ((uint16_t)0x0010) /* nRST_STDBY */
#define FLASH_OBR_RST_MODE                      ((uint16_t)0x0060) /* RST_MODE */
#define FLASH_OBR_BFB2                          ((uint16_t)0x0020) /* BFB2 */

/******************  Bit definition for FLASH_WPR register  ******************/
#define FLASH_WPR_WRP                           ((uint32_t)0xFFFFFFFF) /* Write Protect */

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
#define FLASH_WRPR2_WRPR2                       ((uint32_t)0x000000FF) /* Flash memory write protection option bytes */
#define FLASH_WRPR2_nWRPR2                      ((uint32_t)0x0000FF00) /* Flash memory write protection complemented option bytes */

/******************  Bit definition for FLASH_WRPR3 register  ******************/
#define FLASH_WRPR3_WRPR3                       ((uint32_t)0x00FF0000) /* Flash memory write protection option bytes */
#define FLASH_WRPR3_nWRPR3                      ((uint32_t)0xFF000000) /* Flash memory write protection complemented option bytes */

/******************************************************************************/
/*                General Purpose and Alternate Function I/O                  */
/******************************************************************************/

/*******************  Bit definition for GPIO_CFGLR register  *******************/
#define GPIO_CFGLR_MODE                         ((uint32_t)0x33333333) /* Port x mode bits */

#define GPIO_CFGLR_MODE0                        ((uint32_t)0x00000003) /* MODE0[1:0] bits (Port x mode bits, pin 0) */
#define GPIO_CFGLR_MODE0_0                      ((uint32_t)0x00000001) /* Bit 0 */
#define GPIO_CFGLR_MODE0_1                      ((uint32_t)0x00000002) /* Bit 1 */

#define GPIO_CFGLR_MODE1                        ((uint32_t)0x00000030) /* MODE1[1:0] bits (Port x mode bits, pin 1) */
#define GPIO_CFGLR_MODE1_0                      ((uint32_t)0x00000010) /* Bit 0 */
#define GPIO_CFGLR_MODE1_1                      ((uint32_t)0x00000020) /* Bit 1 */

#define GPIO_CFGLR_MODE2                        ((uint32_t)0x00000300) /* MODE2[1:0] bits (Port x mode bits, pin 2) */
#define GPIO_CFGLR_MODE2_0                      ((uint32_t)0x00000100) /* Bit 0 */
#define GPIO_CFGLR_MODE2_1                      ((uint32_t)0x00000200) /* Bit 1 */

#define GPIO_CFGLR_MODE3                        ((uint32_t)0x00003000) /* MODE3[1:0] bits (Port x mode bits, pin 3) */
#define GPIO_CFGLR_MODE3_0                      ((uint32_t)0x00001000) /* Bit 0 */
#define GPIO_CFGLR_MODE3_1                      ((uint32_t)0x00002000) /* Bit 1 */

#define GPIO_CFGLR_MODE4                        ((uint32_t)0x00030000) /* MODE4[1:0] bits (Port x mode bits, pin 4) */
#define GPIO_CFGLR_MODE4_0                      ((uint32_t)0x00010000) /* Bit 0 */
#define GPIO_CFGLR_MODE4_1                      ((uint32_t)0x00020000) /* Bit 1 */

#define GPIO_CFGLR_MODE5                        ((uint32_t)0x00300000) /* MODE5[1:0] bits (Port x mode bits, pin 5) */
#define GPIO_CFGLR_MODE5_0                      ((uint32_t)0x00100000) /* Bit 0 */
#define GPIO_CFGLR_MODE5_1                      ((uint32_t)0x00200000) /* Bit 1 */

#define GPIO_CFGLR_MODE6                        ((uint32_t)0x03000000) /* MODE6[1:0] bits (Port x mode bits, pin 6) */
#define GPIO_CFGLR_MODE6_0                      ((uint32_t)0x01000000) /* Bit 0 */
#define GPIO_CFGLR_MODE6_1                      ((uint32_t)0x02000000) /* Bit 1 */

#define GPIO_CFGLR_MODE7                        ((uint32_t)0x30000000) /* MODE7[1:0] bits (Port x mode bits, pin 7) */
#define GPIO_CFGLR_MODE7_0                      ((uint32_t)0x10000000) /* Bit 0 */
#define GPIO_CFGLR_MODE7_1                      ((uint32_t)0x20000000) /* Bit 1 */

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

/*******************  Bit definition for GPIO_CFGHR register  *******************/
#define GPIO_CFGHR_MODE                         ((uint32_t)0x33333333) /* Port x mode bits */

#define GPIO_CFGHR_MODE8                        ((uint32_t)0x00000003) /* MODE8[1:0] bits (Port x mode bits, pin 8) */
#define GPIO_CFGHR_MODE8_0                      ((uint32_t)0x00000001) /* Bit 0 */
#define GPIO_CFGHR_MODE8_1                      ((uint32_t)0x00000002) /* Bit 1 */

#define GPIO_CFGHR_MODE9                        ((uint32_t)0x00000030) /* MODE9[1:0] bits (Port x mode bits, pin 9) */
#define GPIO_CFGHR_MODE9_0                      ((uint32_t)0x00000010) /* Bit 0 */
#define GPIO_CFGHR_MODE9_1                      ((uint32_t)0x00000020) /* Bit 1 */

#define GPIO_CFGHR_MODE10                       ((uint32_t)0x00000300) /* MODE10[1:0] bits (Port x mode bits, pin 10) */
#define GPIO_CFGHR_MODE10_0                     ((uint32_t)0x00000100) /* Bit 0 */
#define GPIO_CFGHR_MODE10_1                     ((uint32_t)0x00000200) /* Bit 1 */

#define GPIO_CFGHR_MODE11                       ((uint32_t)0x00003000) /* MODE11[1:0] bits (Port x mode bits, pin 11) */
#define GPIO_CFGHR_MODE11_0                     ((uint32_t)0x00001000) /* Bit 0 */
#define GPIO_CFGHR_MODE11_1                     ((uint32_t)0x00002000) /* Bit 1 */

#define GPIO_CFGHR_MODE12                       ((uint32_t)0x00030000) /* MODE12[1:0] bits (Port x mode bits, pin 12) */
#define GPIO_CFGHR_MODE12_0                     ((uint32_t)0x00010000) /* Bit 0 */
#define GPIO_CFGHR_MODE12_1                     ((uint32_t)0x00020000) /* Bit 1 */

#define GPIO_CFGHR_MODE13                       ((uint32_t)0x00300000) /* MODE13[1:0] bits (Port x mode bits, pin 13) */
#define GPIO_CFGHR_MODE13_0                     ((uint32_t)0x00100000) /* Bit 0 */
#define GPIO_CFGHR_MODE13_1                     ((uint32_t)0x00200000) /* Bit 1 */

#define GPIO_CFGHR_MODE14                       ((uint32_t)0x03000000) /* MODE14[1:0] bits (Port x mode bits, pin 14) */
#define GPIO_CFGHR_MODE14_0                     ((uint32_t)0x01000000) /* Bit 0 */
#define GPIO_CFGHR_MODE14_1                     ((uint32_t)0x02000000) /* Bit 1 */

#define GPIO_CFGHR_MODE15                       ((uint32_t)0x30000000) /* MODE15[1:0] bits (Port x mode bits, pin 15) */
#define GPIO_CFGHR_MODE15_0                     ((uint32_t)0x10000000) /* Bit 0 */
#define GPIO_CFGHR_MODE15_1                     ((uint32_t)0x20000000) /* Bit 1 */

#define GPIO_CFGHR_CNF                          ((uint32_t)0xCCCCCCCC) /* Port x configuration bits */

#define GPIO_CFGHR_CNF8                         ((uint32_t)0x0000000C) /* CNF8[1:0] bits (Port x configuration bits, pin 8) */
#define GPIO_CFGHR_CNF8_0                       ((uint32_t)0x00000004) /* Bit 0 */
#define GPIO_CFGHR_CNF8_1                       ((uint32_t)0x00000008) /* Bit 1 */

#define GPIO_CFGHR_CNF9                         ((uint32_t)0x000000C0) /* CNF9[1:0] bits (Port x configuration bits, pin 9) */
#define GPIO_CFGHR_CNF9_0                       ((uint32_t)0x00000040) /* Bit 0 */
#define GPIO_CFGHR_CNF9_1                       ((uint32_t)0x00000080) /* Bit 1 */

#define GPIO_CFGHR_CNF10                        ((uint32_t)0x00000C00) /* CNF10[1:0] bits (Port x configuration bits, pin 10) */
#define GPIO_CFGHR_CNF10_0                      ((uint32_t)0x00000400) /* Bit 0 */
#define GPIO_CFGHR_CNF10_1                      ((uint32_t)0x00000800) /* Bit 1 */

#define GPIO_CFGHR_CNF11                        ((uint32_t)0x0000C000) /* CNF11[1:0] bits (Port x configuration bits, pin 11) */
#define GPIO_CFGHR_CNF11_0                      ((uint32_t)0x00004000) /* Bit 0 */
#define GPIO_CFGHR_CNF11_1                      ((uint32_t)0x00008000) /* Bit 1 */

#define GPIO_CFGHR_CNF12                        ((uint32_t)0x000C0000) /* CNF12[1:0] bits (Port x configuration bits, pin 12) */
#define GPIO_CFGHR_CNF12_0                      ((uint32_t)0x00040000) /* Bit 0 */
#define GPIO_CFGHR_CNF12_1                      ((uint32_t)0x00080000) /* Bit 1 */

#define GPIO_CFGHR_CNF13                        ((uint32_t)0x00C00000) /* CNF13[1:0] bits (Port x configuration bits, pin 13) */
#define GPIO_CFGHR_CNF13_0                      ((uint32_t)0x00400000) /* Bit 0 */
#define GPIO_CFGHR_CNF13_1                      ((uint32_t)0x00800000) /* Bit 1 */

#define GPIO_CFGHR_CNF14                        ((uint32_t)0x0C000000) /* CNF14[1:0] bits (Port x configuration bits, pin 14) */
#define GPIO_CFGHR_CNF14_0                      ((uint32_t)0x04000000) /* Bit 0 */
#define GPIO_CFGHR_CNF14_1                      ((uint32_t)0x08000000) /* Bit 1 */

#define GPIO_CFGHR_CNF15                        ((uint32_t)0xC0000000) /* CNF15[1:0] bits (Port x configuration bits, pin 15) */
#define GPIO_CFGHR_CNF15_0                      ((uint32_t)0x40000000) /* Bit 0 */
#define GPIO_CFGHR_CNF15_1                      ((uint32_t)0x80000000) /* Bit 1 */

/*******************  Bit definition for GPIO_INDR register  *******************/
#define GPIO_INDR_IDR0                          ((uint16_t)0x0001) /* Port input data, bit 0 */
#define GPIO_INDR_IDR1                          ((uint16_t)0x0002) /* Port input data, bit 1 */
#define GPIO_INDR_IDR2                          ((uint16_t)0x0004) /* Port input data, bit 2 */
#define GPIO_INDR_IDR3                          ((uint16_t)0x0008) /* Port input data, bit 3 */
#define GPIO_INDR_IDR4                          ((uint16_t)0x0010) /* Port input data, bit 4 */
#define GPIO_INDR_IDR5                          ((uint16_t)0x0020) /* Port input data, bit 5 */
#define GPIO_INDR_IDR6                          ((uint16_t)0x0040) /* Port input data, bit 6 */
#define GPIO_INDR_IDR7                          ((uint16_t)0x0080) /* Port input data, bit 7 */
#define GPIO_INDR_IDR8                          ((uint16_t)0x0100) /* Port input data, bit 8 */
#define GPIO_INDR_IDR9                          ((uint16_t)0x0200) /* Port input data, bit 9 */
#define GPIO_INDR_IDR10                         ((uint16_t)0x0400) /* Port input data, bit 10 */
#define GPIO_INDR_IDR11                         ((uint16_t)0x0800) /* Port input data, bit 11 */
#define GPIO_INDR_IDR12                         ((uint16_t)0x1000) /* Port input data, bit 12 */
#define GPIO_INDR_IDR13                         ((uint16_t)0x2000) /* Port input data, bit 13 */
#define GPIO_INDR_IDR14                         ((uint16_t)0x4000) /* Port input data, bit 14 */
#define GPIO_INDR_IDR15                         ((uint16_t)0x8000) /* Port input data, bit 15 */

/*******************  Bit definition for GPIO_OUTDR register  *******************/
#define GPIO_OUTDR_ODR0                         ((uint16_t)0x0001) /* Port output data, bit 0 */
#define GPIO_OUTDR_ODR1                         ((uint16_t)0x0002) /* Port output data, bit 1 */
#define GPIO_OUTDR_ODR2                         ((uint16_t)0x0004) /* Port output data, bit 2 */
#define GPIO_OUTDR_ODR3                         ((uint16_t)0x0008) /* Port output data, bit 3 */
#define GPIO_OUTDR_ODR4                         ((uint16_t)0x0010) /* Port output data, bit 4 */
#define GPIO_OUTDR_ODR5                         ((uint16_t)0x0020) /* Port output data, bit 5 */
#define GPIO_OUTDR_ODR6                         ((uint16_t)0x0040) /* Port output data, bit 6 */
#define GPIO_OUTDR_ODR7                         ((uint16_t)0x0080) /* Port output data, bit 7 */
#define GPIO_OUTDR_ODR8                         ((uint16_t)0x0100) /* Port output data, bit 8 */
#define GPIO_OUTDR_ODR9                         ((uint16_t)0x0200) /* Port output data, bit 9 */
#define GPIO_OUTDR_ODR10                        ((uint16_t)0x0400) /* Port output data, bit 10 */
#define GPIO_OUTDR_ODR11                        ((uint16_t)0x0800) /* Port output data, bit 11 */
#define GPIO_OUTDR_ODR12                        ((uint16_t)0x1000) /* Port output data, bit 12 */
#define GPIO_OUTDR_ODR13                        ((uint16_t)0x2000) /* Port output data, bit 13 */
#define GPIO_OUTDR_ODR14                        ((uint16_t)0x4000) /* Port output data, bit 14 */
#define GPIO_OUTDR_ODR15                        ((uint16_t)0x8000) /* Port output data, bit 15 */

/******************  Bit definition for GPIO_BSHR register  *******************/
#define GPIO_BSHR_BS0                           ((uint32_t)0x00000001) /* Port x Set bit 0 */
#define GPIO_BSHR_BS1                           ((uint32_t)0x00000002) /* Port x Set bit 1 */
#define GPIO_BSHR_BS2                           ((uint32_t)0x00000004) /* Port x Set bit 2 */
#define GPIO_BSHR_BS3                           ((uint32_t)0x00000008) /* Port x Set bit 3 */
#define GPIO_BSHR_BS4                           ((uint32_t)0x00000010) /* Port x Set bit 4 */
#define GPIO_BSHR_BS5                           ((uint32_t)0x00000020) /* Port x Set bit 5 */
#define GPIO_BSHR_BS6                           ((uint32_t)0x00000040) /* Port x Set bit 6 */
#define GPIO_BSHR_BS7                           ((uint32_t)0x00000080) /* Port x Set bit 7 */
#define GPIO_BSHR_BS8                           ((uint32_t)0x00000100) /* Port x Set bit 8 */
#define GPIO_BSHR_BS9                           ((uint32_t)0x00000200) /* Port x Set bit 9 */
#define GPIO_BSHR_BS10                          ((uint32_t)0x00000400) /* Port x Set bit 10 */
#define GPIO_BSHR_BS11                          ((uint32_t)0x00000800) /* Port x Set bit 11 */
#define GPIO_BSHR_BS12                          ((uint32_t)0x00001000) /* Port x Set bit 12 */
#define GPIO_BSHR_BS13                          ((uint32_t)0x00002000) /* Port x Set bit 13 */
#define GPIO_BSHR_BS14                          ((uint32_t)0x00004000) /* Port x Set bit 14 */
#define GPIO_BSHR_BS15                          ((uint32_t)0x00008000) /* Port x Set bit 15 */

#define GPIO_BSHR_BR0                           ((uint32_t)0x00010000) /* Port x Reset bit 0 */
#define GPIO_BSHR_BR1                           ((uint32_t)0x00020000) /* Port x Reset bit 1 */
#define GPIO_BSHR_BR2                           ((uint32_t)0x00040000) /* Port x Reset bit 2 */
#define GPIO_BSHR_BR3                           ((uint32_t)0x00080000) /* Port x Reset bit 3 */
#define GPIO_BSHR_BR4                           ((uint32_t)0x00100000) /* Port x Reset bit 4 */
#define GPIO_BSHR_BR5                           ((uint32_t)0x00200000) /* Port x Reset bit 5 */
#define GPIO_BSHR_BR6                           ((uint32_t)0x00400000) /* Port x Reset bit 6 */
#define GPIO_BSHR_BR7                           ((uint32_t)0x00800000) /* Port x Reset bit 7 */
#define GPIO_BSHR_BR8                           ((uint32_t)0x01000000) /* Port x Reset bit 8 */
#define GPIO_BSHR_BR9                           ((uint32_t)0x02000000) /* Port x Reset bit 9 */
#define GPIO_BSHR_BR10                          ((uint32_t)0x04000000) /* Port x Reset bit 10 */
#define GPIO_BSHR_BR11                          ((uint32_t)0x08000000) /* Port x Reset bit 11 */
#define GPIO_BSHR_BR12                          ((uint32_t)0x10000000) /* Port x Reset bit 12 */
#define GPIO_BSHR_BR13                          ((uint32_t)0x20000000) /* Port x Reset bit 13 */
#define GPIO_BSHR_BR14                          ((uint32_t)0x40000000) /* Port x Reset bit 14 */
#define GPIO_BSHR_BR15                          ((uint32_t)0x80000000) /* Port x Reset bit 15 */

/*******************  Bit definition for GPIO_BCR register  *******************/
#define GPIO_BCR_BR0                            ((uint16_t)0x0001) /* Port x Reset bit 0 */
#define GPIO_BCR_BR1                            ((uint16_t)0x0002) /* Port x Reset bit 1 */
#define GPIO_BCR_BR2                            ((uint16_t)0x0004) /* Port x Reset bit 2 */
#define GPIO_BCR_BR3                            ((uint16_t)0x0008) /* Port x Reset bit 3 */
#define GPIO_BCR_BR4                            ((uint16_t)0x0010) /* Port x Reset bit 4 */
#define GPIO_BCR_BR5                            ((uint16_t)0x0020) /* Port x Reset bit 5 */
#define GPIO_BCR_BR6                            ((uint16_t)0x0040) /* Port x Reset bit 6 */
#define GPIO_BCR_BR7                            ((uint16_t)0x0080) /* Port x Reset bit 7 */
#define GPIO_BCR_BR8                            ((uint16_t)0x0100) /* Port x Reset bit 8 */
#define GPIO_BCR_BR9                            ((uint16_t)0x0200) /* Port x Reset bit 9 */
#define GPIO_BCR_BR10                           ((uint16_t)0x0400) /* Port x Reset bit 10 */
#define GPIO_BCR_BR11                           ((uint16_t)0x0800) /* Port x Reset bit 11 */
#define GPIO_BCR_BR12                           ((uint16_t)0x1000) /* Port x Reset bit 12 */
#define GPIO_BCR_BR13                           ((uint16_t)0x2000) /* Port x Reset bit 13 */
#define GPIO_BCR_BR14                           ((uint16_t)0x4000) /* Port x Reset bit 14 */
#define GPIO_BCR_BR15                           ((uint16_t)0x8000) /* Port x Reset bit 15 */

/******************  Bit definition for GPIO_LCKR register  *******************/
#define GPIO_LCK0                               ((uint32_t)0x00000001) /* Port x Lock bit 0 */
#define GPIO_LCK1                               ((uint32_t)0x00000002) /* Port x Lock bit 1 */
#define GPIO_LCK2                               ((uint32_t)0x00000004) /* Port x Lock bit 2 */
#define GPIO_LCK3                               ((uint32_t)0x00000008) /* Port x Lock bit 3 */
#define GPIO_LCK4                               ((uint32_t)0x00000010) /* Port x Lock bit 4 */
#define GPIO_LCK5                               ((uint32_t)0x00000020) /* Port x Lock bit 5 */
#define GPIO_LCK6                               ((uint32_t)0x00000040) /* Port x Lock bit 6 */
#define GPIO_LCK7                               ((uint32_t)0x00000080) /* Port x Lock bit 7 */
#define GPIO_LCK8                               ((uint32_t)0x00000100) /* Port x Lock bit 8 */
#define GPIO_LCK9                               ((uint32_t)0x00000200) /* Port x Lock bit 9 */
#define GPIO_LCK10                              ((uint32_t)0x00000400) /* Port x Lock bit 10 */
#define GPIO_LCK11                              ((uint32_t)0x00000800) /* Port x Lock bit 11 */
#define GPIO_LCK12                              ((uint32_t)0x00001000) /* Port x Lock bit 12 */
#define GPIO_LCK13                              ((uint32_t)0x00002000) /* Port x Lock bit 13 */
#define GPIO_LCK14                              ((uint32_t)0x00004000) /* Port x Lock bit 14 */
#define GPIO_LCK15                              ((uint32_t)0x00008000) /* Port x Lock bit 15 */
#define GPIO_LCKK                               ((uint32_t)0x00010000) /* Lock key */

/******************  Bit definition for AFIO_ECR register  *******************/
#define AFIO_ECR_PIN                            ((uint8_t)0x0F) /* PIN[3:0] bits (Pin selection) */
#define AFIO_ECR_PIN_0                          ((uint8_t)0x01) /* Bit 0 */
#define AFIO_ECR_PIN_1                          ((uint8_t)0x02) /* Bit 1 */
#define AFIO_ECR_PIN_2                          ((uint8_t)0x04) /* Bit 2 */
#define AFIO_ECR_PIN_3                          ((uint8_t)0x08) /* Bit 3 */

#define AFIO_ECR_PIN_PX0                        ((uint8_t)0x00) /* Pin 0 selected */
#define AFIO_ECR_PIN_PX1                        ((uint8_t)0x01) /* Pin 1 selected */
#define AFIO_ECR_PIN_PX2                        ((uint8_t)0x02) /* Pin 2 selected */
#define AFIO_ECR_PIN_PX3                        ((uint8_t)0x03) /* Pin 3 selected */
#define AFIO_ECR_PIN_PX4                        ((uint8_t)0x04) /* Pin 4 selected */
#define AFIO_ECR_PIN_PX5                        ((uint8_t)0x05) /* Pin 5 selected */
#define AFIO_ECR_PIN_PX6                        ((uint8_t)0x06) /* Pin 6 selected */
#define AFIO_ECR_PIN_PX7                        ((uint8_t)0x07) /* Pin 7 selected */
#define AFIO_ECR_PIN_PX8                        ((uint8_t)0x08) /* Pin 8 selected */
#define AFIO_ECR_PIN_PX9                        ((uint8_t)0x09) /* Pin 9 selected */
#define AFIO_ECR_PIN_PX10                       ((uint8_t)0x0A) /* Pin 10 selected */
#define AFIO_ECR_PIN_PX11                       ((uint8_t)0x0B) /* Pin 11 selected */
#define AFIO_ECR_PIN_PX12                       ((uint8_t)0x0C) /* Pin 12 selected */
#define AFIO_ECR_PIN_PX13                       ((uint8_t)0x0D) /* Pin 13 selected */
#define AFIO_ECR_PIN_PX14                       ((uint8_t)0x0E) /* Pin 14 selected */
#define AFIO_ECR_PIN_PX15                       ((uint8_t)0x0F) /* Pin 15 selected */

#define AFIO_ECR_PORT                           ((uint8_t)0x70) /* PORT[2:0] bits (Port selection) */
#define AFIO_ECR_PORT_0                         ((uint8_t)0x10) /* Bit 0 */
#define AFIO_ECR_PORT_1                         ((uint8_t)0x20) /* Bit 1 */
#define AFIO_ECR_PORT_2                         ((uint8_t)0x40) /* Bit 2 */

#define AFIO_ECR_PORT_PA                        ((uint8_t)0x00) /* Port A selected */
#define AFIO_ECR_PORT_PB                        ((uint8_t)0x10) /* Port B selected */
#define AFIO_ECR_PORT_PC                        ((uint8_t)0x20) /* Port C selected */
#define AFIO_ECR_PORT_PD                        ((uint8_t)0x30) /* Port D selected */
#define AFIO_ECR_PORT_PE                        ((uint8_t)0x40) /* Port E selected */

#define AFIO_ECR_EVOE                           ((uint8_t)0x80) /* Event Output Enable */

/******************  Bit definition for AFIO_PCFR1register  *******************/
#define AFIO_PCFR1_SPI1_REMAP                   ((uint32_t)0x00000001) /* SPI1 remapping */
#define AFIO_PCFR1_I2C1_REMAP                   ((uint32_t)0x00000002) /* I2C1 remapping */
#define AFIO_PCFR1_USART1_REMAP                 ((uint32_t)0x00000004) /* USART1 remapping */
#define AFIO_PCFR1_USART1_REMAP_1               ((uint32_t)0x00200000) /* USART1 remapping higher bit */
#define AFIO_PCFR1_USART2_REMAP                 ((uint32_t)0x00000008) /* USART2 remapping */

#define AFIO_PCFR1_USART3_REMAP                 ((uint32_t)0x00000030) /* USART3_REMAP[1:0] bits (USART3 remapping) */
#define AFIO_PCFR1_USART3_REMAP_0               ((uint32_t)0x00000010) /* Bit 0 */
#define AFIO_PCFR1_USART3_REMAP_1               ((uint32_t)0x00000020) /* Bit 1 */

#define AFIO_PCFR1_USART3_REMAP_NOREMAP         ((uint32_t)0x00000000) /* No remap (TX/PB10, RX/PB11, CK/PB12, CTS/PB13, RTS/PB14) */
#define AFIO_PCFR1_USART3_REMAP_PARTIALREMAP    ((uint32_t)0x00000010) /* Partial remap (TX/PC10, RX/PC11, CK/PC12, CTS/PB13, RTS/PB14) */
#define AFIO_PCFR1_USART3_REMAP_FULLREMAP       ((uint32_t)0x00000030) /* Full remap (TX/PD8, RX/PD9, CK/PD10, CTS/PD11, RTS/PD12) */

#define AFIO_PCFR1_TIM1_REMAP                   ((uint32_t)0x000000C0) /* TIM1_REMAP[1:0] bits (TIM1 remapping) */
#define AFIO_PCFR1_TIM1_REMAP_0                 ((uint32_t)0x00000040) /* Bit 0 */
#define AFIO_PCFR1_TIM1_REMAP_1                 ((uint32_t)0x00000080) /* Bit 1 */

#define AFIO_PCFR1_TIM1_REMAP_NOREMAP           ((uint32_t)0x00000000) /* No remap (ETR/PA12, CH1/PA8, CH2/PA9, CH3/PA10, CH4/PA11, BKIN/PB12, CH1N/PB13, CH2N/PB14, CH3N/PB15) */
#define AFIO_PCFR1_TIM1_REMAP_PARTIALREMAP1     ((uint32_t)0x00000040) /* Partial remap (ETR/PA12, CH1/PA8, CH2/PA9, CH3/PA10, CH4/PA11, BKIN/PA6, CH1N/PA7, CH2N/PB0, CH3N/PB1) */
#define AFIO_PCFR1_TIM1_REMAP_PARTIALREMAP      AFIO_PCFR1_TIM1_REMAP_PARTIALREMAP1  /* legacy compatibility */
#define AFIO_PCFR1_TIM1_REMAP_PARTIALREMAP2     ((uint32_t)0x00000080) /* Partial remap (ETR/PD4, CH1/PD2, CH2/PA1, CH3/PC3, CH4/PC4, BKIN/PC2, CH1N/PD0, CN2N/PA2, CH3N/PD1) */
#define AFIO_PCFR1_TIM1_REMAP_FULLREMAP         ((uint32_t)0x000000C0) /* Full remap (ETR/PE7, CH1/PE9, CH2/PE11, CH3/PE13, CH4/PE14, BKIN/PE15, CH1N/PE8, CH2N/PE10, CH3N/PE12) */

#define AFIO_PCFR1_TIM2_REMAP                   ((uint32_t)0x00000300) /* TIM2_REMAP[1:0] bits (TIM2 remapping) */
#define AFIO_PCFR1_TIM2_REMAP_0                 ((uint32_t)0x00000100) /* Bit 0 */
#define AFIO_PCFR1_TIM2_REMAP_1                 ((uint32_t)0x00000200) /* Bit 1 */

#define AFIO_PCFR1_TIM2_REMAP_NOREMAP           ((uint32_t)0x00000000) /* No remap (CH1/ETR/PA0, CH2/PA1, CH3/PA2, CH4/PA3) */
#define AFIO_PCFR1_TIM2_REMAP_PARTIALREMAP1     ((uint32_t)0x00000100) /* Partial remap (CH1/ETR/PA15, CH2/PB3, CH3/PA2, CH4/PA3) */
#define AFIO_PCFR1_TIM2_REMAP_PARTIALREMAP2     ((uint32_t)0x00000200) /* Partial remap (CH1/ETR/PA0, CH2/PA1, CH3/PB10, CH4/PB11) */
#define AFIO_PCFR1_TIM2_REMAP_FULLREMAP         ((uint32_t)0x00000300) /* Full remap (CH1/ETR/PA15, CH2/PB3, CH3/PB10, CH4/PB11) */

#define AFIO_PCFR1_TIM3_REMAP                   ((uint32_t)0x00000C00) /* TIM3_REMAP[1:0] bits (TIM3 remapping) */
#define AFIO_PCFR1_TIM3_REMAP_0                 ((uint32_t)0x00000400) /* Bit 0 */
#define AFIO_PCFR1_TIM3_REMAP_1                 ((uint32_t)0x00000800) /* Bit 1 */

#define AFIO_PCFR1_TIM3_REMAP_NOREMAP           ((uint32_t)0x00000000) /* No remap (CH1/PA6, CH2/PA7, CH3/PB0, CH4/PB1) */
#define AFIO_PCFR1_TIM3_REMAP_PARTIALREMAP      ((uint32_t)0x00000800) /* Partial remap (CH1/PB4, CH2/PB5, CH3/PB0, CH4/PB1) */
#define AFIO_PCFR1_TIM3_REMAP_FULLREMAP         ((uint32_t)0x00000C00) /* Full remap (CH1/PC6, CH2/PC7, CH3/PC8, CH4/PC9) */

#define AFIO_PCFR1_TIM4_REMAP                   ((uint32_t)0x00001000) /* TIM4_REMAP bit (TIM4 remapping) */

#define AFIO_PCFR1_CAN_REMAP                    ((uint32_t)0x00006000) /* CAN_REMAP[1:0] bits (CAN Alternate function remapping) */
#define AFIO_PCFR1_CAN_REMAP_0                  ((uint32_t)0x00002000) /* Bit 0 */
#define AFIO_PCFR1_CAN_REMAP_1                  ((uint32_t)0x00004000) /* Bit 1 */

#define AFIO_PCFR1_CAN_REMAP_REMAP1             ((uint32_t)0x00000000) /* CANRX mapped to PA11, CANTX mapped to PA12 */
#define AFIO_PCFR1_CAN_REMAP_REMAP2             ((uint32_t)0x00004000) /* CANRX mapped to PB8, CANTX mapped to PB9 */
#define AFIO_PCFR1_CAN_REMAP_REMAP3             ((uint32_t)0x00006000) /* CANRX mapped to PD0, CANTX mapped to PD1 */

#define AFIO_PCFR1_PD01_REMAP                   ((uint32_t)0x00008000) /* Port D0/Port D1 mapping on OSC_IN/OSC_OUT */
#define AFIO_PCFR1_TIM5CH4_IREMAP               ((uint32_t)0x00010000) /* TIM5 Channel4 Internal Remap */
#define AFIO_PCFR1_ADC1_ETRGINJ_REMAP           ((uint32_t)0x00020000) /* ADC 1 External Trigger Injected Conversion remapping */
#define AFIO_PCFR1_ADC1_ETRGREG_REMAP           ((uint32_t)0x00040000) /* ADC 1 External Trigger Regular Conversion remapping */
#define AFIO_PCFR1_ADC2_ETRGINJ_REMAP           ((uint32_t)0x00080000) /* ADC 2 External Trigger Injected Conversion remapping */
#define AFIO_PCFR1_ADC2_ETRGREG_REMAP           ((uint32_t)0x00100000) /* ADC 2 External Trigger Regular Conversion remapping */

#define AFIO_PCFR1_SWJ_CFG                      ((uint32_t)0x07000000) /* SWJ_CFG[2:0] bits (Serial Wire JTAG configuration) */
#define AFIO_PCFR1_SWJ_CFG_0                    ((uint32_t)0x01000000) /* Bit 0 */
#define AFIO_PCFR1_SWJ_CFG_1                    ((uint32_t)0x02000000) /* Bit 1 */
#define AFIO_PCFR1_SWJ_CFG_2                    ((uint32_t)0x04000000) /* Bit 2 */

#define AFIO_PCFR1_SWJ_CFG_RESET                ((uint32_t)0x00000000) /* Full SWJ (JTAG-DP + SW-DP) : Reset State */
#define AFIO_PCFR1_SWJ_CFG_NOJNTRST             ((uint32_t)0x01000000) /* Full SWJ (JTAG-DP + SW-DP) but without JNTRST */
#define AFIO_PCFR1_SWJ_CFG_JTAGDISABLE          ((uint32_t)0x02000000) /* JTAG-DP Disabled and SW-DP Enabled */
#define AFIO_PCFR1_SWJ_CFG_DISABLE              ((uint32_t)0x04000000) /* JTAG-DP Disabled and SW-DP Disabled */



/*****************  Bit definition for AFIO_EXTICR1 register  *****************/
#define AFIO_EXTICR1_EXTI0                      ((uint16_t)0x000F) /* EXTI 0 configuration */
#define AFIO_EXTICR1_EXTI1                      ((uint16_t)0x00F0) /* EXTI 1 configuration */
#define AFIO_EXTICR1_EXTI2                      ((uint16_t)0x0F00) /* EXTI 2 configuration */
#define AFIO_EXTICR1_EXTI3                      ((uint16_t)0xF000) /* EXTI 3 configuration */

#define AFIO_EXTICR1_EXTI0_PA                   ((uint16_t)0x0000) /* PA[0] pin */
#define AFIO_EXTICR1_EXTI0_PB                   ((uint16_t)0x0001) /* PB[0] pin */
#define AFIO_EXTICR1_EXTI0_PC                   ((uint16_t)0x0002) /* PC[0] pin */
#define AFIO_EXTICR1_EXTI0_PD                   ((uint16_t)0x0003) /* PD[0] pin */
#define AFIO_EXTICR1_EXTI0_PE                   ((uint16_t)0x0004) /* PE[0] pin */
#define AFIO_EXTICR1_EXTI0_PF                   ((uint16_t)0x0005) /* PF[0] pin */
#define AFIO_EXTICR1_EXTI0_PG                   ((uint16_t)0x0006) /* PG[0] pin */

#define AFIO_EXTICR1_EXTI1_PA                   ((uint16_t)0x0000) /* PA[1] pin */
#define AFIO_EXTICR1_EXTI1_PB                   ((uint16_t)0x0010) /* PB[1] pin */
#define AFIO_EXTICR1_EXTI1_PC                   ((uint16_t)0x0020) /* PC[1] pin */
#define AFIO_EXTICR1_EXTI1_PD                   ((uint16_t)0x0030) /* PD[1] pin */
#define AFIO_EXTICR1_EXTI1_PE                   ((uint16_t)0x0040) /* PE[1] pin */
#define AFIO_EXTICR1_EXTI1_PF                   ((uint16_t)0x0050) /* PF[1] pin */
#define AFIO_EXTICR1_EXTI1_PG                   ((uint16_t)0x0060) /* PG[1] pin */

#define AFIO_EXTICR1_EXTI2_PA                   ((uint16_t)0x0000) /* PA[2] pin */
#define AFIO_EXTICR1_EXTI2_PB                   ((uint16_t)0x0100) /* PB[2] pin */
#define AFIO_EXTICR1_EXTI2_PC                   ((uint16_t)0x0200) /* PC[2] pin */
#define AFIO_EXTICR1_EXTI2_PD                   ((uint16_t)0x0300) /* PD[2] pin */
#define AFIO_EXTICR1_EXTI2_PE                   ((uint16_t)0x0400) /* PE[2] pin */
#define AFIO_EXTICR1_EXTI2_PF                   ((uint16_t)0x0500) /* PF[2] pin */
#define AFIO_EXTICR1_EXTI2_PG                   ((uint16_t)0x0600) /* PG[2] pin */

#define AFIO_EXTICR1_EXTI3_PA                   ((uint16_t)0x0000) /* PA[3] pin */
#define AFIO_EXTICR1_EXTI3_PB                   ((uint16_t)0x1000) /* PB[3] pin */
#define AFIO_EXTICR1_EXTI3_PC                   ((uint16_t)0x2000) /* PC[3] pin */
#define AFIO_EXTICR1_EXTI3_PD                   ((uint16_t)0x3000) /* PD[3] pin */
#define AFIO_EXTICR1_EXTI3_PE                   ((uint16_t)0x4000) /* PE[3] pin */
#define AFIO_EXTICR1_EXTI3_PF                   ((uint16_t)0x5000) /* PF[3] pin */
#define AFIO_EXTICR1_EXTI3_PG                   ((uint16_t)0x6000) /* PG[3] pin */

/*****************  Bit definition for AFIO_EXTICR2 register  *****************/
#define AFIO_EXTICR2_EXTI4                      ((uint16_t)0x000F) /* EXTI 4 configuration */
#define AFIO_EXTICR2_EXTI5                      ((uint16_t)0x00F0) /* EXTI 5 configuration */
#define AFIO_EXTICR2_EXTI6                      ((uint16_t)0x0F00) /* EXTI 6 configuration */
#define AFIO_EXTICR2_EXTI7                      ((uint16_t)0xF000) /* EXTI 7 configuration */

#define AFIO_EXTICR2_EXTI4_PA                   ((uint16_t)0x0000) /* PA[4] pin */
#define AFIO_EXTICR2_EXTI4_PB                   ((uint16_t)0x0001) /* PB[4] pin */
#define AFIO_EXTICR2_EXTI4_PC                   ((uint16_t)0x0002) /* PC[4] pin */
#define AFIO_EXTICR2_EXTI4_PD                   ((uint16_t)0x0003) /* PD[4] pin */
#define AFIO_EXTICR2_EXTI4_PE                   ((uint16_t)0x0004) /* PE[4] pin */
#define AFIO_EXTICR2_EXTI4_PF                   ((uint16_t)0x0005) /* PF[4] pin */
#define AFIO_EXTICR2_EXTI4_PG                   ((uint16_t)0x0006) /* PG[4] pin */

#define AFIO_EXTICR2_EXTI5_PA                   ((uint16_t)0x0000) /* PA[5] pin */
#define AFIO_EXTICR2_EXTI5_PB                   ((uint16_t)0x0010) /* PB[5] pin */
#define AFIO_EXTICR2_EXTI5_PC                   ((uint16_t)0x0020) /* PC[5] pin */
#define AFIO_EXTICR2_EXTI5_PD                   ((uint16_t)0x0030) /* PD[5] pin */
#define AFIO_EXTICR2_EXTI5_PE                   ((uint16_t)0x0040) /* PE[5] pin */
#define AFIO_EXTICR2_EXTI5_PF                   ((uint16_t)0x0050) /* PF[5] pin */
#define AFIO_EXTICR2_EXTI5_PG                   ((uint16_t)0x0060) /* PG[5] pin */

#define AFIO_EXTICR2_EXTI6_PA                   ((uint16_t)0x0000) /* PA[6] pin */
#define AFIO_EXTICR2_EXTI6_PB                   ((uint16_t)0x0100) /* PB[6] pin */
#define AFIO_EXTICR2_EXTI6_PC                   ((uint16_t)0x0200) /* PC[6] pin */
#define AFIO_EXTICR2_EXTI6_PD                   ((uint16_t)0x0300) /* PD[6] pin */
#define AFIO_EXTICR2_EXTI6_PE                   ((uint16_t)0x0400) /* PE[6] pin */
#define AFIO_EXTICR2_EXTI6_PF                   ((uint16_t)0x0500) /* PF[6] pin */
#define AFIO_EXTICR2_EXTI6_PG                   ((uint16_t)0x0600) /* PG[6] pin */

#define AFIO_EXTICR2_EXTI7_PA                   ((uint16_t)0x0000) /* PA[7] pin */
#define AFIO_EXTICR2_EXTI7_PB                   ((uint16_t)0x1000) /* PB[7] pin */
#define AFIO_EXTICR2_EXTI7_PC                   ((uint16_t)0x2000) /* PC[7] pin */
#define AFIO_EXTICR2_EXTI7_PD                   ((uint16_t)0x3000) /* PD[7] pin */
#define AFIO_EXTICR2_EXTI7_PE                   ((uint16_t)0x4000) /* PE[7] pin */
#define AFIO_EXTICR2_EXTI7_PF                   ((uint16_t)0x5000) /* PF[7] pin */
#define AFIO_EXTICR2_EXTI7_PG                   ((uint16_t)0x6000) /* PG[7] pin */

/*****************  Bit definition for AFIO_EXTICR3 register  *****************/
#define AFIO_EXTICR3_EXTI8                      ((uint16_t)0x000F) /* EXTI 8 configuration */
#define AFIO_EXTICR3_EXTI9                      ((uint16_t)0x00F0) /* EXTI 9 configuration */
#define AFIO_EXTICR3_EXTI10                     ((uint16_t)0x0F00) /* EXTI 10 configuration */
#define AFIO_EXTICR3_EXTI11                     ((uint16_t)0xF000) /* EXTI 11 configuration */

#define AFIO_EXTICR3_EXTI8_PA                   ((uint16_t)0x0000) /* PA[8] pin */
#define AFIO_EXTICR3_EXTI8_PB                   ((uint16_t)0x0001) /* PB[8] pin */
#define AFIO_EXTICR3_EXTI8_PC                   ((uint16_t)0x0002) /* PC[8] pin */
#define AFIO_EXTICR3_EXTI8_PD                   ((uint16_t)0x0003) /* PD[8] pin */
#define AFIO_EXTICR3_EXTI8_PE                   ((uint16_t)0x0004) /* PE[8] pin */
#define AFIO_EXTICR3_EXTI8_PF                   ((uint16_t)0x0005) /* PF[8] pin */
#define AFIO_EXTICR3_EXTI8_PG                   ((uint16_t)0x0006) /* PG[8] pin */

#define AFIO_EXTICR3_EXTI9_PA                   ((uint16_t)0x0000) /* PA[9] pin */
#define AFIO_EXTICR3_EXTI9_PB                   ((uint16_t)0x0010) /* PB[9] pin */
#define AFIO_EXTICR3_EXTI9_PC                   ((uint16_t)0x0020) /* PC[9] pin */
#define AFIO_EXTICR3_EXTI9_PD                   ((uint16_t)0x0030) /* PD[9] pin */
#define AFIO_EXTICR3_EXTI9_PE                   ((uint16_t)0x0040) /* PE[9] pin */
#define AFIO_EXTICR3_EXTI9_PF                   ((uint16_t)0x0050) /* PF[9] pin */
#define AFIO_EXTICR3_EXTI9_PG                   ((uint16_t)0x0060) /* PG[9] pin */

#define AFIO_EXTICR3_EXTI10_PA                  ((uint16_t)0x0000) /* PA[10] pin */
#define AFIO_EXTICR3_EXTI10_PB                  ((uint16_t)0x0100) /* PB[10] pin */
#define AFIO_EXTICR3_EXTI10_PC                  ((uint16_t)0x0200) /* PC[10] pin */
#define AFIO_EXTICR3_EXTI10_PD                  ((uint16_t)0x0300) /* PD[10] pin */
#define AFIO_EXTICR3_EXTI10_PE                  ((uint16_t)0x0400) /* PE[10] pin */
#define AFIO_EXTICR3_EXTI10_PF                  ((uint16_t)0x0500) /* PF[10] pin */
#define AFIO_EXTICR3_EXTI10_PG                  ((uint16_t)0x0600) /* PG[10] pin */

#define AFIO_EXTICR3_EXTI11_PA                  ((uint16_t)0x0000) /* PA[11] pin */
#define AFIO_EXTICR3_EXTI11_PB                  ((uint16_t)0x1000) /* PB[11] pin */
#define AFIO_EXTICR3_EXTI11_PC                  ((uint16_t)0x2000) /* PC[11] pin */
#define AFIO_EXTICR3_EXTI11_PD                  ((uint16_t)0x3000) /* PD[11] pin */
#define AFIO_EXTICR3_EXTI11_PE                  ((uint16_t)0x4000) /* PE[11] pin */
#define AFIO_EXTICR3_EXTI11_PF                  ((uint16_t)0x5000) /* PF[11] pin */
#define AFIO_EXTICR3_EXTI11_PG                  ((uint16_t)0x6000) /* PG[11] pin */

/*****************  Bit definition for AFIO_EXTICR4 register  *****************/
#define AFIO_EXTICR4_EXTI12                     ((uint16_t)0x000F) /* EXTI 12 configuration */
#define AFIO_EXTICR4_EXTI13                     ((uint16_t)0x00F0) /* EXTI 13 configuration */
#define AFIO_EXTICR4_EXTI14                     ((uint16_t)0x0F00) /* EXTI 14 configuration */
#define AFIO_EXTICR4_EXTI15                     ((uint16_t)0xF000) /* EXTI 15 configuration */

#define AFIO_EXTICR4_EXTI12_PA                  ((uint16_t)0x0000) /* PA[12] pin */
#define AFIO_EXTICR4_EXTI12_PB                  ((uint16_t)0x0001) /* PB[12] pin */
#define AFIO_EXTICR4_EXTI12_PC                  ((uint16_t)0x0002) /* PC[12] pin */
#define AFIO_EXTICR4_EXTI12_PD                  ((uint16_t)0x0003) /* PD[12] pin */
#define AFIO_EXTICR4_EXTI12_PE                  ((uint16_t)0x0004) /* PE[12] pin */
#define AFIO_EXTICR4_EXTI12_PF                  ((uint16_t)0x0005) /* PF[12] pin */
#define AFIO_EXTICR4_EXTI12_PG                  ((uint16_t)0x0006) /* PG[12] pin */

#define AFIO_EXTICR4_EXTI13_PA                  ((uint16_t)0x0000) /* PA[13] pin */
#define AFIO_EXTICR4_EXTI13_PB                  ((uint16_t)0x0010) /* PB[13] pin */
#define AFIO_EXTICR4_EXTI13_PC                  ((uint16_t)0x0020) /* PC[13] pin */
#define AFIO_EXTICR4_EXTI13_PD                  ((uint16_t)0x0030) /* PD[13] pin */
#define AFIO_EXTICR4_EXTI13_PE                  ((uint16_t)0x0040) /* PE[13] pin */
#define AFIO_EXTICR4_EXTI13_PF                  ((uint16_t)0x0050) /* PF[13] pin */
#define AFIO_EXTICR4_EXTI13_PG                  ((uint16_t)0x0060) /* PG[13] pin */

#define AFIO_EXTICR4_EXTI14_PA                  ((uint16_t)0x0000) /* PA[14] pin */
#define AFIO_EXTICR4_EXTI14_PB                  ((uint16_t)0x0100) /* PB[14] pin */
#define AFIO_EXTICR4_EXTI14_PC                  ((uint16_t)0x0200) /* PC[14] pin */
#define AFIO_EXTICR4_EXTI14_PD                  ((uint16_t)0x0300) /* PD[14] pin */
#define AFIO_EXTICR4_EXTI14_PE                  ((uint16_t)0x0400) /* PE[14] pin */
#define AFIO_EXTICR4_EXTI14_PF                  ((uint16_t)0x0500) /* PF[14] pin */
#define AFIO_EXTICR4_EXTI14_PG                  ((uint16_t)0x0600) /* PG[14] pin */

#define AFIO_EXTICR4_EXTI15_PA                  ((uint16_t)0x0000) /* PA[15] pin */
#define AFIO_EXTICR4_EXTI15_PB                  ((uint16_t)0x1000) /* PB[15] pin */
#define AFIO_EXTICR4_EXTI15_PC                  ((uint16_t)0x2000) /* PC[15] pin */
#define AFIO_EXTICR4_EXTI15_PD                  ((uint16_t)0x3000) /* PD[15] pin */
#define AFIO_EXTICR4_EXTI15_PE                  ((uint16_t)0x4000) /* PE[15] pin */
#define AFIO_EXTICR4_EXTI15_PF                  ((uint16_t)0x5000) /* PF[15] pin */
#define AFIO_EXTICR4_EXTI15_PG                  ((uint16_t)0x6000) /* PG[15] pin */

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
#define I2C_CTLR1_SMBUS                         ((uint16_t)0x0002) /* SMBus Mode */
#define I2C_CTLR1_SMBTYPE                       ((uint16_t)0x0008) /* SMBus Type */
#define I2C_CTLR1_ENARP                         ((uint16_t)0x0010) /* ARP Enable */
#define I2C_CTLR1_ENPEC                         ((uint16_t)0x0020) /* PEC Enable */
#define I2C_CTLR1_ENGC                          ((uint16_t)0x0040) /* General Call Enable */
#define I2C_CTLR1_NOSTRETCH                     ((uint16_t)0x0080) /* Clock Stretching Disable (Slave mode) */
#define I2C_CTLR1_START                         ((uint16_t)0x0100) /* Start Generation */
#define I2C_CTLR1_STOP                          ((uint16_t)0x0200) /* Stop Generation */
#define I2C_CTLR1_ACK                           ((uint16_t)0x0400) /* Acknowledge Enable */
#define I2C_CTLR1_POS                           ((uint16_t)0x0800) /* Acknowledge/PEC Position (for data reception) */
#define I2C_CTLR1_PEC                           ((uint16_t)0x1000) /* Packet Error Checking */
#define I2C_CTLR1_ALERT                         ((uint16_t)0x2000) /* SMBus Alert */
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
#define I2C_STAR1_TIMEOUT                       ((uint16_t)0x4000) /* Timeout or Tlow Error */
#define I2C_STAR1_SMBALERT                      ((uint16_t)0x8000) /* SMBus Alert */

/*******************  Bit definition for I2C_STAR2 register  ********************/
#define I2C_STAR2_MSL                           ((uint16_t)0x0001) /* Master/Slave */
#define I2C_STAR2_BUSY                          ((uint16_t)0x0002) /* Bus Busy */
#define I2C_STAR2_TRA                           ((uint16_t)0x0004) /* Transmitter/Receiver */
#define I2C_STAR2_GENCALL                       ((uint16_t)0x0010) /* General Call Address (Slave mode) */
#define I2C_STAR2_SMBDEFAULT                    ((uint16_t)0x0020) /* SMBus Device Default Address (Slave mode) */
#define I2C_STAR2_SMBHOST                       ((uint16_t)0x0040) /* SMBus Host Header (Slave mode) */
#define I2C_STAR2_DUALF                         ((uint16_t)0x0080) /* Dual Flag (Slave mode) */
#define I2C_STAR2_PEC                           ((uint16_t)0xFF00) /* Packet Error Checking Register */

/*******************  Bit definition for I2C_CKCFGR register  ********************/
#define I2C_CKCFGR_CCR                          ((uint16_t)0x0FFF) /* Clock Control Register in Fast/Standard mode (Master mode) */
#define I2C_CKCFGR_DUTY                         ((uint16_t)0x4000) /* Fast Mode Duty Cycle */
#define I2C_CKCFGR_FS                           ((uint16_t)0x8000) /* I2C Master Mode Selection */

/******************  Bit definition for I2C_RTR register  *******************/
#define I2C_RTR_TRISE                           ((uint8_t)0x3F) /* Maximum Rise Time in Fast/Standard mode (Master mode) */

/******************************************************************************/
/*                             Power Control                                  */
/******************************************************************************/

/********************  Bit definition for PWR_CTLR register  ********************/
#define PWR_CTLR_LPDS                           ((uint16_t)0x0001) /* Low-Power Deepsleep */
#define PWR_CTLR_PDDS                           ((uint16_t)0x0002) /* Power Down Deepsleep */
#define PWR_CTLR_CWUF                           ((uint16_t)0x0004) /* Clear Wakeup Flag */
#define PWR_CTLR_CSBF                           ((uint16_t)0x0008) /* Clear Standby Flag */
#define PWR_CTLR_PVDE                           ((uint16_t)0x0010) /* Power Voltage Detector Enable */

#define PWR_CTLR_PLS                            ((uint16_t)0x00E0) /* PLS[2:0] bits (PVD Level Selection) */
#define PWR_CTLR_PLS_0                          ((uint16_t)0x0020) /* Bit 0 */
#define PWR_CTLR_PLS_1                          ((uint16_t)0x0040) /* Bit 1 */
#define PWR_CTLR_PLS_2                          ((uint16_t)0x0080) /* Bit 2 */

#define PWR_CTLR_PLS_2V2                        ((uint16_t)0x0000) /* PVD level 2.2V */
#define PWR_CTLR_PLS_2V3                        ((uint16_t)0x0020) /* PVD level 2.3V */
#define PWR_CTLR_PLS_2V4                        ((uint16_t)0x0040) /* PVD level 2.4V */
#define PWR_CTLR_PLS_2V5                        ((uint16_t)0x0060) /* PVD level 2.5V */
#define PWR_CTLR_PLS_2V6                        ((uint16_t)0x0080) /* PVD level 2.6V */
#define PWR_CTLR_PLS_2V7                        ((uint16_t)0x00A0) /* PVD level 2.7V */
#define PWR_CTLR_PLS_2V8                        ((uint16_t)0x00C0) /* PVD level 2.8V */
#define PWR_CTLR_PLS_2V9                        ((uint16_t)0x00E0) /* PVD level 2.9V */

#define PWR_CTLR_DBP                            ((uint16_t)0x0100) /* Disable Backup Domain write protection */

/*******************  Bit definition for PWR_CSR register  ********************/
#define PWR_CSR_WUF                             ((uint16_t)0x0001) /* Wakeup Flag */
#define PWR_CSR_SBF                             ((uint16_t)0x0002) /* Standby Flag */
#define PWR_CSR_PVDO                            ((uint16_t)0x0004) /* PVD Output */
#define PWR_CSR_EWUP                            ((uint16_t)0x0100) /* Enable WKUP pin */

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

#define RCC_HPRE_DIV1                   		((uint32_t)0x00000000) /* SYSCLK not divided */
#define RCC_HPRE_DIV2                   		((uint32_t)0x00000080) /* SYSCLK divided by 2 */
#define RCC_HPRE_DIV4                   		((uint32_t)0x00000090) /* SYSCLK divided by 4 */
#define RCC_HPRE_DIV8                   		((uint32_t)0x000000A0) /* SYSCLK divided by 8 */
#define RCC_HPRE_DIV16                  		((uint32_t)0x000000B0) /* SYSCLK divided by 16 */
#define RCC_HPRE_DIV64                  		((uint32_t)0x000000C0) /* SYSCLK divided by 64 */
#define RCC_HPRE_DIV128                 		((uint32_t)0x000000D0) /* SYSCLK divided by 128 */
#define RCC_HPRE_DIV256                 		((uint32_t)0x000000E0) /* SYSCLK divided by 256 */
#define RCC_HPRE_DIV512                 		((uint32_t)0x000000F0) /* SYSCLK divided by 512 */

#define RCC_PPRE1                               ((uint32_t)0x00000700) /* PRE1[2:0] bits (APB1 prescaler) */
#define RCC_PPRE1_0                             ((uint32_t)0x00000100) /* Bit 0 */
#define RCC_PPRE1_1                             ((uint32_t)0x00000200) /* Bit 1 */
#define RCC_PPRE1_2                             ((uint32_t)0x00000400) /* Bit 2 */

#define RCC_PPRE1_DIV1                          ((uint32_t)0x00000000) /* HCLK not divided */
#define RCC_PPRE1_DIV2                          ((uint32_t)0x00000400) /* HCLK divided by 2 */
#define RCC_PPRE1_DIV4                          ((uint32_t)0x00000500) /* HCLK divided by 4 */
#define RCC_PPRE1_DIV8                          ((uint32_t)0x00000600) /* HCLK divided by 8 */
#define RCC_PPRE1_DIV16                         ((uint32_t)0x00000700) /* HCLK divided by 16 */

#define RCC_PPRE2                               ((uint32_t)0x00003800) /* PRE2[2:0] bits (APB2 prescaler) */
#define RCC_PPRE2_0                             ((uint32_t)0x00000800) /* Bit 0 */
#define RCC_PPRE2_1                             ((uint32_t)0x00001000) /* Bit 1 */
#define RCC_PPRE2_2                             ((uint32_t)0x00002000) /* Bit 2 */

#define RCC_PPRE2_DIV1                          ((uint32_t)0x00000000) /* HCLK not divided */
#define RCC_PPRE2_DIV2                          ((uint32_t)0x00002000) /* HCLK divided by 2 */
#define RCC_PPRE2_DIV4                          ((uint32_t)0x00002800) /* HCLK divided by 4 */
#define RCC_PPRE2_DIV8                          ((uint32_t)0x00003000) /* HCLK divided by 8 */
#define RCC_PPRE2_DIV16                         ((uint32_t)0x00003800) /* HCLK divided by 16 */

#define RCC_ADCPRE                              ((uint32_t)0x0000C000) /* ADCPRE[1:0] bits (ADC prescaler) */
#define RCC_ADCPRE_0                            ((uint32_t)0x00004000) /* Bit 0 */
#define RCC_ADCPRE_1                            ((uint32_t)0x00008000) /* Bit 1 */

#define RCC_ADCPRE_DIV2                         ((uint32_t)0x00000000) /* PCLK2 divided by 2 */
#define RCC_ADCPRE_DIV4                         ((uint32_t)0x00004000) /* PCLK2 divided by 4 */
#define RCC_ADCPRE_DIV6                         ((uint32_t)0x00008000) /* PCLK2 divided by 6 */
#define RCC_ADCPRE_DIV8                         ((uint32_t)0x0000C000) /* PCLK2 divided by 8 */

#define RCC_PLLSRC                              ((uint32_t)0x00010000) /* PLL entry clock source */

#define RCC_PLLXTPRE                            ((uint32_t)0x00020000) /* HSE divider for PLL entry */

#define RCC_PLLMULL                             ((uint32_t)0x003C0000) /* PLLMUL[3:0] bits (PLL multiplication factor) */
#define RCC_PLLMULL_0                           ((uint32_t)0x00040000) /* Bit 0 */
#define RCC_PLLMULL_1                           ((uint32_t)0x00080000) /* Bit 1 */
#define RCC_PLLMULL_2                           ((uint32_t)0x00100000) /* Bit 2 */
#define RCC_PLLMULL_3                           ((uint32_t)0x00200000) /* Bit 3 */

#define RCC_PLLSRC_HSI_Div2                     ((uint32_t)0x00000000) /* HSI clock divided by 2 selected as PLL entry clock source */
#define RCC_PLLSRC_HSE                          ((uint32_t)0x00010000) /* HSE clock selected as PLL entry clock source */

#define RCC_PLLXTPRE_HSE                        ((uint32_t)0x00000000) /* HSE clock not divided for PLL entry */
#define RCC_PLLXTPRE_HSE_Div2                   ((uint32_t)0x00020000) /* HSE clock divided by 2 for PLL entry */

#define RCC_PLLMULL2                            ((uint32_t)0x00000000) /* PLL input clock*2 */
#define RCC_PLLMULL3                            ((uint32_t)0x00040000) /* PLL input clock*3 */
#define RCC_PLLMULL4                            ((uint32_t)0x00080000) /* PLL input clock*4 */
#define RCC_PLLMULL5                            ((uint32_t)0x000C0000) /* PLL input clock*5 */
#define RCC_PLLMULL6                            ((uint32_t)0x00100000) /* PLL input clock*6 */
#define RCC_PLLMULL7                            ((uint32_t)0x00140000) /* PLL input clock*7 */
#define RCC_PLLMULL8                            ((uint32_t)0x00180000) /* PLL input clock*8 */
#define RCC_PLLMULL9                            ((uint32_t)0x001C0000) /* PLL input clock*9 */
#define RCC_PLLMULL10                           ((uint32_t)0x00200000) /* PLL input clock10 */
#define RCC_PLLMULL11                           ((uint32_t)0x00240000) /* PLL input clock*11 */
#define RCC_PLLMULL12                           ((uint32_t)0x00280000) /* PLL input clock*12 */
#define RCC_PLLMULL13                           ((uint32_t)0x002C0000) /* PLL input clock*13 */
#define RCC_PLLMULL14                           ((uint32_t)0x00300000) /* PLL input clock*14 */
#define RCC_PLLMULL15                           ((uint32_t)0x00340000) /* PLL input clock*15 */
#define RCC_PLLMULL16                           ((uint32_t)0x00380000) /* PLL input clock*16 */


#define RCC_USBPRE                              ((uint32_t)0x00400000) /* USB Device prescaler */

#define RCC_CFGR0_MCO                           ((uint32_t)0x07000000) /* MCO[2:0] bits (Microcontroller Clock Output) */
#define RCC_MCO_0                               ((uint32_t)0x01000000) /* Bit 0 */
#define RCC_MCO_1                               ((uint32_t)0x02000000) /* Bit 1 */
#define RCC_MCO_2                               ((uint32_t)0x04000000) /* Bit 2 */

#define RCC_MCO_NOCLOCK                         ((uint32_t)0x00000000) /* No clock */
#define RCC_CFGR0_MCO_SYSCLK                    ((uint32_t)0x04000000) /* System clock selected as MCO source */
#define RCC_CFGR0_MCO_HSI                       ((uint32_t)0x05000000) /* HSI clock selected as MCO source */
#define RCC_CFGR0_MCO_HSE                       ((uint32_t)0x06000000) /* HSE clock selected as MCO source  */
#define RCC_CFGR0_MCO_PLL                       ((uint32_t)0x07000000) /* PLL clock divided by 2 selected as MCO source */

/*******************  Bit definition for RCC_CFGR2 register  *******************/

/*******************  Bit definition for RCC_INTR register  ********************/
#define RCC_LSIRDYF                             ((uint32_t)0x00000001) /* LSI Ready Interrupt flag */
#define RCC_LSERDYF                             ((uint32_t)0x00000002) /* LSE Ready Interrupt flag */
#define RCC_HSIRDYF                             ((uint32_t)0x00000004) /* HSI Ready Interrupt flag */
#define RCC_HSERDYF                             ((uint32_t)0x00000008) /* HSE Ready Interrupt flag */
#define RCC_PLLRDYF                             ((uint32_t)0x00000010) /* PLL Ready Interrupt flag */
#define RCC_CSSF                                ((uint32_t)0x00000080) /* Clock Security System Interrupt flag */
#define RCC_LSIRDYIE                            ((uint32_t)0x00000100) /* LSI Ready Interrupt Enable */
#define RCC_LSERDYIE                            ((uint32_t)0x00000200) /* LSE Ready Interrupt Enable */
#define RCC_HSIRDYIE                            ((uint32_t)0x00000400) /* HSI Ready Interrupt Enable */
#define RCC_HSERDYIE                            ((uint32_t)0x00000800) /* HSE Ready Interrupt Enable */
#define RCC_PLLRDYIE                            ((uint32_t)0x00001000) /* PLL Ready Interrupt Enable */
#define RCC_LSIRDYC                             ((uint32_t)0x00010000) /* LSI Ready Interrupt Clear */
#define RCC_LSERDYC                             ((uint32_t)0x00020000) /* LSE Ready Interrupt Clear */
#define RCC_HSIRDYC                             ((uint32_t)0x00040000) /* HSI Ready Interrupt Clear */
#define RCC_HSERDYC                             ((uint32_t)0x00080000) /* HSE Ready Interrupt Clear */
#define RCC_PLLRDYC                             ((uint32_t)0x00100000) /* PLL Ready Interrupt Clear */
#define RCC_CSSC                                ((uint32_t)0x00800000) /* Clock Security System Interrupt Clear */

/*****************  Bit definition for RCC_APB2PRSTR register  *****************/
#define RCC_AFIORST                             ((uint32_t)0x00000001) /* Alternate Function I/O reset */
#define RCC_IOPARST                             ((uint32_t)0x00000004) /* I/O port A reset */
#define RCC_IOPBRST                             ((uint32_t)0x00000008) /* I/O port B reset */
#define RCC_IOPCRST                             ((uint32_t)0x00000010) /* I/O port C reset */
#define RCC_IOPDRST                             ((uint32_t)0x00000020) /* I/O port D reset */
#define RCC_ADC1RST                             ((uint32_t)0x00000200) /* ADC 1 interface reset */

#define RCC_ADC2RST                             ((uint32_t)0x00000400) /* ADC 2 interface reset */

#define RCC_TIM1RST                             ((uint32_t)0x00000800) /* TIM1 Timer reset */
#define RCC_SPI1RST                             ((uint32_t)0x00001000) /* SPI 1 reset */
#define RCC_USART1RST                           ((uint32_t)0x00004000) /* USART1 reset */

#define RCC_IOPERST                             ((uint32_t)0x00000040) /* I/O port E reset */

/*****************  Bit definition for RCC_APB1PRSTR register  *****************/
#define RCC_TIM2RST                             ((uint32_t)0x00000001) /* Timer 2 reset */
#define RCC_TIM3RST                             ((uint32_t)0x00000002) /* Timer 3 reset */
#define RCC_WWDGRST                             ((uint32_t)0x00000800) /* Window Watchdog reset */
#define RCC_USART2RST                           ((uint32_t)0x00020000) /* USART 2 reset */
#define RCC_I2C1RST                             ((uint32_t)0x00200000) /* I2C 1 reset */

#define RCC_CAN1RST                             ((uint32_t)0x02000000) /* CAN1 reset */

#define RCC_BKPRST                              ((uint32_t)0x08000000) /* Backup interface reset */
#define RCC_PWRRST                              ((uint32_t)0x10000000) /* Power interface reset */

#define RCC_TIM4RST                             ((uint32_t)0x00000004) /* Timer 4 reset */
#define RCC_SPI2RST                             ((uint32_t)0x00004000) /* SPI 2 reset */
#define RCC_USART3RST                           ((uint32_t)0x00040000) /* USART 3 reset */
#define RCC_I2C2RST                             ((uint32_t)0x00400000) /* I2C 2 reset */

#define RCC_USBRST                              ((uint32_t)0x00800000) /* USB Device reset */

/******************  Bit definition for RCC_AHBPCENR register  ******************/
#define RCC_DMA1EN                              ((uint32_t)0x0001) /* DMA1 clock enable */
#define RCC_SRAMEN                              ((uint32_t)0x0004) /* SRAM interface clock enable */
#define RCC_FLITFEN                             ((uint32_t)0x0010) /* FLITF clock enable */
#define RCC_CRCEN                               ((uint32_t)0x0040) /* CRC clock enable */
#define RCC_USBHD                               ((uint32_t)0x1000)
#define RCC_USBFS                               ((uint32_t)0x1000)
#define RCC_USBPD                               ((uint32_t)0x20000)

/******************  Bit definition for RCC_APB2PCENR register  *****************/
#define RCC_AFIOEN                              ((uint32_t)0x00000001) /* Alternate Function I/O clock enable */
#define RCC_IOPAEN                              ((uint32_t)0x00000004) /* I/O port A clock enable */
#define RCC_IOPBEN                              ((uint32_t)0x00000008) /* I/O port B clock enable */
#define RCC_IOPCEN                              ((uint32_t)0x00000010) /* I/O port C clock enable */
#define RCC_IOPDEN                              ((uint32_t)0x00000020) /* I/O port D clock enable */
#define RCC_ADC1EN                              ((uint32_t)0x00000200) /* ADC 1 interface clock enable */

#define RCC_ADC2EN                              ((uint32_t)0x00000400) /* ADC 2 interface clock enable */

#define RCC_TIM1EN                              ((uint32_t)0x00000800) /* TIM1 Timer clock enable */
#define RCC_SPI1EN                              ((uint32_t)0x00001000) /* SPI 1 clock enable */
#define RCC_USART1EN                            ((uint32_t)0x00004000) /* USART1 clock enable */

/*****************  Bit definition for RCC_APB1PCENR register  ******************/
#define RCC_TIM2EN                              ((uint32_t)0x00000001) /* Timer 2 clock enabled*/
#define RCC_TIM3EN                              ((uint32_t)0x00000002) /* Timer 3 clock enable */
#define RCC_WWDGEN                              ((uint32_t)0x00000800) /* Window Watchdog clock enable */
#define RCC_USART2EN                            ((uint32_t)0x00020000) /* USART 2 clock enable */
#define RCC_I2C1EN                              ((uint32_t)0x00200000) /* I2C 1 clock enable */

#define RCC_BKPEN                               ((uint32_t)0x08000000) /* Backup interface clock enable */
#define RCC_PWREN                               ((uint32_t)0x10000000) /* Power interface clock enable */

#define RCC_USBEN                               ((uint32_t)0x00800000) /* USB Device clock enable */

/*******************  Bit definition for RCC_BDCTLR register  *******************/
#define RCC_LSEON                               ((uint32_t)0x00000001) /* External Low Speed oscillator enable */
#define RCC_LSERDY                              ((uint32_t)0x00000002) /* External Low Speed oscillator Ready */
#define RCC_LSEBYP                              ((uint32_t)0x00000004) /* External Low Speed oscillator Bypass */

#define RCC_RTCSEL                              ((uint32_t)0x00000300) /* RTCSEL[1:0] bits (RTC clock source selection) */
#define RCC_RTCSEL_0                            ((uint32_t)0x00000100) /* Bit 0 */
#define RCC_RTCSEL_1                            ((uint32_t)0x00000200) /* Bit 1 */

#define RCC_RTCSEL_NOCLOCK                      ((uint32_t)0x00000000) /* No clock */
#define RCC_RTCSEL_LSE                          ((uint32_t)0x00000100) /* LSE oscillator clock used as RTC clock */
#define RCC_RTCSEL_LSI                          ((uint32_t)0x00000200) /* LSI oscillator clock used as RTC clock */
#define RCC_RTCSEL_HSE                          ((uint32_t)0x00000300) /* HSE oscillator clock divided by 128 used as RTC clock */

#define RCC_RTCEN                               ((uint32_t)0x00008000) /* RTC clock enable */
#define RCC_BDRST                               ((uint32_t)0x00010000) /* Backup domain software reset  */

/*******************  Bit definition for RCC_RSTSCKR register  ********************/
#define RCC_LSION                               ((uint32_t)0x00000001) /* Internal Low Speed oscillator enable */
#define RCC_LSIRDY                              ((uint32_t)0x00000002) /* Internal Low Speed oscillator Ready */
#define RCC_RMVF                                ((uint32_t)0x01000000) /* Remove reset flag */
#define RCC_PINRSTF                             ((uint32_t)0x04000000) /* PIN reset flag */
#define RCC_PORRSTF                             ((uint32_t)0x08000000) /* POR/PDR reset flag */
#define RCC_SFTRSTF                             ((uint32_t)0x10000000) /* Software Reset flag */
#define RCC_IWDGRSTF                            ((uint32_t)0x20000000) /* Independent Watchdog reset flag */
#define RCC_WWDGRSTF                            ((uint32_t)0x40000000) /* Window watchdog reset flag */
#define RCC_LPWRRSTF                            ((uint32_t)0x80000000) /* Low-Power reset flag */

/******************  Bit definition for RCC_AHBRSTR register  *****************/



/******************************************************************************/
/*                             Real-Time Clock                                */
/******************************************************************************/

/*******************  Bit definition for RTC_CTLRH register  ********************/
#define RTC_CTLRH_SECIE                         ((uint8_t)0x01) /* Second Interrupt Enable */
#define RTC_CTLRH_ALRIE                         ((uint8_t)0x02) /* Alarm Interrupt Enable */
#define RTC_CTLRH_OWIE                          ((uint8_t)0x04) /* OverfloW Interrupt Enable */

/*******************  Bit definition for RTC_CTLRL register  ********************/
#define RTC_CTLRL_SECF                          ((uint8_t)0x01) /* Second Flag */
#define RTC_CTLRL_ALRF                          ((uint8_t)0x02) /* Alarm Flag */
#define RTC_CTLRL_OWF                           ((uint8_t)0x04) /* OverfloW Flag */
#define RTC_CTLRL_RSF                           ((uint8_t)0x08) /* Registers Synchronized Flag */
#define RTC_CTLRL_CNF                           ((uint8_t)0x10) /* Configuration Flag */
#define RTC_CTLRL_RTOFF                         ((uint8_t)0x20) /* RTC operation OFF */

/*******************  Bit definition for RTC_PSCH register  *******************/
#define RTC_PSCH_PRL                            ((uint16_t)0x000F) /* RTC Prescaler Reload Value High */

/*******************  Bit definition for RTC_PRLL register  *******************/
#define RTC_PSCL_PRL                            ((uint16_t)0xFFFF) /* RTC Prescaler Reload Value Low */

/*******************  Bit definition for RTC_DIVH register  *******************/
#define RTC_DIVH_RTC_DIV                        ((uint16_t)0x000F) /* RTC Clock Divider High */

/*******************  Bit definition for RTC_DIVL register  *******************/
#define RTC_DIVL_RTC_DIV                        ((uint16_t)0xFFFF) /* RTC Clock Divider Low */

/*******************  Bit definition for RTC_CNTH register  *******************/
#define RTC_CNTH_RTC_CNT                        ((uint16_t)0xFFFF) /* RTC Counter High */

/*******************  Bit definition for RTC_CNTL register  *******************/
#define RTC_CNTL_RTC_CNT                        ((uint16_t)0xFFFF) /* RTC Counter Low */

/*******************  Bit definition for RTC_ALRMH register  *******************/
#define RTC_ALRMH_RTC_ALRM                      ((uint16_t)0xFFFF) /* RTC Alarm High */

/*******************  Bit definition for RTC_ALRML register  *******************/
#define RTC_ALRML_RTC_ALRM                      ((uint16_t)0xFFFF) /* RTC Alarm Low */

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
#define SPI_CTLR1_LSBFIRST                      ((uint16_t)0x0080) /* Frame Format */
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
#define SPI_STATR_CHSIDE                        ((uint8_t)0x04) /* Channel side */
#define SPI_STATR_UDR                           ((uint8_t)0x08) /* Underrun flag */
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

/******************  Bit definition for SPI_HSCR register  *****************/
#define SPI_HSCR_HSRXEN                         ((uint16_t)0x0001) 

/******************************************************************************/
/*                                    TIM                                     */
/******************************************************************************/

/*******************  Bit definition for TIM_CTLR1 register  ********************/
#define TIM_CEN                                 ((uint16_t)0x0001) /* Counter enable */
#define TIM_UDIS                                ((uint16_t)0x0002) /* Update disable */
#define TIM_URS                                 ((uint16_t)0x0004) /* Update request source */
#define TIM_OPM                                 ((uint16_t)0x0008) /* One pulse mode */
#define TIM_DIR                                 ((uint16_t)0x0010) /* Direction */

#define TIM_CMS                                 ((uint16_t)0x0060) /* CMS[1:0] bits (Center-aligned mode selection) */
#define TIM_CMS_0                               ((uint16_t)0x0020) /* Bit 0 */
#define TIM_CMS_1                               ((uint16_t)0x0040) /* Bit 1 */

#define TIM_ARPE                                ((uint16_t)0x0080) /* Auto-reload preload enable */

#define TIM_CTLR1_CKD                           ((uint16_t)0x0300) /* CKD[1:0] bits (clock division) */
#define TIM_CKD_0                               ((uint16_t)0x0100) /* Bit 0 */
#define TIM_CKD_1                               ((uint16_t)0x0200) /* Bit 1 */

/*******************  Bit definition for TIM_CTLR2 register  ********************/
#define TIM_CCPC                                ((uint16_t)0x0001) /* Capture/Compare Preloaded Control */
#define TIM_CCUS                                ((uint16_t)0x0004) /* Capture/Compare Control Update Selection */
#define TIM_CCDS                                ((uint16_t)0x0008) /* Capture/Compare DMA Selection */

#define TIM_MMS                                 ((uint16_t)0x0070) /* MMS[2:0] bits (Master Mode Selection) */
#define TIM_MMS_0                               ((uint16_t)0x0010) /* Bit 0 */
#define TIM_MMS_1                               ((uint16_t)0x0020) /* Bit 1 */
#define TIM_MMS_2                               ((uint16_t)0x0040) /* Bit 2 */

#define TIM_TI1S                                ((uint16_t)0x0080) /* TI1 Selection */
#define TIM_OIS1                                ((uint16_t)0x0100) /* Output Idle state 1 (OC1 output) */
#define TIM_OIS1N                               ((uint16_t)0x0200) /* Output Idle state 1 (OC1N output) */
#define TIM_OIS2                                ((uint16_t)0x0400) /* Output Idle state 2 (OC2 output) */
#define TIM_OIS2N                               ((uint16_t)0x0800) /* Output Idle state 2 (OC2N output) */
#define TIM_OIS3                                ((uint16_t)0x1000) /* Output Idle state 3 (OC3 output) */
#define TIM_OIS3N                               ((uint16_t)0x2000) /* Output Idle state 3 (OC3N output) */
#define TIM_OIS4                                ((uint16_t)0x4000) /* Output Idle state 4 (OC4 output) */

/*******************  Bit definition for TIM_SMCFGR register  *******************/
#define TIM_SMS                                 ((uint16_t)0x0007) /* SMS[2:0] bits (Slave mode selection) */
#define TIM_SMS_0                               ((uint16_t)0x0001) /* Bit 0 */
#define TIM_SMS_1                               ((uint16_t)0x0002) /* Bit 1 */
#define TIM_SMS_2                               ((uint16_t)0x0004) /* Bit 2 */

#define TIM_TS                                  ((uint16_t)0x0070) /* TS[2:0] bits (Trigger selection) */
#define TIM_TS_0                                ((uint16_t)0x0010) /* Bit 0 */
#define TIM_TS_1                                ((uint16_t)0x0020) /* Bit 1 */
#define TIM_TS_2                                ((uint16_t)0x0040) /* Bit 2 */

#define TIM_MSM                                 ((uint16_t)0x0080) /* Master/slave mode */

#define TIM_ETF                                 ((uint16_t)0x0F00) /* ETF[3:0] bits (External trigger filter) */
#define TIM_ETF_0                               ((uint16_t)0x0100) /* Bit 0 */
#define TIM_ETF_1                               ((uint16_t)0x0200) /* Bit 1 */
#define TIM_ETF_2                               ((uint16_t)0x0400) /* Bit 2 */
#define TIM_ETF_3                               ((uint16_t)0x0800) /* Bit 3 */

#define TIM_ETPS                                ((uint16_t)0x3000) /* ETPS[1:0] bits (External trigger prescaler) */
#define TIM_ETPS_0                              ((uint16_t)0x1000) /* Bit 0 */
#define TIM_ETPS_1                              ((uint16_t)0x2000) /* Bit 1 */

#define TIM_ECE                                 ((uint16_t)0x4000) /* External clock enable */
#define TIM_ETP                                 ((uint16_t)0x8000) /* External trigger polarity */

/*******************  Bit definition for TIM_DMAINTENR register  *******************/
#define TIM_UIE                                 ((uint16_t)0x0001) /* Update interrupt enable */
#define TIM_CC1IE                               ((uint16_t)0x0002) /* Capture/Compare 1 interrupt enable */
#define TIM_CC2IE                               ((uint16_t)0x0004) /* Capture/Compare 2 interrupt enable */
#define TIM_CC3IE                               ((uint16_t)0x0008) /* Capture/Compare 3 interrupt enable */
#define TIM_CC4IE                               ((uint16_t)0x0010) /* Capture/Compare 4 interrupt enable */
#define TIM_COMIE                               ((uint16_t)0x0020) /* COM interrupt enable */
#define TIM_TIE                                 ((uint16_t)0x0040) /* Trigger interrupt enable */
#define TIM_BIE                                 ((uint16_t)0x0080) /* Break interrupt enable */
#define TIM_UDE                                 ((uint16_t)0x0100) /* Update DMA request enable */
#define TIM_CC1DE                               ((uint16_t)0x0200) /* Capture/Compare 1 DMA request enable */
#define TIM_CC2DE                               ((uint16_t)0x0400) /* Capture/Compare 2 DMA request enable */
#define TIM_CC3DE                               ((uint16_t)0x0800) /* Capture/Compare 3 DMA request enable */
#define TIM_CC4DE                               ((uint16_t)0x1000) /* Capture/Compare 4 DMA request enable */
#define TIM_COMDE                               ((uint16_t)0x2000) /* COM DMA request enable */
#define TIM_TDE                                 ((uint16_t)0x4000) /* Trigger DMA request enable */

/********************  Bit definition for TIM_INTFR register  ********************/
#define TIM_UIF                                 ((uint16_t)0x0001) /* Update interrupt Flag */
#define TIM_CC1IF                               ((uint16_t)0x0002) /* Capture/Compare 1 interrupt Flag */
#define TIM_CC2IF                               ((uint16_t)0x0004) /* Capture/Compare 2 interrupt Flag */
#define TIM_CC3IF                               ((uint16_t)0x0008) /* Capture/Compare 3 interrupt Flag */
#define TIM_CC4IF                               ((uint16_t)0x0010) /* Capture/Compare 4 interrupt Flag */
#define TIM_COMIF                               ((uint16_t)0x0020) /* COM interrupt Flag */
#define TIM_TIF                                 ((uint16_t)0x0040) /* Trigger interrupt Flag */
#define TIM_BIF                                 ((uint16_t)0x0080) /* Break interrupt Flag */
#define TIM_CC1OF                               ((uint16_t)0x0200) /* Capture/Compare 1 Overcapture Flag */
#define TIM_CC2OF                               ((uint16_t)0x0400) /* Capture/Compare 2 Overcapture Flag */
#define TIM_CC3OF                               ((uint16_t)0x0800) /* Capture/Compare 3 Overcapture Flag */
#define TIM_CC4OF                               ((uint16_t)0x1000) /* Capture/Compare 4 Overcapture Flag */

/*******************  Bit definition for TIM_SWEVGR register  ********************/
#define TIM_UG                                  ((uint8_t)0x01) /* Update Generation */
#define TIM_CC1G                                ((uint8_t)0x02) /* Capture/Compare 1 Generation */
#define TIM_CC2G                                ((uint8_t)0x04) /* Capture/Compare 2 Generation */
#define TIM_CC3G                                ((uint8_t)0x08) /* Capture/Compare 3 Generation */
#define TIM_CC4G                                ((uint8_t)0x10) /* Capture/Compare 4 Generation */
#define TIM_COMG                                ((uint8_t)0x20) /* Capture/Compare Control Update Generation */
#define TIM_TG                                  ((uint8_t)0x40) /* Trigger Generation */
#define TIM_BG                                  ((uint8_t)0x80) /* Break Generation */

/******************  Bit definition for TIM_CHCTLR1 register  *******************/
#define TIM_CC1S                                ((uint16_t)0x0003) /* CC1S[1:0] bits (Capture/Compare 1 Selection) */
#define TIM_CC1S_0                              ((uint16_t)0x0001) /* Bit 0 */
#define TIM_CC1S_1                              ((uint16_t)0x0002) /* Bit 1 */

#define TIM_OC1FE                               ((uint16_t)0x0004) /* Output Compare 1 Fast enable */
#define TIM_OC1PE                               ((uint16_t)0x0008) /* Output Compare 1 Preload enable */

#define TIM_OC1M                                ((uint16_t)0x0070) /* OC1M[2:0] bits (Output Compare 1 Mode) */
#define TIM_OC1M_0                              ((uint16_t)0x0010) /* Bit 0 */
#define TIM_OC1M_1                              ((uint16_t)0x0020) /* Bit 1 */
#define TIM_OC1M_2                              ((uint16_t)0x0040) /* Bit 2 */

#define TIM_OC1CE                               ((uint16_t)0x0080) /* Output Compare 1Clear Enable */

#define TIM_CC2S                                ((uint16_t)0x0300) /* CC2S[1:0] bits (Capture/Compare 2 Selection) */
#define TIM_CC2S_0                              ((uint16_t)0x0100) /* Bit 0 */
#define TIM_CC2S_1                              ((uint16_t)0x0200) /* Bit 1 */

#define TIM_OC2FE                               ((uint16_t)0x0400) /* Output Compare 2 Fast enable */
#define TIM_OC2PE                               ((uint16_t)0x0800) /* Output Compare 2 Preload enable */

#define TIM_OC2M                                ((uint16_t)0x7000) /* OC2M[2:0] bits (Output Compare 2 Mode) */
#define TIM_OC2M_0                              ((uint16_t)0x1000) /* Bit 0 */
#define TIM_OC2M_1                              ((uint16_t)0x2000) /* Bit 1 */
#define TIM_OC2M_2                              ((uint16_t)0x4000) /* Bit 2 */

#define TIM_OC2CE                               ((uint16_t)0x8000) /* Output Compare 2 Clear Enable */

#define TIM_IC1PSC                              ((uint16_t)0x000C) /* IC1PSC[1:0] bits (Input Capture 1 Prescaler) */
#define TIM_IC1PSC_0                            ((uint16_t)0x0004) /* Bit 0 */
#define TIM_IC1PSC_1                            ((uint16_t)0x0008) /* Bit 1 */

#define TIM_IC1F                                ((uint16_t)0x00F0) /* IC1F[3:0] bits (Input Capture 1 Filter) */
#define TIM_IC1F_0                              ((uint16_t)0x0010) /* Bit 0 */
#define TIM_IC1F_1                              ((uint16_t)0x0020) /* Bit 1 */
#define TIM_IC1F_2                              ((uint16_t)0x0040) /* Bit 2 */
#define TIM_IC1F_3                              ((uint16_t)0x0080) /* Bit 3 */

#define TIM_IC2PSC                              ((uint16_t)0x0C00) /* IC2PSC[1:0] bits (Input Capture 2 Prescaler) */
#define TIM_IC2PSC_0                            ((uint16_t)0x0400) /* Bit 0 */
#define TIM_IC2PSC_1                            ((uint16_t)0x0800) /* Bit 1 */

#define TIM_IC2F                                ((uint16_t)0xF000) /* IC2F[3:0] bits (Input Capture 2 Filter) */
#define TIM_IC2F_0                              ((uint16_t)0x1000) /* Bit 0 */
#define TIM_IC2F_1                              ((uint16_t)0x2000) /* Bit 1 */
#define TIM_IC2F_2                              ((uint16_t)0x4000) /* Bit 2 */
#define TIM_IC2F_3                              ((uint16_t)0x8000) /* Bit 3 */

/******************  Bit definition for TIM_CHCTLR2 register  *******************/
#define TIM_CC3S                                ((uint16_t)0x0003) /* CC3S[1:0] bits (Capture/Compare 3 Selection) */
#define TIM_CC3S_0                              ((uint16_t)0x0001) /* Bit 0 */
#define TIM_CC3S_1                              ((uint16_t)0x0002) /* Bit 1 */

#define TIM_OC3FE                               ((uint16_t)0x0004) /* Output Compare 3 Fast enable */
#define TIM_OC3PE                               ((uint16_t)0x0008) /* Output Compare 3 Preload enable */

#define TIM_OC3M                                ((uint16_t)0x0070) /* OC3M[2:0] bits (Output Compare 3 Mode) */
#define TIM_OC3M_0                              ((uint16_t)0x0010) /* Bit 0 */
#define TIM_OC3M_1                              ((uint16_t)0x0020) /* Bit 1 */
#define TIM_OC3M_2                              ((uint16_t)0x0040) /* Bit 2 */

#define TIM_OC3CE                               ((uint16_t)0x0080) /* Output Compare 3 Clear Enable */

#define TIM_CC4S                                ((uint16_t)0x0300) /* CC4S[1:0] bits (Capture/Compare 4 Selection) */
#define TIM_CC4S_0                              ((uint16_t)0x0100) /* Bit 0 */
#define TIM_CC4S_1                              ((uint16_t)0x0200) /* Bit 1 */

#define TIM_OC4FE                               ((uint16_t)0x0400) /* Output Compare 4 Fast enable */
#define TIM_OC4PE                               ((uint16_t)0x0800) /* Output Compare 4 Preload enable */

#define TIM_OC4M                                ((uint16_t)0x7000) /* OC4M[2:0] bits (Output Compare 4 Mode) */
#define TIM_OC4M_0                              ((uint16_t)0x1000) /* Bit 0 */
#define TIM_OC4M_1                              ((uint16_t)0x2000) /* Bit 1 */
#define TIM_OC4M_2                              ((uint16_t)0x4000) /* Bit 2 */

#define TIM_OC4CE                               ((uint16_t)0x8000) /* Output Compare 4 Clear Enable */

#define TIM_IC3PSC                              ((uint16_t)0x000C) /* IC3PSC[1:0] bits (Input Capture 3 Prescaler) */
#define TIM_IC3PSC_0                            ((uint16_t)0x0004) /* Bit 0 */
#define TIM_IC3PSC_1                            ((uint16_t)0x0008) /* Bit 1 */

#define TIM_IC3F                                ((uint16_t)0x00F0) /* IC3F[3:0] bits (Input Capture 3 Filter) */
#define TIM_IC3F_0                              ((uint16_t)0x0010) /* Bit 0 */
#define TIM_IC3F_1                              ((uint16_t)0x0020) /* Bit 1 */
#define TIM_IC3F_2                              ((uint16_t)0x0040) /* Bit 2 */
#define TIM_IC3F_3                              ((uint16_t)0x0080) /* Bit 3 */

#define TIM_IC4PSC                              ((uint16_t)0x0C00) /* IC4PSC[1:0] bits (Input Capture 4 Prescaler) */
#define TIM_IC4PSC_0                            ((uint16_t)0x0400) /* Bit 0 */
#define TIM_IC4PSC_1                            ((uint16_t)0x0800) /* Bit 1 */

#define TIM_IC4F                                ((uint16_t)0xF000) /* IC4F[3:0] bits (Input Capture 4 Filter) */
#define TIM_IC4F_0                              ((uint16_t)0x1000) /* Bit 0 */
#define TIM_IC4F_1                              ((uint16_t)0x2000) /* Bit 1 */
#define TIM_IC4F_2                              ((uint16_t)0x4000) /* Bit 2 */
#define TIM_IC4F_3                              ((uint16_t)0x8000) /* Bit 3 */

/*******************  Bit definition for TIM_CCER register  *******************/
#define TIM_CC1E                                ((uint16_t)0x0001) /* Capture/Compare 1 output enable */
#define TIM_CC1P                                ((uint16_t)0x0002) /* Capture/Compare 1 output Polarity */
#define TIM_CC1NE                               ((uint16_t)0x0004) /* Capture/Compare 1 Complementary output enable */
#define TIM_CC1NP                               ((uint16_t)0x0008) /* Capture/Compare 1 Complementary output Polarity */
#define TIM_CC2E                                ((uint16_t)0x0010) /* Capture/Compare 2 output enable */
#define TIM_CC2P                                ((uint16_t)0x0020) /* Capture/Compare 2 output Polarity */
#define TIM_CC2NE                               ((uint16_t)0x0040) /* Capture/Compare 2 Complementary output enable */
#define TIM_CC2NP                               ((uint16_t)0x0080) /* Capture/Compare 2 Complementary output Polarity */
#define TIM_CC3E                                ((uint16_t)0x0100) /* Capture/Compare 3 output enable */
#define TIM_CC3P                                ((uint16_t)0x0200) /* Capture/Compare 3 output Polarity */
#define TIM_CC3NE                               ((uint16_t)0x0400) /* Capture/Compare 3 Complementary output enable */
#define TIM_CC3NP                               ((uint16_t)0x0800) /* Capture/Compare 3 Complementary output Polarity */
#define TIM_CC4E                                ((uint16_t)0x1000) /* Capture/Compare 4 output enable */
#define TIM_CC4P                                ((uint16_t)0x2000) /* Capture/Compare 4 output Polarity */
#define TIM_CC4NP                               ((uint16_t)0x8000) /* Capture/Compare 4 Complementary output Polarity */

/*******************  Bit definition for TIM_CNT register  ********************/
#define TIM_CNT                                 ((uint16_t)0xFFFF) /* Counter Value */

/*******************  Bit definition for TIM_PSC register  ********************/
#define TIM_PSC                                 ((uint16_t)0xFFFF) /* Prescaler Value */

/*******************  Bit definition for TIM_ATRLR register  ********************/
#define TIM_ARR                                 ((uint16_t)0xFFFF) /* actual auto-reload Value */

/*******************  Bit definition for TIM_RPTCR register  ********************/
#define TIM_REP                                 ((uint8_t)0xFF) /* Repetition Counter Value */

/*******************  Bit definition for TIM_CH1CVR register  *******************/
#define TIM_CCR1                                ((uint16_t)0xFFFF) /* Capture/Compare 1 Value */

/*******************  Bit definition for TIM_CH2CVR register  *******************/
#define TIM_CCR2                                ((uint16_t)0xFFFF) /* Capture/Compare 2 Value */

/*******************  Bit definition for TIM_CH3CVR register  *******************/
#define TIM_CCR3                                ((uint16_t)0xFFFF) /* Capture/Compare 3 Value */

/*******************  Bit definition for TIM_CH4CVR register  *******************/
#define TIM_CCR4                                ((uint16_t)0xFFFF) /* Capture/Compare 4 Value */

/*******************  Bit definition for TIM_BDTR register  *******************/
#define TIM_DTG                                 ((uint16_t)0x00FF) /* DTG[0:7] bits (Dead-Time Generator set-up) */
#define TIM_DTG_0                               ((uint16_t)0x0001) /* Bit 0 */
#define TIM_DTG_1                               ((uint16_t)0x0002) /* Bit 1 */
#define TIM_DTG_2                               ((uint16_t)0x0004) /* Bit 2 */
#define TIM_DTG_3                               ((uint16_t)0x0008) /* Bit 3 */
#define TIM_DTG_4                               ((uint16_t)0x0010) /* Bit 4 */
#define TIM_DTG_5                               ((uint16_t)0x0020) /* Bit 5 */
#define TIM_DTG_6                               ((uint16_t)0x0040) /* Bit 6 */
#define TIM_DTG_7                               ((uint16_t)0x0080) /* Bit 7 */

#define TIM_LOCK                                ((uint16_t)0x0300) /* LOCK[1:0] bits (Lock Configuration) */
#define TIM_LOCK_0                              ((uint16_t)0x0100) /* Bit 0 */
#define TIM_LOCK_1                              ((uint16_t)0x0200) /* Bit 1 */

#define TIM_OSSI                                ((uint16_t)0x0400) /* Off-State Selection for Idle mode */
#define TIM_OSSR                                ((uint16_t)0x0800) /* Off-State Selection for Run mode */
#define TIM_BKE                                 ((uint16_t)0x1000) /* Break enable */
#define TIM_BKP                                 ((uint16_t)0x2000) /* Break Polarity */
#define TIM_AOE                                 ((uint16_t)0x4000) /* Automatic Output enable */
#define TIM_MOE                                 ((uint16_t)0x8000) /* Main Output enable */

/*******************  Bit definition for TIM_DMACFGR register  ********************/
#define TIM_DBA                                 ((uint16_t)0x001F) /* DBA[4:0] bits (DMA Base Address) */
#define TIM_DBA_0                               ((uint16_t)0x0001) /* Bit 0 */
#define TIM_DBA_1                               ((uint16_t)0x0002) /* Bit 1 */
#define TIM_DBA_2                               ((uint16_t)0x0004) /* Bit 2 */
#define TIM_DBA_3                               ((uint16_t)0x0008) /* Bit 3 */
#define TIM_DBA_4                               ((uint16_t)0x0010) /* Bit 4 */

#define TIM_DBL                                 ((uint16_t)0x1F00) /* DBL[4:0] bits (DMA Burst Length) */
#define TIM_DBL_0                               ((uint16_t)0x0100) /* Bit 0 */
#define TIM_DBL_1                               ((uint16_t)0x0200) /* Bit 1 */
#define TIM_DBL_2                               ((uint16_t)0x0400) /* Bit 2 */
#define TIM_DBL_3                               ((uint16_t)0x0800) /* Bit 3 */
#define TIM_DBL_4                               ((uint16_t)0x1000) /* Bit 4 */

/*******************  Bit definition for TIM_DMAADR register  *******************/
#define TIM_DMAR_DMAB                           ((uint16_t)0xFFFF) /* DMA register for burst accesses */

/******************************************************************************/
/*         Universal Synchronous Asynchronous Receiver Transmitter            */
/******************************************************************************/

/*******************  Bit definition for USART_STATR register  *******************/
#define USART_STATR_PE                          ((uint16_t)0x0001) /* Parity Error */
#define USART_STATR_FE                          ((uint16_t)0x0002) /* Framing Error */
#define USART_STATR_NE                          ((uint16_t)0x0004) /* Noise Error Flag */
#define USART_STATR_ORE                         ((uint16_t)0x0008) /* OverRun Error */
#define USART_STATR_IDLE                        ((uint16_t)0x0010) /* IDLE line detected */
#define USART_STATR_RXNE                        ((uint16_t)0x0020) /* Read Data Register Not Empty */
#define USART_STATR_TC                          ((uint16_t)0x0040) /* Transmission Complete */
#define USART_STATR_TXE                         ((uint16_t)0x0080) /* Transmit Data Register Empty */
#define USART_STATR_LBD                         ((uint16_t)0x0100) /* LIN Break Detection Flag */
#define USART_STATR_CTS                         ((uint16_t)0x0200) /* CTS Flag */

/*******************  Bit definition for USART_DATAR register  *******************/
#define USART_DATAR_DR                          ((uint16_t)0x01FF) /* Data value */

/******************  Bit definition for USART_BRR register  *******************/
#define USART_BRR_DIV_Fraction                  ((uint16_t)0x000F) /* Fraction of USARTDIV */
#define USART_BRR_DIV_Mantissa                  ((uint16_t)0xFFF0) /* Mantissa of USARTDIV */

/******************  Bit definition for USART_CTLR1 register  *******************/
#define USART_CTLR1_SBK                         ((uint16_t)0x0001) /* Send Break */
#define USART_CTLR1_RWU                         ((uint16_t)0x0002) /* Receiver wakeup */
#define USART_CTLR1_RE                          ((uint16_t)0x0004) /* Receiver Enable */
#define USART_CTLR1_TE                          ((uint16_t)0x0008) /* Transmitter Enable */
#define USART_CTLR1_IDLEIE                      ((uint16_t)0x0010) /* IDLE Interrupt Enable */
#define USART_CTLR1_RXNEIE                      ((uint16_t)0x0020) /* RXNE Interrupt Enable */
#define USART_CTLR1_TCIE                        ((uint16_t)0x0040) /* Transmission Complete Interrupt Enable */
#define USART_CTLR1_TXEIE                       ((uint16_t)0x0080) /* PE Interrupt Enable */
#define USART_CTLR1_PEIE                        ((uint16_t)0x0100) /* PE Interrupt Enable */
#define USART_CTLR1_PS                          ((uint16_t)0x0200) /* Parity Selection */
#define USART_CTLR1_PCE                         ((uint16_t)0x0400) /* Parity Control Enable */
#define USART_CTLR1_WAKE                        ((uint16_t)0x0800) /* Wakeup method */
#define USART_CTLR1_M                           ((uint16_t)0x1000) /* Word length */
#define USART_CTLR1_UE                          ((uint16_t)0x2000) /* USART Enable */
#define USART_CTLR1_OVER8                       ((uint16_t)0x8000) /* USART Oversmapling 8-bits */

/******************  Bit definition for USART_CTLR2 register  *******************/
#define USART_CTLR2_ADD                         ((uint16_t)0x000F) /* Address of the USART node */
#define USART_CTLR2_LBDL                        ((uint16_t)0x0020) /* LIN Break Detection Length */
#define USART_CTLR2_LBDIE                       ((uint16_t)0x0040) /* LIN Break Detection Interrupt Enable */
#define USART_CTLR2_LBCL                        ((uint16_t)0x0100) /* Last Bit Clock pulse */
#define USART_CTLR2_CPHA                        ((uint16_t)0x0200) /* Clock Phase */
#define USART_CTLR2_CPOL                        ((uint16_t)0x0400) /* Clock Polarity */
#define USART_CTLR2_CLKEN                       ((uint16_t)0x0800) /* Clock Enable */

#define USART_CTLR2_STOP                        ((uint16_t)0x3000) /* STOP[1:0] bits (STOP bits) */
#define USART_CTLR2_STOP_0                      ((uint16_t)0x1000) /* Bit 0 */
#define USART_CTLR2_STOP_1                      ((uint16_t)0x2000) /* Bit 1 */

#define USART_CTLR2_LINEN                       ((uint16_t)0x4000) /* LIN mode enable */

/******************  Bit definition for USART_CTLR3 register  *******************/
#define USART_CTLR3_EIE                         ((uint16_t)0x0001) /* Error Interrupt Enable */
#define USART_CTLR3_IREN                        ((uint16_t)0x0002) /* IrDA mode Enable */
#define USART_CTLR3_IRLP                        ((uint16_t)0x0004) /* IrDA Low-Power */
#define USART_CTLR3_HDSEL                       ((uint16_t)0x0008) /* Half-Duplex Selection */
#define USART_CTLR3_NACK                        ((uint16_t)0x0010) /* Smartcard NACK enable */
#define USART_CTLR3_SCEN                        ((uint16_t)0x0020) /* Smartcard mode enable */
#define USART_CTLR3_DMAR                        ((uint16_t)0x0040) /* DMA Enable Receiver */
#define USART_CTLR3_DMAT                        ((uint16_t)0x0080) /* DMA Enable Transmitter */
#define USART_CTLR3_RTSE                        ((uint16_t)0x0100) /* RTS Enable */
#define USART_CTLR3_CTSE                        ((uint16_t)0x0200) /* CTS Enable */
#define USART_CTLR3_CTSIE                       ((uint16_t)0x0400) /* CTS Interrupt Enable */
#define USART_CTLR3_ONEBIT                      ((uint16_t)0x0800) /* One Bit method */

/******************  Bit definition for USART_GPR register  ******************/
#define USART_GPR_PSC                           ((uint16_t)0x00FF) /* PSC[7:0] bits (Prescaler value) */
#define USART_GPR_PSC_0                         ((uint16_t)0x0001) /* Bit 0 */
#define USART_GPR_PSC_1                         ((uint16_t)0x0002) /* Bit 1 */
#define USART_GPR_PSC_2                         ((uint16_t)0x0004) /* Bit 2 */
#define USART_GPR_PSC_3                         ((uint16_t)0x0008) /* Bit 3 */
#define USART_GPR_PSC_4                         ((uint16_t)0x0010) /* Bit 4 */
#define USART_GPR_PSC_5                         ((uint16_t)0x0020) /* Bit 5 */
#define USART_GPR_PSC_6                         ((uint16_t)0x0040) /* Bit 6 */
#define USART_GPR_PSC_7                         ((uint16_t)0x0080) /* Bit 7 */

#define USART_GPR_GT                            ((uint16_t)0xFF00) /* Guard time value */

/******************************************************************************/
/*                            Window WATCHDOG                                 */
/******************************************************************************/

/*******************  Bit definition for WWDG_CTLR register  ********************/
#define WWDG_CTLR_T                             ((uint8_t)0x7F) /* T[6:0] bits (7-Bit counter (MSB to LSB)) */
#define WWDG_CTLR_T0                            ((uint8_t)0x01) /* Bit 0 */
#define WWDG_CTLR_T1                            ((uint8_t)0x02) /* Bit 1 */
#define WWDG_CTLR_T2                            ((uint8_t)0x04) /* Bit 2 */
#define WWDG_CTLR_T3                            ((uint8_t)0x08) /* Bit 3 */
#define WWDG_CTLR_T4                            ((uint8_t)0x10) /* Bit 4 */
#define WWDG_CTLR_T5                            ((uint8_t)0x20) /* Bit 5 */
#define WWDG_CTLR_T6                            ((uint8_t)0x40) /* Bit 6 */

#define WWDG_CTLR_WDGA                          ((uint8_t)0x80) /* Activation bit */

/*******************  Bit definition for WWDG_CFGR register  *******************/
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
#define WWDG_STATR_EWIF                         ((uint8_t)0x01) /* Early Wakeup Interrupt Flag */

/******************************************************************************/
/*                          ENHANCED FUNNCTION                                */
/******************************************************************************/

/****************************  Enhanced register  *****************************/
#define EXTEN_USBD_LS                           ((uint32_t)0x00000001) /* Bit 0 */
#define EXTEN_USBD_PU_EN                        ((uint32_t)0x00000002) /* Bit 1 */
#define EXTEN_USBFS_IO_EN                       ((uint32_t)0x00000004) /* Bit 2 */
#define EXTEN_USB_5V_SEL                        ((uint32_t)0x00000008) /* Bit 3 */
#define EXTEN_PLL_HSI_PRE                       ((uint32_t)0x00000010) /* Bit 4 */
#define EXTEN_LOCKUP_EN                         ((uint32_t)0x00000040) /* Bit 5 */
#define EXTEN_LOCKUP_RSTF                       ((uint32_t)0x00000080) /* Bit 7 */
#define EXTEN_USBHD_IO_EN                       EXTEN_USBFS_IO_EN

#define EXTEN_ULLDO_TRIM                        ((uint32_t)0x00000300) /* ULLDO_TRIM[1:0] bits */
#define EXTEN_ULLDO_TRIM0                       ((uint32_t)0x00000100) /* Bit 0 */
#define EXTEN_ULLDO_TRIM1                       ((uint32_t)0x00000200) /* Bit 1 */


#define EXTEN_IDO_TRIM                          ((uint32_t)0x00000400) /* Bit 10 */
#define EXTEN_WRITE_EN                          ((uint32_t)0x00004000) /* Bit 14 */
#define EXTEN_SHORT_WAKE                        ((uint32_t)0x00008000) /* Bit 15 */

#define EXTEN_FLASH_CLK_TRIM                    ((uint32_t)0x00070000) /* FLASH_CLK_TRIM[2:0] bits */
#define EXTEN_FLASH_CLK_TRIM0                   ((uint32_t)0x00010000) /* Bit 0 */
#define EXTEN_FLASH_CLK_TRIM1                   ((uint32_t)0x00020000) /* Bit 1 */
#define EXTEN_FLASH_CLK_TRIM2                   ((uint32_t)0x00040000) /* Bit 2 */




#ifdef __cplusplus
}
#endif



#ifdef __cplusplus
extern "C" {
#endif



/* ch32v00x_gpio.c -----------------------------------------------------------*/
/* MASK */
#define ECR_PORTPINCONFIG_MASK    ((uint16_t)0xFF80)
#define LSB_MASK                  ((uint16_t)0xFFFF)
#define DBGAFR_POSITION_MASK      ((uint32_t)0x000F0000)
#define DBGAFR_SDI_MASK           ((uint32_t)0xF8FFFFFF)
#define DBGAFR_LOCATION_MASK      ((uint32_t)0x00200000)
#define DBGAFR_NUMBITS_MASK       ((uint32_t)0x00100000)


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
#define ADC_CTLR1_CLEAR_Mask                 ((uint32_t)0xFFF0FEFF)

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


/* ADC1 RDATAR register base address */
#define RDATAR_ADDRESS                   ((uint32_t)0x4001244C)

/* ch32v20x_bkp.c ------------------------------------------------------------*/
#define OCTLR_CAL_MASK    ((uint16_t)0xFF80)
#define OCTLR_MASK        ((uint16_t)0xFC7F)




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

#define DMA2_Channel6_IT_Mask     ((uint32_t)(DMA_GIF6 | DMA_TCIF6 | DMA_HTIF6 | DMA_TEIF6))
#define DMA2_Channel7_IT_Mask     ((uint32_t)(DMA_GIF7 | DMA_TCIF7 | DMA_HTIF7 | DMA_TEIF7))
#define DMA2_Channel8_IT_Mask     ((uint32_t)(DMA_GIF8 | DMA_TCIF8 | DMA_HTIF8 | DMA_TEIF8))
#define DMA2_Channel9_IT_Mask     ((uint32_t)(DMA_GIF9 | DMA_TCIF9 | DMA_HTIF9 | DMA_TEIF9))
#define DMA2_Channel10_IT_Mask    ((uint32_t)(DMA_GIF10 | DMA_TCIF10 | DMA_HTIF10 | DMA_TEIF10))
#define DMA2_Channel11_IT_Mask    ((uint32_t)(DMA_GIF11 | DMA_TCIF11 | DMA_HTIF11 | DMA_TEIF11))

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

#define ACR_HLFCYA_Mask            ((uint32_t)0xFFFFFFF7)
#define ACR_PRFTBE_Mask            ((uint32_t)0xFFFFFFEF)
#define ACR_PRFTBS_Mask            ((uint32_t)0x00000020)

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
#define CR_FAST_LOCK_Set           ((uint32_t)0x00008000)
#define CR_PAGE_PG                 ((uint32_t)0x00010000)
#define CR_PAGE_ER                 ((uint32_t)0x00020000)
#define CR_BUF_LOAD                ((uint32_t)0x00040000)
#define CR_BUF_RST                 ((uint32_t)0x00080000)

/* FLASH Status Register bits */
#define SR_BSY                     ((uint32_t)0x00000001)
#define SR_PGERR                   ((uint32_t)0x00000004)
#define SR_WRPRTERR                ((uint32_t)0x00000010)
#define SR_EOP                     ((uint32_t)0x00000020)

/* FLASH Mask */
#define RDPRT_Mask                 ((uint32_t)0x00000002)
#define WRP0_Mask                  ((uint32_t)0x000000FF)
#define WRP1_Mask                  ((uint32_t)0x0000FF00)
#define WRP2_Mask                  ((uint32_t)0x00FF0000)
#define WRP3_Mask                  ((uint32_t)0xFF000000)
#define OB_USER_BFB2               ((uint16_t)0x0008)

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
#define ValidAddrEnd               (FLASH_BASE + 0x10000)


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
#define I2C_CTLR1_CLEAR_Mask         ((uint16_t)0xFBF5)

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
#define CTLR_DS_MASK     ((uint32_t)0xFFFFFFFC)
#define CTLR_PLS_MASK    ((uint32_t)0xFFFFFF1F)

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
#define RCC_FLAG_Mask                  ((uint8_t)0x1F)

/* INTR register byte 2 (Bits[15:8]) base address */
#define INTR_BYTE2_ADDRESS         ((uint32_t)0x40021009)

/* INTR register byte 3 (Bits[23:16]) base address */
#define INTR_BYTE3_ADDRESS         ((uint32_t)0x4002100A)

/* CFGR0 register byte 4 (Bits[31:24]) base address */
#define CFGR0_BYTE4_ADDRESS        ((uint32_t)0x40021007)

/* BDCTLR register base address */
#define BDCTLR_ADDRESS             (PERIPH_BASE + BDCTLR_OFFSET)

#ifndef __ASSEMBLER__
static __I uint8_t APBAHBPrescTable[16] = {0, 0, 0, 0, 1, 2, 3, 4, 1, 2, 3, 4, 6, 7, 8, 9};
static __I uint8_t ADCPrescTable[4] = {2, 4, 6, 8};
#endif


/* ch32v20x_rtc.c ------------------------------------------------------------*/

/* RTC_Private_Defines */
#define RTC_LSB_MASK     ((uint32_t)0x0000FFFF) /* RTC LSB Mask */
#define PRLH_MSB_MASK    ((uint32_t)0x000F0000) /* RTC Prescaler MSB Mask */



/* ch32v00x_spi.c ------------------------------------------------------------*/

/* SPI SPE mask */
#define CTLR1_SPE_Set         ((uint16_t)0x0040)
#define CTLR1_SPE_Reset       ((uint16_t)0xFFBF)

/* I2S I2SE mask */
#define I2SCFGR_I2SE_Set      ((uint16_t)0x0400)
#define I2SCFGR_I2SE_Reset    ((uint16_t)0xFBFF)

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

/* SPI or I2S mode selection masks */
#define SPI_Mode_Select       ((uint16_t)0xF7FF)
#define I2S_Mode_Select       ((uint16_t)0x0800)

/* I2S clock source selection masks */
#define I2S2_CLOCK_SRC        ((uint32_t)(0x00020000))
#define I2S3_CLOCK_SRC        ((uint32_t)(0x00040000))
#define I2S_MUL_MASK          ((uint32_t)(0x0000F000))
#define I2S_DIV_MASK          ((uint32_t)(0x000000F0))


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
#define USART_CTLR1_CLEAR_Mask          ((uint16_t)0xE9F3) /* USART CR1 Mask */
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
#define ADC_Mode_RegInjecSimult                        ((uint32_t)0x00010000)
#define ADC_Mode_RegSimult_AlterTrig                   ((uint32_t)0x00020000)
#define ADC_Mode_InjecSimult_FastInterl                ((uint32_t)0x00030000)
#define ADC_Mode_InjecSimult_SlowInterl                ((uint32_t)0x00040000)
#define ADC_Mode_InjecSimult                           ((uint32_t)0x00050000)
#define ADC_Mode_RegSimult                             ((uint32_t)0x00060000)
#define ADC_Mode_FastInterl                            ((uint32_t)0x00070000)
#define ADC_Mode_SlowInterl                            ((uint32_t)0x00080000)
#define ADC_Mode_AlterTrig                             ((uint32_t)0x00090000)

/* ADC_external_trigger_sources_for_regular_channels_conversion */

#define ADC_ExternalTrigConv_T1_CC1                    ((uint32_t)0x00000000)
#define ADC_ExternalTrigConv_T1_CC2                    ((uint32_t)0x00020000)
#define ADC_ExternalTrigConv_T2_CC2                    ((uint32_t)0x00060000)
#define ADC_ExternalTrigConv_T3_TRGO                   ((uint32_t)0x00080000)
#define ADC_ExternalTrigConv_T4_CC4                    ((uint32_t)0x000A0000)
#define ADC_ExternalTrigConv_Ext_IT11_TIM8_TRGO        ((uint32_t)0x000C0000)

#define ADC_ExternalTrigConv_T1_CC3                    ((uint32_t)0x00040000)
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
#define ADC_Channel_10                                 ((uint8_t)0x0A)
#define ADC_Channel_11                                 ((uint8_t)0x0B)
#define ADC_Channel_12                                 ((uint8_t)0x0C)
#define ADC_Channel_13                                 ((uint8_t)0x0D)
#define ADC_Channel_14                                 ((uint8_t)0x0E)
#define ADC_Channel_15                                 ((uint8_t)0x0F)
#define ADC_Channel_16                                 ((uint8_t)0x10)
#define ADC_Channel_17                                 ((uint8_t)0x11)

#define ADC_Channel_TempSensor                         ((uint8_t)ADC_Channel_16)
#define ADC_Channel_Vrefint                            ((uint8_t)ADC_Channel_17)


/* ADC_sampling_time */
#define ADC_SampleTime_1Cycles5                        ((uint8_t)0x00)
#define ADC_SampleTime_7Cycles5                        ((uint8_t)0x01)
#define ADC_SampleTime_13Cycles5                       ((uint8_t)0x02)
#define ADC_SampleTime_28Cycles5                       ((uint8_t)0x03)
#define ADC_SampleTime_41Cycles5                       ((uint8_t)0x04)
#define ADC_SampleTime_55Cycles5                       ((uint8_t)0x05)
#define ADC_SampleTime_71Cycles5                       ((uint8_t)0x06)
#define ADC_SampleTime_239Cycles5                      ((uint8_t)0x07)

/* ADC_external_trigger_sources_for_injected_channels_conversion */
#define ADC_ExternalTrigInjecConv_T2_TRGO              ((uint32_t)0x00002000)
#define ADC_ExternalTrigInjecConv_T2_CC1               ((uint32_t)0x00003000)
#define ADC_ExternalTrigInjecConv_T3_CC4               ((uint32_t)0x00004000)
#define ADC_ExternalTrigInjecConv_T4_TRGO              ((uint32_t)0x00005000)
#define ADC_ExternalTrigInjecConv_Ext_IT15_TIM8_CC4    ((uint32_t)0x00006000)

#define ADC_ExternalTrigInjecConv_T1_TRGO              ((uint32_t)0x00000000)
#define ADC_ExternalTrigInjecConv_T1_CC4               ((uint32_t)0x00001000)
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

/* ADC_external_trigger_sources_delay_channels_definition */
#define ADC_ExternalTrigRegul_DLY                      ((uint32_t)0x00000000)

/* ch32v20x_bkp.h ------------------------------------------------------------*/

/* Tamper_Pin_active_level */
#define BKP_TamperPinLevel_High           ((uint16_t)0x0000)
#define BKP_TamperPinLevel_Low            ((uint16_t)0x0001)

/* RTC_output_source_to_output_on_the_Tamper_pin */
#define BKP_RTCOutputSource_None          ((uint16_t)0x0000)
#define BKP_RTCOutputSource_CalibClock    ((uint16_t)0x0080)
#define BKP_RTCOutputSource_Alarm         ((uint16_t)0x0100)
#define BKP_RTCOutputSource_Second        ((uint16_t)0x0300)

/* Data_Backup_Register */
#define BKP_DR1                           ((uint16_t)0x0004)
#define BKP_DR2                           ((uint16_t)0x0008)
#define BKP_DR3                           ((uint16_t)0x000C)
#define BKP_DR4                           ((uint16_t)0x0010)
#define BKP_DR5                           ((uint16_t)0x0014)
#define BKP_DR6                           ((uint16_t)0x0018)
#define BKP_DR7                           ((uint16_t)0x001C)
#define BKP_DR8                           ((uint16_t)0x0020)
#define BKP_DR9                           ((uint16_t)0x0024)
#define BKP_DR10                          ((uint16_t)0x0028)
#define BKP_DR11                          ((uint16_t)0x0040)
#define BKP_DR12                          ((uint16_t)0x0044)
#define BKP_DR13                          ((uint16_t)0x0048)
#define BKP_DR14                          ((uint16_t)0x004C)
#define BKP_DR15                          ((uint16_t)0x0050)
#define BKP_DR16                          ((uint16_t)0x0054)
#define BKP_DR17                          ((uint16_t)0x0058)
#define BKP_DR18                          ((uint16_t)0x005C)
#define BKP_DR19                          ((uint16_t)0x0060)
#define BKP_DR20                          ((uint16_t)0x0064)
#define BKP_DR21                          ((uint16_t)0x0068)
#define BKP_DR22                          ((uint16_t)0x006C)
#define BKP_DR23                          ((uint16_t)0x0070)
#define BKP_DR24                          ((uint16_t)0x0074)
#define BKP_DR25                          ((uint16_t)0x0078)
#define BKP_DR26                          ((uint16_t)0x007C)
#define BKP_DR27                          ((uint16_t)0x0080)
#define BKP_DR28                          ((uint16_t)0x0084)
#define BKP_DR29                          ((uint16_t)0x0088)
#define BKP_DR30                          ((uint16_t)0x008C)
#define BKP_DR31                          ((uint16_t)0x0090)
#define BKP_DR32                          ((uint16_t)0x0094)
#define BKP_DR33                          ((uint16_t)0x0098)
#define BKP_DR34                          ((uint16_t)0x009C)
#define BKP_DR35                          ((uint16_t)0x00A0)
#define BKP_DR36                          ((uint16_t)0x00A4)
#define BKP_DR37                          ((uint16_t)0x00A8)
#define BKP_DR38                          ((uint16_t)0x00AC)
#define BKP_DR39                          ((uint16_t)0x00B0)
#define BKP_DR40                          ((uint16_t)0x00B4)
#define BKP_DR41                          ((uint16_t)0x00B8)
#define BKP_DR42                          ((uint16_t)0x00BC)




/* ch32v00x_dbgmcu.h ---------------------------------------------------------*/

/* CFGR0 Register */
#define DBGMCU_IWDG_STOP             ((uint32_t)0x00000001)
#define DBGMCU_WWDG_STOP             ((uint32_t)0x00000002)
#define DBGMCU_I2C1_SMBUS_TIMEOUT    ((uint32_t)0x00000004)
#define DBGMCU_I2C2_SMBUS_TIMEOUT    ((uint32_t)0x00000008)
#define DBGMCU_TIM1_STOP             ((uint32_t)0x00000010)
#define DBGMCU_TIM2_STOP             ((uint32_t)0x00000020)
#define DBGMCU_TIM3_STOP             ((uint32_t)0x00000040)
#define DBGMCU_TIM4_STOP             ((uint32_t)0x00000080)

/* CFGR1 Register */
#define DBGMCU_SLEEP                 ((uint32_t)0x00000001)
#define DBGMCU_STOP                  ((uint32_t)0x00000002)
#define DBGMCU_STANDBY               ((uint32_t)0x00000004)

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

#define DMA2_IT_GL1                        ((uint32_t)0x10000001)
#define DMA2_IT_TC1                        ((uint32_t)0x10000002)
#define DMA2_IT_HT1                        ((uint32_t)0x10000004)
#define DMA2_IT_TE1                        ((uint32_t)0x10000008)
#define DMA2_IT_GL2                        ((uint32_t)0x10000010)
#define DMA2_IT_TC2                        ((uint32_t)0x10000020)
#define DMA2_IT_HT2                        ((uint32_t)0x10000040)
#define DMA2_IT_TE2                        ((uint32_t)0x10000080)
#define DMA2_IT_GL3                        ((uint32_t)0x10000100)
#define DMA2_IT_TC3                        ((uint32_t)0x10000200)
#define DMA2_IT_HT3                        ((uint32_t)0x10000400)
#define DMA2_IT_TE3                        ((uint32_t)0x10000800)
#define DMA2_IT_GL4                        ((uint32_t)0x10001000)
#define DMA2_IT_TC4                        ((uint32_t)0x10002000)
#define DMA2_IT_HT4                        ((uint32_t)0x10004000)
#define DMA2_IT_TE4                        ((uint32_t)0x10008000)
#define DMA2_IT_GL5                        ((uint32_t)0x10010000)
#define DMA2_IT_TC5                        ((uint32_t)0x10020000)
#define DMA2_IT_HT5                        ((uint32_t)0x10040000)
#define DMA2_IT_TE5                        ((uint32_t)0x10080000)

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

#define DMA2_FLAG_GL1                      ((uint32_t)0x10000001)
#define DMA2_FLAG_TC1                      ((uint32_t)0x10000002)
#define DMA2_FLAG_HT1                      ((uint32_t)0x10000004)
#define DMA2_FLAG_TE1                      ((uint32_t)0x10000008)
#define DMA2_FLAG_GL2                      ((uint32_t)0x10000010)
#define DMA2_FLAG_TC2                      ((uint32_t)0x10000020)
#define DMA2_FLAG_HT2                      ((uint32_t)0x10000040)
#define DMA2_FLAG_TE2                      ((uint32_t)0x10000080)
#define DMA2_FLAG_GL3                      ((uint32_t)0x10000100)
#define DMA2_FLAG_TC3                      ((uint32_t)0x10000200)
#define DMA2_FLAG_HT3                      ((uint32_t)0x10000400)
#define DMA2_FLAG_TE3                      ((uint32_t)0x10000800)
#define DMA2_FLAG_GL4                      ((uint32_t)0x10001000)
#define DMA2_FLAG_TC4                      ((uint32_t)0x10002000)
#define DMA2_FLAG_HT4                      ((uint32_t)0x10004000)
#define DMA2_FLAG_TE4                      ((uint32_t)0x10008000)
#define DMA2_FLAG_GL5                      ((uint32_t)0x10010000)
#define DMA2_FLAG_TC5                      ((uint32_t)0x10020000)
#define DMA2_FLAG_HT5                      ((uint32_t)0x10040000)
#define DMA2_FLAG_TE5                      ((uint32_t)0x10080000)



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
#define EXTI_Line8     ((uint32_t)0x00000100)  /* External interrupt line 8 */
#define EXTI_Line9     ((uint32_t)0x00000200)  /* External interrupt line 9 */
#define EXTI_Line10    ((uint32_t)0x00000400)  /* External interrupt line 10 */
#define EXTI_Line11    ((uint32_t)0x00000800)  /* External interrupt line 11 */
#define EXTI_Line12    ((uint32_t)0x00001000)  /* External interrupt line 12 */
#define EXTI_Line13    ((uint32_t)0x00002000)  /* External interrupt line 13 */
#define EXTI_Line14    ((uint32_t)0x00004000)  /* External interrupt line 14 */
#define EXTI_Line15    ((uint32_t)0x00008000)  /* External interrupt line 15 */

#define EXTI_Line16    ((uint32_t)0x00010000)  /* External interrupt line 16 Connected to the PVD Output */
#define EXTI_Line17    ((uint32_t)0x00020000)  /* External interrupt line 17 Connected to the RTC Alarm event */
#define EXTI_Line18    ((uint32_t)0x00040000) 
#define EXTI_Line19    ((uint32_t)0x00080000)  /* External interrupt line 19 Connected to the USBFS Wakeup event */

/* ch32v00x_flash.h ----------------------------------------------------------*/


#ifndef __ASSEMBLER__
/* FLASH Status */
typedef enum
{
    FLASH_BUSY = 1,
    FLASH_ERROR_PG,
    FLASH_ERROR_WRP,
    FLASH_COMPLETE,
    FLASH_TIMEOUT,
    FLASH_OP_RANGE_ERROR = 0xFD,
    FLASH_ALIGN_ERROR = 0xFE,
    FLASH_ADR_RANGE_ERROR = 0xFF,
} FLASH_Status;
#endif

/* Flash_Latency */
#define FLASH_Latency_0                  ((uint32_t)0x00000000) /* FLASH Zero Latency cycle */
#define FLASH_Latency_1                  ((uint32_t)0x00000001) /* FLASH One Latency cycle */
#define FLASH_Latency_2                  ((uint32_t)0x00000002) /* FLASH Two Latency cycles */

/* Half_Cycle_Enable_Disable */
#define FLASH_HalfCycleAccess_Enable     ((uint32_t)0x00000008) /* FLASH Half Cycle Enable */
#define FLASH_HalfCycleAccess_Disable    ((uint32_t)0x00000000) /* FLASH Half Cycle Disable */

/* Prefetch_Buffer_Enable_Disable */
#define FLASH_PrefetchBuffer_Enable      ((uint32_t)0x00000010) /* FLASH Prefetch Buffer Enable */
#define FLASH_PrefetchBuffer_Disable     ((uint32_t)0x00000000) /* FLASH Prefetch Buffer Disable */


/* Values to be used with CH32V10x Low and Medium density devices */
#define FLASH_WRProt_Pages0to3           ((uint32_t)0x00000001) /* CH32 Low and Medium density devices: Write protection of page 0 to 3 */
#define FLASH_WRProt_Pages4to7           ((uint32_t)0x00000002) /* CH32 Low and Medium density devices: Write protection of page 4 to 7 */
#define FLASH_WRProt_Pages8to11          ((uint32_t)0x00000004) /* CH32 Low and Medium density devices: Write protection of page 8 to 11 */
#define FLASH_WRProt_Pages12to15         ((uint32_t)0x00000008) /* CH32 Low and Medium density devices: Write protection of page 12 to 15 */
#define FLASH_WRProt_Pages16to19         ((uint32_t)0x00000010) /* CH32 Low and Medium density devices: Write protection of page 16 to 19 */
#define FLASH_WRProt_Pages20to23         ((uint32_t)0x00000020) /* CH32 Low and Medium density devices: Write protection of page 20 to 23 */
#define FLASH_WRProt_Pages24to27         ((uint32_t)0x00000040) /* CH32 Low and Medium density devices: Write protection of page 24 to 27 */
#define FLASH_WRProt_Pages28to31         ((uint32_t)0x00000080) /* CH32 Low and Medium density devices: Write protection of page 28 to 31 */

/* Values to be used with CH32V10x Medium-density devices */
#define FLASH_WRProt_Pages32to35         ((uint32_t)0x00000100) /* CH32 Medium-density devices: Write protection of page 32 to 35 */
#define FLASH_WRProt_Pages36to39         ((uint32_t)0x00000200) /* CH32 Medium-density devices: Write protection of page 36 to 39 */
#define FLASH_WRProt_Pages40to43         ((uint32_t)0x00000400) /* CH32 Medium-density devices: Write protection of page 40 to 43 */
#define FLASH_WRProt_Pages44to47         ((uint32_t)0x00000800) /* CH32 Medium-density devices: Write protection of page 44 to 47 */
#define FLASH_WRProt_Pages48to51         ((uint32_t)0x00001000) /* CH32 Medium-density devices: Write protection of page 48 to 51 */
#define FLASH_WRProt_Pages52to55         ((uint32_t)0x00002000) /* CH32 Medium-density devices: Write protection of page 52 to 55 */
#define FLASH_WRProt_Pages56to59         ((uint32_t)0x00004000) /* CH32 Medium-density devices: Write protection of page 56 to 59 */
#define FLASH_WRProt_Pages60to63         ((uint32_t)0x00008000) /* CH32 Medium-density devices: Write protection of page 60 to 63 */
#define FLASH_WRProt_Pages64to67         ((uint32_t)0x00010000) /* CH32 Medium-density devices: Write protection of page 64 to 67 */
#define FLASH_WRProt_Pages68to71         ((uint32_t)0x00020000) /* CH32 Medium-density devices: Write protection of page 68 to 71 */
#define FLASH_WRProt_Pages72to75         ((uint32_t)0x00040000) /* CH32 Medium-density devices: Write protection of page 72 to 75 */
#define FLASH_WRProt_Pages76to79         ((uint32_t)0x00080000) /* CH32 Medium-density devices: Write protection of page 76 to 79 */
#define FLASH_WRProt_Pages80to83         ((uint32_t)0x00100000) /* CH32 Medium-density devices: Write protection of page 80 to 83 */
#define FLASH_WRProt_Pages84to87         ((uint32_t)0x00200000) /* CH32 Medium-density devices: Write protection of page 84 to 87 */
#define FLASH_WRProt_Pages88to91         ((uint32_t)0x00400000) /* CH32 Medium-density devices: Write protection of page 88 to 91 */
#define FLASH_WRProt_Pages92to95         ((uint32_t)0x00800000) /* CH32 Medium-density devices: Write protection of page 92 to 95 */
#define FLASH_WRProt_Pages96to99         ((uint32_t)0x01000000) /* CH32 Medium-density devices: Write protection of page 96 to 99 */
#define FLASH_WRProt_Pages100to103       ((uint32_t)0x02000000) /* CH32 Medium-density devices: Write protection of page 100 to 103 */
#define FLASH_WRProt_Pages104to107       ((uint32_t)0x04000000) /* CH32 Medium-density devices: Write protection of page 104 to 107 */
#define FLASH_WRProt_Pages108to111       ((uint32_t)0x08000000) /* CH32 Medium-density devices: Write protection of page 108 to 111 */
#define FLASH_WRProt_Pages112to115       ((uint32_t)0x10000000) /* CH32 Medium-density devices: Write protection of page 112 to 115 */
#define FLASH_WRProt_Pages116to119       ((uint32_t)0x20000000) /* CH32 Medium-density devices: Write protection of page 115 to 119 */
#define FLASH_WRProt_Pages120to123       ((uint32_t)0x40000000) /* CH32 Medium-density devices: Write protection of page 120 to 123 */
#define FLASH_WRProt_Pages124to127       ((uint32_t)0x80000000) /* CH32 Medium-density devices: Write protection of page 124 to 127 */

#define FLASH_WRProt_Pages62to255        ((uint32_t)0x80000000) /* CH32 Medium-density devices: Write protection of page 62 to 255 */

#define FLASH_WRProt_AllPages            ((uint32_t)0xFFFFFFFF) /* Write protection of all Pages */


/* Option_Bytes_IWatchdog */
#define OB_IWDG_SW                       ((uint16_t)0x0001) /* Software IWDG selected */
#define OB_IWDG_HW                       ((uint16_t)0x0000) /* Hardware IWDG selected */

/* Option_Bytes_nRST_STOP */
#define OB_STOP_NoRST                    ((uint16_t)0x0002) /* No reset generated when entering in STOP */
#define OB_STOP_RST                      ((uint16_t)0x0000) /* Reset generated when entering in STOP */

/* Option_Bytes_nRST_STDBY */
#define OB_STDBY_NoRST                   ((uint16_t)0x0004) /* No reset generated when entering in STANDBY */
#define OB_STDBY_RST                     ((uint16_t)0x0000) /* Reset generated when entering in STANDBY */


/* FLASH_Interrupts */
#define FLASH_IT_ERROR                   ((uint32_t)0x00000400) /* FPEC error interrupt source */
#define FLASH_IT_EOP                     ((uint32_t)0x00001000) /* End of FLASH Operation Interrupt source */
#define FLASH_IT_BANK1_ERROR             FLASH_IT_ERROR         /* FPEC BANK1 error interrupt source */
#define FLASH_IT_BANK1_EOP               FLASH_IT_EOP           /* End of FLASH BANK1 Operation Interrupt source */

/* FLASH_Flags */
#define FLASH_FLAG_BSY                   ((uint32_t)0x00000001) /* FLASH Busy flag */
#define FLASH_FLAG_EOP                   ((uint32_t)0x00000020) /* FLASH End of Operation flag */
#define FLASH_FLAG_PGERR                 ((uint32_t)0x00000004) /* FLASH Program error flag */
#define FLASH_FLAG_WRPRTERR              ((uint32_t)0x00000010) /* FLASH Write protected error flag */
#define FLASH_FLAG_OPTERR                ((uint32_t)0x00000001) /* FLASH Option Byte error flag */

#define FLASH_FLAG_BANK1_BSY             FLASH_FLAG_BSY       /* FLASH BANK1 Busy flag*/
#define FLASH_FLAG_BANK1_EOP             FLASH_FLAG_EOP       /* FLASH BANK1 End of Operation flag */
#define FLASH_FLAG_BANK1_PGERR           FLASH_FLAG_PGERR     /* FLASH BANK1 Program error flag */
#define FLASH_FLAG_BANK1_WRPRTERR        FLASH_FLAG_WRPRTERR  /* FLASH BANK1 Write protected error flag */




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
#define GPIO_Pin_8                      ((uint16_t)0x0100) /* Pin 8 selected */
#define GPIO_Pin_9                      ((uint16_t)0x0200) /* Pin 9 selected */
#define GPIO_Pin_10                     ((uint16_t)0x0400) /* Pin 10 selected */
#define GPIO_Pin_11                     ((uint16_t)0x0800) /* Pin 11 selected */
#define GPIO_Pin_12                     ((uint16_t)0x1000) /* Pin 12 selected */
#define GPIO_Pin_13                     ((uint16_t)0x2000) /* Pin 13 selected */
#define GPIO_Pin_14                     ((uint16_t)0x4000) /* Pin 14 selected */
#define GPIO_Pin_15                     ((uint16_t)0x8000) /* Pin 15 selected */
#define GPIO_Pin_All                   ((uint16_t)0xFFFF) /* All pins selected */

/* GPIO_Remap_define */

/* GPIO_Remap_define */
#define GPIO_Remap_SPI1                ((uint32_t)0x00000001) /* SPI1 Alternate Function mapping */
#define GPIO_Remap_I2C1                ((uint32_t)0x00000002) /* I2C1 Alternate Function mapping */
#define GPIO_Remap_USART1              ((uint32_t)0x00000004) /* USART1 Alternate Function mapping */
#define GPIO_Remap_USART2              ((uint32_t)0x00000008) /* USART2 Alternate Function mapping */
#define GPIO_PartialRemap_USART3       ((uint32_t)0x00140010) /* USART3 Partial Alternate Function mapping */
#define GPIO_FullRemap_USART3          ((uint32_t)0x00140030) /* USART3 Full Alternate Function mapping */
#define GPIO_PartialRemap_TIM1         ((uint32_t)0x00160040) /* TIM1 Partial Alternate Function mapping */
#define GPIO_FullRemap_TIM1            ((uint32_t)0x001600C0) /* TIM1 Full Alternate Function mapping */
#define GPIO_PartialRemap1_TIM2        ((uint32_t)0x00180100) /* TIM2 Partial1 Alternate Function mapping */
#define GPIO_PartialRemap2_TIM2        ((uint32_t)0x00180200) /* TIM2 Partial2 Alternate Function mapping */
#define GPIO_FullRemap_TIM2            ((uint32_t)0x00180300) /* TIM2 Full Alternate Function mapping */
#define GPIO_PartialRemap_TIM3         ((uint32_t)0x001A0800) /* TIM3 Partial Alternate Function mapping */
#define GPIO_FullRemap_TIM3            ((uint32_t)0x001A0C00) /* TIM3 Full Alternate Function mapping */
#define GPIO_Remap_TIM4                ((uint32_t)0x00001000) /* TIM4 Alternate Function mapping */
#define GPIO_Remap1_CAN1               ((uint32_t)0x001D4000) /* CAN1 Alternate Function mapping */
#define GPIO_Remap2_CAN1               ((uint32_t)0x001D6000) /* CAN1 Alternate Function mapping */
#define GPIO_Remap_PD0PD1              ((uint32_t)0x00008000) /* PD0 and PD1 Alternate Function mapping */
#define GPIO_Remap_ADC1_ETRGINJ        ((uint32_t)0x00200002) /* ADC1 External Trigger Injected Conversion remapping */
#define GPIO_Remap_ADC1_ETRGREG        ((uint32_t)0x00200004) /* ADC1 External Trigger Regular Conversion remapping */
#define GPIO_Remap_SWJ_Disable         ((uint32_t)0x00300400) /* Full SWJ Disabled (JTAG-DP + SW-DP) */
#define GPIO_Remap_TIM2ITR1_PTP_SOF    ((uint32_t)0x00202000) /* Ethernet PTP output or USB OTG SOF (Start of Frame) connected \
                                                                 to TIM2 Internal Trigger 1 for calibration                    \
                                                                 (only for Connectivity line devices) */
#define GPIO_Remap_TIM1_DMA            ((uint32_t)0x80000010) /* TIM1 DMA requests mapping (only for Value line devices) */
#define GPIO_Remap_TIM67_DAC_DMA       ((uint32_t)0x80000800) /* TIM6/TIM7 and DAC DMA requests remapping (only for High density Value line devices) */
#define GPIO_Remap_MISC                ((uint32_t)0x80002000) /* Miscellaneous Remap (DMA2 Channel5 Position and DAC Trigger remapping, \
                                                                   only for High density Value line devices) */
#define GPIO_Remap_PD01                GPIO_Remap_PD0PD1


/* GPIO_Port_Sources */
#define GPIO_PortSourceGPIOA           ((uint8_t)0x00)
#define GPIO_PortSourceGPIOB           ((uint8_t)0x01)
#define GPIO_PortSourceGPIOC           ((uint8_t)0x02)
#define GPIO_PortSourceGPIOD           ((uint8_t)0x03)
#define GPIO_PortSourceGPIOE           ((uint8_t)0x04)
#define GPIO_PortSourceGPIOF           ((uint8_t)0x05)
#define GPIO_PortSourceGPIOG           ((uint8_t)0x06)

/* GPIO_Pin_sources */
#define GPIO_PinSource0                ((uint8_t)0x00)
#define GPIO_PinSource1                ((uint8_t)0x01)
#define GPIO_PinSource2                ((uint8_t)0x02)
#define GPIO_PinSource3                ((uint8_t)0x03)
#define GPIO_PinSource4                ((uint8_t)0x04)
#define GPIO_PinSource5                ((uint8_t)0x05)
#define GPIO_PinSource6                ((uint8_t)0x06)
#define GPIO_PinSource7                ((uint8_t)0x07)
#define GPIO_PinSource8                 ((uint8_t)0x08)
#define GPIO_PinSource9                 ((uint8_t)0x09)
#define GPIO_PinSource10                ((uint8_t)0x0A)
#define GPIO_PinSource11                ((uint8_t)0x0B)
#define GPIO_PinSource12                ((uint8_t)0x0C)
#define GPIO_PinSource13                ((uint8_t)0x0D)
#define GPIO_PinSource14                ((uint8_t)0x0E)
#define GPIO_PinSource15                ((uint8_t)0x0F)


/* ch32v00x_i2c.h ------------------------------------------------------------*/

/* I2C_mode */
#define I2C_Mode_I2C                                         ((uint16_t)0x0000)
#define I2C_Mode_SMBusDevice                                 ((uint16_t)0x0002)
#define I2C_Mode_SMBusHost                                   ((uint16_t)0x000A)

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
#define I2C_Register_RTR                                     ((uint8_t)0x20)

/* I2C_SMBus_alert_pin_level */
#define I2C_SMBusAlert_Low                                   ((uint16_t)0x2000)
#define I2C_SMBusAlert_High                                  ((uint16_t)0xDFFF)

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
#define I2C_IT_SMBALERT                                      ((uint32_t)0x01008000)
#define I2C_IT_TIMEOUT                                       ((uint32_t)0x01004000)
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
#define I2C_FLAG_SMBHOST                                     ((uint32_t)0x00400000)
#define I2C_FLAG_SMBDEFAULT                                  ((uint32_t)0x00200000)
#define I2C_FLAG_GENCALL                                     ((uint32_t)0x00100000)
#define I2C_FLAG_TRA                                         ((uint32_t)0x00040000)
#define I2C_FLAG_BUSY                                        ((uint32_t)0x00020000)
#define I2C_FLAG_MSL                                         ((uint32_t)0x00010000)

/* SR1 register flags */
#define I2C_FLAG_SMBALERT                                    ((uint32_t)0x10008000)
#define I2C_FLAG_TIMEOUT                                     ((uint32_t)0x10004000)
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


/* OPA PSEL enumeration */
typedef enum
{
    CHP0 = 0,
    CHP1,
} OPA_PSEL_TypeDef;


/* OPA NSEL enumeration */
typedef enum
{
    CHN0 = 0,
    CHN1,
} OPA_NSEL_TypeDef;



/* OPA Init Structure definition */
typedef struct
{
    OPA_PSEL_TypeDef PSEL;    /* Specifies the positive channel of OPA */
    OPA_NSEL_TypeDef NSEL;    /* Specifies the negative channel of OPA */
} OPA_InitTypeDef;


/* PVD_detection_level  */
#define PWR_PVDLevel_2V7          ((uint32_t)0x00000000)
#define PWR_PVDLevel_2V9          ((uint32_t)0x00000020)
#define PWR_PVDLevel_3V1          ((uint32_t)0x00000040)
#define PWR_PVDLevel_3V3          ((uint32_t)0x00000060)
#define PWR_PVDLevel_3V5          ((uint32_t)0x00000080)
#define PWR_PVDLevel_3V8          ((uint32_t)0x000000A0)
#define PWR_PVDLevel_4V1          ((uint32_t)0x000000C0)
#define PWR_PVDLevel_4V4          ((uint32_t)0x000000E0)

/* Regulator_state_is_STOP_mode */
#define PWR_Regulator_ON          ((uint32_t)0x00000000)
#define PWR_Regulator_LowPower    ((uint32_t)0x00000001)

/* STOP_mode_entry */
#define PWR_STOPEntry_WFI         ((uint8_t)0x01)
#define PWR_STOPEntry_WFE         ((uint8_t)0x02)

/* PWR_Flag */
#define PWR_FLAG_WU               ((uint32_t)0x00000001)
#define PWR_FLAG_SB               ((uint32_t)0x00000002)
#define PWR_FLAG_PVDO             ((uint32_t)0x00000004)



/* ch32v00x_rcc.h ------------------------------------------------------------*/


/* HSE_configuration */
#define RCC_HSE_OFF                      ((uint32_t)0x00000000)
#define RCC_HSE_ON                       ((uint32_t)0x00010000)
#define RCC_HSE_Bypass                   ((uint32_t)0x00040000)


/* PLL_entry_clock_source */
#define RCC_PLLSource_HSI_Div2          ((uint32_t)0x00000000)
#define RCC_PLLSource_HSE_Div1          ((uint32_t)0x00010000)
#define RCC_PLLSource_HSE_Div2          ((uint32_t)0x00030000)

/* PLL_multiplication_factor */
#define RCC_PLLMul_2                    ((uint32_t)0x00000000)
#define RCC_PLLMul_3                    ((uint32_t)0x00040000)
#define RCC_PLLMul_4                    ((uint32_t)0x00080000)
#define RCC_PLLMul_5                    ((uint32_t)0x000C0000)
#define RCC_PLLMul_6                    ((uint32_t)0x00100000)
#define RCC_PLLMul_7                    ((uint32_t)0x00140000)
#define RCC_PLLMul_8                    ((uint32_t)0x00180000)
#define RCC_PLLMul_9                    ((uint32_t)0x001C0000)
#define RCC_PLLMul_10                   ((uint32_t)0x00200000)
#define RCC_PLLMul_11                   ((uint32_t)0x00240000)
#define RCC_PLLMul_12                   ((uint32_t)0x00280000)
#define RCC_PLLMul_13                   ((uint32_t)0x002C0000)
#define RCC_PLLMul_14                   ((uint32_t)0x00300000)
#define RCC_PLLMul_15                   ((uint32_t)0x00340000)
#define RCC_PLLMul_16                   ((uint32_t)0x00380000)

/* System_clock_source */
#define RCC_SYSCLKSource_HSI             ((uint32_t)0x00000000)
#define RCC_SYSCLKSource_HSE             ((uint32_t)0x00000001)
#define RCC_SYSCLKSource_PLLCLK          ((uint32_t)0x00000002)


/* AHB_clock_source */
#define RCC_SYSCLK_Div1                 ((uint32_t)0x00000000)
#define RCC_SYSCLK_Div2                 ((uint32_t)0x00000080)
#define RCC_SYSCLK_Div4                 ((uint32_t)0x00000090)
#define RCC_SYSCLK_Div8                 ((uint32_t)0x000000A0)
#define RCC_SYSCLK_Div16                ((uint32_t)0x000000B0)
#define RCC_SYSCLK_Div64                ((uint32_t)0x000000C0)
#define RCC_SYSCLK_Div128               ((uint32_t)0x000000D0)
#define RCC_SYSCLK_Div256               ((uint32_t)0x000000E0)
#define RCC_SYSCLK_Div512               ((uint32_t)0x000000F0)

/* APB1_APB2_clock_source */
#define RCC_HCLK_Div1                   ((uint32_t)0x00000000)
#define RCC_HCLK_Div2                   ((uint32_t)0x00000400)
#define RCC_HCLK_Div4                   ((uint32_t)0x00000500)
#define RCC_HCLK_Div8                   ((uint32_t)0x00000600)
#define RCC_HCLK_Div16                  ((uint32_t)0x00000700)


/* RCC_Interrupt_source */
#define RCC_IT_LSIRDY                    ((uint8_t)0x01)
#define RCC_IT_LSERDY                   ((uint8_t)0x02)
#define RCC_IT_HSIRDY                    ((uint8_t)0x04)
#define RCC_IT_HSERDY                    ((uint8_t)0x08)
#define RCC_IT_PLLRDY                    ((uint8_t)0x10)
#define RCC_IT_CSS                       ((uint8_t)0x80)

/* USB_Device_clock_source */
#define RCC_USBCLKSource_PLLCLK_1Div5    ((uint8_t)0x00)
#define RCC_USBCLKSource_PLLCLK_Div1     ((uint8_t)0x01)



/* ADC_clock_source */
#define RCC_PCLK2_Div2                 ((uint32_t)0x00000000)
#define RCC_PCLK2_Div4                 ((uint32_t)0x00004000)
#define RCC_PCLK2_Div6                 ((uint32_t)0x00008000)
#define RCC_PCLK2_Div8                 ((uint32_t)0x0000C000)

/* LSE_configuration */
#define RCC_LSE_OFF                    ((uint8_t)0x00)
#define RCC_LSE_ON                     ((uint8_t)0x01)
#define RCC_LSE_Bypass                 ((uint8_t)0x04)

/* RTC_clock_source */
#define RCC_RTCCLKSource_LSE           ((uint32_t)0x00000100)
#define RCC_RTCCLKSource_LSI           ((uint32_t)0x00000200)
#define RCC_RTCCLKSource_HSE_Div128    ((uint32_t)0x00000300)



/* AHB_peripheral */
#define RCC_AHBPeriph_DMA1             ((uint32_t)0x00000001)
#define RCC_AHBPeriph_DMA2             ((uint32_t)0x00000002)
#define RCC_AHBPeriph_SRAM             ((uint32_t)0x00000004)

#define RCC_AHBPeriph_FLITF            ((uint32_t)0x00000010)

#define RCC_AHBPeriph_CRC              ((uint32_t)0x00000040)
#define RCC_AHBPeriph_FSMC             ((uint32_t)0x00000100)


#define RCC_AHBPeriph_SDIO             ((uint32_t)0x00000400)

#define RCC_AHBPeriph_USBHD            ((uint32_t)0x00001000)


#define RCC_AHBPeriph_USBFS              RCC_AHBPeriph_USBHD

/* APB2_peripheral */
#define RCC_APB2Periph_AFIO            ((uint32_t)0x00000001)
#define RCC_APB2Periph_GPIOA           ((uint32_t)0x00000004)
#define RCC_APB2Periph_GPIOB           ((uint32_t)0x00000008)
#define RCC_APB2Periph_GPIOC           ((uint32_t)0x00000010)
#define RCC_APB2Periph_GPIOD           ((uint32_t)0x00000020)
#define RCC_APB2Periph_GPIOE           ((uint32_t)0x00000040)
#define RCC_APB2Periph_GPIOF             ((uint32_t)0x00000080)
#define RCC_APB2Periph_GPIOG             ((uint32_t)0x00000100)
#define RCC_APB2Periph_ADC1            ((uint32_t)0x00000200)
#define RCC_APB2Periph_ADC2            ((uint32_t)0x00000400)
#define RCC_APB2Periph_TIM1            ((uint32_t)0x00000800)
#define RCC_APB2Periph_SPI1            ((uint32_t)0x00001000)
#define RCC_APB2Periph_TIM8            ((uint32_t)0x00002000)
#define RCC_APB2Periph_USART1          ((uint32_t)0x00004000)
#define RCC_APB2Periph_ADC3              ((uint32_t)0x00008000)
#define RCC_APB2Periph_TIM15             ((uint32_t)0x00010000)
#define RCC_APB2Periph_TIM16             ((uint32_t)0x00020000)
#define RCC_APB2Periph_TIM17             ((uint32_t)0x00040000)
#define RCC_APB2Periph_TIM9              ((uint32_t)0x00080000)
#define RCC_APB2Periph_TIM10             ((uint32_t)0x00100000)
#define RCC_APB2Periph_TIM11             ((uint32_t)0x00200000)

/* APB1_peripheral */
#define RCC_APB1Periph_TIM2            ((uint32_t)0x00000001)
#define RCC_APB1Periph_TIM3            ((uint32_t)0x00000002)
#define RCC_APB1Periph_TIM4            ((uint32_t)0x00000004)
#define RCC_APB1Periph_TIM5            ((uint32_t)0x00000008)
#define RCC_APB1Periph_TIM6            ((uint32_t)0x00000010)
#define RCC_APB1Periph_TIM7            ((uint32_t)0x00000020)
#define RCC_APB1Periph_TIM12             ((uint32_t)0x00000040)
#define RCC_APB1Periph_TIM13             ((uint32_t)0x00000080)
#define RCC_APB1Periph_TIM14             ((uint32_t)0x00000100)
#define RCC_APB1Periph_WWDG            ((uint32_t)0x00000800)
#define RCC_APB1Periph_SPI2            ((uint32_t)0x00004000)
#define RCC_APB1Periph_SPI3            ((uint32_t)0x00008000)
#define RCC_APB1Periph_USART2          ((uint32_t)0x00020000)
#define RCC_APB1Periph_USART3          ((uint32_t)0x00040000)
#define RCC_APB1Periph_UART4           ((uint32_t)0x00080000)
#define RCC_APB1Periph_UART5           ((uint32_t)0x00100000)
#define RCC_APB1Periph_I2C1            ((uint32_t)0x00200000)
#define RCC_APB1Periph_I2C2            ((uint32_t)0x00400000)
#define RCC_APB1Periph_USB             ((uint32_t)0x00800000)
#define RCC_APB1Periph_CAN1            ((uint32_t)0x02000000)
#define RCC_APB1Periph_CAN2            ((uint32_t)0x04000000)
#define RCC_APB1Periph_BKP             ((uint32_t)0x08000000)
#define RCC_APB1Periph_PWR             ((uint32_t)0x10000000)
#define RCC_APB1Periph_DAC             ((uint32_t)0x20000000)
#define RCC_APB1Periph_CEC               ((uint32_t)0x40000000)


/* Clock_source_to_output_on_MCO_pin */
#define RCC_MCO_NoClock                  ((uint8_t)0x00)
#define RCC_MCO_SYSCLK                   ((uint8_t)0x04)
#define RCC_MCO_HSI                      ((uint8_t)0x05)
#define RCC_MCO_HSE                      ((uint8_t)0x06)
#define RCC_MCO_PLLCLK_Div2            	 ((uint8_t)0x07)


/* RCC_Flag */
#define RCC_FLAG_HSIRDY                  ((uint8_t)0x21)
#define RCC_FLAG_HSERDY                  ((uint8_t)0x31)
#define RCC_FLAG_PLLRDY                  ((uint8_t)0x39)
#define RCC_FLAG_LSERDY                	 ((uint8_t)0x41)
#define RCC_FLAG_LSIRDY                  ((uint8_t)0x61)
#define RCC_FLAG_PINRST                  ((uint8_t)0x7A)
#define RCC_FLAG_PORRST                  ((uint8_t)0x7B)
#define RCC_FLAG_SFTRST                  ((uint8_t)0x7C)
#define RCC_FLAG_IWDGRST                 ((uint8_t)0x7D)
#define RCC_FLAG_WWDGRST                 ((uint8_t)0x7E)
#define RCC_FLAG_LPWRRST                 ((uint8_t)0x7F)


/* SysTick_clock_source */
#define SysTick_CLKSource_HCLK_Div8      ((uint32_t)0xFFFFFFFB)
#define SysTick_CLKSource_HCLK           ((uint32_t)0x00000004)


/* ch32v00x_rtc.h ------------------------------------------------------------*/
/* RTC_interrupts_define */
#define RTC_IT_OW         ((uint16_t)0x0004) /* Overflow interrupt */
#define RTC_IT_ALR        ((uint16_t)0x0002) /* Alarm interrupt */
#define RTC_IT_SEC        ((uint16_t)0x0001) /* Second interrupt */

/* RTC_interrupts_flags */
#define RTC_FLAG_RTOFF    ((uint16_t)0x0020) /* RTC Operation OFF flag */
#define RTC_FLAG_RSF      ((uint16_t)0x0008) /* Registers Synchronized flag */
#define RTC_FLAG_OW       ((uint16_t)0x0004) /* Overflow flag */
#define RTC_FLAG_ALR      ((uint16_t)0x0002) /* Alarm flag */
#define RTC_FLAG_SEC      ((uint16_t)0x0001) /* Second flag */



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

#define SPI_FirstBit_LSB                   ((uint16_t)0x0080)


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
/* ch32v10x_usb.h ------------------------------------------------------------*/

#ifndef NULL
  #define NULL    0
#endif

#ifndef VOID
  #define VOID    void
#endif
#ifndef CONST
  #define CONST    const
#endif
#ifndef BOOL
typedef unsigned char BOOL;
#endif
#ifndef BOOLEAN
typedef unsigned char BOOLEAN;
#endif
#ifndef CHAR
typedef char CHAR;
#endif
#ifndef INT8
typedef char INT8;
#endif
#ifndef INT16
typedef short INT16;
#endif
#ifndef INT32
typedef long INT32;
#endif
#ifndef UINT8
typedef unsigned char UINT8;
#endif
#ifndef UINT16
typedef unsigned short UINT16;
#endif
#ifndef UINT32
typedef unsigned long UINT32;
#endif
#ifndef UINT8V
typedef unsigned char volatile UINT8V;
#endif
#ifndef UINT16V
typedef unsigned short volatile UINT16V;
#endif
#ifndef UINT32V
typedef unsigned long volatile UINT32V;
#endif

#ifndef PVOID
typedef void *PVOID;
#endif
#ifndef PCHAR
typedef char *PCHAR;
#endif
#ifndef PCHAR
typedef const char *PCCHAR;
#endif
#ifndef PINT8
typedef char *PINT8;
#endif
#ifndef PINT16
typedef short *PINT16;
#endif
#ifndef PINT32
typedef long *PINT32;
#endif
#ifndef PUINT8
typedef unsigned char *PUINT8;
#endif
#ifndef PUINT16
typedef unsigned short *PUINT16;
#endif
#ifndef PUINT32
typedef unsigned long *PUINT32;
#endif
#ifndef PUINT8V
typedef volatile unsigned char *PUINT8V;
#endif
#ifndef PUINT16V
typedef volatile unsigned short *PUINT16V;
#endif
#ifndef PUINT32V
typedef volatile unsigned long *PUINT32V;
#endif

/******************************************************************************/
/*                         Peripheral memory map                              */
/******************************************************************************/
/*       USB  */
#define R32_USB_CONTROL      (*((PUINT32V)(0x40023400))) // USB control & interrupt enable & device address
#define R8_USB_CTRL          (*((PUINT8V)(0x40023400)))  // USB base control
#define RB_UC_HOST_MODE      0x80                        // enable USB host mode: 0=device mode, 1=host mode
#define RB_UC_LOW_SPEED      0x40                        // enable USB low speed: 0=12Mbps, 1=1.5Mbps
#define RB_UC_DEV_PU_EN      0x20                        // USB device enable and internal pullup resistance enable
#define RB_UC_SYS_CTRL1      0x20                        // USB system control high bit
#define RB_UC_SYS_CTRL0      0x10                        // USB system control low bit
#define MASK_UC_SYS_CTRL     0x30                        // bit mask of USB system control
// bUC_HOST_MODE & bUC_SYS_CTRL1 & bUC_SYS_CTRL0: USB system control
//   0 00: disable USB device and disable internal pullup resistance
//   0 01: enable USB device and disable internal pullup resistance, need external pullup resistance
//   0 1x: enable USB device and enable internal pullup resistance
//   1 00: enable USB host and normal status
//   1 01: enable USB host and force UDP/UDM output SE0 state
//   1 10: enable USB host and force UDP/UDM output J state
//   1 11: enable USB host and force UDP/UDM output resume or K state
#define RB_UC_INT_BUSY       0x08           // enable automatic responding busy for device mode or automatic pause for host mode during interrupt flag UIF_TRANSFER valid
#define RB_UC_RESET_SIE      0x04           // force reset USB SIE, need software clear
#define RB_UC_CLR_ALL        0x02           // force clear FIFO and count of USB
#define RB_UC_DMA_EN         0x01           // DMA enable and DMA interrupt enable for USB

#define R8_UDEV_CTRL         (*((PUINT8V)(0x40023401))) // USB device physical prot control
#define RB_UD_PD_DIS         0x80                       // disable USB UDP/UDM pulldown resistance: 0=enable pulldown, 1=disable
#define RB_UD_DP_PIN         0x20                       // ReadOnly: indicate current UDP pin level
#define RB_UD_DM_PIN         0x10                       // ReadOnly: indicate current UDM pin level
#define RB_UD_LOW_SPEED      0x04                       // enable USB physical port low speed: 0=full speed, 1=low speed
#define RB_UD_GP_BIT         0x02                       // general purpose bit
#define RB_UD_PORT_EN        0x01                       // enable USB physical port I/O: 0=disable, 1=enable

#define R8_UHOST_CTRL        R8_UDEV_CTRL   // USB host physical prot control
#define RB_UH_PD_DIS         0x80           // disable USB UDP/UDM pulldown resistance: 0=enable pulldown, 1=disable
#define RB_UH_DP_PIN         0x20           // ReadOnly: indicate current UDP pin level
#define RB_UH_DM_PIN         0x10           // ReadOnly: indicate current UDM pin level
#define RB_UH_LOW_SPEED      0x04           // enable USB port low speed: 0=full speed, 1=low speed
#define RB_UH_BUS_RESET      0x02           // control USB bus reset: 0=normal, 1=force bus reset
#define RB_UH_PORT_EN        0x01           // enable USB port: 0=disable, 1=enable port, automatic disabled if USB device detached

#define R8_USB_INT_EN        (*((PUINT8V)(0x40023402))) // USB interrupt enable
#define RB_UIE_DEV_SOF       0x80                       // enable interrupt for SOF received for USB device mode
#define RB_UIE_DEV_NAK       0x40                       // enable interrupt for NAK responded for USB device mode
#define RB_UIE_FIFO_OV       0x10                       // enable interrupt for FIFO overflow
#define RB_UIE_HST_SOF       0x08                       // enable interrupt for host SOF timer action for USB host mode
#define RB_UIE_SUSPEND       0x04                       // enable interrupt for USB suspend or resume event
#define RB_UIE_TRANSFER      0x02                       // enable interrupt for USB transfer completion
#define RB_UIE_DETECT        0x01                       // enable interrupt for USB device detected event for USB host mode
#define RB_UIE_BUS_RST       0x01                       // enable interrupt for USB bus reset event for USB device mode

#define R8_USB_DEV_AD        (*((PUINT8V)(0x40023403))) // USB device address
#define RB_UDA_GP_BIT        0x80                       // general purpose bit
#define MASK_USB_ADDR        0x7F                       // bit mask for USB device address

#define R32_USB_STATUS       (*((PUINT32V)(0x40023404))) // USB miscellaneous status & interrupt flag & interrupt status
#define R8_USB_MIS_ST        (*((PUINT8V)(0x40023405)))  // USB miscellaneous status
#define RB_UMS_SOF_PRES      0x80                        // RO, indicate host SOF timer presage status
#define RB_UMS_SOF_ACT       0x40                        // RO, indicate host SOF timer action status for USB host
#define RB_UMS_SIE_FREE      0x20                        // RO, indicate USB SIE free status
#define RB_UMS_R_FIFO_RDY    0x10                        // RO, indicate USB receiving FIFO ready status (not empty)
#define RB_UMS_BUS_RESET     0x08                        // RO, indicate USB bus reset status
#define RB_UMS_SUSPEND       0x04                        // RO, indicate USB suspend status
#define RB_UMS_DM_LEVEL      0x02                        // RO, indicate UDM level saved at device attached to USB host
#define RB_UMS_DEV_ATTACH    0x01                        // RO, indicate device attached status on USB host

#define R8_USB_INT_FG        (*((PUINT8V)(0x40023406))) // USB interrupt flag
#define RB_U_IS_NAK          0x80                       // RO, indicate current USB transfer is NAK received
#define RB_U_TOG_OK          0x40                       // RO, indicate current USB transfer toggle is OK
#define RB_U_SIE_FREE        0x20                       // RO, indicate USB SIE free status
#define RB_UIF_FIFO_OV       0x10                       // FIFO overflow interrupt flag for USB, direct bit address clear or write 1 to clear
#define RB_UIF_HST_SOF       0x08                       // host SOF timer interrupt flag for USB host, direct bit address clear or write 1 to clear
#define RB_UIF_SUSPEND       0x04                       // USB suspend or resume event interrupt flag, direct bit address clear or write 1 to clear
#define RB_UIF_TRANSFER      0x02                       // USB transfer completion interrupt flag, direct bit address clear or write 1 to clear
#define RB_UIF_DETECT        0x01                       // device detected event interrupt flag for USB host mode, direct bit address clear or write 1 to clear
#define RB_UIF_BUS_RST       0x01                       // bus reset event interrupt flag for USB device mode, direct bit address clear or write 1 to clear

#define R8_USB_INT_ST        (*((PUINT8V)(0x40023407))) // USB interrupt status
#define RB_UIS_IS_NAK        0x80                       // RO, indicate current USB transfer is NAK received for USB device mode
#define RB_UIS_TOG_OK        0x40                       // RO, indicate current USB transfer toggle is OK
#define RB_UIS_TOKEN1        0x20                       // RO, current token PID code bit 1 received for USB device mode
#define RB_UIS_TOKEN0        0x10                       // RO, current token PID code bit 0 received for USB device mode
#define MASK_UIS_TOKEN       0x30                       // RO, bit mask of current token PID code received for USB device mode
#define UIS_TOKEN_OUT        0x00
#define UIS_TOKEN_SOF        0x10
#define UIS_TOKEN_IN         0x20
#define UIS_TOKEN_SETUP      0x30
// bUIS_TOKEN1 & bUIS_TOKEN0: current token PID code received for USB device mode
//   00: OUT token PID received
//   01: SOF token PID received
//   10: IN token PID received
//   11: SETUP token PID received
#define MASK_UIS_ENDP        0x0F           // RO, bit mask of current transfer endpoint number for USB device mode
#define MASK_UIS_H_RES       0x0F           // RO, bit mask of current transfer handshake response for USB host mode: 0000=no response, time out from device, others=handshake response PID received

#define R16_USB_RX_LEN       (*((PUINT16V)(0x40023408))) // USB receiving length
#define MASK_UIS_RX_LEN      0x3FF                       // RO, bit mask of current receive length(10 bits for ch32v10x)
#define R32_USB_BUF_MODE     (*((PUINT32V)(0x4002340c))) // USB endpoint buffer mode
#define R8_UEP4_1_MOD        (*((PUINT8V)(0x4002340c)))  // endpoint 4/1 mode
#define RB_UEP1_RX_EN        0x80                        // enable USB endpoint 1 receiving (OUT)
#define RB_UEP1_TX_EN        0x40                        // enable USB endpoint 1 transmittal (IN)
#define RB_UEP1_BUF_MOD      0x10                        // buffer mode of USB endpoint 1
// bUEPn_RX_EN & bUEPn_TX_EN & bUEPn_BUF_MOD: USB endpoint 1/2/3 buffer mode, buffer start address is UEPn_DMA
//   0 0 x:  disable endpoint and disable buffer
//   1 0 0:  64 bytes buffer for receiving (OUT endpoint)
//   1 0 1:  dual 64 bytes buffer by toggle bit bUEP_R_TOG selection for receiving (OUT endpoint), total=128bytes
//   0 1 0:  64 bytes buffer for transmittal (IN endpoint)
//   0 1 1:  dual 64 bytes buffer by toggle bit bUEP_T_TOG selection for transmittal (IN endpoint), total=128bytes
//   1 1 0:  64 bytes buffer for receiving (OUT endpoint) + 64 bytes buffer for transmittal (IN endpoint), total=128bytes
//   1 1 1:  dual 64 bytes buffer by bUEP_R_TOG selection for receiving (OUT endpoint) + dual 64 bytes buffer by bUEP_T_TOG selection for transmittal (IN endpoint), total=256bytes
#define RB_UEP4_RX_EN        0x08           // enable USB endpoint 4 receiving (OUT)
#define RB_UEP4_TX_EN        0x04           // enable USB endpoint 4 transmittal (IN)
// bUEP4_RX_EN & bUEP4_TX_EN: USB endpoint 4 buffer mode, buffer start address is UEP0_DMA
//   0 0:  single 64 bytes buffer for endpoint 0 receiving & transmittal (OUT & IN endpoint)
//   1 0:  single 64 bytes buffer for endpoint 0 receiving & transmittal (OUT & IN endpoint) + 64 bytes buffer for endpoint 4 receiving (OUT endpoint), total=128bytes
//   0 1:  single 64 bytes buffer for endpoint 0 receiving & transmittal (OUT & IN endpoint) + 64 bytes buffer for endpoint 4 transmittal (IN endpoint), total=128bytes
//   1 1:  single 64 bytes buffer for endpoint 0 receiving & transmittal (OUT & IN endpoint)
//           + 64 bytes buffer for endpoint 4 receiving (OUT endpoint) + 64 bytes buffer for endpoint 4 transmittal (IN endpoint), total=192bytes

#define R8_UEP2_3_MOD        (*((PUINT8V)(0x4002340d))) // endpoint 2/3 mode
#define RB_UEP3_RX_EN        0x80                       // enable USB endpoint 3 receiving (OUT)
#define RB_UEP3_TX_EN        0x40                       // enable USB endpoint 3 transmittal (IN)
#define RB_UEP3_BUF_MOD      0x10                       // buffer mode of USB endpoint 3
#define RB_UEP2_RX_EN        0x08                       // enable USB endpoint 2 receiving (OUT)
#define RB_UEP2_TX_EN        0x04                       // enable USB endpoint 2 transmittal (IN)
#define RB_UEP2_BUF_MOD      0x01                       // buffer mode of USB endpoint 2

#define R8_UH_EP_MOD         R8_UEP2_3_MOD  //host endpoint mode
#define RB_UH_EP_TX_EN       0x40           // enable USB host OUT endpoint transmittal
#define RB_UH_EP_TBUF_MOD    0x10           // buffer mode of USB host OUT endpoint
// bUH_EP_TX_EN & bUH_EP_TBUF_MOD: USB host OUT endpoint buffer mode, buffer start address is UH_TX_DMA
//   0 x:  disable endpoint and disable buffer
//   1 0:  64 bytes buffer for transmittal (OUT endpoint)
//   1 1:  dual 64 bytes buffer by toggle bit bUH_T_TOG selection for transmittal (OUT endpoint), total=128bytes
#define RB_UH_EP_RX_EN       0x08           // enable USB host IN endpoint receiving
#define RB_UH_EP_RBUF_MOD    0x01           // buffer mode of USB host IN endpoint
// bUH_EP_RX_EN & bUH_EP_RBUF_MOD: USB host IN endpoint buffer mode, buffer start address is UH_RX_DMA
//   0 x:  disable endpoint and disable buffer
//   1 0:  64 bytes buffer for receiving (IN endpoint)
//   1 1:  dual 64 bytes buffer by toggle bit bUH_R_TOG selection for receiving (IN endpoint), total=128bytes

#define R8_UEP5_6_MOD        (*((PUINT8V)(0x4002340e))) // endpoint 5/6 mode
#define RB_UEP6_RX_EN        0x80                       // enable USB endpoint 6 receiving (OUT)
#define RB_UEP6_TX_EN        0x40                       // enable USB endpoint 6 transmittal (IN)
#define RB_UEP6_BUF_MOD      0x10                       // buffer mode of USB endpoint 6
#define RB_UEP5_RX_EN        0x08                       // enable USB endpoint 5 receiving (OUT)
#define RB_UEP5_TX_EN        0x04                       // enable USB endpoint 5 transmittal (IN)
#define RB_UEP5_BUF_MOD      0x01                       // buffer mode of USB endpoint 5

#define R8_UEP7_MOD          (*((PUINT8V)(0x4002340f))) // endpoint 7 mode
#define RB_UEP7_RX_EN        0x08                       // enable USB endpoint 7 receiving (OUT)
#define RB_UEP7_TX_EN        0x04                       // enable USB endpoint 7 transmittal (IN)
#define RB_UEP7_BUF_MOD      0x01                       // buffer mode of USB endpoint 7

#define R16_UEP0_DMA         (*((PUINT16V)(0x40023410))) // endpoint 0 DMA buffer address
#define R16_UEP1_DMA         (*((PUINT16V)(0x40023414))) // endpoint 1 DMA buffer address
#define R16_UEP2_DMA         (*((PUINT16V)(0x40023418))) // endpoint 2 DMA buffer address
#define R16_UH_RX_DMA        R16_UEP2_DMA                // host rx endpoint buffer high address
#define R16_UEP3_DMA         (*((PUINT16V)(0x4002341c))) // endpoint 3 DMA buffer address

#define R16_UEP4_DMA         (*((PUINT16V)(0x40023420))) // endpoint 4 DMA buffer address
#define R16_UEP5_DMA         (*((PUINT16V)(0x40023424))) // endpoint 5 DMA buffer address
#define R16_UEP6_DMA         (*((PUINT16V)(0x40023428))) // endpoint 6 DMA buffer address
#define R16_UEP7_DMA         (*((PUINT16V)(0x4002342c))) // endpoint 7 DMA buffer address

#define R16_UH_TX_DMA        R16_UEP3_DMA                // host tx endpoint buffer high address
#define R32_USB_EP0_CTRL     (*((PUINT32V)(0x40023430))) // endpoint 0 control & transmittal length
#define R8_UEP0_T_LEN        (*((PUINT8V)(0x40023430)))  // endpoint 0 transmittal length
#define R8_UEP0_CTRL         (*((PUINT8V)(0x40023432)))  // endpoint 0 control
#define R32_USB_EP1_CTRL     (*((PUINT32V)(0x40023434))) // endpoint 1 control & transmittal length
#define R16_UEP1_T_LEN       (*((PUINT16V)(0x40023434))) // endpoint 1 transmittal length(16-bits for ch32v10x)
#define R8_UEP1_CTRL         (*((PUINT8V)(0x40023436)))  // endpoint 1 control
#define RB_UEP_R_TOG         0x80                        // expected data toggle flag of USB endpoint X receiving (OUT): 0=DATA0, 1=DATA1
#define RB_UEP_T_TOG         0x40                        // prepared data toggle flag of USB endpoint X transmittal (IN): 0=DATA0, 1=DATA1
#define RB_UEP_AUTO_TOG      0x10                        // enable automatic toggle after successful transfer completion on endpoint 1/2/3: 0=manual toggle, 1=automatic toggle
#define RB_UEP_R_RES1        0x08                        // handshake response type high bit for USB endpoint X receiving (OUT)
#define RB_UEP_R_RES0        0x04                        // handshake response type low bit for USB endpoint X receiving (OUT)
#define MASK_UEP_R_RES       0x0C                        // bit mask of handshake response type for USB endpoint X receiving (OUT)
#define UEP_R_RES_ACK        0x00
#define UEP_R_RES_TOUT       0x04
#define UEP_R_RES_NAK        0x08
#define UEP_R_RES_STALL      0x0C
// RB_UEP_R_RES1 & RB_UEP_R_RES0: handshake response type for USB endpoint X receiving (OUT)
//   00: ACK (ready)
//   01: no response, time out to host, for non-zero endpoint isochronous transactions
//   10: NAK (busy)
//   11: STALL (error)
#define RB_UEP_T_RES1        0x02           // handshake response type high bit for USB endpoint X transmittal (IN)
#define RB_UEP_T_RES0        0x01           // handshake response type low bit for USB endpoint X transmittal (IN)
#define MASK_UEP_T_RES       0x03           // bit mask of handshake response type for USB endpoint X transmittal (IN)
#define UEP_T_RES_ACK        0x00
#define UEP_T_RES_TOUT       0x01
#define UEP_T_RES_NAK        0x02
#define UEP_T_RES_STALL      0x03
// bUEP_T_RES1 & bUEP_T_RES0: handshake response type for USB endpoint X transmittal (IN)
//   00: DATA0 or DATA1 then expecting ACK (ready)
//   01: DATA0 or DATA1 then expecting no response, time out from host, for non-zero endpoint isochronous transactions
//   10: NAK (busy)
//   11: STALL (error)

#define R8_UH_SETUP          R8_UEP1_CTRL   // host aux setup
#define RB_UH_PRE_PID_EN     0x80           // USB host PRE PID enable for low speed device via hub
#define RB_UH_SOF_EN         0x40           // USB host automatic SOF enable

#define R32_USB_EP2_CTRL     (*((PUINT32V)(0x40023438))) // endpoint 2 control & transmittal length
#define R16_UEP2_T_LEN       (*((PUINT16V)(0x40023438))) // endpoint 2 transmittal length(16-bits for ch32v10x)
#define R8_UEP2_CTRL         (*((PUINT8V)(0x4002343a)))  // endpoint 2 control

#define R8_UH_EP_PID         (*((PUINT8V)(0x40023438)))  // host endpoint and PID
#define MASK_UH_TOKEN        0xF0           // bit mask of token PID for USB host transfer
#define MASK_UH_ENDP         0x0F           // bit mask of endpoint number for USB host transfer

#define R8_UH_RX_CTRL        R8_UEP2_CTRL   // host receiver endpoint control
#define RB_UH_R_TOG          0x80           // expected data toggle flag of host receiving (IN): 0=DATA0, 1=DATA1
#define RB_UH_R_AUTO_TOG     0x10           // enable automatic toggle after successful transfer completion: 0=manual toggle, 1=automatic toggle
#define RB_UH_R_RES          0x04           // prepared handshake response type for host receiving (IN): 0=ACK (ready), 1=no response, time out to device, for isochronous transactions

#define R32_USB_EP3_CTRL     (*((PUINT32V)(0x4002343c))) // endpoint 3 control & transmittal length
#define R16_UEP3_T_LEN       (*((PUINT16V)(0x4002343c))) // endpoint 3 transmittal length(16-bits for ch32v10x)
#define R8_UEP3_CTRL         (*((PUINT8V)(0x4002343e)))  // endpoint 3 control
#define R8_UH_TX_LEN         (*((PUINT16V)(0x4002343c))) //R8_UEP3_T_LEN				// host transmittal endpoint transmittal length

#define R8_UH_TX_CTRL        R8_UEP3_CTRL   // host transmittal endpoint control
#define RB_UH_T_TOG          0x40           // prepared data toggle flag of host transmittal (SETUP/OUT): 0=DATA0, 1=DATA1
#define RB_UH_T_AUTO_TOG     0x10           // enable automatic toggle after successful transfer completion: 0=manual toggle, 1=automatic toggle
#define RB_UH_T_RES          0x01           // expected handshake response type for host transmittal (SETUP/OUT): 0=ACK (ready), 1=no response, time out from device, for isochronous transactions

#define R32_USB_EP4_CTRL     (*((PUINT32V)(0x40023440))) // endpoint 4 control & transmittal length
#define R16_UEP4_T_LEN       (*((PUINT16V)(0x40023440))) // endpoint 4 transmittal length(16-bits for ch32v10x)
#define R8_UEP4_CTRL         (*((PUINT8V)(0x40023442)))  // endpoint 4 control

#define R32_USB_EP5_CTRL     (*((PUINT32V)(0x40023444))) // endpoint 5 control & transmittal length
#define R16_UEP5_T_LEN       (*((PUINT16V)(0x40023444))) // endpoint 5 transmittal length(16-bits for ch32v10x)
#define R8_UEP5_CTRL         (*((PUINT8V)(0x40023446)))  // endpoint 5 control

#define R32_USB_EP6_CTRL     (*((PUINT32V)(0x40023448))) // endpoint 6 control & transmittal length
#define R16_UEP6_T_LEN       (*((PUINT16V)(0x40023448))) // endpoint 6 transmittal length(16-bits for ch32v10x)
#define R8_UEP6_CTRL         (*((PUINT8V)(0x4002344a)))  // endpoint 6 control

#define R32_USB_EP7_CTRL     (*((PUINT32V)(0x4002344c))) // endpoint 7 control & transmittal length
#define R16_UEP7_T_LEN       (*((PUINT16V)(0x4002344c))) // endpoint 7 transmittal length(16-bits for ch32v10x)
#define R8_UEP7_CTRL         (*((PUINT8V)(0x4002344e)))  // endpoint 7 control

/* ch32v10x_usb_host.h -----------------------------------------------------------*/

#define ERR_SUCCESS            0x00
#define ERR_USB_CONNECT        0x15
#define ERR_USB_DISCON         0x16
#define ERR_USB_BUF_OVER       0x17
#define ERR_USB_DISK_ERR       0x1F
#define ERR_USB_TRANSFER       0x20
#define ERR_USB_UNSUPPORT      0xFB
#define ERR_USB_UNKNOWN        0xFE
#define ERR_AOA_PROTOCOL       0x41

#define ROOT_DEV_DISCONNECT    0
#define ROOT_DEV_CONNECTED     1
#define ROOT_DEV_FAILED        2
#define ROOT_DEV_SUCCESS       3
#define DEV_TYPE_KEYBOARD      (USB_DEV_CLASS_HID | 0x20)
#define DEV_TYPE_MOUSE         (USB_DEV_CLASS_HID | 0x30)
#define DEF_AOA_DEVICE         0xF0
#define DEV_TYPE_UNKNOW        0xFF

#define HUB_MAX_PORTS          4
#define WAIT_USB_TOUT_200US    3000


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
	#define DMDATA0 ((volatile uint32_t*)0xe0000380)
	#define DMDATA1 ((volatile uint32_t*)0xe0000384)
	#define DMSTATUS_SENTINEL ((volatile uint32_t*)0xe0000388)// Reads as 0x00000000 if debugger is attached.

// Determination of PLL multiplication factor for non-V003 chips
	#if !defined(FUNCONF_SYSTEM_CORE_CLOCK)
		#define PLL_MULTIPLICATION ((uint32_t)0)
	#else
		#if defined(CH32V10x) || defined(CH32V20x) || defined(CH32V30x_D8)
			#if FUNCONF_PLL_MULTIPLIER == 2
				#define PLL_MULTIPLICATION RCC_PLLMULL2
			#elif FUNCONF_PLL_MULTIPLIER == 3
				#define PLL_MULTIPLICATION RCC_PLLMULL3
			#elif FUNCONF_PLL_MULTIPLIER == 4
				#define PLL_MULTIPLICATION RCC_PLLMULL4
			#elif FUNCONF_PLL_MULTIPLIER == 5
				#define PLL_MULTIPLICATION RCC_PLLMULL5
			#elif FUNCONF_PLL_MULTIPLIER == 6
				#define PLL_MULTIPLICATION RCC_PLLMULL6
			#elif FUNCONF_PLL_MULTIPLIER == 7
				#define PLL_MULTIPLICATION RCC_PLLMULL7
			#elif FUNCONF_PLL_MULTIPLIER == 8
				#define PLL_MULTIPLICATION RCC_PLLMULL8
			#elif FUNCONF_PLL_MULTIPLIER == 9
				#define PLL_MULTIPLICATION RCC_PLLMULL9
			#elif FUNCONF_PLL_MULTIPLIER == 10
				#define PLL_MULTIPLICATION RCC_PLLMULL10
			#elif FUNCONF_PLL_MULTIPLIER == 11
				#define PLL_MULTIPLICATION RCC_PLLMULL11
			#elif FUNCONF_PLL_MULTIPLIER == 12
				#define PLL_MULTIPLICATION RCC_PLLMULL12
			#elif FUNCONF_PLL_MULTIPLIER == 13
				#define PLL_MULTIPLICATION RCC_PLLMULL13
			#elif FUNCONF_PLL_MULTIPLIER == 14
				#define PLL_MULTIPLICATION RCC_PLLMULL14
			#elif FUNCONF_PLL_MULTIPLIER == 15
				#define PLL_MULTIPLICATION RCC_PLLMULL15
			#elif FUNCONF_PLL_MULTIPLIER == 16
				#define PLL_MULTIPLICATION RCC_PLLMULL16
			#elif defined(CH32V20x) && FUNCONF_PLL_MULTIPLIER == 18
				#define PLL_MULTIPLICATION RCC_PLLMULL18
			#else
				#error "Invalid PLL multiplier"
			#endif
		#else
			#if FUNCONF_PLL_MULTIPLIER == 3
				#define PLL_MULTIPLICATION RCC_PLLMULL3_EXTEN
			#elif FUNCONF_PLL_MULTIPLIER == 4
				#define PLL_MULTIPLICATION RCC_PLLMULL4_EXTEN
			#elif FUNCONF_PLL_MULTIPLIER == 5
				#define PLL_MULTIPLICATION RCC_PLLMULL5_EXTEN
			#elif FUNCONF_PLL_MULTIPLIER == 6
				#define PLL_MULTIPLICATION RCC_PLLMULL6_EXTEN
			#elif FUNCONF_PLL_MULTIPLIER == 7
				#define PLL_MULTIPLICATION RCC_PLLMULL7_EXTEN
			#elif FUNCONF_PLL_MULTIPLIER == 8
				#define PLL_MULTIPLICATION RCC_PLLMULL8_EXTEN
			#elif FUNCONF_PLL_MULTIPLIER == 9
				#define PLL_MULTIPLICATION RCC_PLLMULL9_EXTEN
			#elif FUNCONF_PLL_MULTIPLIER == 10
				#define PLL_MULTIPLICATION RCC_PLLMULL10_EXTEN
			#elif FUNCONF_PLL_MULTIPLIER == 11
				#define PLL_MULTIPLICATION RCC_PLLMULL11_EXTEN
			#elif FUNCONF_PLL_MULTIPLIER == 12
				#define PLL_MULTIPLICATION RCC_PLLMULL12_EXTEN
			#elif FUNCONF_PLL_MULTIPLIER == 13
				#define PLL_MULTIPLICATION RCC_PLLMULL13_EXTEN
			#elif FUNCONF_PLL_MULTIPLIER == 14
				#define PLL_MULTIPLICATION RCC_PLLMULL14_EXTEN
			#elif FUNCONF_PLL_MULTIPLIER == 15
				#define PLL_MULTIPLICATION RCC_PLLMULL15_EXTEN
			#elif FUNCONF_PLL_MULTIPLIER == 16
				#define PLL_MULTIPLICATION RCC_PLLMULL16_EXTEN
			#elif FUNCONF_PLL_MULTIPLIER == 18
				#define PLL_MULTIPLICATION RCC_PLLMULL18_EXTEN
			#else
				#error "Invalid PLL multiplier"
			#endif
		#endif
	#endif




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


#define PA1 1
#define PA2 2
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

#define PA0 0
#define PA3 3
#define PA4 4
#define PA5 5
#define PA6 6
#define PA7 7
#define PA8 8
#define PA9 9
#define PA10 10
#define PA11 11
#define PA12 12
#define PA13 13
#define PA14 14
#define PA15 15
#define PB0 16
#define PB1 17
#define PB2 18
#define PB3 19
#define PB4 20
#define PB5 21
#define PB6 22
#define PB7 23
#define PB8 24
#define PB9 25
#define PB10 26
#define PB11 27
#define PB12 28
#define PB13 29
#define PB14 30
#define PB15 31
#define PC8 40
#define PC9 41
#define PC10 42
#define PC11 43
#define PC12 44
#define PC13 45
#define PC14 46
#define PC15 47
#define PD8 56
#define PD9 57
#define PD10 58
#define PD11 59
#define PD12 60
#define PD13 61
#define PD14 62
#define PD15 63

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

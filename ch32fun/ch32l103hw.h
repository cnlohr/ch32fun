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
    SysTick_IRQn = 12,       /* 12 System timer Interrupt                            */
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
	ADC_IRQn = 34,             /* ADC1 global Interrupt                                */
	USB_HP_CAN1_TX_IRQn = 35,  /* USB Device High Priority or CAN1 TX Interrupts       */
	USB_LP_CAN1_RX0_IRQn = 36, /* USB Device Low Priority or CAN1 RX0 Interrupts       */
	CAN1_RX1_IRQn = 37,        /* CAN1 RX1 Interrupt                                   */
	CAN1_SCE_IRQn = 38,        /* CAN1 SCE Interrupt                                   */
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
	LPTIMWakeUp_IRQn = 58,     /* LPTIM WakeUp Interrupt 	                          */
	USBFS_IRQn = 59,           /* USBFS global Interrupt                               */
	USBFSWakeUp_IRQn = 60,     /* USBFS WakeUp Interrupt                               */
	USART4_IRQn = 61,          /* USART4 global Interrupt                              */
	DMA1_Channel8_IRQn = 62,   /* DMA1 Channel 8 global Interrupt                      */
	LPTIM_IRQn = 63,           /* LPTIM global Interrupt                               */
	OPA_IRQn = 64,             /* OPA global Interrupt                                 */
	USBPD_IRQn = 65,           /* USBPD global Interrupt                               */
	
	USBPDWakeUp_IRQn = 67,     /* USBPD WakeUp Interrupt                               */
	CMPWakeUp_IRQn = 68,       /* CMP WakeUp Interrupt                                 */

} IRQn_Type;

#define BASE_VECTOR "\n\
	.balign  2\n\
	.option   push;\n\
	.option   norvc;\n\
	j handle_reset\n\
	.word   0 \n\
	.word   NMI_Handler                /* NMI */ \n\
	.word   HardFault_Handler          /* Hard Fault */ \n\
	.word   0 \n\
	.word   Ecall_M_Mode_Handler       /* Ecall M Mode */ \n\
	.word   0 \n\
	.word   0 \n\
	.word   Ecall_U_Mode_Handler       /* Ecall U Mode */ \n\
	.word   Break_Point_Handler        /* Break Point */ \n\
	.word   0 \n\
	.word   0 \n\
	.word   SysTick_Handler            /* SysTick */ \n \
	.word   0 \n \
	.word   SW_Handler                 /* SW */ \n \
	.word   0 \n \
	/* External Interrupts */ \n \
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
	.word   USB_HP_CAN1_TX_IRQHandler  /* USB HP and CAN1 TX */ \n \
	.word   USB_LP_CAN1_RX0_IRQHandler /* USB LP and CAN1RX0 */ \n \
	.word   CAN1_RX1_IRQHandler        /* CAN1 RX1 */ \n \
	.word   CAN1_SCE_IRQHandler        /* CAN1 SCE */ \n \
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
	.word   LPTIMWakeUp_IRQHandler     /* LPTIM Wake up from suspend */ \n \
	.word   USBFS_IRQHandler           /* USBFS Break */ \n \
	.word   USBFSWakeUp_IRQHandler     /* USBFS Wake up from suspend */ \n \
	.word   USART4_IRQHandler          /* USART4 */ \n \
	.word	DMA1_Channel8_IRQHandler   /* DMA1 Channel 8 global Interrupt */ \n\
	.word   LPTIM_IRQHandler           /* LPTIM global Interrupt */ \n \
	.word   OPA_IRQHandler             /* OPA global Interrupt */ \n \
	.word   USBPD_IRQHandler           /* USBPD global Interrupt */ \n \
	.word   USBPD_WKUP_IRQHandler      /* USBPD WakeUp Interrupt */ \n \
	.word   CMPWakeUp_IRQHandler       /* CMP WakeUp Interrupt */ \n"

#define EXTRA_VECTOR ""

#define DEFAULT_INTERRUPT_VECTOR_CONTENTS BASE_VECTOR EXTRA_VECTOR "\n.option pop;\n"

/* memory mapped structure for SysTick */
typedef struct
{
	__IO uint32_t CTLR;
	__IO uint32_t SR;
	__IO uint64_t CNT;
	__IO uint64_t CMP;
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
    __IO uint32_t CFG;
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
	uint16_t      RESERVED13;
} BKP_TypeDef;

/* Controller Area Network TxMailBox */
typedef struct
{
	__IO uint32_t TXMIR;
	__IO uint32_t TXMDTR;
	__IO uint32_t TXMDLR;
	__IO uint32_t TXMDHR;
} CAN_TxMailBox_TypeDef;

/* Controller Area Network FIFOMailBox */
typedef struct
{
	__IO uint32_t RXMIR;
	__IO uint32_t RXMDTR;
	__IO uint32_t RXMDLR;
	__IO uint32_t RXMDHR;
} CAN_FIFOMailBox_TypeDef;

/* Controller Area Network FilterRegister */
typedef struct
{
	__IO uint32_t FR1;
	__IO uint32_t FR2;
} CAN_FilterRegister_TypeDef;

/* Controller Area Network */
typedef struct
{
	__IO uint32_t              CTLR;
	__IO uint32_t              STATR;
	__IO uint32_t              TSTATR;
	__IO uint32_t              RFIFO0;
	__IO uint32_t              RFIFO1;
	__IO uint32_t              INTENR;
	__IO uint32_t              ERRSR;
	__IO uint32_t              BTIMR;
	__IO uint32_t              TTCTLR;
	__IO uint32_t              TTCNT;
	__IO uint32_t              TERR_CNT;
	__IO uint32_t              CANFD_CR;
	__IO uint32_t              CANFD_BTR;
	__IO uint32_t              CANFD_TDCT;
	__IO uint32_t              CANFD_PSR;
	__IO uint32_t              CANFD_DMA_T[3];
	__IO uint32_t              CANFD_DMA_R[2];
	uint32_t                   RESERVED0[76];
	CAN_TxMailBox_TypeDef      sTxMailBox[3];
	CAN_FIFOMailBox_TypeDef    sFIFOMailBox[2];
	uint32_t                   RESERVED1[12];
	__IO uint32_t              FCTLR;
	__IO uint32_t              FMCFGR;
	uint32_t                   RESERVED2;
	__IO uint32_t              FSCFGR;
	uint32_t                   RESERVED3;
	__IO uint32_t              FAFIFOR;
	uint32_t                   RESERVED4;
	__IO uint32_t              FWR;
	uint32_t                   RESERVED5[8];
	CAN_FilterRegister_TypeDef sFilterRegister[14];
} CAN_TypeDef;

/* CRC Calculation Unit */
typedef struct
{
	__IO uint32_t DATAR;
	__IO uint8_t  IDATAR;
	uint8_t       RESERVED0;
	uint16_t      RESERVED1;
	__IO uint32_t CTLR;
} CRC_TypeDef;


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
	__IO uint32_t INDR;
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
	__IO uint32_t CR;
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
	__IO uint32_t AHBRSTR;
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
	__IO uint32_t CH1CVR_TIM4;
	__IO uint32_t CH1CVR;
	__IO uint32_t CH2CVR_TIM4;
	__IO uint32_t CH2CVR;
	__IO uint32_t CH3CVR_TIM4;
	__IO uint32_t CH3CVR;
	__IO uint32_t CH4CVR_TIM4;
	__IO uint32_t CH4CVR;
	__IO uint16_t BDTR;
	uint16_t      RESERVED13;
	__IO uint16_t DMACFGR;
	uint16_t      RESERVED14;
	__IO uint32_t DMAADR;
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

/* OPA Registers */
typedef struct
{
    __IO uint16_t CFGR1;
    __IO uint16_t CFGR2;
    __IO uint32_t CTLR1;
    __IO uint32_t CTLR2;
    __IO uint32_t RESERVED0;
    __IO uint32_t RESERVED1;
    __IO uint32_t OPCMKEY;
} OPA_TypeDef;

/* LPTIM Registers */
typedef struct
{
    __IO uint32_t ISR;
    __IO uint32_t ICR;
    __IO uint32_t IER;
    __IO uint32_t CFGR;
    __IO uint32_t CR;
    __IO uint16_t CMP;
    uint16_t      RESERVED0;
    __IO uint16_t ARR;
    uint16_t      RESERVED1;
    __IO uint16_t CNT;
    uint16_t      RESERVED2;
} LPTIM_TypeDef;

/* PD Registers */
typedef struct
{
	__IO uint16_t CONFIG;
	__IO uint16_t BMC_CLK_CNT;
	__IO uint8_t  CONTROL;
	__IO uint8_t  TX_SEL;
	__IO uint16_t BMC_TX_SZ;
	__IO uint8_t  DATA_BUF;
	__IO uint8_t  STATUS;
	__IO uint16_t BMC_BYTE_CNT;
	__IO uint16_t PORT_CC1;
	__IO uint16_t PORT_CC2;
	__IO uint16_t DMA;
	__IO uint16_t RESERVED;
} USBPD_TypeDef;

/* USBD Full-Speed Device, Chapter 21.
 NOTE: USBD and CAN controller share a dedicated 512-byte SRAM area for data
 transmission and reception in the design, so when using USBD and CAN functions
 at the same time, this shared area needs to be allocated reasonably to prevent
 data conflicts. */

typedef struct 
{
	__IO uint32_t ADDn_TX;
	__IO uint32_t COUNTn_TX;
	__IO uint32_t ADDn_RX;
	__IO uint32_t COUNTn_RX;
} USBD_BTABLE_TypeDef;

typedef struct
{
	__IO uint32_t EPR[8];
	__IO uint32_t RESERVED[8];
	__IO uint32_t CNTR;
	__IO uint32_t ISTR;
	__IO uint32_t FNR;
	__IO uint32_t DADDR;
	__IO uint32_t BTABLE;
} USBD_TypeDef;

#define CAN_USBD_SHARED_BASE   ((PERIPH_BASE + 0x6000))
#define USBD_BASE              ((PERIPH_BASE + 0x5C00))

/* USBD_CNTR */
#define USBD_CTRM      (1<<15)
#define USBD_PMAOVRM   (1<<14)
#define USBD_ERRM      (1<<13)
#define USBD_WKUPM     (1<<12)
#define USBD_SUSPM     (1<<11)
#define USBD_RESETM    (1<<10)
#define USBD_SOFM      (1<<9)
#define USBD_ESOFM     (1<<8)
#define USBD_RESUME    (1<<4)
#define USBD_FSUP      (1<<3)
#define USBD_LPMODE    (1<<2)
#define USBD_PDWN      (1<<1)
#define USBD_FRES      (1<<0)

/* USBD_ISTR */
#define USBD_CTR       (1<<15)
#define USBD_PMAOVR    (1<<14)
#define USBD_ERR       (1<<13)
#define USBD_WKUP      (1<<12)
#define USBD_SUSP      (1<<11)
#define USBD_RESET     (1<<10)
#define USBD_SOF       (1<<9)
#define USBD_ESOF      (1<<8)
#define USBD_DIR       (1<<4)
#define USBD_EP_ID     (0xf)

/* USBD_FNR */
#define USBD_RXDP      (1<<15)
#define USBD_RXDM      (1<<14)
#define USBD_LCK       (1<<13)
#define USBD_LSOF      (3<<11)
#define USBD_FN        (0x7ff)

/* USBD_DADDR */
#define USBD_EF        (1<<7)
#define USBD_ADD       (0x7f)

/* USBD_EPRx */
#define USBD_CTR_RX    (1<<15)
#define USBD_DTOG_RX   (1<<14)
#define USBD_STAT_RX   (3<<12)
#define USBD_SETUP     (1<<11)
#define USBD_EPTYPE    (3<<9)
#define USBD_EPKIND    (1<<8)
#define USBD_CTR_TX    (1<<7)
#define USBD_DTOG_TX   (1<<6)
#define USBD_STAT_TX   (3<<4)
#define USBD_EA        (0xf)

/* USBD_COUNTx_RX */
#define USBD_BLSIZE    (1<<15)
#define USBD_NUM_BLOCK (0x1f<<10)
#define USBD_COUNTx_RX 0x2ff


#define USBD           ((USBD_TypeDef *) USBD_BASE)

/* USB-FS-OTG Registers, Chapter 23. */
typedef struct
{
	__IO uint8_t  BASE_CTRL;
	__IO uint8_t  UDEV_CTRL;
	__IO uint8_t  INT_EN;
	__IO uint8_t  DEV_ADDR;
	__IO uint8_t  Reserve0;
	__IO uint8_t  MIS_ST;
	__IO uint8_t  INT_FG; // "Combined" register in some situations. (ST_FG)
	__IO uint8_t  INT_ST;
	__IO uint32_t RX_LEN;
	__IO uint8_t  UEP4_1_MOD;
	__IO uint8_t  UEP2_3_MOD;
	__IO uint8_t  UEP5_6_MOD;
	__IO uint8_t  UEP7_MOD;
	__IO uint32_t UEP0_DMA;
	__IO uint32_t UEP1_DMA;
	__IO uint32_t UEP2_DMA;
	__IO uint32_t UEP3_DMA;
	__IO uint32_t UEP4_DMA;
	__IO uint32_t UEP5_DMA;
	__IO uint32_t UEP6_DMA;
	__IO uint32_t UEP7_DMA;
	__IO uint16_t UEP0_TX_LEN;
	__IO uint8_t  UEP0_TX_CTRL;
	__IO uint8_t  UEP0_RX_CTRL;
	__IO uint16_t UEP1_TX_LEN;
	__IO uint8_t  UEP1_TX_CTRL;
	__IO uint8_t  UEP1_RX_CTRL;
	__IO uint16_t UEP2_TX_LEN;
	__IO uint8_t  UEP2_TX_CTRL;
	__IO uint8_t  UEP2_RX_CTRL;
	__IO uint16_t UEP3_TX_LEN;
	__IO uint8_t  UEP3_TX_CTRL;
	__IO uint8_t  UEP3_RX_CTRL;
	__IO uint16_t UEP4_TX_LEN;
	__IO uint8_t  UEP4_TX_CTRL;
	__IO uint8_t  UEP4_RX_CTRL;
	__IO uint16_t UEP5_TX_LEN;
	__IO uint8_t  UEP5_TX_CTRL;
	__IO uint8_t  UEP5_RX_CTRL;
	__IO uint16_t UEP6_TX_LEN;
	__IO uint8_t  UEP6_TX_CTRL;
	__IO uint8_t  UEP6_RX_CTRL;
	__IO uint16_t UEP7_TX_LEN;
	__IO uint8_t  UEP7_TX_CTRL;
	__IO uint8_t  UEP7_RX_CTRL;
	__IO uint32_t Reserve1;
	__IO uint32_t OTG_CR;
	__IO uint32_t OTG_SR;
} USBFSD_TypeDef;

/* R8_USB_CTRL */
#define USBOTG_UC_HOST_MODE         (1<<7)
#define USBOTG_UC_LOW_SPEED         (1<<6)
#define USBOTG_UC_DEV_PU_EN         (1<<5)
#define USBOTG_UC_SYS_CTRL          (1<<4)
#define USBOTG_UC_INT_BUSY          (1<<3)
#define USBOTG_UC_RESET_SIE         (1<<2)
#define USBOTG_UC_CLR_ALL           (1<<1)
#define USBOTG_UC_DMA_EN            (1<<0)

/* R8_USB_INT_EN */
#define USBOTG_UIE_DEV_NAK          (1<<6)
#define USBOTG_UIE_FIFO_OV          (1<<4)
#define USBOTG_UIE_HST_SOF          (1<<3)
#define USBOTG_UIE_SUSPEND          (1<<2)
#define USBOTG_UIE_TRANSFER         (1<<1)
#define USBOTG_UIE_DETECT           (1<<0)
#define USBOTG_UIE_BUS_RST          (1<<0)

/* R8_USB_DEV_AD */
#define USBOTG_UDA_GP_BIT           (1<<7)
#define USBOTG_USB_ADDR             (1<<6)

/* R8_USB_MIS_ST */
#define USBOTG_UMS_SOF_PRES         (1<<7)
#define USBOTG_UMS_SOF_ACT          (1<<6)
#define USBOTG_UMS_SIE_FREE         (1<<5)
#define USBOTG_UMS_R_FIFO_RDY       (1<<4)
#define USBOTG_UMS_BUS_RESET        (1<<3)
#define USBOTG_UMS_SUSPEND          (1<<2)
#define USBOTG_UMS_DM_LEVEL         (1<<1)
#define USBOTG_UMS_DEV_ATTACH       (1<<0)

/* R8_USB_INT_FG */
#define USBOTG_U_IS_NAK             (1<<7)
#define USBOTG_U_TOG_OK             (1<<6)
#define USBOTG_U_SIE_FREE           (1<<5)
#define USBOTG_UIF_FIFO_OV          (1<<4)
#define USBOTG_UIF_HST_SOF          (1<<3)
#define USBOTG_UIF_SUSPEND          (1<<2)
#define USBOTG_UIF_TRANSFER         (1<<1)
#define USBOTG_UIF_DETECT           (1<<0)
#define USBOTG_UIF_BUS_RST          (1<<0)

/* R8_USB_INT_ST */
#define USBOTG_UIS_IS_NAK           (1<<7)
#define USBOTG_UIS_TOG_OK           (1<<6)
#define USBOTG_UIS_TOKEN            (3<<4)
#define USBOTG_UIS_ENDP             0xf
#define USBOTG_UIS_H_RES            0xf

/* R32_USB_OTG_CR */
#define USBOTG_CR_SESS_VTH          (1<<5)
#define USBOTG_CR_VBUS_VTH          (1<<4)
#define USBOTG_CR_OTG_EN            (1<<3)
#define USBOTG_CR_IDPU              (1<<2)
#define USBOTG_CR_CHARGE_VBUS       (1<<1)
#define USBOTG_CR_DISCHAR_VBUS      (1<<0)

/* R32_USB_OTG_SR */
#define USBOTG_SR_ID_DIG            (1<<3)
#define USBOTG_SR_SESS_END          (1<<2)
#define USBOTG_SR_SESS_VLD          (1<<1)
#define USBOTG_SR_VBUS_VLD          (1<<0)

/* R8_UEPn_TX_CTRL */
#define USBOTG_UEP_T_AUTO_TOG       (1<<3)
#define USBOTG_UEP_T_TOG            (1<<2)
#define USBOTG_UEP_T_RES_MASK       (3<<0)      // bit mask of handshake response type for USB endpoint X transmittal (IN)
#define USBOTG_UEP_T_RES_ACK        (0<<1)
#define USBOTG_UEP_T_RES_NONE       (1<<0)
#define USBOTG_UEP_T_RES_NAK        (1<<1)
#define USBOTG_UEP_T_RES_STALL      (3<<0)

#define USBOTG_UEP_R_AUTO_TOG       (1<<3)      // enable automatic toggle after successful transfer completion on endpoint 1/2/3: 0=manual toggle, 1=automatic toggle
#define USBOTG_UEP_R_TOG            (1<<2)      // expected data toggle flag of USB endpoint X receiving (OUT): 0=DATA0, 1=DATA1
#define USBOTG_UEP_R_RES_MASK       (3<<0)      // bit mask of handshake response type for USB endpoint X receiving (OUT)
#define USBOTG_UEP_R_RES_ACK        (0<<1)
#define USBOTG_UEP_R_RES_NONE       (1<<0)
#define USBOTG_UEP_R_RES_NAK        (1<<1)
#define USBOTG_UEP_R_RES_STALL      (3<<0)



/* R8_UEPn_ RX_CTRL */
#define USBOTG_UEP_R_AUTO_TOG       (1<<3)
#define USBOTG_UEP_R_TOG            (1<<2)
#define USBOTG_UEP_R_RES            (3<<0)

/* R8_UEP7_MOD */
#define USBOTG_UEP7_RX_EN           (1<<3)
#define USBOTG_UEP7_TX_EN           (1<<2)
#define USBOTG_UEP7_BUF_MOD         (1<<0)

/* R8_UEP5_6_MOD */
#define USBOTG_UEP6_RX_EN           (1<<7)
#define USBOTG_UEP6_TX_EN           (1<<6)
#define USBOTG_UEP6_BUF_MOD         (1<<4)
#define USBOTG_UEP5_RX_EN           (1<<3)
#define USBOTG_UEP5_TX_EN           (1<<2)
#define USBOTG_UEP5_BUF_MOD         (1<<0)

/* R8_UEP2_3_MOD */
#define USBOTG_UEP3_RX_EN           (1<<7)
#define USBOTG_UEP3_TX_EN           (1<<6)
#define USBOTG_UEP3_BUF_MOD         (1<<4)
#define USBOTG_UEP2_RX_EN           (1<<3)
#define USBOTG_UEP2_TX_EN           (1<<2)
#define USBOTG_UEP2_BUF_MOD         (1<<0)

/* R8_UEP4_1_MOD */
#define USBOTG_UEP1_RX_EN           (1<<7)
#define USBOTG_UEP1_TX_EN           (1<<6)
#define USBOTG_UEP1_BUF_MOD         (1<<4)
#define USBOTG_UEP4_RX_EN           (1<<3)
#define USBOTG_UEP4_TX_EN           (1<<2)
#define USBOTG_UEP4_BUF_MOD         (1<<0)

/* R8_UDEV_CTRL */
#define USBOTG_UD_PD_DIS            (1<<7)
#define USBOTG_UD_DP_PIN            (1<<5)
#define USBOTG_UD_DM_PIN            (1<<4)
#define USBOTG_UD_LOW_SPEED         (1<<2)
#define USBOTG_UD_GP_BIT            (1<<1)
#define USBOTG_UD_PORT_EN           (1<<0)


#define USBFS_UDA_GP_BIT            0x80
#define USBFS_USB_ADDR_MASK         0x7F

#define DEF_USBD_UEP0_SIZE		   64	 /* usb hs/fs device end-point 0 size */
#define UEP_SIZE 64

#define DEF_UEP_IN                  0x80
#define DEF_UEP_OUT                 0x00
#define DEF_UEP_BUSY                0x01
#define DEF_UEP_FREE                0x00

#define DEF_UEP0 0
#define DEF_UEP1 1
#define DEF_UEP2 2
#define DEF_UEP3 3
#define DEF_UEP4 4
#define DEF_UEP5 5
#define DEF_UEP6 6
#define DEF_UEP7 7
#define UNUM_EP 8

typedef struct
{
	__IO uint8_t   BASE_CTRL;
	__IO uint8_t   HOST_CTRL;
	__IO uint8_t   INT_EN;
	__IO uint8_t   DEV_ADDR;
	__IO uint8_t   Reserve0;
	__IO uint8_t   MIS_ST;
	__IO uint8_t   INT_FG;
	__IO uint8_t   INT_ST;
	__IO uint16_t  RX_LEN;
	__IO uint16_t  Reserve1;
	__IO uint8_t   Reserve2;
	__IO uint8_t   HOST_EP_MOD;
	__IO uint16_t  Reserve3;
	__IO uint32_t  Reserve4;
	__IO uint32_t  Reserve5;
	__IO uint32_t  HOST_RX_DMA;
	__IO uint32_t  HOST_TX_DMA;
	__IO uint32_t  Reserve6;
	__IO uint32_t  Reserve7;
	__IO uint32_t  Reserve8;
	__IO uint32_t  Reserve9;
	__IO uint32_t  Reserve10;
	__IO uint16_t  Reserve11;
	__IO uint16_t  HOST_SETUP;
	__IO uint8_t   HOST_EP_PID;
	__IO uint8_t   Reserve12;
	__IO uint8_t   Reserve13;
	__IO uint8_t   HOST_RX_CTRL;
	__IO uint16_t  HOST_TX_LEN;
	__IO uint8_t   HOST_TX_CTRL;
	__IO uint8_t   Reserve14;
	__IO uint32_t  Reserve15;
	__IO uint32_t  Reserve16;
	__IO uint32_t  Reserve17;
	__IO uint32_t  Reserve18;
	__IO uint32_t  Reserve19;
	__IO uint32_t  OTG_CR;
	__IO uint32_t  OTG_SR;
} USBFSH_TypeDef;


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

#endif // !__ASSEMBLER__

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
#define RTC_BASE                                (APB1PERIPH_BASE + 0x2800)
#define WWDG_BASE                               (APB1PERIPH_BASE + 0x2C00)
#define IWDG_BASE                               (APB1PERIPH_BASE + 0x3000)
#define SPI2_BASE                               (APB1PERIPH_BASE + 0x3800)
#define USART2_BASE                             (APB1PERIPH_BASE + 0x4400)
#define USART3_BASE                             (APB1PERIPH_BASE + 0x4800)
#define USART4_BASE                             (APB1PERIPH_BASE + 0x4C00)
#define I2C1_BASE                               (APB1PERIPH_BASE + 0x5400)
#define I2C2_BASE                               (APB1PERIPH_BASE + 0x5800)
#define CAN1_BASE                               (APB1PERIPH_BASE + 0x6400)
#define BKP_BASE                                (APB1PERIPH_BASE + 0x6C00)
#define PWR_BASE                                (APB1PERIPH_BASE + 0x7000)
#define LPTIM_BASE                              (APB1PERIPH_BASE + 0x7C00)

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
#define USART1_BASE                             (APB2PERIPH_BASE + 0x3800)

#define DMA1_BASE                               (AHBPERIPH_BASE + 0x0000)
#define DMA1_Channel1_BASE                      (AHBPERIPH_BASE + 0x0008)
#define DMA1_Channel2_BASE                      (AHBPERIPH_BASE + 0x001C)
#define DMA1_Channel3_BASE                      (AHBPERIPH_BASE + 0x0030)
#define DMA1_Channel4_BASE                      (AHBPERIPH_BASE + 0x0044)
#define DMA1_Channel5_BASE                      (AHBPERIPH_BASE + 0x0058)
#define DMA1_Channel6_BASE                      (AHBPERIPH_BASE + 0x006C)
#define DMA1_Channel7_BASE                      (AHBPERIPH_BASE + 0x0080)
#define DMA1_Channel8_BASE                      (AHBPERIPH_BASE + 0x0094)
#define RCC_BASE                                (AHBPERIPH_BASE + 0x1000)

#define FLASH_R_BASE                            (AHBPERIPH_BASE + 0x2000) /* Flash registers base address */

#define CRC_BASE                                (AHBPERIPH_BASE + 0x3000)
#define EXTEN_BASE                              (AHBPERIPH_BASE + 0x3800)
#define OPA_BASE                                (AHBPERIPH_BASE + 0x6000)
#define USBPD_BASE                              (AHBPERIPH_BASE + 0x7000)

#define USBFS_BASE                              ((uint32_t)0x50000000)

#define OB_BASE                                 ((uint32_t)0x1FFFF800)    /* Flash Option Bytes base address */

#define TS_BASE                                 ((uint32_t)0x1FFFF720)
#define OPA_TRIM_BASE                           ((uint32_t)0x1FFFF724)
#define ADC_TRIM_BASE                           ((uint32_t)0x1FFFF728)
#define HSI_LP_TRIM_BASE                        ((uint32_t)0x1FFFF72A)
#define CHIPID_BASE                             ((uint32_t)0x1FFFF704)
#define USBPD_CFG_BASE                          ((uint32_t)0x1FFFF730)


#define PFIC_BASE    (CORE_PERIPH_BASE + 0xE000)
#define SysTick_BASE    (CORE_PERIPH_BASE + 0xF000)






/* Peripheral declaration */
#define TIM2                                    ((TIM_TypeDef *)TIM2_BASE)
#define TIM3                                    ((TIM_TypeDef *)TIM3_BASE)
#define TIM4                                    ((TIM_TypeDef *)TIM4_BASE)
#define TIM5                                    ((TIM_TypeDef *)TIM5_BASE)
#define RTC                                     ((RTC_TypeDef *)RTC_BASE)
#define WWDG                                    ((WWDG_TypeDef *)WWDG_BASE)
#define IWDG                                    ((IWDG_TypeDef *)IWDG_BASE)
#define SPI2                                    ((SPI_TypeDef *)SPI2_BASE)
#define USART2                                  ((USART_TypeDef *)USART2_BASE)
#define USART3                                  ((USART_TypeDef *)USART3_BASE)
#define USART4                                  ((USART_TypeDef *)USART4_BASE)
#define I2C1                                    ((I2C_TypeDef *)I2C1_BASE)
#define I2C2                                    ((I2C_TypeDef *)I2C2_BASE)
#define CAN1                                    ((CAN_TypeDef *)CAN1_BASE)
#define BKP                                     ((BKP_TypeDef *)BKP_BASE)
#define PWR                                     ((PWR_TypeDef *)PWR_BASE)
#define LPTIM                                   ((LPTIM_TypeDef *)LPTIM_BASE)

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
#define TKey1                                   ((ADC_TypeDef *)ADC1_BASE)
#define TKey2                                   ((ADC_TypeDef *)ADC2_BASE)
#define TIM1                                    ((TIM_TypeDef *)TIM1_BASE)
#define SPI1                                    ((SPI_TypeDef *)SPI1_BASE)
#define USART1                                  ((USART_TypeDef *)USART1_BASE)

#define DMA1                                    ((DMA_TypeDef *)DMA1_BASE)
#define DMA1_Channel1                           ((DMA_Channel_TypeDef *)DMA1_Channel1_BASE)
#define DMA1_Channel2                           ((DMA_Channel_TypeDef *)DMA1_Channel2_BASE)
#define DMA1_Channel3                           ((DMA_Channel_TypeDef *)DMA1_Channel3_BASE)
#define DMA1_Channel4                           ((DMA_Channel_TypeDef *)DMA1_Channel4_BASE)
#define DMA1_Channel5                           ((DMA_Channel_TypeDef *)DMA1_Channel5_BASE)
#define DMA1_Channel6                           ((DMA_Channel_TypeDef *)DMA1_Channel6_BASE)
#define DMA1_Channel7                           ((DMA_Channel_TypeDef *)DMA1_Channel7_BASE)
#define DMA1_Channel8                           ((DMA_Channel_TypeDef *)DMA1_Channel8_BASE)
#define RCC                                     ((RCC_TypeDef *)RCC_BASE)
#define FLASH                                   ((FLASH_TypeDef *)FLASH_R_BASE)
#define CRC                                     ((CRC_TypeDef *)CRC_BASE)
#define USBFSD                                  ((USBFSD_TypeDef *)USBFS_BASE)
#define USBFSH                                  ((USBFSH_TypeDef *)USBFS_BASE)
#define EXTEN                                   ((EXTEN_TypeDef *)EXTEN_BASE)
#define OPA                                     ((OPA_TypeDef *)OPA_BASE)
#define USBPD                                   ((USBPD_TypeDef *)USBPD_BASE)

#define OB                                      ((OB_TypeDef *)OB_BASE)


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
#define ADC_RDISCEN                             ADC_DISCEN

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

#define ADC_TKENABLE                            ((uint32_t)0x01000000)
#define ADC_TK1TUNE                             ((uint32_t)0x02000000)
#define ADC_BUFEN                               ((uint32_t)0x04000000)

#define ADC_PGA                                 ((uint32_t)0x18000000) /* PGA[1:0] bits */
#define ADC_PGA_0                               ((uint32_t)0x08000000)
#define ADC_PGA_1                               ((uint32_t)0x10000000)

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
#define ADC_SMP16                               ((uint32_t)0x001C0000) /* SMP16[2:0] bits (Channel 16 Sample time selection) */
#define ADC_SMP16_0                             ((uint32_t)0x00040000) /* Bit 0 */
#define ADC_SMP16_1                             ((uint32_t)0x00080000) /* Bit 1 */
#define ADC_SMP16_2                             ((uint32_t)0x00100000) /* Bit 2 */

#define ADC_SMP17                               ((uint32_t)0x00E00000) /* SMP17[2:0] bits (Channel 17 Sample time selection) */
#define ADC_SMP17_0                             ((uint32_t)0x00200000) /* Bit 0 */
#define ADC_SMP17_1                             ((uint32_t)0x00400000) /* Bit 1 */
#define ADC_SMP17_2                             ((uint32_t)0x00800000) /* Bit 2 */

#define ADC_SMP18                               ((uint32_t)0x07000000) /* SMP18[2:0] bits (Channel 18 Sample time selection) */
#define ADC_SMP18_0                             ((uint32_t)0x01000000) /* Bit 0 */
#define ADC_SMP18_1                             ((uint32_t)0x02000000) /* Bit 1 */
#define ADC_SMP18_2                             ((uint32_t)0x04000000) /* Bit 2 */

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

/********************  Bit definition for ADC_CFG register  ********************/
#define ADC_BUFTRIM                             ((uint32_t)0x0000000F) /* BUFTRIM[3:0] bits */
#define ADC_BUFTRIM_0                           ((uint32_t)0x00000001)
#define ADC_BUFTRIM_1                           ((uint32_t)0x00000002)
#define ADC_BUFTRIM_2                           ((uint32_t)0x00000004)
#define ADC_BUFTRIM_3                           ((uint32_t)0x00000008)

#define ADC_AWDRST_EN                           ((uint32_t)0x00000010)
#define ADC_LP                                  ((uint32_t)0x00000020)
#define ADC_FIFO_EN                             ((uint32_t)0x00000040)
#define ADC_DUTY_EN                             ((uint32_t)0x00000080)
#define ADC_TKEY_DRV_EN                         ((uint32_t)0x00000100)

#define ADC_TKEY_DRV_OUTEN                      ((uint32_t)0x0007FE00) /* TKEY_DRV_OUTEN[9:0] bits */
#define ADC_TKEY_DRV_OUTEN_0                    ((uint32_t)0x00000200)
#define ADC_TKEY_DRV_OUTEN_1                    ((uint32_t)0x00000400)
#define ADC_TKEY_DRV_OUTEN_2                    ((uint32_t)0x00000800)
#define ADC_TKEY_DRV_OUTEN_3                    ((uint32_t)0x00001000)
#define ADC_TKEY_DRV_OUTEN_4                    ((uint32_t)0x00002000)
#define ADC_TKEY_DRV_OUTEN_5                    ((uint32_t)0x00004000)
#define ADC_TKEY_DRV_OUTEN_6                    ((uint32_t)0x00008000)
#define ADC_TKEY_DRV_OUTEN_7                    ((uint32_t)0x00010000)
#define ADC_TKEY_DRV_OUTEN_8                    ((uint32_t)0x00020000)
#define ADC_TKEY_DRV_OUTEN_9                    ((uint32_t)0x00040000)

#define ADC_TKEY_SEL                            ((uint32_t)0x00180000) /* TKEY_SEL[1:0] bits */
#define ADC_TKEY_SEL_0                          ((uint32_t)0x00080000)
#define ADC_TKEY_SEL_1                          ((uint32_t)0x00080000)

#define ADC_TKEY_WAKE_EN                        ((uint32_t)0x8FE00000) /* TKEY_WAKE_EN[9:0] bits */
#define ADC_TKEY_WAKE_EN_0                      ((uint32_t)0x00200000)
#define ADC_TKEY_WAKE_EN_1                      ((uint32_t)0x00400000)
#define ADC_TKEY_WAKE_EN_2                      ((uint32_t)0x00800000)
#define ADC_TKEY_WAKE_EN_3                      ((uint32_t)0x01000000)
#define ADC_TKEY_WAKE_EN_4                      ((uint32_t)0x02000000)
#define ADC_TKEY_WAKE_EN_5                      ((uint32_t)0x04000000)
#define ADC_TKEY_WAKE_EN_6                      ((uint32_t)0x08000000)
#define ADC_TKEY_WAKE_EN_7                      ((uint32_t)0x10000000)
#define ADC_TKEY_WAKE_EN_8                      ((uint32_t)0x20000000)
#define ADC_TKEY_WAKE_EN_9                      ((uint32_t)0x40000000)

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
/*                         Controller Area Network                            */
/******************************************************************************/

/*******************  Bit definition for CAN_CTLR register  ********************/
#define CAN_CTLR_INRQ                           ((uint16_t)0x0001) /* Initialization Request */
#define CAN_CTLR_SLEEP                          ((uint16_t)0x0002) /* Sleep Mode Request */
#define CAN_CTLR_TXFP                           ((uint16_t)0x0004) /* Transmit FIFO Priority */
#define CAN_CTLR_RFLM                           ((uint16_t)0x0008) /* Receive FIFO Locked Mode */
#define CAN_CTLR_NART                           ((uint16_t)0x0010) /* No Automatic Retransmission */
#define CAN_CTLR_AWUM                           ((uint16_t)0x0020) /* Automatic Wakeup Mode */
#define CAN_CTLR_ABOM                           ((uint16_t)0x0040) /* Automatic Bus-Off Management */
#define CAN_CTLR_TTCM                           ((uint16_t)0x0080) /* Time Triggered Communication Mode */
#define CAN_CTLR_RESET                          ((uint16_t)0x8000) /* CAN software master reset */
#define CAN_CTLR_DBF                            ((uint32_t)0x00010000) /* CAN controller operating state selection during debugging */

/*******************  Bit definition for CAN_STATR register  ********************/
#define CAN_STATR_INAK                          ((uint16_t)0x0001) /* Initialization Acknowledge */
#define CAN_STATR_SLAK                          ((uint16_t)0x0002) /* Sleep Acknowledge */
#define CAN_STATR_ERRI                          ((uint16_t)0x0004) /* Error Interrupt */
#define CAN_STATR_WKUI                          ((uint16_t)0x0008) /* Wakeup Interrupt */
#define CAN_STATR_SLAKI                         ((uint16_t)0x0010) /* Sleep Acknowledge Interrupt */
#define CAN_STATR_TXM                           ((uint16_t)0x0100) /* Transmit Mode */
#define CAN_STATR_RXM                           ((uint16_t)0x0200) /* Receive Mode */
#define CAN_STATR_SAMP                          ((uint16_t)0x0400) /* Last Sample Point */
#define CAN_STATR_RX                            ((uint16_t)0x0800) /* CAN Rx Signal */

/*******************  Bit definition for CAN_TSTATR register  ********************/
#define CAN_TSTATR_RQCP0                        ((uint32_t)0x00000001) /* Request Completed Mailbox0 */
#define CAN_TSTATR_TXOK0                        ((uint32_t)0x00000002) /* Transmission OK of Mailbox0 */
#define CAN_TSTATR_ALST0                        ((uint32_t)0x00000004) /* Arbitration Lost for Mailbox0 */
#define CAN_TSTATR_TERR0                        ((uint32_t)0x00000008) /* Transmission Error of Mailbox0 */
#define CAN_TSTATR_ABRQ0                        ((uint32_t)0x00000080) /* Abort Request for Mailbox0 */
#define CAN_TSTATR_RQCP1                        ((uint32_t)0x00000100) /* Request Completed Mailbox1 */
#define CAN_TSTATR_TXOK1                        ((uint32_t)0x00000200) /* Transmission OK of Mailbox1 */
#define CAN_TSTATR_ALST1                        ((uint32_t)0x00000400) /* Arbitration Lost for Mailbox1 */
#define CAN_TSTATR_TERR1                        ((uint32_t)0x00000800) /* Transmission Error of Mailbox1 */
#define CAN_TSTATR_ABRQ1                        ((uint32_t)0x00008000) /* Abort Request for Mailbox 1 */
#define CAN_TSTATR_RQCP2                        ((uint32_t)0x00010000) /* Request Completed Mailbox2 */
#define CAN_TSTATR_TXOK2                        ((uint32_t)0x00020000) /* Transmission OK of Mailbox 2 */
#define CAN_TSTATR_ALST2                        ((uint32_t)0x00040000) /* Arbitration Lost for mailbox 2 */
#define CAN_TSTATR_TERR2                        ((uint32_t)0x00080000) /* Transmission Error of Mailbox 2 */
#define CAN_TSTATR_ABRQ2                        ((uint32_t)0x00800000) /* Abort Request for Mailbox 2 */
#define CAN_TSTATR_CODE                         ((uint32_t)0x03000000) /* Mailbox Code */

#define CAN_TSTATR_TME                          ((uint32_t)0x1C000000) /* TME[2:0] bits */
#define CAN_TSTATR_TME0                         ((uint32_t)0x04000000) /* Transmit Mailbox 0 Empty */
#define CAN_TSTATR_TME1                         ((uint32_t)0x08000000) /* Transmit Mailbox 1 Empty */
#define CAN_TSTATR_TME2                         ((uint32_t)0x10000000) /* Transmit Mailbox 2 Empty */

#define CAN_TSTATR_LOW                          ((uint32_t)0xE0000000) /* LOW[2:0] bits */
#define CAN_TSTATR_LOW0                         ((uint32_t)0x20000000) /* Lowest Priority Flag for Mailbox 0 */
#define CAN_TSTATR_LOW1                         ((uint32_t)0x40000000) /* Lowest Priority Flag for Mailbox 1 */
#define CAN_TSTATR_LOW2                         ((uint32_t)0x80000000) /* Lowest Priority Flag for Mailbox 2 */

/*******************  Bit definition for CAN_RFIFO0 register  *******************/
#define CAN_RFIFO0_FMP0                         ((uint8_t)0x03) /* FIFO 0 Message Pending */
#define CAN_RFIFO0_FULL0                        ((uint8_t)0x08) /* FIFO 0 Full */
#define CAN_RFIFO0_FOVR0                        ((uint8_t)0x10) /* FIFO 0 Overrun */
#define CAN_RFIFO0_RFOM0                        ((uint8_t)0x20) /* Release FIFO 0 Output Mailbox */

/*******************  Bit definition for CAN_RFIFO1 register  *******************/
#define CAN_RFIFO1_FMP1                         ((uint8_t)0x03) /* FIFO 1 Message Pending */
#define CAN_RFIFO1_FULL1                        ((uint8_t)0x08) /* FIFO 1 Full */
#define CAN_RFIFO1_FOVR1                        ((uint8_t)0x10) /* FIFO 1 Overrun */
#define CAN_RFIFO1_RFOM1                        ((uint8_t)0x20) /* Release FIFO 1 Output Mailbox */

/********************  Bit definition for CAN_INTENR register  *******************/
#define CAN_INTENR_TMEIE                        ((uint32_t)0x00000001) /* Transmit Mailbox Empty Interrupt Enable */
#define CAN_INTENR_FMPIE0                       ((uint32_t)0x00000002) /* FIFO Message Pending Interrupt Enable */
#define CAN_INTENR_FFIE0                        ((uint32_t)0x00000004) /* FIFO Full Interrupt Enable */
#define CAN_INTENR_FOVIE0                       ((uint32_t)0x00000008) /* FIFO Overrun Interrupt Enable */
#define CAN_INTENR_FMPIE1                       ((uint32_t)0x00000010) /* FIFO Message Pending Interrupt Enable */
#define CAN_INTENR_FFIE1                        ((uint32_t)0x00000020) /* FIFO Full Interrupt Enable */
#define CAN_INTENR_FOVIE1                       ((uint32_t)0x00000040) /* FIFO Overrun Interrupt Enable */
#define CAN_INTENR_EWGIE                        ((uint32_t)0x00000100) /* Error Warning Interrupt Enable */
#define CAN_INTENR_EPVIE                        ((uint32_t)0x00000200) /* Error Passive Interrupt Enable */
#define CAN_INTENR_BOFIE                        ((uint32_t)0x00000400) /* Bus-Off Interrupt Enable */
#define CAN_INTENR_LECIE                        ((uint32_t)0x00000800) /* Last Error Code Interrupt Enable */
#define CAN_INTENR_ERRIE                        ((uint32_t)0x00008000) /* Error Interrupt Enable */
#define CAN_INTENR_WKUIE                        ((uint32_t)0x00010000) /* Wakeup Interrupt Enable */
#define CAN_INTENR_SLKIE                        ((uint32_t)0x00020000) /* Sleep Interrupt Enable */

/********************  Bit definition for CAN_ERRSR register  *******************/
#define CAN_ERRSR_EWGF                          ((uint32_t)0x00000001) /* Error Warning Flag */
#define CAN_ERRSR_EPVF                          ((uint32_t)0x00000002) /* Error Passive Flag */
#define CAN_ERRSR_BOFF                          ((uint32_t)0x00000004) /* Bus-Off Flag */

#define CAN_ERRSR_LEC                           ((uint32_t)0x00000070) /* LEC[2:0] bits (Last Error Code) */
#define CAN_ERRSR_LEC_0                         ((uint32_t)0x00000010) /* Bit 0 */
#define CAN_ERRSR_LEC_1                         ((uint32_t)0x00000020) /* Bit 1 */
#define CAN_ERRSR_LEC_2                         ((uint32_t)0x00000040) /* Bit 2 */

#define CAN_ERRSR_TEC                           ((uint32_t)0x00FF0000) /* Least significant byte of the 9-bit Transmit Error Counter */
#define CAN_ERRSR_REC                           ((uint32_t)0xFF000000) /* Receive Error Counter */

/*******************  Bit definition for CAN_BTIMR register  ********************/
#define CAN_BTIMR_BRP                           ((uint32_t)0x000003FF) /* Baud Rate Prescaler */
#define CAN_BTIMR_TS1                           ((uint32_t)0x000F0000) /* Time Segment 1 */
#define CAN_BTIMR_TS2                           ((uint32_t)0x00700000) /* Time Segment 2 */
#define CAN_BTIMR_SJW                           ((uint32_t)0x03000000) /* Resynchronization Jump Width */
#define CAN_BTIMR_LBKM                          ((uint32_t)0x40000000) /* Loop Back Mode (Debug) */
#define CAN_BTIMR_SILM                          ((uint32_t)0x80000000) /* Silent Mode */

/********************  Bit definition for CAN_TTCTLR register  *******************/
#define CAN_TTCTLR_TIMCMV                       ((uint32_t)0x0000FFFF)
#define CAN_TTCTLR_TIMRST                       ((uint32_t)0x00010000)
#define CAN_TTCTLR_MODE                         ((uint32_t)0x00020000)

/********************  Bit definition for CAN_TTCNT register  *******************/
#define CAN_TTCNT                               ((uint32_t)0x0000FFFF)

/********************  Bit definition for CAN_TERR_CNT register  *******************/
#define CAN_TERR_CNT                            ((uint32_t)0x000001FF)

/********************  Bit definition for CANFD_CR register  *******************/
#define CANFD_CR_TX_FD                          ((uint32_t)0x00000001)
#define CANFD_CR_TX_BRS_B                       ((uint32_t)0x0000000E)
#define CANFD_CR_USER_ESI_B                     ((uint32_t)0x00000070)
#define CANFD_CR_RES_EXCEPT                     ((uint32_t)0x00000080)
#define CANFD_CR_CLAS_LONG_TS1                  ((uint32_t)0x00000100)
#define CANFD_CR_RESTRICT_MODE                  ((uint32_t)0x00000200)

/********************  Bit definition for CANFD_BTR register  *******************/
#define CANFD_BTR_BTR_SJW_FD                    ((uint32_t)0x0000000F)
#define CANFD_BTR_BTR_TS2_FD                    ((uint32_t)0x000000F0)
#define CANFD_BTR_BTR_TS1_FD                    ((uint32_t)0x00001F00)
#define CANFD_BTR_BTR_BRP_FD                    ((uint32_t)0x001F0000)
#define CANFD_BTR_BTR_TDCE                      ((uint32_t)0x00800000)

/********************  Bit definition for CANFD_TDCT register  *******************/
#define CANFD_TDCT_TDCO                         ((uint32_t)0x0000003F)
#define CANFD_TDCT_TDC_FILTER                   ((uint32_t)0x00003F00)

/********************  Bit definition for CANFD_PSR register  *******************/
#define CANFD_PSR_TDCV                          ((uint32_t)0x00FF0000)

/********************  Bit definition for CAN_DMA_T0 register  *******************/
#define CANFD_DMA_T0                            ((uint32_t)0x00007FFF)

/********************  Bit definition for CAN_DMA_T1 register  *******************/
#define CANFD_DMA_T1                            ((uint32_t)0x00007FFF)

/********************  Bit definition for CAN_DMA_T2 register  *******************/
#define CANFD_DMA_T2                            ((uint32_t)0x00007FFF)

/********************  Bit definition for CAN_DMA_R0 register  *******************/
#define CANFD_DMA_R0                            ((uint32_t)0x00007FFF)

/********************  Bit definition for CAN_DMA_R1 register  *******************/
#define CANFD_DMA_R1                            ((uint32_t)0x00007FFF)

/******************  Bit definition for CAN_TXMI0R register  ********************/
#define CAN_TXMI0R_TXRQ                         ((uint32_t)0x00000001) /* Transmit Mailbox Request */
#define CAN_TXMI0R_RTR                          ((uint32_t)0x00000002) /* Remote Transmission Request */
#define CAN_TXMI0R_IDE                          ((uint32_t)0x00000004) /* Identifier Extension */
#define CAN_TXMI0R_EXID                         ((uint32_t)0x001FFFF8) /* Extended Identifier */
#define CAN_TXMI0R_STID                         ((uint32_t)0xFFE00000) /* Standard Identifier or Extended Identifier */

/******************  Bit definition for CAN_TXMDT0R register  *******************/
#define CAN_TXMDT0R_DLC                         ((uint32_t)0x0000000F) /* Data Length Code */
#define CAN_TXMDT0R_TGT                         ((uint32_t)0x00000100) /* Transmit Global Time */
#define CAN_TXMDT0R_TIME                        ((uint32_t)0xFFFF0000) /* Message Time Stamp */

/******************  Bit definition for CAN_TXMDL0R register  *******************/
#define CAN_TXMDL0R_DATA0                       ((uint32_t)0x000000FF) /* Data byte 0 */
#define CAN_TXMDL0R_DATA1                       ((uint32_t)0x0000FF00) /* Data byte 1 */
#define CAN_TXMDL0R_DATA2                       ((uint32_t)0x00FF0000) /* Data byte 2 */
#define CAN_TXMDL0R_DATA3                       ((uint32_t)0xFF000000) /* Data byte 3 */

/******************  Bit definition for CAN_TXMDH0R register  *******************/
#define CAN_TXMDH0R_DATA4                       ((uint32_t)0x000000FF) /* Data byte 4 */
#define CAN_TXMDH0R_DATA5                       ((uint32_t)0x0000FF00) /* Data byte 5 */
#define CAN_TXMDH0R_DATA6                       ((uint32_t)0x00FF0000) /* Data byte 6 */
#define CAN_TXMDH0R_DATA7                       ((uint32_t)0xFF000000) /* Data byte 7 */

/*******************  Bit definition for CAN_TXMI1R register  *******************/
#define CAN_TXMI1R_TXRQ                         ((uint32_t)0x00000001) /* Transmit Mailbox Request */
#define CAN_TXMI1R_RTR                          ((uint32_t)0x00000002) /* Remote Transmission Request */
#define CAN_TXMI1R_IDE                          ((uint32_t)0x00000004) /* Identifier Extension */
#define CAN_TXMI1R_EXID                         ((uint32_t)0x001FFFF8) /* Extended Identifier */
#define CAN_TXMI1R_STID                         ((uint32_t)0xFFE00000) /* Standard Identifier or Extended Identifier */

/*******************  Bit definition for CAN_TXMDT1R register  ******************/
#define CAN_TXMDT1R_DLC                         ((uint32_t)0x0000000F) /* Data Length Code */
#define CAN_TXMDT1R_TGT                         ((uint32_t)0x00000100) /* Transmit Global Time */
#define CAN_TXMDT1R_TIME                        ((uint32_t)0xFFFF0000) /* Message Time Stamp */

/*******************  Bit definition for CAN_TXMDL1R register  ******************/
#define CAN_TXMDL1R_DATA0                       ((uint32_t)0x000000FF) /* Data byte 0 */
#define CAN_TXMDL1R_DATA1                       ((uint32_t)0x0000FF00) /* Data byte 1 */
#define CAN_TXMDL1R_DATA2                       ((uint32_t)0x00FF0000) /* Data byte 2 */
#define CAN_TXMDL1R_DATA3                       ((uint32_t)0xFF000000) /* Data byte 3 */

/*******************  Bit definition for CAN_TXMDH1R register  ******************/
#define CAN_TXMDH1R_DATA4                       ((uint32_t)0x000000FF) /* Data byte 4 */
#define CAN_TXMDH1R_DATA5                       ((uint32_t)0x0000FF00) /* Data byte 5 */
#define CAN_TXMDH1R_DATA6                       ((uint32_t)0x00FF0000) /* Data byte 6 */
#define CAN_TXMDH1R_DATA7                       ((uint32_t)0xFF000000) /* Data byte 7 */

/*******************  Bit definition for CAN_TXMI2R register  *******************/
#define CAN_TXMI2R_TXRQ                         ((uint32_t)0x00000001) /* Transmit Mailbox Request */
#define CAN_TXMI2R_RTR                          ((uint32_t)0x00000002) /* Remote Transmission Request */
#define CAN_TXMI2R_IDE                          ((uint32_t)0x00000004) /* Identifier Extension */
#define CAN_TXMI2R_EXID                         ((uint32_t)0x001FFFF8) /* Extended identifier */
#define CAN_TXMI2R_STID                         ((uint32_t)0xFFE00000) /* Standard Identifier or Extended Identifier */

/*******************  Bit definition for CAN_TXMDT2R register  ******************/
#define CAN_TXMDT2R_DLC                         ((uint32_t)0x0000000F) /* Data Length Code */
#define CAN_TXMDT2R_TGT                         ((uint32_t)0x00000100) /* Transmit Global Time */
#define CAN_TXMDT2R_TIME                        ((uint32_t)0xFFFF0000) /* Message Time Stamp */

/*******************  Bit definition for CAN_TXMDL2R register  ******************/
#define CAN_TXMDL2R_DATA0                       ((uint32_t)0x000000FF) /* Data byte 0 */
#define CAN_TXMDL2R_DATA1                       ((uint32_t)0x0000FF00) /* Data byte 1 */
#define CAN_TXMDL2R_DATA2                       ((uint32_t)0x00FF0000) /* Data byte 2 */
#define CAN_TXMDL2R_DATA3                       ((uint32_t)0xFF000000) /* Data byte 3 */

/*******************  Bit definition for CAN_TXMDH2R register  ******************/
#define CAN_TXMDH2R_DATA4                       ((uint32_t)0x000000FF) /* Data byte 4 */
#define CAN_TXMDH2R_DATA5                       ((uint32_t)0x0000FF00) /* Data byte 5 */
#define CAN_TXMDH2R_DATA6                       ((uint32_t)0x00FF0000) /* Data byte 6 */
#define CAN_TXMDH2R_DATA7                       ((uint32_t)0xFF000000) /* Data byte 7 */

/*******************  Bit definition for CAN_RXMI0R register  *******************/
#define CAN_RXMIOR_FDF                          ((uint32_t)0x00000001)
#define CAN_RXMI0R_RTR                          ((uint32_t)0x00000002) /* Remote Transmission Request */
#define CAN_RXMI0R_IDE                          ((uint32_t)0x00000004) /* Identifier Extension */
#define CAN_RXMI0R_EXID                         ((uint32_t)0x001FFFF8) /* Extended Identifier */
#define CAN_RXMI0R_STID                         ((uint32_t)0xFFE00000) /* Standard Identifier or Extended Identifier */

/*******************  Bit definition for CAN_RXMDT0R register  ******************/
#define CAN_RXMDT0R_DLC                         ((uint32_t)0x0000000F) /* Data Length Code */
#define CAN_RXMDT0R_BRS                         ((uint32_t)0x00000010)
#define CAN_RXMDT0R_ESI                         ((uint32_t)0x00000020)
#define CAN_RXMDH0R_RES                         ((uint32_t)0x00000100)
#define CAN_RXMDT0R_FMI                         ((uint32_t)0x0000FF00) /* Filter Match Index */
#define CAN_RXMDT0R_TIME                        ((uint32_t)0xFFFF0000) /* Message Time Stamp */

/*******************  Bit definition for CAN_RXMDL0R register  ******************/
#define CAN_RXMDL0R_DATA0                       ((uint32_t)0x000000FF) /* Data byte 0 */
#define CAN_RXMDL0R_DATA1                       ((uint32_t)0x0000FF00) /* Data byte 1 */
#define CAN_RXMDL0R_DATA2                       ((uint32_t)0x00FF0000) /* Data byte 2 */
#define CAN_RXMDL0R_DATA3                       ((uint32_t)0xFF000000) /* Data byte 3 */

/*******************  Bit definition for CAN_RXMDH0R register  ******************/
#define CAN_RXMDH0R_DATA4                       ((uint32_t)0x000000FF) /* Data byte 4 */
#define CAN_RXMDH0R_DATA5                       ((uint32_t)0x0000FF00) /* Data byte 5 */
#define CAN_RXMDH0R_DATA6                       ((uint32_t)0x00FF0000) /* Data byte 6 */
#define CAN_RXMDH0R_DATA7                       ((uint32_t)0xFF000000) /* Data byte 7 */

/*******************  Bit definition for CAN_RXMI1R register  *******************/
#define CAN_RXMI1R_FDF                          ((uint32_t)0x00000001)
#define CAN_RXMI1R_RTR                          ((uint32_t)0x00000002) /* Remote Transmission Request */
#define CAN_RXMI1R_IDE                          ((uint32_t)0x00000004) /* Identifier Extension */
#define CAN_RXMI1R_EXID                         ((uint32_t)0x001FFFF8) /* Extended identifier */
#define CAN_RXMI1R_STID                         ((uint32_t)0xFFE00000) /* Standard Identifier or Extended Identifier */

/*******************  Bit definition for CAN_RXMDT1R register  ******************/
#define CAN_RXMDT1R_DLC                         ((uint32_t)0x0000000F) /* Data Length Code */
#define CAN_RXMDT1R_BRS                         ((uint32_t)0x00000010)
#define CAN_RXMDT1R_ESI                         ((uint32_t)0x00000020)
#define CAN_RXMDH1R_RES                         ((uint32_t)0x00000100)
#define CAN_RXMDT1R_FMI                         ((uint32_t)0x0000FF00) /* Filter Match Index */
#define CAN_RXMDT1R_TIME                        ((uint32_t)0xFFFF0000) /* Message Time Stamp */

/*******************  Bit definition for CAN_RXMDL1R register  ******************/
#define CAN_RXMDL1R_DATA0                       ((uint32_t)0x000000FF) /* Data byte 0 */
#define CAN_RXMDL1R_DATA1                       ((uint32_t)0x0000FF00) /* Data byte 1 */
#define CAN_RXMDL1R_DATA2                       ((uint32_t)0x00FF0000) /* Data byte 2 */
#define CAN_RXMDL1R_DATA3                       ((uint32_t)0xFF000000) /* Data byte 3 */

/*******************  Bit definition for CAN_RXMDH1R register  ******************/
#define CAN_RXMDH1R_DATA4                       ((uint32_t)0x000000FF) /* Data byte 4 */
#define CAN_RXMDH1R_DATA5                       ((uint32_t)0x0000FF00) /* Data byte 5 */
#define CAN_RXMDH1R_DATA6                       ((uint32_t)0x00FF0000) /* Data byte 6 */
#define CAN_RXMDH1R_DATA7                       ((uint32_t)0xFF000000) /* Data byte 7 */

/*******************  Bit definition for CAN_FCTLR register  ********************/
#define CAN_FCTLR_FINIT                         ((uint8_t)0x01) /* Filter Init Mode */

/*******************  Bit definition for CAN_FMCFGR register  *******************/
#define CAN_FMCFGR_FBM                          ((uint16_t)0x3FFF) /* Filter Mode */
#define CAN_FMCFGR_FBM0                         ((uint16_t)0x0001) /* Filter Init Mode bit 0 */
#define CAN_FMCFGR_FBM1                         ((uint16_t)0x0002) /* Filter Init Mode bit 1 */
#define CAN_FMCFGR_FBM2                         ((uint16_t)0x0004) /* Filter Init Mode bit 2 */
#define CAN_FMCFGR_FBM3                         ((uint16_t)0x0008) /* Filter Init Mode bit 3 */
#define CAN_FMCFGR_FBM4                         ((uint16_t)0x0010) /* Filter Init Mode bit 4 */
#define CAN_FMCFGR_FBM5                         ((uint16_t)0x0020) /* Filter Init Mode bit 5 */
#define CAN_FMCFGR_FBM6                         ((uint16_t)0x0040) /* Filter Init Mode bit 6 */
#define CAN_FMCFGR_FBM7                         ((uint16_t)0x0080) /* Filter Init Mode bit 7 */
#define CAN_FMCFGR_FBM8                         ((uint16_t)0x0100) /* Filter Init Mode bit 8 */
#define CAN_FMCFGR_FBM9                         ((uint16_t)0x0200) /* Filter Init Mode bit 9 */
#define CAN_FMCFGR_FBM10                        ((uint16_t)0x0400) /* Filter Init Mode bit 10 */
#define CAN_FMCFGR_FBM11                        ((uint16_t)0x0800) /* Filter Init Mode bit 11 */
#define CAN_FMCFGR_FBM12                        ((uint16_t)0x1000) /* Filter Init Mode bit 12 */
#define CAN_FMCFGR_FBM13                        ((uint32_t)0x00002000) /* Filter Init Mode bit 13 */
#define CAN_FMCFGR_FBM14                        ((uint32_t)0x00004000) /* Filter Init Mode bit 14 */
#define CAN_FMCFGR_FBM15                        ((uint32_t)0x00008000) /* Filter Init Mode bit 15 */
#define CAN_FMCFGR_FBM16                        ((uint32_t)0x00010000) /* Filter Init Mode bit 16 */
#define CAN_FMCFGR_FBM17                        ((uint32_t)0x00020000) /* Filter Init Mode bit 17 */
#define CAN_FMCFGR_FBM18                        ((uint32_t)0x00040000) /* Filter Init Mode bit 18 */
#define CAN_FMCFGR_FBM19                        ((uint32_t)0x00080000) /* Filter Init Mode bit 19 */
#define CAN_FMCFGR_FBM20                        ((uint32_t)0x00100000) /* Filter Init Mode bit 20 */
#define CAN_FMCFGR_FBM21                        ((uint32_t)0x00200000) /* Filter Init Mode bit 21 */
#define CAN_FMCFGR_FBM22                        ((uint32_t)0x00400000) /* Filter Init Mode bit 22 */
#define CAN_FMCFGR_FBM23                        ((uint32_t)0x00800000) /* Filter Init Mode bit 23 */
#define CAN_FMCFGR_FBM24                        ((uint32_t)0x01000000) /* Filter Init Mode bit 24 */
#define CAN_FMCFGR_FBM25                        ((uint32_t)0x02000000) /* Filter Init Mode bit 25 */
#define CAN_FMCFGR_FBM26                        ((uint32_t)0x04000000) /* Filter Init Mode bit 26 */
#define CAN_FMCFGR_FBM27                        ((uint32_t)0x08000000) /* Filter Init Mode bit 27 */

/*******************  Bit definition for CAN_FSCFGR register  *******************/
#define CAN_FSCFGR_FSC                          ((uint16_t)0x3FFF) /* Filter Scale Configuration */
#define CAN_FSCFGR_FSC0                         ((uint16_t)0x0001) /* Filter Scale Configuration bit 0 */
#define CAN_FSCFGR_FSC1                         ((uint16_t)0x0002) /* Filter Scale Configuration bit 1 */
#define CAN_FSCFGR_FSC2                         ((uint16_t)0x0004) /* Filter Scale Configuration bit 2 */
#define CAN_FSCFGR_FSC3                         ((uint16_t)0x0008) /* Filter Scale Configuration bit 3 */
#define CAN_FSCFGR_FSC4                         ((uint16_t)0x0010) /* Filter Scale Configuration bit 4 */
#define CAN_FSCFGR_FSC5                         ((uint16_t)0x0020) /* Filter Scale Configuration bit 5 */
#define CAN_FSCFGR_FSC6                         ((uint16_t)0x0040) /* Filter Scale Configuration bit 6 */
#define CAN_FSCFGR_FSC7                         ((uint16_t)0x0080) /* Filter Scale Configuration bit 7 */
#define CAN_FSCFGR_FSC8                         ((uint16_t)0x0100) /* Filter Scale Configuration bit 8 */
#define CAN_FSCFGR_FSC9                         ((uint16_t)0x0200) /* Filter Scale Configuration bit 9 */
#define CAN_FSCFGR_FSC10                        ((uint16_t)0x0400) /* Filter Scale Configuration bit 10 */
#define CAN_FSCFGR_FSC11                        ((uint16_t)0x0800) /* Filter Scale Configuration bit 11 */
#define CAN_FSCFGR_FSC12                        ((uint16_t)0x1000) /* Filter Scale Configuration bit 12 */
#define CAN_FSCFGR_FSC13                        ((uint32_t)0x00002000) /* Filter Scale Configuration bit 13 */
#define CAN_FSCFGR_FSC14                        ((uint32_t)0x00004000) /* Filter Scale Configuration bit 14 */
#define CAN_FSCFGR_FSC15                        ((uint32_t)0x00008000) /* Filter Scale Configuration bit 15 */
#define CAN_FSCFGR_FSC16                        ((uint32_t)0x00010000) /* Filter Scale Configuration bit 16 */
#define CAN_FSCFGR_FSC17                        ((uint32_t)0x00020000) /* Filter Scale Configuration bit 17 */
#define CAN_FSCFGR_FSC18                        ((uint32_t)0x00040000) /* Filter Scale Configuration bit 18 */
#define CAN_FSCFGR_FSC19                        ((uint32_t)0x00080000) /* Filter Scale Configuration bit 19 */
#define CAN_FSCFGR_FSC20                        ((uint32_t)0x00100000) /* Filter Scale Configuration bit 20 */
#define CAN_FSCFGR_FSC21                        ((uint32_t)0x00200000) /* Filter Scale Configuration bit 21 */
#define CAN_FSCFGR_FSC22                        ((uint32_t)0x00400000) /* Filter Scale Configuration bit 22 */
#define CAN_FSCFGR_FSC23                        ((uint32_t)0x00800000) /* Filter Scale Configuration bit 23 */
#define CAN_FSCFGR_FSC24                        ((uint32_t)0x01000000) /* Filter Scale Configuration bit 24 */
#define CAN_FSCFGR_FSC25                        ((uint32_t)0x02000000) /* Filter Scale Configuration bit 25 */
#define CAN_FSCFGR_FSC26                        ((uint32_t)0x04000000) /* Filter Scale Configuration bit 26 */
#define CAN_FSCFGR_FSC27                        ((uint32_t)0x08000000) /* Filter Scale Configuration bit 27 */

/******************  Bit definition for CAN_FAFIFOR register  *******************/
#define CAN_FAFIFOR_FFA                         ((uint16_t)0x3FFF) /* Filter FIFO Assignment */
#define CAN_FAFIFOR_FFA0                        ((uint16_t)0x0001) /* Filter FIFO Assignment for Filter 0 */
#define CAN_FAFIFOR_FFA1                        ((uint16_t)0x0002) /* Filter FIFO Assignment for Filter 1 */
#define CAN_FAFIFOR_FFA2                        ((uint16_t)0x0004) /* Filter FIFO Assignment for Filter 2 */
#define CAN_FAFIFOR_FFA3                        ((uint16_t)0x0008) /* Filter FIFO Assignment for Filter 3 */
#define CAN_FAFIFOR_FFA4                        ((uint16_t)0x0010) /* Filter FIFO Assignment for Filter 4 */
#define CAN_FAFIFOR_FFA5                        ((uint16_t)0x0020) /* Filter FIFO Assignment for Filter 5 */
#define CAN_FAFIFOR_FFA6                        ((uint16_t)0x0040) /* Filter FIFO Assignment for Filter 6 */
#define CAN_FAFIFOR_FFA7                        ((uint16_t)0x0080) /* Filter FIFO Assignment for Filter 7 */
#define CAN_FAFIFOR_FFA8                        ((uint16_t)0x0100) /* Filter FIFO Assignment for Filter 8 */
#define CAN_FAFIFOR_FFA9                        ((uint16_t)0x0200) /* Filter FIFO Assignment for Filter 9 */
#define CAN_FAFIFOR_FFA10                       ((uint16_t)0x0400) /* Filter FIFO Assignment for Filter 10 */
#define CAN_FAFIFOR_FFA11                       ((uint16_t)0x0800) /* Filter FIFO Assignment for Filter 11 */
#define CAN_FAFIFOR_FFA12                       ((uint16_t)0x1000) /* Filter FIFO Assignment for Filter 12 */
#define CAN_FAFIFOR_FFA13                       ((uint32_t)0x00002000) /* Filter FIFO Assignment for Filter 13 */
#define CAN_FAFIFOR_FFA14                       ((uint32_t)0x00004000) /* Filter FIFO Assignment for Filter 14 */
#define CAN_FAFIFOR_FFA15                       ((uint32_t)0x00008000) /* Filter FIFO Assignment for Filter 15 */
#define CAN_FAFIFOR_FFA16                       ((uint32_t)0x00010000) /* Filter FIFO Assignment for Filter 16 */
#define CAN_FAFIFOR_FFA17                       ((uint32_t)0x00020000) /* Filter FIFO Assignment for Filter 17 */
#define CAN_FAFIFOR_FFA18                       ((uint32_t)0x00040000) /* Filter FIFO Assignment for Filter 18 */
#define CAN_FAFIFOR_FFA19                       ((uint32_t)0x00080000) /* Filter FIFO Assignment for Filter 19 */
#define CAN_FAFIFOR_FFA20                       ((uint32_t)0x00100000) /* Filter FIFO Assignment for Filter 20 */
#define CAN_FAFIFOR_FFA21                       ((uint32_t)0x00200000) /* Filter FIFO Assignment for Filter 21 */
#define CAN_FAFIFOR_FFA22                       ((uint32_t)0x00400000) /* Filter FIFO Assignment for Filter 22 */
#define CAN_FAFIFOR_FFA23                       ((uint32_t)0x00800000) /* Filter FIFO Assignment for Filter 23 */
#define CAN_FAFIFOR_FFA24                       ((uint32_t)0x01000000) /* Filter FIFO Assignment for Filter 24 */
#define CAN_FAFIFOR_FFA25                       ((uint32_t)0x02000000) /* Filter FIFO Assignment for Filter 25 */
#define CAN_FAFIFOR_FFA26                       ((uint32_t)0x04000000) /* Filter FIFO Assignment for Filter 26 */
#define CAN_FAFIFOR_FFA27                       ((uint32_t)0x08000000) /* Filter FIFO Assignment for Filter 27 */

/*******************  Bit definition for CAN_FWR register  *******************/
#define CAN_FWR_FACT                            ((uint16_t)0x3FFF) /* Filter Active */
#define CAN_FWR_FACT0                           ((uint16_t)0x0001) /* Filter 0 Active */
#define CAN_FWR_FACT1                           ((uint16_t)0x0002) /* Filter 1 Active */
#define CAN_FWR_FACT2                           ((uint16_t)0x0004) /* Filter 2 Active */
#define CAN_FWR_FACT3                           ((uint16_t)0x0008) /* Filter 3 Active */
#define CAN_FWR_FACT4                           ((uint16_t)0x0010) /* Filter 4 Active */
#define CAN_FWR_FACT5                           ((uint16_t)0x0020) /* Filter 5 Active */
#define CAN_FWR_FACT6                           ((uint16_t)0x0040) /* Filter 6 Active */
#define CAN_FWR_FACT7                           ((uint16_t)0x0080) /* Filter 7 Active */
#define CAN_FWR_FACT8                           ((uint16_t)0x0100) /* Filter 8 Active */
#define CAN_FWR_FACT9                           ((uint16_t)0x0200) /* Filter 9 Active */
#define CAN_FWR_FACT10                          ((uint16_t)0x0400) /* Filter 10 Active */
#define CAN_FWR_FACT11                          ((uint16_t)0x0800) /* Filter 11 Active */
#define CAN_FWR_FACT12                          ((uint16_t)0x1000) /* Filter 12 Active */
#define CAN_FWR_FACT13                          ((uint32_t)0x00002000) /* Filter 13 Active */
#define CAN_FWR_FACT14                          ((uint32_t)0x00004000) /* Filter 14 Active */
#define CAN_FWR_FACT15                          ((uint32_t)0x00008000) /* Filter 15 Active */
#define CAN_FWR_FACT16                          ((uint32_t)0x00010000) /* Filter 16 Active */
#define CAN_FWR_FACT17                          ((uint32_t)0x00020000) /* Filter 17 Active */
#define CAN_FWR_FACT18                          ((uint32_t)0x00040000) /* Filter 18 Active */
#define CAN_FWR_FACT19                          ((uint32_t)0x00080000) /* Filter 19 Active */
#define CAN_FWR_FACT20                          ((uint32_t)0x00100000) /* Filter 20 Active */
#define CAN_FWR_FACT21                          ((uint32_t)0x00200000) /* Filter 21 Active */
#define CAN_FWR_FACT22                          ((uint32_t)0x00400000) /* Filter 22 Active */
#define CAN_FWR_FACT23                          ((uint32_t)0x00800000) /* Filter 23 Active */
#define CAN_FWR_FACT24                          ((uint32_t)0x01000000) /* Filter 24 Active */
#define CAN_FWR_FACT25                          ((uint32_t)0x02000000) /* Filter 25 Active */
#define CAN_FWR_FACT26                          ((uint32_t)0x04000000) /* Filter 26 Active */
#define CAN_FWR_FACT27                          ((uint32_t)0x08000000) /* Filter 27 Active */

/*******************  Bit definition for CAN_F0R1 register  *******************/
#define CAN_F0R1_FB0                            ((uint32_t)0x00000001) /* Filter bit 0 */
#define CAN_F0R1_FB1                            ((uint32_t)0x00000002) /* Filter bit 1 */
#define CAN_F0R1_FB2                            ((uint32_t)0x00000004) /* Filter bit 2 */
#define CAN_F0R1_FB3                            ((uint32_t)0x00000008) /* Filter bit 3 */
#define CAN_F0R1_FB4                            ((uint32_t)0x00000010) /* Filter bit 4 */
#define CAN_F0R1_FB5                            ((uint32_t)0x00000020) /* Filter bit 5 */
#define CAN_F0R1_FB6                            ((uint32_t)0x00000040) /* Filter bit 6 */
#define CAN_F0R1_FB7                            ((uint32_t)0x00000080) /* Filter bit 7 */
#define CAN_F0R1_FB8                            ((uint32_t)0x00000100) /* Filter bit 8 */
#define CAN_F0R1_FB9                            ((uint32_t)0x00000200) /* Filter bit 9 */
#define CAN_F0R1_FB10                           ((uint32_t)0x00000400) /* Filter bit 10 */
#define CAN_F0R1_FB11                           ((uint32_t)0x00000800) /* Filter bit 11 */
#define CAN_F0R1_FB12                           ((uint32_t)0x00001000) /* Filter bit 12 */
#define CAN_F0R1_FB13                           ((uint32_t)0x00002000) /* Filter bit 13 */
#define CAN_F0R1_FB14                           ((uint32_t)0x00004000) /* Filter bit 14 */
#define CAN_F0R1_FB15                           ((uint32_t)0x00008000) /* Filter bit 15 */
#define CAN_F0R1_FB16                           ((uint32_t)0x00010000) /* Filter bit 16 */
#define CAN_F0R1_FB17                           ((uint32_t)0x00020000) /* Filter bit 17 */
#define CAN_F0R1_FB18                           ((uint32_t)0x00040000) /* Filter bit 18 */
#define CAN_F0R1_FB19                           ((uint32_t)0x00080000) /* Filter bit 19 */
#define CAN_F0R1_FB20                           ((uint32_t)0x00100000) /* Filter bit 20 */
#define CAN_F0R1_FB21                           ((uint32_t)0x00200000) /* Filter bit 21 */
#define CAN_F0R1_FB22                           ((uint32_t)0x00400000) /* Filter bit 22 */
#define CAN_F0R1_FB23                           ((uint32_t)0x00800000) /* Filter bit 23 */
#define CAN_F0R1_FB24                           ((uint32_t)0x01000000) /* Filter bit 24 */
#define CAN_F0R1_FB25                           ((uint32_t)0x02000000) /* Filter bit 25 */
#define CAN_F0R1_FB26                           ((uint32_t)0x04000000) /* Filter bit 26 */
#define CAN_F0R1_FB27                           ((uint32_t)0x08000000) /* Filter bit 27 */
#define CAN_F0R1_FB28                           ((uint32_t)0x10000000) /* Filter bit 28 */
#define CAN_F0R1_FB29                           ((uint32_t)0x20000000) /* Filter bit 29 */
#define CAN_F0R1_FB30                           ((uint32_t)0x40000000) /* Filter bit 30 */
#define CAN_F0R1_FB31                           ((uint32_t)0x80000000) /* Filter bit 31 */

/*******************  Bit definition for CAN_F1R1 register  *******************/
#define CAN_F1R1_FB0                            ((uint32_t)0x00000001) /* Filter bit 0 */
#define CAN_F1R1_FB1                            ((uint32_t)0x00000002) /* Filter bit 1 */
#define CAN_F1R1_FB2                            ((uint32_t)0x00000004) /* Filter bit 2 */
#define CAN_F1R1_FB3                            ((uint32_t)0x00000008) /* Filter bit 3 */
#define CAN_F1R1_FB4                            ((uint32_t)0x00000010) /* Filter bit 4 */
#define CAN_F1R1_FB5                            ((uint32_t)0x00000020) /* Filter bit 5 */
#define CAN_F1R1_FB6                            ((uint32_t)0x00000040) /* Filter bit 6 */
#define CAN_F1R1_FB7                            ((uint32_t)0x00000080) /* Filter bit 7 */
#define CAN_F1R1_FB8                            ((uint32_t)0x00000100) /* Filter bit 8 */
#define CAN_F1R1_FB9                            ((uint32_t)0x00000200) /* Filter bit 9 */
#define CAN_F1R1_FB10                           ((uint32_t)0x00000400) /* Filter bit 10 */
#define CAN_F1R1_FB11                           ((uint32_t)0x00000800) /* Filter bit 11 */
#define CAN_F1R1_FB12                           ((uint32_t)0x00001000) /* Filter bit 12 */
#define CAN_F1R1_FB13                           ((uint32_t)0x00002000) /* Filter bit 13 */
#define CAN_F1R1_FB14                           ((uint32_t)0x00004000) /* Filter bit 14 */
#define CAN_F1R1_FB15                           ((uint32_t)0x00008000) /* Filter bit 15 */
#define CAN_F1R1_FB16                           ((uint32_t)0x00010000) /* Filter bit 16 */
#define CAN_F1R1_FB17                           ((uint32_t)0x00020000) /* Filter bit 17 */
#define CAN_F1R1_FB18                           ((uint32_t)0x00040000) /* Filter bit 18 */
#define CAN_F1R1_FB19                           ((uint32_t)0x00080000) /* Filter bit 19 */
#define CAN_F1R1_FB20                           ((uint32_t)0x00100000) /* Filter bit 20 */
#define CAN_F1R1_FB21                           ((uint32_t)0x00200000) /* Filter bit 21 */
#define CAN_F1R1_FB22                           ((uint32_t)0x00400000) /* Filter bit 22 */
#define CAN_F1R1_FB23                           ((uint32_t)0x00800000) /* Filter bit 23 */
#define CAN_F1R1_FB24                           ((uint32_t)0x01000000) /* Filter bit 24 */
#define CAN_F1R1_FB25                           ((uint32_t)0x02000000) /* Filter bit 25 */
#define CAN_F1R1_FB26                           ((uint32_t)0x04000000) /* Filter bit 26 */
#define CAN_F1R1_FB27                           ((uint32_t)0x08000000) /* Filter bit 27 */
#define CAN_F1R1_FB28                           ((uint32_t)0x10000000) /* Filter bit 28 */
#define CAN_F1R1_FB29                           ((uint32_t)0x20000000) /* Filter bit 29 */
#define CAN_F1R1_FB30                           ((uint32_t)0x40000000) /* Filter bit 30 */
#define CAN_F1R1_FB31                           ((uint32_t)0x80000000) /* Filter bit 31 */

/*******************  Bit definition for CAN_F2R1 register  *******************/
#define CAN_F2R1_FB0                            ((uint32_t)0x00000001) /* Filter bit 0 */
#define CAN_F2R1_FB1                            ((uint32_t)0x00000002) /* Filter bit 1 */
#define CAN_F2R1_FB2                            ((uint32_t)0x00000004) /* Filter bit 2 */
#define CAN_F2R1_FB3                            ((uint32_t)0x00000008) /* Filter bit 3 */
#define CAN_F2R1_FB4                            ((uint32_t)0x00000010) /* Filter bit 4 */
#define CAN_F2R1_FB5                            ((uint32_t)0x00000020) /* Filter bit 5 */
#define CAN_F2R1_FB6                            ((uint32_t)0x00000040) /* Filter bit 6 */
#define CAN_F2R1_FB7                            ((uint32_t)0x00000080) /* Filter bit 7 */
#define CAN_F2R1_FB8                            ((uint32_t)0x00000100) /* Filter bit 8 */
#define CAN_F2R1_FB9                            ((uint32_t)0x00000200) /* Filter bit 9 */
#define CAN_F2R1_FB10                           ((uint32_t)0x00000400) /* Filter bit 10 */
#define CAN_F2R1_FB11                           ((uint32_t)0x00000800) /* Filter bit 11 */
#define CAN_F2R1_FB12                           ((uint32_t)0x00001000) /* Filter bit 12 */
#define CAN_F2R1_FB13                           ((uint32_t)0x00002000) /* Filter bit 13 */
#define CAN_F2R1_FB14                           ((uint32_t)0x00004000) /* Filter bit 14 */
#define CAN_F2R1_FB15                           ((uint32_t)0x00008000) /* Filter bit 15 */
#define CAN_F2R1_FB16                           ((uint32_t)0x00010000) /* Filter bit 16 */
#define CAN_F2R1_FB17                           ((uint32_t)0x00020000) /* Filter bit 17 */
#define CAN_F2R1_FB18                           ((uint32_t)0x00040000) /* Filter bit 18 */
#define CAN_F2R1_FB19                           ((uint32_t)0x00080000) /* Filter bit 19 */
#define CAN_F2R1_FB20                           ((uint32_t)0x00100000) /* Filter bit 20 */
#define CAN_F2R1_FB21                           ((uint32_t)0x00200000) /* Filter bit 21 */
#define CAN_F2R1_FB22                           ((uint32_t)0x00400000) /* Filter bit 22 */
#define CAN_F2R1_FB23                           ((uint32_t)0x00800000) /* Filter bit 23 */
#define CAN_F2R1_FB24                           ((uint32_t)0x01000000) /* Filter bit 24 */
#define CAN_F2R1_FB25                           ((uint32_t)0x02000000) /* Filter bit 25 */
#define CAN_F2R1_FB26                           ((uint32_t)0x04000000) /* Filter bit 26 */
#define CAN_F2R1_FB27                           ((uint32_t)0x08000000) /* Filter bit 27 */
#define CAN_F2R1_FB28                           ((uint32_t)0x10000000) /* Filter bit 28 */
#define CAN_F2R1_FB29                           ((uint32_t)0x20000000) /* Filter bit 29 */
#define CAN_F2R1_FB30                           ((uint32_t)0x40000000) /* Filter bit 30 */
#define CAN_F2R1_FB31                           ((uint32_t)0x80000000) /* Filter bit 31 */

/*******************  Bit definition for CAN_F3R1 register  *******************/
#define CAN_F3R1_FB0                            ((uint32_t)0x00000001) /* Filter bit 0 */
#define CAN_F3R1_FB1                            ((uint32_t)0x00000002) /* Filter bit 1 */
#define CAN_F3R1_FB2                            ((uint32_t)0x00000004) /* Filter bit 2 */
#define CAN_F3R1_FB3                            ((uint32_t)0x00000008) /* Filter bit 3 */
#define CAN_F3R1_FB4                            ((uint32_t)0x00000010) /* Filter bit 4 */
#define CAN_F3R1_FB5                            ((uint32_t)0x00000020) /* Filter bit 5 */
#define CAN_F3R1_FB6                            ((uint32_t)0x00000040) /* Filter bit 6 */
#define CAN_F3R1_FB7                            ((uint32_t)0x00000080) /* Filter bit 7 */
#define CAN_F3R1_FB8                            ((uint32_t)0x00000100) /* Filter bit 8 */
#define CAN_F3R1_FB9                            ((uint32_t)0x00000200) /* Filter bit 9 */
#define CAN_F3R1_FB10                           ((uint32_t)0x00000400) /* Filter bit 10 */
#define CAN_F3R1_FB11                           ((uint32_t)0x00000800) /* Filter bit 11 */
#define CAN_F3R1_FB12                           ((uint32_t)0x00001000) /* Filter bit 12 */
#define CAN_F3R1_FB13                           ((uint32_t)0x00002000) /* Filter bit 13 */
#define CAN_F3R1_FB14                           ((uint32_t)0x00004000) /* Filter bit 14 */
#define CAN_F3R1_FB15                           ((uint32_t)0x00008000) /* Filter bit 15 */
#define CAN_F3R1_FB16                           ((uint32_t)0x00010000) /* Filter bit 16 */
#define CAN_F3R1_FB17                           ((uint32_t)0x00020000) /* Filter bit 17 */
#define CAN_F3R1_FB18                           ((uint32_t)0x00040000) /* Filter bit 18 */
#define CAN_F3R1_FB19                           ((uint32_t)0x00080000) /* Filter bit 19 */
#define CAN_F3R1_FB20                           ((uint32_t)0x00100000) /* Filter bit 20 */
#define CAN_F3R1_FB21                           ((uint32_t)0x00200000) /* Filter bit 21 */
#define CAN_F3R1_FB22                           ((uint32_t)0x00400000) /* Filter bit 22 */
#define CAN_F3R1_FB23                           ((uint32_t)0x00800000) /* Filter bit 23 */
#define CAN_F3R1_FB24                           ((uint32_t)0x01000000) /* Filter bit 24 */
#define CAN_F3R1_FB25                           ((uint32_t)0x02000000) /* Filter bit 25 */
#define CAN_F3R1_FB26                           ((uint32_t)0x04000000) /* Filter bit 26 */
#define CAN_F3R1_FB27                           ((uint32_t)0x08000000) /* Filter bit 27 */
#define CAN_F3R1_FB28                           ((uint32_t)0x10000000) /* Filter bit 28 */
#define CAN_F3R1_FB29                           ((uint32_t)0x20000000) /* Filter bit 29 */
#define CAN_F3R1_FB30                           ((uint32_t)0x40000000) /* Filter bit 30 */
#define CAN_F3R1_FB31                           ((uint32_t)0x80000000) /* Filter bit 31 */

/*******************  Bit definition for CAN_F4R1 register  *******************/
#define CAN_F4R1_FB0                            ((uint32_t)0x00000001) /* Filter bit 0 */
#define CAN_F4R1_FB1                            ((uint32_t)0x00000002) /* Filter bit 1 */
#define CAN_F4R1_FB2                            ((uint32_t)0x00000004) /* Filter bit 2 */
#define CAN_F4R1_FB3                            ((uint32_t)0x00000008) /* Filter bit 3 */
#define CAN_F4R1_FB4                            ((uint32_t)0x00000010) /* Filter bit 4 */
#define CAN_F4R1_FB5                            ((uint32_t)0x00000020) /* Filter bit 5 */
#define CAN_F4R1_FB6                            ((uint32_t)0x00000040) /* Filter bit 6 */
#define CAN_F4R1_FB7                            ((uint32_t)0x00000080) /* Filter bit 7 */
#define CAN_F4R1_FB8                            ((uint32_t)0x00000100) /* Filter bit 8 */
#define CAN_F4R1_FB9                            ((uint32_t)0x00000200) /* Filter bit 9 */
#define CAN_F4R1_FB10                           ((uint32_t)0x00000400) /* Filter bit 10 */
#define CAN_F4R1_FB11                           ((uint32_t)0x00000800) /* Filter bit 11 */
#define CAN_F4R1_FB12                           ((uint32_t)0x00001000) /* Filter bit 12 */
#define CAN_F4R1_FB13                           ((uint32_t)0x00002000) /* Filter bit 13 */
#define CAN_F4R1_FB14                           ((uint32_t)0x00004000) /* Filter bit 14 */
#define CAN_F4R1_FB15                           ((uint32_t)0x00008000) /* Filter bit 15 */
#define CAN_F4R1_FB16                           ((uint32_t)0x00010000) /* Filter bit 16 */
#define CAN_F4R1_FB17                           ((uint32_t)0x00020000) /* Filter bit 17 */
#define CAN_F4R1_FB18                           ((uint32_t)0x00040000) /* Filter bit 18 */
#define CAN_F4R1_FB19                           ((uint32_t)0x00080000) /* Filter bit 19 */
#define CAN_F4R1_FB20                           ((uint32_t)0x00100000) /* Filter bit 20 */
#define CAN_F4R1_FB21                           ((uint32_t)0x00200000) /* Filter bit 21 */
#define CAN_F4R1_FB22                           ((uint32_t)0x00400000) /* Filter bit 22 */
#define CAN_F4R1_FB23                           ((uint32_t)0x00800000) /* Filter bit 23 */
#define CAN_F4R1_FB24                           ((uint32_t)0x01000000) /* Filter bit 24 */
#define CAN_F4R1_FB25                           ((uint32_t)0x02000000) /* Filter bit 25 */
#define CAN_F4R1_FB26                           ((uint32_t)0x04000000) /* Filter bit 26 */
#define CAN_F4R1_FB27                           ((uint32_t)0x08000000) /* Filter bit 27 */
#define CAN_F4R1_FB28                           ((uint32_t)0x10000000) /* Filter bit 28 */
#define CAN_F4R1_FB29                           ((uint32_t)0x20000000) /* Filter bit 29 */
#define CAN_F4R1_FB30                           ((uint32_t)0x40000000) /* Filter bit 30 */
#define CAN_F4R1_FB31                           ((uint32_t)0x80000000) /* Filter bit 31 */

/*******************  Bit definition for CAN_F5R1 register  *******************/
#define CAN_F5R1_FB0                            ((uint32_t)0x00000001) /* Filter bit 0 */
#define CAN_F5R1_FB1                            ((uint32_t)0x00000002) /* Filter bit 1 */
#define CAN_F5R1_FB2                            ((uint32_t)0x00000004) /* Filter bit 2 */
#define CAN_F5R1_FB3                            ((uint32_t)0x00000008) /* Filter bit 3 */
#define CAN_F5R1_FB4                            ((uint32_t)0x00000010) /* Filter bit 4 */
#define CAN_F5R1_FB5                            ((uint32_t)0x00000020) /* Filter bit 5 */
#define CAN_F5R1_FB6                            ((uint32_t)0x00000040) /* Filter bit 6 */
#define CAN_F5R1_FB7                            ((uint32_t)0x00000080) /* Filter bit 7 */
#define CAN_F5R1_FB8                            ((uint32_t)0x00000100) /* Filter bit 8 */
#define CAN_F5R1_FB9                            ((uint32_t)0x00000200) /* Filter bit 9 */
#define CAN_F5R1_FB10                           ((uint32_t)0x00000400) /* Filter bit 10 */
#define CAN_F5R1_FB11                           ((uint32_t)0x00000800) /* Filter bit 11 */
#define CAN_F5R1_FB12                           ((uint32_t)0x00001000) /* Filter bit 12 */
#define CAN_F5R1_FB13                           ((uint32_t)0x00002000) /* Filter bit 13 */
#define CAN_F5R1_FB14                           ((uint32_t)0x00004000) /* Filter bit 14 */
#define CAN_F5R1_FB15                           ((uint32_t)0x00008000) /* Filter bit 15 */
#define CAN_F5R1_FB16                           ((uint32_t)0x00010000) /* Filter bit 16 */
#define CAN_F5R1_FB17                           ((uint32_t)0x00020000) /* Filter bit 17 */
#define CAN_F5R1_FB18                           ((uint32_t)0x00040000) /* Filter bit 18 */
#define CAN_F5R1_FB19                           ((uint32_t)0x00080000) /* Filter bit 19 */
#define CAN_F5R1_FB20                           ((uint32_t)0x00100000) /* Filter bit 20 */
#define CAN_F5R1_FB21                           ((uint32_t)0x00200000) /* Filter bit 21 */
#define CAN_F5R1_FB22                           ((uint32_t)0x00400000) /* Filter bit 22 */
#define CAN_F5R1_FB23                           ((uint32_t)0x00800000) /* Filter bit 23 */
#define CAN_F5R1_FB24                           ((uint32_t)0x01000000) /* Filter bit 24 */
#define CAN_F5R1_FB25                           ((uint32_t)0x02000000) /* Filter bit 25 */
#define CAN_F5R1_FB26                           ((uint32_t)0x04000000) /* Filter bit 26 */
#define CAN_F5R1_FB27                           ((uint32_t)0x08000000) /* Filter bit 27 */
#define CAN_F5R1_FB28                           ((uint32_t)0x10000000) /* Filter bit 28 */
#define CAN_F5R1_FB29                           ((uint32_t)0x20000000) /* Filter bit 29 */
#define CAN_F5R1_FB30                           ((uint32_t)0x40000000) /* Filter bit 30 */
#define CAN_F5R1_FB31                           ((uint32_t)0x80000000) /* Filter bit 31 */

/*******************  Bit definition for CAN_F6R1 register  *******************/
#define CAN_F6R1_FB0                            ((uint32_t)0x00000001) /* Filter bit 0 */
#define CAN_F6R1_FB1                            ((uint32_t)0x00000002) /* Filter bit 1 */
#define CAN_F6R1_FB2                            ((uint32_t)0x00000004) /* Filter bit 2 */
#define CAN_F6R1_FB3                            ((uint32_t)0x00000008) /* Filter bit 3 */
#define CAN_F6R1_FB4                            ((uint32_t)0x00000010) /* Filter bit 4 */
#define CAN_F6R1_FB5                            ((uint32_t)0x00000020) /* Filter bit 5 */
#define CAN_F6R1_FB6                            ((uint32_t)0x00000040) /* Filter bit 6 */
#define CAN_F6R1_FB7                            ((uint32_t)0x00000080) /* Filter bit 7 */
#define CAN_F6R1_FB8                            ((uint32_t)0x00000100) /* Filter bit 8 */
#define CAN_F6R1_FB9                            ((uint32_t)0x00000200) /* Filter bit 9 */
#define CAN_F6R1_FB10                           ((uint32_t)0x00000400) /* Filter bit 10 */
#define CAN_F6R1_FB11                           ((uint32_t)0x00000800) /* Filter bit 11 */
#define CAN_F6R1_FB12                           ((uint32_t)0x00001000) /* Filter bit 12 */
#define CAN_F6R1_FB13                           ((uint32_t)0x00002000) /* Filter bit 13 */
#define CAN_F6R1_FB14                           ((uint32_t)0x00004000) /* Filter bit 14 */
#define CAN_F6R1_FB15                           ((uint32_t)0x00008000) /* Filter bit 15 */
#define CAN_F6R1_FB16                           ((uint32_t)0x00010000) /* Filter bit 16 */
#define CAN_F6R1_FB17                           ((uint32_t)0x00020000) /* Filter bit 17 */
#define CAN_F6R1_FB18                           ((uint32_t)0x00040000) /* Filter bit 18 */
#define CAN_F6R1_FB19                           ((uint32_t)0x00080000) /* Filter bit 19 */
#define CAN_F6R1_FB20                           ((uint32_t)0x00100000) /* Filter bit 20 */
#define CAN_F6R1_FB21                           ((uint32_t)0x00200000) /* Filter bit 21 */
#define CAN_F6R1_FB22                           ((uint32_t)0x00400000) /* Filter bit 22 */
#define CAN_F6R1_FB23                           ((uint32_t)0x00800000) /* Filter bit 23 */
#define CAN_F6R1_FB24                           ((uint32_t)0x01000000) /* Filter bit 24 */
#define CAN_F6R1_FB25                           ((uint32_t)0x02000000) /* Filter bit 25 */
#define CAN_F6R1_FB26                           ((uint32_t)0x04000000) /* Filter bit 26 */
#define CAN_F6R1_FB27                           ((uint32_t)0x08000000) /* Filter bit 27 */
#define CAN_F6R1_FB28                           ((uint32_t)0x10000000) /* Filter bit 28 */
#define CAN_F6R1_FB29                           ((uint32_t)0x20000000) /* Filter bit 29 */
#define CAN_F6R1_FB30                           ((uint32_t)0x40000000) /* Filter bit 30 */
#define CAN_F6R1_FB31                           ((uint32_t)0x80000000) /* Filter bit 31 */

/*******************  Bit definition for CAN_F7R1 register  *******************/
#define CAN_F7R1_FB0                            ((uint32_t)0x00000001) /* Filter bit 0 */
#define CAN_F7R1_FB1                            ((uint32_t)0x00000002) /* Filter bit 1 */
#define CAN_F7R1_FB2                            ((uint32_t)0x00000004) /* Filter bit 2 */
#define CAN_F7R1_FB3                            ((uint32_t)0x00000008) /* Filter bit 3 */
#define CAN_F7R1_FB4                            ((uint32_t)0x00000010) /* Filter bit 4 */
#define CAN_F7R1_FB5                            ((uint32_t)0x00000020) /* Filter bit 5 */
#define CAN_F7R1_FB6                            ((uint32_t)0x00000040) /* Filter bit 6 */
#define CAN_F7R1_FB7                            ((uint32_t)0x00000080) /* Filter bit 7 */
#define CAN_F7R1_FB8                            ((uint32_t)0x00000100) /* Filter bit 8 */
#define CAN_F7R1_FB9                            ((uint32_t)0x00000200) /* Filter bit 9 */
#define CAN_F7R1_FB10                           ((uint32_t)0x00000400) /* Filter bit 10 */
#define CAN_F7R1_FB11                           ((uint32_t)0x00000800) /* Filter bit 11 */
#define CAN_F7R1_FB12                           ((uint32_t)0x00001000) /* Filter bit 12 */
#define CAN_F7R1_FB13                           ((uint32_t)0x00002000) /* Filter bit 13 */
#define CAN_F7R1_FB14                           ((uint32_t)0x00004000) /* Filter bit 14 */
#define CAN_F7R1_FB15                           ((uint32_t)0x00008000) /* Filter bit 15 */
#define CAN_F7R1_FB16                           ((uint32_t)0x00010000) /* Filter bit 16 */
#define CAN_F7R1_FB17                           ((uint32_t)0x00020000) /* Filter bit 17 */
#define CAN_F7R1_FB18                           ((uint32_t)0x00040000) /* Filter bit 18 */
#define CAN_F7R1_FB19                           ((uint32_t)0x00080000) /* Filter bit 19 */
#define CAN_F7R1_FB20                           ((uint32_t)0x00100000) /* Filter bit 20 */
#define CAN_F7R1_FB21                           ((uint32_t)0x00200000) /* Filter bit 21 */
#define CAN_F7R1_FB22                           ((uint32_t)0x00400000) /* Filter bit 22 */
#define CAN_F7R1_FB23                           ((uint32_t)0x00800000) /* Filter bit 23 */
#define CAN_F7R1_FB24                           ((uint32_t)0x01000000) /* Filter bit 24 */
#define CAN_F7R1_FB25                           ((uint32_t)0x02000000) /* Filter bit 25 */
#define CAN_F7R1_FB26                           ((uint32_t)0x04000000) /* Filter bit 26 */
#define CAN_F7R1_FB27                           ((uint32_t)0x08000000) /* Filter bit 27 */
#define CAN_F7R1_FB28                           ((uint32_t)0x10000000) /* Filter bit 28 */
#define CAN_F7R1_FB29                           ((uint32_t)0x20000000) /* Filter bit 29 */
#define CAN_F7R1_FB30                           ((uint32_t)0x40000000) /* Filter bit 30 */
#define CAN_F7R1_FB31                           ((uint32_t)0x80000000) /* Filter bit 31 */

/*******************  Bit definition for CAN_F8R1 register  *******************/
#define CAN_F8R1_FB0                            ((uint32_t)0x00000001) /* Filter bit 0 */
#define CAN_F8R1_FB1                            ((uint32_t)0x00000002) /* Filter bit 1 */
#define CAN_F8R1_FB2                            ((uint32_t)0x00000004) /* Filter bit 2 */
#define CAN_F8R1_FB3                            ((uint32_t)0x00000008) /* Filter bit 3 */
#define CAN_F8R1_FB4                            ((uint32_t)0x00000010) /* Filter bit 4 */
#define CAN_F8R1_FB5                            ((uint32_t)0x00000020) /* Filter bit 5 */
#define CAN_F8R1_FB6                            ((uint32_t)0x00000040) /* Filter bit 6 */
#define CAN_F8R1_FB7                            ((uint32_t)0x00000080) /* Filter bit 7 */
#define CAN_F8R1_FB8                            ((uint32_t)0x00000100) /* Filter bit 8 */
#define CAN_F8R1_FB9                            ((uint32_t)0x00000200) /* Filter bit 9 */
#define CAN_F8R1_FB10                           ((uint32_t)0x00000400) /* Filter bit 10 */
#define CAN_F8R1_FB11                           ((uint32_t)0x00000800) /* Filter bit 11 */
#define CAN_F8R1_FB12                           ((uint32_t)0x00001000) /* Filter bit 12 */
#define CAN_F8R1_FB13                           ((uint32_t)0x00002000) /* Filter bit 13 */
#define CAN_F8R1_FB14                           ((uint32_t)0x00004000) /* Filter bit 14 */
#define CAN_F8R1_FB15                           ((uint32_t)0x00008000) /* Filter bit 15 */
#define CAN_F8R1_FB16                           ((uint32_t)0x00010000) /* Filter bit 16 */
#define CAN_F8R1_FB17                           ((uint32_t)0x00020000) /* Filter bit 17 */
#define CAN_F8R1_FB18                           ((uint32_t)0x00040000) /* Filter bit 18 */
#define CAN_F8R1_FB19                           ((uint32_t)0x00080000) /* Filter bit 19 */
#define CAN_F8R1_FB20                           ((uint32_t)0x00100000) /* Filter bit 20 */
#define CAN_F8R1_FB21                           ((uint32_t)0x00200000) /* Filter bit 21 */
#define CAN_F8R1_FB22                           ((uint32_t)0x00400000) /* Filter bit 22 */
#define CAN_F8R1_FB23                           ((uint32_t)0x00800000) /* Filter bit 23 */
#define CAN_F8R1_FB24                           ((uint32_t)0x01000000) /* Filter bit 24 */
#define CAN_F8R1_FB25                           ((uint32_t)0x02000000) /* Filter bit 25 */
#define CAN_F8R1_FB26                           ((uint32_t)0x04000000) /* Filter bit 26 */
#define CAN_F8R1_FB27                           ((uint32_t)0x08000000) /* Filter bit 27 */
#define CAN_F8R1_FB28                           ((uint32_t)0x10000000) /* Filter bit 28 */
#define CAN_F8R1_FB29                           ((uint32_t)0x20000000) /* Filter bit 29 */
#define CAN_F8R1_FB30                           ((uint32_t)0x40000000) /* Filter bit 30 */
#define CAN_F8R1_FB31                           ((uint32_t)0x80000000) /* Filter bit 31 */

/*******************  Bit definition for CAN_F9R1 register  *******************/
#define CAN_F9R1_FB0                            ((uint32_t)0x00000001) /* Filter bit 0 */
#define CAN_F9R1_FB1                            ((uint32_t)0x00000002) /* Filter bit 1 */
#define CAN_F9R1_FB2                            ((uint32_t)0x00000004) /* Filter bit 2 */
#define CAN_F9R1_FB3                            ((uint32_t)0x00000008) /* Filter bit 3 */
#define CAN_F9R1_FB4                            ((uint32_t)0x00000010) /* Filter bit 4 */
#define CAN_F9R1_FB5                            ((uint32_t)0x00000020) /* Filter bit 5 */
#define CAN_F9R1_FB6                            ((uint32_t)0x00000040) /* Filter bit 6 */
#define CAN_F9R1_FB7                            ((uint32_t)0x00000080) /* Filter bit 7 */
#define CAN_F9R1_FB8                            ((uint32_t)0x00000100) /* Filter bit 8 */
#define CAN_F9R1_FB9                            ((uint32_t)0x00000200) /* Filter bit 9 */
#define CAN_F9R1_FB10                           ((uint32_t)0x00000400) /* Filter bit 10 */
#define CAN_F9R1_FB11                           ((uint32_t)0x00000800) /* Filter bit 11 */
#define CAN_F9R1_FB12                           ((uint32_t)0x00001000) /* Filter bit 12 */
#define CAN_F9R1_FB13                           ((uint32_t)0x00002000) /* Filter bit 13 */
#define CAN_F9R1_FB14                           ((uint32_t)0x00004000) /* Filter bit 14 */
#define CAN_F9R1_FB15                           ((uint32_t)0x00008000) /* Filter bit 15 */
#define CAN_F9R1_FB16                           ((uint32_t)0x00010000) /* Filter bit 16 */
#define CAN_F9R1_FB17                           ((uint32_t)0x00020000) /* Filter bit 17 */
#define CAN_F9R1_FB18                           ((uint32_t)0x00040000) /* Filter bit 18 */
#define CAN_F9R1_FB19                           ((uint32_t)0x00080000) /* Filter bit 19 */
#define CAN_F9R1_FB20                           ((uint32_t)0x00100000) /* Filter bit 20 */
#define CAN_F9R1_FB21                           ((uint32_t)0x00200000) /* Filter bit 21 */
#define CAN_F9R1_FB22                           ((uint32_t)0x00400000) /* Filter bit 22 */
#define CAN_F9R1_FB23                           ((uint32_t)0x00800000) /* Filter bit 23 */
#define CAN_F9R1_FB24                           ((uint32_t)0x01000000) /* Filter bit 24 */
#define CAN_F9R1_FB25                           ((uint32_t)0x02000000) /* Filter bit 25 */
#define CAN_F9R1_FB26                           ((uint32_t)0x04000000) /* Filter bit 26 */
#define CAN_F9R1_FB27                           ((uint32_t)0x08000000) /* Filter bit 27 */
#define CAN_F9R1_FB28                           ((uint32_t)0x10000000) /* Filter bit 28 */
#define CAN_F9R1_FB29                           ((uint32_t)0x20000000) /* Filter bit 29 */
#define CAN_F9R1_FB30                           ((uint32_t)0x40000000) /* Filter bit 30 */
#define CAN_F9R1_FB31                           ((uint32_t)0x80000000) /* Filter bit 31 */

/*******************  Bit definition for CAN_F10R1 register  ******************/
#define CAN_F10R1_FB0                           ((uint32_t)0x00000001) /* Filter bit 0 */
#define CAN_F10R1_FB1                           ((uint32_t)0x00000002) /* Filter bit 1 */
#define CAN_F10R1_FB2                           ((uint32_t)0x00000004) /* Filter bit 2 */
#define CAN_F10R1_FB3                           ((uint32_t)0x00000008) /* Filter bit 3 */
#define CAN_F10R1_FB4                           ((uint32_t)0x00000010) /* Filter bit 4 */
#define CAN_F10R1_FB5                           ((uint32_t)0x00000020) /* Filter bit 5 */
#define CAN_F10R1_FB6                           ((uint32_t)0x00000040) /* Filter bit 6 */
#define CAN_F10R1_FB7                           ((uint32_t)0x00000080) /* Filter bit 7 */
#define CAN_F10R1_FB8                           ((uint32_t)0x00000100) /* Filter bit 8 */
#define CAN_F10R1_FB9                           ((uint32_t)0x00000200) /* Filter bit 9 */
#define CAN_F10R1_FB10                          ((uint32_t)0x00000400) /* Filter bit 10 */
#define CAN_F10R1_FB11                          ((uint32_t)0x00000800) /* Filter bit 11 */
#define CAN_F10R1_FB12                          ((uint32_t)0x00001000) /* Filter bit 12 */
#define CAN_F10R1_FB13                          ((uint32_t)0x00002000) /* Filter bit 13 */
#define CAN_F10R1_FB14                          ((uint32_t)0x00004000) /* Filter bit 14 */
#define CAN_F10R1_FB15                          ((uint32_t)0x00008000) /* Filter bit 15 */
#define CAN_F10R1_FB16                          ((uint32_t)0x00010000) /* Filter bit 16 */
#define CAN_F10R1_FB17                          ((uint32_t)0x00020000) /* Filter bit 17 */
#define CAN_F10R1_FB18                          ((uint32_t)0x00040000) /* Filter bit 18 */
#define CAN_F10R1_FB19                          ((uint32_t)0x00080000) /* Filter bit 19 */
#define CAN_F10R1_FB20                          ((uint32_t)0x00100000) /* Filter bit 20 */
#define CAN_F10R1_FB21                          ((uint32_t)0x00200000) /* Filter bit 21 */
#define CAN_F10R1_FB22                          ((uint32_t)0x00400000) /* Filter bit 22 */
#define CAN_F10R1_FB23                          ((uint32_t)0x00800000) /* Filter bit 23 */
#define CAN_F10R1_FB24                          ((uint32_t)0x01000000) /* Filter bit 24 */
#define CAN_F10R1_FB25                          ((uint32_t)0x02000000) /* Filter bit 25 */
#define CAN_F10R1_FB26                          ((uint32_t)0x04000000) /* Filter bit 26 */
#define CAN_F10R1_FB27                          ((uint32_t)0x08000000) /* Filter bit 27 */
#define CAN_F10R1_FB28                          ((uint32_t)0x10000000) /* Filter bit 28 */
#define CAN_F10R1_FB29                          ((uint32_t)0x20000000) /* Filter bit 29 */
#define CAN_F10R1_FB30                          ((uint32_t)0x40000000) /* Filter bit 30 */
#define CAN_F10R1_FB31                          ((uint32_t)0x80000000) /* Filter bit 31 */

/*******************  Bit definition for CAN_F11R1 register  ******************/
#define CAN_F11R1_FB0                           ((uint32_t)0x00000001) /* Filter bit 0 */
#define CAN_F11R1_FB1                           ((uint32_t)0x00000002) /* Filter bit 1 */
#define CAN_F11R1_FB2                           ((uint32_t)0x00000004) /* Filter bit 2 */
#define CAN_F11R1_FB3                           ((uint32_t)0x00000008) /* Filter bit 3 */
#define CAN_F11R1_FB4                           ((uint32_t)0x00000010) /* Filter bit 4 */
#define CAN_F11R1_FB5                           ((uint32_t)0x00000020) /* Filter bit 5 */
#define CAN_F11R1_FB6                           ((uint32_t)0x00000040) /* Filter bit 6 */
#define CAN_F11R1_FB7                           ((uint32_t)0x00000080) /* Filter bit 7 */
#define CAN_F11R1_FB8                           ((uint32_t)0x00000100) /* Filter bit 8 */
#define CAN_F11R1_FB9                           ((uint32_t)0x00000200) /* Filter bit 9 */
#define CAN_F11R1_FB10                          ((uint32_t)0x00000400) /* Filter bit 10 */
#define CAN_F11R1_FB11                          ((uint32_t)0x00000800) /* Filter bit 11 */
#define CAN_F11R1_FB12                          ((uint32_t)0x00001000) /* Filter bit 12 */
#define CAN_F11R1_FB13                          ((uint32_t)0x00002000) /* Filter bit 13 */
#define CAN_F11R1_FB14                          ((uint32_t)0x00004000) /* Filter bit 14 */
#define CAN_F11R1_FB15                          ((uint32_t)0x00008000) /* Filter bit 15 */
#define CAN_F11R1_FB16                          ((uint32_t)0x00010000) /* Filter bit 16 */
#define CAN_F11R1_FB17                          ((uint32_t)0x00020000) /* Filter bit 17 */
#define CAN_F11R1_FB18                          ((uint32_t)0x00040000) /* Filter bit 18 */
#define CAN_F11R1_FB19                          ((uint32_t)0x00080000) /* Filter bit 19 */
#define CAN_F11R1_FB20                          ((uint32_t)0x00100000) /* Filter bit 20 */
#define CAN_F11R1_FB21                          ((uint32_t)0x00200000) /* Filter bit 21 */
#define CAN_F11R1_FB22                          ((uint32_t)0x00400000) /* Filter bit 22 */
#define CAN_F11R1_FB23                          ((uint32_t)0x00800000) /* Filter bit 23 */
#define CAN_F11R1_FB24                          ((uint32_t)0x01000000) /* Filter bit 24 */
#define CAN_F11R1_FB25                          ((uint32_t)0x02000000) /* Filter bit 25 */
#define CAN_F11R1_FB26                          ((uint32_t)0x04000000) /* Filter bit 26 */
#define CAN_F11R1_FB27                          ((uint32_t)0x08000000) /* Filter bit 27 */
#define CAN_F11R1_FB28                          ((uint32_t)0x10000000) /* Filter bit 28 */
#define CAN_F11R1_FB29                          ((uint32_t)0x20000000) /* Filter bit 29 */
#define CAN_F11R1_FB30                          ((uint32_t)0x40000000) /* Filter bit 30 */
#define CAN_F11R1_FB31                          ((uint32_t)0x80000000) /* Filter bit 31 */

/*******************  Bit definition for CAN_F12R1 register  ******************/
#define CAN_F12R1_FB0                           ((uint32_t)0x00000001) /* Filter bit 0 */
#define CAN_F12R1_FB1                           ((uint32_t)0x00000002) /* Filter bit 1 */
#define CAN_F12R1_FB2                           ((uint32_t)0x00000004) /* Filter bit 2 */
#define CAN_F12R1_FB3                           ((uint32_t)0x00000008) /* Filter bit 3 */
#define CAN_F12R1_FB4                           ((uint32_t)0x00000010) /* Filter bit 4 */
#define CAN_F12R1_FB5                           ((uint32_t)0x00000020) /* Filter bit 5 */
#define CAN_F12R1_FB6                           ((uint32_t)0x00000040) /* Filter bit 6 */
#define CAN_F12R1_FB7                           ((uint32_t)0x00000080) /* Filter bit 7 */
#define CAN_F12R1_FB8                           ((uint32_t)0x00000100) /* Filter bit 8 */
#define CAN_F12R1_FB9                           ((uint32_t)0x00000200) /* Filter bit 9 */
#define CAN_F12R1_FB10                          ((uint32_t)0x00000400) /* Filter bit 10 */
#define CAN_F12R1_FB11                          ((uint32_t)0x00000800) /* Filter bit 11 */
#define CAN_F12R1_FB12                          ((uint32_t)0x00001000) /* Filter bit 12 */
#define CAN_F12R1_FB13                          ((uint32_t)0x00002000) /* Filter bit 13 */
#define CAN_F12R1_FB14                          ((uint32_t)0x00004000) /* Filter bit 14 */
#define CAN_F12R1_FB15                          ((uint32_t)0x00008000) /* Filter bit 15 */
#define CAN_F12R1_FB16                          ((uint32_t)0x00010000) /* Filter bit 16 */
#define CAN_F12R1_FB17                          ((uint32_t)0x00020000) /* Filter bit 17 */
#define CAN_F12R1_FB18                          ((uint32_t)0x00040000) /* Filter bit 18 */
#define CAN_F12R1_FB19                          ((uint32_t)0x00080000) /* Filter bit 19 */
#define CAN_F12R1_FB20                          ((uint32_t)0x00100000) /* Filter bit 20 */
#define CAN_F12R1_FB21                          ((uint32_t)0x00200000) /* Filter bit 21 */
#define CAN_F12R1_FB22                          ((uint32_t)0x00400000) /* Filter bit 22 */
#define CAN_F12R1_FB23                          ((uint32_t)0x00800000) /* Filter bit 23 */
#define CAN_F12R1_FB24                          ((uint32_t)0x01000000) /* Filter bit 24 */
#define CAN_F12R1_FB25                          ((uint32_t)0x02000000) /* Filter bit 25 */
#define CAN_F12R1_FB26                          ((uint32_t)0x04000000) /* Filter bit 26 */
#define CAN_F12R1_FB27                          ((uint32_t)0x08000000) /* Filter bit 27 */
#define CAN_F12R1_FB28                          ((uint32_t)0x10000000) /* Filter bit 28 */
#define CAN_F12R1_FB29                          ((uint32_t)0x20000000) /* Filter bit 29 */
#define CAN_F12R1_FB30                          ((uint32_t)0x40000000) /* Filter bit 30 */
#define CAN_F12R1_FB31                          ((uint32_t)0x80000000) /* Filter bit 31 */

/*******************  Bit definition for CAN_F13R1 register  ******************/
#define CAN_F13R1_FB0                           ((uint32_t)0x00000001) /* Filter bit 0 */
#define CAN_F13R1_FB1                           ((uint32_t)0x00000002) /* Filter bit 1 */
#define CAN_F13R1_FB2                           ((uint32_t)0x00000004) /* Filter bit 2 */
#define CAN_F13R1_FB3                           ((uint32_t)0x00000008) /* Filter bit 3 */
#define CAN_F13R1_FB4                           ((uint32_t)0x00000010) /* Filter bit 4 */
#define CAN_F13R1_FB5                           ((uint32_t)0x00000020) /* Filter bit 5 */
#define CAN_F13R1_FB6                           ((uint32_t)0x00000040) /* Filter bit 6 */
#define CAN_F13R1_FB7                           ((uint32_t)0x00000080) /* Filter bit 7 */
#define CAN_F13R1_FB8                           ((uint32_t)0x00000100) /* Filter bit 8 */
#define CAN_F13R1_FB9                           ((uint32_t)0x00000200) /* Filter bit 9 */
#define CAN_F13R1_FB10                          ((uint32_t)0x00000400) /* Filter bit 10 */
#define CAN_F13R1_FB11                          ((uint32_t)0x00000800) /* Filter bit 11 */
#define CAN_F13R1_FB12                          ((uint32_t)0x00001000) /* Filter bit 12 */
#define CAN_F13R1_FB13                          ((uint32_t)0x00002000) /* Filter bit 13 */
#define CAN_F13R1_FB14                          ((uint32_t)0x00004000) /* Filter bit 14 */
#define CAN_F13R1_FB15                          ((uint32_t)0x00008000) /* Filter bit 15 */
#define CAN_F13R1_FB16                          ((uint32_t)0x00010000) /* Filter bit 16 */
#define CAN_F13R1_FB17                          ((uint32_t)0x00020000) /* Filter bit 17 */
#define CAN_F13R1_FB18                          ((uint32_t)0x00040000) /* Filter bit 18 */
#define CAN_F13R1_FB19                          ((uint32_t)0x00080000) /* Filter bit 19 */
#define CAN_F13R1_FB20                          ((uint32_t)0x00100000) /* Filter bit 20 */
#define CAN_F13R1_FB21                          ((uint32_t)0x00200000) /* Filter bit 21 */
#define CAN_F13R1_FB22                          ((uint32_t)0x00400000) /* Filter bit 22 */
#define CAN_F13R1_FB23                          ((uint32_t)0x00800000) /* Filter bit 23 */
#define CAN_F13R1_FB24                          ((uint32_t)0x01000000) /* Filter bit 24 */
#define CAN_F13R1_FB25                          ((uint32_t)0x02000000) /* Filter bit 25 */
#define CAN_F13R1_FB26                          ((uint32_t)0x04000000) /* Filter bit 26 */
#define CAN_F13R1_FB27                          ((uint32_t)0x08000000) /* Filter bit 27 */
#define CAN_F13R1_FB28                          ((uint32_t)0x10000000) /* Filter bit 28 */
#define CAN_F13R1_FB29                          ((uint32_t)0x20000000) /* Filter bit 29 */
#define CAN_F13R1_FB30                          ((uint32_t)0x40000000) /* Filter bit 30 */
#define CAN_F13R1_FB31                          ((uint32_t)0x80000000) /* Filter bit 31 */

/*******************  Bit definition for CAN_F0R2 register  *******************/
#define CAN_F0R2_FB0                            ((uint32_t)0x00000001) /* Filter bit 0 */
#define CAN_F0R2_FB1                            ((uint32_t)0x00000002) /* Filter bit 1 */
#define CAN_F0R2_FB2                            ((uint32_t)0x00000004) /* Filter bit 2 */
#define CAN_F0R2_FB3                            ((uint32_t)0x00000008) /* Filter bit 3 */
#define CAN_F0R2_FB4                            ((uint32_t)0x00000010) /* Filter bit 4 */
#define CAN_F0R2_FB5                            ((uint32_t)0x00000020) /* Filter bit 5 */
#define CAN_F0R2_FB6                            ((uint32_t)0x00000040) /* Filter bit 6 */
#define CAN_F0R2_FB7                            ((uint32_t)0x00000080) /* Filter bit 7 */
#define CAN_F0R2_FB8                            ((uint32_t)0x00000100) /* Filter bit 8 */
#define CAN_F0R2_FB9                            ((uint32_t)0x00000200) /* Filter bit 9 */
#define CAN_F0R2_FB10                           ((uint32_t)0x00000400) /* Filter bit 10 */
#define CAN_F0R2_FB11                           ((uint32_t)0x00000800) /* Filter bit 11 */
#define CAN_F0R2_FB12                           ((uint32_t)0x00001000) /* Filter bit 12 */
#define CAN_F0R2_FB13                           ((uint32_t)0x00002000) /* Filter bit 13 */
#define CAN_F0R2_FB14                           ((uint32_t)0x00004000) /* Filter bit 14 */
#define CAN_F0R2_FB15                           ((uint32_t)0x00008000) /* Filter bit 15 */
#define CAN_F0R2_FB16                           ((uint32_t)0x00010000) /* Filter bit 16 */
#define CAN_F0R2_FB17                           ((uint32_t)0x00020000) /* Filter bit 17 */
#define CAN_F0R2_FB18                           ((uint32_t)0x00040000) /* Filter bit 18 */
#define CAN_F0R2_FB19                           ((uint32_t)0x00080000) /* Filter bit 19 */
#define CAN_F0R2_FB20                           ((uint32_t)0x00100000) /* Filter bit 20 */
#define CAN_F0R2_FB21                           ((uint32_t)0x00200000) /* Filter bit 21 */
#define CAN_F0R2_FB22                           ((uint32_t)0x00400000) /* Filter bit 22 */
#define CAN_F0R2_FB23                           ((uint32_t)0x00800000) /* Filter bit 23 */
#define CAN_F0R2_FB24                           ((uint32_t)0x01000000) /* Filter bit 24 */
#define CAN_F0R2_FB25                           ((uint32_t)0x02000000) /* Filter bit 25 */
#define CAN_F0R2_FB26                           ((uint32_t)0x04000000) /* Filter bit 26 */
#define CAN_F0R2_FB27                           ((uint32_t)0x08000000) /* Filter bit 27 */
#define CAN_F0R2_FB28                           ((uint32_t)0x10000000) /* Filter bit 28 */
#define CAN_F0R2_FB29                           ((uint32_t)0x20000000) /* Filter bit 29 */
#define CAN_F0R2_FB30                           ((uint32_t)0x40000000) /* Filter bit 30 */
#define CAN_F0R2_FB31                           ((uint32_t)0x80000000) /* Filter bit 31 */

/*******************  Bit definition for CAN_F1R2 register  *******************/
#define CAN_F1R2_FB0                            ((uint32_t)0x00000001) /* Filter bit 0 */
#define CAN_F1R2_FB1                            ((uint32_t)0x00000002) /* Filter bit 1 */
#define CAN_F1R2_FB2                            ((uint32_t)0x00000004) /* Filter bit 2 */
#define CAN_F1R2_FB3                            ((uint32_t)0x00000008) /* Filter bit 3 */
#define CAN_F1R2_FB4                            ((uint32_t)0x00000010) /* Filter bit 4 */
#define CAN_F1R2_FB5                            ((uint32_t)0x00000020) /* Filter bit 5 */
#define CAN_F1R2_FB6                            ((uint32_t)0x00000040) /* Filter bit 6 */
#define CAN_F1R2_FB7                            ((uint32_t)0x00000080) /* Filter bit 7 */
#define CAN_F1R2_FB8                            ((uint32_t)0x00000100) /* Filter bit 8 */
#define CAN_F1R2_FB9                            ((uint32_t)0x00000200) /* Filter bit 9 */
#define CAN_F1R2_FB10                           ((uint32_t)0x00000400) /* Filter bit 10 */
#define CAN_F1R2_FB11                           ((uint32_t)0x00000800) /* Filter bit 11 */
#define CAN_F1R2_FB12                           ((uint32_t)0x00001000) /* Filter bit 12 */
#define CAN_F1R2_FB13                           ((uint32_t)0x00002000) /* Filter bit 13 */
#define CAN_F1R2_FB14                           ((uint32_t)0x00004000) /* Filter bit 14 */
#define CAN_F1R2_FB15                           ((uint32_t)0x00008000) /* Filter bit 15 */
#define CAN_F1R2_FB16                           ((uint32_t)0x00010000) /* Filter bit 16 */
#define CAN_F1R2_FB17                           ((uint32_t)0x00020000) /* Filter bit 17 */
#define CAN_F1R2_FB18                           ((uint32_t)0x00040000) /* Filter bit 18 */
#define CAN_F1R2_FB19                           ((uint32_t)0x00080000) /* Filter bit 19 */
#define CAN_F1R2_FB20                           ((uint32_t)0x00100000) /* Filter bit 20 */
#define CAN_F1R2_FB21                           ((uint32_t)0x00200000) /* Filter bit 21 */
#define CAN_F1R2_FB22                           ((uint32_t)0x00400000) /* Filter bit 22 */
#define CAN_F1R2_FB23                           ((uint32_t)0x00800000) /* Filter bit 23 */
#define CAN_F1R2_FB24                           ((uint32_t)0x01000000) /* Filter bit 24 */
#define CAN_F1R2_FB25                           ((uint32_t)0x02000000) /* Filter bit 25 */
#define CAN_F1R2_FB26                           ((uint32_t)0x04000000) /* Filter bit 26 */
#define CAN_F1R2_FB27                           ((uint32_t)0x08000000) /* Filter bit 27 */
#define CAN_F1R2_FB28                           ((uint32_t)0x10000000) /* Filter bit 28 */
#define CAN_F1R2_FB29                           ((uint32_t)0x20000000) /* Filter bit 29 */
#define CAN_F1R2_FB30                           ((uint32_t)0x40000000) /* Filter bit 30 */
#define CAN_F1R2_FB31                           ((uint32_t)0x80000000) /* Filter bit 31 */

/*******************  Bit definition for CAN_F2R2 register  *******************/
#define CAN_F2R2_FB0                            ((uint32_t)0x00000001) /* Filter bit 0 */
#define CAN_F2R2_FB1                            ((uint32_t)0x00000002) /* Filter bit 1 */
#define CAN_F2R2_FB2                            ((uint32_t)0x00000004) /* Filter bit 2 */
#define CAN_F2R2_FB3                            ((uint32_t)0x00000008) /* Filter bit 3 */
#define CAN_F2R2_FB4                            ((uint32_t)0x00000010) /* Filter bit 4 */
#define CAN_F2R2_FB5                            ((uint32_t)0x00000020) /* Filter bit 5 */
#define CAN_F2R2_FB6                            ((uint32_t)0x00000040) /* Filter bit 6 */
#define CAN_F2R2_FB7                            ((uint32_t)0x00000080) /* Filter bit 7 */
#define CAN_F2R2_FB8                            ((uint32_t)0x00000100) /* Filter bit 8 */
#define CAN_F2R2_FB9                            ((uint32_t)0x00000200) /* Filter bit 9 */
#define CAN_F2R2_FB10                           ((uint32_t)0x00000400) /* Filter bit 10 */
#define CAN_F2R2_FB11                           ((uint32_t)0x00000800) /* Filter bit 11 */
#define CAN_F2R2_FB12                           ((uint32_t)0x00001000) /* Filter bit 12 */
#define CAN_F2R2_FB13                           ((uint32_t)0x00002000) /* Filter bit 13 */
#define CAN_F2R2_FB14                           ((uint32_t)0x00004000) /* Filter bit 14 */
#define CAN_F2R2_FB15                           ((uint32_t)0x00008000) /* Filter bit 15 */
#define CAN_F2R2_FB16                           ((uint32_t)0x00010000) /* Filter bit 16 */
#define CAN_F2R2_FB17                           ((uint32_t)0x00020000) /* Filter bit 17 */
#define CAN_F2R2_FB18                           ((uint32_t)0x00040000) /* Filter bit 18 */
#define CAN_F2R2_FB19                           ((uint32_t)0x00080000) /* Filter bit 19 */
#define CAN_F2R2_FB20                           ((uint32_t)0x00100000) /* Filter bit 20 */
#define CAN_F2R2_FB21                           ((uint32_t)0x00200000) /* Filter bit 21 */
#define CAN_F2R2_FB22                           ((uint32_t)0x00400000) /* Filter bit 22 */
#define CAN_F2R2_FB23                           ((uint32_t)0x00800000) /* Filter bit 23 */
#define CAN_F2R2_FB24                           ((uint32_t)0x01000000) /* Filter bit 24 */
#define CAN_F2R2_FB25                           ((uint32_t)0x02000000) /* Filter bit 25 */
#define CAN_F2R2_FB26                           ((uint32_t)0x04000000) /* Filter bit 26 */
#define CAN_F2R2_FB27                           ((uint32_t)0x08000000) /* Filter bit 27 */
#define CAN_F2R2_FB28                           ((uint32_t)0x10000000) /* Filter bit 28 */
#define CAN_F2R2_FB29                           ((uint32_t)0x20000000) /* Filter bit 29 */
#define CAN_F2R2_FB30                           ((uint32_t)0x40000000) /* Filter bit 30 */
#define CAN_F2R2_FB31                           ((uint32_t)0x80000000) /* Filter bit 31 */

/*******************  Bit definition for CAN_F3R2 register  *******************/
#define CAN_F3R2_FB0                            ((uint32_t)0x00000001) /* Filter bit 0 */
#define CAN_F3R2_FB1                            ((uint32_t)0x00000002) /* Filter bit 1 */
#define CAN_F3R2_FB2                            ((uint32_t)0x00000004) /* Filter bit 2 */
#define CAN_F3R2_FB3                            ((uint32_t)0x00000008) /* Filter bit 3 */
#define CAN_F3R2_FB4                            ((uint32_t)0x00000010) /* Filter bit 4 */
#define CAN_F3R2_FB5                            ((uint32_t)0x00000020) /* Filter bit 5 */
#define CAN_F3R2_FB6                            ((uint32_t)0x00000040) /* Filter bit 6 */
#define CAN_F3R2_FB7                            ((uint32_t)0x00000080) /* Filter bit 7 */
#define CAN_F3R2_FB8                            ((uint32_t)0x00000100) /* Filter bit 8 */
#define CAN_F3R2_FB9                            ((uint32_t)0x00000200) /* Filter bit 9 */
#define CAN_F3R2_FB10                           ((uint32_t)0x00000400) /* Filter bit 10 */
#define CAN_F3R2_FB11                           ((uint32_t)0x00000800) /* Filter bit 11 */
#define CAN_F3R2_FB12                           ((uint32_t)0x00001000) /* Filter bit 12 */
#define CAN_F3R2_FB13                           ((uint32_t)0x00002000) /* Filter bit 13 */
#define CAN_F3R2_FB14                           ((uint32_t)0x00004000) /* Filter bit 14 */
#define CAN_F3R2_FB15                           ((uint32_t)0x00008000) /* Filter bit 15 */
#define CAN_F3R2_FB16                           ((uint32_t)0x00010000) /* Filter bit 16 */
#define CAN_F3R2_FB17                           ((uint32_t)0x00020000) /* Filter bit 17 */
#define CAN_F3R2_FB18                           ((uint32_t)0x00040000) /* Filter bit 18 */
#define CAN_F3R2_FB19                           ((uint32_t)0x00080000) /* Filter bit 19 */
#define CAN_F3R2_FB20                           ((uint32_t)0x00100000) /* Filter bit 20 */
#define CAN_F3R2_FB21                           ((uint32_t)0x00200000) /* Filter bit 21 */
#define CAN_F3R2_FB22                           ((uint32_t)0x00400000) /* Filter bit 22 */
#define CAN_F3R2_FB23                           ((uint32_t)0x00800000) /* Filter bit 23 */
#define CAN_F3R2_FB24                           ((uint32_t)0x01000000) /* Filter bit 24 */
#define CAN_F3R2_FB25                           ((uint32_t)0x02000000) /* Filter bit 25 */
#define CAN_F3R2_FB26                           ((uint32_t)0x04000000) /* Filter bit 26 */
#define CAN_F3R2_FB27                           ((uint32_t)0x08000000) /* Filter bit 27 */
#define CAN_F3R2_FB28                           ((uint32_t)0x10000000) /* Filter bit 28 */
#define CAN_F3R2_FB29                           ((uint32_t)0x20000000) /* Filter bit 29 */
#define CAN_F3R2_FB30                           ((uint32_t)0x40000000) /* Filter bit 30 */
#define CAN_F3R2_FB31                           ((uint32_t)0x80000000) /* Filter bit 31 */

/*******************  Bit definition for CAN_F4R2 register  *******************/
#define CAN_F4R2_FB0                            ((uint32_t)0x00000001) /* Filter bit 0 */
#define CAN_F4R2_FB1                            ((uint32_t)0x00000002) /* Filter bit 1 */
#define CAN_F4R2_FB2                            ((uint32_t)0x00000004) /* Filter bit 2 */
#define CAN_F4R2_FB3                            ((uint32_t)0x00000008) /* Filter bit 3 */
#define CAN_F4R2_FB4                            ((uint32_t)0x00000010) /* Filter bit 4 */
#define CAN_F4R2_FB5                            ((uint32_t)0x00000020) /* Filter bit 5 */
#define CAN_F4R2_FB6                            ((uint32_t)0x00000040) /* Filter bit 6 */
#define CAN_F4R2_FB7                            ((uint32_t)0x00000080) /* Filter bit 7 */
#define CAN_F4R2_FB8                            ((uint32_t)0x00000100) /* Filter bit 8 */
#define CAN_F4R2_FB9                            ((uint32_t)0x00000200) /* Filter bit 9 */
#define CAN_F4R2_FB10                           ((uint32_t)0x00000400) /* Filter bit 10 */
#define CAN_F4R2_FB11                           ((uint32_t)0x00000800) /* Filter bit 11 */
#define CAN_F4R2_FB12                           ((uint32_t)0x00001000) /* Filter bit 12 */
#define CAN_F4R2_FB13                           ((uint32_t)0x00002000) /* Filter bit 13 */
#define CAN_F4R2_FB14                           ((uint32_t)0x00004000) /* Filter bit 14 */
#define CAN_F4R2_FB15                           ((uint32_t)0x00008000) /* Filter bit 15 */
#define CAN_F4R2_FB16                           ((uint32_t)0x00010000) /* Filter bit 16 */
#define CAN_F4R2_FB17                           ((uint32_t)0x00020000) /* Filter bit 17 */
#define CAN_F4R2_FB18                           ((uint32_t)0x00040000) /* Filter bit 18 */
#define CAN_F4R2_FB19                           ((uint32_t)0x00080000) /* Filter bit 19 */
#define CAN_F4R2_FB20                           ((uint32_t)0x00100000) /* Filter bit 20 */
#define CAN_F4R2_FB21                           ((uint32_t)0x00200000) /* Filter bit 21 */
#define CAN_F4R2_FB22                           ((uint32_t)0x00400000) /* Filter bit 22 */
#define CAN_F4R2_FB23                           ((uint32_t)0x00800000) /* Filter bit 23 */
#define CAN_F4R2_FB24                           ((uint32_t)0x01000000) /* Filter bit 24 */
#define CAN_F4R2_FB25                           ((uint32_t)0x02000000) /* Filter bit 25 */
#define CAN_F4R2_FB26                           ((uint32_t)0x04000000) /* Filter bit 26 */
#define CAN_F4R2_FB27                           ((uint32_t)0x08000000) /* Filter bit 27 */
#define CAN_F4R2_FB28                           ((uint32_t)0x10000000) /* Filter bit 28 */
#define CAN_F4R2_FB29                           ((uint32_t)0x20000000) /* Filter bit 29 */
#define CAN_F4R2_FB30                           ((uint32_t)0x40000000) /* Filter bit 30 */
#define CAN_F4R2_FB31                           ((uint32_t)0x80000000) /* Filter bit 31 */

/*******************  Bit definition for CAN_F5R2 register  *******************/
#define CAN_F5R2_FB0                            ((uint32_t)0x00000001) /* Filter bit 0 */
#define CAN_F5R2_FB1                            ((uint32_t)0x00000002) /* Filter bit 1 */
#define CAN_F5R2_FB2                            ((uint32_t)0x00000004) /* Filter bit 2 */
#define CAN_F5R2_FB3                            ((uint32_t)0x00000008) /* Filter bit 3 */
#define CAN_F5R2_FB4                            ((uint32_t)0x00000010) /* Filter bit 4 */
#define CAN_F5R2_FB5                            ((uint32_t)0x00000020) /* Filter bit 5 */
#define CAN_F5R2_FB6                            ((uint32_t)0x00000040) /* Filter bit 6 */
#define CAN_F5R2_FB7                            ((uint32_t)0x00000080) /* Filter bit 7 */
#define CAN_F5R2_FB8                            ((uint32_t)0x00000100) /* Filter bit 8 */
#define CAN_F5R2_FB9                            ((uint32_t)0x00000200) /* Filter bit 9 */
#define CAN_F5R2_FB10                           ((uint32_t)0x00000400) /* Filter bit 10 */
#define CAN_F5R2_FB11                           ((uint32_t)0x00000800) /* Filter bit 11 */
#define CAN_F5R2_FB12                           ((uint32_t)0x00001000) /* Filter bit 12 */
#define CAN_F5R2_FB13                           ((uint32_t)0x00002000) /* Filter bit 13 */
#define CAN_F5R2_FB14                           ((uint32_t)0x00004000) /* Filter bit 14 */
#define CAN_F5R2_FB15                           ((uint32_t)0x00008000) /* Filter bit 15 */
#define CAN_F5R2_FB16                           ((uint32_t)0x00010000) /* Filter bit 16 */
#define CAN_F5R2_FB17                           ((uint32_t)0x00020000) /* Filter bit 17 */
#define CAN_F5R2_FB18                           ((uint32_t)0x00040000) /* Filter bit 18 */
#define CAN_F5R2_FB19                           ((uint32_t)0x00080000) /* Filter bit 19 */
#define CAN_F5R2_FB20                           ((uint32_t)0x00100000) /* Filter bit 20 */
#define CAN_F5R2_FB21                           ((uint32_t)0x00200000) /* Filter bit 21 */
#define CAN_F5R2_FB22                           ((uint32_t)0x00400000) /* Filter bit 22 */
#define CAN_F5R2_FB23                           ((uint32_t)0x00800000) /* Filter bit 23 */
#define CAN_F5R2_FB24                           ((uint32_t)0x01000000) /* Filter bit 24 */
#define CAN_F5R2_FB25                           ((uint32_t)0x02000000) /* Filter bit 25 */
#define CAN_F5R2_FB26                           ((uint32_t)0x04000000) /* Filter bit 26 */
#define CAN_F5R2_FB27                           ((uint32_t)0x08000000) /* Filter bit 27 */
#define CAN_F5R2_FB28                           ((uint32_t)0x10000000) /* Filter bit 28 */
#define CAN_F5R2_FB29                           ((uint32_t)0x20000000) /* Filter bit 29 */
#define CAN_F5R2_FB30                           ((uint32_t)0x40000000) /* Filter bit 30 */
#define CAN_F5R2_FB31                           ((uint32_t)0x80000000) /* Filter bit 31 */

/*******************  Bit definition for CAN_F6R2 register  *******************/
#define CAN_F6R2_FB0                            ((uint32_t)0x00000001) /* Filter bit 0 */
#define CAN_F6R2_FB1                            ((uint32_t)0x00000002) /* Filter bit 1 */
#define CAN_F6R2_FB2                            ((uint32_t)0x00000004) /* Filter bit 2 */
#define CAN_F6R2_FB3                            ((uint32_t)0x00000008) /* Filter bit 3 */
#define CAN_F6R2_FB4                            ((uint32_t)0x00000010) /* Filter bit 4 */
#define CAN_F6R2_FB5                            ((uint32_t)0x00000020) /* Filter bit 5 */
#define CAN_F6R2_FB6                            ((uint32_t)0x00000040) /* Filter bit 6 */
#define CAN_F6R2_FB7                            ((uint32_t)0x00000080) /* Filter bit 7 */
#define CAN_F6R2_FB8                            ((uint32_t)0x00000100) /* Filter bit 8 */
#define CAN_F6R2_FB9                            ((uint32_t)0x00000200) /* Filter bit 9 */
#define CAN_F6R2_FB10                           ((uint32_t)0x00000400) /* Filter bit 10 */
#define CAN_F6R2_FB11                           ((uint32_t)0x00000800) /* Filter bit 11 */
#define CAN_F6R2_FB12                           ((uint32_t)0x00001000) /* Filter bit 12 */
#define CAN_F6R2_FB13                           ((uint32_t)0x00002000) /* Filter bit 13 */
#define CAN_F6R2_FB14                           ((uint32_t)0x00004000) /* Filter bit 14 */
#define CAN_F6R2_FB15                           ((uint32_t)0x00008000) /* Filter bit 15 */
#define CAN_F6R2_FB16                           ((uint32_t)0x00010000) /* Filter bit 16 */
#define CAN_F6R2_FB17                           ((uint32_t)0x00020000) /* Filter bit 17 */
#define CAN_F6R2_FB18                           ((uint32_t)0x00040000) /* Filter bit 18 */
#define CAN_F6R2_FB19                           ((uint32_t)0x00080000) /* Filter bit 19 */
#define CAN_F6R2_FB20                           ((uint32_t)0x00100000) /* Filter bit 20 */
#define CAN_F6R2_FB21                           ((uint32_t)0x00200000) /* Filter bit 21 */
#define CAN_F6R2_FB22                           ((uint32_t)0x00400000) /* Filter bit 22 */
#define CAN_F6R2_FB23                           ((uint32_t)0x00800000) /* Filter bit 23 */
#define CAN_F6R2_FB24                           ((uint32_t)0x01000000) /* Filter bit 24 */
#define CAN_F6R2_FB25                           ((uint32_t)0x02000000) /* Filter bit 25 */
#define CAN_F6R2_FB26                           ((uint32_t)0x04000000) /* Filter bit 26 */
#define CAN_F6R2_FB27                           ((uint32_t)0x08000000) /* Filter bit 27 */
#define CAN_F6R2_FB28                           ((uint32_t)0x10000000) /* Filter bit 28 */
#define CAN_F6R2_FB29                           ((uint32_t)0x20000000) /* Filter bit 29 */
#define CAN_F6R2_FB30                           ((uint32_t)0x40000000) /* Filter bit 30 */
#define CAN_F6R2_FB31                           ((uint32_t)0x80000000) /* Filter bit 31 */

/*******************  Bit definition for CAN_F7R2 register  *******************/
#define CAN_F7R2_FB0                            ((uint32_t)0x00000001) /* Filter bit 0 */
#define CAN_F7R2_FB1                            ((uint32_t)0x00000002) /* Filter bit 1 */
#define CAN_F7R2_FB2                            ((uint32_t)0x00000004) /* Filter bit 2 */
#define CAN_F7R2_FB3                            ((uint32_t)0x00000008) /* Filter bit 3 */
#define CAN_F7R2_FB4                            ((uint32_t)0x00000010) /* Filter bit 4 */
#define CAN_F7R2_FB5                            ((uint32_t)0x00000020) /* Filter bit 5 */
#define CAN_F7R2_FB6                            ((uint32_t)0x00000040) /* Filter bit 6 */
#define CAN_F7R2_FB7                            ((uint32_t)0x00000080) /* Filter bit 7 */
#define CAN_F7R2_FB8                            ((uint32_t)0x00000100) /* Filter bit 8 */
#define CAN_F7R2_FB9                            ((uint32_t)0x00000200) /* Filter bit 9 */
#define CAN_F7R2_FB10                           ((uint32_t)0x00000400) /* Filter bit 10 */
#define CAN_F7R2_FB11                           ((uint32_t)0x00000800) /* Filter bit 11 */
#define CAN_F7R2_FB12                           ((uint32_t)0x00001000) /* Filter bit 12 */
#define CAN_F7R2_FB13                           ((uint32_t)0x00002000) /* Filter bit 13 */
#define CAN_F7R2_FB14                           ((uint32_t)0x00004000) /* Filter bit 14 */
#define CAN_F7R2_FB15                           ((uint32_t)0x00008000) /* Filter bit 15 */
#define CAN_F7R2_FB16                           ((uint32_t)0x00010000) /* Filter bit 16 */
#define CAN_F7R2_FB17                           ((uint32_t)0x00020000) /* Filter bit 17 */
#define CAN_F7R2_FB18                           ((uint32_t)0x00040000) /* Filter bit 18 */
#define CAN_F7R2_FB19                           ((uint32_t)0x00080000) /* Filter bit 19 */
#define CAN_F7R2_FB20                           ((uint32_t)0x00100000) /* Filter bit 20 */
#define CAN_F7R2_FB21                           ((uint32_t)0x00200000) /* Filter bit 21 */
#define CAN_F7R2_FB22                           ((uint32_t)0x00400000) /* Filter bit 22 */
#define CAN_F7R2_FB23                           ((uint32_t)0x00800000) /* Filter bit 23 */
#define CAN_F7R2_FB24                           ((uint32_t)0x01000000) /* Filter bit 24 */
#define CAN_F7R2_FB25                           ((uint32_t)0x02000000) /* Filter bit 25 */
#define CAN_F7R2_FB26                           ((uint32_t)0x04000000) /* Filter bit 26 */
#define CAN_F7R2_FB27                           ((uint32_t)0x08000000) /* Filter bit 27 */
#define CAN_F7R2_FB28                           ((uint32_t)0x10000000) /* Filter bit 28 */
#define CAN_F7R2_FB29                           ((uint32_t)0x20000000) /* Filter bit 29 */
#define CAN_F7R2_FB30                           ((uint32_t)0x40000000) /* Filter bit 30 */
#define CAN_F7R2_FB31                           ((uint32_t)0x80000000) /* Filter bit 31 */

/*******************  Bit definition for CAN_F8R2 register  *******************/
#define CAN_F8R2_FB0                            ((uint32_t)0x00000001) /* Filter bit 0 */
#define CAN_F8R2_FB1                            ((uint32_t)0x00000002) /* Filter bit 1 */
#define CAN_F8R2_FB2                            ((uint32_t)0x00000004) /* Filter bit 2 */
#define CAN_F8R2_FB3                            ((uint32_t)0x00000008) /* Filter bit 3 */
#define CAN_F8R2_FB4                            ((uint32_t)0x00000010) /* Filter bit 4 */
#define CAN_F8R2_FB5                            ((uint32_t)0x00000020) /* Filter bit 5 */
#define CAN_F8R2_FB6                            ((uint32_t)0x00000040) /* Filter bit 6 */
#define CAN_F8R2_FB7                            ((uint32_t)0x00000080) /* Filter bit 7 */
#define CAN_F8R2_FB8                            ((uint32_t)0x00000100) /* Filter bit 8 */
#define CAN_F8R2_FB9                            ((uint32_t)0x00000200) /* Filter bit 9 */
#define CAN_F8R2_FB10                           ((uint32_t)0x00000400) /* Filter bit 10 */
#define CAN_F8R2_FB11                           ((uint32_t)0x00000800) /* Filter bit 11 */
#define CAN_F8R2_FB12                           ((uint32_t)0x00001000) /* Filter bit 12 */
#define CAN_F8R2_FB13                           ((uint32_t)0x00002000) /* Filter bit 13 */
#define CAN_F8R2_FB14                           ((uint32_t)0x00004000) /* Filter bit 14 */
#define CAN_F8R2_FB15                           ((uint32_t)0x00008000) /* Filter bit 15 */
#define CAN_F8R2_FB16                           ((uint32_t)0x00010000) /* Filter bit 16 */
#define CAN_F8R2_FB17                           ((uint32_t)0x00020000) /* Filter bit 17 */
#define CAN_F8R2_FB18                           ((uint32_t)0x00040000) /* Filter bit 18 */
#define CAN_F8R2_FB19                           ((uint32_t)0x00080000) /* Filter bit 19 */
#define CAN_F8R2_FB20                           ((uint32_t)0x00100000) /* Filter bit 20 */
#define CAN_F8R2_FB21                           ((uint32_t)0x00200000) /* Filter bit 21 */
#define CAN_F8R2_FB22                           ((uint32_t)0x00400000) /* Filter bit 22 */
#define CAN_F8R2_FB23                           ((uint32_t)0x00800000) /* Filter bit 23 */
#define CAN_F8R2_FB24                           ((uint32_t)0x01000000) /* Filter bit 24 */
#define CAN_F8R2_FB25                           ((uint32_t)0x02000000) /* Filter bit 25 */
#define CAN_F8R2_FB26                           ((uint32_t)0x04000000) /* Filter bit 26 */
#define CAN_F8R2_FB27                           ((uint32_t)0x08000000) /* Filter bit 27 */
#define CAN_F8R2_FB28                           ((uint32_t)0x10000000) /* Filter bit 28 */
#define CAN_F8R2_FB29                           ((uint32_t)0x20000000) /* Filter bit 29 */
#define CAN_F8R2_FB30                           ((uint32_t)0x40000000) /* Filter bit 30 */
#define CAN_F8R2_FB31                           ((uint32_t)0x80000000) /* Filter bit 31 */

/*******************  Bit definition for CAN_F9R2 register  *******************/
#define CAN_F9R2_FB0                            ((uint32_t)0x00000001) /* Filter bit 0 */
#define CAN_F9R2_FB1                            ((uint32_t)0x00000002) /* Filter bit 1 */
#define CAN_F9R2_FB2                            ((uint32_t)0x00000004) /* Filter bit 2 */
#define CAN_F9R2_FB3                            ((uint32_t)0x00000008) /* Filter bit 3 */
#define CAN_F9R2_FB4                            ((uint32_t)0x00000010) /* Filter bit 4 */
#define CAN_F9R2_FB5                            ((uint32_t)0x00000020) /* Filter bit 5 */
#define CAN_F9R2_FB6                            ((uint32_t)0x00000040) /* Filter bit 6 */
#define CAN_F9R2_FB7                            ((uint32_t)0x00000080) /* Filter bit 7 */
#define CAN_F9R2_FB8                            ((uint32_t)0x00000100) /* Filter bit 8 */
#define CAN_F9R2_FB9                            ((uint32_t)0x00000200) /* Filter bit 9 */
#define CAN_F9R2_FB10                           ((uint32_t)0x00000400) /* Filter bit 10 */
#define CAN_F9R2_FB11                           ((uint32_t)0x00000800) /* Filter bit 11 */
#define CAN_F9R2_FB12                           ((uint32_t)0x00001000) /* Filter bit 12 */
#define CAN_F9R2_FB13                           ((uint32_t)0x00002000) /* Filter bit 13 */
#define CAN_F9R2_FB14                           ((uint32_t)0x00004000) /* Filter bit 14 */
#define CAN_F9R2_FB15                           ((uint32_t)0x00008000) /* Filter bit 15 */
#define CAN_F9R2_FB16                           ((uint32_t)0x00010000) /* Filter bit 16 */
#define CAN_F9R2_FB17                           ((uint32_t)0x00020000) /* Filter bit 17 */
#define CAN_F9R2_FB18                           ((uint32_t)0x00040000) /* Filter bit 18 */
#define CAN_F9R2_FB19                           ((uint32_t)0x00080000) /* Filter bit 19 */
#define CAN_F9R2_FB20                           ((uint32_t)0x00100000) /* Filter bit 20 */
#define CAN_F9R2_FB21                           ((uint32_t)0x00200000) /* Filter bit 21 */
#define CAN_F9R2_FB22                           ((uint32_t)0x00400000) /* Filter bit 22 */
#define CAN_F9R2_FB23                           ((uint32_t)0x00800000) /* Filter bit 23 */
#define CAN_F9R2_FB24                           ((uint32_t)0x01000000) /* Filter bit 24 */
#define CAN_F9R2_FB25                           ((uint32_t)0x02000000) /* Filter bit 25 */
#define CAN_F9R2_FB26                           ((uint32_t)0x04000000) /* Filter bit 26 */
#define CAN_F9R2_FB27                           ((uint32_t)0x08000000) /* Filter bit 27 */
#define CAN_F9R2_FB28                           ((uint32_t)0x10000000) /* Filter bit 28 */
#define CAN_F9R2_FB29                           ((uint32_t)0x20000000) /* Filter bit 29 */
#define CAN_F9R2_FB30                           ((uint32_t)0x40000000) /* Filter bit 30 */
#define CAN_F9R2_FB31                           ((uint32_t)0x80000000) /* Filter bit 31 */

/*******************  Bit definition for CAN_F10R2 register  ******************/
#define CAN_F10R2_FB0                           ((uint32_t)0x00000001) /* Filter bit 0 */
#define CAN_F10R2_FB1                           ((uint32_t)0x00000002) /* Filter bit 1 */
#define CAN_F10R2_FB2                           ((uint32_t)0x00000004) /* Filter bit 2 */
#define CAN_F10R2_FB3                           ((uint32_t)0x00000008) /* Filter bit 3 */
#define CAN_F10R2_FB4                           ((uint32_t)0x00000010) /* Filter bit 4 */
#define CAN_F10R2_FB5                           ((uint32_t)0x00000020) /* Filter bit 5 */
#define CAN_F10R2_FB6                           ((uint32_t)0x00000040) /* Filter bit 6 */
#define CAN_F10R2_FB7                           ((uint32_t)0x00000080) /* Filter bit 7 */
#define CAN_F10R2_FB8                           ((uint32_t)0x00000100) /* Filter bit 8 */
#define CAN_F10R2_FB9                           ((uint32_t)0x00000200) /* Filter bit 9 */
#define CAN_F10R2_FB10                          ((uint32_t)0x00000400) /* Filter bit 10 */
#define CAN_F10R2_FB11                          ((uint32_t)0x00000800) /* Filter bit 11 */
#define CAN_F10R2_FB12                          ((uint32_t)0x00001000) /* Filter bit 12 */
#define CAN_F10R2_FB13                          ((uint32_t)0x00002000) /* Filter bit 13 */
#define CAN_F10R2_FB14                          ((uint32_t)0x00004000) /* Filter bit 14 */
#define CAN_F10R2_FB15                          ((uint32_t)0x00008000) /* Filter bit 15 */
#define CAN_F10R2_FB16                          ((uint32_t)0x00010000) /* Filter bit 16 */
#define CAN_F10R2_FB17                          ((uint32_t)0x00020000) /* Filter bit 17 */
#define CAN_F10R2_FB18                          ((uint32_t)0x00040000) /* Filter bit 18 */
#define CAN_F10R2_FB19                          ((uint32_t)0x00080000) /* Filter bit 19 */
#define CAN_F10R2_FB20                          ((uint32_t)0x00100000) /* Filter bit 20 */
#define CAN_F10R2_FB21                          ((uint32_t)0x00200000) /* Filter bit 21 */
#define CAN_F10R2_FB22                          ((uint32_t)0x00400000) /* Filter bit 22 */
#define CAN_F10R2_FB23                          ((uint32_t)0x00800000) /* Filter bit 23 */
#define CAN_F10R2_FB24                          ((uint32_t)0x01000000) /* Filter bit 24 */
#define CAN_F10R2_FB25                          ((uint32_t)0x02000000) /* Filter bit 25 */
#define CAN_F10R2_FB26                          ((uint32_t)0x04000000) /* Filter bit 26 */
#define CAN_F10R2_FB27                          ((uint32_t)0x08000000) /* Filter bit 27 */
#define CAN_F10R2_FB28                          ((uint32_t)0x10000000) /* Filter bit 28 */
#define CAN_F10R2_FB29                          ((uint32_t)0x20000000) /* Filter bit 29 */
#define CAN_F10R2_FB30                          ((uint32_t)0x40000000) /* Filter bit 30 */
#define CAN_F10R2_FB31                          ((uint32_t)0x80000000) /* Filter bit 31 */

/*******************  Bit definition for CAN_F11R2 register  ******************/
#define CAN_F11R2_FB0                           ((uint32_t)0x00000001) /* Filter bit 0 */
#define CAN_F11R2_FB1                           ((uint32_t)0x00000002) /* Filter bit 1 */
#define CAN_F11R2_FB2                           ((uint32_t)0x00000004) /* Filter bit 2 */
#define CAN_F11R2_FB3                           ((uint32_t)0x00000008) /* Filter bit 3 */
#define CAN_F11R2_FB4                           ((uint32_t)0x00000010) /* Filter bit 4 */
#define CAN_F11R2_FB5                           ((uint32_t)0x00000020) /* Filter bit 5 */
#define CAN_F11R2_FB6                           ((uint32_t)0x00000040) /* Filter bit 6 */
#define CAN_F11R2_FB7                           ((uint32_t)0x00000080) /* Filter bit 7 */
#define CAN_F11R2_FB8                           ((uint32_t)0x00000100) /* Filter bit 8 */
#define CAN_F11R2_FB9                           ((uint32_t)0x00000200) /* Filter bit 9 */
#define CAN_F11R2_FB10                          ((uint32_t)0x00000400) /* Filter bit 10 */
#define CAN_F11R2_FB11                          ((uint32_t)0x00000800) /* Filter bit 11 */
#define CAN_F11R2_FB12                          ((uint32_t)0x00001000) /* Filter bit 12 */
#define CAN_F11R2_FB13                          ((uint32_t)0x00002000) /* Filter bit 13 */
#define CAN_F11R2_FB14                          ((uint32_t)0x00004000) /* Filter bit 14 */
#define CAN_F11R2_FB15                          ((uint32_t)0x00008000) /* Filter bit 15 */
#define CAN_F11R2_FB16                          ((uint32_t)0x00010000) /* Filter bit 16 */
#define CAN_F11R2_FB17                          ((uint32_t)0x00020000) /* Filter bit 17 */
#define CAN_F11R2_FB18                          ((uint32_t)0x00040000) /* Filter bit 18 */
#define CAN_F11R2_FB19                          ((uint32_t)0x00080000) /* Filter bit 19 */
#define CAN_F11R2_FB20                          ((uint32_t)0x00100000) /* Filter bit 20 */
#define CAN_F11R2_FB21                          ((uint32_t)0x00200000) /* Filter bit 21 */
#define CAN_F11R2_FB22                          ((uint32_t)0x00400000) /* Filter bit 22 */
#define CAN_F11R2_FB23                          ((uint32_t)0x00800000) /* Filter bit 23 */
#define CAN_F11R2_FB24                          ((uint32_t)0x01000000) /* Filter bit 24 */
#define CAN_F11R2_FB25                          ((uint32_t)0x02000000) /* Filter bit 25 */
#define CAN_F11R2_FB26                          ((uint32_t)0x04000000) /* Filter bit 26 */
#define CAN_F11R2_FB27                          ((uint32_t)0x08000000) /* Filter bit 27 */
#define CAN_F11R2_FB28                          ((uint32_t)0x10000000) /* Filter bit 28 */
#define CAN_F11R2_FB29                          ((uint32_t)0x20000000) /* Filter bit 29 */
#define CAN_F11R2_FB30                          ((uint32_t)0x40000000) /* Filter bit 30 */
#define CAN_F11R2_FB31                          ((uint32_t)0x80000000) /* Filter bit 31 */

/*******************  Bit definition for CAN_F12R2 register  ******************/
#define CAN_F12R2_FB0                           ((uint32_t)0x00000001) /* Filter bit 0 */
#define CAN_F12R2_FB1                           ((uint32_t)0x00000002) /* Filter bit 1 */
#define CAN_F12R2_FB2                           ((uint32_t)0x00000004) /* Filter bit 2 */
#define CAN_F12R2_FB3                           ((uint32_t)0x00000008) /* Filter bit 3 */
#define CAN_F12R2_FB4                           ((uint32_t)0x00000010) /* Filter bit 4 */
#define CAN_F12R2_FB5                           ((uint32_t)0x00000020) /* Filter bit 5 */
#define CAN_F12R2_FB6                           ((uint32_t)0x00000040) /* Filter bit 6 */
#define CAN_F12R2_FB7                           ((uint32_t)0x00000080) /* Filter bit 7 */
#define CAN_F12R2_FB8                           ((uint32_t)0x00000100) /* Filter bit 8 */
#define CAN_F12R2_FB9                           ((uint32_t)0x00000200) /* Filter bit 9 */
#define CAN_F12R2_FB10                          ((uint32_t)0x00000400) /* Filter bit 10 */
#define CAN_F12R2_FB11                          ((uint32_t)0x00000800) /* Filter bit 11 */
#define CAN_F12R2_FB12                          ((uint32_t)0x00001000) /* Filter bit 12 */
#define CAN_F12R2_FB13                          ((uint32_t)0x00002000) /* Filter bit 13 */
#define CAN_F12R2_FB14                          ((uint32_t)0x00004000) /* Filter bit 14 */
#define CAN_F12R2_FB15                          ((uint32_t)0x00008000) /* Filter bit 15 */
#define CAN_F12R2_FB16                          ((uint32_t)0x00010000) /* Filter bit 16 */
#define CAN_F12R2_FB17                          ((uint32_t)0x00020000) /* Filter bit 17 */
#define CAN_F12R2_FB18                          ((uint32_t)0x00040000) /* Filter bit 18 */
#define CAN_F12R2_FB19                          ((uint32_t)0x00080000) /* Filter bit 19 */
#define CAN_F12R2_FB20                          ((uint32_t)0x00100000) /* Filter bit 20 */
#define CAN_F12R2_FB21                          ((uint32_t)0x00200000) /* Filter bit 21 */
#define CAN_F12R2_FB22                          ((uint32_t)0x00400000) /* Filter bit 22 */
#define CAN_F12R2_FB23                          ((uint32_t)0x00800000) /* Filter bit 23 */
#define CAN_F12R2_FB24                          ((uint32_t)0x01000000) /* Filter bit 24 */
#define CAN_F12R2_FB25                          ((uint32_t)0x02000000) /* Filter bit 25 */
#define CAN_F12R2_FB26                          ((uint32_t)0x04000000) /* Filter bit 26 */
#define CAN_F12R2_FB27                          ((uint32_t)0x08000000) /* Filter bit 27 */
#define CAN_F12R2_FB28                          ((uint32_t)0x10000000) /* Filter bit 28 */
#define CAN_F12R2_FB29                          ((uint32_t)0x20000000) /* Filter bit 29 */
#define CAN_F12R2_FB30                          ((uint32_t)0x40000000) /* Filter bit 30 */
#define CAN_F12R2_FB31                          ((uint32_t)0x80000000) /* Filter bit 31 */

/*******************  Bit definition for CAN_F13R2 register  ******************/
#define CAN_F13R2_FB0                           ((uint32_t)0x00000001) /* Filter bit 0 */
#define CAN_F13R2_FB1                           ((uint32_t)0x00000002) /* Filter bit 1 */
#define CAN_F13R2_FB2                           ((uint32_t)0x00000004) /* Filter bit 2 */
#define CAN_F13R2_FB3                           ((uint32_t)0x00000008) /* Filter bit 3 */
#define CAN_F13R2_FB4                           ((uint32_t)0x00000010) /* Filter bit 4 */
#define CAN_F13R2_FB5                           ((uint32_t)0x00000020) /* Filter bit 5 */
#define CAN_F13R2_FB6                           ((uint32_t)0x00000040) /* Filter bit 6 */
#define CAN_F13R2_FB7                           ((uint32_t)0x00000080) /* Filter bit 7 */
#define CAN_F13R2_FB8                           ((uint32_t)0x00000100) /* Filter bit 8 */
#define CAN_F13R2_FB9                           ((uint32_t)0x00000200) /* Filter bit 9 */
#define CAN_F13R2_FB10                          ((uint32_t)0x00000400) /* Filter bit 10 */
#define CAN_F13R2_FB11                          ((uint32_t)0x00000800) /* Filter bit 11 */
#define CAN_F13R2_FB12                          ((uint32_t)0x00001000) /* Filter bit 12 */
#define CAN_F13R2_FB13                          ((uint32_t)0x00002000) /* Filter bit 13 */
#define CAN_F13R2_FB14                          ((uint32_t)0x00004000) /* Filter bit 14 */
#define CAN_F13R2_FB15                          ((uint32_t)0x00008000) /* Filter bit 15 */
#define CAN_F13R2_FB16                          ((uint32_t)0x00010000) /* Filter bit 16 */
#define CAN_F13R2_FB17                          ((uint32_t)0x00020000) /* Filter bit 17 */
#define CAN_F13R2_FB18                          ((uint32_t)0x00040000) /* Filter bit 18 */
#define CAN_F13R2_FB19                          ((uint32_t)0x00080000) /* Filter bit 19 */
#define CAN_F13R2_FB20                          ((uint32_t)0x00100000) /* Filter bit 20 */
#define CAN_F13R2_FB21                          ((uint32_t)0x00200000) /* Filter bit 21 */
#define CAN_F13R2_FB22                          ((uint32_t)0x00400000) /* Filter bit 22 */
#define CAN_F13R2_FB23                          ((uint32_t)0x00800000) /* Filter bit 23 */
#define CAN_F13R2_FB24                          ((uint32_t)0x01000000) /* Filter bit 24 */
#define CAN_F13R2_FB25                          ((uint32_t)0x02000000) /* Filter bit 25 */
#define CAN_F13R2_FB26                          ((uint32_t)0x04000000) /* Filter bit 26 */
#define CAN_F13R2_FB27                          ((uint32_t)0x08000000) /* Filter bit 27 */
#define CAN_F13R2_FB28                          ((uint32_t)0x10000000) /* Filter bit 28 */
#define CAN_F13R2_FB29                          ((uint32_t)0x20000000) /* Filter bit 29 */
#define CAN_F13R2_FB30                          ((uint32_t)0x40000000) /* Filter bit 30 */
#define CAN_F13R2_FB31                          ((uint32_t)0x80000000) /* Filter bit 31 */

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
#define DMA_GIF8                                ((uint32_t)0x10000000) /* Channel 8 Global interrupt flag */
#define DMA_TCIF8                               ((uint32_t)0x20000000) /* Channel 8 Transfer Complete flag */
#define DMA_HTIF8                               ((uint32_t)0x40000000) /* Channel 8 Half Transfer flag */
#define DMA_TEIF8                               ((uint32_t)0x80000000) /* Channel 8 Transfer Error flag */

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
#define DMA_CGIF8                               ((uint32_t)0x10000000) /* Channel 8 Global interrupt clear */
#define DMA_CTCIF8                              ((uint32_t)0x20000000) /* Channel 8 Transfer Complete clear */
#define DMA_CHTIF8                              ((uint32_t)0x40000000) /* Channel 8 Half Transfer clear */
#define DMA_CTEIF8                              ((uint32_t)0x80000000) /* Channel 8 Transfer Error clear */

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

/******************  Bit definition for DMA_CNTR8 register  ******************/
#define DMA_CNTR8_NDT                           ((uint16_t)0xFFFF) /* Number of data to Transfer */

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

/******************  Bit definition for DMA_PADDR8 register  *******************/
#define DMA_PADDR8_PA                           ((uint32_t)0xFFFFFFFF) /* Peripheral Address */

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

/******************  Bit definition for DMA_MADDR8 register  *******************/
#define DMA_MADDR8_MA                           ((uint32_t)0xFFFFFFFF) /* Memory Address */

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
#define EXTI_INTENR_MR20                        ((uint32_t)0x00100000) /* Interrupt Mask on line 20 */
#define EXTI_INTENR_MR21                        ((uint32_t)0x00200000) /* Interrupt Mask on line 21 */
#define EXTI_INTENR_MR22                        ((uint32_t)0x00400000) /* Interrupt Mask on line 22 */

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
#define EXTI_EVENR_MR20                         ((uint32_t)0x00100000) /* Event Mask on line 20 */
#define EXTI_EVENR_MR21                         ((uint32_t)0x00200000) /* Event Mask on line 21 */
#define EXTI_EVENR_MR22                         ((uint32_t)0x00400000) /* Event Mask on line 22 */

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
#define EXTI_RTENR_TR20                         ((uint32_t)0x00100000) /* Rising trigger event configuration bit of line 20 */
#define EXTI_RTENR_TR21                         ((uint32_t)0x00200000) /* Rising trigger event configuration bit of line 21 */
#define EXTI_RTENR_TR22                         ((uint32_t)0x00400000) /* Rising trigger event configuration bit of line 22 */

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
#define EXTI_FTENR_TR20                         ((uint32_t)0x00100000) /* Falling trigger event configuration bit of line 20 */
#define EXTI_FTENR_TR21                         ((uint32_t)0x00200000) /* Falling trigger event configuration bit of line 21 */
#define EXTI_FTENR_TR22                         ((uint32_t)0x00400000) /* Falling trigger event configuration bit of line 22 */

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
#define EXTI_SWIEVR_SWIEVR20                    ((uint32_t)0x00100000) /* Software Interrupt on line 20 */
#define EXTI_SWIEVR_SWIEVR21                    ((uint32_t)0x00200000) /* Software Interrupt on line 21 */
#define EXTI_SWIEVR_SWIEVR22                    ((uint32_t)0x00400000) /* Software Interrupt on line 22 */

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
#define EXTI_INTF_INTF20                        ((uint32_t)0x00100000) /* Pending bit for line 20 */
#define EXTI_INTF_INTF21                        ((uint32_t)0x00200000) /* Pending bit for line 21 */
#define EXTI_INTF_INTF22                        ((uint32_t)0x00400000) /* Pending bit for line 22 */

/******************************************************************************/
/*                      FLASH and Option Bytes Registers                      */
/******************************************************************************/



/*******************  Bit definition for FLASH_ACTLR register  ******************/
#define FLASH_ACTLR_LATENCY                     ((uint32_t)0x03) /* LATENCY[2:0] bits (Latency) */
#define FLASH_ACTLR_LATENCY_0                   ((uint32_t)0x00) /* Bit 0 */
#define FLASH_ACTLR_LATENCY_1                   ((uint32_t)0x01) /* Bit 0 */
#define FLASH_ACTLR_LATENCY_2                   ((uint32_t)0x02) /* Bit 1 */

/******************  Bit definition for FLASH_KEYR register  ******************/
#define FLASH_KEYR_FKEYR                        ((uint32_t)0xFFFFFFFF) /* FPEC Key */

/*****************  Bit definition for FLASH_OBKEYR register  ****************/
#define FLASH_OBKEYR_OBKEYR                     ((uint32_t)0xFFFFFFFF) /* Option Byte Key */

/******************  Bit definition for FLASH_STATR register  *******************/
#define FLASH_STATR_BSY                         ((uint8_t)0x01) /* Busy */
#define FLASH_STATR_PGERR                       ((uint8_t)0x04) /* Programming Error */
#define FLASH_STATR_WRPRTERR                    ((uint8_t)0x10) /* Write Protection Error */
#define FLASH_STATR_EOP                         ((uint8_t)0x20) /* End of operation */
#define FLASH_STATR_FWAKE_FLAG                  ((uint8_t)0x40)
#define FLASH_STATR_TURBO                       ((uint8_t)0x80)

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
#define FLASH_CTLR_FWAKEIE                      ((uint32_t)0x00002000)
#define FLASH_CTLR_FLOCK                        ((uint32_t)0x00008000) /* Fast Lock */
#define FLASH_CTLR_FTPG                         ((uint32_t)0x00010000) /* Page Programming 256Byte */
#define FLASH_CTLR_FTER                         ((uint32_t)0x00020000) /* Page Erase 256Byte */
#define FLASH_CTLR_BUFLOAD                      ((uint32_t)0x00040000) 
#define FLASH_CTLR_BUFRST                       ((uint32_t)0x00080000) 
#define FLASH_CTLR_BER32                        ((uint32_t)0x00800000) /* Block Erase 32K */

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
#define FLASH_OBR_CFGCANM                       ((uint32_t)0x00000080)

#define FLASH_OBR_DATA0                         ((uint32_t)0x0003FC00) /* DATA0 */
#define FLASH_OBR_DATA1                         ((uint32_t)0x03FC0000) /* DATA1 */

/******************  Bit definition for FLASH_WPR register  ******************/
#define FLASH_WPR_WRP                           ((uint32_t)0xFFFFFFFF) /* Write Protect */

/******************  Bit definition for FLASH_OBR_MODEKEYR register  ******************/
#define FLASH_OBR_MODEKEYR                      ((uint32_t)0xFFFFFFFF)

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
#define AFIO_PCFR1_SPI1_RM                      ((uint32_t)0x00000001) /* SPI1 remapping */
#define AFIO_PCFR1_I2C1_RM                      ((uint32_t)0x00000002) /* I2C1 remapping */
#define AFIO_PCFR1_USART1_RM                    ((uint32_t)0x00000004) /* USART1 remapping */
#define AFIO_PCFR1_USART2_RM                    ((uint32_t)0x00000008) /* USART2 remapping */

#define AFIO_PCFR1_USART3_RM                    ((uint32_t)0x00000030) /* USART3_RM[1:0] bits (USART3 remapping) */
#define AFIO_PCFR1_USART3_RM_0                  ((uint32_t)0x00000010) /* Bit 0 */
#define AFIO_PCFR1_USART3_RM_1                  ((uint32_t)0x00000020) /* Bit 1 */

#define AFIO_PCFR1_TIM1_RM                      ((uint32_t)0x000000C0) /* TIM1_RM[1:0] bits (TIM1 remapping) */
#define AFIO_PCFR1_TIM1_RM_0                    ((uint32_t)0x00000040) /* Bit 0 */
#define AFIO_PCFR1_TIM1_RM_1                    ((uint32_t)0x00000080) /* Bit 1 */

#define AFIO_PCFR1_TIM2_RM                      ((uint32_t)0x00000300) /* TIM2_RM[1:0] bits (TIM2 remapping) */
#define AFIO_PCFR1_TIM2_RM_0                    ((uint32_t)0x00000100) /* Bit 0 */
#define AFIO_PCFR1_TIM2_RM_1                    ((uint32_t)0x00000200) /* Bit 1 */

#define AFIO_PCFR1_TIM3_RM                      ((uint32_t)0x00000400) /* TIM3_RM bits (TIM3 remapping) */

#define AFIO_PCFR1_TIM4_RM                      ((uint32_t)0x00001000) /* TIM4_RM bit (TIM4 remapping) */

#define AFIO_PCFR1_CAN_RM                       ((uint32_t)0x00006000) /* CAN_RM[1:0] bits (CAN Alternate function remapping) */
#define AFIO_PCFR1_CAN_RM_0                     ((uint32_t)0x00002000) /* Bit 0 */
#define AFIO_PCFR1_CAN_RM_1                     ((uint32_t)0x00004000) /* Bit 1 */

#define AFIO_PCFR1_PD0PD1_RM                    ((uint32_t)0x00008000) /* Port D0/Port D1 mapping on OSC_IN/OSC_OUT */
#define AFIO_PCFR1_PD01_RM                      AFIO_PCFR1_PD0PD1_RM

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

/*****************  Bit definition for AFIO_CR register  *****************/
#define AFIO_CR_USBPD_IN_HVT                    ((uint32_t)0x00000200)
#define AFIO_CR_UPD_BC_VSRC                     ((uint32_t)0x00010000)
#define AFIO_CR_UDM_BC_VSRC                     ((uint32_t)0x00020000)
#define AFIO_CR_UPD_BC_CMPE                     ((uint32_t)0x00040000)
#define AFIO_CR_UDM_BC_CMPE                     ((uint32_t)0x00080000)
#define AFIO_CR_UPD_BC_CMPO                     ((uint32_t)0x00100000)
#define AFIO_CR_UDM_BC_CMPO                     ((uint32_t)0x00200000)

/*****************  Bit definition for AFIO_PCFR2 register  *****************/
#define AFIO_PCFR2_USART4_RM                    ((uint32_t)0x00010000)
#define AFIO_PCFR2_USART2_RM_H                  ((uint32_t)0x00040000)

#define AFIO_PCFR2_USART1_RM_H                  ((uint32_t)0x00180000)
#define AFIO_PCFR2_USART1_RM_H_0                ((uint32_t)0x00080000)
#define AFIO_PCFR2_USART1_RM_H_1                ((uint32_t)0x00100000)

#define AFIO_PCFR2_TIM2_RM_H                    ((uint32_t)0x00200000)
#define AFIO_PCFR2_TIM1_RM_H                    ((uint32_t)0x00400000)
#define AFIO_PCFR2_I2C_RM_H                     ((uint32_t)0x00800000)
#define AFIO_PCFR2_SPI1_RM_H                    ((uint32_t)0x01000000)
#define AFIO_PCFR2_LPTIM_RM                     ((uint32_t)0x02000000)

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

#define PWR_CTLR_PLS_MODE0                      ((uint16_t)0x0000) /* PVD level 0 */
#define PWR_CTLR_PLS_MODE1                      ((uint16_t)0x0020) /* PVD level 1 */
#define PWR_CTLR_PLS_MODE2                      ((uint16_t)0x0040) /* PVD level 2 */
#define PWR_CTLR_PLS_MODE3                      ((uint16_t)0x0060) /* PVD level 3 */
#define PWR_CTLR_PLS_MODE4                      ((uint16_t)0x0080) /* PVD level 4 */
#define PWR_CTLR_PLS_MODE5                      ((uint16_t)0x00A0) /* PVD level 5 */
#define PWR_CTLR_PLS_MODE6                      ((uint16_t)0x00C0) /* PVD level 6 */
#define PWR_CTLR_PLS_MODE7                      ((uint16_t)0x00E0) /* PVD level 7 */

#define PWR_CTLR_DBP                            ((uint16_t)0x0100) /* Disable Backup Domain write protection */

#define PWR_CTLR_FLASH_LP_REG                   ((uint16_t)0x0200)

#define PWR_CTLR_FLASH_LP                       ((uint16_t)0x0C00) /* FLASH_LP [1:0]*/
#define PWR_CTLR_FLASH_LP_0                     ((uint16_t)0x0400)
#define PWR_CTLR_FLASH_LP_1                     ((uint16_t)0x0800)

#define PWR_CTLR_AUTO_LDO_EC                    ((uint16_t)0x1000)
#define PWR_CTLR_LDO_EC                         ((uint16_t)0x2000)
#define PWR_CTLR_R2KSTY                         ((uint32_t)0x00010000)
#define PWR_CTLR_R18KSTY                        ((uint32_t)0x00020000)
#define PWR_CTLR_R2KVBAT                        ((uint32_t)0x00040000)
#define PWR_CTLR_R18KVBAT                       ((uint32_t)0x00080000)
#define PWR_RAMLV                               ((uint32_t)0x00100000)

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
#define RCC_HSILP                               ((uint32_t)0x00000004)
#define RCC_HSITRIM                             ((uint32_t)0x000000F8) /* Internal High Speed clock trimming */
#define RCC_HSICAL                              ((uint32_t)0x0000FF00) /* Internal High Speed clock Calibration */
#define RCC_HSEON                               ((uint32_t)0x00010000) /* External High Speed clock enable */
#define RCC_HSERDY                              ((uint32_t)0x00020000) /* External High Speed clock ready flag */
#define RCC_HSEBYP                              ((uint32_t)0x00040000) /* External High Speed clock Bypass */
#define RCC_CSSON                               ((uint32_t)0x00080000) /* Clock Security System enable */
#define RCC_HSELP                               ((uint32_t)0x00100000)
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
#define RCC_PLLMULL18                           ((uint32_t)0x003C0000) /* PLL input clock*18 */

#define RCC_CFGR0_USBPRE                        ((uint32_t)0x00C00000) /* USBPRE[1:0] bits*/
#define RCC_USBPRE_0                            ((uint32_t)0x00400000) /* Bit 0 */
#define RCC_USBPRE_1                            ((uint32_t)0x00800000) /* Bit 1 */

#define RCC_CFGR0_MCO                           ((uint32_t)0x07000000) /* MCO[2:0] bits (Microcontroller Clock Output) */
#define RCC_MCO_0                               ((uint32_t)0x01000000) /* Bit 0 */
#define RCC_MCO_1                               ((uint32_t)0x02000000) /* Bit 1 */
#define RCC_MCO_2                               ((uint32_t)0x04000000) /* Bit 2 */

#define RCC_MCO_NOCLOCK                         ((uint32_t)0x00000000) /* No clock */
#define RCC_CFGR0_MCO_SYSCLK                    ((uint32_t)0x04000000) /* System clock selected as MCO source */
#define RCC_CFGR0_MCO_HSI                       ((uint32_t)0x05000000) /* HSI clock selected as MCO source */
#define RCC_CFGR0_MCO_HSE                       ((uint32_t)0x06000000) /* HSE clock selected as MCO source  */
#define RCC_CFGR0_MCO_PLL                       ((uint32_t)0x07000000) /* PLL clock divided by 2 selected as MCO source */

#define RCC_ADC_DUTY_CHG                        ((uint32_t)0x70000000) /* DUTY_CHG[2:0] bits */
#define RCC_ADC_DUTY_CHG_0                      ((uint32_t)0x10000000)
#define RCC_ADC_DUTY_CHG_1                      ((uint32_t)0x20000000)
#define RCC_ADC_DUTY_CHG_2                      ((uint32_t)0x40000000)

#define RCC_ADC_PRE_ADJ                         ((uint32_t)0x80000000)

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
#define RCC_TIM4RST                             ((uint32_t)0x00000004)
#define RCC_WWDGRST                             ((uint32_t)0x00000800) /* Window Watchdog reset */
#define RCC_SPI2RST                             ((uint32_t)0x00004000)
#define RCC_USART2RST                           ((uint32_t)0x00020000) /* USART 2 reset */
#define RCC_USART3RST                           ((uint32_t)0x00040000) /* USART 2 reset */
#define RCC_USART4RST                           ((uint32_t)0x00080000) /* USART 2 reset */
#define RCC_I2C1RST                             ((uint32_t)0x00200000) /* I2C 1 reset */
#define RCC_I2C2RST                             ((uint32_t)0x00400000) /* I2C 1 reset */
#define RCC_CAN1RST                             ((uint32_t)0x02000000) /* CAN1 reset */

#define RCC_BKPRST                              ((uint32_t)0x08000000) /* Backup interface reset */
#define RCC_PWRRST                              ((uint32_t)0x10000000) /* Power interface reset */

#define RCC_TIM4RST                             ((uint32_t)0x00000004) /* Timer 4 reset */
#define RCC_SPI2RST                             ((uint32_t)0x00004000) /* SPI 2 reset */
#define RCC_USART3RST                           ((uint32_t)0x00040000) /* USART 3 reset */
#define RCC_I2C2RST                             ((uint32_t)0x00400000) /* I2C 2 reset */

#define RCC_USBRST                              ((uint32_t)0x00800000) /* USB Device reset */
#define RCC_LPTIMRST                            ((uint32_t)0x80000000) /* USB Device reset */

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
#define RCC_TIM4EN                              ((uint32_t)0x00000004)
#define RCC_WWDGEN                              ((uint32_t)0x00000800) /* Window Watchdog clock enable */
#define RCC_SPI2EN                              ((uint32_t)0x00004000)
#define RCC_USART2EN                            ((uint32_t)0x00020000) /* USART 2 clock enable */
#define RCC_USART3EN                            ((uint32_t)0x00040000) /* USART 3 clock enable */
#define RCC_USART4EN                            ((uint32_t)0x00080000) /* USART 4 clock enable */
#define RCC_I2C1EN                              ((uint32_t)0x00200000) /* I2C 1 clock enable */
#define RCC_I2C2EN                              ((uint32_t)0x00400000) /* I2C 2 clock enable */
#define RCC_CANEN                               ((uint32_t)0x02000000)
#define RCC_BKPEN                               ((uint32_t)0x08000000) /* Backup interface clock enable */
#define RCC_PWREN                               ((uint32_t)0x10000000) /* Power interface clock enable */

#define RCC_USBEN                               ((uint32_t)0x00800000) /* USB Device clock enable */
#define RCC_LPTIMEN                             ((uint32_t)0x80000000)

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

/******************  Bit definition for SPI_HSCR register  ******************/
#define SPI_HSRXEN                              ((uint16_t)0x0001)


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

#define TIM_BKSEL                               ((uint16_t)0x1000)
#define TIM_TMR_CAP_OV_EN                       ((uint16_t)0x4000)
#define TIM_TMR_CAP_LVL_EN                      ((uint16_t)0x8000)

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

#define TIM4_CNT                                ((uint32_t)0xFFFFFFFF) /* Counter Value */

/*******************  Bit definition for TIM_PSC register  ********************/
#define TIM_PSC                                 ((uint16_t)0xFFFF) /* Prescaler Value */

/*******************  Bit definition for TIM_ATRLR register  ********************/
#define TIM_ARR                                 ((uint16_t)0xFFFF) /* actual auto-reload Value */

/*******************  Bit definition for TIM_RPTCR register  ********************/
#define TIM_REP                                 ((uint8_t)0xFF) /* Repetition Counter Value */

/*******************  Bit definition for TIM_CH1CVR register  *******************/
#define TIM_CCR1                                ((uint16_t)0xFFFF) /* Capture/Compare 1 Value */
#define TIM_LEVEL1                              ((uint32_t)0x00010000)

/*******************  Bit definition for TIM_CH2CVR register  *******************/
#define TIM_CCR2                                ((uint16_t)0xFFFF) /* Capture/Compare 2 Value */
#define TIM_LEVEL2                              ((uint32_t)0x00010000)

/*******************  Bit definition for TIM_CH3CVR register  *******************/
#define TIM_CCR3                                ((uint16_t)0xFFFF) /* Capture/Compare 3 Value */
#define TIM_LEVEL3                              ((uint32_t)0x00010000)

/*******************  Bit definition for TIM_CH4CVR register  *******************/
#define TIM_CCR4                                ((uint16_t)0xFFFF) /* Capture/Compare 4 Value */
#define TIM_LEVEL4                              ((uint32_t)0x00010000)

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
#define EXTEN_ETH_10M_EN                        ((uint32_t)0x00000004) /* Bit 2 */
#define EXTEN_ETH_RGMII_SEL                     ((uint32_t)0x00000008) /* Bit 3 (only on parts with RGMII) */
#define EXTEN_PLL_HSI_PRE                       ((uint32_t)0x00000010) /* Bit 4 */
#define EXTEN_LOCKUP_EN                         ((uint32_t)0x00000040) /* Bit 5 */
#define EXTEN_LOCKUP_RSTF                       ((uint32_t)0x00000080) /* Bit 7 */

#define EXTEN_ULLDO_TRIM                        ((uint32_t)0x00000700) /* ULLDO_TRIM[2:0] bits */
#define EXTEN_ULLDO_TRIM0                       ((uint32_t)0x00000100) /* Bit 0 */
#define EXTEN_ULLDO_TRIM1                       ((uint32_t)0x00000200) /* Bit 1 */
#define EXTEN_ULLDO_TRIM2                       ((uint32_t)0x00000400) /* Bit 2 */

#define EXTEN_LDO_TRIM                          ((uint32_t)0x00003000) /* LDO_TRIM[1:0] bits */
#define EXTEN_LDO_TRIM0                         ((uint32_t)0x00001000) /* Bit 0 */
#define EXTEN_LDO_TRIM1                         ((uint32_t)0x00002000) /* Bit 1 */

/******************************************************************************/
/*                               DEBUG SUPPORT                                */
/******************************************************************************/
/*******************  Bit definition for DBGMCU_DR register  *******************/
#define DBG_DEBUGMCU_SLEEP                      ((uint32_t)0x00000001)
#define DBG_DEBUGMCU_STOP                       ((uint32_t)0x00000002)
#define DBG_DEBUGMCU_STBY                       ((uint32_t)0x00000004)
#define DBG_DEBUGMCU_IWDG_STOP                  ((uint32_t)0x00000100)
#define DBG_DEBUGMCU_WWDG_STOP                  ((uint32_t)0x00000200)
#define DBG_DEBUGMCU_I2C1SMBUS_TOUT             ((uint32_t)0x00000400)
#define DBG_DEBUGMCU_I2C2SMBUS_TOUT             ((uint32_t)0x00000800)
#define DBG_DEBUGMCU_TIM1_STOP                  ((uint32_t)0x00001000)
#define DBG_DEBUGMCU_TIM2_STOP                  ((uint32_t)0x00002000)
#define DBG_DEBUGMCU_TIM3_STOP                  ((uint32_t)0x00004000)
#define DBG_DEBUGMCU_TIM4_STOP                  ((uint32_t)0x00008000)
#define DBG_DEBUGMCU_CAN_STOP                   ((uint32_t)0x00010000)

/******************************************************************************/
/*                              OPTICAL PARAMETER                             */
/******************************************************************************/

/*******************  Bit definition for OPA_CFGR1 register  *******************/
#define OPA_CFGR1_POLLEN                        ((uint32_t)0x00000001)
#define OPA_CFGR1_BKINEN                        ((uint32_t)0x00000004)
#define OPA_CFGR1_RSTEN                         ((uint32_t)0x00000010)
#define OPA_CFGR1_OPCMLOCK                      ((uint32_t)0x00000080)
#define OPA_CFGR1_IEOUT                         ((uint32_t)0x00000100)
#define OPA_CFGR1_IECNT                         ((uint32_t)0x00000400)
#define OPA_CFGR1_NMIEN                         ((uint32_t)0x00000800)
#define OPA_CFGR1_IFOUT                         ((uint32_t)0x00001000)
#define OPA_CFGR1_IFCNT                         ((uint32_t)0x00004000)

/*******************  Bit definition for OPA_CFGR2 register  *******************/
#define OPA_CFGR2_POLL_VLU                      ((uint32_t)0x000001FF)
#define OPA_CFGR2_POLL_NUM                      ((uint32_t)0x00000E00)
#define OPA_CFGR2_POLL_CNT                      ((uint32_t)0x00007000)

/*******************  Bit definition for OPA_CTLR1 register  *******************/
#define OPA_CTLR1_EN1                           ((uint32_t)0x00000001)
#define OPA_CTLR1_MODE1                         ((uint32_t)0x0000000E)
#define OPA_CTLR1_PSEL1                         ((uint32_t)0x00000070)
#define OPA_CTLR1_FBEN1                         ((uint32_t)0x00000080)
#define OPA_CTLR1_NSEL1                         ((uint32_t)0x00000F00)
#define OPA_CTLR1_LP1                           ((uint32_t)0x00001000)
#define OPA_CTLR1_INTRIMP                       ((uint32_t)0x00010000)
#define OPA_CTLR1_ITRIMP                        ((uint32_t)0x003E0000)
#define OPA_CTLR1_INTRIMN                       ((uint32_t)0x01000000)
#define OPA_CTLR1_ITRIMN                        ((uint32_t)0x3E000000)

/*******************  Bit definition for OPA_CTLR2 register  *******************/
#define OPA_CTLR2_EN1                           ((uint32_t)0x00000001)
#define OPA_CTLR2_MODE1                         ((uint32_t)0x00000006)
#define OPA_CTLR2_NSEL1                         ((uint32_t)0x00000008)
#define OPA_CTLR2_PSEL1                         ((uint32_t)0x00000010)
#define OPA_CTLR2_HYEN1                         ((uint32_t)0x00000020)
#define OPA_CTLR2_LP1                           ((uint32_t)0x00000040)
#define OPA_CTLR2_EN2                           ((uint32_t)0x00000100)
#define OPA_CTLR2_MODE2                         ((uint32_t)0x00000600)
#define OPA_CTLR2_NSEL2                         ((uint32_t)0x00000800)
#define OPA_CTLR2_PSEL2                         ((uint32_t)0x00001000)
#define OPA_CTLR2_HYEN2                         ((uint32_t)0x00002000)
#define OPA_CTLR2_LP2                           ((uint32_t)0x00004000)
#define OPA_CTLR2_EN3                           ((uint32_t)0x00010000)
#define OPA_CTLR2_MODE3                         ((uint32_t)0x00060000)
#define OPA_CTLR2_NSEL3                         ((uint32_t)0x00080000)
#define OPA_CTLR2_PSEL3                         ((uint32_t)0x00100000)
#define OPA_CTLR2_HYEN3                         ((uint32_t)0x00200000)
#define OPA_CTLR2_LP3                           ((uint32_t)0x00400000)

#define OPA_CTLR2_WKUP_MD                       ((uint32_t)0x03000000)
#define OPA_CTLR2_WKUP_MD_0                     ((uint32_t)0x01000000)
#define OPA_CTLR2_WKUP_MD_1                     ((uint32_t)0x02000000)

/*******************  Bit definition for OPCMKEY register  *******************/
#define OPCM_KEY                                ((uint32_t)0xFFFFFFFF)

/******************************************************************************/
/*                              LOW POWER TIM                                 */
/******************************************************************************/
/*******************  Bit definition for LPTIM_ISR register  *******************/
#define LPTIM_ISR_CMPM                          ((uint32_t)0x00000001)
#define LPTIM_ISR_ARRM                          ((uint32_t)0x00000002)
#define LPTIM_ISR_EXTTRIG                       ((uint32_t)0x00000004)
#define LPTIM_ISR_CMPOK                         ((uint32_t)0x00000008)
#define LPTIM_ISR_ARROK                         ((uint32_t)0000000010)
#define LPTIM_ISR_UP                            ((uint32_t)0x00000020)
#define LPTIM_ISR_DOWN                          ((uint32_t)0x00000040)
#define LPTIM_ISR_DIRSYNC                       ((uint32_t)0x00000080)

/*******************  Bit definition for LPTIM_ICR register  *******************/
#define LPTIM_ICR_CMPMCF                        ((uint32_t)0x00000001)
#define LPTIM_ICR_ARRMCF                        ((uint32_t)0x00000002)
#define LPTIM_ICR_EXTTRIGCF                     ((uint32_t)0x00000004)
#define LPTIM_ICR_CMPOKCF                       ((uint32_t)0x00000008)
#define LPTIM_ICR_ARROKCF                       ((uint32_t)0x00000010)
#define LPTIM_ICR_UPCF                          ((uint32_t)0x00000020)
#define LPTIM_ICR_DOWNCF                        ((uint32_t)0x00000040)

/*******************  Bit definition for LPTIM_IER register  *******************/
#define LPTIM_IER_CMPMIE                        ((uint32_t)0x00000001)
#define LPTIM_IER_ARRMIE                        ((uint32_t)0x00000002)
#define LPTIM_IER_EXTTRIGIE                     ((uint32_t)0x00000004)
#define LPTIM_IER_CMPOKIE                       ((uint32_t)0x00000008)
#define LPTIM_IER_ARROKIE                       ((uint32_t)0x00000010)
#define LPTIM_IER_UPIE                          ((uint32_t)0x00000020)
#define LPTIM_IER_DOWNIE                        ((uint32_t)0x00000040)

/*******************  Bit definition for LPTIM_CFGR register  *******************/
#define LPTIM_CFGR_CKSEL                        ((uint32_t)0x00000001)
#define LPTIM_CFGR_CKPOL                        ((uint32_t)0x00000006)
#define LPTIM_CFGR_CKFLT                        ((uint32_t)0x00000018)
#define LPTIM_CFGR_TRGFLT                       ((uint32_t)0x000000C0)
#define LPTIM_CFGR_PRESC                        ((uint32_t)0x00000E00)
#define LPTIM_CFGR_TRIGSEL                      ((uint32_t)0x00006000)
#define LPTIM_CFGR_TRIGEN                       ((uint32_t)0x00060000)
#define LPTIM_CFGR_TIMOUT                       ((uint32_t)0x00080000)
#define LPTIM_CFGR_WAVE                         ((uint32_t)0x00100000)
#define LPTIM_CFGR_WAVPOL                       ((uint32_t)0x00200000)
#define LPTIM_CFGR_PRELOAD                      ((uint32_t)0x00400000)
#define LPTIM_CFGR_CONTMODE                     ((uint32_t)0x00800000)
#define LPTIM_CFGR_ENC                          ((uint32_t)0x01000000)
#define LPTIM_CFGR_CLKSEL                       ((uint32_t)0x06000000)
#define LPTIM_CFGR_FORCEPWM                     ((uint32_t)0x08000000)

/*******************  Bit definition for LPTIM_CR register  *******************/
#define LPTIM_CR_ENABLE                         ((uint32_t)0x00000001)
#define LPTIM_CR_SNGSTRT                        ((uint32_t)0x00000002)
#define LPTIM_CR_CNTSTRT                        ((uint32_t)0x00000004)
#define LPTIM_CR_OUTEN                          ((uint32_t)0x00000008)
#define LPTIM_CR_DIR_EXTEN                      ((uint32_t)0x00000010)

/*******************  Bit definition for LPTIM_CMP register  *******************/
#define LPTIM_CMP                               ((uint32_t)0x0000FFFF)

/*******************  Bit definition for LPTIM_ARR register  *******************/
#define LPTIM_ARR                               ((uint32_t)0x0000FFFF)

/*******************  Bit definition for LPTIM_CNT register  *******************/
#define LPTIM_COUNT                             ((uint32_t)0x0000FFFF)

/******************************************************************************/
/*                              TOUCH KEY                                     */
/******************************************************************************/
/*******************  Bit definition for TKEY_CHARGE register  *******************/
#define TKEY_CHGOFFSET                          ((uint32_t)0x000003FF)

/*******************  Bit definition for TKEY_ACT_DCG register  *******************/
#define TKEY_TKACTDCG                           ((uint32_t)0x000003FF)

/*******************  Bit definition for TKEY_DR register  *******************/
#define TKEY_DR                                 ((uint32_t)0x0000FFFF)


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


/* ch32v20x_can.c ------------------------------------------------------------*/
/* CAN CTLR Register bits */
#define CTLR_DBF                     ((uint32_t)0x00010000)

/* CAN Mailbox Transmit Request */
#define TMIDxR_TXRQ                  ((uint32_t)0x00000001)

/* CAN FCTLR Register bits */
#define FCTLR_FINIT                  ((uint32_t)0x00000001)

/* Time out for INAK bit */
#define INAK_TIMEOUT                 ((uint32_t)0x0000FFFF)
/* Time out for SLAK bit */
#define SLAK_TIMEOUT                 ((uint32_t)0x0000FFFF)


/* Flags in TSTATR register */
#define CAN_FLAGS_TSTATR             ((uint32_t)0x08000000)
/* Flags in RFIFO1 register */
#define CAN_FLAGS_RFIFO1             ((uint32_t)0x04000000)
/* Flags in RFIFO0 register */
#define CAN_FLAGS_RFIFO0             ((uint32_t)0x02000000)
/* Flags in STATR register */
#define CAN_FLAGS_STATR              ((uint32_t)0x01000000)
/* Flags in ERRSR register */
#define CAN_FLAGS_ERRSR              ((uint32_t)0x00F00000)

/* Mailboxes definition */
#define CAN_TXMAILBOX_0              ((uint8_t)0x00)
#define CAN_TXMAILBOX_1              ((uint8_t)0x01)
#define CAN_TXMAILBOX_2              ((uint8_t)0x02)


#define CAN_MODE_MASK                ((uint32_t) 0x00000003)



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
#define DMA1_Channel8_IT_Mask    ((uint32_t)(DMA_GIF8 | DMA_TCIF8 | DMA_HTIF8 | DMA_TEIF8))


/* DMA2 FLAG mask */
// Editor's note: Overloaded Definition.
#define DMA2_FLAG_Mask            ((uint32_t)0x10000000)

/* DMA registers Masks */
#define CFGR_CLEAR_Mask          ((uint32_t)0xFFFF800F)

/* ch32v00x_exti.c -----------------------------------------------------------*/


/* No interrupt selected */
#define EXTI_LINENONE    ((uint32_t)0x00000)

/* ch32v00x_flash.c ----------------------------------------------------------*/



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
#define CR_BER32                   ((uint32_t)0x00040000)
#define CR_BER64                   ((uint32_t)0x00080000)
#define CR_PG_STRT                 ((uint32_t)0x00200000)

/* FLASH Status Register bits */
#define SR_BSY                     ((uint32_t)0x00000001)
#define SR_WR_BSY                  ((uint32_t)0x00000002)
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

/* EEPROM address */
#define EEPROM_ADDRESS             ((uint32_t)0x8070000)

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


/* ch32v20x_opa.c ------------------------------------------------------------*/
#define OPA_MASK         ((uint32_t)0x000F)
#define OPA_Total_NUM    4


/* ch32v00x_pwr.c ------------------------------------------------------------*/


/* PWR registers bit mask */
/* CTLR register bit mask */
#define CTLR_DS_MASK     ((uint32_t)0xFFFFFFFD)
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
#define ADC_ExternalTrigConv_T1_CC3                    ((uint32_t)0x00040000)
#define ADC_ExternalTrigConv_T2_CC2                    ((uint32_t)0x00060000)
#define ADC_ExternalTrigConv_T3_TRGO                   ((uint32_t)0x00080000)
#define ADC_ExternalTrigConv_T4_CC4                    ((uint32_t)0x000A0000)
#define ADC_ExternalTrigConv_Ext_IT11                  ((uint32_t)0x000C0000)
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
#define ADC_Channel_18                                 ((uint8_t)0x12)
#define ADC_Channel_TempSensor                         ((uint8_t)ADC_Channel_16)
#define ADC_Channel_Vrefint                            ((uint8_t)ADC_Channel_17)
#define ADC_Channel_CalInternal                        ((uint8_t)ADC_Channel_18)

/*ADC_output_buffer*/
#define ADC_OutputBuffer_Enable                        ((uint32_t)0x04000000)
#define ADC_OutputBuffer_Disable                       ((uint32_t)0x00000000)
#define ADC_BUFEN                                      ((uint32_t)0x04000000)

/*ADC_pga*/
#define ADC_Pga_1                                      ((uint32_t)0x00000000)
#define ADC_Pga_4                                      ((uint32_t)0x08000000)
#define ADC_Pga_16                                     ((uint32_t)0x10000000)
#define ADC_Pga_64                                     ((uint32_t)0x18000000)

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
#define ADC_ExternalTrigInjecConv_T1_TRGO              ((uint32_t)0x00000000)
#define ADC_ExternalTrigInjecConv_T1_CC4               ((uint32_t)0x00001000)
#define ADC_ExternalTrigInjecConv_T2_TRGO              ((uint32_t)0x00002000)
#define ADC_ExternalTrigInjecConv_T2_CC1               ((uint32_t)0x00003000)
#define ADC_ExternalTrigInjecConv_T3_CC4               ((uint32_t)0x00004000)
#define ADC_ExternalTrigInjecConv_T4_TRGO              ((uint32_t)0x00005000)
#define ADC_ExternalTrigInjecConv_Ext_IT15             ((uint32_t)0x00006000)
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

/* ADC_Sample_mode_definition */
#define ADC_Sample_NoOver_1M_Mode                      ((uint32_t)0x00000000)
#define ADC_Sample_Over_1M_Mode                        ((uint32_t)0x00000020)

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



/* ch32v20x_can.h ------------------------------------------------------------*/

/* CAN_sleep_constants */
#define CAN_InitStatus_Failed               ((uint8_t)0x00) /* CAN initialization failed */
#define CAN_InitStatus_Success              ((uint8_t)0x01) /* CAN initialization OK */

/* CAN_Mode */
#define CAN_Mode_Normal                     ((uint8_t)0x00) /* normal mode */
#define CAN_Mode_LoopBack                   ((uint8_t)0x01) /* loopback mode */
#define CAN_Mode_Silent                     ((uint8_t)0x02) /* silent mode */
#define CAN_Mode_Silent_LoopBack            ((uint8_t)0x03) /* loopback combined with silent mode */

/* CAN_Operating_Mode */
#define CAN_OperatingMode_Initialization    ((uint8_t)0x00) /* Initialization mode */
#define CAN_OperatingMode_Normal            ((uint8_t)0x01) /* Normal mode */
#define CAN_OperatingMode_Sleep             ((uint8_t)0x02) /* sleep mode */

/* CAN_Mode_Status */
#define CAN_ModeStatus_Failed               ((uint8_t)0x00)                   /* CAN entering the specific mode failed */
#define CAN_ModeStatus_Success              ((uint8_t)!CAN_ModeStatus_Failed) /* CAN entering the specific mode Succeed */

/* CAN_synchronisation_jump_width */
#define CAN_SJW_1tq                         ((uint8_t)0x00) /* 1 time quantum */
#define CAN_SJW_2tq                         ((uint8_t)0x01) /* 2 time quantum */
#define CAN_SJW_3tq                         ((uint8_t)0x02) /* 3 time quantum */
#define CAN_SJW_4tq                         ((uint8_t)0x03) /* 4 time quantum */

/* CAN_time_quantum_in_bit_segment_1 */
#define CAN_BS1_1tq                         ((uint8_t)0x00) /* 1 time quantum */
#define CAN_BS1_2tq                         ((uint8_t)0x01) /* 2 time quantum */
#define CAN_BS1_3tq                         ((uint8_t)0x02) /* 3 time quantum */
#define CAN_BS1_4tq                         ((uint8_t)0x03) /* 4 time quantum */
#define CAN_BS1_5tq                         ((uint8_t)0x04) /* 5 time quantum */
#define CAN_BS1_6tq                         ((uint8_t)0x05) /* 6 time quantum */
#define CAN_BS1_7tq                         ((uint8_t)0x06) /* 7 time quantum */
#define CAN_BS1_8tq                         ((uint8_t)0x07) /* 8 time quantum */
#define CAN_BS1_9tq                         ((uint8_t)0x08) /* 9 time quantum */
#define CAN_BS1_10tq                        ((uint8_t)0x09) /* 10 time quantum */
#define CAN_BS1_11tq                        ((uint8_t)0x0A) /* 11 time quantum */
#define CAN_BS1_12tq                        ((uint8_t)0x0B) /* 12 time quantum */
#define CAN_BS1_13tq                        ((uint8_t)0x0C) /* 13 time quantum */
#define CAN_BS1_14tq                        ((uint8_t)0x0D) /* 14 time quantum */
#define CAN_BS1_15tq                        ((uint8_t)0x0E) /* 15 time quantum */
#define CAN_BS1_16tq                        ((uint8_t)0x0F) /* 16 time quantum */

/* CAN_time_quantum_in_bit_segment_2 */
#define CAN_BS2_1tq                         ((uint8_t)0x00) /* 1 time quantum */
#define CAN_BS2_2tq                         ((uint8_t)0x01) /* 2 time quantum */
#define CAN_BS2_3tq                         ((uint8_t)0x02) /* 3 time quantum */
#define CAN_BS2_4tq                         ((uint8_t)0x03) /* 4 time quantum */
#define CAN_BS2_5tq                         ((uint8_t)0x04) /* 5 time quantum */
#define CAN_BS2_6tq                         ((uint8_t)0x05) /* 6 time quantum */
#define CAN_BS2_7tq                         ((uint8_t)0x06) /* 7 time quantum */
#define CAN_BS2_8tq                         ((uint8_t)0x07) /* 8 time quantum */

/* CAN_filter_mode */
#define CAN_FilterMode_IdMask               ((uint8_t)0x00) /* identifier/mask mode */
#define CAN_FilterMode_IdList               ((uint8_t)0x01) /* identifier list mode */

/* CAN_filter_scale */
#define CAN_FilterScale_16bit               ((uint8_t)0x00) /* Two 16-bit filters */
#define CAN_FilterScale_32bit               ((uint8_t)0x01) /* One 32-bit filter */

/* CAN_filter_FIFO */
#define CAN_Filter_FIFO0                    ((uint8_t)0x00) /* Filter FIFO 0 assignment for filter x */
#define CAN_Filter_FIFO1                    ((uint8_t)0x01) /* Filter FIFO 1 assignment for filter x */

/* CAN_identifier_type */
#define CAN_Id_Standard                     ((uint32_t)0x00000000) /* Standard Id */
#define CAN_Id_Extended                     ((uint32_t)0x00000004) /* Extended Id */

/* CAN_remote_transmission_request */
#define CAN_RTR_Data                        ((uint32_t)0x00000000) /* Data frame */
#define CAN_RTR_Remote                      ((uint32_t)0x00000002) /* Remote frame */

/* CAN_transmit_constants */
#define CAN_TxStatus_Failed                 ((uint8_t)0x00) /* CAN transmission failed */
#define CAN_TxStatus_Ok                     ((uint8_t)0x01) /* CAN transmission succeeded */
#define CAN_TxStatus_Pending                ((uint8_t)0x02) /* CAN transmission pending */
#define CAN_TxStatus_NoMailBox              ((uint8_t)0x04) /* CAN cell did not provide an empty mailbox */

/* CAN_receive_FIFO_number_constants */
#define CAN_FIFO0                           ((uint8_t)0x00) /* CAN FIFO 0 used to receive */
#define CAN_FIFO1                           ((uint8_t)0x01) /* CAN FIFO 1 used to receive */

/* CAN_sleep_constants */
#define CAN_Sleep_Failed                    ((uint8_t)0x00) /* CAN did not enter the sleep mode */
#define CAN_Sleep_Ok                        ((uint8_t)0x01) /* CAN entered the sleep mode */

/* CAN_wake_up_constants */
#define CAN_WakeUp_Failed                   ((uint8_t)0x00) /* CAN did not leave the sleep mode */
#define CAN_WakeUp_Ok                       ((uint8_t)0x01) /* CAN leaved the sleep mode */

/* CAN_Error_Code_constants */
#define CAN_ErrorCode_NoErr                 ((uint8_t)0x00) /* No Error */
#define CAN_ErrorCode_StuffErr              ((uint8_t)0x10) /* Stuff Error */
#define CAN_ErrorCode_FormErr               ((uint8_t)0x20) /* Form Error */
#define CAN_ErrorCode_ACKErr                ((uint8_t)0x30) /* Acknowledgment Error */
#define CAN_ErrorCode_BitRecessiveErr       ((uint8_t)0x40) /* Bit Recessive Error */
#define CAN_ErrorCode_BitDominantErr        ((uint8_t)0x50) /* Bit Dominant Error */
#define CAN_ErrorCode_CRCErr                ((uint8_t)0x60) /* CRC Error  */
#define CAN_ErrorCode_SoftwareSetErr        ((uint8_t)0x70) /* Software Set Error */

/* CAN_flags */
/* Transmit Flags */
/* If the flag is 0x3XXXXXXX, it means that it can be used with CAN_GetFlagStatus()
 * and CAN_ClearFlag() functions.
 * If the flag is 0x1XXXXXXX, it means that it can only be used with CAN_GetFlagStatus() function.
*/
#define CAN_FLAG_RQCP0                      ((uint32_t)0x38000001) /* Request MailBox0 Flag */
#define CAN_FLAG_RQCP1                      ((uint32_t)0x38000100) /* Request MailBox1 Flag */
#define CAN_FLAG_RQCP2                      ((uint32_t)0x38010000) /* Request MailBox2 Flag */

/* Receive Flags */
#define CAN_FLAG_FMP0                       ((uint32_t)0x12000003) /* FIFO 0 Message Pending Flag */
#define CAN_FLAG_FF0                        ((uint32_t)0x32000008) /* FIFO 0 Full Flag            */
#define CAN_FLAG_FOV0                       ((uint32_t)0x32000010) /* FIFO 0 Overrun Flag         */
#define CAN_FLAG_FMP1                       ((uint32_t)0x14000003) /* FIFO 1 Message Pending Flag */
#define CAN_FLAG_FF1                        ((uint32_t)0x34000008) /* FIFO 1 Full Flag            */
#define CAN_FLAG_FOV1                       ((uint32_t)0x34000010) /* FIFO 1 Overrun Flag         */

/* Operating Mode Flags */
#define CAN_FLAG_WKU                        ((uint32_t)0x31000008) /* Wake up Flag */
#define CAN_FLAG_SLAK                       ((uint32_t)0x31000012) /* Sleep acknowledge Flag */
/* Note:
 *When SLAK intterupt is disabled (SLKIE=0), no polling on SLAKI is possible.
 *In this case the SLAK bit can be polled.
*/


/* Error Flags */
#define CAN_FLAG_EWG                        ((uint32_t)0x10F00001) /* Error Warning Flag   */
#define CAN_FLAG_EPV                        ((uint32_t)0x10F00002) /* Error Passive Flag   */
#define CAN_FLAG_BOF                        ((uint32_t)0x10F00004) /* Bus-Off Flag         */
#define CAN_FLAG_LEC                        ((uint32_t)0x30F00070) /* Last error code Flag */

/* CAN_interrupts */
#define CAN_IT_TME                          ((uint32_t)0x00000001) /* Transmit mailbox empty Interrupt*/

/* Receive Interrupts */
#define CAN_IT_FMP0                         ((uint32_t)0x00000002) /* FIFO 0 message pending Interrupt*/
#define CAN_IT_FF0                          ((uint32_t)0x00000004) /* FIFO 0 full Interrupt*/
#define CAN_IT_FOV0                         ((uint32_t)0x00000008) /* FIFO 0 overrun Interrupt*/
#define CAN_IT_FMP1                         ((uint32_t)0x00000010) /* FIFO 1 message pending Interrupt*/
#define CAN_IT_FF1                          ((uint32_t)0x00000020) /* FIFO 1 full Interrupt*/
#define CAN_IT_FOV1                         ((uint32_t)0x00000040) /* FIFO 1 overrun Interrupt*/

/* Operating Mode Interrupts */
#define CAN_IT_WKU                          ((uint32_t)0x00010000) /* Wake-up Interrupt*/
#define CAN_IT_SLK                          ((uint32_t)0x00020000) /* Sleep acknowledge Interrupt*/

/* Error Interrupts */
#define CAN_IT_EWG                          ((uint32_t)0x00000100) /* Error warning Interrupt*/
#define CAN_IT_EPV                          ((uint32_t)0x00000200) /* Error passive Interrupt*/
#define CAN_IT_BOF                          ((uint32_t)0x00000400) /* Bus-off Interrupt*/
#define CAN_IT_LEC                          ((uint32_t)0x00000800) /* Last error code Interrupt*/
#define CAN_IT_ERR                          ((uint32_t)0x00008000) /* Error Interrupt*/

/* Flags named as Interrupts : kept only for FW compatibility */
#define CAN_IT_RQCP0                        CAN_IT_TME
#define CAN_IT_RQCP1                        CAN_IT_TME
#define CAN_IT_RQCP2                        CAN_IT_TME

/* CAN_Legacy */
#define CANINITFAILED                       CAN_InitStatus_Failed
#define CANINITOK                           CAN_InitStatus_Success
#define CAN_FilterFIFO0                     CAN_Filter_FIFO0
#define CAN_FilterFIFO1                     CAN_Filter_FIFO1
#define CAN_ID_STD                          CAN_Id_Standard
#define CAN_ID_EXT                          CAN_Id_Extended
#define CAN_RTR_DATA                        CAN_RTR_Data
#define CAN_RTR_REMOTE                      CAN_RTR_Remote
#define CANTXFAILE                          CAN_TxStatus_Failed
#define CANTXOK                             CAN_TxStatus_Ok
#define CANTXPENDING                        CAN_TxStatus_Pending
#define CAN_NO_MB                           CAN_TxStatus_NoMailBox
#define CANSLEEPFAILED                      CAN_Sleep_Failed
#define CANSLEEPOK                          CAN_Sleep_Ok
#define CANWAKEUPFAILED                     CAN_WakeUp_Failed
#define CANWAKEUPOK                         CAN_WakeUp_Ok

/* CAN_BS1_Mode */
#define CAN_BS1_4bit                        ((uint32_t)0x00000000)
#define CAN_BS1_6bit                        ((uint32_t)0x00000100)

/* CANFD_data_length_code */
#define CANFD_DLC_BYTES_0                   ((uint32_t)0x0000) /* 0 bytes data field  */
#define CANFD_DLC_BYTES_1                   ((uint32_t)0x0001) /* 1 bytes data field  */
#define CANFD_DLC_BYTES_2                   ((uint32_t)0x0002) /* 2 bytes data field  */
#define CANFD_DLC_BYTES_3                   ((uint32_t)0x0003) /* 3 bytes data field  */
#define CANFD_DLC_BYTES_4                   ((uint32_t)0x0004) /* 4 bytes data field  */
#define CANFD_DLC_BYTES_5                   ((uint32_t)0x0005) /* 5 bytes data field  */
#define CANFD_DLC_BYTES_6                   ((uint32_t)0x0006) /* 6 bytes data field  */
#define CANFD_DLC_BYTES_7                   ((uint32_t)0x0007) /* 7 bytes data field  */
#define CANFD_DLC_BYTES_8                   ((uint32_t)0x0008) /* 8 bytes data field  */
#define CANFD_DLC_BYTES_12                  ((uint32_t)0x0009) /* 12 bytes data field */
#define CANFD_DLC_BYTES_16                  ((uint32_t)0x000A) /* 16 bytes data field */
#define CANFD_DLC_BYTES_20                  ((uint32_t)0x000B) /* 20 bytes data field */
#define CANFD_DLC_BYTES_24                  ((uint32_t)0x000C) /* 24 bytes data field */
#define CANFD_DLC_BYTES_32                  ((uint32_t)0x000D) /* 32 bytes data field */
#define CANFD_DLC_BYTES_48                  ((uint32_t)0x000E) /* 48 bytes data field */
#define CANFD_DLC_BYTES_64                  ((uint32_t)0x000F) /* 64 bytes data field */

/* CANFD_synchronisation_jump_width */
#define CANFD_SJW_1tq                          ((uint8_t)0x00) /* 1 time quantum */
#define CANFD_SJW_2tq                          ((uint8_t)0x01) /* 2 time quantum */
#define CANFD_SJW_3tq                          ((uint8_t)0x02) /* 3 time quantum */
#define CANFD_SJW_4tq                          ((uint8_t)0x03) /* 4 time quantum */
#define CANFD_SJW_5tq                          ((uint8_t)0x04) /* 5 time quantum */
#define CANFD_SJW_6tq                          ((uint8_t)0x05) /* 6 time quantum */
#define CANFD_SJW_7tq                          ((uint8_t)0x06) /* 7 time quantum */
#define CANFD_SJW_8tq                          ((uint8_t)0x07) /* 8 time quantum */
#define CANFD_SJW_9tq                          ((uint8_t)0x08) /* 9 time quantum */
#define CANFD_SJW_10tq                         ((uint8_t)0x09) /* 10 time quantum */
#define CANFD_SJW_11tq                         ((uint8_t)0x0A) /* 11 time quantum */
#define CANFD_SJW_12tq                         ((uint8_t)0x0B) /* 12 time quantum */
#define CANFD_SJW_13tq                         ((uint8_t)0x0C) /* 13 time quantum */
#define CANFD_SJW_14tq                         ((uint8_t)0x0D) /* 14 time quantum */
#define CANFD_SJW_15tq                         ((uint8_t)0x0E) /* 15 time quantum */
#define CANFD_SJW_16tq                         ((uint8_t)0x0F) /* 16 time quantum */

/* CANFD_time_quantum_in_bit_segment_1 */
#define CANFD_BS1_1tq                         ((uint8_t)0x00) /* 1 time quantum */
#define CANFD_BS1_2tq                         ((uint8_t)0x01) /* 2 time quantum */
#define CANFD_BS1_3tq                         ((uint8_t)0x02) /* 3 time quantum */
#define CANFD_BS1_4tq                         ((uint8_t)0x03) /* 4 time quantum */
#define CANFD_BS1_5tq                         ((uint8_t)0x04) /* 5 time quantum */
#define CANFD_BS1_6tq                         ((uint8_t)0x05) /* 6 time quantum */
#define CANFD_BS1_7tq                         ((uint8_t)0x06) /* 7 time quantum */
#define CANFD_BS1_8tq                         ((uint8_t)0x07) /* 8 time quantum */
#define CANFD_BS1_9tq                         ((uint8_t)0x08) /* 9 time quantum */
#define CANFD_BS1_10tq                        ((uint8_t)0x09) /* 10 time quantum */
#define CANFD_BS1_11tq                        ((uint8_t)0x0A) /* 11 time quantum */
#define CANFD_BS1_12tq                        ((uint8_t)0x0B) /* 12 time quantum */
#define CANFD_BS1_13tq                        ((uint8_t)0x0C) /* 13 time quantum */
#define CANFD_BS1_14tq                        ((uint8_t)0x0D) /* 14 time quantum */
#define CANFD_BS1_15tq                        ((uint8_t)0x0E) /* 15 time quantum */
#define CANFD_BS1_16tq                        ((uint8_t)0x0F) /* 16 time quantum */
#define CANFD_BS1_17tq                         ((uint8_t)0x10) /* 17 time quantum */
#define CANFD_BS1_18tq                         ((uint8_t)0x11) /* 18 time quantum */
#define CANFD_BS1_19tq                         ((uint8_t)0x12) /* 19 time quantum */
#define CANFD_BS1_20tq                         ((uint8_t)0x13) /* 20 time quantum */
#define CANFD_BS1_21tq                         ((uint8_t)0x14) /* 21 time quantum */
#define CANFD_BS1_22tq                         ((uint8_t)0x15) /* 22 time quantum */
#define CANFD_BS1_23tq                         ((uint8_t)0x16) /* 23 time quantum */
#define CANFD_BS1_24tq                         ((uint8_t)0x17) /* 24 time quantum */
#define CANFD_BS1_25tq                         ((uint8_t)0x18) /* 25 time quantum */
#define CANFD_BS1_26tq                         ((uint8_t)0x19) /* 26 time quantum */
#define CANFD_BS1_27tq                         ((uint8_t)0x1A) /* 27 time quantum */
#define CANFD_BS1_28tq                         ((uint8_t)0x1B) /* 28 time quantum */
#define CANFD_BS1_29tq                         ((uint8_t)0x1C) /* 29 time quantum */
#define CANFD_BS1_30tq                         ((uint8_t)0x1D) /* 30 time quantum */
#define CANFD_BS1_31tq                         ((uint8_t)0x1E) /* 31 time quantum */
#define CANFD_BS1_32tq                         ((uint8_t)0x1F) /* 32 time quantum */

/* CANFD_time_quantum_in_bit_segment_2 */
#define CANFD_BS2_1tq                          ((uint8_t)0x00) /* 1 time quantum */
#define CANFD_BS2_2tq                          ((uint8_t)0x01) /* 2 time quantum */
#define CANFD_BS2_3tq                          ((uint8_t)0x02) /* 3 time quantum */
#define CANFD_BS2_4tq                          ((uint8_t)0x03) /* 4 time quantum */
#define CANFD_BS2_5tq                          ((uint8_t)0x04) /* 5 time quantum */
#define CANFD_BS2_6tq                          ((uint8_t)0x05) /* 6 time quantum */
#define CANFD_BS2_7tq                          ((uint8_t)0x06) /* 7 time quantum */
#define CANFD_BS2_8tq                          ((uint8_t)0x07) /* 8 time quantum */
#define CANFD_BS2_9tq                          ((uint8_t)0x08) /* 9 time quantum */
#define CANFD_BS2_10tq                         ((uint8_t)0x09) /* 10 time quantum */
#define CANFD_BS2_11tq                         ((uint8_t)0x0A) /* 11 time quantum */
#define CANFD_BS2_12tq                         ((uint8_t)0x0B) /* 12 time quantum */
#define CANFD_BS2_13tq                         ((uint8_t)0x0C) /* 13 time quantum */
#define CANFD_BS2_14tq                         ((uint8_t)0x0D) /* 14 time quantum */
#define CANFD_BS2_15tq                         ((uint8_t)0x0E) /* 15 time quantum */
#define CANFD_BS2_16tq                         ((uint8_t)0x0F) /* 16 time quantum */

/* CAN_Transmit_Mailbox_number_constants */
#define CAN_Transmit_Mailbox0               ((uint8_t)0x00)
#define CAN_Transmit_Mailbox1               ((uint8_t)0x01)
#define CAN_Transmit_Mailbox2               ((uint8_t)0x02)


/* ch32v00x_dac.h ------------------------------------------------------------*/

/* DAC_trigger_selection */
#define DAC_Trigger_None                   ((uint32_t)0x00000000) /* Conversion is automatic once the DAC1_DHRxxxx register
                                                                     has been loaded, and not by external trigger */
#define DAC_Trigger_T6_TRGO                ((uint32_t)0x00000004) /* TIM6 TRGO selected as external conversion trigger for DAC channel */
#define DAC_Trigger_T8_TRGO                ((uint32_t)0x0000000C) /* TIM8 TRGO selected as external conversion trigger for DAC channel
                                                                     only in High-density devices*/
#define DAC_Trigger_T7_TRGO                ((uint32_t)0x00000014) /* TIM7 TRGO selected as external conversion trigger for DAC channel */
#define DAC_Trigger_T5_TRGO                ((uint32_t)0x0000001C) /* TIM5 TRGO selected as external conversion trigger for DAC channel */
#define DAC_Trigger_T2_TRGO                ((uint32_t)0x00000024) /* TIM2 TRGO selected as external conversion trigger for DAC channel */
#define DAC_Trigger_T4_TRGO                ((uint32_t)0x0000002C) /* TIM4 TRGO selected as external conversion trigger for DAC channel */
#define DAC_Trigger_Ext_IT9                ((uint32_t)0x00000034) /* EXTI Line9 event selected as external conversion trigger for DAC channel */
#define DAC_Trigger_Software               ((uint32_t)0x0000003C) /* Conversion started by software trigger for DAC channel */

/* DAC_wave_generation */
#define DAC_WaveGeneration_None            ((uint32_t)0x00000000)
#define DAC_WaveGeneration_Noise           ((uint32_t)0x00000040)
#define DAC_WaveGeneration_Triangle        ((uint32_t)0x00000080)


/* DAC_lfsrunmask_triangleamplitude */
#define DAC_LFSRUnmask_Bit0                ((uint32_t)0x00000000) /* Unmask DAC channel LFSR bit0 for noise wave generation */
#define DAC_LFSRUnmask_Bits1_0             ((uint32_t)0x00000100) /* Unmask DAC channel LFSR bit[1:0] for noise wave generation */
#define DAC_LFSRUnmask_Bits2_0             ((uint32_t)0x00000200) /* Unmask DAC channel LFSR bit[2:0] for noise wave generation */
#define DAC_LFSRUnmask_Bits3_0             ((uint32_t)0x00000300) /* Unmask DAC channel LFSR bit[3:0] for noise wave generation */
#define DAC_LFSRUnmask_Bits4_0             ((uint32_t)0x00000400) /* Unmask DAC channel LFSR bit[4:0] for noise wave generation */
#define DAC_LFSRUnmask_Bits5_0             ((uint32_t)0x00000500) /* Unmask DAC channel LFSR bit[5:0] for noise wave generation */
#define DAC_LFSRUnmask_Bits6_0             ((uint32_t)0x00000600) /* Unmask DAC channel LFSR bit[6:0] for noise wave generation */
#define DAC_LFSRUnmask_Bits7_0             ((uint32_t)0x00000700) /* Unmask DAC channel LFSR bit[7:0] for noise wave generation */
#define DAC_LFSRUnmask_Bits8_0             ((uint32_t)0x00000800) /* Unmask DAC channel LFSR bit[8:0] for noise wave generation */
#define DAC_LFSRUnmask_Bits9_0             ((uint32_t)0x00000900) /* Unmask DAC channel LFSR bit[9:0] for noise wave generation */
#define DAC_LFSRUnmask_Bits10_0            ((uint32_t)0x00000A00) /* Unmask DAC channel LFSR bit[10:0] for noise wave generation */
#define DAC_LFSRUnmask_Bits11_0            ((uint32_t)0x00000B00) /* Unmask DAC channel LFSR bit[11:0] for noise wave generation */
#define DAC_TriangleAmplitude_1            ((uint32_t)0x00000000) /* Select max triangle amplitude of 1 */
#define DAC_TriangleAmplitude_3            ((uint32_t)0x00000100) /* Select max triangle amplitude of 3 */
#define DAC_TriangleAmplitude_7            ((uint32_t)0x00000200) /* Select max triangle amplitude of 7 */
#define DAC_TriangleAmplitude_15           ((uint32_t)0x00000300) /* Select max triangle amplitude of 15 */
#define DAC_TriangleAmplitude_31           ((uint32_t)0x00000400) /* Select max triangle amplitude of 31 */
#define DAC_TriangleAmplitude_63           ((uint32_t)0x00000500) /* Select max triangle amplitude of 63 */
#define DAC_TriangleAmplitude_127          ((uint32_t)0x00000600) /* Select max triangle amplitude of 127 */
#define DAC_TriangleAmplitude_255          ((uint32_t)0x00000700) /* Select max triangle amplitude of 255 */
#define DAC_TriangleAmplitude_511          ((uint32_t)0x00000800) /* Select max triangle amplitude of 511 */
#define DAC_TriangleAmplitude_1023         ((uint32_t)0x00000900) /* Select max triangle amplitude of 1023 */
#define DAC_TriangleAmplitude_2047         ((uint32_t)0x00000A00) /* Select max triangle amplitude of 2047 */
#define DAC_TriangleAmplitude_4095         ((uint32_t)0x00000B00) /* Select max triangle amplitude of 4095 */

/* DAC_output_buffer */
#define DAC_OutputBuffer_Enable            ((uint32_t)0x00000000)
#define DAC_OutputBuffer_Disable           ((uint32_t)0x00000002)

/* DAC_Channel_selection */
#define DAC_Channel_1                      ((uint32_t)0x00000000)
#define DAC_Channel_2                      ((uint32_t)0x00000010)

/* DAC_data_alignment */
#define DAC_Align_12b_R                    ((uint32_t)0x00000000)
#define DAC_Align_12b_L                    ((uint32_t)0x00000004)
#define DAC_Align_8b_R                     ((uint32_t)0x00000008)

/* DAC_wave_generation */
#define DAC_Wave_Noise                     ((uint32_t)0x00000040)
#define DAC_Wave_Triangle                  ((uint32_t)0x00000080)

/* ch32v00x_dbgmcu.h ---------------------------------------------------------*/

/* CFGR0 Register */
#define DBGMCU_SLEEP                 ((uint32_t)0x00000001)
#define DBGMCU_STOP                  ((uint32_t)0x00000002)
#define DBGMCU_STANDBY               ((uint32_t)0x00000004)
#define DBGMCU_IWDG_STOP             ((uint32_t)0x00000100)
#define DBGMCU_WWDG_STOP             ((uint32_t)0x00000200)
#define DBGMCU_I2C1_SMBUS_TIMEOUT    ((uint32_t)0x00000400)
#define DBGMCU_I2C2_SMBUS_TIMEOUT    ((uint32_t)0x00000800)
#define DBGMCU_TIM1_STOP             ((uint32_t)0x00001000)
#define DBGMCU_TIM2_STOP             ((uint32_t)0x00002000)
#define DBGMCU_TIM3_STOP             ((uint32_t)0x00004000)
#define DBGMCU_TIM4_STOP             ((uint32_t)0x00008000)
#define DBGMCU_TIM5_STOP             ((uint32_t)0x00010000)
#define DBGMCU_TIM6_STOP             ((uint32_t)0x00020000)
#define DBGMCU_TIM7_STOP             ((uint32_t)0x00040000)
#define DBGMCU_TIM8_STOP             ((uint32_t)0x00080000)
#define DBGMCU_CAN1_STOP             ((uint32_t)0x00100000)
#define DBGMCU_CAN2_STOP             ((uint32_t)0x00200000)
#define DBGMCU_TIM9_STOP             ((uint32_t)0x00400000)
#define DBGMCU_TIM10_STOP            ((uint32_t)0x00800000)

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
#define DMA1_IT_GL8                        ((uint32_t)0x10000000)
#define DMA1_IT_TC8                        ((uint32_t)0x20000000)
#define DMA1_IT_HT8                        ((uint32_t)0x40000000)
#define DMA1_IT_TE8                        ((uint32_t)0x80000000)


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
#define DMA1_FLAG_GL8                      ((uint32_t)0x10000000)
#define DMA1_FLAG_TC8                      ((uint32_t)0x20000000)
#define DMA1_FLAG_HT8                      ((uint32_t)0x40000000)
#define DMA1_FLAG_TE8                      ((uint32_t)0x80000000)




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
#define EXTI_Line8     ((uint32_t)0x00100) /* External interrupt line 8 */
#define EXTI_Line9     ((uint32_t)0x00200) /* External interrupt line 9 */
#define EXTI_Line10    ((uint32_t)0x00400)  /* External interrupt line 10 */
#define EXTI_Line11    ((uint32_t)0x00800)  /* External interrupt line 11 */
#define EXTI_Line12    ((uint32_t)0x01000)  /* External interrupt line 12 */
#define EXTI_Line13    ((uint32_t)0x02000)  /* External interrupt line 13 */
#define EXTI_Line14    ((uint32_t)0x04000)  /* External interrupt line 14 */
#define EXTI_Line15    ((uint32_t)0x08000)  /* External interrupt line 15 */
#define EXTI_Line16    ((uint32_t)0x10000)  /* External interrupt line 16 Connected to the PVD Output */
#define EXTI_Line17    ((uint32_t)0x20000)  /* External interrupt line 17 Connected to the RTC Alarm event */
#define EXTI_Line19    ((uint32_t)0x80000)  /* External interrupt line 19 Connected to the USBPD Wakeup event */
#define EXTI_Line20    ((uint32_t)0x100000) /* External interrupt line 20 Connected to the USBFS Wakeup event */
#define EXTI_Line21    ((uint32_t)0x200000) /* External interrupt line 21 Connected to the LPTIM Wakeup event */
#define EXTI_Line22    ((uint32_t)0x400000) /* External interrupt line 22 Connected to the COMP Wakeup event */

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
    FLASH_RDP,
    FLASH_OP_RANGE_ERROR = 0xFD,
    FLASH_ALIGN_ERROR = 0xFE,
    FLASH_ADR_RANGE_ERROR = 0xFF,
} FLASH_Status;
#endif

/* Flash_Latency */
#define FLASH_Latency_0                ((uint32_t)0x00000000) /* FLASH Zero Latency cycle */
#define FLASH_Latency_1                ((uint32_t)0x00000001) /* FLASH One Latency cycle */
#define FLASH_Latency_2                ((uint32_t)0x00000002) /* FLASH Two Latency cycle */

/* Write Protect */
#define FLASH_WRProt_Sectors0          ((uint32_t)0x00000001) /* Write protection of sector 0 */
#define FLASH_WRProt_Sectors1          ((uint32_t)0x00000002) /* Write protection of sector 1 */
#define FLASH_WRProt_Sectors2          ((uint32_t)0x00000004) /* Write protection of sector 2 */
#define FLASH_WRProt_Sectors3          ((uint32_t)0x00000008) /* Write protection of sector 3 */
#define FLASH_WRProt_Sectors4          ((uint32_t)0x00000010) /* Write protection of sector 4 */
#define FLASH_WRProt_Sectors5          ((uint32_t)0x00000020) /* Write protection of sector 5 */
#define FLASH_WRProt_Sectors6          ((uint32_t)0x00000040) /* Write protection of sector 6 */
#define FLASH_WRProt_Sectors7          ((uint32_t)0x00000080) /* Write protection of sector 7 */
#define FLASH_WRProt_Sectors8          ((uint32_t)0x00000100) /* Write protection of sector 8 */
#define FLASH_WRProt_Sectors9          ((uint32_t)0x00000200) /* Write protection of sector 9 */
#define FLASH_WRProt_Sectors10         ((uint32_t)0x00000400) /* Write protection of sector 10 */
#define FLASH_WRProt_Sectors11         ((uint32_t)0x00000800) /* Write protection of sector 11 */
#define FLASH_WRProt_Sectors12         ((uint32_t)0x00001000) /* Write protection of sector 12 */
#define FLASH_WRProt_Sectors13         ((uint32_t)0x00002000) /* Write protection of sector 13 */
#define FLASH_WRProt_Sectors14         ((uint32_t)0x00004000) /* Write protection of sector 14 */
#define FLASH_WRProt_Sectors15         ((uint32_t)0x00008000) /* Write protection of sector 15 */
#define FLASH_WRProt_Sectors16         ((uint32_t)0x00010000) /* Write protection of sector 16 */
#define FLASH_WRProt_Sectors17         ((uint32_t)0x00020000) /* Write protection of sector 17 */
#define FLASH_WRProt_Sectors18         ((uint32_t)0x00040000) /* Write protection of sector 18 */
#define FLASH_WRProt_Sectors19         ((uint32_t)0x00080000) /* Write protection of sector 19 */
#define FLASH_WRProt_Sectors20         ((uint32_t)0x00100000) /* Write protection of sector 20 */
#define FLASH_WRProt_Sectors21         ((uint32_t)0x00200000) /* Write protection of sector 21 */
#define FLASH_WRProt_Sectors22         ((uint32_t)0x00400000) /* Write protection of sector 22 */
#define FLASH_WRProt_Sectors23         ((uint32_t)0x00800000) /* Write protection of sector 23 */
#define FLASH_WRProt_Sectors24         ((uint32_t)0x01000000) /* Write protection of sector 24 */
#define FLASH_WRProt_Sectors25         ((uint32_t)0x02000000) /* Write protection of sector 25 */
#define FLASH_WRProt_Sectors26         ((uint32_t)0x04000000) /* Write protection of sector 26 */
#define FLASH_WRProt_Sectors27         ((uint32_t)0x08000000) /* Write protection of sector 27 */
#define FLASH_WRProt_Sectors28         ((uint32_t)0x10000000) /* Write protection of sector 28 */
#define FLASH_WRProt_Sectors29         ((uint32_t)0x20000000) /* Write protection of sector 29 */
#define FLASH_WRProt_Sectors30         ((uint32_t)0x40000000) /* Write protection of sector 30 */
#define FLASH_WRProt_Sectors31         ((uint32_t)0x80000000) /* Write protection of sector 31  */

#define FLASH_WRProt_AllSectors        ((uint32_t)0xFFFFFFFF) /* Write protection of all Sectors */


/* Option_Bytes_IWatchdog */
#define OB_IWDG_SW                       ((uint16_t)0x0001) /* Software IWDG selected */
#define OB_IWDG_HW                       ((uint16_t)0x0000) /* Hardware IWDG selected */

/* Option_Bytes_nRST_STOP */
#define OB_STOP_NoRST                    ((uint16_t)0x0002) /* No reset generated when entering in STOP */
#define OB_STOP_RST                      ((uint16_t)0x0000) /* Reset generated when entering in STOP */

/* Option_Bytes_nRST_STDBY */
#define OB_STDBY_NoRST                   ((uint16_t)0x0004) /* No reset generated when entering in STANDBY */
#define OB_STDBY_RST                     ((uint16_t)0x0000) /* Reset generated when entering in STANDBY */

/* Option_Bytes_OB_CAN_BusOff_MODE */
#define OB_CAN_BusOff_MODE1            ((uint16_t)0x0020) /* Bus off recovery mode1  */
#define OB_CAN_BusOff_MODE2            ((uint16_t)0x0000) /* Bus off recovery mode2 */

/* FLASH_Interrupts */
#define FLASH_IT_ERROR                   ((uint32_t)0x00000400) /* FPEC error interrupt source */
#define FLASH_IT_EOP                     ((uint32_t)0x00001000) /* End of FLASH Operation Interrupt source */
#define FLASH_IT_BANK1_ERROR             FLASH_IT_ERROR         /* FPEC BANK1 error interrupt source */
#define FLASH_IT_BANK1_EOP               FLASH_IT_EOP           /* End of FLASH BANK1 Operation Interrupt source */

/* FLASH_Flags */
#define FLASH_FLAG_BSY                   ((uint32_t)0x00000001) /* FLASH Busy flag */
#define FLASH_FLAG_WRPRTERR              ((uint32_t)0x00000010) /* FLASH Write protected error flag */
#define FLASH_FLAG_EOP                   ((uint32_t)0x00000020) /* FLASH End of Operation flag */
#define FLASH_FLAG_WAKE_UP               ((uint32_t)0x00000040) /* FLASH Wake up of Operation flag */
#define FLASH_FLAG_OPTERR                ((uint32_t)0x00000001) /* FLASH Option Byte error flag */

#define FLASH_FLAG_BANK1_BSY             FLASH_FLAG_BSY       /* FLASH BANK1 Busy flag*/
#define FLASH_FLAG_BANK1_EOP             FLASH_FLAG_EOP       /* FLASH BANK1 End of Operation flag */
#define FLASH_FLAG_BANK1_WRPRTERR        FLASH_FLAG_WRPRTERR  /* FLASH BANK1 Write protected error flag */

/* FLASH_Access_CLK */
#define FLASH_Access_SYSTEM_HALF      ((uint32_t)0x00000000) /* FLASH Enhance Clock = SYSTEM */
#define FLASH_Access_SYSTEM           ((uint32_t)0x02000000) /* Enhance_CLK = SYSTEM/2 */



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
//bit[31:30] = 11b - PCFR1-bit[15-0] and PCFR2-bit[26:16]
/* bit[29:27] = 000b */
#define GPIO_PartialRemap1_SPI1         ((uint32_t)0xC0000001) /* SPI1 Partial1 Alternate Function mapping */
#define GPIO_PartialRemap2_SPI1         ((uint32_t)0xC1000000) /* SPI1 Partial2 Alternate Function mapping */
#define GPIO_FullRemap_SPI1             ((uint32_t)0xC1000001) /* SPI1 Full Alternate Function mapping */
/* bit[29:27] = 001b */
#define GPIO_PartialRemap1_I2C1         ((uint32_t)0xC8800000) /* I2C1 Partial1 Alternate Function mapping */
#define GPIO_FullRemap_I2C1             ((uint32_t)0xC8800002) /* I2C1 Full Alternate Function mapping */
/* bit[29:27] = 010b */
#define GPIO_PartialRemap1_USART1       ((uint32_t)0xD0000004) /* USART1 Partial1 Alternate Function mapping */
#define GPIO_PartialRemap2_USART1       ((uint32_t)0xD0080000) /* USART1 Partial2 Alternate Function mapping */
#define GPIO_PartialRemap3_USART1       ((uint32_t)0xD0080004) /* USART1 Partial3 Alternate Function mapping */
#define GPIO_PartialRemap4_USART1       ((uint32_t)0xD0100000) /* USART1 Partial4 Alternate Function mapping */
#define GPIO_FullRemap_USART1           ((uint32_t)0xD0100004) /* USART1 Full Alternate Function mapping */
/* bit[29:27] = 011b */
#define GPIO_PartialRemap1_USART2       ((uint32_t)0xD8000008) /* USART2 Partial1 Alternate Function mapping */
#define GPIO_PartialRemap2_USART2       ((uint32_t)0xD8040000) /* USART2 Partial2 Alternate Function mapping */
#define GPIO_FullRemap_USART2           ((uint32_t)0xD8040008) /* USART2 Full Alternate Function mapping */
/* bit[29:27] = 100b */
#define GPIO_PartialRemap1_TIM1         ((uint32_t)0xE0000040) /* TIM1 Partial1 Alternate Function mapping */
#define GPIO_PartialRemap2_TIM1         ((uint32_t)0xE0000080) /* TIM1 Partial2 Alternate Function mapping */
#define GPIO_PartialRemap3_TIM1         ((uint32_t)0xE00000C0) /* TIM1 Partial3 Alternate Function mapping */
#define GPIO_PartialRemap4_TIM1         ((uint32_t)0xE0400000) /* TIM1 Partial4 Alternate Function mapping */
#define GPIO_PartialRemap5_TIM1         ((uint32_t)0xE0400040) /* TIM1 Partial5 Alternate Function mapping */
#define GPIO_FullRemap_TIM1             ((uint32_t)0xE04000C0) /* TIM1 Full Alternate Function mapping */
/* bit[29:27] = 101b */
#define GPIO_PartialRemap1_TIM2         ((uint32_t)0xE8000100) /* TIM2 Partial1 Alternate Function mapping */
#define GPIO_PartialRemap2_TIM2         ((uint32_t)0xE8000200) /* TIM2 Partial2 Alternate Function mapping */
#define GPIO_PartialRemap3_TIM2         ((uint32_t)0xE8000300) /* TIM2 Partial3 Alternate Function mapping */
#define GPIO_PartialRemap4_TIM2         ((uint32_t)0xE8200000) /* TIM2 Partial4 Alternate Function mapping */
#define GPIO_PartialRemap5_TIM2         ((uint32_t)0xE8200100) /* TIM2 Partial5 Alternate Function mapping */
#define GPIO_FullRemap_TIM2             ((uint32_t)0xE8200300) /* TIM2 Full Alternate Function mapping */

//bit[31:30] = 00b - PCFR1
#define GPIO_PartialRemap_USART3        ((uint32_t)0x00140020) /* USART3 Partial Alternate Function mapping */
#define GPIO_FullRemap_USART3           ((uint32_t)0x00140030) /* USART3 Full Alternate Function mapping */
#define GPIO_Remap_TIM3                 ((uint32_t)0x00000400) /* TIM3 Alternate Function mapping */
#define GPIO_Remap_TIM4                 ((uint32_t)0x00001000) /* TIM4 Alternate Function mapping */
#define GPIO_Remap1_CAN1                ((uint32_t)0x001D4000) /* CAN1 Alternate Function mapping */
#define GPIO_Remap2_CAN1                ((uint32_t)0x001D6000) /* CAN1 Alternate Function mapping */
#define GPIO_Remap_PD01                 ((uint32_t)0x00008000) /* PD01 Alternate Function mapping */
#define GPIO_Remap_SWJ_Disable          ((uint32_t)0x00300400) /* GPIO_Remap_SWJ_Disable - Full SDI Disabled (SDI) */

//bit[31:30] = 01b - PCFR2
#define GPIO_Remap_USART4               ((uint32_t)0x40000001) /* USART4 Alternate Function mapping */
#define GPIO_Remap_LPTIM                ((uint32_t)0x40000200) /* LPTIM Alternate Function mapping */

/* GPIO_Port_Sources */
#define GPIO_PortSourceGPIOA            ((uint8_t)0x00)
#define GPIO_PortSourceGPIOB            ((uint8_t)0x01)
#define GPIO_PortSourceGPIOC            ((uint8_t)0x02)
#define GPIO_PortSourceGPIOD            ((uint8_t)0x03)
#define GPIO_PortSourceGPIOE            ((uint8_t)0x04)
#define GPIO_PortSourceGPIOF            ((uint8_t)0x05)
#define GPIO_PortSourceGPIOG            ((uint8_t)0x06)

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

/* ch32l103_lptim.h -----------------------------------------------------------*/

/* LPTIM_Clock_Source */
#define LPTIM_ClockSource_In                      ((uint32_t)0x00000000)
#define LPTIM_ClockSource_Ex                      ((uint32_t)0x00000001)

/* LPTIM_ClockPolarity */
#define LPTIM_ClockPolarity_Rising                ((uint32_t)0x00000000)
#define LPTIM_ClockPolarity_Falling               ((uint32_t)0x00000002)
#define LPTIM_ClockPolarity_Rising_Falling        ((uint32_t)0x00000004)

/* LPTIM_ClockPrescalerTime */
#define LPTIM_ClockSampleTime_0T                  ((uint32_t)0x00000000)
#define LPTIM_ClockSampleTime_2T                  ((uint32_t)0x00000008)
#define LPTIM_ClockSampleTime_4T                  ((uint32_t)0x00000010)
#define LPTIM_ClockSampleTime_8T                  ((uint32_t)0x00000018)

/* LPTIM_TriggerSampleTime */
#define LPTIM_TriggerSampleTime_0T                ((uint32_t)0x00000000)
#define LPTIM_TriggerSampleTime_2T                ((uint32_t)0x00000040)
#define LPTIM_TriggerSampleTime_4T                ((uint32_t)0x00000080)
#define LPTIM_TriggerSampleTime_8T                ((uint32_t)0x000000C0)

/* LPTIM_ClockPrescaler */
#define LPTIM_TClockPrescaler_DIV1                ((uint32_t)0x00000000)
#define LPTIM_TClockPrescaler_DIV2                ((uint32_t)0x00000200)
#define LPTIM_TClockPrescaler_DIV4                ((uint32_t)0x00000400)
#define LPTIM_TClockPrescaler_DIV8                ((uint32_t)0x00000600)
#define LPTIM_TClockPrescaler_DIV16               ((uint32_t)0x00000800)
#define LPTIM_TClockPrescaler_DIV32               ((uint32_t)0x00000A00)
#define LPTIM_TClockPrescaler_DIV64               ((uint32_t)0x00000C00)
#define LPTIM_TClockPrescaler_DIV128              ((uint32_t)0x00000E00)

/* LPTIM_TriggerSource */
#define LPTIM_TriggerSource_ETR                   ((uint32_t)0x00000000)
#define LPTIM_TriggerSource_RTC_ALARM             ((uint32_t)0x00002000)
#define LPTIM_TriggerSource_TAMP                  ((uint32_t)0x00004000)

/* LPTIM_ExTriggerPolarity */
#define LPTIM_ExTriggerPolarity_Disable           ((uint32_t)0x00000000)
#define LPTIM_ExTriggerPolarity_Rising            ((uint32_t)0x00020000)
#define LPTIM_ExTriggerPolarity_Falling           ((uint32_t)0x00040000)
#define LPTIM_ExTriggerPolarity_Rising_Falling    ((uint32_t)0x00060000)

/* LPTIM_OutputPolarity */
#define LPTIM_OutputPolarity_High                 ((uint32_t)0x00000000)
#define LPTIM_OutputPolarity_Low                  ((uint32_t)0x00200000)

/* LPTIM_UpdateMode */
#define LPTIM_UpdateMode0                         ((uint32_t)0x00000000)
#define LPTIM_UpdateMode1                         ((uint32_t)0x00400000)

/* LPTIM_CountSource */
#define LPTIM_CountSource_Internal                ((uint32_t)0x00000000)
#define LPTIM_CountSource_External                ((uint32_t)0x00800000)

/* LPTIM_InClockSource */
#define LPTIM_InClockSource_PCLK1                 ((uint32_t)0x00000000)
#define LPTIM_InClockSource_HSI                   ((uint32_t)0x02000000)
#define LPTIM_InClockSource_LSE                   ((uint32_t)0x04000000)
#define LPTIM_InClockSource_LSI                   ((uint32_t)0x06000000)

/* LPTIM_Flag_Definition */
#define LPTIM_FLAG_DIR_SYNC                       ((uint32_t)0x00000080)
#define LPTIM_FLAG_DOWN                           ((uint32_t)0x00000040)
#define LPTIM_FLAG_UP                             ((uint32_t)0x00000020)
#define LPTIM_FLAG_ARROK                          ((uint32_t)0x00000010)
#define LPTIM_FLAG_CMPOK                          ((uint32_t)0x00000008)
#define LPTIM_FLAG_EXTTRIG                        ((uint32_t)0x00000004)
#define LPTIM_FLAG_ARRM                           ((uint32_t)0x00000002)
#define LPTIM_FLAG_CMPM                           ((uint32_t)0x00000001)

/* LPTIM_Interrupts_Definition */
#define LPTIM_IT_DOWN                             ((uint32_t)0x00000040)
#define LPTIM_IT_UP                               ((uint32_t)0x00000020)
#define LPTIM_IT_ARROK                            ((uint32_t)0x00000010)
#define LPTIM_IT_CMPOK                            ((uint32_t)0x00000008)
#define LPTIM_IT_EXTTRIG                          ((uint32_t)0x00000004)
#define LPTIM_IT_ARRM                             ((uint32_t)0x00000002)
#define LPTIM_IT_CMPM                             ((uint32_t)0x00000001)


#define LPYIM_OutputPolarity_High                 LPTIM_OutputPolarity_High
#define LPYIM_OutputPolarity_Low                  LPTIM_OutputPolarity_Low 
#define LPYIM_UpdateMode0                         LPTIM_UpdateMode0 
#define LPYIM_UpdateMode1                         LPTIM_UpdateMode1

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

/* OPA_member_enumeration */
typedef enum
{
    OPA1 = 0,
} OPA_Num_TypeDef;

/* OPA_out_channel_enumeration */
typedef enum
{
    OUT_IO_OUT0 = 0,       /* PA3 */
    OUT_IO_OUT1,           /* PB1 */
    OUT_IO_OUT2,           /* PA2 */
    OUT_IO_OUT3,           /* PA4 */
    OUT_IO_OUT4,           /* PB0 */
    OUT_IO_OFF
} OPA_Mode_TypeDef;

/* OPA_PSEL_enumeration */
typedef enum
{
    CHP0 = 0,              /* PB15 */
    CHP1,                  /* PB0 */
    CHP2,                  /* PB14 */
    CHP3,                  /* PA7 */
    CHP4,                  /* PA0 */
    CHP5,                  /* PA6 */
    CHP_OFF
} OPA_PSEL_TypeDef;

/* OPA_FB_enumeration */
typedef enum
{
    FB_OFF = 0,
    FB_ON
} OPA_FB_TypeDef;

/* OPA_NSEL_enumeration */
typedef enum
{
    CHN0 = 0,             /* PB11 */
    CHN1,                 /* PA6 */
    CHN2,                 /* PB10 */
    CHN3,                 /* PA5 */
    CHN4,                 /* PA1 */
    CHN5,                 /* PA7 */
    CHN2_PGA_32xIN,       /* PB10 */
    CHN_PGA_8xIN,
    CHN_PGA_16xIN,
    CHN_PGA_32xIN,
    CHN_PGA_64xIN,
    CHN_OFF = 0xF
} OPA_NSEL_TypeDef;

/* OPA_PSEL_POLL_enumeration */
typedef enum
{
    CHP_OPA1_OFF = 0,
    CHP_OPA1_ON,
} OPA_PSEL_POLL_TypeDef;

/* OPA_BKIN_EN_enumeration */
typedef enum
{
    BKIN_OPA1_OFF = 0,  /* TIM1 braking signal source form IO input */
    BKIN_OPA1_ON,       /* TIM1 braking signal source form OPA output */
} OPA_BKIN_EN_TypeDef;

/* OPA_RST_EN_enumeration */
typedef enum
{
    RST_OPA1_OFF = 0,
    RST_OPA1_ON,
} OPA_RST_EN_TypeDef;

/* OPA_OUT_IE_enumeration */
typedef enum
{
    OUT_IE_OPA1_OFF = 0,
    OUT_IE_OPA1_ON,
} OPA_OUT_IE_TypeDef;

/* OPA_CNT_IE_enumeration */
typedef enum
{
    CNT_IE_OFF = 0,
    CNT_IE_ON,
} OPA_CNT_IE_TypeDef;

/* OPA_NMI_IE_enumeration */
typedef enum
{
    NMI_IE_OFF = 0,
    NMI_IE_ON,
} OPA_NMI_IE_TypeDef;

/* OPA_PSEL_POLL_NUM_enumeration */
typedef enum
{
    CHP_POLL_NUM_1 = 0,
    CHP_POLL_NUM_2,
    CHP_POLL_NUM_3,
	CHP_POLL_NUM_4,
    CHP_POLL_NUM_5,
    CHP_POLL_NUM_6
} OPA_PSEL_POLL_NUM_TypeDef;

/* Offset_voltage_adjustment_value_polarity */
typedef enum
{
    OPA_Vos_Ads_N = 0,
    OPA_Vos_Ads_P
} OPA_Vos_ADS_POLARITY_TypeDef;

/* OPA Init Structure definition */
typedef struct
{
    uint16_t                       OPA_POLL_Interval; /* OPA polling interval = (OPA_POLL_Interval+1)*1us
                                                      This parameter must range from 0 to 0x1FF.*/
    OPA_Num_TypeDef                OPA_NUM;  /* Specifies the members of OPA */
    OPA_Mode_TypeDef               Mode;     /* Specifies the mode of OPA */
    OPA_PSEL_TypeDef               PSEL;     /* Specifies the positive channel of OPA */
    OPA_FB_TypeDef                 FB;       /* Specifies the internal feedback resistor of OPA */
    OPA_NSEL_TypeDef               NSEL;     /* Specifies the negative channel of OPA */
    OPA_PSEL_POLL_TypeDef          PSEL_POLL; /* Specifies the positive channel poll of OPA */
    OPA_BKIN_EN_TypeDef            BKIN_EN;  /* Specifies the brake input source of OPA */
    OPA_RST_EN_TypeDef             RST_EN;   /* Specifies the reset source of OPA */
    OPA_OUT_IE_TypeDef             OUT_IE;   /* Specifies the out interrupt of OPA */
    OPA_CNT_IE_TypeDef             CNT_IE;   /* Specifies the out interrupt rising edge of sampling data */
    OPA_NMI_IE_TypeDef             NMI_IE;   /* Specifies the out NIM interrupt of OPA */
    OPA_PSEL_POLL_NUM_TypeDef      POLL_NUM; /* Specifies the number of forward inputs*/
} OPA_InitTypeDef;

/* CMP_member_enumeration */
typedef enum
{
    CMP1 = 0,
    CMP2,
    CMP3
} CMP_Num_TypeDef;

/* CMP_out_channel_enumeration */
typedef enum
{
    OUT_IO0 = 0,
    OUT_IO1,
    OUT_IO_TIM2
} CMP_Mode_TypeDef;

/* CMP_NSEL_enumeration */
typedef enum
{
    CMP_CHN0 = 0,
    CMP_CHN1,
} CMP_NSEL_TypeDef;

/* CMP_PSEL_enumeration */
typedef enum
{
    CMP_CHP_0 = 0,
    CMP_CHP_1,
} CMP_PSEL_TypeDef;

#define CMP_CHP1 CMP_CHP_0
#define CMP_CHP2 CMP_CHP_1

/* CMP_HYEN_enumeration */
typedef enum
{
    CMP_HYEN0 = 0,
    CMP_HYEN1,
} CMP_HYEN_TypeDef;

/* CMP Init Structure definition */
typedef struct
{
    CMP_Num_TypeDef    CMP_NUM;  /* Specifies the members of CMP */
    CMP_Mode_TypeDef   Mode;     /* Specifies the mode of CMP */
    CMP_NSEL_TypeDef   NSEL;     /* Specifies the negative channel of CMP */
    CMP_PSEL_TypeDef   PSEL;     /* Specifies the positive channel of CMP */
    CMP_HYEN_TypeDef   HYEN;     /* Specifies the hysteresis comparator of CMP */
} CMP_InitTypeDef;

/* Current channel for OPA polling enumeration */
typedef enum
{
    O1P0 = 0,
    O1P1,
    O1P2,
    O1P3,
    O1P4,
    O1P5,
} OPA_POLL_NUM_TypeDef;

/* OPA_flags_definition */
#define OPA_FLAG_OUT_OPA1                  ((uint16_t)0x1000)
#define OPA_FLAG_OUT_CNT                   ((uint16_t)0x4000)

/* CMP_WakeUp_IO_mode_definition */
#define CMP_WakeUp_Rising_Falling          ((uint32_t)0x01000000)
#define CMP_WakeUp_Rising                  ((uint32_t)0x02000000)
#define CMP_WakeUp_Falling                 ((uint32_t)0x03000000)

#endif

/* ch32v00x_pwr.h ------------------------------------------------------------*/



/* PVD_detection_level  */
#define PWR_PVDLevel_0            ((uint32_t)0x00000000)
#define PWR_PVDLevel_1            ((uint32_t)0x00000020)
#define PWR_PVDLevel_2            ((uint32_t)0x00000040)
#define PWR_PVDLevel_3            ((uint32_t)0x00000060)
#define PWR_PVDLevel_4            ((uint32_t)0x00000080)
#define PWR_PVDLevel_5            ((uint32_t)0x000000A0)
#define PWR_PVDLevel_6            ((uint32_t)0x000000C0)
#define PWR_PVDLevel_7            ((uint32_t)0x000000E0)

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

/* PLL_multiplication_factor for other CH32L103  */
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

#define RCC_PLLMul_18                   ((uint32_t)0x003C0000)

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
#define RCC_USBCLKSource_PLLCLK_Div1    ((uint32_t)0x00000000)
#define RCC_USBCLKSource_PLLCLK_Div2    ((uint32_t)0x00400000)
#define RCC_USBCLKSource_PLLCLK_Div1_5  ((uint32_t)0x00800000)

/* ADC_clock_source */
#define RCC_PCLK2_Div2                 ((uint32_t)0x00000000)
#define RCC_PCLK2_Div4                 ((uint32_t)0x00004000)
#define RCC_PCLK2_Div6                 ((uint32_t)0x00008000)
#define RCC_PCLK2_Div8                 ((uint32_t)0x0000C000)
#define RCC_HCLK_ADC                   ((uint32_t)0x80000000)

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


#define RCC_AHBPeriph_CRC              ((uint32_t)0x00000040)
#define RCC_AHBPeriph_USBFS            ((uint32_t)0x00001000)
#define RCC_AHBPeriph_USBPD            ((uint32_t)0x00020000)


/* APB2_peripheral */
#define RCC_APB2Periph_AFIO            ((uint32_t)0x00000001)
#define RCC_APB2Periph_GPIOA           ((uint32_t)0x00000004)
#define RCC_APB2Periph_GPIOB           ((uint32_t)0x00000008)
#define RCC_APB2Periph_GPIOC           ((uint32_t)0x00000010)
#define RCC_APB2Periph_GPIOD           ((uint32_t)0x00000020)
#define RCC_APB2Periph_GPIOE           ((uint32_t)0x00000040)
#define RCC_APB2Periph_ADC1            ((uint32_t)0x00000200)
#define RCC_APB2Periph_ADC2            ((uint32_t)0x00000400)
#define RCC_APB2Periph_TIM1            ((uint32_t)0x00000800)
#define RCC_APB2Periph_SPI1            ((uint32_t)0x00001000)
#define RCC_APB2Periph_TIM8            ((uint32_t)0x00002000)
#define RCC_APB2Periph_USART1          ((uint32_t)0x00004000)
#define RCC_APB2Periph_TIM9            ((uint32_t)0x00080000)
#define RCC_APB2Periph_TIM10           ((uint32_t)0x00100000)

/* APB1_peripheral */
#define RCC_APB1Periph_TIM2            ((uint32_t)0x00000001)
#define RCC_APB1Periph_TIM3            ((uint32_t)0x00000002)
#define RCC_APB1Periph_TIM4            ((uint32_t)0x00000004)
#define RCC_APB1Periph_TIM5            ((uint32_t)0x00000008)
#define RCC_APB1Periph_TIM6            ((uint32_t)0x00000010)
#define RCC_APB1Periph_TIM7            ((uint32_t)0x00000020)
#define RCC_APB1Periph_UART6           ((uint32_t)0x00000040)
#define RCC_APB1Periph_UART7           ((uint32_t)0x00000080)
#define RCC_APB1Periph_UART8           ((uint32_t)0x00000100)
#define RCC_APB1Periph_WWDG            ((uint32_t)0x00000800)
#define RCC_APB1Periph_SPI2            ((uint32_t)0x00004000)
#define RCC_APB1Periph_SPI3            ((uint32_t)0x00008000)
#define RCC_APB1Periph_USART2          ((uint32_t)0x00020000)
#define RCC_APB1Periph_USART3          ((uint32_t)0x00040000)
#define RCC_APB1Periph_USART4          ((uint32_t)0x00080000)
#define RCC_APB1Periph_UART5           ((uint32_t)0x00100000)
#define RCC_APB1Periph_I2C1            ((uint32_t)0x00200000)
#define RCC_APB1Periph_I2C2            ((uint32_t)0x00400000)
#define RCC_APB1Periph_USB             ((uint32_t)0x00800000)
#define RCC_APB1Periph_CAN1            ((uint32_t)0x02000000)
#define RCC_APB1Periph_CAN2            ((uint32_t)0x04000000)
#define RCC_APB1Periph_BKP             ((uint32_t)0x08000000)
#define RCC_APB1Periph_PWR             ((uint32_t)0x10000000)
#define RCC_APB1Periph_DAC             ((uint32_t)0x20000000)
#define RCC_APB1Periph_LPTIM           ((uint32_t)0x80000000)

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

/* ADC_clock_H_Level_Duty_Cycle */
#define RCC_ADC_H_Level_Mode0   ((uint32_t)0x00000000)
#define RCC_ADC_H_Level_Mode1   ((uint32_t)0x10000000)
#define RCC_ADC_H_Level_Mode2   ((uint32_t)0x20000000)
#define RCC_ADC_H_Level_Mode3   ((uint32_t)0x30000000)
#define RCC_ADC_H_Level_Mode4   ((uint32_t)0x40000000)
#define RCC_ADC_H_Level_Mode5   ((uint32_t)0x50000000)
#define RCC_ADC_H_Level_Mode6   ((uint32_t)0x60000000)
#define RCC_ADC_H_Level_Mode7   ((uint32_t)0x70000000)

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

/* I2S_Mode */
#define I2S_Mode_SlaveTx                   ((uint16_t)0x0000)
#define I2S_Mode_SlaveRx                   ((uint16_t)0x0100)
#define I2S_Mode_MasterTx                  ((uint16_t)0x0200)
#define I2S_Mode_MasterRx                  ((uint16_t)0x0300)

/* I2S_Standard */
#define I2S_Standard_Phillips              ((uint16_t)0x0000)
#define I2S_Standard_MSB                   ((uint16_t)0x0010)
#define I2S_Standard_LSB                   ((uint16_t)0x0020)
#define I2S_Standard_PCMShort              ((uint16_t)0x0030)
#define I2S_Standard_PCMLong               ((uint16_t)0x00B0)

/* I2S_Data_Format */
#define I2S_DataFormat_16b                 ((uint16_t)0x0000)
#define I2S_DataFormat_16bextended         ((uint16_t)0x0001)
#define I2S_DataFormat_24b                 ((uint16_t)0x0003)
#define I2S_DataFormat_32b                 ((uint16_t)0x0005)

/* I2S_MCLK_Output */
#define I2S_MCLKOutput_Enable              ((uint16_t)0x0200)
#define I2S_MCLKOutput_Disable             ((uint16_t)0x0000)

/* I2S_Audio_Frequency */
#define I2S_AudioFreq_192k                 ((uint32_t)192000)
#define I2S_AudioFreq_96k                  ((uint32_t)96000)
#define I2S_AudioFreq_48k                  ((uint32_t)48000)
#define I2S_AudioFreq_44k                  ((uint32_t)44100)
#define I2S_AudioFreq_32k                  ((uint32_t)32000)
#define I2S_AudioFreq_22k                  ((uint32_t)22050)
#define I2S_AudioFreq_16k                  ((uint32_t)16000)
#define I2S_AudioFreq_11k                  ((uint32_t)11025)
#define I2S_AudioFreq_8k                   ((uint32_t)8000)
#define I2S_AudioFreq_Default              ((uint32_t)2)

/* I2S_Clock_Polarity */
#define I2S_CPOL_Low                       ((uint16_t)0x0000)
#define I2S_CPOL_High                      ((uint16_t)0x0008)


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

/* TIM_Capture_Mode */
#define TIM_Capture_Mode0                  ((uint16_t)0x0000)
#define TIM_Capture_Mode1                  ((uint16_t)0x4000)

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
#define USART_FLAG_RX_BUSY                   ((uint16_t)0x0400)
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


/* ch32l103_usb.h ------------------------------------------------------------*/
/*******************************************************************************/
/* USB Communication Related Macro Definition */

/* USB Endpoint0 Size */
#ifndef DEFAULT_ENDP0_SIZE
#define DEFAULT_ENDP0_SIZE          8
#endif

/* USB Buffer Size */
#ifndef USBFS_MAX_PACKET_SIZE
#define USBFS_MAX_PACKET_SIZE      64
#endif

/* USB PID */
#ifndef USB_PID_SETUP
#define USB_PID_NULL                0x00
#define USB_PID_SOF                 0x05
#define USB_PID_SETUP               0x0D
#define USB_PID_IN                  0x09
#define USB_PID_OUT                 0x01
#define USB_PID_NYET                0x06
#define USB_PID_ACK                 0x02
#define USB_PID_NAK                 0x0A
#define USB_PID_STALL               0x0E
#define USB_PID_DATA0               0x03
#define USB_PID_DATA1               0x0B
#define USB_PID_PRE                 0x0C
#endif

/* USB standard device request code */
#ifndef USB_GET_DESCRIPTOR
#define USB_GET_STATUS              0x00
#define USB_CLEAR_FEATURE           0x01
#define USB_SET_FEATURE             0x03
#define USB_SET_ADDRESS             0x05
#define USB_GET_DESCRIPTOR          0x06
#define USB_SET_DESCRIPTOR          0x07
#define USB_GET_CONFIGURATION       0x08
#define USB_SET_CONFIGURATION       0x09
#define USB_GET_INTERFACE           0x0A
#define USB_SET_INTERFACE           0x0B
#define USB_SYNCH_FRAME             0x0C
#endif

#define DEF_STRING_DESC_LANG        0x00
#define DEF_STRING_DESC_MANU        0x01
#define DEF_STRING_DESC_PROD        0x02
#define DEF_STRING_DESC_SERN        0x03

/* USB hub class request code */
#ifndef HUB_GET_DESCRIPTOR
#define HUB_GET_STATUS              0x00
#define HUB_CLEAR_FEATURE           0x01
#define HUB_GET_STATE               0x02
#define HUB_SET_FEATURE             0x03
#define HUB_GET_DESCRIPTOR          0x06
#define HUB_SET_DESCRIPTOR          0x07
#endif

/* USB HID class request code */
#ifndef HID_GET_REPORT
#define HID_GET_REPORT              0x01
#define HID_GET_IDLE                0x02
#define HID_GET_PROTOCOL            0x03
#define HID_SET_REPORT              0x09
#define HID_SET_IDLE                0x0A
#define HID_SET_PROTOCOL            0x0B
#endif

/* Bit Define for USB Request Type */
#ifndef USB_REQ_TYP_MASK
#define USB_REQ_TYP_IN              0x80
#define USB_REQ_TYP_OUT             0x00
#define USB_REQ_TYP_READ            0x80
#define USB_REQ_TYP_WRITE           0x00
#define USB_REQ_TYP_MASK            0x60
#define USB_REQ_TYP_STANDARD        0x00
#define USB_REQ_TYP_CLASS           0x20
#define USB_REQ_TYP_VENDOR          0x40
#define USB_REQ_TYP_RESERVED        0x60
#define USB_REQ_RECIP_MASK          0x1F
#define USB_REQ_RECIP_DEVICE        0x00
#define USB_REQ_RECIP_INTERF        0x01
#define USB_REQ_RECIP_ENDP          0x02
#define USB_REQ_RECIP_OTHER         0x03
#define USB_REQ_FEAT_REMOTE_WAKEUP  0x01
#define USB_REQ_FEAT_ENDP_HALT      0x00
#endif

/* USB Descriptor Type */
#ifndef USB_DESCR_TYP_DEVICE
#define USB_DESCR_TYP_DEVICE        0x01
#define USB_DESCR_TYP_CONFIG        0x02
#define USB_DESCR_TYP_STRING        0x03
#define USB_DESCR_TYP_INTERF        0x04
#define USB_DESCR_TYP_ENDP          0x05
#define USB_DESCR_TYP_QUALIF        0x06
#define USB_DESCR_TYP_SPEED         0x07
#define USB_DESCR_TYP_OTG           0x09
#define USB_DESCR_TYP_BOS           0X0F
#define USB_DESCR_TYP_HID           0x21
#define USB_DESCR_TYP_REPORT        0x22
#define USB_DESCR_TYP_PHYSIC        0x23
#define USB_DESCR_TYP_CS_INTF       0x24
#define USB_DESCR_TYP_CS_ENDP       0x25
#define USB_DESCR_TYP_HUB           0x29
#endif

/* USB Device Class */
#ifndef USB_DEV_CLASS_HUB
#define USB_DEV_CLASS_RESERVED      0x00
#define USB_DEV_CLASS_AUDIO         0x01
#define USB_DEV_CLASS_COMMUNIC      0x02
#define USB_DEV_CLASS_HID           0x03
#define USB_DEV_CLASS_MONITOR       0x04
#define USB_DEV_CLASS_PHYSIC_IF     0x05
#define USB_DEV_CLASS_POWER         0x06
#define USB_DEV_CLASS_IMAGE         0x06
#define USB_DEV_CLASS_PRINTER       0x07
#define USB_DEV_CLASS_STORAGE       0x08
#define USB_DEV_CLASS_HUB           0x09
#define USB_DEV_CLASS_VEN_SPEC      0xFF
#endif

/* USB Hub Class Request */
#ifndef HUB_GET_HUB_DESCRIPTOR
#define HUB_CLEAR_HUB_FEATURE       0x20
#define HUB_CLEAR_PORT_FEATURE      0x23
#define HUB_GET_BUS_STATE           0xA3
#define HUB_GET_HUB_DESCRIPTOR      0xA0
#define HUB_GET_HUB_STATUS          0xA0
#define HUB_GET_PORT_STATUS         0xA3
#define HUB_SET_HUB_DESCRIPTOR      0x20
#define HUB_SET_HUB_FEATURE         0x20
#define HUB_SET_PORT_FEATURE        0x23
#endif

/* Hub Class Feature Selectors */
#ifndef HUB_PORT_RESET
#define HUB_C_HUB_LOCAL_POWER       0
#define HUB_C_HUB_OVER_CURRENT      1
#define HUB_PORT_CONNECTION         0
#define HUB_PORT_ENABLE             1
#define HUB_PORT_SUSPEND            2
#define HUB_PORT_OVER_CURRENT       3
#define HUB_PORT_RESET              4
#define HUB_PORT_POWER              8
#define HUB_PORT_LOW_SPEED          9
#define HUB_C_PORT_CONNECTION       16
#define HUB_C_PORT_ENABLE           17
#define HUB_C_PORT_SUSPEND          18
#define HUB_C_PORT_OVER_CURRENT     19
#define HUB_C_PORT_RESET            20
#endif

/* USB HID Class Request Code */
#ifndef HID_GET_REPORT
#define HID_GET_REPORT              0x01
#define HID_GET_IDLE                0x02
#define HID_GET_PROTOCOL            0x03
#define HID_SET_REPORT              0x09
#define HID_SET_IDLE                0x0A
#define HID_SET_PROTOCOL            0x0B
#endif

/* USB CDC Class request code */
#ifndef CDC_GET_LINE_CODING
#define CDC_GET_LINE_CODING         0x21                                      /* This request allows the host to find out the currently configured line coding */
#define CDC_SET_LINE_CODING         0x20                                      /* Configures DTE rate, stop-bits, parity, and number-of-character */
#define CDC_SET_LINE_CTLSTE         0x22                                      /* This request generates RS-232/V.24 style control signals */
#define CDC_SEND_BREAK              0x23                                      /* Sends special carrier modulation used to specify RS-232 style break */
#endif

/* USB UDisk */
#ifndef USB_BO_CBW_SIZE
#define USB_BO_CBW_SIZE             0x1F
#define USB_BO_CSW_SIZE             0x0D
#endif
#ifndef USB_BO_CBW_SIG0
#define USB_BO_CBW_SIG0             0x55
#define USB_BO_CBW_SIG1             0x53
#define USB_BO_CBW_SIG2             0x42
#define USB_BO_CBW_SIG3             0x43
#define USB_BO_CSW_SIG0             0x55
#define USB_BO_CSW_SIG1             0x53
#define USB_BO_CSW_SIG2             0x42
#define USB_BO_CSW_SIG3             0x53
#endif

/*******************************************************************************/
/* USBFS Related Register Macro Definition */

/* R8_USB_CTRL */
#define USBFS_UC_HOST_MODE          0x80
#define USBFS_UC_LOW_SPEED          0x40
#define USBFS_UC_SYS_CTRL_MASK      0x30
#define USBFS_UC_SYS_CTRL0          0x00
#define USBFS_UC_SYS_CTRL1          0x10
#define USBFS_UC_SYS_CTRL2          0x20
#define USBFS_UC_SYS_CTRL3          0x30
#define USBFS_UC_DEV_PU_EN          0x20
#define USBFS_UC_INT_BUSY           0x08
#define USBFS_UC_RESET_SIE          0x04
#define USBFS_UC_CLR_ALL            0x02
#define USBFS_UC_DMA_EN             0x01

/* R8_USB_INT_EN */
#define USBFS_UIE_DEV_NAK           0x40
#define USBFS_UID_1_WIRE            0x20
#define USBFS_UIE_FIFO_OV           0x10
#define USBFS_UIE_HST_SOF           0x08
#define USBFS_UIE_SUSPEND           0x04
#define USBFS_UIE_TRANSFER          0x02
#define USBFS_UIE_DETECT            0x01
#define USBFS_UIE_BUS_RST           0x01

/* R8_USB_DEV_AD */
#define USBFS_UDA_GP_BIT            0x80
#define USBFS_USB_ADDR_MASK         0x7F

/* R8_USB_MIS_ST */
#define USBFS_UMS_SOF_PRES          0x80
#define USBFS_UMS_SOF_ACT           0x40
#define USBFS_UMS_SIE_FREE          0x20
#define USBFS_UMS_R_FIFO_RDY        0x10
#define USBFS_UMS_BUS_RESET         0x08
#define USBFS_UMS_SUSPEND           0x04
#define USBFS_UMS_DM_LEVEL          0x02
#define USBFS_UMS_DEV_ATTACH        0x01

/* R8_USB_INT_FG */
#define USBFS_U_IS_NAK              0x80    // RO, indicate current USB transfer is NAK received
#define USBFS_U_TOG_OK              0x40    // RO, indicate current USB transfer toggle is OK
#define USBFS_U_SIE_FREE            0x20    // RO, indicate USB SIE free status
#define USBFS_UIF_FIFO_OV           0x10    // FIFO overflow interrupt flag for USB, direct bit address clear or write 1 to clear
#define USBFS_UIF_HST_SOF           0x08    // host SOF timer interrupt flag for USB host, direct bit address clear or write 1 to clear
#define USBFS_UIF_SUSPEND           0x04    // USB suspend or resume event interrupt flag, direct bit address clear or write 1 to clear
#define USBFS_UIF_TRANSFER          0x02    // USB transfer completion interrupt flag, direct bit address clear or write 1 to clear
#define USBFS_UIF_DETECT            0x01    // device detected event interrupt flag for USB host mode, direct bit address clear or write 1 to clear
#define USBFS_UIF_BUS_RST           0x01    // bus reset event interrupt flag for USB device mode, direct bit address clear or write 1 to clear

/* R8_USB_INT_ST */
#define USBFS_SETUP_ACT             0x80      // RO, indicate current SETUP transaction completed
#define USBFS_UIS_TOG_OK            0x40      // RO, indicate current USB transfer toggle is OK
#define USBFS_UIS_TOKEN_MASK        0x30      // RO, bit mask of current token PID code received for USB device mode
#define USBFS_UIS_TOKEN_OUT         0x00
#define USBFS_UIS_TOKEN_IN          0x20
#define USBFS_UIS_TOKEN_SETUP       0x30
// bUIS_TOKEN1 & bUIS_TOKEN0: current token PID code received for USB device mode
//   00: OUT token PID received
//   10: IN token PID received
//   11: SETUP token PID received
#define USBFS_UIS_ENDP_MASK         0x0F      // RO, bit mask of current transfer endpoint number for USB device mode
#define USBFS_UIS_H_RES_MASK        0x0F      // RO, bit mask of current transfer handshake response for USB host mode: 0000=no response, time out from device, others=handshake response PID received

/* R8_UDEV_CTRL */
#define USBFS_UD_PD_DIS             0x80      // disable USB UDP/UDM pulldown resistance: 0=enable pulldown, 1=disable
#define USBFS_UD_DP_PIN             0x20      // ReadOnly: indicate current UDP pin level
#define USBFS_UD_DM_PIN             0x10      // ReadOnly: indicate current UDM pin level
#define USBFS_UD_LOW_SPEED          0x04      // enable USB physical port low speed: 0=full speed, 1=low speed
#define USBFS_UD_GP_BIT             0x02      // general purpose bit
#define USBFS_UD_PORT_EN            0x01      // enable USB physical port I/O: 0=disable, 1=enable

/* R8_UEP4_1_MOD */
#define USBFS_UEP1_RX_EN            0x80      // enable USB endpoint 1 receiving (OUT)
#define USBFS_UEP1_TX_EN            0x40      // enable USB endpoint 1 transmittal (IN)
#define USBFS_UEP1_BUF_MOD          0x10      // buffer mode of USB endpoint 1
#define USBFS_UEP4_RX_EN            0x08      // enable USB endpoint 4 receiving (OUT)
#define USBFS_UEP4_TX_EN            0x04      // enable USB endpoint 4 transmittal (IN)
#define USBFS_UEP4_BUF_MOD          0x01

/* R8_UEP2_3_MOD */
#define USBFS_UEP3_RX_EN            0x80      // enable USB endpoint 3 receiving (OUT)
#define USBFS_UEP3_TX_EN            0x40      // enable USB endpoint 3 transmittal (IN)
#define USBFS_UEP3_BUF_MOD          0x10      // buffer mode of USB endpoint 3
#define USBFS_UEP2_RX_EN            0x08      // enable USB endpoint 2 receiving (OUT)
#define USBFS_UEP2_TX_EN            0x04      // enable USB endpoint 2 transmittal (IN)
#define USBFS_UEP2_BUF_MOD          0x01      // buffer mode of USB endpoint 2

/* R8_UEP5_6_MOD */
#define USBFS_UEP6_RX_EN            0x80      // enable USB endpoint 6 receiving (OUT)
#define USBFS_UEP6_TX_EN            0x40      // enable USB endpoint 6 transmittal (IN)
#define USBFS_UEP6_BUF_MOD          0x10      // buffer mode of USB endpoint 6
#define USBFS_UEP5_RX_EN            0x08      // enable USB endpoint 5 receiving (OUT)
#define USBFS_UEP5_TX_EN            0x04      // enable USB endpoint 5 transmittal (IN)
#define USBFS_UEP5_BUF_MOD          0x01      // buffer mode of USB endpoint 5

/* R8_UEP7_MOD */
#define USBFS_UEP7_RX_EN            0x08      // enable USB endpoint 7 receiving (OUT)
#define USBFS_UEP7_TX_EN            0x04      // enable USB endpoint 7 transmittal (IN)
#define USBFS_UEP7_BUF_MOD          0x01      // buffer mode of USB endpoint 7

/* R8_UEPn_TX_CTRL */
#define USBFS_UEP_T_AUTO_TOG        0x08      // enable automatic toggle after successful transfer completion on endpoint 1/2/3: 0=manual toggle, 1=automatic toggle
#define USBFS_UEP_T_TOG             0x04      // prepared data toggle flag of USB endpoint X transmittal (IN): 0=DATA0, 1=DATA1
#define USBFS_UEP_T_RES_MASK        0x03      // bit mask of handshake response type for USB endpoint X transmittal (IN)
#define USBFS_UEP_T_RES_ACK         0x00
#define USBFS_UEP_T_RES_NONE        0x01
#define USBFS_UEP_T_RES_NAK         0x02
#define USBFS_UEP_T_RES_STALL       0x03
// bUEP_T_RES1 & bUEP_T_RES0: handshake response type for USB endpoint X transmittal (IN)
//   00: DATA0 or DATA1 then expecting ACK (ready)
//   01: DATA0 or DATA1 then expecting no response, time out from host, for non-zero endpoint isochronous transactions
//   10: NAK (busy)
//   11: STALL (error)
// host aux setup

/* R8_UEPn_RX_CTRL, n=0-7 */
#define USBFS_UEP_R_AUTO_TOG        0x08      // enable automatic toggle after successful transfer completion on endpoint 1/2/3: 0=manual toggle, 1=automatic toggle
#define USBFS_UEP_R_TOG             0x04      // expected data toggle flag of USB endpoint X receiving (OUT): 0=DATA0, 1=DATA1
#define USBFS_UEP_R_RES_MASK        0x03      // bit mask of handshake response type for USB endpoint X receiving (OUT)
#define USBFS_UEP_R_RES_ACK         0x00
#define USBFS_UEP_R_RES_NONE        0x01
#define USBFS_UEP_R_RES_NAK         0x02
#define USBFS_UEP_R_RES_STALL       0x03
// RB_UEP_R_RES1 & RB_UEP_R_RES0: handshake response type for USB endpoint X receiving (OUT)
//   00: ACK (ready)
//   01: no response, time out to host, for non-zero endpoint isochronous transactions
//   10: NAK (busy)
//   11: STALL (error)

/* R8_UHOST_CTRL */
#define USBFS_UH_PD_DIS             0x80      // disable USB UDP/UDM pulldown resistance: 0=enable pulldown, 1=disable
#define USBFS_UH_DP_PIN             0x20      // ReadOnly: indicate current UDP pin level
#define USBFS_UH_DM_PIN             0x10      // ReadOnly: indicate current UDM pin level
#define USBFS_UH_LOW_SPEED          0x04      // enable USB port low speed: 0=full speed, 1=low speed
#define USBFS_UH_BUS_RESET          0x02      // control USB bus reset: 0=normal, 1=force bus reset
#define USBFS_UH_PORT_EN            0x01      // enable USB port: 0=disable, 1=enable port, automatic disabled if USB device detached

/* R32_UH_EP_MOD */
#define USBFS_UH_EP_TX_EN           0x40      // enable USB host OUT endpoint transmittal
#define USBFS_UH_EP_TBUF_MOD        0x10      // buffer mode of USB host OUT endpoint
#define USBFS_UH_EP_RX_EN           0x08      // enable USB host IN endpoint receiving
#define USBFS_UH_EP_RBUF_MOD        0x01      // buffer mode of USB host IN endpoint

/* R16_UH_SETUP */
#define USBFS_UH_PRE_PID_EN         0x0400      // USB host PRE PID enable for low speed device via hub
#define USBFS_UH_SOF_EN             0x0004      // USB host automatic SOF enable

/* R8_UH_EP_PID */
#define USBFS_UH_TOKEN_MASK         0xF0      // bit mask of token PID for USB host transfer
#define USBFS_UH_ENDP_MASK          0x0F      // bit mask of endpoint number for USB host transfer

/* R8_UH_RX_CTRL */
#define USBFS_UH_R_AUTO_TOG         0x08      // enable automatic toggle after successful transfer completion: 0=manual toggle, 1=automatic toggle
#define USBFS_UH_R_TOG              0x04      // expected data toggle flag of host receiving (IN): 0=DATA0, 1=DATA1
#define USBFS_UH_R_RES              0x01      // prepared handshake response type for host receiving (IN): 0=ACK (ready), 1=no response, time out to device, for isochronous transactions

/* R8_UH_TX_CTRL */
#define USBFS_UH_T_AUTO_TOG         0x08      // enable automatic toggle after successful transfer completion: 0=manual toggle, 1=automatic toggle
#define USBFS_UH_T_TOG              0x04      // prepared data toggle flag of host transmittal (SETUP/OUT): 0=DATA0, 1=DATA1
#define USBFS_UH_T_RES              0x01      // expected handshake response type for host transmittal (SETUP/OUT): 0=ACK (ready), 1=no response, time out from device, for isochronous transactions

/*******************************************************************************/
/* Struct Definition */

/* USB Setup Request */
typedef struct __attribute__((packed)) _USB_SETUP_REQ
{
    uint8_t  bRequestType;
    uint8_t  bRequest;
    uint16_t wValue;
    uint16_t wIndex;
    uint16_t wLength;
} USB_SETUP_REQ, *PUSB_SETUP_REQ;

/* USB Device Descriptor */
typedef struct __attribute__((packed)) _USB_DEVICE_DESCR
{
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint16_t bcdUSB;
    uint8_t  bDeviceClass;
    uint8_t  bDeviceSubClass;
    uint8_t  bDeviceProtocol;
    uint8_t  bMaxPacketSize0;
    uint16_t idVendor;
    uint16_t idProduct;
    uint16_t bcdDevice;
    uint8_t  iManufacturer;
    uint8_t  iProduct;
    uint8_t  iSerialNumber;
    uint8_t  bNumConfigurations;
} USB_DEV_DESCR, *PUSB_DEV_DESCR;

/* USB Configuration Descriptor */
typedef struct __attribute__((packed)) _USB_CONFIG_DESCR
{
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint16_t wTotalLength;
    uint8_t  bNumInterfaces;
    uint8_t  bConfigurationValue;
    uint8_t  iConfiguration;
    uint8_t  bmAttributes;
    uint8_t  MaxPower;
} USB_CFG_DESCR, *PUSB_CFG_DESCR;

/* USB Interface Descriptor */
typedef struct __attribute__((packed)) _USB_INTERF_DESCR
{
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint8_t  bInterfaceNumber;
    uint8_t  bAlternateSetting;
    uint8_t  bNumEndpoints;
    uint8_t  bInterfaceClass;
    uint8_t  bInterfaceSubClass;
    uint8_t  bInterfaceProtocol;
    uint8_t  iInterface;
} USB_ITF_DESCR, *PUSB_ITF_DESCR;

/* USB Endpoint Descriptor */
typedef struct __attribute__((packed)) _USB_ENDPOINT_DESCR
{
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint8_t  bEndpointAddress;
    uint8_t  bmAttributes;
    uint8_t  wMaxPacketSizeL;
    uint8_t  wMaxPacketSizeH;
    uint8_t  bInterval;
} USB_ENDP_DESCR, *PUSB_ENDP_DESCR;

/* USB Configuration Descriptor Set */
typedef struct __attribute__((packed)) _USB_CONFIG_DESCR_LONG
{
    USB_CFG_DESCR  cfg_descr;
    USB_ITF_DESCR  itf_descr;
    USB_ENDP_DESCR endp_descr[ 1 ];
} USB_CFG_DESCR_LONG, *PUSB_CFG_DESCR_LONG;

/* USB HUB Descriptor */
typedef struct __attribute__((packed)) _USB_HUB_DESCR
{
    uint8_t  bDescLength;
    uint8_t  bDescriptorType;
    uint8_t  bNbrPorts;
    uint8_t  wHubCharacteristicsL;
    uint8_t  wHubCharacteristicsH;
    uint8_t  bPwrOn2PwrGood;
    uint8_t  bHubContrCurrent;
    uint8_t  DeviceRemovable;
    uint8_t  PortPwrCtrlMask;
} USB_HUB_DESCR, *PUSB_HUB_DESCR;

/* USB HID Descriptor */
typedef struct __attribute__((packed)) _USB_HID_DESCR
{
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint16_t bcdHID;
    uint8_t  bCountryCode;
    uint8_t  bNumDescriptors;
    uint8_t  bDescriptorTypeX;
    uint8_t  wDescriptorLengthL;
    uint8_t  wDescriptorLengthH;
} USB_HID_DESCR, *PUSB_HID_DESCR;

/* USB UDisk */
typedef struct __attribute__((packed)) _UDISK_BOC_CBW
{
    uint32_t mCBW_Sig;
    uint32_t mCBW_Tag;
    uint32_t mCBW_DataLen;
    uint8_t  mCBW_Flag;
    uint8_t  mCBW_LUN;
    uint8_t  mCBW_CB_Len;
    uint8_t  mCBW_CB_Buf[ 16 ];
} UDISK_BOC_CBW, *PXUDISK_BOC_CBW;

/* USB UDisk */
typedef struct __attribute__((packed)) _UDISK_BOC_CSW
{
    uint32_t mCBW_Sig;
    uint32_t mCBW_Tag;
    uint32_t mCSW_Residue;
    uint8_t  mCSW_Status;
} UDISK_BOC_CSW, *PXUDISK_BOC_CSW;

/* ch32l103_usbpd.h -----------------------------------------------------------*/

#ifndef VOID
#define VOID                    void
#endif
#ifndef CONST
#define CONST                   const
#endif
#ifndef BOOL
typedef unsigned char           BOOL;
#endif
#ifndef BOOLEAN
typedef unsigned char           BOOLEAN;
#endif
#ifndef CHAR
typedef char                    CHAR;
#endif
#ifndef INT8
typedef char                    INT8;
#endif
#ifndef INT16
typedef short                   INT16;
#endif
#ifndef INT32
typedef long                    INT32;
#endif
#ifndef UINT8
typedef unsigned char           UINT8;
#endif
#ifndef UINT16
typedef unsigned short          UINT16;
#endif
#ifndef UINT32
typedef unsigned long           UINT32;
#endif
#ifndef UINT8V
typedef unsigned char volatile  UINT8V;
#endif
#ifndef UINT16V
typedef unsigned short volatile UINT16V;
#endif
#ifndef UINT32V
typedef unsigned long volatile  UINT32V;
#endif

#ifndef PVOID
typedef void                    *PVOID;
#endif
#ifndef PCHAR
typedef char                    *PCHAR;
#endif
#ifndef PCHAR
typedef const char              *PCCHAR;
#endif
#ifndef PINT8
typedef char                    *PINT8;
#endif
#ifndef PINT16
typedef short                   *PINT16;
#endif
#ifndef PINT32
typedef long                    *PINT32;
#endif
#ifndef PUINT8
typedef unsigned char           *PUINT8;
#endif
#ifndef PUINT16
typedef unsigned short          *PUINT16;
#endif
#ifndef PUINT32
typedef unsigned long           *PUINT32;
#endif
#ifndef PUINT8V
typedef volatile unsigned char  *PUINT8V;
#endif
#ifndef PUINT16V
typedef volatile unsigned short *PUINT16V;
#endif
#ifndef PUINT32V
typedef volatile unsigned long  *PUINT32V;
#endif

 /******************************************************************************/
/* Related macro definitions */

/* Define the return value of the function */
#ifndef  SUCCESS
#define  SUCCESS                   0
#endif
#ifndef  FAIL
#define  FAIL                      0xFF
#endif

/* Register Bit Definition */
/* USBPD->CONFIG */
#define PD_FILT_ED           (1<<0)            /* PD pin input filter enable */
#define PD_ALL_CLR          (1<<1)             /* Clear all interrupt flags */
#define CC_SEL              (1<<2)             /* Select PD communication port */
#define PD_DMA_EN           (1<<3)             /* Enable DMA for USBPD */
#define PD_RST_EN           (1<<4)             /* PD mode reset command enable */
#define WAKE_POLAR          (1<<5)             /* PD port wake-up level */
#define IE_PD_IO            (1<<10)            /* PD IO interrupt enable */
#define IE_RX_BIT           (1<<11)            /* Receive bit interrupt enable */
#define IE_RX_BYTE          (1<<12)            /* Receive byte interrupt enable */
#define IE_RX_ACT           (1<<13)            /* Receive completion interrupt enable */
#define IE_RX_RESET         (1<<14)            /* Reset interrupt enable */
#define IE_TX_END           (1<<15)            /* Transfer completion interrupt enable */

/* USBPD->CONTROL */
#define PD_TX_EN            (1<<0)             /* USBPD transceiver mode and transmit enable */
#define BMC_START           (1<<1)             /* BMC send start signal */
#define RX_STATE_0          (1<<2)             /* PD received state bit 0 */
#define RX_STATE_1          (1<<3)             /* PD received state bit 1 */
#define RX_STATE_2          (1<<4)             /* PD received state bit 2 */
#define DATA_FLAG           (1<<5)             /* Cache data valid flag bit */
#define TX_BIT_BACK         (1<<6)             /* Indicates the current bit status of the BMC when sending the code */
#define BMC_BYTE_HI         (1<<7)             /* Indicates the current half-byte status of the PD data being sent and received */

/* USBPD->TX_SEL */
#define TX_SEL1             (0<<0)
#define TX_SEL1_SYNC1       (0<<0)             /* 0-SYNC1 */
#define TX_SEL1_RST1        (1<<0)             /* 1-RST1 */
#define TX_SEL2_Mask        (3<<2)
#define TX_SEL2_SYNC1       (0<<2)             /* 00-SYNC1 */
#define TX_SEL2_SYNC3       (1<<2)             /* 01-SYNC3 */
#define TX_SEL2_RST1        (2<<2)             /* 1x-RST1 */
#define TX_SEL3_Mask        (3<<4)
#define TX_SEL3_SYNC1       (0<<4)             /* 00-SYNC1 */
#define TX_SEL3_SYNC3       (1<<4)             /* 01-SYNC3 */
#define TX_SEL3_RST1        (2<<4)             /* 1x-RST1 */
#define TX_SEL4_Mask        (3<<6)
#define TX_SEL4_SYNC2       (0<<6)             /* 00-SYNC2 */
#define TX_SEL4_SYNC3       (1<<6)             /* 01-SYNC3 */
#define TX_SEL4_RST2        (2<<6)             /* 1x-RST2 */

/* USBPD->STATUS */
#define BMC_AUX            (3<<0)              /* BMC auxiliary information */
#define BMC_AUX_INVALID    (0<<0)              /* 00-Invalid */
#define BMC_AUX_SOP0       (1<<0)              /* 01-SOP0 */
#define BMC_AUX_SOP1_HRST  (2<<0)              /* 10-SOP1 hard reset */
#define BMC_AUX_SOP2_CRST  (3<<0)              /* 11-SOP2 cable reset */
#define BUF_ERR            (1<<2)              /* BUFFER or DMA error interrupt flag */
#define IF_RX_BIT          (1<<3)              /* Receive bit or 5bit interrupt flag */
#define IF_RX_BYTE         (1<<4)              /* Receive byte or SOP interrupt flag */
#define IF_RX_ACT          (1<<5)              /* Receive completion interrupt flag */
#define IF_RX_RESET        (1<<6)              /* Receive reset interrupt flag */
#define IF_TX_END          (1<<7)              /* Transfer completion interrupt flag */

/* USBPD->PORT_CC1 */
/* USBPD->PORT_CC2 */
#define PA_CC_AI           (1<<0)               /* CC port comparator analogue input */
#define CC_PD              (1<<1)               /* CC port pull-down resistor enable */
#define CC_PU_Mask         (3<<2)               /* Clear CC port pull-up current  */
#define CC_NO_PU           (0<<2)               /* 00-Prohibit pull-up current */
#define CC_PU_330          (1<<2)               /* 01-330uA */
#define CC_PU_180          (2<<2)               /* 10-180uA */
#define CC_PU_80           (3<<2)               /* 11-80uA */
#define CC_LVE             (1<<4)               /* CC port output low voltage enable */
#define CC_CE              (7<<5)               /* Enable the voltage comparator on port CC */
#define CC_NO_CMP          (0<<5)               /* 000-closed */
#define CC_CMP_22          (2<<5)               /* 010-0.22V */
#define CC_CMP_45          (3<<5)               /* 011-0.45V */
#define CC_CMP_55          (4<<5)               /* 100-0.55V */
#define CC_CMP_66          (5<<5)               /* 101-0.66V */
#define CC_CMP_95          (6<<5)               /* 110-0.95V */
#define CC_CMP_123         (7<<5)               /* 111-1.23V */

#define USBPD_IN_HVT       (1<<9)
/*********************************************************
* PD pin PB6/PB7 high threshold input mode:
* 1: High threshold input (2.2V typical), to reduce the I/O power consumption during PD communication
* 0: Normal GPIO threshold input
* *******************************************************/

/* Control Message Types */
#define DEF_TYPE_RESERVED          0x00
#define DEF_TYPE_GOODCRC           0x01                                         /* Send By: Source,Sink,Cable Plug */
#define DEF_TYPE_GOTOMIN           0x02                                         /* Send By: Source */
#define DEF_TYPE_ACCEPT            0x03                                         /* Send By: Source,Sink,Cable Plug */
#define DEF_TYPE_REJECT            0x04                                         /* Send By: Source,Sink,Cable Plug */
#define DEF_TYPE_PING              0x05                                         /* Send By: Source */
#define DEF_TYPE_PS_RDY            0x06                                         /* Send By: Source,Sink */
#define DEF_TYPE_GET_SRC_CAP       0x07                                         /* Send By: Sink,DRP */
#define DEF_TYPE_GET_SNK_CAP       0x08                                         /* Send By: Source,DRP */
#define DEF_TYPE_DR_SWAP           0x09                                         /* Send By: Source,Sink */
#define DEF_TYPE_PR_SWAP           0x0A                                         /* Send By: Source,Sink */
#define DEF_TYPE_VCONN_SWAP        0x0B                                         /* Send By: Source,Sink */
#define DEF_TYPE_WAIT              0x0C                                         /* Send By: Source,Sink */
#define DEF_TYPE_SOFT_RESET        0x0D                                         /* Send By: Source,Sink */
#define DEF_TYPE_DATA_RESET        0x0E                                         /* Send By: Source,Sink */
#define DEF_TYPE_DATA_RESET_CMP    0x0F                                         /* Send By: Source,Sink */
#define DEF_TYPE_NOT_SUPPORT       0x10                                         /* Send By: Source,Sink,Cable Plug */
#define DEF_TYPE_GET_SRC_CAP_EX    0x11                                         /* Send By: Sink,DRP */
#define DEF_TYPE_GET_STATUS        0x12                                         /* Send By: Source,Sink */
#define DEF_TYPE_GET_STATUS_R      0X02                                         /* ext=1 */
#define DEF_TYPE_FR_SWAP           0x13                                         /* Send By: Sink */
#define DEF_TYPE_GET_PPS_STATUS    0x14                                         /* Send By: Sink */
#define DEF_TYPE_GET_CTY_CODES     0x15                                         /* Send By: Source,Sink */
#define DEF_TYPE_GET_SNK_CAP_EX    0x16                                         /* Send By: Source,DRP */
#define DEF_TYPE_GET_SRC_INFO      0x17                                         /* Send By: Sink,DRP */
#define DEF_TYPE_GET_REVISION      0x18                                         /* Send By: Source,Sink */

/* Data Message Types */
#define DEF_TYPE_SRC_CAP           0x01                                         /* Send By: Source,Dual-Role Power */
#define DEF_TYPE_REQUEST           0x02                                         /* Send By: Sink */
#define DEF_TYPE_BIST              0x03                                         /* Send By: Tester,Source,Sink */
#define DEF_TYPE_SNK_CAP           0x04                                         /* Send By: Sink,Dual-Role Power */
#define DEF_TYPE_BAT_STATUS        0x05                                         /* Send By: Source,Sink */
#define DEF_TYPE_ALERT             0x06                                         /* Send By: Source,Sink */
#define DEF_TYPE_GET_CTY_INFO      0x07                                         /* Send By: Source,Sink */
#define DEF_TYPE_ENTER_USB         0x08                                         /* Send By: DFP */
#define DEF_TYPE_EPR_REQUEST       0x09                                         /* Send By: Sink */
#define DEF_TYPE_EPR_MODE          0x0A                                         /* Send By: Source,Sink */
#define DEF_TYPE_SRC_INFO          0x0B                                         /* Send By: Source */
#define DEF_TYPE_REVISION          0x0C                                         /* Send By: Source,Sink,Cable Plug */
#define DEF_TYPE_VENDOR_DEFINED    0x0F                                         /* Send By: Source,Sink,Cable Plug */

/* Vendor Define Message Command */
#define DEF_VDM_DISC_IDENT         0x01
#define DEF_VDM_DISC_SVID          0x02
#define DEF_VDM_DISC_MODE          0x03
#define DEF_VDM_ENTER_MODE         0x04
#define DEF_VDM_EXIT_MODE          0x05
#define DEF_VDM_ATTENTION          0x06
#define DEF_VDM_DP_S_UPDATE        0x10
#define DEF_VDM_DP_CONFIG          0x11

/* PD Revision */
#define DEF_PD_REVISION_10         0x00
#define DEF_PD_REVISION_20         0x01
#define DEF_PD_REVISION_30         0x02


/* PD PHY Channel */
#define DEF_PD_CC1                 0x00
#define DEF_PD_CC2                 0x01

#define PIN_CC1                    GPIO_Pin_6
#define PIN_CC2                    GPIO_Pin_7

/* PD Tx Status */
#define DEF_PD_TX_OK               0x00
#define DEF_PD_TX_FAIL             0x01

/* PDO INDEX */
#define PDO_INDEX_1                1
#define PDO_INDEX_2                2
#define PDO_INDEX_3                3
#define PDO_INDEX_4                4
#define PDO_INDEX_5                5

/******************************************************************************/
#define UPD_TMR_TX_96M    (160-1)                                            /* timer value for USB PD BMC transmittal @Fsys=96MHz */
#define UPD_TMR_RX_96M    (240-1)                                            /* timer value for USB PD BMC receiving @Fsys=96MHz */
#define UPD_TMR_TX_48M    (80-1)                                             /* timer value for USB PD BMC transmittal @Fsys=48MHz */
#define UPD_TMR_RX_48M    (120-1)                                            /* timer value for USB PD BMC receiving @Fsys=48MHz */
#define UPD_TMR_TX_24M    (40-1)                                             /* timer value for USB PD BMC transmittal @Fsys=24MHz */
#define UPD_TMR_RX_24M    (60-1)                                             /* timer value for USB PD BMC receiving @Fsys=24MHz */
#define UPD_TMR_TX_12M    (20-1)                                             /* timer value for USB PD BMC transmittal @Fsys=12MHz */
#define UPD_TMR_RX_12M    (30-1)                                             /* timer value for USB PD BMC receiving @Fsys=12MHz */

#define MASK_PD_STAT      0x03                                               /* Bit mask for current PD status */
#define PD_RX_SOP0        0x01                                               /* SOP0 received */
#define PD_RX_SOP1_HRST   0x02                                               /* SOP1 or Hard Reset received */
#define PD_RX_SOP2_CRST   0x03                                               /* SOP2 or Cable Reset received */

#define UPD_SOP0          ( TX_SEL1_SYNC1 | TX_SEL2_SYNC1 | TX_SEL3_SYNC1 | TX_SEL4_SYNC2 )     /* SOP1 */
#define UPD_SOP1          ( TX_SEL1_SYNC1 | TX_SEL2_SYNC1 | TX_SEL3_SYNC3 | TX_SEL4_SYNC3 )     /* SOP2 */
#define UPD_SOP2          ( TX_SEL1_SYNC1 | TX_SEL2_SYNC3 | TX_SEL3_SYNC1 | TX_SEL4_SYNC3 )     /* SOP3 */
#define UPD_HARD_RESET    ( TX_SEL1_RST1  | TX_SEL2_RST1  | TX_SEL3_RST1  | TX_SEL4_RST2  )     /* Hard Reset*/
#define UPD_CABLE_RESET   ( TX_SEL1_RST1  | TX_SEL2_SYNC1 | TX_SEL3_RST1  | TX_SEL4_SYNC3 )     /* Cable Reset*/


#define bCC_CMP_22        0X01
#define bCC_CMP_45        0X02
#define bCC_CMP_55        0X04
#define bCC_CMP_66        0X08
#define bCC_CMP_95        0X10
#define bCC_CMP_123       0X20
#define bCC_CMP_220       0X40

/******************************************************************************/
/* PD State Machine */
typedef enum
{
    STA_IDLE = 0,                                                               /* 0: No task status */
    STA_DISCONNECT,                                                             /* 1: Disconnection */
    STA_SRC_CONNECT,                                                            /* 2: SRC connect */
    STA_RX_SRC_CAP_WAIT,                                                        /* 3: Waiting to receive SRC_CAP */
    STA_RX_SRC_CAP,                                                             /* 4: SRC_CAP received */
    STA_TX_REQ,                                                                 /* 5: Send REQUEST */
    STA_RX_ACCEPT_WAIT,                                                         /* 6: Waiting to receive ACCEPT */
    STA_RX_ACCEPT,                                                              /* 7: ACCEPT received */
    STA_RX_REJECT,                                                              /* 8: REJECT received */
    STA_RX_PS_RDY_WAIT,                                                         /* 9: Waiting to receive PS_RDY */
    STA_RX_PS_RDY,                                                              /* 10: PS_RDY received */
    STA_SINK_CONNECT,                                                           /* 11: SNK access */
    STA_TX_SRC_CAP,                                                             /* 12: Send SRC_CAP */
    STA_RX_REQ_WAIT,                                                            /* 13: Waiting to receive REQUEST */
    STA_RX_REQ,                                                                 /* 14: REQUEST received */
    STA_TX_ACCEPT,                                                              /* 15: Send ACCEPT */
    STA_TX_REJECT,                                                              /* 16: Send REJECT */
    STA_ADJ_VOL,                                                                /* 17: Adjustment of output voltage and current */
    STA_TX_PS_RDY,                                                              /* 18: Send PS_RDY */
    STA_TX_DR_SWAP,                                                             /* 19: Send DR_SWAP */
    STA_RX_DR_SWAP_ACCEPT,                                                      /* 20: Waiting to receive the answer ACCEPT from DR_SWAP */
    STA_TX_PR_SWAP,                                                             /* 21: Send PR_SWAP */
    STA_RX_PR_SWAP_ACCEPT,                                                      /* 22: Waiting to receive the answer ACCEPT from PR_SWAP */
    STA_RX_PR_SWAP_PS_RDY,                                                      /* 23: Waiting to receive the answer PS_RDY from PR_SWAP */
    STA_TX_PR_SWAP_PS_RDY,                                                      /* 24: Send answer PS_RDY for PR_SWAP */
    STA_PR_SWAP_RECON_WAIT,                                                     /* 25: Wait for PR_SWAP before reconnecting */
    STA_SRC_RECON_WAIT,                                                         /* 26: Waiting for SRC to reconnect */
    STA_SINK_RECON_WAIT,                                                        /* 27: Waiting for SNK to reconnect */
    STA_RX_APD_PS_RDY_WAIT,                                                     /* 28: Waiting for PS_RDY from the receiving adapter */
    STA_RX_APD_PS_RDY,                                                          /* 29: PS_RDY received from the adapter */
    STA_MODE_SWITCH,                                                            /* 30: Mode switching */
    STA_TX_SOFTRST,                                                             /* 31: Sending a software reset */
    STA_TX_HRST,                                                                /* 32: Send hardware reset */
    STA_PHY_RST,                                                                /* 33: PHY reset */
    STA_APD_IDLE_WAIT,                                                          /* 34: Waiting for the adapter to become idle */
} CC_STATUS;

/******************************************************************************/
/* PD Message Header Struct */
typedef union
{
    struct _Message_Header
    {
        UINT8  MsgType: 5;                                                      /* Message Type */
        UINT8  PDRole: 1;                                                       /* 0-UFP; 1-DFP */
        UINT8  SpecRev: 2;                                                      /* 00-Rev1.0; 01-Rev2.0; 10-Rev3.0; */
        UINT8  PRRole: 1;                                                       /* 0-Sink; 1-Source */
        UINT8  MsgID: 3;
        UINT8  NumDO: 3;
        UINT8  Ext: 1;
    }Message_Header;
    UINT16 Data;
}_Message_Header;

/******************************************************************************/
/* Bit definition */
typedef union
{
    struct _BITS_
    {
        UINT8  Msg_Recvd: 1;                                                    /* Notify the main program of the receipt of a PD packet */
        UINT8  Connected: 1;                                                    /* PD Physical Layer Connected Flag */
        UINT8  Stop_Det_Chk: 1;                                                 /* 0-Enable detection; 1-Disable disconnection detection */
        UINT8  PD_Role: 1;                                                      /* 0-UFP; 1-DFP */
        UINT8  PR_Role: 1;                                                      /* 0-Sink; 1-Source */
        UINT8  Auto_Ack_PRRole: 1;                                              /* Role used by auto-responder 0:SINK; 1:SOURCE */
        UINT8  PD_Version: 1;                                                   /* PD version 0-PD2.0; 1-PD3.0 */
        UINT8  VDM_Version: 1;                                                  /* VDM Version 0-1.0 1-2.0 */
        UINT8  HPD_Connected: 1;                                                /* HPD Physical Layer Connected Flag */
        UINT8  HPD_Det_Chk: 1;                                                  /* 0-turn off HPD connection detection; 1-turn on HPD connection detection */
        UINT8  CC_Sel_En: 1;                                                    /* 0-CC channel selection toggle enable; 1-CC channel selection toggle disable */
        UINT8  CC_Sel_State: 1;                                                 /* 0-CC channel selection switches to 0; 1-CC channel selection switches to 1 */
        UINT8  PD_Comm_Succ: 1;                                                 /* 0-PD communication unsuccessful; 1-PD communication successful; */
        UINT8  Recv: 3;
    }Bit;
    UINT16 Bit_Flag;
}_BIT_FLAG;

/* PD control-related structures */
typedef struct _PD_CONTROL
{
    CC_STATUS PD_State;                                                         /* PD communication status machine */
    CC_STATUS PD_State_Last;                                                    /* PD communication status machine (last value) */
    UINT8  Msg_ID;                                                              /* ID of the message sent */
    UINT8  Det_Timer;                                                           /* PD connection status detection timing */
    UINT8  Det_Cnt;                                                             /* Number of PD connection status detections */
    UINT8  Det_Sel_Cnt;                                                         /* Number of SEL toggles for PD connection status detection */
    UINT8  HPD_Det_Timer;                                                       /* HPD connection detection timing */
    UINT8  HPD_Det_Cnt;                                                         /* HPD pin connection status detection count */
    UINT16 PD_Comm_Timer;                                                       /* PD shared timing variables */
    UINT8  ReqPDO_Idx;                                                          /* Index of the requested PDO, valid values 1-7 */
    UINT16 PD_BusIdle_Timer;                                                    /* Bus Idle Time Timer */
    UINT8  Mode_Try_Cnt;                                                        /* Number of retries for current mode, highest bit marks mode */
    UINT8  Err_Op_Cnt;                                                          /* Exception operation count */
    UINT8  Adapter_Idle_Cnt;                                                    /* Adapter communication idle timing */
    _BIT_FLAG Flag;                                                             /* Flag byte bit definition */
}PD_CONTROL, *pPD_CONTROL;

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
		#if defined(CH32V10x) || defined(CH32V20x) || defined(CH32L103) || defined(CH32V30x_D8)
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
			#elif FUNCONF_PLL_MULTIPLIER == 18
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

#define LL_TX_POWER_MINUS_18_DBM       0x01
#define LL_TX_POWER_MINUS_10_DBM       0x03
#define LL_TX_POWER_MINUS_5_DBM        0x05
#define LL_TX_POWER_MINUS_3_DBM        0x07
#define LL_TX_POWER_0_DBM              0x09
#define LL_TX_POWER_1_DBM              0x0B
#define LL_TX_POWER_2_DBM              0x0D
#define LL_TX_POWER_3_DBM              0x11
#define LL_TX_POWER_4_DBM              0x15
#define LL_TX_POWER_5_DBM              0x1B
#define LL_TX_POWER_6_DBM              0x25
#define LL_TX_POWER_7_DBM              0x3F

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

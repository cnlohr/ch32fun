#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#ifdef CH570_CH572
#define CRCPOLY1           BB2
#define ACCESSADDRESS1     BB3
#define CTRL_TX            BB13
#define CRCINIT2           BB22
#define CRCPOLY2           BB23
#define ACCESSADDRESS2     BB24
#define TMR                LL25
#define FRAME_BUF          LL30
#define STATE_BUF          LL31
#define CTRL_MOD_RFSTOP    0xfffff8ff
#define DEVSETMODE_ON      ((BB->CTRL_CFG & 0xfffcffff) | 0x20000)
#define DEVSETMODE_OFF     ((BB->CTRL_CFG & 0xfffcffff) | 0x10000)
#define DEVSETMODE_TUNE    0x0558
#define DEVSETMODE_TX      0x0258
#define DEVSETMODE_RX      0x0158
#define CTRL_CFG_PHY_1M    ((BB->CTRL_CFG & 0xfffffcff) | 0x100)
#define CTRL_CFG_PHY_2M    (BB->CTRL_CFG & 0xfffffcff)
#define LL_STATUS_TX       0x20000
#define CTRL_CFG_START_TX  0x1000000
#elif defined(CH571_CH573)
#define TXBUF 		       DMA4
#define ACCESSADDRESS1     BB2
#define CTRL_TX            BB11
#define TMR                LL24
#define FRAME_BUF          LL28
#define STATE_BUF          LL29
#define RFEND_TXCTUNE_INIT 0x180000
#define CTRL_MOD_RFSTOP    0xfffffff8
#define DEVSETMODE_TUNE    0x5d
#define DEVSETMODE_TX      0x5a
#define DEVSETMODE_RX      0x59
#define CTRL_CFG_PHY_1M    (BB->CTRL_CFG | 0x10000000)
#define LL_STATUS_TX       0x20000
#define CTRL_CFG_START_TX  (BB->CTRL_CFG & 0xefffffff)
#elif defined(CH582_CH583)
#define ACCESSADDRESS1     BB2
#define CTRL_TX            BB11
#define TMR                LL25
#define FRAME_BUF          LL28
#define STATE_BUF          LL29
#define RFEND_TXCTUNE_INIT 0x880000
#define CTRL_TX_TXPOWER    0x80010e78
#define CTRL_MOD_RFSTOP    0xfffffff8
#define DEVSETMODE_ON      ((BB->CTRL_CFG & 0xfffffe7f) | 0x100)
#define DEVSETMODE_OFF     ((BB->CTRL_CFG & 0xfffffe7f) | 0x80)
#define DEVSETMODE_TUNE    0x00dd
#define DEVSETMODE_TX      0x00da
#define DEVSETMODE_RX      0x00d9
#define CTRL_CFG_PHY_1M    ((BB->CTRL_CFG & 0xffff0fff) | 0x1000)
#define CTRL_CFG_PHY_2M    (BB->CTRL_CFG & 0xffff0fff)
#define CTRL_CFG_PHY_CODED ((BB->CTRL_CFG & 0xffff0fff) | 0x2000)
#define LL_STATUS_TX       0x2000
#define CTRL_CFG_START_TX  0x800000
#elif (defined(CH584_CH585) || defined(CH591_CH592))
#define ACCESSADDRESS1     BB2
#define CTRL_TX            BB11
#define TMR                LL25
#define FRAME_BUF          LL30
#define STATE_BUF          LL31
#define CTRL_MOD_RFSTOP    0xfffff8ff
#define DEVSETMODE_ON      ((BB->CTRL_CFG & 0xfffffcff) | 0x280)
#define DEVSETMODE_OFF     ((BB->CTRL_CFG & 0xfffffcff) | 0x100)
#define DEVSETMODE_TUNE    0x0558
#define DEVSETMODE_TX      0x0258
#define DEVSETMODE_RX      0x0158
#define CTRL_CFG_PHY_1M    (BB->CTRL_CFG & 0xffffff7f)
#define CTRL_CFG_PHY_2M    (BB->CTRL_CFG | 0x80)
#define LL_STATUS_TX       0x20000
#define CTRL_CFG_START_TX  0x800000
#elif defined(CH32V20x)
#define CH32V208
#define ACCESSADDRESS1     BB2
#define CTRL_TX            BB11
#define TMR                LL25
#define FRAME_BUF          LL28
#define STATE_BUF          LL29
#define RFEND_TXCTUNE_INIT 0x100000
#define CTRL_TX_TXPOWER    0x80010ec8
#define CTRL_MOD_RFSTOP    0xfffffff8
#define DEVSETMODE_ON      ((BB->CTRL_CFG & 0xfffffe7f) | 0x100)
#define DEVSETMODE_OFF     ((BB->CTRL_CFG & 0xfffffe7f) | 0x80)
#define DEVSETMODE_TUNE    0x5d
#define DEVSETMODE_TX      0x5a
#define DEVSETMODE_RX      0x59
#define CTRL_CFG_PHY_1M    ((BB->CTRL_CFG & 0xffff0fff) | 0x1000)
#define CTRL_CFG_PHY_2M    (BB->CTRL_CFG & 0xffff0fff)
#define CTRL_CFG_PHY_CODED ((BB->CTRL_CFG & 0xffff0fff) | 0x2000)
#define LL_STATUS_TX       0x2000
#define CTRL_CFG_START_TX  0x800000
#else
#error "MCU_TARGET selected in Makefile is not supported"
#endif

#ifdef CH32V208
#define BB_BASE  (0x40024100) // Baseband, digital part of the PHY
#define LL_BASE  (0x40024200) // Link Layer, MAC
#define RF_BASE  (0x40025000) // Radio frontend, analog part of the PHY
#else
#define DMA_BASE (0x4000c000)
#define BB_BASE  (0x4000c100)
#define LL_BASE  (0x4000c200)
#define RF_BASE  (0x4000d000)
#endif

#define DMA ((DMA_Type *) DMA_BASE)
#define BB  ((BB_Type *)  BB_BASE)
#define LL  ((LL_Type *)  LL_BASE)
#define RF  ((RF_Type *)  RF_BASE)

#ifdef CH571_CH573
typedef struct {
	volatile uint32_t DMA0;
	volatile uint32_t DMA1;
	volatile uint32_t DMA2;
	volatile uint32_t DMA3;
	volatile uint32_t DMA4;
	volatile uint32_t DMA5;
	volatile uint32_t DMA6;
	volatile uint32_t DMA7;
} DMA_Type;
#endif

typedef struct {
	// bits 0..5 = Channel
	// bit 6 = disable whitening.
	// bit 8 = 1 during normal TX/operation, but clearing does not affect TX.  Note: 0 at reset, set in software.
	// bit 9 = settable, but unknown effect.
	// bit 10 = 1 during normal TX/operation, but clearing does not affect TX.  Note: 1 at reset, not touched in software.
	// bit 16 = cleared by firmware upon TX, but does not seem to have an effect on the TX.
	// bit 17 = settable, but unknown effect
	// bit 20 = settable, but unknown effect.
	// bit 24 = set at end of tx routine
	// bit 29-31 = settable, but unknown effect.
	volatile uint32_t CTRL_CFG;

	volatile uint32_t CRCINIT1;
	volatile uint32_t BB2; // ch570/2: CRCPOLY1, [ch582/3 ch591/2]: ACCESSADDRESS1
	volatile uint32_t BB3; // ch570/2 ACCESSADDRESS1
	volatile uint32_t BB4;
	volatile uint32_t BB5;
	volatile uint32_t BB6;
	volatile uint32_t BB7;
	volatile uint32_t BB8;
	volatile uint32_t BB9;
	volatile uint32_t BB10;
	volatile uint32_t BB11; // ch582/3, ch584/5, ch591/2: CTRL_TX
	volatile uint32_t BB12;

	// default, pre TX is a4000009
	// bit 0: Set normally, but cleared in software when TXing (maybe a ready bit?)
	// bit 1: Unset normally, but cleared anyway by software when TXing (maybe a fault bit?)
	// bit 2: Disables TX.
	// bit 4: Normally 0, but, if set to 1, seems to increase preamble length.
	// bit 8: Normally 0, but, if set, no clear effect.
	// bit 9: Normally 0, but, if set, no clear effect.
	// bits 24-30: TX Power.  Normally 0xA4
	// Oddly, bit 31 seems to maybe be always set.
	volatile uint32_t BB13; // ch570/2: CTRL_TX
	volatile uint32_t BB14;
	volatile uint32_t BB15;
	volatile uint32_t BB16;
	volatile uint32_t BB17;
	volatile uint32_t BB18;
	volatile uint32_t BB19;
	volatile uint32_t BB20;
	volatile uint32_t BB21;
	volatile uint32_t BB22; // ch570/2: CRCINIT2
	volatile uint32_t BB23; // ch570/2: CRCPOLY2
	volatile uint32_t BB24; // ch570/2: ACCESSADDRESS2
} BB_Type;

typedef struct {
	volatile uint32_t LL0;
	volatile uint32_t LL1;
	volatile uint32_t STATUS;
	volatile uint32_t INT_EN;
	volatile uint32_t LL4;
	volatile uint32_t LL5;
	volatile uint32_t LL6;
	volatile uint32_t LL7;
	volatile uint32_t LL8;
	volatile uint32_t LL9;
	volatile uint32_t LL10;
	volatile uint32_t LL11;
	volatile uint32_t LL12;
	volatile uint32_t LL13;
	volatile uint32_t LL14;
	volatile uint32_t LL15;
	volatile uint32_t LL16;
	volatile uint32_t LL17;
	volatile uint32_t LL18;
	volatile uint32_t LL19;

	// Controls a lot of higher-level functions.
	//  For Tuning: 0x30558
	//  For  Idle:  0x30000
	//  For Sending:0x30258
	// Bit 3: Somehow, enables BB
	// Bit 4: Normally 1, controls length/send times of BB, if unset, BB will double-send part of signals.
	// Bit 6: Normally 1, Unknown effect.
	// Bit 9: If 0, no output.
	// Bit 10: Somehow required for TX?
	// Bit 16-17: Normally 1, unknown effect. Seems to suppress odd carrier burst after message.
	volatile uint32_t CTRL_MOD;
	volatile uint32_t LL21;
	volatile uint32_t LL22;
	volatile uint32_t LL23;
	volatile uint32_t LL24; // ch571/3: TMR
	volatile uint32_t LL25; // ch570/2, ch582/3, ch591/2: TMR
	volatile uint32_t LL26;
	volatile uint32_t LL27;
	volatile uint32_t LL28; // ch582/3: FRAME_BUF
	volatile uint32_t LL29; // ch582/3: STATE_BUF
	volatile uint32_t LL30; // ch570/2, ch591/2: FRAME_BUF
	volatile uint32_t LL31; // ch570/2, ch591/2: STATE_BUF
} LL_Type;

typedef struct {
	volatile uint32_t RF0;
	volatile uint32_t RF1;
	volatile uint32_t RF2;
	volatile uint32_t RF3;
	volatile uint32_t RF4;
	volatile uint32_t RF5;
	volatile uint32_t RF6;
	volatile uint32_t RF7;
	volatile uint32_t RF8;
	volatile uint32_t RF9;
	volatile uint32_t RF10;
	volatile uint32_t RF11;
	volatile uint32_t RF12;
	volatile uint32_t RF13;
	volatile uint32_t TXTUNE_CTRL;
	volatile uint32_t RF15;
	volatile uint32_t RF16;
	volatile uint32_t RF17;
	volatile uint32_t RF18;
	volatile uint32_t RF19;
	volatile uint32_t RF20;
	volatile uint32_t RF21;
	volatile uint32_t RF22;
	volatile uint32_t RF23;
	volatile uint32_t RF24;
	volatile uint32_t RF25;
	volatile uint32_t RF26;
	volatile uint32_t RF27;
	volatile uint32_t RF28;
	volatile uint32_t RF29;
	volatile uint32_t RF30;
	volatile uint32_t RF31;
	volatile uint32_t RF32;
	volatile uint32_t RF33;
	volatile uint32_t RF34;
	volatile uint32_t RF35;
	volatile uint32_t TXCTUNE_CO_CTRL;
	volatile uint32_t TXCTUNE_GA_CTRL;
	volatile uint32_t RF38;
	volatile uint32_t RXTUNE;
	volatile uint32_t TXCTUNE_CO[10];
	volatile uint32_t TXCTUNE_GA[3];
} RF_Type;

uint8_t channel_map[] = {1,2,3,4,5,6,7,8,9,10,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,0,11,39};
#define CO_MID (uint8_t)(RF->TXTUNE_CTRL & ~0xffffffc0)
#define GA_MID (uint8_t)((RF->TXTUNE_CTRL & ~0x80ffffff) >> 24)

#define PHY_1M 1
#define PHY_2M 2
#define PHY_S2 4
#define PHY_S8 8

void DevSetMode(uint16_t mode);
__attribute__((aligned(4))) uint32_t LLE_BUF[0x110];
__attribute__((aligned(4))) uint32_t LLE_BUF2[0x110];
volatile uint32_t tuneFilter;
volatile uint32_t tuneFilter2M;
volatile uint32_t rx_ready;



#ifdef CH571_CH573
__attribute__((interrupt))
void BB_IRQHandler() {
	// printf("BB\n");
	if(BB->BB14 & (1<<6)) {
		BB->BB14 &= 0xffffff9f;
	}
	if(BB->BB14 & (1<<1)) {
		BB->BB14 = 0xfffffffd;
		BB->BB20 = 0x45;
	}
	if(BB->BB14 & (1<<4)) {
		BB->BB14 = 0xffffffef;
		BB->BB20 = 0;
	}
}
#endif

__attribute__((interrupt))
void LLE_IRQHandler() {
	// printf("LL\n");
#ifdef CH571_CH573
	if(LL->STATUS & (1<<9)) {
		LL->TMR = 400;
		BB->CTRL_TX = (BB->CTRL_TX & 0xfffffffc) | 2;
		BB->CTRL_CFG |= 0x10000000;
	}
	LL->STATUS = 0;
#elif defined(CH582_CH583)
	if((LL->STATUS & (1<<14)) && (LL->INT_EN & (1<<14))) {
		LL->LL26 = 0xffffffff;
		LL->STATUS = 0x4000;
	}
	else
#endif
	{
		LL->STATUS &= LL->INT_EN;
		BB->CTRL_TX = (BB->CTRL_TX & 0xfffffffc) | 1;
	}
	DevSetMode(0);
	LL->CTRL_MOD &= CTRL_MOD_RFSTOP;
	LL->LL0 |= 0x08;

#ifdef ISLER_CALLBACK
	ISLER_CALLBACK();
#else
	rx_ready = 1;
#endif
}

void RFEND_Reset() {
#ifdef CH571_CH573
	RF->RF3 |= 0x1000;
	ADD_N_NOPS(20);
	RF->RF3 &= 0xffffefff;
	ADD_N_NOPS(20);
	RF->RF3 |= 0x1000;
	ADD_N_NOPS(20);
	RF->RF3 |= 1;
	ADD_N_NOPS(20);
	RF->RF3 &= 0xfffffffe;
	ADD_N_NOPS(20);
	RF->RF3 |= 1;
	ADD_N_NOPS(20);
	RF->RF3 |= 0x100;
	ADD_N_NOPS(20);
	RF->RF3 &= 0xfffffeff;
	ADD_N_NOPS(20);
	RF->RF3 |= 0x100;
	ADD_N_NOPS(20);
#elif defined(CH32V208)
	RF->RF3 = 0x1101;
	ADD_N_NOPS(20);
	RF->RF3 = 0;
	ADD_N_NOPS(20);
	RF->RF3 = 0x1101;
#endif
}

void DevInit(uint8_t TxPower) {
#ifdef CH571_CH573
	DMA->DMA4 = (uint32_t)LLE_BUF;
	DMA->DMA5 = (uint32_t)LLE_BUF;
	DMA->DMA6 = (uint32_t)LLE_BUF2;
	DMA->DMA7 = (uint32_t)LLE_BUF2;
	DMA->DMA2 |= 0x2000;
	DMA->DMA3 |= 0x2000;
	DMA->DMA2 |= 0x1000;
	DMA->DMA3 |= 0x1000;
	DMA->DMA0 |= 2;
	DMA->DMA0 |= 0x20;

	LL->LL5 = 0x50;
	LL->LL7 = 10;
	LL->LL9 = 0x8c;
	LL->LL13 = 0x8c;
	LL->LL17 = 0x50;
	LL->LL19 = 10;
#elif defined(CH570_CH572) || defined(CH582_CH583) || defined(CH584_CH585) || defined(CH591_CH592)
	LL->LL5 = 0x8c;
	LL->LL7 = 0x76;
	LL->LL9 = 0x8c;
	LL->LL13 = 0x8c;
	LL->LL17 = 0x8c;
	LL->LL19 = 0x76;
#elif defined(CH32V208)
	LL->LL5 = 0x8c;
	LL->LL7 = 0x6c;
	LL->LL9 = 0x8c;
	LL->LL13 = 0x8c;
	LL->LL17 = 0x8c;
	LL->LL19 = 0x6c;
#endif

#ifdef CH570_CH572
	LL->LL11 = 0x6c;
	LL->LL15 = 0x6c;
	LL->LL1 = 0x78;
	LL->LL21 = 0;
	LL->INT_EN = 0x16000f;
#elif defined(CH571_CH573)
	LL->LL11 = 0x3c;
	LL->LL15 = 0x3c;
	LL->LL22 = 0xf6;
	LL->INT_EN = 0xc303;
	NVIC->FIBADDRR = 0x20000000;
	NVIC->VTFADDR[2] = (uint32_t)LLE_IRQHandler -NVIC->FIBADDRR;
#elif defined(CH582_CH583) || defined(CH32V208)
	LL->LL11 = 0x3c;
	LL->LL15 = 0x3c;
	LL->INT_EN = 0xf00f;
#elif defined(CH584_CH585)
	LL->LL11 = 0x6e;
	LL->LL15 = 0x6e;
	LL->LL1 &= 0xffffffe1;
	LL->LL21 = 0;
	LL->INT_EN = 0x1f000f;
#elif defined(CH591_CH592)
	LL->LL6 = 0x78;
	LL->LL8 = 0xffffffff;
	LL->LL11 = 0x6e;
	LL->LL21 = 0x14;
	LL->INT_EN = 0x1f000f;
#endif

	LL->STATE_BUF = (uint32_t)LLE_BUF;
	LL->STATUS = 0xffffffff;
	RF->RF10 = 0x480;

#ifdef CH570_CH572
	RF->RF12 &= 0xfff9ffff;
	RF->RF12 |= 0x70000000;
	RF->RF15 = (RF->RF15 & 0xf8ffffff) | 0x2000000;
	RF->RF15 = (RF->RF15 & 0x1fffffff) | 0x40000000;
	RF->RF18 &= 0xfff8ffff;
	RF->RF20 = (RF->RF20 & 0xfffff8ff) | 0x300;
	RF->RF23 |= 0x70000;
	RF->RF23 |= 0x700000;

	BB->BB14 = 0x2020c;
	BB->BB15 = 0x50;
	BB->CTRL_TX = (BB->CTRL_TX & 0x1ffffff) | (TxPower | 0x40) << 0x19;
	BB->CTRL_CFG &= 0xfffffcff;
#elif defined(CH571_CH573) || defined(CH582_CH583) || defined(CH32V208)
	RFEND_Reset();
	RF->RF18 = (RF->RF18 & 0x8fffffff) | 0x20000000;
	RF->RF18 = (RF->RF18 & 0xf8ffffff) | 0x4000000;
	RF->RF18 = (RF->RF18 & 0xfffffff0) | 9;
	RF->RF18 &= 0xfff8ffff;
	RF->RF18 |= 0x80000000;
	RF->RF19 = (RF->RF19 & 0xfffffff8) | 3;
	RF->RF19 = (RF->RF19 & 0xffffff8f) | 0x30;
	RF->RF19 = (RF->RF19 & 0xfffff8ff) | 0x300;
	RF->RF19 &= 0xfeffffff;
	RF->RF19 |= 0x2000000;
	RF->RF20 = (RF->RF20 & 0xffff0fff) | 0x4000;
	RF->RF21 = (RF->RF21 & 0xfffffff0) | 0xc;
	RF->RF21 |= 0x80;
	RF->RF21 &= 0xffffefff;
	RF->RF15 = (RF->RF15 & 0xffff0fff) | 0x8000;
	RF->RF15 = (RF->RF15 & 0xf8ffffff) | 0x2000000;
	RF->RF15 = (RF->RF15 & 0x1fffffff) | 0x40000000;
	RF->RF11 |= 0x700000;
	RF->RF11 &= 0xf8ffffff;
	RF->RF11 = (RF->RF11 & 0xffffcfff) | 0x2000;
	RF->RF11 = (RF->RF11 & 0xfffcffff) | 0x20000;
	RF->RF12 &= 0xfffffff0;
	RF->RF12 &= 0xffffff0f;
	RF->RF12 &= 0xfffff8ff;
	RF->RF12 |= 0x700000;
	RF->RF12 = (RF->RF12 & 0x8fffffff) | 0x50000000;
	RF->TXTUNE_CTRL = (RF->TXTUNE_CTRL & 0xff07ffff) | RFEND_TXCTUNE_INIT;
	RF->TXTUNE_CTRL |= 0x80000000;

#ifdef CH571_CH573
	BB->CTRL_CFG = (TxPower << 8) | BB->CTRL_CFG | 0x1008000;
	BB->CTRL_CFG = (BB->CTRL_CFG & 0xffffc0ff) | (TxPower & 0x3f) << 8;
	SYS_SAFE_ACCESS(
		R16_AUX_POWER_ADJ = (TxPower < 0x15) ? (R16_AUX_POWER_ADJ & 0xffef):
												(R16_AUX_POWER_ADJ | 0x10);
		);
	BB->CTRL_TX = 0x10e78;
	BB->BB6 |= 0x8000;
	BB->BB6 = (BB->BB6 & 0xffff807f) | 0x3500;
	BB->BB13 = 0x152;

	// NVIC->VTFADDR[3] = (uint32_t)BB_IRQHandler +0x14000000; // why 14000000?
#elif defined(CH582_CH583) || defined(CH32V208)
	BB->CTRL_CFG |= 0x800000;
	BB->CTRL_CFG |= 0x10000000;
	BB->BB13 = 0x1d0;
	BB->CTRL_TX = TxPower << 0x19 | CTRL_TX_TXPOWER;
	BB->CTRL_TX = (BB->CTRL_TX & 0x81ffffff) | (TxPower & 0x3f) << 0x19;
	BB->BB8 = 0x90083;

	// NVIC->VTFADDR[3] = (uint32_t)BB_IRQHandler +0x20000000; // why 20000000?
#endif
#elif defined(CH584_CH585) || defined(CH591_CH592)
	RF->RF12 = (RF->RF12 & 0x8fffffff) | 0x10077700;
	RF->RF15 = (RF->RF15 & 0x18ff0fff) | 0x42005000;
	RF->RF19 &= 0xfffcff88;
	RF->RF21 = (RF->RF21 & 0xfffffff0) | 9;
	RF->RF23 &= 0xff88ffff;

	BB->CTRL_CFG |= 0x800000;
	BB->BB14 = 0x3ff; // ch584/5
	BB->BB13 = 0x50;
	BB->CTRL_TX = (BB->CTRL_TX & 0x81ffffff) | (TxPower & 0x3f) << 0x19;
	uint32_t uVar3 = 0x1000000;
	uint32_t uVar4 = RF->RF23 & 0xf8ffffff;
	if(TxPower < 29) { // ch585: 27
		/* uVar3 and uVar4 are initialized properly already */
	}
	else if(TxPower < 35) {
		uVar3 = 0x3000000;
	}
	else if(TxPower < 59) {
		uVar3 = 0x5000000;
	}
	else {
		uVar4 = RF->RF23;
		uVar3 = 0x7000000;
	}
	RF->RF23 = uVar4 | uVar3;
	BB->BB15 = 0x2020c; // ch584/5
	BB->BB4 = (BB->BB4 & 0xffffffc0) | 0xe;
#endif

	NVIC->VTFIDR[3] = 0x14;
}

void DevSetMode(uint16_t mode) {
#if !defined(CH571_CH573)
	if(mode) {
		BB->CTRL_CFG = DEVSETMODE_ON;
		RF->RF2 |= 0x330000;
	}
	else {
		BB->CTRL_CFG = DEVSETMODE_OFF;
		RF->RF2 &= 0xffcdffff;
	}
#ifdef CH582_CH583
	mode = (mode == 0) ? 0x80 : mode;
#elif !defined(CH32V208)
	mode |= 0x30000;
#endif
#endif // ! CH571_CH573
	LL->CTRL_MOD = mode;
}

uint32_t RFEND_TXCTune(uint8_t channel) {
	// 0xbf = 2401 MHz
	RF->RF1 &= 0xfffffffe;
	RF->TXTUNE_CTRL = (RF->TXTUNE_CTRL & 0xfffe00ff) | (0xbf00 + (channel_map[channel] << 8));
	RF->RF1 |= 1;

	LL->TMR = 8000;
	while(!(RF->TXCTUNE_CO_CTRL & (1 << 25)) || !(RF->TXCTUNE_CO_CTRL & (1 << 26))) {
		if(LL->TMR == 0) {
			break;
		}
	}

	uint8_t nCO = (uint8_t)RF->TXCTUNE_CO_CTRL & 0x3f;
	uint8_t nGA = (uint8_t)(RF->TXCTUNE_GA_CTRL >> 10) & 0x7f;

	// printf("nCO,nGA ch:%u idx:%u %u,%u\n", channel, channel_map[channel], nCO,nGA);

	return (nGA << 24) | nCO;
}

void RFEND_TXTune() {
	RF->RF1 &= 0xfffffeff;
	RF->RF10 &= 0xffffefff;
	RF->RF11 &= 0xffffffef;
	RF->RF2 |= 0x20000;
	RF->RF1 |= 0x10;

	// 2401 MHz
	uint32_t tune2401 = RFEND_TXCTune(37);
	uint8_t nCO2401 = (uint8_t)(tune2401 & 0x3f);
	uint8_t nGA2401 = (uint8_t)(tune2401 >> 24) & 0x7f;

	// 2480 MHz
	uint32_t tune2480 = RFEND_TXCTune(39);
	uint8_t nCO2480 = (uint8_t)(tune2480 & 0x3f);
	uint8_t nGA2480 = (uint8_t)(tune2480 >> 24) & 0x7f;

	// 2440 MHz
	uint32_t tune2440 = RFEND_TXCTune(18);
	uint8_t nCO2440 = (uint8_t)(tune2440 & 0x3f);
	uint8_t nGA2440 = (uint8_t)(tune2440 >> 24) & 0x7f;

	uint32_t dCO0140 = nCO2401 - nCO2440;
	uint32_t dCO4080 = nCO2440 - nCO2480;
	uint8_t tune = 0;
	uint8_t int_points = sizeof(RF->TXCTUNE_CO) /2;
	uint8_t txctune_co[sizeof(RF->TXCTUNE_CO)] = {0};
	for(int f = 0; f < int_points; f++) {
		tune = (dCO0140 * (int_points -f)) / int_points;
		txctune_co[f] = tune | (tune << 4);
	}
	for(int f = int_points; f < sizeof(RF->TXCTUNE_CO); f++) {
		tune = (dCO4080 * (f -int_points)) / int_points;
		txctune_co[f] = tune | (tune << 4);
	}
	for(int i = 0; i < sizeof(txctune_co) /4; i++) {
		RF->TXCTUNE_CO[i] = ((uint32_t*)txctune_co)[i];
	}

	// This GA interpolating is not exactly what is done in EVT
	// Actually the reception on a BLE monitor is better when this is left out completely
	// This will need some proper experimentation by people with 2.4GHz SDRs
	uint32_t dGA0140 = nGA2401 - nGA2440;
	uint32_t dGA4080 = nGA2440 - nGA2480;
	int_points = sizeof(RF->TXCTUNE_GA) /2;
	uint8_t txctune_ga[sizeof(RF->TXCTUNE_GA)] = {0};
	for(int f = 1; f < int_points; f++) {
		tune = (dGA0140 * (int_points -f)) / int_points;
		txctune_ga[f] = tune | (tune << 4);
	}
	for(int f = int_points; f < sizeof(RF->TXCTUNE_GA) -1; f++) {
		tune = (dGA4080 * (f -int_points)) / int_points;
		txctune_ga[f] = tune | (tune << 4);
	}
	for(int i = 0; i < (sizeof(txctune_ga) /4); i++) {
		RF->TXCTUNE_GA[i] = ((uint32_t*)txctune_ga)[i];
	}


#if 0
	printf("2401 2440 2480 CO: %u %u %u, GA: %u %u %u\n", nCO2401, nCO2440, nCO2480, nGA2401, nGA2440, nGA2480);
	for(int i = 0; i < 10; i++ ) {
		printf( "%d: %08lx\n", i, RF->TXCTUNE_CO[i] );
	}
	for(int i = 0; i < 3; i++ ) {
		printf( "%d: %08lx\n", i, RF->TXCTUNE_GA[i] );
	}
#endif

	RF->RF1 &= 0xffffffef;
	RF->RF1 &= 0xfffffffe;
	RF->RF10 |= 0x1000;
	RF->RF11 |= 0x10;
	RF->TXTUNE_CTRL = (RF->TXTUNE_CTRL & 0xffffffc0) | (tune2440 & 0x3f);
	RF->TXTUNE_CTRL = (RF->TXTUNE_CTRL & 0x80ffffff) | (tune2440 & 0x7f000000);

	// FTune
	RF->RF1 |= 0x100;
}

void RFEND_RXTune() {
	RF->RF20 &= 0xfffeffff;
	RF->RF2 |= 0x200000;
	RF->RF3 = (RF->RF3 & 0xffffffef) | 0x10;
	RF->RF1 |= 0x1000;

	LL->TMR = 100;
	while(LL->TMR && ((RF->RXTUNE >> 8) & 1));

	tuneFilter = RF->RXTUNE & 0x1f;
	RF->RF20 |= 0x10000;
	RF->RF20 = (RF->RF20 & 0xffffffe0) | tuneFilter;
	RF->RF2 &= 0xffdfffff;
	tuneFilter2M = (tuneFilter +2 < 0x1f) ? (tuneFilter +2) : 0x1f;

	// RXADC
	RF->RF22 &= 0xfffeffff;
	RF->RF2 |= 0x10000;
	RF->RF3 = (RF->RF3 & 0xfffffeff) | 0x100;
	RF->RF1 = (RF->RF1 & 0xfffeffff) | 0x100000;
}

void RegInit() {
	DevSetMode(DEVSETMODE_TUNE);
	RFEND_TXTune();
	RFEND_RXTune();
	DevSetMode(0);
}

void RFCoreInit(uint8_t TxPower) {
#if defined(CH571_CH573) || defined(CH584_CH585) // maybe all?
	NVIC->IENR[0] = 0x1000;
	NVIC->IRER[0] = 0x1000;
#endif
	DevInit(TxPower);
	RegInit();
	NVIC->IPRIOR[0x15] |= 0x80;
	NVIC_EnableIRQ(LLE_IRQn);
}

void DevSetChannel(uint8_t channel) {
#ifdef CH571_CH573
	BB->BB6 = (BB->BB6 & 0xf8ffffff) | 0x4000000;
	BB->BB6 = (BB->BB6 & 0xffffff83) | 0x1c;
#endif
	RF->RF11 &= 0xfffffffd;
	BB->CTRL_CFG = (BB->CTRL_CFG & 0xffffff80) | (channel & 0x7f);
}

void Frame_TX(uint8_t adv[], size_t len, uint8_t channel, uint8_t phy_mode) {
	__attribute__((aligned(4))) uint8_t  ADV_BUF[len+2]; // for the advertisement, which is 37 bytes + 2 header bytes

	BB->CTRL_TX = (BB->CTRL_TX & 0xfffffffc) | 1;

	DevSetChannel(channel);

	// Uncomment to disable whitening to debug RF.
	//BB->CTRL_CFG |= (1<<6);
	DevSetMode(DEVSETMODE_TX);

	BB->ACCESSADDRESS1 = 0x8E89BED6; // access address
	BB->CRCINIT1 = 0x555555; // crc init
#ifdef CH570_CH572
	BB->ACCESSADDRESS2 = 0x8E89BED6;
	BB->CRCINIT2 = 0x555555;
	BB->CRCPOLY1 = (BB->CRCPOLY1 & 0xff000000) | 0x80032d; // crc poly
	BB->CRCPOLY2 = (BB->CRCPOLY2 & 0xff000000) | 0x80032d;
#endif
	// LL->LL1 = (LL->LL1 & 0xfffffffe) | 1; // The "| 1" is for AUTO mode, to swap between RX <-> TX when either happened

	ADV_BUF[0] = 0x02; // PDU 0x00, 0x02, 0x06 seem to work, with only 0x02 showing up on the phone
	ADV_BUF[1] = len ;
	memcpy(&ADV_BUF[2], adv, len);

#if defined(CH571_CH573)
	DMA->TXBUF = (uint32_t)ADV_BUF;
#else
	LL->FRAME_BUF = (uint32_t)ADV_BUF;
#endif

	// Wait for tuning bit to clear.
	for( int timeout = 3000; !(RF->RF26 & 0x1000000) && timeout >= 0; timeout-- );

#if defined(CH582_CH583) || defined(CH32V208)
	BB->CTRL_CFG = (phy_mode == PHY_2M) ? CTRL_CFG_PHY_2M:
				   (phy_mode == PHY_S2) ? CTRL_CFG_PHY_CODED:
				   (phy_mode == PHY_S8) ? CTRL_CFG_PHY_CODED:
										  CTRL_CFG_PHY_1M; // default 1M for now
	if(phy_mode > PHY_2M) { // coded phy
		BB->CTRL_CFG = (BB->CTRL_CFG & 0xffff3fff) | ((phy_mode == PHY_S2) ? 0x4000 : 0);
	}
#elif defined(CH571_CH573)
	BB->CTRL_CFG = CTRL_CFG_PHY_1M; // no 2M PHY on ch571/3
#else
	BB->CTRL_CFG = (phy_mode == PHY_2M) ? CTRL_CFG_PHY_2M:
										  CTRL_CFG_PHY_1M; // default 1M for now
#endif

#if defined(CH570_CH572)
	BB->BB9 = (BB->BB9 & 0xf9ffffff) | ((phy_mode == PHY_2M) ? 0 : 0x2000000);
#endif

#if defined(CH571_CH573)
	BB->BB11 = (BB->BB11 & 0xfffffffc); // |2 for RX
#endif

	// This clears bit 17 (If set, seems to have no impact.)
	LL->LL4 &= 0xfffdffff;

#if !defined(CH571_CH573)
	LL->STATUS = LL_STATUS_TX;
#endif
	LL->TMR = (uint32_t)(len *512); // needs optimisation, per phy mode

	BB->CTRL_CFG |= CTRL_CFG_START_TX;
	BB->CTRL_TX &= 0xfffffffc;

	LL->LL0 = 2; // Not sure what this does, but on RX it's 1

	while(LL->TMR); // wait for tx buffer to empty
	DevSetMode(0);
	if(LL->LL0 & 3) {
		LL->CTRL_MOD &= CTRL_MOD_RFSTOP;
		LL->LL0 |= 0x08;
	}
}

void Frame_RX(uint8_t frame_info[], uint8_t channel, uint8_t phy_mode) {
	DevSetMode(0);
	if(LL->LL0 & 3) {
		LL->CTRL_MOD &= CTRL_MOD_RFSTOP;
		LL->LL0 |= 0x08;
	}
	LL->TMR = 0;

	DevSetChannel(channel);
	DevSetMode(DEVSETMODE_RX);

#if defined(CH582_CH583) || defined(CH32V208)
	BB->CTRL_CFG = (phy_mode == PHY_2M) ? CTRL_CFG_PHY_2M:
				   (phy_mode == PHY_S2) ? CTRL_CFG_PHY_CODED:
				   (phy_mode == PHY_S8) ? CTRL_CFG_PHY_CODED:
										  CTRL_CFG_PHY_1M; // default 1M for now
	if(phy_mode > PHY_2M) { // coded phy
		BB->CTRL_CFG = (BB->CTRL_CFG & 0xffff3fff) | ((phy_mode == PHY_S2) ? 0x4000 : 0);
	}
#elif defined(CH571_CH573)
	BB->CTRL_CFG = CTRL_CFG_PHY_1M; // no 2M PHY on ch571/3
#else
	BB->CTRL_CFG = (phy_mode == PHY_2M) ? CTRL_CFG_PHY_2M:
										  CTRL_CFG_PHY_1M; // default 1M for now
#endif

#ifdef CH570_CH572
	BB->BB9 = (BB->BB9 & 0xf9ffffff) | ((phy_mode == PHY_2M) ? 0 : 0x2000000);
	RF->RF20 = (RF->RF20 & 0xffffffe0) | ((phy_mode == PHY_2M) ? (tuneFilter2M & 0x1f) : (tuneFilter & 0x1f));
	BB->BB5 = (BB->BB5 & 0xffffffc0) | ((phy_mode == PHY_2M) ? 0xd : 0xb);
	BB->BB7 = (BB->BB7 & 0xff00fc00) | ((phy_mode == PHY_2M) ? 0x7f00a0 : 0x79009c);
#elif defined(CH571_CH573)
	BB->BB11 = (BB->BB11 & 0xfffffffc) | 2; // no |2 for TX
#elif defined(CH582_CH583) || defined(CH32V208)
#if defined(CH582_CH583)
	BB->BB4 = (phy_mode < PHY_S2) ? 0x3722d0 : 0x3722df;
#elif defined(CH32V208)
	BB->BB4 = (phy_mode < PHY_S2) ? 0x3222d0 : 0x34a4df;
#endif
	BB->BB5 = (phy_mode < PHY_S2) ? 0x8101901 : 0x8301ff1;
	BB->BB6 = (phy_mode < PHY_S2) ? 0x31624 : 0x31619;
	BB->BB8 = (phy_mode < PHY_S2) ? 0x90083 : 0x90086;
	BB->BB9 = 0x1006310;
	BB->BB10 = (phy_mode < PHY_S2) ? 0x28be : 0x28de;
#elif defined(CH584_CH585) || defined(CH591_CH592)
	BB->BB6 = (BB->BB6 & 0xfffffc00) | ((phy_mode == PHY_2M) ? 0x13a : 0x132);
	BB->BB4 = (BB->BB4 & 0x00ffffff) | ((phy_mode == PHY_2M) ? 0x78000000 : 0x7f000000);
#endif

	BB->ACCESSADDRESS1 = 0x8E89BED6; // access address
	BB->CRCINIT1 = 0x555555; // crc init
#ifdef CH570_CH572
	BB->ACCESSADDRESS2 = 0x8E89BED6;
	BB->CRCINIT2 = 0x555555;
	BB->CRCPOLY1 = (BB->CRCPOLY1 & 0xff000000) | 0x80032d; // crc poly
	BB->CRCPOLY2 = (BB->CRCPOLY2 & 0xff000000) | 0x80032d;
#endif

	//LL->LL1 = (LL->LL1 & 0xfffffffe) | 1; // 1: AUTO mode, to swap between RX <-> TX when either happened. 0: BASIC
	//LL->FRAME_BUF = (uint32_t)frame_info; // also this only in AUTO mode

	LL->LL0 = 1; // Not sure what this does, but on TX it's 2
	rx_ready = 0;
}

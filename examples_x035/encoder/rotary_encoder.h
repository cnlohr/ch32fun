#pragma once
#include "ch32fun.h"
#include <stdint.h>

// Hardware quadrature-encoder reader built on a CH32X035 timer's "encoder
// mode" (RM 12.3.10), instead of polling pins and decoding in software.
//
// encoder_init() takes one of the encoder_pin_config_t constants below,
// naming one specific timer+pin-pair combination, and does everything
// needed to bring that timer up in encoder mode on those pins: clock
// enable, AFIO remap, GPIO setup, and the timer's own encoder-mode
// registers.

// Describes timer + CH1/CH2 pin-pair combination.
// Only TIM1 and TIM2 support "Encoder Mode".
typedef struct
{
	TIM_TypeDef *tim;
	volatile uint32_t *rcc_pcenr; // RCC->APB1PCENR or RCC->APB2PCENR
	volatile uint32_t *rcc_prstr; // RCC->APB1PRSTR or RCC->APB2PRSTR
	uint32_t rcc_mask; // this timer's bit in both of the above
	uint32_t afio_clear; // this timer's whole TIMx_RM field
	uint32_t afio_set; // ...and the specific remap value within it
	uint8_t pin_ch1;
	uint8_t pin_ch2;
} encoder_pin_config_t;

// Timer/pin combinations suitable for reading an encoder, as per RM 8.3.2.1.
// Only TIM1 and TIM2 support "encoder mode" on CH1 and CH2. In addition to
// the three possible configurations below, TIM2 would also allow (PB21, PB15),
// (PB16, PB17), (PC19, PA12), and (PC19, PC14). For each, at least one of the
// pins can't currently be addressed.
static const encoder_pin_config_t ENCODER_TIM1_PB9_PB10 = {
	.tim = TIM1,
	.rcc_pcenr = &RCC->APB2PCENR,
	.rcc_prstr = &RCC->APB2PRSTR,
	.rcc_mask = RCC_APB2Periph_TIM1,
	.afio_clear = AFIO_PCFR1_TIM1_REMAP,
	.afio_set = 0, // TIM1_RM = 000
	.pin_ch1 = PB9,
	.pin_ch2 = PB10,
};
static const encoder_pin_config_t ENCODER_TIM1_PC0_PC1 = {
	.tim = TIM1,
	.rcc_pcenr = &RCC->APB2PCENR,
	.rcc_prstr = &RCC->APB2PRSTR,
	.rcc_mask = RCC_APB2Periph_TIM1,
	.afio_clear = AFIO_PCFR1_TIM1_REMAP,
	.afio_set = AFIO_PCFR1_TIM1_REMAP_0 | AFIO_PCFR1_TIM1_REMAP_1, // TIM1_RM = 011
	.pin_ch1 = PC0,
	.pin_ch2 = PC1,
};
static const encoder_pin_config_t ENCODER_TIM2_PA0_PA1 = {
	.tim = TIM2,
	.rcc_pcenr = &RCC->APB1PCENR,
	.rcc_prstr = &RCC->APB1PRSTR,
	.rcc_mask = RCC_APB1Periph_TIM2,
	.afio_clear = AFIO_PCFR1_TIM2_REMAP,
	.afio_set = 0, // TIM2_RM = 000
	.pin_ch1 = PA0,
	.pin_ch2 = PA1,
};

typedef struct
{
	TIM_TypeDef *tim;
	uint16_t last_cnt;
} encoder_t;

RV_STATIC_INLINE void encoder_init( encoder_t *enc, encoder_pin_config_t cfg )
{
	TIM_TypeDef *tim = cfg.tim;

	*cfg.rcc_pcenr |= cfg.rcc_mask; // enable this timer's peripheral clock
	AFIO->PCFR1 = ( AFIO->PCFR1 & ~cfg.afio_clear ) | cfg.afio_set; // route CH1/CH2 onto the chosen pins
	*cfg.rcc_prstr |= cfg.rcc_mask; // Reset pulse
	*cfg.rcc_prstr &= ~cfg.rcc_mask;

	funPinMode( cfg.pin_ch1, GPIO_CFGLR_IN_PUPD ); // configure pins as inputs with pullup
	funPinMode( cfg.pin_ch2, GPIO_CFGLR_IN_PUPD );
	funDigitalWrite( cfg.pin_ch1, FUN_HIGH );
	funDigitalWrite( cfg.pin_ch2, FUN_HIGH );

	enc->tim = tim;

	tim->CHCTLR1 =
		TIM_CC1S_0 | TIM_CC2S_0 | TIM_IC1F | TIM_IC2F; // CC1S=CC2S=01 (map to TI1/TI2), IC1F=IC2F=0xF (max filter)
	tim->CCER = TIM_CC1E | TIM_CC2E; // enable capture channels 1 & 2

	tim->PSC = 0; // no prescaler
	tim->ATRLR = 0xFFFF; // free-run over the full 16-bit range
	tim->SMCFGR = TIM_EncoderMode_TI12; // SMS = 011: x4 decode on TI1 & TI2
	tim->SWEVGR = TIM_UG; // latch PSC/ATRLR, clears CNT to 0
	tim->CTLR1 |= TIM_CEN;

	enc->last_cnt = tim->CNT;
}

// Returns the delta since the last call. Doesn't need to be called in a loop, since the hardware
// tracks the state on its own.
RV_STATIC_INLINE int16_t encoder_get_delta( encoder_t *enc )
{
	uint16_t cnt = enc->tim->CNT;
	int16_t d = (int16_t)( cnt - enc->last_cnt );
	enc->last_cnt = cnt;
	return d;
}

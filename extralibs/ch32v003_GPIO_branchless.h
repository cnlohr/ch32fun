// 2023-06-26 recallmenot

//######## necessities

// include guards
#ifndef CH32V003_GPIO_BR_H
#define CH32V003_GPIO_BR_H

// includes
#include <stdint.h>								//uintN_t support
#include "../ch32fun/ch32fun.h"



/*######## library description
This is a speedy and light GPIO library due to
	static inlining of most functions
	compile-time abstraction
	branchless where it counts
*/



/*######## library usage and configuration

first, enable the desired port.

digital usage is quite Arduino-like:
pinMode
digitalWrite
digitalWrite_lo
digitalWrite_hi
digitalRead

pins are referenced as a single byte, ST-style:
the upper 4 bytes specify the port, where A is 0, C is 2 etc.
the lower 4 bytes specify the pin
both are used as uint4_t, not as 4 individual bits, allowing to address up to 16 pins on up to 16 ports.
this style of referencing a pin is called "GPIOv" in this library.

additionally, there are functions to operate an entire port at once
this can be useful where setting all pins one by one would be too inefficient / unnecessary
an example where this may be useful: https://www.youtube.com/watch?v=cy6o8TrDUFU
GPIO_port_digitalWrite
GPIO_port_digitalRead



analog-to-digital usage is almost Arduino-like:
pinMode
ADCinit
analogRead

By default, this library inserts a delay of 300 µs between configuration of the ADC input mux and the time the conversion starts.
This serves to counteract the high input impedance of the ADC, especially if it is increased by external resistors.
The input impedance of port A appears to be especially large.
You may modify it to your liking using the following define before including this library.
#define GPIO_ADC_MUX_DELAY 1200

GPIO_ADC_sampletime controls the time each conversion is granted, by default it is GPIO_ADC_sampletime_241cy_default, all options originate from the GPIO_ADC_sampletimes enum.
To alter it, you have 3 options:
 * `#define GPIO_ADC_sampletime GPIO_ADC_sampletime_43cy` before including this library
 * call the GPIO_ADC_set_sampletime function-like macro to momentarrily set it for one channel
 * call the GPIO_ADC_set_sampletimes_all function-like macro to to momentarrily set it for all channels

You may also disable the ADC to save power between infrequent measurements.



digital-to-analog (PWM) usage is quite different:
pinMode
GPIO_timX_map
GPIO_timX_init
GPIO_timX_enableCH
GPIO_timX_analogWrite

This is due to the fact that the CH32V003 has 2 timers, which each can be connected to 4 pre-defined sets (mappings) of pins.
Then you address the 4 channels of the timers, instead of the pins.

By default, the timers will be configured to count up to 2^10, which is 10 bits or 1024 discrete steps.
You may alter this to suit your needs, for example to an 8 bit resolution (256 discrete steps).
Insert this before including this library:
#define GPIO_timer_resolution (1 << 8)

By default, the timers will operate with a clock prescaler of 2 but you may choose 1 or 4 if you wish to alter the speed.
Insert this before including this library:
#define GPIO_timer_prescaler TIM_CKD_DIV1;		// APB_CLOCK / 1024 / 1 = 46.9kHz

You may calculate the base frequency of the timer (the rate of repetition of your signal) like follows:
fpwm = APB_CLOCK / resolution / prescaler
This puts the defaults at an inaudible 23.4kHz.
The higher the frequency, the greater the EMI radiation will be.
With low frequencies, say below 1000Hz, LEDs may exhibit perceivable flicker.

Since this library enables compare capture preload (OCxPE of CHCTLRy), writing a value into the compare register using analogWrite will automatically apply it (=load into shadow register) when the timer starts its next cycle.
This avoids a bug whereby writing a compare value lower than the current counter value, the output will glitch high for the next cycle, resulting in flickery updates.
Writing `TIMx->SWEVGR |= TIM_UG` will immediately update the shadow register and cause the same issue.

*/



//######## ports, pins and states: use these for the functions below!

#define GPIOv_from_PORT_PIN( GPIO_port_n, pin )

enum GPIO_port_n {
	GPIO_port_A = 0b00,
	GPIO_port_C = 0b10,
	GPIO_port_D = 0b11,
};

enum GPIO_pinModes {
	GPIO_pinMode_I_floating,
	GPIO_pinMode_I_pullUp,
	GPIO_pinMode_I_pullDown,
	GPIO_pinMode_I_analog,
	GPIO_pinMode_O_pushPull,
	GPIO_pinMode_O_openDrain,
	GPIO_pinMode_O_pushPullMux,
	GPIO_pinMode_O_openDrainMux,
};

enum lowhigh {
	low,
	high,
};

// analog inputs
enum GPIO_analog_inputs {
	GPIO_Ain0_A2,
	GPIO_Ain1_A1,
	GPIO_Ain2_C4,
	GPIO_Ain3_D2,
	GPIO_Ain4_D3,
	GPIO_Ain5_D5,
	GPIO_Ain6_D6,
	GPIO_Ain7_D4,
	GPIO_AinVref,
	GPIO_AinVcal,
};

// how many cycles the ADC shall sample the input for (speed vs precision)
enum GPIO_ADC_sampletimes {
	GPIO_ADC_sampletime_3cy,
	GPIO_ADC_sampletime_9cy,
	GPIO_ADC_sampletime_15cy,
	GPIO_ADC_sampletime_30cy,
	GPIO_ADC_sampletime_43cy,
	GPIO_ADC_sampletime_57cy,
	GPIO_ADC_sampletime_73cy,
	GPIO_ADC_sampletime_241cy_default,
};

enum GPIO_tim1_output_sets {
	GPIO_tim1_output_set_0__D2_A1_C3_C4__D0_A2_D1,
	GPIO_tim1_output_set_1__C6_C7_C0_D3__C3_C4_D1,
	GPIO_tim1_output_set_2__D2_A1_C3_C4__D0_A2_D1,
	GPIO_tim1_output_set_3__C4_C7_C5_D4__C3_D2_C6,
};

enum GPIO_tim2_output_sets {
	GPIO_tim2_output_set_0__D4_D3_C0_D7,
	GPIO_tim2_output_set_1__C5_C2_D2_C1,
	GPIO_tim2_output_set_2__C1_D3_C0_D7,
	GPIO_tim2_output_set_3__C1_C7_D6_D5,
};



//######## interface function overview: use these!
// most functions have been reduced to function-like macros, actual definitions downstairs

// setup
#define GPIO_port_enable(GPIO_port_n)
#define GPIO_pinMode(GPIOv, pinMode, GPIO_Speed)

// digital
#define GPIO_digitalWrite_hi(GPIOv)
#define GPIO_digitalWrite_lo(GPIOv)
#define GPIO_digitalWrite(GPIOv, lowhigh)
#define GPIO_digitalWrite_branching(GPIOv, lowhigh)
#define GPIO_digitalRead(GPIOv)
#define GPIO_port_digitalWrite(GPIO_port_n, byte)
#define GPIO_port_digitalRead(GPIO_port_n)

// analog to digital
static inline void GPIO_ADCinit();
#define GPIO_ADC_set_sampletime(GPIO_analog_input, GPIO_ADC_sampletime)
#define GPIO_ADC_set_sampletimes_all(GPIO_ADC_sampletime)
#define GPIO_ADC_set_power(enable)
#define GPIO_ADC_calibrate()
static inline uint16_t GPIO_analogRead(enum GPIO_analog_inputs input);

// digital to analog (PWM)
#define GPIO_tim1_map(GPIO_tim1_output_set)
#define GPIO_tim2_map(GPIO_tim2_output_set)
static inline void GPIO_tim1_init();
static inline void GPIO_tim2_init();
#define GPIO_tim1_enableCH(channel)
#define GPIO_tim2_enableCH(channel)
#define GPIO_tim1_analogWrite(channel, value)
#define GPIO_tim2_analogWrite(channel, value)



//######## internal function declarations



//######## internal variables



//######## preprocessor macros

#define CONCAT(a, b) a ## b
#define CONCAT_INDIRECT(a, b) CONCAT(a, b)

#undef GPIOv_from_PORT_PIN
#define GPIOv_from_PORT_PIN( GPIO_port_n, pin )		((GPIO_port_n << 4 ) | (pin))
#define GPIOv_to_PORT( GPIOv )				(GPIOv >> 4 )
#define GPIOv_to_PIN( GPIOv )				(GPIOv & 0b1111)
#define GPIOv_to_GPIObase( GPIOv )			((GPIO_TypeDef*)(uintptr_t)((GPIOA_BASE + (0x400 * (GPIOv >> 4)))))

#define GPIOx_to_port_n2(GPIOx)				GPIOx_to_port_n_##GPIOx
#define GPIOx_to_port_n(GPIOx)				GPIOx_to_port_n2(GPIOx)
#define GPIOx_to_port_n_GPIO_port_A			0b00
#define GPIOx_to_port_n_GPIO_port_C			0b10
#define GPIOx_to_port_n_GPIO_port_D			0b11

#define GPIO_port_n_to_GPIOx2(GPIO_port_n)		GPIO_port_n_to_GPIOx_##GPIO_port_n
#define GPIO_port_n_to_GPIOx(GPIO_port_n)		GPIO_port_n_to_GPIOx2(GPIO_port_n)
#define GPIO_port_n_to_GPIOx_GPIO_port_A		GPIOA
#define GPIO_port_n_to_GPIOx_GPIO_port_C		GPIOC
#define GPIO_port_n_to_GPIOx_GPIO_port_D		GPIOD

#define GPIO_port_n_to_RCC_APB2Periph2(GPIO_port_n)	GPIO_port_n_to_RCC_APB2Periph_##GPIO_port_n
#define GPIO_port_n_to_RCC_APB2Periph(GPIO_port_n)	GPIO_port_n_to_RCC_APB2Periph2(GPIO_port_n)
#define GPIO_port_n_to_RCC_APB2Periph_GPIO_port_A	RCC_APB2Periph_GPIOA
#define GPIO_port_n_to_RCC_APB2Periph_GPIO_port_C	RCC_APB2Periph_GPIOC
#define GPIO_port_n_to_RCC_APB2Periph_GPIO_port_D	RCC_APB2Periph_GPIOD

#define GPIO_pinMode_to_CFG2(GPIO_pinMode, GPIO_Speed)				GPIO_pinMode_to_CFG_##GPIO_pinMode(GPIO_Speed)
#define GPIO_pinMode_to_CFG(GPIO_pinMode, GPIO_Speed)				GPIO_pinMode_to_CFG2(GPIO_pinMode, GPIO_Speed)
#define GPIO_pinMode_to_CFG_GPIO_pinMode_I_floating(GPIO_Speed)			(GPIO_Speed_In	| GPIO_CNF_IN_FLOATING)
#define GPIO_pinMode_to_CFG_GPIO_pinMode_I_pullUp(GPIO_Speed)			(GPIO_Speed_In	| GPIO_CNF_IN_PUPD)
#define GPIO_pinMode_to_CFG_GPIO_pinMode_I_pullDown(GPIO_Speed)			(GPIO_Speed_In	| GPIO_CNF_IN_PUPD)
#define GPIO_pinMode_to_CFG_GPIO_pinMode_I_analog(GPIO_Speed)			(GPIO_Speed_In	| GPIO_CNF_IN_ANALOG)
#define GPIO_pinMode_to_CFG_GPIO_pinMode_O_pushPull(GPIO_Speed)			(GPIO_Speed	| GPIO_CNF_OUT_PP)
#define GPIO_pinMode_to_CFG_GPIO_pinMode_O_openDrain(GPIO_Speed)		(GPIO_Speed	| GPIO_CNF_OUT_OD)
#define GPIO_pinMode_to_CFG_GPIO_pinMode_O_pushPullMux(GPIO_Speed)		(GPIO_Speed	| GPIO_CNF_OUT_PP_AF)
#define GPIO_pinMode_to_CFG_GPIO_pinMode_O_openDrainMux(GPIO_Speed)		(GPIO_Speed	| GPIO_CNF_IN_ANALOG)

#define GPIO_pinMode_set_PUPD2(GPIO_pinMode, GPIOv)				GPIO_pinMode_set_PUPD_##GPIO_pinMode(GPIOv)
#define GPIO_pinMode_set_PUPD(GPIO_pinMode, GPIOv)				GPIO_pinMode_set_PUPD2(GPIO_pinMode, GPIOv)
#define GPIO_pinMode_set_PUPD_GPIO_pinMode_I_floating(GPIOv)
#define GPIO_pinMode_set_PUPD_GPIO_pinMode_I_pullUp(GPIOv)			GPIOv_to_GPIObase(GPIOv)->BSHR = (1 << GPIOv_to_PIN(GPIOv))
#define GPIO_pinMode_set_PUPD_GPIO_pinMode_I_pullDown(GPIOv)			GPIOv_to_GPIObase(GPIOv)->BSHR = (1 << (GPIOv_to_PIN(GPIOv) + 16))
#define GPIO_pinMode_set_PUPD_GPIO_pinMode_I_analog(GPIOv)
#define GPIO_pinMode_set_PUPD_GPIO_pinMode_O_pushPull(GPIOv)
#define GPIO_pinMode_set_PUPD_GPIO_pinMode_O_openDrain(GPIOv)
#define GPIO_pinMode_set_PUPD_GPIO_pinMode_O_pushPullMux(GPIOv)
#define GPIO_pinMode_set_PUPD_GPIO_pinMode_O_openDrainMux(GPIOv)

#define GPIO_port_pinMode_set_PUPD2(GPIO_pinMode, GPIO_port_n)			GPIO_port_pinMode_set_PUPD_##GPIO_pinMode(GPIO_port_n)
#define GPIO_port_pinMode_set_PUPD(GPIO_pinMode, GPIO_port_n)			GPIO_port_pinMode_set_PUPD2(GPIO_pinMode, GPIO_port_n)
#define GPIO_port_pinMode_set_PUPD_GPIO_pinMode_I_floating(GPIO_port_n)
#define GPIO_port_pinMode_set_PUPD_GPIO_pinMode_I_pullUp(GPIO_port_n)		GPIO_port_n_to_GPIOx(GPIO_port_n)->OUTDR = 0b11111111
#define GPIO_port_pinMode_set_PUPD_GPIO_pinMode_I_pullDown(GPIO_port_n)		GPIO_port_n_to_GPIOx(GPIO_port_n)->OUTDR = 0b00000000
#define GPIO_port_pinMode_set_PUPD_GPIO_pinMode_I_analog(GPIO_port_n)
#define GPIO_port_pinMode_set_PUPD_GPIO_pinMode_O_pushPull(GPIO_port_n)
#define GPIO_port_pinMode_set_PUPD_GPIO_pinMode_O_openDrain(GPIO_port_n)
#define GPIO_port_pinMode_set_PUPD_GPIO_pinMode_O_pushPullMux(GPIO_port_n)
#define GPIO_port_pinMode_set_PUPD_GPIO_pinMode_O_openDrainMux(GPIO_port_n)

#if !defined(GPIO_ADC_MUX_DELAY)
#define GPIO_ADC_MUX_DELAY 200
#endif

#if !defined(GPIO_ADC_sampletime)
#define GPIO_ADC_sampletime GPIO_ADC_sampletime_241cy_default
#endif

#if !defined(GPIO_timer_resolution)
#define GPIO_timer_resolution (1 << 10)
#endif

#if !defined(GPIO_timer_prescaler)
#define GPIO_timer_prescaler TIM_CKD_DIV2		// APB_CLOCK / 1024 / 2 = 23.4kHz
#endif

//######## define requirements / maintenance defines



//######## small function definitions, static inline


#undef GPIO_port_enable
#define GPIO_port_enable(GPIO_port_n) RCC->APB2PCENR |= GPIO_port_n_to_RCC_APB2Periph(GPIO_port_n);

#define GPIO_port_pinMode(GPIO_port_n, pinMode, GPIO_Speed) ({								\
	GPIO_port_n_to_GPIOx(GPIO_port_n)->CFGLR =	(GPIO_pinMode_to_CFG(pinMode, GPIO_Speed) << (4 * 0)) | 	\
							(GPIO_pinMode_to_CFG(pinMode, GPIO_Speed) << (4 * 1)) | 	\
							(GPIO_pinMode_to_CFG(pinMode, GPIO_Speed) << (4 * 2)) | 	\
							(GPIO_pinMode_to_CFG(pinMode, GPIO_Speed) << (4 * 3)) | 	\
							(GPIO_pinMode_to_CFG(pinMode, GPIO_Speed) << (4 * 4)) | 	\
							(GPIO_pinMode_to_CFG(pinMode, GPIO_Speed) << (4 * 5)) | 	\
							(GPIO_pinMode_to_CFG(pinMode, GPIO_Speed) << (4 * 6)) |		\
							(GPIO_pinMode_to_CFG(pinMode, GPIO_Speed) << (4 * 7));		\
	GPIO_port_pinMode_set_PUPD(pinMode, GPIO_port_n);								\
})

#undef GPIO_port_digitalWrite
#define GPIO_port_digitalWrite(GPIO_port_n, byte)	GPIO_port_n_to_GPIOx(GPIO_port_n)->OUTDR = byte

#undef GPIO_port_digitalRead
#define GPIO_port_digitalRead(GPIO_port_n)		(GPIO_port_n_to_GPIOx(GPIO_port_n)->INDR & 0b11111111)

#undef GPIO_pinMode
#define GPIO_pinMode(GPIOv, pinMode, GPIO_Speed) ({									\
	GPIOv_to_GPIObase(GPIOv)->CFGLR &= ~(0b1111 << (4 * GPIOv_to_PIN(GPIOv)));					\
	GPIOv_to_GPIObase(GPIOv)->CFGLR |= (GPIO_pinMode_to_CFG(pinMode, GPIO_Speed) << (4 * GPIOv_to_PIN(GPIOv)));	\
	GPIO_pinMode_set_PUPD(pinMode, GPIOv);										\
})

#undef GPIO_digitalWrite_hi
#define GPIO_digitalWrite_hi(GPIOv)					GPIOv_to_GPIObase(GPIOv)->BSHR = (1 << GPIOv_to_PIN(GPIOv))
#undef GPIO_digitalWrite_lo
#define GPIO_digitalWrite_lo(GPIOv)					GPIOv_to_GPIObase(GPIOv)->BSHR = (1 << (16 + GPIOv_to_PIN(GPIOv)))

#undef GPIO_digitalWrite
#define GPIO_digitalWrite(GPIOv, lowhigh)				GPIO_digitalWrite_##lowhigh(GPIOv)
#define GPIO_digitalWrite_low(GPIOv)					GPIO_digitalWrite_lo(GPIOv)
#define GPIO_digitalWrite_0(GPIOv)					GPIO_digitalWrite_lo(GPIOv)
#define GPIO_digitalWrite_high(GPIOv)					GPIO_digitalWrite_hi(GPIOv)
#define GPIO_digitalWrite_1(GPIOv)					GPIO_digitalWrite_hi(GPIOv)

#undef GPIO_digitalWrite_branching
#define GPIO_digitalWrite_branching(GPIOv, lowhigh)			(lowhigh ? (GPIO_digitalWrite_hi(GPIOv)) : (GPIO_digitalWrite_lo(GPIOv)))

#undef GPIO_digitalRead
#define GPIO_digitalRead(GPIOv)			 			((GPIOv_to_GPIObase(GPIOv)->INDR >> GPIOv_to_PIN(GPIOv)) & 0b1)

#undef GPIO_ADC_set_sampletime
// 0:7 => 3/9/15/30/43/57/73/241 cycles
#define GPIO_ADC_set_sampletime(GPIO_analog_input, GPIO_ADC_sampletime) ({	\
	ADC1->SAMPTR2 &= ~(0b111) << (3 * GPIO_analog_input);			\
	ADC1->SAMPTR2 |= GPIO_ADC_sampletime << (3 * GPIO_analog_input);	\
})

#undef GPIO_ADC_set_sampletimes_all
#define GPIO_ADC_set_sampletimes_all(GPIO_ADC_sampletime) ({	\
	ADC1->SAMPTR2 &= 0;					\
	ADC1->SAMPTR2 |=					\
			GPIO_ADC_sampletime << (0 * 3)		\
		|	GPIO_ADC_sampletime << (1 * 3)		\
		|	GPIO_ADC_sampletime << (2 * 3)		\
		|	GPIO_ADC_sampletime << (3 * 3)		\
		|	GPIO_ADC_sampletime << (4 * 3)		\
		|	GPIO_ADC_sampletime << (5 * 3)		\
		|	GPIO_ADC_sampletime << (6 * 3)		\
		|	GPIO_ADC_sampletime << (7 * 3)		\
		|	GPIO_ADC_sampletime << (8 * 3)		\
		|	GPIO_ADC_sampletime << (9 * 3);		\
	ADC1->SAMPTR1 &= 0;					\
	ADC1->SAMPTR1 |=					\
			GPIO_ADC_sampletime << (0 * 3)		\
		|	GPIO_ADC_sampletime << (1 * 3)		\
		|	GPIO_ADC_sampletime << (2 * 3)		\
		|	GPIO_ADC_sampletime << (3 * 3)		\
		|	GPIO_ADC_sampletime << (4 * 3)		\
		|	GPIO_ADC_sampletime << (5 * 3);		\
})

#undef GPIO_ADC_set_power
#define GPIO_ADC_set_power2(enable) GPIO_ADC_set_power_##enable
#define GPIO_ADC_set_power(enable) GPIO_ADC_set_power2(enable)
#define GPIO_ADC_set_power_1 ADC1->CTLR2 |= ADC_ADON
#define GPIO_ADC_set_power_0 ADC1->CTLR2 &= ~(ADC_ADON)

#undef GPIO_ADC_calibrate
#define GPIO_ADC_calibrate() ({			\
	ADC1->CTLR2 |= ADC_RSTCAL;		\
	while(ADC1->CTLR2 & ADC_RSTCAL);	\
	ADC1->CTLR2 |= ADC_CAL;			\
	while(ADC1->CTLR2 & ADC_CAL);		\
})

// large but will likely only ever be called once
static inline void GPIO_ADCinit() {
	// select ADC clock source
	// ADCCLK = 24 MHz => RCC_ADCPRE = 0: divide by 2
	RCC->CFGR0 &= ~(0x1F<<11);

	// enable clock to the ADC
	RCC->APB2PCENR |= RCC_APB2Periph_ADC1;

	// Reset the ADC to init all regs
	RCC->APB2PRSTR |= RCC_APB2Periph_ADC1;
	RCC->APB2PRSTR &= ~RCC_APB2Periph_ADC1;

	// set sampling time for all inputs to 241 cycles
	GPIO_ADC_set_sampletimes_all(GPIO_ADC_sampletime);

	// set trigger to software
	ADC1->CTLR2 |= ADC_EXTSEL;

	// pre-clear conversion queue
	ADC1->RSQR1 = 0;
	ADC1->RSQR2 = 0;
	ADC1->RSQR3 = 0;

	// power the ADC
	GPIO_ADC_set_power(1);
	GPIO_ADC_calibrate();
}

static inline uint16_t GPIO_analogRead(enum GPIO_analog_inputs input) {
	// set mux to selected input
	ADC1->RSQR3 = input;
	// allow everything to precharge
	Delay_Us(GPIO_ADC_MUX_DELAY);
	// start sw conversion (auto clears)
	ADC1->CTLR2 |= ADC_SWSTART;
	// wait for conversion complete
	while(!(ADC1->STATR & ADC_EOC)) {}
	// get result
	return ADC1->RDATAR;
}



#undef GPIO_tim1_map
#define GPIO_tim1_map(GPIO_tim1_output_set) ({					\
	RCC->APB2PCENR |= RCC_APB2Periph_AFIO;					\
	AFIO->PCFR1 |= ((GPIO_tim1_output_set & 0b11) << 6);			\
})

#undef GPIO_tim2_map
#define GPIO_tim2_map(GPIO_tim2_output_set) ({					\
	RCC->APB2PCENR |= RCC_APB2Periph_AFIO;					\
	AFIO->PCFR1 |= ((GPIO_tim2_output_set & 0b11) << 8);			\
})

static inline void GPIO_tim1_init() {
	// enable TIM1
	RCC->APB2PCENR |= RCC_APB2Periph_TIM1;
	// reset TIM1 to init all regs
	RCC->APB2PRSTR |= RCC_APB2Periph_TIM1;
	RCC->APB2PRSTR &= ~RCC_APB2Periph_TIM1;
	// SMCFGR: default clk input is CK_INT
	// set clock prescaler divider 
	TIM1->PSC = GPIO_timer_prescaler;
	// set PWM total cycle width
	TIM1->ATRLR = GPIO_timer_resolution;
	// CTLR1: default is up, events generated, edge align
	// enable auto-reload of preload
	TIM1->CTLR1 |= TIM_ARPE;
	// initialize counter
	TIM1->SWEVGR |= TIM_UG;
	// disengage brake
	TIM1->BDTR |= TIM_MOE;
	// Enable TIM1
	TIM1->CTLR1 |= TIM_CEN;
}
static inline void GPIO_tim2_init() {
	// enable TIM2
	RCC->APB1PCENR |= RCC_APB1Periph_TIM2;
	// reset TIM2 to init all regs
	RCC->APB1PRSTR |= RCC_APB1Periph_TIM2;
	RCC->APB1PRSTR &= ~RCC_APB1Periph_TIM2;
	// SMCFGR: default clk input is CK_INT
	// set clock prescaler divider 
	TIM2->PSC = GPIO_timer_prescaler;
	// set PWM total cycle width
	TIM2->ATRLR = GPIO_timer_resolution;
	// CTLR1: default is up, events generated, edge align
	// enable auto-reload of preload
	TIM2->CTLR1 |= TIM_ARPE;
	// initialize counter
	TIM2->SWEVGR |= TIM_UG;
	// Enable TIM2
	TIM2->CTLR1 |= TIM_CEN;
}

#define GPIO_timer_channel_set2(timer, channel)		GPIO_timer_channel_set_##channel(timer)
#define GPIO_timer_channel_set(timer, channel)		GPIO_timer_channel_set2(timer, channel)
#define GPIO_timer_channel_set_1(timer)			timer->CHCTLR1 |=  (TIM_OCMode_PWM1 | TIM_OCPreload_Enable)
#define GPIO_timer_channel_set_2(timer)			timer->CHCTLR1 |= ((TIM_OCMode_PWM1 | TIM_OCPreload_Enable) << 8)
#define GPIO_timer_channel_set_3(timer)			timer->CHCTLR2 |=  (TIM_OCMode_PWM1 | TIM_OCPreload_Enable)
#define GPIO_timer_channel_set_4(timer)			timer->CHCTLR2 |= ((TIM_OCMode_PWM1 | TIM_OCPreload_Enable) << 8)

#undef GPIO_tim1_enableCH
#define GPIO_tim1_enableCH(channel) ({						\
	GPIO_timer_channel_set(TIM1, channel);					\
	TIM1->CCER |= (TIM_OutputState_Enable) << (4 * (channel - 1));		\
})
#undef GPIO_tim2_enableCH
#define GPIO_tim2_enableCH(channel) ({						\
	GPIO_timer_channel_set(TIM2, channel);					\
	TIM2->CCER |= (TIM_OutputState_Enable ) << (4 * (channel - 1));		\
})

#define GPIO_timer_CVR(channel)				CONCAT_INDIRECT(CH, CONCAT_INDIRECT(channel, CVR))

#undef GPIO_tim1_analogWrite
#define GPIO_tim1_analogWrite(channel, value) 		TIM1->GPIO_timer_CVR(channel) = value;
#undef GPIO_tim2_analogWrite
#define GPIO_tim2_analogWrite(channel, value)		TIM2->GPIO_timer_CVR(channel) = value;

#endif // CH32V003_GPIO_BR_H

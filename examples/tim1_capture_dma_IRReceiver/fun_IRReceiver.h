#include "ch32fun.h"
#include <stdio.h>

// #define IR_RECEIVER_DEBUGLOG_ENABLE

// TIM2_CH1 -> PD2 -> DMA1_CH2
#define DMA_IN	DMA1_Channel2

// Limit to 64 events
#define TIM1_BUFFER_SIZE 64
#define TIM1_BUFFER_LAST_IDX (TIM1_BUFFER_SIZE - 1)

// For generating the mask/modulus, this must be a power of 2 size.
uint16_t ir_ticks_buff[TIM1_BUFFER_SIZE];

void fun_irReceiver_init(u8 irPin) {
	// Enable GPIOs
	RCC->APB2PCENR |= RCC_APB2Periph_TIM1 | RCC_APB2Periph_AFIO;
	RCC->AHBPCENR |= RCC_AHBPeriph_DMA1;

	// T1C1 -> What we are sampling.
	funPinMode(irPin, GPIO_CFGLR_IN_PUPD);
	funDigitalWrite(irPin, 1);

	//# TIM1_TRIG/TIM1_COM/TIM1_CH4
	DMA_IN->MADDR = (uint32_t)ir_ticks_buff;
	DMA_IN->PADDR = (uint32_t)&TIM1->CH1CVR; // Input
	DMA_IN->CFGR = 
		0					|				// PERIPHERAL to MEMORY
		0					|				// Low priority.
		DMA_CFGR1_MSIZE_0	|				// 16-bit memory
		DMA_CFGR1_PSIZE_0	|				// 16-bit peripheral
		DMA_CFGR1_MINC		|				// Increase memory.
		DMA_CFGR1_CIRC		|				// Circular mode.
		0					|				// NO Half-trigger
		0					|				// NO Whole-trigger
		DMA_CFGR1_EN;						// Enable
	DMA_IN->CNTR = TIM1_BUFFER_SIZE;

	TIM1->PSC = 0x01ff;		// set TIM1 clock prescaler divider (Massive prescaler)
	TIM1->ATRLR = 65535;	// set PWM total cycle width

	//# Tim 1 input / capture (CC1S = 01)
	TIM1->CHCTLR1 = TIM_CC1S_0;

	//# Add here CC1P to switch from UP->GOING to DOWN->GOING log times.
	TIM1->CCER = TIM_CC1E | TIM_CC1P;
	
	//# initialize counter
	TIM1->SWEVGR = TIM_UG;

	//# Enable TIM1
	TIM1->CTLR1 = TIM_ARPE | TIM_CEN;
	TIM1->DMAINTENR = TIM_CC1DE | TIM_UDE; // Enable DMA for T1CC1
}


u16 ir_data[4] = {0};
u32 ir_timeout;
u8 ir_started = 0;

int ir_tail = 0;
int ir_bits_processed = 0;

void fun_irReceiver_task(void(*handler)(u16, u16)) {
		// Must perform modulus here, in case DMA_IN->CNTR == 0.
	int head = (TIM1_BUFFER_SIZE - DMA_IN->CNTR) & TIM1_BUFFER_LAST_IDX;
	u32 now = millis();

	while( head != ir_tail ) {
		u32 time_of_event = ir_ticks_buff[ir_tail];
		u8 prev_event_idx = (ir_tail-1) & TIM1_BUFFER_LAST_IDX;		// modulus to loop back
		u32 prev_time_of_event = ir_ticks_buff[prev_event_idx];
		u32 time_dif = time_of_event - prev_time_of_event;

		#ifdef IR_RECEIVER_DEBUGLOG_ENABLE
			printf("%d (%d) \t\t [%d]%ld, [%d]%ld\n", time_dif, time_dif > 150,
				ir_tail, time_of_event, prev_event_idx, prev_time_of_event);
		#endif

		// Performs modulus to loop back.
		ir_tail = (ir_tail+1) & TIM1_BUFFER_LAST_IDX;

		//# start collecting ir data
		if (ir_started) {
			//# filter 2nd start frame (> 1000 ticks)
			if (time_dif > 1000) continue;
            int bit_pos = 15 - (ir_bits_processed & 0x0F);  // ir_bits_processed % 16
            int word_idx = ir_bits_processed >> 4;          // ir_bits_processed / 16

			//# filter for Logical HIGH
			// logical HIGH (~200 ticks), logical LOW (~100 ticks)
			if (time_dif > 150) {
				// MSB first (reversed)
				ir_data[word_idx] |= (1 << bit_pos);
			}

			ir_bits_processed++;	
		}

		//# 1st start frame. Expect > 1200 ticks
		else if (time_dif > 500) {
			ir_started = 1;
			ir_timeout = now;
		}
	}

	// 100ms timeout
	if (ir_started && ((now - ir_timeout) > 100)) {
		if (ir_bits_processed > 0) {
			#ifdef IR_RECEIVER_DEBUGLOG_ENABLE
				printf("\nbits processed: %d\n", ir_bits_processed);
				printf( "0x%04X 0x%04X 0x%04X 0x%04X\n", ir_data[0], ir_data[1], ir_data[2], ir_data[3] );
				printf("CLEARED\n\n");
			#endif

			handler(ir_data[0], ir_data[1]);
		}

		//# clear out ir data
		memset(ir_data, 0, sizeof(ir_data));
		ir_started = 0;
		ir_bits_processed = 0;
	}
}




#define IR_USE_TIM1_PWM

// carrier frequency = 38kHz
// period = 1/38000 = 26.5µs
// half period = 26.5µs / 2 = ~13µs
#define IR_HALF_PERIOD_US 13

u8 irSender_pin;

void fun_irSender_init(u8 pin) {
	irSender_pin = pin;
	funPinMode(irSender_pin, GPIO_CFGLR_OUT_50Mhz_PP);

	// Reset TIM1 to init all regs
	RCC->APB2PRSTR |= RCC_APB2Periph_TIM1;
	RCC->APB2PRSTR &= ~RCC_APB2Periph_TIM1;

	// Prescaler 1+1=2
	TIM1->PSC = 1;
	
	// timer_period = system_clock/carrier_frequency = 48Mhz/38kHz = ~1263.1579
	TIM1->ATRLR = 631;		// 50% duty cycle = 1263/2 = 631.5
	TIM2->CH2CVR = 316;     // width 50% duty cycle

	// Reload immediately
	TIM1->SWEVGR |= TIM_UG;

	// CH1 Mode is output, PWM1 (CC1S = 00, OC1M = 110)
	TIM1->CHCTLR1 |= TIM_OC1M_2 | TIM_OC1M_1;

	// Enable TIM1 outputs
	TIM1->BDTR |= TIM_MOE;
	
	// Enable TIM1
	TIM1->CTLR1 |= TIM_CEN;
}

void IR_send_carrier_pulse(u32 duration_us, u32 space_us) {
	#ifdef IR_USE_TIM1_PWM
		u32 cycles = duration_us / (IR_HALF_PERIOD_US * 2);
		
		for(u32 i = 0; i < cycles; i++) {
			funDigitalWrite(irSender_pin, 1);  	// Set high
			Delay_Us(IR_HALF_PERIOD_US);
			funDigitalWrite(irSender_pin, 0);   // Set low
			Delay_Us(IR_HALF_PERIOD_US);
		}

		// Ensure pin is low during space
		funDigitalWrite(irSender_pin, 0);
	#else
		// Start CH1N output, positive pol
		TIM1->CCER |= TIM_CC1NE | TIM_CC1NP;
		Delay_Us(duration_us);

		// Stop CH1N output
		TIM1->CCER &= ~(TIM_CC1NE | TIM_CC1NP);
	#endif
	
    Delay_Us(space_us);
}

void fun_irSend_NECData(u16 data) {
	for (int i = 15; i >= 0; i--) {
		u8 bit = (data >> i) & 1;		// MSB first
		u32 space = bit ? 1600 : 510;
		IR_send_carrier_pulse(510, space);
	}
}

u16 address = 0x00FF;
u16 command = 0xA56D;

void fun_irSender_send() {
	IR_send_carrier_pulse(9000, 4500);

	fun_irSend_NECData(address);
	fun_irSend_NECData(command);

    // Stop bit
    IR_send_carrier_pulse(510, 1000);
}
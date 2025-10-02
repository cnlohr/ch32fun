#include "ch32fun.h"
#include <stdio.h>
#include "fun_crc.h"

#define IR_RECEIVER_DEBUGLOG_ENABLE

// TIM2_CH1 -> PD2 -> DMA1_CH2
#define DMA_IN	DMA1_Channel2

// Limit to 64 events
#define TIM1_BUFFER_SIZE 64
#define TIM1_BUFFER_LAST_IDX (TIM1_BUFFER_SIZE - 1)

#define IR_LOGICAL_HIGH_THRESHOLD 300

//! ####################################
//! SETUP FUNCTION
//! ####################################

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

	TIM1->PSC = 0x00ff;		// set TIM1 clock prescaler divider (Massive prescaler)
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


//! ####################################
//! RECEIVE FUNCTION
//! ####################################

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
			if (time_dif > IR_LOGICAL_HIGH_THRESHOLD) {
				ir_data[word_idx] |= (1 << bit_pos);		// MSB first (reversed)
			}

			#ifdef IR_RECEIVER_DEBUGLOG_ENABLE
				printf("%d (%d) - 0x%04X \t [%d]%ld, [%d]%ld D%d\n",
					time_dif, time_dif > IR_LOGICAL_HIGH_THRESHOLD,
					ir_data[word_idx],
					ir_tail, time_of_event, prev_event_idx, prev_time_of_event, bit_pos);
				if (bit_pos % 8 == 0) printf("\n");
			#endif

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
				// printf("\nbits processed: %d\n", ir_bits_processed);
				printf( "0x%04X 0x%04X 0x%04X 0x%04X\n",
					ir_data[0], ir_data[1], ir_data[2], ir_data[3] );

				u64 combined = combine_64(ir_data[0], ir_data[1], ir_data[2], ir_data[3]);
				u8 check = crch16_ccitt_check64(combined, &combined);
				printf("check: %d\n", check);
			#endif

			handler(ir_data[0], ir_data[1]);
		}

		//# clear out ir data
		memset(ir_data, 0, sizeof(ir_data));
		ir_started = 0;
		ir_bits_processed = 0;
	}
}
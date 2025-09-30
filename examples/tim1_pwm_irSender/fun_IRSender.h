#include "ch32fun.h"
#include <stdio.h>

// #define IR_USE_TIM1_PWM

#define NEC_PULSE_WIDTH_US 560
#define NEC_LOGIC_1_WIDTH_US 1680
#define NEC_LOGIC_0_WIDTH_US 560

//# Timer 1 pin mappings by AFIO->PCFR1
/*  00	AFIO_PCFR1_TIM1_REMAP_NOREMAP
		(ETR/PC5, BKIN/PC2)
		CH1/CH1N PD2/PD0
		CH2/CH2N PA1/PA2
		CH3/CH3N PC3/PD1	//! PD1 SWIO
		CH4 PC4
	01	AFIO_PCFR1_TIM1_REMAP_PARTIALREMAP1
		(ETR/PA12, CH1/PA8, CH2/PA9, CH3/PA10, CH4/PA11, BKIN/PA6, CH1N/PA7, CH2N/PB0, CH3N/PB1)
		CH1/CH1N PC6/PC3	//! PC6 SPI-MOSI
		CH2/CH2N PC7/PC4	//! PC7 SPI-MISO
		CH3/CH3N PC0/PD1	//! PD1 SWIO
		CH4 PD3
	10	AFIO_PCFR1_TIM1_REMAP_PARTIALREMAP2
		(ETR/PD4, CH1/PD2, CH2/PA1, CH3/PC3, CH4/PC4, BKIN/PC2, CH1N/PD0, CN2N/PA2, CH3N/PD1)
		CH1/CH1N PD2/PD0
		CH2/CH2N PA1/PA2
		CH3/CH3N PC3/PD1	//! PD1 SWIO
		CH4 PC4
	11	AFIO_PCFR1_TIM1_REMAP_FULLREMAP
		(ETR/PE7, CH1/PE9, CH2/PE11, CH3/PE13, CH4/PE14, BKIN/PE15, CH1N/PE8, CH2N/PE10, CH3N/PE12)
		CH1/CH1N PC4/PC3	
		CH2/CH2N PC7/PD2	//! PC7 SPI-MISO
		CH3/CH3N PC5/PC6	//! PC5 SPI-SCK, PC6 SPI-MOSI
		CH4 PD4?
*/


//! ####################################
//! SETUP FUNCTION
//! ####################################

u8 irSender_pin;

u8 fun_irSender_init(u8 pin) {
	irSender_pin = pin;

	#ifdef IR_USE_TIM1_PWM
		funPinMode(irSender_pin, GPIO_Speed_10MHz | GPIO_CNF_OUT_PP_AF);

		// Enable TIM1
		RCC->APB2PCENR |= RCC_APB2Periph_TIM1;
			
		// Reset TIM1 to init all regs
		RCC->APB2PRSTR |= RCC_APB2Periph_TIM1;
		RCC->APB2PRSTR &= ~RCC_APB2Periph_TIM1;
		
		// PWM settings
		TIM1->PSC = 0x0000;			// Prescaler 
		TIM1->ATRLR = 1263;			// PWM period: 48Mhz/38kHz = 1263
		TIM1->CH1CVR = 632;			// PWM duty cycle: 50% = 1263/2
		TIM1->SWEVGR |= TIM_UG;		// Reload immediately

		// CH1 Mode is output, PWM1 (CC1S = 00, OC1M = 110)
		TIM1->CHCTLR1 |= TIM_OC1M_2 | TIM_OC1M_1;
		
		TIM1->BDTR |= TIM_MOE;		// Enable TIM1 outputs
		TIM1->CTLR1 |= TIM_CEN;		// Enable TIM1

		return 1;
	#else
		funPinMode(irSender_pin, GPIO_CFGLR_OUT_10Mhz_PP);
		return 0;
	#endif
}


//! ####################################
//! TRANSMIT FUNCTIONS
//! ####################################

// carrier frequency = 38kHz
// period = 1/38000 = 26.5µs
// half period = 26.5µs / 2 = ~13µs
#define IR_CARRIER_HALF_PERIOD_US 13
#define IR_CARRIER_CYCLES(duration_us) duration_us / (IR_CARRIER_HALF_PERIOD_US * 2)

void IR_send_carrier_pulse(u32 duration_us, u32 space_us) {
	#ifdef IR_USE_TIM1_PWM
		//# Start CH1N output
		TIM1->CCER |= TIM_CC1NE | TIM_CC1NP;
		Delay_Us(duration_us);

		//# Stop CH1N output
		TIM1->CCER &= ~(TIM_CC1NE | TIM_CC1NP);
	#else
		for(u32 i = 0; i < IR_CARRIER_CYCLES(duration_us); i++) {
			funDigitalWrite(irSender_pin, 1);  	// Set high
			Delay_Us( IR_CARRIER_HALF_PERIOD_US );
			funDigitalWrite(irSender_pin, 0);   // Set low
			Delay_Us( IR_CARRIER_HALF_PERIOD_US );
		}

		// Ensure pin is low during space
		funDigitalWrite(irSender_pin, 0);
	#endif
	
	Delay_Us(space_us);
}

void fun_irSend_NECData(u16 data) {
	for (int i = 15; i >= 0; i--) {
		u8 bit = (data >> i) & 1;		// MSB first
		u32 space = bit ? NEC_LOGIC_1_WIDTH_US : NEC_LOGIC_0_WIDTH_US;
		IR_send_carrier_pulse(NEC_PULSE_WIDTH_US, space);
	}
}

u32 sending_data = 0x00FFA56D;

void fun_irSender_send(u16 address, u16 command) {
	IR_send_carrier_pulse(9000, 4500);

	fun_irSend_NECData(address);
	fun_irSend_NECData(command);

	// Stop bit
	IR_send_carrier_pulse(NEC_PULSE_WIDTH_US, 1000);
}


//! ####################################
//! ASYNC TRANSMIT FUNCTIONS
//! ####################################

typedef enum {
	IRSender_Start_Pulse,
	IRSender_Start_Pulse_Space,
	IRSender_Data_Pulse,
	IRSender_Idle
} IRSender_State_t;

typedef struct {
	int pin;
	int state;
	int duty_state;						// toggle to simulate PWM
	u32 timeRef;
	int remaining_carrier_cycles;
	int remaining_data_bits;
	int transmitting_bit;				// MSB first
	u16 logical_spacing;
} IRSender_Model_t;

IRSender_Model_t irSenderM;

//# cycle pulse: send the pulse and toggle it for the next cycle
void irSender_cycle_pulse(u32 time_us, u8 value) {
	// prepare for next cycle
	funDigitalWrite(irSender_pin, value);
	irSenderM.duty_state = !value;			// toggle
	irSenderM.remaining_carrier_cycles--;	// update cycle
	irSenderM.timeRef = time_us;			// update time	
}

void fun_irSender_sendAsync(u16 address, u16 command) {
	//! start the pulses
	irSenderM.state = IRSender_Start_Pulse;
	irSenderM.remaining_carrier_cycles = 9000 / IR_CARRIER_HALF_PERIOD_US;
	irSender_cycle_pulse(micros(), 1);
}

void fun_irSender_task() {
	if (irSenderM.state == IRSender_Idle) return;
	u32 time_us = micros();
	u32 time_changed = time_us - irSenderM.timeRef;

	//# send the Start_Pulses - 9000us
	if ((irSenderM.state == IRSender_Start_Pulse) && (time_changed > IR_CARRIER_HALF_PERIOD_US)) {
		// calculated cycles count for 9000us
		if (irSenderM.remaining_carrier_cycles == 0) {
			//! end the Start_Pulses - make sure OFF for next stage
			irSenderM.state = IRSender_Start_Pulse_Space;
			funDigitalWrite(irSender_pin, 0);
		} else {
			irSender_cycle_pulse(time_us, irSenderM.duty_state);
		}
	}

	//# wait for Start_Pulses space - 4500us
	else if ((irSenderM.state == IRSender_Start_Pulse_Space) && (time_changed > 4500)) {
		//! start the pulses for next stage
		irSenderM.state = IRSender_Data_Pulse;
		irSender_cycle_pulse(time_us, 1);

		irSenderM.remaining_carrier_cycles = NEC_PULSE_WIDTH_US / IR_CARRIER_HALF_PERIOD_US;
		irSenderM.remaining_data_bits = 31;		// update remaining data bits
		irSenderM.logical_spacing = 0;
	}

	//# handle the Data_Pulses
	else if (irSenderM.state == IRSender_Data_Pulse) {
		// wait for spacing if any
		if ((irSenderM.logical_spacing > 0) && (time_changed > irSenderM.logical_spacing)) {
			if (irSenderM.remaining_carrier_cycles == 0) {
				//! end the Data_Pulses - make sure OFF
				irSenderM.state = IRSender_Idle;
				funDigitalWrite(irSender_pin, 0);
			} else {
				irSender_cycle_pulse(time_us, 1);
				irSenderM.remaining_carrier_cycles = NEC_PULSE_WIDTH_US / IR_CARRIER_HALF_PERIOD_US;
				irSenderM.remaining_data_bits--;	// update remaining data bits
				irSenderM.logical_spacing = 0;
			}
		}
		else if (time_changed > IR_CARRIER_HALF_PERIOD_US) {
			irSender_cycle_pulse(time_us, irSenderM.duty_state);

			if (irSenderM.remaining_carrier_cycles == 0) {
				// determine the data bit and the spacing for it. MSB first
				u8 bit = (sending_data >> irSenderM.remaining_data_bits) & 1;
				irSenderM.logical_spacing = bit ? NEC_LOGIC_1_WIDTH_US : NEC_LOGIC_0_WIDTH_US;
				//! make sure off for spacing
				funDigitalWrite(irSender_pin, 0);
			}
		}
	}
}

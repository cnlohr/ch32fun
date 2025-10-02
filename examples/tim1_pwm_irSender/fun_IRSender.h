#include "ch32fun.h"
#include <stdio.h>
#include "fun_crc.h"

#define IR_USE_TIM1_PWM

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

// Enable/disable PWM carrier
static inline void PWM_ON(void)  { TIM1->CCER |=  TIM_CC1NE; }
static inline void PWM_OFF(void) { TIM1->CCER &= ~TIM_CC1NE; }

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

		TIM1->CCER |= TIM_CC1NP;
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

void _IR_carrier_pulse(u32 duration_us, u32 space_us) {
	#ifdef IR_USE_TIM1_PWM
		//# Start CH1N output
		PWM_ON();
		Delay_Us(duration_us);

		//# Stop CH1N output
		PWM_OFF();
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
		_IR_carrier_pulse(NEC_PULSE_WIDTH_US, space);
	}
}

void fun_irSender_send(u16 address, u16 command, u16 extra) {
	u64 combined = combine_64(address, command, extra, 0x00);
	u16 crc = crc16_ccitt_compute(combined);

	u64 dataout = 0;
	u64 combined2 = combine_64(address, command, extra, crc);
	u8 check = crc16_ccitt_check64(combined2, &dataout);

	printf("check crc 0x%04X: %d\r\n", crc, check);
	printf("Data: 0x%08X%08X\n", 
		(uint32_t)(combined2 >> 32), 
		(uint32_t)(combined2 & 0xFFFFFFFF));

	_IR_carrier_pulse(9000, 4500);

	fun_irSend_NECData(address);
	fun_irSend_NECData(command);
	fun_irSend_NECData(extra);
	fun_irSend_NECData(crc);

	// Stop bit
	_IR_carrier_pulse(NEC_PULSE_WIDTH_US, 1000);
}


//! ####################################
//! ASYNC TRANSMIT FUNCTIONS
//! ####################################


typedef enum {
	IR_Start_Pulse,
	IR_Start_Pulse_Burst,
	IR_Start_Pulse_Space,
	IR_Data_Pulse,
	IR_Idle
} IRSender_State_t;

typedef struct {
	int pin;
	int state;
	int duty_state;						// toggle to simulate PWM
	u32 timeRef;
	int remaining_data_bits;
	u16 logical_spacing;
} IRSender_Model_t;

IRSender_Model_t irSenderM;

u32 sending_data = 0x00FFA56D;

void fun_irSender_sendAsync(u16 address, u16 command) {
	//! start the pulses
	irSenderM.state = IR_Start_Pulse;
	funDigitalWrite(irSender_pin, 0);
	irSenderM.timeRef = micros();
	printf("\nSending Data\r\n");
}

void fun_irSender_task() {
	if (irSenderM.state == IR_Idle) return;
	u32 now = micros();
	u32 elapsed = now - irSenderM.timeRef;

	switch (irSenderM.state) {
		case IR_Data_Pulse:
			// Handle spacing first
			// if (irSenderM.logical_spacing > 0) {
			// 	if (elapsed > irSenderM.logical_spacing) {
			// 		irSenderM.logical_spacing = 0;
			// 		irSenderM.timeRef = now;
			// 	}
			// 	break;
			// }

			// If no spacing, send next pulse
			if (irSenderM.remaining_data_bits > 0) {
				PWM_ON();
				Delay_Us(NEC_PULSE_WIDTH_US);
				PWM_OFF();

				u8 bit = (sending_data >> (irSenderM.remaining_data_bits-1)) & 1;
				
				//! This makes no sense whatsoever: logic 0 suppose to have spacing of
				//! NEC_LOGIC_0_WIDTH_US which is 560us but the code would just ignore
				//! the timeout of 560us. it works for ~1000 :|
				u16 nec_logic_0_spacing = 1100;
				irSenderM.logical_spacing = bit ? NEC_LOGIC_1_WIDTH_US : NEC_LOGIC_0_WIDTH_US;
				irSenderM.remaining_data_bits--;
				irSenderM.timeRef = now;
				Delay_Us(irSenderM.logical_spacing);
			}
			// Only end when both conditions are met
			else {
				printf("\nEnd the Data_Pulses\r\n");
				irSenderM.state = IR_Idle;
				funDigitalWrite(irSender_pin, 0);
			}
			break;

		case IR_Start_Pulse:
			//# Start CH1N output
			PWM_ON();
			irSenderM.state = IR_Start_Pulse_Burst;
			irSenderM.timeRef = now;
			break;
		
		case IR_Start_Pulse_Burst:
			//# send the Start_Pulses - 9000us
			if (elapsed > 9000) {
				//# Stop CH1N output
				PWM_OFF();
				irSenderM.state = IR_Start_Pulse_Space;
				irSenderM.timeRef = now;
			}
			break;

		case IR_Start_Pulse_Space:
			//# wait for Start_Pulses space - 4500us
			if (elapsed > 4500) {
				irSenderM.state = IR_Data_Pulse;
				irSenderM.timeRef = now;
				irSenderM.remaining_data_bits = 32;		// update remaining data bits
				irSenderM.logical_spacing = 0;
			}
			break;

	}
}

#include "ch32fun.h"
#include <stdio.h>

// Connect motor to PD5, PD6, and PC7.

// This is actually 1/F_CPU per PWM period advance.
uint32_t target_rotor_speed = 3500;

// Higher values here use longer but more accurate PWM periods.
#define PWM_PERIOD_PO2 5

void handle_debug_input( int numbytes, uint8_t * data )
{
	if( numbytes > 0 )
	{
		int targspeed = data[numbytes-1] - '0';
		if( targspeed >= 0 && targspeed <= 9 )
		{
			// Kinda a random target speed, but making 0..9 make sense.
			target_rotor_speed = (665600>>PWM_PERIOD_PO2) / (targspeed+1);
		}
	}
}

void adc_init()
{
	RCC->APB2PCENR |= RCC_APB2Periph_ADC1;

	RCC->APB2PRSTR |= RCC_APB2Periph_ADC1;
	RCC->APB2PRSTR &= ~RCC_APB2Periph_ADC1;

	// ADCCLK = 24 MHz => RCC_ADCPRE divide by 2
	RCC->CFGR0 &= ~RCC_ADCPRE;  // Clear out the bis in case they were set
	RCC->CFGR0 |= RCC_ADCPRE_DIV2;

	// Set up single conversion on chl 7
	ADC1->RSQR1 = 0;
	ADC1->RSQR2 = 0;
	ADC1->RSQR3 = 6;	// 0-9 for 8 ext inputs and two internals

	ADC1->SAMPTR2 = (1<<(3*6));

	// Turn on ADC and set rule group to sw trig
	// 3 = Use TRGO event for Timer 2 to fire ADC rule.
	ADC1->CTLR2 = ADC_ADON | ADC_EXTTRIG | ( ADC_EXTSEL_0 | ADC_EXTSEL_1 ); 

	// Reset calibration
	ADC1->CTLR2 |= ADC_RSTCAL;
	while(ADC1->CTLR2 & ADC_RSTCAL);

	// Calibrate ADC
	ADC1->CTLR2 |= ADC_CAL;
	while(ADC1->CTLR2 & ADC_CAL);

	ADC1->CTLR1 = ADC_SCAN;
}

void t2pwm_init( void )
{
	// Enable GPIOD and TIM2
	RCC->APB2PCENR |= RCC_APB2Periph_GPIOD |
						RCC_APB2Periph_GPIOC |
						RCC_APB2Periph_AFIO;
	RCC->APB1PCENR |= RCC_APB1Periph_TIM2;

	funPinMode( PD5, GPIO_CFGLR_OUT_2Mhz_AF_PP );
	funPinMode( PD6, GPIO_CFGLR_OUT_2Mhz_AF_PP );
	funPinMode( PC7, GPIO_CFGLR_OUT_2Mhz_AF_PP );

	/*
		Timer 2 pin mappings by AFIO->PCFR1
			00	AFIO_PCFR1_TIM2_REMAP_NOREMAP
				D4		T2CH1ETR
				D3		T2CH2
				C0		T2CH3
				D7		T2CH4  --note: requires disabling nRST in opt
			01	AFIO_PCFR1_TIM2_REMAP_PARTIALREMAP1
				C5		T2CH1ETR_
				C2		T2CH2_
				D2		T2CH3_
				C1		T2CH4_
			10	AFIO_PCFR1_TIM2_REMAP_PARTIALREMAP2
				C1		T2CH1ETR_
				D3		T2CH2
				C0		T2CH3
				D7		T2CH4  --note: requires disabling nRST in opt
			11	AFIO_PCFR1_TIM2_REMAP_FULLREMAP
				C1		T2CH1ETR_
				C7		T2CH2_
				D6		T2CH3_
				D5		T2CH4_
	*/
	AFIO->PCFR1 = AFIO_PCFR1_TIM2_REMAP_FULLREMAP;

	// Reset TIM2 to init all regs
	RCC->APB1PRSTR |= RCC_APB1Periph_TIM2;
	RCC->APB1PRSTR &= ~RCC_APB1Periph_TIM2;
	
	// SMCFGR: default clk input is CK_INT
	// set TIM2 clock prescaler divider 
	TIM2->PSC = 0x0000;
	// set PWM total cycle width
	TIM2->ATRLR = (1<<PWM_PERIOD_PO2)-1;
	
	// for channel 1 and 2, let CCxS stay 00 (output), set OCxM to 110 (PWM I)
	// enabling preload causes the new pulse width in compare capture register only to come into effect when UG bit in SWEVGR is set (= initiate update) (auto-clears)
	TIM2->CHCTLR1 |= TIM_OC2M_2 | TIM_OC2M_1 | TIM_OC2PE;
	TIM2->CHCTLR2 |= TIM_OC3M_2 | TIM_OC3M_1 | TIM_OC3PE | TIM_OC4M_2 | TIM_OC4M_1 | TIM_OC4PE;

	// CTLR1: default is up, events generated, edge align
	// enable auto-reload of preload
	TIM2->CTLR1 |= TIM_ARPE;

	TIM2->CCER |= TIM_CC2E | (TIM_CC2P );
	TIM2->CCER |= TIM_CC3E | (TIM_CC3P );
	TIM2->CCER |= TIM_CC4E | (TIM_CC4P );

	// initialize counter
	TIM2->SWEVGR |= TIM_UG;

	// Setup TRGO for ADC. We want to synchronize the ADC with this timer.
	TIM2->CTLR2 = TIM_MMS_1;

	// Enable TIM2
	TIM2->CTLR1 |= TIM_CEN;
}

//  /‾‾\__...
//  ‾\__/‾...
//  __/‾‾\...
static int trapezoid( int val )
{
	if( val >= ((1<<PWM_PERIOD_PO2)*6) )
	{
		val -= ((1<<PWM_PERIOD_PO2)*6);
	}
	if( val < ((1<<PWM_PERIOD_PO2)*3) )
	{
		if( val < ((1<<PWM_PERIOD_PO2)) )
		{
			return val;
		}
		else
		{
			return ((1<<PWM_PERIOD_PO2))-1;
		}
	}
	else
	{
		if( val < ((1<<PWM_PERIOD_PO2)*4) )
			return ((1<<PWM_PERIOD_PO2)*4) - 1 - val;
		else
			return 0;
	}
}


volatile int g_advance_speed = 1000000;
int adcinflection[6]; // In case we want to monitor them.
int last_spin_cancel;
int last_spinningness;

void SysTick_Handler(void) __attribute__((interrupt));
void SysTick_Handler(void)
{
	static int position;
	static int updchecknext;
	static int updchecknext_id;

	uint32_t newComp;
	do
	{
		newComp = SysTick->CMP + g_advance_speed;
		SysTick->CMP = newComp;
		SysTick->SR = 0;

		// 3 phases, each 120 degrees out of phase.
		TIM2->CH2CVR = trapezoid( position + ((0<<PWM_PERIOD_PO2)) );
		TIM2->CH3CVR = trapezoid( position + ((2<<PWM_PERIOD_PO2)) );
		TIM2->CH4CVR = trapezoid( position + ((4<<PWM_PERIOD_PO2)) );

		int didupd = 0;

		// Periodically update, from the ADC, the current voltage of
		// the monitor pin.
		if( position == updchecknext )
		{
			adcinflection[updchecknext_id] = ADC1->RDATAR;

			updchecknext += (1<<PWM_PERIOD_PO2);
			updchecknext_id ++;

			if( updchecknext == ((6<<PWM_PERIOD_PO2)) )
			{
				updchecknext = 0;
				updchecknext_id = 0;

				// On end, we can speed up our rotor or slow it down if it makes sense.
				int advance = g_advance_speed;
				if( advance > target_rotor_speed )
				{
					advance = advance - ((advance>>8)*(advance>>9))-1;
					if( advance < target_rotor_speed )
						advance = target_rotor_speed;
				}
				else if( advance < target_rotor_speed )
				{
					advance = advance + ((advance>>8)*(advance>>9))-1;
					if( advance > target_rotor_speed )
						advance = target_rotor_speed;
				}
				g_advance_speed = advance;
			}

			// Detct faults.
			int spinningness = - adcinflection[0] + adcinflection[2] + adcinflection[3]  - adcinflection[5] - 512;
			if( 
				( g_advance_speed < 9000 && spinningness < 100 ) || 
				( g_advance_speed < 3000 && spinningness < 500 )
			)
			{
				last_spin_cancel = spinningness;
				// Restart.
				g_advance_speed = 100000;
			}
			last_spinningness = spinningness;
		}

		// set position of rotator.
		position++;
		if( position >= ((6<<PWM_PERIOD_PO2)) ) position = 0;

	} while( (int)SysTick->CNT - (int)newComp >= 0 );
}

static void systick_init()
{
	// enable the SysTick IRQ
	NVIC_EnableIRQ(SysTick_IRQn);
	
	// Enable SysTick counter, IRQ, HCLK/1
	SysTick->CTLR = SYSTICK_CTLR_STE | SYSTICK_CTLR_STIE |
					SYSTICK_CTLR_STCLK;

	// Set the tick interval to 1ms for normal op
	SysTick->CMP = SysTick->CNT + g_advance_speed;
}	



int main()
{
	uint32_t count = 0;
	SystemInit();
	Delay_Ms( 100 );

	adc_init();
	t2pwm_init();

	g_advance_speed = 100000;

	systick_init();

	int ttp = SysTick->CNT + 1200000;
	while(1)
	{
		printf( "%d,%d,%d\n", g_advance_speed, last_spin_cancel, last_spinningness );
	}
}

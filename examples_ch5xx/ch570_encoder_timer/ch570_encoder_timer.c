#include "ch32fun.h"
#include <stdio.h>

#define ENC_A           PA4
#define ENC_B           PA9
#define ENC_BUT         PA10

// 2 - Counting on T1 edge
// 1 - Counting on T2 edge
// 3 - Counting on T1 AND T2 edge
#define ENC_MODE        1

// 0 - A = PA7, B = PA2
// 1 - A = PA2, B = PA7
// 2 - A = PA4, B = PA9
// 3 - A = PA9, B = PA4
#define ENC_REMAP       2

// 0 - GND
// 1 - VCC
#define ENC_COMMON      0

volatile int enc_val = 0;
volatile int button_val = 0;

int main()
{
	SystemInit();

#if ENC_COMMON
	funPinMode(ENC_A, GPIO_ModeIN_PD);
	funPinMode(ENC_B, GPIO_ModeIN_PD);
	funPinMode(ENC_BUT, GPIO_ModeIN_PD);
	R32_PA_OUT |= ENC_BUT; // Rising edge mode for GPIO interrupt
#else
	funPinMode(ENC_A, GPIO_ModeIN_PU);
	funPinMode(ENC_B, GPIO_ModeIN_PU);
	funPinMode(ENC_BUT, GPIO_ModeIN_PU);
	R32_PA_CLR |= ENC_BUT; // Falling edge mode for GPIO interrupt
#endif

#if ENC_REMAP
	R16_PIN_ALTERNATE_H |= (ENC_REMAP << 6);
#endif

	// Encoder setup
	R8_ENC_REG_CTRL = (ENC_MODE << 1) | RB_START_ENC_EN; // Set encoder mode and enable it
	R32_ENC_REG_CEND = 0xFFFF; // Encoder counter highest number, max is 0xFFFFFF

	R8_ENC_INTER_EN = RB_IE_DIR_DEC | RB_IE_DIR_INC; // Enabling encoder increase/decrease interrupt
	NVIC_EnableIRQ(ENCODER_IRQn);

	// Button setup
	R16_PA_INT_MODE |= ENC_BUT; // Set GPIO external interrupt to edge mode
	R16_PA_INT_EN |= ENC_BUT; // Enable GPIO external interrupt for button pin

	NVIC_EnableIRQ(GPIOA_IRQn);

	while(1)
	{
		if (enc_val) 
		{
			printf("%d %s\n", (int16_t)R32_ENC_REG_CCNT, enc_val>0?"INC":"DEC");
			enc_val = 0;
		}
		if (button_val)
		{
			printf("Button pressed\n");
			button_val = 0;
			
			// Clear encoder counter value
			R8_ENC_REG_CTRL |= RB_RD_CLR_EN;
			R32_ENC_REG_CCNT;
			R8_ENC_REG_CTRL &= ~RB_RD_CLR_EN;
		}
	}
}

__INTERRUPT
void ENCODER_IRQHandler()
{
	if (R8_ENC_INT_FLAG & RB_IF_DIR_INC)
	{
		enc_val = 1;
		R8_ENC_INT_FLAG = RB_IF_DIR_INC;
	}
	
	if (R8_ENC_INT_FLAG & RB_IF_DIR_DEC)
	{
		enc_val = -1;
		R8_ENC_INT_FLAG = RB_IF_DIR_DEC;
	}
}

__INTERRUPT
void GPIOA_IRQHandler()
{
	button_val = 1;
	R16_PA_INT_IF = ENC_BUT;
}

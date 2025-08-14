#include "ch32fun.h"

// Use Pin PD4
#define PIN_REG GPIOD
#define PIN_VAL 0x40
#define PIN_NUM 4

#define SCALE  64

int main(){
	uint32_t lung = 1, roll = 0, volume = 1;
	int32_t io = 1;

	SystemInit();
	RCC->APB2PCENR |= RCC_APB2Periph_GPIOD;
	PIN_REG->CFGLR &= ~(0xf << (PIN_NUM * 6));
	PIN_REG->CFGLR |= (GPIO_Speed_50MHz | GPIO_CNF_OUT_PP) << (PIN_NUM * 6);

	while(1){
		for(uint32_t r = SCALE; r != 0; r--){
			roll += lung;
			PIN_REG->BSHR = PIN_VAL<<((roll > lung) << PIN_NUM);
		}

		volume += io;
		if((volume == 1) || (volume == 65535))
			io = -io;
		else
		  if(io == 1)
				lung += ((volume << 1) - 1);
			else
				lung -= ((volume << 1) - 1);
	}
}

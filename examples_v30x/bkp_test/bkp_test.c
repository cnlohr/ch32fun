// Simple example to test BKP functions to store and retrieve data from backup registers
// The backup registers provides 42 16-bits data that can stores up to 84 bytes of user data
// The backup registers are cleared when a tamper is detected

// program behaviors:
// - write data to backup registers
// - read data from backup registers
// - clear backup registers when a tamper is detected on rising edge of PC13
// Note: only PC13 can be used for tamper detection

#include "ch32fun.h"
#include <stdio.h>
#include "ch32v30x_bkp.h"

#define LED_PIN PB10

void blink_led(int time) {
	for (int i = 0; i < time; i++) {
		funDigitalWrite(LED_PIN, 1);
		Delay_Ms( 50 );
		funDigitalWrite(LED_PIN, 0);
		Delay_Ms( 50 );
	}
}

int main() {
	SystemInit();
	Delay_Ms(100);
	funGpioInitAll();

	printf("\n~ BKP Test ~\n");
	funPinMode(LED_PIN, GPIO_CFGLR_OUT_10Mhz_PP);
	blink_led(1);

	// setup BKP
	BKP_init();

	// write data to BKP
	printf("write data to BKP\n");
	BKP_WriteData(1, 0x1234);
	BKP_WriteData(2, 0x5678);
	BKP_WriteData(11, 0x9abc);
	BKP_WriteData(12, 0xdef0);

	// read BKP
	u16 read1 = BKP_ReadData(1);
	u16 read2 = BKP_ReadData(2);
	u16 read11 = BKP_ReadData(11);
	u16 read12 = BKP_ReadData(12);
	printf("read data from BKP\n");
	printf("DataR1: %04x\n", read1);
	printf("DataR2: %04x\n", read2);
	printf("DataR11: %04x\n", read11);
	printf("DataR12: %04x\n", read12);

	// setup tamper (optional)
	BKP_tamperInit(1);

	while(1) {
		blink_led(1);
		Delay_Ms(1000);
	}
}

// Tamper interrupt handler (optional)
__attribute__((interrupt)) void TAMPER_IRQHandler(void) {
	// Check tamper interrupt flag
	if (!(BKP->TPCSR & BKP_TPIE)) return;
	// Tamper detected! All backup registers are now cleared
	// Clear the interrupt flag
	BKP->TPCSR |= BKP_CTI;
	
	// Handle the security event
	printf("\nintrution detected. removing data\n");

	u16 read1 = BKP_ReadData(1);
	u16 read2 = BKP_ReadData(2);
	u16 read11 = BKP_ReadData(11);
	u16 read12 = BKP_ReadData(12);
	printf("DataR1: %04x\n", read1);
	printf("DataR2: %04x\n", read2);
	printf("DataR11: %04x\n", read11);
	printf("DataR12: %04x\n", read12);
}
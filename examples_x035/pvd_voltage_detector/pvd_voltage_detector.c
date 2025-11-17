// This example demonstrates how to use the PVD to monitor the main power supply voltage.
//The PVD triggers an alert when the supply voltage drops below a configurable threshold.

// Testing Methods:
// Method 1: Using a Potentiometer
// 		Connect a potentiometer across the power supply input and ground
// 		Adjust the potentiometer to vary the input voltage
// 		Observe the PVD status output as the voltage crosses the threshold

// Method 2: Using a Voltage Divider
//		Add a series resistor to the power input to create a voltage drop
//		Monitor the PVD output to detect when voltage falls below the set threshold

// Available Voltage Thresholds:
// threshold 0 = 2.1V falling / 2.12V rising
// threshold 1 = 2.3V falling / 2.32V rising
// threshold 2 = 3.0V falling / 3.02V rising
// threshold 3 = 4.0V falling / 4.02V rising

#include "ch32fun.h"
#include <stdio.h>
#include "register_debug_utilities.h"

void configure_PVD(u8 threshold) {
	// Enable PWR clock
	RCC->APB1PCENR |= RCC_APB1Periph_PWR;

	if (threshold > 3) threshold = 3;
	printf("\n");
	printf("Before write:\n");
	UTIL_PRINT_REG16(PWR->CTLR, "PWR_CTLR");

	PWR->CTLR |= (threshold << 5);
	printf("After setting:\n");
	UTIL_PRINT_REG16(PWR->CTLR, "PWR_CTLR");
	printf("\n");
}

// threshold1 = 2.1V, theshold2 = 2.3V, threshold3 = 3.0V, threshold4 = 4.0V
void check_PVD_status(void) {	
	// Get threshold setting: the PLS[1:0] bits
	uint8_t threshold_setting = (PWR->CTLR >> 5) & 0x03;
	
	// get the PVD0 status bit: it's set when the voltage drops below the threshold
	uint8_t pvd_alert = (PWR->CSR >> 2) & 0x01;
	
	const char *thresholds[] = { "2.1V", "2.3V", "3.0V", "4.0V" };
	printf("\nThreshold: %s ", thresholds[threshold_setting]);
	printf("Status: %s\n", pvd_alert ? "BELOW THRESHOLD!" : "Normal");
}

int main() {
	SystemInit();
	funGpioInitAll(); // Enable GPIOs

	printf("\n~PVD Voltage Detector Example~\n");
	printf("Chip ID: %08lX\r\n", ESIG->UID0);
	printf("Chip Capacity: %d KB\r\n",ESIG->CAP);
	configure_PVD(2);

	while(1) {
		check_PVD_status();
		Delay_Ms(1000);
	}
}

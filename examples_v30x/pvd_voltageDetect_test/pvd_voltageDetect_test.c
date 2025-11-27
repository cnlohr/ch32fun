// Simple example that shows how to use the PVD voltage detector functions
// the VLEVEL value in the EXTEN_CTR register defines the chip VDD supports minimum supply voltage
// VLEVEL = 0: VDD min = 1.8V, VLEVEL = 1: VDD min = 2.4V
// When the input supply voltage drop bellow the threshold the PVD will trigger an alert

// Testing Methods:
// Method 1: Using a Potentiometer
// 		Connect a potentiometer across the power supply input and ground
// 		Adjust the potentiometer to vary the input voltage
// 		Observe the PVD status output as the voltage crosses the threshold

// Method 2: Using a Voltage Divider
//		Add a series resistor to the power input to create a voltage drop
//		Monitor the PVD output to detect when voltage falls below the set threshold

#include "ch32fun.h"
#include <stdio.h>
#include "lib_pvd.h"

void PVD_statusPrint() {
	const char *v1[] = { "2.29", "2.46", "2.55", "2.67", "2.78", "2.93", "3.06", "3.19" };
	const char *v0[] = { "2.13", "2.25", "2.32", "2.42", "2.51", "2.61", "2.69", "2.79" };
	int VLEVEL = (EXTEN->FEATURE_SIGN) & 0x01;
	const char *threshold_str = (VLEVEL == 0) ? v0[PVD_getThreshold()] : v1[PVD_getThreshold()];

	printf("\nThreshold: %sV", threshold_str);
	printf("\nStatus: %s\n", PVD_getAlert() ? "BELOW THRESHOLD!" : "Normal");
}

int main() {
	SystemInit();
	Delay_Ms(100);

	printf("\n~ PVD Voltage Detector Test ~\n");
	PVD_init(7); 			// select threshold 0-7
	printf("EXTEN_CRTS: 0x%08X\n", EXTEN->FEATURE_SIGN);

	while(1) {
		PVD_statusPrint();
		Delay_Ms(1000);
	}
}
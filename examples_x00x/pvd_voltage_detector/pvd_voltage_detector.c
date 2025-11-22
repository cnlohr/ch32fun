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
#include "lib_pvd.h"

void PVD_statusPrint() {
	const char *thresholds[] = { "2.1V", "2.3V", "3.0V", "4.0V" };
	const char *threshold_str = thresholds[PVD_getThreshold()];

	printf("\nThreshold: %sV", threshold_str);
	printf("\nStatus: %s\n", PVD_getAlert() ? "BELOW THRESHOLD!" : "Normal");
}

int main() {
	SystemInit();
	Delay_Ms(100);

	printf("\n~PVD Voltage Detector Example~\n");
	PVD_init(1);

	while(1) {
		PVD_statusPrint();
		Delay_Ms(1000);
	}
}

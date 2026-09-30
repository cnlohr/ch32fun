#include <stdio.h>
#include "ch32fun.h"
#include "NecIrDecoder.h"

#define PIN_IR PD6
NecIrDecoder ir_decoder(PIN_IR); // Using PD4 as the IR input pin

NecDecodedData data_to_print;
bool should_print = false;

int main() {
    SystemInit();
    funGpioInitAll();
    printf("CH32V003 ch32fun NEC IR Decoder\n");
    ir_decoder.begin();
    printf("IR Decoder Ready. Point your remote at the sensor.\n");

    while(1) {
        // --- Main non-blocking logic ---
        // 1. Check for data (FAST)
        if (ir_decoder.available()) {
            // 2. Read the data immediately (FAST)
            data_to_print = ir_decoder.read();
            should_print = true;
        }

        // 3. Do the slow printing (out of the critical path)
        if (should_print) {
            printf("Decoded NEC IR Data:\n");
            printf("  Address: 0x%02X\n", data_to_print.address);
            printf("  Command: 0x%02X\n", data_to_print.command);
            printf("  Raw Data: 0x%08lX\n", data_to_print.raw_data);
            printf("  Is Repeat: %s\n", data_to_print.is_repeat ? "Yes" : "No");
            printf("  Valid: %s\n", data_to_print.valid ? "Yes" : "No");
            printf("-------------------------\n");
            switch (data_to_print.command) {
                case 0x46: printf("UP\n"); break;
                case 0x44: printf("LEFT\n"); break;
                case 0x40: printf("OK\n"); break;
                case 0x43: printf("RIGHT\n"); break;
                case 0x15: printf("DOWN\n"); break;
                case 0x16: printf("1\n"); break;
                case 0x19: printf("2\n"); break;
                case 0x0D: printf("3\n"); break;
                case 0x0C: printf("4\n"); break;
                case 0x18: printf("5\n"); break;
                case 0x5E: printf("6\n"); break;
                case 0x08: printf("7\n"); break;
                case 0x1C: printf("8\n"); break;
                case 0x5A: printf("9\n"); break;
                case 0x42: printf("*\n"); break;
                case 0x52: printf("0\n"); break;
                case 0x4A: printf("#\n"); break;
                default: printf("Error: Button not recognized\n");
            }
            should_print = false;
        }





    }
}

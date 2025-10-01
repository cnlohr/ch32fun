#include "ch32fun.h"
#include <stdio.h>


static void fun_irDa_init(u16 baud_rate) {
	// Enable UART and GPIOD
	RCC->APB2PCENR |= RCC_APB2Periph_GPIOD | RCC_APB2Periph_USART1;

    // GPIOD->CFGLR &= ~(0xf<<(4*5));
	// GPIOD->CFGLR |= (GPIO_Speed_10MHz | GPIO_CNF_OUT_PP_AF)<<(4*5);

    // Configure PD5 as USART1_TX (IrDA TX output)
	funPinMode( PD5, GPIO_Speed_10MHz | GPIO_CNF_OUT_PP_AF);

    // // Configure PD6 as USART1_RX (IrDA RX input) 
    // funPinMode( PD6, GPIO_Speed_10MHz | GPIO_CNF_IN_PUPD);

	// Setup UART for Tx 8n1
	USART1->CTLR1 = USART_WordLength_8b | USART_Parity_No | USART_Mode_Tx;
	USART1->CTLR2 = USART_StopBits_1;
	USART1->CTLR3 = USART_HardwareFlowControl_None;


    // USART1->CTLR2 |= USART_CTLR2_STOP;         //# STOP
    // USART1->CTLR2 |= USART_CTLR2_LINEN;        //# LINEN
    // USART1->CTLR2 |= USART_CTLR2_CLKEN;        //# CLKEN
    
    // USART1->CTLR3 |= USART_CTLR3_SCEN;         //# SCEN
    // USART1->CTLR3 |= USART_CTLR3_HDSEL;        //# HDSEL

    // Enable USART, TX, RX, and IrDA
    // USART1->CTLR1 |= USART_CTLR1_UE | USART_CTLR1_TE | USART_CTLR1_RE;
    // USART1->CTLR3 |= USART_CTLR3_IREN;


	// Set baud rate and enable UART
	USART1->BRR = ((FUNCONF_SYSTEM_CORE_CLOCK) + (baud_rate)/2) / (baud_rate);
	USART1->CTLR1 |= CTLR1_UE_Set;
}


// void USART1_SendChar(char c) {
//     // Wait for TX buffer empty
//     while(!(USART1->STATR & USART_FLAG_TC)) {
//         printf("Waiting for TXE... STATR: 0x%04X\n", USART1->STATR);
//     }
//     USART1->DATAR = c;
//     printf("Sent char: 0x%02X\n", c);
// }


void USART1_SendChar(char c) {
    while(!(USART1->STATR & (1<<7))); // TXE
    USART1->DATAR = c;
}

void test_send(void) {
    fun_irDa_init(115200);
    printf("Sending IrDA...\n");

    while(1) {
        USART1_SendChar('H');
        // USART1_SendChar('i');
        // USART1_SendChar('\n');

        Delay_Ms(1000);
        printf("*");
    }
}


char USART1_GetChar(void) {
    while(!(USART1->STATR & (1<<5))); // RXNE
    return (char)USART1->DATAR;
}

// Check if data is available to receive
uint8_t USART1_DataAvailable(void) {
    return (USART1->STATR & (1<<5)); // RXNE flag
}

// Non-blocking receive - returns -1 if no data
int USART1_GetChar_NonBlocking(void) {
    if(USART1->STATR & (1<<5)) {
        return (char)USART1->DATAR;
    }
    return -1;
}


// Simple test with both TX and RX
void test_receive(void) {
    fun_irDa_init(115200);
    printf("Listening for IrDA...\n");
    
    while(1) {
        char c = USART1_GetChar();
        putchar(c);
    }
}
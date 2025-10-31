#include "ch32fun.h"
#include <stdio.h>

#define CH32V003_SPI_SPEED_HZ	1000000
#define SPI_RST_PIN				PC3
#define LORA_CS_PIN				PC4
#define LORA_FREQUENCY			915E6

#include "../../extralibs/ch32v003_SPI.h"
#include "systick_irq.h"
#include "fun_sx127x.h"

void SPI_Configure() {
	// reset control register
	SPI1->CTLR1 = 0;

	// Enable GPIO Port C and SPI peripheral
	RCC->APB2PCENR |= RCC_APB2Periph_GPIOC | RCC_APB2Periph_SPI1;

	// PC5 is SCLK
	GPIOC->CFGLR &= ~(0xf << (4*5));
	GPIOC->CFGLR |= (GPIO_Speed_50MHz | GPIO_CNF_OUT_PP_AF) << (4*5);

	// PC6 is MOSI
	GPIOC->CFGLR &= ~(0xf << (4*6));
	GPIOC->CFGLR |= (GPIO_Speed_50MHz | GPIO_CNF_OUT_PP_AF) << (4*6);

	// PC7 is MISO
	GPIOC->CFGLR &= ~(0xf << (4 * 7));
	GPIOC->CFGLR |= GPIO_CNF_IN_FLOATING << (4 * 7);

	// Configure SPI
	SPI1->CTLR1 |= SPI_CPHA_1Edge | SPI_CPOL_Low
				| SPI_Mode_Master| SPI_BaudRatePrescaler_4
				| SPI_NSS_Soft | SPI_DataSize_8b;
	
	SPI1->CTLR1 |= SPI_Direction_2Lines_FullDuplex;

	SPI1->CTLR1 |= CTLR1_SPE_Set;		// Enable SPI Port
}

int main() {
	SystemInit();
	Delay_Ms(50);

	printf("\r\r\n\nLoRa SX127x test.\r\n");
	systick_init();

	funGpioInitAll();
	SPI_Configure();

	//# configure reset pin
	funPinMode(SPI_RST_PIN, GPIO_Speed_10MHz | GPIO_CNF_OUT_PP);
	funDigitalWrite(SPI_RST_PIN, 1);
	
	//# Reset Spi Devices
	funDigitalWrite(SPI_RST_PIN, 0);
	Delay_Ms(100);
	funDigitalWrite(SPI_RST_PIN, 1);
	Delay_Ms(100);

	//# init loRa
	fun_sx127x_init(LORA_FREQUENCY, LORA_CS_PIN);
	fun_sx127x_setTxPower(17);

	u32 time = millis();

	while(1) {
		int packetSize = fun_sx127x_parsePacket();

		if (packetSize) {
			char buf[packetSize];
			fun_sx127x_readPacket(buf, packetSize);
			int rssi = fun_sx127x_getRssi(LORA_FREQUENCY);
			int snr = fun_sx127x_getSNR();
			// buf[packetSize] = 0;
			printf("\nReceive Packet: '%s' RSSI %d, SNR %d\n\r", buf, rssi, snr);
		}

		if (millis() - time < 3000) continue;
		time = millis();

		//# send limited bytes
		printf("sending signal\n");
		u8 message[32];
		int len = sprintf(message, "Hello Cutie ;) %ld", time);
		fun_sx127x_send(message, len);
	}
}
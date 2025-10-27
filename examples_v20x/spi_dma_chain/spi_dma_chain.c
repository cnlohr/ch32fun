#include "ch32fun.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define DEBUG_LEVEL 2 // 0-3 higher level - more prints in the terminal

#define NODE_ID 0xaa
#define SPI_TRANSMISSION_TIMEOUT 10000
#define SPI_POLLING_INTERVAL 1500
#define ENUM_REQUEST_DELAY 3000
#define SPI_FORCED_SENDING 0 // Send even if we don't know any nodes
#define SPI_ENABLE_POLLING 1 // Periodically send empty messages on Master to receive any pending data from Slave

#if CH32V20x == 1
#define LED PB2
#define BUTTON PA0
#define BUTTON_POLARITY 1
#define LED_ON 1
#elif CH32V30x == 1
#define LED PA15
#define BUTTON PB3
#define BUTTON_POLARITY 0
#define LED_ON 1
#else
#define LED PC8
#define BUTTON PC0
#define BUTTON_POLARITY 1
#define LED_ON 0
#endif

#if BUTTON_POLARITY
#define readButton funDigitalRead(BUTTON)
#else
#define readButton !funDigitalRead(BUTTON)
#endif

#define BUTTON_SEND_DELAY 1000

#define MAX_NODES_N 64 // Maximum nodes number in the chain
#if (MAX_NODES_N > 255)
#error "You can have only 255 nodes max"
#endif

#define reloadTimer(x, y) x = SysTick->CNT + Ticks_from_Ms(y)
#define checkTimer(x) (int64_t)(SysTick->CNT - x) > 0

#define DMA_BASE_ADDR(n) (DMA1_Channel1_BASE+(0x14*(n-1)))
#define DMA1_CH(n) ((DMA_Channel_TypeDef *)DMA_BASE_ADDR(n))
#define DMA1_IRQ(x) DMA1_Channel1_IRQn + x - 1

#define SPI1_CS1 PA4
#define SPI2_CS1 PB12

#define SPI_Master 1
#define SPI_Slave 0

#define DMA_In 0
#define DMA_Out 1

#define PACKET_SIZE 10
#define BUFFER_SIZE PACKET_SIZE*10

typedef enum {
	SPI_IDLE,
	SPI_POLLING,
	SPI_IN_PROGRESS,
	SPI_DONE,
} transfer_t;

typedef enum {
	CMD_IDLE = 0,
	CMD_ENUM_REQUEST = 0x11,
	CMD_ENUM_UPDATE = 0x12,
	CMD_ENUM_REPLY = 0x13,
	CMD_READ_SENSOR = 0x21,
	CMD_LED_TOGGLE = 0x31,
	CMD_LED_ON = 0x32,
	CMD_LED_OFF = 0x33,
} command_t;

struct command_struct {
	command_t type;
	uint16_t msg_number;
};

typedef enum {
	ENUM_EMPTY = 0,
	ENUM_WAIT = 1,
	ENUM_DONE = 2,
	ENUM_INIT,
	ENUM_CHANGED,
} enumeration_t;

typedef struct {
	__attribute__ ((aligned(4))) uint8_t in[BUFFER_SIZE+1];
	__attribute__ ((aligned(4))) uint8_t out[BUFFER_SIZE+1];
	volatile uint32_t pos_out; // Position in the array for the next packet to be placed at
	volatile uint32_t pos_in;
	volatile uint32_t sending_pos; // Position we are actually reading data from during send
	volatile uint32_t receiving_pos; // or writing to during receive
	volatile uint32_t prev_in_cntr; // Previously captured value of DMA_IN->CNTR
} buffer_t;

typedef struct {
	uint8_t map[MAX_NODES_N - 1];
	uint8_t n_nodes;
} node_map_t;

typedef struct spi {
	SPI_TypeDef * addr;
	DMA_Channel_TypeDef * dma_in;
	DMA_Channel_TypeDef * dma_out;
	uint32_t cs_pin;
	buffer_t * buffer;
	struct spi * another; // Pointer to the opposite SPI
	volatile transfer_t transmission; // Current SPI transmission status
	volatile enumeration_t enumeration_state;
	node_map_t nodes;
	uint16_t message_number;
	struct command_struct in_pending_command;
	struct command_struct out_pending_command;
	uint8_t previous_out_message[PACKET_SIZE]; // Save previous message to be able to repeat it if receiver NAKs
	volatile uint16_t transmission_size; // Number of bytes received in last transmission
	uint16_t needs_process; // Actual number of bytes that needs to be processed in "process_message"
	int16_t incoming_message_pos; // Position in buffer to start next processing from
	uint8_t master; // Is this SPI in Master mode?
} spi_t;

buffer_t buffer;
buffer_t buffer_spi1;
buffer_t buffer_spi2;

spi_t spi2;

spi_t spi1 = {
	SPI1,
	DMA1_Channel2,
	DMA1_Channel3,
	SPI1_CS1,
	&buffer_spi1,
	&spi2,
	SPI_IDLE,
};

spi_t spi2 = {
	SPI2,
	DMA1_Channel4,
	DMA1_Channel5,
	SPI2_CS1,
	&buffer_spi2,
	&spi1,
	SPI_IDLE,
};

spi_t* spi_master = &spi1;
spi_t* spi_slave = &spi2;

uint64_t timer = 0;
uint64_t enum_timer = 0;
uint64_t polling_timer = 0;
uint64_t button_debounce_timer = 0;
uint64_t button_timer = 0;
bool last_button_state = false;

void process_incoming(spi_t * spi) {
	// Make local copies because new transmission can mess everything up
	__disable_irq();
	uint32_t transfer_size = spi->transmission_size;
	uint32_t local_pos_in = spi->buffer->pos_in;
	spi->transmission_size = 0;
	__enable_irq();

	// Now we need to find a new position in buffer for next transmission
	if (!spi->master) {
		spi->buffer->pos_in += transfer_size;
		if (spi->buffer->pos_in >= BUFFER_SIZE) spi->buffer->pos_in -= BUFFER_SIZE;
		spi->needs_process = transfer_size;
	} else {
		// Calculating how many bytes we actually received
		// If we use DMA better rely on it's counter
		if (spi->dma_in->CFGR & DMA_CFGR1_EN) {

			uint32_t new_pos_in = BUFFER_SIZE - spi->dma_in->CNTR;
			if (new_pos_in > spi->buffer->pos_in) {
				transfer_size = (new_pos_in - spi->buffer->pos_in);
			} else {
				transfer_size = (new_pos_in + (BUFFER_SIZE - spi->buffer->pos_in));
			}
			spi->needs_process = transfer_size;
			
			spi->buffer->pos_in = new_pos_in;
		}
	}

	for (int i = 0; i < transfer_size; i += PACKET_SIZE) {
		if (i) local_pos_in += PACKET_SIZE;
		if (local_pos_in >= BUFFER_SIZE) local_pos_in -= BUFFER_SIZE; // Loop back in buffer

#if DEBUG_LEVEL > 2
		printf("Processing %s: [%ld] ", (spi->master?"Master":"Slave"), local_pos_in);
		for (int n = 0; n < PACKET_SIZE; n++) {
			printf("%02x ", spi->buffer->in[local_pos_in+n]);
		}
		printf("\n");
#endif
		
		if (spi->buffer->in[local_pos_in] == 0) continue; // Skip message if sender field is blank
		
		// Initialize enumeration or reset the one if node map is already filled
		// For this we check for messages from a direct neighbor - receiver_id = 0
		// And also check if sender_id have changed compared to what's in nodes.map
		if (spi->buffer->in[local_pos_in + 1] == 0 && spi->nodes.map[0] != spi->buffer->in[local_pos_in]) {
			spi->nodes.map[0] = spi->buffer->in[local_pos_in];
			spi->enumeration_state = ENUM_INIT;
			spi->nodes.n_nodes = 1; // We can only be sure about the neighboring node now
		}

		if (!(spi->buffer->in[local_pos_in+2]|spi->buffer->in[local_pos_in+3])) { // Skip also if message number field is empty
			continue;
		}

		uint8_t inc_id = spi->buffer->in[local_pos_in + 1];
		// If this message is intended for this node, or is a broadcast
		if (inc_id == NODE_ID || inc_id == 0 || inc_id == 0xff) {
			if (spi->incoming_message_pos < 0) {
				spi->incoming_message_pos = local_pos_in;
				spi->needs_process -= i; // Adjust the size to process, if we skipped some messages prior in the buffer
			}
		// Else pass message further
		} 
#if (SPI_FORCED_SENDING)
		else {
#else
		else if (spi->another->nodes.n_nodes) {
#endif
			// Copy message to another SPI's OUT buffer
			memcpy(&spi->another->buffer->out[spi->another->buffer->pos_out], &spi->buffer->in[local_pos_in], PACKET_SIZE);
			if ((spi->another->buffer->pos_out += PACKET_SIZE) >= BUFFER_SIZE) spi->another->buffer->pos_out = 0;
		}
	}
}

__attribute__((interrupt)) void DMA1_Channel3_IRQHandler( void ) 
{
	if (DMA1->INTFR & DMA1_IT_TC3) {
		DMA1_CH(3)->CFGR &= ~DMA_CFGR1_EN; // Disable OUT DMA
		if (spi1.master) {
			if (spi1.transmission != SPI_IDLE) spi1.transmission = SPI_DONE; // Notify main
		} else if (spi_slave->buffer->sending_pos != spi_slave->buffer->pos_out) {
			spi_slave->dma_out->MADDR = (uint32_t)&spi_slave->buffer->out[spi_slave->buffer->sending_pos];
			if (spi_slave->buffer->pos_out > spi_slave->buffer->sending_pos) {
				spi_slave->dma_out->CNTR = spi_slave->buffer->pos_out - spi_slave->buffer->sending_pos;
				spi_slave->buffer->sending_pos = spi_slave->buffer->pos_out;
			} else {
				spi_slave->dma_out->CNTR = BUFFER_SIZE - spi_slave->buffer->sending_pos;
				spi_slave->buffer->sending_pos = 0;
			}
			spi_slave->dma_out->CFGR |= DMA_CFGR1_EN;
		}
	}
	DMA1->INTFCR = DMA1_IT_GL3 | DMA1_IT_GL2; // Clear all interrupt flags for both DMA channels for SPI1
}

__attribute__((interrupt)) void DMA1_Channel5_IRQHandler(void) {
	if (DMA1->INTFR & DMA1_IT_TC5) {
		DMA1_CH(5)->CFGR &= ~DMA_CFGR1_EN;
		if (spi2.master) {
			if (spi2.transmission != SPI_IDLE) spi2.transmission = SPI_DONE;
		} else if (spi_slave->buffer->sending_pos != spi_slave->buffer->pos_out) {
			spi_slave->dma_out->MADDR = (uint32_t)&spi_slave->buffer->out[spi_slave->buffer->sending_pos];
			if (spi_slave->buffer->pos_out > spi_slave->buffer->sending_pos) {
				spi_slave->dma_out->CNTR = spi_slave->buffer->pos_out - spi_slave->buffer->sending_pos;
				spi_slave->buffer->sending_pos = spi_slave->buffer->pos_out;
			} else {
				spi_slave->dma_out->CNTR = BUFFER_SIZE - spi_slave->buffer->sending_pos;
				spi_slave->buffer->sending_pos = 0;
			}
			spi_slave->dma_out->CFGR |= DMA_CFGR1_EN;
		}
	}
	DMA1->INTFCR = DMA1_IT_GL4 | DMA1_IT_GL5; // Clear all interrupt flags for both DMA channels for SPI2
}

// GCC (with -Os) ignores inline keyword here, if you really want you have to use "__attribute__((always_inline))"
// __attribute__((always_inline)) static inline void spi_nodma_transmission(spi_t * spi, uint8_t * buf, uint32_t len) {
void spi_nodma_transmission(spi_t * spi, uint8_t * buf, uint32_t len) {
	uint32_t timeout = SPI_TRANSMISSION_TIMEOUT;
	uint32_t sent = 0;
	uint32_t received = 0;

	if (buf && len == 0) buf = NULL;
	if (buf == NULL) {
		len = spi->buffer->pos_out + (BUFFER_SIZE - spi->buffer->sending_pos);
		if (len >= BUFFER_SIZE) len -= BUFFER_SIZE;
	}

	while (timeout--) {
		// If SPI slave, we only run this function in interrupt
		if (((spi->addr->STATR & SPI_STATR_TXE) && len) || !sent) {
			// Write byte from buffer
			if (!len && !sent) {
				// By default always send node ID in the first byte of transmission
				// to indicate to master that there is node connected.
				// Otherwise receiver will read all zeroes and won't know if there is active node here.
				spi->addr->DATAR = (uint8_t)NODE_ID;
				ADD_N_NOPS(2);
				spi->addr->DATAR = 0;
			} else if (buf == NULL) {
				spi->addr->DATAR = spi->buffer->out[spi->buffer->sending_pos++];
				if (spi->buffer->sending_pos >= BUFFER_SIZE) spi->buffer->sending_pos = 0;
				len--;
			} else {
				spi->addr->DATAR = buf[sent];
				len--;
			}
			// if (!len) spi->addr->DATAR = 0; // Clear the buffer, or it last value will be sent continuously
			timeout = SPI_TRANSMISSION_TIMEOUT; // Reload timeout
			sent++;
		}

		if ((spi->addr->STATR & SPI_STATR_RXNE)) {
			// Read from SPI to a buffer
			spi->buffer->in[spi->buffer->receiving_pos++] = spi->addr->DATAR;
			if (spi->buffer->receiving_pos >= BUFFER_SIZE) spi->buffer->receiving_pos = 0;
			timeout = SPI_TRANSMISSION_TIMEOUT;
			received++;
			// If out buffer is already empty, but we now receiving extra packet.
			// Clear DATAR or we will sent garbage (last byte over an over).
			if (!len && received == PACKET_SIZE+1) spi->addr->DATAR = 0;
		}

		if (!spi->master && funDigitalRead(spi->cs_pin)) {
			break;
		}
		// If SPI master, we only run while there is something to send, or until timeout if POLLING
		if (spi->master && (!len) && spi->transmission != SPI_POLLING) break;
	}

	spi->buffer->sending_pos = PACKET_SIZE * (spi->buffer->sending_pos / PACKET_SIZE); // Align position to the packet start for next transmission
	spi->needs_process = received;
	if (received < PACKET_SIZE) spi->needs_process = PACKET_SIZE;

	// If we actually received anything, set flag to process incoming buffer
	// Previously we called a function directly, but it was messing with SPI timings (holding up IRQ for too long)
	if (received) {
		spi->transmission_size += received;
	}

	EXTI->INTFR = 1 << (spi->cs_pin % 16);
}

void spi_dma_slave() {
	// Falling edge - start of transmission
	if (!funDigitalRead(spi_slave->cs_pin)) {
		if (spi_slave->dma_out->CNTR == 0 && spi_slave->buffer->sending_pos != spi_slave->buffer->pos_out) {
			spi_slave->dma_out->CFGR &= ~DMA_CFGR1_EN;
			spi_slave->dma_out->MADDR = (uint32_t)&spi_slave->buffer->out[spi_slave->buffer->sending_pos];
			if (spi_slave->buffer->pos_out > spi_slave->buffer->sending_pos) {
				spi_slave->dma_out->CNTR = spi_slave->buffer->pos_out - spi_slave->buffer->sending_pos;
				spi_slave->buffer->sending_pos = spi_slave->buffer->pos_out;
			} else {
				spi_slave->dma_out->CNTR = BUFFER_SIZE - spi_slave->buffer->sending_pos;
				spi_slave->buffer->sending_pos = 0;
			}
			spi_slave->dma_out->CFGR |= DMA_CFGR1_EN;

		} else if (spi_slave->dma_out->CNTR == 0 || (spi_slave->dma_out->CFGR & DMA_CFGR1_EN)) {
			// By default always send node ID in the first byte of transmission
			// to indicate to master that there is node connected.
			// Otherwise receiver will read all zeroes and won't know if there is active node here.
			spi_slave->addr->DATAR = (uint8_t)NODE_ID;
			ADD_N_NOPS(2);
			spi_slave->addr->DATAR = 0;
		}
	// Rising edge - end of transmission
	} else {
		uint32_t received = 0;

		if (spi_slave->buffer->prev_in_cntr >= spi_slave->dma_in->CNTR) received = spi_slave->buffer->prev_in_cntr - spi_slave->dma_in->CNTR;
		else received = spi_slave->buffer->prev_in_cntr + (BUFFER_SIZE - spi_slave->dma_in->CNTR);
		spi_slave->buffer->prev_in_cntr = spi_slave->dma_in->CNTR;

		// If we actually received anything, set flag to process incoming buffer
		// Previously we called a function directly, but it was messing with SPI timings (holding up IRQ for too long)
		if (received) {
			spi_slave->transmission_size += received;
		}
	}

	EXTI->INTFR = 1 << (spi_slave->cs_pin % 16); // Clear external interrupt flag
}

// All EXTI handlers are declared to be able to easily change trigger pin with a function.
// On practice it is recommended to leave only the one handler that is actually being use. It will save ~400B of flash.
// Also without "__attribute__((always_inline))" or "-fno-ipa-icf-functions" in gcc options,
// this way of declaring interrupt handlers produced unexpected and unwanted results.
__attribute__((interrupt)) __attribute__((always_inline)) void EXTI0_IRQHandler( void ) { spi_dma_slave(); }
__attribute__((interrupt)) __attribute__((always_inline)) void EXTI1_IRQHandler( void ) { spi_dma_slave(); }
__attribute__((interrupt)) __attribute__((always_inline)) void EXTI2_IRQHandler( void ) { spi_dma_slave(); }
__attribute__((interrupt)) __attribute__((always_inline)) void EXTI3_IRQHandler( void ) { spi_dma_slave(); }
__attribute__((interrupt)) __attribute__((always_inline)) void EXTI4_IRQHandler( void ) { spi_dma_slave(); }
__attribute__((interrupt)) __attribute__((always_inline)) void EXTI9_5_IRQHandler( void ) { spi_dma_slave(); }
__attribute__((interrupt)) __attribute__((always_inline)) void EXTI15_10_IRQHandler( void ) { spi_dma_slave(); }

static inline void setupEXTI(uint8_t pin, uint8_t rising) {
	uint8_t port = (uint8_t)(pin / 16);
	pin = pin%16;
	uint8_t exti = (uint8_t)(pin / 4);

	AFIO->EXTICR[exti] |= port;

	if (rising == 0) {
		EXTI->RTENR &= ~(1 << pin); // disable rising edge trigger
		EXTI->FTENR |= 1 << pin; // enable falling edge trigger
	} else if (rising == 1) {
		EXTI->RTENR |= 1 << pin; // enable rising edge trigger
		EXTI->FTENR &= ~(1 << pin); // disable falling edge trigger
	} else{
		EXTI->RTENR |= 1 << pin; // enable rising edge trigger
		EXTI->FTENR |= 1 << pin; // enable falling edge trigger
	}

	IRQn_Type irqn;
	if (pin < 5) irqn = (IRQn_Type)((uint8_t)EXTI0_IRQn + pin);
	else if (pin < 10) irqn = EXTI9_5_IRQn;
	else irqn = EXTI15_10_IRQn;

	// printf("exti priority %02x\n", NVIC);
	NVIC_EnableIRQ(irqn);
#if defined(FUNCONF_ENABLE_HPE) && FUNCONF_ENABLE_HPE
	NVIC_SetPriority(irqn, 0<<7); // Set HIGH preemptive priority for the IRQ
#endif
	EXTI->INTENR |= 1 << pin; // Enable EXTIx interrupt
}

void setupDMA(uint8_t ch, spi_t * spi, uint8_t mode, uint8_t sixteen_bits, uint8_t* buffer)
{
	RCC->AHBPCENR |= RCC_AHBPeriph_DMA1;

	DMA1_CH(ch)->PADDR = (uint32_t)&spi->addr->DATAR;
	DMA1_CH(ch)->MADDR = (uint32_t)buffer;
	DMA1_CH(ch)->CNTR = (mode?0:BUFFER_SIZE);
	DMA1_CH(ch)->CFGR =
		DMA_M2M_Disable |
		(mode?DMA_Priority_VeryHigh:DMA_Priority_High) |
		(sixteen_bits?DMA_MemoryDataSize_Word:DMA_MemoryDataSize_Byte) |
		(sixteen_bits?DMA_PeripheralDataSize_Word:DMA_PeripheralDataSize_Byte) |
		DMA_MemoryInc_Enable |
		(mode?DMA_Mode_Normal:DMA_Mode_Circular) |
		(mode?DMA_DIR_PeripheralDST:DMA_DIR_PeripheralSRC) |
		(mode?DMA_IT_TC:0);

	if (mode) {
		NVIC_EnableIRQ( DMA1_IRQ(ch) );
#if defined(FUNCONF_ENABLE_HPE) && FUNCONF_ENABLE_HPE && (TARGET_MCU!=CH32V103)
		NVIC_SetPriority(DMA1_IRQ(ch), 1<<7);
#endif
	}
	DMA1_CH(ch)->CFGR |= DMA_CFGR1_EN;
}

void setup_SPI(spi_t * spi, uint8_t mode, uint8_t sixteen_bits)
{
	printf("SPI%d - %s\n", (spi->addr == SPI1)?1:2, mode?"master":"slave");

	// I've found that GPIO modes other than 2MHz, at least in case of slave SPI MISO doesn't really work that great (if at all).
	// 2MHz is more stable and I didn't find any downsides.
	if (spi->addr == SPI1) {
		funPinMode(SPI1_CS1, mode?GPIO_CFGLR_OUT_2Mhz_PP:GPIO_CNF_IN_PUPD);
		funPinMode(PA5, mode?GPIO_CFGLR_OUT_2Mhz_AF_PP:GPIO_CNF_IN_FLOATING);
		funPinMode(PA6, mode?GPIO_CNF_IN_FLOATING:GPIO_CFGLR_OUT_2Mhz_AF_PP);
		funPinMode(PA7, mode?GPIO_CFGLR_OUT_2Mhz_AF_PP:GPIO_CNF_IN_FLOATING);
		RCC->APB2PRSTR = RCC_SPI1RST;
		RCC->APB2PRSTR = 0;
		RCC->APB2PCENR |= RCC_APB2Periph_SPI1;
	} else {
		funPinMode(SPI2_CS1, mode?GPIO_CFGLR_OUT_2Mhz_PP:GPIO_CNF_IN_PUPD);
		funPinMode(PB13, mode?GPIO_CFGLR_OUT_2Mhz_AF_PP:GPIO_CNF_IN_FLOATING);
		funPinMode(PB14, mode?GPIO_CNF_IN_FLOATING:GPIO_CFGLR_OUT_2Mhz_AF_PP);
		funPinMode(PB15, mode?GPIO_CFGLR_OUT_2Mhz_AF_PP:GPIO_CNF_IN_FLOATING);
		RCC->APB1PRSTR = RCC_SPI2RST;
		RCC->APB1PRSTR = 0;
		RCC->APB1PCENR |= RCC_APB1Periph_SPI2;
	}

	// Configure SPI
	spi->addr->CTLR1 = (mode?SPI_NSS_Soft:0) | SPI_CPHA_1Edge | SPI_CPOL_Low | (sixteen_bits?SPI_DataSize_16b:0) | SPI_BaudRatePrescaler_16 | (mode?SPI_Mode_Master:0);
	// spi->CRCR = 7;

	spi->master = mode;
	spi->addr->CTLR2 = SPI_CTLR2_RXDMAEN | SPI_CTLR2_TXDMAEN;

	if (spi->addr == SPI1) {
		setupDMA(2, spi, DMA_In, sixteen_bits, spi->buffer->in);
		setupDMA(3, spi, DMA_Out, sixteen_bits, spi->buffer->out);
	} else {
		setupDMA(4, spi, DMA_In, sixteen_bits, spi->buffer->in);
		setupDMA(5, spi, DMA_Out, sixteen_bits, spi->buffer->out);
	}

	spi->addr->CTLR1 |= CTLR1_SPE_Set; // Enable SPI

}

void spi_send(spi_t * spi, uint8_t n_packets) {
#if DEBUG_LEVEL > 1
	printf("%s OUT [%ld] ", (spi->master?"Master":"Slave"), spi->buffer->pos_out);
	for (int n = 0; n < PACKET_SIZE; n++) {
		printf("%02x ", spi->buffer->out[spi->buffer->pos_out+n]);
	}
	printf("\n");
#endif
	// Update position in buffer for next data
	// Loop the buffer if needed
	if ((spi->buffer->pos_out += PACKET_SIZE * n_packets) >= BUFFER_SIZE) spi->buffer->pos_out -= BUFFER_SIZE;

	// Master needs to initiate a transmission if it's not in progress already
	// Slave will transmit in interrupt when master will initiate, so nothing to do here
	if (spi->master && spi->transmission == SPI_IDLE) {
		spi->dma_out->CFGR &= ~DMA_CFGR1_EN; // Disable DMA to be able to make changes
		spi->dma_out->MADDR = (uint32_t)&spi->buffer->out[spi->buffer->sending_pos]; // Update buffer postion address

		if (spi->buffer->sending_pos >= spi->buffer->pos_out) { // This means we are looping over buffer
			spi->dma_out->CNTR = BUFFER_SIZE - spi->buffer->sending_pos;
			spi->buffer->sending_pos = 0; // We'll send the rest in another transmission
		} else {
			spi->dma_out->CNTR = spi->buffer->pos_out - spi->buffer->sending_pos;
			spi->buffer->sending_pos = spi->buffer->pos_out;
		}

		funDigitalWrite(spi->cs_pin, 0); // CS low to start transmission
		spi->transmission = SPI_IN_PROGRESS;
		Delay_Ms(2); // Let slave to react
		spi->dma_out->CFGR |= DMA_CFGR1_EN;
		reloadTimer(polling_timer, SPI_POLLING_INTERVAL);
	}
}

void spi_send_message(spi_t * spi, uint8_t receiver_id, uint16_t message_id, uint8_t* buf, uint8_t len) {
#if !SPI_FORCED_SENDING
	if (spi->nodes.map[0] == 0) return;
#endif
	if (!len) return;

	spi->buffer->out[spi->buffer->pos_out] = (uint8_t)NODE_ID;
	spi->buffer->out[spi->buffer->pos_out+1] = receiver_id;
	if (!message_id) {
		if (spi->message_number++ > 16383) spi->message_number = 1;
		spi->buffer->out[spi->buffer->pos_out+2] = (uint8_t)(spi->message_number>>6);
		spi->buffer->out[spi->buffer->pos_out+3] = (uint8_t)((spi->message_number<<2) | 3);
	} else {
		spi->buffer->out[spi->buffer->pos_out+2] = (uint8_t)(message_id>>6);
		spi->buffer->out[spi->buffer->pos_out+3] = (uint8_t)(message_id);
	}

	// TODO: make a loop to send long messages in multiple packets
	if (len > PACKET_SIZE - 4) len = PACKET_SIZE - 4;
	if (len < PACKET_SIZE - 4) memset(&spi->buffer->out[spi->buffer->pos_out+4+len], 0, PACKET_SIZE - 4 - len);
	memcpy(&spi->buffer->out[spi->buffer->pos_out+4], buf, len);

	spi_send(spi, 1);
}

void spi_send_control(spi_t * spi, uint8_t receiver_id, uint16_t message_id, uint8_t ack) {
#if !SPI_FORCED_SENDING
	if (spi->nodes.map[0] == 0) return;
#endif
	spi->buffer->out[spi->buffer->pos_out] = (uint8_t)NODE_ID;
	spi->buffer->out[spi->buffer->pos_out+1] = receiver_id;
	spi->buffer->out[spi->buffer->pos_out+2] = (uint8_t)(message_id>>8);
	spi->buffer->out[spi->buffer->pos_out+3] = (uint8_t)(message_id);

	// second bit 1 - ACK, 0 - NAK
	if (ack) spi->buffer->out[spi->buffer->pos_out+3] |= 2;
	else spi->buffer->out[spi->buffer->pos_out+3] &= ~(2);
	for (int i = 4; i < PACKET_SIZE; i++) {
		spi->buffer->out[spi->buffer->pos_out+i] = 0;
	}

	spi_send(spi, 1);
}

void spi_poll() {
#if SPI_ENABLE_POLLING
#if DEBUG_LEVEL
	printf("Polling...\n");
#endif
	// Slave will ignore empty message, but will send any data it has to send
	spi_master->buffer->out[spi_master->buffer->pos_out] = NODE_ID;
	for (int i = 1; i < PACKET_SIZE; i++) {
		spi_master->buffer->out[spi_master->buffer->pos_out + i] = 0;
	}
	spi_send(spi_master, 1);
#endif
}

void enumerate(spi_t * spi, command_t cmd) {
#if DEBUG_LEVEL > 1
	printf("%s ENUM OUT %d\n", spi->master?"Master":"Slave", cmd);
#endif
	if (cmd == CMD_ENUM_UPDATE && spi->another->nodes.n_nodes && spi->nodes.n_nodes) {
		spi_send_message(spi, spi->nodes.map[0], 0, (uint8_t*)(&(uint8_t[]){CMD_ENUM_UPDATE, spi->another->nodes.map[0], spi->another->nodes.map[1], spi->another->nodes.map[2], spi->another->nodes.map[3], spi->another->nodes.map[4]}), 6);
		if (spi->another->nodes.n_nodes > 5) {
			spi_send_message(spi, spi->nodes.map[0], (spi->message_number<<2)|1, &spi->another->nodes.map[5], spi->another->nodes.n_nodes - 5);
			spi_send_message(spi, spi->nodes.map[0], (spi->message_number<<2)|1, (uint8_t*)(&(uint8_t[]){0xff}), 1);
		}
	} else if (cmd == CMD_ENUM_REQUEST) {
		spi_send_message(spi, spi->nodes.map[0], 0, (uint8_t[]){CMD_ENUM_REQUEST}, 1);
		spi->out_pending_command.type = CMD_ENUM_REQUEST;
		spi->out_pending_command.msg_number = spi->message_number;
	} else if (cmd == CMD_ENUM_REPLY) {
		if (spi->another->nodes.n_nodes) { // Not necessary chack but for clearance
			spi_send_message(spi, spi->nodes.map[0], (spi->in_pending_command.msg_number<<2)|2, &spi->another->nodes.map[0], spi->another->nodes.n_nodes);
		}
		spi_send_message(spi, spi->nodes.map[0], (spi->in_pending_command.msg_number<<2)|2, (uint8_t*)(&(uint8_t[]){0xff}), 1);
	}
}

void process_message(spi_t * spi) {
	if ((uint8_t)spi->enumeration_state > (uint8_t)ENUM_DONE ) {
		// Send to another part of the chain updated nodes map
		enumerate(spi->another, CMD_ENUM_UPDATE);
		// If we found a new neighbour we want to send our known map to it too
		if (spi->enumeration_state == ENUM_INIT) {
			enumerate(spi, CMD_ENUM_UPDATE);
			spi->enumeration_state = ENUM_WAIT;
			reloadTimer(enum_timer, ENUM_REQUEST_DELAY);
		} else {
			spi->enumeration_state = ENUM_DONE;
		}
#if DEBUG_LEVEL
		printf("SPI %s nodes map updated: [%02X", (spi->master?"master":"slave"), spi->nodes.map[0]);
		for (int i = 1; i < spi->nodes.n_nodes; i++) {
			printf(", %02X", spi->nodes.map[i]);
		}
		printf("]\n");
#endif
	}

	if (spi->incoming_message_pos < 0 || spi->needs_process == 0) {
		// spi->transmission_packets_n = 0;
		// spi->incoming_message_pos = -1;
		return;
	}

	uint8_t local_message[PACKET_SIZE];
	
	do {
		// Copying a message to process, so it won't be overwritten by the IRQ in the process
		memcpy(local_message, &spi->buffer->in[spi->incoming_message_pos], PACKET_SIZE);

#if DEBUG_LEVEL > 1
		printf("%s IN [%d] ", spi->master?"Master":"Slave", spi->incoming_message_pos);
		for (int n = 0; n < PACKET_SIZE; n++) {
			printf("%02x ", local_message[n]);
		}
		printf("\n");
#endif
		
		if ((spi->incoming_message_pos += PACKET_SIZE) >= BUFFER_SIZE) spi->incoming_message_pos = 0;
		// Check the message number that is located in 3rd and 4th bits
		uint16_t message_id = (local_message[2]<<8) | local_message[3];
		// Valid message always has to have a number
		// It should be filtered in process_incomming, but still checking here
		if (!message_id) continue;
		// LSB of the message number is to mark new message vs a reply
		uint16_t message_number = message_id >> 2;

		bool message_is_reply = !(message_id & 1);
		bool message_is_new = message_id & 1;
		bool ack = message_id & 2;
		bool message_is_sequel = (message_is_new && !ack);

		// Resend a command/message if NAK received while we waited a proper answer
		if (message_is_reply && !ack && spi->out_pending_command.type != CMD_IDLE && spi->out_pending_command.msg_number == message_number) {
			spi_send_message(spi, spi->previous_out_message[0], 0, spi->previous_out_message, PACKET_SIZE);
		// If this is a new message and we have no ongoing commands to finish
		} else if (message_is_new && spi->in_pending_command.type == CMD_IDLE) {
			spi->in_pending_command.msg_number = message_number;
			switch (local_message[4]) {
				case CMD_ENUM_REQUEST:
#if DEBUG_LEVEL
					printf("CMD_ENUM_REQUEST\n");
#endif
					// Reply with known node map
					enumerate(spi, CMD_ENUM_REPLY);
					break;
				case CMD_ENUM_UPDATE:
#if DEBUG_LEVEL
					printf("CMD_ENUM_UPDATE\n");
#endif
					spi->in_pending_command.type = CMD_ENUM_UPDATE;
					spi->nodes.n_nodes = 1; // Rewrite any old nodes in map
					for (int i = 5; i < PACKET_SIZE; i++) {
						if (local_message[i] == 0xff || local_message[i] == 0) {
							spi->in_pending_command.type = CMD_IDLE;
							// Send nodes map update if there is actually anything
							if (spi->nodes.n_nodes > 1) spi->enumeration_state = ENUM_CHANGED;
							break;
						}
						spi->nodes.map[spi->nodes.n_nodes++] = local_message[i];
					}
					break;
				case CMD_LED_TOGGLE:
#if DEBUG_LEVEL
					printf("CMD_LED_TOGGLE\n");
#endif
					// Turn LED on/off
					// Ack
					funDigitalWrite(LED, !funDigitalRead(LED));
					spi_send_control(spi, local_message[0], message_number<<2, 1);
					break;
				case CMD_LED_OFF:
#if DEBUG_LEVEL
					printf("CMD_LED_OFF\n");
#endif
					funDigitalWrite(LED, !LED_ON);
					spi_send_control(spi, local_message[0], message_number<<2, 1);
					break;
				case CMD_LED_ON:
#if DEBUG_LEVEL
					printf("CMD_LED_ON\n");
#endif
					funDigitalWrite(LED, LED_ON);
					spi_send_control(spi, local_message[0], message_number<<2, 1);
					break;
				case CMD_READ_SENSOR:
#if DEBUG_LEVEL
					printf("CMD_READ_SENSOR\n");
#endif
					// read_sensor_local();
					// spi->in_pending_command.type = local_message[4];
					// Or send cashed data if we do it frequently
					break;
				default:
					break;
			}
		// If this is a new message, but we are still finishing replying to previous
		} else if (spi->in_pending_command.type != CMD_IDLE) {
			// TODO: Make a timeout for pending messages. Otherwise it can break if node doesn't reply.
			if (spi->in_pending_command.msg_number != message_number || !message_is_sequel) {
#if DEBUG_LEVEL
				printf("Unanswer message is pending: %d. Incomming message skipped.\n", spi->in_pending_command.type);
#endif
				// Maybe NAK here so the sender can repeat request later
				spi_send_control(spi, local_message[0], message_number, 0);
				continue; // Don't synchronize message number, skip to next message 
			}
			switch (spi->in_pending_command.type) {
				case CMD_ENUM_UPDATE:
#if DEBUG_LEVEL
					printf("CMD_ENUM_UPDATE SEQ\n");
#endif
					for (int i = 4; i < PACKET_SIZE; i++) {
						if (local_message[i] == 0xff || local_message[i] == 0) {
							spi->out_pending_command.type = CMD_IDLE;
							if (spi->nodes.n_nodes > 1) spi->enumeration_state = ENUM_CHANGED;
							break;
						}
						spi->nodes.map[spi->nodes.n_nodes++] = local_message[i];
					}
					break;
			}
		// If this is a reply and we are waiting for one
		} else if (!message_is_new && spi->out_pending_command.type != CMD_IDLE) {
			// And if this is a reply to a correct message
			if (message_number == spi->out_pending_command.msg_number) {
				switch (spi->out_pending_command.type) {
					case CMD_ENUM_REQUEST:
					case CMD_ENUM_UPDATE:
						for (int i = 4; i < PACKET_SIZE; i++) {
							if (local_message[i] == 0xff || local_message[i] == 0) {
								spi->out_pending_command.type = CMD_IDLE;
								if (spi->nodes.n_nodes > 1) spi->enumeration_state = ENUM_CHANGED;
								break;
							}
							spi->nodes.map[spi->nodes.n_nodes++] = local_message[i];
						}
						break;
					case CMD_LED_TOGGLE:
						spi->out_pending_command.type = CMD_IDLE;
						break;
					case CMD_READ_SENSOR:
						// Process incomming sensor data, just printing for now
#if DEBUG_LEVEL
						printf("Sensor data from [%02X]:%02x-%02x-%02x-%02x-%02x-%02x\n", local_message[1], local_message[4], local_message[5], local_message[6], local_message[7], local_message[8], local_message[9]);
#endif
						spi->out_pending_command.type = CMD_IDLE;
						break;
				}
			} else {
				// This shouldn't happen? A reply with a different number, where would it come from?
#if DEBUG_LEVEL
				printf("Unexpected message number: %d, expected: %d. ID %04x\n", message_number, spi->out_pending_command.msg_number, message_id);
#endif
				continue;
			}
		// If this is a reply but we didn't request anything - act confused but do nothing
		} else if (!message_is_new) {
#if DEBUG_LEVEL
			printf("Unexpected reply. Number: %d, ID: %04x. ", message_number, message_id);
			for (uint8_t i = 0; i < PACKET_SIZE; i++) {
				printf("%02x ", local_message[i]);
			}
			printf("\n");
#endif
		}
	} while ((spi->needs_process -= PACKET_SIZE) > 0);
	spi->needs_process = 0;
	spi->incoming_message_pos = -1;
}

void read_sensor_local() {
	SPI_TypeDef * master = spi_master->addr;

	if (spi_master->transmission != SPI_IDLE) {
		while ((master->STATR & SPI_I2S_FLAG_BSY));
		funDigitalWrite(spi_master->cs_pin, 1);
		spi_master->transmission = SPI_IDLE;
	}

	uint16_t save_ctlr2 = master->CTLR2;
	master->CTLR2 = 0; // Disable SPI DMA, and interrupts if enabled

	// do sensor stuff

	// Wait until done
	while ((master->STATR & SPI_I2S_FLAG_BSY));

	master->CTLR2 = save_ctlr2; // Restore SPI DMA and interrupt settings
}

int main()
{
	SystemInit();

	funGpioInitAll();

	funPinMode(LED, GPIO_CFGLR_OUT_2Mhz_PP);
	funPinMode(BUTTON, GPIO_CFGLR_IN_PUPD);
	funDigitalWrite(LED, LED_ON);
	funDigitalWrite(BUTTON, !BUTTON_POLARITY);

	printf("SPI setup\n");
	memset(spi_master->buffer->in, 0, BUFFER_SIZE);
	memset(spi_master->buffer->out, 0, BUFFER_SIZE);

	memset(spi_slave->buffer->in, 0, BUFFER_SIZE);
	memset(spi_slave->buffer->out, 0, BUFFER_SIZE);

	setup_SPI(spi_master, SPI_Master, 0);
	funDigitalWrite(spi_master->cs_pin, 1);
	spi_master->incoming_message_pos = -1;

	setup_SPI(spi_slave, SPI_Slave, 0);
	funDigitalWrite(spi_slave->cs_pin, 0);

	setupEXTI(spi_slave->cs_pin, 2);

#if CH32V10x == 1
	NVIC->CFGR = NVIC_KEY1|1;
#endif

	printf("Node ID - %02X\n\n-------\n\n", (uint8_t)NODE_ID);

	funDigitalWrite(LED, !LED_ON);

#if NODE_ID == 0xab
	uint8_t message_slave[PACKET_SIZE-4] = {0xff, 0xb7, 0xb5, 0xb3, 0xb2, 0xb1};
#else
	uint8_t message_slave[PACKET_SIZE-4] = {0xff, 0xc7, 0xc5, 0xc3, 0xc2, 0xc1};
#endif
	uint8_t message[PACKET_SIZE-4] = {0xff, 0xA1, 0xA2, 0xA3, 0xA5, 0xA7};

	while(1)
	{
		if (checkTimer(button_debounce_timer) && readButton != last_button_state) {
			last_button_state = readButton;
			reloadTimer(button_debounce_timer, 50);
			if (last_button_state == true) {
				funDigitalWrite(LED, LED_ON);
				if (spi_master->nodes.n_nodes && checkTimer(button_timer)) {
					printf("LED toggle to [%02X]\n", spi_master->nodes.map[spi_master->nodes.n_nodes-1]);
					spi_send_message(spi_master, spi_master->nodes.map[spi_master->nodes.n_nodes-1], 0, (uint8_t[]){CMD_LED_TOGGLE}, 1);
					reloadTimer(button_timer, BUTTON_SEND_DELAY);
				}
			} else {
				funDigitalWrite(LED, !LED_ON);
			}
		}

		if (checkTimer(timer)) {
			spi_send_message(spi_master, 0, 0, message, PACKET_SIZE - 4);
			spi_send_message(spi_slave, 0, 0, message_slave, PACKET_SIZE - 4);
			reloadTimer(timer, 1000);
		}
		
		if (checkTimer(polling_timer) && spi_master->transmission == SPI_IDLE) {
			spi_poll();
			reloadTimer(polling_timer, SPI_POLLING_INTERVAL);
		}

		if (enum_timer && checkTimer(enum_timer)) {
			enum_timer = 0;
			if (spi_master->enumeration_state == ENUM_WAIT) {
				enumerate(spi_master, CMD_ENUM_REQUEST);	
			}
			if (spi_slave->enumeration_state == ENUM_WAIT) {
				enumerate(spi_slave, CMD_ENUM_REQUEST);	
			}
		}

		if (!(spi_master->addr->STATR & SPI_I2S_FLAG_BSY) && spi_master->transmission != SPI_IDLE) {
			funDigitalWrite(spi_master->cs_pin, 1);
			// Maybe add a small delay here
			Delay_Ms(2);
			spi_master->transmission = SPI_IDLE;

			process_incoming(spi_master);
			
			if (spi_master->buffer->sending_pos != spi_master->buffer->pos_out) {
#if DEBUG_LEVEL > 1
				printf("Master has something to send %ld - %ld\n", spi_master->buffer->sending_pos, spi_master->buffer->pos_out);
#endif
				spi_send(spi_master, 0);
			// In case we received any messages during last transmission, poll to see if there is anything in qeue
			} else if (spi_master->incoming_message_pos >= 0) {
				spi_poll();
			}
		}
		process_message(spi_master);
		if (spi_slave->transmission_size) {
			process_incoming(spi_slave);
			// If we passed something downstream, start transmission immediately
			if (spi_master->transmission == SPI_IDLE && spi_master->buffer->sending_pos != spi_master->buffer->pos_out) {
#if DEBUG_LEVEL > 1
				printf("Master has something to send %ld - %ld\n", spi_master->buffer->sending_pos, spi_master->buffer->pos_out);
#endif
				spi_send(spi_master, 0);
			}
		}
		process_message(spi_slave);
	}
}

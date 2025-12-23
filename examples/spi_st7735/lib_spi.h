#include "ch32fun.h"

uint8_t SPI_DC_PIN = -1;

static void SPI_init(uint8_t rst_pin, uint8_t dc_pin) {
    // // reset control register
	// SPI1->CTLR1 = 0;

    // // Enable GPIO Port C and SPI peripheral
    // RCC->APB2PCENR |= RCC_APB2Periph_GPIOC | RCC_APB2Periph_SPI1;

    // // PC5 is SCLK
    // GPIOC->CFGLR &= ~(0xf << (4*5));
    // GPIOC->CFGLR |= (GPIO_Speed_50MHz | GPIO_CNF_OUT_PP_AF) << (4*5);

    // // PC6 is MOSI
    // GPIOC->CFGLR &= ~(0xf << (4*6));
    // GPIOC->CFGLR |= (GPIO_Speed_50MHz | GPIO_CNF_OUT_PP_AF) << (4*6);

    // // PC7 is MISO
    // GPIOC->CFGLR &= ~(0xf << (4 * 7));
    // GPIOC->CFGLR |= GPIO_CNF_IN_FLOATING << (4 * 7);

    // // Configure SPI
    // SPI1->CTLR1 |= SPI_CPHA_1Edge | SPI_CPOL_Low
    //             | SPI_Mode_Master| SPI_BaudRatePrescaler_4
    //             | SPI_NSS_Soft | SPI_DataSize_8b;
    
    // SPI1->CTLR1 |= SPI_Direction_2Lines_FullDuplex;
    // // SPI1->CTLR1 |= SPI_Direction_1Line_Tx;

    // SPI1->CTLR1 |= CTLR1_SPE_Set;            // Enable SPI Port

    // //# Reset SPI Devices
    // if (rst_pin != -1) {
    //     funPinMode(rst_pin, GPIO_Speed_10MHz | GPIO_CNF_OUT_PP);
    //     funDigitalWrite(rst_pin, 1);
        
    //     // Reset Spi Devices
    //     funDigitalWrite(rst_pin, 0);
    //     Delay_Ms(100);
    //     funDigitalWrite(rst_pin, 1);
    //     Delay_Ms(100);
    // }
    
    if (dc_pin != -1) {
        // funPinMode(dc_pin, GPIO_Speed_10MHz | GPIO_CNF_OUT_PP);
        SPI_DC_PIN = dc_pin;   
    }
}

static void SPI_DMA_init(DMA_Channel_TypeDef* DMA_Channel) {
    // Enable Tx DMA
    SPI1->CTLR2 |= SPI_I2S_DMAReq_Tx;

    // Enable DMA peripheral
    RCC->AHBPCENR |= RCC_AHBPeriph_DMA1;

    // Configure DMA
    DMA_Channel->PADDR = (uint32_t)&SPI1->DATAR;
    DMA_Channel->CFGR = DMA_M2M_Disable | DMA_Priority_VeryHigh 
                        | DMA_MemoryDataSize_Byte | DMA_PeripheralDataSize_Byte
                        | DMA_MemoryInc_Enable | DMA_PeripheralInc_Disable
                        | DMA_Mode_Circular | DMA_DIR_PeripheralDST;
}

static void SPI_send_DMA(const uint8_t* buffer, uint16_t len) {
    FN_SPI_DC_HIGH();
    
    DMA1_Channel3->CNTR  = len;
    DMA1_Channel3->MADDR = (uint32_t)buffer;

    // Start DMA transfer
    DMA1_Channel3->CFGR |= DMA_CFGR1_EN;  

    DMA1->INTFCR = DMA1_FLAG_TC3;
    while (!(DMA1->INTFR & DMA1_FLAG_TC3));

    // Stop DMA transfer
    DMA1_Channel3->CFGR &= ~DMA_CFGR1_EN;
}

//# write read raw
static inline uint8_t SPI_read_8() {
	return SPI1->DATAR;
}
static inline uint16_t SPI_read_16() {
	return SPI1->DATAR;
}
static inline void SPI_write_8(uint8_t data) {
	SPI1->DATAR = data;
}
static inline void SPI_write_16(uint16_t data) {
	SPI1->DATAR = data;
}

//# write read wait
static inline void SPI_wait_TX_complete() {
    while (!(SPI1->STATR & SPI_STATR_TXE)) { }
}

static inline uint8_t SPI_is_RX_empty() {
    return SPI1->STATR & SPI_STATR_RXNE;
}

static inline void SPI_wait_RX_available() {
    while (!(SPI1->STATR & SPI_STATR_RXNE)) { }
}

static inline void SPI_wait_not_busy() {
    while ((SPI1->STATR & SPI_STATR_BSY) != 0) { }
}


//! INTERFACES
void FN_SPI_DC_LOW() {
    if (SPI_DC_PIN == -1) return;
    funDigitalWrite(SPI_DC_PIN, 0);
}

void FN_SPI_DC_HIGH() {
    if (SPI_DC_PIN == -1) return;
    funDigitalWrite(SPI_DC_PIN, 1);
}

static void SPI_cmd_8(uint8_t cmd) {
    FN_SPI_DC_LOW();
    SPI_write_8(cmd);
    SPI_wait_TX_complete();
}

static void SPI_cmd_data_8(uint8_t data) {
    FN_SPI_DC_HIGH();
    SPI_write_8(data);
    SPI_wait_TX_complete();
}

static void SPI_cmd_data_16(uint16_t data) {
    FN_SPI_DC_HIGH();

    SPI_write_8(data >> 8);
    SPI_wait_TX_complete();
    SPI_write_8(data);
    SPI_wait_TX_complete();
}


static inline void SPI_wait_transmit_finished() {
    SPI_wait_TX_complete();
    SPI_wait_not_busy();
}

void SPI_end() {
    SPI1->CTLR1 &= ~(SPI_CTLR1_SPE);
}

uint8_t SPI_transfer_8(uint8_t data) {
    SPI_write_8(data);
    SPI_wait_TX_complete();
    asm volatile("nop");
    SPI_wait_RX_available();
    return SPI_read_8();
}

uint16_t SPI_transfer_16(uint16_t data) {
    SPI_write_16(data);
    SPI_wait_TX_complete();
    asm volatile("nop");
    SPI_wait_RX_available();
    return SPI_read_16();
}

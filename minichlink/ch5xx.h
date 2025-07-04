#ifndef CH5XXH
#define CH5XXH

#define DEBUG_CH5xx_MINICHLINK 1

#define R8_SAFE_ACCESS_SIG  0x40001040
#define R8_WDOG_COUNT       0x40001043
#define R8_RESET_STATUS     0x40001044
#define R8_GLOB_ROM_CFG     0x40001044
#define R8_GLOB_CFG_INFO    0x40001045
#define R8_RST_WDOG_CTRL    0x40001046
#define R8_GLOB_RESET_KEEP  0x40001047
#define R32_FLASH_DATA      0x40001800
#define R32_FLASH_CONTROL   0x40001804
#define R8_FLASH_DATA       0x40001804
#define R8_FLASH_CTRL       0x40001806
#define R8_FLASH_CFG        0x40001807

void ch5xx_set_clock(void * dev, uint32_t clock);
uint8_t ch5xx_flash_wait(void* dev);
void ch5xx_flash_close(void* dev);
uint8_t ch5xx_flash_open(void* dev, uint8_t op);
int ch5xx_blink(void* dev, uint8_t port, uint8_t pin, uint32_t delay);
uint8_t ch5xx_flash_addr(void* dev, uint8_t cmd, uint32_t addr);
uint8_t ch5xx_flash_in(void* dev);
void ch5xx_flash_out(void* dev, uint8_t addr);
void ch5xx_flash_end(void* dev);
uint8_t ch5xx_flash_begin(void* dev, uint8_t cmd);
void ch5xx_write_byte_safe(void* dev, uint32_t addr, uint8_t value);
void ch5xx_write_word_safe(void* dev, uint32_t addr, uint16_t value);
int ch5xx_read_options(void* dev, uint32_t addr, uint8_t* buffer);
int ch5xx_read_options_bulk(void* dev, uint32_t addr, uint8_t* buffer, uint32_t len);
int ch5xx_read_secret_uuid(void* dev, uint8_t* buffer);
int ch5xx_read_uuid(void* dev, uint8_t* buffer);
int ch5xx_write_flash(void* dev, uint32_t start_addr, uint8_t* data, uint32_t len);
int ch5xx_print_info(void* dev);
int CH5xxErase(void* dev, uint32_t addr, uint32_t len, int type);
int CH5xxWriteBinaryBlob(void* dev, uint32_t address_to_write, uint32_t blob_size, const uint8_t* blob);
int CH5xxReadBinaryBlob(void* dev, uint32_t address_to_read_from, uint32_t read_size, uint8_t* blob);
void CH5xxBlink(void* dev, uint8_t port, uint8_t pin, uint32_t delay);
void CH5xxTestPC(void* dev);
void ch570_disable_acauto(void* dev);
void ch570_disable_read_protection(void* dev);
int ch5xx_verify_data(void* dev, uint32_t addr, uint8_t* data, uint32_t len);

#endif
#ifndef FUNPROG_CH5XX_H
#define FUNPROG_CH5XX_H

#define R8_SAFE_ACCESS_SIG_T     0x40001040
#define R8_WDOG_COUNT_T          0x40001043
#define R8_RESET_STATUS_T        0x40001044
#define R8_GLOB_ROM_CFG_T        0x40001044
#define R8_GLOB_CFG_INFO_T       0x40001045
#define R8_RST_WDOG_CTRL_T       0x40001046
#define R8_GLOB_RESET_KEEP_T     0x40001047
#define R32_FLASH_DATA_T         0x40001800
#define R32_FLASH_CONTROL_T      0x40001804
#define R8_FLASH_DATA_T          0x40001804
#define R8_FLASH_CTRL_T          0x40001806
#define R8_FLASH_CFG_T           0x40001807

#include "funprog_port.h"
#define MCFDelayUS               DELAY_US

#define MCFReadByte              ReadByte
#define MCFReadWord              ReadWord
#define MCFWriteByte             WriteByte
#define MCFWriteWord             WriteWord
#define MCFWaitForDoneOp         WaitForDoneOp
#define MCFDetermineChipType(x)  DetermineChipTypeAndSectorInfo(x, NULL)

int ch5xx_set_clock(struct SWIOState * iss, uint32_t clock);
uint8_t ch5xx_flash_wait(struct SWIOState * iss);
void ch5xx_flash_close(struct SWIOState * iss);
uint8_t ch5xx_flash_open(struct SWIOState * iss, uint8_t op);
uint8_t ch5xx_flash_addr(struct SWIOState * iss, uint8_t cmd, uint32_t addr);
uint8_t ch5xx_flash_in(struct SWIOState * iss);
void ch5xx_flash_out(struct SWIOState * iss, uint8_t addr);
void ch5xx_flash_end(struct SWIOState * iss);
uint8_t ch5xx_flash_begin(struct SWIOState * iss, uint8_t cmd);
void ch5xx_write_safe(struct SWIOState * iss, uint32_t addr, uint32_t value, uint8_t mode);
int ch5xx_read_options(struct SWIOState * iss, uint32_t addr, uint8_t* buffer);
int ch5xx_read_options_bulk(struct SWIOState * iss, uint32_t addr, uint8_t* buffer, uint32_t len);
int ch5xx_write_flash(struct SWIOState * iss, uint32_t start_addr, uint8_t* data, uint32_t len);
int ch5xx_microblob_init(struct SWIOState * iss, uint32_t start_addr);
void ch5xx_microblob_end(struct SWIOState * iss);
int ch5xx_write_flash_using_microblob(struct SWIOState * iss, uint32_t start_addr, uint8_t* data, uint32_t len);
void ch570_disable_acauto(struct SWIOState * iss);
void ch570_disable_read_protection(struct SWIOState * iss);
int ch5xx_verify_data(struct SWIOState * iss, uint32_t addr, uint8_t* data, uint32_t len);
int CH5xxErase(struct SWIOState * iss, uint32_t addr, uint32_t len, enum MemoryArea area);
int ch5xx_microblob_init(struct SWIOState * iss, uint32_t start_addr);

#include "funprog_ch5xx.c"

#endif
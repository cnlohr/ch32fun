#ifndef CHIPS_H
#define CHIPS_H
#include "minichlink.h"

struct RiscVChip_s {
	char name_str[10]; // Name of the chip
	enum RiscVChip family_id; // ChipID[3]
	uint16_t model_id; // ChipID[4-5] & 0xFFF0
	uint32_t sector_size; // For erasing and writing flash
	uint32_t flash_offset; // 0x0800000 on CH32 and 0 on the most of CH5xx
	uint32_t flash_size; // Some chips have configurable flash/RAM size here maximum value will be written
	uint32_t bootloader_offset;
	uint32_t bootloader_size;
	uint32_t options_offset; // Offset of the Option Bytes partition
	uint32_t options_size; // Size in bytes of the Option Bytes partition
	uint32_t eeprom_offset; // EEPROM partition is present on some CH5xx chips
	uint32_t eeprom_size; // EEPROM size
	uint32_t ram_base; // A few old chips have weird RAM offset, the rest have 0x20000000
	uint32_t ram_size; // RAM size in bytes
	uint8_t interface_speed; // WCH-LinkE has 3 speed settings for SWD communcation 6MHz, 4MHz and 400KHz. Some chips can't handle the highest speed
	enum ProgProtocol protocol;	// Currently there are 2 distinct flashing protocols one is for CH32 chips and the other is for CH5xx chips
	uint8_t no_autoexec; // Some chips seem to lack autoexec functionality in DM (3.12.8 in RISC-V External Debug Support manual)
	uint16_t isp_id_mask; // ID to detect model of the chip using ISP bootloader
};

const struct RiscVChip_s* FindChip(uint32_t chip_id);
const struct RiscVChip_s* FindChipISP(uint16_t chip_id);
uint32_t getMemoryEnd(const struct RiscVChip_s * chip, enum MemoryArea area);

extern const struct RiscVChip_s ch32v003;
extern const struct RiscVChip_s ch32v002;
extern const struct RiscVChip_s ch32v004;
extern const struct RiscVChip_s ch32v005;
extern const struct RiscVChip_s ch32v006;
extern const struct RiscVChip_s ch32v007;
extern const struct RiscVChip_s ch32x033;
extern const struct RiscVChip_s ch32x035;
extern const struct RiscVChip_s ch32v103;
extern const struct RiscVChip_s ch32l103;
extern const struct RiscVChip_s ch32v203;
extern const struct RiscVChip_s ch32v205;
extern const struct RiscVChip_s ch32v208;
extern const struct RiscVChip_s ch32v303;
extern const struct RiscVChip_s ch32v305;
extern const struct RiscVChip_s ch32v307;
extern const struct RiscVChip_s ch32v317;
extern const struct RiscVChip_s ch32h415;
extern const struct RiscVChip_s ch32h416;
extern const struct RiscVChip_s ch32h417;
extern const struct RiscVChip_s ch32m030;
extern const struct RiscVChip_s ch564;
extern const struct RiscVChip_s ch564c;
extern const struct RiscVChip_s ch565;
extern const struct RiscVChip_s ch569;
extern const struct RiscVChip_s ch570;
extern const struct RiscVChip_s ch571;
extern const struct RiscVChip_s ch572;
extern const struct RiscVChip_s ch573;
extern const struct RiscVChip_s ch573q;
extern const struct RiscVChip_s ch581;
extern const struct RiscVChip_s ch582;
extern const struct RiscVChip_s ch583;
extern const struct RiscVChip_s ch584;
extern const struct RiscVChip_s ch585;
extern const struct RiscVChip_s ch591;
extern const struct RiscVChip_s ch592;
extern const struct RiscVChip_s ch641;
extern const struct RiscVChip_s ch643;
extern const struct RiscVChip_s ch645;

#endif
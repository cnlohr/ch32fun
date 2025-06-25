#ifndef CHIPS_H
#define CHIPS_H
#include "minichlink.h"



struct RiscVChip_s {
  char name_str[10];
	enum RiscVChip family_id; // ChipID[3]
	uint16_t model_id;  // ChipID[4-5] & 0xFFF0
	uint32_t ram_base;
	uint32_t ram_size;
	uint32_t sector_size;
	uint32_t flash_offset;
	uint32_t flash_size;
	uint32_t bootloader_offset;
	uint32_t bootloader_size;
	uint32_t eeprom_offset;
	uint32_t eeprom_size;
	uint32_t options_offset;
	uint32_t options_size;
	uint8_t interface_speed;
  enum ProgProtocol protocol;
  uint8_t unsupported;
};

const struct RiscVChip_s* FindChip(uint32_t chip_id);

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
extern const struct RiscVChip_s ch32v208;
extern const struct RiscVChip_s ch32v303;
extern const struct RiscVChip_s ch32v305;
extern const struct RiscVChip_s ch32v307;
extern const struct RiscVChip_s ch32v317;
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
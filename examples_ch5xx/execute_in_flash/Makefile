all: flash

TARGET := execute_in_flash
TARGET_MCU := CH582

include ../../ch32fun/ch32fun.mk

# Write blink to DataFlash, then flash and reboot the main app.
FLASH_COMMAND := $(MINICHLINK)/minichlink -w $(TARGET)_blink.bin eeprom && \
	$(MINICHLINK)/minichlink -w $(TARGET).bin flash -b
cv_flash: $(TARGET)_blink.bin

# Build the blink code as a callable function linked at the DataFlash address.
$(TARGET)_blink.elf: blink.c blink_flash.ld funconfig.h
	$(PREFIX)-gcc $(CFLAGS) -fno-lto -nostdlib -Wl,--gc-sections \
		-Wl,-T,blink_flash.ld -o $@ blink.c -lgcc

# Pad to 256 bytes so the whole DataFlash page is programmed.
$(TARGET)_blink.bin: $(TARGET)_blink.elf
	$(PREFIX)-objcopy --pad-to=0x00070100 -O binary $< $@

flash: cv_flash
flash-dataflash: $(TARGET)_blink.bin $(MINICHLINK)/minichlink
	$(MINICHLINK)/minichlink -w $< eeprom

clean: cv_clean
	$(RM) $(TARGET)_blink.elf $(TARGET)_blink.bin

all : flash

TARGET:=extendedflash
TARGET_MCU:=CH32V208
TARGET_MCU_PACKAGE:=CH32V208GB

FLASH_COMMAND=$(MINICHLINK)/minichlink -w $(TARGET).bin flash && $(MINICHLINK)/minichlink -w $(TARGET)_ext.bin 0x08020000 -b

include ../../ch32fun/ch32fun.mk

flash : cv_flash
clean : cv_clean
	rm -f $(TARGET)_ext.bin


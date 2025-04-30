all : runfromram

TARGET:=app_without_flash
TARGET_MCU:=CH32V003
#TARGET_MCU:=CH582
#TARGET_MCU_PACKAGE:=CH582F

LINKER_SCRIPT:=app_in_ram.ld

include ../../ch32fun/ch32fun.mk

flash : runfromram

clean : cv_clean

runfromram : $(TARGET).bin
	$(MINICHLINK)/minichlink -w $(TARGET).bin 0x20000000
	$(MINICHLINK)/minichlink -s 0x18 0 # Disable auto execution
	$(MINICHLINK)/minichlink -s 0x04 0x20000000 # Write DATA0 = RAM start
	$(MINICHLINK)/minichlink -s 0x17 0x002307b1 # Write DATA0 to DPC
	$(MINICHLINK)/minichlink -s 0x10 0x40000001 -T # Resume Request

flash : cv_flash
clean : cv_clean


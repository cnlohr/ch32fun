all : runfromram

TARGET:=app_without_flash
TARGET_MCU:=CH32V003
# TARGET_MCU:=CH582
# TARGET_MCU_PACKAGE:=CH582F

LINKER_SCRIPT:=app_in_ram.ld

include ../../ch32fun/ch32fun.mk

flash : runfromram

clean : cv_clean

# Halt request, full halt request, disable autoexecution, write to RAM, set EPC=start of ram, issue resume request.
# Optionally, you could put a -T here to immediately terminal.

runfromram : $(TARGET).bin
	$(MINICHLINK)/minichlink -ks 0x10 0x80000001 -s 0x10 0x80000003 -s 0x10 0x80000001 -s 0x18 0 -w $(TARGET).bin 0x20000000 -s 0x04 0x20000000 -s 0x17 0x002307b1 -s 0x10 0x40000001 -T


flash : cv_flash
clean : cv_clean


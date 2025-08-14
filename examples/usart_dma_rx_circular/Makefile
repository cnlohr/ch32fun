all : flash_openocd

TARGET:=usart_dma_rx_circular

TARGET_MCU?=CH32V003
include ../../ch32fun/ch32fun.mk

flash : cv_flash
clean : cv_clean

MRS_OPENOCD_PATH:=A:/prog/MounRiver/MounRiver_Studio2/resources/app/resources/win32/components/WCH/OpenOCD/OpenOCD/bin
flash_openocd: $(TARGET).elf
	$(MRS_OPENOCD_PATH)/openocd.exe -f $(MRS_OPENOCD_PATH)/wch-riscv.cfg -c "program $(TARGET).elf verify reset exit"
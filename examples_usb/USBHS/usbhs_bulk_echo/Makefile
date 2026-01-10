all : flash

TARGET:=usbhs_bulk_echo
TARGET_MCU:=CH585
TARGET_MCU_PACKAGE:=CH585M

include ../../../ch32fun/ch32fun.mk

bootloader :
	./$(TARGET).py -b

flash : bootloader cv_flash
clean : cv_clean

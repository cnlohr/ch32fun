all : flash

TARGET:=flash_write

TARGET_MCU:=CH32V305
TARGET_MCU_PACKAGE:=CH32V305RBT6

include ../../ch32fun/ch32fun.mk

monitor :
	$(MINICHLINK)/minichlink -aT

flash : cv_flash
clean : cv_clean


all : flash

TARGET:=aes
TARGET_MCU:=CH592
TARGET_MCU_PACKAGE:=CH592F

all : $(TARGET).bin

include ../../ch32fun/ch32fun.mk

flash : cv_flash
clean : cv_clean

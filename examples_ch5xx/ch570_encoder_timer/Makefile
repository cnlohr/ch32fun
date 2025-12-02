all : flash

TARGET:=ch570_encoder_timer
TARGET_MCU:=CH570
TARGET_MCU_PACKAGE:=CH570D

all : $(TARGET).bin

include ../../ch32fun/ch32fun.mk

flash : cv_flash
clean : cv_clean

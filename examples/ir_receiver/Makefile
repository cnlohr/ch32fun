all : flash

TARGET:=ir_receiver
TARGET_EXT:=cpp

ADDITIONAL_C_FILES+=NecIrDecoder.cpp

TARGET_MCU?=CH32V003
include ../../ch32fun/ch32fun.mk

CFLAGS+=-fno-rtti -DCPLUSPLUS

flash : cv_flash
clean : cv_clean

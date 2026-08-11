all : flash

TARGET:=usbd_cdc_tty
TARGET_MCU:=CH32V208
TARGET_MCU_PACKAGE:=CH32V208WBU6

ADDITIONAL_C_FILES += ../../../extralibs/usbd.c
include ../../../ch32fun/ch32fun.mk

flash : cv_flash
clean : cv_clean

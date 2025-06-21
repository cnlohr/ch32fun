all : flash

TARGET:=spi_lsm6dsv32x

TARGET_MCU?=CH32V203
ADDITIONAL_C_FILES:=lsm6dsv32x-pid/lsm6dsv32x_reg.c
include ../../ch32fun/ch32fun.mk

flash : cv_flash
clean : cv_clean


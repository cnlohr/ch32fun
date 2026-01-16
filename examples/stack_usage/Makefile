all : flash

TARGET:=stack_usage

TARGET_MCU?=CH32V003
# TARGET_MCU:=CH570
# TARGET_MCU_PACKAGE:=CH570D
include ../../ch32fun/ch32fun.mk

CFLAGS += -fno-omit-frame-pointer -fno-optimize-sibling-calls

flash : cv_flash
clean : cv_clean


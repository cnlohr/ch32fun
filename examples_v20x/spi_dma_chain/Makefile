all : flash

TARGET:=spi_dma_chain
TARGET_MCU:=CH32V203
# TARGET_MCU:=CH32V307
# TARGET_MCU:=CH32V103
ENABLE_FPU:=0
include ../../ch32fun/ch32fun.mk

CFLAGS+=-Wno-switch -Wno-attributes -fno-ipa-icf-variables -fno-ipa-icf-functions
# CFLAGS+=-O0

flash : cv_flash
clean : cv_clean

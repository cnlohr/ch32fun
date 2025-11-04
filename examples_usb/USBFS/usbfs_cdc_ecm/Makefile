
TARGET:=usbfs_cdc_ecm
# TARGET_MCU:=CH32X035
# TARGET_MCU:=CH592
# TARGET_MCU_PACKAGE:=CH592F
# TARGET_MCU:=CH32V103
# TARGET_MCU:=CH32V203
# TARGET_MCU_PACKAGE:=CH32V203F8
TARGET_MCU:=CH32V208
TARGET_MCU_PACKAGE:=CH32V208WBU6
# TARGET_MCU:=CH32V307
ENABLE_FPU:=0
ADDITIONAL_C_FILES:=

FSPATH := uip
FSFILES := $(shell find $(FSPATH)/fs/ -type f)

UIPFILES := $(filter-out $(FSPATH)/fsdata.c, $(wildcard uip/*.c))

ADDITIONAL_C_FILES += $(filter-out $(TARGET).c, $(wildcard *.c))
ADDITIONAL_C_FILES += $(UIPFILES)
EXTRA_CFLAGS := -I./uip

all : $(FSPATH)/fsdata.c flash

include ../../../ch32fun/ch32fun.mk
CFLAGS+=-D$(TARGET_MCU_PACKAGE)

$(FSPATH)/fsdata.c: $(FSFILES)
	@echo "Building filesystem ..."
	cd $(FSPATH) && ./makefsdata

flash : cv_flash
clean : cv_clean


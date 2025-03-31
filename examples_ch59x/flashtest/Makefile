all : flash

TARGET:=flashtest
TARGET_MCU:=CH592
TARGET_MCU_PACKAGE:=CH592F

include ../../ch32fun/ch32fun.mk
LDFLAGS+=-L. -lISP5xx

flash : cv_flash
clean : cv_clean

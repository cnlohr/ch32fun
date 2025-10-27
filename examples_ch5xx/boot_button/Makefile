all : flash

TARGET:=boot_button
TARGET_MCU:=CH570 # only 582/3, 584/5 ch59x and 570/2 are supported currently
TARGET_MCU_PACKAGE:=CH570D

include ../../ch32fun/ch32fun.mk

flash : cv_flash
clean : cv_clean

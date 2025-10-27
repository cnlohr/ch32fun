all : flash

TARGET:=usbhs_device
TARGET_MCU:=CH585
TARGET_MCU_PACKAGE:=CH585F
# TARGET_MCU:=CH32V307
ENABLE_FPU:=0

ADDITIONAL_C_FILES:=
include ../../../ch32fun/ch32fun.mk

install_udev_rules : 
	sudo cp 99-v203demo.rules /etc/udev/rules.d/
	sudo udevadm control --reload
	sudo udevadm trigger

flash : cv_flash
clean : cv_clean


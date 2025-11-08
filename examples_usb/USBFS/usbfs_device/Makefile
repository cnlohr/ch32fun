all : flash

TARGET:=usbfs_device
# TARGET_MCU:=CH32X035
# TARGET_MCU:=CH32V103
TARGET_MCU:=CH32V203
# TARGET_MCU:=CH32V307
# TARGET_MCU:=CH582
# TARGET_MCU_PACKAGE:=CH582F
include ../../../ch32fun/ch32fun.mk

install_udev_rules : 
	sudo cp ./99-v203demo.rules /etc/udev/rules.d/
	sudo udevadm control --reload
	sudo udevadm trigger

flash : cv_flash
clean : cv_clean


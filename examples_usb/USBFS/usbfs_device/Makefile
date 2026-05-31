all : flash

TARGET:=usbfs_device
# TARGET_MCU:=CH32X035
# TARGET_MCU:=CH32V103
TARGET_MCU:=CH32V203
# TARGET_MCU:=CH32V307
# TARGET_MCU:=CH582
# TARGET_MCU_PACKAGE:=CH582F
# TARGET_MCU:=CH570
include ../../../ch32fun/ch32fun.mk

install_udev_rules : 
	sudo cp ./99-v203demo.rules /etc/udev/rules.d/
	sudo udevadm control --reload
	sudo udevadm trigger

# Below this is largely designed around the CH5xx

go_into_bootloader :
	(cd usbhidtool && make go_into_bootloader && cd .. )

userreflash : go_into_bootloader flash

usermonitor :
	(cd usbhidtool && make usermonitor && cd .. )

flash : cv_flash
clean : cv_clean


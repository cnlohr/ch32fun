all : flash

TARGET:=usbfs_cdc_uart
# TARGET_MCU:=CH32X035
# TARGET_MCU:=CH592
# TARGET_MCU_PACKAGE:=CH592F
TARGET_MCU:=CH32V203
# TARGET_MCU_PACKAGE:=CH32V208WBU6
ENABLE_FPU:=0
ADDITIONAL_C_FILES:=
include ../../../ch32fun/ch32fun.mk
# CFLAGS+=-MD
install_udev_rules : 
	sudo cp ../../../examples_usb/USBFS/usbfs_cdc_uart/99-ch32fun.rules /etc/udev/rules.d/
	sudo udevadm control --reload
	sudo udevadm trigger


flash : cv_flash
clean : cv_clean


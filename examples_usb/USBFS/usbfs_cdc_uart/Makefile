all : flash

TARGET:=usbfs_cdc_uart
# TARGET_MCU:=CH32X035
# TARGET_MCU:=CH32V103
# TARGET_MCU:=CH32V203
# TARGET_MCU_PACKAGE:=CH32V203F8
# TARGET_MCU:=CH32V208
# TARGET_MCU_PACKAGE:=CH32V208WBU6
# TARGET_MCU:=CH32V307
TARGET_MCU:=CH570
TARGET_MCU_PACKAGE:=CH570D
# TARGET_MCU:=CH592
# TARGET_MCU_PACKAGE:=CH592F
ENABLE_FPU:=0
ADDITIONAL_C_FILES:=
include ../../../ch32fun/ch32fun.mk
CFLAGS+=-D$(TARGET_MCU_PACKAGE)
install_udev_rules : 
	sudo cp ../../../examples_usb/USBFS/usbfs_cdc_uart/99-ch32fun.rules /etc/udev/rules.d/
	sudo udevadm control --reload
	sudo udevadm trigger


flash : cv_flash
clean : cv_clean


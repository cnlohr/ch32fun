# USBFS exapmles

This folder contains examples that use USBFS peripheral, the most common implementation among all WCH's RiscV chips. On some models it also called USBOTG, but it's basically the same peripheral + extra register for OTG functionality.

## USBFS pinout

|   |CH32X035|CH32V103|CH32L103|CH32V20x|CH32V30x|CH570/2|CH571/3|CH582/3|CH584/5|CH591/2|
|:-:|:-:     |:-:     |:-:     |:-:     |:-:     |:-:    |:-:    |:-:    |:-:    |:-:    |
|D+ |PC17    |PA12    |PA12    |B7      |PA12    |PA1    |PB11   |PB11   |PB11   |PB11   |
|D- |PC16    |PA11    |PA11    |B6      |PA11    |PA0    |PB10   |PB10   |PB10   |PB10   |

## Library

Examples in this folder use a simple low-level library ``fsusb.h`` that can be found in ``ch32/extralibs`` folder. Every example has ``usb_config.h`` file in addition to the common set of files found in other examples. To compile an example for your specific MCU model you need to change ``TARGET_MCU`` variable in the ``Makefile``. For some chips you should also specify the exact model in ``TARGET_MCU_PACKAGE`` variable, for example ``TARGET_MCU_PACKAGE:=CH32V203F8``. This will enable different settings if needed.

## Linux UDEV rules

To be able to use your device in linux in some cases you will need to install a UDEV rule that will apply permissions for the device based on its VID:PID. Basic rule file for the default ch32fun VID:PID pair can be found with every example. To install it you need to copy that file to ``/etc/udev/rules.d/`` directory of your linux install, or you can use ``make install_udev_rules`` command from inside an example directory.
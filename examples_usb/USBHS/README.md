# USBHS examples

This folder contains examples that use USBHS peripheral. This is a list of chips that have this peripheral:

- CH32V205
- CH32V305
- CH32V307
- CH32V317
- CH32H415
- CH32H416
- CH32H417
- CH565
- CH569
- CH584
- CH585

## USBHS pinout

|   |CH32V205|CH32V30x|CH32H41x|CH565/9|CH584/5|
|:-:|:-:     |:-:     |:-:     |:-:    |:-:    |
|D+ |PB6     |PB7     |PB8     |UD+    |PB13   |
|D- |PB7     |PB6     |PB9     |UD-    |PB12   |

## Library

Examples in this folder use a simple low-level library ``hsusb.h`` that can be found in ``ch32/extralibs`` folder. Every example has ``usb_config.h`` file in addition to the common set of files found in other examples. To compile an example for your specific MCU model you need to change ``TARGET_MCU`` variable in the ``Makefile``. For some chips you should also specify the exact model in ``TARGET_MCU_PACKAGE`` variable, for example ``TARGET_MCU_PACKAGE:=CH585F``. This will enable different settings if needed.

## Linux UDEV rules

To be able to use your device in linux in some cases you will need to install a UDEV rule that will apply permissions for the device based on its VID:PID. Basic rule file for the default ch32fun VID:PID pair can be found with every example. To install it you need to copy that file to ``/etc/udev/rules.d/`` directory of your linux install, or you can use ``make install_udev_rules`` command from inside an example directory.

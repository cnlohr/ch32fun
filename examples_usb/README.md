# Universal USB examples

This folder contains examples for different USB peripherals that can be found in WCH RiscV chips. Currently there are 3 implementations of USB 2.0 protocol in WHC's hardware: USBFS, USBD and USBHS. The most common of them is USBFS - it can be found in all chips that have hardware USB. It can be used in a device mode and in most cases also in host mode. USBD can only be used in device mode and it has different impelemtation. USBHS (USBHD) is only present on CH32V305/307/317 and also on CH585.

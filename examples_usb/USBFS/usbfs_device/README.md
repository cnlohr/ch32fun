# USBFS test device

This is a simple example of basic USBFS functionality. It creates two HID devices - a mouse and a keyboard. *Keyboard* send 3 presses of ``8`` and *mouse* slowly crawls the cursor diagonally to the bottom right corner of a screen.

To test HIDAPI transfers you can use a ``testtop`` program that can be found in a corresponding subdirectory.

## HIDAPI performance

This example was tested on CH32X035, CH32V103, CH32V203, CH32V208, CH32V307, CH570, CH573, CH582, CH585 and CH592. It should also work on all other WCH's MCUs with a USB peripheral. Different MCU models showed different performance using HIDAPI for data transfer. The fastest are v20x and v30x families. CH32V203 showed unexpected quirk, where it was failing most of HIDAPI transfers if mouse/keyboard EPs were sending their data. After increasing polling interval for those in USB descriptor ``testtop`` experienced only small number of errors, while *mouse* was still sending data. This behavior was seen only on CH32V203, which is very odd and I couldn't find where the issue may be.

Another observation worth noting is that ``testtop`` showed different performance on different PCs. On one it was visibly capped by something at 125KB/s. More tests needed to find explanation for this behavior.

## Notes about performance on CH5xx

CH5xx don't have a cached flash like CH32. To achieve comparable performance EVT puts all critical functions into RAM. This increases performance significantly, but it's still worse than on CH32 models. To enable this you can use ``FUSB_FROM_RAM`` option that will put all relevant functions to RAM.

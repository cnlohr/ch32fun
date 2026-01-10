#!/usr/bin/env python
"""
requires pyusb, which should be pippable
SUBSYSTEM=="usb", ATTR{idVendor}=="1209", ATTR{idProduct}=="d035", MODE="666"
sudo udevadm control --reload-rules && sudo udevadm trigger
"""
import os
import argparse
import usb.core
import usb.util
from timeit import default_timer as timer

CH_USB_VENDOR_ID    = 0x1209    # VID
CH_USB_PRODUCT_ID   = 0xd035    # PID
CH_USB_INTERFACE    = 0         # interface number
CH_USB_EP_IN        = 0x81      # endpoint for reply transfer in
CH_USB_EP_OUT       = 0x02      # endpoint for command transfer out
CH_USB_PACKET_SIZE  = 256       # packet size
CH_USB_TIMEOUT_MS   = 2000      # timeout for USB operations

CH_CMD_REBOOT       = 0xa2
CH_STR_REBOOT       = (CH_CMD_REBOOT, 0x01, 0x00, 0x01)

# Find the device
device = usb.core.find(idVendor=CH_USB_VENDOR_ID, idProduct=CH_USB_PRODUCT_ID)

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('-b', '--bootloader', help='Reboot to bootloader', action='store_true')
    args = parser.parse_args()

    if device is None:
        print("MCU not found")
        exit(0)

    if args.bootloader:
        print('rebooting to bootloader')
        bootloader()
    else:
        for _ in range(5):
            echo()
    
    print('done')

def bootloader():
    device.write(CH_USB_EP_OUT, CH_STR_REBOOT)

def echo():
    buf = os.urandom(4)
    start = timer()
    device.write(CH_USB_EP_OUT, buf)
    r = device.read(CH_USB_EP_IN, CH_USB_PACKET_SIZE, CH_USB_TIMEOUT_MS)
    end = timer()
    print(f'echo {bytes(r).hex(':')} (chk:{bytes(r) == buf}) took {end - start} seconds')


if __name__ == '__main__':
    main()

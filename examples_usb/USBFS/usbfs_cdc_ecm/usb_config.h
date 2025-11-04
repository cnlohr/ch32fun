#ifndef _USB_CONFIG_H
#define _USB_CONFIG_H

#include "funconfig.h"
#include "ch32fun.h"

#define FUSB_CONFIG_EPS       4 // Include EP0 in this count
#define FUSB_EP1_MODE         1 // TX
#define FUSB_EP2_MODE        -1 // RX
#define FUSB_EP3_MODE         1 // TX
#define FUSB_SUPPORTS_SLEEP   0
#define FUSB_HID_INTERFACES   0
#define FUSB_CURSED_TURBO_DMA 0 // Hacky, but seems fine, shaves 2.5us off filling 64-byte buffers.
#define FUSB_HID_USER_REPORTS 0
#define FUSB_IO_PROFILE       0
#define FUSB_USE_HPE          FUNCONF_ENABLE_HPE
#define FUSB_USER_HANDLERS    1
#define FUSB_USE_DMA7_COPY    0
#define FUSB_VDD_5V           FUNCONF_USE_5V_VDD

#include "usb_defines.h"

#define FUSB_USB_VID 0x1209
#define FUSB_USB_PID 0xd035
#define FUSB_USB_REV 0x0007
#define FUSB_STR_MANUFACTURER u"ch32fun"
#define FUSB_STR_PRODUCT      u"USB CDC-ECM"
#define FUSB_STR_SERIAL       u"00229708A003" // MAC

//Taken from http://www.usbmadesimple.co.uk/ums_ms_desc_dev.htm
static const uint8_t device_descriptor[] = {
	18, //bLength - Length of this descriptor
	1,  //bDescriptorType - Type (Device)
	0x10, 0x01, //bcdUSB - The highest USB spec version this device supports (USB1.1)
	0x02, //bDeviceClass - Device Class
	0x0, //bDeviceSubClass - Device Subclass
	0x0, //bDeviceProtocol - Device Protocol  (000 = use config descriptor)
	64, //bMaxPacketSize - Max packet size for EP0
   (uint8_t)(FUSB_USB_VID), (uint8_t)(FUSB_USB_VID >> 8), //idVendor - ID Vendor
	(uint8_t)(FUSB_USB_PID), (uint8_t)(FUSB_USB_PID >> 8), //idProduct - ID Product
	(uint8_t)(FUSB_USB_REV), (uint8_t)(FUSB_USB_REV >> 8), //bcdDevice - Device Release Number
	1, //iManufacturer - Index of Manufacturer string
	2, //iProduct - Index of Product string
	3, //iSerialNumber - Index of Serial string
	1, //bNumConfigurations - Max number of configurations (if more then 1, you can switch between them)
};

/* Configuration Descriptor Set */
static const uint8_t config_descriptor[ ] =
{
  0x09,        // bLength
  0x02,        // bDescriptorType (Configuration)
  0x47, 0x00,  // wTotalLength - 'riscv64-unknown-elf-objdump -t *.elf | grep "config_descriptor"'
  0x02,        // bNumInterfaces 2
  0x01,        // bConfigurationValue
  0x00,        // iConfiguration (String Index)
  0x80,        // bmAttributes
  0x32,        // bMaxPower 100mA

  0x09,        // bLength
  0x04,        // bDescriptorType - Interface
  0x00,        // bInterfaceNumber - 0
  0x00,        // bAlternateSetting
  0x01,        // bNumEndpoints - 1
  0x02,        // bInterfaceClass - CDC
  0x06,        // bInterfaceSubClass - Ethernet Control Model
  0x00,        // bInterfaceProtocol - None 
  0x00,        // iInterface (String Index)

  // Setting up CDC interface (Table 18)
  0x05,        // bLength
  0x24,        // bDescriptorType - CS_INTERFACE (Table 12)
  0x00,        // bDescriptorSubType - Header Functional Descriptor (Table 13)
  0x10, 0x01,  // bcdCDC - USB version - USB1.1
  
  // Union Descriptor Functional Descriptor
  0x05,        // bLength
  0x24,        // bDescriptorType - CS_INTERFACE
  0x06,        // bDescriptorSubType - Union Descriptor Functional Descriptor (Table 13)
  0x00,        // bControlInterface (Interface number of the control (Communications Class) interface)
  0x01,        // bSubordinateInterface0 (Interface number of the subordinate (Data Class) interface)

  // Ethernet Networking Functional descriptor
  0x0D,                // bLength - 13 bytes
  0x24,                // bDescriptortype, CS_INTERFACE
  0x0F,                // bDescriptorsubtype, ETHERNET NETWORKING
  0x03,                // iMACAddress, Index of MAC address string
  0x00,0x00,0x00,0x00, // bmEthernetStatistics - Handles None
#if 1
  0xEA,05,             // wMaxSegmentSize - 1514 bytes
#else
  0x40,00,             // wMaxSegmentSize - 64 bytes
#endif
  0x00,0x00,           // wNumberMCFilters - No multicast filters
  0x00,                // bNumberPowerFilters - No wake-up feature
   
  // Notification Endpoint descriptor
  0x07,       // bLength
  0x05,       // bDescriptorType - Endpoint
  0x81,       // bEndpointAddress - IN endpoint
  0x03,       // bmAttributes - Interrupt type
  0x40, 0x00, // wMaxPacketSize 64
  0xFF,       // bInterval

  // Transmission interface with two bulk endpoints
  0x09,        // bLength
  0x04,        // bDescriptorType (Interface)
  0x01,        // bInterfaceNumber 1
  0x00,        // bAlternateSetting
  0x02,        // bNumEndpoints 2
  0x0A,        // bInterfaceClass
  0x00,        // bInterfaceSubClass
  0x00,        // bInterfaceProtocol - Transparent
  0x00,        // iInterface (String Index)
  // EP2 OUT - host to device
  0x07,        // bLength
  0x05,        // bDescriptorType (Endpoint)
  0x02,        // bEndpointAddress (OUT/H2D)
  0x02,        // bmAttributes (Bulk)
  0x40, 0x00,  // wMaxPacketSize 64
  0x00,        // bInterval 0 (unit depends on device speed)
  // EP3 IN - device to host
  0x07,        // bLength
  0x05,        // bDescriptorType (Endpoint)
  0x83,        // bEndpointAddress (IN/D2H)
  0x02,        // bmAttributes (Bulk)
  0x40, 0x00,  // wMaxPacketSize 64
  0x00,        // bInterval 0 (unit depends on device speed)

  // 67 bytes
};

struct usb_string_descriptor_struct {
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint16_t wString[];
};
const static struct usb_string_descriptor_struct language __attribute__((section(".rodata"))) = {
	4,
	3,
	{0x0409}  // Language ID - English US (look in USB_LANGIDs)
};
const static struct usb_string_descriptor_struct string1 __attribute__((section(".rodata")))  = {
	sizeof(FUSB_STR_MANUFACTURER),
	3,  // bDescriptorType - String Descriptor (0x03)
	FUSB_STR_MANUFACTURER
};
const static struct usb_string_descriptor_struct string2 __attribute__((section(".rodata")))  = {
	sizeof(FUSB_STR_PRODUCT),
	3,
	FUSB_STR_PRODUCT
};
const static struct usb_string_descriptor_struct string3 __attribute__((section(".rodata")))  = {
	sizeof(FUSB_STR_SERIAL),
	3,
	FUSB_STR_SERIAL
};

// This table defines which descriptor data is sent for each specific
// request from the host (in wValue and wIndex).
const static struct descriptor_list_struct {
	uint32_t	lIndexValue;  // (uint16_t)Index of a descriptor in config or Language ID for string descriptors | (uint8_t)Descriptor type | (uint8_t)Type of string descriptor
	const uint8_t	*addr;
	uint8_t		length;
} descriptor_list[] = {
	{0x00000100, device_descriptor, sizeof(device_descriptor)},
	{0x00000200, config_descriptor, sizeof(config_descriptor)},
	// {0x00002100, config_descriptor + 18, 9 }, // Not sure why, this seems to be useful for Windows + Android.

	{0x00000300, (const uint8_t *)&language, 4},
	{0x04090301, (const uint8_t *)&string1, string1.bLength},
	{0x04090302, (const uint8_t *)&string2, string2.bLength},
	{0x04090303, (const uint8_t *)&string3, string3.bLength}
};
#define DESCRIPTOR_LIST_ENTRIES ((sizeof(descriptor_list))/(sizeof(struct descriptor_list_struct)) )


#endif


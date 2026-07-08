#ifndef _USB_CONFIG_H
#define _USB_CONFIG_H

#include "funconfig.h"
#include "ch32fun.h"

#define FUSB_CONFIG_EPS       4 // Include EP0 in this count
#define FUSB_EP1_MODE         1 // TX (IN)
#define FUSB_EP2_MODE        -1 // RX (OUT)
#define FUSB_EP3_MODE         1 // TX (IN)
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
#define FUSB_STR_PRODUCT      u"USB-UART link"
#define FUSB_STR_SERIAL       u"007"

//Taken from http://www.usbmadesimple.co.uk/ums_ms_desc_dev.htm
static const tusb_desc_device_t device_descriptor = {
  .bLength = sizeof(tusb_desc_device_t),
  .bDescriptorType = TUSB_DESC_DEVICE,
  .bcdUSB = 0x0110,
  .bDeviceClass = TUSB_CLASS_CDC,
  .bDeviceSubClass = 0,
  .bDeviceProtocol = 0,
  .bMaxPacketSize0 = 64,
  .idVendor = FUSB_USB_VID,
  .idProduct = FUSB_USB_PID,
  .bcdDevice = FUSB_USB_REV,
  .iManufacturer = 1,
  .iProduct = 2,
  .iSerialNumber = 3,
  .bNumConfigurations = 1,
};

/* Configuration Descriptor Set */
static const struct {
  tusb_desc_configuration_t cfg;
  tusb_desc_interface_t if0;
  csInterfaceDescriptor csif0;
  cdcInterfaceDescriptor cdcif0;
  acmInterfaceDescriptor acm0;
  unionFunctionalDescriptor union0;
  tusb_desc_endpoint_t ep1in;
  tusb_desc_interface_t if1;
  tusb_desc_endpoint_t ep2out;
  tusb_desc_endpoint_t ep3;
} config_descriptor = {
  .cfg = {
    .bLength = sizeof(tusb_desc_configuration_t),
    .bDescriptorType = TUSB_DESC_CONFIGURATION,
    .wTotalLength = sizeof(config_descriptor),
    .bNumInterfaces = 2,
    .bConfigurationValue = 1,
    .bmAttributes = 0x80, // TODO, use enum
    .bMaxPower = 100 / 2,
  },
  .if0 = {
    .bLength = sizeof(tusb_desc_interface_t),
    .bDescriptorType = TUSB_DESC_INTERFACE,
    .bInterfaceNumber = 0,
    .bAlternateSetting = 0,
    .bNumEndpoints = 1,
    .bInterfaceClass = TUSB_CLASS_CDC,
    .bInterfaceSubClass = 2, // Abstract Control Model (Table 4 in CDC120.pdf)
    .bInterfaceProtocol = 1, // AT Commands: V.250 etc (Table 5)
    .iInterface = 0,
  },
  // Setting up CDC interface (Table 18)
  .csif0 = {
    .bLength = sizeof(csInterfaceDescriptor),
    .bDescriptorType = TUSB_DESC_CS_INTERFACE, // (Table 12)
    .bDescriptorSubType = 0, // Header Functional Descriptor (Table 13)
    .bcdCDC = 0x0110,        // USB version - USB1.1
  },
  // Call Management Functional Descriptor
  .cdcif0 = {
    .bLength = sizeof(cdcInterfaceDescriptor),
    .bDescriptorType = TUSB_DESC_CS_INTERFACE,
    .bDescriptorSubType = 1, // Call Management Functional Descriptor (Table 13)
    // Bit 0 — Device handles call management itself:
    //  1 = device handles call management (e.g. call setup, termination, etc.)
    //  0 = host handles it
    // Bit 1 — Device can send/receive call management information over a Data Class interface:
    //  1 = can use the Data Class interface for call management
    //  0 = must use the Communication Class interface
    .bmCapabilities = 0, // (Table 3 in PSTN120.pdf)
    .bDataInterface = 1, // Indicates that multiplexed commands are handled via data interface 01h (same value as used in the UNION Functional Descriptor)
  },
  .acm0 = {
    .bLength = sizeof(acmInterfaceDescriptor),
    .bDescriptorType = TUSB_DESC_CS_INTERFACE,
    .bDescriptorSubType = 2, // Abstract Control Management Functional Descriptor (Table 13)
    .bmAttributes = 0x02,    // Device supports the request combination of Set_Line_Coding, Set_Control_Line_State, Get_Line_Coding, and the notification Serial_State (Table 4 in PSTN120.pdf)
  },
  // Union Descriptor Functional Descriptor
  .union0 = {
    .bLength = sizeof(unionFunctionalDescriptor),
    .bDescriptorType = TUSB_DESC_CS_INTERFACE,
    .bDescriptorSubType = 6, // Union Descriptor Functional Descriptor (Table 13)
    .bControlInterface = 0,  // (Interface number of the control (Communications Class) interface)
    .bSubordinateInterface0 = 1, // (Interface number of the subordinate (Data Class) interface)
  },
  // Setting up EP1 for CDC config interface
  .ep1in = {
    .bLength = sizeof(tusb_desc_endpoint_t),
    .bDescriptorType = TUSB_DESC_ENDPOINT,
    .bEndpointAddress = 0x81,
    .bmAttributes.xfer = 3, // interrupt
    .wMaxPacketSize.size = 64,
    .bInterval = 1,
  },
  // Transmission interface with two bulk endpoints
  .if1 = {
    .bLength = sizeof(tusb_desc_interface_t),
    .bDescriptorType = TUSB_DESC_INTERFACE,
    .bInterfaceNumber = 1,
    .bAlternateSetting = 0,
    .bNumEndpoints = 2,
    .bInterfaceClass = TUSB_CLASS_CDC_DATA,
    .bInterfaceSubClass = 0,
    .bInterfaceProtocol = 0, // Transparent
    .iInterface = 0,
  },
  // EP2 - device to host
  .ep2out = {
    .bLength = sizeof(tusb_desc_endpoint_t),
    .bDescriptorType = TUSB_DESC_ENDPOINT,
    .bEndpointAddress = 0x02,
    .bmAttributes.xfer = 0x02, // bulk
    .wMaxPacketSize.size = 64,
    .bInterval = 0,
  },
  // EP3 - host to device
  .ep3 = {
    .bLength = sizeof(tusb_desc_endpoint_t),
    .bDescriptorType = TUSB_DESC_ENDPOINT,
    .bEndpointAddress = 0x83,
    .bmAttributes.xfer = 0x02, // bulk
    .wMaxPacketSize.size = 64,
    .bInterval = 0,
  },
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
	{0x00000100, (const uint8_t*)&device_descriptor, sizeof(device_descriptor)},
	{0x00000200, (const uint8_t*)&config_descriptor, sizeof(config_descriptor)},
	// {0x00002100, config_descriptor + 18, 9 }, // Not sure why, this seems to be useful for Windows + Android.

	{0x00000300, (const uint8_t *)&language, 4},
	{0x04090301, (const uint8_t *)&string1, string1.bLength},
	{0x04090302, (const uint8_t *)&string2, string2.bLength},
	{0x04090303, (const uint8_t *)&string3, string3.bLength}
};
#define DESCRIPTOR_LIST_ENTRIES ((sizeof(descriptor_list))/(sizeof(struct descriptor_list_struct)) )


#endif


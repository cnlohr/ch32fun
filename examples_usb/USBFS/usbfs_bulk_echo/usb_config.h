#ifndef _USB_CONFIG_H
#define _USB_CONFIG_H

#include "funconfig.h"
#include "ch32fun.h"

#define FUSB_CONFIG_EPS       3 // Include EP0 in this count
#define FUSB_EP1_MODE         1 // TX (OUT)
#define FUSB_EP2_MODE         -1 // RX (IN)
#define USB_EP_TX             1
#define USB_EP_RX             2
#define FUSB_SUPPORTS_SLEEP   0
#define FUSB_IO_PROFILE       0
#define FUSB_USE_HPE          FUNCONF_ENABLE_HPE
#define FUSB_EP_SIZE          64
#define FUSB_SPEED            USB_SPEED_HIGH
#define FUSB_USER_HANDLERS    1 // To enable HandleDataOut
#define FUSB_OUT_FLOW_CONTROL 0 // 0: auto ack

#include "usb_defines.h"

#define FUSB_USB_VID          0x1209
#define FUSB_USB_PID          0xd035
#define FUSB_USB_REV          0x0007
#define FUSB_STR_MANUFACTURER u"ch32fun"
#define FUSB_STR_PRODUCT      u"Bulk demo"
#define FUSB_STR_SERIAL       u"585"

//Taken from http://www.usbmadesimple.co.uk/ums_ms_desc_dev.htm
static const uint8_t device_descriptor[] = {
	18, //Length
	1,  //Type (Device)
	0x00, 0x02, //Spec
	0x0, //Device Class
	0x0, //Device Subclass
	0x0, //Device Protocol  (000 = use config descriptor)
	64, //Max packet size for EP0
	(uint8_t)(FUSB_USB_VID), (uint8_t)(FUSB_USB_VID >> 8), //idVendor - ID Vendor
	(uint8_t)(FUSB_USB_PID), (uint8_t)(FUSB_USB_PID >> 8), //idProduct - ID Product
	(uint8_t)(FUSB_USB_REV), (uint8_t)(FUSB_USB_REV >> 8), //bcdDevice - Device Release Number
	1, //Manufacturer string
	2, //Product string
	3, //Serial string
	1, //Max number of configurations
};

/* Configuration Descriptor Set */
static const uint8_t config_descriptor[ ] =
{
    /* Configuration Descriptor */
    0x09,                                                   // bLength
    0x02,                                                   // bDescriptorType
    0x20, 0x00,                                             // wTotalLength
    0x01,                                                   // bNumInterfaces (1)
    0x01,                                                   // bConfigurationValue
    0x00,                                                   // iConfiguration
    0xA0,                                                   // bmAttributes: Bus Powered; Remote Wakeup
    0x32,                                                   // MaxPower: 100mA

    /* Interface Descriptor (Bulk) */
    0x09,                                                   // bLength
    0x04,                                                   // bDescriptorType
    0x00,                                                   // bInterfaceNumber
    0x00,                                                   // bAlternateSetting
    0x02,                                                   // bNumEndpoints
    0xff,                                                   // bInterfaceClass
    0xff,                                                   // bInterfaceSubClass
    0xff,                                                   // bInterfaceProtocol: Other
    0x03,                                                   // iInterface

    /* Endpoint Descriptor (Bulk) */
    0x07,                                                   // bLength
    0x05,                                                   // bDescriptorType
    0x81,                                                   // bEndpointAddress: IN Endpoint 1 (BULK)
    0x02,                                                   // bmAttributes
    0x40, 0x00,                                             // wMaxPacketSize
    0x01,                                                   // bInterval: 125uS

    /* Endpoint Descriptor (Bulk) */
    0x07,                                                   // bLength
    0x05,                                                   // bDescriptorType
    0x02,                                                   // bEndpointAddress: OUT Endpoint 2 (BULK)
    0x02,                                                   // bmAttributes
    0x40, 0x00,                                             // wMaxPacketSize
    0x01,                                                   // bInterval: 125uS
};

struct usb_string_descriptor_struct {
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint16_t wString[];
};
const static struct usb_string_descriptor_struct string0 __attribute__((section(".rodata"))) = {
	4,
	3,
	{0x0409}
};
const static struct usb_string_descriptor_struct string1 __attribute__((section(".rodata")))  = {
	sizeof(FUSB_STR_MANUFACTURER),
	3,
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
	uint32_t	lIndexValue;
	const uint8_t	*addr;
	uint8_t		length;
} descriptor_list[] = {
	{0x00000100, device_descriptor, sizeof(device_descriptor)},
	{0x00000200, config_descriptor, sizeof(config_descriptor)},
	{0x00000300, (const uint8_t *)&string0, 4},
	{0x04090301, (const uint8_t *)&string1, sizeof(FUSB_STR_MANUFACTURER)},
	{0x04090302, (const uint8_t *)&string2, sizeof(FUSB_STR_PRODUCT)},
	{0x04090303, (const uint8_t *)&string3, sizeof(FUSB_STR_SERIAL)}
};
#define DESCRIPTOR_LIST_ENTRIES ((sizeof(descriptor_list))/(sizeof(struct descriptor_list_struct)) )

#endif

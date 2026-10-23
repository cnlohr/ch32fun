
/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Ha Thach (tinyusb.org)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * This file is part of the TinyUSB stack.
 */

/** \ingroup group_usb_definitions
 *  \defgroup USBDef_Type USB Types
 *  @{ */

// Cribbed from the WCH x035 example code, but originally from USB IF.

#ifndef _TUSB_TYPES_H_
#define _TUSB_TYPES_H_

#include <stdbool.h>
#include <stdint.h>


typedef struct {
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint8_t bDescriptorSubType;
  uint16_t bcdCDC;
} __attribute__((packed)) csInterfaceDescriptor;

typedef struct {
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint8_t bDescriptorSubType;
  uint8_t bmCapabilities;
  uint8_t bDataInterface;
} __attribute__((packed)) cdcInterfaceDescriptor;

typedef struct {
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint8_t bDescriptorSubType;
  uint8_t bmAttributes;
} __attribute__((packed)) acmInterfaceDescriptor;

typedef struct {
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint8_t bDescriptorSubType;
  uint8_t bControlInterface;
  uint8_t bSubordinateInterface0;
} __attribute__((packed)) unionFunctionalDescriptor;

/*------------------------------------------------------------------*/
/* From Linux
 *------------------------------------------------------------------*/


#define USB_DIR_OUT			0		/* to device */
#define USB_DIR_IN			0x80		/* to host */

#define USB_TYPE_MASK			(0x03 << 5)
#define USB_TYPE_STANDARD		(0x00 << 5)
#define USB_TYPE_CLASS			(0x01 << 5)
#define USB_TYPE_VENDOR			(0x02 << 5)
#define USB_TYPE_RESERVED		(0x03 << 5)


/*
 * USB recipients, the third of three bRequestType fields
 */
#define USB_RECIP_MASK			0x1f
#define USB_RECIP_DEVICE		0x00
#define USB_RECIP_INTERFACE		0x01
#define USB_RECIP_ENDPOINT		0x02
#define USB_RECIP_OTHER			0x03
/* From Wireless USB 1.0 */
#define USB_RECIP_PORT			0x04
#define USB_RECIP_RPIPE		0x05

#define TU_ATTR_PACKED __attribute__((packed))
#ifndef TU_BIT
#define TU_BIT(n)             (1U << (n))
#endif
#ifndef TU_STRCAT
#define TU_STRCAT(a, b)   a##b            ///< concat without expand
#define TU_XSTRCAT(a, b)  TU_STRCAT(a, b) ///< expand then concat
#endif

#ifndef _TU_COUNTER_
#if defined __COUNTER__ && __COUNTER__ != __COUNTER__
  #define _TU_COUNTER_ __COUNTER__
#else
  #define _TU_COUNTER_ __LINE__
#endif
#endif

// Compile-time Assert
#if defined (__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
  #define TU_VERIFY_STATIC   _Static_assert
#elif defined (__cplusplus) && __cplusplus >= 201103L
  #define TU_VERIFY_STATIC   static_assert
#else
  #define TU_VERIFY_STATIC(const_expr, _mess) enum { TU_XSTRCAT(_verify_static_, _TU_COUNTER_) = 1/(!!(const_expr)) }
#endif

#ifdef __cplusplus
 extern "C" {
#endif

/*------------------------------------------------------------------*/
/* CONSTANTS
 *------------------------------------------------------------------*/

/// defined base on EHCI specs value for Endpoint Speed
typedef enum
{
  TUSB_SPEED_FULL = 0,
  TUSB_SPEED_LOW     ,
  TUSB_SPEED_HIGH,
  TUSB_SPEED_INVALID = 0xff,
}tusb_speed_t;

/// defined base on USB Specs Endpoint's bmAttributes
typedef enum
{
  TUSB_XFER_CONTROL = 0 ,
  TUSB_XFER_ISOCHRONOUS ,
  TUSB_XFER_BULK        ,
  TUSB_XFER_INTERRUPT
}tusb_xfer_type_t;

typedef enum
{
  TUSB_DIR_OUT = 0,
  TUSB_DIR_IN  = 1,

  TUSB_DIR_IN_MASK = 0x80
}tusb_dir_t;

/// USB Descriptor Types
typedef enum
{
  TUSB_DESC_DEVICE                = 0x01,
  TUSB_DESC_CONFIGURATION         = 0x02,
  TUSB_DESC_STRING                = 0x03,
  TUSB_DESC_INTERFACE             = 0x04,
  TUSB_DESC_ENDPOINT              = 0x05,
  TUSB_DESC_DEVICE_QUALIFIER      = 0x06,
  TUSB_DESC_OTHER_SPEED_CONFIG    = 0x07,
  TUSB_DESC_INTERFACE_POWER       = 0x08,
  TUSB_DESC_OTG                   = 0x09,
  TUSB_DESC_DEBUG                 = 0x0A,
  TUSB_DESC_INTERFACE_ASSOCIATION = 0x0B,

  TUSB_DESC_BOS                   = 0x0F,
  TUSB_DESC_DEVICE_CAPABILITY     = 0x10,

  TUSB_DESC_FUNCTIONAL            = 0x21,

  // Class Specific Descriptor
  TUSB_DESC_CS_DEVICE             = 0x21,
  TUSB_DESC_CS_CONFIGURATION      = 0x22,
  TUSB_DESC_CS_STRING             = 0x23,
  TUSB_DESC_CS_INTERFACE          = 0x24,
  TUSB_DESC_CS_ENDPOINT           = 0x25,

  TUSB_DESC_SUPERSPEED_ENDPOINT_COMPANION     = 0x30,
  TUSB_DESC_SUPERSPEED_ISO_ENDPOINT_COMPANION = 0x31
}tusb_desc_type_t;

typedef enum
{
  TUSB_REQ_GET_STATUS        = 0  ,
  TUSB_REQ_CLEAR_FEATURE     = 1  ,
  TUSB_REQ_RESERVED          = 2  ,
  TUSB_REQ_SET_FEATURE       = 3  ,
  TUSB_REQ_RESERVED2         = 4  ,
  TUSB_REQ_SET_ADDRESS       = 5  ,
  TUSB_REQ_GET_DESCRIPTOR    = 6  ,
  TUSB_REQ_SET_DESCRIPTOR    = 7  ,
  TUSB_REQ_GET_CONFIGURATION = 8  ,
  TUSB_REQ_SET_CONFIGURATION = 9  ,
  TUSB_REQ_GET_INTERFACE     = 10 ,
  TUSB_REQ_SET_INTERFACE     = 11 ,
  TUSB_REQ_SYNCH_FRAME       = 12
}tusb_request_code_t;

typedef enum
{
  TUSB_REQ_FEATURE_EDPT_HALT     = 0,
  TUSB_REQ_FEATURE_REMOTE_WAKEUP = 1,
  TUSB_REQ_FEATURE_TEST_MODE     = 2
}tusb_request_feature_selector_t;

typedef enum
{
  TUSB_REQ_TYPE_STANDARD = 0,
  TUSB_REQ_TYPE_CLASS,
  TUSB_REQ_TYPE_VENDOR,
  TUSB_REQ_TYPE_INVALID
} tusb_request_type_t;

typedef enum
{
  TUSB_REQ_RCPT_DEVICE =0,
  TUSB_REQ_RCPT_INTERFACE,
  TUSB_REQ_RCPT_ENDPOINT,
  TUSB_REQ_RCPT_OTHER
} tusb_request_recipient_t;

// https://www.usb.org/defined-class-codes
typedef enum
{
  TUSB_CLASS_UNSPECIFIED          = 0    ,
  TUSB_CLASS_AUDIO                = 1    ,
  TUSB_CLASS_CDC                  = 2    ,
  TUSB_CLASS_HID                  = 3    ,
  TUSB_CLASS_RESERVED_4           = 4    ,
  TUSB_CLASS_PHYSICAL             = 5    ,
  TUSB_CLASS_IMAGE                = 6    ,
  TUSB_CLASS_PRINTER              = 7    ,
  TUSB_CLASS_MSC                  = 8    ,
  TUSB_CLASS_HUB                  = 9    ,
  TUSB_CLASS_CDC_DATA             = 10   ,
  TUSB_CLASS_SMART_CARD           = 11   ,
  TUSB_CLASS_RESERVED_12          = 12   ,
  TUSB_CLASS_CONTENT_SECURITY     = 13   ,
  TUSB_CLASS_VIDEO                = 14   ,
  TUSB_CLASS_PERSONAL_HEALTHCARE  = 15   ,
  TUSB_CLASS_AUDIO_VIDEO          = 16   ,

  TUSB_CLASS_DIAGNOSTIC           = 0xDC ,
  TUSB_CLASS_WIRELESS_CONTROLLER  = 0xE0 ,
  TUSB_CLASS_MISC                 = 0xEF ,
  TUSB_CLASS_APPLICATION_SPECIFIC = 0xFE ,
  TUSB_CLASS_VENDOR_SPECIFIC      = 0xFF
}tusb_class_code_t;

typedef enum
{
  MISC_SUBCLASS_COMMON = 2
}misc_subclass_type_t;

typedef enum
{
  MISC_PROTOCOL_IAD = 1
}misc_protocol_type_t;

typedef enum
{
  APP_SUBCLASS_USBTMC = 0x03,
  APP_SUBCLASS_DFU_RUNTIME = 0x01
} app_subclass_type_t;

typedef enum
{
  DEVICE_CAPABILITY_WIRELESS_USB               = 0x01,
  DEVICE_CAPABILITY_USB20_EXTENSION            = 0x02,
  DEVICE_CAPABILITY_SUPERSPEED_USB             = 0x03,
  DEVICE_CAPABILITY_CONTAINER_id               = 0x04,
  DEVICE_CAPABILITY_PLATFORM                   = 0x05,
  DEVICE_CAPABILITY_POWER_DELIVERY             = 0x06,
  DEVICE_CAPABILITY_BATTERY_INFO               = 0x07,
  DEVICE_CAPABILITY_PD_CONSUMER_PORT           = 0x08,
  DEVICE_CAPABILITY_PD_PROVIDER_PORT           = 0x09,
  DEVICE_CAPABILITY_SUPERSPEED_PLUS            = 0x0A,
  DEVICE_CAPABILITY_PRECESION_TIME_MEASUREMENT = 0x0B,
  DEVICE_CAPABILITY_WIRELESS_USB_EXT           = 0x0C,
  DEVICE_CAPABILITY_BILLBOARD                  = 0x0D,
  DEVICE_CAPABILITY_AUTHENTICATION             = 0x0E,
  DEVICE_CAPABILITY_BILLBOARD_EX               = 0x0F,
  DEVICE_CAPABILITY_CONFIGURATION_SUMMARY      = 0x10
}device_capability_type_t;

enum {
  TUSB_DESC_CONFIG_ATT_REMOTE_WAKEUP = TU_BIT(5),
  TUSB_DESC_CONFIG_ATT_SELF_POWERED  = TU_BIT(6),
};

#define TUSB_DESC_CONFIG_POWER_MA(x)  ((x)/2)

/// Device State TODO remove
typedef enum
{
  TUSB_DEVICE_STATE_UNPLUG = 0  ,
  TUSB_DEVICE_STATE_CONFIGURED  ,
  TUSB_DEVICE_STATE_SUSPENDED   ,
}tusb_device_state_t;

typedef enum
{
  XFER_RESULT_SUCCESS,
  XFER_RESULT_FAILED,
  XFER_RESULT_STALLED,
}xfer_result_t;

enum // TODO remove
{
  DESC_OFFSET_LEN  = 0,
  DESC_OFFSET_TYPE = 1
};

enum
{
  INTERFACE_INVALID_NUMBER = 0xff
};


typedef enum
{
  MS_OS_20_SET_HEADER_DESCRIPTOR       = 0x00,
  MS_OS_20_SUBSET_HEADER_CONFIGURATION = 0x01,
  MS_OS_20_SUBSET_HEADER_FUNCTION      = 0x02,
  MS_OS_20_FEATURE_COMPATBLE_ID        = 0x03,
  MS_OS_20_FEATURE_REG_PROPERTY        = 0x04,
  MS_OS_20_FEATURE_MIN_RESUME_TIME     = 0x05,
  MS_OS_20_FEATURE_MODEL_ID            = 0x06,
  MS_OS_20_FEATURE_CCGP_DEVICE         = 0x07,
  MS_OS_20_FEATURE_VENDOR_REVISION     = 0x08
} microsoft_os_20_type_t;

enum
{
  CONTROL_STAGE_SETUP,
  CONTROL_STAGE_DATA,
  CONTROL_STAGE_ACK
};

//--------------------------------------------------------------------+
// USB Descriptors
//--------------------------------------------------------------------+

/// USB Device Descriptor
typedef struct TU_ATTR_PACKED
{
  uint8_t  bLength            ; ///< Size of this descriptor in bytes.
  uint8_t  bDescriptorType    ; ///< DEVICE Descriptor Type.
  uint16_t bcdUSB             ; ///< BUSB Specification Release Number in Binary-Coded Decimal (i.e., 2.10 is 210H). This field identifies the release of the USB Specification with which the device and its descriptors are compliant.

  uint8_t  bDeviceClass       ; ///< Class code (assigned by the USB-IF). \li If this field is reset to zero, each interface within a configuration specifies its own class information and the various interfaces operate independently. \li If this field is set to a value between 1 and FEH, the device supports different class specifications on different interfaces and the interfaces may not operate independently. This value identifies the class definition used for the aggregate interfaces. \li If this field is set to FFH, the device class is vendor-specific.
  uint8_t  bDeviceSubClass    ; ///< Subclass code (assigned by the USB-IF). These codes are qualified by the value of the bDeviceClass field. \li If the bDeviceClass field is reset to zero, this field must also be reset to zero. \li If the bDeviceClass field is not set to FFH, all values are reserved for assignment by the USB-IF.
  uint8_t  bDeviceProtocol    ; ///< Protocol code (assigned by the USB-IF). These codes are qualified by the value of the bDeviceClass and the bDeviceSubClass fields. If a device supports class-specific protocols on a device basis as opposed to an interface basis, this code identifies the protocols that the device uses as defined by the specification of the device class. \li If this field is reset to zero, the device does not use class-specific protocols on a device basis. However, it may use classspecific protocols on an interface basis. \li If this field is set to FFH, the device uses a vendor-specific protocol on a device basis.
  uint8_t  bMaxPacketSize0    ; ///< Maximum packet size for endpoint zero (only 8, 16, 32, or 64 are valid). For HS devices is fixed to 64.

  uint16_t idVendor           ; ///< Vendor ID (assigned by the USB-IF).
  uint16_t idProduct          ; ///< Product ID (assigned by the manufacturer).
  uint16_t bcdDevice          ; ///< Device release number in binary-coded decimal.
  uint8_t  iManufacturer      ; ///< Index of string descriptor describing manufacturer.
  uint8_t  iProduct           ; ///< Index of string descriptor describing product.
  uint8_t  iSerialNumber      ; ///< Index of string descriptor describing the device's serial number.

  uint8_t  bNumConfigurations ; ///< Number of possible configurations.
} tusb_desc_device_t;

TU_VERIFY_STATIC( sizeof(tusb_desc_device_t) == 18, "size is not correct");

// USB Binary Device Object Store (BOS) Descriptor
typedef struct TU_ATTR_PACKED
{
  uint8_t  bLength         ; ///< Size of this descriptor in bytes
  uint8_t  bDescriptorType ; ///< CONFIGURATION Descriptor Type
  uint16_t wTotalLength    ; ///< Total length of data returned for this descriptor
  uint8_t  bNumDeviceCaps  ; ///< Number of device capability descriptors in the BOS
} tusb_desc_bos_t;

/// USB Configuration Descriptor
typedef struct TU_ATTR_PACKED
{
  uint8_t  bLength             ; ///< Size of this descriptor in bytes
  uint8_t  bDescriptorType     ; ///< CONFIGURATION Descriptor Type
  uint16_t wTotalLength        ; ///< Total length of data returned for this configuration. Includes the combined length of all descriptors (configuration, interface, endpoint, and class- or vendor-specific) returned for this configuration.

  uint8_t  bNumInterfaces      ; ///< Number of interfaces supported by this configuration
  uint8_t  bConfigurationValue ; ///< Value to use as an argument to the SetConfiguration() request to select this configuration.
  uint8_t  iConfiguration      ; ///< Index of string descriptor describing this configuration
  uint8_t  bmAttributes        ; ///< Configuration characteristics \n D7: Reserved (set to one)\n D6: Self-powered \n D5: Remote Wakeup \n D4...0: Reserved (reset to zero) \n D7 is reserved and must be set to one for historical reasons. \n A device configuration that uses power from the bus and a local source reports a non-zero value in bMaxPower to indicate the amount of bus power required and sets D6. The actual power source at runtime may be determined using the GetStatus(DEVICE) request (see USB 2.0 spec Section 9.4.5). \n If a device configuration supports remote wakeup, D5 is set to one.
  uint8_t  bMaxPower           ; ///< Maximum power consumption of the USB device from the bus in this specific configuration when the device is fully operational. Expressed in 2 mA units (i.e., 50 = 100 mA).
} tusb_desc_configuration_t;

/// USB Interface Descriptor
typedef struct TU_ATTR_PACKED
{
  uint8_t  bLength            ; ///< Size of this descriptor in bytes
  uint8_t  bDescriptorType    ; ///< INTERFACE Descriptor Type

  uint8_t  bInterfaceNumber   ; ///< Number of this interface. Zero-based value identifying the index in the array of concurrent interfaces supported by this configuration.
  uint8_t  bAlternateSetting  ; ///< Value used to select this alternate setting for the interface identified in the prior field
  uint8_t  bNumEndpoints      ; ///< Number of endpoints used by this interface (excluding endpoint zero). If this value is zero, this interface only uses the Default Control Pipe.
  uint8_t  bInterfaceClass    ; ///< Class code (assigned by the USB-IF). \li A value of zero is reserved for future standardization. \li If this field is set to FFH, the interface class is vendor-specific. \li All other values are reserved for assignment by the USB-IF.
  uint8_t  bInterfaceSubClass ; ///< Subclass code (assigned by the USB-IF). \n These codes are qualified by the value of the bInterfaceClass field. \li If the bInterfaceClass field is reset to zero, this field must also be reset to zero. \li If the bInterfaceClass field is not set to FFH, all values are reserved for assignment by the USB-IF.
  uint8_t  bInterfaceProtocol ; ///< Protocol code (assigned by the USB). \n These codes are qualified by the value of the bInterfaceClass and the bInterfaceSubClass fields. If an interface supports class-specific requests, this code identifies the protocols that the device uses as defined by the specification of the device class. \li If this field is reset to zero, the device does not use a class-specific protocol on this interface. \li If this field is set to FFH, the device uses a vendor-specific protocol for this interface.
  uint8_t  iInterface         ; ///< Index of string descriptor describing this interface
} tusb_desc_interface_t;

/// USB Endpoint Descriptor
typedef struct TU_ATTR_PACKED
{
  uint8_t  bLength          ; ///< Size of this descriptor in bytes
  uint8_t  bDescriptorType  ; ///< ENDPOINT Descriptor Type

  uint8_t  bEndpointAddress ; ///< The address of the endpoint on the USB device described by this descriptor. The address is encoded as follows: \n Bit 3...0: The endpoint number \n Bit 6...4: Reserved, reset to zero \n Bit 7: Direction, ignored for control endpoints 0 = OUT endpoint 1 = IN endpoint.

  struct TU_ATTR_PACKED {
    uint8_t xfer  : 2;
    uint8_t sync  : 2;
    uint8_t usage : 2;
    uint8_t       : 2;
  } bmAttributes     ; ///< This field describes the endpoint's attributes when it is configured using the bConfigurationValue. \n Bits 1..0: Transfer Type \n- 00 = Control \n- 01 = Isochronous \n- 10 = Bulk \n- 11 = Interrupt \n If not an isochronous endpoint, bits 5..2 are reserved and must be set to zero. If isochronous, they are defined as follows: \n Bits 3..2: Synchronization Type \n- 00 = No Synchronization \n- 01 = Asynchronous \n- 10 = Adaptive \n- 11 = Synchronous \n Bits 5..4: Usage Type \n- 00 = Data endpoint \n- 01 = Feedback endpoint \n- 10 = Implicit feedback Data endpoint \n- 11 = Reserved \n Refer to Chapter 5 of USB 2.0 specification for more information. \n All other bits are reserved and must be reset to zero. Reserved bits must be ignored by the host.

  struct TU_ATTR_PACKED {
    uint16_t size           : 11; ///< Maximum packet size this endpoint is capable of sending or receiving when this configuration is selected. \n For isochronous endpoints, this value is used to reserve the bus time in the schedule, required for the per-(micro)frame data payloads. The pipe may, on an ongoing basis, actually use less bandwidth than that reserved. The device reports, if necessary, the actual bandwidth used via its normal, non-USB defined mechanisms. \n For all endpoints, bits 10..0 specify the maximum packet size (in bytes). \n For high-speed isochronous and interrupt endpoints: \n Bits 12..11 specify the number of additional transaction opportunities per microframe: \n- 00 = None (1 transaction per microframe) \n- 01 = 1 additional (2 per microframe) \n- 10 = 2 additional (3 per microframe) \n- 11 = Reserved \n Bits 15..13 are reserved and must be set to zero.
    uint16_t hs_period_mult : 2;
    uint16_t TU_RESERVED    : 3;
  }wMaxPacketSize;

  uint8_t  bInterval        ; ///< Interval for polling endpoint for data transfers. Expressed in frames or microframes depending on the device operating speed (i.e., either 1 millisecond or 125 us units). \n- For full-/high-speed isochronous endpoints, this value must be in the range from 1 to 16. The bInterval value is used as the exponent for a \f$ 2^(bInterval-1) \f$ value; e.g., a bInterval of 4 means a period of 8 (\f$ 2^(4-1) \f$). \n- For full-/low-speed interrupt endpoints, the value of this field may be from 1 to 255. \n- For high-speed interrupt endpoints, the bInterval value is used as the exponent for a \f$ 2^(bInterval-1) \f$ value; e.g., a bInterval of 4 means a period of 8 (\f$ 2^(4-1) \f$) . This value must be from 1 to 16. \n- For high-speed bulk/control OUT endpoints, the bInterval must specify the maximum NAK rate of the endpoint. A value of 0 indicates the endpoint never NAKs. Other values indicate at most 1 NAK each bInterval number of microframes. This value must be in the range from 0 to 255. \n Refer to Chapter 5 of USB 2.0 specification for more information.
} tusb_desc_endpoint_t;

/// USB Other Speed Configuration Descriptor
typedef struct TU_ATTR_PACKED
{
  uint8_t  bLength             ; ///< Size of descriptor
  uint8_t  bDescriptorType     ; ///< Other_speed_Configuration Type
  uint16_t wTotalLength        ; ///< Total length of data returned

  uint8_t  bNumInterfaces      ; ///< Number of interfaces supported by this speed configuration
  uint8_t  bConfigurationValue ; ///< Value to use to select configuration
  uint8_t  IConfiguration      ; ///< Index of string descriptor
  uint8_t  bmAttributes        ; ///< Same as Configuration descriptor
  uint8_t  bMaxPower           ; ///< Same as Configuration descriptor
} tusb_desc_other_speed_t;

/// USB Device Qualifier Descriptor
typedef struct TU_ATTR_PACKED
{
  uint8_t  bLength            ; ///< Size of descriptor
  uint8_t  bDescriptorType    ; ///< Device Qualifier Type
  uint16_t bcdUSB             ; ///< USB specification version number (e.g., 0200H for V2.00)

  uint8_t  bDeviceClass       ; ///< Class Code
  uint8_t  bDeviceSubClass    ; ///< SubClass Code
  uint8_t  bDeviceProtocol    ; ///< Protocol Code
  uint8_t  bMaxPacketSize0    ; ///< Maximum packet size for other speed
  uint8_t  bNumConfigurations ; ///< Number of Other-speed Configurations
  uint8_t  bReserved          ; ///< Reserved for future use, must be zero
} tusb_desc_device_qualifier_t;

/// USB Interface Association Descriptor (IAD ECN)
typedef struct TU_ATTR_PACKED
{
  uint8_t bLength           ; ///< Size of descriptor
  uint8_t bDescriptorType   ; ///< Other_speed_Configuration Type

  uint8_t bFirstInterface   ; ///< Index of the first associated interface.
  uint8_t bInterfaceCount   ; ///< Total number of associated interfaces.

  uint8_t bFunctionClass    ; ///< Interface class ID.
  uint8_t bFunctionSubClass ; ///< Interface subclass ID.
  uint8_t bFunctionProtocol ; ///< Interface protocol ID.

  uint8_t iFunction         ; ///< Index of the string descriptor describing the interface association.
} tusb_desc_interface_assoc_t;

// USB String Descriptor
typedef struct TU_ATTR_PACKED
{
  uint8_t  bLength         ; ///< Size of this descriptor in bytes
  uint8_t  bDescriptorType ; ///< Descriptor Type
  uint16_t unicode_string[];
} tusb_desc_string_t;

// USB Binary Device Object Store (BOS)
typedef struct TU_ATTR_PACKED
{
  uint8_t bLength;
  uint8_t bDescriptorType ;
  uint8_t bDevCapabilityType;
  uint8_t bReserved;
  uint8_t PlatformCapabilityUUID[16];
  uint8_t CapabilityData[];
} tusb_desc_bos_platform_t;

// USB WebuSB URL Descriptor
typedef struct TU_ATTR_PACKED
{
  uint8_t bLength;
  uint8_t bDescriptorType;
  uint8_t bScheme;
  char    url[];
} tusb_desc_webusb_url_t;

/*------------------------------------------------------------------*/
/* Types
 *------------------------------------------------------------------*/
typedef struct TU_ATTR_PACKED{
  union {
    struct TU_ATTR_PACKED {
      uint8_t recipient :  5; ///< Recipient type tusb_request_recipient_t.
      uint8_t type      :  2; ///< Request type tusb_request_type_t.
      uint8_t direction :  1; ///< Direction type. tusb_dir_t
    } bmRequestType_bit;

    uint8_t bmRequestType;
  };

  uint8_t  bRequest;
  uint16_t wValue;
  uint16_t wIndex;
  uint16_t wLength;
} tusb_control_request_t;

TU_VERIFY_STATIC( sizeof(tusb_control_request_t) == 8, "size is not correct");

// TODO move to somewhere suitable
static inline uint8_t bm_request_type(uint8_t direction, uint8_t type, uint8_t recipient)
{
  return ((uint8_t) (direction << 7)) | ((uint8_t) (type << 5)) | (recipient);
}

//--------------------------------------------------------------------+
// Endpoint helper
//--------------------------------------------------------------------+

// Get direction from Endpoint address
static inline tusb_dir_t tu_edpt_dir(uint8_t addr)
{
  return (addr & TUSB_DIR_IN_MASK) ? TUSB_DIR_IN : TUSB_DIR_OUT;
}

// Get Endpoint number from address
static inline uint8_t tu_edpt_number(uint8_t addr)
{
  return (uint8_t)(addr & (~TUSB_DIR_IN_MASK));
}

static inline uint8_t tu_edpt_addr(uint8_t num, uint8_t dir)
{
  return (uint8_t)(num | (dir ? TUSB_DIR_IN_MASK : 0));
}

//--------------------------------------------------------------------+
// Descriptor helper
//--------------------------------------------------------------------+
static inline uint8_t const * tu_desc_next(void const* desc)
{
  uint8_t const* desc8 = (uint8_t const*) desc;
  return desc8 + desc8[DESC_OFFSET_LEN];
}

static inline uint8_t tu_desc_type(void const* desc)
{
  return ((uint8_t const*) desc)[DESC_OFFSET_TYPE];
}

static inline uint8_t tu_desc_len(void const* desc)
{
  return ((uint8_t const*) desc)[DESC_OFFSET_LEN];
}

#ifdef __cplusplus
 }
#endif


// from tinyusb_hid.h

#ifdef __cplusplus
 extern "C" {
#endif

#define TU_U16_HIGH(u16)      ((uint8_t) (((u16) >> 8) & 0x00ff))
#define TU_U16_LOW(u16)       ((uint8_t) ((u16)       & 0x00ff))
#define U16_TO_U8S_BE(u16)    TU_U16_HIGH(u16), TU_U16_LOW(u16)
#define U16_TO_U8S_LE(u16)    TU_U16_LOW(u16), TU_U16_HIGH(u16)

#ifndef TU_ATTR_PACKED
#define TU_ATTR_PACKED __attribute__((packed))
#endif
#ifndef TU_BIT
#define TU_BIT( x ) (1<<(x))
#endif

/* USB constant and structure define */

/* USB PID */
#ifndef USB_PID_SETUP
  #define USB_PID_NULL     0x00  /* reserved PID */
  #define USB_PID_SOF      0x05
  #define USB_PID_SETUP    0x0D
  #define USB_PID_IN       0x09
  #define USB_PID_OUT      0x01
  #define USB_PID_ACK      0x02
  #define USB_PID_NAK      0x0A
  #define USB_PID_STALL    0x0E
  #define USB_PID_DATA0    0x03
  #define USB_PID_DATA1    0x0B
  #define USB_PID_PRE      0x0C
#endif

/* USB standard device request code */
#ifndef USB_GET_DESCRIPTOR
  #define USB_GET_STATUS           0x00
  #define USB_CLEAR_FEATURE        0x01
  #define USB_SET_FEATURE          0x03
  #define USB_SET_ADDRESS          0x05
  #define USB_GET_DESCRIPTOR       0x06
  #define USB_SET_DESCRIPTOR       0x07
  #define USB_GET_CONFIGURATION    0x08
  #define USB_SET_CONFIGURATION    0x09
  #define USB_GET_INTERFACE        0x0A
  #define USB_SET_INTERFACE        0x0B
  #define USB_SYNCH_FRAME          0x0C
#endif

/* USB hub class request code */
#ifndef HUB_GET_DESCRIPTOR
  #define HUB_GET_STATUS        0x00
  #define HUB_CLEAR_FEATURE     0x01
  #define HUB_GET_STATE         0x02
  #define HUB_SET_FEATURE       0x03
  #define HUB_GET_DESCRIPTOR    0x06
  #define HUB_SET_DESCRIPTOR    0x07
#endif

/* USB HID class request code */
#ifndef HID_GET_REPORT
  #define HID_GET_REPORT      0x01
  #define HID_GET_IDLE        0x02
  #define HID_GET_PROTOCOL    0x03
  #define HID_SET_REPORT      0x09
  #define HID_SET_IDLE        0x0A
  #define HID_SET_PROTOCOL    0x0B
#endif

/* USB CDC Class request code */
#ifndef CDC_GET_LINE_CODING
#define CDC_GET_LINE_CODING   0X21      /* This request allows the host to find out the currently configured line coding */
#define CDC_SET_LINE_CODING   0x20      /* Configures DTE rate, stop-bits, parity, and number-of-character */
#define CDC_SET_LINE_CTLSTE   0X22      /* This request generates RS-232/V.24 style control signals */
#define CDC_SEND_BREAK        0X23      /* Sends special carrier modulation used to specify RS-232 style break */
#endif

/* Bit define for USB request type */
#ifndef USB_REQ_TYP_MASK
  #define USB_REQ_TYP_IN          0x80  /* control IN, device to host */
  #define USB_REQ_TYP_OUT         0x00  /* control OUT, host to device */
  #define USB_REQ_TYP_READ        0x80  /* control read, device to host */
  #define USB_REQ_TYP_WRITE       0x00  /* control write, host to device */
  #define USB_REQ_TYP_MASK        0x60  /* bit mask of request type */
  #define USB_REQ_TYP_STANDARD    0x00
  #define USB_REQ_TYP_CLASS       0x20
  #define USB_REQ_TYP_VENDOR      0x40
  #define USB_REQ_TYP_RESERVED    0x60
  #define USB_REQ_RECIP_MASK      0x1F  /* bit mask of request recipient */
  #define USB_REQ_RECIP_DEVICE    0x00
  #define USB_REQ_RECIP_INTERF    0x01
  #define USB_REQ_RECIP_ENDP      0x02
  #define USB_REQ_RECIP_OTHER     0x03
  #define USB_REQ_FEAT_REMOTE_WAKEUP  0x01
  #define USB_REQ_FEAT_ENDP_HALT      0x00
#endif

/* USB request type for hub class request */
#ifndef HUB_GET_HUB_DESCRIPTOR
  #define HUB_CLEAR_HUB_FEATURE     0x20
  #define HUB_CLEAR_PORT_FEATURE    0x23
  #define HUB_GET_BUS_STATE         0xA3
  #define HUB_GET_HUB_DESCRIPTOR    0xA0
  #define HUB_GET_HUB_STATUS        0xA0
  #define HUB_GET_PORT_STATUS       0xA3
  #define HUB_SET_HUB_DESCRIPTOR    0x20
  #define HUB_SET_HUB_FEATURE       0x20
  #define HUB_SET_PORT_FEATURE      0x23
#endif

/* Hub class feature selectors */
#ifndef HUB_PORT_RESET
  #define HUB_C_HUB_LOCAL_POWER      0
  #define HUB_C_HUB_OVER_CURRENT     1
  #define HUB_PORT_CONNECTION        0
  #define HUB_PORT_ENABLE            1
  #define HUB_PORT_SUSPEND           2
  #define HUB_PORT_OVER_CURRENT      3
  #define HUB_PORT_RESET             4
  #define HUB_PORT_POWER             8
  #define HUB_PORT_LOW_SPEED         9
  #define HUB_C_PORT_CONNECTION      16
  #define HUB_C_PORT_ENABLE          17
  #define HUB_C_PORT_SUSPEND         18
  #define HUB_C_PORT_OVER_CURRENT    19
  #define HUB_C_PORT_RESET           20
#endif

/* USB descriptor type */
#ifndef USB_DESCR_TYP_DEVICE
  #define USB_DESCR_TYP_DEVICE     0x01
  #define USB_DESCR_TYP_CONFIG     0x02
  #define USB_DESCR_TYP_STRING     0x03
  #define USB_DESCR_TYP_INTERF     0x04
  #define USB_DESCR_TYP_ENDP       0x05
  #define USB_DESCR_TYP_QUALIF     0x06
  #define USB_DESCR_TYP_SPEED      0x07
  #define USB_DESCR_TYP_OTG        0x09
  #define USB_DESCR_TYP_HID        0x21
  #define USB_DESCR_TYP_REPORT     0x22
  #define USB_DESCR_TYP_PHYSIC     0x23
  #define USB_DESCR_TYP_CS_INTF    0x24
  #define USB_DESCR_TYP_CS_ENDP    0x25
  #define USB_DESCR_TYP_HUB        0x29
#endif

/* USB device class */
#ifndef USB_DEV_CLASS_HUB
  #define USB_DEV_CLASS_RESERVED     0x00
  #define USB_DEV_CLASS_AUDIO        0x01
  #define USB_DEV_CLASS_COMMUNIC     0x02
  #define USB_DEV_CLASS_HID          0x03
  #define USB_DEV_CLASS_MONITOR      0x04
  #define USB_DEV_CLASS_PHYSIC_IF    0x05
  #define USB_DEV_CLASS_IMAGE        0x06
  #define USB_DEV_CLASS_PRINTER      0x07
  #define USB_DEV_CLASS_STORAGE      0x08
  #define USB_DEV_CLASS_HUB          0x09
  #define USB_DEV_CLASS_VEN_SPEC     0xFF
#endif

/* USB endpoint type and attributes */
#ifndef USB_ENDP_TYPE_MASK
  #define USB_ENDP_DIR_MASK      0x80
  #define USB_ENDP_ADDR_MASK     0x0F
  #define USB_ENDP_TYPE_MASK     0x03
  #define USB_ENDP_TYPE_CTRL     0x00
  #define USB_ENDP_TYPE_ISOCH    0x01
  #define USB_ENDP_TYPE_BULK     0x02
  #define USB_ENDP_TYPE_INTER    0x03
#endif

#ifndef USB_DEVICE_ADDR
  #define USB_DEVICE_ADDR    0x02
#endif
#ifndef DEFAULT_ENDP0_SIZE
  #define DEFAULT_ENDP0_SIZE 8  /* default maximum packet size for endpoint 0 */
#endif
#ifndef MAX_PACKET_SIZE
  #define MAX_PACKET_SIZE    64  /* maximum packet size */
#endif
#ifndef USB_BO_CBW_SIZE
  #define USB_BO_CBW_SIZE    0x1F
  #define USB_BO_CSW_SIZE    0x0D
#endif
#ifndef USB_BO_CBW_SIG0
  #define USB_BO_CBW_SIG0    0x55
  #define USB_BO_CBW_SIG1    0x53
  #define USB_BO_CBW_SIG2    0x42
  #define USB_BO_CBW_SIG3    0x43
  #define USB_BO_CSW_SIG0    0x55
  #define USB_BO_CSW_SIG1    0x53
  #define USB_BO_CSW_SIG2    0x42
  #define USB_BO_CSW_SIG3    0x53
#endif

#define DEF_STRING_DESC_LANG 0x00
#define DEF_STRING_DESC_MANU 0x01
#define DEF_STRING_DESC_PROD 0x02
#define DEF_STRING_DESC_SERN 0x03


//--------------------------------------------------------------------+
// Common Definitions
//--------------------------------------------------------------------+
/** \defgroup ClassDriver_HID_Common Common Definitions
 *  @{ */

 /// USB HID Descriptor
typedef struct TU_ATTR_PACKED
{
  uint8_t  bLength;         /**< Numeric expression that is the total size of the HID descriptor */
  uint8_t  bDescriptorType; /**< Constant name specifying type of HID descriptor. */

  uint16_t bcdHID;          /**< Numeric expression identifying the HID Class Specification release */
  uint8_t  bCountryCode;    /**< Numeric expression identifying country code of the localized hardware.  */
  uint8_t  bNumDescriptors; /**< Numeric expression specifying the number of class descriptors */

  uint8_t  bReportType;     /**< Type of HID class report. */
  uint16_t wReportLength;   /**< the total size of the Report descriptor. */
} tusb_hid_descriptor_hid_t;

/// HID Subclass
typedef enum
{
  HID_SUBCLASS_NONE = 0, ///< No Subclass
  HID_SUBCLASS_BOOT = 1  ///< Boot Interface Subclass
}hid_subclass_type_t;

/// HID Protocol
typedef enum
{
  HID_PROTOCOL_NONE     = 0, ///< None
  HID_PROTOCOL_KEYBOARD = 1, ///< Keyboard
  HID_PROTOCOL_MOUSE    = 2  ///< Mouse
}hid_protocol_type_t;

/// HID Descriptor Type
typedef enum
{
  HID_DESC_TYPE_HID      = 0x21, ///< HID Descriptor
  HID_DESC_TYPE_REPORT   = 0x22, ///< Report Descriptor
  HID_DESC_TYPE_PHYSICAL = 0x23  ///< Physical Descriptor
}hid_descriptor_type_t;

/// HID Request Report Type
typedef enum
{
  HID_REPORT_TYPE_INVALID = 0,
  HID_REPORT_TYPE_INPUT,      ///< Input
  HID_REPORT_TYPE_OUTPUT,     ///< Output
  HID_REPORT_TYPE_FEATURE     ///< Feature
}hid_report_type_t;

/// HID Class Specific Control Request
typedef enum
{
  HID_REQ_CONTROL_GET_REPORT   = 0x01, ///< Get Report
  HID_REQ_CONTROL_GET_IDLE     = 0x02, ///< Get Idle
  HID_REQ_CONTROL_GET_PROTOCOL = 0x03, ///< Get Protocol
  HID_REQ_CONTROL_SET_REPORT   = 0x09, ///< Set Report
  HID_REQ_CONTROL_SET_IDLE     = 0x0a, ///< Set Idle
  HID_REQ_CONTROL_SET_PROTOCOL = 0x0b  ///< Set Protocol
}hid_request_type_t;

/// HID Country Code
typedef enum
{
  HID_LOCAL_NotSupported = 0   , ///< NotSupported
  HID_LOCAL_Arabic             , ///< Arabic
  HID_LOCAL_Belgian            , ///< Belgian
  HID_LOCAL_Canadian_Bilingual , ///< Canadian_Bilingual
  HID_LOCAL_Canadian_French    , ///< Canadian_French
  HID_LOCAL_Czech_Republic     , ///< Czech_Republic
  HID_LOCAL_Danish             , ///< Danish
  HID_LOCAL_Finnish            , ///< Finnish
  HID_LOCAL_French             , ///< French
  HID_LOCAL_German             , ///< German
  HID_LOCAL_Greek              , ///< Greek
  HID_LOCAL_Hebrew             , ///< Hebrew
  HID_LOCAL_Hungary            , ///< Hungary
  HID_LOCAL_International      , ///< International
  HID_LOCAL_Italian            , ///< Italian
  HID_LOCAL_Japan_Katakana     , ///< Japan_Katakana
  HID_LOCAL_Korean             , ///< Korean
  HID_LOCAL_Latin_American     , ///< Latin_American
  HID_LOCAL_Netherlands_Dutch  , ///< Netherlands/Dutch
  HID_LOCAL_Norwegian          , ///< Norwegian
  HID_LOCAL_Persian_Farsi      , ///< Persian (Farsi)
  HID_LOCAL_Poland             , ///< Poland
  HID_LOCAL_Portuguese         , ///< Portuguese
  HID_LOCAL_Russia             , ///< Russia
  HID_LOCAL_Slovakia           , ///< Slovakia
  HID_LOCAL_Spanish            , ///< Spanish
  HID_LOCAL_Swedish            , ///< Swedish
  HID_LOCAL_Swiss_French       , ///< Swiss/French
  HID_LOCAL_Swiss_German       , ///< Swiss/German
  HID_LOCAL_Switzerland        , ///< Switzerland
  HID_LOCAL_Taiwan             , ///< Taiwan
  HID_LOCAL_Turkish_Q          , ///< Turkish-Q
  HID_LOCAL_UK                 , ///< UK
  HID_LOCAL_US                 , ///< US
  HID_LOCAL_Yugoslavia         , ///< Yugoslavia
  HID_LOCAL_Turkish_F            ///< Turkish-F
} hid_country_code_t;

/** @} */

//--------------------------------------------------------------------+
// GAMEPAD
//--------------------------------------------------------------------+
/** \addtogroup ClassDriver_HID_Gamepad Gamepad
 *  @{ */

/* From https://www.kernel.org/doc/html/latest/input/gamepad.html
          ____________________________              __
         / [__ZL__]          [__ZR__] \               |
        / [__ TL __]        [__ TR __] \              | Front Triggers
     __/________________________________\__         __|
    /                                  _   \          |
   /      /\           __             (N)   \         |
  /       ||      __  |MO|  __     _       _ \        | Main Pad
 |    <===DP===> |SE|      |ST|   (W) -|- (E) |       |
  \       ||    ___          ___       _     /        |
  /\      \/   /   \        /   \     (S)   /\      __|
 /  \________ | LS  | ____ |  RS | ________/  \       |
|         /  \ \___/ /    \ \___/ /  \         |      | Control Sticks
|        /    \_____/      \_____/    \        |    __|
|       /                              \       |
 \_____/                                \_____/

     |________|______|    |______|___________|
       D-Pad    Left       Right   Action Pad
               Stick       Stick

                 |_____________|
                    Menu Pad

  Most gamepads have the following features:
  - Action-Pad 4 buttons in diamonds-shape (on the right side) NORTH, SOUTH, WEST and EAST.
  - D-Pad (Direction-pad) 4 buttons (on the left side) that point up, down, left and right.
  - Menu-Pad Different constellations, but most-times 2 buttons: SELECT - START.
  - Analog-Sticks provide freely moveable sticks to control directions, Analog-sticks may also
  provide a digital button if you press them.
  - Triggers are located on the upper-side of the pad in vertical direction. The upper buttons
  are normally named Left- and Right-Triggers, the lower buttons Z-Left and Z-Right.
  - Rumble Many devices provide force-feedback features. But are mostly just simple rumble motors.
 */

/// HID Gamepad Protocol Report.
typedef struct TU_ATTR_PACKED
{
  int8_t  x;         ///< Delta x  movement of left analog-stick
  int8_t  y;         ///< Delta y  movement of left analog-stick
  int8_t  z;         ///< Delta z  movement of right analog-joystick
  int8_t  rz;        ///< Delta Rz movement of right analog-joystick
  int8_t  rx;        ///< Delta Rx movement of analog left trigger
  int8_t  ry;        ///< Delta Ry movement of analog right trigger
  uint8_t hat;       ///< Buttons mask for currently pressed buttons in the DPad/hat
  uint16_t buttons;  ///< Buttons mask for currently pressed buttons
}hid_gamepad_report_t;

/// HID Switch Gamepad Protocol Report.
typedef struct TU_ATTR_PACKED
{
  uint16_t buttons;  ///< Buttons mask for currently pressed buttons
  uint8_t hat;       ///< Buttons mask for currently pressed buttons in the DPad/hat
  int8_t  x;         ///< Delta x  movement of left analog-stick
  int8_t  y;         ///< Delta y  movement of left analog-stick
  int8_t  rx;        ///< Delta Rx movement of analog left trigger
  int8_t  ry;        ///< Delta Ry movement of analog right trigger
  int8_t  z;         ///< Delta z  movement of right analog-joystick
  int8_t  rz;        ///< Delta Rz movement of right analog-joystick
}hid_gamepad_ns_report_t;

/// Standard Gamepad Buttons Bitmap (from Linux input event codes)
typedef enum
{
  GAMEPAD_BUTTON_A      = TU_BIT(0),  ///< A/South button
  GAMEPAD_BUTTON_B      = TU_BIT(1),  ///< B/East button
  GAMEPAD_BUTTON_C      = TU_BIT(2),  ///< C button
  GAMEPAD_BUTTON_X      = TU_BIT(3),  ///< X/North button
  GAMEPAD_BUTTON_Y      = TU_BIT(4),  ///< Y/West button
  GAMEPAD_BUTTON_Z      = TU_BIT(5),  ///< Z button
  GAMEPAD_BUTTON_TL     = TU_BIT(6),  ///< L1 button
  GAMEPAD_BUTTON_TR     = TU_BIT(7),  ///< R1 button
  GAMEPAD_BUTTON_TL2    = TU_BIT(8),  ///< L2 button
  GAMEPAD_BUTTON_TR2    = TU_BIT(9),  ///< R2 button
  GAMEPAD_BUTTON_SELECT = TU_BIT(10), ///< Select button
  GAMEPAD_BUTTON_START  = TU_BIT(11), ///< Start button
  GAMEPAD_BUTTON_MODE   = TU_BIT(12), ///< Mode button
  GAMEPAD_BUTTON_THUMBL = TU_BIT(13), ///< L3 button
  GAMEPAD_BUTTON_THUMBR = TU_BIT(14), ///< R3 button
//GAMEPAD_BUTTON_       = TU_BIT(15), ///< Undefined button
}hid_gamepad_button_bm_t;

/// Switch Gamepad Buttons Bitmap
typedef enum
{
  GAMEPAD_NS_BUTTON_Y       = 0x01,
  GAMEPAD_NS_BUTTON_B       = 0x02,
  GAMEPAD_NS_BUTTON_A       = 0x04,
  GAMEPAD_NS_BUTTON_X       = 0x08,
  GAMEPAD_NS_BUTTON_TL      = 0x10,
  GAMEPAD_NS_BUTTON_TR      = 0x20,
  GAMEPAD_NS_BUTTON_TL2     = 0x40,
  GAMEPAD_NS_BUTTON_TR2     = 0x80,
  GAMEPAD_NS_BUTTON_MINUS   = 0x100,
  GAMEPAD_NS_BUTTON_PLUS    = 0x200,
  GAMEPAD_NS_BUTTON_THUMBL  = 0x400,
  GAMEPAD_NS_BUTTON_THUMBR  = 0x800,
  GAMEPAD_NS_BUTTON_HOME    = 0x1000,
  GAMEPAD_NS_BUTTON_CAPTURE = 0x2000,
  GAMEPAD_NS_BUTTON_Z       = 0x4000, ///UNUSED?
}hid_gamepad_ns_button_bm_t;

/// Standard Gamepad HAT/DPAD Buttons (from Linux input event codes)
typedef enum
{
  GAMEPAD_HAT_CENTERED   = 0,  ///< DPAD_CENTERED
  GAMEPAD_HAT_UP         = 1,  ///< DPAD_UP
  GAMEPAD_HAT_UP_RIGHT   = 2,  ///< DPAD_UP_RIGHT
  GAMEPAD_HAT_RIGHT      = 3,  ///< DPAD_RIGHT
  GAMEPAD_HAT_DOWN_RIGHT = 4,  ///< DPAD_DOWN_RIGHT
  GAMEPAD_HAT_DOWN       = 5,  ///< DPAD_DOWN
  GAMEPAD_HAT_DOWN_LEFT  = 6,  ///< DPAD_DOWN_LEFT
  GAMEPAD_HAT_LEFT       = 7,  ///< DPAD_LEFT
  GAMEPAD_HAT_UP_LEFT    = 8,  ///< DPAD_UP_LEFT
}hid_gamepad_hat_t;

/// Switch Gamepad HAT/DPAD Buttons (from Linux input event codes)
typedef enum
{
  GAMEPAD_NS_HAT_CENTERED   = 8,  ///< DPAD_CENTERED
  GAMEPAD_NS_HAT_UP         = 0,  ///< DPAD_UP
  GAMEPAD_NS_HAT_UP_RIGHT   = 1,  ///< DPAD_UP_RIGHT
  GAMEPAD_NS_HAT_RIGHT      = 2,  ///< DPAD_RIGHT
  GAMEPAD_NS_HAT_DOWN_RIGHT = 3,  ///< DPAD_DOWN_RIGHT
  GAMEPAD_NS_HAT_DOWN       = 4,  ///< DPAD_DOWN
  GAMEPAD_NS_HAT_DOWN_LEFT  = 5,  ///< DPAD_DOWN_LEFT
  GAMEPAD_NS_HAT_LEFT       = 6,  ///< DPAD_LEFT
  GAMEPAD_NS_HAT_UP_LEFT    = 7,  ///< DPAD_UP_LEFT
}hid_gamepad_ns_hat_t;

/// @}

//--------------------------------------------------------------------+
// MOUSE
//--------------------------------------------------------------------+
/** \addtogroup ClassDriver_HID_Mouse Mouse
 *  @{ */

/// Standard HID Boot Protocol Mouse Report.
typedef struct TU_ATTR_PACKED
{
  uint8_t buttons; /**< buttons mask for currently pressed buttons in the mouse. */
  int8_t  x;       /**< Current delta x movement of the mouse. */
  int8_t  y;       /**< Current delta y movement on the mouse. */
  int8_t  wheel;   /**< Current delta wheel movement on the mouse. */
  int8_t  pan;     // using AC Pan
} hid_mouse_report_t;

/// Standard Mouse Buttons Bitmap
typedef enum
{
  MOUSE_BUTTON_LEFT     = TU_BIT(0), ///< Left button
  MOUSE_BUTTON_RIGHT    = TU_BIT(1), ///< Right button
  MOUSE_BUTTON_MIDDLE   = TU_BIT(2), ///< Middle button
  MOUSE_BUTTON_BACKWARD = TU_BIT(3), ///< Backward button,
  MOUSE_BUTTON_FORWARD  = TU_BIT(4), ///< Forward button,
}hid_mouse_button_bm_t;

/// @}

//--------------------------------------------------------------------+
// Keyboard
//--------------------------------------------------------------------+
/** \addtogroup ClassDriver_HID_Keyboard Keyboard
 *  @{ */

/// Standard HID Boot Protocol Keyboard Report.
typedef struct TU_ATTR_PACKED
{
  uint8_t modifier;   /**< Keyboard modifier (KEYBOARD_MODIFIER_* masks). */
  uint8_t reserved;   /**< Reserved for OEM use, always set to 0. */
  uint8_t keycode[6]; /**< Key codes of the currently pressed keys. */
} hid_keyboard_report_t;

/// Keyboard modifier codes bitmap
typedef enum
{
  KEYBOARD_MODIFIER_LEFTCTRL   = TU_BIT(0), ///< Left Control
  KEYBOARD_MODIFIER_LEFTSHIFT  = TU_BIT(1), ///< Left Shift
  KEYBOARD_MODIFIER_LEFTALT    = TU_BIT(2), ///< Left Alt
  KEYBOARD_MODIFIER_LEFTGUI    = TU_BIT(3), ///< Left Window
  KEYBOARD_MODIFIER_RIGHTCTRL  = TU_BIT(4), ///< Right Control
  KEYBOARD_MODIFIER_RIGHTSHIFT = TU_BIT(5), ///< Right Shift
  KEYBOARD_MODIFIER_RIGHTALT   = TU_BIT(6), ///< Right Alt
  KEYBOARD_MODIFIER_RIGHTGUI   = TU_BIT(7)  ///< Right Window
}hid_keyboard_modifier_bm_t;

typedef enum
{
  KEYBOARD_LED_NUMLOCK    = TU_BIT(0), ///< Num Lock LED
  KEYBOARD_LED_CAPSLOCK   = TU_BIT(1), ///< Caps Lock LED
  KEYBOARD_LED_SCROLLLOCK = TU_BIT(2), ///< Scroll Lock LED
  KEYBOARD_LED_COMPOSE    = TU_BIT(3), ///< Composition Mode
  KEYBOARD_LED_KANA       = TU_BIT(4) ///< Kana mode
}hid_keyboard_led_bm_t;

/// @}

//--------------------------------------------------------------------+
// HID KEYCODE
//--------------------------------------------------------------------+
#define HID_KEY_NONE                      0x00
#define HID_KEY_A                         0x04
#define HID_KEY_B                         0x05
#define HID_KEY_C                         0x06
#define HID_KEY_D                         0x07
#define HID_KEY_E                         0x08
#define HID_KEY_F                         0x09
#define HID_KEY_G                         0x0A
#define HID_KEY_H                         0x0B
#define HID_KEY_I                         0x0C
#define HID_KEY_J                         0x0D
#define HID_KEY_K                         0x0E
#define HID_KEY_L                         0x0F
#define HID_KEY_M                         0x10
#define HID_KEY_N                         0x11
#define HID_KEY_O                         0x12
#define HID_KEY_P                         0x13
#define HID_KEY_Q                         0x14
#define HID_KEY_R                         0x15
#define HID_KEY_S                         0x16
#define HID_KEY_T                         0x17
#define HID_KEY_U                         0x18
#define HID_KEY_V                         0x19
#define HID_KEY_W                         0x1A
#define HID_KEY_X                         0x1B
#define HID_KEY_Y                         0x1C
#define HID_KEY_Z                         0x1D
#define HID_KEY_1                         0x1E
#define HID_KEY_2                         0x1F
#define HID_KEY_3                         0x20
#define HID_KEY_4                         0x21
#define HID_KEY_5                         0x22
#define HID_KEY_6                         0x23
#define HID_KEY_7                         0x24
#define HID_KEY_8                         0x25
#define HID_KEY_9                         0x26
#define HID_KEY_0                         0x27
#define HID_KEY_ENTER                     0x28
#define HID_KEY_ESCAPE                    0x29
#define HID_KEY_BACKSPACE                 0x2A
#define HID_KEY_TAB                       0x2B
#define HID_KEY_SPACE                     0x2C
#define HID_KEY_MINUS                     0x2D
#define HID_KEY_EQUAL                     0x2E
#define HID_KEY_BRACKET_LEFT              0x2F
#define HID_KEY_BRACKET_RIGHT             0x30
#define HID_KEY_BACKSLASH                 0x31
#define HID_KEY_EUROPE_1                  0x32
#define HID_KEY_SEMICOLON                 0x33
#define HID_KEY_APOSTROPHE                0x34
#define HID_KEY_GRAVE                     0x35
#define HID_KEY_COMMA                     0x36
#define HID_KEY_PERIOD                    0x37
#define HID_KEY_SLASH                     0x38
#define HID_KEY_CAPS_LOCK                 0x39
#define HID_KEY_F1                        0x3A
#define HID_KEY_F2                        0x3B
#define HID_KEY_F3                        0x3C
#define HID_KEY_F4                        0x3D
#define HID_KEY_F5                        0x3E
#define HID_KEY_F6                        0x3F
#define HID_KEY_F7                        0x40
#define HID_KEY_F8                        0x41
#define HID_KEY_F9                        0x42
#define HID_KEY_F10                       0x43
#define HID_KEY_F11                       0x44
#define HID_KEY_F12                       0x45
#define HID_KEY_PRINT_SCREEN              0x46
#define HID_KEY_SCROLL_LOCK               0x47
#define HID_KEY_PAUSE                     0x48
#define HID_KEY_INSERT                    0x49
#define HID_KEY_HOME                      0x4A
#define HID_KEY_PAGE_UP                   0x4B
#define HID_KEY_DELETE                    0x4C
#define HID_KEY_END                       0x4D
#define HID_KEY_PAGE_DOWN                 0x4E
#define HID_KEY_ARROW_RIGHT               0x4F
#define HID_KEY_ARROW_LEFT                0x50
#define HID_KEY_ARROW_DOWN                0x51
#define HID_KEY_ARROW_UP                  0x52
#define HID_KEY_NUM_LOCK                  0x53
#define HID_KEY_KEYPAD_DIVIDE             0x54
#define HID_KEY_KEYPAD_MULTIPLY           0x55
#define HID_KEY_KEYPAD_SUBTRACT           0x56
#define HID_KEY_KEYPAD_ADD                0x57
#define HID_KEY_KEYPAD_ENTER              0x58
#define HID_KEY_KEYPAD_1                  0x59
#define HID_KEY_KEYPAD_2                  0x5A
#define HID_KEY_KEYPAD_3                  0x5B
#define HID_KEY_KEYPAD_4                  0x5C
#define HID_KEY_KEYPAD_5                  0x5D
#define HID_KEY_KEYPAD_6                  0x5E
#define HID_KEY_KEYPAD_7                  0x5F
#define HID_KEY_KEYPAD_8                  0x60
#define HID_KEY_KEYPAD_9                  0x61
#define HID_KEY_KEYPAD_0                  0x62
#define HID_KEY_KEYPAD_DECIMAL            0x63
#define HID_KEY_EUROPE_2                  0x64
#define HID_KEY_APPLICATION               0x65
#define HID_KEY_POWER                     0x66
#define HID_KEY_KEYPAD_EQUAL              0x67
#define HID_KEY_F13                       0x68
#define HID_KEY_F14                       0x69
#define HID_KEY_F15                       0x6A
#define HID_KEY_F16                       0x6B
#define HID_KEY_F17                       0x6C
#define HID_KEY_F18                       0x6D
#define HID_KEY_F19                       0x6E
#define HID_KEY_F20                       0x6F
#define HID_KEY_F21                       0x70
#define HID_KEY_F22                       0x71
#define HID_KEY_F23                       0x72
#define HID_KEY_F24                       0x73
#define HID_KEY_EXECUTE                   0x74
#define HID_KEY_HELP                      0x75
#define HID_KEY_MENU                      0x76
#define HID_KEY_SELECT                    0x77
#define HID_KEY_STOP                      0x78
#define HID_KEY_AGAIN                     0x79
#define HID_KEY_UNDO                      0x7A
#define HID_KEY_CUT                       0x7B
#define HID_KEY_COPY                      0x7C
#define HID_KEY_PASTE                     0x7D
#define HID_KEY_FIND                      0x7E
#define HID_KEY_MUTE                      0x7F
#define HID_KEY_VOLUME_UP                 0x80
#define HID_KEY_VOLUME_DOWN               0x81
#define HID_KEY_LOCKING_CAPS_LOCK         0x82
#define HID_KEY_LOCKING_NUM_LOCK          0x83
#define HID_KEY_LOCKING_SCROLL_LOCK       0x84
#define HID_KEY_KEYPAD_COMMA              0x85
#define HID_KEY_KEYPAD_EQUAL_SIGN         0x86
#define HID_KEY_KANJI1                    0x87
#define HID_KEY_KANJI2                    0x88
#define HID_KEY_KANJI3                    0x89
#define HID_KEY_KANJI4                    0x8A
#define HID_KEY_KANJI5                    0x8B
#define HID_KEY_KANJI6                    0x8C
#define HID_KEY_KANJI7                    0x8D
#define HID_KEY_KANJI8                    0x8E
#define HID_KEY_KANJI9                    0x8F
#define HID_KEY_LANG1                     0x90
#define HID_KEY_LANG2                     0x91
#define HID_KEY_LANG3                     0x92
#define HID_KEY_LANG4                     0x93
#define HID_KEY_LANG5                     0x94
#define HID_KEY_LANG6                     0x95
#define HID_KEY_LANG7                     0x96
#define HID_KEY_LANG8                     0x97
#define HID_KEY_LANG9                     0x98
#define HID_KEY_ALTERNATE_ERASE           0x99
#define HID_KEY_SYSREQ_ATTENTION          0x9A
#define HID_KEY_CANCEL                    0x9B
#define HID_KEY_CLEAR                     0x9C
#define HID_KEY_PRIOR                     0x9D
#define HID_KEY_RETURN                    0x9E
#define HID_KEY_SEPARATOR                 0x9F
#define HID_KEY_OUT                       0xA0
#define HID_KEY_OPER                      0xA1
#define HID_KEY_CLEAR_AGAIN               0xA2
#define HID_KEY_CRSEL_PROPS               0xA3
#define HID_KEY_EXSEL                     0xA4
// RESERVED					                      0xA5-DF
#define HID_KEY_CONTROL_LEFT              0xE0
#define HID_KEY_SHIFT_LEFT                0xE1
#define HID_KEY_ALT_LEFT                  0xE2
#define HID_KEY_GUI_LEFT                  0xE3
#define HID_KEY_CONTROL_RIGHT             0xE4
#define HID_KEY_SHIFT_RIGHT               0xE5
#define HID_KEY_ALT_RIGHT                 0xE6
#define HID_KEY_GUI_RIGHT                 0xE7


//--------------------------------------------------------------------+
// REPORT DESCRIPTOR
//--------------------------------------------------------------------+
//------------- ITEM & TAG -------------//
#define HID_REPORT_DATA_0(data)
#define HID_REPORT_DATA_1(data) , data
#define HID_REPORT_DATA_2(data) , U16_TO_U8S_LE(data)
#define HID_REPORT_DATA_3(data) , U32_TO_U8S_LE(data)

#define HID_REPORT_ITEM(data, tag, type, size) \
  (((tag) << 4) | ((type) << 2) | (size)) HID_REPORT_DATA_##size(data)

#define RI_TYPE_MAIN   0
#define RI_TYPE_GLOBAL 1
#define RI_TYPE_LOCAL  2

//------------- MAIN ITEMS 6.2.2.4 -------------//
#define HID_INPUT(x)           HID_REPORT_ITEM(x,  8, RI_TYPE_MAIN, 1)
#define HID_OUTPUT(x)          HID_REPORT_ITEM(x,  9, RI_TYPE_MAIN, 1)
#define HID_COLLECTION(x)      HID_REPORT_ITEM(x, 10, RI_TYPE_MAIN, 1)
#define HID_FEATURE(x)         HID_REPORT_ITEM(x, 11, RI_TYPE_MAIN, 1)
#define HID_COLLECTION_END     HID_REPORT_ITEM(x, 12, RI_TYPE_MAIN, 0)

//------------- INPUT, OUTPUT, FEATURE 6.2.2.5 -------------//
#define HID_DATA             (0<<0)
#define HID_CONSTANT         (1<<0)

#define HID_ARRAY            (0<<1)
#define HID_VARIABLE         (1<<1)

#define HID_ABSOLUTE         (0<<2)
#define HID_RELATIVE         (1<<2)

#define HID_WRAP_NO          (0<<3)
#define HID_WRAP             (1<<3)

#define HID_LINEAR           (0<<4)
#define HID_NONLINEAR        (1<<4)

#define HID_PREFERRED_STATE  (0<<5)
#define HID_PREFERRED_NO     (1<<5)

#define HID_NO_NULL_POSITION (0<<6)
#define HID_NULL_STATE       (1<<6)

#define HID_NON_VOLATILE     (0<<7)
#define HID_VOLATILE         (1<<7)

#define HID_BITFIELD         (0<<8)
#define HID_BUFFERED_BYTES   (1<<8)

//------------- COLLECTION ITEM 6.2.2.6 -------------//
enum {
  HID_COLLECTION_PHYSICAL = 0,
  HID_COLLECTION_APPLICATION,
  HID_COLLECTION_LOGICAL,
  HID_COLLECTION_REPORT,
  HID_COLLECTION_NAMED_ARRAY,
  HID_COLLECTION_USAGE_SWITCH,
  HID_COLLECTION_USAGE_MODIFIER
};

//------------- GLOBAL ITEMS 6.2.2.7 -------------//
#define HID_USAGE_PAGE(x)         HID_REPORT_ITEM(x, 0, RI_TYPE_GLOBAL, 1)
#define HID_USAGE_PAGE_N(x, n)    HID_REPORT_ITEM(x, 0, RI_TYPE_GLOBAL, n)

#define HID_LOGICAL_MIN(x)        HID_REPORT_ITEM(x, 1, RI_TYPE_GLOBAL, 1)
#define HID_LOGICAL_MIN_N(x, n)   HID_REPORT_ITEM(x, 1, RI_TYPE_GLOBAL, n)

#define HID_LOGICAL_MAX(x)        HID_REPORT_ITEM(x, 2, RI_TYPE_GLOBAL, 1)
#define HID_LOGICAL_MAX_N(x, n)   HID_REPORT_ITEM(x, 2, RI_TYPE_GLOBAL, n)

#define HID_PHYSICAL_MIN(x)       HID_REPORT_ITEM(x, 3, RI_TYPE_GLOBAL, 1)
#define HID_PHYSICAL_MIN_N(x, n)  HID_REPORT_ITEM(x, 3, RI_TYPE_GLOBAL, n)

#define HID_PHYSICAL_MAX(x)       HID_REPORT_ITEM(x, 4, RI_TYPE_GLOBAL, 1)
#define HID_PHYSICAL_MAX_N(x, n)  HID_REPORT_ITEM(x, 4, RI_TYPE_GLOBAL, n)

#define HID_UNIT_EXPONENT(x)      HID_REPORT_ITEM(x, 5, RI_TYPE_GLOBAL, 1)
#define HID_UNIT_EXPONENT_N(x, n) HID_REPORT_ITEM(x, 5, RI_TYPE_GLOBAL, n)

#define HID_UNIT(x)               HID_REPORT_ITEM(x, 6, RI_TYPE_GLOBAL, 1)
#define HID_UNIT_N(x, n)          HID_REPORT_ITEM(x, 6, RI_TYPE_GLOBAL, n)

#define HID_REPORT_SIZE(x)        HID_REPORT_ITEM(x, 7, RI_TYPE_GLOBAL, 1)
#define HID_REPORT_SIZE_N(x, n)   HID_REPORT_ITEM(x, 7, RI_TYPE_GLOBAL, n)

#define HID_REPORT_ID(x)          HID_REPORT_ITEM(x, 8, RI_TYPE_GLOBAL, 1),
#define HID_REPORT_ID_N(x)        HID_REPORT_ITEM(x, 8, RI_TYPE_GLOBAL, n),

#define HID_REPORT_COUNT(x)       HID_REPORT_ITEM(x, 9, RI_TYPE_GLOBAL, 1)
#define HID_REPORT_COUNT_N(x, n)  HID_REPORT_ITEM(x, 9, RI_TYPE_GLOBAL, n)

#define HID_PUSH                  HID_REPORT_ITEM(x, 10, RI_TYPE_GLOBAL, 0)
#define HID_POP                   HID_REPORT_ITEM(x, 11, RI_TYPE_GLOBAL, 0)

//------------- LOCAL ITEMS 6.2.2.8 -------------//
#define HID_USAGE(x)              HID_REPORT_ITEM(x, 0, RI_TYPE_LOCAL, 1)
#define HID_USAGE_N(x, n)         HID_REPORT_ITEM(x, 0, RI_TYPE_LOCAL, n)

#define HID_USAGE_MIN(x)          HID_REPORT_ITEM(x, 1, RI_TYPE_LOCAL, 1)
#define HID_USAGE_MIN_N(x, n)     HID_REPORT_ITEM(x, 1, RI_TYPE_LOCAL, n)

#define HID_USAGE_MAX(x)          HID_REPORT_ITEM(x, 2, RI_TYPE_LOCAL, 1)
#define HID_USAGE_MAX_N(x, n)     HID_REPORT_ITEM(x, 2, RI_TYPE_LOCAL, n)

//--------------------------------------------------------------------+
// Usage Table
//--------------------------------------------------------------------+

/// HID Usage Table - Table 1: Usage Page Summary
enum {
  HID_USAGE_PAGE_DESKTOP         = 0x01,
  HID_USAGE_PAGE_SIMULATE        = 0x02,
  HID_USAGE_PAGE_VIRTUAL_REALITY = 0x03,
  HID_USAGE_PAGE_SPORT           = 0x04,
  HID_USAGE_PAGE_GAME            = 0x05,
  HID_USAGE_PAGE_GENERIC_DEVICE  = 0x06,
  HID_USAGE_PAGE_KEYBOARD        = 0x07,
  HID_USAGE_PAGE_LED             = 0x08,
  HID_USAGE_PAGE_BUTTON          = 0x09,
  HID_USAGE_PAGE_ORDINAL         = 0x0a,
  HID_USAGE_PAGE_TELEPHONY       = 0x0b,
  HID_USAGE_PAGE_CONSUMER        = 0x0c,
  HID_USAGE_PAGE_DIGITIZER       = 0x0d,
  HID_USAGE_PAGE_PID             = 0x0f,
  HID_USAGE_PAGE_UNICODE         = 0x10,
  HID_USAGE_PAGE_ALPHA_DISPLAY   = 0x14,
  HID_USAGE_PAGE_MEDICAL         = 0x40,
  HID_USAGE_PAGE_MONITOR         = 0x80, //0x80 - 0x83
  HID_USAGE_PAGE_POWER           = 0x84, // 0x084 - 0x87
  HID_USAGE_PAGE_BARCODE_SCANNER = 0x8c,
  HID_USAGE_PAGE_SCALE           = 0x8d,
  HID_USAGE_PAGE_MSR             = 0x8e,
  HID_USAGE_PAGE_CAMERA          = 0x90,
  HID_USAGE_PAGE_ARCADE          = 0x91,
  HID_USAGE_PAGE_VENDOR          = 0xFF00 // 0xFF00 - 0xFFFF
};

/// HID Usage Table - Table 6: Generic Desktop Page
enum {
  HID_USAGE_DESKTOP_POINTER                               = 0x01,
  HID_USAGE_DESKTOP_MOUSE                                 = 0x02,
  HID_USAGE_DESKTOP_JOYSTICK                              = 0x04,
  HID_USAGE_DESKTOP_GAMEPAD                               = 0x05,
  HID_USAGE_DESKTOP_KEYBOARD                              = 0x06,
  HID_USAGE_DESKTOP_KEYPAD                                = 0x07,
  HID_USAGE_DESKTOP_MULTI_AXIS_CONTROLLER                 = 0x08,
  HID_USAGE_DESKTOP_TABLET_PC_SYSTEM                      = 0x09,
  HID_USAGE_DESKTOP_X                                     = 0x30,
  HID_USAGE_DESKTOP_Y                                     = 0x31,
  HID_USAGE_DESKTOP_Z                                     = 0x32,
  HID_USAGE_DESKTOP_RX                                    = 0x33,
  HID_USAGE_DESKTOP_RY                                    = 0x34,
  HID_USAGE_DESKTOP_RZ                                    = 0x35,
  HID_USAGE_DESKTOP_SLIDER                                = 0x36,
  HID_USAGE_DESKTOP_DIAL                                  = 0x37,
  HID_USAGE_DESKTOP_WHEEL                                 = 0x38,
  HID_USAGE_DESKTOP_HAT_SWITCH                            = 0x39,
  HID_USAGE_DESKTOP_COUNTED_BUFFER                        = 0x3a,
  HID_USAGE_DESKTOP_BYTE_COUNT                            = 0x3b,
  HID_USAGE_DESKTOP_MOTION_WAKEUP                         = 0x3c,
  HID_USAGE_DESKTOP_START                                 = 0x3d,
  HID_USAGE_DESKTOP_SELECT                                = 0x3e,
  HID_USAGE_DESKTOP_VX                                    = 0x40,
  HID_USAGE_DESKTOP_VY                                    = 0x41,
  HID_USAGE_DESKTOP_VZ                                    = 0x42,
  HID_USAGE_DESKTOP_VBRX                                  = 0x43,
  HID_USAGE_DESKTOP_VBRY                                  = 0x44,
  HID_USAGE_DESKTOP_VBRZ                                  = 0x45,
  HID_USAGE_DESKTOP_VNO                                   = 0x46,
  HID_USAGE_DESKTOP_FEATURE_NOTIFICATION                  = 0x47,
  HID_USAGE_DESKTOP_RESOLUTION_MULTIPLIER                 = 0x48,
  HID_USAGE_DESKTOP_SYSTEM_CONTROL                        = 0x80,
  HID_USAGE_DESKTOP_SYSTEM_POWER_DOWN                     = 0x81,
  HID_USAGE_DESKTOP_SYSTEM_SLEEP                          = 0x82,
  HID_USAGE_DESKTOP_SYSTEM_WAKE_UP                        = 0x83,
  HID_USAGE_DESKTOP_SYSTEM_CONTEXT_MENU                   = 0x84,
  HID_USAGE_DESKTOP_SYSTEM_MAIN_MENU                      = 0x85,
  HID_USAGE_DESKTOP_SYSTEM_APP_MENU                       = 0x86,
  HID_USAGE_DESKTOP_SYSTEM_MENU_HELP                      = 0x87,
  HID_USAGE_DESKTOP_SYSTEM_MENU_EXIT                      = 0x88,
  HID_USAGE_DESKTOP_SYSTEM_MENU_SELECT                    = 0x89,
  HID_USAGE_DESKTOP_SYSTEM_MENU_RIGHT                     = 0x8A,
  HID_USAGE_DESKTOP_SYSTEM_MENU_LEFT                      = 0x8B,
  HID_USAGE_DESKTOP_SYSTEM_MENU_UP                        = 0x8C,
  HID_USAGE_DESKTOP_SYSTEM_MENU_DOWN                      = 0x8D,
  HID_USAGE_DESKTOP_SYSTEM_COLD_RESTART                   = 0x8E,
  HID_USAGE_DESKTOP_SYSTEM_WARM_RESTART                   = 0x8F,
  HID_USAGE_DESKTOP_DPAD_UP                               = 0x90,
  HID_USAGE_DESKTOP_DPAD_DOWN                             = 0x91,
  HID_USAGE_DESKTOP_DPAD_RIGHT                            = 0x92,
  HID_USAGE_DESKTOP_DPAD_LEFT                             = 0x93,
  HID_USAGE_DESKTOP_SYSTEM_DOCK                           = 0xA0,
  HID_USAGE_DESKTOP_SYSTEM_UNDOCK                         = 0xA1,
  HID_USAGE_DESKTOP_SYSTEM_SETUP                          = 0xA2,
  HID_USAGE_DESKTOP_SYSTEM_BREAK                          = 0xA3,
  HID_USAGE_DESKTOP_SYSTEM_DEBUGGER_BREAK                 = 0xA4,
  HID_USAGE_DESKTOP_APPLICATION_BREAK                     = 0xA5,
  HID_USAGE_DESKTOP_APPLICATION_DEBUGGER_BREAK            = 0xA6,
  HID_USAGE_DESKTOP_SYSTEM_SPEAKER_MUTE                   = 0xA7,
  HID_USAGE_DESKTOP_SYSTEM_HIBERNATE                      = 0xA8,
  HID_USAGE_DESKTOP_SYSTEM_DISPLAY_INVERT                 = 0xB0,
  HID_USAGE_DESKTOP_SYSTEM_DISPLAY_INTERNAL               = 0xB1,
  HID_USAGE_DESKTOP_SYSTEM_DISPLAY_EXTERNAL               = 0xB2,
  HID_USAGE_DESKTOP_SYSTEM_DISPLAY_BOTH                   = 0xB3,
  HID_USAGE_DESKTOP_SYSTEM_DISPLAY_DUAL                   = 0xB4,
  HID_USAGE_DESKTOP_SYSTEM_DISPLAY_TOGGLE_INT_EXT         = 0xB5,
  HID_USAGE_DESKTOP_SYSTEM_DISPLAY_SWAP_PRIMARY_SECONDARY = 0xB6,
  HID_USAGE_DESKTOP_SYSTEM_DISPLAY_LCD_AUTOSCALE          = 0xB7
};


/// HID Usage Table: Consumer Page (0x0C)
/// Only contains controls that supported by Windows (whole list is too long)
enum
{
  // Generic Control
  HID_USAGE_CONSUMER_CONTROL                           = 0x0001,

  // Power Control
  HID_USAGE_CONSUMER_POWER                             = 0x0030,
  HID_USAGE_CONSUMER_RESET                             = 0x0031,
  HID_USAGE_CONSUMER_SLEEP                             = 0x0032,

  // Screen Brightness
  HID_USAGE_CONSUMER_BRIGHTNESS_INCREMENT              = 0x006F,
  HID_USAGE_CONSUMER_BRIGHTNESS_DECREMENT              = 0x0070,

  // These HID usages operate only on mobile systems (battery powered) and
  // require Windows 8 (build 8302 or greater).
  HID_USAGE_CONSUMER_WIRELESS_RADIO_CONTROLS           = 0x000C,
  HID_USAGE_CONSUMER_WIRELESS_RADIO_BUTTONS            = 0x00C6,
  HID_USAGE_CONSUMER_WIRELESS_RADIO_LED                = 0x00C7,
  HID_USAGE_CONSUMER_WIRELESS_RADIO_SLIDER_SWITCH      = 0x00C8,

  // Media Control
  HID_USAGE_CONSUMER_PLAY_PAUSE                        = 0x00CD,
  HID_USAGE_CONSUMER_SCAN_NEXT                         = 0x00B5,
  HID_USAGE_CONSUMER_SCAN_PREVIOUS                     = 0x00B6,
  HID_USAGE_CONSUMER_STOP                              = 0x00B7,
  HID_USAGE_CONSUMER_VOLUME                            = 0x00E0,
  HID_USAGE_CONSUMER_MUTE                              = 0x00E2,
  HID_USAGE_CONSUMER_BASS                              = 0x00E3,
  HID_USAGE_CONSUMER_TREBLE                            = 0x00E4,
  HID_USAGE_CONSUMER_BASS_BOOST                        = 0x00E5,
  HID_USAGE_CONSUMER_VOLUME_INCREMENT                  = 0x00E9,
  HID_USAGE_CONSUMER_VOLUME_DECREMENT                  = 0x00EA,
  HID_USAGE_CONSUMER_BASS_INCREMENT                    = 0x0152,
  HID_USAGE_CONSUMER_BASS_DECREMENT                    = 0x0153,
  HID_USAGE_CONSUMER_TREBLE_INCREMENT                  = 0x0154,
  HID_USAGE_CONSUMER_TREBLE_DECREMENT                  = 0x0155,

  // Application Launcher
  HID_USAGE_CONSUMER_AL_CONSUMER_CONTROL_CONFIGURATION = 0x0183,
  HID_USAGE_CONSUMER_AL_EMAIL_READER                   = 0x018A,
  HID_USAGE_CONSUMER_AL_CALCULATOR                     = 0x0192,
  HID_USAGE_CONSUMER_AL_LOCAL_BROWSER                  = 0x0194,

  // Browser/Explorer Specific
  HID_USAGE_CONSUMER_AC_SEARCH                         = 0x0221,
  HID_USAGE_CONSUMER_AC_HOME                           = 0x0223,
  HID_USAGE_CONSUMER_AC_BACK                           = 0x0224,
  HID_USAGE_CONSUMER_AC_FORWARD                        = 0x0225,
  HID_USAGE_CONSUMER_AC_STOP                           = 0x0226,
  HID_USAGE_CONSUMER_AC_REFRESH                        = 0x0227,
  HID_USAGE_CONSUMER_AC_BOOKMARKS                      = 0x022A,

  // Mouse Horizontal scroll
  HID_USAGE_CONSUMER_AC_PAN                            = 0x0238,
};

/*--------------------------------------------------------------------
 * ASCII to KEYCODE Conversion
 *  Expand to array of [128][2] (shift, keycode)
 *
 * Usage: example to convert input chr into keyboard report (modifier + keycode)
 *
 *  uint8_t const conv_table[128][2] =  { HID_ASCII_TO_KEYCODE };
 *
 *  uint8_t keycode[6] = { 0 };
 *  uint8_t modifier   = 0;
 *
 *  if ( conv_table[chr][0] ) modifier = KEYBOARD_MODIFIER_LEFTSHIFT;
 *  keycode[0] = conv_table[chr][1];
 *  tud_hid_keyboard_report(report_id, modifier, keycode);
 *
 *--------------------------------------------------------------------*/
#define HID_ASCII_TO_KEYCODE \
    {0, 0                     }, /* 0x00 Null      */ \
    {0, 0                     }, /* 0x01           */ \
    {0, 0                     }, /* 0x02           */ \
    {0, 0                     }, /* 0x03           */ \
    {0, 0                     }, /* 0x04           */ \
    {0, 0                     }, /* 0x05           */ \
    {0, 0                     }, /* 0x06           */ \
    {0, 0                     }, /* 0x07           */ \
    {0, HID_KEY_BACKSPACE     }, /* 0x08 Backspace */ \
    {0, HID_KEY_TAB           }, /* 0x09 Tab       */ \
    {0, HID_KEY_RETURN        }, /* 0x0A Line Feed */ \
    {0, 0                     }, /* 0x0B           */ \
    {0, 0                     }, /* 0x0C           */ \
    {0, HID_KEY_RETURN        }, /* 0x0D CR        */ \
    {0, 0                     }, /* 0x0E           */ \
    {0, 0                     }, /* 0x0F           */ \
    {0, 0                     }, /* 0x10           */ \
    {0, 0                     }, /* 0x11           */ \
    {0, 0                     }, /* 0x12           */ \
    {0, 0                     }, /* 0x13           */ \
    {0, 0                     }, /* 0x14           */ \
    {0, 0                     }, /* 0x15           */ \
    {0, 0                     }, /* 0x16           */ \
    {0, 0                     }, /* 0x17           */ \
    {0, 0                     }, /* 0x18           */ \
    {0, 0                     }, /* 0x19           */ \
    {0, 0                     }, /* 0x1A           */ \
    {0, HID_KEY_ESCAPE        }, /* 0x1B Escape    */ \
    {0, 0                     }, /* 0x1C           */ \
    {0, 0                     }, /* 0x1D           */ \
    {0, 0                     }, /* 0x1E           */ \
    {0, 0                     }, /* 0x1F           */ \
                                                      \
    {0, HID_KEY_SPACE         }, /* 0x20           */ \
    {1, HID_KEY_1             }, /* 0x21 !         */ \
    {1, HID_KEY_APOSTROPHE    }, /* 0x22 "         */ \
    {1, HID_KEY_3             }, /* 0x23 #         */ \
    {1, HID_KEY_4             }, /* 0x24 $         */ \
    {1, HID_KEY_5             }, /* 0x25 %         */ \
    {1, HID_KEY_7             }, /* 0x26 &         */ \
    {0, HID_KEY_APOSTROPHE    }, /* 0x27 '         */ \
    {1, HID_KEY_9             }, /* 0x28 (         */ \
    {1, HID_KEY_0             }, /* 0x29 )         */ \
    {1, HID_KEY_8             }, /* 0x2A *         */ \
    {1, HID_KEY_EQUAL         }, /* 0x2B +         */ \
    {0, HID_KEY_COMMA         }, /* 0x2C ,         */ \
    {0, HID_KEY_MINUS         }, /* 0x2D -         */ \
    {0, HID_KEY_PERIOD        }, /* 0x2E .         */ \
    {0, HID_KEY_SLASH         }, /* 0x2F /         */ \
    {0, HID_KEY_0             }, /* 0x30 0         */ \
    {0, HID_KEY_1             }, /* 0x31 1         */ \
    {0, HID_KEY_2             }, /* 0x32 2         */ \
    {0, HID_KEY_3             }, /* 0x33 3         */ \
    {0, HID_KEY_4             }, /* 0x34 4         */ \
    {0, HID_KEY_5             }, /* 0x35 5         */ \
    {0, HID_KEY_6             }, /* 0x36 6         */ \
    {0, HID_KEY_7             }, /* 0x37 7         */ \
    {0, HID_KEY_8             }, /* 0x38 8         */ \
    {0, HID_KEY_9             }, /* 0x39 9         */ \
    {1, HID_KEY_SEMICOLON     }, /* 0x3A :         */ \
    {0, HID_KEY_SEMICOLON     }, /* 0x3B ;         */ \
    {1, HID_KEY_COMMA         }, /* 0x3C <         */ \
    {0, HID_KEY_EQUAL         }, /* 0x3D =         */ \
    {1, HID_KEY_PERIOD        }, /* 0x3E >         */ \
    {1, HID_KEY_SLASH         }, /* 0x3F ?         */ \
                                                      \
    {1, HID_KEY_2             }, /* 0x40 @         */ \
    {1, HID_KEY_A             }, /* 0x41 A         */ \
    {1, HID_KEY_B             }, /* 0x42 B         */ \
    {1, HID_KEY_C             }, /* 0x43 C         */ \
    {1, HID_KEY_D             }, /* 0x44 D         */ \
    {1, HID_KEY_E             }, /* 0x45 E         */ \
    {1, HID_KEY_F             }, /* 0x46 F         */ \
    {1, HID_KEY_G             }, /* 0x47 G         */ \
    {1, HID_KEY_H             }, /* 0x48 H         */ \
    {1, HID_KEY_I             }, /* 0x49 I         */ \
    {1, HID_KEY_J             }, /* 0x4A J         */ \
    {1, HID_KEY_K             }, /* 0x4B K         */ \
    {1, HID_KEY_L             }, /* 0x4C L         */ \
    {1, HID_KEY_M             }, /* 0x4D M         */ \
    {1, HID_KEY_N             }, /* 0x4E N         */ \
    {1, HID_KEY_O             }, /* 0x4F O         */ \
    {1, HID_KEY_P             }, /* 0x50 P         */ \
    {1, HID_KEY_Q             }, /* 0x51 Q         */ \
    {1, HID_KEY_R             }, /* 0x52 R         */ \
    {1, HID_KEY_S             }, /* 0x53 S         */ \
    {1, HID_KEY_T             }, /* 0x55 T         */ \
    {1, HID_KEY_U             }, /* 0x55 U         */ \
    {1, HID_KEY_V             }, /* 0x56 V         */ \
    {1, HID_KEY_W             }, /* 0x57 W         */ \
    {1, HID_KEY_X             }, /* 0x58 X         */ \
    {1, HID_KEY_Y             }, /* 0x59 Y         */ \
    {1, HID_KEY_Z             }, /* 0x5A Z         */ \
    {0, HID_KEY_BRACKET_LEFT  }, /* 0x5B [         */ \
    {0, HID_KEY_BACKSLASH     }, /* 0x5C '\'       */ \
    {0, HID_KEY_BRACKET_RIGHT }, /* 0x5D ]         */ \
    {1, HID_KEY_6             }, /* 0x5E ^         */ \
    {1, HID_KEY_MINUS         }, /* 0x5F _         */ \
                                                      \
    {0, HID_KEY_GRAVE         }, /* 0x60 `         */ \
    {0, HID_KEY_A             }, /* 0x61 a         */ \
    {0, HID_KEY_B             }, /* 0x62 b         */ \
    {0, HID_KEY_C             }, /* 0x63 c         */ \
    {0, HID_KEY_D             }, /* 0x66 d         */ \
    {0, HID_KEY_E             }, /* 0x65 e         */ \
    {0, HID_KEY_F             }, /* 0x66 f         */ \
    {0, HID_KEY_G             }, /* 0x67 g         */ \
    {0, HID_KEY_H             }, /* 0x68 h         */ \
    {0, HID_KEY_I             }, /* 0x69 i         */ \
    {0, HID_KEY_J             }, /* 0x6A j         */ \
    {0, HID_KEY_K             }, /* 0x6B k         */ \
    {0, HID_KEY_L             }, /* 0x6C l         */ \
    {0, HID_KEY_M             }, /* 0x6D m         */ \
    {0, HID_KEY_N             }, /* 0x6E n         */ \
    {0, HID_KEY_O             }, /* 0x6F o         */ \
    {0, HID_KEY_P             }, /* 0x70 p         */ \
    {0, HID_KEY_Q             }, /* 0x71 q         */ \
    {0, HID_KEY_R             }, /* 0x72 r         */ \
    {0, HID_KEY_S             }, /* 0x73 s         */ \
    {0, HID_KEY_T             }, /* 0x75 t         */ \
    {0, HID_KEY_U             }, /* 0x75 u         */ \
    {0, HID_KEY_V             }, /* 0x76 v         */ \
    {0, HID_KEY_W             }, /* 0x77 w         */ \
    {0, HID_KEY_X             }, /* 0x78 x         */ \
    {0, HID_KEY_Y             }, /* 0x79 y         */ \
    {0, HID_KEY_Z             }, /* 0x7A z         */ \
    {1, HID_KEY_BRACKET_LEFT  }, /* 0x7B {         */ \
    {1, HID_KEY_BACKSLASH     }, /* 0x7C |         */ \
    {1, HID_KEY_BRACKET_RIGHT }, /* 0x7D }         */ \
    {1, HID_KEY_GRAVE         }, /* 0x7E ~         */ \
    {0, HID_KEY_DELETE        }  /* 0x7F Delete    */ \

/*--------------------------------------------------------------------
 * KEYCODE to Ascii Conversion
 *  Expand to array of [128][2] (ascii without shift, ascii with shift)
 *
 * Usage: example to convert ascii from keycode (key) and shift modifier (shift).
 * Here we assume key < 128 ( printable )
 *
 *  uint8_t const conv_table[128][2] =  { HID_KEYCODE_TO_ASCII };
 *  char ch = shift ? conv_table[chr][1] : conv_table[chr][0];
 *
 *--------------------------------------------------------------------*/
#define HID_KEYCODE_TO_ASCII    \
    {0     , 0      }, /* 0x00 */ \
    {0     , 0      }, /* 0x01 */ \
    {0     , 0      }, /* 0x02 */ \
    {0     , 0      }, /* 0x03 */ \
    {'a'   , 'A'    }, /* 0x04 */ \
    {'b'   , 'B'    }, /* 0x05 */ \
    {'c'   , 'C'    }, /* 0x06 */ \
    {'d'   , 'D'    }, /* 0x07 */ \
    {'e'   , 'E'    }, /* 0x08 */ \
    {'f'   , 'F'    }, /* 0x09 */ \
    {'g'   , 'G'    }, /* 0x0a */ \
    {'h'   , 'H'    }, /* 0x0b */ \
    {'i'   , 'I'    }, /* 0x0c */ \
    {'j'   , 'J'    }, /* 0x0d */ \
    {'k'   , 'K'    }, /* 0x0e */ \
    {'l'   , 'L'    }, /* 0x0f */ \
    {'m'   , 'M'    }, /* 0x10 */ \
    {'n'   , 'N'    }, /* 0x11 */ \
    {'o'   , 'O'    }, /* 0x12 */ \
    {'p'   , 'P'    }, /* 0x13 */ \
    {'q'   , 'Q'    }, /* 0x14 */ \
    {'r'   , 'R'    }, /* 0x15 */ \
    {'s'   , 'S'    }, /* 0x16 */ \
    {'t'   , 'T'    }, /* 0x17 */ \
    {'u'   , 'U'    }, /* 0x18 */ \
    {'v'   , 'V'    }, /* 0x19 */ \
    {'w'   , 'W'    }, /* 0x1a */ \
    {'x'   , 'X'    }, /* 0x1b */ \
    {'y'   , 'Y'    }, /* 0x1c */ \
    {'z'   , 'Z'    }, /* 0x1d */ \
    {'1'   , '!'    }, /* 0x1e */ \
    {'2'   , '@'    }, /* 0x1f */ \
    {'3'   , '#'    }, /* 0x20 */ \
    {'4'   , '$'    }, /* 0x21 */ \
    {'5'   , '%'    }, /* 0x22 */ \
    {'6'   , '^'    }, /* 0x23 */ \
    {'7'   , '&'    }, /* 0x24 */ \
    {'8'   , '*'    }, /* 0x25 */ \
    {'9'   , '('    }, /* 0x26 */ \
    {'0'   , ')'    }, /* 0x27 */ \
    {'\r'  , '\r'   }, /* 0x28 */ \
    {'\x1b', '\x1b' }, /* 0x29 */ \
    {'\b'  , '\b'   }, /* 0x2a */ \
    {'\t'  , '\t'   }, /* 0x2b */ \
    {' '   , ' '    }, /* 0x2c */ \
    {'-'   , '_'    }, /* 0x2d */ \
    {'='   , '+'    }, /* 0x2e */ \
    {'['   , '{'    }, /* 0x2f */ \
    {']'   , '}'    }, /* 0x30 */ \
    {'\\'  , '|'    }, /* 0x31 */ \
    {'#'   , '~'    }, /* 0x32 */ \
    {';'   , ':'    }, /* 0x33 */ \
    {'\''  , '\"'   }, /* 0x34 */ \
    {'`'   , '~'    }, /* 0x35 */ \
    {','   , '<'    }, /* 0x36 */ \
    {'.'   , '>'    }, /* 0x37 */ \
    {'/'   , '?'    }, /* 0x38 */ \
                                  \
    {0     , 0      }, /* 0x39 */ \
    {0     , 0      }, /* 0x3a */ \
    {0     , 0      }, /* 0x3b */ \
    {0     , 0      }, /* 0x3c */ \
    {0     , 0      }, /* 0x3d */ \
    {0     , 0      }, /* 0x3e */ \
    {0     , 0      }, /* 0x3f */ \
    {0     , 0      }, /* 0x40 */ \
    {0     , 0      }, /* 0x41 */ \
    {0     , 0      }, /* 0x42 */ \
    {0     , 0      }, /* 0x43 */ \
    {0     , 0      }, /* 0x44 */ \
    {0     , 0      }, /* 0x45 */ \
    {0     , 0      }, /* 0x46 */ \
    {0     , 0      }, /* 0x47 */ \
    {0     , 0      }, /* 0x48 */ \
    {0     , 0      }, /* 0x49 */ \
    {0     , 0      }, /* 0x4a */ \
    {0     , 0      }, /* 0x4b */ \
    {0     , 0      }, /* 0x4c */ \
    {0     , 0      }, /* 0x4d */ \
    {0     , 0      }, /* 0x4e */ \
    {0     , 0      }, /* 0x4f */ \
    {0     , 0      }, /* 0x50 */ \
    {0     , 0      }, /* 0x51 */ \
    {0     , 0      }, /* 0x52 */ \
    {0     , 0      }, /* 0x53 */ \
                                  \
    {'/'   , '/'    }, /* 0x54 */ \
    {'*'   , '*'    }, /* 0x55 */ \
    {'-'   , '-'    }, /* 0x56 */ \
    {'+'   , '+'    }, /* 0x57 */ \
    {'\r'  , '\r'   }, /* 0x58 */ \
    {'1'   , 0      }, /* 0x59 */ \
    {'2'   , 0      }, /* 0x5a */ \
    {'3'   , 0      }, /* 0x5b */ \
    {'4'   , 0      }, /* 0x5c */ \
    {'5'   , '5'    }, /* 0x5d */ \
    {'6'   , 0      }, /* 0x5e */ \
    {'7'   , 0      }, /* 0x5f */ \
    {'8'   , 0      }, /* 0x60 */ \
    {'9'   , 0      }, /* 0x61 */ \
    {'0'   , 0      }, /* 0x62 */ \
    {'0'   , 0      }, /* 0x63 */ \
    {'='   , '='    }, /* 0x67 */ \


#ifdef __cplusplus
 }
#endif





#endif /* _TUSB_TYPES_H_ */



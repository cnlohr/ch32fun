#ifdef _WIN32

/*******************************************************
 HIDAPI - Multi-Platform library for
 communication with HID devices.

 Alan Ott
 Signal 11 Software

 libusb/hidapi Team

 Copyright 2022, All Rights Reserved.

 At the discretion of the user of this library,
 this software may be licensed under the terms of the
 GNU General Public License v3, a BSD-Style license, or the
 original HIDAPI license as outlined in the LICENSE.txt,
 LICENSE-gpl3.txt, LICENSE-bsd.txt, and LICENSE-orig.txt
 files located at the root of the source distribution.
 These files may also be found in the public source
 code repository located at:
        https://github.com/libusb/hidapi .
********************************************************/

#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)
/* Do not warn about wcsncpy usage.
   https://docs.microsoft.com/cpp/c-runtime-library/security-features-in-the-crt */
#define _CRT_SECURE_NO_WARNINGS
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include <windows.h>

#ifndef _NTDEF_
typedef LONG NTSTATUS;
#endif

#ifdef __MINGW32__
#include <ntdef.h>
#include <winbase.h>
#define WC_ERR_INVALID_CHARS 0x00000080
#endif

#ifdef __CYGWIN__
#include <ntdef.h>
#include <wctype.h>
#define _wcsdup wcsdup
#endif

/*#define HIDAPI_USE_DDK*/

typedef USHORT USAGE;

#ifdef HIDAPI_USE_DDK

#include <cfgmgr32.h>
#include <initguid.h>
#include <devpkey.h>
#include <propkey.h>
#include <hidpi.h>
#include <hidsdi.h>

#else

/* This part of the header mimics cfgmgr32.h,
    but only what is used by HIDAPI */

#define INITGUID

#ifdef __TINYC__

#define WC_ERR_INVALID_CHARS      0x00000080  // error for invalid chars
typedef struct _tagpropertykey {
  GUID  fmtid;
  DWORD pid;
} PROPERTYKEY;
typedef WCHAR          *PZZWSTR,    *PUZZWSTR;

#ifndef _DEVPROPDEF_H_
#define _DEVPROPDEF_H_

typedef ULONG DEVPROPTYPE, *PDEVPROPTYPE;

#define DEVPROP_TYPEMOD_ARRAY                   0x00001000
#define DEVPROP_TYPEMOD_LIST                    0x00002000

#define DEVPROP_TYPE_EMPTY                      0x00000000
#define DEVPROP_TYPE_NULL                       0x00000001
#define DEVPROP_TYPE_SBYTE                      0x00000002
#define DEVPROP_TYPE_BYTE                       0x00000003
#define DEVPROP_TYPE_INT16                      0x00000004
#define DEVPROP_TYPE_UINT16                     0x00000005
#define DEVPROP_TYPE_INT32                      0x00000006
#define DEVPROP_TYPE_UINT32                     0x00000007
#define DEVPROP_TYPE_INT64                      0x00000008
#define DEVPROP_TYPE_UINT64                     0x00000009
#define DEVPROP_TYPE_FLOAT                      0x0000000A
#define DEVPROP_TYPE_DOUBLE                     0x0000000B
#define DEVPROP_TYPE_DECIMAL                    0x0000000C
#define DEVPROP_TYPE_GUID                       0x0000000D
#define DEVPROP_TYPE_CURRENCY                   0x0000000E
#define DEVPROP_TYPE_DATE                       0x0000000F
#define DEVPROP_TYPE_FILETIME                   0x00000010
#define DEVPROP_TYPE_BOOLEAN                    0x00000011
#define DEVPROP_TYPE_STRING                     0x00000012
#define DEVPROP_TYPE_STRING_LIST (DEVPROP_TYPE_STRING|DEVPROP_TYPEMOD_LIST)
#define DEVPROP_TYPE_SECURITY_DESCRIPTOR        0x00000013
#define DEVPROP_TYPE_SECURITY_DESCRIPTOR_STRING 0x00000014
#define DEVPROP_TYPE_DEVPROPKEY                 0x00000015
#define DEVPROP_TYPE_DEVPROPTYPE                0x00000016
#define DEVPROP_TYPE_BINARY      (DEVPROP_TYPE_BYTE|DEVPROP_TYPEMOD_ARRAY)
#define DEVPROP_TYPE_ERROR                      0x00000017
#define DEVPROP_TYPE_NTSTATUS                   0x00000018
#define DEVPROP_TYPE_STRING_INDIRECT            0x00000019

#define MAX_DEVPROP_TYPE                        0x00000019
#define MAX_DEVPROP_TYPEMOD                     0x00002000

#define DEVPROP_MASK_TYPE                       0x00000FFF
#define DEVPROP_MASK_TYPEMOD                    0x0000F000

typedef CHAR DEVPROP_BOOLEAN, *PDEVPROP_BOOLEAN;

#define DEVPROP_TRUE ((DEVPROP_BOOLEAN)-1)
#define DEVPROP_FALSE ((DEVPROP_BOOLEAN) 0)

#ifndef DEVPROPKEY_DEFINED
#define DEVPROPKEY_DEFINED

typedef GUID DEVPROPGUID, *PDEVPROPGUID;
typedef ULONG DEVPROPID, *PDEVPROPID;

typedef struct _DEVPROPKEY {
  DEVPROPGUID fmtid;
  DEVPROPID pid;
} DEVPROPKEY, *PDEVPROPKEY;

#endif /* DEVPROPKEY_DEFINED */

#define DEVPROPID_FIRST_USABLE 2

#endif /* _DEVPROPDEF_H_ */

#ifdef DEFINE_DEVPROPKEY
#undef DEFINE_DEVPROPKEY
#endif
#ifdef INITGUID
#ifdef __cplusplus
#define DEFINE_DEVPROPKEY(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8, pid) EXTERN_C const DEVPROPKEY DECLSPEC_SELECTANY name = {{ l, w1, w2, {b1, b2, b3, b4, b5, b6, b7, b8}}, pid}
#else
#define DEFINE_DEVPROPKEY(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8, pid) const DEVPROPKEY DECLSPEC_SELECTANY name = {{ l, w1, w2, {b1, b2, b3, b4, b5, b6, b7, b8}}, pid}
#endif
#else
#define DEFINE_DEVPROPKEY(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8, pid) EXTERN_C const DEVPROPKEY name
#endif /* INITGUID */

#ifndef IsEqualDevPropKey

#ifdef __cplusplus
#define IsEqualDevPropKey(a, b) (((a).pid == (b).pid) && IsEqualIID((a).fmtid, (b).fmtid))
#else
#define IsEqualDevPropKey(a, b) (((a).pid == (b).pid) && IsEqualIID(&(a).fmtid, &(b).fmtid))
#endif

#endif /* !IsEqualDevPropKey */

#if !defined(PID_FIRST_USABLE)
#define PID_FIRST_USABLE 2
#endif

/* See the definitions of PROPERTYKEY in wtypes.h, and GUID in guiddef.h. "l" is short for "long", "w" for "word", "b" for "byte", and "pid" for "property identifier". */
#undef DEFINE_PROPERTYKEY
#if   defined(INITGUID) &&  defined(__cplusplus)
#define DEFINE_PROPERTYKEY(name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8,pid) EXTERN_C const PROPERTYKEY DECLSPEC_SELECTANY name = {{l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}},pid}
#elif defined(INITGUID) && !defined(__cplusplus)
#define DEFINE_PROPERTYKEY(name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8,pid)          const PROPERTYKEY DECLSPEC_SELECTANY name = {{l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}},pid}
#else
#define DEFINE_PROPERTYKEY(name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8,pid) EXTERN_C const PROPERTYKEY                    name
#endif



/* This implementation differs from the Windows SDK in order to correctly match the type of REFGUID used in `IsEqualIID()` (defined in guiddef.h) when __cplusplus is not defined. */
#if   !defined(IsEqualPropertyKey) &&  defined(__cplusplus)
#define IsEqualPropertyKey(a,b) (((a).pid == (b).pid) && IsEqualIID( (a).fmtid,  (b).fmtid))
#elif !defined(IsEqualPropertyKey) && !defined(__cplusplus)
#define IsEqualPropertyKey(a,b) (((a).pid == (b).pid) && IsEqualIID(&(a).fmtid, &(b).fmtid))
#endif



#if   !defined(REFPROPERTYKEY) &&  defined(__cplusplus)
#define REFPROPERTYKEY const PROPERTYKEY &
#elif !defined(REFPROPERTYKEY) && !defined(__cplusplus)
#define REFPROPERTYKEY const PROPERTYKEY * __MIDL_CONST
#endif

#if !defined(_PROPERTYKEY_EQUALITY_OPERATORS_)
#define _PROPERTYKEY_EQUALITY_OPERATORS_
#if defined(__cplusplus)
extern "C++"
{
    inline bool operator == (REFPROPERTYKEY k0, REFPROPERTYKEY k1) { return  IsEqualPropertyKey(k0, k1); }
    inline bool operator != (REFPROPERTYKEY k0, REFPROPERTYKEY k1) { return !IsEqualPropertyKey(k0, k1); }
}
#endif
#endif
#else // !__TINYC__
#include <devpropdef.h>
#include <propkeydef.h>
#endif


/* This part of the header mimics hidpi.h,
    but only what is used by HIDAPI */

typedef enum _HIDP_REPORT_TYPE
{
    HidP_Input,
    HidP_Output,
    HidP_Feature
} HIDP_REPORT_TYPE;

typedef struct _HIDP_PREPARSED_DATA * PHIDP_PREPARSED_DATA;

typedef struct _HIDP_CAPS
{
    USAGE    Usage;
    USAGE    UsagePage;
    USHORT   InputReportByteLength;
    USHORT   OutputReportByteLength;
    USHORT   FeatureReportByteLength;
    USHORT   Reserved[17];

    USHORT   NumberLinkCollectionNodes;

    USHORT   NumberInputButtonCaps;
    USHORT   NumberInputValueCaps;
    USHORT   NumberInputDataIndices;

    USHORT   NumberOutputButtonCaps;
    USHORT   NumberOutputValueCaps;
    USHORT   NumberOutputDataIndices;

    USHORT   NumberFeatureButtonCaps;
    USHORT   NumberFeatureValueCaps;
    USHORT   NumberFeatureDataIndices;
} HIDP_CAPS, *PHIDP_CAPS;

#define HIDP_STATUS_SUCCESS                0x00110000
#define HIDP_STATUS_INVALID_PREPARSED_DATA 0xc0110001

typedef NTSTATUS (__stdcall *HidP_GetCaps_)(PHIDP_PREPARSED_DATA preparsed_data, PHIDP_CAPS caps);

typedef DWORD RETURN_TYPE;
typedef RETURN_TYPE CONFIGRET;
typedef DWORD DEVNODE, DEVINST;
typedef DEVNODE* PDEVNODE, * PDEVINST;
typedef WCHAR* DEVNODEID_W, * DEVINSTID_W;

#define CR_SUCCESS (0x00000000)
#define CR_BUFFER_SMALL (0x0000001A)
#define CR_FAILURE (0x00000013)

#define CM_LOCATE_DEVNODE_NORMAL 0x00000000

#define CM_GET_DEVICE_INTERFACE_LIST_PRESENT (0x00000000)

typedef CONFIGRET(__stdcall* CM_Locate_DevNodeW_)(PDEVINST pdnDevInst, DEVINSTID_W pDeviceID, ULONG ulFlags);
typedef CONFIGRET(__stdcall* CM_Get_Parent_)(PDEVINST pdnDevInst, DEVINST dnDevInst, ULONG ulFlags);
typedef CONFIGRET(__stdcall* CM_Get_DevNode_PropertyW_)(DEVINST dnDevInst, CONST DEVPROPKEY* PropertyKey, DEVPROPTYPE* PropertyType, PBYTE PropertyBuffer, PULONG PropertyBufferSize, ULONG ulFlags);
typedef CONFIGRET(__stdcall* CM_Get_Device_Interface_PropertyW_)(LPCWSTR pszDeviceInterface, CONST DEVPROPKEY* PropertyKey, DEVPROPTYPE* PropertyType, PBYTE PropertyBuffer, PULONG PropertyBufferSize, ULONG ulFlags);
typedef CONFIGRET(__stdcall* CM_Get_Device_Interface_List_SizeW_)(PULONG pulLen, LPGUID InterfaceClassGuid, DEVINSTID_W pDeviceID, ULONG ulFlags);
typedef CONFIGRET(__stdcall* CM_Get_Device_Interface_ListW_)(LPGUID InterfaceClassGuid, DEVINSTID_W pDeviceID, PZZWSTR Buffer, ULONG BufferLen, ULONG ulFlags);

// from devpkey.h
DEFINE_DEVPROPKEY(DEVPKEY_NAME, 0xb725f130, 0x47ef, 0x101a, 0xa5, 0xf1, 0x02, 0x60, 0x8c, 0x9e, 0xeb, 0xac, 10); // DEVPROP_TYPE_STRING
DEFINE_DEVPROPKEY(DEVPKEY_Device_Manufacturer, 0xa45c254e, 0xdf1c, 0x4efd, 0x80, 0x20, 0x67, 0xd1, 0x46, 0xa8, 0x50, 0xe0, 13); // DEVPROP_TYPE_STRING
DEFINE_DEVPROPKEY(DEVPKEY_Device_InstanceId, 0x78c34fc8, 0x104a, 0x4aca, 0x9e, 0xa4, 0x52, 0x4d, 0x52, 0x99, 0x6e, 0x57, 256); // DEVPROP_TYPE_STRING
DEFINE_DEVPROPKEY(DEVPKEY_Device_HardwareIds, 0xa45c254e, 0xdf1c, 0x4efd, 0x80, 0x20, 0x67, 0xd1, 0x46, 0xa8, 0x50, 0xe0, 3); // DEVPROP_TYPE_STRING_LIST
DEFINE_DEVPROPKEY(DEVPKEY_Device_CompatibleIds, 0xa45c254e, 0xdf1c, 0x4efd, 0x80, 0x20, 0x67, 0xd1, 0x46, 0xa8, 0x50, 0xe0, 4); // DEVPROP_TYPE_STRING_LIST
DEFINE_DEVPROPKEY(DEVPKEY_Device_ContainerId, 0x8c7ed206, 0x3f8a, 0x4827, 0xb3, 0xab, 0xae, 0x9e, 0x1f, 0xae, 0xfc, 0x6c, 2); // DEVPROP_TYPE_GUID

// from propkey.h
DEFINE_PROPERTYKEY(PKEY_DeviceInterface_Bluetooth_DeviceAddress, 0x2BD67D8B, 0x8BEB, 0x48D5, 0x87, 0xE0, 0x6C, 0xDA, 0x34, 0x28, 0x04, 0x0A, 1); // DEVPROP_TYPE_STRING
DEFINE_PROPERTYKEY(PKEY_DeviceInterface_Bluetooth_Manufacturer, 0x2BD67D8B, 0x8BEB, 0x48D5, 0x87, 0xE0, 0x6C, 0xDA, 0x34, 0x28, 0x04, 0x0A, 4); // DEVPROP_TYPE_STRING
DEFINE_PROPERTYKEY(PKEY_DeviceInterface_Bluetooth_ModelNumber, 0x2BD67D8B, 0x8BEB, 0x48D5, 0x87, 0xE0, 0x6C, 0xDA, 0x34, 0x28, 0x04, 0x0A, 5); // DEVPROP_TYPE_STRING


#define HID_OUT_CTL_CODE(id) CTL_CODE(FILE_DEVICE_KEYBOARD, (id), METHOD_OUT_DIRECT, FILE_ANY_ACCESS)
#define IOCTL_HID_GET_FEATURE HID_OUT_CTL_CODE(100)
#define IOCTL_HID_GET_INPUT_REPORT HID_OUT_CTL_CODE(104)

#include "hidapi.h"

typedef struct _HIDD_ATTRIBUTES{
	ULONG Size;
	USHORT VendorID;
	USHORT ProductID;
	USHORT VersionNumber;
} HIDD_ATTRIBUTES, *PHIDD_ATTRIBUTES;

typedef void (__stdcall *HidD_GetHidGuid_)(LPGUID hid_guid);
typedef BOOLEAN (__stdcall *HidD_GetAttributes_)(HANDLE device, PHIDD_ATTRIBUTES attrib);
typedef BOOLEAN (__stdcall *HidD_GetSerialNumberString_)(HANDLE device, PVOID buffer, ULONG buffer_len);
typedef BOOLEAN (__stdcall *HidD_GetManufacturerString_)(HANDLE handle, PVOID buffer, ULONG buffer_len);
typedef BOOLEAN (__stdcall *HidD_GetProductString_)(HANDLE handle, PVOID buffer, ULONG buffer_len);
typedef BOOLEAN (__stdcall *HidD_SetFeature_)(HANDLE handle, PVOID data, ULONG length);
typedef BOOLEAN (__stdcall *HidD_GetFeature_)(HANDLE handle, PVOID data, ULONG length);
typedef BOOLEAN (__stdcall* HidD_SetOutputReport_)(HANDLE handle, PVOID data, ULONG length); 
typedef BOOLEAN (__stdcall *HidD_GetInputReport_)(HANDLE handle, PVOID data, ULONG length);
typedef BOOLEAN (__stdcall *HidD_GetIndexedString_)(HANDLE handle, ULONG string_index, PVOID buffer, ULONG buffer_len);
typedef BOOLEAN (__stdcall *HidD_GetPreparsedData_)(HANDLE handle, PHIDP_PREPARSED_DATA *preparsed_data);
typedef BOOLEAN (__stdcall *HidD_FreePreparsedData_)(PHIDP_PREPARSED_DATA preparsed_data);
typedef BOOLEAN (__stdcall *HidD_SetNumInputBuffers_)(HANDLE handle, ULONG number_buffers);

#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef MIN
#undef MIN
#endif
#define MIN(x,y) ((x) < (y)? (x): (y))

/* MAXIMUM_USB_STRING_LENGTH from usbspec.h is 255 */
/* BLUETOOTH_DEVICE_NAME_SIZE from bluetoothapis.h is 256 */
#define MAX_STRING_WCHARS 256

/* For certain USB devices, using a buffer larger or equal to 127 wchars results
   in successful completion of HID API functions, but a broken string is stored
   in the output buffer. This behaviour persists even if HID API is bypassed and
   HID IOCTLs are passed to the HID driver directly. Therefore, for USB devices,
   the buffer MUST NOT exceed 126 WCHARs.
*/

#define MAX_STRING_WCHARS_USB 126

static struct hid_api_version api_version = {
	.major = HID_API_VERSION_MAJOR,
	.minor = HID_API_VERSION_MINOR,
	.patch = HID_API_VERSION_PATCH
};

#ifndef HIDAPI_USE_DDK
/* Since we're not building with the DDK, and the HID header
   files aren't part of the Windows SDK, we define what we need ourselves.
   In lookup_functions(), the function pointers
   defined below are set. */

static HidD_GetHidGuid_ HidD_GetHidGuid;
static HidD_GetAttributes_ HidD_GetAttributes;
static HidD_GetSerialNumberString_ HidD_GetSerialNumberString;
static HidD_GetManufacturerString_ HidD_GetManufacturerString;
static HidD_GetProductString_ HidD_GetProductString;
static HidD_SetFeature_ HidD_SetFeature;
static HidD_GetFeature_ HidD_GetFeature;
static HidD_SetOutputReport_ HidD_SetOutputReport; 
static HidD_GetInputReport_ HidD_GetInputReport;
static HidD_GetIndexedString_ HidD_GetIndexedString;
static HidD_GetPreparsedData_ HidD_GetPreparsedData;
static HidD_FreePreparsedData_ HidD_FreePreparsedData;
static HidP_GetCaps_ HidP_GetCaps;
static HidD_SetNumInputBuffers_ HidD_SetNumInputBuffers;

static CM_Locate_DevNodeW_ CM_Locate_DevNodeW = NULL;
static CM_Get_Parent_ CM_Get_Parent = NULL;
static CM_Get_DevNode_PropertyW_ CM_Get_DevNode_PropertyW = NULL;
static CM_Get_Device_Interface_PropertyW_ CM_Get_Device_Interface_PropertyW = NULL;
static CM_Get_Device_Interface_List_SizeW_ CM_Get_Device_Interface_List_SizeW = NULL;
static CM_Get_Device_Interface_ListW_ CM_Get_Device_Interface_ListW = NULL;

static HMODULE hid_lib_handle = NULL;
static HMODULE cfgmgr32_lib_handle = NULL;
static BOOLEAN hidapi_initialized = FALSE;

static void free_library_handles()
{
	if (hid_lib_handle)
		FreeLibrary(hid_lib_handle);
	hid_lib_handle = NULL;
	if (cfgmgr32_lib_handle)
		FreeLibrary(cfgmgr32_lib_handle);
	cfgmgr32_lib_handle = NULL;
}

static int lookup_functions()
{
	hid_lib_handle = LoadLibraryW(L"hid.dll");
	if (hid_lib_handle == NULL) {
		goto err;
	}

	cfgmgr32_lib_handle = LoadLibraryW(L"cfgmgr32.dll");
	if (cfgmgr32_lib_handle == NULL) {
		goto err;
	}

#if defined(__GNUC__)
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wcast-function-type"
#endif
#define RESOLVE(lib_handle, x) x = (x##_)GetProcAddress(lib_handle, #x); if (!x) goto err;

	RESOLVE(hid_lib_handle, HidD_GetHidGuid);
	RESOLVE(hid_lib_handle, HidD_GetAttributes);
	RESOLVE(hid_lib_handle, HidD_GetSerialNumberString);
	RESOLVE(hid_lib_handle, HidD_GetManufacturerString);
	RESOLVE(hid_lib_handle, HidD_GetProductString);
	RESOLVE(hid_lib_handle, HidD_SetFeature);
	RESOLVE(hid_lib_handle, HidD_GetFeature);
	RESOLVE(hid_lib_handle, HidD_SetOutputReport);
	RESOLVE(hid_lib_handle, HidD_GetInputReport);
	RESOLVE(hid_lib_handle, HidD_GetIndexedString);
	RESOLVE(hid_lib_handle, HidD_GetPreparsedData);
	RESOLVE(hid_lib_handle, HidD_FreePreparsedData);
	RESOLVE(hid_lib_handle, HidP_GetCaps);
	RESOLVE(hid_lib_handle, HidD_SetNumInputBuffers);

	RESOLVE(cfgmgr32_lib_handle, CM_Locate_DevNodeW);
	RESOLVE(cfgmgr32_lib_handle, CM_Get_Parent);
	RESOLVE(cfgmgr32_lib_handle, CM_Get_DevNode_PropertyW);
	RESOLVE(cfgmgr32_lib_handle, CM_Get_Device_Interface_PropertyW);
	RESOLVE(cfgmgr32_lib_handle, CM_Get_Device_Interface_List_SizeW);
	RESOLVE(cfgmgr32_lib_handle, CM_Get_Device_Interface_ListW);

#undef RESOLVE
#if defined(__GNUC__)
# pragma GCC diagnostic pop
#endif

	return 0;

err:
	free_library_handles();
	return -1;
}

#endif /* HIDAPI_USE_DDK */

struct hid_device_ {
		HANDLE device_handle;
		BOOL blocking;
		USHORT output_report_length;
		unsigned char *write_buf;
		size_t input_report_length;
		USHORT feature_report_length;
		unsigned char *feature_buf;
		wchar_t *last_error_str;
		BOOL read_pending;
		char *read_buf;
		OVERLAPPED ol;
		OVERLAPPED write_ol;
		struct hid_device_info* device_info;
		DWORD write_timeout_ms;
};

static hid_device *new_hid_device()
{
	hid_device *dev = (hid_device*) calloc(1, sizeof(hid_device));

	if (dev == NULL) {
		return NULL;
	}

	dev->device_handle = INVALID_HANDLE_VALUE;
	dev->blocking = TRUE;
	dev->output_report_length = 0;
	dev->write_buf = NULL;
	dev->input_report_length = 0;
	dev->feature_report_length = 0;
	dev->feature_buf = NULL;
	dev->last_error_str = NULL;
	dev->read_pending = FALSE;
	dev->read_buf = NULL;
	memset(&dev->ol, 0, sizeof(dev->ol));
	dev->ol.hEvent = CreateEvent(NULL, FALSE, FALSE /*initial state f=nonsignaled*/, NULL);
	memset(&dev->write_ol, 0, sizeof(dev->write_ol));
	dev->write_ol.hEvent = CreateEvent(NULL, FALSE, FALSE /*initial state f=nonsignaled*/, NULL);
	dev->device_info = NULL;
	dev->write_timeout_ms = 1000;

	return dev;
}

static void free_hid_device(hid_device *dev)
{
	CloseHandle(dev->ol.hEvent);
	CloseHandle(dev->write_ol.hEvent);
	CloseHandle(dev->device_handle);
	free(dev->last_error_str);
	dev->last_error_str = NULL;
	free(dev->write_buf);
	free(dev->feature_buf);
	free(dev->read_buf);
	hid_free_enumeration(dev->device_info);
	free(dev);
}

static void register_winapi_error_to_buffer(wchar_t **error_buffer, const WCHAR *op)
{
	free(*error_buffer);
	*error_buffer = NULL;

	/* Only clear out error messages if NULL is passed into op */
	if (!op) {
		return;
	}

	WCHAR system_err_buf[1024];
	DWORD error_code = GetLastError();

	DWORD system_err_len = FormatMessageW(
		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		error_code,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		system_err_buf, ARRAYSIZE(system_err_buf),
		NULL);

	DWORD op_len = (DWORD)wcslen(op);

	DWORD op_prefix_len =
		op_len
		+ 15 /*: (0x00000000) */
		;
	DWORD msg_len =
		+ op_prefix_len
		+ system_err_len
		;

	*error_buffer = (WCHAR *)calloc(msg_len + 1, sizeof (WCHAR));
	WCHAR *msg = *error_buffer;

	if (!msg)
		return;

	int printf_written = swprintf(msg, (size_t)(msg_len + 1), L"%.*ls: (0x%08X) %.*ls", (int)op_len, op, error_code, (int)system_err_len, system_err_buf);

	if (printf_written < 0)
	{
		/* Highly unlikely */
		msg[0] = L'\0';
		return;
	}

	/* Get rid of the CR and LF that FormatMessage() sticks at the
	   end of the message. Thanks Microsoft! */
	while(msg[msg_len-1] == L'\r' || msg[msg_len-1] == L'\n' || msg[msg_len-1] == L' ')
	{
		msg[msg_len-1] = L'\0';
		msg_len--;
	}
}

#if defined(__GNUC__)
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Warray-bounds"
#endif
/* A bug in GCC/mingw gives:
 * error: array subscript 0 is outside array bounds of 'wchar_t *[0]' {aka 'short unsigned int *[]'} [-Werror=array-bounds]
 * |         free(*error_buffer);
 * Which doesn't make sense in this context. */

static void register_string_error_to_buffer(wchar_t **error_buffer, const WCHAR *string_error)
{
	free(*error_buffer);
	*error_buffer = NULL;

	if (string_error) {
		*error_buffer = _wcsdup(string_error);
	}
}

#if defined(__GNUC__)
# pragma GCC diagnostic pop
#endif

static void register_winapi_error(hid_device *dev, const WCHAR *op)
{
	register_winapi_error_to_buffer(&dev->last_error_str, op);
}

static void register_string_error(hid_device *dev, const WCHAR *string_error)
{
	register_string_error_to_buffer(&dev->last_error_str, string_error);
}

static wchar_t *last_global_error_str = NULL;

static void register_global_winapi_error(const WCHAR *op)
{
	register_winapi_error_to_buffer(&last_global_error_str, op);
}

static void register_global_error(const WCHAR *string_error)
{
	register_string_error_to_buffer(&last_global_error_str, string_error);
}

static HANDLE open_device(const wchar_t *path, BOOL open_rw)
{
	HANDLE handle;
	DWORD desired_access = (open_rw)? (GENERIC_WRITE | GENERIC_READ): 0;
	DWORD share_mode = FILE_SHARE_READ|FILE_SHARE_WRITE;

	handle = CreateFileW(path,
		desired_access,
		share_mode,
		NULL,
		OPEN_EXISTING,
		FILE_FLAG_OVERLAPPED,/*FILE_ATTRIBUTE_NORMAL,*/
		0);

	return handle;
}

HID_API_EXPORT const struct hid_api_version* HID_API_CALL hid_version(void)
{
	return &api_version;
}

HID_API_EXPORT const char* HID_API_CALL hid_version_str(void)
{
	return HID_API_VERSION_STR;
}

int HID_API_EXPORT hid_init(void)
{
	register_global_error(NULL);
#ifndef HIDAPI_USE_DDK
	if (!hidapi_initialized) {
		if (lookup_functions() < 0) {
			register_global_winapi_error(L"resolve DLL functions");
			return -1;
		}
		hidapi_initialized = TRUE;
	}
#endif
	return 0;
}

int HID_API_EXPORT hid_exit(void)
{
#ifndef HIDAPI_USE_DDK
	free_library_handles();
	hidapi_initialized = FALSE;
#endif
	register_global_error(NULL);
	return 0;
}

static void* hid_internal_get_devnode_property(DEVINST dev_node, const DEVPROPKEY* property_key, DEVPROPTYPE expected_property_type)
{
	ULONG len = 0;
	CONFIGRET cr;
	DEVPROPTYPE property_type;
	PBYTE property_value = NULL;

	cr = CM_Get_DevNode_PropertyW(dev_node, property_key, &property_type, NULL, &len, 0);
	if (cr != CR_BUFFER_SMALL || property_type != expected_property_type)
		return NULL;

	property_value = (PBYTE)calloc(len, sizeof(BYTE));
	cr = CM_Get_DevNode_PropertyW(dev_node, property_key, &property_type, property_value, &len, 0);
	if (cr != CR_SUCCESS) {
		free(property_value);
		return NULL;
	}

	return property_value;
}

static void* hid_internal_get_device_interface_property(const wchar_t* interface_path, const DEVPROPKEY* property_key, DEVPROPTYPE expected_property_type)
{
	ULONG len = 0;
	CONFIGRET cr;
	DEVPROPTYPE property_type;
	PBYTE property_value = NULL;

	cr = CM_Get_Device_Interface_PropertyW(interface_path, property_key, &property_type, NULL, &len, 0);
	if (cr != CR_BUFFER_SMALL || property_type != expected_property_type)
		return NULL;

	property_value = (PBYTE)calloc(len, sizeof(BYTE));
	cr = CM_Get_Device_Interface_PropertyW(interface_path, property_key, &property_type, property_value, &len, 0);
	if (cr != CR_SUCCESS) {
		free(property_value);
		return NULL;
	}

	return property_value;
}

static void hid_internal_towupper(wchar_t* string)
{
	for (wchar_t* p = string; *p; ++p) *p = towupper(*p);
}

static int hid_internal_extract_int_token_value(wchar_t* string, const wchar_t* token)
{
	int token_value;
	wchar_t* startptr, * endptr;

	startptr = wcsstr(string, token);
	if (!startptr)
		return -1;

	startptr += wcslen(token);
	token_value = wcstol(startptr, &endptr, 16);
	if (endptr == startptr)
		return -1;

	return token_value;
}

static void hid_internal_get_usb_info(struct hid_device_info* dev, DEVINST dev_node)
{
	wchar_t *device_id = NULL, *hardware_ids = NULL;

	device_id = hid_internal_get_devnode_property(dev_node, &DEVPKEY_Device_InstanceId, DEVPROP_TYPE_STRING);
	if (!device_id)
		goto end;

	/* Normalize to upper case */
	hid_internal_towupper(device_id);

	/* Check for Xbox Common Controller class (XUSB) device.
	   https://docs.microsoft.com/windows/win32/xinput/directinput-and-xusb-devices
	   https://docs.microsoft.com/windows/win32/xinput/xinput-and-directinput
	*/
	if (hid_internal_extract_int_token_value(device_id, L"IG_") != -1) {
		/* Get devnode parent to reach out USB device. */
		if (CM_Get_Parent(&dev_node, dev_node, 0) != CR_SUCCESS)
			goto end;
	}

	/* Get the hardware ids from devnode */
	hardware_ids = hid_internal_get_devnode_property(dev_node, &DEVPKEY_Device_HardwareIds, DEVPROP_TYPE_STRING_LIST);
	if (!hardware_ids)
		goto end;

	/* Get additional information from USB device's Hardware ID
	   https://docs.microsoft.com/windows-hardware/drivers/install/standard-usb-identifiers
	   https://docs.microsoft.com/windows-hardware/drivers/usbcon/enumeration-of-interfaces-not-grouped-in-collections
	*/
	for (wchar_t* hardware_id = hardware_ids; *hardware_id; hardware_id += wcslen(hardware_id) + 1) {
		/* Normalize to upper case */
		hid_internal_towupper(hardware_id);

		if (dev->release_number == 0) {
			/* USB_DEVICE_DESCRIPTOR.bcdDevice value. */
			int release_number = hid_internal_extract_int_token_value(hardware_id, L"REV_");
			if (release_number != -1) {
				dev->release_number = (unsigned short)release_number;
			}
		}

		if (dev->interface_number == -1) {
			/* USB_INTERFACE_DESCRIPTOR.bInterfaceNumber value. */
			int interface_number = hid_internal_extract_int_token_value(hardware_id, L"MI_");
			if (interface_number != -1) {
				dev->interface_number = interface_number;
			}
		}
	}

	/* Try to get USB device manufacturer string if not provided by HidD_GetManufacturerString. */
	if (wcslen(dev->manufacturer_string) == 0) {
		wchar_t* manufacturer_string = hid_internal_get_devnode_property(dev_node, &DEVPKEY_Device_Manufacturer, DEVPROP_TYPE_STRING);
		if (manufacturer_string) {
			free(dev->manufacturer_string);
			dev->manufacturer_string = manufacturer_string;
		}
	}

	/* Try to get USB device serial number if not provided by HidD_GetSerialNumberString. */
	if (wcslen(dev->serial_number) == 0) {
		DEVINST usb_dev_node = dev_node;
		if (dev->interface_number != -1) {
			/* Get devnode parent to reach out composite parent USB device.
			   https://docs.microsoft.com/windows-hardware/drivers/usbcon/enumeration-of-the-composite-parent-device
			*/
			if (CM_Get_Parent(&usb_dev_node, dev_node, 0) != CR_SUCCESS)
				goto end;
		}

		/* Get the device id of the USB device. */
		free(device_id);
		device_id = hid_internal_get_devnode_property(usb_dev_node, &DEVPKEY_Device_InstanceId, DEVPROP_TYPE_STRING);
		if (!device_id)
			goto end;

		/* Extract substring after last '\\' of Instance ID.
		   For USB devices it may contain device's serial number.
		   https://docs.microsoft.com/windows-hardware/drivers/install/instance-ids
		*/
		for (wchar_t *ptr = device_id + wcslen(device_id); ptr > device_id; --ptr) {
			/* Instance ID is unique only within the scope of the bus.
			   For USB devices it means that serial number is not available. Skip. */
			if (*ptr == L'&')
				break;

			if (*ptr == L'\\') {
				free(dev->serial_number);
				dev->serial_number = _wcsdup(ptr + 1);
				break;
			}
		}
	}

	/* If we can't get the interface number, it means that there is only one interface. */
	if (dev->interface_number == -1)
		dev->interface_number = 0;

end:
	free(device_id);
	free(hardware_ids);
}

/* HidD_GetProductString/HidD_GetManufacturerString/HidD_GetSerialNumberString is not working for BLE HID devices
   Request this info via dev node properties instead.
   https://docs.microsoft.com/answers/questions/401236/hidd-getproductstring-with-ble-hid-device.html
*/
static void hid_internal_get_ble_info(struct hid_device_info* dev, DEVINST dev_node)
{
	if (wcslen(dev->manufacturer_string) == 0) {
		/* Manufacturer Name String (UUID: 0x2A29) */
		wchar_t* manufacturer_string = hid_internal_get_devnode_property(dev_node, (const DEVPROPKEY*)&PKEY_DeviceInterface_Bluetooth_Manufacturer, DEVPROP_TYPE_STRING);
		if (manufacturer_string) {
			free(dev->manufacturer_string);
			dev->manufacturer_string = manufacturer_string;
		}
	}

	if (wcslen(dev->serial_number) == 0) {
		/* Serial Number String (UUID: 0x2A25) */
		wchar_t* serial_number = hid_internal_get_devnode_property(dev_node, (const DEVPROPKEY*)&PKEY_DeviceInterface_Bluetooth_DeviceAddress, DEVPROP_TYPE_STRING);
		if (serial_number) {
			free(dev->serial_number);
			dev->serial_number = serial_number;
		}
	}

	if (wcslen(dev->product_string) == 0) {
		/* Model Number String (UUID: 0x2A24) */
		wchar_t* product_string = hid_internal_get_devnode_property(dev_node, (const DEVPROPKEY*)&PKEY_DeviceInterface_Bluetooth_ModelNumber, DEVPROP_TYPE_STRING);
		if (!product_string) {
			DEVINST parent_dev_node = 0;
			/* Fallback: Get devnode grandparent to reach out Bluetooth LE device node */
			if (CM_Get_Parent(&parent_dev_node, dev_node, 0) == CR_SUCCESS) {
				/* Device Name (UUID: 0x2A00) */
				product_string = hid_internal_get_devnode_property(parent_dev_node, &DEVPKEY_NAME, DEVPROP_TYPE_STRING);
			}
		}

		if (product_string) {
			free(dev->product_string);
			dev->product_string = product_string;
		}
	}
}

/* Unfortunately, HID_API_BUS_xxx constants alone aren't enough to distinguish between BLUETOOTH and BLE */

#define HID_API_BUS_FLAG_BLE 0x01

typedef struct hid_internal_detect_bus_type_result_ {
	DEVINST dev_node;
	hid_bus_type bus_type;
	unsigned int bus_flags;
} hid_internal_detect_bus_type_result;

static hid_internal_detect_bus_type_result hid_internal_detect_bus_type(const wchar_t* interface_path)
{
	wchar_t *device_id = NULL, *compatible_ids = NULL;
	CONFIGRET cr;
	DEVINST dev_node;
	hid_internal_detect_bus_type_result result = { 0 };

	/* Get the device id from interface path */
	device_id = hid_internal_get_device_interface_property(interface_path, &DEVPKEY_Device_InstanceId, DEVPROP_TYPE_STRING);
	if (!device_id)
		goto end;

	/* Open devnode from device id */
	cr = CM_Locate_DevNodeW(&dev_node, (DEVINSTID_W)device_id, CM_LOCATE_DEVNODE_NORMAL);
	if (cr != CR_SUCCESS)
		goto end;

	/* Get devnode parent */
	cr = CM_Get_Parent(&dev_node, dev_node, 0);
	if (cr != CR_SUCCESS)
		goto end;

	/* Get the compatible ids from parent devnode */
	compatible_ids = hid_internal_get_devnode_property(dev_node, &DEVPKEY_Device_CompatibleIds, DEVPROP_TYPE_STRING_LIST);
	if (!compatible_ids)
		goto end;

	/* Now we can parse parent's compatible IDs to find out the device bus type */
	for (wchar_t* compatible_id = compatible_ids; *compatible_id; compatible_id += wcslen(compatible_id) + 1) {
		/* Normalize to upper case */
		hid_internal_towupper(compatible_id);

		/* USB devices
		   https://docs.microsoft.com/windows-hardware/drivers/hid/plug-and-play-support
		   https://docs.microsoft.com/windows-hardware/drivers/install/standard-usb-identifiers */
		if (wcsstr(compatible_id, L"USB") != NULL) {
			result.bus_type = HID_API_BUS_USB;
			break;
		}

		/* Bluetooth devices
		   https://docs.microsoft.com/windows-hardware/drivers/bluetooth/installing-a-bluetooth-device */
		if (wcsstr(compatible_id, L"BTHENUM") != NULL) {
			result.bus_type = HID_API_BUS_BLUETOOTH;
			break;
		}

		/* Bluetooth LE devices */
		if (wcsstr(compatible_id, L"BTHLEDEVICE") != NULL) {
			result.bus_type = HID_API_BUS_BLUETOOTH;
			result.bus_flags |= HID_API_BUS_FLAG_BLE;
			break;
		}

		/* I2C devices
		   https://docs.microsoft.com/windows-hardware/drivers/hid/plug-and-play-support-and-power-management */
		if (wcsstr(compatible_id, L"PNP0C50") != NULL) {
			result.bus_type = HID_API_BUS_I2C;
			break;
		}

		/* SPI devices
		   https://docs.microsoft.com/windows-hardware/drivers/hid/plug-and-play-for-spi */
		if (wcsstr(compatible_id, L"PNP0C51") != NULL) {
			result.bus_type = HID_API_BUS_SPI;
			break;
		}
	}

	result.dev_node = dev_node;

end:
	free(device_id);
	free(compatible_ids);
	return result;
}

static char *hid_internal_UTF16toUTF8(const wchar_t *src)
{
	char *dst = NULL;
	int len = WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, src, -1, NULL, 0, NULL, NULL);
	if (len) {
		dst = (char*)calloc(len, sizeof(char));
		if (dst == NULL) {
			return NULL;
		}
		WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, src, -1, dst, len, NULL, NULL);
	}

	return dst;
}

static wchar_t *hid_internal_UTF8toUTF16(const char *src)
{
	wchar_t *dst = NULL;
	int len = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, src, -1, NULL, 0);
	if (len) {
		dst = (wchar_t*)calloc(len, sizeof(wchar_t));
		if (dst == NULL) {
			return NULL;
		}
		MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, src, -1, dst, len);
	}

	return dst;
}

static struct hid_device_info *hid_internal_get_device_info(const wchar_t *path, HANDLE handle)
{
	struct hid_device_info *dev = NULL; /* return object */
	HIDD_ATTRIBUTES attrib;
	PHIDP_PREPARSED_DATA pp_data = NULL;
	HIDP_CAPS caps;
	wchar_t string[MAX_STRING_WCHARS + 1];
	ULONG len;
	ULONG size;
	hid_internal_detect_bus_type_result detect_bus_type_result;

	/* Create the record. */
	dev = (struct hid_device_info*)calloc(1, sizeof(struct hid_device_info));

	if (dev == NULL) {
		return NULL;
	}

	/* Fill out the record */
	dev->next = NULL;
	dev->path = hid_internal_UTF16toUTF8(path);
	dev->interface_number = -1;

	attrib.Size = sizeof(HIDD_ATTRIBUTES);
	if (HidD_GetAttributes(handle, &attrib)) {
		/* VID/PID */
		dev->vendor_id = attrib.VendorID;
		dev->product_id = attrib.ProductID;

		/* Release Number */
		dev->release_number = attrib.VersionNumber;
	}

	/* Get the Usage Page and Usage for this device. */
	if (HidD_GetPreparsedData(handle, &pp_data)) {
		if (HidP_GetCaps(pp_data, &caps) == HIDP_STATUS_SUCCESS) {
			dev->usage_page = caps.UsagePage;
			dev->usage = caps.Usage;
		}

		HidD_FreePreparsedData(pp_data);
	}

	/* detect bus type before reading string descriptors */
	detect_bus_type_result = hid_internal_detect_bus_type(path);
	dev->bus_type = detect_bus_type_result.bus_type;

	len = dev->bus_type == HID_API_BUS_USB ? MAX_STRING_WCHARS_USB : MAX_STRING_WCHARS;
	string[len] = L'\0';
	size = len * sizeof(wchar_t);

	/* Serial Number */
	string[0] = L'\0';
	HidD_GetSerialNumberString(handle, string, size);
	dev->serial_number = _wcsdup(string);

	/* Manufacturer String */
	string[0] = L'\0';
	HidD_GetManufacturerString(handle, string, size);
	dev->manufacturer_string = _wcsdup(string);

	/* Product String */
	string[0] = L'\0';
	HidD_GetProductString(handle, string, size);
	dev->product_string = _wcsdup(string);

	/* now, the portion that depends on string descriptors */
	switch (dev->bus_type) {
	case HID_API_BUS_USB:
		hid_internal_get_usb_info(dev, detect_bus_type_result.dev_node);
		break;

	case HID_API_BUS_BLUETOOTH:
		if (detect_bus_type_result.bus_flags & HID_API_BUS_FLAG_BLE)
			hid_internal_get_ble_info(dev, detect_bus_type_result.dev_node);
		break;

	case HID_API_BUS_UNKNOWN:
	case HID_API_BUS_SPI:
	case HID_API_BUS_I2C:
		/* shut down -Wswitch */
		break;
	}

	return dev;
}

struct hid_device_info HID_API_EXPORT * HID_API_CALL hid_enumerate(unsigned short vendor_id, unsigned short product_id)
{
	struct hid_device_info *root = NULL; /* return object */
	struct hid_device_info *cur_dev = NULL;
	GUID interface_class_guid;
	CONFIGRET cr;
	wchar_t* device_interface_list = NULL;
	DWORD len;

	if (hid_init() < 0) {
		/* register_global_error: global error is reset by hid_init */
		return NULL;
	}

	/* Retrieve HID Interface Class GUID
	   https://docs.microsoft.com/windows-hardware/drivers/install/guid-devinterface-hid */
	HidD_GetHidGuid(&interface_class_guid);

	/* Get the list of all device interfaces belonging to the HID class. */
	/* Retry in case of list was changed between calls to
	  CM_Get_Device_Interface_List_SizeW and CM_Get_Device_Interface_ListW */
	do {
		cr = CM_Get_Device_Interface_List_SizeW(&len, &interface_class_guid, NULL, CM_GET_DEVICE_INTERFACE_LIST_PRESENT);
		if (cr != CR_SUCCESS) {
			register_global_error(L"Failed to get size of HID device interface list");
			break;
		}

		if (device_interface_list != NULL) {
			free(device_interface_list);
		}

		device_interface_list = (wchar_t*)calloc(len, sizeof(wchar_t));
		if (device_interface_list == NULL) {
			register_global_error(L"Failed to allocate memory for HID device interface list");
			return NULL;
		}
		cr = CM_Get_Device_Interface_ListW(&interface_class_guid, NULL, device_interface_list, len, CM_GET_DEVICE_INTERFACE_LIST_PRESENT);
		if (cr != CR_SUCCESS && cr != CR_BUFFER_SMALL) {
			register_global_error(L"Failed to get HID device interface list");
		}
	} while (cr == CR_BUFFER_SMALL);

	if (cr != CR_SUCCESS) {
		goto end_of_function;
	}

	/* Iterate over each device interface in the HID class, looking for the right one. */
	for (wchar_t* device_interface = device_interface_list; *device_interface; device_interface += wcslen(device_interface) + 1) {
		HANDLE device_handle = INVALID_HANDLE_VALUE;
		HIDD_ATTRIBUTES attrib;

		/* Open read-only handle to the device */
		device_handle = open_device(device_interface, FALSE);

		/* Check validity of device_handle. */
		if (device_handle == INVALID_HANDLE_VALUE) {
			/* Unable to open the device. */
			continue;
		}

		/* Get the Vendor ID and Product ID for this device. */
		attrib.Size = sizeof(HIDD_ATTRIBUTES);
		if (!HidD_GetAttributes(device_handle, &attrib)) {
			goto cont_close;
		}

		/* Check the VID/PID to see if we should add this
		   device to the enumeration list. */
		if ((vendor_id == 0x0 || attrib.VendorID == vendor_id) &&
		    (product_id == 0x0 || attrib.ProductID == product_id)) {

			/* VID/PID match. Create the record. */
			struct hid_device_info *tmp = hid_internal_get_device_info(device_interface, device_handle);

			if (tmp == NULL) {
				goto cont_close;
			}

			if (cur_dev) {
				cur_dev->next = tmp;
			}
			else {
				root = tmp;
			}
			cur_dev = tmp;
		}

cont_close:
		CloseHandle(device_handle);
	}

	if (root == NULL) {
		if (vendor_id == 0 && product_id == 0) {
			register_global_error(L"No HID devices found in the system.");
		} else {
			register_global_error(L"No HID devices with requested VID/PID found in the system.");
		}
	}

end_of_function:
	free(device_interface_list);

	return root;
}

void  HID_API_EXPORT HID_API_CALL hid_free_enumeration(struct hid_device_info *devs)
{
	/* TODO: Merge this with the Linux version. This function is platform-independent. */
	struct hid_device_info *d = devs;
	while (d) {
		struct hid_device_info *next = d->next;
		free(d->path);
		free(d->serial_number);
		free(d->manufacturer_string);
		free(d->product_string);
		free(d);
		d = next;
	}
}

HID_API_EXPORT hid_device * HID_API_CALL hid_open(unsigned short vendor_id, unsigned short product_id, const wchar_t *serial_number)
{
	/* TODO: Merge this functions with the Linux version. This function should be platform independent. */
	struct hid_device_info *devs, *cur_dev;
	const char *path_to_open = NULL;
	hid_device *handle = NULL;

	/* register_global_error: global error is reset by hid_enumerate/hid_init */
	devs = hid_enumerate(vendor_id, product_id);
	if (!devs) {
		/* register_global_error: global error is already set by hid_enumerate */
		return NULL;
	}

	cur_dev = devs;
	while (cur_dev) {
		if (cur_dev->vendor_id == vendor_id &&
		    cur_dev->product_id == product_id) {
			if (serial_number) {
				if (cur_dev->serial_number && wcscmp(serial_number, cur_dev->serial_number) == 0) {
					path_to_open = cur_dev->path;
					break;
				}
			}
			else {
				path_to_open = cur_dev->path;
				break;
			}
		}
		cur_dev = cur_dev->next;
	}

	if (path_to_open) {
		/* Open the device */
		handle = hid_open_path(path_to_open);
	} else {
		register_global_error(L"Device with requested VID/PID/(SerialNumber) not found");
	}

	hid_free_enumeration(devs);

	return handle;
}

HID_API_EXPORT hid_device * HID_API_CALL hid_open_path(const char *path)
{
	hid_device *dev = NULL;
	wchar_t* interface_path = NULL;
	HANDLE device_handle = INVALID_HANDLE_VALUE;
	PHIDP_PREPARSED_DATA pp_data = NULL;
	HIDP_CAPS caps;

	if (hid_init() < 0) {
		/* register_global_error: global error is reset by hid_init */
		goto end_of_function;
	}

	interface_path = hid_internal_UTF8toUTF16(path);
	if (!interface_path) {
		register_global_error(L"Path conversion failure");
		goto end_of_function;
	}

	/* Open a handle to the device */
	device_handle = open_device(interface_path, TRUE);

	/* Check validity of write_handle. */
	if (device_handle == INVALID_HANDLE_VALUE) {
		/* System devices, such as keyboards and mice, cannot be opened in
		   read-write mode, because the system takes exclusive control over
		   them.  This is to prevent keyloggers.  However, feature reports
		   can still be sent and received.  Retry opening the device, but
		   without read/write access. */
		device_handle = open_device(interface_path, FALSE);

		/* Check the validity of the limited device_handle. */
		if (device_handle == INVALID_HANDLE_VALUE) {
			register_global_winapi_error(L"open_device");
			goto end_of_function;
		}
	}

	/* Set the Input Report buffer size to 64 reports. */
	if (!HidD_SetNumInputBuffers(device_handle, 64)) {
		register_global_winapi_error(L"set input buffers");
		goto end_of_function;
	}

	/* Get the Input Report length for the device. */
	if (!HidD_GetPreparsedData(device_handle, &pp_data)) {
		register_global_winapi_error(L"get preparsed data");
		goto end_of_function;
	}

	if (HidP_GetCaps(pp_data, &caps) != HIDP_STATUS_SUCCESS) {
		register_global_error(L"HidP_GetCaps");
		goto end_of_function;
	}

	dev = new_hid_device();

	if (dev == NULL) {
		register_global_error(L"hid_device allocation error");
		goto end_of_function;
	}

	dev->device_handle = device_handle;
	device_handle = INVALID_HANDLE_VALUE;

	dev->output_report_length = caps.OutputReportByteLength;
	dev->input_report_length = caps.InputReportByteLength;
	dev->feature_report_length = caps.FeatureReportByteLength;
	dev->read_buf = (char*) malloc(dev->input_report_length);
	dev->device_info = hid_internal_get_device_info(interface_path, dev->device_handle);

end_of_function:
	free(interface_path);

	if (device_handle != INVALID_HANDLE_VALUE) {
		CloseHandle(device_handle);
	}

	if (pp_data) {
		HidD_FreePreparsedData(pp_data);
	}

	return dev;
}

void HID_API_EXPORT_CALL hid_winapi_set_write_timeout(hid_device *dev, unsigned long timeout)
{
	dev->write_timeout_ms = timeout;
}

int HID_API_EXPORT HID_API_CALL hid_write(hid_device *dev, const unsigned char *data, size_t length)
{
	DWORD bytes_written = 0;
	int function_result = -1;
	BOOL res;
	BOOL overlapped = FALSE;

	unsigned char *buf;

	if (!data || !length) {
		register_string_error(dev, L"Zero buffer/length");
		return function_result;
	}

	register_string_error(dev, NULL);

	/* Make sure the right number of bytes are passed to WriteFile. Windows
	   expects the number of bytes which are in the _longest_ report (plus
	   one for the report number) bytes even if the data is a report
	   which is shorter than that. Windows gives us this value in
	   caps.OutputReportByteLength. If a user passes in fewer bytes than this,
	   use cached temporary buffer which is the proper size. */
	if (length >= dev->output_report_length) {
		/* The user passed the right number of bytes. Use the buffer as-is. */
		buf = (unsigned char *) data;
	} else {
		if (dev->write_buf == NULL) {
			dev->write_buf = (unsigned char *) malloc(dev->output_report_length);

			if (dev->write_buf == NULL) {
				register_string_error(dev, L"hid_write/malloc");
				goto end_of_function;
			}
		}

		buf = dev->write_buf;
		memcpy(buf, data, length);
		memset(buf + length, 0, dev->output_report_length - length);
		length = dev->output_report_length;
	}

	res = WriteFile(dev->device_handle, buf, (DWORD) length, &bytes_written, &dev->write_ol);

	if (!res) {
		if (GetLastError() != ERROR_IO_PENDING) {
			/* WriteFile() failed. Return error. */
			register_winapi_error(dev, L"WriteFile");
			goto end_of_function;
		}
		overlapped = TRUE;
	} else {
		/* WriteFile() succeeded synchronously. */
		function_result = bytes_written;
	}

	if (overlapped) {
		/* Wait for the transaction to complete. This makes
		   hid_write() synchronous. */
		res = WaitForSingleObject(dev->write_ol.hEvent, dev->write_timeout_ms);
		if (res != WAIT_OBJECT_0) {
			/* There was a Timeout. */
			register_winapi_error(dev, L"hid_write/WaitForSingleObject");
			goto end_of_function;
		}

		/* Get the result. */
		res = GetOverlappedResult(dev->device_handle, &dev->write_ol, &bytes_written, FALSE/*wait*/);
		if (res) {
			function_result = bytes_written;
		}
		else {
			/* The Write operation failed. */
			register_winapi_error(dev, L"hid_write/GetOverlappedResult");
			goto end_of_function;
		}
	}

end_of_function:
	return function_result;
}


int HID_API_EXPORT HID_API_CALL hid_read_timeout(hid_device *dev, unsigned char *data, size_t length, int milliseconds)
{
	DWORD bytes_read = 0;
	size_t copy_len = 0;
	BOOL res = FALSE;
	BOOL overlapped = FALSE;

	if (!data || !length) {
		register_string_error(dev, L"Zero buffer/length");
		return -1;
	}

	register_string_error(dev, NULL);

	/* Copy the handle for convenience. */
	HANDLE ev = dev->ol.hEvent;

	if (!dev->read_pending) {
		/* Start an Overlapped I/O read. */
		dev->read_pending = TRUE;
		memset(dev->read_buf, 0, dev->input_report_length);
		ResetEvent(ev);
		res = ReadFile(dev->device_handle, dev->read_buf, (DWORD) dev->input_report_length, &bytes_read, &dev->ol);

		if (!res) {
			if (GetLastError() != ERROR_IO_PENDING) {
				/* ReadFile() has failed.
				   Clean up and return error. */
				register_winapi_error(dev, L"ReadFile");
				CancelIo(dev->device_handle);
				dev->read_pending = FALSE;
				goto end_of_function;
			}
			overlapped = TRUE;
		}
	}
	else {
		overlapped = TRUE;
	}

	if (overlapped) {
		/* See if there is any data yet. */
		res = WaitForSingleObject(ev, milliseconds >= 0 ? (DWORD)milliseconds : INFINITE);
		if (res != WAIT_OBJECT_0) {
			/* There was no data this time. Return zero bytes available,
			   but leave the Overlapped I/O running. */
			return 0;
		}

		/* Get the number of bytes read. The actual data has been copied to the data[]
		   array which was passed to ReadFile(). We must not wait here because we've
		   already waited on our event above, and since it's auto-reset, it will have
		   been reset back to unsignalled by now. */
		res = GetOverlappedResult(dev->device_handle, &dev->ol, &bytes_read, FALSE/*don't wait now - already did on the prev step*/);
	}
	/* Set pending back to false, even if GetOverlappedResult() returned error. */
	dev->read_pending = FALSE;

	if (res && bytes_read > 0) {
		if (dev->read_buf[0] == 0x0) {
			/* If report numbers aren't being used, but Windows sticks a report
			   number (0x0) on the beginning of the report anyway. To make this
			   work like the other platforms, and to make it work more like the
			   HID spec, we'll skip over this byte. */
			bytes_read--;
			copy_len = length > bytes_read ? bytes_read : length;
			memcpy(data, dev->read_buf+1, copy_len);
		}
		else {
			/* Copy the whole buffer, report number and all. */
			copy_len = length > bytes_read ? bytes_read : length;
			memcpy(data, dev->read_buf, copy_len);
		}
	}
	if (!res) {
		register_winapi_error(dev, L"hid_read_timeout/GetOverlappedResult");
	}

end_of_function:
	if (!res) {
		return -1;
	}

	return (int) copy_len;
}

int HID_API_EXPORT HID_API_CALL hid_read(hid_device *dev, unsigned char *data, size_t length)
{
	return hid_read_timeout(dev, data, length, (dev->blocking)? -1: 0);
}

int HID_API_EXPORT HID_API_CALL hid_set_nonblocking(hid_device *dev, int nonblock)
{
	dev->blocking = !nonblock;
	return 0; /* Success */
}

int HID_API_EXPORT HID_API_CALL hid_send_feature_report(hid_device *dev, const unsigned char *data, size_t length)
{
	BOOL res = FALSE;
	unsigned char *buf;
	size_t length_to_send;

	if (!data || !length) {
		register_string_error(dev, L"Zero buffer/length");
		return -1;
	}

	register_string_error(dev, NULL);

	/* Windows expects at least caps.FeatureReportByteLength bytes passed
	   to HidD_SetFeature(), even if the report is shorter. Any less sent and
	   the function fails with error ERROR_INVALID_PARAMETER set. Any more
	   and HidD_SetFeature() silently truncates the data sent in the report
	   to caps.FeatureReportByteLength. */
	if (length >= dev->feature_report_length) {
		buf = (unsigned char *) data;
		length_to_send = length;
	} else {
		if (dev->feature_buf == NULL) {
			dev->feature_buf = (unsigned char *) malloc(dev->feature_report_length);

			if (dev->feature_buf == NULL) {
				register_string_error(dev, L"hid_send_feature_report/malloc");
				return -1;
			}
		}

		buf = dev->feature_buf;
		memcpy(buf, data, length);
		memset(buf + length, 0, dev->feature_report_length - length);
		length_to_send = dev->feature_report_length;
	}

	res = HidD_SetFeature(dev->device_handle, (PVOID)buf, (DWORD) length_to_send);

	if (!res) {
		register_winapi_error(dev, L"HidD_SetFeature");
		return -1;
	}

	return (int) length;
}

static int hid_get_report(hid_device *dev, DWORD report_type, unsigned char *data, size_t length)
{
	BOOL res;
	DWORD bytes_returned = 0;

	OVERLAPPED ol;
	memset(&ol, 0, sizeof(ol));

	if (!data || !length) {
		register_string_error(dev, L"Zero buffer/length");
		return -1;
	}

	register_string_error(dev, NULL);

	res = DeviceIoControl(dev->device_handle,
		report_type,
		data, (DWORD) length,
		data, (DWORD) length,
		&bytes_returned, &ol);

	if (!res) {
		if (GetLastError() != ERROR_IO_PENDING) {
			/* DeviceIoControl() failed. Return error. */
			register_winapi_error(dev, L"Get Input/Feature Report DeviceIoControl");
			return -1;
		}
	}

	/* Wait here until the write is done. This makes
	   hid_get_feature_report() synchronous. */
	res = GetOverlappedResult(dev->device_handle, &ol, &bytes_returned, TRUE/*wait*/);
	if (!res) {
		/* The operation failed. */
		register_winapi_error(dev, L"Get Input/Feature Report GetOverLappedResult");
		return -1;
	}

	/* When numbered reports aren't used,
	   bytes_returned seem to include only what is actually received from the device
	   (not including the first byte with 0, as an indication "no numbered reports"). */
	if (data[0] == 0x0) {
		bytes_returned++;
	}

	return bytes_returned;
}

int HID_API_EXPORT HID_API_CALL hid_get_feature_report(hid_device *dev, unsigned char *data, size_t length)
{
	/* We could use HidD_GetFeature() instead, but it doesn't give us an actual length, unfortunately */
	return hid_get_report(dev, IOCTL_HID_GET_FEATURE, data, length);
}

int HID_API_EXPORT HID_API_CALL hid_send_output_report(hid_device* dev, const unsigned char* data, size_t length)
{
	BOOL res = FALSE;
	unsigned char *buf;
	size_t length_to_send;

	if (!data || !length) {
		register_string_error(dev, L"Zero buffer/length");
		return -1;
	}

	register_string_error(dev, NULL);

	/* Windows expects at least caps.OutputeportByteLength bytes passed
	   to HidD_SetOutputReport(), even if the report is shorter. Any less sent and
	   the function fails with error ERROR_INVALID_PARAMETER set. Any more 
	   and HidD_SetOutputReport() silently truncates the data sent in the report
	   to caps.OutputReportByteLength. */
	if (length >= dev->output_report_length) {
		buf = (unsigned char *) data;
		length_to_send = length;
	} else {
		if (dev->write_buf == NULL) {
			dev->write_buf = (unsigned char *) malloc(dev->output_report_length);

			if (dev->write_buf == NULL) {
				register_string_error(dev, L"hid_send_output_report/malloc");
				return -1;
			}
		}

		buf = dev->write_buf;
		memcpy(buf, data, length);
		memset(buf + length, 0, dev->output_report_length - length);
		length_to_send = dev->output_report_length;
	}

	res = HidD_SetOutputReport(dev->device_handle, (PVOID)buf, (DWORD) length_to_send);
	if (!res) {
		register_string_error(dev, L"HidD_SetOutputReport");
		return -1;
	}

	return (int) length;
}

int HID_API_EXPORT HID_API_CALL hid_get_input_report(hid_device *dev, unsigned char *data, size_t length)
{
	/* We could use HidD_GetInputReport() instead, but it doesn't give us an actual length, unfortunately */
	return hid_get_report(dev, IOCTL_HID_GET_INPUT_REPORT, data, length);
}

void HID_API_EXPORT HID_API_CALL hid_close(hid_device *dev)
{
	if (!dev)
		return;

	CancelIo(dev->device_handle);
	free_hid_device(dev);
}

int HID_API_EXPORT_CALL HID_API_CALL hid_get_manufacturer_string(hid_device *dev, wchar_t *string, size_t maxlen)
{
	if (!string || !maxlen) {
		register_string_error(dev, L"Zero buffer/length");
		return -1;
	}

	if (!dev->device_info) {
		register_string_error(dev, L"NULL device info");
		return -1;
	}

	wcsncpy(string, dev->device_info->manufacturer_string, maxlen);
	string[maxlen - 1] = L'\0';

	register_string_error(dev, NULL);

	return 0;
}

int HID_API_EXPORT_CALL HID_API_CALL hid_get_product_string(hid_device *dev, wchar_t *string, size_t maxlen)
{
	if (!string || !maxlen) {
		register_string_error(dev, L"Zero buffer/length");
		return -1;
	}

	if (!dev->device_info) {
		register_string_error(dev, L"NULL device info");
		return -1;
	}

	wcsncpy(string, dev->device_info->product_string, maxlen);
	string[maxlen - 1] = L'\0';

	register_string_error(dev, NULL);

	return 0;
}

int HID_API_EXPORT_CALL HID_API_CALL hid_get_serial_number_string(hid_device *dev, wchar_t *string, size_t maxlen)
{
	if (!string || !maxlen) {
		register_string_error(dev, L"Zero buffer/length");
		return -1;
	}

	if (!dev->device_info) {
		register_string_error(dev, L"NULL device info");
		return -1;
	}

	wcsncpy(string, dev->device_info->serial_number, maxlen);
	string[maxlen - 1] = L'\0';

	register_string_error(dev, NULL);

	return 0;
}

HID_API_EXPORT struct hid_device_info * HID_API_CALL hid_get_device_info(hid_device *dev) {
	if (!dev->device_info)
	{
		register_string_error(dev, L"NULL device info");
		return NULL;
	}

	return dev->device_info;
}

int HID_API_EXPORT_CALL HID_API_CALL hid_get_indexed_string(hid_device *dev, int string_index, wchar_t *string, size_t maxlen)
{
	BOOL res;

	if (dev->device_info && dev->device_info->bus_type == HID_API_BUS_USB && maxlen > MAX_STRING_WCHARS_USB) {
		string[MAX_STRING_WCHARS_USB] = L'\0';
		maxlen = MAX_STRING_WCHARS_USB;
	}

	res = HidD_GetIndexedString(dev->device_handle, string_index, string, (ULONG)maxlen * sizeof(wchar_t));
	if (!res) {
		register_winapi_error(dev, L"HidD_GetIndexedString");
		return -1;
	}

	register_string_error(dev, NULL);

	return 0;
}

int HID_API_EXPORT_CALL hid_winapi_get_container_id(hid_device *dev, GUID *container_id)
{
	wchar_t *interface_path = NULL, *device_id = NULL;
	CONFIGRET cr = CR_FAILURE;
	DEVINST dev_node;
	DEVPROPTYPE property_type;
	ULONG len;

	if (!container_id) {
		register_string_error(dev, L"Invalid Container ID");
		return -1;
	}

	register_string_error(dev, NULL);

	interface_path = hid_internal_UTF8toUTF16(dev->device_info->path);
	if (!interface_path) {
		register_string_error(dev, L"Path conversion failure");
		goto end;
	}

	/* Get the device id from interface path */
	device_id = hid_internal_get_device_interface_property(interface_path, &DEVPKEY_Device_InstanceId, DEVPROP_TYPE_STRING);
	if (!device_id) {
		register_string_error(dev, L"Failed to get device interface property InstanceId");
		goto end;
	}

	/* Open devnode from device id */
	cr = CM_Locate_DevNodeW(&dev_node, (DEVINSTID_W)device_id, CM_LOCATE_DEVNODE_NORMAL);
	if (cr != CR_SUCCESS) {
		register_string_error(dev, L"Failed to locate device node");
		goto end;
	}

	/* Get the container id from devnode */
	len = sizeof(*container_id);
	cr = CM_Get_DevNode_PropertyW(dev_node, &DEVPKEY_Device_ContainerId, &property_type, (PBYTE)container_id, &len, 0);
	if (cr == CR_SUCCESS && property_type != DEVPROP_TYPE_GUID)
		cr = CR_FAILURE;

	if (cr != CR_SUCCESS)
		register_string_error(dev, L"Failed to read ContainerId property from device node");

end:
	free(interface_path);
	free(device_id);

	return cr == CR_SUCCESS ? 0 : -1;
}


int HID_API_EXPORT_CALL hid_get_report_descriptor(hid_device *dev, unsigned char *buf, size_t buf_size)
{
	// Decision made: Not to include hidapi_descriptor_reconstruct, since it's a massive pain.
	return -1;
}

HID_API_EXPORT const wchar_t * HID_API_CALL  hid_error(hid_device *dev)
{
	if (dev) {
		if (dev->last_error_str == NULL)
			return L"Success";
		return (wchar_t*)dev->last_error_str;
	}

	if (last_global_error_str == NULL)
		return L"Success";
	return last_global_error_str;
}

//#ifndef hidapi_winapi_EXPORTS
//#include "hidapi_descriptor_reconstruct.c"
//#endif

#ifdef __cplusplus
} /* extern "C" */
#endif


#elif defined( __MACOSX__ )
/*******************************************************
 HIDAPI - Multi-Platform library for
 communication with HID devices.

 Alan Ott
 Signal 11 Software

 libusb/hidapi Team

 Copyright 2022, All Rights Reserved.

 At the discretion of the user of this library,
 this software may be licensed under the terms of the
 GNU General Public License v3, a BSD-Style license, or the
 original HIDAPI license as outlined in the LICENSE.txt,
 LICENSE-gpl3.txt, LICENSE-bsd.txt, and LICENSE-orig.txt
 files located at the root of the source distribution.
 These files may also be found in the public source
 code repository located at:
        https://github.com/libusb/hidapi .
********************************************************/

/* See Apple Technical Note TN2187 for details on IOHidManager. */

#include <IOKit/hid/IOHIDManager.h>
#include <IOKit/hid/IOHIDKeys.h>
#include <IOKit/IOKitLib.h>
#include <IOKit/usb/USBSpec.h>
#include <CoreFoundation/CoreFoundation.h>
#include <mach/mach_error.h>
#include <stdbool.h>
#include <wchar.h>
#include <locale.h>
#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>
#include <dlfcn.h>

#include "hidapi.h"

/* Barrier implementation because Mac OSX doesn't have pthread_barrier.
   It also doesn't have clock_gettime(). So much for POSIX and SUSv2.
   This implementation came from Brent Priddy and was posted on
   StackOverflow. It is used with his permission. */
typedef int pthread_barrierattr_t;
typedef struct pthread_barrier {
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int count;
    int trip_count;
} pthread_barrier_t;

static int pthread_barrier_init(pthread_barrier_t *barrier, const pthread_barrierattr_t *attr, unsigned int count)
{
	(void) attr;

	if (count == 0) {
		errno = EINVAL;
		return -1;
	}

	if (pthread_mutex_init(&barrier->mutex, 0) < 0) {
		return -1;
	}
	if (pthread_cond_init(&barrier->cond, 0) < 0) {
		pthread_mutex_destroy(&barrier->mutex);
		return -1;
	}
	barrier->trip_count = count;
	barrier->count = 0;

	return 0;
}

static int pthread_barrier_destroy(pthread_barrier_t *barrier)
{
	pthread_cond_destroy(&barrier->cond);
	pthread_mutex_destroy(&barrier->mutex);
	return 0;
}

static int pthread_barrier_wait(pthread_barrier_t *barrier)
{
	pthread_mutex_lock(&barrier->mutex);
	++(barrier->count);
	if (barrier->count >= barrier->trip_count) {
		barrier->count = 0;
		pthread_mutex_unlock(&barrier->mutex);
		pthread_cond_broadcast(&barrier->cond);
		return 1;
	}
	else {
		do {
			pthread_cond_wait(&barrier->cond, &(barrier->mutex));
		}
		while (barrier->count != 0);

		pthread_mutex_unlock(&barrier->mutex);
		return 0;
	}
}

static int return_data(hid_device *dev, unsigned char *data, size_t length);

/* Linked List of input reports received from the device. */
struct input_report {
	uint8_t *data;
	size_t len;
	struct input_report *next;
};

static struct hid_api_version api_version = {
	.major = HID_API_VERSION_MAJOR,
	.minor = HID_API_VERSION_MINOR,
	.patch = HID_API_VERSION_PATCH
};

/* - Run context - */
static	IOHIDManagerRef hid_mgr = 0x0;
static	int is_macos_10_10_or_greater = 0;
static	IOOptionBits device_open_options = 0;
static	wchar_t *last_global_error_str = NULL;
/* --- */

struct hid_device_ {
	IOHIDDeviceRef device_handle;
	IOOptionBits open_options;
	int blocking;
	int disconnected;
	CFStringRef run_loop_mode;
	CFRunLoopRef run_loop;
	CFRunLoopSourceRef source;
	uint8_t *input_report_buf;
	CFIndex max_input_report_len;
	struct input_report *input_reports;
	struct hid_device_info* device_info;

	pthread_t thread;
	pthread_mutex_t mutex; /* Protects input_reports */
	pthread_cond_t condition;
	pthread_barrier_t barrier; /* Ensures correct startup sequence */
	pthread_barrier_t shutdown_barrier; /* Ensures correct shutdown sequence */
	int shutdown_thread;
	wchar_t *last_error_str;
};

static hid_device *new_hid_device(void)
{
	hid_device *dev = (hid_device*) calloc(1, sizeof(hid_device));
	if (dev == NULL) {
		return NULL;
	}

	dev->device_handle = NULL;
	dev->open_options = device_open_options;
	dev->blocking = 1;
	dev->disconnected = 0;
	dev->run_loop_mode = NULL;
	dev->run_loop = NULL;
	dev->source = NULL;
	dev->input_report_buf = NULL;
	dev->input_reports = NULL;
	dev->device_info = NULL;
	dev->shutdown_thread = 0;
	dev->last_error_str = NULL;

	/* Thread objects */
	pthread_mutex_init(&dev->mutex, NULL);
	pthread_cond_init(&dev->condition, NULL);
	pthread_barrier_init(&dev->barrier, NULL, 2);
	pthread_barrier_init(&dev->shutdown_barrier, NULL, 2);

	return dev;
}

static void free_hid_device(hid_device *dev)
{
	if (!dev)
		return;

	/* Delete any input reports still left over. */
	struct input_report *rpt = dev->input_reports;
	while (rpt) {
		struct input_report *next = rpt->next;
		free(rpt->data);
		free(rpt);
		rpt = next;
	}

	/* Free the string and the report buffer. The check for NULL
	   is necessary here as CFRelease() doesn't handle NULL like
	   free() and others do. */
	if (dev->run_loop_mode)
		CFRelease(dev->run_loop_mode);
	if (dev->source)
		CFRelease(dev->source);
	free(dev->input_report_buf);
	hid_free_enumeration(dev->device_info);

	/* Clean up the thread objects */
	pthread_barrier_destroy(&dev->shutdown_barrier);
	pthread_barrier_destroy(&dev->barrier);
	pthread_cond_destroy(&dev->condition);
	pthread_mutex_destroy(&dev->mutex);

	/* Free the structure itself. */
	free(dev);
}


/* The caller must free the returned string with free(). */
static wchar_t *utf8_to_wchar_t(const char *utf8)
{
	wchar_t *ret = NULL;

	if (utf8) {
		size_t wlen = mbstowcs(NULL, utf8, 0);
		if ((size_t) -1 == wlen) {
			return wcsdup(L"");
		}
		ret = (wchar_t*) calloc(wlen+1, sizeof(wchar_t));
		if (ret == NULL) {
			/* as much as we can do at this point */
			return NULL;
		}
		mbstowcs(ret, utf8, wlen+1);
		ret[wlen] = 0x0000;
	}

	return ret;
}


/* Makes a copy of the given error message (and decoded according to the
 * currently locale) into the wide string pointer pointed by error_str.
 * The last stored error string is freed.
 * Use register_error_str(NULL) to free the error message completely. */
static void register_error_str(wchar_t **error_str, const char *msg)
{
	free(*error_str);
	*error_str = utf8_to_wchar_t(msg);
}

/* Similar to register_error_str, but allows passing a format string with va_list args into this function. */
static void register_error_str_vformat(wchar_t **error_str, const char *format, va_list args)
{
	char msg[1024];
	vsnprintf(msg, sizeof(msg), format, args);

	register_error_str(error_str, msg);
}

/* Set the last global error to be reported by hid_error(NULL).
 * The given error message will be copied (and decoded according to the
 * currently locale, so do not pass in string constants).
 * The last stored global error message is freed.
 * Use register_global_error(NULL) to indicate "no error". */
static void register_global_error(const char *msg)
{
	register_error_str(&last_global_error_str, msg);
}

/* Similar to register_global_error, but allows passing a format string into this function. */
static void register_global_error_format(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	register_error_str_vformat(&last_global_error_str, format, args);
	va_end(args);
}

/* Set the last error for a device to be reported by hid_error(dev).
 * The given error message will be copied (and decoded according to the
 * currently locale, so do not pass in string constants).
 * The last stored device error message is freed.
 * Use register_device_error(dev, NULL) to indicate "no error". */
static void register_device_error(hid_device *dev, const char *msg)
{
	register_error_str(&dev->last_error_str, msg);
}

/* Similar to register_device_error, but you can pass a format string into this function. */
static void register_device_error_format(hid_device *dev, const char *format, ...)
{
	va_list args;
	va_start(args, format);
	register_error_str_vformat(&dev->last_error_str, format, args);
	va_end(args);
}


static CFArrayRef get_array_property(IOHIDDeviceRef device, CFStringRef key)
{
	CFTypeRef ref = IOHIDDeviceGetProperty(device, key);
	if (ref != NULL && CFGetTypeID(ref) == CFArrayGetTypeID()) {
		return (CFArrayRef)ref;
	} else {
		return NULL;
	}
}

static int32_t get_int_property(IOHIDDeviceRef device, CFStringRef key)
{
	CFTypeRef ref;
	int32_t value = 0;

	ref = IOHIDDeviceGetProperty(device, key);
	if (ref) {
		if (CFGetTypeID(ref) == CFNumberGetTypeID()) {
			CFNumberGetValue((CFNumberRef) ref, kCFNumberSInt32Type, &value);
			return value;
		}
	}
	return 0;
}

static bool try_get_int_property(IOHIDDeviceRef device, CFStringRef key, int32_t *out_val)
{
	bool result = false;
	CFTypeRef ref;

	ref = IOHIDDeviceGetProperty(device, key);
	if (ref) {
		if (CFGetTypeID(ref) == CFNumberGetTypeID()) {
			result = CFNumberGetValue((CFNumberRef) ref, kCFNumberSInt32Type, out_val);
		}
	}
	return result;
}

static bool try_get_ioregistry_int_property(io_service_t service, CFStringRef property, int32_t *out_val)
{
	bool result = false;
	CFTypeRef ref = IORegistryEntryCreateCFProperty(service, property, kCFAllocatorDefault, 0);

	if (ref) {
		if (CFGetTypeID(ref) == CFNumberGetTypeID()) {
			result = CFNumberGetValue(ref, kCFNumberSInt32Type, out_val);
		}

		CFRelease(ref);
	}

	return result;
}

static CFArrayRef get_usage_pairs(IOHIDDeviceRef device)
{
	return get_array_property(device, CFSTR(kIOHIDDeviceUsagePairsKey));
}

static unsigned short get_vendor_id(IOHIDDeviceRef device)
{
	return get_int_property(device, CFSTR(kIOHIDVendorIDKey));
}

static unsigned short get_product_id(IOHIDDeviceRef device)
{
	return get_int_property(device, CFSTR(kIOHIDProductIDKey));
}

static int32_t get_max_report_length(IOHIDDeviceRef device)
{
	return get_int_property(device, CFSTR(kIOHIDMaxInputReportSizeKey));
}

static int get_string_property(IOHIDDeviceRef device, CFStringRef prop, wchar_t *buf, size_t len)
{
	CFStringRef str;

	if (!len)
		return 0;

	str = (CFStringRef) IOHIDDeviceGetProperty(device, prop);

	buf[0] = 0;

	if (str && CFGetTypeID(str) == CFStringGetTypeID()) {
		CFIndex str_len = CFStringGetLength(str);
		CFRange range;
		CFIndex used_buf_len;
		CFIndex chars_copied;

		len --;

		range.location = 0;
		range.length = ((size_t) str_len > len)? len: (size_t) str_len;
		chars_copied = CFStringGetBytes(str,
			range,
			kCFStringEncodingUTF32LE,
			(char) '?',
			FALSE,
			(UInt8*)buf,
			len * sizeof(wchar_t),
			&used_buf_len);

		if (chars_copied <= 0)
			buf[0] = 0;
		else
			buf[chars_copied] = 0;

		return 0;
	}
	else
		return -1;

}

static int get_serial_number(IOHIDDeviceRef device, wchar_t *buf, size_t len)
{
	return get_string_property(device, CFSTR(kIOHIDSerialNumberKey), buf, len);
}

static int get_manufacturer_string(IOHIDDeviceRef device, wchar_t *buf, size_t len)
{
	return get_string_property(device, CFSTR(kIOHIDManufacturerKey), buf, len);
}

static int get_product_string(IOHIDDeviceRef device, wchar_t *buf, size_t len)
{
	return get_string_property(device, CFSTR(kIOHIDProductKey), buf, len);
}


/* Implementation of wcsdup() for Mac. */
static wchar_t *dup_wcs(const wchar_t *s)
{
	size_t len = wcslen(s);
	wchar_t *ret = (wchar_t*) malloc((len+1)*sizeof(wchar_t));
	wcscpy(ret, s);

	return ret;
}

/* Initialize the IOHIDManager. Return 0 for success and -1 for failure. */
static int init_hid_manager(void)
{
	/* Initialize all the HID Manager Objects */
	hid_mgr = IOHIDManagerCreate(kCFAllocatorDefault, kIOHIDOptionsTypeNone);
	if (hid_mgr) {
		IOHIDManagerSetDeviceMatching(hid_mgr, NULL);
		IOHIDManagerScheduleWithRunLoop(hid_mgr, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);
		return 0;
	}

	register_global_error("Failed to create IOHIDManager");
	return -1;
}

HID_API_EXPORT const struct hid_api_version* HID_API_CALL hid_version(void)
{
	return &api_version;
}

HID_API_EXPORT const char* HID_API_CALL hid_version_str(void)
{
	return HID_API_VERSION_STR;
}

/* Initialize the IOHIDManager if necessary. This is the public function, and
   it is safe to call this function repeatedly. Return 0 for success and -1
   for failure. */
int HID_API_EXPORT hid_init(void)
{
	register_global_error(NULL);

	if (!hid_mgr) {
		is_macos_10_10_or_greater = (kCFCoreFoundationVersionNumber >= 1151.16); /* kCFCoreFoundationVersionNumber10_10 */
		hid_darwin_set_open_exclusive(1); /* Backward compatibility */
		return init_hid_manager();
	}

	/* Already initialized. */
	return 0;
}

int HID_API_EXPORT hid_exit(void)
{
	if (hid_mgr) {
		/* Close the HID manager. */
		IOHIDManagerClose(hid_mgr, kIOHIDOptionsTypeNone);
		CFRelease(hid_mgr);
		hid_mgr = NULL;
	}

	/* Free global error message */
	register_global_error(NULL);

	return 0;
}

static void process_pending_events(void) {
	SInt32 res;
	do {
		res = CFRunLoopRunInMode(kCFRunLoopDefaultMode, 0.001, FALSE);
	} while(res != kCFRunLoopRunFinished && res != kCFRunLoopRunTimedOut);
}

static int read_usb_interface_from_hid_service_parent(io_service_t hid_service)
{
	int32_t result = -1;
	bool success = false;
	io_registry_entry_t current = IO_OBJECT_NULL;
	kern_return_t res;
	int parent_number = 0;

	res = IORegistryEntryGetParentEntry(hid_service, kIOServicePlane, &current);
	while (KERN_SUCCESS == res
			/* Only search up to 3 parent entries.
			 * With the default driver - the parent-of-interest supposed to be the first one,
			 * but lets assume some custom drivers or so, with deeper tree. */
			&& parent_number < 3) {
		io_registry_entry_t parent = IO_OBJECT_NULL;
		int32_t interface_number = -1;
		parent_number++;

		success = try_get_ioregistry_int_property(current, CFSTR(kUSBInterfaceNumber), &interface_number);
		if (success) {
			result = interface_number;
			break;
		}

		res = IORegistryEntryGetParentEntry(current, kIOServicePlane, &parent);
		if (parent) {
			IOObjectRelease(current);
			current = parent;
		}

	}

	if (current) {
		IOObjectRelease(current);
		current = IO_OBJECT_NULL;
	}

	return result;
}

static struct hid_device_info *create_device_info_with_usage(IOHIDDeviceRef dev, int32_t usage_page, int32_t usage)
{
	unsigned short dev_vid;
	unsigned short dev_pid;
	int BUF_LEN = 256;
	wchar_t buf[BUF_LEN];
	CFTypeRef transport_prop;

	struct hid_device_info *cur_dev;
	io_service_t hid_service;
	kern_return_t res;
	uint64_t entry_id = 0;

	if (dev == NULL) {
		return NULL;
	}

	cur_dev = (struct hid_device_info *)calloc(1, sizeof(struct hid_device_info));
	if (cur_dev == NULL) {
		return NULL;
	}

	dev_vid = get_vendor_id(dev);
	dev_pid = get_product_id(dev);

	cur_dev->usage_page = usage_page;
	cur_dev->usage = usage;

	/* Fill out the record */
	cur_dev->next = NULL;

	/* Fill in the path (as a unique ID of the service entry) */
	cur_dev->path = NULL;
	hid_service = IOHIDDeviceGetService(dev);
	if (hid_service != MACH_PORT_NULL) {
		res = IORegistryEntryGetRegistryEntryID(hid_service, &entry_id);
	}
	else {
		res = KERN_INVALID_ARGUMENT;
	}

	if (res == KERN_SUCCESS) {
		/* max value of entry_id(uint64_t) is 18446744073709551615 which is 20 characters long,
		   so for (max) "path" string 'DevSrvsID:18446744073709551615' we would need
		   9+1+20+1=31 bytes buffer, but allocate 32 for simple alignment */
		const size_t path_len = 32;
		cur_dev->path = calloc(1, path_len);
		if (cur_dev->path != NULL) {
			snprintf(cur_dev->path, path_len, "DevSrvsID:%llu", entry_id);
		}
	}

	if (cur_dev->path == NULL) {
		/* for whatever reason, trying to keep it a non-NULL string */
		cur_dev->path = strdup("");
	}

	/* Serial Number */
	get_serial_number(dev, buf, BUF_LEN);
	cur_dev->serial_number = dup_wcs(buf);

	/* Manufacturer and Product strings */
	get_manufacturer_string(dev, buf, BUF_LEN);
	cur_dev->manufacturer_string = dup_wcs(buf);
	get_product_string(dev, buf, BUF_LEN);
	cur_dev->product_string = dup_wcs(buf);

	/* VID/PID */
	cur_dev->vendor_id = dev_vid;
	cur_dev->product_id = dev_pid;

	/* Release Number */
	cur_dev->release_number = get_int_property(dev, CFSTR(kIOHIDVersionNumberKey));

	/* Interface Number.
	 * We can only retrieve the interface number for USB HID devices.
	 * See below */
	cur_dev->interface_number = -1;

	/* Bus Type */
	transport_prop = IOHIDDeviceGetProperty(dev, CFSTR(kIOHIDTransportKey));

	if (transport_prop != NULL && CFGetTypeID(transport_prop) == CFStringGetTypeID()) {
		if (CFStringCompare((CFStringRef)transport_prop, CFSTR(kIOHIDTransportUSBValue), 0) == kCFCompareEqualTo) {
			int32_t interface_number = -1;
			cur_dev->bus_type = HID_API_BUS_USB;

			/* A IOHIDDeviceRef used to have this simple property,
			 * until macOS 13.3 - we will try to use it. */
			if (try_get_int_property(dev, CFSTR(kUSBInterfaceNumber), &interface_number)) {
				cur_dev->interface_number = interface_number;
			} else {
				/* Otherwise fallback to io_service_t property.
				 * (of one of the parent services). */
				cur_dev->interface_number = read_usb_interface_from_hid_service_parent(hid_service);

				/* If the above doesn't work -
				 * no (known) fallback exists at this point. */
			}

		/* Match "Bluetooth", "BluetoothLowEnergy" and "Bluetooth Low Energy" strings */
		} else if (CFStringHasPrefix((CFStringRef)transport_prop, CFSTR(kIOHIDTransportBluetoothValue))) {
			cur_dev->bus_type = HID_API_BUS_BLUETOOTH;
		} else if (CFStringCompare((CFStringRef)transport_prop, CFSTR(kIOHIDTransportI2CValue), 0) == kCFCompareEqualTo) {
			cur_dev->bus_type = HID_API_BUS_I2C;
		} else  if (CFStringCompare((CFStringRef)transport_prop, CFSTR(kIOHIDTransportSPIValue), 0) == kCFCompareEqualTo) {
			cur_dev->bus_type = HID_API_BUS_SPI;
		}
	}

	return cur_dev;
}

static struct hid_device_info *create_device_info(IOHIDDeviceRef device)
{
	const int32_t primary_usage_page = get_int_property(device, CFSTR(kIOHIDPrimaryUsagePageKey));
	const int32_t primary_usage = get_int_property(device, CFSTR(kIOHIDPrimaryUsageKey));

	/* Primary should always be first, to match previous behavior. */
	struct hid_device_info *root = create_device_info_with_usage(device, primary_usage_page, primary_usage);
	struct hid_device_info *cur = root;

	if (!root)
		return NULL;

	CFArrayRef usage_pairs = get_usage_pairs(device);

	if (usage_pairs != NULL) {
		struct hid_device_info *next = NULL;
		for (CFIndex i = 0; i < CFArrayGetCount(usage_pairs); i++) {
			CFTypeRef dict = CFArrayGetValueAtIndex(usage_pairs, i);
			if (CFGetTypeID(dict) != CFDictionaryGetTypeID()) {
				continue;
			}

			CFTypeRef usage_page_ref, usage_ref;
			int32_t usage_page, usage;

			if (!CFDictionaryGetValueIfPresent((CFDictionaryRef)dict, CFSTR(kIOHIDDeviceUsagePageKey), &usage_page_ref) ||
			    !CFDictionaryGetValueIfPresent((CFDictionaryRef)dict, CFSTR(kIOHIDDeviceUsageKey), &usage_ref) ||
					CFGetTypeID(usage_page_ref) != CFNumberGetTypeID() ||
					CFGetTypeID(usage_ref) != CFNumberGetTypeID() ||
					!CFNumberGetValue((CFNumberRef)usage_page_ref, kCFNumberSInt32Type, &usage_page) ||
					!CFNumberGetValue((CFNumberRef)usage_ref, kCFNumberSInt32Type, &usage)) {
					continue;
			}
			if (usage_page == primary_usage_page && usage == primary_usage)
				continue; /* Already added. */

			next = create_device_info_with_usage(device, usage_page, usage);
			cur->next = next;
			if (next != NULL) {
				cur = next;
			}
		}
	}

	return root;
}

struct hid_device_info  HID_API_EXPORT *hid_enumerate(unsigned short vendor_id, unsigned short product_id)
{
	struct hid_device_info *root = NULL; /* return object */
	struct hid_device_info *cur_dev = NULL;
	CFIndex num_devices;
	int i;

	/* Set up the HID Manager if it hasn't been done */
	if (hid_init() < 0) {
		return NULL;
	}
	/* register_global_error: global error is set/reset by hid_init */

	/* give the IOHIDManager a chance to update itself */
	process_pending_events();

	/* Get a list of the Devices */
	CFMutableDictionaryRef matching = NULL;
	if (vendor_id != 0 || product_id != 0) {
		matching = CFDictionaryCreateMutable(kCFAllocatorDefault, kIOHIDOptionsTypeNone, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);

		if (matching && vendor_id != 0) {
			CFNumberRef v = CFNumberCreate(kCFAllocatorDefault, kCFNumberShortType, &vendor_id);
			CFDictionarySetValue(matching, CFSTR(kIOHIDVendorIDKey), v);
			CFRelease(v);
		}

		if (matching && product_id != 0) {
			CFNumberRef p = CFNumberCreate(kCFAllocatorDefault, kCFNumberShortType, &product_id);
			CFDictionarySetValue(matching, CFSTR(kIOHIDProductIDKey), p);
			CFRelease(p);
		}
	}
	IOHIDManagerSetDeviceMatching(hid_mgr, matching);
	if (matching != NULL) {
		CFRelease(matching);
	}

	CFSetRef device_set = IOHIDManagerCopyDevices(hid_mgr);

	IOHIDDeviceRef *device_array = NULL;

	if (device_set != NULL) {
		/* Convert the list into a C array so we can iterate easily. */
		num_devices = CFSetGetCount(device_set);
		device_array = (IOHIDDeviceRef*) calloc(num_devices, sizeof(IOHIDDeviceRef));
		CFSetGetValues(device_set, (const void **) device_array);
	} else {
		num_devices = 0;
	}

	/* Iterate over each device, making an entry for it. */
	for (i = 0; i < num_devices; i++) {

		IOHIDDeviceRef dev = device_array[i];
		if (!dev) {
			continue;
		}

		struct hid_device_info *tmp = create_device_info(dev);
		if (tmp == NULL) {
			continue;
		}

		if (cur_dev) {
			cur_dev->next = tmp;
		}
		else {
			root = tmp;
		}
		cur_dev = tmp;

		/* move the pointer to the tail of returned list */
		while (cur_dev->next != NULL) {
			cur_dev = cur_dev->next;
		}
	}

	free(device_array);
	if (device_set != NULL)
		CFRelease(device_set);

	if (root == NULL) {
		if (vendor_id == 0 && product_id == 0) {
			register_global_error("No HID devices found in the system.");
		} else {
			register_global_error("No HID devices with requested VID/PID found in the system.");
		}
	}

	return root;
}

void  HID_API_EXPORT hid_free_enumeration(struct hid_device_info *devs)
{
	/* This function is identical to the Linux version. Platform independent. */
	struct hid_device_info *d = devs;
	while (d) {
		struct hid_device_info *next = d->next;
		free(d->path);
		free(d->serial_number);
		free(d->manufacturer_string);
		free(d->product_string);
		free(d);
		d = next;
	}
}

hid_device * HID_API_EXPORT hid_open(unsigned short vendor_id, unsigned short product_id, const wchar_t *serial_number)
{
	/* This function is identical to the Linux version. Platform independent. */

	struct hid_device_info *devs, *cur_dev;
	const char *path_to_open = NULL;
	hid_device * handle = NULL;

	/* register_global_error: global error is reset by hid_enumerate/hid_init */
	devs = hid_enumerate(vendor_id, product_id);
	if (devs == NULL) {
		/* register_global_error: global error is already set by hid_enumerate */
		return NULL;
	}

	cur_dev = devs;
	while (cur_dev) {
		if (cur_dev->vendor_id == vendor_id &&
		    cur_dev->product_id == product_id) {
			if (serial_number) {
				if (wcscmp(serial_number, cur_dev->serial_number) == 0) {
					path_to_open = cur_dev->path;
					break;
				}
			}
			else {
				path_to_open = cur_dev->path;
				break;
			}
		}
		cur_dev = cur_dev->next;
	}

	if (path_to_open) {
		handle = hid_open_path(path_to_open);
	} else {
		register_global_error("Device with requested VID/PID/(SerialNumber) not found");
	}

	hid_free_enumeration(devs);

	return handle;
}

static void hid_device_removal_callback(void *context, IOReturn result,
                                        void *sender)
{
	(void) result;
	(void) sender;

	/* Stop the Run Loop for this device. */
	hid_device *d = (hid_device*) context;

	d->disconnected = 1;
	CFRunLoopStop(d->run_loop);
}

/* The Run Loop calls this function for each input report received.
   This function puts the data into a linked list to be picked up by
   hid_read(). */
static void hid_report_callback(void *context, IOReturn result, void *sender,
                         IOHIDReportType report_type, uint32_t report_id,
                         uint8_t *report, CFIndex report_length)
{
	(void) result;
	(void) sender;
	(void) report_type;
	(void) report_id;

	struct input_report *rpt;
	hid_device *dev = (hid_device*) context;

	/* Make a new Input Report object */
	rpt = (struct input_report*) calloc(1, sizeof(struct input_report));
	rpt->data = (uint8_t*) calloc(1, report_length);
	memcpy(rpt->data, report, report_length);
	rpt->len = report_length;
	rpt->next = NULL;

	/* Lock this section */
	pthread_mutex_lock(&dev->mutex);

	/* Attach the new report object to the end of the list. */
	if (dev->input_reports == NULL) {
		/* The list is empty. Put it at the root. */
		dev->input_reports = rpt;
	}
	else {
		/* Find the end of the list and attach. */
		struct input_report *cur = dev->input_reports;
		int num_queued = 0;
		while (cur->next != NULL) {
			cur = cur->next;
			num_queued++;
		}
		cur->next = rpt;

		/* Pop one off if we've reached 30 in the queue. This
		   way we don't grow forever if the user never reads
		   anything from the device. */
		if (num_queued > 30) {
			return_data(dev, NULL, 0);
		}
	}

	/* Signal a waiting thread that there is data. */
	pthread_cond_signal(&dev->condition);

	/* Unlock */
	pthread_mutex_unlock(&dev->mutex);

}

/* This gets called when the read_thread's run loop gets signaled by
   hid_close(), and serves to stop the read_thread's run loop. */
static void perform_signal_callback(void *context)
{
	hid_device *dev = (hid_device*) context;
	CFRunLoopStop(dev->run_loop); /*TODO: CFRunLoopGetCurrent()*/
}

static void *read_thread(void *param)
{
	hid_device *dev = (hid_device*) param;
	SInt32 code;

	/* Move the device's run loop to this thread. */
	IOHIDDeviceScheduleWithRunLoop(dev->device_handle, CFRunLoopGetCurrent(), dev->run_loop_mode);

	/* Create the RunLoopSource which is used to signal the
	   event loop to stop when hid_close() is called. */
	CFRunLoopSourceContext ctx;
	memset(&ctx, 0, sizeof(ctx));
	ctx.version = 0;
	ctx.info = dev;
	ctx.perform = &perform_signal_callback;
	dev->source = CFRunLoopSourceCreate(kCFAllocatorDefault, 0/*order*/, &ctx);
	CFRunLoopAddSource(CFRunLoopGetCurrent(), dev->source, dev->run_loop_mode);

	/* Store off the Run Loop so it can be stopped from hid_close()
	   and on device disconnection. */
	dev->run_loop = CFRunLoopGetCurrent();

	/* Notify the main thread that the read thread is up and running. */
	pthread_barrier_wait(&dev->barrier);

	/* Run the Event Loop. CFRunLoopRunInMode() will dispatch HID input
	   reports into the hid_report_callback(). */
	while (!dev->shutdown_thread && !dev->disconnected) {
		code = CFRunLoopRunInMode(dev->run_loop_mode, 1000/*sec*/, FALSE);
		/* Return if the device has been disconnected */
		if (code == kCFRunLoopRunFinished || code == kCFRunLoopRunStopped) {
			dev->disconnected = 1;
			break;
		}


		/* Break if The Run Loop returns Finished or Stopped. */
		if (code != kCFRunLoopRunTimedOut &&
		    code != kCFRunLoopRunHandledSource) {
			/* There was some kind of error. Setting
			   shutdown seems to make sense, but
			   there may be something else more appropriate */
			dev->shutdown_thread = 1;
			break;
		}
	}

	/* Now that the read thread is stopping, Wake any threads which are
	   waiting on data (in hid_read_timeout()). Do this under a mutex to
	   make sure that a thread which is about to go to sleep waiting on
	   the condition actually will go to sleep before the condition is
	   signaled. */
	pthread_mutex_lock(&dev->mutex);
	pthread_cond_broadcast(&dev->condition);
	pthread_mutex_unlock(&dev->mutex);

	/* Wait here until hid_close() is called and makes it past
	   the call to CFRunLoopWakeUp(). This thread still needs to
	   be valid when that function is called on the other thread. */
	pthread_barrier_wait(&dev->shutdown_barrier);

	return NULL;
}

/* \p path must be one of:
     - in format 'DevSrvsID:<RegistryEntryID>' (as returned by hid_enumerate);
     - a valid path to an IOHIDDevice in the IOService plane (as returned by IORegistryEntryGetPath,
       e.g.: "IOService:/AppleACPIPlatformExpert/PCI0@0/AppleACPIPCI/EHC1@1D,7/AppleUSBEHCI/PLAYSTATION(R)3 Controller@fd120000/IOUSBInterface@0/IOUSBHIDDriver");
   Second format is for compatibility with paths accepted by older versions of HIDAPI.
*/
static io_registry_entry_t hid_open_service_registry_from_path(const char *path)
{
	if (path == NULL)
		return MACH_PORT_NULL;

	/* Get the IORegistry entry for the given path */
	if (strncmp("DevSrvsID:", path, 10) == 0) {
		char *endptr;
		uint64_t entry_id = strtoull(path + 10, &endptr, 10);
		if (*endptr == '\0') {
			return IOServiceGetMatchingService((mach_port_t) 0, IORegistryEntryIDMatching(entry_id));
		}
	}
	else {
		/* Fallback to older format of the path */
		return IORegistryEntryFromPath((mach_port_t) 0, path);
	}

	return MACH_PORT_NULL;
}

hid_device * HID_API_EXPORT hid_open_path(const char *path)
{
	hid_device *dev = NULL;
	io_registry_entry_t entry = MACH_PORT_NULL;
	IOReturn ret = kIOReturnInvalid;
	char str[32];

	/* Set up the HID Manager if it hasn't been done */
	if (hid_init() < 0) {
		return NULL;
	}
	/* register_global_error: global error is set/reset by hid_init */

	dev = new_hid_device();
	if (!dev) {
		register_global_error("Couldn't allocate memory");
		return NULL;
	}

	/* Get the IORegistry entry for the given path */
	entry = hid_open_service_registry_from_path(path);
	if (entry == MACH_PORT_NULL) {
		/* Path wasn't valid (maybe device was removed?) */
		register_global_error("hid_open_path: device mach entry not found with the given path");
		goto return_error;
	}

	/* Create an IOHIDDevice for the entry */
	dev->device_handle = IOHIDDeviceCreate(kCFAllocatorDefault, entry);
	if (dev->device_handle == NULL) {
		/* Error creating the HID device */
		register_global_error("hid_open_path: failed to create IOHIDDevice from the mach entry");
		goto return_error;
	}

	/* Open the IOHIDDevice */
	ret = IOHIDDeviceOpen(dev->device_handle, dev->open_options);
	if (ret != kIOReturnSuccess) {
		register_global_error_format("hid_open_path: failed to open IOHIDDevice from mach entry: (0x%08X) %s", ret, mach_error_string(ret));
		goto return_error;
	}

	/* Create the buffers for receiving data */
	dev->max_input_report_len = (CFIndex) get_max_report_length(dev->device_handle);
	dev->input_report_buf = (uint8_t*) calloc(dev->max_input_report_len, sizeof(uint8_t));

	/* Create the Run Loop Mode for this device.
	   printing the reference seems to work. */
	snprintf(str, sizeof(str), "HIDAPI_%p", (void*) dev->device_handle);
	dev->run_loop_mode =
		CFStringCreateWithCString(NULL, str, kCFStringEncodingASCII);

	/* Attach the device to a Run Loop */
	IOHIDDeviceRegisterInputReportCallback(
		dev->device_handle, dev->input_report_buf, dev->max_input_report_len,
		&hid_report_callback, dev);
	IOHIDDeviceRegisterRemovalCallback(dev->device_handle, hid_device_removal_callback, dev);

	/* Start the read thread */
	pthread_create(&dev->thread, NULL, read_thread, dev);

	/* Wait here for the read thread to be initialized. */
	pthread_barrier_wait(&dev->barrier);

	IOObjectRelease(entry);
	return dev;

return_error:
	if (dev->device_handle != NULL)
		CFRelease(dev->device_handle);

	if (entry != MACH_PORT_NULL)
		IOObjectRelease(entry);

	free_hid_device(dev);
	return NULL;
}

static int set_report(hid_device *dev, IOHIDReportType type, const unsigned char *data, size_t length)
{
	const unsigned char *data_to_send = data;
	CFIndex length_to_send = length;
	IOReturn res;
	unsigned char report_id;

	register_device_error(dev, NULL);

	if (!data || (length == 0)) {
		register_device_error(dev, strerror(EINVAL));
		return -1;
	}

	report_id = data[0];

	if (report_id == 0x0) {
		/* Not using numbered Reports.
		   Don't send the report number. */
		data_to_send = data+1;
		length_to_send = length-1;
	}

	/* Avoid crash if the device has been unplugged. */
	if (dev->disconnected) {
		register_device_error(dev, "Device is disconnected");
		return -1;
	}

	res = IOHIDDeviceSetReport(dev->device_handle,
	                           type,
	                           report_id,
	                           data_to_send, length_to_send);

	if (res != kIOReturnSuccess) {
		register_device_error_format(dev, "IOHIDDeviceSetReport failed: (0x%08X) %s", res, mach_error_string(res));
		return -1;
	}

	return (int) length;
}

static int get_report(hid_device *dev, IOHIDReportType type, unsigned char *data, size_t length)
{
	unsigned char *report = data;
	CFIndex report_length = length;
	IOReturn res = kIOReturnSuccess;
	const unsigned char report_id = data[0];

	register_device_error(dev, NULL);

	if (report_id == 0x0) {
		/* Not using numbered Reports.
		   Don't send the report number. */
		report = data+1;
		report_length = length-1;
	}

	/* Avoid crash if the device has been unplugged. */
	if (dev->disconnected) {
		register_device_error(dev, "Device is disconnected");
		return -1;
	}

	res = IOHIDDeviceGetReport(dev->device_handle,
	                           type,
	                           report_id,
	                           report, &report_length);

	if (res != kIOReturnSuccess) {
		register_device_error_format(dev, "IOHIDDeviceGetReport failed: (0x%08X) %s", res, mach_error_string(res));
		return -1;
	}

	if (report_id == 0x0) { /* 0 report number still present at the beginning */
		report_length++;
	}

	return (int) report_length;
}

int HID_API_EXPORT hid_write(hid_device *dev, const unsigned char *data, size_t length)
{
	return set_report(dev, kIOHIDReportTypeOutput, data, length);
}

/* Helper function, so that this isn't duplicated in hid_read(). */
static int return_data(hid_device *dev, unsigned char *data, size_t length)
{
	/* Copy the data out of the linked list item (rpt) into the
	   return buffer (data), and delete the liked list item. */
	struct input_report *rpt = dev->input_reports;
	size_t len = (length < rpt->len)? length: rpt->len;
	if (data != NULL) {
		memcpy(data, rpt->data, len);
	}
	dev->input_reports = rpt->next;
	free(rpt->data);
	free(rpt);
	return (int) len;
}

static int cond_wait(hid_device *dev, pthread_cond_t *cond, pthread_mutex_t *mutex)
{
	while (!dev->input_reports) {
		int res = pthread_cond_wait(cond, mutex);
		if (res != 0)
			return res;

		/* A res of 0 means we may have been signaled or it may
		   be a spurious wakeup. Check to see that there's actually
		   data in the queue before returning, and if not, go back
		   to sleep. See the pthread_cond_timedwait() man page for
		   details. */

		if (dev->shutdown_thread || dev->disconnected) {
			return -1;
		}
	}

	return 0;
}

static int cond_timedwait(hid_device *dev, pthread_cond_t *cond, pthread_mutex_t *mutex, const struct timespec *abstime)
{
	while (!dev->input_reports) {
		int res = pthread_cond_timedwait(cond, mutex, abstime);
		if (res != 0)
			return res;

		/* A res of 0 means we may have been signaled or it may
		   be a spurious wakeup. Check to see that there's actually
		   data in the queue before returning, and if not, go back
		   to sleep. See the pthread_cond_timedwait() man page for
		   details. */

		if (dev->shutdown_thread || dev->disconnected) {
			return -1;
		}
	}

	return 0;

}

int HID_API_EXPORT hid_read_timeout(hid_device *dev, unsigned char *data, size_t length, int milliseconds)
{
	int bytes_read = -1;

	/* Lock the access to the report list. */
	pthread_mutex_lock(&dev->mutex);

	/* There's an input report queued up. Return it. */
	if (dev->input_reports) {
		/* Return the first one */
		bytes_read = return_data(dev, data, length);
		goto ret;
	}

	/* Return if the device has been disconnected. */
	if (dev->disconnected) {
		bytes_read = -1;
		register_device_error(dev, "hid_read_timeout: device disconnected");
		goto ret;
	}

	if (dev->shutdown_thread) {
		/* This means the device has been closed (or there
		   has been an error. An error code of -1 should
		   be returned. */
		bytes_read = -1;
		register_device_error(dev, "hid_read_timeout: thread shutdown");
		goto ret;
	}

	/* There is no data. Go to sleep and wait for data. */

	if (milliseconds == -1) {
		/* Blocking */
		int res;
		res = cond_wait(dev, &dev->condition, &dev->mutex);
		if (res == 0)
			bytes_read = return_data(dev, data, length);
		else {
			/* There was an error, or a device disconnection. */
			register_device_error(dev, "hid_read_timeout: error waiting for more data");
			bytes_read = -1;
		}
	}
	else if (milliseconds > 0) {
		/* Non-blocking, but called with timeout. */
		int res;
		struct timespec ts;
		struct timeval tv;
		gettimeofday(&tv, NULL);
		TIMEVAL_TO_TIMESPEC(&tv, &ts);
		ts.tv_sec += milliseconds / 1000;
		ts.tv_nsec += (milliseconds % 1000) * 1000000;
		if (ts.tv_nsec >= 1000000000L) {
			ts.tv_sec++;
			ts.tv_nsec -= 1000000000L;
		}

		res = cond_timedwait(dev, &dev->condition, &dev->mutex, &ts);
		if (res == 0) {
			bytes_read = return_data(dev, data, length);
		} else if (res == ETIMEDOUT) {
			bytes_read = 0;
		} else {
			register_device_error(dev, "hid_read_timeout:  error waiting for more data");
			bytes_read = -1;
		}
	}
	else {
		/* Purely non-blocking */
		bytes_read = 0;
	}

ret:
	/* Unlock */
	pthread_mutex_unlock(&dev->mutex);
	return bytes_read;
}

int HID_API_EXPORT hid_read(hid_device *dev, unsigned char *data, size_t length)
{
	return hid_read_timeout(dev, data, length, (dev->blocking)? -1: 0);
}

int HID_API_EXPORT hid_set_nonblocking(hid_device *dev, int nonblock)
{
	/* All Nonblocking operation is handled by the library. */
	dev->blocking = !nonblock;

	return 0;
}

int HID_API_EXPORT hid_send_feature_report(hid_device *dev, const unsigned char *data, size_t length)
{
	return set_report(dev, kIOHIDReportTypeFeature, data, length);
}

int HID_API_EXPORT hid_get_feature_report(hid_device *dev, unsigned char *data, size_t length)
{
	return get_report(dev, kIOHIDReportTypeFeature, data, length);
}

int HID_API_EXPORT hid_send_output_report(hid_device *dev, const unsigned char *data, size_t length)
{
	return set_report(dev, kIOHIDReportTypeOutput, data, length);
}

int HID_API_EXPORT HID_API_CALL hid_get_input_report(hid_device *dev, unsigned char *data, size_t length)
{
	return get_report(dev, kIOHIDReportTypeInput, data, length);
}

void HID_API_EXPORT hid_close(hid_device *dev)
{
	if (!dev)
		return;

	/* Disconnect the report callback before close.
	   See comment below.
	*/
	if (is_macos_10_10_or_greater || !dev->disconnected) {
		IOHIDDeviceRegisterInputReportCallback(
			dev->device_handle, dev->input_report_buf, dev->max_input_report_len,
			NULL, dev);
		IOHIDDeviceRegisterRemovalCallback(dev->device_handle, NULL, dev);
		IOHIDDeviceUnscheduleFromRunLoop(dev->device_handle, dev->run_loop, dev->run_loop_mode);
		IOHIDDeviceScheduleWithRunLoop(dev->device_handle, CFRunLoopGetMain(), kCFRunLoopDefaultMode);
	}

	/* Cause read_thread() to stop. */
	dev->shutdown_thread = 1;

	/* Wake up the run thread's event loop so that the thread can exit. */
	CFRunLoopSourceSignal(dev->source);
	CFRunLoopWakeUp(dev->run_loop);

	/* Notify the read thread that it can shut down now. */
	pthread_barrier_wait(&dev->shutdown_barrier);

	/* Wait for read_thread() to end. */
	pthread_join(dev->thread, NULL);

	/* Close the OS handle to the device, but only if it's not
	   been unplugged. If it's been unplugged, then calling
	   IOHIDDeviceClose() will crash.

	   UPD: The crash part was true in/until some version of macOS.
	   Starting with macOS 10.15, there is an opposite effect in some environments:
	   crash happenes if IOHIDDeviceClose() is not called.
	   Not leaking a resource in all tested environments.
	*/
	if (is_macos_10_10_or_greater || !dev->disconnected) {
		IOHIDDeviceClose(dev->device_handle, dev->open_options);
	}

	/* Clear out the queue of received reports. */
	pthread_mutex_lock(&dev->mutex);
	while (dev->input_reports) {
		return_data(dev, NULL, 0);
	}
	pthread_mutex_unlock(&dev->mutex);
	CFRelease(dev->device_handle);

	free_hid_device(dev);
}

int HID_API_EXPORT_CALL hid_get_manufacturer_string(hid_device *dev, wchar_t *string, size_t maxlen)
{
	if (!string || !maxlen)
	{
		register_device_error(dev, "Zero buffer/length");
		return -1;
	}

	struct hid_device_info *info = hid_get_device_info(dev);
	if (!info)
	{
		// hid_get_device_info will have set an error already
		return -1;
	}

	wcsncpy(string, info->manufacturer_string, maxlen);
	string[maxlen - 1] = L'\0';

	return 0;
}

int HID_API_EXPORT_CALL hid_get_product_string(hid_device *dev, wchar_t *string, size_t maxlen)
{
	if (!string || !maxlen) {
		register_device_error(dev, "Zero buffer/length");
		return -1;
	}

	struct hid_device_info *info = hid_get_device_info(dev);
	if (!info) {
		// hid_get_device_info will have set an error already
		return -1;
	}

	wcsncpy(string, info->product_string, maxlen);
	string[maxlen - 1] = L'\0';

	return 0;
}

int HID_API_EXPORT_CALL hid_get_serial_number_string(hid_device *dev, wchar_t *string, size_t maxlen)
{
	if (!string || !maxlen) {
		register_device_error(dev, "Zero buffer/length");
		return -1;
	}

	struct hid_device_info *info = hid_get_device_info(dev);
	if (!info) {
		// hid_get_device_info will have set an error already
		return -1;
	}

	wcsncpy(string, info->serial_number, maxlen);
	string[maxlen - 1] = L'\0';

	return 0;
}

HID_API_EXPORT struct hid_device_info *HID_API_CALL hid_get_device_info(hid_device *dev) {
	if (!dev->device_info) {
		dev->device_info = create_device_info(dev->device_handle);
		if (!dev->device_info) {
			register_device_error(dev, "Failed to create hid_device_info");
		}
	}

	return dev->device_info;
}

int HID_API_EXPORT_CALL hid_get_indexed_string(hid_device *dev, int string_index, wchar_t *string, size_t maxlen)
{
	(void) dev;
	(void) string_index;
	(void) string;
	(void) maxlen;

	register_device_error(dev, "hid_get_indexed_string: not available on this platform");
	return -1;
}

int HID_API_EXPORT_CALL hid_darwin_get_location_id(hid_device *dev, uint32_t *location_id)
{
	int res = get_int_property(dev->device_handle, CFSTR(kIOHIDLocationIDKey));
	if (res != 0) {
		*location_id = (uint32_t) res;
		return 0;
	} else {
		register_device_error(dev, "Failed to get IOHIDLocationID property");
		return -1;
	}
}

void HID_API_EXPORT_CALL hid_darwin_set_open_exclusive(int open_exclusive)
{
	device_open_options = (open_exclusive == 0) ? kIOHIDOptionsTypeNone : kIOHIDOptionsTypeSeizeDevice;
}

int HID_API_EXPORT_CALL hid_darwin_get_open_exclusive(void)
{
	return (device_open_options == kIOHIDOptionsTypeSeizeDevice) ? 1 : 0;
}

int HID_API_EXPORT_CALL hid_darwin_is_device_open_exclusive(hid_device *dev)
{
	if (!dev)
		return -1;

	return (dev->open_options == kIOHIDOptionsTypeSeizeDevice) ? 1 : 0;
}

int HID_API_EXPORT_CALL hid_get_report_descriptor(hid_device *dev, unsigned char *buf, size_t buf_size)
{
	CFTypeRef ref = IOHIDDeviceGetProperty(dev->device_handle, CFSTR(kIOHIDReportDescriptorKey));
	if (ref != NULL && CFGetTypeID(ref) == CFDataGetTypeID()) {
		CFDataRef report_descriptor = (CFDataRef) ref;
		const UInt8 *descriptor_buf = CFDataGetBytePtr(report_descriptor);
		CFIndex descriptor_buf_len = CFDataGetLength(report_descriptor);
		size_t copy_len = (size_t) descriptor_buf_len;

		if (descriptor_buf == NULL || descriptor_buf_len < 0) {
			register_device_error(dev, "Zero buffer/length");
			return -1;
		}

		if (buf_size < copy_len) {
			copy_len = buf_size;
		}

		memcpy(buf, descriptor_buf, copy_len);
		return copy_len;
	}
	else {
		register_device_error(dev, "Failed to get kIOHIDReportDescriptorKey property");
		return -1;
	}
}

HID_API_EXPORT const wchar_t * HID_API_CALL  hid_error(hid_device *dev)
{
	if (dev) {
		if (dev->last_error_str == NULL)
			return L"Success";
		return dev->last_error_str;
	}

	if (last_global_error_str == NULL)
		return L"Success";
	return last_global_error_str;
}

#elif defined(__linux__)

/*******************************************************
 HIDAPI - Multi-Platform library for
 communication with HID devices.

 Alan Ott
 Signal 11 Software

 libusb/hidapi Team

 Copyright 2022, All Rights Reserved.

 At the discretion of the user of this library,
 this software may be licensed under the terms of the
 GNU General Public License v3, a BSD-Style license, or the
 original HIDAPI license as outlined in the LICENSE.txt,
 LICENSE-gpl3.txt, LICENSE-bsd.txt, and LICENSE-orig.txt
 files located at the root of the source distribution.
 These files may also be found in the public source
 code repository located at:
        https://github.com/libusb/hidapi .
********************************************************/

/* C */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <locale.h>
#include <errno.h>

/* Unix */
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/utsname.h>
#include <fcntl.h>
#include <poll.h>

/* Linux */
#include <linux/hidraw.h>
#include <linux/version.h>
#include <linux/input.h>
#include <libudev.h>

#include "hidapi.h"

#ifdef HIDAPI_ALLOW_BUILD_WORKAROUND_KERNEL_2_6_39
/* This definitions first appeared in Linux Kernel 2.6.39 in linux/hidraw.h.
    hidapi doesn't support kernels older than that,
    so we don't define macros below explicitly, to fail builds on old kernels.
    For those who really need this as a workaround (e.g. to be able to build on old build machines),
    can workaround by defining the macro above.
*/
#ifndef HIDIOCSFEATURE
#define HIDIOCSFEATURE(len)    _IOC(_IOC_WRITE|_IOC_READ, 'H', 0x06, len)
#endif
#ifndef HIDIOCGFEATURE
#define HIDIOCGFEATURE(len)    _IOC(_IOC_WRITE|_IOC_READ, 'H', 0x07, len)
#endif

#endif


// HIDIOCGINPUT and HIDIOCSOUTPUT are not defined in Linux kernel headers < 5.11.
// These definitions are from hidraw.h in Linux >= 5.11.
// https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/commit/?id=f43d3870cafa2a0f3854c1819c8385733db8f9ae
#ifndef HIDIOCGINPUT
#define HIDIOCGINPUT(len)    _IOC(_IOC_WRITE|_IOC_READ, 'H', 0x0A, len)
#endif
#ifndef HIDIOCSOUTPUT
#define HIDIOCSOUTPUT(len)   _IOC(_IOC_WRITE|_IOC_READ, 'H', 0x0B, len)
#endif

struct hid_device_ {
	int device_handle;
	int blocking;
	wchar_t *last_error_str;
	struct hid_device_info* device_info;
};

static struct hid_api_version api_version = {
	.major = HID_API_VERSION_MAJOR,
	.minor = HID_API_VERSION_MINOR,
	.patch = HID_API_VERSION_PATCH
};

static wchar_t *last_global_error_str = NULL;


static hid_device *new_hid_device(void)
{
	hid_device *dev = (hid_device*) calloc(1, sizeof(hid_device));
	if (dev == NULL) {
		return NULL;
	}

	dev->device_handle = -1;
	dev->blocking = 1;
	dev->last_error_str = NULL;
	dev->device_info = NULL;

	return dev;
}


/* The caller must free the returned string with free(). */
static wchar_t *utf8_to_wchar_t(const char *utf8)
{
	wchar_t *ret = NULL;

	if (utf8) {
		size_t wlen = mbstowcs(NULL, utf8, 0);
		if ((size_t) -1 == wlen) {
			return wcsdup(L"");
		}
		ret = (wchar_t*) calloc(wlen+1, sizeof(wchar_t));
		if (ret == NULL) {
			/* as much as we can do at this point */
			return NULL;
		}
		mbstowcs(ret, utf8, wlen+1);
		ret[wlen] = 0x0000;
	}

	return ret;
}


/* Makes a copy of the given error message (and decoded according to the
 * currently locale) into the wide string pointer pointed by error_str.
 * The last stored error string is freed.
 * Use register_error_str(NULL) to free the error message completely. */
static void register_error_str(wchar_t **error_str, const char *msg)
{
	free(*error_str);
	*error_str = utf8_to_wchar_t(msg);
}

/* Semilar to register_error_str, but allows passing a format string with va_list args into this function. */
static void register_error_str_vformat(wchar_t **error_str, const char *format, va_list args)
{
	char msg[256];
	vsnprintf(msg, sizeof(msg), format, args);

	register_error_str(error_str, msg);
}

/* Set the last global error to be reported by hid_error(NULL).
 * The given error message will be copied (and decoded according to the
 * currently locale, so do not pass in string constants).
 * The last stored global error message is freed.
 * Use register_global_error(NULL) to indicate "no error". */
static void register_global_error(const char *msg)
{
	register_error_str(&last_global_error_str, msg);
}

/* Similar to register_global_error, but allows passing a format string into this function. */
static void register_global_error_format(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	register_error_str_vformat(&last_global_error_str, format, args);
	va_end(args);
}

/* Set the last error for a device to be reported by hid_error(dev).
 * The given error message will be copied (and decoded according to the
 * currently locale, so do not pass in string constants).
 * The last stored device error message is freed.
 * Use register_device_error(dev, NULL) to indicate "no error". */
static void register_device_error(hid_device *dev, const char *msg)
{
	register_error_str(&dev->last_error_str, msg);
}

/* Similar to register_device_error, but you can pass a format string into this function. */
static void register_device_error_format(hid_device *dev, const char *format, ...)
{
	va_list args;
	va_start(args, format);
	register_error_str_vformat(&dev->last_error_str, format, args);
	va_end(args);
}

/* Get an attribute value from a udev_device and return it as a whar_t
   string. The returned string must be freed with free() when done.*/
static wchar_t *copy_udev_string(struct udev_device *dev, const char *udev_name)
{
	return utf8_to_wchar_t(udev_device_get_sysattr_value(dev, udev_name));
}

/*
 * Gets the size of the HID item at the given position
 * Returns 1 if successful, 0 if an invalid key
 * Sets data_len and key_size when successful
 */
static int get_hid_item_size(const __u8 *report_descriptor, __u32 size, unsigned int pos, int *data_len, int *key_size)
{
	int key = report_descriptor[pos];
	int size_code;

	/*
	 * This is a Long Item. The next byte contains the
	 * length of the data section (value) for this key.
	 * See the HID specification, version 1.11, section
	 * 6.2.2.3, titled "Long Items."
	 */
	if ((key & 0xf0) == 0xf0) {
		if (pos + 1 < size)
		{
			*data_len = report_descriptor[pos + 1];
			*key_size = 3;
			return 1;
		}
		*data_len = 0; /* malformed report */
		*key_size = 0;
	}

	/*
	 * This is a Short Item. The bottom two bits of the
	 * key contain the size code for the data section
	 * (value) for this key. Refer to the HID
	 * specification, version 1.11, section 6.2.2.2,
	 * titled "Short Items."
	 */
	size_code = key & 0x3;
	switch (size_code) {
	case 0:
	case 1:
	case 2:
		*data_len = size_code;
		*key_size = 1;
		return 1;
	case 3:
		*data_len = 4;
		*key_size = 1;
		return 1;
	default:
		/* Can't ever happen since size_code is & 0x3 */
		*data_len = 0;
		*key_size = 0;
		break;
	};

	/* malformed report */
	return 0;
}

/*
 * Get bytes from a HID Report Descriptor.
 * Only call with a num_bytes of 0, 1, 2, or 4.
 */
static __u32 get_hid_report_bytes(const __u8 *rpt, size_t len, size_t num_bytes, size_t cur)
{
	/* Return if there aren't enough bytes. */
	if (cur + num_bytes >= len)
		return 0;

	if (num_bytes == 0)
		return 0;
	else if (num_bytes == 1)
		return rpt[cur + 1];
	else if (num_bytes == 2)
		return (rpt[cur + 2] * 256 + rpt[cur + 1]);
	else if (num_bytes == 4)
		return (
			rpt[cur + 4] * 0x01000000 +
			rpt[cur + 3] * 0x00010000 +
			rpt[cur + 2] * 0x00000100 +
			rpt[cur + 1] * 0x00000001
		);
	else
		return 0;
}

/*
 * Iterates until the end of a Collection.
 * Assumes that *pos is exactly at the beginning of a Collection.
 * Skips all nested Collection, i.e. iterates until the end of current level Collection.
 *
 * The return value is non-0 when an end of current Collection is found,
 * 0 when error is occured (broken Descriptor, end of a Collection is found before its begin,
 *  or no Collection is found at all).
 */
static int hid_iterate_over_collection(const __u8 *report_descriptor, __u32 size, unsigned int *pos, int *data_len, int *key_size)
{
	int collection_level = 0;

	while (*pos < size) {
		int key = report_descriptor[*pos];
		int key_cmd = key & 0xfc;

		/* Determine data_len and key_size */
		if (!get_hid_item_size(report_descriptor, size, *pos, data_len, key_size))
			return 0; /* malformed report */

		switch (key_cmd) {
		case 0xa0: /* Collection 6.2.2.4 (Main) */
			collection_level++;
			break;
		case 0xc0: /* End Collection 6.2.2.4 (Main) */
			collection_level--;
			break;
		}

		if (collection_level < 0) {
			/* Broken descriptor or someone is using this function wrong,
			 * i.e. should be called exactly at the collection start */
			return 0;
		}

		if (collection_level == 0) {
			/* Found it!
			 * Also possible when called not at the collection start, but should not happen if used correctly */
			return 1;
		}

		*pos += *data_len + *key_size;
	}

	return 0; /* Did not find the end of a Collection */
}

struct hid_usage_iterator {
	unsigned int pos;
	int usage_page_found;
	unsigned short usage_page;
};

/*
 * Retrieves the device's Usage Page and Usage from the report descriptor.
 * The algorithm returns the current Usage Page/Usage pair whenever a new
 * Collection is found and a Usage Local Item is currently in scope.
 * Usage Local Items are consumed by each Main Item (See. 6.2.2.8).
 * The algorithm should give similar results as Apple's:
 *   https://developer.apple.com/documentation/iokit/kiohiddeviceusagepairskey?language=objc
 * Physical Collections are also matched (macOS does the same).
 *
 * This function can be called repeatedly until it returns non-0
 * Usage is found. pos is the starting point (initially 0) and will be updated
 * to the next search position.
 *
 * The return value is 0 when a pair is found.
 * 1 when finished processing descriptor.
 * -1 on a malformed report.
 */
static int get_next_hid_usage(const __u8 *report_descriptor, __u32 size, struct hid_usage_iterator *ctx, unsigned short *usage_page, unsigned short *usage)
{
	int data_len, key_size;
	int initial = ctx->pos == 0; /* Used to handle case where no top-level application collection is defined */

	int usage_found = 0;

	while (ctx->pos < size) {
		int key = report_descriptor[ctx->pos];
		int key_cmd = key & 0xfc;

		/* Determine data_len and key_size */
		if (!get_hid_item_size(report_descriptor, size, ctx->pos, &data_len, &key_size))
			return -1; /* malformed report */

		switch (key_cmd) {
		case 0x4: /* Usage Page 6.2.2.7 (Global) */
			ctx->usage_page = get_hid_report_bytes(report_descriptor, size, data_len, ctx->pos);
			ctx->usage_page_found = 1;
			break;

		case 0x8: /* Usage 6.2.2.8 (Local) */
			if (data_len == 4) { /* Usages 5.5 / Usage Page 6.2.2.7 */
				ctx->usage_page = get_hid_report_bytes(report_descriptor, size, 2, ctx->pos + 2);
				ctx->usage_page_found = 1;
				*usage = get_hid_report_bytes(report_descriptor, size, 2, ctx->pos);
				usage_found = 1;
			}
			else {
				*usage = get_hid_report_bytes(report_descriptor, size, data_len, ctx->pos);
				usage_found = 1;
			}
			break;

		case 0xa0: /* Collection 6.2.2.4 (Main) */
			if (!hid_iterate_over_collection(report_descriptor, size, &ctx->pos, &data_len, &key_size)) {
				return -1;
			}

			/* A pair is valid - to be reported when Collection is found */
			if (usage_found && ctx->usage_page_found) {
				*usage_page = ctx->usage_page;
				return 0;
			}

			break;
		}

		/* Skip over this key and its associated data */
		ctx->pos += data_len + key_size;
	}

	/* If no top-level application collection is found and usage page/usage pair is found, pair is valid
	   https://docs.microsoft.com/en-us/windows-hardware/drivers/hid/top-level-collections */
	if (initial && usage_found && ctx->usage_page_found) {
			*usage_page = ctx->usage_page;
			return 0; /* success */
	}

	return 1; /* finished processing */
}

/*
 * Retrieves the hidraw report descriptor from a file.
 * When using this form, <sysfs_path>/device/report_descriptor, elevated privileges are not required.
 */
static int get_hid_report_descriptor(const char *rpt_path, struct hidraw_report_descriptor *rpt_desc)
{
	int rpt_handle;
	ssize_t res;

	rpt_handle = open(rpt_path, O_RDONLY | O_CLOEXEC);
	if (rpt_handle < 0) {
		register_global_error_format("open failed (%s): %s", rpt_path, strerror(errno));
		return -1;
	}

	/*
	 * Read in the Report Descriptor
	 * The sysfs file has a maximum size of 4096 (which is the same as HID_MAX_DESCRIPTOR_SIZE) so we should always
	 * be ok when reading the descriptor.
	 * In practice if the HID descriptor is any larger I suspect many other things will break.
	 */
	memset(rpt_desc, 0x0, sizeof(*rpt_desc));
	res = read(rpt_handle, rpt_desc->value, HID_MAX_DESCRIPTOR_SIZE);
	if (res < 0) {
		register_global_error_format("read failed (%s): %s", rpt_path, strerror(errno));
	}
	rpt_desc->size = (__u32) res;

	close(rpt_handle);
	return (int) res;
}

/* return size of the descriptor, or -1 on failure */
static int get_hid_report_descriptor_from_sysfs(const char *sysfs_path, struct hidraw_report_descriptor *rpt_desc)
{
	int res = -1;
	/* Construct <sysfs_path>/device/report_descriptor */
	size_t rpt_path_len = strlen(sysfs_path) + 25 + 1;
	char* rpt_path = (char*) calloc(1, rpt_path_len);
	snprintf(rpt_path, rpt_path_len, "%s/device/report_descriptor", sysfs_path);

	res = get_hid_report_descriptor(rpt_path, rpt_desc);
	free(rpt_path);

	return res;
}

/* return non-zero if successfully parsed */
static int parse_hid_vid_pid_from_uevent(const char *uevent, unsigned *bus_type, unsigned short *vendor_id, unsigned short *product_id)
{
	char tmp[1024];
	size_t uevent_len = strlen(uevent);
	if (uevent_len > sizeof(tmp) - 1)
		uevent_len = sizeof(tmp) - 1;
	memcpy(tmp, uevent, uevent_len);
	tmp[uevent_len] = '\0';

	char *saveptr = NULL;
	char *line;
	char *key;
	char *value;

	line = strtok_r(tmp, "\n", &saveptr);
	while (line != NULL) {
		/* line: "KEY=value" */
		key = line;
		value = strchr(line, '=');
		if (!value) {
			goto next_line;
		}
		*value = '\0';
		value++;

		if (strcmp(key, "HID_ID") == 0) {
			/**
			 *        type vendor   product
			 * HID_ID=0003:000005AC:00008242
			 **/
			int ret = sscanf(value, "%x:%hx:%hx", bus_type, vendor_id, product_id);
			if (ret == 3) {
				return 1;
			}
		}

next_line:
		line = strtok_r(NULL, "\n", &saveptr);
	}

	register_global_error("Couldn't find/parse HID_ID");
	return 0;
}

/* return non-zero if successfully parsed */
static int parse_hid_vid_pid_from_uevent_path(const char *uevent_path, unsigned *bus_type, unsigned short *vendor_id, unsigned short *product_id)
{
	int handle;
	ssize_t res;

	handle = open(uevent_path, O_RDONLY | O_CLOEXEC);
	if (handle < 0) {
		register_global_error_format("open failed (%s): %s", uevent_path, strerror(errno));
		return 0;
	}

	char buf[1024];
	res = read(handle, buf, sizeof(buf) - 1); /* -1 for '\0' at the end */
	close(handle);

	if (res < 0) {
		register_global_error_format("read failed (%s): %s", uevent_path, strerror(errno));
		return 0;
	}

	buf[res] = '\0';
	return parse_hid_vid_pid_from_uevent(buf, bus_type, vendor_id, product_id);
}

/* return non-zero if successfully read/parsed */
static int parse_hid_vid_pid_from_sysfs(const char *sysfs_path, unsigned *bus_type, unsigned short *vendor_id, unsigned short *product_id)
{
	int res = 0;
	/* Construct <sysfs_path>/device/uevent */
	size_t uevent_path_len = strlen(sysfs_path) + 14 + 1;
	char* uevent_path = (char*) calloc(1, uevent_path_len);
	snprintf(uevent_path, uevent_path_len, "%s/device/uevent", sysfs_path);

	res = parse_hid_vid_pid_from_uevent_path(uevent_path, bus_type, vendor_id, product_id);
	free(uevent_path);

	return res;
}

static int get_hid_report_descriptor_from_hidraw(hid_device *dev, struct hidraw_report_descriptor *rpt_desc)
{
	int desc_size = 0;

	/* Get Report Descriptor Size */
	int res = ioctl(dev->device_handle, HIDIOCGRDESCSIZE, &desc_size);
	if (res < 0) {
		register_device_error_format(dev, "ioctl(GRDESCSIZE): %s", strerror(errno));
		return res;
	}

	/* Get Report Descriptor */
	memset(rpt_desc, 0x0, sizeof(*rpt_desc));
	rpt_desc->size = desc_size;
	res = ioctl(dev->device_handle, HIDIOCGRDESC, rpt_desc);
	if (res < 0) {
		register_device_error_format(dev, "ioctl(GRDESC): %s", strerror(errno));
	}

	return res;
}

/*
 * The caller is responsible for free()ing the (newly-allocated) character
 * strings pointed to by serial_number_utf8 and product_name_utf8 after use.
 */
static int parse_uevent_info(const char *uevent, unsigned *bus_type,
	unsigned short *vendor_id, unsigned short *product_id,
	char **serial_number_utf8, char **product_name_utf8)
{
	char tmp[1024];

	if (!uevent) {
		return 0;
	}

	size_t uevent_len = strlen(uevent);
	if (uevent_len > sizeof(tmp) - 1)
		uevent_len = sizeof(tmp) - 1;
	memcpy(tmp, uevent, uevent_len);
	tmp[uevent_len] = '\0';

	char *saveptr = NULL;
	char *line;
	char *key;
	char *value;

	int found_id = 0;
	int found_serial = 0;
	int found_name = 0;

	line = strtok_r(tmp, "\n", &saveptr);
	while (line != NULL) {
		/* line: "KEY=value" */
		key = line;
		value = strchr(line, '=');
		if (!value) {
			goto next_line;
		}
		*value = '\0';
		value++;

		if (strcmp(key, "HID_ID") == 0) {
			/**
			 *        type vendor   product
			 * HID_ID=0003:000005AC:00008242
			 **/
			int ret = sscanf(value, "%x:%hx:%hx", bus_type, vendor_id, product_id);
			if (ret == 3) {
				found_id = 1;
			}
		} else if (strcmp(key, "HID_NAME") == 0) {
			/* The caller has to free the product name */
			*product_name_utf8 = strdup(value);
			found_name = 1;
		} else if (strcmp(key, "HID_UNIQ") == 0) {
			/* The caller has to free the serial number */
			*serial_number_utf8 = strdup(value);
			found_serial = 1;
		}

next_line:
		line = strtok_r(NULL, "\n", &saveptr);
	}

	return (found_id && found_name && found_serial);
}


static struct hid_device_info * create_device_info_for_device(struct udev_device *raw_dev)
{
	struct hid_device_info *root = NULL;
	struct hid_device_info *cur_dev = NULL;

	const char *sysfs_path;
	const char *dev_path;
	const char *str;
	struct udev_device *hid_dev; /* The device's HID udev node. */
	struct udev_device *usb_dev; /* The device's USB udev node. */
	struct udev_device *intf_dev; /* The device's interface (in the USB sense). */
	unsigned short dev_vid;
	unsigned short dev_pid;
	char *serial_number_utf8 = NULL;
	char *product_name_utf8 = NULL;
	unsigned bus_type;
	int result;
	struct hidraw_report_descriptor report_desc;

	sysfs_path = udev_device_get_syspath(raw_dev);
	dev_path = udev_device_get_devnode(raw_dev);

	hid_dev = udev_device_get_parent_with_subsystem_devtype(
		raw_dev,
		"hid",
		NULL);

	if (!hid_dev) {
		/* Unable to find parent hid device. */
		goto end;
	}

	result = parse_uevent_info(
		udev_device_get_sysattr_value(hid_dev, "uevent"),
		&bus_type,
		&dev_vid,
		&dev_pid,
		&serial_number_utf8,
		&product_name_utf8);

	if (!result) {
		/* parse_uevent_info() failed for at least one field. */
		goto end;
	}

	/* Filter out unhandled devices right away */
	switch (bus_type) {
		case BUS_BLUETOOTH:
		case BUS_I2C:
		case BUS_USB:
		case BUS_SPI:
			break;

		default:
			goto end;
	}

	/* Create the record. */
	root = (struct hid_device_info*) calloc(1, sizeof(struct hid_device_info));
	if (!root)
		goto end;

	cur_dev = root;

	/* Fill out the record */
	cur_dev->next = NULL;
	cur_dev->path = dev_path? strdup(dev_path): NULL;

	/* VID/PID */
	cur_dev->vendor_id = dev_vid;
	cur_dev->product_id = dev_pid;

	/* Serial Number */
	cur_dev->serial_number = utf8_to_wchar_t(serial_number_utf8);

	/* Release Number */
	cur_dev->release_number = 0x0;

	/* Interface Number */
	cur_dev->interface_number = -1;

	switch (bus_type) {
		case BUS_USB:
			/* The device pointed to by raw_dev contains information about
				the hidraw device. In order to get information about the
				USB device, get the parent device with the
				subsystem/devtype pair of "usb"/"usb_device". This will
				be several levels up the tree, but the function will find
				it. */
			usb_dev = udev_device_get_parent_with_subsystem_devtype(
					raw_dev,
					"usb",
					"usb_device");

			/* uhid USB devices
			 * Since this is a virtual hid interface, no USB information will
			 * be available. */
			if (!usb_dev) {
				/* Manufacturer and Product strings */
				cur_dev->manufacturer_string = wcsdup(L"");
				cur_dev->product_string = utf8_to_wchar_t(product_name_utf8);
				break;
			}

			cur_dev->manufacturer_string = copy_udev_string(usb_dev, "manufacturer");
			cur_dev->product_string = copy_udev_string(usb_dev, "product");

			cur_dev->bus_type = HID_API_BUS_USB;

			str = udev_device_get_sysattr_value(usb_dev, "bcdDevice");
			cur_dev->release_number = (str)? strtol(str, NULL, 16): 0x0;

			/* Get a handle to the interface's udev node. */
			intf_dev = udev_device_get_parent_with_subsystem_devtype(
					raw_dev,
					"usb",
					"usb_interface");
			if (intf_dev) {
				str = udev_device_get_sysattr_value(intf_dev, "bInterfaceNumber");
				cur_dev->interface_number = (str)? strtol(str, NULL, 16): -1;
			}

			break;

		case BUS_BLUETOOTH:
			cur_dev->manufacturer_string = wcsdup(L"");
			cur_dev->product_string = utf8_to_wchar_t(product_name_utf8);

			cur_dev->bus_type = HID_API_BUS_BLUETOOTH;

			break;
		case BUS_I2C:
			cur_dev->manufacturer_string = wcsdup(L"");
			cur_dev->product_string = utf8_to_wchar_t(product_name_utf8);

			cur_dev->bus_type = HID_API_BUS_I2C;

			break;

		case BUS_SPI:
			cur_dev->manufacturer_string = wcsdup(L"");
			cur_dev->product_string = utf8_to_wchar_t(product_name_utf8);

			cur_dev->bus_type = HID_API_BUS_SPI;

			break;

		default:
			/* Unknown device type - this should never happen, as we
			 * check for USB and Bluetooth devices above */
			break;
	}

	/* Usage Page and Usage */
	result = get_hid_report_descriptor_from_sysfs(sysfs_path, &report_desc);
	if (result >= 0) {
		unsigned short page = 0, usage = 0;
		struct hid_usage_iterator usage_iterator;
		memset(&usage_iterator, 0, sizeof(usage_iterator));

		/*
		 * Parse the first usage and usage page
		 * out of the report descriptor.
		 */
		if (!get_next_hid_usage(report_desc.value, report_desc.size, &usage_iterator, &page, &usage)) {
			cur_dev->usage_page = page;
			cur_dev->usage = usage;
		}

		/*
		 * Parse any additional usage and usage pages
		 * out of the report descriptor.
		 */
		while (!get_next_hid_usage(report_desc.value, report_desc.size, &usage_iterator, &page, &usage)) {
			/* Create new record for additional usage pairs */
			struct hid_device_info *tmp = (struct hid_device_info*) calloc(1, sizeof(struct hid_device_info));
			struct hid_device_info *prev_dev = cur_dev;

			if (!tmp)
				continue;
			cur_dev->next = tmp;
			cur_dev = tmp;

			/* Update fields */
			cur_dev->path = dev_path? strdup(dev_path): NULL;
			cur_dev->vendor_id = dev_vid;
			cur_dev->product_id = dev_pid;
			cur_dev->serial_number = prev_dev->serial_number? wcsdup(prev_dev->serial_number): NULL;
			cur_dev->release_number = prev_dev->release_number;
			cur_dev->interface_number = prev_dev->interface_number;
			cur_dev->manufacturer_string = prev_dev->manufacturer_string? wcsdup(prev_dev->manufacturer_string): NULL;
			cur_dev->product_string = prev_dev->product_string? wcsdup(prev_dev->product_string): NULL;
			cur_dev->usage_page = page;
			cur_dev->usage = usage;
			cur_dev->bus_type = prev_dev->bus_type;
		}
	}

end:
	free(serial_number_utf8);
	free(product_name_utf8);

	return root;
}

static struct hid_device_info * create_device_info_for_hid_device(hid_device *dev) {
	struct udev *udev;
	struct udev_device *udev_dev;
	struct stat s;
	int ret = -1;
	struct hid_device_info *root = NULL;

	register_device_error(dev, NULL);

	/* Get the dev_t (major/minor numbers) from the file handle. */
	ret = fstat(dev->device_handle, &s);
	if (-1 == ret) {
		register_device_error(dev, "Failed to stat device handle");
		return NULL;
	}

	/* Create the udev object */
	udev = udev_new();
	if (!udev) {
		register_device_error(dev, "Couldn't create udev context");
		return NULL;
	}

	/* Open a udev device from the dev_t. 'c' means character device. */
	udev_dev = udev_device_new_from_devnum(udev, 'c', s.st_rdev);
	if (udev_dev) {
		root = create_device_info_for_device(udev_dev);
	}

	if (!root) {
		/* TODO: have a better error reporting via create_device_info_for_device */
		register_device_error(dev, "Couldn't create hid_device_info");
	}

	udev_device_unref(udev_dev);
	udev_unref(udev);

	return root;
}

HID_API_EXPORT const struct hid_api_version* HID_API_CALL hid_version(void)
{
	return &api_version;
}

HID_API_EXPORT const char* HID_API_CALL hid_version_str(void)
{
	return HID_API_VERSION_STR;
}

int HID_API_EXPORT hid_init(void)
{
	const char *locale;

	/* indicate no error */
	register_global_error(NULL);

	/* Set the locale if it's not set. */
	locale = setlocale(LC_CTYPE, NULL);
	if (!locale)
		setlocale(LC_CTYPE, "");

	return 0;
}

int HID_API_EXPORT hid_exit(void)
{
	/* Free global error message */
	register_global_error(NULL);

	return 0;
}

struct hid_device_info  HID_API_EXPORT *hid_enumerate(unsigned short vendor_id, unsigned short product_id)
{
	struct udev *udev;
	struct udev_enumerate *enumerate;
	struct udev_list_entry *devices, *dev_list_entry;

	struct hid_device_info *root = NULL; /* return object */
	struct hid_device_info *cur_dev = NULL;

	hid_init();
	/* register_global_error: global error is reset by hid_init */

	/* Create the udev object */
	udev = udev_new();
	if (!udev) {
		register_global_error("Couldn't create udev context");
		return NULL;
	}

	/* Create a list of the devices in the 'hidraw' subsystem. */
	enumerate = udev_enumerate_new(udev);
	udev_enumerate_add_match_subsystem(enumerate, "hidraw");
	udev_enumerate_scan_devices(enumerate);
	devices = udev_enumerate_get_list_entry(enumerate);
	/* For each item, see if it matches the vid/pid, and if so
	   create a udev_device record for it */
	udev_list_entry_foreach(dev_list_entry, devices) {
		const char *sysfs_path;
		unsigned short dev_vid = 0;
		unsigned short dev_pid = 0;
		unsigned bus_type = 0;
		struct udev_device *raw_dev; /* The device's hidraw udev node. */
		struct hid_device_info * tmp;

		/* Get the filename of the /sys entry for the device
		   and create a udev_device object (dev) representing it */
		sysfs_path = udev_list_entry_get_name(dev_list_entry);
		if (!sysfs_path)
			continue;

		if (vendor_id != 0 || product_id != 0) {
			if (!parse_hid_vid_pid_from_sysfs(sysfs_path, &bus_type, &dev_vid, &dev_pid))
				continue;

			if (vendor_id != 0 && vendor_id != dev_vid)
				continue;
			if (product_id != 0 && product_id != dev_pid)
				continue;
		}

		raw_dev = udev_device_new_from_syspath(udev, sysfs_path);
		if (!raw_dev)
			continue;

		tmp = create_device_info_for_device(raw_dev);
		if (tmp) {
			if (cur_dev) {
				cur_dev->next = tmp;
			}
			else {
				root = tmp;
			}
			cur_dev = tmp;

			/* move the pointer to the tail of returned list */
			while (cur_dev->next != NULL) {
				cur_dev = cur_dev->next;
			}
		}

		udev_device_unref(raw_dev);
	}
	/* Free the enumerator and udev objects. */
	udev_enumerate_unref(enumerate);
	udev_unref(udev);

	if (root == NULL) {
		if (vendor_id == 0 && product_id == 0) {
			register_global_error("No HID devices found in the system.");
		} else {
			register_global_error("No HID devices with requested VID/PID found in the system.");
		}
	}

	return root;
}

void  HID_API_EXPORT hid_free_enumeration(struct hid_device_info *devs)
{
	struct hid_device_info *d = devs;
	while (d) {
		struct hid_device_info *next = d->next;
		free(d->path);
		free(d->serial_number);
		free(d->manufacturer_string);
		free(d->product_string);
		free(d);
		d = next;
	}
}

hid_device * hid_open(unsigned short vendor_id, unsigned short product_id, const wchar_t *serial_number)
{
	struct hid_device_info *devs, *cur_dev;
	const char *path_to_open = NULL;
	hid_device *handle = NULL;

	/* register_global_error: global error is reset by hid_enumerate/hid_init */
	devs = hid_enumerate(vendor_id, product_id);
	if (devs == NULL) {
		/* register_global_error: global error is already set by hid_enumerate */
		return NULL;
	}

	cur_dev = devs;
	while (cur_dev) {
		if (cur_dev->vendor_id == vendor_id &&
		    cur_dev->product_id == product_id) {
			if (serial_number) {
				if (wcscmp(serial_number, cur_dev->serial_number) == 0) {
					path_to_open = cur_dev->path;
					break;
				}
			}
			else {
				path_to_open = cur_dev->path;
				break;
			}
		}
		cur_dev = cur_dev->next;
	}

	if (path_to_open) {
		/* Open the device */
		handle = hid_open_path(path_to_open);
	} else {
		register_global_error("Device with requested VID/PID/(SerialNumber) not found");
	}

	hid_free_enumeration(devs);

	return handle;
}

hid_device * HID_API_EXPORT hid_open_path(const char *path)
{
	hid_device *dev = NULL;

	hid_init();
	/* register_global_error: global error is reset by hid_init */

	dev = new_hid_device();
	if (!dev) {
		register_global_error("Couldn't allocate memory");
		return NULL;
	}

	dev->device_handle = open(path, O_RDWR | O_CLOEXEC);

	if (dev->device_handle >= 0) {
		int res, desc_size = 0;

		/* Make sure this is a HIDRAW device - responds to HIDIOCGRDESCSIZE */
		res = ioctl(dev->device_handle, HIDIOCGRDESCSIZE, &desc_size);
		if (res < 0) {
			hid_close(dev);
			register_global_error_format("ioctl(GRDESCSIZE) error for '%s', not a HIDRAW device?: %s", path, strerror(errno));
			return NULL;
		}

		return dev;
	}
	else {
		/* Unable to open a device. */
		free(dev);
		register_global_error_format("Failed to open a device with path '%s': %s", path, strerror(errno));
		return NULL;
	}
}


int HID_API_EXPORT hid_write(hid_device *dev, const unsigned char *data, size_t length)
{
	int bytes_written;

	if (!data || (length == 0)) {
		errno = EINVAL;
		register_device_error(dev, strerror(errno));
		return -1;
	}

	bytes_written = write(dev->device_handle, data, length);

	register_device_error(dev, (bytes_written == -1)? strerror(errno): NULL);

	return bytes_written;
}


int HID_API_EXPORT hid_read_timeout(hid_device *dev, unsigned char *data, size_t length, int milliseconds)
{
	/* Set device error to none */
	register_device_error(dev, NULL);

	int bytes_read;

	if (milliseconds >= 0) {
		/* Milliseconds is either 0 (non-blocking) or > 0 (contains
		   a valid timeout). In both cases we want to call poll()
		   and wait for data to arrive.  Don't rely on non-blocking
		   operation (O_NONBLOCK) since some kernels don't seem to
		   properly report device disconnection through read() when
		   in non-blocking mode.  */
		int ret;
		struct pollfd fds;

		fds.fd = dev->device_handle;
		fds.events = POLLIN;
		fds.revents = 0;
		ret = poll(&fds, 1, milliseconds);
		if (ret == 0) {
			/* Timeout */
			return ret;
		}
		if (ret == -1) {
			/* Error */
			register_device_error(dev, strerror(errno));
			return ret;
		}
		else {
			/* Check for errors on the file descriptor. This will
			   indicate a device disconnection. */
			if (fds.revents & (POLLERR | POLLHUP | POLLNVAL)) {
				// We cannot use strerror() here as no -1 was returned from poll().
				register_device_error(dev, "hid_read_timeout: unexpected poll error (device disconnected)");
				return -1;
			}
		}
	}

	bytes_read = read(dev->device_handle, data, length);
	if (bytes_read < 0) {
		if (errno == EAGAIN || errno == EINPROGRESS)
			bytes_read = 0;
		else
			register_device_error(dev, strerror(errno));
	}

	return bytes_read;
}

int HID_API_EXPORT hid_read(hid_device *dev, unsigned char *data, size_t length)
{
	return hid_read_timeout(dev, data, length, (dev->blocking)? -1: 0);
}

int HID_API_EXPORT hid_set_nonblocking(hid_device *dev, int nonblock)
{
	/* Do all non-blocking in userspace using poll(), since it looks
	   like there's a bug in the kernel in some versions where
	   read() will not return -1 on disconnection of the USB device */

	dev->blocking = !nonblock;
	return 0; /* Success */
}


int HID_API_EXPORT hid_send_feature_report(hid_device *dev, const unsigned char *data, size_t length)
{
	int res;

	register_device_error(dev, NULL);

	res = ioctl(dev->device_handle, HIDIOCSFEATURE(length), data);
	if (res < 0)
		register_device_error_format(dev, "ioctl (SFEATURE): %s", strerror(errno));

	return res;
}

int HID_API_EXPORT hid_get_feature_report(hid_device *dev, unsigned char *data, size_t length)
{
	int res;

	register_device_error(dev, NULL);

	res = ioctl(dev->device_handle, HIDIOCGFEATURE(length), data);
	if (res < 0)
		register_device_error_format(dev, "ioctl (GFEATURE): %s", strerror(errno));

	return res;
}

int HID_API_EXPORT HID_API_CALL hid_send_output_report(hid_device *dev, const unsigned char *data, size_t length)
{
	int res;

	register_device_error(dev, NULL);

	res = ioctl(dev->device_handle, HIDIOCSOUTPUT(length), data);
	if (res < 0)
		register_device_error_format(dev, "ioctl (SOUTPUT): %s", strerror(errno));

	return res;
}

int HID_API_EXPORT HID_API_CALL hid_get_input_report(hid_device *dev, unsigned char *data, size_t length)
{
	int res;

	register_device_error(dev, NULL);

	res = ioctl(dev->device_handle, HIDIOCGINPUT(length), data);
	if (res < 0)
		register_device_error_format(dev, "ioctl (GINPUT): %s", strerror(errno));

	return res;
}

void HID_API_EXPORT hid_close(hid_device *dev)
{
	if (!dev)
		return;

	close(dev->device_handle);

	/* Free the device error message */
	register_device_error(dev, NULL);

	hid_free_enumeration(dev->device_info);

	free(dev);
}


int HID_API_EXPORT_CALL hid_get_manufacturer_string(hid_device *dev, wchar_t *string, size_t maxlen)
{
	if (!string || !maxlen) {
		register_device_error(dev, "Zero buffer/length");
		return -1;
	}

	struct hid_device_info *info = hid_get_device_info(dev);
	if (!info) {
		// hid_get_device_info will have set an error already
		return -1;
	}

	if (info->manufacturer_string) {
		wcsncpy(string, info->manufacturer_string, maxlen);
		string[maxlen - 1] = L'\0';
	}
	else {
		string[0] = L'\0';
	}

	return 0;
}

int HID_API_EXPORT_CALL hid_get_product_string(hid_device *dev, wchar_t *string, size_t maxlen)
{
	if (!string || !maxlen) {
		register_device_error(dev, "Zero buffer/length");
		return -1;
	}

	struct hid_device_info *info = hid_get_device_info(dev);
	if (!info) {
		// hid_get_device_info will have set an error already
		return -1;
	}

	if (info->product_string) {
		wcsncpy(string, info->product_string, maxlen);
		string[maxlen - 1] = L'\0';
	}
	else {
		string[0] = L'\0';
	}

	return 0;
}

int HID_API_EXPORT_CALL hid_get_serial_number_string(hid_device *dev, wchar_t *string, size_t maxlen)
{
	if (!string || !maxlen) {
		register_device_error(dev, "Zero buffer/length");
		return -1;
	}

	struct hid_device_info *info = hid_get_device_info(dev);
	if (!info) {
		// hid_get_device_info will have set an error already
		return -1;
	}

	if (info->serial_number) {
		wcsncpy(string, info->serial_number, maxlen);
		string[maxlen - 1] = L'\0';
	}
	else {
		string[0] = L'\0';
	}

	return 0;
}


HID_API_EXPORT struct hid_device_info *HID_API_CALL hid_get_device_info(hid_device *dev) {
	if (!dev->device_info) {
		// Lazy initialize device_info
		dev->device_info = create_device_info_for_hid_device(dev);
	}

	// create_device_info_for_hid_device will set an error if needed
	return dev->device_info;
}

int HID_API_EXPORT_CALL hid_get_indexed_string(hid_device *dev, int string_index, wchar_t *string, size_t maxlen)
{
	(void)string_index;
	(void)string;
	(void)maxlen;

	register_device_error(dev, "hid_get_indexed_string: not supported by hidraw");

	return -1;
}


int HID_API_EXPORT_CALL hid_get_report_descriptor(hid_device *dev, unsigned char *buf, size_t buf_size)
{
	struct hidraw_report_descriptor rpt_desc;
	int res = get_hid_report_descriptor_from_hidraw(dev, &rpt_desc);
	if (res < 0) {
		/* error already registered */
		return res;
	}

	if (rpt_desc.size < buf_size) {
		buf_size = (size_t) rpt_desc.size;
	}

	memcpy(buf, rpt_desc.value, buf_size);

	return (int) buf_size;
}


/* Passing in NULL means asking for the last global error message. */
HID_API_EXPORT const wchar_t * HID_API_CALL  hid_error(hid_device *dev)
{
	if (dev) {
		if (dev->last_error_str == NULL)
			return L"Success";
		return dev->last_error_str;
	}

	if (last_global_error_str == NULL)
		return L"Success";
	return last_global_error_str;
}


#else
// HIDAPI LIBUSB

/*******************************************************
 HIDAPI - Multi-Platform library for
 communication with HID devices.

 Alan Ott
 Signal 11 Software

 libusb/hidapi Team

 Copyright 2022, All Rights Reserved.

 At the discretion of the user of this library,
 this software may be licensed under the terms of the
 GNU General Public License v3, a BSD-Style license, or the
 original HIDAPI license as outlined in the LICENSE.txt,
 LICENSE-gpl3.txt, LICENSE-bsd.txt, and LICENSE-orig.txt
 files located at the root of the source distribution.
 These files may also be found in the public source
 code repository located at:
        https://github.com/libusb/hidapi .
********************************************************/

#define _GNU_SOURCE /* needed for wcsdup() before glibc 2.10 */

/* C */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <locale.h>
#include <errno.h>

/* Unix */
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/utsname.h>
#include <fcntl.h>
#include <wchar.h>

/* GNU / LibUSB */
#include <libusb.h>
#if !defined(__ANDROID__) && !defined(NO_ICONV)
#include <iconv.h>
#ifndef ICONV_CONST
#define ICONV_CONST
#endif
#endif

#ifdef __FreeBSD__
#include <hidapi.h>
#else
#include "hidapi_libusb.h"
#endif

#ifndef HIDAPI_THREAD_MODEL_INCLUDE
#define HIDAPI_THREAD_MODEL_INCLUDE "hidapi_thread_pthread.h"
#endif
#include HIDAPI_THREAD_MODEL_INCLUDE

#ifdef __cplusplus
extern "C" {
#endif

#ifdef DEBUG_PRINTF
#define LOG(...) fprintf(stderr, __VA_ARGS__)
#else
#define LOG(...) do {} while (0)
#endif

#ifndef __FreeBSD__
#define DETACH_KERNEL_DRIVER
#endif

/* Uncomment to enable the retrieval of Usage and Usage Page in
hid_enumerate(). Warning, on platforms different from FreeBSD
this is very invasive as it requires the detach
and re-attach of the kernel driver. See comments inside hid_enumerate().
libusb HIDAPI programs are encouraged to use the interface number
instead to differentiate between interfaces on a composite HID device. */
/*#define INVASIVE_GET_USAGE*/

/* Linked List of input reports received from the device. */
struct input_report {
	uint8_t *data;
	size_t len;
	struct input_report *next;
};


struct hid_device_ {
	/* Handle to the actual device. */
	libusb_device_handle *device_handle;

	/* USB Configuration Number of the device */
	int config_number;
	/* The interface number of the HID */
	int interface;

	uint16_t report_descriptor_size;

	/* Endpoint information */
	int input_endpoint;
	int output_endpoint;
	int input_ep_max_packet_size;

	/* Indexes of Strings */
	int manufacturer_index;
	int product_index;
	int serial_index;
	struct hid_device_info* device_info;

	/* Whether blocking reads are used */
	int blocking; /* boolean */

	/* Read thread objects */
	hidapi_thread_state thread_state;
	int shutdown_thread;
	int transfer_loop_finished;
	struct libusb_transfer *transfer;

	/* List of received input reports. */
	struct input_report *input_reports;

	/* Was kernel driver detached by libusb */
#ifdef DETACH_KERNEL_DRIVER
	int is_driver_detached;
#endif
};

static struct hid_api_version api_version = {
	.major = HID_API_VERSION_MAJOR,
	.minor = HID_API_VERSION_MINOR,
	.patch = HID_API_VERSION_PATCH
};

static libusb_context *usb_context = NULL;

uint16_t get_usb_code_for_current_locale(void);
static int return_data(hid_device *dev, unsigned char *data, size_t length);

static hid_device *new_hid_device(void)
{
	hid_device *dev = (hid_device*) calloc(1, sizeof(hid_device));
	dev->blocking = 1;

	hidapi_thread_state_init(&dev->thread_state);

	return dev;
}

static void free_hid_device(hid_device *dev)
{
	/* Clean up the thread objects */
	hidapi_thread_state_destroy(&dev->thread_state);

	hid_free_enumeration(dev->device_info);

	/* Free the device itself */
	free(dev);
}

#if 0
/*TODO: Implement this function on hidapi/libusb.. */
static void register_error(hid_device *dev, const char *op)
{

}
#endif

/* Get bytes from a HID Report Descriptor.
   Only call with a num_bytes of 0, 1, 2, or 4. */
static uint32_t get_bytes(uint8_t *rpt, size_t len, size_t num_bytes, size_t cur)
{
	/* Return if there aren't enough bytes. */
	if (cur + num_bytes >= len)
		return 0;

	if (num_bytes == 0)
		return 0;
	else if (num_bytes == 1) {
		return rpt[cur+1];
	}
	else if (num_bytes == 2) {
		return (rpt[cur+2] * 256 + rpt[cur+1]);
	}
	else if (num_bytes == 4) {
		return (rpt[cur+4] * 0x01000000 +
		        rpt[cur+3] * 0x00010000 +
		        rpt[cur+2] * 0x00000100 +
		        rpt[cur+1] * 0x00000001);
	}
	else
		return 0;
}

/* Retrieves the device's Usage Page and Usage from the report
   descriptor. The algorithm is simple, as it just returns the first
   Usage and Usage Page that it finds in the descriptor.
   The return value is 0 on success and -1 on failure. */
static int get_usage(uint8_t *report_descriptor, size_t size,
                     unsigned short *usage_page, unsigned short *usage)
{
	unsigned int i = 0;
	int size_code;
	int data_len, key_size;
	int usage_found = 0, usage_page_found = 0;

	while (i < size) {
		int key = report_descriptor[i];
		int key_cmd = key & 0xfc;

		//printf("key: %02hhx\n", key);

		if ((key & 0xf0) == 0xf0) {
			/* This is a Long Item. The next byte contains the
			   length of the data section (value) for this key.
			   See the HID specification, version 1.11, section
			   6.2.2.3, titled "Long Items." */
			if (i+1 < size)
				data_len = report_descriptor[i+1];
			else
				data_len = 0; /* malformed report */
			key_size = 3;
		}
		else {
			/* This is a Short Item. The bottom two bits of the
			   key contain the size code for the data section
			   (value) for this key.  Refer to the HID
			   specification, version 1.11, section 6.2.2.2,
			   titled "Short Items." */
			size_code = key & 0x3;
			switch (size_code) {
			case 0:
			case 1:
			case 2:
				data_len = size_code;
				break;
			case 3:
				data_len = 4;
				break;
			default:
				/* Can't ever happen since size_code is & 0x3 */
				data_len = 0;
				break;
			};
			key_size = 1;
		}

		if (key_cmd == 0x4) {
			*usage_page  = get_bytes(report_descriptor, size, data_len, i);
			usage_page_found = 1;
			//printf("Usage Page: %x\n", (uint32_t)*usage_page);
		}
		if (key_cmd == 0x8) {
			if (data_len == 4) { /* Usages 5.5 / Usage Page 6.2.2.7 */
				*usage_page = get_bytes(report_descriptor, size, 2, i + 2);
				usage_page_found = 1;
				*usage = get_bytes(report_descriptor, size, 2, i);
				usage_found = 1;
			}
			else {
				*usage = get_bytes(report_descriptor, size, data_len, i);
				usage_found = 1;
			}
			//printf("Usage: %x\n", (uint32_t)*usage);
		}

		if (usage_page_found && usage_found)
			return 0; /* success */

		/* Skip over this key and it's associated data */
		i += data_len + key_size;
	}

	return -1; /* failure */
}

#if defined(__FreeBSD__) && __FreeBSD__ < 10
/* The libusb version included in FreeBSD < 10 doesn't have this function. In
   mainline libusb, it's inlined in libusb.h. This function will bear a striking
   resemblance to that one, because there's about one way to code it.

   Note that the data parameter is Unicode in UTF-16LE encoding.
   Return value is the number of bytes in data, or LIBUSB_ERROR_*.
 */
static inline int libusb_get_string_descriptor(libusb_device_handle *dev,
	uint8_t descriptor_index, uint16_t lang_id,
	unsigned char *data, int length)
{
	return libusb_control_transfer(dev,
		LIBUSB_ENDPOINT_IN | 0x0, /* Endpoint 0 IN */
		LIBUSB_REQUEST_GET_DESCRIPTOR,
		(LIBUSB_DT_STRING << 8) | descriptor_index,
		lang_id, data, (uint16_t) length, 1000);
}

#endif


/* Get the first language the device says it reports. This comes from
   USB string #0. */
static uint16_t get_first_language(libusb_device_handle *dev)
{
	uint16_t buf[32];
	int len;

	/* Get the string from libusb. */
	len = libusb_get_string_descriptor(dev,
			0x0, /* String ID */
			0x0, /* Language */
			(unsigned char*)buf,
			sizeof(buf));
	if (len < 4)
		return 0x0;

	return buf[1]; /* First two bytes are len and descriptor type. */
}

static int is_language_supported(libusb_device_handle *dev, uint16_t lang)
{
	uint16_t buf[32];
	int len;
	int i;

	/* Get the string from libusb. */
	len = libusb_get_string_descriptor(dev,
			0x0, /* String ID */
			0x0, /* Language */
			(unsigned char*)buf,
			sizeof(buf));
	if (len < 4)
		return 0x0;


	len /= 2; /* language IDs are two-bytes each. */
	/* Start at index 1 because there are two bytes of protocol data. */
	for (i = 1; i < len; i++) {
		if (buf[i] == lang)
			return 1;
	}

	return 0;
}


/* This function returns a newly allocated wide string containing the USB
   device string numbered by the index. The returned string must be freed
   by using free(). */
static wchar_t *get_usb_string(libusb_device_handle *dev, uint8_t idx)
{
	char buf[512];
	int len;
	wchar_t *str = NULL;

#if !defined(__ANDROID__) && !defined(NO_ICONV) /* we don't use iconv on Android, or when it is explicitly disabled */
	wchar_t wbuf[256];
	/* iconv variables */
	iconv_t ic;
	size_t inbytes;
	size_t outbytes;
	size_t res;
	ICONV_CONST char *inptr;
	char *outptr;
#endif

	/* Determine which language to use. */
	uint16_t lang;
	lang = get_usb_code_for_current_locale();
	if (!is_language_supported(dev, lang))
		lang = get_first_language(dev);

	/* Get the string from libusb. */
	len = libusb_get_string_descriptor(dev,
			idx,
			lang,
			(unsigned char*)buf,
			sizeof(buf));
	if (len < 2) /* we always skip first 2 bytes */
		return NULL;

#if defined(__ANDROID__) || defined(NO_ICONV)

	/* Bionic does not have iconv support nor wcsdup() function, so it
	   has to be done manually.  The following code will only work for
	   code points that can be represented as a single UTF-16 character,
	   and will incorrectly convert any code points which require more
	   than one UTF-16 character.

	   Skip over the first character (2-bytes).  */
	len -= 2;
	str = (wchar_t*) malloc((len / 2 + 1) * sizeof(wchar_t));
	int i;
	for (i = 0; i < len / 2; i++) {
		str[i] = buf[i * 2 + 2] | (buf[i * 2 + 3] << 8);
	}
	str[len / 2] = 0x00000000;

#else

	/* buf does not need to be explicitly NULL-terminated because
	   it is only passed into iconv() which does not need it. */

	/* Initialize iconv. */
	ic = iconv_open("WCHAR_T", "UTF-16LE");
	if (ic == (iconv_t)-1) {
		LOG("iconv_open() failed\n");
		return NULL;
	}

	/* Convert to native wchar_t (UTF-32 on glibc/BSD systems).
	   Skip the first character (2-bytes). */
	inptr = buf+2;
	inbytes = len-2;
	outptr = (char*) wbuf;
	outbytes = sizeof(wbuf);
	res = iconv(ic, &inptr, &inbytes, &outptr, &outbytes);
	if (res == (size_t)-1) {
		LOG("iconv() failed\n");
		goto err;
	}

	/* Write the terminating NULL. */
	wbuf[sizeof(wbuf)/sizeof(wbuf[0])-1] = 0x00000000;
	if (outbytes >= sizeof(wbuf[0]))
		*((wchar_t*)outptr) = 0x00000000;

	/* Allocate and copy the string. */
	str = wcsdup(wbuf);

err:
	iconv_close(ic);

#endif

	return str;
}

/**
  Max length of the result: "000-000.000.000.000.000.000.000:000.000" (39 chars).
  64 is used for simplicity/alignment.
*/
static void get_path(char (*result)[64], libusb_device *dev, int config_number, int interface_number)
{
	char *str = *result;

	/* Note that USB3 port count limit is 7; use 8 here for alignment */
	uint8_t port_numbers[8] = {0, 0, 0, 0, 0, 0, 0, 0};
	int num_ports = libusb_get_port_numbers(dev, port_numbers, 8);

	if (num_ports > 0) {
		int n = snprintf(str, sizeof("000-000"), "%u-%u", libusb_get_bus_number(dev), port_numbers[0]);
		for (uint8_t i = 1; i < num_ports; i++) {
			n += snprintf(&str[n], sizeof(".000"), ".%u", port_numbers[i]);
		}
		n += snprintf(&str[n], sizeof(":000.000"), ":%u.%u", (uint8_t)config_number, (uint8_t)interface_number);
		str[n] = '\0';
	} else {
		/* Likely impossible, but check: USB3.0 specs limit number of ports to 7 and buffer size here is 8 */
		if (num_ports == LIBUSB_ERROR_OVERFLOW) {
			LOG("make_path() failed. buffer overflow error\n");
		} else {
			LOG("make_path() failed. unknown error\n");
		}
		str[0] = '\0';
	}
}

static char *make_path(libusb_device *dev, int config_number, int interface_number)
{
	char str[64];
	get_path(&str, dev, config_number, interface_number);
	return strdup(str);
}

HID_API_EXPORT const struct hid_api_version* HID_API_CALL hid_version(void)
{
	return &api_version;
}

HID_API_EXPORT const char* HID_API_CALL hid_version_str(void)
{
	return HID_API_VERSION_STR;
}

int HID_API_EXPORT hid_init(void)
{
	if (!usb_context) {
		const char *locale;

		/* Init Libusb */
		if (libusb_init(&usb_context))
			return -1;

		/* Set the locale if it's not set. */
		locale = setlocale(LC_CTYPE, NULL);
		if (!locale)
			setlocale(LC_CTYPE, "");
	}

	return 0;
}

int HID_API_EXPORT hid_exit(void)
{
	if (usb_context) {
		libusb_exit(usb_context);
		usb_context = NULL;
	}

	return 0;
}

static int hid_get_report_descriptor_libusb(libusb_device_handle *handle, int interface_num, uint16_t expected_report_descriptor_size, unsigned char *buf, size_t buf_size)
{
	unsigned char tmp[HID_API_MAX_REPORT_DESCRIPTOR_SIZE];

	if (expected_report_descriptor_size > HID_API_MAX_REPORT_DESCRIPTOR_SIZE)
		expected_report_descriptor_size = HID_API_MAX_REPORT_DESCRIPTOR_SIZE;

	/* Get the HID Report Descriptor.
	   See USB HID Specification, section 7.1.1
	*/
	int res = libusb_control_transfer(handle, LIBUSB_ENDPOINT_IN|LIBUSB_RECIPIENT_INTERFACE, LIBUSB_REQUEST_GET_DESCRIPTOR, (LIBUSB_DT_REPORT << 8), interface_num, tmp, expected_report_descriptor_size, 5000);
	if (res < 0) {
		LOG("libusb_control_transfer() for getting the HID Report descriptor failed with %d: %s\n", res, libusb_error_name(res));
		return -1;
	}

	if (res > (int)buf_size)
		res = (int)buf_size;

	memcpy(buf, tmp, (size_t)res);
	return res;
}

/**
 * Requires an opened device with *claimed interface*.
 */
static void fill_device_info_usage(struct hid_device_info *cur_dev, libusb_device_handle *handle, int interface_num, uint16_t expected_report_descriptor_size)
{
	unsigned char hid_report_descriptor[HID_API_MAX_REPORT_DESCRIPTOR_SIZE];
	unsigned short page = 0, usage = 0;

	int res = hid_get_report_descriptor_libusb(handle, interface_num, expected_report_descriptor_size, hid_report_descriptor, sizeof(hid_report_descriptor));
	if (res >= 0) {
		/* Parse the usage and usage page
		   out of the report descriptor. */
		get_usage(hid_report_descriptor, res,  &page, &usage);
	}

	cur_dev->usage_page = page;
	cur_dev->usage = usage;
}

#ifdef INVASIVE_GET_USAGE
static void invasive_fill_device_info_usage(struct hid_device_info *cur_dev, libusb_device_handle *handle, int interface_num, uint16_t report_descriptor_size)
{
	int res = 0;

#ifdef DETACH_KERNEL_DRIVER
	int detached = 0;
	/* Usage Page and Usage */
	res = libusb_kernel_driver_active(handle, interface_num);
	if (res == 1) {
		res = libusb_detach_kernel_driver(handle, interface_num);
		if (res < 0)
			LOG("Couldn't detach kernel driver, even though a kernel driver was attached.\n");
		else
			detached = 1;
	}
#endif

	res = libusb_claim_interface(handle, interface_num);
	if (res >= 0) {
		fill_device_info_usage(cur_dev, handle, interface_num, report_descriptor_size);

		/* Release the interface */
		res = libusb_release_interface(handle, interface_num);
		if (res < 0)
			LOG("Can't release the interface.\n");
	}
	else
		LOG("Can't claim interface: (%d) %s\n", res, libusb_error_name(res));

#ifdef DETACH_KERNEL_DRIVER
	/* Re-attach kernel driver if necessary. */
	if (detached) {
		res = libusb_attach_kernel_driver(handle, interface_num);
		if (res < 0)
			LOG("Couldn't re-attach kernel driver.\n");
	}
#endif
}
#endif /* INVASIVE_GET_USAGE */

/**
 * Create and fill up most of hid_device_info fields.
 * usage_page/usage is not filled up.
 */
static struct hid_device_info * create_device_info_for_device(libusb_device *device, libusb_device_handle *handle, struct libusb_device_descriptor *desc, int config_number, int interface_num)
{
	struct hid_device_info *cur_dev = calloc(1, sizeof(struct hid_device_info));
	if (cur_dev == NULL) {
		return NULL;
	}

	/* VID/PID */
	cur_dev->vendor_id = desc->idVendor;
	cur_dev->product_id = desc->idProduct;

	cur_dev->release_number = desc->bcdDevice;

	cur_dev->interface_number = interface_num;

	cur_dev->bus_type = HID_API_BUS_USB;

	cur_dev->path = make_path(device, config_number, interface_num);

	if (!handle) {
		return cur_dev;
	}

	if (desc->iSerialNumber > 0)
		cur_dev->serial_number = get_usb_string(handle, desc->iSerialNumber);

	/* Manufacturer and Product strings */
	if (desc->iManufacturer > 0)
		cur_dev->manufacturer_string = get_usb_string(handle, desc->iManufacturer);
	if (desc->iProduct > 0)
		cur_dev->product_string = get_usb_string(handle, desc->iProduct);

	return cur_dev;
}

static uint16_t get_report_descriptor_size_from_interface_descriptors(const struct libusb_interface_descriptor *intf_desc)
{
	int i = 0;
	int found_hid_report_descriptor = 0;
	uint16_t result = HID_API_MAX_REPORT_DESCRIPTOR_SIZE;
	const unsigned char *extra = intf_desc->extra;
	int extra_length = intf_desc->extra_length;

	/*
	 "extra" contains a HID descriptor
	 See section 6.2.1 of HID 1.1 specification.
	*/

	while (extra_length >= 2) { /* Descriptor header: bLength/bDescriptorType */
		if (extra[1] == LIBUSB_DT_HID) { /* bDescriptorType */
			if (extra_length < 6) {
				LOG("Broken HID descriptor: not enough data\n");
				break;
			}
			unsigned char bNumDescriptors = extra[5];
			if (extra_length < (6 + 3 * bNumDescriptors)) {
				LOG("Broken HID descriptor: not enough data for Report metadata\n");
				break;
			}
			for (i = 0; i < bNumDescriptors; i++) {
				if (extra[6 + 3 * i] == LIBUSB_DT_REPORT) {
					result = (uint16_t)extra[6 + 3 * i + 2] << 8 | extra[6 + 3 * i + 1];
					found_hid_report_descriptor = 1;
					break;
				}
			}

			if (!found_hid_report_descriptor) {
				/* We expect to find exactly 1 HID descriptor (LIBUSB_DT_HID)
				   which should contain exactly one HID Report Descriptor metadata (LIBUSB_DT_REPORT). */
				LOG("Broken HID descriptor: missing Report descriptor\n");
			}
			break;
		}

		if (extra[0] == 0) { /* bLength */
			LOG("Broken HID Interface descriptors: zero-sized descriptor\n");
			break;
		}

		/* Iterate over to the next Descriptor */
		extra_length -= extra[0];
		extra += extra[0];
	}

	return result;
}

static int is_xbox360(unsigned short vendor_id, const struct libusb_interface_descriptor *intf_desc)
{
	static const int xb360_iface_subclass = 93;
	static const int xb360_iface_protocol = 1; /* Wired */
	static const int xb360w_iface_protocol = 129; /* Wireless */
	static const int supported_vendors[] = {
		0x0079, /* GPD Win 2 */
		0x044f, /* Thrustmaster */
		0x045e, /* Microsoft */
		0x046d, /* Logitech */
		0x056e, /* Elecom */
		0x06a3, /* Saitek */
		0x0738, /* Mad Catz */
		0x07ff, /* Mad Catz */
		0x0e6f, /* PDP */
		0x0f0d, /* Hori */
		0x1038, /* SteelSeries */
		0x11c9, /* Nacon */
		0x12ab, /* Unknown */
		0x1430, /* RedOctane */
		0x146b, /* BigBen */
		0x1532, /* Razer Sabertooth */
		0x15e4, /* Numark */
		0x162e, /* Joytech */
		0x1689, /* Razer Onza */
		0x1949, /* Lab126, Inc. */
		0x1bad, /* Harmonix */
		0x20d6, /* PowerA */
		0x24c6, /* PowerA */
		0x2c22, /* Qanba */
		0x2dc8, /* 8BitDo */
		0x9886, /* ASTRO Gaming */
	};

	if (intf_desc->bInterfaceClass == LIBUSB_CLASS_VENDOR_SPEC &&
	    intf_desc->bInterfaceSubClass == xb360_iface_subclass &&
	    (intf_desc->bInterfaceProtocol == xb360_iface_protocol ||
	     intf_desc->bInterfaceProtocol == xb360w_iface_protocol)) {
		size_t i;
		for (i = 0; i < sizeof(supported_vendors)/sizeof(supported_vendors[0]); ++i) {
			if (vendor_id == supported_vendors[i]) {
				return 1;
			}
		}
	}
	return 0;
}

static int is_xboxone(unsigned short vendor_id, const struct libusb_interface_descriptor *intf_desc)
{
	static const int xb1_iface_subclass = 71;
	static const int xb1_iface_protocol = 208;
	static const int supported_vendors[] = {
		0x044f, /* Thrustmaster */
		0x045e, /* Microsoft */
		0x0738, /* Mad Catz */
		0x0e6f, /* PDP */
		0x0f0d, /* Hori */
		0x10f5, /* Turtle Beach */
		0x1532, /* Razer Wildcat */
		0x20d6, /* PowerA */
		0x24c6, /* PowerA */
		0x2dc8, /* 8BitDo */
		0x2e24, /* Hyperkin */
		0x3537, /* GameSir */
	};

	if (intf_desc->bInterfaceNumber == 0 &&
	    intf_desc->bInterfaceClass == LIBUSB_CLASS_VENDOR_SPEC &&
	    intf_desc->bInterfaceSubClass == xb1_iface_subclass &&
	    intf_desc->bInterfaceProtocol == xb1_iface_protocol) {
		size_t i;
		for (i = 0; i < sizeof(supported_vendors)/sizeof(supported_vendors[0]); ++i) {
			if (vendor_id == supported_vendors[i]) {
				return 1;
			}
		}
	}
	return 0;
}

static int should_enumerate_interface(unsigned short vendor_id, const struct libusb_interface_descriptor *intf_desc)
{
#if 0
	printf("Checking interface 0x%x %d/%d/%d/%d\n", vendor_id, intf_desc->bInterfaceNumber, intf_desc->bInterfaceClass, intf_desc->bInterfaceSubClass, intf_desc->bInterfaceProtocol);
#endif

	if (intf_desc->bInterfaceClass == LIBUSB_CLASS_HID)
		return 1;

	/* Also enumerate Xbox 360 controllers */
	if (is_xbox360(vendor_id, intf_desc))
		return 1;

	/* Also enumerate Xbox One controllers */
	if (is_xboxone(vendor_id, intf_desc))
		return 1;

	return 0;
}

struct hid_device_info  HID_API_EXPORT *hid_enumerate(unsigned short vendor_id, unsigned short product_id)
{
	libusb_device **devs;
	libusb_device *dev;
	libusb_device_handle *handle = NULL;
	ssize_t num_devs;
	int i = 0;

	struct hid_device_info *root = NULL; /* return object */
	struct hid_device_info *cur_dev = NULL;

	if(hid_init() < 0)
		return NULL;

	num_devs = libusb_get_device_list(usb_context, &devs);
	if (num_devs < 0)
		return NULL;
	while ((dev = devs[i++]) != NULL) {
		struct libusb_device_descriptor desc;
		struct libusb_config_descriptor *conf_desc = NULL;
		int j, k;

		int res = libusb_get_device_descriptor(dev, &desc);
		if (res < 0)
			continue;

		unsigned short dev_vid = desc.idVendor;
		unsigned short dev_pid = desc.idProduct;

		if ((vendor_id != 0x0 && vendor_id != dev_vid) ||
		    (product_id != 0x0 && product_id != dev_pid)) {
			continue;
		}

		res = libusb_get_active_config_descriptor(dev, &conf_desc);
		if (res < 0)
			libusb_get_config_descriptor(dev, 0, &conf_desc);
		if (conf_desc) {
			for (j = 0; j < conf_desc->bNumInterfaces; j++) {
				const struct libusb_interface *intf = &conf_desc->interface[j];
				for (k = 0; k < intf->num_altsetting; k++) {
					const struct libusb_interface_descriptor *intf_desc;
					intf_desc = &intf->altsetting[k];
					if (should_enumerate_interface(dev_vid, intf_desc)) {
						struct hid_device_info *tmp;

						res = libusb_open(dev, &handle);

#ifdef __ANDROID__
						if (handle) {
							/* There is (a potential) libusb Android backend, in which
							   device descriptor is not accurate up until the device is opened.
							   https://github.com/libusb/libusb/pull/874#discussion_r632801373
							   A workaround is to re-read the descriptor again.
							   Even if it is not going to be accepted into libusb master,
							   having it here won't do any harm, since reading the device descriptor
							   is as cheap as copy 18 bytes of data. */
							libusb_get_device_descriptor(dev, &desc);
						}
#endif

						tmp = create_device_info_for_device(dev, handle, &desc, conf_desc->bConfigurationValue, intf_desc->bInterfaceNumber);
						if (tmp) {
#ifdef INVASIVE_GET_USAGE
							/* TODO: have a runtime check for this section. */

							/*
							This section is removed because it is too
							invasive on the system. Getting a Usage Page
							and Usage requires parsing the HID Report
							descriptor. Getting a HID Report descriptor
							involves claiming the interface. Claiming the
							interface involves detaching the kernel driver.
							Detaching the kernel driver is hard on the system
							because it will unclaim interfaces (if another
							app has them claimed) and the re-attachment of
							the driver will sometimes change /dev entry names.
							It is for these reasons that this section is
							optional. For composite devices, use the interface
							field in the hid_device_info struct to distinguish
							between interfaces. */
							if (handle) {
								uint16_t report_descriptor_size = get_report_descriptor_size_from_interface_descriptors(intf_desc);

								invasive_fill_device_info_usage(tmp, handle, intf_desc->bInterfaceNumber, report_descriptor_size);
							}
#endif /* INVASIVE_GET_USAGE */

							if (cur_dev) {
								cur_dev->next = tmp;
							}
							else {
								root = tmp;
							}
							cur_dev = tmp;
						}

						if (res >= 0) {
							libusb_close(handle);
							handle = NULL;
						}
						break;
					}
				} /* altsettings */
			} /* interfaces */
			libusb_free_config_descriptor(conf_desc);
		}
	}

	libusb_free_device_list(devs, 1);

	return root;
}

void  HID_API_EXPORT hid_free_enumeration(struct hid_device_info *devs)
{
	struct hid_device_info *d = devs;
	while (d) {
		struct hid_device_info *next = d->next;
		free(d->path);
		free(d->serial_number);
		free(d->manufacturer_string);
		free(d->product_string);
		free(d);
		d = next;
	}
}

hid_device * hid_open(unsigned short vendor_id, unsigned short product_id, const wchar_t *serial_number)
{
	struct hid_device_info *devs, *cur_dev;
	const char *path_to_open = NULL;
	hid_device *handle = NULL;

	devs = hid_enumerate(vendor_id, product_id);
	cur_dev = devs;
	while (cur_dev) {
		if (cur_dev->vendor_id == vendor_id &&
		    cur_dev->product_id == product_id) {
			if (serial_number) {
				if (cur_dev->serial_number &&
				    wcscmp(serial_number, cur_dev->serial_number) == 0) {
					path_to_open = cur_dev->path;
					break;
				}
			}
			else {
				path_to_open = cur_dev->path;
				break;
			}
		}
		cur_dev = cur_dev->next;
	}

	if (path_to_open) {
		/* Open the device */
		handle = hid_open_path(path_to_open);
	}

	hid_free_enumeration(devs);

	return handle;
}

static void LIBUSB_CALL read_callback(struct libusb_transfer *transfer)
{
	hid_device *dev = transfer->user_data;
	int res;

	if (transfer->status == LIBUSB_TRANSFER_COMPLETED) {

		struct input_report *rpt = (struct input_report*) malloc(sizeof(*rpt));
		rpt->data = (uint8_t*) malloc(transfer->actual_length);
		memcpy(rpt->data, transfer->buffer, transfer->actual_length);
		rpt->len = transfer->actual_length;
		rpt->next = NULL;

		hidapi_thread_mutex_lock(&dev->thread_state);

		/* Attach the new report object to the end of the list. */
		if (dev->input_reports == NULL) {
			/* The list is empty. Put it at the root. */
			dev->input_reports = rpt;
			hidapi_thread_cond_signal(&dev->thread_state);
		}
		else {
			/* Find the end of the list and attach. */
			struct input_report *cur = dev->input_reports;
			int num_queued = 0;
			while (cur->next != NULL) {
				cur = cur->next;
				num_queued++;
			}
			cur->next = rpt;

			/* Pop one off if we've reached 30 in the queue. This
			   way we don't grow forever if the user never reads
			   anything from the device. */
			if (num_queued > 30) {
				return_data(dev, NULL, 0);
			}
		}
		hidapi_thread_mutex_unlock(&dev->thread_state);
	}
	else if (transfer->status == LIBUSB_TRANSFER_CANCELLED) {
		dev->shutdown_thread = 1;
	}
	else if (transfer->status == LIBUSB_TRANSFER_NO_DEVICE) {
		dev->shutdown_thread = 1;
	}
	else if (transfer->status == LIBUSB_TRANSFER_TIMED_OUT) {
		//LOG("Timeout (normal)\n");
	}
	else {
		LOG("Unknown transfer code: %d\n", transfer->status);
	}

	if (dev->shutdown_thread) {
		dev->transfer_loop_finished = 1;
		return;
	}

	/* Re-submit the transfer object. */
	res = libusb_submit_transfer(transfer);
	if (res != 0) {
		LOG("Unable to submit URB: (%d) %s\n", res, libusb_error_name(res));
		dev->shutdown_thread = 1;
		dev->transfer_loop_finished = 1;
	}
}


static void *read_thread(void *param)
{
	int res;
	hid_device *dev = param;
	uint8_t *buf;
	const size_t length = dev->input_ep_max_packet_size;

	/* Set up the transfer object. */
	buf = (uint8_t*) malloc(length);
	dev->transfer = libusb_alloc_transfer(0);
	libusb_fill_interrupt_transfer(dev->transfer,
		dev->device_handle,
		dev->input_endpoint,
		buf,
		(int)length,
		read_callback,
		dev,
		5000/*timeout*/);

	/* Make the first submission. Further submissions are made
	   from inside read_callback() */
	res = libusb_submit_transfer(dev->transfer);
	if(res < 0) {
                LOG("libusb_submit_transfer failed: %d %s. Stopping read_thread from running\n", res, libusb_error_name(res));
                dev->shutdown_thread = 1;
                dev->transfer_loop_finished = 1;
	}

	/* Notify the main thread that the read thread is up and running. */
	hidapi_thread_barrier_wait(&dev->thread_state);

	/* Handle all the events. */
	while (!dev->shutdown_thread) {
		res = libusb_handle_events(usb_context);
		if (res < 0) {
			/* There was an error. */
			LOG("read_thread(): (%d) %s\n", res, libusb_error_name(res));

			/* Break out of this loop only on fatal error.*/
			if (res != LIBUSB_ERROR_BUSY &&
			    res != LIBUSB_ERROR_TIMEOUT &&
			    res != LIBUSB_ERROR_OVERFLOW &&
			    res != LIBUSB_ERROR_INTERRUPTED) {
				dev->shutdown_thread = 1;
				break;
			}
		}
	}

	/* Cancel any transfer that may be pending. This call will fail
	   if no transfers are pending, but that's OK. */
	libusb_cancel_transfer(dev->transfer);

	while (!dev->transfer_loop_finished)
		libusb_handle_events_completed(usb_context, &dev->transfer_loop_finished);

	/* Now that the read thread is stopping, Wake any threads which are
	   waiting on data (in hid_read_timeout()). Do this under a mutex to
	   make sure that a thread which is about to go to sleep waiting on
	   the condition actually will go to sleep before the condition is
	   signaled. */
	hidapi_thread_mutex_lock(&dev->thread_state);
	hidapi_thread_cond_broadcast(&dev->thread_state);
	hidapi_thread_mutex_unlock(&dev->thread_state);

	/* The dev->transfer->buffer and dev->transfer objects are cleaned up
	   in hid_close(). They are not cleaned up here because this thread
	   could end either due to a disconnect or due to a user
	   call to hid_close(). In both cases the objects can be safely
	   cleaned up after the call to hidapi_thread_join() (in hid_close()), but
	   since hid_close() calls libusb_cancel_transfer(), on these objects,
	   they can not be cleaned up here. */

	return NULL;
}

static void init_xbox360(libusb_device_handle *device_handle, unsigned short idVendor, unsigned short idProduct, const struct libusb_config_descriptor *conf_desc)
{
	(void)conf_desc;

	if ((idVendor == 0x05ac && idProduct == 0x055b) /* Gamesir-G3w */ ||
	    idVendor == 0x0f0d /* Hori Xbox controllers */) {
		unsigned char data[20];

		/* The HORIPAD FPS for Nintendo Switch requires this to enable input reports.
		   This VID/PID is also shared with other HORI controllers, but they all seem
		   to be fine with this as well.
		 */
		memset(data, 0, sizeof(data));
		libusb_control_transfer(device_handle, 0xC1, 0x01, 0x100, 0x0, data, sizeof(data), 100);
	}
}

static void init_xboxone(libusb_device_handle *device_handle, unsigned short idVendor, unsigned short idProduct, const struct libusb_config_descriptor *conf_desc)
{
	static const int vendor_microsoft = 0x045e;
	static const int xb1_iface_subclass = 71;
	static const int xb1_iface_protocol = 208;
	int j, k, res;

	(void)idProduct;

	for (j = 0; j < conf_desc->bNumInterfaces; j++) {
		const struct libusb_interface *intf = &conf_desc->interface[j];
		for (k = 0; k < intf->num_altsetting; k++) {
			const struct libusb_interface_descriptor *intf_desc = &intf->altsetting[k];
			if (intf_desc->bInterfaceClass == LIBUSB_CLASS_VENDOR_SPEC &&
			    intf_desc->bInterfaceSubClass == xb1_iface_subclass &&
			    intf_desc->bInterfaceProtocol == xb1_iface_protocol) {
				int bSetAlternateSetting = 0;

				/* Newer Microsoft Xbox One controllers have a high speed alternate setting */
				if (idVendor == vendor_microsoft &&
				    intf_desc->bInterfaceNumber == 0 && intf_desc->bAlternateSetting == 1) {
					bSetAlternateSetting = 1;
				} else if (intf_desc->bInterfaceNumber != 0 && intf_desc->bAlternateSetting == 0) {
					bSetAlternateSetting = 1;
				}

				if (bSetAlternateSetting) {
					res = libusb_claim_interface(device_handle, intf_desc->bInterfaceNumber);
					if (res < 0) {
						LOG("can't claim interface %d: %d\n", intf_desc->bInterfaceNumber, res);
						continue;
					}

					LOG("Setting alternate setting for VID/PID 0x%x/0x%x interface %d to %d\n",  idVendor, idProduct, intf_desc->bInterfaceNumber, intf_desc->bAlternateSetting);

					res = libusb_set_interface_alt_setting(device_handle, intf_desc->bInterfaceNumber, intf_desc->bAlternateSetting);
					if (res < 0) {
						LOG("xbox init: can't set alt setting %d: %d\n", intf_desc->bInterfaceNumber, res);
					}

					libusb_release_interface(device_handle, intf_desc->bInterfaceNumber);
				}
			}
		}
	}
}

static int hidapi_initialize_device(hid_device *dev, const struct libusb_interface_descriptor *intf_desc, const struct libusb_config_descriptor *conf_desc)
{
	int i =0;
	int res = 0;
	struct libusb_device_descriptor desc;
	libusb_get_device_descriptor(libusb_get_device(dev->device_handle), &desc);

#ifdef DETACH_KERNEL_DRIVER
	/* Detach the kernel driver, but only if the
	   device is managed by the kernel */
	dev->is_driver_detached = 0;
	if (libusb_kernel_driver_active(dev->device_handle, intf_desc->bInterfaceNumber) == 1) {
		res = libusb_detach_kernel_driver(dev->device_handle, intf_desc->bInterfaceNumber);
		if (res < 0) {
			LOG("Unable to detach Kernel Driver: (%d) %s\n", res, libusb_error_name(res));
			return 0;
		}
		else {
			dev->is_driver_detached = 1;
			LOG("Driver successfully detached from kernel.\n");
		}
	}
#endif
	res = libusb_claim_interface(dev->device_handle, intf_desc->bInterfaceNumber);
	if (res < 0) {
		LOG("can't claim interface %d: (%d) %s\n", intf_desc->bInterfaceNumber, res, libusb_error_name(res));

#ifdef DETACH_KERNEL_DRIVER
		if (dev->is_driver_detached) {
			res = libusb_attach_kernel_driver(dev->device_handle, intf_desc->bInterfaceNumber);
			if (res < 0)
				LOG("Failed to reattach the driver to kernel: (%d) %s\n", res, libusb_error_name(res));
		}
#endif
		return 0;
	}

	/* Initialize XBox 360 controllers */
	if (is_xbox360(desc.idVendor, intf_desc)) {
		init_xbox360(dev->device_handle, desc.idVendor, desc.idProduct, conf_desc);
	}

	/* Initialize XBox One controllers */
	if (is_xboxone(desc.idVendor, intf_desc)) {
		init_xboxone(dev->device_handle, desc.idVendor, desc.idProduct, conf_desc);
	}

	/* Store off the string descriptor indexes */
	dev->manufacturer_index = desc.iManufacturer;
	dev->product_index      = desc.iProduct;
	dev->serial_index       = desc.iSerialNumber;

	/* Store off the USB information */
	dev->config_number = conf_desc->bConfigurationValue;
	dev->interface = intf_desc->bInterfaceNumber;

	dev->report_descriptor_size = get_report_descriptor_size_from_interface_descriptors(intf_desc);

	dev->input_endpoint = 0;
	dev->input_ep_max_packet_size = 0;
	dev->output_endpoint = 0;

	/* Find the INPUT and OUTPUT endpoints. An
	   OUTPUT endpoint is not required. */
	for (i = 0; i < intf_desc->bNumEndpoints; i++) {
		const struct libusb_endpoint_descriptor *ep
			= &intf_desc->endpoint[i];

		/* Determine the type and direction of this
		   endpoint. */
		int is_interrupt =
			(ep->bmAttributes & LIBUSB_TRANSFER_TYPE_MASK)
		      == LIBUSB_TRANSFER_TYPE_INTERRUPT;
		int is_output =
			(ep->bEndpointAddress & LIBUSB_ENDPOINT_DIR_MASK)
		      == LIBUSB_ENDPOINT_OUT;
		int is_input =
			(ep->bEndpointAddress & LIBUSB_ENDPOINT_DIR_MASK)
		      == LIBUSB_ENDPOINT_IN;

		/* Decide whether to use it for input or output. */
		if (dev->input_endpoint == 0 &&
		    is_interrupt && is_input) {
			/* Use this endpoint for INPUT */
			dev->input_endpoint = ep->bEndpointAddress;
			dev->input_ep_max_packet_size = ep->wMaxPacketSize;
		}
		if (dev->output_endpoint == 0 &&
		    is_interrupt && is_output) {
			/* Use this endpoint for OUTPUT */
			dev->output_endpoint = ep->bEndpointAddress;
		}
	}

	hidapi_thread_create(&dev->thread_state, read_thread, dev);

	/* Wait here for the read thread to be initialized. */
	hidapi_thread_barrier_wait(&dev->thread_state);
	return 1;
}


hid_device * HID_API_EXPORT hid_open_path(const char *path)
{
	hid_device *dev = NULL;

	libusb_device **devs = NULL;
	libusb_device *usb_dev = NULL;
	int res = 0;
	int d = 0;
	int good_open = 0;

	if(hid_init() < 0)
		return NULL;

	dev = new_hid_device();
	if (!dev) {
		LOG("hid_open_path failed: Couldn't allocate memory\n");
		return NULL;
	}

	libusb_get_device_list(usb_context, &devs);
	while ((usb_dev = devs[d++]) != NULL && !good_open) {
		struct libusb_device_descriptor desc;
		struct libusb_config_descriptor *conf_desc = NULL;
		int j,k;

		res = libusb_get_device_descriptor(usb_dev, &desc);
		if (res < 0)
			continue;

		res = libusb_get_active_config_descriptor(usb_dev, &conf_desc);
		if (res < 0)
			libusb_get_config_descriptor(usb_dev, 0, &conf_desc);
		if (!conf_desc)
			continue;

		for (j = 0; j < conf_desc->bNumInterfaces && !good_open; j++) {
			const struct libusb_interface *intf = &conf_desc->interface[j];
			for (k = 0; k < intf->num_altsetting && !good_open; k++) {
				const struct libusb_interface_descriptor *intf_desc = &intf->altsetting[k];
				if (should_enumerate_interface(desc.idVendor, intf_desc)) {
					char dev_path[64];
					get_path(&dev_path, usb_dev, conf_desc->bConfigurationValue, intf_desc->bInterfaceNumber);
					if (!strcmp(dev_path, path)) {
						/* Matched Paths. Open this device */

						/* OPEN HERE */
						res = libusb_open(usb_dev, &dev->device_handle);
						if (res < 0) {
							LOG("can't open device\n");
							break;
						}
						good_open = hidapi_initialize_device(dev, intf_desc, conf_desc);
						if (!good_open)
							libusb_close(dev->device_handle);
					}
				}
			}
		}
		libusb_free_config_descriptor(conf_desc);
	}

	libusb_free_device_list(devs, 1);

	/* If we have a good handle, return it. */
	if (good_open) {
		return dev;
	}
	else {
		/* Unable to open any devices. */
		free_hid_device(dev);
		return NULL;
	}
}


HID_API_EXPORT hid_device * HID_API_CALL hid_libusb_wrap_sys_device(intptr_t sys_dev, int interface_num)
{
/* 0x01000107 is a LIBUSB_API_VERSION for 1.0.23 - version when libusb_wrap_sys_device was introduced */
#if (!defined(HIDAPI_TARGET_LIBUSB_API_VERSION) || HIDAPI_TARGET_LIBUSB_API_VERSION >= 0x01000107) && (LIBUSB_API_VERSION >= 0x01000107)
	hid_device *dev = NULL;
	struct libusb_config_descriptor *conf_desc = NULL;
	const struct libusb_interface_descriptor *selected_intf_desc = NULL;
	int res = 0;
	int j = 0, k = 0;

	if(hid_init() < 0)
		return NULL;

	dev = new_hid_device();
	if (!dev) {
		LOG("libusb_wrap_sys_device failed: Couldn't allocate memory\n");
		return NULL;
	}

	res = libusb_wrap_sys_device(usb_context, sys_dev, &dev->device_handle);
	if (res < 0) {
		LOG("libusb_wrap_sys_device failed: %d %s\n", res, libusb_error_name(res));
		goto err;
	}

	res = libusb_get_active_config_descriptor(libusb_get_device(dev->device_handle), &conf_desc);
	if (res < 0)
		libusb_get_config_descriptor(libusb_get_device(dev->device_handle), 0, &conf_desc);

	if (!conf_desc) {
		LOG("Failed to get configuration descriptor: %d %s\n", res, libusb_error_name(res));
		goto err;
	}

	/* find matching HID interface */
	for (j = 0; j < conf_desc->bNumInterfaces && !selected_intf_desc; j++) {
		const struct libusb_interface *intf = &conf_desc->interface[j];
		for (k = 0; k < intf->num_altsetting; k++) {
			const struct libusb_interface_descriptor *intf_desc = &intf->altsetting[k];
			if (intf_desc->bInterfaceClass == LIBUSB_CLASS_HID) {
				if (interface_num < 0 || interface_num == intf_desc->bInterfaceNumber) {
					selected_intf_desc = intf_desc;
					break;
				}
			}
		}
	}

	if (!selected_intf_desc) {
		if (interface_num < 0) {
			LOG("Sys USB device doesn't contain a HID interface\n");
		}
		else {
			LOG("Sys USB device doesn't contain a HID interface with number %d\n", interface_num);
		}
		goto err;
	}

	if (!hidapi_initialize_device(dev, selected_intf_desc, conf_desc))
		goto err;

	return dev;

err:
	if (conf_desc)
		libusb_free_config_descriptor(conf_desc);
	if (dev->device_handle)
		libusb_close(dev->device_handle);
	free_hid_device(dev);
#else
	(void)sys_dev;
	(void)interface_num;
	LOG("libusb_wrap_sys_device is not available\n");
#endif
	return NULL;
}


int HID_API_EXPORT hid_write(hid_device *dev, const unsigned char *data, size_t length)
{
	int res;
	int report_number;
	int skipped_report_id = 0;

	if (dev->output_endpoint <= 0) {
		/* No interrupt out endpoint. Use the Control Endpoint */
		return hid_send_output_report(dev, data, length);
	}

	if (!data || (length ==0)) {
		return -1;
	}

	report_number = data[0];

	if (report_number == 0x0) {
		data++;
		length--;
		skipped_report_id = 1;
	}

	/* Use the interrupt out endpoint */
	int actual_length;
	res = libusb_interrupt_transfer(dev->device_handle,
		dev->output_endpoint,
		(unsigned char*)data,
		(int)length,
		&actual_length, 1000);

	if (res < 0)
		return -1;

	if (skipped_report_id)
		actual_length++;

	return actual_length;
}

/* Helper function, to simplify hid_read().
   This should be called with dev->mutex locked. */
static int return_data(hid_device *dev, unsigned char *data, size_t length)
{
	/* Copy the data out of the linked list item (rpt) into the
	   return buffer (data), and delete the liked list item. */
	struct input_report *rpt = dev->input_reports;
	size_t len = (length < rpt->len)? length: rpt->len;
	if (len > 0)
		memcpy(data, rpt->data, len);
	dev->input_reports = rpt->next;
	free(rpt->data);
	free(rpt);
	return (int)len;
}

static void cleanup_mutex(void *param)
{
	hid_device *dev = param;
	hidapi_thread_mutex_unlock(&dev->thread_state);
}


int HID_API_EXPORT hid_read_timeout(hid_device *dev, unsigned char *data, size_t length, int milliseconds)
{
#if 0
	int transferred;
	int res = libusb_interrupt_transfer(dev->device_handle, dev->input_endpoint, data, length, &transferred, 5000);
	LOG("transferred: %d\n", transferred);
	return transferred;
#endif
	/* by initialising this variable right here, GCC gives a compilation warning/error: */
	/* error: variable ‘bytes_read’ might be clobbered by ‘longjmp’ or ‘vfork’ [-Werror=clobbered] */
	int bytes_read; /* = -1; */

	hidapi_thread_mutex_lock(&dev->thread_state);
	hidapi_thread_cleanup_push(cleanup_mutex, dev);

	bytes_read = -1;

	/* There's an input report queued up. Return it. */
	if (dev->input_reports) {
		/* Return the first one */
		bytes_read = return_data(dev, data, length);
		goto ret;
	}

	if (dev->shutdown_thread) {
		/* This means the device has been disconnected.
		   An error code of -1 should be returned. */
		bytes_read = -1;
		goto ret;
	}

	if (milliseconds == -1) {
		/* Blocking */
		while (!dev->input_reports && !dev->shutdown_thread) {
			hidapi_thread_cond_wait(&dev->thread_state);
		}
		if (dev->input_reports) {
			bytes_read = return_data(dev, data, length);
		}
	}
	else if (milliseconds > 0) {
		/* Non-blocking, but called with timeout. */
		int res;
		hidapi_timespec ts;
		hidapi_thread_gettime(&ts);
		hidapi_thread_addtime(&ts, milliseconds);

		while (!dev->input_reports && !dev->shutdown_thread) {
			res = hidapi_thread_cond_timedwait(&dev->thread_state, &ts);
			if (res == 0) {
				if (dev->input_reports) {
					bytes_read = return_data(dev, data, length);
					break;
				}

				/* If we're here, there was a spurious wake up
				   or the read thread was shutdown. Run the
				   loop again (ie: don't break). */
			}
			else if (res == HIDAPI_THREAD_TIMED_OUT) {
				/* Timed out. */
				bytes_read = 0;
				break;
			}
			else {
				/* Error. */
				bytes_read = -1;
				break;
			}
		}
	}
	else {
		/* Purely non-blocking */
		bytes_read = 0;
	}

ret:
	hidapi_thread_mutex_unlock(&dev->thread_state);
	hidapi_thread_cleanup_pop(0);

	return bytes_read;
}

int HID_API_EXPORT hid_read(hid_device *dev, unsigned char *data, size_t length)
{
	return hid_read_timeout(dev, data, length, dev->blocking ? -1 : 0);
}

int HID_API_EXPORT hid_set_nonblocking(hid_device *dev, int nonblock)
{
	dev->blocking = !nonblock;

	return 0;
}


int HID_API_EXPORT hid_send_feature_report(hid_device *dev, const unsigned char *data, size_t length)
{
	int res = -1;
	int skipped_report_id = 0;
	int report_number = data[0];

	if (report_number == 0x0) {
		data++;
		length--;
		skipped_report_id = 1;
	}

	res = libusb_control_transfer(dev->device_handle,
		LIBUSB_REQUEST_TYPE_CLASS|LIBUSB_RECIPIENT_INTERFACE|LIBUSB_ENDPOINT_OUT,
		0x09/*HID set_report*/,
		(3/*HID feature*/ << 8) | report_number,
		dev->interface,
		(unsigned char *)data, length,
		1000/*timeout millis*/);

	if (res < 0)
		return -1;

	/* Account for the report ID */
	if (skipped_report_id)
		length++;

	return (int)length;
}

int HID_API_EXPORT hid_get_feature_report(hid_device *dev, unsigned char *data, size_t length)
{
	int res = -1;
	int skipped_report_id = 0;
	int report_number = data[0];

	if (report_number == 0x0) {
		/* Offset the return buffer by 1, so that the report ID
		   will remain in byte 0. */
		data++;
		length--;
		skipped_report_id = 1;
	}
	res = libusb_control_transfer(dev->device_handle,
		LIBUSB_REQUEST_TYPE_CLASS|LIBUSB_RECIPIENT_INTERFACE|LIBUSB_ENDPOINT_IN,
		0x01/*HID get_report*/,
		(3/*HID feature*/ << 8) | report_number,
		dev->interface,
		(unsigned char *)data, length,
		1000/*timeout millis*/);

	if (res < 0)
		return -1;

	if (skipped_report_id)
		res++;

	return res;
}

int HID_API_EXPORT hid_send_output_report(hid_device *dev, const unsigned char *data, size_t length)
{
	int res = -1;
	int skipped_report_id = 0;
	int report_number = data[0];

	if (report_number == 0x0) {
		data++;
		length--;
		skipped_report_id = 1;
	}

	res = libusb_control_transfer(dev->device_handle,
		LIBUSB_REQUEST_TYPE_CLASS|LIBUSB_RECIPIENT_INTERFACE|LIBUSB_ENDPOINT_OUT,
		0x09/*HID set_report*/,
		(2/*HID output*/ << 8) | report_number,
		dev->interface,
		(unsigned char *)data, length,
		1000/*timeout millis*/);

	if (res < 0)
		return -1;

	/* Account for the report ID */
	if (skipped_report_id)
		length++;

	return length;
}

int HID_API_EXPORT HID_API_CALL hid_get_input_report(hid_device *dev, unsigned char *data, size_t length)
{
	int res = -1;
	int skipped_report_id = 0;
	int report_number = data[0];

	if (report_number == 0x0) {
		/* Offset the return buffer by 1, so that the report ID
		   will remain in byte 0. */
		data++;
		length--;
		skipped_report_id = 1;
	}
	res = libusb_control_transfer(dev->device_handle,
		LIBUSB_REQUEST_TYPE_CLASS|LIBUSB_RECIPIENT_INTERFACE|LIBUSB_ENDPOINT_IN,
		0x01/*HID get_report*/,
		(1/*HID Input*/ << 8) | report_number,
		dev->interface,
		(unsigned char *)data, length,
		1000/*timeout millis*/);

	if (res < 0)
		return -1;

	if (skipped_report_id)
		res++;

	return res;
}

void HID_API_EXPORT hid_close(hid_device *dev)
{
	if (!dev)
		return;

	/* Cause read_thread() to stop. */
	dev->shutdown_thread = 1;
	libusb_cancel_transfer(dev->transfer);

	/* Wait for read_thread() to end. */
	hidapi_thread_join(&dev->thread_state);

	/* Clean up the Transfer objects allocated in read_thread(). */
	free(dev->transfer->buffer);
	dev->transfer->buffer = NULL;
	libusb_free_transfer(dev->transfer);

	/* release the interface */
	libusb_release_interface(dev->device_handle, dev->interface);

	/* reattach the kernel driver if it was detached */
#ifdef DETACH_KERNEL_DRIVER
	if (dev->is_driver_detached) {
		int res = libusb_attach_kernel_driver(dev->device_handle, dev->interface);
		if (res < 0)
			LOG("Failed to reattach the driver to kernel.\n");
	}
#endif

	/* Close the handle */
	libusb_close(dev->device_handle);

	/* Clear out the queue of received reports. */
	hidapi_thread_mutex_lock(&dev->thread_state);
	while (dev->input_reports) {
		return_data(dev, NULL, 0);
	}
	hidapi_thread_mutex_unlock(&dev->thread_state);

	free_hid_device(dev);
}


int HID_API_EXPORT_CALL hid_get_manufacturer_string(hid_device *dev, wchar_t *string, size_t maxlen)
{
	return hid_get_indexed_string(dev, dev->manufacturer_index, string, maxlen);
}

int HID_API_EXPORT_CALL hid_get_product_string(hid_device *dev, wchar_t *string, size_t maxlen)
{
	return hid_get_indexed_string(dev, dev->product_index, string, maxlen);
}

int HID_API_EXPORT_CALL hid_get_serial_number_string(hid_device *dev, wchar_t *string, size_t maxlen)
{
	return hid_get_indexed_string(dev, dev->serial_index, string, maxlen);
}

HID_API_EXPORT struct hid_device_info *HID_API_CALL hid_get_device_info(hid_device *dev) {
	if (!dev->device_info) {
		struct libusb_device_descriptor desc;
		libusb_device *usb_device = libusb_get_device(dev->device_handle);
		libusb_get_device_descriptor(usb_device, &desc);

		dev->device_info = create_device_info_for_device(usb_device, dev->device_handle, &desc, dev->config_number, dev->interface);
		// device error already set by create_device_info_for_device, if any

		if (dev->device_info) {
			fill_device_info_usage(dev->device_info, dev->device_handle, dev->interface, dev->report_descriptor_size);
		}
	}

	return dev->device_info;
}

int HID_API_EXPORT_CALL hid_get_indexed_string(hid_device *dev, int string_index, wchar_t *string, size_t maxlen)
{
	wchar_t *str;

	str = get_usb_string(dev->device_handle, string_index);
	if (str) {
		wcsncpy(string, str, maxlen);
		string[maxlen-1] = L'\0';
		free(str);
		return 0;
	}
	else
		return -1;
}


int HID_API_EXPORT_CALL hid_get_report_descriptor(hid_device *dev, unsigned char *buf, size_t buf_size)
{
	return hid_get_report_descriptor_libusb(dev->device_handle, dev->interface, dev->report_descriptor_size, buf, buf_size);
}


HID_API_EXPORT const wchar_t * HID_API_CALL  hid_error(hid_device *dev)
{
	(void)dev;
	return L"hid_error is not implemented yet";
}


struct lang_map_entry {
	const char *name;
	const char *string_code;
	uint16_t usb_code;
};

#define LANG(name,code,usb_code) { name, code, usb_code }
static struct lang_map_entry lang_map[] = {
	LANG("Afrikaans", "af", 0x0436),
	LANG("Albanian", "sq", 0x041C),
	LANG("Arabic - United Arab Emirates", "ar_ae", 0x3801),
	LANG("Arabic - Bahrain", "ar_bh", 0x3C01),
	LANG("Arabic - Algeria", "ar_dz", 0x1401),
	LANG("Arabic - Egypt", "ar_eg", 0x0C01),
	LANG("Arabic - Iraq", "ar_iq", 0x0801),
	LANG("Arabic - Jordan", "ar_jo", 0x2C01),
	LANG("Arabic - Kuwait", "ar_kw", 0x3401),
	LANG("Arabic - Lebanon", "ar_lb", 0x3001),
	LANG("Arabic - Libya", "ar_ly", 0x1001),
	LANG("Arabic - Morocco", "ar_ma", 0x1801),
	LANG("Arabic - Oman", "ar_om", 0x2001),
	LANG("Arabic - Qatar", "ar_qa", 0x4001),
	LANG("Arabic - Saudi Arabia", "ar_sa", 0x0401),
	LANG("Arabic - Syria", "ar_sy", 0x2801),
	LANG("Arabic - Tunisia", "ar_tn", 0x1C01),
	LANG("Arabic - Yemen", "ar_ye", 0x2401),
	LANG("Armenian", "hy", 0x042B),
	LANG("Azeri - Latin", "az_az", 0x042C),
	LANG("Azeri - Cyrillic", "az_az", 0x082C),
	LANG("Basque", "eu", 0x042D),
	LANG("Belarusian", "be", 0x0423),
	LANG("Bulgarian", "bg", 0x0402),
	LANG("Catalan", "ca", 0x0403),
	LANG("Chinese - China", "zh_cn", 0x0804),
	LANG("Chinese - Hong Kong SAR", "zh_hk", 0x0C04),
	LANG("Chinese - Macau SAR", "zh_mo", 0x1404),
	LANG("Chinese - Singapore", "zh_sg", 0x1004),
	LANG("Chinese - Taiwan", "zh_tw", 0x0404),
	LANG("Croatian", "hr", 0x041A),
	LANG("Czech", "cs", 0x0405),
	LANG("Danish", "da", 0x0406),
	LANG("Dutch - Netherlands", "nl_nl", 0x0413),
	LANG("Dutch - Belgium", "nl_be", 0x0813),
	LANG("English - Australia", "en_au", 0x0C09),
	LANG("English - Belize", "en_bz", 0x2809),
	LANG("English - Canada", "en_ca", 0x1009),
	LANG("English - Caribbean", "en_cb", 0x2409),
	LANG("English - Ireland", "en_ie", 0x1809),
	LANG("English - Jamaica", "en_jm", 0x2009),
	LANG("English - New Zealand", "en_nz", 0x1409),
	LANG("English - Philippines", "en_ph", 0x3409),
	LANG("English - Southern Africa", "en_za", 0x1C09),
	LANG("English - Trinidad", "en_tt", 0x2C09),
	LANG("English - Great Britain", "en_gb", 0x0809),
	LANG("English - United States", "en_us", 0x0409),
	LANG("Estonian", "et", 0x0425),
	LANG("Farsi", "fa", 0x0429),
	LANG("Finnish", "fi", 0x040B),
	LANG("Faroese", "fo", 0x0438),
	LANG("French - France", "fr_fr", 0x040C),
	LANG("French - Belgium", "fr_be", 0x080C),
	LANG("French - Canada", "fr_ca", 0x0C0C),
	LANG("French - Luxembourg", "fr_lu", 0x140C),
	LANG("French - Switzerland", "fr_ch", 0x100C),
	LANG("Gaelic - Ireland", "gd_ie", 0x083C),
	LANG("Gaelic - Scotland", "gd", 0x043C),
	LANG("German - Germany", "de_de", 0x0407),
	LANG("German - Austria", "de_at", 0x0C07),
	LANG("German - Liechtenstein", "de_li", 0x1407),
	LANG("German - Luxembourg", "de_lu", 0x1007),
	LANG("German - Switzerland", "de_ch", 0x0807),
	LANG("Greek", "el", 0x0408),
	LANG("Hebrew", "he", 0x040D),
	LANG("Hindi", "hi", 0x0439),
	LANG("Hungarian", "hu", 0x040E),
	LANG("Icelandic", "is", 0x040F),
	LANG("Indonesian", "id", 0x0421),
	LANG("Italian - Italy", "it_it", 0x0410),
	LANG("Italian - Switzerland", "it_ch", 0x0810),
	LANG("Japanese", "ja", 0x0411),
	LANG("Korean", "ko", 0x0412),
	LANG("Latvian", "lv", 0x0426),
	LANG("Lithuanian", "lt", 0x0427),
	LANG("F.Y.R.O. Macedonia", "mk", 0x042F),
	LANG("Malay - Malaysia", "ms_my", 0x043E),
	LANG("Malay – Brunei", "ms_bn", 0x083E),
	LANG("Maltese", "mt", 0x043A),
	LANG("Marathi", "mr", 0x044E),
	LANG("Norwegian - Bokml", "no_no", 0x0414),
	LANG("Norwegian - Nynorsk", "no_no", 0x0814),
	LANG("Polish", "pl", 0x0415),
	LANG("Portuguese - Portugal", "pt_pt", 0x0816),
	LANG("Portuguese - Brazil", "pt_br", 0x0416),
	LANG("Raeto-Romance", "rm", 0x0417),
	LANG("Romanian - Romania", "ro", 0x0418),
	LANG("Romanian - Republic of Moldova", "ro_mo", 0x0818),
	LANG("Russian", "ru", 0x0419),
	LANG("Russian - Republic of Moldova", "ru_mo", 0x0819),
	LANG("Sanskrit", "sa", 0x044F),
	LANG("Serbian - Cyrillic", "sr_sp", 0x0C1A),
	LANG("Serbian - Latin", "sr_sp", 0x081A),
	LANG("Setsuana", "tn", 0x0432),
	LANG("Slovenian", "sl", 0x0424),
	LANG("Slovak", "sk", 0x041B),
	LANG("Sorbian", "sb", 0x042E),
	LANG("Spanish - Spain (Traditional)", "es_es", 0x040A),
	LANG("Spanish - Argentina", "es_ar", 0x2C0A),
	LANG("Spanish - Bolivia", "es_bo", 0x400A),
	LANG("Spanish - Chile", "es_cl", 0x340A),
	LANG("Spanish - Colombia", "es_co", 0x240A),
	LANG("Spanish - Costa Rica", "es_cr", 0x140A),
	LANG("Spanish - Dominican Republic", "es_do", 0x1C0A),
	LANG("Spanish - Ecuador", "es_ec", 0x300A),
	LANG("Spanish - Guatemala", "es_gt", 0x100A),
	LANG("Spanish - Honduras", "es_hn", 0x480A),
	LANG("Spanish - Mexico", "es_mx", 0x080A),
	LANG("Spanish - Nicaragua", "es_ni", 0x4C0A),
	LANG("Spanish - Panama", "es_pa", 0x180A),
	LANG("Spanish - Peru", "es_pe", 0x280A),
	LANG("Spanish - Puerto Rico", "es_pr", 0x500A),
	LANG("Spanish - Paraguay", "es_py", 0x3C0A),
	LANG("Spanish - El Salvador", "es_sv", 0x440A),
	LANG("Spanish - Uruguay", "es_uy", 0x380A),
	LANG("Spanish - Venezuela", "es_ve", 0x200A),
	LANG("Southern Sotho", "st", 0x0430),
	LANG("Swahili", "sw", 0x0441),
	LANG("Swedish - Sweden", "sv_se", 0x041D),
	LANG("Swedish - Finland", "sv_fi", 0x081D),
	LANG("Tamil", "ta", 0x0449),
	LANG("Tatar", "tt", 0X0444),
	LANG("Thai", "th", 0x041E),
	LANG("Turkish", "tr", 0x041F),
	LANG("Tsonga", "ts", 0x0431),
	LANG("Ukrainian", "uk", 0x0422),
	LANG("Urdu", "ur", 0x0420),
	LANG("Uzbek - Cyrillic", "uz_uz", 0x0843),
	LANG("Uzbek – Latin", "uz_uz", 0x0443),
	LANG("Vietnamese", "vi", 0x042A),
	LANG("Xhosa", "xh", 0x0434),
	LANG("Yiddish", "yi", 0x043D),
	LANG("Zulu", "zu", 0x0435),
	LANG(NULL, NULL, 0x0),
};

uint16_t get_usb_code_for_current_locale(void)
{
	char *locale;
	char search_string[64];
	char *ptr;
	struct lang_map_entry *lang;

	/* Get the current locale. */
	locale = setlocale(0, NULL);
	if (!locale)
		return 0x0;

	/* Make a copy of the current locale string. */
	strncpy(search_string, locale, sizeof(search_string)-1);
	search_string[sizeof(search_string)-1] = '\0';

	/* Chop off the encoding part, and make it lower case. */
	ptr = search_string;
	while (*ptr) {
		*ptr = tolower(*ptr);
		if (*ptr == '.') {
			*ptr = '\0';
			break;
		}
		ptr++;
	}

	/* Find the entry which matches the string code of our locale. */
	lang = lang_map;
	while (lang->string_code) {
		if (!strcmp(lang->string_code, search_string)) {
			return lang->usb_code;
		}
		lang++;
	}

	/* There was no match. Find with just the language only. */
	/* Chop off the variant. Chop it off at the '_'. */
	ptr = search_string;
	while (*ptr) {
		*ptr = tolower(*ptr);
		if (*ptr == '_') {
			*ptr = '\0';
			break;
		}
		ptr++;
	}

#if 0 /* TODO: Do we need this? */
	/* Find the entry which matches the string code of our language. */
	lang = lang_map;
	while (lang->string_code) {
		if (!strcmp(lang->string_code, search_string)) {
			return lang->usb_code;
		}
		lang++;
	}
#endif

	/* Found nothing. */
	return 0x0;
}

#ifdef __cplusplus
}
#endif

#endif



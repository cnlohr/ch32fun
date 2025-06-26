/*	USB PD Library
 *	2025-06-21 Bogdan Ionescu
 *	Configuration:
 *		- USBPD_IMPLEMENTATION: Enable USB PD implementation
 *		- FUNCONF_USBPD_NO_STR: Disable string conversion functions
 *	Notes:
 *		- This library is based on the USB Power Delivery Specification.
 *			https://www.usb.org/document-library/usb-power-delivery
 *		- Packed bitfield structs are used for de/serialization of USB PD messages and
 *			are taken directly from the spec above.
 *		- Not all messages are implemented.
 *		- Formatting macros are provided next to the struct deffinitions.
 *	Basic usage:
 *		USBPD_VCC_e vcc = eUSBPD_VCC_5V0; // set the VCC voltage
 *		USBPD_Result_e result = USBPD_Init( vcc ); // initialize the peripheral
 *
 *		// wait for negotiation to complete.
 *		while ( eUSBPD_BUSY == ( result = USBPD_SinkNegotiate() ) );
 *
 *		USBPD_SPR_CapabilitiesMessage_t *capabilities;
 *		const size_t count = USBPD_GetCapabilities( &capabilities );
 *		USBPD_SelectPDO( count - 1, voltage ); // select the last supply (voltage is only used for PPS)
 *
 *	The above is not a complete example, check the funtion declarations below for more details.
 */

#pragma once

#include "ch32fun.h"
#include "funconfig.h"
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifndef PACKED
#define PACKED __attribute__( ( packed ) )
#endif

// USB PD spec type definitions
typedef enum PACKED
{
	eUSBPD_PORTDATAROLE_DFP = 0, // Downstream Facing Port
	eUSBPD_PORTDATAROLE_UFP = 1, // Upstream Facing Port
} USBPD_PortDataRole_e;
typedef enum PACKED
{
	eUSBPD_PORTPOWEROLE_SINK = 0, // Sink Power Role
	eUSBPD_PORTPOWEROLE_SOURCE = 1, // Source Power Role
} USBPD_PortPowerRole_e;

typedef enum PACKED
{
	eUSBPD_REV_10 = 0x00u, // Revision 1.0
	eUSBPD_REV_20 = 0x01u, // Revision 2.0
	eUSBPD_REV_30 = 0x02u, // Revision 3.0
} USBPD_SpecificationRevision_e;

typedef enum PACKED
{
	eUSBPD_CTRL_MSG_GOODCRC = 0x01u,
	eUSBPD_CTRL_MSG_GOTOMIN = 0x02u, // Depracated
	eUSBPD_CTRL_MSG_ACCEPT = 0x03u,
	eUSBPD_CTRL_MSG_REJECT = 0x04u,
	eUSBPD_CTRL_MSG_PING = 0x05u, // Deprecated
	eUSBPD_CTRL_MSG_PS_RDY = 0x06u,
	eUSBPD_CTRL_MSG_GET_SOURCE_CAP = 0x07u,
	eUSBPD_CTRL_MSG_GET_SINK_CAP = 0x08u,
	eUSBPD_CTRL_MSG_DR_SWAP = 0x09u,
	eUSBPD_CTRL_MSG_PR_SWAP = 0x0Au,
	eUSBPD_CTRL_MSG_VCONN_SWAP = 0x0Bu,
	eUSBPD_CTRL_MSG_WAIT = 0x0Cu,
	eUSBPD_CTRL_MSG_SOFT_RESET = 0x0Du,
	eUSBPD_CTRL_MSG_DATA_RESET = 0x0Eu,
	eUSBPD_CTRL_MSG_DATA_RESET_COMPLETE = 0x0Fu,
	eUSBPD_CTRL_MSG_NOT_SUPPORTED = 0x10u,
	eUSBPD_CTRL_MSG_GET_SOURCE_CAPEXT = 0x11u,
	eUSBPD_CTRL_MSG_GET_STATUS = 0x12u,
	eUSBPD_CTRL_MSG_FR_SWAP = 0x13u,
	eUSBPD_CTRL_MSG_GET_PPS_STATUS = 0x14u,
	eUSBPD_CTRL_MSG_GET_COUNTRY_CODES = 0x15u,
	eUSBPD_CTRL_MSG_GET_SINK_CAPEXT = 0x16u,
	eUSBPD_CTRL_MSG_GET_SOURCE_INFO = 0x17u,
	eUSBPD_CTRL_MSG_GET_REVISION = 0x18u,
} USBPD_ControlMessage_e;

typedef enum PACKED
{
	eUSBPD_DATA_MSG_SOURCE_CAP = 0x01u,
	eUSBPD_DATA_MSG_REQUEST = 0x02u,
	eUSBPD_DATA_MSG_BIST = 0x03u,
	eUSBPD_DATA_MSG_SINK_CAP = 0x04u,
	eUSBPD_DATA_MSG_BATTERY_STATUS = 0x05u,
	eUSBPD_DATA_MSG_ALERT = 0x06u,
	eUSBPD_DATA_MSG_GET_COUNTRY_INFO = 0x07u,
	eUSBPD_DATA_MSG_ENTER_USB = 0x08u,
	eUSBPD_DATA_MSG_EPR_REUEST = 0x09u,
	eUSBPD_DATA_MSG_EPR_MODE = 0x0Au,
	eUSBPD_DATA_MSG_SOURCE_INFO = 0x0Bu,
	eUSBPD_DATA_MSG_REVISION = 0x0Cu,
	eUSBPD_DATA_MSG_VENDOR_DEFINED = 0x0Fu
} USBPD_DataMessage_e;

typedef union
{
	uint16_t data;
	struct
	{
		uint16_t MessageType : 5u; // USBPD_ControlMessage_t | USBPD_DataMessage_t
		USBPD_PortDataRole_e PortDataRole : 1u;
		USBPD_SpecificationRevision_e SpecificationRevision : 2u;
		USBPD_PortPowerRole_e PortPowerRole : 1u;
		uint16_t MessageID : 3u;
		uint16_t NumberOfDataObjects : 3u; // 0: Control Message, >0: Data Message
		uint16_t Extended : 1u;
	};
} USBPD_MessageHeader_t;

typedef USBPD_MessageHeader_t USBPD_ControlMessage_t;
static_assert( sizeof( USBPD_MessageHeader_t ) == sizeof( uint16_t ), "USBPD_MessageHeader_t size mismatch" );

typedef enum PACKED
{
	eUSBPD_PDO_FIXED = 0,
	eUSBPD_PDO_BATTERY = 1,
	eUSBPD_PDO_VARIABLE = 2,
	eUSBPD_PDO_AUGMENTED = 3,
} USBPD_PowerDataObject_e;

typedef enum PACKED
{
	eUSBPD_APDO_SPR_PPS = 0, // Standard Power Range Programmable Power Supply
	eUSBPD_APDO_EPR_AVS = 1, // Extended Power Range Adjustable Voltage Supply
	eUSBPD_APDO_SPR_AVS = 2, // Standard Power Range Adjustable Voltage Supply
	eUSBPD_APDO_RESERVED = 3,
} USBPD_AugmentedPDO_e;

typedef enum PACKED
{
	eUSBPD_PEAK_CURRENT_0 = 0, /* Peak Current equals IoC */

	eUSBPD_PEAK_CURRENT_1 = 1, /* 150% IoC for 1ms @ 5% duty cycle
	                              125% IoC for 2ms @ 10% duty cycle
	                              110% IoC for 10ms @ 50% duty cycle */

	eUSBPD_PEAK_CURRENT_2 = 2, /* 200% IoC for 1ms @ 5% duty cycle
	                              150% IoC for 2ms @ 10% duty cycle
	                              125% IoC for 10ms @ 50% duty cycle */

	eUSBPD_PEAK_CURRENT_3 = 3, /* 200% IoC for 1ms @ 5% duty cycle
	                              175% IoC for 2ms @ 10% duty cycle
	                              150% IoC for 10ms @ 50% duty cycle */
} USBPD_PeakCurrent_e;

typedef struct
{
	uint32_t data : 28u; // PDO specific data based on PDO type
	USBPD_AugmentedPDO_e AugmentedType : 2u; // shall be eUSBPD_APDO_SPR_PPS
	USBPD_PowerDataObject_e PDOType : 2u; // shall be eUSBPD_PDO_AUGMENTED
} USBPD_PDOHeader_t;

typedef struct
{
	uint32_t MaxCurrentIn10mA : 10u;
	uint32_t VoltageIn50mV : 10u;
	USBPD_PeakCurrent_e PeakCurrent : 2u;
	uint32_t Reserved_22bit : 1u;
	uint32_t EPRModeCapable : 1u;
	uint32_t UnchunkedExtendedMessage : 1u;
	uint32_t DualRoleData : 1u;
	uint32_t USBCommunicationsCapable : 1u;
	uint32_t UnconstrainedPower : 1u;
	uint32_t USBSuspendSupported : 1u;
	uint32_t DualRolePower : 1u;
	USBPD_PowerDataObject_e PDOType : 2u; // shall be eUSBPD_PDO_FIXED
} USBPD_SourceFixedSupplyPDO_t;

#define FIXED_SUPPLY_FMT                 \
	"\nFixed Supply:\n"                  \
	"\tMax Current: %d mA\n"             \
	"\tVoltage: %d mV\n"                 \
	"\tPeak Current: %d\n"               \
	"\tEPR Mode Capable: %s\n"           \
	"\tUnchunked Extended Message: %s\n" \
	"\tDual Role Data: %s\n"             \
	"\tUSB Communications Capable: %s\n" \
	"\tUnconstrained Power: %s\n"        \
	"\tUSB Suspend Supported: %s\n"      \
	"\tDual Role Power: %s\n"

#define FIXED_SUPPLY_FMT_ARGS( pdo )                                                                  \
	( ( pdo )->FixedSupply.MaxCurrentIn10mA * 10 ), ( ( pdo )->FixedSupply.VoltageIn50mV * 50 ),      \
		( ( pdo )->FixedSupply.PeakCurrent ), ( ( pdo )->FixedSupply.EPRModeCapable ? "Yes" : "No" ), \
		( ( pdo )->FixedSupply.UnchunkedExtendedMessage ? "Yes" : "No" ),                             \
		( ( pdo )->FixedSupply.DualRoleData ? "Yes" : "No" ),                                         \
		( ( pdo )->FixedSupply.USBCommunicationsCapable ? "Yes" : "No" ),                             \
		( ( pdo )->FixedSupply.UnconstrainedPower ? "Yes" : "No" ),                                   \
		( ( pdo )->FixedSupply.USBSuspendSupported ? "Yes" : "No" ),                                  \
		( ( pdo )->FixedSupply.DualRolePower ? "Yes" : "No" )

typedef struct
{
	uint32_t MaxCurrentIn10mA : 10u;
	uint32_t MinVoltageIn50mV : 10u;
	uint32_t MaxVoltageIn50mV : 10u;
	USBPD_PowerDataObject_e PDOType : 2u; // shall be eUSBPD_PDO_VARIABLE
} USBPD_VariablePDO_t;

#define VARIABLE_SUPPLY_FMT  \
	"\nVariable Supply:\n"   \
	"\tMax Current: %d mA\n" \
	"\tMin Voltage: %d mV\n" \
	"\tMax Voltage: %d mV\n"

#define VARIABLE_SUPPLY_FMT_ARGS( pdo )                                                                   \
	( ( pdo )->VariableSupply.MaxCurrentIn10mA * 10 ), ( ( pdo )->VariableSupply.MinVoltageIn50mV * 50 ), \
		( ( pdo )->VariableSupply.MaxVoltageIn50mV * 50 )

typedef struct
{
	uint32_t MaxPowerIn250mW : 10u;
	uint32_t MinVoltageIn50mV : 10u;
	uint32_t MaxVoltageIn50mV : 10u;
	USBPD_PowerDataObject_e PDOType : 2u; // shall be eUSBPD_PDO_BATTERY
} USBPD_BatteryPDO_t;

#define BATTERY_SUPPLY_FMT   \
	"\nBattery Supply:\n"    \
	"\tMax Power: %d mW\n"   \
	"\tMin Voltage: %d mV\n" \
	"\tMax Voltage: %d mV\n"

#define BATTERY_SUPPLY_FMT_ARGS( pdo )                                                                  \
	( ( pdo )->BatterySupply.MaxPowerIn250mW * 250 ), ( ( pdo )->BatterySupply.MinVoltageIn50mV * 50 ), \
		( ( pdo )->BatterySupply.MaxVoltageIn50mV * 50 )

typedef struct
{
	uint32_t MaxCurrentIn50mA : 7u;
	uint32_t Reserved_7bit : 1u; // shall be set to zero
	uint32_t MinVoltageIn100mV : 8u;
	uint32_t Reserved_16bit : 1u; // shall be set to zero
	uint32_t MaxVoltageIn100mV : 8u;
	uint32_t Reserved_25_26bit : 2u; // shall be set to zero
	uint32_t PPSpowerLimited : 1u;
	USBPD_AugmentedPDO_e AugmentedType : 2u; // shall be eUSBPD_APDO_SPR_PPS
	USBPD_PowerDataObject_e PDOType : 2u; // shall be eUSBPD_PDO_AUGMENTED
} USBPD_SPR_PPS_APDO_t;

#define SPR_PPS_FMT          \
	"\nPPS Supply:\n"        \
	"\tMax Current: %d mA\n" \
	"\tMin Voltage: %d mV\n" \
	"\tMax Voltage: %d mV\n" \
	"\tPPS Power Limited: %s\n"

#define SPR_PPS_FMT_ARGS( pdo )                                                               \
	( ( pdo )->SPR_PPS.MaxCurrentIn50mA * 50 ), ( ( pdo )->SPR_PPS.MinVoltageIn100mV * 100 ), \
		( ( pdo )->SPR_PPS.MaxVoltageIn100mV * 100 ), ( ( pdo )->SPR_PPS.PPSpowerLimited ? "Yes" : "No" )

typedef struct
{
	uint32_t PDPIn1W : 8u;
	uint32_t MinVoltageIn100mV : 8u;
	uint32_t Reserved_16bit : 1u; // shall be set to zero
	uint32_t MaxVoltageIn100mV : 9u;
	USBPD_PeakCurrent_e PeakCurrent : 2u;

	USBPD_AugmentedPDO_e AugmentedType : 2u; // shall be eUSBPD_APDO_EPR_AVS
	USBPD_PowerDataObject_e PDOType : 2u; // shall be eUSBPD_PDO_AUGMENTED
} USBPD_EPR_AVS_APDO_t;

#define EPR_AVS_FMT          \
	"\nEPR AVS Supply:\n"    \
	"\tPDP: %d W\n"          \
	"\tMin Voltage: %d mV\n" \
	"\tMax Voltage: %d mV\n" \
	"\tPeak Current: %d\n"

#define EPR_AVS_FMT_ARGS( pdo )                                                 \
	( ( pdo )->EPR_AVS.PDPIn1W ), ( ( pdo )->EPR_AVS.MinVoltageIn100mV * 100 ), \
		( ( pdo )->EPR_AVS.MaxVoltageIn100mV * 100 ), ( ( pdo )->EPR_AVS.PeakCurrent )

typedef struct
{
	uint32_t MaxCurrent15To20VIn10mA : 10u;
	uint32_t MaxCurrent9to15VIn10mA : 10u;
	uint32_t Reserved_20_25bit : 6u; // shall be set to zero
	USBPD_PeakCurrent_e PeakCurrent : 2u;
	USBPD_AugmentedPDO_e AugmentedType : 2u; // shall be eUSBPD_APDO_SPR_AVS
	USBPD_PowerDataObject_e PDOType : 2u; // shall be eUSBPD_PDO_AUGMENTED
} USBPD_SPR_AVS_APDO_t;

#define SPR_AVS_FMT                 \
	"\nSPR AVS Supply:\n"           \
	"\tMax Current 15-20V: %d mA\n" \
	"\tMax Current 9-15V: %d mA\n"  \
	"\tPeak Current: %d\n"

#define SPR_AVS_FMT_ARGS( pdo )                                                                          \
	( ( pdo )->SPR_AVS.MaxCurrent15To20VIn10mA * 10 ), ( ( pdo )->SPR_AVS.MaxCurrent9to15VIn10mA * 10 ), \
		( ( pdo )->SPR_AVS.PeakCurrent )

typedef union
{
	USBPD_PDOHeader_t Header;
	USBPD_SourceFixedSupplyPDO_t FixedSupply;
	USBPD_VariablePDO_t VariableSupply;
	USBPD_BatteryPDO_t BatterySupply;
	USBPD_SPR_PPS_APDO_t SPR_PPS;
	USBPD_EPR_AVS_APDO_t EPR_AVS;
	USBPD_SPR_AVS_APDO_t SPR_AVS;
} USBPD_SourcePDO_t;

static_assert( sizeof( USBPD_SourcePDO_t ) == sizeof( uint32_t ), "USBPD_SourcePDO_t size mismatch" );

typedef enum // Do not PACK, messes up alignment
{
	eUSBPD_FAST_ROLE_SWAP_NOT_SUPPORTED = 0,
	eUSBPD_FAST_ROLE_SWAP_DEFAULT = 1,
	eUSBPD_FAST_ROLE_SWAP_1A5 = 2, // 1.5A @ 5V
	eUSBPD_FAST_ROLE_SWAP_3A = 3, // 3A @ 5V
} USBPD_FastRoleSwapRequiredCurrent_e;

typedef struct
{
	uint32_t CurrentIn10mA : 10u;
	uint32_t VoltageIn50mV : 10u;
	uint32_t Reserved_20_22bit : 3u; // shall be set to zero
	USBPD_FastRoleSwapRequiredCurrent_e FastRoleSwap : 2u;
	uint32_t DualRoleData : 1u;
	uint32_t USBComsCapable : 1u;
	uint32_t UnconstrainedPower : 1u;
	uint32_t HigherCapability : 1u;
	uint32_t DualRolePower : 1u;
	USBPD_PowerDataObject_e PDOType : 2u; // shall be eUSBPD_PDO_FIXED
} USBPD_SinkFixedSupplyPDO_t;

typedef union
{
	USBPD_PDOHeader_t Header;
	USBPD_SinkFixedSupplyPDO_t FixedSupply;
	USBPD_VariablePDO_t VariableSupply;
	USBPD_BatteryPDO_t BatterySupply;
	USBPD_SPR_PPS_APDO_t SPR_PPS;
	USBPD_EPR_AVS_APDO_t EPR_AVS;
	USBPD_SPR_AVS_APDO_t SPR_AVS;
} USBPD_SinkPDO_t;

static_assert( sizeof( USBPD_SourcePDO_t ) == sizeof( uint32_t ), "USBPD_SourcePDO_t size mismatch" );
static_assert( sizeof( USBPD_SinkPDO_t ) == sizeof( uint32_t ), "USBPD_SinkPDO_t size mismatch" );

typedef union
{
	USBPD_SourcePDO_t Source[7];
	USBPD_SinkPDO_t Sink[7];
} USBPD_SPR_CapabilitiesMessage_t;

static_assert( sizeof( USBPD_SPR_CapabilitiesMessage_t ) == ( 7 * sizeof( uint32_t ) ),
	"USBPD_SPR_CapabilitiesMessage_t size mismatch" );

typedef struct
{
	uint32_t MaxCurrentIn10mA : 10u;
	uint32_t OperatingCurrentIn10mA : 10u;
	uint32_t Reserved_20_21bit : 2u; // shall be set to zero
	uint32_t ERPCapable : 1u;
	uint32_t UnchunkedExtendedMessage : 1u;
	uint32_t NoUSBSuspended : 1u;
	uint32_t USBComsCapable : 1u;
	uint32_t CapabilityMissmatch : 1u;
	uint32_t Giveback : 1u; // Deprecated, shall be set to zero
	uint32_t ObjectPosition : 4u; // Reserved and shall not be used
} USBPD_FixedAndVariableRDO_t;

typedef struct
{
	uint32_t MaxPowerIn250mW : 10u;
	uint32_t OperatingPowerIn250mW : 10u;
	uint32_t Reserved_20_21bit : 2u; // shall be set to zero
	uint32_t ERPCapable : 1u;
	uint32_t UnchunkedExtendedMessage : 1u;
	uint32_t NoUSBSuspended : 1u;
	uint32_t USBComsCapable : 1u;
	uint32_t CapabilityMissmatch : 1u;
	uint32_t Giveback : 1u; // Deprecated, shall be set to zero
	uint32_t ObjectPosition : 4u; // Reserved and shall not be used
} USBPD_BatteryRDO_t;

typedef struct
{
	uint32_t OperatingCurrentIn50mA : 7u;
	uint32_t Reserved_7_8bit : 2u; // shall be set to zero
	uint32_t OutputVoltageIn20mV : 12u;
	uint32_t Reserved_21bit : 1u; // shall be set to zero
	uint32_t ERPCapable : 1u;
	uint32_t UnchunkedExtendedMessage : 1u;
	uint32_t NoUSBSuspended : 1u;
	uint32_t USBComsCapable : 1u;
	uint32_t CapabilityMissmatch : 1u;
	uint32_t Reserved_27bit : 1u; // Deprecated, shall be set to zero
	uint32_t ObjectPosition : 4u; // Reserved and shall not be used
} USBPD_PPS_RDO_t;

typedef struct
{
	uint32_t OperatingCurrentIn50mA : 7u;
	uint32_t Reserved_7_8bit : 2u; // shall be set to zero
	uint32_t OutputVoltageIn100mV : 12u; // NOTE: Output voltage in 25mV units, the least two significant bits Shall
	                                     // be set to zero making the effective voltage step size 100mV.
	uint32_t Reserved_21bit : 1u; // shall be set to zero
	uint32_t ERPCapable : 1u;
	uint32_t UnchunkedExtendedMessage : 1u;
	uint32_t NoUSBSuspended : 1u;
	uint32_t USBComsCapable : 1u;
	uint32_t CapabilityMissmatch : 1u;
	uint32_t Reserved_27bit : 1u; // Deprecated, shall be set to zero
	uint32_t ObjectPosition : 4u; // Reserved and shall not be used
} USBPD_AVS_RDO_t;

typedef union
{
	USBPD_FixedAndVariableRDO_t FixedAndVariable;
	USBPD_BatteryRDO_t Battery;
	USBPD_PPS_RDO_t PPS;
} USBPD_RequestDataObject_t;

static_assert( sizeof( USBPD_RequestDataObject_t ) == sizeof( uint32_t ), "USBPD_RequestDataObject_t size mismatch" );

// TODO: Define the rest of the message types sections 6.4.3 -> 6.5.16

typedef enum
{
	eUSBPD_MAX_EXTENDED_MSG_LEN = 260,
	eUSBPD_MAX_EXTENDED_MSG_CHUNK_LEN = 26,
	eUSBPD_MAX_EXTENDED_MSG_LEGACY_LEN = 26,
} USBPD_ValueParameters_t;

typedef enum
{
	eUSBPD_OK = 0,
	eUSBPD_BUSY,
	eUSBPD_ERROR,
	eUSBPD_ERROR_ARGS,
	eUSBPD_ERROR_NOT_SUPPORTED,
	eUSBPD_ERROR_TIMEOUT,
} USBPD_Result_e;

typedef enum
{
	eUSBPD_VCC_3V3 = 0,
	eUSBPD_VCC_5V0 = 1,
} USBPD_VCC_e;

typedef enum
{
	eUSBPD_CCNONE = 0,
	eUSBPD_CC1 = 1,
	eUSBPD_CC2 = 2,
} USBPD_CC_e;

typedef enum
{
	eSTATE_IDLE,
	eSTATE_CABLE_DETECT,
	eSTATE_SOURCE_CAP,
	eSTATE_WAIT_ACCEPT,
	eSTATE_WAIT_PS_RDY,
	eSTATE_PS_RDY,
	eSTATE_MAX,
} USBPD_State_e;

/**
 * @brief  Initialize the USB PD module
 * @param  vcc: VCC voltage level (3.3V or 5V)
 * @return USBPD_Result_e
 */
USBPD_Result_e USBPD_Init( USBPD_VCC_e vcc );

/**
 * @brief  Negotiate with the USB PD Source, must be called periodically
 * @param  None
 * @return USBPD_BUSY if negotiation is in progress, eUSBPD_OK if successful, or an error code
 */
USBPD_Result_e USBPD_SinkNegotiate( void );

/**
 * @brief  Reset the USB PD module
 * @param  None
 * @return None
 */
void USBPD_Reset( void );

/**
 * @brief  Get the current state of the USB PD module
 * @param  None
 * @return USBPD_State_e representing the current state of the module
 */
USBPD_State_e USBPD_GetState( void );

/**
 * @brief  Convert USB PD state to string
 * @param state: USBPD_State_e to convert
 * @return Pointer to a string representing the state
 */
const char *USBPD_StateToStr( USBPD_State_e state );

/**
 * @brief  Convert USB PD result to string
 * @param  result: USBPD_Result_e to convert
 * @return Pointer to a string representing the result
 */
const char *USBPD_ResultToStr( USBPD_Result_e result );

/**
 * @brief  Select a new Power Data Object (PDO) to be used. No re-negotiation is needed.
 * @param  index: Index of the PDO to select (0-based)
 * @param  voltageIn100mV: Desired output voltage in 100mV units (e.g., 500 for 5V) (only applicable for PPS)
 * @return USBPD_Result_e
 */
USBPD_Result_e USBPD_SelectPDO( uint8_t index, uint32_t voltageIn100mV );

/**
 * @brief  Get the capabilities of the USB PD Source
 * @param[out] capabilities: Pointer to a pointer where the capabilities message structure is stored
 * @return Number of Power Data Objects (PDOs) in the capabilities message
 */
size_t USBPD_GetCapabilities( USBPD_SPR_CapabilitiesMessage_t **capabilities );

/**
 * @brief  Check if the Power Data Object is a Programmable Power Supply (PPS)
 * @param[in] pdo: Pointer to the Power Data Object to check
 * @return true if the PDO is a PPS, false otherwise
 */
bool USBPD_IsPPS( const USBPD_SourcePDO_t *pdo );

/**
 * @brief  Get the USB PD Specification Revision
 * @param  None
 * @return USBPD_SpecificationRevision_e representing the USB PD specification revision
 */
USBPD_SpecificationRevision_e USBPD_GetVersion( void );

#if defined( USBPD_IMPLEMENTATION )

#include <string.h>

typedef struct
{
	uint32_t ccCount;
	volatile USBPD_State_e state;
	USBPD_SpecificationRevision_e pdVersion;
	USBPD_CC_e lastCCLine;
	USBPD_SPR_CapabilitiesMessage_t caps;
	uint8_t messageID;
	uint8_t pdoCount;
	bool gotSourceGoodCRC;
} USBPD_Instance_t;

static __attribute__( ( aligned( 4 ) ) ) uint8_t s_buffer[34];
static USBPD_Instance_t s_instance = {
	.pdVersion = eUSBPD_REV_30,
};

static USBPD_CC_e GetActiveCCLine( void );
static void SwitchRXMode( void );
static void SendMessage( uint8_t size );
static void ParsePacket( void );

USBPD_Result_e USBPD_Init( USBPD_VCC_e vcc )
{
	RCC->APB2PCENR |= RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO;
	RCC->AHBPCENR |= RCC_USBPD;

	GPIOC->CFGHR &= ~( 0xf << ( ( 14 & 7 ) << 2 ) );
	GPIOC->CFGHR |= ( GPIO_Speed_10MHz | GPIO_CNF_OUT_OD ) << ( ( 14 & 7 ) << 2 );
	GPIOC->CFGHR &= ~( 0xf << ( ( 15 & 7 ) << 2 ) );
	GPIOC->CFGHR |= ( GPIO_Speed_10MHz | GPIO_CNF_OUT_OD ) << ( ( 15 & 7 ) << 2 );

	AFIO->CTLR |= USBPD_IN_HVT;
	if ( vcc == eUSBPD_VCC_3V3 )
	{
		AFIO->CTLR |= USBPD_PHY_V33;
	}

	USBPD->DMA = (uint32_t)s_buffer;
	USBPD->CONFIG = IE_RX_ACT | IE_RX_RESET | IE_TX_END | PD_DMA_EN | PD_FILT_EN;
	USBPD->STATUS = BUF_ERR | IF_RX_BIT | IF_RX_BYTE | IF_RX_ACT | IF_RX_RESET | IF_TX_END;

	// disable CC comparators
	USBPD->PORT_CC1 &= ~( CC_CMP_MASK | PA_CC_AI );
	USBPD->PORT_CC2 &= ~( CC_CMP_MASK | PA_CC_AI );
	// set CC comparator voltage
	USBPD->PORT_CC1 |= CC_CMP_66;
	USBPD->PORT_CC2 |= CC_CMP_66;

	return eUSBPD_OK;
}

USBPD_Result_e USBPD_SinkNegotiate( void )
{
	switch ( s_instance.state )
	{
		case eSTATE_IDLE:;
			const uint8_t ccLine = GetActiveCCLine();
			if ( ccLine == eUSBPD_CCNONE )
			{
				s_instance.ccCount = 0;
				s_instance.lastCCLine = eUSBPD_CCNONE;
				break;
			}

			if ( s_instance.lastCCLine != ccLine )
			{
				s_instance.lastCCLine = ccLine;
				s_instance.ccCount = 0;
			}
			else
			{
				s_instance.ccCount++;
			}

			if ( s_instance.ccCount > 10 )
			{
				if ( ccLine == eUSBPD_CC2 )
				{
					USBPD->CONFIG |= CC_SEL;
				}
				else
				{
					USBPD->CONFIG &= ~CC_SEL;
				}

				s_instance.ccCount = 0;
				s_instance.state = eSTATE_CABLE_DETECT;

				SwitchRXMode();
				// NVIC_SetPriority(USBPD_IRQn, 0x00); // TODO: Is this needed?
				NVIC_EnableIRQ( USBPD_IRQn );
			}
			break;

		case eSTATE_SOURCE_CAP:
			USBPD_SelectPDO( 0, 0 ); // Select the first PDO by default
			s_instance.state = eSTATE_WAIT_ACCEPT;
			break;

		case eSTATE_PS_RDY: return eUSBPD_OK;

		default: break;
	}

	return eUSBPD_BUSY;
}

void USBPD_Reset( void )
{
	NVIC_DisableIRQ( USBPD_IRQn );
	s_instance = ( USBPD_Instance_t ){
		.pdVersion = eUSBPD_REV_30,
	};
}

USBPD_State_e USBPD_GetState( void )
{
	return s_instance.state;
}

#if FUNCONF_USBPD_NO_STR
const char *USBPD_StateToStr( USBPD_State_e state )
{
	(void)state;
	return "";
}

const char *USBPD_ResultToStr( USBPD_Result_e result )
{
	(void)result;
	return "";
}
#else
const char *USBPD_StateToStr( USBPD_State_e state )
{
	switch ( state )
	{
		case eSTATE_IDLE: return "Idle";
		case eSTATE_CABLE_DETECT: return "Cable Detected";
		case eSTATE_SOURCE_CAP: return "Got Source Capabilities";
		case eSTATE_WAIT_ACCEPT: return "Waiting for Accept";
		case eSTATE_WAIT_PS_RDY: return "Waiting for PS_Ready";
		case eSTATE_PS_RDY: return "Power Supply Ready";
		default: return "Unknown State";
	}
};

const char *USBPD_ResultToStr( USBPD_Result_e result )
{
	switch ( result )
	{
		case eUSBPD_OK: return "OK";
		case eUSBPD_BUSY: return "Busy";
		case eUSBPD_ERROR: return "Error";
		case eUSBPD_ERROR_ARGS: return "Error Args";
		case eUSBPD_ERROR_NOT_SUPPORTED: return "Error Not Supported";
		case eUSBPD_ERROR_TIMEOUT: return "Error Timeout";
		default: return "Unknown Result";
	}
}
#endif // FUNCONF_USBPD_NO_STR

USBPD_Result_e USBPD_SelectPDO( uint8_t index, uint32_t voltageIn100mV )
{
	if ( index >= s_instance.pdoCount )
	{
		return eUSBPD_ERROR_ARGS;
	}

	const USBPD_SourcePDO_t *const pdo = &s_instance.caps.Source[index];

	*(USBPD_MessageHeader_t *)&s_buffer[0] = ( USBPD_MessageHeader_t ){
		.MessageID = s_instance.messageID,
		.MessageType = eUSBPD_DATA_MSG_REQUEST,
		.NumberOfDataObjects = 1u,
		.SpecificationRevision = s_instance.pdVersion,
	};
	USBPD_RequestDataObject_t *const rdo = (USBPD_RequestDataObject_t *)&s_buffer[sizeof( USBPD_MessageHeader_t )];

	if ( USBPD_IsPPS( pdo ) )
	{
		// Clamp voltage to min/max NOTE: Maybe we should return an error if the voltage is out of range?
		const uint32_t minVoltage = pdo->SPR_PPS.MinVoltageIn100mV;
		const uint32_t maxVoltage = pdo->SPR_PPS.MaxVoltageIn100mV;
		voltageIn100mV = voltageIn100mV > maxVoltage ? maxVoltage : voltageIn100mV;
		voltageIn100mV = voltageIn100mV < minVoltage ? minVoltage : voltageIn100mV;

		*rdo = ( USBPD_RequestDataObject_t ){
			.PPS =
				{
					.ObjectPosition = index + 1,
					.OutputVoltageIn20mV = voltageIn100mV * 5,
					.OperatingCurrentIn50mA = pdo->SPR_PPS.MaxCurrentIn50mA,
					.NoUSBSuspended = 1u,
					.USBComsCapable = 1u, // TODO: Should have these are arguments or define
				},
		};
	}
	else
	{
		*rdo = ( USBPD_RequestDataObject_t ){
			.FixedAndVariable =
				{
					.ObjectPosition = index + 1,
					.MaxCurrentIn10mA = pdo->FixedSupply.MaxCurrentIn10mA,
					.OperatingCurrentIn10mA = pdo->FixedSupply.MaxCurrentIn10mA,
					.USBComsCapable = 1u,
					.NoUSBSuspended = 1u,
				},
		};
	}

	SendMessage( 6 );

	return eUSBPD_OK;
}

/**
 * @brief  Get the capabilities of the USB PD Source
 * @param[out] capabilities: pointer to the capabilities message structure
 * @return Number of Power Data Objects (PDOs) in the capabilities message
 */
size_t USBPD_GetCapabilities( USBPD_SPR_CapabilitiesMessage_t **capabilities )
{
	if ( s_instance.pdoCount == 0 )
	{
		return 0;
	}

	if ( capabilities )
	{
		*capabilities = &s_instance.caps;
	}

	return s_instance.pdoCount;
}

bool USBPD_IsPPS( const USBPD_SourcePDO_t *pdo )
{
	return ( pdo->Header.PDOType == eUSBPD_PDO_AUGMENTED ) && ( pdo->Header.AugmentedType == eUSBPD_APDO_SPR_PPS );
}

USBPD_SpecificationRevision_e USBPD_GetVersion( void )
{
	return s_instance.pdVersion;
}

/**
 * @brief  Check CC line status
 * @param  None
 * @return USBPD_CC_t
 */
static USBPD_CC_e GetActiveCCLine( void )
{
	// Switch to CC1
	USBPD->CONFIG &= ~CC_SEL;
	Delay_Us( 1 );
	// check if CC1 is connected
	if ( USBPD->PORT_CC1 & PA_CC_AI )
	{
		return eUSBPD_CC1;
	}

	// Switch to CC2
	USBPD->CONFIG |= CC_SEL;
	Delay_Us( 1 );
	if ( USBPD->PORT_CC2 & PA_CC_AI )
	{
		return eUSBPD_CC2;
	}

	return eUSBPD_CCNONE;
}

/**
 * @brief  Switch to RX mode
 * @param  None
 * @return None
 */
static void SwitchRXMode( void )
{
	USBPD->BMC_CLK_CNT = UPD_TMR_RX;
	USBPD->CONTROL = ( USBPD->CONTROL & ~PD_TX_EN ) | BMC_START;
}

/**
 * @brief  Begin transmission of PD message
 * @param  size: size of the message in bytes
 * @return None
 */
static void SendMessage( uint8_t size )
{
	USBPD->BMC_CLK_CNT = UPD_TMR_TX;
	USBPD->TX_SEL = UPD_SOP0;
	USBPD->BMC_TX_SZ = size;
	USBPD->STATUS = 0;
	USBPD->CONTROL |= BMC_START | PD_TX_EN;
}

/**
 * @brief  Parse the received packet
 * @param  None
 * @return None
 */
static void ParsePacket( void )
{
	bool sendGoodCRC = true;
	USBPD_MessageHeader_t message = *(USBPD_MessageHeader_t *)s_buffer;

	USBPD_State_e nextState = s_instance.state;

	if ( message.NumberOfDataObjects == 0u )
	{
		switch ( (USBPD_ControlMessage_e)message.MessageType )
		{
			case eUSBPD_CTRL_MSG_GOODCRC:
				sendGoodCRC = false;
				s_instance.messageID++;
				break;

			case eUSBPD_CTRL_MSG_ACCEPT: nextState = eSTATE_WAIT_PS_RDY; break;

			case eUSBPD_CTRL_MSG_REJECT: nextState = eSTATE_SOURCE_CAP; break;

			case eUSBPD_CTRL_MSG_PS_RDY: nextState = eSTATE_PS_RDY; break;

			default: break;
		}
	}
	else
	{
		switch ( (USBPD_DataMessage_e)message.MessageType )
		{

			case eUSBPD_DATA_MSG_SOURCE_CAP:
				nextState = eSTATE_SOURCE_CAP;
				s_instance.pdoCount = message.NumberOfDataObjects;
				s_instance.pdVersion = message.SpecificationRevision;
				memcpy( &s_instance.caps, &s_buffer[2], sizeof( USBPD_SPR_CapabilitiesMessage_t ) );
				break;

			default: break;
		}
	}

	if ( message.Extended || sendGoodCRC )
	{
		Delay_Us( 30 );
		USBPD_ControlMessage_t reply = ( USBPD_ControlMessage_t ){
			.MessageID = message.MessageID,
			.MessageType = eUSBPD_CTRL_MSG_GOODCRC,
			.SpecificationRevision = s_instance.pdVersion,
		};
		*(uint16_t *)&s_buffer[0] = reply.data;
		SendMessage( sizeof( reply ) );
	}

	s_instance.state = nextState;
}

void USBPD_IRQHandler( void ) __attribute__( ( interrupt ) );
void USBPD_IRQHandler( void )
{
	// Receive complete interrupt
	if ( USBPD->STATUS & IF_RX_ACT )
	{
		// Check if we received a SOP0 packet
		if ( ( ( USBPD->STATUS & BMC_AUX_MASK ) == BMC_AUX_SOP0 ) && ( USBPD->BMC_BYTE_CNT >= 6 ) )
		{
			ParsePacket();
		}
		USBPD->STATUS |= IF_RX_ACT;
	}

	// Transmit complete interrupt (GoodCRC only)
	if ( USBPD->STATUS & IF_TX_END )
	{
		SwitchRXMode();
		USBPD->STATUS |= IF_TX_END;
	}

	// Reset interrupt
	if ( USBPD->STATUS & IF_RX_RESET )
	{
		USBPD->STATUS |= IF_RX_RESET;
	}
}

#endif

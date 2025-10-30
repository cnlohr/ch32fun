#ifndef _MINICHLINK_H
#define _MINICHLINK_H

#include <stdint.h>
// #include "chips.h"

enum RAMSplit;

struct MiniChlinkFunctions
{
	// All functions return 0 if OK, negative number if fault, positive number as status code.

	// Low-level functions, if they exist.
	int (*WriteReg32)( void * dev, uint8_t reg_7_bit, uint32_t command );
	int (*ReadReg32)( void * dev, uint8_t reg_7_bit, uint32_t * commandresp );
	int (*FlushLLCommands)( void * dev );
	int (*DelayUS)( void * dev, int microseconds );

	// Higher-level functions can be generated automatically.
	int (*SetupInterface)( void * dev );
	int (*Control3v3)( void * dev, int bOn );
	int (*Control5v)( void * dev, int bOn );
	int (*Unbrick)( void * dev ); // Turns on chip, erases everything, powers off.

	int (*DetermineChipType)( void * dev ); // Determine chip type (may need to pause chip, so only do if you really need to know)

	int (*Exit)( void * dev );

	int (*HaltMode)( void * dev, int mode ); //0 for halt, 1 for reset, 2 for resume
	int (*ConfigureNRSTAsGPIO)( void * dev, int one_if_yes_gpio );
	int (*ConfigureReadProtection)( void * dev, int one_if_yes_protect );
	int (*SetSplit)( void * dev, enum RAMSplit split );

	// No boundary or limit rules.  Must support any combination of alignment and size.
	int (*WriteBinaryBlob)( void * dev, uint32_t address_to_write, uint32_t blob_size, const uint8_t * blob );
	int (*ReadBinaryBlob)( void * dev, uint32_t address_to_read_from, uint32_t read_size, uint8_t * blob );

	int (*Erase)( void * dev, uint32_t address, uint32_t length, int type ); //type = 0 for fast, 1 for whole-chip

	// MUST be 4-byte-aligned.
	int (*VoidHighLevelState)( void * dev );
	int (*WriteWord)( void * dev, uint32_t address_to_write, uint32_t data );
	int (*ReadWord)( void * dev, uint32_t address_to_read, uint32_t * data );

	// Debugging operations.
	//  Note: You must already be in break mode to use these otherwise they
	//  will return nonsensical data.
	// For x0...xN, use 0x1000 + regno.
	// For PC, use 0x7b1
	int (*ReadCPURegister)( void * dev, uint32_t regno, uint32_t * regret );
	int (*WriteCPURegister)( void * dev, uint32_t regno, uint32_t regval );

	// Actually returns 17 registers (All 16 CPU registers + the debug register)
	int (*ReadAllCPURegisters)( void * dev, uint32_t * regret );
	int (*WriteAllCPURegisters)( void * dev, uint32_t * regret );

	int (*SetEnableBreakpoints)( void * dev, int halt_on_break, int single_step );

	int (*PrepForLongOp)( void * dev ); // Called before the command that will take a while.
	int (*WaitForFlash)( void * dev );
	int (*WaitForDoneOp)( void * dev, int ignore );

	int (*PrintChipInfo)( void * dev );

	// Geared for flash, but could be anything.  Note: If in flash, must also erase.
	int (*BlockWrite64)( void * dev, uint32_t address_to_write, const uint8_t * data );

	// Returns positive if received text.
	// Returns negative if error.
	// Returns 0 if no text waiting.
	// Note: YOU CANNOT make lsb of leaveflagA bit in place 0x80 be high!!!
	int (*PollTerminal)( void * dev, uint8_t * buffer, int maxlen, uint32_t leaveflagA, int leaveflagB );

	int (*PerformSongAndDance)( void * dev );

	int (*VendorCommand)( void * dev, const char * command );

	// Probably no need to override these.  The base layer handles them.
	int (*WriteHalfWord)( void * dev, uint32_t address_to_write, uint16_t data );
	int (*ReadHalfWord)( void * dev, uint32_t address_to_read, uint16_t * data );

	int (*WriteByte)( void * dev, uint32_t address_to_write, uint8_t data );
	int (*ReadByte)( void * dev, uint32_t address_to_read, uint8_t * data );
	int (*GetUUID)( void * dev, uint8_t * buffer );
	int (*SetClock)( void * dev, uint32_t clock );
	int (*EnableDebug)( void * dev, uint8_t disable );
	int (*ResetInterface)( void * dev );
};

/** If you are writing a driver, the minimal number of functions you can implement are:
	WriteReg32
	ReadReg32
	FlushLLCommands
*/

// inline static int IsAddressFlash( uint32_t addy ) { return ( addy & 0xff000000 ) == 0x08000000 || ( addy & 0x1FFF0000 ) == 0x1FFF0000; }
inline static int IsAddressFlash( uint32_t addy ) { return !( addy & 0xe0000000 ); }

#define HALT_MODE_HALT_AND_RESET    0
#define HALT_MODE_REBOOT            1
#define HALT_MODE_RESUME            2
#define HALT_MODE_GO_TO_BOOTLOADER  3
#define HALT_MODE_HALT_BUT_NO_RESET 5

// Convert a 4-character string to an int.
#define STTAG( x ) (*((uint32_t*)(x)))

struct InternalState;

struct ProgrammerStructBase
{
	struct InternalState * internal;
	// You can put other things here.
};

#define MAX_FLASH_SECTORS 262144

enum RiscVChip {
	CHIP_UNKNOWN = 0x00,
	CHIP_CH32V10x = 0x01,
	CHIP_CH57x = 0x02,
	CHIP_CH56x = 0x03,
	CHIP_CH32V20x = 0x05,
	CHIP_CH32V30x = 0x06,
	CHIP_CH58x = 0x07,
	CHIP_CH32V003 = 0x09,
	CHIP_CH59x = 0x0b,
	CHIP_CH643 = 0x0c,
	CHIP_CH32X03x = 0x0d,
	CHIP_CH32L103 = 0x0e,
	CHIP_CH564 = 0x0f,

	CHIP_CH32V00x = 0x4e,

	CHIP_CH570 = 0x8b,
	CHIP_CH585 = 0x4b,
	CHIP_CH645 = 0x46,
	CHIP_CH641 = 0x49,
	CHIP_CH32V317 = 0x86,
	CHIP_CH32M030 = 0x8e,
	CHIP_CH32H41x = 0xc6,
	CHIP_CH32V205 = 0xce,
	
};

enum RAMSplit {
	// For supported V30x and some V20x devices
	FLASH_192_RAM_128 = 0x00,
	FLASH_224_RAM_96  = 0x01,
	FLASH_256_RAM_64  = 0x02,
	FLASH_288_RAM_32  = 0x03,

	// For some V20x devices
	FLASH_128_RAM_64  = 0x10,
	FLASH_144_RAM_48  = 0x11,
	FLASH_160_RAM_32  = 0x12,

	FLASH_DEFAULT = 0xFF,
};

enum ProgProtocol {
	PROTOCOL_UNSUPPORTED = 0x00,
	PROTOCOL_DEFAULT = 0x01,
	PROTOCOL_CH5xx = 0x02,
	PROTOCOL_CH56x = 0x03,
};

enum MemoryArea {
	DEFAULT_AREA = 0,
	PROGRAM_AREA = 1,
	BOOTLOADER_AREA = 2,
	OPTIONS_AREA = 3,
	EEPROM_AREA = 4,
	RAM_AREA = 5,
};

struct InternalState
{
	uint32_t statetag;
	uint32_t currentstateval;
	uint32_t flash_unlocked;
	int lastwriteflags;
	int processor_in_mode;
	int autoincrement;
	uint32_t ram_base;
	uint32_t ram_size;
	int sector_size;
	int flash_size;
	const struct RiscVChip_s* target_chip;
	enum RiscVChip target_chip_type;
	uint32_t target_chip_id;
	uint8_t flash_sector_status[MAX_FLASH_SECTORS];  // 0 means unerased/unknown. 1 means erased.
	int nr_registers_for_debug; // Updated by PostSetupConfigureInterface
	uint8_t isp_xor_key[8];
	enum MemoryArea current_area;
	uint32_t clock_set;
	uint8_t init_skip;
};


#define DMDATA0        0x04
#define DMDATA1        0x05
#define DMCONTROL      0x10
#define DMSTATUS       0x11
#define DMHARTINFO     0x12
#define DMABSTRACTCS   0x16
#define DMCOMMAND      0x17
#define DMABSTRACTAUTO 0x18
#define DMPROGBUF0     0x20
#define DMPROGBUF1     0x21
#define DMPROGBUF2     0x22
#define DMPROGBUF3     0x23
#define DMPROGBUF4     0x24
#define DMPROGBUF5     0x25
#define DMPROGBUF6     0x26
#define DMPROGBUF7     0x27
#define DMHALTSUM0     0x40

#define DMCPBR       0x7C
#define DMCFGR       0x7D
#define DMSHDWCFGR   0x7E
#define DMCHIPID     0x7F

#if defined( WIN32 ) || defined( _WIN32 )
#if defined( MINICHLINK_AS_LIBRARY )
	#define DLLDECORATE __declspec(dllexport)
#elif defined( MINICHLINK_IMPORT )
	#define DLLDECORATE __declspec(dllimport)
#else
	#define DLLDECORATE
#endif
#else
	#define DLLDECORATE
#endif

#ifndef TERMINAL_INPUT_BUFFER
#define TERMINAL_INPUT_BUFFER 0
#endif

#define TERMINAL_BUFFER_SIZE 512

#define STR_(x) #x
#define STR(x) STR_(x)

#ifndef TERMINAL_ACCENT_COLOR
#define TERMINAL_ACCENT_COLOR 5;208 // Chose color from predefined palette
// #define TERMINAL_ACCENT_COLOR 2;180;11;64  // Use R;G;B for color (can't be dimmed though)
#endif

#define TERMIANL_INPUT_SENT "\x1b[1F\x1b[2K\x1b[2K\033[38;" STR(TERMINAL_ACCENT_COLOR) "m> "
#define TERMINAL_SEND_LABEL "\n\x1b[2K\033[7m\033[1m\033[38;" STR(TERMINAL_ACCENT_COLOR) "mSend:\x1b[0m "
#define TERMINAL_SEND_BUSY "\n\x1b[2K\033[7m\033[1m\033[2m\033[38;" STR(TERMINAL_ACCENT_COLOR) "mSend:\x1b[0m "
#define TERMINAL_CLEAR_PREV "\x1b[1F\x1b[2K"
#define TERMINAL_CLEAR_CUR "\x1b[2K\x1b[F"
#define TERMINAL_DIM "\x1b[2m"

/* initialization hints for init functions */
/* could be expanded with more in the future (e.g., PID/VID hints, priorities, ...)*/
/* not all init functions currently need these hints. */
typedef struct {
	const char * serial_port;
	const char * specific_programmer;
} init_hints_t;

void * MiniCHLinkInitAsDLL(struct MiniChlinkFunctions ** MCFO, const init_hints_t* init_hints) DLLDECORATE;
extern struct MiniChlinkFunctions MCF;

// Returns 'dev' on success, else 0.
void * TryInit_WCHLinkE(void);
void * TryInit_WCHISP(void);
void * TryInit_ESP32S2CHFUN(void);
void * TryInit_NHCLink042(void);
void * TryInit_B003Fun(uint32_t id);
void * TryInit_Ardulink(const init_hints_t*);

// Returns 0 if ok, populated, 1 if not populated.
int SetupAutomaticHighLevelFunctions( void * dev );

// Useful for converting numbers like 0x, etc.
int64_t SimpleReadNumberInt( const char * number, int64_t defaultNumber );

// For drivers to call
int DefaultVoidHighLevelState( void * dev );
int InternalUnlockBootloader( void * dev );
int InternalIsMemoryErased( struct InternalState * iss, uint32_t address );
void InternalMarkMemoryNotErased( struct InternalState * iss, uint32_t address );
int InternalUnlockFlash( void * dev, struct InternalState * iss );

// GDBSever Functions
int SetupGDBServer( void * dev );
int PollGDBServer( void * dev );
int IsGDBServerInShadowHaltState( void * dev );
void ExitGDBServer( void * dev );

int DetectMemoryArea( void * dev, uint32_t address );
int CheckMemoryLocation( void * dev, enum MemoryArea area, uint32_t address, uint32_t length );

#endif


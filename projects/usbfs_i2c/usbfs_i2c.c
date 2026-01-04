#include "ch32fun.h"
#include <stdio.h>
#include <string.h>
#include "fsusb.h"

#define USB_PACKET_SIZE 64

extern volatile uint8_t usb_debug;
uint8_t debug;
volatile char terminal_input;
#define DEBUG_PRINT(format, args...) if (debug) printf(format, ##args)

#if defined(CH570_CH572)
#define PIN_SCL PA3
#define PIN_SDA PA2
#else
#define PIN_SCL PA5
#define PIN_SDA PA4
#endif

#define I2C_NO_DELAY 1 // Ignore requests for delay change from driver
#define DEFAULT_DELAY 2
uint32_t delay_normal = DEFAULT_DELAY;
uint32_t delay_half = DEFAULT_DELAY/2;

#if I2C_NO_DELAY
#define DELAY1 Delay_Us(1);
#define DELAY2 Delay_Us(2);
#else
#define DELAY1 Delay_Us(delay_half);
#define DELAY2 Delay_Us(delay_normal);
#endif

#define DSCL_IHIGH     { funPinMode(PIN_SCL, GPIO_CFGLR_IN_PUPD); funDigitalWrite(PIN_SCL, 1); }
#define DSDA_IHIGH     { funPinMode(PIN_SDA, GPIO_CFGLR_IN_PUPD); funDigitalWrite(PIN_SDA, 1); }
#define DSDA_INPUT     { funPinMode(PIN_SDA, GPIO_CFGLR_IN_PUPD); funDigitalWrite(PIN_SDA, 1); }
#define DSCL_OUTPUT    { funDigitalWrite(PIN_SCL, 0); funPinMode(PIN_SCL, GPIO_CFGLR_OUT_2Mhz_PP); }
#define DSDA_OUTPUT    { funDigitalWrite(PIN_SDA, 0); funPinMode(PIN_SDA, GPIO_CFGLR_OUT_2Mhz_PP); }
#define READ_DSDA      funDigitalRead(PIN_SDA)
#define I2CNEEDGETBYTE 1
#define I2CNEEDSCAN    1

#include "static_i2c.h"

// Defines for linux driver from https://github.com/harbaum/I2C-Tiny-USB/
/* commands from USB, must e.g. match command ids in kernel driver */
#define CMD_ECHO       0
#define CMD_GET_FUNC   1
#define CMD_SET_DELAY  2
#define CMD_GET_STATUS 3

#define CMD_I2C_IO     4
#define CMD_I2C_BEGIN  1
#define CMD_I2C_END    2

/* linux kernel flags */
#define I2C_M_TEN                           0x10 /* we have a ten bit chip address */
#define I2C_M_RD                            0x01
#define I2C_M_NOSTART                       0x4000
#define I2C_M_REV_DIR_ADDR                  0x2000
#define I2C_M_IGNORE_NAK                    0x1000
#define I2C_M_NO_RD_ACK                     0x0800

/* To determine what functionality is present */
#define I2C_FUNC_I2C			                  0x00000001
#define I2C_FUNC_10BIT_ADDR		              0x00000002
#define I2C_FUNC_PROTOCOL_MANGLING	        0x00000004 /* I2C_M_{REV_DIR_ADDR,NOSTART,..} */
#define I2C_FUNC_SMBUS_HWPEC_CALC	          0x00000008 /* SMBus 2.0 */
#define I2C_FUNC_SMBUS_READ_WORD_DATA_PEC   0x00000800 /* SMBus 2.0 */
#define I2C_FUNC_SMBUS_WRITE_WORD_DATA_PEC  0x00001000 /* SMBus 2.0 */
#define I2C_FUNC_SMBUS_PROC_CALL_PEC	      0x00002000 /* SMBus 2.0 */
#define I2C_FUNC_SMBUS_BLOCK_PROC_CALL_PEC  0x00004000 /* SMBus 2.0 */
#define I2C_FUNC_SMBUS_BLOCK_PROC_CALL	    0x00008000 /* SMBus 2.0 */
#define I2C_FUNC_SMBUS_QUICK                0x00010000
#define I2C_FUNC_SMBUS_READ_BYTE            0x00020000
#define I2C_FUNC_SMBUS_WRITE_BYTE           0x00040000
#define I2C_FUNC_SMBUS_READ_BYTE_DATA	      0x00080000
#define I2C_FUNC_SMBUS_WRITE_BYTE_DATA	    0x00100000
#define I2C_FUNC_SMBUS_READ_WORD_DATA	      0x00200000
#define I2C_FUNC_SMBUS_WRITE_WORD_DATA      0x00400000
#define I2C_FUNC_SMBUS_PROC_CALL            0x00800000
#define I2C_FUNC_SMBUS_READ_BLOCK_DATA      0x01000000
#define I2C_FUNC_SMBUS_WRITE_BLOCK_DATA     0x02000000
#define I2C_FUNC_SMBUS_READ_I2C_BLOCK       0x04000000 /* I2C-like block xfer  */
#define I2C_FUNC_SMBUS_WRITE_I2C_BLOCK      0x08000000 /* w/ 1-byte reg. addr. */
#define I2C_FUNC_SMBUS_READ_I2C_BLOCK_2     0x10000000 /* I2C-like block xfer  */
#define I2C_FUNC_SMBUS_WRITE_I2C_BLOCK_2    0x20000000 /* w/ 2-byte reg. addr. */
#define I2C_FUNC_SMBUS_READ_BLOCK_DATA_PEC  0x40000000 /* SMBus 2.0 */
#define I2C_FUNC_SMBUS_WRITE_BLOCK_DATA_PEC 0x80000000 /* SMBus 2.0 */

#define I2C_FUNC_SMBUS_BYTE                 I2C_FUNC_SMBUS_READ_BYTE | \
                                            I2C_FUNC_SMBUS_WRITE_BYTE
#define I2C_FUNC_SMBUS_BYTE_DATA            I2C_FUNC_SMBUS_READ_BYTE_DATA | \
                                            I2C_FUNC_SMBUS_WRITE_BYTE_DATA
#define I2C_FUNC_SMBUS_WORD_DATA            I2C_FUNC_SMBUS_READ_WORD_DATA | \
                                            I2C_FUNC_SMBUS_WRITE_WORD_DATA
#define I2C_FUNC_SMBUS_BLOCK_DATA           I2C_FUNC_SMBUS_READ_BLOCK_DATA | \
                                            I2C_FUNC_SMBUS_WRITE_BLOCK_DATA
#define I2C_FUNC_SMBUS_I2C_BLOCK            I2C_FUNC_SMBUS_READ_I2C_BLOCK | \
                                            I2C_FUNC_SMBUS_WRITE_I2C_BLOCK

#define I2C_FUNC_SMBUS_EMUL                 I2C_FUNC_SMBUS_QUICK | \
                                            I2C_FUNC_SMBUS_BYTE | \
                                            I2C_FUNC_SMBUS_BYTE_DATA | \
                                            I2C_FUNC_SMBUS_WORD_DATA | \
                                            I2C_FUNC_SMBUS_PROC_CALL | \
                                            I2C_FUNC_SMBUS_WRITE_BLOCK_DATA | \
                                            I2C_FUNC_SMBUS_WRITE_BLOCK_DATA_PEC | \
                                            I2C_FUNC_SMBUS_I2C_BLOCK


uint32_t func = I2C_FUNC_I2C | I2C_FUNC_SMBUS_EMUL; // Actually supported in this firmware

struct i2c_cmd {
	uint8_t type;
	uint8_t cmd;
	uint16_t flags;
	uint16_t addr;
	uint16_t len;
};

#define STATUS_IDLE          0
#define STATUS_ADDRESS_ACK   1
#define STATUS_ADDRESS_NAK   2

typedef struct {
	uint8_t status;
	struct i2c_cmd cmd;
	uint32_t pending_len;
	uint8_t buffer[USB_PACKET_SIZE];
} usb_i2c_t;

usb_i2c_t dev;

void RepeatStart() {
	DELAY1
	DSDA_IHIGH
	DELAY1
	DSCL_IHIGH
	DELAY1
	DSDA_OUTPUT
	DELAY1
	DSCL_OUTPUT
	DELAY1
}

int i2c_do() {
	uint8_t read = !!(dev.cmd.flags & I2C_M_RD);

	DEBUG_PRINT("I2C %s to [0x%02x], len = %d, flags = %02x\n", read?"read":"write", dev.cmd.addr, dev.cmd.len, dev.cmd.flags); 

	if (dev.cmd.cmd & CMD_I2C_BEGIN) {
		SendStart();
		DEBUG_PRINT("---SEND START---\n");
	} else {
		RepeatStart();
		DEBUG_PRINT("---REPEAT START---\n");
	}

	// send DEVICE address
	if (SendByte(dev.cmd.addr << 1 | read)) {
		DEBUG_PRINT("I2C SendByte - failed to send %s command to %02x\n", read?"read":"write", dev.cmd.addr);

		dev.status = STATUS_ADDRESS_NAK;
		dev.pending_len = 0;
		SendStop();
		DEBUG_PRINT("---SEND STOP---\n");
		return 0;
	}

	dev.status = STATUS_ADDRESS_ACK;
	dev.pending_len = dev.cmd.len;

	if (read) {
		for (int i = 0; i < USB_PACKET_SIZE && dev.pending_len; i++) {
			dev.pending_len--;
			dev.buffer[i] = GetByte(!dev.pending_len);
		}
	}

	if ((dev.cmd.cmd & CMD_I2C_END) && !dev.pending_len) {
		SendStop();
		DEBUG_PRINT("---SEND STOP---\n");
	}

	if (read) return(dev.cmd.len?dev.cmd.len:-1);
	else return(dev.pending_len?dev.pending_len:-1);
}

int HandleSetupCustom(struct _USBState * ctx, int setup_code) {
	int ret = -1;
	if (ctx->USBFS_SetupReqType & USB_REQ_TYP_CLASS) {
		DEBUG_PRINT("SETUP Custom\n");
	} else if (ctx->USBFS_SetupReqType & USB_REQ_TYP_VENDOR) {
		uint8_t* ctrl0buff = ctx->ENDPOINTS[0];
		ctx->pCtrlPayloadPtr = dev.buffer;

		DEBUG_PRINT("SETUP Vendor %02x %02x %02x %02x %02x %02x %02x %02x\n", ctrl0buff[0], ctrl0buff[1], ctrl0buff[2], ctrl0buff[3], ctrl0buff[4], ctrl0buff[5], ctrl0buff[6], ctrl0buff[7]);

		switch(ctrl0buff[1]) {

			case CMD_ECHO:
				dev.buffer[0] = ctrl0buff[2];
				dev.buffer[1] = ctrl0buff[3];
				return 2;
				break;

			case CMD_GET_FUNC:
				memcpy(dev.buffer, (uint8_t*)&func, 4);
				return 4;
				break;

			case CMD_SET_DELAY:
				delay_normal = *(uint16_t*)(ctrl0buff+2);
				if (!delay_normal) delay_normal = 1;
				delay_half = delay_normal/2;
				if (!delay_half) delay_half = 1;

				DEBUG_PRINT("Request for delay %ldus\n", delay_normal);
				break;

			case CMD_I2C_IO:
			case CMD_I2C_IO + CMD_I2C_BEGIN:
			case CMD_I2C_IO                 + CMD_I2C_END:
			case CMD_I2C_IO + CMD_I2C_BEGIN + CMD_I2C_END:
				memcpy(&dev.cmd, ctrl0buff, 8);
				return i2c_do();
				break;

			case CMD_GET_STATUS:
				dev.buffer[0] = dev.status;
				return 1;
				break;
		}
	}
	return ret;
}

void HandleDataOut(struct _USBState * ctx, int endp, uint8_t * data, int len) {
	DEBUG_PRINT("HandleDataOut EP%d len = %d, exp = %ld\n",endp, len, dev.pending_len);

	if (dev.pending_len && len) {
		if (len > dev.pending_len) {
			DEBUG_PRINT("Incomming data length is bigger than expected!\n");
			len = dev.pending_len;
		}

		uint8_t i;
		for (i = 0; i < len; i++) {
			dev.pending_len--;
			DEBUG_PRINT("data = %x\n", *data);
			
			uint8_t res = SendByte(*data++);
			
			if (res) {
				DEBUG_PRINT("write failed\n");
				break;
			}
		}
		len -= i;

		// end transfer on last byte
		if ((dev.cmd.cmd & CMD_I2C_END) && !dev.pending_len) {
			SendStop();
			DEBUG_PRINT("---SEND STOP---\n");
		}

		ctx->USBFS_SetupReqLen = dev.pending_len; // This will toggle R_TOG
	} else {
		ctx->USBFS_SetupReqLen = 0; // This will just ACK
	}
}

int HandleInRequest(struct _USBState * ctx, int endp, uint8_t * data, int len) {
	uint8_t i;

	DEBUG_PRINT("HandleInRequest EP%d, len = %d, exp = %ld\n", endp, len, dev.pending_len);

	if (dev.pending_len && len) {
		if (len > dev.pending_len) {
			DEBUG_PRINT("Requested data length is bigger than expected!\n");
			len = dev.pending_len;
		}

		for (i=0; i<len; i++) {
			dev.pending_len--;
			*data = GetByte(dev.pending_len == 0);
			DEBUG_PRINT("data = %x\n", *data);
			data++;
		}

		// end transfer on last byte
		if ((dev.cmd.cmd & CMD_I2C_END) && !dev.pending_len) {
			SendStop();
			DEBUG_PRINT("---SEND STOP---\n");
		}
	}

	return len;
}

void handle_debug_input(int numbytes, uint8_t * data) {
	terminal_input = data[0];
}

int main() {
	SystemInit();
	funGpioInitAll();

	ConfigI2C();

	printf("----Scanning I2C Bus for Devices---\n");
	Scan();
	printf("----Done Scanning----\n\n");

	USBFSSetup();
	printf("Started USB\n\n");
	printf( "You can enable debug messages:\n" );
	printf( "Type 'd' for USB and 'D' for I2C\n" );
	printf( "---------------------------------\n" );
	
	while (1) {
#if FUNCONF_USE_DEBUGPRINTF
		poll_input();
#endif
		if (terminal_input) {
			switch (terminal_input) {
				case 'd':
					usb_debug = (usb_debug)?0:1;
					printf("USB debug %s\n", (usb_debug)?"ON":"OFF");
					break;

				case 'D':
					debug = (debug)?0:1;
					printf("I2C Debug %s\n", (debug)?"ON":"OFF");
					break;
			}
			terminal_input = 0;
		}
	}
}
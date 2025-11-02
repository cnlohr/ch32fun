#include "ch32fun.h"
#include "iSLER.h"
#include <stdio.h>

#define PRINT_STRUCT_BYTES(struct_ptr, format) do { \
	const uint8_t* bytes = (const uint8_t*)(struct_ptr); \
	for (size_t i = 0; i < sizeof(*(struct_ptr)); i++) { \
		printf(format " ", bytes[i]); \
	} \
	printf("\n"); \
} while(0)


#define BLE_AD_MAC(mac) \
	(mac & 0xFF), (mac>>8) & 0xFF, \
	(mac>>16) & 0xFF, (mac>>24) & 0xFF, \
	(mac>>32) & 0xFF, (mac>>40) & 0xFF

#define BLE_AD_FLAGS(flags) 0x02, 0x01, flags

#define PHY_MODE				PHY_1M
#define MAX_PAYLOAD_LEN		 16

#ifndef PACKED
#define PACKED __attribute__( ( packed ) )
#endif

typedef struct PACKED {
	uint16_t preamble;		  // Sync pattern (0xAABB)
	uint16_t control_bits;	  // control bits
	uint16_t msgCode;		   // message integrity check
	uint8_t dest[6];			// destination

	uint8_t group_id;
	uint8_t data_len;		  			 // length
	uint8_t payload[MAX_PAYLOAD_LEN];	// max payload length
} MESS_DataFrame_t;

typedef struct PACKED {
	uint8_t mac[6];
	uint8_t field_adv_flags[3];
	uint8_t name_len;
	uint8_t ad_type_local_name;
	uint8_t name[20];
	uint8_t data_len;
	uint8_t field_sev_data[3];
	MESS_DataFrame_t dataFrame;
} iSLER_frame_t;

typedef struct PACKED {
	uint8_t command;
	uint32_t value1;
	uint32_t value2;
} remote_command_t;


// BLE advertisements are sent on channels 37, 38 and 39
uint8_t adv_channels[] = {37, 38, 39};
// uint8_t adv_channels[] = {37};

void Frame_TX2(uint8_t adv[], size_t len, uint8_t channel, uint8_t phy_mode) {
	__attribute__((aligned(4))) uint8_t  ADV_BUF[len+2]; // for the advertisement, which is 37 bytes + 2 header bytes

	BB->CTRL_TX = (BB->CTRL_TX & 0xfffffffc) | 1;

	DevSetChannel(channel);


	// Uncomment to disable whitening to debug RF.
	//BB->CTRL_CFG |= (1<<6);
	DevSetMode(DEVSETMODE_TX);

	BB->ACCESSADDRESS1 = 0x8E89BED6; // access address
	BB->CRCINIT1 = 0x555555; // crc init

	// LL->LL1 = (LL->LL1 & 0xfffffffe) | 1; // The "| 1" is for AUTO mode, to swap between RX <-> TX when either happened

	ADV_BUF[0] = 0x02; // PDU 0x00, 0x02, 0x06 seem to work, with only 0x02 showing up on the phone
	ADV_BUF[1] = len ;
	memcpy(&ADV_BUF[2], adv, len);
	LL->FRAME_BUF = (uint32_t)ADV_BUF;
	
	// Wait for tuning bit to clear.
	for( int timeout = 3000; !(RF->RF26 & 0x1000000) && timeout >= 0; timeout-- );
	
	BB->CTRL_CFG = (phy_mode == PHY_2M) ? CTRL_CFG_PHY_2M:
										  CTRL_CFG_PHY_1M; // default 1M for now


	// This clears bit 17 (If set, seems to have no impact.)
	LL->LL4 &= 0xfffdffff;

	LL->TMR = (uint32_t)(len*100); // needs optimisation, per phy mode

	BB->CTRL_CFG |= CTRL_CFG_START_TX;
	BB->CTRL_TX &= 0xfffffffc;

	LL->LL0 = 2; // Not sure what this does, but on RX it's 1

	while(LL->TMR); // wait for tx buffer to empty
	
	DevSetMode(0);
	if(LL->LL0 & 3) {
		LL->CTRL_MOD &= CTRL_MOD_RFSTOP;
		LL->LL0 |= 0x08;
	}
}


iSLER_frame_t frame = {
	.mac = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66},
	.field_adv_flags = {0x02, 0x01, 0x06},
	.name_len = 21,	 // name length is only 20 + local name byte
	.ad_type_local_name = 0x09,
	.name = { 'b','e', 'e', '-', '5', '5', '5' },
	.data_len = sizeof(MESS_DataFrame_t) + 3,
	.field_sev_data = {0xFF, 0xD7, 0x07},
	.dataFrame = {
		.preamble = 0xA1A2,
		.control_bits = 0xB1B2,
		.msgCode = 0xC1C2,
		.dest = {0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6},
		.group_id = 0x55,
		.data_len = sizeof(remote_command_t)
	}
};

void chMess_advertise(remote_command_t *cmd) {
	memcpy(&(frame.dataFrame.payload), cmd, sizeof(remote_command_t));

	// printf("Frame: ");
	// PRINT_STRUCT_BYTES(&frame, "%02X");
	// printf("\n");

	for(int c = 0; c < sizeof(adv_channels); c++) {
		Frame_TX2((uint8_t*)&frame, sizeof(frame), adv_channels[c], PHY_MODE);
	}
}

remote_command_t* chMess_rx_handler() {
	// The chip stores the incoming frame in LLE_BUF, defined in extralibs/iSLER.h
	uint8_t *frame = (uint8_t*)LLE_BUF;
	iSLER_frame_t* rx_frame = (iSLER_frame_t*)(frame + 2);
	uint8_t target_mac[] = { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66 };

	if (memcmp(rx_frame->mac, target_mac, 6) == 0) {
		// first 8 bytes contains: [RSSI x 1Byte] [len x 1Byte] [MAC x 6Bytes]
		// The first two bytes of the frame are metadata with RSSI and length
		// printf("RSSI:%d len:%d MAC:", frame[0], frame[1]);
		// PRINT_ARRAY(rx_frame->mac, "%02X");
		// printf("Raw Data: ");
		// PRINT_ARRAY_WITH_SIZE(frame, frame[1], "%02X");
		
		remote_command_t *cmd = (remote_command_t*)rx_frame->dataFrame.payload;
		// printf("Command: %02X Value1: %08X Value2: %08X\n", 
		// cmd->command, cmd->value1, cmd->value2);
		return cmd;
	}

	return NULL;
}

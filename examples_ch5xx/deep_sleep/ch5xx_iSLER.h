#include "ch32fun.h"
#include "iSLER.h"
#include <stdio.h>

#define BLE_AD_MAC(mac) \
    (mac & 0xFF), (mac>>8) & 0xFF, \
    (mac>>16) & 0xFF, (mac>>24) & 0xFF, \
    (mac>>32) & 0xFF, (mac>>40) & 0xFF

#define BLE_AD_FLAGS(flags) 0x02, 0x01, flags

#define PHY_MODE                PHY_1M
#define MAX_PACKET_LEN          255
#define REPORT_ALL              1 // if 0 only report received Find My advertisements

#ifndef PACKED
#define PACKED __attribute__( ( packed ) )
#endif

typedef struct PACKED {
	uint16_t preamble;          // Sync pattern (0xAABB)
	uint16_t control_bits;      // control bits
	uint16_t msgCode;           // message integrity check
	uint8_t dest[6];            // destination

	uint8_t group_id;
	uint8_t data_len;           // length
	uint8_t payload[20];      	// variable length
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
uint8_t dev_name[] = "ch32fun999";


void fun_iSLER_adv_data(MESS_DataFrame_t *dataFrame, remote_command_t *cmd, size_t data_len) {
	if (!REPORT_ALL) return;

    dataFrame->data_len = data_len;
	memcpy(dataFrame->payload, cmd, data_len);

	iSLER_frame_t frame = {
		.mac = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66},
		.field_adv_flags = {0x02, 0x01, 0x06},
		.name_len = 21,
		.ad_type_local_name = 0x09,
		.name = { 'b','e', 'e', '-', '5', '5', '5' },
		.data_len = sizeof(MESS_DataFrame_t) + 3,
		.field_sev_data = {0xFF, 0xD7, 0x07},
		.dataFrame = *dataFrame
	};

	// PRINT_SEPARATOR();
	// printf("Frame: ");
	// PRINT_STRUCT_BYTES(&frame, "%02X");
	// printf("\n");

	for(int c = 0; c < sizeof(adv_channels); c++) {
		Frame_TX((uint8_t*)&frame, sizeof(frame), adv_channels[c], PHY_MODE);
	}
}

remote_command_t* modiSLER_rx_handler() {
	// The chip stores the incoming frame in LLE_BUF, defined in extralibs/iSLER.h
	uint8_t *frame = (uint8_t*)LLE_BUF;
	iSLER_frame_t* rx_frame = (iSLER_frame_t*)(frame + 2);
	uint8_t target_mac[] = { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66 };

	if (memcmp(rx_frame->mac, target_mac, 6) == 0) {
		// first 8 bytes contains: [RSSI x 1Byte] [len x 1Byte] [MAC x 6Bytes]
		// The first two bytes of the frame are metadata with RSSI and length
		// PRINT_SEPARATOR();
		// printf("RSSI:%d len:%d MAC:", frame[0], frame[1]);
		// PRINT_ARRAY(rx_frame->mac, "%02X");
		// printf("Raw Data: ");
		// PRINT_ARRAY_WITH_SIZE(frame, frame[1], "%02X");
		
		remote_command_t *cmd = (remote_command_t*)rx_frame->dataFrame.payload;
		// printf("Command: %02X Value1: %08X Value2: %08X\n", 
		// 	cmd->command, cmd->value1, cmd->value2);
		return cmd;
	}

	return NULL;
}

MESS_DataFrame_t dataFrame = {
	.preamble = 0xA1A2,
	.control_bits = 0xB1B2,
	.msgCode = 0xC1C2,
	.dest = {0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6},
	.group_id = 0x55,
};

void fun_iSLER_adv_command(remote_command_t *cmd) {
    fun_iSLER_adv_data(&dataFrame, cmd, sizeof(remote_command_t));
}
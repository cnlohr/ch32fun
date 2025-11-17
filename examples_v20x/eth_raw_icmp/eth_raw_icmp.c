#include "ch32fun.h"
#include <stdio.h>

#define MY_IP_ADDR { 192, 168, 1, 100 }

#define CH32V208_ETH_IMPLEMENTATION
#include "../../extralibs/ch32v208_eth.h"

static uint8_t g_my_ip[4] = MY_IP_ADDR;
static uint8_t g_my_mac[6];

/*
 * Internet checksum (RFC 1071)
 */
static uint16_t checksum( const uint8_t *data, uint16_t len )
{
	uint32_t sum = 0;
	const uint16_t *ptr = (const uint16_t *)data;
	while ( len > 1 )
	{
		sum += *ptr++;
		len -= 2;
	}
	if ( len ) sum += *(uint8_t *)ptr;
	while ( sum >> 16 ) sum = ( sum & 0xFFFF ) + ( sum >> 16 );
	return ~sum;
}

static void process_packet( const uint8_t *pkt, uint16_t len )
{
	// minimum packet size for ARP (14 Ethernet + 28 ARP = 42 bytes min)
	if ( len < 42 ) return;

	/*
	 * ARP request handler
	 *
	 * Check if this is an ARP request (opcode 1) asking for our IP.
	 * Packet layout:
	 *   [0-5]   destination MAC
	 *   [6-11]  source MAC
	 *   [12-13] ethertype (0x0806 for ARP)
	 *   [14-19] hardware type, protocol type, sizes
	 *   [20-21] opcode (1=request, 2=reply)
	 *   [22-27] sender MAC
	 *   [28-31] sender IP
	 *   [32-37] target MAC
	 *   [38-41] target IP
	 */
	if ( pkt[12] == 0x08 && pkt[13] == 0x06 && // EtherType = 0x0806 (ARP)
		 pkt[20] == 0x00 && pkt[21] == 0x01 && // opcode = 0x0001 (ARP Request)
		 memcmp( pkt + 38, g_my_ip, 4 ) == 0 ) // target IP = us
	{
		uint16_t max_len;
		uint8_t *reply = eth_get_tx_buffer( &max_len );

		if ( !reply ) return; // no TX buffers available (queue full)

		// ethernet header
		memcpy( reply, pkt + 6, 6 ); // dst MAC = sender's MAC
		memcpy( reply + 6, g_my_mac, 6 ); // src MAC = our MAC
		reply[12] = 0x08;
		reply[13] = 0x06; // EtherType = 0x0806 (ARP)

		// ARP header
		reply[14] = 0x00;
		reply[15] = 0x01; // hardware type = ethernet
		reply[16] = 0x08;
		reply[17] = 0x00; // protocol type = IPv4
		reply[18] = 6; // hardware addr len
		reply[19] = 4; // protocol addr len
		reply[20] = 0x00;
		reply[21] = 0x02; // opcode = reply

		// ARP payload
		memcpy( reply + 22, g_my_mac, 6 ); // sender MAC
		memcpy( reply + 28, g_my_ip, 4 ); // sender IP
		memcpy( reply + 32, pkt + 22, 10 ); // target MAC+IP from requester

		eth_send_packet_zerocopy( 42 );
		return;
	}

	/*
	 * ICMP echo request handler
	 *
	 * Respond to ping requests directed at our IP.
	 * Packet layout:
	 *   [0-13]  ethernet header
	 *   [14-33] IPv4 header (20 bytes)
	 *     [23]  protocol (1 = ICMP)
	 *     [26-29] source IP
	 *     [30-33] destination IP
	 *   [34+]   ICMP header + data
	 *     [34]  type (8 = echo request, 0 = echo reply)
	 *     [36-37] checksum
	 */
	if ( pkt[12] == 0x08 && pkt[13] == 0x00 && // ethertype IPv4
		 pkt[23] == 0x01 && // protocol = ICMP
		 pkt[34] == 0x08 && // type = echo request
		 memcmp( pkt + 30, g_my_ip, 4 ) == 0 ) // dst IP = us
	{
		uint16_t ip_len = ( pkt[16] << 8 ) | pkt[17];
		uint16_t reply_len = 14 + ip_len;

		if ( reply_len > len ) return; // malformed packet

		uint16_t max_len;
		uint8_t *reply = eth_get_tx_buffer( &max_len );

		if ( !reply || reply_len > max_len ) return; // no TX buffers or packet too large

		// copy entire packet and modify in place
		memcpy( reply, pkt, reply_len );

		// swap ethernet addresses
		memcpy( reply, pkt + 6, 6 ); // dst MAC
		memcpy( reply + 6, g_my_mac, 6 ); // src MAC

		// swap IP addresses
		memcpy( reply + 26, pkt + 30, 4 ); // src IP
		memcpy( reply + 30, pkt + 26, 4 ); // dst IP

		// recalculate IP checksum
		reply[24] = 0;
		reply[25] = 0;
		*(uint16_t *)( reply + 24 ) = checksum( reply + 14, 20 );

		// convert echo request to echo reply
		reply[34] = 0; // type = echo reply

		// recalculate ICMP checksum
		reply[36] = 0;
		reply[37] = 0;
		*(uint16_t *)( reply + 36 ) = checksum( reply + 34, ip_len - 20 );

		eth_send_packet_zerocopy( reply_len );
	}
}

static void link_status_callback( bool link_up )
{
	printf( "ETH: Link %s\n", link_up ? "UP" : "DOWN" );
}

int main( void )
{
	SystemInit();
	printf( "CH32V208 ETH10M test\n" );

	eth_config_t cfg = { .mac_addr = NULL,
		.rx_callback = NULL, // use zero-copy polling instead
		.link_callback = link_status_callback,
		.promiscuous_mode = false,
		.broadcast_filter = true,
		.multicast_filter = false };

	if ( eth_init( &cfg ) != 0 )
	{
		printf( "ERROR: eth_init failed\n" );
		while ( 1 );
	}

	eth_get_mac_address( g_my_mac );
	printf( "IP:  %d.%d.%d.%d\n", g_my_ip[0], g_my_ip[1], g_my_ip[2], g_my_ip[3] );
	printf( "MAC: %02X:%02X:%02X:%02X:%02X:%02X\n", g_my_mac[0], g_my_mac[1], g_my_mac[2], g_my_mac[3], g_my_mac[4],
		g_my_mac[5] );
	printf( "\nWaiting for link...\n" );

	const uint32_t ticks_per_ms = ( FUNCONF_SYSTEM_CORE_CLOCK / 1000 );
	const uint32_t poll_interval_ticks = 100 * ticks_per_ms; // 100ms poll interval
	uint64_t last_poll_tick = SysTick->CNT;

	while ( 1 )
	{
		// check for received packets (zero-copy)
		uint16_t pkt_len;
		const uint8_t *pkt = eth_get_rx_packet( &pkt_len );

		if ( pkt && pkt_len > 0 )
		{
			process_packet( pkt, pkt_len );
			eth_release_rx_packet(); // return buffer to DMA ring
		}

		uint64_t now = SysTick->CNT;
		if ( ( now - last_poll_tick ) >= poll_interval_ticks )
		{
			eth_poll_link();
			last_poll_tick = now;
		}
	}
}

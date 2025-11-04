#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "uip.h"
#include "uip_arp.h"

#include "dhcpd.h"

#ifndef DHCP_LOG_ENABLE
#define DHCP_LOG_ENABLE 0
#endif

#ifndef DHCP_CAPTIVE_PORTAL_ENABLE
// this doesn't work yet
#define DHCP_CAPTIVE_PORTAL_ENABLE 0
#endif

#if DHCP_LOG_ENABLE
#include <stdio.h>
#define DHCP_LOG( ... ) printf( "dhcp: " __VA_ARGS__ );
#else
#define DHCP_LOG( ... )
#endif

#define DHCP_SERVER_PORT 67
#define DHCP_CLIENT_PORT 68

// Bootp message types
#define BOOTREQUEST 1
#define BOOTREPLY 2

// DHCP Message types
#define DHCP_DISCOVER 1
#define DHCP_OFFER 2
#define DHCP_REQUEST 3
#define DHCP_ACK 5
#define DHCP_HOSTNAME 12

#define DHCP_MESSAGE_TYPE 53
#define SERVER_IDENTIFIER 54

// DHCP Option codes
#define DHCP_OPTION_SUBNET 1
#define DHCP_OPTION_ROUTER 3
#define DHCP_OPTION_DNS 6
#define DHCP_OPTION_LEASE 51
#define DHCP_OPTION_CAPTIVE_PORTAL 114
#define DHCP_OPTION_END 255

#define DHCP_HEADER_SIZE 236
// Minimum size for a DHCP DISCOVER/REQUEST message seems to be:
// 244 bytes = DHCP Header (236 bytes) + DHCP magic (4) + type (1) + length (1) + message (1) + 0xFF
#define DHCP_MESSAGE_SIZE_MIN 244
// 576 is max per RFC 2131 pg. 10, for un-extended message
#define DHCP_MESSAGE_SIZE_MAX 576
#define DHCP_RESPONSE_SIZE ( DHCP_HEADER_SIZE + sizeof( dhcp_response_options_t ) )

const uint8_t host[4] = { DHCP_SERVER_IP_BYTES };
const uint8_t client[4] = { DHCP_CLIENT_IP_BYTES };
static const uint8_t dhcp_option_magic[4] = { 0x63, 0x82, 0x53, 0x63 };

// From: https://datatracker.ietf.org/doc/html/rfc2131#page-37
typedef struct
{
	uint8_t op;
	uint8_t htype;
	uint8_t hlen;
	uint8_t hops;
	uint32_t xid;
	uint16_t secs;
	uint16_t flags;
	uint32_t ciaddr;
	uint32_t yiaddr;
	uint32_t siaddr;
	uint32_t giaddr;
	uint8_t chaddr[16];
	uint32_t sname[16];
	uint32_t file[32];
	uint8_t options[0]; // zero size pointer
} dhcp_message_t;

static_assert( sizeof( dhcp_message_t ) == DHCP_HEADER_SIZE, "dhcp_message_t size incorrect" );
#if DHCP_CAPTIVE_PORTAL_ENABLE
#define CAPTIVE_PORTAL_URL "http://XXX.XXX.XXX.XXX/index.html"
#endif

typedef struct
{
	uint8_t code;
	uint8_t len;
	uint8_t data[];
} dhcp_option_t;

// Only options used for DHCP OFFER/ACK
typedef struct
{
	uint8_t magic[4];
	uint8_t msg_type_option;
	uint8_t msg_type_len;
	uint8_t msg_type_val;
	uint8_t server_id_option;
	uint8_t server_id_len;
	uint8_t server_id_val[4];
	uint8_t subnet_option;
	uint8_t subnet_len;
	uint8_t subnet_val[4];
	uint8_t lease_option;
	uint8_t lease_len;
	uint8_t lease_val[4];
#if DHCP_CAPTIVE_PORTAL_ENABLE
	uint8_t captive_portal_option;
	uint8_t captive_portal_len;
	char captive_portal_val[sizeof( CAPTIVE_PORTAL_URL ) - 1];
	uint8_t router_option;
	uint8_t router_len;
	uint8_t router_val[4];
	uint8_t dns_option;
	uint8_t dns_len;
	uint8_t dns_val[4];
#endif
	uint8_t end_option;
} dhcp_response_options_t;

#if DHCP_LOG_ENABLE
static char *mac_to_str( uint8_t mac[6] )
{
	static char str[20];
	snprintf( str, 20, "%02x:%02x:%02x:%02x:%02x:%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5] );
	return str;
}
#endif

static int dhcp_get_request_type( dhcp_message_t *request, ssize_t request_len )
{
	const int option_len = request_len - DHCP_HEADER_SIZE;
	int idx = 4; // skip magic number
	dhcp_option_t *option = (dhcp_option_t *)&request->options[idx];

	while ( option->code != 0xFF && idx + 3 <= option_len )
	{
		if ( option->code == DHCP_MESSAGE_TYPE )
		{
			if ( option->data[0] == DHCP_DISCOVER )
			{
				return DHCP_DISCOVER;
			}
			if ( option->data[0] == DHCP_REQUEST )
			{
				return DHCP_REQUEST;
			}
			idx += option->len + 2;
		}
		else if ( option->code == 0 )
		{
			// padding
			idx++;
		}
		else
		{
			// some unsupported option type
			idx += option->len + 2;
		}
		option = (dhcp_option_t *)&request->options[idx];
	}

	return -1;
}

static int dhcp_create_response( dhcp_message_t *response, unsigned char type )
{
	response->op = BOOTREPLY;
	memset( &response->flags, 0, sizeof( response->flags ) );

	memcpy( &response->yiaddr, client, sizeof( response->yiaddr ) );


	dhcp_response_options_t *options = (dhcp_response_options_t *)&response->options;
	*options = ( dhcp_response_options_t ){
		.magic = { 0x63, 0x82, 0x53, 0x63 },
		.msg_type_option = DHCP_MESSAGE_TYPE,
		.msg_type_len = 1,
		.msg_type_val = type,
		.server_id_option = SERVER_IDENTIFIER,
		.server_id_len = 4,
		.subnet_option = DHCP_OPTION_SUBNET,
		.subnet_len = 4,
		.lease_option = DHCP_OPTION_LEASE,
		.lease_len = 4,
		.lease_val = { 0x00, 0x37, 0x5f, 0x00 }, // 42 days in second
		.server_id_val = { DHCP_SERVER_IP_BYTES },
      .subnet_val = { 255, 255, 255, 0 },
#if DHCP_CAPTIVE_PORTAL_ENABLE
		.captive_portal_option = DHCP_OPTION_CAPTIVE_PORTAL,
      .router_option = DHCP_OPTION_ROUTER,
      .router_len = 4,
		.router_val = { DHCP_SERVER_IP_BYTES },
      .dns_option = DHCP_OPTION_DNS,
      .dns_len = 4,
		.dns_val = { DHCP_SERVER_IP_BYTES },
#endif
		.end_option = DHCP_OPTION_END,
	};

#if DHCP_CAPTIVE_PORTAL_ENABLE
	options->captive_portal_len = snprintf( options->captive_portal_val, sizeof( CAPTIVE_PORTAL_URL ) - 1,
		"http://%d.%d.%d.%d/index.html", DHCP_SERVER_IP_BYTES );
#endif

	return 0;
}

static int dhcp_is_request_valid( const dhcp_message_t *request, int request_len )
{
	if ( request_len < DHCP_MESSAGE_SIZE_MIN || request_len > DHCP_MESSAGE_SIZE_MAX ) return 0;

	// 1 == ethernet, from RFC 1700, "Hardware Type" table
	if ( request->htype != 1 )
	{
		DHCP_LOG( "Received request for unsupported hardware type: %d\n", request->htype );
		return 0;
	}

	if ( request->op != BOOTREQUEST ) return 0;

	if ( memcmp( &request->options, &dhcp_option_magic, 4 ) != 0 ) return 0;

	return 1;
}

int dhcpd_init( void )
{
	DHCP_LOG( "dhcpd_init: server ip %d.%d.%d.%d\n", host[0], host[1], host[2], host[3] );

	u16_t addr[2];
	uip_ipaddr( addr, host[0], host[1], host[2], host[3] );
	uip_sethostaddr( addr );
	uip_ipaddr( addr, 0, 0, 0, 0 );
	uip_setdraddr( addr );
	struct uip_udp_conn *conn = uip_udp_new( addr, DHCP_CLIENT_PORT );

	if ( conn == NULL ) return -1;

	DHCP_LOG( "dhcpd_init: bind dhcp server port %d\n", DHCP_SERVER_PORT );
	uip_udp_bind( conn, HTONS( DHCP_SERVER_PORT ) );

	return 0;
}

void dhcpd_udp_appcall( void )
{
	struct uip_udp_conn *udp_conn = uip_udp_conn;

	if ( !uip_newdata() ) return;

	DHCP_LOG( "dhcpd_udp_appcall\n" );
	if ( DHCP_SERVER_PORT == HTONS( udp_conn->lport ) )
	{
		printf( "dhcpd handle_dhcpd\n" );
		dhcp_message_t *request = (dhcp_message_t *)uip_appdata;
		dhcp_message_t *response = (dhcp_message_t *)uip_appdata;
		size_t request_len = uip_datalen();

		if ( !dhcp_is_request_valid( request, request_len ) )
		{
			DHCP_LOG( "Received invalid DHCP request\n" );
			return;
		}

		int type = -1;
		switch ( dhcp_get_request_type( request, request_len ) )
		{
			case DHCP_DISCOVER:
				if ( request->hlen == 6 )
					DHCP_LOG( "Received DHCP DISCOVER from client: %s\n", mac_to_str( request->chaddr ) );
				type = DHCP_OFFER;
				break;
			case DHCP_REQUEST:
				if ( request->hlen == 6 )
					DHCP_LOG( "Received DHCP REQUEST from client: %s\n", mac_to_str( request->chaddr ) );
				type = DHCP_ACK;
				break;
			default: DHCP_LOG( "Received unknown DHCP message type\n" ); break;
		}

		if ( type != -1 )
		{
			dhcp_create_response( response, type );
			uip_udp_send( DHCP_RESPONSE_SIZE );
		}
	}
}


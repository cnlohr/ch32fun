#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "uip.h"
#include "uip_arp.h"

#include "dns.h"
#include "dhcpd.h"

#if DNS_LOG_ENABLE
#include <stdio.h>
#define DNS_LOG( ... ) printf( "dns: " __VA_ARGS__ );
#else
#define DNS_LOG( ... )
#endif

#define DNS_RESPONSE_FLAGS 0x8180

typedef struct __attribute__( ( packed ) )
{
	uint16_t id;
	uint16_t flags;
	uint16_t questions;
	uint16_t answers;
	uint16_t authority;
	uint16_t additional;
	uint8_t data[0];
} dns_message_t;

static_assert( sizeof( dns_message_t ) == 12, "dns_message_t size incorrect" );

typedef struct __attribute__( ( packed ) )
{
	uint16_t type;
	uint16_t class;
	uint32_t ttl;
	uint16_t len;
	uint8_t addr[4];
} dns_answer_t;

static_assert( sizeof( dns_answer_t ) == 14, "dns_answer_t size incorrect" );

#define HTONL( x ) \
	( ( ( ( x ) & 0xFF ) << 24 ) | ( ( ( x ) << 8 ) & 0x00FF0000 ) | ( ( ( x ) >> 8 ) & 0x0000FF00 ) | ( ( x ) >> 24 ) )

int dns_init( void )
{
	u16_t addr[2];
	uip_ipaddr( addr, client[0], client[1], client[2], client[3] );
	struct uip_udp_conn *conn = uip_udp_new( addr, 0 );

	if ( conn == NULL ) return -1;

	DNS_LOG( "dns_init: bind dns server port %d\n", 53 );
	uip_udp_bind( conn, HTONS( 53 ) );
	return 0;
}


void dns_udp_appcall( void )
{
	struct uip_udp_conn *udp_conn = uip_udp_conn;

	if ( !uip_newdata() ) return;

	if ( 53 == HTONS( udp_conn->lport ) )
	{
      // response buffer is the same as the request buffer
		dns_message_t *response = (dns_message_t *)uip_appdata;
		const size_t request_len = uip_datalen();
		const size_t questions_len = request_len - sizeof( dns_message_t );

		DNS_LOG( "DNS request len=%d, questions=%d\n", (int)request_len, (int)HTONS( response->questions ) );

		response->flags = HTONS( DNS_RESPONSE_FLAGS );
		response->questions = HTONS( 1 );
		response->answers = HTONS( 1 );
		response->authority = 0;
		response->additional = 0;

		uint8_t *answers = &response->data[questions_len];
		// Hardcode pointer to question name
		answers[0] = 0xC0; // Pointer
		answers[1] = sizeof( dns_message_t ); // first question starts right after header

		dns_answer_t *answer = (dns_answer_t *)&answers[2];
		*answer = ( dns_answer_t ){
			.type = HTONS( 1 ), // A
			.class = HTONS( 1 ), // IN
			.ttl = HTONL( 300 ), // 5 minutes
			.len = HTONS( 4 ),
			.addr = { host[0], host[1], host[2], host[3] },
		};

		const size_t response_len = request_len + 2 + sizeof( dns_answer_t );
		uip_udp_send( response_len );
	}
}

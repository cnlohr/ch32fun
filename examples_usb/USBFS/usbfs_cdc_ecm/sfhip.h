#ifndef _SFHIP_H
#define _SFHIP_H

/**
 General coding considerations:
  1. Consider something like RISC-V where the first 6 parameters are passed
     by a0..a5 registers.
  2. Consider something like RV32E, where you only get 16 total registers, so
     try to write the code to not register spill.  You can do this by writing
     the code so at any one point, only a few variables are needed.
  3. Think about compile-time perf by baking in easy-to-compile optimizations
     i.e. when checking against a big endian number, take the BE of a
     constant instead of converting to host-local.
  4. Make optimizations easy for the compiler, for instance, pull value out
     of structs in such a way to use temporary registers.


 Responsibilities:
  1. The PHY layer is respoinsible for checking the 32-bit MAC CRC.
  2. The PHY layer should pass in packets without the checksum at the end.


  Implementation details:

YOU MUST HAVE as sfhip object.

  A sfhip object, by default the DHCP client is on, so the IP
  is just what is used for a DHCP request.

  sfhip hip = {
    .ip =      HIPIP( 192, 168,  14, 251 ),
    .mask =    HIPIP( 255, 255, 255, 0   ),
    .gateway = HIPIP( 192, 168,  14, 1   ),
    .self_mac = { 0xf0, 0x11, 0x22, 0x33, 0x44, 0x55 },
    .hostname = "sfhip_test_linux",
  };

YOU MUST CALL:

  When your PHY Receives Packet, call:

    int sfhip_accept_packet( sfhip * hip, sfhip_phy_packet_mtu * data, int
length );

  You can hand this any frame that is able to store at least SFHIP_MTU bytes
  plus MAC header, plus HIP_PHY_HEADER_LENGTH_BYTES. You can usually re-use DMA
  blocks note that this must be volatile. Replies are sent back off of this
  buffer.

  length is the number of received bytes, but you must still make sure sfhip
  can write back up to the MTU.


  In an idle loop, or periodically:

    int sfhip_tick( sfhip * hip, sfhip_phy_packet_mtu * scratch, int
milliseconds );

  milliseconds can be zero, but it must be set to one or greater whenever time
  actually transpires.


YOU MUST IMPLEMENT:

  SFHIP wishes to send a packet.  This is called.  Note that data may be (and
  usually is) the buffer that was passed into accept or tick.

    int sfhip_send_packet( sfhip * hip, sfhip_phy_packet * data, int length );

IF YOU ENABLE SFHIP_DHCP_CLIENT

  SFHIP will call this whenever an IP is leased.
  void sfhip_got_dhcp_lease( sfhip * hip, sfhip_address addr );

IF YOU WANT TO RECEIVE/SEND UDP SOCKETS

  #define SFHIP_UDP_USER_HANDLER example_udp_user_handler

  where example_udp_user_handler is

    int example_udp_user_handler(
      sfhip * hip,
      sfhip_phy_packet_mtu * pkt,
      uint8_t * payload,
      int ulen,
      int source_port,
      int destination_port );

   RETURN VALUE is 0 for no packet sent or 1 for packet sent.

     sfhip_mac_header * mac = &pkt->mac_header;
     sfhip_ip_header  * ip = (sfhip_ip_header *)(mac + 1 );
     int plen = ####
     payload[0] = 'X';
     payload[1] = '\n';
     sfhip_send_udp_packet( hip, pkt, mac->source, ip->source_address,
         destination_port, source_port, plen );
     return 1;

IF YOU HAVE SFHIP_TCP_SOCKETS you must implement:

  A function to accept or reject connections. This will be called and you must
  return 1 to accept, 0 to abort the connection.

    int sfhip_tcp_accept_connection( sfhip * hip, int sockno, int localport,
hipbe32 remote_host );

  A function for all data events, this is used for:
    1. Receiving data from the peer (by the tune of ip_payload_length)
    2. ACKs from the peer if acked is set, you may move on.
    3. If you can send data, max_out_payload will be nonzero.

  Note that this can be called either from tick, for notifying that data
  may be able to be sent, or, in response to an incoming packet.

  You may return 0, for no operation, positive to send data, or one of
    #define SFHIP_TCP_OUTPUT_FIN         -1
    #define SFHIP_TCP_OUTPUT_SYNACK      -2
    #define SFHIP_TCP_OUTPUT_RESET       -3
    #define SFHIP_TCP_OUTPUT_KEEPALIVE   -4
    #define SFHIP_TCP_OUTPUT_ACK         -5 ***

  to perform a different operation.

  *** = These will be automatically generated when appropriate, the user does
  not need to worry about it.

    sfhip_length_or_tcp_code sfhip_tcp_event( sfhip * hip, int sockno,
      uint8_t * ip_payload, int ip_payload_length, int max_out_payload,
      int acked );

  And finally, to get notification of a socket closeure, you will get:

    void sfhip_tcp_socket_closed( sfhip * hip, int sockno );

*/

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifndef HIP_PHY_HEADER_LENGTH_BYTES
	#define HIP_PHY_HEADER_LENGTH_BYTES 0
#endif

#ifndef HIPSTATIC_ASSERT
	#define HIPSTATIC_ASSERT _Static_assert
#endif

#ifndef SFHIP_DHCP_CLIENT
	#define SFHIP_DHCP_CLIENT 1
#endif

#ifndef SFHIP_MTU
	#define SFHIP_MTU 1536
#endif

#ifndef SFHIP_CHECK_UDP_CHECKSUM
	#define SFHIP_CHECK_UDP_CHECKSUM 1
#endif

#ifndef SFHIP_EMIT_UDP_CHECKSUM
	#define SFHIP_EMIT_UDP_CHECKSUM 1
#endif

// Set to 0 for not TCP support
#ifndef SFHIP_TCP_SOCKETS
	#define SFHIP_TCP_SOCKETS 16
#endif

#ifndef SFHIP_CHECK_TCP_CHECKSUM
	#define SFHIP_CHECK_TCP_CHECKSUM 1
#endif

#ifndef SFHIP_EMIT_TCP_CHECKSUM
	#define SFHIP_EMIT_TCP_CHECKSUM 1
#endif

#ifndef SFHIP_WARN
	#define SFHIP_WARN( x... )
#endif

// Options
// #define SFHIP_UDP_USER_HANDLER (function name)

// #define SFHIP_TCP_OVERRIDE_HANDLER (function name)

///////////////////////////////////////////////////////////////////////////////
// Internal

#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
typedef uint16_t hipbe16;
typedef uint32_t hipbe32;
	#define HIPHTONS( x ) ( x )
	#define HIPHTONL( x ) ( x )
	#define HIPNTOHS( x ) ( x )
	#define HIPNTOHL( x ) ( x )
#else
	#ifdef __CHECKER__
		#define __hipbitwise__ __attribute__( ( bitwise ) )
	#else
		#define __hipbitwise__
	#endif

	#ifdef __CHECK_ENDIAN__
		#define __hipbitwise __hipbitwise__
	#else
		#define __hipbitwise
	#endif
typedef uint16_t __hipbitwise hipbe16;
typedef uint32_t __hipbitwise hipbe32;
	#define HIPHTONS( x ) ( ( ( ( x ) & 0xff ) << 8 ) | ( ( x ) >> 8 ) )
	#define HIPHTONL( x )                                                                            \
		( ( ( ( x ) & 0xff ) << 24 ) | ( ( ( x ) & 0xff00 ) << 8 ) | ( ( ( x ) & 0xff0000 ) >> 8 ) | \
		  ( ( ( x ) & 0xff000000 ) >> 24 ) )
	#define HIPNTOHS( x ) ( ( ( ( x ) & 0xff ) << 8 ) | ( ( x ) >> 8 ) )
	#define HIPNTOHL( x )                                                                            \
		( ( ( ( x ) & 0xff ) << 24 ) | ( ( ( x ) & 0xff00 ) << 8 ) | ( ( ( x ) & 0xff0000 ) >> 8 ) | \
		  ( ( ( x ) & 0xff000000 ) >> 24 ) )
#endif

#define HIPALIGN16 __attribute__( ( aligned( 2 ) ) )
#define HIPALIGN32 __attribute__( ( aligned( 32 ) ) )
#define HIPPACK16  HIPALIGN16 __attribute__( ( packed ) )
#define HIPPACK32  HIPALIGN32 __attribute__( ( packed ) )
#define HIPPACK    __attribute__( ( packed ) )

#define HIPMACSTR    "%02x:%02x:%02x:%02x:%02x:%02x"
#define HIPMACV( x ) x.mac[0], x.mac[1], x.mac[2], x.mac[3], x.mac[4], x.mac[5]

#define HIPIPSTR "%d.%d.%d.%d"
#define HIPIPV( x )                                                 \
	( HIPNTOHL( x ) >> 24 ) & 0xff, ( HIPNTOHL( x ) >> 16 ) & 0xff, \
	    ( HIPNTOHL( x ) >> 8 ) & 0xff, ( HIPNTOHL( x ) >> 0 ) & 0xff

#define HIPMACEQUAL( x, y )                                   \
	( ( (uint16_t *)x.mac )[0] == ( (uint16_t *)y.mac )[0] && \
	  ( (uint16_t *)x.mac )[1] == ( (uint16_t *)y.mac )[1] && \
	  ( (uint16_t *)x.mac )[2] == ( (uint16_t *)y.mac )[2] )

#define SFHIP_IPPROTO_UDP  17
#define SFHIP_IPPROTO_TCP  6
#define SFHIP_IPPROTO_ICMP 1

#define SFHIP_TCP_SOCKETS_FLAG_FIN   1
#define SFHIP_TCP_SOCKETS_FLAG_SYN   2
#define SFHIP_TCP_SOCKETS_FLAG_RESET 4
#define SFHIP_TCP_SOCKETS_FLAG_PSH   8
#define SFHIP_TCP_SOCKETS_FLAG_ACK   16

#define SFHIP_TCP_MODE_CLOSED       0
#define SFHIP_TCP_MODE_SENT_SYN_ACK 1
#define SFHIP_TCP_MODE_ESTABLISHED  2
#define SFHIP_TCP_MODE_CLOSING_WAIT 3

#define SFHIP_TCP_OUTPUT_FIN       -1
#define SFHIP_TCP_OUTPUT_SYNACK    -2
#define SFHIP_TCP_OUTPUT_RESET     -3
#define SFHIP_TCP_OUTPUT_KEEPALIVE -4
#define SFHIP_TCP_OUTPUT_ACK       -5

typedef int sfhip_length_or_tcp_code;

///////////////////////////////////////////////////////////////////////////////

// For fixed IPs, this compiles to a constant number.
#define HIPIP( a, b, c, d )                                                                        \
	HIPHTONL( ( ( ( d ) & 0xff ) << 0 ) | ( ( ( c ) & 0xff ) << 8 ) | ( ( ( b ) & 0xff ) << 16 ) | \
	          ( ( ( a ) & 0xff ) << 24 ) )

#define MAXIMUM_UDP_REPLY ( SFHIP_MTU - sizeof( sfhip_mac_header ) - sizeof( sfhip_ip_header ) - \
	                        sizeof( sfhip_udp_header ) )

#define MAXIMUM_TCP_REPLY ( SFHIP_MTU - sizeof( sfhip_mac_header ) - sizeof( sfhip_ip_header ) - \
	                        sizeof( sfhip_tcp_header ) )

typedef struct HIPPACK { uint32_t v; } hipunalignedu32;
typedef struct HIPPACK16 { uint32_t v; } hipunalignedu32a16;

typedef struct HIPPACK16
{
	uint8_t mac[6];
} hipmac;

typedef hipbe32 sfhip_address;

typedef struct HIPPACK16
{
	uint8_t version_ihl;
	uint8_t dscp_ecn;
	hipbe16 length;
	hipbe16 identification;
	hipbe16 offset_and_flags;
	uint8_t ttl;
	uint8_t protocol;
	hipbe16 header_checksum;
	hipbe32 source_address;
	hipbe32 destination_address;
	// Possibly more fields, check IHL in flags.
} sfhip_ip_header;

typedef struct HIPPACK16
{
	hipmac destination;
	hipmac source;
	hipbe16 ethertype;
	// struct sfhip_ip_header ip_header HIPALIGN32;
	// struct sfhip_arp_header arp_header HIPALIGN32;
	//  etc...
} sfhip_mac_header;

typedef struct HIPPACK16
{
	uint8_t phy_header[HIP_PHY_HEADER_LENGTH_BYTES];
	sfhip_mac_header mac_header;
} sfhip_phy_packet;

typedef struct HIPPACK16
{
	uint8_t phy_header[HIP_PHY_HEADER_LENGTH_BYTES];
	sfhip_mac_header mac_header;
	uint8_t payload[SFHIP_MTU - sizeof( sfhip_mac_header )];
} sfhip_phy_packet_mtu;

typedef struct HIPPACK16
{
	hipbe16 source_port;
	hipbe16 destination_port;
	hipbe16 length;
	hipbe16 checksum;
} sfhip_udp_header;

typedef struct HIPPACK16
{
	hipbe16 source_port;
	hipbe16 destination_port;
	hipbe32 seqno;
	hipbe32 ackno;
	hipbe16 flags;
	hipbe16 window;
	hipbe16 checksum;
	hipbe16 urgent;
} sfhip_tcp_header;

typedef struct HIPPACK16
{
	hipbe16 hwtype;
	hipbe16 protocol;
	uint8_t hwlen;
	uint8_t protolen;
	hipbe16 operation;
	hipmac sender;
	sfhip_address sproto;
	hipmac target;
	sfhip_address tproto;
} sfhip_arp_header;

typedef struct HIPPACK16
{
	uint8_t type;
	uint8_t code;
	hipbe16 csum;
	hipbe16 identifier;
	hipbe16 sequence;
} sfhip_icmp_header;

#if SFHIP_TCP_SOCKETS
typedef struct
{
	void * opaque;

	sfhip_address remote_address; // this being nonzero is what determines if a
	                              // socket is active or not.
	hipbe16 local_port;
	hipbe16 remote_port;
	uint32_t seq_num;
	uint32_t ack_num;
	hipmac remote_mac;
	uint16_t retry;
	uint32_t pending_send_time;
	uint16_t pending_send_size;
	uint8_t mode; // SFHIP_TCP_MODE_*
	uint8_t retry_number;
	uint8_t ms1024_since_last_rx_packet; // For keep-alive
} tcp_socket;
#endif

typedef struct
{
	void * opaque;

	hipmac self_mac;
	sfhip_address ip;
	sfhip_address mask;
	sfhip_address gateway;

	// This will wrap around.
	uint32_t ms_elapsed;

#if SFHIP_DHCP_CLIENT
	// if -1, has perma IP
	// max lease 24.9 days.
	int32_t dhcp_timer;
	uint32_t dhcp_transaction_id_last;
	const char * hostname;
#endif

#if SFHIP_TCP_SOCKETS
	tcp_socket tcps[SFHIP_TCP_SOCKETS];
#endif

	// Smaller types
	uint16_t tick_event_last_sent;
	uint16_t txid;
	// Bitfileds go at end.
#if SFHIP_DHCP_CLIENT
	int need_to_discover : 1;
	int tries_before_discover : 3;
#endif

} sfhip;

// You must call.
int sfhip_accept_packet( sfhip * hip, sfhip_phy_packet_mtu * data, int length );
int sfhip_tick( sfhip * hip, sfhip_phy_packet_mtu * scratch, int milliseconds );

// You must implement.
int sfhip_send_packet( sfhip * hip, sfhip_phy_packet * data, int length );

#if SFHIP_DHCP_CLIENT
void sfhip_got_dhcp_lease( sfhip * hip, sfhip_address addr );
#endif

#if SFHIP_TCP_SOCKETS
int sfhip_tcp_accept_connection(
    sfhip * hip, int sockno, int localport,
    hipbe32 remote_host ); // return 0 to accept, -1 to abort.

sfhip_length_or_tcp_code sfhip_tcp_event( sfhip * hip, int sockno,
                                          uint8_t * ip_payload,
                                          int ip_payload_length,
                                          int max_out_payload, int acked );
void sfhip_tcp_socket_closed( sfhip * hip, int sockno );
#endif

// Utility functions
int sfhip_send_udp_packet( sfhip * hip,
                           sfhip_phy_packet_mtu * pkt,
                           hipmac destination_mac,
                           sfhip_address destination_address,
                           int source_port,
                           int destination_port,
                           int payload_length ) __attribute__( ( noinline ) );

// Constants
extern hipmac sfhip_mac_broadcast;

#ifdef SFHIP_IMPLEMENTATION

// Internal functions
hipbe16 internet_checksum( uint8_t * data, int length ) __attribute__( ( noinline ) );

void sfhip_make_ip_packet( sfhip * hip,
                           sfhip_phy_packet_mtu * pkt,
                           hipmac destination_mac,
                           sfhip_address destination_address ) __attribute__( ( noinline ) );

// Shortcuts to reply-to-sender.
int sfhip_mac_reply( sfhip * hip, sfhip_phy_packet * data, int length );
int sfhip_ip_reply( sfhip * hip, sfhip_phy_packet * data, int length );

hipmac sfhip_mac_broadcast = { { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff } };

int sfhip_mac_reply( sfhip * hip, sfhip_phy_packet * data, int length )
{
	sfhip_mac_header * mac = &data->mac_header;
	mac->destination = mac->source;
	mac->source = hip->self_mac;
	return sfhip_send_packet( hip, data, length );
}

int sfhip_ip_reply( sfhip * hip, sfhip_phy_packet * data, int length )
{
	sfhip_mac_header * mac = &data->mac_header;
	sfhip_ip_header * iph = (void *)( mac + 1 );
	iph->destination_address = iph->source_address;
	iph->source_address = hip->ip;
	return sfhip_mac_reply( hip, data, length );
}

hipbe16 sfhip_internet_checksum( uint16_t * data, int length )
{
	uint32_t sum = 0;
	uint16_t * end = data + ( length >> 1 );
	for ( ; data != end; data++ )
		sum += *data;
	if ( length & 1 )
		sum += *( (uint8_t *)data );
	while ( sum >> 16 )
		sum = ( sum & 0xffff ) + ( sum >> 16 );

	return ( ( (uint16_t)~sum ) );
}

void sfhip_make_ip_packet( sfhip * hip,
                           sfhip_phy_packet_mtu * pkt,
                           hipmac destination_mac,
                           sfhip_address destination_address )
{
	// A little weird. We don't take on a source or destination port here.
	// No need to dirty up the ABI register allocation.
	sfhip_mac_header * mac = &pkt->mac_header;
	mac->destination = destination_mac;
	mac->source = hip->self_mac;
	mac->ethertype = HIPHTONS( 0x0800 );

	sfhip_ip_header * ip = (sfhip_ip_header *)( mac + 1 );

	ip->destination_address = destination_address;
	ip->version_ihl = 0x45;
	ip->dscp_ecn = 0x00;
	ip->length = 0;
	ip->identification = hip->txid++; // Endian doesn't matter.

	// Do not initialize ttl, protocol or header_checksum, since they are
	// used for the UDP pseduo-header checksum later.
	ip->offset_and_flags = 0x0000;
	ip->source_address = hip->ip;
}

int sfhip_send_udp_packet( sfhip * hip,
                           sfhip_phy_packet_mtu * pkt,
                           hipmac destination_mac,
                           sfhip_address destination_address,
                           int source_port,
                           int destination_port,
                           int payload_length )
{
	sfhip_make_ip_packet( hip, pkt, destination_mac, destination_address );

	sfhip_ip_header * ip = (sfhip_ip_header *)( ( &pkt->mac_header ) + 1 );
	sfhip_udp_header * udp = (sfhip_udp_header *)( ip + 1 );

	udp->length = 0;
	udp->checksum = 0;
	udp->destination_port = HIPHTONS( destination_port );
	udp->source_port = HIPHTONS( source_port );

	ip->length = HIPHTONS( sizeof( sfhip_ip_header ) + sizeof( sfhip_udp_header ) +
	                       payload_length );
	udp->length = HIPHTONS( payload_length + sizeof( sfhip_udp_header ) );

	// Build and compute checksum on UDP pseudo-header in-place.
	uint16_t * csumstart = ( (void *)udp ) - 12;
	csumstart[0] = SFHIP_IPPROTO_UDP << 8;
	csumstart[1] = udp->length; // XXX Why no flip endian? SUSSY

	#if SFHIP_EMIT_UDP_CHECKSUM
	uint16_t udpcsum = sfhip_internet_checksum(
	    (uint16_t *)csumstart, payload_length + sizeof( sfhip_udp_header ) + 12 );
	// Per RFC 768, on send, if checksum is 0x0000, set it to 0xffff.
	if ( udpcsum == 0x0000 )
		udpcsum = 0xffff;
	udp->checksum = udpcsum;
	#endif

	// Fixup overwritten pseudo header. Note these fields are never
	// initialized, so we have to initialize them here!!
	ip->ttl = 64;
	ip->protocol = SFHIP_IPPROTO_UDP;
	ip->header_checksum = 0;

	uint16_t hs =
	    sfhip_internet_checksum( (uint16_t *)ip, sizeof( sfhip_ip_header ) );
	ip->header_checksum = hs;

	int packlen = payload_length + HIP_PHY_HEADER_LENGTH_BYTES +
	              sizeof( sfhip_mac_header ) + sizeof( sfhip_ip_header ) +
	              sizeof( sfhip_udp_header );
	return sfhip_send_packet( hip, (sfhip_phy_packet *)pkt, packlen );
}

	#if SFHIP_DHCP_CLIENT

int sfhip_dhcp_client_request( sfhip * hip, sfhip_phy_packet_mtu * scratch )
{
	// No matter what, we want to give the server time to respond.
	hip->dhcp_timer = 2;

	typedef struct HIPPACK16
	{
		uint8_t phy_header[HIP_PHY_HEADER_LENGTH_BYTES];
		sfhip_mac_header mac_header;
		sfhip_ip_header ip_header;
		sfhip_udp_header udp_header;
		uint8_t request;
		uint8_t hwtype;
		uint8_t hwlen;
		uint8_t hops;
		uint32_t transaction_id;
		hipbe16 seconds_elapsed;
		hipbe16 bootp_flags;
		sfhip_address client_address;
		sfhip_address next_address;
		sfhip_address next_server_address;
		sfhip_address relay_agent_address;
		hipmac client_mac;
		char macpadding[10];
		char server_name[64];
		char boot_name[128];
		hipbe32 magic_cookie;
		uint8_t additional_dhcp_payload[SFHIP_MTU - 282];
	} sfhip_phy_packet_dhcp_request;

	sfhip_phy_packet_dhcp_request * req_packet =
	    (sfhip_phy_packet_dhcp_request *)scratch;

	if ( !hip->need_to_discover && ( hip->tries_before_discover++ ) > 3 )
	{
		hip->need_to_discover = true;
	}

	uint32_t txid = hip->dhcp_transaction_id_last = hip->ms_elapsed;

	// Only set fixed value fields here, dynamic fields manually fill in
	*req_packet = ( sfhip_phy_packet_dhcp_request ){
	    .request = 0x01, // "Request"
	    .hwtype = 0x01,  // "Ethernet"
	    .hwlen = 0x06,   // MAC Address length
	    .hops = 0,
	    .seconds_elapsed = HIPHTONS( 1 ),
	    .bootp_flags = 0,                       // Unicast
	    .magic_cookie = HIPHTONL( 0x63825363 ), // DHCP magic cookie.
	    .additional_dhcp_payload = {
	        0x35, // DHCP Request
	        0x01, // Length 1
	        0x00, // Placeholder. See below.
	        0x37, // Parameter Request List
	        0x02,
	        0x01,
	        0x03, // Request subnet mask and router
	    } };

	// Splitting the dynamic poritons of the struct that are dynamic
	// cuts out about 100 bytes of code.
	req_packet->transaction_id = txid;
	req_packet->client_address = hip->ip;
	req_packet->client_mac = hip->self_mac;

	// Discover or request
	req_packet->additional_dhcp_payload[2] = hip->need_to_discover ? 0x01 : 0x03;

	// 8 = size of additional_dhcp_payload that is filled out.
	uint8_t * dhcpend = req_packet->additional_dhcp_payload + 7;
	uint8_t * dhcpeof = (uint8_t *)( req_packet + 1 );

	// Need some free room.
	if ( dhcpeof - dhcpend < 8 )
		return 0;

	if ( !hip->need_to_discover && hip->ip )
	{
		typedef struct HIPPACK
		{
			uint8_t req, len;
			hipbe32 ip;
		} dhcp_ip_request_packet;

		*( (dhcp_ip_request_packet *)dhcpend ) = ( dhcp_ip_request_packet ){ 0x32, 0x04, hip->ip };
		dhcpend += sizeof( dhcp_ip_request_packet );
	}

	if ( hip->hostname )
	{
		*( dhcpend++ ) = 0x0c;
		uint8_t * sstart = dhcpend++;
		const char * s = hip->hostname;
		char c;
		while ( ( c = *( s++ ) ) != 0 )
		{
			*( dhcpend++ ) = c;
			if ( dhcpeof - dhcpend < 3 )
				return 0; // Make sure we don't overflow.
		}
		*sstart = dhcpend - sstart - 1;
	}

	*( dhcpend++ ) = 0xff; // DHCP end.

	int plen = (int)( dhcpend - (uint8_t *)&req_packet->request );

	sfhip_send_udp_packet( hip, (sfhip_phy_packet_mtu *)req_packet, sfhip_mac_broadcast, 0xffffffff, 68, 67, plen );

	return 1;
}

int sfhip_dhcp_handle( sfhip * hip,
                       sfhip_phy_packet_mtu * original_packet,
                       uint8_t * data,
                       int length )
{
	typedef struct HIPPACK16
	{
		uint8_t request;
		uint8_t hwtype;
		uint8_t hwlen;
		uint8_t hops;
		uint32_t transaction_id;
		hipbe16 seconds_elapsed;
		hipbe16 bootp_flags;
		sfhip_address client_address;
		sfhip_address your_client_address;
		sfhip_address next_server_address;
		sfhip_address relay_agent_address;
		hipmac client_mac;
		char macpadding[10];
		char server_name[64];
		char boot_name[128];
		hipbe32 magic_cookie;
	} dhcp_reply;

	// Need +2 to be able to read the code + length for first
	// DHCP entry.
	if ( (size_t)length < sizeof( dhcp_reply ) + 2 )
		return -1;

	dhcp_reply * d = (dhcp_reply *)data;

	// Make sure it's a bootp reply, and it's ours.
	if ( d->request != 0x02 ||
	     d->transaction_id != hip->dhcp_transaction_id_last ||
	     d->magic_cookie != HIPHTONL( 0x63825363 ) ||
	     !HIPMACEQUAL( d->client_mac, hip->self_mac ) )
		return 0;

	uint32_t dhcp_ack_lease_time = 0;
	uint8_t dhcp_type = 0;
	sfhip_address dhcp_offer_router = 0;
	sfhip_address dhcp_offer_mask = 0;

	uint8_t * dhcp = (uint8_t *)( d + 1 );
	uint8_t * dhcpend = (uint8_t *)( data ) + length;
	do
	{
		uint8_t dhcp_code = *( dhcp++ );
		uint8_t dhcp_length = *( dhcp++ );
		if ( dhcp + dhcp_length >= dhcpend )
			break;

		uint32_t value = 0;
		int i = 0;
		for ( i = 0; i < dhcp_length; i++ )
			value = ( value << 8 ) | (uint32_t)dhcp[i];

		switch ( dhcp_code )
		{
			case 53: {
				// DHCP message type
				dhcp_type = dhcp[0];
				if ( dhcp_type == 6 )
				{
					// NAK: If nak, try to get an IP without preconception.
					hip->need_to_discover = true;

					// Re-request immediately.
					sfhip_dhcp_client_request( hip, original_packet );

					return 1;
				}
				// Offer = 2, Ack == 5
				break;
			}
			case 51: // Address lease time
			case 58: // Address renewal time  (prefer, if present)
				if ( dhcp_code == 58 )
					dhcp_ack_lease_time = value;
				else if ( dhcp_ack_lease_time == 0 )
					dhcp_ack_lease_time = value;
				break;
			case 1:
				dhcp_offer_mask = HIPHTONL( value );
				break;
			case 3:
				dhcp_offer_router = HIPHTONL( value );
				break;
			case 255:
				dhcp = dhcpend;
				break; // Force abort.
		};
		dhcp += dhcp_length;
	} while ( dhcp + 2 < dhcpend );

	int sent = 0;

	if ( dhcp_type == 2 && dhcp_offer_router && dhcp_offer_mask &&
	     d->your_client_address )
	{
		hip->need_to_discover = false;
		hip->tries_before_discover = 0;
		hip->ip = d->your_client_address;
		hip->mask = dhcp_offer_mask;
		hip->gateway = dhcp_offer_router;

		sfhip_got_dhcp_lease( hip, hip->ip );

		// printf( "IP   " HIPIPSTR "\nMASK " HIPIPSTR "\nGATE " HIPIPSTR "\n",
		//	 HIPIPV( hip->ip ), HIPIPV( hip->mask ), HIPIPV( hip->gateway ) );

		// Properly request.  Immediately.
		sent = sfhip_dhcp_client_request( hip, original_packet );
	}

	if ( dhcp_type == 5 )
	{
		// Lease is confirmed.

		// 2 hours (default lease time)
		if ( dhcp_ack_lease_time == 0 )
			dhcp_ack_lease_time = 7200;

		// Make sure we don't overflow our timer for leasing.
		if ( dhcp_ack_lease_time >= 2147482 )
			dhcp_ack_lease_time = 2147482;

		hip->dhcp_timer = dhcp_ack_lease_time;
	}

	return sent;
}

	#endif

int sfhip_handle_udp( sfhip * hip,
                      sfhip_phy_packet_mtu * pkt,
                      void * ip_payload,
                      int ip_payload_length )
{
	sfhip_udp_header * udp = ip_payload;

	int payload_remain = ip_payload_length - sizeof( sfhip_udp_header );

	if ( payload_remain < 0 )
		return -1;

	int ulen = HIPNTOHS( udp->length ) - 8;

	if ( ulen != payload_remain || ulen < 0 )
	{
		// UDP packet size does not match, or runt packet.
		return -1;
	}

	#if SFHIP_CHECK_UDP_CHECKSUM
	if ( udp->checksum )
	{
		if ( sfhip_internet_checksum( ip_payload - 12, ulen + 20 ) )
			return -1;
	}
	#endif

	int source_port = HIPNTOHS( udp->source_port );
	int destination_port = HIPNTOHS( udp->destination_port );
	uint8_t * payload = (uint8_t *)( udp + 1 );

	#if SFHIP_DHCP_CLIENT
	if ( source_port == 67 && destination_port == 68 )
		return sfhip_dhcp_handle( hip, pkt, payload, ulen );
	#endif

	#ifdef SFHIP_UDP_USER_HANDLER

	int SFHIP_UDP_USER_HANDLER( sfhip * hip, sfhip_phy_packet_mtu * pkt,
	                            uint8_t * payload, int ulen, int source_port, int destination_port );

	return SFHIP_UDP_USER_HANDLER( hip, pkt, payload, ulen, source_port,
	                               destination_port );
	#else
	return 0;
	#endif
}

	#if SFHIP_TCP_SOCKETS

void sfhip_make_tcp_packet( sfhip * hip,
                            sfhip_phy_packet_mtu * pkt,
                            tcp_socket * sock )
{
//	sfhip_mac_header * mac = &pkt->mac_header;
//	sfhip_ip_header * ip = (sfhip_ip_header *)( mac + 1 );
//	sfhip_tcp_header * tcp = (sfhip_tcp_header *)( ip + 1 );

	sfhip_make_ip_packet( hip, pkt, sock->remote_mac, sock->remote_address );
}

int sfhip_send_tcp_packet( sfhip * hip,
                           sfhip_phy_packet_mtu * pkt,
                           sfhip_length_or_tcp_code payload_length,
                           tcp_socket * sock )
{
	sfhip_ip_header * ip = (sfhip_ip_header *)( ( &pkt->mac_header ) + 1 );
	sfhip_tcp_header * tcp = (sfhip_tcp_header *)( ip + 1 );

	int optionadd = 0;
	int flags = 0;
	int seqsub = 0;

	switch ( payload_length )
	{
		case 0:
			return 0;
		default:
			if ( payload_length > 0 )
			{
				flags = SFHIP_TCP_SOCKETS_FLAG_PSH;
				sock->pending_send_size = payload_length;
				break;
			}
		case SFHIP_TCP_OUTPUT_ACK:
			payload_length = 0;
			break;
		case SFHIP_TCP_OUTPUT_RESET:
			flags = SFHIP_TCP_SOCKETS_FLAG_RESET;
			sock->remote_address = 0;
			sock->seq_num = HIPHTONL( tcp->ackno );
			payload_length = 0;
			break;
		case SFHIP_TCP_OUTPUT_SYNACK:
			flags = SFHIP_TCP_SOCKETS_FLAG_SYN;
			sock->pending_send_size = 1;
			payload_length = 0;
			break;
		case SFHIP_TCP_OUTPUT_FIN:
			flags = SFHIP_TCP_SOCKETS_FLAG_FIN;
			sock->mode = SFHIP_TCP_MODE_CLOSING_WAIT;
			sock->pending_send_size = 1;
			payload_length = 0;
			break;
		case SFHIP_TCP_OUTPUT_KEEPALIVE:
			flags = SFHIP_TCP_SOCKETS_FLAG_PSH;
			payload_length = 0;
			seqsub = 1; // one less sequence numbers is how TCP handles keepalive.
			break;
	}

	flags |= SFHIP_TCP_SOCKETS_FLAG_ACK;

	// uip does this... not sure why.
	if ( flags & SFHIP_TCP_SOCKETS_FLAG_SYN )
	{
		payload_length = 4;
		optionadd = 4;
		( (hipunalignedu32a16 *)( tcp + 1 ) )->v = HIPHTONL(
		    0x02040000 |
		    ( SFHIP_MTU - sizeof( sfhip_tcp_header ) - sizeof( sfhip_ip_header ) -
		      sizeof( sfhip_phy_packet ) - 18 /* to just make it a smoler */ ) );
	}

	tcp->source_port = sock->local_port;
	tcp->destination_port = sock->remote_port;
	tcp->seqno = HIPHTONL( sock->seq_num - seqsub );
	tcp->ackno = HIPHTONL( sock->ack_num );
	tcp->window = HIPHTONS( SFHIP_MTU - sizeof( sfhip_tcp_header ) -
	                        sizeof( sfhip_ip_header ) - sizeof( sfhip_phy_packet ) );
	tcp->checksum = 0;
	tcp->urgent = 0;

	tcp->flags = HIPHTONS( ( (uint8_t)flags ) | ( ( ( sizeof( sfhip_tcp_header ) + optionadd ) >> 2 ) << 12 ) );

	ip->length = HIPHTONS( sizeof( sfhip_ip_header ) + sizeof( sfhip_tcp_header ) + payload_length );

	// Build and compute checksum on TCP pseudo-header in-place.
	uint16_t * csumstart = ( (void *)tcp ) - 12;
	csumstart[0] = SFHIP_IPPROTO_TCP << 8;
	csumstart[1] = HIPHTONS( sizeof( sfhip_tcp_header ) + payload_length );

		#if SFHIP_EMIT_TCP_CHECKSUM
	uint16_t csum = sfhip_internet_checksum( (uint16_t *)csumstart, payload_length + sizeof( sfhip_tcp_header ) + 12 );
	// No 0x0000 option for payload (maybe) TODO checkme.
	// if( udpcsum == 0x0000 ) udpcsum = 0xffff;
	tcp->checksum = csum;
		#endif

	// Fixup overwritten pseudo header. Note these fields are never
	// initialized, so we have to initialize them here!!
	ip->ttl = 64;
	ip->protocol = SFHIP_IPPROTO_TCP;
	ip->header_checksum = 0;

	uint16_t hs =
	    sfhip_internet_checksum( (uint16_t *)ip, sizeof( sfhip_ip_header ) );
	ip->header_checksum = hs;

	int packlen = payload_length + HIP_PHY_HEADER_LENGTH_BYTES +
	              sizeof( sfhip_mac_header ) + sizeof( sfhip_ip_header ) +
	              sizeof( sfhip_tcp_header );
	return sfhip_send_packet( hip, (sfhip_phy_packet *)pkt, packlen );
}

int sfhip_makeandsend_tcp_packet( sfhip * hip,
                                  sfhip_phy_packet_mtu * pkt,
                                  sfhip_length_or_tcp_code payload_length,
                                  tcp_socket * sock )
{
	sfhip_make_tcp_packet( hip, pkt, sock );
	return sfhip_send_tcp_packet( hip, pkt, payload_length, sock );
}

int sfhip_handle_tcp( sfhip * hip,
                      sfhip_phy_packet_mtu * data,
                      void * ip_payload,
                      int ip_payload_length )
{
	sfhip_tcp_header * tcp = ip_payload;

	if ( ip_payload_length - sizeof( sfhip_tcp_header ) < 0 )
		return -1;

	uint16_t flags = HIPNTOHS( tcp->flags );

	int hlen = ( flags >> 12 ) << 2;

	// TCP packet size does not match, or runt packet.
	if ( ip_payload_length < 0 )
		return -1;

		#if SFHIP_CHECK_TCP_CHECKSUM || SFHIP_CHECK_UDP_CHECKSUM
	sfhip_address sender = ( (hipunalignedu32a16 *)ip_payload )[-2].v;
		#else
	sfhip_address sender = ( (sfhip_ip_header *)( data->payload ) )->source_address;
		#endif

		#if SFHIP_CHECK_TCP_CHECKSUM
	// Build pseudo-header for checksum.
	uint16_t ccsum =
	    sfhip_internet_checksum( ip_payload - 12, ip_payload_length + 12 );

	if ( ccsum )
		return -1;
		#endif

	ip_payload_length -= hlen;
	ip_payload += hlen;

	tcp_socket * ts = hip->tcps;

	sfhip_length_or_tcp_code payload_output = 0;

	int sockno = 0;
	tcp_socket * tsend = ts + SFHIP_TCP_SOCKETS;
	do
	{
		if ( ts->remote_address == sender && ts->remote_port == tcp->source_port &&
		     ts->local_port == tcp->destination_port )
		{
			break;
		}
		ts++;
		sockno++;
	} while ( ts != tsend );

	uint32_t seqno = HIPNTOHL( tcp->seqno );
	uint32_t ackno = HIPNTOHL( tcp->ackno );

	// In case we need to abort.  Do not initialize.
	// If we do need to abort, it will be initialized later.
	tcp_socket sabort;

	if ( ts == tsend || ( flags & SFHIP_TCP_SOCKETS_FLAG_SYN ) )
	{
		// This is funky because we might be in a situation where
		// the syn packet from the remote side was lost.  If so
		// we have to just accept the new packet.  And even more
		// tricky, what if something weied happened like ack/seq
		// changing, like if the connection went away and came
		// back.
		if ( ts == tsend )
		{
			// Tricky: This code path also happens for non-syn
			// packets that don't match the filter.  So if they
			// are non-syn packets, then, we should immediately
			// drop the connection.

			int o = 0;

			if ( flags & SFHIP_TCP_SOCKETS_FLAG_SYN )
			{
				ts = hip->tcps;
				sockno = 0;
				do
				{
					if ( !ts->remote_address )
						break;

					ts++;
					sockno++;
					// No free sockets.
					if ( ts == tsend )
					{
						ts = &sabort;
						break;
					}
				} while ( 1 );

				o = sfhip_tcp_accept_connection(
				    hip, sockno, HIPNTOHS( tcp->destination_port ), sender );
			}

			// This will get triggered if the application rejects the connection, or
			// if we have a totally unsolicited message
			if ( !o )
			{
				ts = &sabort;
			}

			*ts = ( tcp_socket ){
			    .remote_address = sender,
			    .local_port = tcp->destination_port,
			    .remote_port = tcp->source_port,
			    .seq_num = HIPNTOHL( hip->ms_elapsed ),
			    .ack_num = HIPNTOHL( tcp->seqno ),
			    .remote_mac = data->mac_header.source,
			};
		}
		else
		{
			// We have a matching packet.  But we got a repeated SYN.

			ts->ack_num = seqno;
			ts->remote_mac = data->mac_header.source;
		}

		// No socket allocated, need to abort. This could be either because
		// we were not granted a socket, or, we got a non-syn packet.
		if ( ts == &sabort )
			payload_output = SFHIP_TCP_OUTPUT_RESET;
		else
			payload_output = SFHIP_TCP_OUTPUT_SYNACK;

		ts->ack_num++;

		// Increment seq because we are sending a synack.
		ts->mode = SFHIP_TCP_MODE_SENT_SYN_ACK;
		ts->pending_send_size = 1; // syn counts as data.

		goto send_reply_addheader;
	}

	// Here, we now have ts, our socket. Figure out what to do.
	// Any other wacky situations have been handled above.

	sfhip_make_tcp_packet( hip, data, ts );

	ts->ms1024_since_last_rx_packet = 0;

	// Tricky - this has to be here, because if the remote side
	// sends us an ack, but we miss it, we can get stuck in this
	// mode. Remote sides have no way of confirming we received
	// an ack.  So, we assume that we dropped it and we can pick
	// up with the next PSH.

	if ( ts->mode == SFHIP_TCP_MODE_SENT_SYN_ACK )
	{
		int ackdiff = ackno - ts->seq_num;
		if ( ackdiff == 1 )
		{
			ts->mode = SFHIP_TCP_MODE_ESTABLISHED;
			ts->seq_num = ackno;
			ts->pending_send_size = 0;
		}
		else
		{
			SFHIP_WARN( "ACK Disagreement on SYN (%d)\n", ackdiff );
		}
	}

	// Discussion: there is some concern for the most convenient way for a
	// client to process this data, whether you call-back the ACK first or
	// you process the psh.

	int acked = 0;

	if ( flags & SFHIP_TCP_SOCKETS_FLAG_ACK )
	{
		int ackdiff = ackno - ts->seq_num;

		if ( ts->pending_send_size )
		{
			if ( ackdiff != ts->pending_send_size )
			{
				// SFHIP_WARN( "ACK Disagreement on established connection (%d,
				// %d)\n", ackdiff, ts->pending_send_size );

				// This can happen if a packet was dropped in normal course.
				// but ackdiff = 0 for that. No need to check the other corner cases.
			}
			else
			{
				ts->pending_send_size = 0;
				ts->pending_send_time = 0;
				ts->seq_num = ackno;
				acked = ackdiff;
				if ( ts->mode == SFHIP_TCP_MODE_CLOSING_WAIT )
				{
					sfhip_tcp_socket_closed( hip, sockno );
					ts->remote_address = 0;
					// Don't stop here, do the rest of the FIN flag check
				}
			}
		}
	}

	{
		int received_payload = 0;

		if ( flags & SFHIP_TCP_SOCKETS_FLAG_PSH )
		{
			if ( ts->mode != SFHIP_TCP_MODE_ESTABLISHED )
			{
				payload_output = SFHIP_TCP_OUTPUT_RESET;
				goto send_reply;
			}

			int seqdiff = seqno - ts->ack_num;

			if ( seqdiff == 0 )
			{
				ts->ack_num += ip_payload_length;
				received_payload = ip_payload_length;
			}
		}

		int cansend;
		if ( ts->pending_send_size == 0 )
			cansend = MAXIMUM_TCP_REPLY;
		else
			cansend = 0;

		payload_output = sfhip_tcp_event( hip, sockno, ip_payload, received_payload, cansend, acked );

		// Tricky: If we had a PSH, and no reply, we still need to send an ACK
		if ( payload_output == 0 && ( flags & SFHIP_TCP_SOCKETS_FLAG_PSH ) )
		{
			payload_output = SFHIP_TCP_OUTPUT_ACK;
		}
	}

	if ( flags & SFHIP_TCP_SOCKETS_FLAG_FIN )
	{
		// This is not hit on retry sends from a remote side.  Instead in those
		// cases, we just hit the RESET path above.

		ts->ack_num = seqno + 1;

		if ( ts->mode == SFHIP_TCP_MODE_CLOSING_WAIT )
		{
			// This is the normal path for after the foreign side sends a
			// FINACK to us in response to our FIN to them.
			payload_output = SFHIP_TCP_OUTPUT_ACK;
		}
		else
		{
			ts->pending_send_size = 1; // FIN counts as size.
			ts->mode = SFHIP_TCP_MODE_CLOSING_WAIT;
			payload_output = SFHIP_TCP_OUTPUT_FIN;
		}

		goto send_reply;
	}

	if ( payload_output )
		goto send_reply;

	return 0;

send_reply_addheader:
	sfhip_make_tcp_packet( hip, data, ts );

send_reply:
	return sfhip_send_tcp_packet( hip, data, payload_output, ts );
}
	#endif

int sfhip_accept_packet( sfhip * hip, sfhip_phy_packet_mtu * data, int length )
{
	// Make sure packet is not a runt frame.  This includes the PHY and etherlink
	// frame.
	int payload_length = length - sizeof( sfhip_phy_packet );

	if ( payload_length < 0 )
		return -1;

	sfhip_mac_header * mac = &data->mac_header;

	int ethertype_be = mac->ethertype;

	if ( !( HIPMACEQUAL( mac->destination, sfhip_mac_broadcast ) ||
	        HIPMACEQUAL( mac->destination, hip->self_mac ) ) )
	{
		// printf( "MEF" HIPMACSTR " "  HIPMACSTR "\n", HIPMACV( mac->destination ),
		// HIPMACV( sfhip_mac_broadcast ) );
		return 0;
	}

	// Filter for IP4.
	if ( ethertype_be == HIPHTONS( 0x0800 ) )
	{
		payload_length -= sizeof( sfhip_ip_header );

		if ( payload_length < 0 )
			return -1;

		// Assume phy_header is opaque to us.
		sfhip_ip_header * iph = (void *)( mac + 1 );

		int hlen = ( iph->version_ihl & 0xf ) << 2;
		int version = iph->version_ihl >> 4;

		// Make sure it's a valid IPv4 header.
		if ( hlen < 20 || version != 4 )
			return 0;

		int ip_payload_length = HIPNTOHS( iph->length ) - hlen;

		void * ip_payload = ( (void *)iph ) + hlen;

		// Check for packet overflow.
		if ( ip_payload_length > payload_length )
			return -1;

		payload_length -= hlen;

		// Here, you have the following to work with:
		// ip_payload_length = payload length of internal IP packet, of UDP, for
		// instance, need to subtract that. ip_payload     = pointer to payload
		// header, i.e. a UDP packet header.

		int protocol = iph->protocol;

		if ( protocol == SFHIP_IPPROTO_ICMP )
		{
			if ( (size_t)ip_payload_length < sizeof( sfhip_icmp_header ) )
				return -1;

			sfhip_icmp_header * icmp = ip_payload;

			// Only handle requests, no replies yet.
			if ( icmp->type == 8 )
			{
				icmp->type = 0;
				icmp->csum = 0;
				icmp->csum = sfhip_internet_checksum( ip_payload, ip_payload_length );
				sfhip_ip_reply( hip, (sfhip_phy_packet *)data, length );
			}
			return 0;
		}

	#if SFHIP_CHECK_UDP_CHECKSUM || SFHIP_CHECK_TCP_CHECKSUM
		// Setup the psudoheader.  We can use a common setup for UDP and TCP.
		struct pseudo_header
		{
			uint32_t protolen;
			uint32_t sourceaddy;
			uint32_t destaddy;
		} HIPPACK16 * pse = ip_payload - 12;

		sfhip_address * pseudoheader = ip_payload - 12;
		if ( hlen < 20 )
		{
			pse->sourceaddy = iph->source_address;
			pse->destaddy = iph->destination_address;
		}
		pse->protolen = ( iph->protocol << 24 ) | HIPNTOHS( ip_payload_length );
	#endif

		switch ( protocol )
		{
			case SFHIP_IPPROTO_UDP:
				return sfhip_handle_udp( hip, data, ip_payload, ip_payload_length );
	#if SFHIP_TCP_SOCKETS
			case SFHIP_IPPROTO_TCP:
				if ( iph->destination_address == hip->ip )
					return sfhip_handle_tcp( hip, data, ip_payload, ip_payload_length );
				break;
	#elif defined( SFHIP_TCP_OVERRIDE_HANDLER )
			case SFHIP_IPPROTO_TCP:

				int SFHIP_TCP_OVERRIDE_HANDLER( sfhip * hip,
				                                sfhip_phy_packet_mtu * data,
				                                void * ip_payload,
				                                int ip_payload_length );

				if ( iph->destination_address == hip->ip )
					return SFHIP_TCP_OVERRIDE_HANDLER( hip, data, ip_payload, ip_payload_length );
				break;
	#endif

			default:
				break;
		}

		return 0;
	}
	else if ( ethertype_be == HIPHTONS( 0x0806 ) )
	{
		// ARP packet
		sfhip_arp_header * arp = (void *)( mac + 1 );

		payload_length -= sizeof( sfhip_arp_header );

		if ( payload_length < 0 )
			return -1;

		if ( arp->operation == HIPHTONS( 0x01 ) )
		{
			// ARP request

			// TODO: Should we support broadcast replies?
			if ( arp->tproto != hip->ip )
				return 0;

			// Edit ARP and send it back.
			arp->target = arp->sender;
			arp->tproto = arp->sproto;
			arp->sender = hip->self_mac;
			arp->sproto = hip->ip;
			arp->operation = HIPHTONS( 0x02 );

			return sfhip_mac_reply( hip, (sfhip_phy_packet *)data, length );
		}
		else
		{
			// ARP reply (handle later, only useful when we are a client)
		}
	}
	else
	{
		// Other possible protocol, i.e. 0x86DD == IPv6
		// printf( "BE: %04x\n", ethertype_be );
	}

	return 0;
}

int sfhip_tick( sfhip * hip, sfhip_phy_packet_mtu * scratch, int dt_ms )
{
	int sent = 0;
	int cursor = 0;
	int tsl = hip->tick_event_last_sent;

	int hipms = hip->ms_elapsed;
	int deltamask = ( hipms ^ ( hipms + dt_ms ) );

	// This is true on second-boundaries.  We do this by lookin
	// for changes in bits >= 1024 ms.  This could be precisely
	// 1 second, but I rather like this cute behavior.
	int second_tick = !!( deltamask & 0xfffffc00 );

	#if SFHIP_DHCP_CLIENT
	if ( tsl < ++cursor )
	{
		if ( second_tick )
		{
			if ( hip->dhcp_timer-- < 0 )
			{
				sent = sfhip_dhcp_client_request( hip, scratch );
				goto done;
			}
		}
	}
	#endif

	#if SFHIP_TCP_SOCKETS
	tcp_socket * ss = hip->tcps;
	tcp_socket * ssend = ss + SFHIP_TCP_SOCKETS;

	int max_tcp_payload = SFHIP_MTU - sizeof( sfhip_mac_header ) -
	                      sizeof( sfhip_ip_header ) - sizeof( sfhip_tcp_header );

	int socket_number = 0;
	do
	{
		if ( tsl < ++cursor )
		{
			if ( ss->remote_address )
			{
				int retry_number = ss->retry_number;

				// TODO: can these be refactored?

				if ( ss->mode == SFHIP_TCP_MODE_ESTABLISHED )
				{
					// Slow standoff, or waiting for someone to send data.
					if ( !ss->pending_send_size ||
					     ss->pending_send_time > ( ( (uint32_t)retry_number ) + 1 ) << 8 )
					{
						// This is called whenever we are free to send, OR, we
						// have waited a long time for an ACK and yet no ACK is
						// present.
						uint8_t * tcp_payload_buffer =
						    (uint8_t *)( (sfhip_tcp_header *)( ( (sfhip_ip_header *)scratch->payload ) + 1 ) +
						                 1 );

						int retrying = ss->pending_send_size;

						if ( retrying )
						{
							retry_number++;
							ss->retry_number = retry_number;
						}

						if ( retry_number > 15 )
						{
							// Kill off connection.
							sent = SFHIP_TCP_OUTPUT_RESET;
							ss->remote_address = 0;
						}
						else
						{
							sent = sfhip_tcp_event( hip, socket_number, tcp_payload_buffer, 0, max_tcp_payload, 0 );
						}

						if ( sent )
						{
							sfhip_makeandsend_tcp_packet( hip, scratch, sent, ss );
							goto done;
						}
						ss->pending_send_time = 0;
					}
				}
				else
				{
					if ( ss->pending_send_time > (uint32_t)( retry_number + 1 ) << 8 )
					{

						int sent = 0;
						if ( ss->mode == SFHIP_TCP_MODE_CLOSING_WAIT )
							sent = SFHIP_TCP_OUTPUT_FIN;
						else if ( ss->mode == SFHIP_TCP_MODE_SENT_SYN_ACK )
							sent = SFHIP_TCP_OUTPUT_SYNACK;
						else
						{
							SFHIP_WARN(
							    "Invalid state recorded inside of sfhip_tick for TCP (%d)\n",
							    ss->mode );
						}

						if ( retry_number >= 15 )
						{
							// Actually kill off connection.
							ss->remote_address = 0;
						}
						else
						{
							sfhip_makeandsend_tcp_packet( hip, scratch, sent, ss );
						}

						ss->retry_number = retry_number + 1;

						goto done;
					}
				}

				if ( ss->pending_send_size )
					ss->pending_send_time += dt_ms;

				if ( second_tick )
				{
					int msp = ss->ms1024_since_last_rx_packet++;
					if ( msp > 240 )
					{
						sent = 1;
						if ( msp == 255 )
						{
							// Terminate connection (timeout)
							sfhip_makeandsend_tcp_packet( hip, scratch, SFHIP_TCP_OUTPUT_RESET, ss );
							ss->remote_address = 0;
						}
						else
						{
							sfhip_makeandsend_tcp_packet( hip, scratch, SFHIP_TCP_OUTPUT_KEEPALIVE, ss );
						}
						goto done;
					}
				}
			}
		}
		ss++;
		socket_number++;
	} while ( ss != ssend );
	#endif

done:
	hip->ms_elapsed += dt_ms;

	if ( !sent )
		cursor = 0;

	hip->tick_event_last_sent = cursor;

	return sent;
}

// Configuration asserts

HIPSTATIC_ASSERT( ( ( HIP_PHY_HEADER_LENGTH_BYTES ) & 3 ) == 0,
                  "HIP_PHY_HEADER_LENGTH_BYTES must be divisible by 4" );

HIPSTATIC_ASSERT( sizeof( sfhip_phy_packet ) ==
                      sizeof( sfhip_mac_header ) + HIP_PHY_HEADER_LENGTH_BYTES,
                  "phy packet misalignment" );
HIPSTATIC_ASSERT( sizeof( sfhip_mac_header ) == 14, "mac packet size incorrect" );
HIPSTATIC_ASSERT( sizeof( sfhip_arp_header ) == 28, "arp packet size incorrect" );

#endif

#endif

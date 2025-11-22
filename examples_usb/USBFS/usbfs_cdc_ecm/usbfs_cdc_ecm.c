#include "ch32fun.h"
#include <stdio.h>
#include <string.h>

#define SFHIP_DHCP_CLIENT 0
#define SFHIP_IMPLEMENTATION
#include "sfhip.h"

#define BIG 1
#include "data.h"

#include "fsusb.h"

// Logs
#define USBSTATS_ENABLE 0
#define ETHLOG_ENABLE 0
#define HEXDUMP_ENABLE ( ETHLOG_ENABLE > 1 ) || 1

#define SYSTICK_ONE_MILLISECOND ( (uint32_t)FUNCONF_SYSTEM_CORE_CLOCK / 1000 )

/* CDC ECM Class requests Section 6.2 in CDC ECM spec */
#define SET_ETHERNET_MULTICAST_FILTERS 0x40
#define SET_ETHERNET_POWER_MANAGEMENT_PATTERN_FILTER 0x41
#define GET_ETHERNET_POWER_MANAGEMENT_PATTERN_FILTER 0x42
#define SET_ETHERNET_PACKET_FILTER 0x43
#define GET_ETHERNET_STATISTIC 0x44
/* 45h-4Fh RESERVED (future use) */

#define USB_ECM_NOTIFY_ITF 0x00
#define EP_NOTIFY 0x01
#define EP_RECV 0x02
#define EP_SEND 0x03

#define USB_ACK -1
#define USB_NAK 0

#define IP_FMT "%d.%d.%d.%d"
#define IP_FMT_ARGS( addr ) \
	( addr )[0] & 0xff, ( ( addr )[0] >> 8 ) & 0xff, ( addr )[1] & 0xff, ( ( addr )[1] >> 8 ) & 0xff

typedef struct __attribute__( ( packed ) )
{
	uint8_t bmRequestType;
	uint8_t bRequest;
	uint16_t wValue;
	uint16_t wIndex;
	uint16_t wLength;
} usb_request_t;

static __attribute__( ( aligned( 4 ) ) ) usb_request_t notify_nc = {
	.bmRequestType = 0xA1,
	.bRequest = 0 /* NETWORK_CONNECTION */,
	.wValue = 1 /* Connected */,
	.wIndex = USB_ECM_NOTIFY_ITF,
	.wLength = 0,
};

static struct
{
	int in[4];
	int out[4];
} usb_stats = { 0 };
static int debugger = 0;

static sfhip hip = {
	.ip = HIPIP( 192, 168, 14, 1 ),
	.mask = HIPIP( 255, 255, 255, 0 ),
	.gateway = HIPIP( 192, 168, 14, 1 ),
	.self_mac = { { 0xf0, 0x11, 0x22, 0x33, 0x44, 0x55 } },
#if SFHIP_DHCP_CLIENT
	.hostname = "ch32v_ecm",
#endif
};

typedef enum
{
	HTP_START,
	HTP_REQUEST_DONE,
	HTP_REQUEST_DATA,
	HTP_DATA_SENT,
	HTP_ERROR,
	HTP_DONE,
} httpparsestate;

typedef struct
{
	httpparsestate state : 8;
	char *data;
	size_t data_len;
	size_t sent;
} http;

http https[SFHIP_TCP_SOCKETS];

extern volatile uint8_t usb_debug;
static volatile uint32_t SysTick_Ms = 0;
static volatile bool send_nc = false;

static volatile uint8_t buff[sizeof( sfhip_phy_packet_mtu )];
static volatile uint32_t buff_len = 0;
static volatile bool busy = false;
__attribute__( ( aligned( 4 ) ) ) sfhip_phy_packet_mtu packet_buf;

static void ethdev_init( void );
static size_t ethdev_read( void );
static void ethdev_send( const uint8_t *buf, size_t len );

static void systick_init( void );

#if HEXDUMP_ENABLE
static void hexdump( const void *ptr, size_t len );
#endif

int main()
{
	SystemInit();
	RCC->AHBPCENR = RCC_AHBPeriph_SRAM | RCC_AHBPeriph_DMA1;

	systick_init();

	funGpioInitAll();
	debugger = !WaitForDebuggerToAttach( 1000 );

	if ( debugger ) printf( "Starting %dMHz\n", FUNCONF_SYSTEM_CORE_CLOCK / 1000000 );
	usb_debug = 0;

	USBFSSetup();

	ethdev_init();

	if ( debugger ) printf( "Started USB CDC ECM + HTTPD example\n" );


	uint32_t last_ms = SysTick_Ms;

	for ( ;; )
	{

		if ( send_nc )
		{
			(void)USBFS_SendEndpointNEW( EP_NOTIFY, (uint8_t *)&notify_nc, sizeof( notify_nc ), 0 );
			send_nc = false;
		}

		const size_t len = ethdev_read();
		if ( len )
		{
			sfhip_accept_packet( &hip, &packet_buf, len );
		}
		const uint32_t now = SysTick_Ms;
		const uint32_t delta_ms = now - last_ms;
		last_ms = now;
		sfhip_tick( &hip, &packet_buf, (int)delta_ms );
	}
}

int sfhip_send_packet( sfhip *hip, sfhip_phy_packet *data, int length )
{
	ethdev_send( (uint8_t *)data, (size_t)length );
	return 0;
}

int sfhip_tcp_accept_connection( sfhip *hip, int sockno, int localport, hipbe32 remote_host )
{
	// return 0 to accept, -1 to abort.
	if ( localport == 80 )
	{
		http *h = https + sockno;
		h->state = HTP_START;
		h->sent = 0;
		h->data = NULL;
		return 1;
	}
	else
	{
		printf( "Invalid port (%d)\n", localport );
		return 0;
	}
}

sfhip_length_or_tcp_code sfhip_tcp_event(
	sfhip *hip, int sockno, uint8_t *ip_payload, int ip_payload_length, int max_out_payload, int acked )
{
	http *h = https + sockno;

	const httpparsestate s = h->state;

	if ( s == HTP_START && ip_payload_length )
	{
		char *p = (char *)ip_payload;
		char *end = (char *)p + ip_payload_length;

		if ( p[0] != 'G' || p[1] != 'E' || p[2] != 'T' || p[3] != ' ' )
		{
			if ( debugger ) printf( "HTTP parse error: not a GET\n" );
			h->state = HTP_ERROR;
			return SFHIP_TCP_OUTPUT_FIN;
		}

		// Simple HTTP parser for GET *
		for ( p += 4; p < end; ++p )
		{
			if ( *p == ' ' || *p == '\r' || *p == '\n' )
			{
				*p = 0;
				break;
			}
		}

		if ( p == end )
		{
			if ( debugger ) printf( "HTTP parse error: no space after URL\n" );
			h->state = HTP_ERROR;
			return SFHIP_TCP_OUTPUT_FIN;
		}


		// Check URL
		const char *const url = (char *)ip_payload + 4;

		if ( debugger ) printf( "HTTP Request for URL: %s\n", url );

		if ( strcmp( url, "/" ) == 0 )
		{
			// TODO: lookup file
			h->state = HTP_REQUEST_DONE;
			h->data = (char *)index_html;
			h->data_len = sizeof( index_html ) - 1;
			h->sent = 0;
		}
		else
		{
			h->state = HTP_REQUEST_DONE;
			h->data = (char *)e404_html;
			h->data_len = sizeof( e404_html ) - 1;
			h->sent = 0;
		}
	}

	if ( h->state == HTP_ERROR )
	{
		// closed connection, maybe should be 500
		h->state = HTP_DONE;
	}

	// Phase one - if TCP send is confirmed,
	if ( acked && max_out_payload )
	{
		switch ( h->state )
		{
			case HTP_REQUEST_DATA: h->state = HTP_DATA_SENT; break;
			default: break;
		}
	}

	// If we can send our message, send it.
	if ( !max_out_payload ) return 0;

	// Phase two - send a TCP reply.
	switch ( h->state )
	{
		case HTP_DATA_SENT:
			h->sent += max_out_payload;
			// fallthrough
		case HTP_REQUEST_DONE:
			h->state = HTP_REQUEST_DATA;
			// fallthrough
		case HTP_REQUEST_DATA:
			if ( h->sent >= h->data_len )
			{
				h->state = HTP_DONE;
				break;
			}
			size_t len = h->data_len - h->sent;
			if ( len > (size_t)max_out_payload ) len = (size_t)max_out_payload;
			memcpy( ip_payload, &h->data[h->sent], len );
			return len;
		case HTP_DONE: return SFHIP_TCP_OUTPUT_FIN;
		default: return 0;
	}
	return 0;
}

void sfhip_tcp_socket_closed( sfhip *hip, int sockno )
{
	if ( debugger ) printf( "Socket %d Closed\n", sockno );
}

int HandleInRequest( struct _USBState *ctx, int endp, uint8_t *data, int len )
{
	usb_stats.in[endp]++;

	int ret = USB_NAK; // Just NAK, we will send data async
	return ret;
}

void HandleDataOut( struct _USBState *ctx, int endp, uint8_t *data, int len )
{
	usb_stats.out[endp]++;

	if ( endp == 0 )
	{
		ctx->USBFS_SetupReqLen = 0; // To ACK
	}
	if ( endp == EP_RECV )
	{
		if ( busy )
		{
			// still processing previous packet
			USBFS_SendNAK( EP_RECV, 0 );
			return;
		}

		if ( ( buff_len + len ) > sizeof( buff ) )
		{
			// Overflowing buffer
			buff_len = 0;
			USBFS_SendNAK( EP_RECV, 0 );
			return;
		}

		memcpy( (void *)( buff + buff_len ), (void *)data, len );

		buff_len += len;
		if ( len < 64 )
		{
			// printf( "RECV done, total len: %d\n", (int)buff_len );
			busy = true;
		}
		// USBFS_SendACK( EP_RECV, 0 );
		ctx->USBFS_SetupReqLen = 0; // To ACK
	}
}

int HandleSetupCustom( struct _USBState *ctx, int setup_code )
{
	int ret = USB_NAK;
	// if ( debugger ) printf( "HandleSetupCustom - 0x%02x, len = %d\n", setup_code, ctx->USBFS_SetupReqLen );
	if ( ctx->USBFS_SetupReqType & USB_REQ_TYP_CLASS )
	{
		switch ( setup_code )
		{
			case SET_ETHERNET_MULTICAST_FILTERS:
			case SET_ETHERNET_POWER_MANAGEMENT_PATTERN_FILTER:
			case GET_ETHERNET_POWER_MANAGEMENT_PATTERN_FILTER:
			case GET_ETHERNET_STATISTIC:
				// Optional
				ret = USB_ACK;
				break;

			case SET_ETHERNET_PACKET_FILTER:
				// This is the only mandatory request to implement
				send_nc = true;
				notify_nc.wIndex = ctx->USBFS_IndexValue;
				ret = USB_ACK;
				break;
		}
	}
	return ret;
}

static void ethdev_init( void )
{
	if ( debugger ) printf( "ethdev_init\n" );
	// Wait for USB enumeration
	while ( 1 )
	{
		if ( send_nc )
		{
			while ( -1 == USBFS_SendEndpointNEW( EP_NOTIFY, (uint8_t *)&notify_nc, sizeof( notify_nc ), 0 ) )
				;
			send_nc = false;
			break;
		}
	}
	Delay_Ms( 100 );
	if ( debugger ) printf( "ethdev_init done\n" );
}

static size_t ethdev_read( void )
{
	if ( !busy ) return 0;

#if ETHLOG_ENABLE > 2
	if ( debugger ) printf( "ethdev_read: buff_len=%d\n", (int)buff_len );
#endif
	const size_t len = buff_len;
	memcpy( (void *)&packet_buf, (void *)buff, len );
	buff_len = 0;
	busy = false;
	return len;
}

static void ethdev_send( const uint8_t *data, size_t length )
{
#if ETHLOG_ENABLE > 0
	if ( debugger )
	{
		printf( "ethdev_send: len=%d\n", (int)length );
#if HEXDUMP_ENABLE
		hexdump( data, length );
#endif
	}
#endif

	size_t remaining = length;
	while ( remaining )
	{
		const size_t len = ( remaining > 64 ) ? 64 : remaining;
		const size_t sent = length - remaining;
		const uint8_t *chunk = &data[sent];
		remaining -= len;

		// TODO: do I need to copy the last packet
		const bool last = ( remaining == 0 );

		const bool aligned = ( ( (uint32_t)chunk & 0x3 ) == 0 );

		// Wait for endpoint to be free
		while ( -1 == USBFS_SendEndpointNEW( EP_SEND, (uint8_t *)chunk, len, !aligned ) )
			;

		// Handle zero-length packet if length is multiple of endpoint size
		if ( len == 64 && last )
		{
			while ( -1 == USBFS_SendEndpointNEW( EP_SEND, NULL, 0, 0 ) )
				;
		}
	}
}

/*
 * Initialises the SysTick to trigger an IRQ with auto-reload, using HCLK/1 as
 * its clock source
 */
static void systick_init( void )
{
	// Reset any pre-existing configuration
	SysTick->CTLR = 0x0000;

	// Set the SysTick Compare Register to trigger in 1 millisecond
	SysTick->CMP = SysTick->CNT + SYSTICK_ONE_MILLISECOND;

	SysTick_Ms = 0x00000000;

	// Set the SysTick Configuration
	// NOTE: By not setting SYSTICK_CTLR_STRE, we maintain compatibility with
	// busywait delay funtions used by ch32v003_fun.
	SysTick->CTLR |= SYSTICK_CTLR_STE | // Enable Counter
	                 SYSTICK_CTLR_STIE | // Enable Interrupts
	                 SYSTICK_CTLR_STCLK; // Set Clock Source to HCLK/1

	// Enable the SysTick IRQ
	NVIC_EnableIRQ( SysTick_IRQn );
}

/*
 * SysTick ISR - must be lightweight to prevent the CPU from bogging down.
 * Increments Compare Register and systick_millis when triggered (every 1ms)
 * NOTE: the `__attribute__((interrupt))` attribute is very important
 */
void SysTick_Handler( void ) __attribute__( ( interrupt ) );
void SysTick_Handler( void )
{
	// Set the SysTick Compare Register to trigger in 1 millisecond
	SysTick->CMP = SysTick->CNT + SYSTICK_ONE_MILLISECOND;

	// Clear the trigger state for the next IRQ
	SysTick->SR = 0x00000000;

	// Increment the milliseconds count
	SysTick_Ms++;
}

#if HEXDUMP_ENABLE
static inline void hexdump( const void *ptr, size_t len )
{
	const uint8_t *b = (const uint8_t *)ptr;
	for ( size_t i = 0; i < len; i++ )
	{
		if ( ( i & 0x0f ) == 0 ) printf( "\n%04x: ", (unsigned int)i );
		printf( "%02x ", b[i] );
	}
	printf( "\n" );
}
#endif

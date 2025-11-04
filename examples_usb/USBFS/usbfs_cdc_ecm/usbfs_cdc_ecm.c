#include "ch32fun.h"
#include <stdio.h>
#include <string.h>

#include "fsusb.h"

#include "uip.h"
#include "uipopt.h"
#include "uip_arp.h"

#include "dhcpd.h"
#include "dns.h"
#include "httpd.h"

// Logs
#define HEXDUMP_ENABLE 0
#define ARPDEBUG_ENABLE 0
#define IPDEBUG_ENABLE 0
#define USBSTATS_ENABLE 0

#define BUF ( (struct uip_eth_hdr *)&uip_buf[0] )

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

extern volatile uint8_t usb_debug;
static volatile uint32_t SysTick_Ms = 0;
static volatile bool send_nc = false;

static volatile uint8_t buff[UIP_BUFSIZE];
static volatile uint32_t buff_len = 0;
static volatile bool busy = false;

static void ethdev_init( void );
static size_t ethdev_read( void );
static void ethdev_send( void );

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

	if ( debugger ) printf( "Started USB CDC ECM + uIP HTTPD example\n" );

	uip_init();
	httpd_init();

	// different to the IF MAC
	static const struct uip_eth_addr mac = { .addr = { 0x00, 0x22, 0x97, 0x08, 0xA0, 0x69 } };
	uip_setethaddr( mac );

	u16_t ipaddr[2];
	uip_ipaddr( ipaddr, 255, 255, 255, 0 );
	uip_setnetmask( ipaddr );

#if DHCPD_ENABLE
	dhcpd_init();
#else
	dns_init();
	uip_ipaddr( ipaddr, 192, 168, 8, 111 );
	uip_sethostaddr( ipaddr );
	uip_ipaddr( ipaddr, 192, 168, 8, 1 );
	uip_setdraddr( ipaddr );
#endif

#if DNS_ENABLE
	dns_init();
#endif

	uint32_t last_ms = SysTick_Ms;
	int arptimer = 0;

	for ( ;; )
	{

		if ( send_nc )
		{
			(void)USBFS_SendEndpointNEW( EP_NOTIFY, (uint8_t *)&notify_nc, sizeof( notify_nc ), 0 );
			send_nc = false;
		}

		/* Let the ethdev network device driver read an entire IP packet
	  into the uip_buf. If it must wait for more than 0.5 seconds, it
	  will return with the return value 0. If so, we know that it is
	  time to call upon the uip_periodic(). Otherwise, the ethdev has
	  received an IP packet that is to be processed by uIP. */
		uip_len = ethdev_read();
		if ( uip_len > 0 )
		{
			if ( BUF->type == htons( UIP_ETHTYPE_IP ) )
			{
#if IPDEBUG_ENABLE
				uip_tcpip_hdr *hdr = (uip_tcpip_hdr *)&uip_buf[UIP_LLH_LEN];
				if ( debugger )
					printf( "IP (%d): " IP_FMT " -> " IP_FMT "\n", uip_len, IP_FMT_ARGS( hdr->srcipaddr ),
						IP_FMT_ARGS( hdr->destipaddr ) );
#endif

				uip_arp_ipin();
				uip_input();
				/* If the above function invocation resulted in data that
				   should be sent out on the network, the global variable
				   uip_len is set to a value > 0. */
				if ( uip_len > 0 )
				{
					uip_arp_out();
					ethdev_send();
				}
			}
			else if ( BUF->type == htons( UIP_ETHTYPE_ARP ) )
			{
#if ARPDEBUG_ENABLE
				if ( debugger )
					printf( "ARP (%d): %02x:%02x:%02x:%02x:%02x:%02x -> %02x:%02x:%02x:%02x:%02x:%02x\n", uip_len,
						BUF->src.addr[0], BUF->src.addr[1], BUF->src.addr[2], BUF->src.addr[3], BUF->src.addr[4],
						BUF->src.addr[5], BUF->dest.addr[0], BUF->dest.addr[1], BUF->dest.addr[2], BUF->dest.addr[3],
						BUF->dest.addr[4], BUF->dest.addr[5] );
#endif
				uip_arp_arpin();
				/* If the above function invocation resulted in data that
				   should be sent out on the network, the global variable
				   uip_len is set to a value > 0. */
				if ( uip_len > 0 )
				{
					ethdev_send();
				}
			}
		}

		if ( ( SysTick_Ms - last_ms ) >= 500 )
		{
			last_ms = SysTick_Ms;
			for ( int i = 0; i < UIP_CONNS; i++ )
			{
				uip_periodic( i );
				/* If the above function invocation resulted in data that
				   should be sent out on the network, the global variable
				   uip_len is set to a value > 0. */
				if ( uip_len > 0 )
				{
					uip_arp_out();
					ethdev_send();
				}
			}

#if UIP_UDP
			for ( int i = 0; i < UIP_UDP_CONNS; i++ )
			{
				uip_udp_periodic( i );
				/* If the above function invocation resulted in data that
				   should be sent out on the network, the global variable
				   uip_len is set to a value > 0. */
				if ( uip_len > 0 )
				{
					uip_arp_out();
					ethdev_send();
				}
			}
#endif /* UIP_UDP */

#if USBSTATS_ENABLE
			if ( ( arptimer & 0b11 ) == 2 )
			{
				if ( debugger )
					printf( "%ld:\tUSB Stats: EP0 %d/%d EP1 %d/%d EP2 %d/%d EP3 %d/%d\n", SysTick_Ms, usb_stats.in[0],
						usb_stats.out[0], usb_stats.in[1], usb_stats.out[1], usb_stats.in[2], usb_stats.out[2],
						usb_stats.in[3], usb_stats.out[3] );
			}
#endif

			/* Call the ARP timer function every 10 seconds. */
			if ( ++arptimer == 20 )
			{
				uip_arp_timer();
				arptimer = 0;
			}
		}
	}
}

#define API_HEADER "HTTP/1.0 200 OK\r\nServer: uIP/0.9\r\nContent-type: application/json\r\n\r\n"

int uip_api_handler( const char *endpoint, char **data, int *len )
{
	static char responseBuffer[256] = API_HEADER;

	static char *const payloadStart = responseBuffer + sizeof( API_HEADER ) - 1;
	static const size_t payloadCapacity = sizeof( responseBuffer ) - sizeof( API_HEADER );

	if ( strcmp( endpoint, "status" ) == 0 )
	{
		const int ret = snprintf( payloadStart, payloadCapacity, "{\"uptime_ms\": %lu}\r\n", SysTick_Ms );
		*data = responseBuffer;
		*len = ( ret > 0 ) ? ret + sizeof( API_HEADER ) - 1 : 0;
		return 1;
	}
	return 0;
}

void udp_appcall( void )
{
#if DHCPD_ENABLE
	dhcpd_udp_appcall();
#endif
#if DNS_ENABLE
	dns_udp_appcall();
#endif
}

void uip_log( char *msg )
{
	if ( debugger ) printf( "uIP: %s\n", msg );
}

int HandleInRequest( struct _USBState *ctx, int endp, uint8_t *data, int len )
{
	usb_stats.in[endp]++;

	int ret = USB_NAK; // Just NAK
	switch ( endp )
	{
		case EP_NOTIFY:
			// ret = USB_ACK; // Just ACK
			break;
		case EP_SEND:
			// ret = USB_ACK; // ACK, without it RX was stuck in some cases, leaving for now as a reminder
			break;
	}
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
		// TODO: NAK doesn't work????
		if ( busy )
		{
			// still processing previous packet
			// printf( "RECV busy, dropping packet %d\n", len );
			// USBFS_SendNAK( EP_RECV, 0 );
			return;
		}

		if ( ( buff_len + len ) > sizeof( buff ) )
		{
			buff_len = 0;
			// USBFS_SendNAK( EP_RECV, 0 );
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

	if ( debugger && 0 ) printf( "ethdev_read: buff_len=%d\n", (int)buff_len );
	const size_t len = buff_len;
	memcpy( (void *)uip_buf, (void *)buff, len );
	buff_len = 0;
	busy = false;
	return len;
}

static void ethdev_send( void )
{
	const size_t offset = 40 + UIP_LLH_LEN;

	// NOTE: uIP 0.9 doesn't place appdata in contiguous buffer to avoid copy on slip devices
	// However, for USB CDC ECM, we need a contiguous buffer
	if ( ( uip_len > offset ) && ( uip_appdata != &uip_buf[offset] ) )
	{
		// Need to copy appdata into contiguous buffer
		memcpy( &uip_buf[offset], (void *)uip_appdata, uip_len - offset );
	}

	if ( debugger && 0 )
	{
		printf( "ethdev_send: uip_len=%d\n", (int)uip_len );
#if HEXDUMP_ENABLE
		hexdump( (const void *)uip_buf, uip_len );
#endif
	}

	size_t remaining = uip_len;
	while ( remaining )
	{
		const size_t len = ( remaining > 64 ) ? 64 : remaining;
		uint8_t *buf = &uip_buf[uip_len - remaining];
		remaining -= len;

		// TODO: do I need to copy the last packet
		const bool last = ( remaining == 0 );

		// Wait for endpoint to be free
		while ( -1 == USBFS_SendEndpointNEW( EP_SEND, buf, len, 1 ) )
			;

		// Handle zero-length packet if uip_len is multiple of endpoint size
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
	NVIC_EnableIRQ( SysTicK_IRQn );
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

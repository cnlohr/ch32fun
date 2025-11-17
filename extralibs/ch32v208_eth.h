/*
 * Single-File-Header for the CH32V208 (and CH579?) built-in 10BASE-T MAC/PHY.
 *
 * This peripheral uses POINTER-BASED DMA, not descriptor-based DMA:
 *
 * Hardware:
 *   - ERXST register: Points to buffer for next RX packet
 *   - ETXST register: Points to buffer for current TX packet
 *   - Hardware DMAs directly to/from these addresses
 *   - Software must manually update pointers after each packet
 *
 * Software (this driver):
 *   - Creates descriptor ring for bookkeeping
 *   - Descriptors are NEVER accessed by hardware
 *   - OWN bit is pure software: 1=empty, 0=filled
 *   - Provides ring-buffer API on top of simple pointers
 *
 * USAGE
 *
 * Include once with implementation:
 *
 *   #define CH32V208_ETH_IMPLEMENTATION
 *   #include "ch32v208_eth.h"
 *
 * Init:
 *
 *   eth_config_t config = {
 *       .mac_addr = my_mac,          // NULL = use chip default
 *       .rx_callback = eth_rx_cb,    // NULL if using manual polling
 *       .link_callback = link_cb,    // optional
 *       .activity_callback = led_cb, // optional
 *       .broadcast_filter = true,
 *   };
 *   eth_init(&config);
 *
 * Main loop:
 *
 *   while (1) {
 *       eth_poll_link();  // call every 50-100ms
 *       eth_process_rx(); // IF using callback mode
 *   }
 *
 * RECEIVING PACKETS
 *
 * Callback mode:
 *
 *   void eth_rx_cb(const uint8_t *packet, uint16_t length) {
 *       // process packet (called from eth_process_rx)
 *   }
 *
 * Manual polling (zero-copy, for sfhip, lwIP etc):
 *
 *   uint16_t length;
 *   const uint8_t *packet;
 *   while ((packet = eth_get_rx_packet(&length)) != NULL) {
 *       // process packet directly from DMA buffer
 *       eth_release_rx_packet();  // must call when done
 *   }
 *
 * SENDING PACKETS
 *
 * Simple (with memcpy):
 *
 *   uint8_t packet[64];
 *   // ... fill packet ...
 *   int ret = eth_send_packet(packet, 64);
 *   // ret: 0=success, -1=queue full
 *
 * Zero-copy mode (build directly in DMA buffer):
 *
 *   uint16_t max_len;
 *   uint8_t *buf = eth_get_tx_buffer(&max_len);
 *   if (buf) {
 *       // build packet directly in DMA buffer
 *       eth_send_packet_zerocopy(actual_length);
 *   }
 *
 * CONFIGURATION
 *
 * Define before including header to customize:
 *
 *   ETH_RX_BUF_COUNT        RX descriptor ring size (default: 4)
 *   ETH_TX_BUF_COUNT        TX queue depth (default: 2)
 *   ETH_MAX_PACKET_SIZE     MAC MTU (default: 1536)
 *   ETH_RX_BUF_SIZE         RX buffer size (default: ETH_MAX_PACKET_SIZE)
 *   ETH_TX_BUF_SIZE         TX buffer size (default: ETH_MAX_PACKET_SIZE)
 *   ETH_ENABLE_STATS        Enable eth_get_stats() and eth_reset_stats()
 */

#ifndef _CH32V208_ETH_H
#define _CH32V208_ETH_H

#include <stdbool.h>
#include <stdint.h>

#ifndef ETH_RX_BUF_COUNT
#define ETH_RX_BUF_COUNT 4
#endif

#ifndef ETH_TX_BUF_COUNT
#define ETH_TX_BUF_COUNT 2
#endif

#ifndef ETH_MAX_PACKET_SIZE
#define ETH_MAX_PACKET_SIZE 1536
#endif

#ifndef ETH_RX_BUF_SIZE
#define ETH_RX_BUF_SIZE ETH_MAX_PACKET_SIZE
#endif

#ifndef ETH_TX_BUF_SIZE
#define ETH_TX_BUF_SIZE ETH_MAX_PACKET_SIZE
#endif

#define ETH_MAC_ADDR_LEN 6

// Define ETH_ENABLE_STATS before including this header to enable stats collection
// #define ETH_ENABLE_STATS

// Callback types
typedef void ( *eth_rx_callback_t )( const uint8_t *packet, uint16_t length );
typedef void ( *eth_link_callback_t )( bool link_up );
typedef void ( *eth_activity_callback_t )( void );

// Ethernet configuration
typedef struct
{
	uint8_t *mac_addr; // MAC address (can be NULL to use chip default)
	eth_rx_callback_t rx_callback; // Called when packet received
	eth_link_callback_t link_callback; // Called when link status changes
	eth_activity_callback_t activity_callback; // Called on TX/RX activity, can be used for LED
	bool promiscuous_mode; // Enable promiscuous mode
	bool broadcast_filter; // Accept broadcast packets
	bool multicast_filter; // Accept multicast packets
} eth_config_t;

#ifdef ETH_ENABLE_STATS
// Ethernet statistics
typedef struct
{
	uint32_t rx_packets;
	uint32_t tx_packets;
	uint32_t rx_errors;
	uint32_t tx_errors;
	uint32_t rx_dropped;
	uint32_t tx_dropped;
} eth_stats_t;
#endif

#ifdef __cplusplus
extern "C"
{
#endif

	/**
	 * Init the Ethernet peripheral
	 * @param config Ptr to config struct
	 * @return 0 on success, negative on error
	 */
	int eth_init( const eth_config_t *config );

	/**
	 * Send an Ethernet packet
	 * @param packet Ptr to packet data (incl. Ethernet header)
	 * @param length Length of pkt in bytes
	 * @return 0: success, -1: queue full
	 */
	int eth_send_packet( const uint8_t *packet, uint16_t length );

	/**
	 * Get pointer to next available TX buffer for zero-copy transmission
	 * @param length max length available in buffer
	 * @return pointer to TX buffer, or NULL if queue full
	 * @note After writing packet data, call eth_send_packet_zerocopy() with actual length
	 */
	uint8_t *eth_get_tx_buffer( uint16_t *max_length );

	/**
	 * Commit a zero-copy TX buffer for transmission
	 * @param length packet length written to buffer from eth_get_tx_buffer()
	 * @return 0 on success
	 */
	int eth_send_packet_zerocopy( uint16_t length );

	/**
	 * Process received packets (call from main loop)
	 * This will invoke the rx_callback for each received pkt
	 */
	void eth_process_rx( void );

	/**
	 * Get pointer to next received packet (alternative to eth_process_rx with callback)
	 * @param length Pointer to store packet length
	 * @return Pointer to packet data, or NULL if no packet ready
	 * @note Caller MUST call eth_release_rx_packet() when done with the packet
	 * @note Packet data is valid only until eth_release_rx_packet() is called
	 */
	const uint8_t *eth_get_rx_packet( uint16_t *length );

	/**
	 * Release currently held RX packet back to DMA
	 * @note Must be called after eth_get_rx_packet() to free the descriptor
	 */
	void eth_release_rx_packet( void );

	/**
	 * Poll link status and handle auto-negotiation
	 * this is based on WCHNET_LinkProcess
	 * https://github.com/openwch/ch32v20x/blob/main/EVT/EXAM/ETH/NetLib/eth_driver.c#L131
	 * TODO: cable polarity reversal is untested
	 *
	 * call this periodically from main loop (i.e. every 50-100ms)
	 */
	void eth_poll_link( void );

	/**
	 * Get current MAC
	 * @param mac_addr Buffer to store MAC address (6 bytes)
	 */
	void eth_get_mac_address( uint8_t *mac_addr );

	/**
	 * Check if link is up
	 * @return true if link is up
	 */
	bool eth_is_link_up( void );

#ifdef ETH_ENABLE_STATS
	/**
	 * Get stats
	 * @param stats Pointer to statistics structure to fill
	 */
	void eth_get_stats( eth_stats_t *stats );

	/**
	 * Reset stat counters
	 */
	void eth_reset_stats( void );
#endif

#ifdef __cplusplus
}
#endif

#endif // _CH32V208_ETH_H


#ifdef CH32V208_ETH_IMPLEMENTATION

#include "ch32fun.h"
#include <string.h>

// DMA descriptor
typedef struct
{
	volatile uint32_t Status;
	volatile uint32_t Buffer1Addr;
} ETH_DMADESCTypeDef;

// TX queue management
typedef struct
{
	volatile uint32_t head;
	volatile uint32_t tail;
	volatile bool is_full; // this is only really needed if ETH_TX_BUF_COUNT == 1
} tx_queue_t;

// driver state
typedef struct
{
	uint32_t rx_head_idx;
	uint32_t rx_tail_idx;
	tx_queue_t tx_q;
	uint8_t mac_addr[ETH_MAC_ADDR_LEN];
	eth_rx_callback_t rx_callback;
	eth_link_callback_t link_callback;
	eth_activity_callback_t activity_callback;
	volatile bool link_irq_flag;
#ifdef ETH_ENABLE_STATS
	eth_stats_t stats;
#endif
	// autoneg & polarity state
	uint8_t phy_mdix_mode; // current MDI/MDIX setting
	uint8_t crc_error_count; // CRC errors since link up
	bool polarity_detect_active;
	uint8_t negotiation_poll_count;
	enum
	{
		LINK_STATE_DOWN,
		LINK_STATE_NEGOTIATING,
		LINK_STATE_UP
	} link_state;
} eth_driver_state_t;

__attribute__( ( aligned( 4 ) ) ) static ETH_DMADESCTypeDef g_dma_rx_descs[ETH_RX_BUF_COUNT];
__attribute__( ( aligned( 4 ) ) ) static ETH_DMADESCTypeDef g_dma_tx_descs[ETH_TX_BUF_COUNT];
__attribute__( ( aligned( 4 ) ) ) static uint8_t g_mac_rx_bufs[ETH_RX_BUF_COUNT * ETH_RX_BUF_SIZE];
__attribute__( ( aligned( 4 ) ) ) static uint8_t g_mac_tx_bufs[ETH_TX_BUF_COUNT * ETH_TX_BUF_SIZE];

static eth_driver_state_t g_eth_state = { 0 };

static void phy_write_reg( uint8_t reg_add, uint16_t reg_val )
{
	R32_ETH_MIWR = ( reg_add & RB_ETH_MIREGADR_MASK ) | RB_ETH_MIWR_MIIWR | ( reg_val << RB_ETH_MIWR_DATA_SHIFT );
}

static uint16_t phy_read_reg( uint8_t reg_add )
{
	ETH10M->MIERGADR = reg_add;
	return ETH10M->MIRD;
}

static inline void tx_queue_init( tx_queue_t *q )
{
	q->head = 0;
	q->tail = 0;
	q->is_full = false;
}

static inline bool tx_queue_is_empty( const tx_queue_t *q )
{
	return !q->is_full && ( q->head == q->tail );
}

static inline bool tx_queue_is_full( const tx_queue_t *q )
{
	return q->is_full;
}

static inline void tx_queue_produce( tx_queue_t *q )
{
	q->head = ( q->head + 1 ) % ETH_TX_BUF_COUNT;
	if ( q->head == q->tail )
	{
		q->is_full = true; // head caught up to tail = full
	}
}

static inline void tx_queue_consume( tx_queue_t *q )
{
	q->tail = ( q->tail + 1 ) % ETH_TX_BUF_COUNT;
	q->is_full = false; // after consuming, definitely not full
}

static void eth_get_chip_mac_addr( uint8_t *mac )
{
	const uint8_t *macaddr_src = (const uint8_t *)( ROM_CFG_USERADR_ID + 5 );
	for ( int i = 0; i < 6; i++ )
	{
		mac[i] = *( macaddr_src-- );
	}
}

static void tx_start_if_possible( void )
{
	if ( ETH10M->ECON1 & RB_ETH_ECON1_TXRTS )
	{
		return;
	}

	if ( tx_queue_is_empty( &g_eth_state.tx_q ) )
	{
		return;
	}

	uint32_t idx = g_eth_state.tx_q.tail;
	ETH_DMADESCTypeDef *desc = &g_dma_tx_descs[idx];
	uint16_t len = desc->Status;

	ETH10M->ETXLN = len; // set tx packet len
	ETH10M->ETXST = desc->Buffer1Addr; // set tx buf start address (DMA source)
	ETH10M->ECON1 |= RB_ETH_ECON1_TXRTS; // set tx req flag to start DMA transmission
}

/**
 * Start PHY auto-negotiation with specific MDI/MDIX mode
 */
static void phy_start_autoneg( uint8_t mdix_mode )
{
	// reset and restart auto-negotiation
	phy_write_reg( PHY_BMCR, PHY_BMCR_RESET );
	Delay_Us( 100 );
	// configure MDI/MDIX mode
	phy_write_reg( PHY_MDIX, ( mdix_mode & MDIX_MODE_MASK ) | MDIX_PN_POLARITY_NORMAL );
	// enable auto-negotiation
	phy_write_reg( PHY_BMCR, PHY_BMCR_AN_ENABLE | PHY_BMCR_AN_RESTART );

	g_eth_state.phy_mdix_mode = mdix_mode;
}

/**
 * Try next MDI/MDIX mode in sequence: AUTO -> MDIX -> MDI -> AUTO
 */
static void phy_try_next_mdix_mode( void )
{
	uint8_t next_mode;

	switch ( g_eth_state.phy_mdix_mode & MDIX_MODE_MASK )
	{
		case MDIX_MODE_AUTO:
			next_mode = MDIX_MODE_MDIX; // try forced MDIX
			break;
		case MDIX_MODE_MDIX:
			next_mode = MDIX_MODE_MDI; // try forced MDI
			break;
		default:
			next_mode = MDIX_MODE_AUTO; // back to auto
			break;
	}

	phy_start_autoneg( next_mode );
}

/**
 * Handle cable polarity issue (P/N reversal)
 */
static void phy_fix_polarity( void )
{
	uint16_t mdix_reg = phy_read_reg( PHY_MDIX );

	// toggle P/N polarity between NORMAL and REVERSED
	mdix_reg ^= MDIX_PN_POLARITY_REVERSED;

	phy_write_reg( PHY_MDIX, mdix_reg );
	g_eth_state.crc_error_count = 0;
}

static void eth_link_up_handler( void )
{
	// read auto-negotiation registers
	uint16_t anar = phy_read_reg( PHY_ANAR ); // what we advertised
	uint16_t anlpar = phy_read_reg( PHY_ANLPAR ); // what partner advertised
	uint16_t common = anar & anlpar;

	// check if both sides support full-duplex
	bool is_full_duplex = ( common & PHY_ANLPAR_10BASE_TFD ) != 0;
	// configure MAC to match negotiated duplex mode
	if ( is_full_duplex )
	{
		ETH10M->MACON2 |= RB_ETH_MACON2_FULDPX;
	}
	else
	{
		ETH10M->MACON2 &= ~RB_ETH_MACON2_FULDPX;
	}
	// enable CRC error packet reception for polarity detection
	ETH10M->ERXFCON |= RB_ETH_ERXFCON_CRCEN;

	g_eth_state.link_state = LINK_STATE_UP;
	g_eth_state.crc_error_count = 0;
	g_eth_state.polarity_detect_active = true;
	g_eth_state.negotiation_poll_count = 0;

	if ( g_eth_state.link_callback )
	{
		g_eth_state.link_callback( true );
	}
}

/**
 * Configure MAC for link down
 */
static void eth_link_down_handler( void )
{
	// Disable polarity detection
	g_eth_state.polarity_detect_active = false;
	ETH10M->ERXFCON &= ~RB_ETH_ERXFCON_CRCEN;

	g_eth_state.link_state = LINK_STATE_DOWN;

	if ( g_eth_state.link_callback )
	{
		g_eth_state.link_callback( false );
	}
}

int eth_init( const eth_config_t *config )
{
	if ( !config )
	{
		return -1;
	}

	memset( &g_eth_state, 0, sizeof( g_eth_state ) );

	g_eth_state.rx_callback = config->rx_callback;
	g_eth_state.link_callback = config->link_callback;
	g_eth_state.activity_callback = config->activity_callback;

	if ( config->mac_addr )
	{
		memcpy( g_eth_state.mac_addr, config->mac_addr, ETH_MAC_ADDR_LEN );
	}
	else
	{
		eth_get_chip_mac_addr( g_eth_state.mac_addr );
	}

	RCC->APB2PCENR |= RCC_APB2Periph_AFIO;
	RCC->CFGR0 |= RCC_ETHPRE; // Ethernet clock prescaler
	EXTEN->EXTEN_CTR |= EXTEN_ETH_10M_EN; // Extended Control Register, 10M Ethernet enable and clock enable

	// Transmit/Receive module reset
	ETH10M->ECON1 = RB_ETH_ECON1_TXRST | RB_ETH_ECON1_RXRST;
	ETH10M->ECON1 = 0;

	uint32_t rx_filter;

	if ( config->promiscuous_mode )
	{
		// Promiscuous: disable filtering (ERXFCON=0 *probably* receives everything)
		rx_filter = 0;
	}
	else
	{
		// Normal mode: enable filtering and ALWAYS receive unicast to our MAC
		rx_filter = RB_ETH_ERXFCON_EN | RB_ETH_ERXFCON_UCEN;

		// broadcast packet reception
		if ( config->broadcast_filter )
		{
			rx_filter |= RB_ETH_ERXFCON_BCEN;
		}
		// multicast packet reception
		if ( config->multicast_filter )
		{
			rx_filter |= RB_ETH_ERXFCON_MCEN;
		}
	}

	ETH10M->ERXFCON = rx_filter;
	// MAC layer receive enable
	ETH10M->MACON1 = RB_ETH_MACON1_MARXEN;

	// Pad short packets with 0x00 to 60 bytes, then append 4-byte CRC
	// Hardware pads CRC
	ETH10M->MACON2 = PADCFG_AUTO_3 | RB_ETH_MACON2_TXCRCEN;
	// Set maximum frame length (MTU)
	ETH10M->MAMXFL = ETH_MAX_PACKET_SIZE;

	// MAC is reversed
	R8_ETH_MAADRL1 = g_eth_state.mac_addr[5];
	R8_ETH_MAADRL2 = g_eth_state.mac_addr[4];
	R8_ETH_MAADRL3 = g_eth_state.mac_addr[3];
	R8_ETH_MAADRL4 = g_eth_state.mac_addr[2];
	R8_ETH_MAADRL5 = g_eth_state.mac_addr[1];
	R8_ETH_MAADRL6 = g_eth_state.mac_addr[0];

	// PHY Analog Parameter Setting, default value and "Rated driver"
	ETH10M->ECON2 = RB_ETH_ECON2_DEFAULT;

	tx_queue_init( &g_eth_state.tx_q );
	for ( int i = 0; i < ETH_TX_BUF_COUNT; i++ )
	{
		g_dma_tx_descs[i].Status = 0;
		g_dma_tx_descs[i].Buffer1Addr = (uint32_t)&g_mac_tx_bufs[i * ETH_TX_BUF_SIZE];
	}

	// init RX descriptor ring (DMA reads from these)
	g_eth_state.rx_head_idx = 0;
	g_eth_state.rx_tail_idx = 0;
	for ( int i = 0; i < ETH_RX_BUF_COUNT; i++ )
	{
		g_dma_rx_descs[i].Status = ETH_DMARxDesc_OWN; // DMA owns all initially
		g_dma_rx_descs[i].Buffer1Addr = (uint32_t)&g_mac_rx_bufs[i * ETH_RX_BUF_SIZE];
	}

	// start RX
	ETH10M->ERXST = g_dma_rx_descs[0].Buffer1Addr;
	ETH10M->ECON1 = RB_ETH_ECON1_RXEN;

	g_eth_state.link_state = LINK_STATE_DOWN;
	g_eth_state.negotiation_poll_count = 0;
	g_eth_state.polarity_detect_active = false;
	g_eth_state.crc_error_count = 0;

	// start auto-negotiation with AUTO MDI/MDIX
	phy_start_autoneg( MDIX_MODE_AUTO );
	Delay_Ms( 100 ); // Give PHY time to initialize

	// clear all pending interrupt flags
	ETH10M->EIR = 0xFF;
	ETH10M->ESTAT |= RB_ETH_ESTAT_INT | RB_ETH_ESTAT_BUFER;

	ETH10M->EIE = RB_ETH_EIE_INTIE | // Ethernet interrupt enable (master enable)
	              RB_ETH_EIE_RXIE | // RX complete interrupt
	              RB_ETH_EIE_TXIE | // TX complete interrupt
	              RB_ETH_EIE_LINKIE | // Link status change interrupt
	              RB_ETH_EIE_TXERIE | // TX error interrupt (collision, underrun, etc.)
	              RB_ETH_EIE_RXERIE | // RX error interrupt (CRC error, overrun, etc.)
	              RB_ETH_EIE_R_EN50; // Built-in 50ohm impedance matching resistor enable

	NVIC_EnableIRQ( ETH_IRQn );

	return 0;
}

int eth_send_packet( const uint8_t *packet, uint16_t length )
{
	if ( tx_queue_is_full( &g_eth_state.tx_q ) )
	{
#ifdef ETH_ENABLE_STATS
		g_eth_state.stats.tx_dropped++;
#endif
		return -1;
	}

	// reserve our slot in the queue
	uint32_t idx = g_eth_state.tx_q.head;
	tx_queue_produce( &g_eth_state.tx_q );

	uint8_t *tx_buf = (uint8_t *)g_dma_tx_descs[idx].Buffer1Addr;
	memcpy( tx_buf, packet, length );
	g_dma_tx_descs[idx].Status = length;

	tx_start_if_possible();
	return 0;
}

uint8_t *eth_get_tx_buffer( uint16_t *max_length )
{
	if ( tx_queue_is_full( &g_eth_state.tx_q ) )
	{
		return NULL;
	}

	if ( max_length )
	{
		*max_length = ETH_TX_BUF_SIZE;
	}

	uint32_t idx = g_eth_state.tx_q.head;
	return (uint8_t *)g_dma_tx_descs[idx].Buffer1Addr;
}


int eth_send_packet_zerocopy( uint16_t length )
{
	uint32_t idx = g_eth_state.tx_q.head;
	g_dma_tx_descs[idx].Status = length;

	tx_queue_produce( &g_eth_state.tx_q );
	tx_start_if_possible();

	return 0;
}

const uint8_t *eth_get_rx_packet( uint16_t *length )
{
	if ( !length )
	{
		return NULL;
	}

	uint32_t tail_idx = g_eth_state.rx_tail_idx;

	if ( g_dma_rx_descs[tail_idx].Status & ETH_DMARxDesc_OWN )
	{
		return NULL; // no packet ready
	}

	// extract packet length from descriptor status field
	*length = ( g_dma_rx_descs[tail_idx].Status & ETH_DMARxDesc_FL ) >> ETH_DMARxDesc_FrameLengthShift;

	// return pointer to packet buffer
	return (const uint8_t *)g_dma_rx_descs[tail_idx].Buffer1Addr;
}

void eth_release_rx_packet( void )
{
#ifdef ETH_ENABLE_STATS
	g_eth_state.stats.rx_packets++;
#endif
	uint32_t tail_idx = g_eth_state.rx_tail_idx;

	// give descriptor back to DMA
	g_dma_rx_descs[tail_idx].Status = ETH_DMARxDesc_OWN;

	// advance to next descriptor in ring
	g_eth_state.rx_tail_idx = ( tail_idx + 1 ) % ETH_RX_BUF_COUNT;
}


void eth_process_rx( void )
{
	uint16_t length;
	const uint8_t *packet;

	// process all packets that DMA has released
	while ( ( packet = eth_get_rx_packet( &length ) ) != NULL )
	{
		// deliver to user callback if registered
		if ( g_eth_state.rx_callback )
		{
			g_eth_state.rx_callback( packet, length );
		}

		eth_release_rx_packet();
	}
}

void eth_poll_link( void )
{
	if ( g_eth_state.link_irq_flag )
	{
		g_eth_state.link_irq_flag = false;
		g_eth_state.negotiation_poll_count = 0;
	}

	uint16_t bmsr = phy_read_reg( PHY_BMSR );
	uint16_t anlpar = phy_read_reg( PHY_ANLPAR );

	bool phy_link = ( bmsr & PHY_BMSR_LINK_STATUS ) != 0;
	bool an_complete = ( bmsr & PHY_BMSR_AN_COMPLETE ) != 0;

	switch ( g_eth_state.link_state )
	{

		case LINK_STATE_DOWN:
			if ( phy_link && an_complete && ( anlpar != 0 ) )
			{
				// valid link with successful negotiation
				eth_link_up_handler();
			}
			else if ( phy_link && an_complete && ( anlpar == 0 ) )
			{
				// false auto-negotiation completion (ANLPAR=0)
				// reset PHY and try different mode
				g_eth_state.link_state = LINK_STATE_NEGOTIATING;
				g_eth_state.negotiation_poll_count = 0;
				phy_write_reg( PHY_BMCR, PHY_BMCR_RESET );
				Delay_Us( 100 );
				phy_try_next_mdix_mode();
			}
			break;

		case LINK_STATE_NEGOTIATING:
			if ( phy_link && an_complete && ( anlpar != 0 ) )
			{
				// negotiation succeeded
				eth_link_up_handler();
			}
			else if ( phy_link && an_complete && ( anlpar == 0 ) )
			{
				// still no valid partner response after negotiation
				g_eth_state.negotiation_poll_count++;

				if ( g_eth_state.negotiation_poll_count >= 10 )
				{
					// try next MDI/MDIX mode after 10 polls
					g_eth_state.negotiation_poll_count = 0;
					phy_write_reg( PHY_BMCR, PHY_BMCR_RESET );
					Delay_Us( 100 );
					phy_try_next_mdix_mode();
				}
			}
			else if ( !phy_link )
			{
				// link went down during negotiation
				g_eth_state.link_state = LINK_STATE_DOWN;
				g_eth_state.negotiation_poll_count = 0;
			}
			break;

		case LINK_STATE_UP:
			if ( !phy_link )
			{
				// link went down
				eth_link_down_handler();
				phy_start_autoneg( MDIX_MODE_AUTO );
			}
			else if ( g_eth_state.polarity_detect_active )
			{
				// monitor for polarity issues
				if ( g_eth_state.crc_error_count >= 3 )
				{
					phy_fix_polarity();
					g_eth_state.polarity_detect_active = false;
					ETH10M->ERXFCON &= ~RB_ETH_ERXFCON_CRCEN;
				}
			}
			break;
	}
}

void eth_get_mac_address( uint8_t *mac_addr )
{
	if ( mac_addr )
	{
		memcpy( mac_addr, g_eth_state.mac_addr, ETH_MAC_ADDR_LEN );
	}
}

bool eth_is_link_up( void )
{
	return g_eth_state.link_state == LINK_STATE_UP;
}

#ifdef ETH_ENABLE_STATS
void eth_get_stats( eth_stats_t *stats )
{
	if ( stats )
	{
		*stats = g_eth_state.stats;
	}
}

void eth_reset_stats( void )
{
	memset( &g_eth_state.stats, 0, sizeof( eth_stats_t ) );
}
#endif

void ETH_IRQHandler( void ) __attribute__( ( interrupt ) ) __attribute__( ( used ) );
void ETH_IRQHandler( void )
{
	uint32_t flags = ETH10M->EIR;

	uint32_t head_idx = g_eth_state.rx_head_idx;

	if ( flags & RB_ETH_EIR_RXIF )
	{
		ETH10M->EIR = RB_ETH_EIR_RXIF; // clear interrupt flag

		// check if DMA still owns the current head descriptor
		if ( g_dma_rx_descs[head_idx].Status & ETH_DMARxDesc_OWN )
		{
			uint16_t rx_len = ETH10M->ERXLN;

			if ( rx_len == 0 || rx_len > ETH_RX_BUF_SIZE )
			{
#ifdef ETH_ENABLE_STATS
				g_eth_state.stats.rx_errors++;
#endif
				return;
			}

			// check for RX errors
			uint8_t estat = ETH10M->ESTAT;
			const uint8_t error_mask =
				RB_ETH_ESTAT_BUFER | RB_ETH_ESTAT_RXCRCER | RB_ETH_ESTAT_RXNIBBLE | RB_ETH_ESTAT_RXMORE;

			if ( estat & error_mask )
			{
				// track CRC errors specifically for polarity detection
				if ( ( estat & RB_ETH_ESTAT_RXCRCER ) && g_eth_state.polarity_detect_active )
				{
					g_eth_state.crc_error_count++;
				}

#ifdef ETH_ENABLE_STATS
				g_eth_state.stats.rx_errors++;
#endif
				return; // discard
			}

			// check if next descriptor is available
			uint32_t next_idx = ( head_idx + 1 ) % ETH_RX_BUF_COUNT;

			if ( !( g_dma_rx_descs[next_idx].Status & ETH_DMARxDesc_OWN ) )
			{
				// ring full
#ifdef ETH_ENABLE_STATS
				g_eth_state.stats.rx_dropped++;
#endif
			}
			else
			{
				// packet is ready and we have space
				// mark current descriptor as ready for CPU processing
				g_dma_rx_descs[head_idx].Status &= ~ETH_DMARxDesc_OWN;

				// add frame metadata
				g_dma_rx_descs[head_idx].Status |= ( ETH_DMARxDesc_FS | ETH_DMARxDesc_LS | // Single segment frame
													 ( ETH10M->ERXLN << ETH_DMARxDesc_FrameLengthShift ) );

				// advance head to next descriptor for DMA
				g_eth_state.rx_head_idx = next_idx;

				// tell MAC where to write next packet
				ETH10M->ERXST = g_dma_rx_descs[next_idx].Buffer1Addr;

				// signal activity
				if ( g_eth_state.activity_callback )
				{
					g_eth_state.activity_callback();
				}
			}
		}
	}

	if ( flags & RB_ETH_EIR_TXIF )
	{
		ETH10M->EIR = RB_ETH_EIR_TXIF;

		if ( !tx_queue_is_empty( &g_eth_state.tx_q ) )
		{
#ifdef ETH_ENABLE_STATS
			g_eth_state.stats.tx_packets++;
#endif
			tx_queue_consume( &g_eth_state.tx_q );
		}

		tx_start_if_possible();

		// signal activity
		if ( g_eth_state.activity_callback )
		{
			g_eth_state.activity_callback();
		}
	}

	if ( flags & RB_ETH_EIR_TXERIF )
	{
		ETH10M->EIR = RB_ETH_EIR_TXERIF;
#ifdef ETH_ENABLE_STATS
		g_eth_state.stats.tx_errors++;
#endif

		if ( !tx_queue_is_empty( &g_eth_state.tx_q ) )
		{
			tx_queue_consume( &g_eth_state.tx_q );
		}
		tx_start_if_possible();
	}

	if ( flags & RB_ETH_EIR_RXERIF )
	{
		ETH10M->EIR = RB_ETH_EIR_RXERIF;
		ETH10M->ECON1 |= RB_ETH_ECON1_RXEN;
#ifdef ETH_ENABLE_STATS
		g_eth_state.stats.rx_errors++;
#endif
	}

	if ( flags & RB_ETH_EIR_LINKIF )
	{
		g_eth_state.link_irq_flag = true;
		ETH10M->EIR = RB_ETH_EIR_LINKIF;
	}
}

#endif // CH32V208_ETH_IMPLEMENTATION

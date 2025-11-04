#ifndef __DHCPD_H__
#define __DHCPD_H__

int dhcpd_init(void);

void dhcpd_udp_appcall(void);

#define DHCP_SERVER_IP_BYTES 172, 16, 42, 1
#define DHCP_CLIENT_IP_BYTES 172, 16, 42, 2
		
extern const uint8_t host[4];
extern const uint8_t client[4];
#endif


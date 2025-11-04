# CDC-ECM example

This example implements a USB CDC-ECM device using a slightly modified version of uIP v0.9 TCP/IP stack. When connected to a host computer, it will appear as a network interface, allowing the host to communicate with the device.

> [!NOTE]
> Debug logs increase latency.

# Features

The example provides an http server with a simple status API at `/api/status`, returning JSON data about the device as well as a DHCP server and DNS server (optional, see Config section).

```sh
ping 172.16.42.1
curl -vv 'http://172.16.42.1/'
curl -vv 'http://172.16.42.1/api/status'
curl -vv 'http://any.address.you.want/'
```

You can add more pages to the http server by adding files to the `uip/fs/` folder. The files will be embedded into the binary during build.

# Config

## DHCPD
If you want the device to assign IP addresses to connected hosts, you need to enable DHCPD in `uip/uipopt.h`:

```c
#define DHCPD_ENABLE 1
```
The server will only ever assign 1 address: `172.16.42.2`.

## DNS
If you want all domains to get resolved to the device, enable DNS in `uip/uipopt.h`:

```c
#define DNS_ENABLE 1
```
> [!WARNING]
> Enabling this will absolutely break normal DNS resolution on the host, as all domains will resolve to the device's IP address, which will make your internet unusable. Use with caution.

# Reference
 - https://www.xmos.com/download/AN00131:-USB-CDC-ECM-Class-for-Ethernet-over-USB(2.0.2rc1).pdf/
 - https://github.com/majbthrd/D21ecm
 - https://github.com/majbthrd/stm32ecm
 - https://wiki.postmarketos.org/wiki/USB_Network
 - https://github.com/adamdunkels/uip/tree/uip-0-9
 - https://github.com/gl-inet/uboot-ipq60xx/blob/master/uip/dhcpd.c


# CDC-ECM example

This example implements a USB CDC-ECM device using the SFHIP stack. When connected to a host computer, it will appear as a network interface, allowing the host to communicate with the device.

> [!NOTE]
> Debug logs increase latency.

# Features

The example provides an http server with a simple status API at `/api/status`, returning JSON data about the device.

```sh
ping 172.16.42.1
curl -vv 'http://172.16.42.1/'
curl -vv 'http://172.16.42.1/api/status'
curl -vv 'http://any.address.you.want/'
```

# Reference
 - https://github.com/cnlohr/sfhip
 - https://www.xmos.com/download/AN00131:-USB-CDC-ECM-Class-for-Ethernet-over-USB(2.0.2rc1).pdf/
 - https://github.com/majbthrd/D21ecm
 - https://github.com/majbthrd/stm32ecm
 - https://wiki.postmarketos.org/wiki/USB_Network
 - https://github.com/adamdunkels/uip/tree/uip-0-9
 - https://github.com/gl-inet/uboot-ipq60xx/blob/master/uip/dhcpd.c


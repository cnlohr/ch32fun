You can forward all Layer 2 traffic from one interface to another while still having internet access on your host by creating a network bridge. This effectively turns your Linux machine into a transparent switch, forwarding Ethernet frames between the bridged interfaces. To maintain internet connectivity for the host, you'll assign an IP address to the bridge interface itself.

Here's a breakdown of the process using the modern `iproute2` suite, which is standard on most Linux distributions.

### Identify Your Network Interfaces

First, you need to identify the names of the network interfaces you want to bridge. You can list all available interfaces using the following command:

```bash
ip link show
```

Let's assume you want to bridge `eth0` and `eth1`.

### Create a Bridge Interface

Now, create a new bridge interface. We'll name it `br0`:

```bash
sudo ip link add name br0 type bridge
```

### Add Interfaces to the Bridge

Next, you'll add the physical interfaces (`eth0` and `eth1` in our example) to the bridge. This process is often referred to as "enslaving" the interfaces to the bridge.

Before adding an interface to a bridge, it's a good practice to ensure it doesn't have an IP address assigned to it and is in a "promiscuous" mode to accept all traffic.

```bash
sudo ip link set eth0 promisc on
sudo ip link set eth1 promisc on
sudo ip link set eth0 master br0
sudo ip link set eth1 master br0
```

### Assign an IP Address to the Bridge

This is the key step to ensure your host machine maintains internet access. Instead of the physical interfaces having IP addresses, you will now assign the IP address to the bridge interface (`br0`). This can be done either via DHCP or by setting a static IP.

For DHCP:

```bash
sudo dhclient br0
```

For a Static IP:

You'll need to know the IP address, subnet mask, and default gateway for your network.

For example:

```bash
sudo ip addr add 192.168.8.115/24 dev br0
sudo ip route add default via 192.168.8.1
```

### Bring the Interfaces Up

Finally, ensure all the involved interfaces are up and running:

```bash
sudo ip link set eth0 up
sudo ip link set eth1 up
sudo ip link set br0 up
```

### Verification

You can verify that the bridge is correctly set up with the following commands:

  - To see the bridge and its attached interfaces: `brctl show` or `bridge link show`
  - To check the IP address of the bridge: `ip addr show br0`

Your host should now have internet access through the `br0` interface, and any traffic coming into `eth0` will be forwarded to `eth1` at Layer 2, and vice-versa.

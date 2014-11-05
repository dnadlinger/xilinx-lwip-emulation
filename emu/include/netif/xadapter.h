/**
 * An implementation of parts of <netif/xadapter.h>, more specifically the
 * functions for using lwIP with the Xilinx EMAC, allowing to transparently
 * replace it with a tap-device based implementation for emulation on a
 * Linux/... host.
 *
 * Author: David Nadlinger <code@klickverbot.at>
 */
#include <stdint.h>
#include "lwip/ip_addr.h"
#include "lwip/netif.h"

struct netif *xemac_add(
    struct netif *netif,
    struct ip_addr *ipaddr,
    struct ip_addr *netmask,
    struct ip_addr *gw,
    unsigned char *mac_ethernet_address,
    unsigned mac_baseaddr
);

int xemacif_input(struct netif *netif);

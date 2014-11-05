/**
 * Author: David Nadlinger <code@klickverbot.at>
 */
#include "netif/xadapter.h"

#include "lwip/timers.h"
#include "netif/etharp.h"
#include "mintapif.h"

struct netif *xemac_add(struct netif *netif, struct ip_addr *ipaddr,
    struct ip_addr *netmask, struct ip_addr *gw,
    unsigned char *mac_ethernet_address, unsigned mac_baseaddr
) {
    (void)mac_ethernet_address;
    (void)mac_baseaddr;

    return netif_add(netif, ipaddr, netmask, gw, mac_ethernet_address,
        mintapif_init, ethernet_input);
}

int xemacif_input(struct netif *netif) {
    mintapif_select(netif);
    sys_check_timeouts();
    return 0;
}

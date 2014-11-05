#include <stdio.h>
#include "lwip/err.h"
#include "lwip/init.h"
#include "lwip/ip.h"
#include "lwip/tcp.h"
#include "netif/xadapter.h"

// This would be defined in soem Xilinx-supplied headers (or your stubbed-out
// version) in a real application, here it doesn't matter.
#define XPAR_XEMACPS_0_BASEADDR 0x0

err_t recv_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
    printf("Received a packet.\n");

    if (!p) {
        tcp_close(tpcb);
        tcp_recv(tpcb, NULL);
        return ERR_OK;
    }

    tcp_recved(tpcb, p->len);
    pbuf_free(p);

    return ERR_OK;
}


static err_t accept_callback(void *arg, struct tcp_pcb *newpcb, err_t err) {
    printf("Got a connection.\n");
    tcp_recv(newpcb, recv_callback);
    return ERR_OK;
}

static err_t server_listen() {
    unsigned const port = 12345;

    struct tcp_pcb *pcb = tcp_new();
    if (!pcb) {
        printf("Error allocating TCP PCB structure.\n");
        return -1;
    }

    if (tcp_bind(pcb, IP_ADDR_ANY, port) != ERR_OK) {
        printf("Unable to bind to port.\n");
        return -1;
    }

    pcb = tcp_listen(pcb);
    if (!pcb) {
        printf("Failed to allocate in tcp_listen.\n");
        return -1;
    }

    tcp_accept(pcb, accept_callback);
    return ERR_OK;
}

int main() {
    unsigned char mac_addr[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05};

    struct ip_addr ip4addr;
    IP4_ADDR(&ip4addr, 192, 168, 0, 2);

    struct ip_addr gateway;
    IP4_ADDR(&gateway, 192, 168, 0, 1);

    struct ip_addr netmask;
    IP4_ADDR(&netmask, 255, 255, 255, 0);

    lwip_init();

    struct netif interface;
    if (!xemac_add(&interface, &ip4addr, &netmask,
        &gateway, mac_addr, XPAR_XEMACPS_0_BASEADDR)
    ) {
        return -1;
    }

    netif_set_default(&interface);

    // In a real project, you'd probably have mock implementations for these
    // functions defined by the Xilinx SDK too:
    //
    // init_platform();
    // platform_enable_interrupts();

    netif_set_up(&interface);

    if (server_listen() != ERR_OK) {
        return -1;
    }

    printf("Accepting connections.\n");

    while (1) {
        xemacif_input(&interface);
    }

    return 0;
}

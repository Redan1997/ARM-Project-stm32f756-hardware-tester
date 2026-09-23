/*
 * user.c
 *
 *  Created on: 16 ביולי 2026
 *      Author: Redan
 *
 */
#include "user.h"
#include "main.h"
#include "lwip/netif.h"
#include "ip_addr.h"
#include "udp_config.h"
#include <stdio.h>

extern struct netif gnetif;
static uint8_t ip_printed = 0;

void user_main(void)
{
	printf("\033[2J\033[H");
	printf("=== LwIP Network Initialization ===\r\n");
	printf("Waiting for network connection and IP assignment...\r\n");
	udp_test_server_init();
}

void user_loop(void)
{
	if (!ip_printed && netif_is_up(&gnetif) && gnetif.ip_addr.addr != 0) {
		printf("\r\nNetwork Connected Successfully!\r\n");
		printf("IP Address : %s\r\n", ip4addr_ntoa(netif_ip4_addr(&gnetif)));
		printf("Netmask    : %s\r\n", ip4addr_ntoa(netif_ip4_netmask(&gnetif)));
		printf("Gateway    : %s\r\n", ip4addr_ntoa(netif_ip4_gw(&gnetif)));
		ip_printed = 1;
	}
}

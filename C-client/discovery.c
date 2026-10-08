/**
 * @file    discovery.c
 * @author: Redan
 * @date:   30 בספט׳ 2026
 * @brief   Implementation of UUT discovery via UDP broadcast.
 */

#include "discovery.h"
#include "protocol.h"
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>

int discover_uut_ip(uint16_t port, int timeout_ms, char *ip_out, size_t ip_out_len)
{
	int sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock < 0) {
		return 0;
	}

	int broadcast_enable = 1;
	if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &broadcast_enable, sizeof(broadcast_enable)) < 0) {
		close(sock);
		return 0;
	}

	struct timeval tv;
	tv.tv_sec = timeout_ms / 1000;
	tv.tv_usec = (timeout_ms % 1000) * 1000;
	if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
		close(sock);
		return 0;
	}

	struct sockaddr_in broadcast_addr;
	memset(&broadcast_addr, 0, sizeof(broadcast_addr));
	broadcast_addr.sin_family = AF_INET;
	broadcast_addr.sin_port = htons(port);
	broadcast_addr.sin_addr.s_addr = htonl(INADDR_BROADCAST);
	
	TestHeader probe;
	memset(&probe, 0, sizeof(probe));
	probe.test_id = 0;
	probe.peripheral = 0x00; /* not TIMER/UART/SPI/I2C/ADC - falls to dispatch_test's default case */
	probe.iterations = 0;
	probe.pattern_length = 0;
	size_t probe_size = sizeof(probe.test_id) + sizeof(probe.peripheral)
                    						   + sizeof(probe.iterations) + sizeof(probe.pattern_length);

	if (sendto(sock, &probe, probe_size, 0,
			(struct sockaddr *)&broadcast_addr, sizeof(broadcast_addr)) < 0) {
		close(sock);
		return 0;
	}

	ResultPacket response;
	struct sockaddr_in from_addr;
	socklen_t from_len = sizeof(from_addr);
	ssize_t received = recvfrom(sock, &response, sizeof(response), 0,
			(struct sockaddr *)&from_addr, &from_len);
	close(sock);

	if (received != (ssize_t)sizeof(ResultPacket)) {
		return 0; /* timeout or short read */
	}
	if (inet_ntop(AF_INET, &from_addr.sin_addr, ip_out, ip_out_len) == NULL) {
		return 0;
	}
	return 1;
}

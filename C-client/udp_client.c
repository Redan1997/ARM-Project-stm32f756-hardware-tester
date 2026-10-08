/**
 * @file    udp_client.c
 * @author: Redan
 * @date:   30 בספט׳ 2026
 * @brief   Implementation of the UDP client using POSIX sockets.
 */

#include "udp_client.h"
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>

int udp_client_init(UdpClient *client, const char *uut_ip, uint16_t uut_port)
{
	client->sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (client->sock_fd < 0) {
		fprintf(stderr, "Failed to create UDP socket: %s\n", strerror(errno));
		return -1;
	}

	memset(&client->uut_addr, 0, sizeof(client->uut_addr));
	client->uut_addr.sin_family = AF_INET;
	client->uut_addr.sin_port = htons(uut_port);
	if (inet_pton(AF_INET, uut_ip, &client->uut_addr.sin_addr) != 1) {
		fprintf(stderr, "Invalid UUT IP address: %s\n", uut_ip);
		close(client->sock_fd);
		return -1;
	}
	return 0;
}

void udp_client_close(UdpClient *client)
{
	close(client->sock_fd);
}

int udp_client_send_and_wait(UdpClient *client, const TestHeader *cmd, size_t cmd_size,
		int timeout_ms, ResultPacket *response)
{
	ssize_t sent = sendto(client->sock_fd, cmd, cmd_size, 0,
			(struct sockaddr *)&client->uut_addr, sizeof(client->uut_addr));
	if (sent < 0 || (size_t)sent != cmd_size) {
		return 0;
	}

	struct timeval tv;
	tv.tv_sec = timeout_ms / 1000;
	tv.tv_usec = (timeout_ms % 1000) * 1000;
	if (setsockopt(client->sock_fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
		return 0;
	}

	struct sockaddr_in from_addr;
	socklen_t from_len = sizeof(from_addr);
	ssize_t received = recvfrom(client->sock_fd, response, sizeof(*response), 0,
			(struct sockaddr *)&from_addr, &from_len);

	return (received == (ssize_t)sizeof(ResultPacket)) ? 1 : 0;
}

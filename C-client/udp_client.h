/**
 * @file    udp_client.h
 * @author: Redan
 * @date:   30 בספט׳ 2026
 * @brief   UDP client: sends a TestHeader command to the UUT
 *          and blocks (with timeout) for a ResultPacket response.
 */

#ifndef UDP_CLIENT_H_
#define UDP_CLIENT_H_

#include <stdint.h>
#include <netinet/in.h>
#include "protocol.h"

/** @brief Holds one UDP socket bound to talk to one UUT. */
typedef struct {
	int sock_fd;
	struct sockaddr_in uut_addr;
} UdpClient;

/**
 * @brief           Opens a UDP socket and resolves the UUT's address.
 * @param client    Client to initialize.
 * @param uut_ip    UUT's IPv4 address, dotted-decimal string.
 * @param uut_port  UUT's UDP port.
 * @return          0 on success, -1 on socket/address setup failure.
 */
int udp_client_init(UdpClient *client, const char *uut_ip, uint16_t uut_port);

/** @brief Closes the underlying socket. */
void udp_client_close(UdpClient *client);

/**
 * @brief               Sends a test command and blocks for a response.
 * @param client        Initialized client.
 * @param cmd           Command to send (only cmd_size bytes of it are sent).
 * @param cmd_size      Actual wire size of the command (7 + pattern_length).
 * @param timeout_ms    How long to wait for a response before giving up.
 * @param[out] response Filled with the UUT's reply if one arrives.
 * @return              1 if a response was received before the timeout, 0 otherwise.
 */
int udp_client_send_and_wait(UdpClient *client, const TestHeader *cmd, size_t cmd_size,
		int timeout_ms, ResultPacket *response);

#endif /* UDP_CLIENT_H_ */

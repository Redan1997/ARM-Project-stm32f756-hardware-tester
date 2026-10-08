/**
 * @file    discovery.h
 * @author: Redan
 * @date:   30 בספט׳ 2026
 * @brief   Finds the UUT's IP automatically, so the user doesn't have to
 *          type it in. Works by broadcasting a minimal probe packet with
 *          an unrecognized peripheral id (0x00) - the UUT's dispatcher
 *          replies to any such packet immediately, without running any
 *          real test, and the reply's source address reveals its IP.
 */

#ifndef DISCOVERY_H_
#define DISCOVERY_H_

#include <stdint.h>
#include <stddef.h>

/**
 * @brief Broadcasts a discovery probe and waits for the UUT's reply.
 * @param port        UDP port the UUT listens on.
 * @param timeout_ms  How long to wait for a reply before giving up.
 * @param[out] ip_out Filled with the discovered IP as a dotted-decimal string.
 * @param ip_out_len  Size of ip_out.
 * @return 1 if a UUT replied and its IP was captured, 0 on timeout/failure.
 */
int discover_uut_ip(uint16_t port, int timeout_ms, char *ip_out, size_t ip_out_len);

#endif /* DISCOVERY_H_ */
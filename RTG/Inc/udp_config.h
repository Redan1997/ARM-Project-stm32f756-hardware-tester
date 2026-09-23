/**
 * @file udp_config.h
 * @author Redan
 * @date 22 בספט׳ 2026
 */

#ifndef INC_UDP_CONFIG_H_
#define INC_UDP_CONFIG_H_


/** @brief UDP port the UUT listens on for test commands. */
#define TEST_UDP_PORT 1997

/**
 * @brief Initializes the UDP test server.
 *        This function sets up a UDP PCB, binds it to TEST_UDP_PORT,
 *        and registers a receive callback function.
 */
void udp_test_server_init(void);

#endif /* INC_UDP_CONFIG_H_ */

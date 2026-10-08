/**
 * @file udp_config.c
 * @author Redan
 * @date 22 בספט׳ 2026
 * @brief UDP test server initialization and callback implementation.
 */


#include "udp_config.h"
#include "test_protocol.h"
#include "peripheal_test.h"
#include "lwip/udp.h"
#include "lwip/pbuf.h"
#include <string.h>
#include <stddef.h>

static struct udp_pcb *test_pcb;

/**
 * @brief Dispatches a test request to the appropriate peripheral test function.
 * @param peripheral Peripheral bitfield from TestHeader_t.
 * @param data Pointer to the test data.
 * @param length Length of the test data.
 * @param iterations Number of times to run the test.
 * @return Test result.
 */
static uint8_t dispatch_test(uint8_t peripheral, uint8_t *data, uint16_t length, uint8_t iterations)
{
	switch (peripheral) {
	case PERIPHERAL_TIMER: return Timer_test(data, length, iterations);
	case PERIPHERAL_UART:  return UART_test_loopback(data, length, iterations);
	case PERIPHERAL_SPI:   return SPI_test_loopback(data, length, iterations);
	case PERIPHERAL_I2C:   return I2C_test_loopback(data, length, iterations);
	case PERIPHERAL_ADC:   return ADC_test(data, length, iterations);
	default:                return TEST_RESULT_FAILURE;
	}
}

/**
 * @brief lwIP receive callback: parses a TestHeader_t, runs the
 *        requested test, and sends a ResultPacket_t back to the sender.
 *        Runs in the main-loop context (called from MX_LWIP_Process(),
 *        not from an interrupt), so it may safely block for the
 *        duration of the test.
 */
static void udp_test_recv_callback(void *arg, struct udp_pcb *pcb, struct pbuf *p,const ip_addr_t *addr, u16_t port)
{
	if (p == NULL) {
		return;
	}
	const size_t fixed_len = sizeof(TestHeader_t) - TEST_MAX_PATTERN_LEN; /* 7 bytes */
	if (p->tot_len < fixed_len) {// ensure the packet is large enough to contain the fixed header
		pbuf_free(p);
		return;
	}

	static TestHeader_t header;
	memset(&header, 0, sizeof(TestHeader_t));

	/* Read just the fixed fields first - this also tells us pattern_length,
	 * which we need before we know how many more bytes to expect/copy. */
	pbuf_copy_partial(p, &header, fixed_len, 0);
	size_t expected_total = fixed_len + header.pattern_length;
	if (p->tot_len < expected_total) {
		pbuf_free(p);
		return;
	}
	/* Copy exactly pattern_length bytes (max 255)*/
	pbuf_copy_partial(p, header.bit_pattern, header.pattern_length, fixed_len);
	/*test start*/
	uint8_t result = dispatch_test(header.peripheral, header.bit_pattern,header.pattern_length, header.iterations);

	ResultPacket_t response = {.test_id=header.test_id,.test_result = result}; //result of test

	struct pbuf *reply = pbuf_alloc(PBUF_TRANSPORT, sizeof(ResultPacket_t), PBUF_RAM);// allocate a pbuf for the response
	if (reply != NULL) {
		memcpy(reply->payload, &response, sizeof(ResultPacket_t));
		udp_sendto(pcb, reply, addr, port);
		pbuf_free(reply);
	}

	pbuf_free(p);
}

void udp_test_server_init(void)
{
	test_pcb = udp_new();// create a new UDP control block
	if (test_pcb == NULL) {
		return;
	}
	//bind to any IP address on the port.
	if(udp_bind(test_pcb, IP_ADDR_ANY, TEST_UDP_PORT) != ERR_OK) {
		udp_remove(test_pcb);
		test_pcb = NULL;
		return;
	}
	udp_recv(test_pcb, udp_test_recv_callback, NULL);
}
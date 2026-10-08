/**
 * @file    protocol.h
 * @author: Redan
 * @date:   30 בספט׳ 2026
 * @brief   structures for the proprietary UDP test protocol,
 *          mirroring the UUT's test_protocol.h exactly. Field order,
 *          sizes, and packing must match the firmware side byte-for-byte.
 */

#ifndef PROTOCOL_H_
#define PROTOCOL_H_

#include <stdint.h>
#include <stddef.h>

/** @brief Peripheral selector bitfield*/
#define PERIPHERAL_TIMER 0x01
#define PERIPHERAL_UART  0x02
#define PERIPHERAL_SPI   0x04
#define PERIPHERAL_I2C   0x08
#define PERIPHERAL_ADC   0x16

/** @brief Result codes*/
#define TEST_RESULT_SUCCESS 0x01
#define TEST_RESULT_FAILURE 0xFF

/** @brief Max bit-pattern size*/
#define MAX_PATTERN_LEN 256

/**
 * @brief command packet sent to the UUT. bit_pattern's *used* length is
 *        pattern_length; only the fixed header plus pattern_length bytes
 *        are actually placed on the wire ,not the full 256-byte array.
 */
typedef struct __attribute__((packed)) {
	uint32_t test_id;
	uint8_t  peripheral;
	uint8_t  iterations;
	uint8_t  pattern_length;
	uint8_t  bit_pattern[MAX_PATTERN_LEN];
} TestHeader;

/** @brief Result packet received from the UUT. */
typedef struct __attribute__((packed)) {
	uint32_t test_id;
	uint8_t  test_result;
} ResultPacket;

/**
 * @brief Maps a peripheral name (as typed at the CLI) to its protocol byte.
 * @param name        Case-insensitive name: timer, uart, spi, i2c, adc.
 * @param (return) id      Filled with the matching peripheral byte on success.
 * @return 1 if name was recognized, 0 otherwise.
 */
int peripheral_name_to_id(const char *name, uint8_t *id);

/** @brief Returns a display name for a peripheral byte, or "UNKNOWN" if unrecognized. */
const char *peripheral_id_to_name(uint8_t id);

#endif /* PROTOCOL_H_ */

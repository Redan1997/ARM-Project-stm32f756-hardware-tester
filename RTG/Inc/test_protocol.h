/**
 * @file	test_protocol.h
 * @Author: Redan Created on: 20 בספט׳ 2026
 * @brief 	Wire-format structures for the proprietary UDP test protocol
 *          exchanged between the P.C. server and the UUT
 */

#ifndef INC_TEST_PROTOCOL_H_
#define INC_TEST_PROTOCOL_H_

#include <stdint.h>
#include "test_config.h"

/** @brief Enum for identifying different peripheral types
 */
typedef enum {
	PERIPHERAL_TIMER = 0x01,
	PERIPHERAL_UART  = 0x02,
	PERIPHERAL_SPI   = 0x04,
	PERIPHERAL_I2C   = 0x08,
	PERIPHERAL_ADC   = 0x16
} PeripheralId_t;

/*
 * Test ID - unsigned 4 bytes
 * Peripheral to be tested - unsigned 1 byte (only one peripheral per test)
 * 		0x01 - Timer
 * 		0x02 - UART
 * 		0x04 - SPI
 * 		0x08 - I2C
 * 		0x16 - ADC
 * Number of test iterations - unsigned 1 byte
 * BIT pattern length - unsigned 1 byte
 * Bit pattern - unsigned 1 byte (string of characters sent to the uut)
 */

/**
 * @brief Test header structure that received from the P.C. Testing Program to the UUT
 */
typedef struct {
	uint32_t test_id;
	uint8_t  peripheral;
	uint8_t  iterations;
	uint8_t  pattern_length;
	uint8_t  bit_pattern[TEST_MAX_PATTERN_LEN];
}__attribute__((packed)) TestHeader_t;
/**
 * @brief Test result structure that sent from the UUT back to the P.C. Testing Program
 */
typedef enum {
	TEST_RESULT_SUCCESS = 0x01,
	TEST_RESULT_FAILURE = 0xFF
}TestResult_t;

/**
 * @brief Test result structure that sent from the UUT back to the P.C. Testing Program
 */
typedef struct {
	uint32_t test_id;
	uint8_t  test_result;
}__attribute__((packed)) ResultPacket_t;

#endif /* INC_TEST_PROTOCOL_H_ */

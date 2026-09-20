/**
 * @file 	peripheal_test.h
 * @Author: Redan Created on: 16 בספט׳ 2026
 * @brief	loopback hardware verification test for uart,spi,i2c.
 * 			Each function drives a master/slave pair, round-trips the given
 *          bit pattern for the requested number of iterations, and reports
 *          pass/fail per the protocol's TestResult_t values.
 */

#ifndef INC_PERIPHEAL_TEST_H_
#define INC_PERIPHEAL_TEST_H_

#include <stdint.h>
#include <stdio.h>
/**
 * @brief Runs a UART0(UART4) <-> UART1(USART6) loopback test.
 * @param data       Bit pattern to send.
 * @param length     Pattern length in bytes.
 * @param iterations Number of round-trips to perform.
 * @return TEST_RESULT_SUCCESS (0x01) or TEST_RESULT_FAILURE (0xFF).
 */
uint8_t UART_test_loopback(uint8_t *data, uint16_t length, uint8_t iterations);

/**
 * @brief Runs an I2C1(master) <-> I2C2(slave) loopback test.
 * @param data       Bit pattern to send.
 * @param length     Pattern length in bytes.
 * @param iterations Number of round-trips to perform.
 * @return TEST_RESULT_SUCCESS (0x01) or TEST_RESULT_FAILURE (0xFF).
 */
uint8_t I2C_test_loopback(uint8_t *data, uint16_t length, uint8_t iterations);

/**
 * @brief Runs an SPI1(master) <-> SPI4(slave) loopback test.
 * @param data       Bit pattern to send.
 * @param length     Pattern length in bytes.
 * @param iterations Number of round-trips to perform.
 * @return TEST_RESULT_SUCCESS (0x01) or TEST_RESULT_FAILURE (0xFF).
 */
uint8_t SPI_test_loopback(uint8_t *data, uint16_t length, uint8_t iterations);

uint8_t SPI_test_loopback(uint8_t *data, uint16_t length, uint8_t iterations);

/**
 * @brief Verifies ADC1 against the chip's factory-calibrated internal
 *        VREFINT channel - no external wiring required.
 * @param data       Unused (kept for uniform dispatch signature).
 * @param length     Unused (kept for uniform dispatch signature).
 * @param iterations Number of conversions to check.
 * @return TEST_RESULT_SUCCESS (0x01) or TEST_RESULT_FAILURE (0xFF).
 */
uint8_t ADC_test(uint8_t *data, uint16_t length, uint8_t iterations);

/**
 * @brief Verifies TIM2 counts at the expected rate, checked against
 *        SysTick (an independent clock path) - no external wiring required.
 * @param data       Unused (kept for uniform dispatch signature).
 * @param length     Unused (kept for uniform dispatch signature).
 * @param iterations Number of timing windows to check.
 * @return TEST_RESULT_SUCCESS (0x01) or TEST_RESULT_FAILURE (0xFF).
 */
uint8_t Timer_test(uint8_t *data, uint16_t length, uint8_t iterations);

#endif /* INC_PERIPHEAL_TEST_H_ */

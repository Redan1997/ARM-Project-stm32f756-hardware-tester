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

/**
 * @brief   verfication of the ADC1 against the chip's factory-calibrated internal VREFINT channel
            VREFINT is a stable voltage reference that can be used to verify the ADC's accuracy
            the test reads the VREFINT channel multiple times and compares the result against the expected value(from datasheets reference voltage),
            allowing for a small tolerance. conversion tolerance for ADC tests, in raw 12-bit ADC LSBs.
 * @param iterations Number of conversions to check.
 * @return TEST_RESULT_SUCCESS (0x01) or TEST_RESULT_FAILURE (0xFF).
 */
uint8_t ADC_test(uint8_t *data, uint16_t length, uint8_t iterations);

/**
 * @brief Verifies TIM2 by free-running it as a counter and checking it
 *        counts at the expected rate over a known real-time window,
 *        measured via HAL_GetTick() (SysTick - an independent clock
 *        path from TIM2). No external wiring needed: a misconfigured
 *        prescaler or a dead timer block shows up as a count far outside
 *        the expected value for the elapsed wall-clock time.
 * @param iterations Number of timing windows to check.
 * @return TEST_RESULT_SUCCESS (0x01) or TEST_RESULT_FAILURE (0xFF).
 */
uint8_t Timer_test(uint8_t *data, uint16_t length, uint8_t iterations);

#endif /* INC_PERIPHEAL_TEST_H_ */

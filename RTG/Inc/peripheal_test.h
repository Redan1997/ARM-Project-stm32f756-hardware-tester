/**
 * @file    peripheral_test.h
 * @author  Redan
 * @date    16 בספט׳ 2026
 * @brief   Hardware verification test interfaces for UART, SPI, I2C, ADC, and Timer.
 * @details Each loopback function drives a master/slave peripheral pair, round-trips
 *          the given bit pattern for the requested iterations, and reports pass/fail
 *          results. ADC and Timer share the same uniform function signature to
 *          support a simple dispatch table/switch mechanism.
 */

#ifndef INC_PERIPHEAL_TEST_H_
#define INC_PERIPHEAL_TEST_H_

#include <stdint.h>
#include <stdio.h>
/**
 * @brief  Runs a 	  UART0(UART4) <-> UART1(USART6) loopback test.
 * @param  data       Bit pattern to send.
 * @param  length     Pattern length in bytes.
 * @param  iterations Number of round-trips to perform.
 * @return uint8_t    TEST_RESULT_SUCCESS (0x01) or TEST_RESULT_FAILURE (0xFF).
 */
uint8_t UART_test_loopback(uint8_t *data, uint16_t length, uint8_t iterations);

/**
 * @brief  Runs an    I2C1(master) <-> I2C2(slave) loopback test.
 * @param  data       Bit pattern to send.
 * @param  length     Pattern length in bytes.
 * @param  iterations Number of round-trips to perform.
 * @return uint8_t    TEST_RESULT_SUCCESS (0x01) or TEST_RESULT_FAILURE (0xFF).
 */
uint8_t I2C_test_loopback(uint8_t *data, uint16_t length, uint8_t iterations);

/**
 * @brief  Runs an    SPI1(master) <-> SPI4(slave) loopback test.
 * @param  data       Bit pattern to send.
 * @param  length     Pattern length in bytes.
 * @param  iterations Number of round-trips to perform.
 * @return uint8_t    TEST_RESULT_SUCCESS (0x01) or TEST_RESULT_FAILURE (0xFF).
 */
uint8_t SPI_test_loopback(uint8_t *data, uint16_t length, uint8_t iterations);

/**
 * @brief  Executes the ADC VREFINT internal channel test across specified iterations.
 * @note   Uses a uniform function signature shared by all peripheral test modules
 *         to support dispatch via a single function pointer table / switch.
 * @param  data       Unused by ADC test (kept for signature uniformity).
 * @param  length     Unused by ADC test (kept for signature uniformity).
 * @param  iterations Number of test iterations to execute.
 * @return uint8_t    TEST_RESULT_SUCCESS (0x01) or TEST_RESULT_FAILURE (0xFF).
 */
uint8_t ADC_test(uint8_t *data, uint16_t length, uint8_t iterations);

/**
 * @brief  Executes the TIM2 hardware timer test across specified iterations.
 * @note   Uses a uniform function signature shared by all peripheral test modules
 *         to support dispatch via a single function pointer table / switch.
 * @param  data       Unused by Timer test (kept for signature uniformity).
 * @param  length     Unused by Timer test (kept for signature uniformity).
 * @param  iterations Number of test iterations to execute.
 * @return uint8_t    TEST_RESULT_SUCCESS (0x01) or TEST_RESULT_FAILURE (0xFF).
 */
uint8_t Timer_test(uint8_t *data, uint16_t length, uint8_t iterations);

#endif /* INC_PERIPHEAL_TEST_H_ */

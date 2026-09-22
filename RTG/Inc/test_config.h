/**
 * @file	test_config.h
 * @Author: Redan Created on: 20 בספט׳ 2026
 * @brief   Central configuration for peripheral hardware verification tests.
 *          Keeping timeouts / thresholds / addresses here avoids magic
 *          numbers scattered across the test modules.
 */

#ifndef INC_TEST_CONFIG_H_
#define INC_TEST_CONFIG_H_

/** @brief I2C slave address, pre-shifted to HAL's 8-bit address format (7-bit addr 0x20). */
#define I2C_SLAVE_ADDR          (0x20 << 1)

/** @brief Max bit-pattern size, matching the protocol's 1-byte length field (0-255). */
#define TEST_MAX_PATTERN_LEN    256

/** @brief Max time to wait for a DMA-complete callback before declaring a timeout failure. */
#define TEST_TIMEOUT_MS         1000

/** @brief Conversion tolerance for ADC tests */
#define ADC_TOLERANCE 20

/** @brief Data length above which CRC-32 compare replaces byte-by-byte compare . */
#define CRC_COMPARE_THRESHOLD   100

/** @brief APB1 timer clock feeding TIM2, per .ioc RCC.APB1TimFreq_Value. */
#define TIM2_INPUT_CLOCK_HZ     108000000

/** @brief TIM2 prescaler register value, per .ioc TIM2.Prescaler. */
#define TIM2_PSC                107

/** @brief TIM2's actual counting rate: input clock / (PSC + 1). */
#define TIM2_CLOCK_HZ           (TIM2_INPUT_CLOCK_HZ / (TIM2_PSC + 1))

/** @brief Real-time window sampled per iteration, in milliseconds. */
#define TIMER_TEST_WINDOW_MS    100

/** @brief Allowed deviation from the expected count, as a percentage. */
#define TIMER_TOLERANCE_PCT     2

#endif /* INC_TEST_CONFIG_H_ */

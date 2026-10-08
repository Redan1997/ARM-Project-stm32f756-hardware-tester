/**
 * @file timer_test.c
 * @author: Redan
 * @date: 20 בספט׳ 2026
 * @note  TIM2 is fed by the APB1 timer clock (108MHz), with PSC=107
 *        (divider = PSC+1 = 108), giving an exact 1MHz tick rate.
 *        ARR=0xFFFFFFFF.
 */

#include "peripheal_test.h"
#include "test_config.h"
#include "test_protocol.h"
#include "tim.h"

uint8_t Timer_test(uint8_t *data, uint16_t length, uint8_t iterations)
{
	/* Mark unused parameters to maintain a uniform signature without compiler warnings */
	(void)data;
	(void)length;
	//SUCCESS here does not mean the peripheral was verified working nothing was tested so nothing failed
	if (iterations == 0) {
		return TEST_RESULT_SUCCESS;
	}
	uint32_t expected = (uint32_t)(((uint64_t)TIM2_CLOCK_HZ * TIMER_TEST_WINDOW_MS) / 1000U);
	uint32_t max_diff = (expected * TIMER_TOLERANCE_PCT) / 100U;

	for (uint8_t i = 0; i < iterations; i++) {
		__HAL_TIM_SET_COUNTER(&htim2, 0);
		HAL_TIM_Base_Start(&htim2);

		uint32_t t0 = HAL_GetTick();
		while (HAL_GetTick() - t0 < TIMER_TEST_WINDOW_MS);

		uint32_t counted = __HAL_TIM_GET_COUNTER(&htim2);
		HAL_TIM_Base_Stop(&htim2);

		if (counted > expected + max_diff || counted < expected - max_diff) {
			return TEST_RESULT_FAILURE;
		}
	}
	return TEST_RESULT_SUCCESS;
}

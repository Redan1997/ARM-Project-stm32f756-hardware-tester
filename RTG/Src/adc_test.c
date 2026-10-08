/**
 * @file    adc_test.c
 * @author: Redan
 * @date:   20 בספט׳ 2026
 * @brief Verifies ADC1 against the chip's factory-calibrated internal
 *        VREFINT channel
 */
#include "peripheal_test.h"
#include "adc.h"
#include "dma_error_report.h"
#include "test_config.h"
#include "test_protocol.h"
#include "stm32f7xx_ll_adc.h"
static volatile uint8_t adc_conv_done = 0;

uint8_t ADC_test(uint8_t *data, uint16_t length, uint8_t iterations) {
	/* Mark unused parameters to maintain a uniform signature without compiler warnings */
	(void)data;
	(void)length;
	//SUCCESS here does not mean the peripheral was verified working nothing was tested so nothing failed
	if (iterations == 0) {
		return TEST_RESULT_SUCCESS;
	}
	uint16_t vrefint_cal = *VREFINT_CAL_ADDR;/*((uint16_t*) (0x1FF0F44A))*/
	uint32_t adc_value;
	HAL_StatusTypeDef status;

	for (uint8_t i = 0; i < iterations; i++) {
		adc_conv_done=0;
		adc_value=0;
		status = HAL_ADC_Start_DMA(&hadc1, (uint32_t *)&adc_value, 1);
		if (status != HAL_OK) {
			report_dma_error(status, ADc, HAL_ADC_GetError(&hadc1));
			return TEST_RESULT_FAILURE;
		}


		uint32_t time= HAL_GetTick();
		while(!adc_conv_done){
			if(HAL_GetTick()-time>TEST_TIMEOUT_MS){
				HAL_ADC_Stop_DMA(&hadc1);
				return TEST_RESULT_FAILURE;
			}
		}
		HAL_ADC_Stop_DMA(&hadc1);
		uint16_t adc16Bit=(uint16_t)(adc_value&0xFFFF);
		if (adc16Bit < (vrefint_cal - ADC_TOLERANCE) || adc16Bit > (vrefint_cal + ADC_TOLERANCE)) {
			return TEST_RESULT_FAILURE;
		}
	}
	return TEST_RESULT_SUCCESS;
}
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
	if (hadc->Instance == ADC1) {
		adc_conv_done = 1;
	}
}

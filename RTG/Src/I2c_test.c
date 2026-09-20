/*
 * @file I2c_test.c
 * @author: Redan
 * @date: 20 בספט׳ 2026
 * @brief implementation of the I2C loopback test function, using DMA for both master and slave transfers.
 * @GPIO: ic21: PB8(SCL), PB9(SDA) - master with pull-up resistors master address 0x10 (7-bit) (not used in this test)
 *        ic22: PF1(SCL), PF0(SDA) - slave with pull-up resistors, slave address 0x20 (7-bit)
 */

#include "peripheal_test.h"
#include "test_config.h"
#include "test_protocol.h"
#include "data_compare.h"
#include "i2c.h"
#include "dma_error_report.h"
#include <string.h>

static volatile uint8_t flagI2C2=0;
static volatile uint8_t flagI2C1=0;


uint8_t I2C_test_loopback(uint8_t *data, uint16_t length, uint8_t iterations){
	static uint8_t rx_buffer_x[TEST_MAX_PATTERN_LEN];
	static uint8_t rx_buffer_y[TEST_MAX_PATTERN_LEN];
	HAL_StatusTypeDef status;
	for(uint8_t i=0;i<iterations;i++){
		memset(rx_buffer_x, 0, length);
		memset(rx_buffer_y, 0, length);
		flagI2C1=0;
		flagI2C2=0;
		status = HAL_I2C_Slave_Receive_DMA(&hi2c2, rx_buffer_y, length);
		if (status != HAL_OK) {
			report_dma_error(status, I2C, HAL_I2C_GetError(&hi2c2));
			return TEST_RESULT_FAILURE;
		}
		status = HAL_I2C_Master_Transmit_DMA(&hi2c1, I2C_SLAVE_ADDR, data, length);
		if (status != HAL_OK) {
			report_dma_error(status, I2C, HAL_I2C_GetError(&hi2c1));
			return TEST_RESULT_FAILURE;
		}
		uint32_t start_time = HAL_GetTick();
		while(!flagI2C2){
			if(HAL_GetTick() - start_time > TEST_TIMEOUT_MS){ // Timeout
				return TEST_RESULT_FAILURE;
			}
		}
		flagI2C2=0;
		status = HAL_I2C_Slave_Transmit_DMA(&hi2c2, rx_buffer_y, length);
		if (status != HAL_OK) {
			report_dma_error(status, I2C, HAL_I2C_GetError(&hi2c2));
			return TEST_RESULT_FAILURE;
		}
		status = HAL_I2C_Master_Receive_DMA(&hi2c1, I2C_SLAVE_ADDR, rx_buffer_x, length);
		if (status != HAL_OK) {
			report_dma_error(status, I2C, HAL_I2C_GetError(&hi2c1));
			return TEST_RESULT_FAILURE;
		}
		start_time = HAL_GetTick();
		while(!flagI2C1){
			if(HAL_GetTick() - start_time > TEST_TIMEOUT_MS){ // Timeout
				return TEST_RESULT_FAILURE;
			}
		}
		if(!CRC_COMPARE(data, rx_buffer_x, length))
			return TEST_RESULT_FAILURE;
	}
	return TEST_RESULT_SUCCESS;
}

void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
	if (hi2c->Instance == I2C2)
	{
		flagI2C2 = 1;
	}
}

void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
	if (hi2c->Instance == I2C1)
	{
		flagI2C1 = 1;
	}
}

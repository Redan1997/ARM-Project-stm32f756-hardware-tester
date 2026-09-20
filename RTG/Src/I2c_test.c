/*
 * I2c_test.c
 *
 *  Created on: 20 בספט׳ 2026
 *      Author: Redan
 */

#include "peripheal_test.h"
#include "test_config.h"
#include "test_protocol.h"
#include "data_compare.h"
#include "i2c.h"
#include <string.h>

static volatile uint8_t flagI2C2=0;
static volatile uint8_t flagI2C1=0;


uint8_t I2C_test_loopback(uint8_t *data, uint16_t length, uint8_t iterations){
	static uint8_t rx_buffer_x[256];
	static uint8_t rx_buffer_y[256];
	for(uint8_t i=0;i<iterations;i++){
		memset(rx_buffer_x, 0, length);
		memset(rx_buffer_y, 0, length);
		flagI2C1=0;
		flagI2C2=0;
		HAL_I2C_Slave_Receive_DMA(&hi2c2, rx_buffer_y, length);
		HAL_I2C_Master_Transmit_DMA(&hi2c1, I2C_SLAVE_ADDR, data, length);
		uint32_t start_time = HAL_GetTick();
		while(!flagI2C2){
			if(HAL_GetTick() - start_time > 1000){ // Timeout
				return 0xff;
			}
		}
		flagI2C2=0;
		HAL_I2C_Slave_Transmit_DMA(&hi2c2, rx_buffer_y, length);
		HAL_I2C_Master_Receive_DMA(&hi2c1, I2C_SLAVE_ADDR, rx_buffer_x, length);
		start_time = HAL_GetTick();
		while(!flagI2C1){
			if(HAL_GetTick() - start_time > 1000){ // Timeout
				return 0xff;
			}
		}
		if(!CRC_COMPARE(data, rx_buffer_x, length))
					return 0xff;
	}
	return 0x01;
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

/*
 * uart_test.c
 *
 *  Created on: 20 בספט׳ 2026
 *      Author: Redan
 */

#include "peripheal_test.h"
#include "test_config.h"
#include "test_protocol.h"
#include "data_compare.h"
#include "usart.h"
#include <string.h>

volatile uint8_t flagUART4=0;
volatile uint8_t flagUART6=0;
uint8_t UART_test_loopback(uint8_t *data, uint16_t length, uint8_t iterations){
	static uint8_t rx_buffer_x[256];
	static uint8_t rx_buffer_y[256];
	for(uint8_t i=0;i<iterations;i++){
		flagUART6=0;
		flagUART4=0;
		memset(rx_buffer_x, 0, length);
		memset(rx_buffer_y, 0, length);
		HAL_UART_Receive_DMA(&huart6, rx_buffer_y, length);
		HAL_UART_Transmit_DMA(&huart4, data, length);
		uint32_t start_time = HAL_GetTick();
		while(!flagUART4){
			if(HAL_GetTick() - start_time > 1000){ // Timeout
				return 0xff;
			}
		}
		flagUART4 = 0;
		HAL_UART_Receive_DMA(&huart4, rx_buffer_x, length);
		HAL_UART_Transmit_DMA(&huart6, rx_buffer_y, length);
		start_time = HAL_GetTick();
		while(!flagUART6){
			if(HAL_GetTick() - start_time > 1000){ // Timeout
				return 0xff;
			}
		}
		flagUART6=0;
		if(!CRC_COMPARE(data, rx_buffer_x, length)){
			return 0xff;
		}
	}
	return 0x01;
}
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart->Instance == USART6)
	{
		flagUART4=1;/// better not to call receive nor transmit from callback gotdata
	}
	if(huart->Instance== UART4){
		flagUART6=1;
	}
}

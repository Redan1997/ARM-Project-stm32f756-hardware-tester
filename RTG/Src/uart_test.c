/**
 * @file uart_test.c
 * @author: Redan
 * @date: 20 בספט׳ 2026
 * @brief implementation of the UART loopback test function, using DMA for both master and slave transfers.	
 * @GPIO: uart4: PC10(TX), PC11(RX) - Asynchronous baud rate 115200, 8bit data frame, 1 stop bit, no parity
 *        usart6: PC6(TX), PC7(RX) - Asynchronous baud rate 115200, 8bit data frame, 1 stop bit, no parity
 */

#include "peripheal_test.h"
#include "test_config.h"
#include "test_protocol.h"
#include "data_compare.h"
#include "usart.h"
#include <string.h>
#include "dma_error_report.h"

static volatile uint8_t flagUART4=0;
static volatile uint8_t flagUART6=0;

uint8_t UART_test_loopback(uint8_t *data, uint16_t length, uint8_t iterations){
	static uint8_t rx_buffer_x[TEST_MAX_PATTERN_LEN];
	static uint8_t rx_buffer_y[TEST_MAX_PATTERN_LEN];
	HAL_StatusTypeDef status;
	for(uint8_t i=0;i<iterations;i++){
		flagUART6=0;
		flagUART4=0;
		memset(rx_buffer_x, 0, length);
		memset(rx_buffer_y, 0, length);
		status = HAL_UART_Receive_DMA(&huart6, rx_buffer_y, length);
		if (status != HAL_OK) {
			report_dma_error(status, UART, HAL_UART_GetError(&huart6));
			return TEST_RESULT_FAILURE;
		}
		status = HAL_UART_Transmit_DMA(&huart4, data, length);
		if (status != HAL_OK) {
			report_dma_error(status, UART, HAL_UART_GetError(&huart4));
			return TEST_RESULT_FAILURE;
		}
		uint32_t start_time = HAL_GetTick();
		while(!flagUART4){
			if(HAL_GetTick() - start_time > TEST_TIMEOUT_MS){ // Timeout
				return TEST_RESULT_FAILURE;
			}
		}
		flagUART4 = 0;
		status = HAL_UART_Receive_DMA(&huart4, rx_buffer_x, length);
		if (status != HAL_OK) {
			report_dma_error(status, UART, HAL_UART_GetError(&huart4));
			return TEST_RESULT_FAILURE;
		}
		status = HAL_UART_Transmit_DMA(&huart6, rx_buffer_y, length);
		if (status != HAL_OK) {
			report_dma_error(status, UART, HAL_UART_GetError(&huart6));
			return TEST_RESULT_FAILURE;
		}
		start_time = HAL_GetTick();
		while(!flagUART6){
			if(HAL_GetTick() - start_time > TEST_TIMEOUT_MS){ // Timeout
				return TEST_RESULT_FAILURE;
			}
		}
		flagUART6=0;
		if(!CRC_COMPARE(data, rx_buffer_x, length)){
			return TEST_RESULT_FAILURE;
		}
	}
	return TEST_RESULT_SUCCESS;
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

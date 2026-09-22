/*
 * user.c
 *
 *  Created on: 16 ביולי 2026
 *      Author: Redan
 *
 */
#include "main.h"
#include "user.h"
#include <stdio.h>
#include "tim.h"
#include <string.h>
#include "peripheal_test.h"
void user_main(void){
	/*CALLED FROM CORE/SRC/MAIN.c*/
	HAL_UART_Transmit(&huart3, (uint8_t*)"\033[2J\033[H", 7, HAL_MAX_DELAY);//clean terminal
	uint8_t sample_data[] = "TEST_PATTERNTEST_PATTERNTEST_PATTERNTEST_PATTERNTEST_PATTERNTEST_PATTERNTEST_PATTERNTEST_PATTERNTEST_PATTERNTEST_PATTERN";
	uint8_t result = UART_test_loopback(sample_data, strlen((char*)sample_data), 5);
	if(result == 0x01){
		HAL_UART_Transmit(&huart3, (uint8_t*)"UART Test Passed\r\n", 18, HAL_MAX_DELAY);
	}else{
		HAL_UART_Transmit(&huart3, (uint8_t*)"UART Test Failed\n", 17, HAL_MAX_DELAY);
	}
	result =SPI_test_loopback(sample_data, (uint16_t)strlen((char*)sample_data), 5);
	if(result == 0x01){
		HAL_UART_Transmit(&huart3, (uint8_t*)"SPI Test Passed\r\n", 18, HAL_MAX_DELAY);
	}else{
		HAL_UART_Transmit(&huart3, (uint8_t*)"SPI Test Failed\n", 17, HAL_MAX_DELAY);
	}
	result = I2C_test_loopback(sample_data, strlen((char*)sample_data), 5);
	if(result == 0x01){
		HAL_UART_Transmit(&huart3, (uint8_t*)"I2C Test Passed\r\n", 18, HAL_MAX_DELAY);
	}else{
		HAL_UART_Transmit(&huart3, (uint8_t*)"I2C Test Failed\n", 17, HAL_MAX_DELAY);
	}
	result = ADC_test(NULL, 0, 1);
	if(result == 0x01){
		HAL_UART_Transmit(&huart3, (uint8_t*)"ADC Test Passed\r\n", 18, HAL_MAX_DELAY);
	}else{
		HAL_UART_Transmit(&huart3, (uint8_t*)"ADC Test Failed\n", 17, HAL_MAX_DELAY);
	}
	result = ADC_test(NULL, 0, 5);
	if(result == 0x01){
		HAL_UART_Transmit(&huart3, (uint8_t*)"ADC Test 2 Passed\r\n", 18, HAL_MAX_DELAY);
	}else{
		HAL_UART_Transmit(&huart3, (uint8_t*)"ADC Test 2 Failed\n", 17, HAL_MAX_DELAY);
	}
	result = Timer_test(NULL, 0, 1);
	if(result == 0x01){
		HAL_UART_Transmit(&huart3, (uint8_t*)"timer Test Passed\r\n", 18, HAL_MAX_DELAY);
	}else{
		HAL_UART_Transmit(&huart3, (uint8_t*)"timer Test Failed\n", 17, HAL_MAX_DELAY);
	}
	result = Timer_test(NULL, 0, 5);
	if(result == 0x01){
		HAL_UART_Transmit(&huart3, (uint8_t*)"timer Test 2 Passed\r\n", 18, HAL_MAX_DELAY);
	}else{
		HAL_UART_Transmit(&huart3, (uint8_t*)"timer Test 2 Failed\n", 17, HAL_MAX_DELAY);
	}
}






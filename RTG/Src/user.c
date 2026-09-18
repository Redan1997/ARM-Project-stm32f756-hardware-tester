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
#include "dma.h"
#include "tim.h"
#include "i2c.h"
#include "spi.h"
#include "usart.h"
#include <string.h>
/*
 * Test ID - unsigned 4 bytes
 * Peripheral to be tested - unsigned 1 byte (only one peripheral per test)
 * 		0x01 - Timer
 * 		0x02 - UART
 * 		0x04 - SPI
 * 		0x08 - I2C
 * 		0x16 - ADC
 * Number of test iterations - unsigned 1 byte
 * BIT pattern length - unsigned 1 byte
 * Bit pattern - unsigned 1 byte (string of characters sent to the uut)
 */
typedef struct {
	uint32_t test_id;
	uint8_t  peripheral;
	uint8_t  iterations;
	uint8_t  pattern_length;
	uint8_t  bit_pattern[256];
}__attribute__((packed)) TestHeader_t;

/*
The result protocol sent from the UUT back to the P.C. Testing
Program will contain the following:
o Test-ID  – 4 Bytes (a number given to the test so it'll be easy to
map it to the later on test result).
o Test Result – 1 Byte (bitfield:  1 – test succeeded, 0xff –test
failed).
 */

typedef struct {
	uint32_t test_id;
	uint8_t  test_result;
}__attribute__((packed)) ResultPacket_t ;
volatile uint8_t flag=0;
volatile uint8_t flagUART4=0;
volatile uint8_t flagSPI4=0;
volatile uint8_t flagI2C2=0;

#define I2C_SLAVE_ADDR 0x20 <<1
uint8_t UART_test_loopback(uint8_t *data, uint16_t length, uint8_t iterations);
uint8_t I2C_test_loopback(uint8_t *data, uint16_t length, uint8_t iterations);
uint8_t SPI_test_loopback(uint8_t *data, uint16_t length, uint8_t iterations);
void user_main(void){
	/*CALLED FROM CORE/SRC/MAIN.c*/
	HAL_UART_Transmit(&huart3, (uint8_t*)"\033[2J\033[H", 7, HAL_MAX_DELAY);//clean terminal
	uint8_t sample_data[] = "TEST_PATTERN";
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
}
uint8_t UART_test_loopback(uint8_t *data, uint16_t length, uint8_t iterations){
	static uint8_t rx_buffer_x[256];
	static uint8_t rx_buffer_y[256];
	for(uint8_t i=0;i<iterations;i++){
		flag=0;
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
		while(!flag){
			if(HAL_GetTick() - start_time > 1000){ // Timeout
				return 0xff;
			}
		}
		flag=0;
		if(memcmp(data, rx_buffer_x, length)!=0){
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
		flag=1;
	}
}

uint8_t I2C_test_loopback(uint8_t *data, uint16_t length, uint8_t iterations){
	static uint8_t rx_buffer_x[256];
	static uint8_t rx_buffer_y[256];
	for(uint8_t i=0;i<iterations;i++){
		memset(rx_buffer_x, 0, length);
		memset(rx_buffer_y, 0, length);
		flag=0;
		flagUART4=0;
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
		while(!flag){
			if(HAL_GetTick() - start_time > 1000){ // Timeout
				return 0xff;
			}
		}
		if(memcmp(data, rx_buffer_x, length)!=0)
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
        flag = 1;
    }
}

uint8_t SPI_test_loopback(uint8_t *data, uint16_t length, uint8_t iterations)
{
    static uint8_t rx_buffer_x[256];
    static uint8_t rx_buffer_y[256];

    for (uint8_t i = 0; i < iterations; i++) {
        memset(rx_buffer_x, 0, length);
        memset(rx_buffer_y, 0, length);
        flag = 0;
        flagSPI4 = 0;
        HAL_StatusTypeDef status;

        // Start the transfer and catch the return value
        status = HAL_SPI_Receive_DMA(&hspi1, rx_buffer_y, length);



        if (status != HAL_OK) {
                    printf("\r\n=== SPI DMA Error Detected ===\r\n");

                    // 1. Check the immediate function return status
                    if (status == HAL_BUSY) {
                        printf("Status: HAL_BUSY (The SPI peripheral or DMA is already running a task)\r\n");
                    } else if (status == HAL_ERROR) {
                        printf("Status: HAL_ERROR (Parameter error or severe configuration issue)\r\n");
                    } else if (status == HAL_TIMEOUT) {
                        printf("Status: HAL_TIMEOUT\r\n");
                    }

                    // 2. Decode the specific internal hardware error code
                    uint32_t spi_error = HAL_SPI_GetError(&hspi1);
                    printf("SPI Error Code (Hex): 0x%08LX\r\n", spi_error);

                    // Break down the bitmask error codes
                    if (spi_error & HAL_SPI_ERROR_OVR)  printf("-> ERROR: Overrun (Data arrived before previous data was read)\r\n");
                    if (spi_error & HAL_SPI_ERROR_MODF) printf("-> ERROR: Mode Fault\r\n");
                    if (spi_error & HAL_SPI_ERROR_CRC)  printf("-> ERROR: CRC error\r\n");
                    if (spi_error & HAL_SPI_ERROR_DMA)  printf("-> ERROR: DMA transfer error\r\n");
                    if (spi_error == HAL_SPI_ERROR_NONE) printf("-> No hardware error flag set. Check your buffer pointers or lengths.\r\n");
                }
        status= HAL_SPI_Transmit_DMA(&hspi4, data, length);
        if (status != HAL_OK) {
                    printf("\r\n=== SPI DMA Error Detected ===\r\n");

                    // 1. Check the immediate function return status
                    if (status == HAL_BUSY) {
                        printf("Status: HAL_BUSY (The SPI peripheral or DMA is already running a task)\r\n");
                    } else if (status == HAL_ERROR) {
                        printf("Status: HAL_ERROR (Parameter error or severe configuration issue)\r\n");
                    } else if (status == HAL_TIMEOUT) {
                        printf("Status: HAL_TIMEOUT\r\n");
                    }

                    // 2. Decode the specific internal hardware error code
                    uint32_t spi_error = HAL_SPI_GetError(&hspi1);
                    printf("SPI Error Code (Hex): 0x%08LX\r\n", spi_error);

                    // Break down the bitmask error codes
                    if (spi_error & HAL_SPI_ERROR_OVR)  printf("-> ERROR: Overrun (Data arrived before previous data was read)\r\n");
                    if (spi_error & HAL_SPI_ERROR_MODF) printf("-> ERROR: Mode Fault\r\n");
                    if (spi_error & HAL_SPI_ERROR_CRC)  printf("-> ERROR: CRC error\r\n");
                    if (spi_error & HAL_SPI_ERROR_DMA)  printf("-> ERROR: DMA transfer error\r\n");
                    if (spi_error == HAL_SPI_ERROR_NONE) printf("-> No hardware error flag set. Check your buffer pointers or lengths.\r\n");
                }

        uint32_t t0 = HAL_GetTick();
        while (!flagSPI4) {
            if (HAL_GetTick() - t0 > 1000) return 0xff;
        }
        flagSPI4 = 0;
        status= HAL_SPI_Transmit_DMA(&hspi1, data, length);
                if (status != HAL_OK) {
                            printf("\r\n=== SPI DMA Error Detected ===\r\n");

                            // 1. Check the immediate function return status
                            if (status == HAL_BUSY) {
                                printf("Status: HAL_BUSY (The SPI peripheral or DMA is already running a task)\r\n");
                            } else if (status == HAL_ERROR) {
                                printf("Status: HAL_ERROR (Parameter error or severe configuration issue)\r\n");
                            } else if (status == HAL_TIMEOUT) {
                                printf("Status: HAL_TIMEOUT\r\n");
                            }

                            // 2. Decode the specific internal hardware error code
                            uint32_t spi_error = HAL_SPI_GetError(&hspi1);
                            printf("SPI Error Code (Hex): 0x%08LX\r\n", spi_error);

                            // Break down the bitmask error codes
                            if (spi_error & HAL_SPI_ERROR_OVR)  printf("-> ERROR: Overrun (Data arrived before previous data was read)\r\n");
                            if (spi_error & HAL_SPI_ERROR_MODF) printf("-> ERROR: Mode Fault\r\n");
                            if (spi_error & HAL_SPI_ERROR_CRC)  printf("-> ERROR: CRC error\r\n");
                            if (spi_error & HAL_SPI_ERROR_DMA)  printf("-> ERROR: DMA transfer error\r\n");
                            if (spi_error == HAL_SPI_ERROR_NONE) printf("-> No hardware error flag set. Check your buffer pointers or lengths.\r\n");
                        }


        status= HAL_SPI_Receive_DMA(&hspi4, rx_buffer_x, length);
                if (status != HAL_OK) {
                            printf("\r\n=== SPI DMA Error Detected ===\r\n");

                            // 1. Check the immediate function return status
                            if (status == HAL_BUSY) {
                                printf("Status: HAL_BUSY (The SPI peripheral or DMA is already running a task)\r\n");
                            } else if (status == HAL_ERROR) {
                                printf("Status: HAL_ERROR (Parameter error or severe configuration issue)\r\n");
                            } else if (status == HAL_TIMEOUT) {
                                printf("Status: HAL_TIMEOUT\r\n");
                            }

                            // 2. Decode the specific internal hardware error code
                            uint32_t spi_error = HAL_SPI_GetError(&hspi1);
                            printf("SPI Error Code (Hex): 0x%08LX\r\n", spi_error);

                            // Break down the bitmask error codes
                            if (spi_error & HAL_SPI_ERROR_OVR)  printf("-> ERROR: Overrun (Data arrived before previous data was read)\r\n");
                            if (spi_error & HAL_SPI_ERROR_MODF) printf("-> ERROR: Mode Fault\r\n");
                            if (spi_error & HAL_SPI_ERROR_CRC)  printf("-> ERROR: CRC error\r\n");
                            if (spi_error & HAL_SPI_ERROR_DMA)  printf("-> ERROR: DMA transfer error\r\n");
                            if (spi_error == HAL_SPI_ERROR_NONE) printf("-> No hardware error flag set. Check your buffer pointers or lengths.\r\n");
                        }
        t0 = HAL_GetTick();
        while (!flag) {
            if (HAL_GetTick() - t0 > 1000) return 0xff;
        }
        flag = 0;

        if (memcmp(data, rx_buffer_x, length) != 0) {
            return 0xff;
        }
    }
    return 0x01;
}

/* Overridden HAL Callback for Full-Duplex Transmit/Receive */
void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi) {
    if (hspi->Instance == SPI1) {
        flagSPI4 = 1;
    }
    if (hspi->Instance == SPI4) {
        flag = 1;
    }
}
void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi){
	if (hspi->Instance == SPI1) {
	        flagSPI4 = 1;
	    }
	    if (hspi->Instance == SPI4) {
	        flag = 1;
	    }
}

/*
 * spi_test.c
 *
 *  Created on: 20 בספט׳ 2026
 *      Author: Redan
 */


#include "peripheal_test.h"
#include "test_config.h"
#include "test_protocol.h"
#include "data_compare.h"
#include "dma_error_report.h"
#include "spi.h"
#include <string.h>

volatile uint8_t flagSPI4=0;
volatile uint8_t flagSPI1=0;
uint8_t SPI_test_loopback(uint8_t *data, uint16_t length, uint8_t iterations)
{
    static uint8_t rx_buffer_x[256];
    static uint8_t rx_buffer_y[256];

    for (uint8_t i = 0; i < iterations; i++) {
        memset(rx_buffer_x, 0, length);
        memset(rx_buffer_y, 0, length);
        flagSPI1 = 0;
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
        while (!flagSPI1) {
            if (HAL_GetTick() - t0 > 1000) return 0xff;
        }
        flagSPI1 = 0;

        if (!CRC_COMPARE(data, rx_buffer_x, length)) {
            return 0xff;
        }
    }
    return 0x01;
}

void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi){
	if (hspi->Instance == SPI1) {
	        flagSPI4 = 1;
	    }
	    if (hspi->Instance == SPI4) {
	        flagSPI1 = 1;
	    }
}

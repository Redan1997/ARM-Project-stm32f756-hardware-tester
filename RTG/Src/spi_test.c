/**
 * @file spi_test.c
 * @author: Redan
 * @date: 20 בספט׳ 2026
 * @brief implementation of the SPI loopback test function, using DMA for both master and slave transfers.
 * @GPIO: spi4: PE2(SCK), PE5(MISO), PE6(MOSI) - full-duplex master with 8bit data frame, CPOL=high, CPHA=2 prescaler=2 at brud rate of 18.0MHz
 *        spi1: PA5(SCK), PA6(MISO), PB5(MOSI) - full-duplex slave with 8bit data frame, CPOL=high, CPHA=2 
 */


#include "peripheal_test.h"
#include "test_config.h"
#include "test_protocol.h"
#include "data_compare.h"
#include "dma_error_report.h"
#include "spi.h"
#include <string.h>

static volatile uint8_t flagSPI4=0;
static volatile uint8_t flagSPI1=0;
uint8_t SPI_test_loopback(uint8_t *data, uint16_t length, uint8_t iterations)
{
	//SUCCESS here does not mean the peripheral was verified working nothing was tested so nothing failed
	if (iterations == 0 || length == 0|| data == NULL) {
		return TEST_RESULT_SUCCESS;
	}
	static uint8_t rx_buffer_x[TEST_MAX_PATTERN_LEN];
	static uint8_t rx_buffer_y[TEST_MAX_PATTERN_LEN];
	HAL_StatusTypeDef status;

	for (uint8_t i = 0; i < iterations; i++) {
		memset(rx_buffer_x, 0, length);
		memset(rx_buffer_y, 0, length);
		flagSPI1 = 0;
		flagSPI4 = 0;

		status = HAL_SPI_Receive_DMA(&hspi1, rx_buffer_y, length);
		if (status != HAL_OK) {
			report_dma_error(status, SPI, HAL_SPI_GetError(&hspi1));
			return TEST_RESULT_FAILURE;
		}
		status= HAL_SPI_Transmit_DMA(&hspi4, data, length);
		if (status != HAL_OK) {
			report_dma_error(status, SPI, HAL_SPI_GetError(&hspi4));
			return TEST_RESULT_FAILURE;
		}

		uint32_t t0 = HAL_GetTick();
		while (!flagSPI4) {
			if (HAL_GetTick() - t0 > TEST_TIMEOUT_MS) return TEST_RESULT_FAILURE;
		}
		flagSPI4 = 0;
		status= HAL_SPI_Transmit_DMA(&hspi1, rx_buffer_y, length);
		if (status != HAL_OK) {
			report_dma_error(status, SPI, HAL_SPI_GetError(&hspi1));
			return TEST_RESULT_FAILURE;
		}
		status= HAL_SPI_Receive_DMA(&hspi4, rx_buffer_x, length);
		if (status != HAL_OK) {
			report_dma_error(status, SPI, HAL_SPI_GetError(&hspi4));
			return TEST_RESULT_FAILURE;
		}
		t0 = HAL_GetTick();
		while (!flagSPI1) {
			if (HAL_GetTick() - t0 > TEST_TIMEOUT_MS) return TEST_RESULT_FAILURE;
		}
		flagSPI1 = 0;

		if (!CRC_COMPARE(data, rx_buffer_x, length)) {
			return TEST_RESULT_FAILURE;
		}
	}
	return TEST_RESULT_SUCCESS;
}

void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi){
	if (hspi->Instance == SPI1) {
		flagSPI4 = 1;
	}
	if (hspi->Instance == SPI4) {
		flagSPI1 = 1;
	}
}

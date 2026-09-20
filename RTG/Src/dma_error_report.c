/*
 * @file    dma_error_report.c
 * @brief   Implementation of DMA error reporting functions
 *
 */

#include "dma_error_report.h"
#include "user.h"
#include <stdio.h>

void report_dma_error(HAL_StatusTypeDef const status, TYPE_DMA_PERIPHERAL type, uint32_t const error_code)
{
    if (status == HAL_OK) {
        return;
    }
    static const char *peripheral_names[] = {
        "UART",
         "SPI",
         "I2C",
        "ADC"
    };
    printf("\r\n=== %s DMA Error Report ===\r\n", peripheral_names[type]);

    switch (status) {
        case HAL_ERROR:
            printf("Status: HAL_ERROR\r\n");
            break;
        case HAL_BUSY:
            printf("Status: HAL_BUSY\r\n");
            break;
        case HAL_TIMEOUT:
            printf("Status: HAL_TIMEOUT\r\n");
            break;
        default:
            printf("Status: Unknown (%d)\r\n", status);
            break;
    }
    printf("Error Code: 0x%08lX\r\n", error_code);

    switch (type) {
        case UART:
             if (error_code & HAL_SPI_ERROR_OVR)  printf("-> Overrun\r\n");
            if (error_code & HAL_SPI_ERROR_MODF) printf("-> Mode Fault\r\n");
            if (error_code & HAL_SPI_ERROR_CRC)  printf("-> CRC error\r\n");
            if (error_code & HAL_SPI_ERROR_DMA)  printf("-> DMA error\r\n");
            break;
        case SPI:
            if (error_code & HAL_SPI_ERROR_OVR)  printf("-> Overrun\r\n");
            if (error_code & HAL_SPI_ERROR_MODF) printf("-> Mode Fault\r\n");
            if (error_code & HAL_SPI_ERROR_CRC)  printf("-> CRC error\r\n");
            if (error_code & HAL_SPI_ERROR_DMA)  printf("-> DMA error\r\n");
            break;
        case I2C:
            if (error_code & HAL_I2C_ERROR_BERR)  printf("-> Bus error\r\n");
            if (error_code & HAL_I2C_ERROR_ARLO)  printf("-> Arbitration lost\r\n");
            if (error_code & HAL_I2C_ERROR_AF)    printf("-> Acknowledge failure\r\n");
            if (error_code & HAL_I2C_ERROR_OVR)   printf("-> Overrun/Underrun\r\n");
            if (error_code & HAL_I2C_ERROR_DMA)   printf("-> DMA error\r\n");
            break;
    }
}

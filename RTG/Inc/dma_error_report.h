/*
 * @file    dma_error_report.h
 * @brief   single, peripheral DMA error report for failure handling
 *
 */

#ifndef DMA_ERROR_REPORT_H
#define DMA_ERROR_REPORT_H
#include "main.h"

typedef enum{
    UART,
    SPI,
    I2C,
    ADc
}TYPE_DMA_PERIPHERAL;

/**
 * @brief Prints a uniform diagnostic report for a failed HAL_*_DMA call.
 *        No-op when status is HAL_OK, so it is safe to call unconditionally
 *        after every HAL_*_DMA invocation.
 * @param status     Return value of the HAL_*_DMA call.
 * @param type       Peripheral family the call belongs to.
 * @param error_code Peripheral's own error code, from HAL_*_GetError().
 */
void report_dma_error(HAL_StatusTypeDef const status, TYPE_DMA_PERIPHERAL const type, uint32_t const error_code);

#endif /* DMA_ERROR_REPORT_H */

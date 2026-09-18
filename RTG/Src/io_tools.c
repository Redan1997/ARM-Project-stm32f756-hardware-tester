/*
 * io_tools.c
 *
 *  Created on: 16 ביולי 2026
 *      Author: Redan
 */

#include "user.h"
#include "stm32f7xx_hal.h"

extern UART_HandleTypeDef huart3;

#define UART_DEBUG &huart3
// printf
int __io_putchar(int ch)
{
	HAL_UART_Transmit(UART_DEBUG, (uint8_t *)&ch, 1, 0xFFFF);
	return ch;
}

int _write(int file, char *ptr, int len)
{
	HAL_UART_Transmit(UART_DEBUG, (uint8_t *)ptr, len, 0xFFFF);
	return len;
}

// scanf
int _read(int file, char *ptr, int len) {
    int bytes_read = 0;

    while (bytes_read < len) {
        uint8_t ch = 0;

        // Receive 1 character
        if (HAL_UART_Receive(&huart3, &ch, 1, HAL_MAX_DELAY) != HAL_OK) {
            break;
        }

        // Handle Enter key (Carriage Return -> Newline translation)
        if (ch == '\r') {
            HAL_UART_Transmit(&huart3, &ch, 1, HAL_MAX_DELAY); // Echo newline
            ch = '\n';
            HAL_UART_Transmit(&huart3, &ch, 1, HAL_MAX_DELAY); // Echo newline
            *ptr++ = (char)ch;
            bytes_read++;
            break; // Break the loop so the string gets processed immediately
        }
        // Handle Backspace properly on the serial monitor
        else if (ch == '\b' || ch == 127) {
            if (bytes_read > 0) {
                HAL_UART_Transmit(&huart3, (uint8_t *)"\b \b", 3, HAL_MAX_DELAY);
                ptr--;
                bytes_read--;
            }
            continue;
        }
        // Handle all other characters (including spaces!)
        else {
            HAL_UART_Transmit(&huart3, &ch, 1, HAL_MAX_DELAY); // Echo character
            *ptr++ = (char)ch;
            bytes_read++;
        }
    }

    return bytes_read;
}

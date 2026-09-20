/*
 * @file 	data_compare.h
 * @Author: Redan Created on: 20 בספט׳ 2026
 * @brief
 */

#ifndef INC_DATA_COMPARE_H_
#define INC_DATA_COMPARE_H_
#include <stdint.h>

/*
 * @brief Compares orginal data vs. received test data from serial peripheral protocols.
 *        Uses the hardware CRC-32 peripheral for buffers larger than
 *        CRC_COMPARE_THRESHOLD(100) bytes, and a direct byte compare otherwise.
 * @param data      Pointer to the originally transmitted data.
 * @param received  Pointer to the data received back after loopback.
 * @param length    Number of bytes to compare.
 * @return 1 if the data matches, 0 on mismatch.
 */
uint8_t CRC_COMPARE(const uint8_t *data,const uint8_t *received ,uint16_t length);


#endif /* INC_DATA_COMPARE_H_ */

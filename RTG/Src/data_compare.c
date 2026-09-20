/*
 * @file 	data_compare.c
 * @Author: Redan Created on: 20 בספט׳ 2026
 * @brief 	implementation of the CRC/memcmp data comparison helper
 */

#include "data_compare.h"
#include "test_config.h"
#include "crc.h"
#include <string.h>

uint8_t CRC_COMPARE(const uint8_t *data,const uint8_t *received ,uint16_t length){
	if(length>CRC_COMPARE_THRESHOLD){
		uint32_t crc_data=HAL_CRC_Calculate(&hcrc, (uint32_t*)data, length);
		uint32_t crc_received=HAL_CRC_Calculate(&hcrc, (uint32_t*)received, length);
		return(crc_data==crc_received);
	}
	return (memcmp(data, received, length) == 0);

}

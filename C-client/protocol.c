/**
 * @file    protocol.c
 * @brief   Implementation of the peripheral name/id mapping helpers.
 */

#include "protocol.h"
#include <strings.h> /* strcasecmp c11*/

int peripheral_name_to_id(const char *name, uint8_t *id)
{
    if (strcasecmp(name, "timer") == 0)
     { *id = PERIPHERAL_TIMER; return 1; }
    if (strcasecmp(name, "uart")  == 0) 
    { *id = PERIPHERAL_UART;  return 1; }
    if (strcasecmp(name, "spi")   == 0) 
    { *id = PERIPHERAL_SPI;   return 1; }
    if (strcasecmp(name, "i2c")   == 0) 
    { *id = PERIPHERAL_I2C;   return 1; }
    if (strcasecmp(name, "adc")   == 0)
     { *id = PERIPHERAL_ADC;   return 1; }
    return 0;
}

const char *peripheral_id_to_name(uint8_t id)
{
    switch (id) {
        case PERIPHERAL_TIMER: return "TIMER";
        case PERIPHERAL_UART:  return "UART";
        case PERIPHERAL_SPI:   return "SPI";
        case PERIPHERAL_I2C:   return "I2C";
        case PERIPHERAL_ADC:   return "ADC";
        default:               return "UNKNOWN";
    }
}

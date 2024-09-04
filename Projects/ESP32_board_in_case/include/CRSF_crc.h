#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"
#include "inttypes.h"


void generate_CRC(uint8_t poly);
uint8_t crc8(const uint8_t *data, uint8_t len);

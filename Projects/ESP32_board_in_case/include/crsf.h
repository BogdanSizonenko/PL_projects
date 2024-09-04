#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "CRSF_crc.h"


typedef enum
{
    CRSF_TYPE_CHANNELS = 0x16,
    CRSF_TYPE_BATTERY = 0x08,
    CRSF_TYPE_GPS = 0x02,
    CRSF_TYPE_ALTITUDE = 0x09,
    CRSF_TYPE_ATTITUDE = 0x1E
} crsf_type_t;

typedef enum
{
    CRSF_DEST_FC = 0xC8,
    CRSF_DEST_RADIO = 0xEA
} crsf_dest_t;

bool check_CRSF_sync(uint8_t* data);
uint8_t* get_length_CRSF(uint8_t* data);
crsf_dest_t get_destination_CRSF(uint8_t* data);
crsf_type_t get_frametype_CRSF(uint8_t* data);
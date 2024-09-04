#include "CRSF_crc.h"


static uint8_t crc8_table[256] = {0};

void generate_CRC(uint8_t poly)
{
    for (int idx=0; idx<256; ++idx)
    {
        uint8_t crc = idx;
        for (int shift=0; shift<8; ++shift)
        {
            crc = (crc << 1) ^ ((crc & 0x80) ? poly : 0);
        }
        crc8_table[idx] = crc & 0xff;
    }
}

// Function to calculate CRC8 checksum
uint8_t crc8(const uint8_t *data, uint8_t len) {
    uint8_t crc = 0;
    while (len--)
    {
        crc = crc8_table[crc ^ *data++];
    }

    return crc;
}
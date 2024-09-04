#include "crsf.h"


//Проверяем байт синхронизации
bool check_CRSF_sync(uint8_t* data){
    if (data[0] == 0xc8 && data[-1] == crc8(data[2], (data[1] - 1))){
        printf('True');
        return true;
    } else {
        return false;
    };
}

//Получаем длину пакета
uint8_t* get_length_CRSF(uint8_t* data){
    return data[1];
}

//Получаем байт места назначения пакета
crsf_dest_t get_destination_CRSF(uint8_t* data){
    crsf_dest_t crsf_dest;
    for (crsf_dest = CRSF_DEST_FC; crsf_dest <= CRSF_DEST_RADIO; crsf_dest++){
        if (data[0] == crsf_dest){
            return crsf_dest;
        }
    }
}

//Получаем тип пакета CRSF
crsf_type_t get_frametype_CRSF(uint8_t* data){
    crsf_type_t crsf_type;
    for (crsf_type = CRSF_TYPE_CHANNELS; crsf_type <= CRSF_TYPE_ATTITUDE; crsf_type++){
        if(data[2] == crsf_type){
            return crsf_type;
        }
    }
} 



/*void CRSF_send_payload(const void* payload, crsf_dest_t destination, crsf_type_t type, uint8_t payload_length)
{
    uint8_t packet[payload_length+4]; //payload + dest + len + type + crc

    packet[0] = destination;
    packet[1] = payload_length+2; // size of payload + type + crc
    packet[2] = type;

    memcpy(&packet[3], payload, payload_length);

    //calculate crc
    unsigned char checksum = crc8(&packet[2], payload_length+1);
    
    packet[payload_length+3] = checksum;

    //send frame
    uart_write_bytes(UART_1, &packet, payload_length+4);
}*/
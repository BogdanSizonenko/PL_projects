#include "uart_init.h"
#include "uart_init.c"
#include "main.h"
#include "CRSF_crc.h"
#include "crsf.h"


SemaphoreHandle_t xMutex;
crsf_channels_t received_channels = {0};

static void rx_task(void *arg){
    uart_event_t event;
    uint8_t* data = (uint8_t*) malloc(RX_BUF_SIZE);

    for(;;){
        if (xQueueReceive(uart_queue, (void *)&event, (TickType_t)portMAX_DELAY)){

            bzero(data, RX_BUF_SIZE);
            if  (event.type == UART_DATA) {

                uart_read_bytes(UART_2, data, event.size, portMAX_DELAY);

                uart_write_bytes(UART_1, data, event.size);

                uint8_t type = data[2];
                uint8_t length = data[1];
                uint8_t dest = data[0];

                uint8_t payload_length = length -2;
                uint8_t payload[payload_length];

                for (int i = 0; i < payload_length; i++) {

                    payload[i] = data[i+3];
    
                }

                if (type == CRSF_TYPE_CHANNELS) {

                    xSemaphoreTake(xMutex, portMAX_DELAY);
                    received_channels = *(crsf_channels_t*)payload;
                    xSemaphoreGive(xMutex);

                }
            }
        }
    }
    free(data);
    data = NULL;
    vTaskDelete(NULL);
}

void CRSF_init(void) 
{
    generate_CRC(0xd5);

    uart_init();

    xMutex = xSemaphoreCreateMutex();

    xTaskCreate(rx_task, "uart_rx_task", 1024*4, NULL, configMAX_PRIORITIES - 1, NULL);
}

void CRSF_receive_channels(crsf_channels_t * channels) 
{
    xSemaphoreTake(xMutex, portMAX_DELAY);
    *channels = received_channels;
    xSemaphoreGive(xMutex);
}

void app_main() 
{
    crsf_channels_t channels = {0};
    CRSF_init();
    
    while (1)
    {
        CRSF_receive_channels(&channels);
        printf(">Channel 1: %d\n", channels.ch1);
        printf(">Channel 2: %d\n", channels.ch2);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
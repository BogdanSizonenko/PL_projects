#include "uart_init.h"


static const int RX_BUF_SIZE = 1024;

void uart_init(void) 
{
    uart_config_t uart_config = {
        .baud_rate = 420000,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };

    uart_param_config(UART_2, &uart_config);
    uart_set_pin(UART_2, TXD_2_PIN, RXD_2_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    ESP_ERROR_CHECK(uart_driver_install(UART_2, RX_BUF_SIZE, RX_BUF_SIZE, 10, &uart_queue, 0));

    uart_param_config(UART_1, &uart_config);
    uart_set_pin(UART_1, TXD_1_PIN, RXD_1_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    ESP_ERROR_CHECK(uart_driver_install(UART_1, RX_BUF_SIZE, RX_BUF_SIZE, 0, NULL, 0));
}
#include "uart_init.h"

static const int BUFFER_SIZE = 1024;

void uart_init(void)
{
    uart_config_t uart_config = {
        .baud_rate = 416666,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    };

    uart_param_config(UART_1, &uart_config);
    uart_set_pin(UART_1, TBS_TX_PIN, TBS_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    ESP_ERROR_CHECK(uart_driver_install(UART_1, BUFFER_SIZE, BUFFER_SIZE, 0, 0, 0));
}
#include "driver/uart.h"
#include "driver/gpio.h"
#include "freertos/queue.h"
#include "esp_log.h"


#define TBS_TX_PIN (GPIO_NUM_4)
#define TBS_RX_PIN (GPIO_NUM_5)
#define UART_1 UART_NUM_1

void uart_init(void);
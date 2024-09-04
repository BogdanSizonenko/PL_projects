#include "driver/uart.h"
#include "driver/gpio.h"
#include "freertos/queue.h"
#include <stdio.h>

static QueueHandle_t uart_queue;

#define TXD_2_PIN (GPIO_NUM_17)
#define RXD_2_PIN (GPIO_NUM_16)
#define TXD_1_PIN (GPIO_NUM_14)
#define RXD_1_PIN (GPIO_NUM_12)

#define UART_2 UART_NUM_2
#define UART_1 UART_NUM_1

void uart_init(void);
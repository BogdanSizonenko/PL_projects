#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/timers.h"
#include "esp_log.h"

#include "uart_init.h"
#include "uart_init.c"


static const char bindFrame[] = {
            0xC8,
            0x07,
            0x32,
            0xEC,
            0xC8,
            0x10,
            0x01,
            0x9E,
            0xE8,
        };

static bool powerLedFlag = false;

// Bind button 
static const gpio_num_t TBS_BIND_BUTTON = GPIO_NUM_35;
static SemaphoreHandle_t bind_semphr = NULL;
static TimerHandle_t button_timer = NULL;

//LED POWER and REC button
static const gpio_num_t POWER_REC_BUTTON = GPIO_NUM_22;
static const gpio_num_t POWER_LED_BUTTON = GPIO_NUM_34;
static const gpio_num_t REC_LED_BUTTON = GPIO_NUM_32;
static const gpio_num_t POWER_REC_LED = GPIO_NUM_33;
static const gpio_num_t OK_BUTTON = GPIO_NUM_26;
static const gpio_num_t STOP_BUTTON = GPIO_NUM_25;
static const gpio_num_t LED_BIND_BUTTON = GPIO_NUM_27;


int timeToPushButton = 1000 / portTICK_PERIOD_MS;

void buttonBindHandle(TimerHandle_t timer)
{
    gpio_intr_enable(TBS_BIND_BUTTON);
}

void buttonPowerRecHandle(TimerHandle_t timer)
{
    gpio_intr_enable(POWER_LED_BUTTON);
    gpio_intr_enable(REC_LED_BUTTON);
}

static void IRAM_ATTR isrBindButtonPress(void* arg)
{
   gpio_intr_disable(TBS_BIND_BUTTON);
   xSemaphoreGiveFromISR(bind_semphr, pdFALSE);
   xTimerStartFromISR(button_timer, pdFALSE);

}

static void IRAM_ATTR isrPowerLedButton(void* arg)
{
    gpio_intr_disable(POWER_LED_BUTTON);
}

static void IRAM_ATTR isrRecLedButton(void* arg)
{
    gpio_intr_disable(REC_LED_BUTTON);
}

static void IRAM_ATTR isrPowerRecButton(void* arg)
{
    gpio_intr_disable(POWER_REC_BUTTON);
}

void power_rec_button_init()
{
    esp_rom_gpio_pad_select_gpio(POWER_REC_BUTTON);
    ESP_ERROR_CHECK(gpio_set_direction(POWER_REC_BUTTON, GPIO_MODE_INPUT));
    ESP_ERROR_CHECK(gpio_pullup_en(POWER_REC_BUTTON));
    ESP_ERROR_CHECK(gpio_set_intr_type(POWER_REC_BUTTON, GPIO_INTR_POSEDGE));
    ESP_ERROR_CHECK(gpio_isr_handler_add(POWER_REC_BUTTON, isrPowerRecButton, (void *)POWER_REC_BUTTON));
}

void power_button_init(void)
{
    esp_rom_gpio_pad_select_gpio(POWER_LED_BUTTON);
    ESP_ERROR_CHECK(gpio_set_direction(POWER_LED_BUTTON, GPIO_MODE_INPUT));
    ESP_ERROR_CHECK(gpio_pulldown_en(POWER_LED_BUTTON));
    ESP_ERROR_CHECK(gpio_set_intr_type(POWER_LED_BUTTON, GPIO_INTR_POSEDGE));
    ESP_ERROR_CHECK(gpio_isr_handler_add(POWER_LED_BUTTON, isrPowerLedButton, (void *)POWER_LED_BUTTON));
}

void rec_button_init(void)
{
    esp_rom_gpio_pad_select_gpio(REC_LED_BUTTON);
    ESP_ERROR_CHECK(gpio_set_direction(REC_LED_BUTTON, GPIO_MODE_INPUT));
    ESP_ERROR_CHECK(gpio_pulldown_en(REC_LED_BUTTON));
    ESP_ERROR_CHECK(gpio_set_intr_type(REC_LED_BUTTON, GPIO_INTR_POSEDGE));
    ESP_ERROR_CHECK(gpio_isr_handler_add(REC_LED_BUTTON, isrRecLedButton, (void *)REC_LED_BUTTON));
}

void ok_button_init(void)
{
    //ESP_LOGI("OK_button", "OK button init in");
    esp_rom_gpio_pad_select_gpio(OK_BUTTON);
    gpio_set_direction(OK_BUTTON, GPIO_MODE_OUTPUT);
    gpio_set_level(OK_BUTTON, 0);
    //ESP_LOGI("OK_button", "OK button init out");
}

void stop_button_init(void)
{
    //ESP_LOGI("STOP_button", "STOP button init in");
    esp_rom_gpio_pad_select_gpio(STOP_BUTTON);
    gpio_set_direction(STOP_BUTTON, GPIO_MODE_OUTPUT);
    gpio_set_level(STOP_BUTTON, 1);
    //ESP_LOGI("STOP_button", "STOP button init out");
}

void power_rec_led_init(void)
{
    //ESP_LOGI("rec", "rec input");
    esp_rom_gpio_pad_select_gpio(POWER_REC_LED);
    gpio_set_direction(POWER_REC_LED, GPIO_MODE_OUTPUT);
    gpio_set_level(POWER_REC_LED, 0);
    //ESP_LOGI("rec", "rec output");
}

void led_bind_button_init(void)
{
    esp_rom_gpio_pad_select_gpio(LED_BIND_BUTTON);
    gpio_set_direction(LED_BIND_BUTTON, GPIO_MODE_OUTPUT);
    gpio_set_level(LED_BIND_BUTTON, 1);
}

void bind_button_init(void)
{
    //ESP_LOGI("BIND_button", "BIND button init in");
    esp_rom_gpio_pad_select_gpio(TBS_BIND_BUTTON);
    ESP_ERROR_CHECK(gpio_set_direction(TBS_BIND_BUTTON, GPIO_MODE_INPUT));
    ESP_ERROR_CHECK(gpio_pulldown_en(TBS_BIND_BUTTON));
    ESP_ERROR_CHECK(gpio_set_intr_type(TBS_BIND_BUTTON, GPIO_INTR_POSEDGE));
    ESP_ERROR_CHECK(gpio_install_isr_service(0));
    ESP_ERROR_CHECK(gpio_isr_handler_add(TBS_BIND_BUTTON, isrBindButtonPress, (void *)TBS_BIND_BUTTON));
    //ESP_LOGI("BIND_button", "BIND button init out");
}


void send_bind_packet(void)
{   
    uart_write_bytes(UART_1, bindFrame, 9);
}

void bindTask(void *pvParams)
{
    for(;;)
    {
        if (xSemaphoreTake(bind_semphr, portMAX_DELAY) == pdTRUE)
        {
            //ESP_LOGI("BIND button", "Try send bind packet");
            send_bind_packet();
            //ESP_LOGI("BIND button", "Send bind packet after pull BIND button");
        }
    }
}

void powerLedTask(void *pvParams)
{
    for(;;)
    {
        if (gpio_get_level(POWER_LED_BUTTON) == 0 && powerLedFlag == false)
        {
            ESP_LOGI("POWER_LED", "POWER ON");
            gpio_set_level(POWER_REC_LED, 1);
            powerLedFlag = true;
        }
        if (gpio_get_level(POWER_LED_BUTTON) == 1)
        {
            gpio_set_level(POWER_REC_LED, 0);
        }
    }
}

void recLedTask(void *pvParams)
{
    for(;;)
    {
        if (gpio_get_level(REC_LED_BUTTON) == 1 && gpio_get_level(POWER_LED_BUTTON) == 0)
        {
            ESP_LOGI("REC_LED", "REC ON");
            gpio_set_level(POWER_REC_LED, 0);
            powerLedFlag = true;
        }else{
            powerLedFlag = false;
        }
    }
}

void powerRecButtonTask(void *pvParams){
    for(;;)
    {
        if (gpio_get_level(POWER_REC_BUTTON) == 1)
        {
            gpio_set_level(OK_BUTTON, 1);
            gpio_set_level(STOP_BUTTON, 0);
        } else{
            gpio_set_level(STOP_BUTTON, 1);
            gpio_set_level(OK_BUTTON, 0);
        }
    }
}

void bindMain(void)
{
    bind_semphr = xSemaphoreCreateBinary();
    button_timer = xTimerCreate(
        "Button timer",
        timeToPushButton,
        pdFALSE,
        NULL,
        buttonBindHandle);
    xTaskCreate(bindTask, "BindTask", 3000, NULL, 1, NULL);
}

void powerRecLedMain(void)
{
    button_timer = xTimerCreate(
        "Button timer",
        timeToPushButton,
        pdFALSE,
        NULL,
        buttonPowerRecHandle);
    xTaskCreate(powerLedTask, "powerLedTask", 3000, NULL, 1, NULL);
    xTaskCreate(recLedTask, "recLedTask", 3000, NULL, 1, NULL);
}

void powerRecButtonMain(void)
{
    xTaskCreate(powerRecButtonTask, "powerRecButtonTask", 3000, NULL, 1, NULL);
}

void init_funcs(void)
{
    uart_init();
    led_bind_button_init();
    bind_button_init();
    power_button_init();
    rec_button_init();
    power_rec_led_init();
    power_rec_button_init();
    ok_button_init();
    stop_button_init();
}

void app_main() 
{   
    init_funcs();
    powerRecLedMain();
    powerRecButtonMain();
    bindMain();
}
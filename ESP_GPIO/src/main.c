#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include <esp_log.h>
#include "esp_timer.h"

#define RLE_PIN GPIO_NUM_8
#define READ_PIN GPIO_NUM_6

volatile bool isInterrupted = 0;
void readTask(void *pvParameter) {
    int level = gpio_get_level(READ_PIN);
    int prev_level = !level;
    while (1) {
        int level = gpio_get_level(READ_PIN);
        if(level != prev_level) {
            ESP_LOGI("read","Lettura da task pin %d", level);
            prev_level = level;
        }
        vTaskDelay(pdMS_TO_TICKS(250));

    }
}
static void interruptManager(void *pvParameter) {
    ESP_LOGI("interrupt","Interrupt");
    // delete task
    vTaskDelete(NULL);
}
static void IRAM_ATTR  button_isr_handler() {
    xTaskCreate(&interruptManager, "interruptManager", 2048, NULL, 5, NULL);
   
}


void app_main(void) {
    // Configurazione del pin come output
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL << RLE_PIN);
    io_conf.pull_down_en = GPIO_PULLDOWN_ENABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&io_conf);

    // Configurazione del pin come input
    //io_conf.intr_type = GPIO_INTR_DISABLE;
     io_conf.intr_type = GPIO_INTR_ANYEDGE ;
     io_conf.mode = GPIO_MODE_INPUT;
     io_conf.pin_bit_mask = (1ULL << READ_PIN);
     io_conf.pull_down_en = GPIO_PULLDOWN_ENABLE;
     io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
     gpio_config(&io_conf);
     gpio_install_isr_service(ESP_INTR_FLAG_IRAM);
     gpio_isr_handler_add(READ_PIN, button_isr_handler, NULL);
    //configurazione del pin con isr


    uint32_t _level = 0; 
    //gpio low
    gpio_set_level(RLE_PIN, _level);

    xTaskCreate(&readTask, "readTask", 2048, NULL, 5, NULL);
    


    // Loop principale
    while (1) {
        // Attende 1 secondo prima di ripetere il loop
       // int level = gpio_get_level(READ_PIN);
       // ESP_LOGI("read","Lettura pin %d", level);
        vTaskDelay(pdMS_TO_TICKS(1000));

        _level = !_level;
        gpio_set_level(RLE_PIN, _level);
    }
}
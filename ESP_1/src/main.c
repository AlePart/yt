#include <esp_log.h>
#include <freertos/FreeRTOS.h>

static void task(void *pvParameter) {
    while (1) {
        ESP_LOGI("task", "Hello World!");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void app_main() {

    ESP_LOGW("main", "Hello World!");
    xTaskCreate(task, "task", 2048, NULL, 5, NULL);
    while (1) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

}
#include <stdio.h>
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_spiffs.h"
#include "esp_err.h"

void init_nvs() {
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
}

void increment_counter() {
    nvs_handle_t my_handle;
    esp_err_t err;

    err = nvs_open("storage", NVS_READWRITE, &my_handle);
    if (err != ESP_OK) {
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
        return;
    }

    //read data from nvs
    int32_t value = 0;
    err = nvs_get_i32(my_handle, "key", &value);
    if(err == ESP_OK) {
        printf("The value is %d\n", (int)value);
    } else {
        printf("The value is not initialized yet!\n");
    }

    value++;
    err = nvs_set_i32(my_handle, "key", value);
    printf((err != ESP_OK) ? "Failed to write!\n" : "Value written to NVS\n");

    

    err = nvs_commit(my_handle);
    printf((err != ESP_OK) ? "Failed to commit!\n" : "NVS committed\n");

    err = nvs_get_i32(my_handle, "key", &value);
    if(err == ESP_OK) {
        printf("The value is %d\n", (int)value);
    } else {
        printf("The value is not initialized yet!\n");
    }

    nvs_close(my_handle);
}


void init_spiffs() {
    esp_vfs_spiffs_conf_t conf = {
        .base_path = "/spiffs",
        .partition_label = NULL,
        .max_files = 5,
        .format_if_mount_failed = true
    };

    esp_err_t ret = esp_vfs_spiffs_register(&conf);
    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            printf("Failed to mount or format filesystem\n");
        } else if (ret == ESP_ERR_NOT_FOUND) {
            printf("Failed to find SPIFFS partition\n");
        } else {
            printf("Failed to initialize SPIFFS (%s)\n", esp_err_to_name(ret));
        }
        return;
    }

    size_t total = 0, used = 0;
    ret = esp_spiffs_info(NULL, &total, &used);
    if (ret != ESP_OK) {
        printf("Failed to get SPIFFS partition information (%s)\n", esp_err_to_name(ret));
    } else {
        printf("Partition size: total: %d, used: %d\n", total, used);
    }
}

void write_to_spiffs() {
    FILE* f = fopen("/spiffs/hello.txt", "w");
    if (f == NULL) {
        printf("Failed to open file for writing\n");
        return;
    }
    fprintf(f, "Qualsiasi cosa!\n");
    fclose(f);
    printf("File written to SPIFFS\n");
}

void read_from_spiffs() {
    FILE* f = fopen("/spiffs/hello.txt", "r");
    if (f == NULL) {
        printf("Failed to open file for reading\n");
        return;
    }
    char line[64];
    fgets(line, sizeof(line), f);
    fclose(f);
    printf("Read from file: '%s'\n", line);
}

void app_main(void) {
    init_nvs();
    increment_counter();
    

     init_spiffs();
     write_to_spiffs();
     read_from_spiffs();
}

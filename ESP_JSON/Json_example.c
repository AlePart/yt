#include <stdio.h>
#include "esp_log.h"
#include "esp_spiffs.h"
#include "esp_err.h"
#include <cJSON.h>
#include <string.h>

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

void write_json(const char *path, const cJSON* json) {
    char* json_str = cJSON_Print(json);
    FILE* f = fopen(path, "w");
    if (f == NULL) {
        printf("Failed to open file for writing\n");
        free(json_str);
        return;
    }
    fwrite(json_str, 1, strlen(json_str), f);
    fclose(f);
    free(json_str);  // Free the allocated memory for json_str
}

cJSON* read_file(const char* path) {
    FILE* f = fopen(path, "r");
    if (f == NULL) {
        return NULL;
    }
    fseek(f, 0, SEEK_END);
    size_t file_size = ftell(f); // Get the file size
    rewind(f); // Set the file pointer back to the start

    char* buf = malloc(file_size + 1); // Allocate memory with extra space for null-terminator
    if (buf == NULL) {
        fclose(f);
        return NULL;
    }
    fread(buf, 1, file_size, f);
    buf[file_size] = '\0'; // Null-terminate the string
    cJSON* json = cJSON_Parse(buf);
    free(buf);
    fclose(f);
    return json;
}

void delete_file(const char* path) {
    remove(path);
}

cJSON* add_son_daughter(cJSON* family, const char* name, int age) {
    cJSON* children = cJSON_GetObjectItem(family, "children");
    if (children == NULL) {
        children = cJSON_AddArrayToObject(family, "children");
    }
    cJSON* child = cJSON_CreateObject();
    cJSON_AddItemToArray(children, child);
    cJSON_AddStringToObject(child, "name", name);
    cJSON_AddNumberToObject(child, "age", age);

    return children;
}

cJSON* createJson() {
    cJSON* family = cJSON_CreateObject();
    cJSON* father = cJSON_CreateObject();
    cJSON* mother = cJSON_CreateObject();

    cJSON_AddStringToObject(father, "name", "jose");
    cJSON_AddNumberToObject(father, "age", 50);

    cJSON_AddStringToObject(mother, "name", "maria");
    cJSON_AddNumberToObject(mother, "age", 45);

    cJSON_AddItemToObject(family, "father", father);
    cJSON_AddItemToObject(family, "mother", mother);

    add_son_daughter(family, "jose jr", 25);
    add_son_daughter(family, "maria jr", 20);

    return family;
}

void app_main(void) {
    init_spiffs();
    cJSON* data = read_file("/spiffs/data.json");
    if (data == NULL) {
        ESP_LOGW("JSON", "FILE NOT FOUND");
        data = createJson();
        cJSON* element = cJSON_GetObjectItem(data, "father");
        cJSON* age = cJSON_GetObjectItem(element, "age");
        cJSON_SetNumberValue(age, 51);
        write_json("/spiffs/data.json", data);
    } else {
        ESP_LOGI("JSON", "FILE FOUND");
        printf("%s\n", cJSON_Print(data));

        delete_file("/spiffs/data.json");
    }
    // Clean up
    if (data) {
        cJSON_Delete(data);
    }
}

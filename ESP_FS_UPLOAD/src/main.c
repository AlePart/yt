#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_mac.h"
#include <mongoose.h>
#include "esp_spiffs.h"


#define EXAMPLE_ESP_WIFI_SSID      "test"
#define EXAMPLE_ESP_WIFI_PASS      "12345678"
#define EXAMPLE_MAX_STA_CONN       4

static const char *TAG = "wifi_ap";


char* get_mac_address()
{
    uint8_t base_mac_addr[6] = {0};
    esp_read_mac(base_mac_addr, ESP_MAC_WIFI_STA);
    char* mac = (char*)malloc(18);
    sprintf(mac, "%02X:%02X:%02X:%02X:%02X:%02X", base_mac_addr[0], base_mac_addr[1], base_mac_addr[2], base_mac_addr[3], base_mac_addr[4], base_mac_addr[5]);
    return mac;
}

void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    if (event_id == WIFI_EVENT_AP_STACONNECTED) {
        wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;
        ESP_LOGI(TAG, "Device connected: %s", get_mac_address());
    } else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
        wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*) event_data;
        ESP_LOGI(TAG, "Device disconnected: %s", get_mac_address());
    }
}

void wifi_init_softap(void)
{
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_ap();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        NULL));

    wifi_config_t wifi_config = {
        .ap = {
            .ssid = EXAMPLE_ESP_WIFI_SSID,
            .ssid_len = strlen(EXAMPLE_ESP_WIFI_SSID),
          //  .channel = EXAMPLE_ESP_WIFI_CHANNEL,
            .password = EXAMPLE_ESP_WIFI_PASS,
            .max_connection = EXAMPLE_MAX_STA_CONN,
#ifdef CONFIG_ESP_WIFI_SOFTAP_SAE_SUPPORT
            .authmode = WIFI_AUTH_WPA3_PSK,
            .sae_pwe_h2e = WPA3_SAE_PWE_BOTH,
#else /* CONFIG_ESP_WIFI_SOFTAP_SAE_SUPPORT */
            .authmode = WIFI_AUTH_WPA2_PSK,
#endif
            .pmf_cfg = {
                    .required = true,
            },
        },
    };
    if (strlen(EXAMPLE_ESP_WIFI_PASS) == 0) {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "wifi_init_softap finished. SSID:%s password:%s",
             EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS);
}
void init_spiffs()
{
    esp_vfs_spiffs_conf_t conf = {
      .base_path = "/spiffs",
      .partition_label = NULL,
      .max_files = 5,
      .format_if_mount_failed = true
    };
    esp_err_t ret = esp_vfs_spiffs_register(&conf);
    if (ret != ESP_OK) {
      if (ret == ESP_FAIL) {
        ESP_LOGE(TAG, "Failed to mount or format filesystem");
      } else if (ret == ESP_ERR_NOT_FOUND) {
        ESP_LOGE(TAG, "Failed to find SPIFFS partition");
      } else {
        ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
      }
      return;
    }
    size_t total = 0, used = 0;
    ret = esp_spiffs_info(NULL, &total, &used);
    if (ret != ESP_OK) {
      ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret));
    } else {
      ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
    }
}
void init_wifi()
{
        esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_LOGI(TAG, "ESP_WIFI_MODE_AP");
    wifi_init_softap();
}


char* spiffs_get_file_content(const char* path)
{
    FILE* f = fopen(path, "r");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open file %s", path);
        return NULL;
    }
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);
    char* content = (char*)malloc(fsize + 1);
    fread(content, 1, fsize, f);
    fclose(f);
    content[fsize] = 0;
    return content;

}


static void ev_handler(struct mg_connection *nc, int ev, void *ev_data) {
    if (ev == MG_EV_HTTP_MSG) {
        
    struct mg_http_message *hm = (struct mg_http_message *) ev_data;  
    if (mg_match(hm->uri, mg_str("/mac"), NULL)) {              
      mg_http_reply(nc, 200, "", get_mac_address());
    }
    else if (mg_match(hm->uri, mg_str("/hello"), NULL)) {              
      mg_http_reply(nc, 200, "", "Hello World");
    } 
    else if (mg_match(hm->uri, mg_str("/"), NULL)) {  

        char* content = spiffs_get_file_content("/spiffs/index.html");
        if (content != NULL) {
            mg_http_reply(nc, 200, "", content);
            free(content);
        } else {
            mg_http_reply(nc, 404, "", "404 Not Found");
        }
    }
    else {
      //404
      mg_http_reply(nc, 404, "", "404 Not Found");           
    }
    }
}

void start_webserver()
{
    struct mg_mgr mgr;
    struct mg_connection *nc;

    mg_mgr_init(&mgr);
    nc=mg_http_listen(&mgr, "http://0.0.0.0", ev_handler, NULL);  
    if (nc == NULL) {
        ESP_LOGE(TAG, "Failed to create listener");
        return;
    }

    ESP_LOGI(TAG, "Starting web server on port %s", "80");

    while (1) {
        mg_mgr_poll(&mgr, 1000);
    }
    mg_mgr_free(&mgr);
}

void app_main(void)
{
    init_wifi();
    init_spiffs();
    xTaskCreate(&start_webserver, "start_webserver", 4096, NULL, 5, NULL);

}


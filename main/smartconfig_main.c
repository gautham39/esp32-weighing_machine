#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_wpa2.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_netif.h"
#include "esp_smartconfig.h"
#include "smartconfig_tasks.h"
#include"driver/uart.h"
#include "freertos/semphr.h"
#include "mqtt_client.h"
#include "mqtt_events.h"
#include "driver/gpio.h"
#include "cJSON.h"
#include "hx711.h"
#include <math.h>


#define BUF_SIZE (2048)
#define UART_RX_BUF_SIZE 1024
#define ECHO_UART_PORT_NUM      (UART_NUM_0)
#define ECHO_UART_BAUD_RATE     (115200)
float reference_weight;
float current_weight;
 int s_retry_num2 = 0;
#define times 10
 EventGroupHandle_t s_wifi_event_group;

 const int CONNECTED_BIT = BIT0;
 const int ESPTOUCH_DONE_BIT = BIT1;
 const int  WIFI_FAIL_BIT = BIT2;

 const char *TAG2 = "hx711-example";
nvs_handle_t nvs_storage;
TaskHandle_t xHandle = NULL;
    
 long hx711_read();

 void initialise_wifi(void)
{
    ESP_ERROR_CHECK(esp_netif_init());
   
    s_wifi_event_group = xEventGroupCreate();
   
    ESP_ERROR_CHECK(esp_event_loop_create_default());
   
    esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta();
    
    assert(sta_netif);

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );

    ESP_ERROR_CHECK( esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL) );
    ESP_ERROR_CHECK( esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL) );
    ESP_ERROR_CHECK( esp_event_handler_register(SC_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL) );

    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK( esp_wifi_start() );
}


void app_main(void)
{
   

    ESP_ERROR_CHECK( nvs_flash_init());
     uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    };
    uart_param_config(UART_NUM_0, &uart_config);
    uart_set_pin(UART_NUM_0, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(UART_NUM_0, BUF_SIZE * 2, BUF_SIZE * 2,0,NULL ,0);
    
/* initialize the wifi and set station mode */
    initialise_wifi();

    /*check if any credentials are stored and try to connect with it */
    esp_err_t err = nvs_open("wifi_storage", NVS_READONLY, &nvs_storage);
    
    if (err == ESP_OK) {
        ESP_LOGE(TAG2, "IN NVS OPEN  ");

        char ssid1[33];
            char password1[65];

        size_t ssid_len = sizeof(ssid1);
        size_t password_len = sizeof(password1);
        ESP_LOGE(TAG2, "BEFORE SSID AND PWD COPY ");

        err = nvs_get_str(nvs_storage, "ssid", ssid1, &ssid_len);
        err = nvs_get_str(nvs_storage, "password", password1, &password_len);
        ESP_LOGE(TAG2, "BEFORE NVS CLOSE  ");

        nvs_close(nvs_storage);

        if (err == ESP_OK) {
            ESP_LOGE(TAG2, "BEFORE WIFI CONFIG");

            wifi_config_t wifi_config = {0};
            strncpy((char*)wifi_config.sta.ssid, ssid1, 32);
            strncpy((char*)wifi_config.sta.password, password1, 64);
                ESP_LOGE(TAG2, "BEFORE TRYING TO CONNECT ");

            esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
            esp_wifi_connect();
            vTaskDelay(250 / portTICK_PERIOD_MS);
            xTaskCreate(smartconfig_task, "smartconfig_task", 4096, NULL, 3, NULL);

        } else {  // Start SmartConfig if no stored credential

            
            xTaskCreate(smartconfig_task, "smartconfig_task", 4096, NULL, 3, NULL);

            ESP_LOGE(TAG2, "NO DATA STORED  ");

              
        }
    }
    else {
          ESP_LOGE(TAG2, "Error opening NVS: %s", esp_err_to_name(err));
          xTaskCreate(smartconfig_task, "start_smartconfig", 4096, NULL, 1, &xHandle);
        return;

    }


}
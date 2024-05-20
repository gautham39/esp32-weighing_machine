#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"
#include "esp_log.h"
#include "mqtt_client.h"
#include "mqtt_events.h"
#include "smartconfig_tasks.h"
#include "sdkconfig.h"
#include "esp_smartconfig.h"
#include "cJSON.h"
#include "esp_ota_ops.h"
#include "esp_http_client.h"
#include "esp_https_ota.h"
#include "string.h"
#include "fota.h"
#include "hx711.h"
#include <math.h>


#define TARE_INTERVAL 11000 // Tare every 60 seconds (adjust as needed)

/*declaration of variables*/
char tx_buff[250]; 
char *TAG = "MQTT_EXAMPLE";
int32_t zero_offset = 45400;
float calibration_factor = 65.18;

#define BLINK_GPIO GPIO_NUM_2
#define THRESHOLD 50.0f
esp_mqtt_client_handle_t client1 ;
float last_weight=0.0;
extern float current_weight;
extern const char *TAG2 ;
TickType_t last_tare_time;
int count=0;
float weight_in_grams;
extern TaskHandle_t xHandle ;



void  mqtt_event_handler();

 void log_error_if_nonzero(const char *message, int error_code)
{
    if (error_code != 0) {
        ESP_LOGE(TAG, "Last error %s: 0x%x", message, error_code);
    }
}


void tare_check() 
    {

        if (count>60  && weight_in_grams<5) 
        {
         // Update last tare time
            ESP_LOGI(TAG, "Re-taring...");
            count=0;
            HX711_tare(); // Re-tare with 10 readings
        }
       

    }



/*set the brocker and start connection  of mqtt*/
void  mqtt_app_start();

 void mqtt_app_start(void)
{
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = "mqtt://test.mosquitto.org:1883"
    };

    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    /* The last argument may be used to pass data to the event handler, in this example mqtt_event_handler */
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    
    esp_mqtt_client_start(client);
    
   
    /*read weight value check if it increased or decreased */

}

void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)

{
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%" PRIi32 "", base, event_id);
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;
    client1 = client ;

    int msg_id;
    switch ((esp_mqtt_event_id_t)event_id) 
    {
    
    /*initialize a pub and sub topic to tx and rx data */
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
        msg_id = esp_mqtt_client_publish(client, "/esp/led_status", "device connected ", 0, 0, 0);
        ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);

        msg_id = esp_mqtt_client_subscribe(client, "esp32/led/control", 1);
        ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);
        

    while (true) {
        tare_check();
        int32_t raw_data = HX711_read_average(10);

        if (raw_data < 0) 
        {
            ESP_LOGE(TAG2, "HX711 read error");
        } else 
        
        {

             weight_in_grams = hx711_to_grams(raw_data, zero_offset, calibration_factor);
             if(weight_in_grams<0)
            {
                weight_in_grams=0;
            }
            
            current_weight = weight_in_grams;
            ESP_LOGI(TAG2, "Weight : %.2f grams", weight_in_grams);
           
        }
            char buffer[100];
    
            if (fabs(current_weight - last_weight) >THRESHOLD) 
        {
            if (current_weight > last_weight && current_weight>0) {
                ESP_LOGI(TAG, "Weight increased by more than 50 grams. Current weight: %.2f grams", current_weight);
                sprintf(buffer, "weight increased , latest weight   is %.2f  grams \n", current_weight);
                esp_mqtt_client_publish(client1, "/esp/led_status", buffer, 0, 0, 0);
                ESP_LOGI("MonitoringTask", "Published alert to MQTT /n");
                memset(buffer,'\0',sizeof(buffer));
            
            } 
            else
            {

                sprintf(buffer, "weight decreased , latest weight   is %.2f  grams \n", current_weight);
                esp_mqtt_client_publish(client1, "/esp/led_status", buffer, 0, 0, 0);
                ESP_LOGI("MonitoringTask", "Published alert to MQTT /n");
                memset(buffer,'\0',sizeof(buffer));
                ESP_LOGI(TAG, "Weight decreased by more than 50 grams. Current weight: %.2f grams", current_weight);
            }
        }

        // Update the last recorded weight
        last_weight = current_weight;
        
        vTaskDelay(pdMS_TO_TICKS(100));  // Wait 100 millisecond between readings
        count=count+1;
    }


        break;

        
    case MQTT_EVENT_DISCONNECTED: 
        ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        break;

    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
            break;

    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG, "MQTT_EVENT_DATA");

    /* Set the GPIO as a push/pull output */
        gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);

/*print the received data from client*/
        printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
        printf("DATA=%.*s\r\n", event->data_len, event->data);


/*parse cJSON data*/
        cJSON *json = cJSON_Parse(event->data); 
            if (json == NULL) { 
                const char *error_ptr = cJSON_GetErrorPtr(); 
            if (error_ptr != NULL) { 
                printf("Error: %s\n", error_ptr); 
            } 
            cJSON_Delete(json); 
            return ; 
            } 

        const cJSON *led = cJSON_GetObjectItemCaseSensitive(json, "LED");
        const cJSON *smartconfig = cJSON_GetObjectItemCaseSensitive(json, "smartconfig");
        const cJSON *update = cJSON_GetObjectItemCaseSensitive(json, "update");
    
    if (cJSON_IsString(led) && (led->valuestring != NULL)) {
        printf("led: %s\n", led->valuestring);
    }
       

/*check what command has been received */
        if (cJSON_IsString(led) && (led->valuestring != NULL)) {

        if (strncmp(led->valuestring, "on", strlen(led->valuestring)) == 0) {
                    gpio_set_level(BLINK_GPIO, 1);// Turn LED on
                    esp_mqtt_client_publish(client, "/esp/led_status", "LED is ON \n", 0, 0, 0);
                    printf("LED is ON\n");
                    break;
                    }

        else if (strncmp(led->valuestring, "off", strlen(led->valuestring)) == 0) {
                        gpio_set_level(BLINK_GPIO,0 );// Turn LED off
                        esp_mqtt_client_publish(client, "/esp/led_status", "LED is OFF\n", 0, 0, 0);
                         printf("LED is OFF\n");
                         break;
                    }
        }
            
            /*smartconfig command */
        if (cJSON_IsString(smartconfig) && (smartconfig->valuestring != NULL)) {
    
            if (strncmp(smartconfig->valuestring, "start", strlen(smartconfig->valuestring)) == 0) {
                    esp_mqtt_client_unsubscribe(client, "esp32/led/control");
                    esp_mqtt_client_disconnect(client);
                    vTaskDelay(pdMS_TO_TICKS(20)); // 500 ms delay
                    esp_wifi_disconnect();
                    esp_smartconfig_stop();
                    esp_wifi_stop();
                    esp_wifi_start();
                    ESP_ERROR_CHECK(nvs_flash_erase());
                    nvs_flash_init();
                    vTaskDelete(xHandle);
                    vTaskDelay(pdMS_TO_TICKS(200)); // 500 ms delay
                    smartconfig_task();
             }
        }
        
        /*update command*/
        if (cJSON_IsString(update) && (update->valuestring != NULL)) {
                
                xTaskCreate(&simple_ota_example_task, "ota_example_task", 8192, update->valuestring, 5, NULL);   
            }
        break;
    
    default:
        ESP_LOGI(TAG, "Other event id:%d", event->event_id);
        break;
    }
}



#include <inttypes.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "driver/gpio.h"
#include "esp_smartconfig.h"
#include "smartconfig_tasks.h"
#include "hx711.h"

#define HX711_DOUT_PIN 18
#define HX711_PD_SCK_PIN 19

int32_t data;
extern int32_t zero_offset;
extern  float calibration_factor ;
extern  int32_t raw_data;
extern const char *TAG2 ;
static unsigned long OFFSET = 0;	// used for tare weight

long hx711_read() {

    gpio_set_direction(HX711_DOUT_PIN, GPIO_MODE_INPUT);
    gpio_set_direction(HX711_PD_SCK_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(HX711_PD_SCK_PIN, 0); // Initial clock state low
    long count = 0;
    // Wait for DOUT to go low, indicating data is ready
    while (gpio_get_level(HX711_DOUT_PIN) == 1);

    // Read 24 bits
    for (int i = 0; i < 24; i++) {
        gpio_set_level(HX711_PD_SCK_PIN, 1);
        count = (count << 1) | gpio_get_level(HX711_DOUT_PIN);
        gpio_set_level(HX711_PD_SCK_PIN, 0);
    }

    // Send additional clock pulse to set the gain to 128
    gpio_set_level(HX711_PD_SCK_PIN, 1);
    gpio_set_level(HX711_PD_SCK_PIN, 0);

    // Convert the 24-bit value to a signed 32-bit integer
    if (count & 0x800000) { // If the 24th bit is set, it's a negative value
        count |= ~((1 << 24) - 1); // Sign extension
    }

    return count;
}

float hx711_to_grams(int32_t raw_data, int32_t zero_offset, float calibration_factor) {
    
    int32_t adjusted_data = raw_data - zero_offset;
    return adjusted_data / calibration_factor;
}


void HX711_set_offset(unsigned long offset)
 {
    	ESP_LOGI(TAG2, "==========changed offset===========");

	zero_offset = offset;
}

unsigned long  HX711_read_average(char times) 
{
	unsigned long sum = 0;
	for (char i = 0; i < 10; i++) 
	{
		sum += hx711_read();   /*  hx711_to_grams(raw_data, zero_offset, calibration_factor);*/
	}
	unsigned long avg = sum/10;
    return sum / 10;
}


void HX711_tare() 
{
	ESP_LOGI(TAG2, "===================== START TARE ====================");
	unsigned long sum = 0; 
	sum = hx711_read();
	HX711_set_offset(sum);
	ESP_LOGI(TAG2, "===================== END TARE: %ld ====================",sum);
}

 






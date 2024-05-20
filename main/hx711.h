
#define HX711_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/event_groups.h"
#include "driver/gpio.h"

unsigned long  HX711_read_average(char times);

void HX711_set_offset(unsigned long offset);

void HX711_tare();

long hx711_read();

float hx711_to_grams(int32_t raw_data, int32_t zero_offset, float calibration_factor);

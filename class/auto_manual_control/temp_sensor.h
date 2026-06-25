#ifndef TEMP_SENSOR_H
#define TEMP_SENSOR_H

#include <Arduino.h>

#define TEMP_SENSOR_PIN 10  // GPIO10，DS18B20数据引脚

typedef enum {
    TEMP_VERY_LOW = 0,
    TEMP_LOW = 1,
    TEMP_NORMAL = 2,
    TEMP_HIGH = 3,
    TEMP_VERY_HIGH = 4
} TempLevel;

void temp_sensor_init(void);
float temp_sensor_read_celsius(void);
TempLevel temp_sensor_get_level(float temp);
const char* temp_sensor_get_level_string(TempLevel level);

#endif
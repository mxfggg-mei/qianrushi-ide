#ifndef TEMP_SENSOR_H
#define TEMP_SENSOR_H

#include <Arduino.h>

// 温度传感器引脚定义
#define TEMP_SENSOR_PIN 10  // GPIO10，DS18B20数据引脚

// 温度状态等级定义
typedef enum {
    TEMP_VERY_LOW = 0,   // 非常低
    TEMP_LOW = 1,        // 低
    TEMP_NORMAL = 2,     // 正常
    TEMP_HIGH = 3,       // 高
    TEMP_VERY_HIGH = 4   // 非常高
} TempLevel;

// 函数声明
void temp_sensor_init(void);
float temp_sensor_read_celsius(void);
float temp_sensor_read_fahrenheit(void);
TempLevel temp_sensor_get_level(float temp);
const char* temp_sensor_get_level_string(TempLevel level);

#endif
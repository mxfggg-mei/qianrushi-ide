#ifndef LIGHT_SENSOR_H
#define LIGHT_SENSOR_H

#include <Arduino.h>

// 光感传感器引脚定义
#define LIGHT_SENSOR_PIN 1  // GPIO1

// 光照强度等级定义
typedef enum {
    LIGHT_DARK = 0,      // 黑暗
    LIGHT_DIM = 1,       // 昏暗
    LIGHT_NORMAL = 2,    // 正常
    LIGHT_BRIGHT = 3,    // 明亮
    LIGHT_VERY_BRIGHT = 4 // 非常明亮
} LightLevel;

// 函数声明
void light_sensor_init(void);
uint16_t light_sensor_read_raw(void);
float light_sensor_read_voltage(void);
LightLevel light_sensor_get_level(void);
const char* light_sensor_get_level_string(LightLevel level);

#endif
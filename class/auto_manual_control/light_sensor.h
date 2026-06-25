#ifndef LIGHT_SENSOR_H
#define LIGHT_SENSOR_H

#include <Arduino.h>

#define LIGHT_SENSOR_PIN 1  // GPIO1

typedef enum {
    LIGHT_DARK = 0,
    LIGHT_DIM = 1,
    LIGHT_NORMAL = 2,
    LIGHT_BRIGHT = 3,
    LIGHT_VERY_BRIGHT = 4
} LightLevel;

void light_sensor_init(void);
uint16_t light_sensor_read_raw(void);
LightLevel light_sensor_get_level(void);
const char* light_sensor_get_level_string(LightLevel level);

#endif
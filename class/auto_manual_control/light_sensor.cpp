#include "light_sensor.h"

void light_sensor_init(void) {
    pinMode(LIGHT_SENSOR_PIN, INPUT);
    analogSetAttenuation(ADC_11db);
    analogReadResolution(12);
}

uint16_t light_sensor_read_raw(void) {
    return analogRead(LIGHT_SENSOR_PIN);
}

LightLevel light_sensor_get_level(void) {
    uint16_t raw_value = light_sensor_read_raw();
    
    if (raw_value < 800) {
        return LIGHT_VERY_BRIGHT;
    } else if (raw_value < 1800) {
        return LIGHT_BRIGHT;
    } else if (raw_value < 2200) {
        return LIGHT_NORMAL;
    } else if (raw_value < 2800) {
        return LIGHT_DIM;
    } else {
        return LIGHT_DARK;
    }
}

const char* light_sensor_get_level_string(LightLevel level) {
    switch(level) {
        case LIGHT_DARK: return "黑暗";
        case LIGHT_DIM: return "昏暗";
        case LIGHT_NORMAL: return "正常";
        case LIGHT_BRIGHT: return "明亮";
        case LIGHT_VERY_BRIGHT: return "非常明亮";
        default: return "未知";
    }
}
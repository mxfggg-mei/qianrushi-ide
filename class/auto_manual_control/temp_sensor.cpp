#include "temp_sensor.h"
#include <OneWire.h>
#include <DallasTemperature.h>

OneWire oneWire(TEMP_SENSOR_PIN);
DallasTemperature sensors(&oneWire);

void temp_sensor_init(void) {
    sensors.begin();
}

float temp_sensor_read_celsius(void) {
    sensors.requestTemperatures();
    return sensors.getTempCByIndex(0);
}

TempLevel temp_sensor_get_level(float temp) {
    if (temp < 10) {
        return TEMP_VERY_LOW;
    } else if (temp < 18) {
        return TEMP_LOW;
    } else if (temp < 28) {
        return TEMP_NORMAL;
    } else if (temp < 35) {
        return TEMP_HIGH;
    } else {
        return TEMP_VERY_HIGH;
    }
}

const char* temp_sensor_get_level_string(TempLevel level) {
    switch(level) {
        case TEMP_VERY_LOW: return "非常低";
        case TEMP_LOW: return "低";
        case TEMP_NORMAL: return "正常";
        case TEMP_HIGH: return "高";
        case TEMP_VERY_HIGH: return "非常高";
        default: return "未知";
    }
}
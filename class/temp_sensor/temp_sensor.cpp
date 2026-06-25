#include "temp_sensor.h"
#include <OneWire.h>
#include <DallasTemperature.h>

// 创建OneWire对象
OneWire oneWire(TEMP_SENSOR_PIN);

// 创建DallasTemperature对象
DallasTemperature sensors(&oneWire);

/**
 * @brief 温度传感器初始化函数
 */
void temp_sensor_init(void) {
    sensors.begin();
}

/**
 * @brief 读取温度值（摄氏度）
 * @return 温度值（℃）
 */
float temp_sensor_read_celsius(void) {
    sensors.requestTemperatures();  // 发送转换命令
    return sensors.getTempCByIndex(0);  // 获取第一个传感器的温度
}

/**
 * @brief 读取温度值（华氏度）
 * @return 温度值（℉）
 */
float temp_sensor_read_fahrenheit(void) {
    return temp_sensor_read_celsius() * 9.0 / 5.0 + 32.0;
}

/**
 * @brief 获取温度等级
 * @param temp 温度值（℃）
 * @return 温度等级
 */
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

/**
 * @brief 获取温度等级的字符串描述
 * @param level 温度等级
 * @return 字符串描述
 */
const char* temp_sensor_get_level_string(TempLevel level) {
    switch(level) {
        case TEMP_VERY_LOW:
            return "非常低";
        case TEMP_LOW:
            return "低";
        case TEMP_NORMAL:
            return "正常";
        case TEMP_HIGH:
            return "高";
        case TEMP_VERY_HIGH:
            return "非常高";
        default:
            return "未知";
    }
}
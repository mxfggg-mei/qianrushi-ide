#include "light_sensor.h"

/**
 * @brief 光感传感器初始化函数
 * @details 设置ADC引脚为输入模式
 */
void light_sensor_init(void) {
    pinMode(LIGHT_SENSOR_PIN, INPUT);
    // 设置ADC衰减为11dB，支持0-2.9V输入
    analogSetAttenuation(ADC_11db);
    // 设置ADC分辨率为12位（0-4095）
    analogReadResolution(12);
}

/**
 * @brief 读取光感传感器原始ADC值
 * @return ADC值（0-4095）
 */
uint16_t light_sensor_read_raw(void) {
    return analogRead(LIGHT_SENSOR_PIN);
}

/**
 * @brief 读取光感传感器电压值
 * @return 电压值（0-3.3V）
 */
float light_sensor_read_voltage(void) {
    uint16_t raw_value = light_sensor_read_raw();
    // 转换为电压（参考电压3.3V）
    return raw_value * (3.3 / 4095.0);
}

/**
 * @brief 获取光照强度等级
 * @return 光照等级
 */
LightLevel light_sensor_get_level(void) {
    uint16_t raw_value = light_sensor_read_raw();
    
    // 根据ADC值判断光照等级（光感传感器输出反特性：光照越强，ADC值越小）
    // 阈值调低，更容易触发开灯
    if (raw_value < 800) {
        return LIGHT_VERY_BRIGHT;  // ADC值小，光照最强
    } else if (raw_value < 1800) {
        return LIGHT_BRIGHT;
    } else if (raw_value < 2200) {
        return LIGHT_NORMAL;
    } else if (raw_value < 2800) {
        return LIGHT_DIM;  // 调低阈值，稍微暗一点就判定为昏暗
    } else {
        return LIGHT_DARK;  // ADC值大，光照最弱
    }
}

/**
 * @brief 获取光照等级的字符串描述
 * @param level 光照等级
 * @return 字符串描述
 */
const char* light_sensor_get_level_string(LightLevel level) {
    switch(level) {
        case LIGHT_DARK:
            return "黑暗";
        case LIGHT_DIM:
            return "昏暗";
        case LIGHT_NORMAL:
            return "正常";
        case LIGHT_BRIGHT:
            return "明亮";
        case LIGHT_VERY_BRIGHT:
            return "非常明亮";
        default:
            return "未知";
    }
}
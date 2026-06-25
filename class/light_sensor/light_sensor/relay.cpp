#include "relay.h"

/**
 * @brief 继电器初始化函数
 * @details 设置继电器引脚为输出模式，初始状态为断开
 */
void relay_init(void) {
    pinMode(LIGHT_RELAY_PIN, OUTPUT);
    digitalWrite(LIGHT_RELAY_PIN, RELAY_OFF);  // 初始断开
}

/**
 * @brief 继电器吸合（打开）
 */
void relay_on(void) {
    digitalWrite(LIGHT_RELAY_PIN, RELAY_ON);
}

/**
 * @brief 继电器断开（关闭）
 */
void relay_off(void) {
    digitalWrite(LIGHT_RELAY_PIN, RELAY_OFF);
}

/**
 * @brief 继电器状态切换
 */
void relay_toggle(void) {
    if (digitalRead(LIGHT_RELAY_PIN) == RELAY_ON) {
        relay_off();
    } else {
        relay_on();
    }
}
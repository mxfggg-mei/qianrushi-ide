/**
 * @file relay.cpp
 * @brief 继电器控制模块实现文件
 * @details 实现继电器初始化和控制功能
 */

#include "relay.h"

/**
 * @brief 继电器初始化函数
 * @details 将继电器引脚设置为输出模式，初始状态为断开
 */
void relay_init(void)
{
    pinMode(RELAY_PIN, OUTPUT);         // 设置为输出模式
    digitalWrite(RELAY_PIN, RELAY_OFF);  // 初始断开
}

/**
 * @brief 继电器控制函数
 * @param sta 继电器状态：RELAY_ON（吸合）或 RELAY_OFF（断开）
 */
void relay_ctrl(uint8_t sta)
{
    digitalWrite(RELAY_PIN, sta);
}
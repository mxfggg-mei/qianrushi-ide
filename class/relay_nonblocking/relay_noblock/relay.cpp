/**
 * @file relay.cpp
 * @brief 继电器控制模块实现文件
 * @details 实现继电器初始化和控制功能
 *          可以控制继电器吸合和断开，用于控制大功率设备
 */

#include "relay.h"

/**
 * @brief 继电器初始化函数
 * @details 将继电器控制引脚设置为输出模式，并初始化为断开状态
 *          初始化时继电器处于断开状态，确保安全
 */
void relay_init(void)
{
    pinMode(RELAY_PIN, OUTPUT);         // 设置为输出模式
    digitalWrite(RELAY_PIN, RELAY_OFF); // 初始断开继电器
}

/**
 * @brief 继电器控制函数
 * @param sta 继电器状态：RELAY_ON（吸合）或 RELAY_OFF（断开）
 * @details 通过设置引脚电平来控制继电器状态
 *          - RELAY_ON（HIGH）：继电器吸合，连接负载
 *          - RELAY_OFF（LOW）：继电器断开，切断负载
 */
void relay_ctrl(uint8_t sta)
{
    digitalWrite(RELAY_PIN, sta); // 设置继电器状态
}
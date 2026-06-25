/**
 * @file relay.cpp
 * @brief 继电器控制模块实现文件（双继电器版）
 * @details 实现双继电器初始化和控制功能
 *          LED与继电器1共用GPIO6
 */

#include "relay.h"

/**
 * @brief 继电器初始化函数
 * @details 将两个继电器引脚设置为输出模式，初始状态为断开
 */
void relay_init(void)
{
    pinMode(RELAY1_PIN, OUTPUT);         // 继电器1/LED引脚设置为输出
    pinMode(RELAY2_PIN, OUTPUT);         // 继电器2引脚设置为输出
    
    digitalWrite(RELAY1_PIN, RELAY_OFF); // 初始断开继电器1
    digitalWrite(RELAY2_PIN, RELAY_OFF); // 初始断开继电器2
}

/**
 * @brief 继电器1控制函数
 * @param sta 继电器状态：RELAY_ON（吸合）或 RELAY_OFF（断开）
 */
void relay1_ctrl(uint8_t sta)
{
    digitalWrite(RELAY1_PIN, sta);
}

/**
 * @brief 继电器2控制函数
 * @param sta 继电器状态：RELAY_ON（吸合）或 RELAY_OFF（断开）
 */
void relay2_ctrl(uint8_t sta)
{
    digitalWrite(RELAY2_PIN, sta);
}
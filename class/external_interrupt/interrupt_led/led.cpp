/**
 * @file led.cpp
 * @brief LED控制模块实现文件
 * @details 实现LED初始化功能，配置GPIO6为输出模式      
 *          初始状态为熄灭（高电平）
 */

#include "led.h"

/**
 * @brief LED初始化函数
 * @details 将LED引脚设置为输出模式，并初始化为熄灭状态
 *          假设LED为共阳极接法：
 *          - 高电平（1）：LED熄灭
 *          - 低电平（0）：LED点亮
 */
void led_init(void)
{
    pinMode(LED_PIN, OUTPUT);         // 设置为输出模式
    digitalWrite(LED_PIN, HIGH);      // 初始熄灭（高电平）
}
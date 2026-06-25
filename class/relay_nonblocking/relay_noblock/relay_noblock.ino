/**
 * @file relay_noblock.ino
 * @brief 继电器控制主程序（非阻塞方式）
 * @details 使用millis()实现非阻塞延时，继电器每2秒切换一次状态
 *          非阻塞方式不会阻塞其他代码执行，可以同时处理多个任务
 */

#include "relay.h"

unsigned long last_time = 0;     // 记录上次状态切换的时间戳（毫秒）
const long interval = 2000;      // 状态切换间隔（毫秒），这里是2秒
uint8_t relay_sta = RELAY_OFF;   // 继电器当前状态，初始为断开

/**
 * @brief 初始化函数
 * @details 只执行一次，初始化继电器模块
 */
void setup()
{
    relay_init();  // 调用继电器初始化函数
}

/**
 * @brief 主循环函数
 * @details 使用非阻塞方式实现继电器定时切换
 *          使用millis()获取当前时间，判断是否需要切换状态
 *          优点：不会阻塞程序，可以在loop()中添加其他代码
 */
void loop()
{
    unsigned long now = millis(); // 获取当前运行时间（毫秒）

    // 判断是否达到切换时间间隔
    if (now - last_time >= interval)
    {
        last_time = now;           // 更新上次切换时间
        relay_sta = !relay_sta;    // 翻转继电器状态（0变1，1变0）
        relay_ctrl(relay_sta);     // 应用新状态到继电器
    }
    // 这里可以添加其他代码，不会被delay阻塞
}
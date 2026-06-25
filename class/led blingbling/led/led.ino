/**
 * @file led.ino
 * @brief LED闪烁主程序
 * @details 实现LED闪烁效果，每隔200ms切换一次状态
 *          LED亮200ms，灭200ms，形成闪烁效果
 */

#include "led.h"

/**
 * @brief 初始化函数
 * @details 只执行一次，初始化LED模块
 */
void setup()
{
    led_init();  // 调用LED初始化函数
}

/**
 * @brief 主循环函数
 * @details 无限循环执行，实现LED闪烁效果
 *          亮200ms，灭200ms，循环往复
 */
void loop()
{
    LED(0);   // 点亮LED（低电平点亮）
    delay(200); // 延时200ms
    LED(1);   // 熄灭LED（高电平熄灭）
    delay(200); // 延时200ms
}
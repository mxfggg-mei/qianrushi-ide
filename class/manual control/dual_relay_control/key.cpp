/**
 * @file key.cpp
 * @brief 双按键控制模块实现文件
 * @details 使用浮空输入模式，让硬件决定电平
 */

#include "key.h"

/**
 * @brief 按键初始化函数
 * @details 将按键引脚设置为浮空输入模式
 */
void key_init(void)
{
    pinMode(KEY1_PIN, INPUT); // KEY1设置为浮空输入
    pinMode(KEY2_PIN, INPUT); // KEY2设置为浮空输入
}
/**
 * @file key.cpp
 * @brief 按键检测模块实现文件
 * @details 自适应按键检测，自动识别高低电平触发
 */

#include "key.h"

static int press_level = 0;  // 按键按下时的电平（0=低电平触发，1=高电平触发）

/**
 * @brief 按键初始化函数
 * @details 设置引脚为浮空输入，让硬件决定电平
 */
void key_init(void)
{
    pinMode(KEY_PIN, INPUT); // 浮空输入模式，不使用内部电阻
}

/**
 * @brief 校准按键电平
 * @details 检测按键未按下时的电平，自动设置按下电平
 */
void key_calibrate(void)
{
    delay(100);
    int idle_level = digitalRead(KEY_PIN);
    press_level = !idle_level;  // 按下电平 = 空闲电平的反
    
    Serial.print("校准完成: ");
    Serial.print("空闲电平=");
    Serial.print(idle_level);
    Serial.print(", 按下电平=");
    Serial.println(press_level);
}

/**
 * @brief 读取按键状态
 * @return 1=按下，0=未按下
 */
int key_read(void)
{
    return (digitalRead(KEY_PIN) == press_level) ? 1 : 0;
}

/**
 * @brief 检测按键是否按下（带消抖）
 * @return 1=按下，0=未按下
 */
int key_is_pressed(void)
{
    static int last_state = 0;
    
    if (key_read() == 1) {
        delay(10);  // 消抖
        if (key_read() == 1 && last_state == 0) {
            last_state = 1;
            return 1;  // 返回单次按下事件
        }
    } else {
        last_state = 0;
    }
    
    return 0;
}
/**
 * @file exti.cpp
 * @brief 外部中断模块实现文件
 * @details 使用CHANGE模式检测按键变化，只有完成按下→弹起才翻转LED
 *          添加软件消抖，确保按键稳定后才处理
 */

#include "exti.h"
#include "key.h"

uint8_t led_state = 0;       // LED状态变量
static uint8_t key_pressed = 0;  // 按键按下标志
static unsigned long last_time = 0;  // 上次状态变化时间

void key_isr(void);

/**
 * @brief 外部中断初始化函数
 */
void exti_init(void) {
    // 使用CHANGE模式，检测任何电平变化
    attachInterrupt(digitalPinToInterrupt(KEY_PIN), key_isr, CHANGE);
}

/**
 * @brief 中断服务函数
 * @details 检测按键的完整动作：按下→弹起，带消抖处理
 */
void IRAM_ATTR key_isr(void) {
    // 消抖：忽略20ms内的重复触发
    if (millis() - last_time < 20) {
        return;
    }
    last_time = millis();
    
    int key_val = digitalRead(KEY_PIN);
    
    if (key_val == 0) {
        // 按键按下（低电平）
        key_pressed = 1;
    } else {
        // 按键松开（高电平）
        if (key_pressed == 1) {
            // 完成一次完整的按下→弹起动作
            led_state = !led_state;
            key_pressed = 0;  // 重置标志
        }
    }
}
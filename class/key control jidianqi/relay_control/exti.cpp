/**
 * @file exti.cpp
 * @brief 外部中断模块实现文件
 */

#include "exti.h"
#include "key.h"

uint8_t system_enable = 0;  // 系统使能标志：0=关闭，1=开启
uint8_t relay_toggle = 0;   // 继电器切换标志：1=需要切换

void key1_isr(void);
void key2_isr(void);

void exti_init(void) {
    key_init();
    attachInterrupt(digitalPinToInterrupt(KEY1_PIN), key1_isr, CHANGE);
    attachInterrupt(digitalPinToInterrupt(KEY2_PIN), key2_isr, CHANGE);
}

/**
 * @brief KEY1中断服务函数（总开关）
 */
void IRAM_ATTR key1_isr(void) {
    static unsigned long key1_last_time = 0;
    
    // 消抖：忽略50ms内的重复触发
    if (millis() - key1_last_time < 50) {
        return;
    }
    key1_last_time = millis();
    
    // 适配硬件：按下低电平切换使能
    if (digitalRead(KEY1_PIN) == LOW) {
        system_enable = !system_enable;
    }
}

/**
 * @brief KEY2中断服务函数（继电器控制）
 */
void IRAM_ATTR key2_isr(void) {
    static uint8_t key2_pressed = 0;
    static unsigned long key2_last_time = 0;
    
    // 消抖：忽略50ms内的重复触发
    if (millis() - key2_last_time < 50) {
        return;
    }
    key2_last_time = millis();
    
    // 修正电平判断：按下=LOW，松开=HIGH（匹配多数硬件）
    if (digitalRead(KEY2_PIN) == LOW) {
        // KEY2按下（低电平）
        key2_pressed = 1;
    } else {
        // KEY2松开（高电平）→ 完成按下→松开才触发
        if (key2_pressed == 1 && system_enable == 1) {
            relay_toggle = 1;
        }
        key2_pressed = 0;
    }
}
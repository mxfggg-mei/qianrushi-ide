#ifndef RELAY_H
#define RELAY_H

#include <Arduino.h>

// 照明继电器引脚定义
#define LIGHT_RELAY_PIN 6  // GPIO6，用于控制照明灯

// 继电器状态定义
#define RELAY_ON    HIGH  // 继电器吸合（高电平）
#define RELAY_OFF   LOW   // 继电器断开（低电平）

// 函数声明
void relay_init(void);
void relay_on(void);
void relay_off(void);
void relay_toggle(void);

#endif
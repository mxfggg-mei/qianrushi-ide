/**
 * @file relay.h
 * @brief 继电器控制模块头文件（双继电器版）
 * @details 定义两个继电器控制引脚和状态宏
 *          LED与继电器1强绑定（同一引脚GPIO6）
 */

#ifndef __RELAY_H
#define __RELAY_H

#include "Arduino.h"

// 继电器控制引脚
#define RELAY1_PIN  6   // 继电器1控制引脚（同时控制LED），GPIO6
#define RELAY2_PIN  7   // 继电器2控制引脚，GPIO7

// 继电器状态
#define RELAY_ON   HIGH  // 继电器吸合状态（高电平）
#define RELAY_OFF  LOW   // 继电器断开状态（低电平）

// LED与继电器1强绑定
#define LED_PIN    RELAY1_PIN
#define LED_ON     RELAY_ON
#define LED_OFF    RELAY_OFF
#define LED(x)     digitalWrite(LED_PIN, x)

void relay_init(void);        // 继电器初始化函数声明
void relay1_ctrl(uint8_t sta); // 继电器1控制函数
void relay2_ctrl(uint8_t sta); // 继电器2控制函数

#endif
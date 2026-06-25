/**
 * @file relay.h
 * @brief 继电器控制模块头文件
 */

#ifndef __RELAY_H
#define __RELAY_H

#include "Arduino.h"

#define RELAY_PIN   6    // 继电器控制引脚（IN引脚），GPIO6     
#define RELAY_ON    HIGH // 继电器吸合状态（高电平吸合）
#define RELAY_OFF   LOW  // 继电器断开状态（低电平断开）

void relay_init(void);       // 继电器初始化函数
void relay_ctrl(uint8_t sta); // 继电器控制函数

#endif
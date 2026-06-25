/**
 * @file key.h
 * @brief 双按键控制模块头文件
 * @details 定义两个按键引脚：KEY1（总开关）和 KEY2（控制开关）
 *          KEY1: GPIO20（自锁按键，总开关）
 *          KEY2: GPIO21（普通按键，控制继电器）
 */

#ifndef __KEY_H
#define __KEY_H

#include "Arduino.h"

#define KEY1_PIN   20   // KEY1引脚（总开关，自锁按键）
#define KEY2_PIN   21   // KEY2引脚（控制开关，普通按键）

#define KEY1       digitalRead(KEY1_PIN)  // 读取KEY1状态
#define KEY2       digitalRead(KEY2_PIN)  // 读取KEY2状态

void key_init(void);  // 按键初始化函数声明

#endif
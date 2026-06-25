/**
 * @file key.h
 * @brief 按键检测模块头文件
 * @details 定义按键引脚和相关宏，声明初始化函数
 *          使用GPIO21作为按键输入引脚
 */

#ifndef __KEY_H
#define __KEY_H

#include "Arduino.h"

#define KEY_PIN   21   // 按键连接的GPIO引脚号（GPIO21）

void key_init(void);       // 按键初始化函数声明
void key_calibrate(void);  // 按键校准函数声明
int key_read(void);        // 读取按键状态（1=按下，0=未按下）
int key_is_pressed(void);  // 检测按键是否按下

#endif
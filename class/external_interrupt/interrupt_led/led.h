/**
 * @file led.h
 * @brief LED控制模块头文件
 * @details 定义LED引脚和相关宏，声明初始化函数
 *          使用GPIO6作为LED输出引脚        
 */

#ifndef __LED_H
#define __LED_H

#include "Arduino.h"

#define LED_PIN  6             // LED输出引脚，GPIO6    
#define LED(x)   digitalWrite(LED_PIN, x)  // 宏定义：快捷设置LED状态
#define LED_TOGGLE() digitalWrite(LED_PIN, !digitalRead(LED_PIN)) // 宏定义：翻转LED状态

void led_init(void);  // LED初始化函数声明

#endif
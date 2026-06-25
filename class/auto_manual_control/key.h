#ifndef __KEY_H
#define __KEY_H

#include "Arduino.h"

#define KEY1_PIN   20   // KEY1引脚（总开关）
#define KEY2_PIN   21   // KEY2引脚（控制开关）

#define KEY1       digitalRead(KEY1_PIN)  // 读取KEY1状态
#define KEY2       digitalRead(KEY2_PIN)  // 读取KEY2状态

void key_init(void);  // 按键初始化函数声明

#endif
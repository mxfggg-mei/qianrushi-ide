/**
 * @file exti.h
 * @brief 外部中断模块头文件
 */

#ifndef __EXTI_H
#define __EXTI_H

#include "Arduino.h"

extern uint8_t led_state;

void exti_init(void);

#endif
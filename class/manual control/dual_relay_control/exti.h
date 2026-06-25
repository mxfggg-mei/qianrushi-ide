#ifndef __EXTI_H
#define __EXTI_H

#include <Arduino.h>

#define KEY1_PIN 20
#define KEY2_PIN 21

extern bool system_enable;
extern bool key2_short;
extern bool key2_long_2s;
extern bool auto_mode;
extern bool long_triggered;

void exti_init(void);
void exti_task(void);

#endif
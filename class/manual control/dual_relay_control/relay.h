#ifndef RELAY_H
#define RELAY_H

#include <Arduino.h>
#include "exti.h"

#define RELAY1_PIN 6
#define RELAY2_PIN 7

void relay_init(void);
void relay_task(void);
void relay_auto_run(void);
void relay_manual_run(bool key2_short);
void relay_all_off(void);

#endif
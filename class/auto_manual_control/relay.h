#ifndef RELAY_H
#define RELAY_H

#include <Arduino.h>

#define RELAY1_PIN 6   // GPIO6，照明灯控制
#define RELAY2_PIN 7   // GPIO7，风扇控制

#define RELAY_ON    HIGH
#define RELAY_OFF   LOW

#define TEMP_THRESHOLD_DEFAULT 28.0f
#define LIGHT_THRESHOLD_DEFAULT 119

extern bool auto_mode;
extern float temp_threshold;
extern uint16_t light_threshold;

void relay_init(void);
void relay_task(void);
void relay_auto_run(void);
void relay_manual_run(void);
void relay_all_off(void);
bool relay_get_relay1_status(void);
bool relay_get_relay2_status(void);
bool relay_set_relay1(bool on);
bool relay_set_relay2(bool on);
bool relay_set_mode(bool auto_mode_set);
void relay_set_temp_threshold(float temp);
void relay_set_light_threshold(uint16_t light);

#endif
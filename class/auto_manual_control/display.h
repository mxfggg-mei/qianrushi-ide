#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>

#define I2C_SDA 4    // I2C SDA引脚
#define I2C_SCL 5    // I2C SCL引脚

void display_init(void);
void display_update_info(bool mode_auto, bool main_on, uint16_t light_val, uint16_t light_max, 
                         float temp_val, float temp_max, bool led_on, bool fan_on, bool wifi_connected);

#endif
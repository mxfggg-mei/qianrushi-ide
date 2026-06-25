#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>

#define I2C_SDA 4    // I2C SDA引脚
#define I2C_SCL 5    // I2C SCL引脚

void display_init(void);
void display_clear(void);
void display_show_text(uint8_t x, uint8_t y, const char* text);
void display_show_number(uint8_t x, uint8_t y, float num, uint8_t decimals);
void display_update(void);
void display_show_logo(void);

#endif
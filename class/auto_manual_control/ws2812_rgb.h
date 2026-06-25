#ifndef __WS2812_RGB_H
#define __WS2812_RGB_H

#include <Arduino.h>

#define WS2812_PIN 0    // GPIO0控制万色灯

typedef struct {
    uint8_t r;  // 红色 0-255
    uint8_t g;  // 绿色 0-255
    uint8_t b;  // 蓝色 0-255
} RGBColor;

void ws2812_init(void);
void ws2812_set_color(uint8_t r, uint8_t g, uint8_t b);
void ws2812_set_color_struct(RGBColor color);
void ws2812_clear(void);
void ws2812_show(void);
void ws2812_boot_sequence(void);
void ws2812_task(bool wifi_connected, bool mqtt_connected, bool led_on, bool fan_on);

#endif
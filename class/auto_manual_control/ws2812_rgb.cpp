#include "ws2812_rgb.h"
#include <Adafruit_NeoPixel.h>

Adafruit_NeoPixel pixel(1, WS2812_PIN, NEO_GRB + NEO_KHZ800);

static uint32_t hsv_to_rgb(uint16_t hue, uint8_t sat, uint8_t val) {
    uint8_t r, g, b;
    uint16_t h = hue % 60;
    uint8_t f = h * 255 / 60;
    uint8_t p = (255 - sat) * val / 255;
    uint8_t q = (255 - (sat * f / 255)) * val / 255;
    uint8_t t = (255 - (sat * (255 - f) / 255)) * val / 255;
    
    switch (hue / 60) {
        case 0: r = val; g = t; b = p; break;
        case 1: r = q; g = val; b = p; break;
        case 2: r = p; g = val; b = t; break;
        case 3: r = p; g = q; b = val; break;
        case 4: r = t; g = p; b = val; break;
        default: r = val; g = p; b = q; break;
    }
    return pixel.Color(r, g, b);
}

void ws2812_init(void) {
    pixel.begin();
    pixel.setBrightness(80);
    pixel.clear();
    pixel.show();
}

void ws2812_set_color(uint8_t r, uint8_t g, uint8_t b) {
    pixel.setPixelColor(0, pixel.Color(r, g, b));
    pixel.show();
}

void ws2812_set_color_struct(RGBColor color) {
    pixel.setPixelColor(0, pixel.Color(color.r, color.g, color.b));
    pixel.show();
}

void ws2812_clear(void) {
    pixel.clear();
    pixel.show();
}

void ws2812_show(void) {
    pixel.show();
}

void ws2812_boot_sequence(void) {
    for (uint16_t hue = 0; hue < 360; hue += 5) {
        pixel.setPixelColor(0, hsv_to_rgb(hue, 255, 150));
        pixel.show();
        delay(15);
    }
    pixel.clear();
    pixel.show();
}

typedef enum {
    INDICATOR_MODE_LED_FAN,
    INDICATOR_MODE_LED_ONLY,
    INDICATOR_MODE_FAN_ONLY,
    INDICATOR_MODE_MQTT_DISCONNECTED,
    INDICATOR_MODE_WIFI_DISCONNECTED,
    INDICATOR_MODE_NORMAL
} IndicatorMode;

static IndicatorMode current_mode = INDICATOR_MODE_NORMAL;
static unsigned long last_time = 0;
static unsigned long last_update = 0;
static int state_index = 0;
static uint8_t fade_r = 0, fade_g = 0, fade_b = 0;

// 非阻塞渐变状态机变量
static bool is_fading = false;
static uint8_t fade_target_r = 0, fade_target_g = 0, fade_target_b = 0;
static unsigned long fade_start_time = 0;
static unsigned long fade_duration = 0;
static uint8_t fade_start_r = 0, fade_start_g = 0, fade_start_b = 0;

// 渐变是否正在进行中（任何模式都可能触发渐变）
static bool any_fade_active(void) {
    return is_fading;
}

// 在 ws2812_task 开头调用，更新渐变进度（非阻塞）
static void fade_update(void) {
    if (!is_fading) return;
    unsigned long elapsed = millis() - fade_start_time;
    if (elapsed >= fade_duration) {
        // 渐变结束
        fade_r = fade_target_r;
        fade_g = fade_target_g;
        fade_b = fade_target_b;
        pixel.setPixelColor(0, pixel.Color(fade_r, fade_g, fade_b));
        pixel.show();
        is_fading = false;
    } else {
        float progress = (float)elapsed / fade_duration;
        fade_r = fade_start_r + (fade_target_r - fade_start_r) * progress;
        fade_g = fade_start_g + (fade_target_g - fade_start_g) * progress;
        fade_b = fade_start_b + (fade_target_b - fade_start_b) * progress;
        pixel.setPixelColor(0, pixel.Color(fade_r, fade_g, fade_b));
        pixel.show();
    }
}

// 启动一次非阻塞渐变（异步，不等待完成）
static void fade_to(uint8_t tr, uint8_t tg, uint8_t tb, unsigned long duration) {
    fade_start_r = fade_r;
    fade_start_g = fade_g;
    fade_start_b = fade_b;
    fade_target_r = tr;
    fade_target_g = tg;
    fade_target_b = tb;
    fade_start_time = millis();
    fade_duration = duration > 0 ? duration : 1;
    is_fading = true;
}

void ws2812_task(bool wifi_connected, bool mqtt_connected, bool led_on, bool fan_on) {
    // 先更新非阻塞渐变
    fade_update();
    
    unsigned long now = millis();
    
    // 如果正在渐变，不切换新状态（等待渐变完成）
    if (is_fading) return;
    
    IndicatorMode new_mode;
    
    if (!wifi_connected) {
        new_mode = INDICATOR_MODE_WIFI_DISCONNECTED;
    } else if (!mqtt_connected) {
        new_mode = INDICATOR_MODE_MQTT_DISCONNECTED;
    } else if (led_on && fan_on) {
        new_mode = INDICATOR_MODE_LED_FAN;
    } else if (led_on) {
        new_mode = INDICATOR_MODE_LED_ONLY;
    } else if (fan_on) {
        new_mode = INDICATOR_MODE_FAN_ONLY;
    } else {
        new_mode = INDICATOR_MODE_NORMAL;
    }
    
    if (new_mode != current_mode) {
        current_mode = new_mode;
        state_index = 0;
        last_time = now;
        
        if (current_mode == INDICATOR_MODE_MQTT_DISCONNECTED) {
            fade_r = 200;
            fade_g = 0;
            fade_b = 0;
            pixel.setPixelColor(0, pixel.Color(200, 0, 0));
            pixel.show();
        }
    }
    
    switch (current_mode) {
        case INDICATOR_MODE_LED_FAN: {
            // LED灯亮且风扇启动：红灯100ms，全灭50ms，红灯100ms，全灭50ms，绿灯100ms，全灭50ms，绿灯100ms，全灭50ms，蓝灯100ms，全灭50ms，蓝灯100ms，全灭350ms
            static const int states[] = {100, 50, 100, 50, 100, 50, 100, 50, 100, 50, 100, 350};
            static const uint8_t colors[][3] = {
                {200, 0, 0}, {0, 0, 0}, {200, 0, 0}, {0, 0, 0},
                {0, 200, 0}, {0, 0, 0}, {0, 200, 0}, {0, 0, 0},
                {0, 0, 200}, {0, 0, 0}, {0, 0, 200}, {0, 0, 0}
            };
            
            if (now - last_time >= states[state_index]) {
                last_time = now;
                pixel.setPixelColor(0, pixel.Color(colors[state_index][0], colors[state_index][1], colors[state_index][2]));
                pixel.show();
                fade_r = colors[state_index][0];
                fade_g = colors[state_index][1];
                fade_b = colors[state_index][2];
                state_index = (state_index + 1) % 12;
            }
            break;
        }
        
        case INDICATOR_MODE_LED_ONLY: {
            // LED灯亮：红灯300ms，全灭200ms，绿灯300ms，全灭200ms，蓝灯300ms，全灭200ms
            static const int states[] = {300, 200, 300, 200, 300, 200};
            static const uint8_t colors[][3] = {
                {200, 0, 0}, {0, 0, 0}, {0, 200, 0}, {0, 0, 0}, {0, 0, 200}, {0, 0, 0}
            };
            
            if (now - last_time >= states[state_index]) {
                last_time = now;
                pixel.setPixelColor(0, pixel.Color(colors[state_index][0], colors[state_index][1], colors[state_index][2]));
                pixel.show();
                fade_r = colors[state_index][0];
                fade_g = colors[state_index][1];
                fade_b = colors[state_index][2];
                state_index = (state_index + 1) % 6;
            }
            break;
        }
        
        case INDICATOR_MODE_FAN_ONLY: {
            // 风扇启动：红灯500ms，绿灯500ms，蓝灯500ms，全灭700ms
            static const int states[] = {500, 500, 500, 700};
            static const uint8_t colors[][3] = {
                {200, 0, 0}, {0, 200, 0}, {0, 0, 200}, {0, 0, 0}
            };
            
            if (now - last_time >= states[state_index]) {
                last_time = now;
                pixel.setPixelColor(0, pixel.Color(colors[state_index][0], colors[state_index][1], colors[state_index][2]));
                pixel.show();
                fade_r = colors[state_index][0];
                fade_g = colors[state_index][1];
                fade_b = colors[state_index][2];
                state_index = (state_index + 1) % 4;
            }
            break;
        }
        
        case INDICATOR_MODE_MQTT_DISCONNECTED: {
            // MQTT未连接：红灯显示，在250ms内过渡到绿色，再在250ms内过渡到全灭，在250ms内过渡到蓝色，再在250ms内过渡到全灭，渐变回红色循环
            if (now - last_time >= 250) {
                last_time = now;
                if (state_index == 0) {
                    fade_to(0, 200, 0, 250);
                } else if (state_index == 1) {
                    fade_to(0, 0, 0, 250);
                } else if (state_index == 2) {
                    fade_to(0, 0, 200, 250);
                } else if (state_index == 3) {
                    fade_to(0, 0, 0, 250);
                } else {
                    fade_to(200, 0, 0, 250);
                }
                state_index = (state_index + 1) % 5;
            }
            break;
        }
        
        case INDICATOR_MODE_WIFI_DISCONNECTED: {
            // WiFi未连接：红灯200ms，绿灯200ms，全灭500ms
            static const int states[] = {200, 200, 500};
            static const uint8_t colors[][3] = {
                {200, 0, 0}, {0, 200, 0}, {0, 0, 0}
            };
            
            if (now - last_time >= states[state_index]) {
                last_time = now;
                pixel.setPixelColor(0, pixel.Color(colors[state_index][0], colors[state_index][1], colors[state_index][2]));
                pixel.show();
                fade_r = colors[state_index][0];
                fade_g = colors[state_index][1];
                fade_b = colors[state_index][2];
                state_index = (state_index + 1) % 3;
            }
            break;
        }
        
        case INDICATOR_MODE_NORMAL: {
            // 所有状态正常：全灭开始，渐变到红色(1秒)，渐变到绿色(1秒)，渐变到蓝色(1秒)，渐变到全灭(1秒)，保持全灭200ms
            static const int states[] = {1000, 1000, 1000, 1000, 200};
            static const uint8_t colors[][3] = {
                {200, 0, 0}, {0, 200, 0}, {0, 0, 200}, {0, 0, 0}, {0, 0, 0}
            };
            
            if (now - last_time >= states[state_index]) {
                last_time = now;
                if (state_index == 0) {
                    fade_to(200, 0, 0, 1000);
                } else if (state_index == 1) {
                    fade_to(0, 200, 0, 1000);
                } else if (state_index == 2) {
                    fade_to(0, 0, 200, 1000);
                } else if (state_index == 3) {
                    fade_to(0, 0, 0, 1000);
                }
                if (state_index < 4) {
                    state_index = (state_index + 1) % 5;
                } else {
                    state_index = 0;
                }
                last_time = millis();
            }
            break;
        }
    }
}
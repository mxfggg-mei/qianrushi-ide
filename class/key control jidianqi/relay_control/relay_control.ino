/**
 * @file relay_control.ino
 * @brief 按键控制继电器开关实验主程序（中断方式）
 */

#include "key.h"
#include "relay.h"
#include "exti.h"

uint8_t relay_state = RELAY_OFF;

void setup() {
    relay_init();
    exti_init();
    
    relay_state = RELAY_OFF;
    relay_ctrl(RELAY_OFF);
    
    Serial.begin(115200);
    while (!Serial) { delay(10); }
    
    Serial.println("=== 按键控制继电器 ===");
    Serial.println("KEY1(GPIO20): 总开关，按住使能系统");
    Serial.println("KEY2(GPIO21): 控制键，按下翻转继电器");
    Serial.println("继电器(GPIO6): 受控设备");
    Serial.println("-----------------------------------");
}

void loop() {
    if (relay_toggle == 1) {
        relay_toggle = 0;

        if (system_enable == 1) {
            relay_state = !relay_state;
            relay_ctrl(relay_state);
        } else {
            relay_state = RELAY_OFF;
            relay_ctrl(RELAY_OFF);
        }
    }

    if (system_enable == 0 && relay_state != RELAY_OFF) {
        relay_state = RELAY_OFF;
        relay_ctrl(RELAY_OFF);
    }
    
    // 实时输出中文状态
    static unsigned long last_print = 0;
    if (millis() - last_print >= 500) {
        last_print = millis();
        
        Serial.print("KEY1: ");
        Serial.print(KEY1 ? "按下" : "松开");
        Serial.print(" | KEY2: ");
        Serial.print(KEY2 ? "按下" : "松开");
        Serial.print(" | 系统: ");
        Serial.print(system_enable ? "使能" : "禁用");
        Serial.print(" | 继电器: ");
        Serial.println(relay_state ? "吸合" : "断开");
    }
}
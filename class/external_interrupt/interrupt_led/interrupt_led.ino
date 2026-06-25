/**
 * @file interrupt_led.ino
 * @brief 外部中断实验主程序
 * @details 使用外部中断实现按键控制LED翻转
 *          完全参照 key jiance/key 的配置
 */

#include "key.h"
#include "led.h"
#include "exti.h"

void setup() {
    key_init();              // 初始化按键
    led_init();              // 初始化LED
    exti_init();             // 初始化外部中断
    Serial.begin(115200);    // 开启串口通信
    
    while (!Serial) {
        delay(10);
    }
    
    Serial.println("=== 外部中断实验启动 ===");
    Serial.println("按键引脚: GPIO21");
    Serial.println("LED引脚: GPIO6");
}

void loop() {
    // 主循环中控制LED状态
    if (led_state == 1) {
        LED(LOW);   // 点亮LED
    } else {
        LED(HIGH);  // 熄灭LED
    }
    
    // 每秒打印状态
    static unsigned long last_print = 0;
    if (millis() - last_print >= 1000) {
        last_print = millis();
        Serial.print("LED状态: ");
        Serial.println(led_state ? "亮" : "灭");
    }
}
/**
 * @file key.ino
 * @brief 按键检测主程序 - 自适应版本
 * @details 自动检测按键电平逻辑，解决电平反相问题
 */

#include "key.h"

void setup()
{
    key_init();                          // 调用按键初始化函数
    Serial.begin(115200);                // 初始化串口，波特率115200
    
    while (!Serial) {
        delay(10);
    }
    
    Serial.println("=== 按键检测程序启动 ===");
    Serial.print("按键引脚: GPIO");
    Serial.println(KEY_PIN);
    Serial.println("连接方式: 浮空输入 (INPUT)");
    Serial.println("正在校准按键...");
    
    // 调用校准函数
    key_calibrate();
    
    Serial.println("-------------------------");
    Serial.println("等待按键按下...");
}

void loop()
{
    // 显示实时状态
    static unsigned long last_time = 0;
    if (millis() - last_time >= 100) {
        last_time = millis();
        int raw_value = digitalRead(KEY_PIN);
        int key_state = key_read();
        
        Serial.print("原始值: ");
        Serial.print(raw_value);
        Serial.print(" | 按键状态: ");
        Serial.println(key_state ? "按下" : "未按下");
    }
    
    // 检测按键按下事件
    if (key_is_pressed()) {
        Serial.println("★ 检测到按键按下!");
    }
}
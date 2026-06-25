#include "temp_sensor.h"
#include "relay.h"

// 风扇自动控制标志
bool fan_auto_control = true;  // 自动控制使能

void setup() {
    Serial.begin(115200);
    while (!Serial) { delay(10); }
    
    temp_sensor_init();
    relay_init();
    
    Serial.println("===== 温度检测与风扇控制系统启动 =====");
    Serial.println("温度传感器: DS18B20");
    Serial.println("数据引脚: GPIO10");
    Serial.println("风扇继电器引脚: GPIO7");
    Serial.println("测温范围: -55°C ~ +125°C");
    Serial.println("-----------------------------");
}

void loop() {
    // 读取温度
    float temp_c = temp_sensor_read_celsius();
    float temp_f = temp_sensor_read_fahrenheit();
    TempLevel level = temp_sensor_get_level(temp_c);
    
    // 温度控制风扇逻辑：温度高时自动打开风扇
    if (fan_auto_control) {
        if (level == TEMP_HIGH || level == TEMP_VERY_HIGH) {
            relay_on();
        } else {
            relay_off();
        }
    }
    
    // 获取继电器状态
    bool relay_state = (digitalRead(FAN_RELAY_PIN) == RELAY_ON);
    
    // 串口输出
    Serial.print("温度: ");
    Serial.print(temp_c, 2);
    Serial.print(" °C (");
    Serial.print(temp_f, 2);
    Serial.print(" °F) | 状态: ");
    Serial.print(temp_sensor_get_level_string(level));
    Serial.print(" | 风扇: ");
    Serial.println(relay_state ? "开" : "关");
    
    delay(1000);
}
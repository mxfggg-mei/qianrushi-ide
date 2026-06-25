#include "light_sensor.h"
#include "relay.h"

// 照明控制标志
bool light_auto_control = true;  // 自动控制使能

void setup() {
    Serial.begin(115200);
    while (!Serial) { delay(10); }
    
    light_sensor_init();
    relay_init();
    
    Serial.println("===== 光控照明系统启动 =====");
    Serial.println("光感传感器引脚: GPIO1");
    Serial.println("照明继电器引脚: GPIO6");
    Serial.println("ADC分辨率: 12位 (0-4095)");
    Serial.println("电压范围: 0-3.3V");
    Serial.println("-----------------------------");
}

void loop() {
    // 读取光感数据
    uint16_t raw_value = light_sensor_read_raw();
    float voltage = light_sensor_read_voltage();
    LightLevel level = light_sensor_get_level();
    
    // 光控照明逻辑：光照很弱时自动打开照明灯
    if (light_auto_control) {
        if (level == LIGHT_DARK || level == LIGHT_DIM) {
            relay_on();
        } else {
            relay_off();
        }
    }
    
    // 获取继电器状态
    bool relay_state = (digitalRead(LIGHT_RELAY_PIN) == RELAY_ON);
    
    // 串口输出
    Serial.print("ADC值: ");
    Serial.print(raw_value);
    Serial.print(" | 电压: ");
    Serial.print(voltage, 3);
    Serial.print(" V | 光照强度: ");
    Serial.print(light_sensor_get_level_string(level));
    Serial.print(" | 照明灯: ");
    Serial.println(relay_state ? "开" : "关");
    
    delay(500);
}
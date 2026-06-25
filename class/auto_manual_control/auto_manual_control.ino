#include "exti.h"
#include "relay.h"
#include "light_sensor.h"
#include "temp_sensor.h"
#include "display.h"
#include "wifi_connect.h"
#include "ws2812_rgb.h"
#include "mqtt_connect.h"

void setup() {
  Serial.begin(115200);
  while (!Serial) { delay(10); }
  
  exti_init();
  relay_init();
  light_sensor_init();
  temp_sensor_init();
  display_init();
  ws2812_init();
  
  Serial.println("===== 智能环境控制系统 =====");
  Serial.println("KEY1(GPIO20): 自锁按键，系统使能");
  Serial.println("KEY2(GPIO21): 长按2秒切换模式");
  Serial.println("RELAY1(GPIO6): 照明灯");
  Serial.println("RELAY2(GPIO7): 风扇");
  Serial.println("光照传感器(GPIO1)");
  Serial.println("温度传感器(GPIO10)");
  Serial.println("OLED SDA(GPIO4), SCL(GPIO5)");
  Serial.println("WS2812 RGB(GPIO0)");
  Serial.println("WiFi: ESP32内置模块");
  Serial.println("MQTT: ESP32内置模块");
  Serial.println("-----------------------------");
  Serial.println("串口命令:");
  Serial.println("  disconnect - 断开WiFi并禁用扫描");
  Serial.println("  connect    - 重新启用WiFi连接");
  Serial.println("  mqtt_config - 配置MQTT参数");
  Serial.println("  mqtt_clear - 清除MQTT配置");
  Serial.println("  mqtt_status - 查看MQTT状态");
  Serial.println("  mqtt_report - 手动上报状态");
  Serial.println("-----------------------------");
  Serial.print("模式:");
  Serial.println(auto_mode ? "自动模式" : "手动模式");
  
  ws2812_boot_sequence();
  
  wifi_init();
  
  mqtt_init();
}

void loop() {
  exti_task();
  relay_task();
  wifi_task();
  mqtt_task();
  
  // 统一处理串口命令，避免多个函数竞争读取串口
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    input.trim();
    
    // WiFi 命令
    if (input.equalsIgnoreCase("disconnect")) {
      Serial.println("WiFi disconnect command received");
      wifi_set_enabled(false);
      Serial.println("WiFi disabled. Use 'connect' to re-enable.");
    } else if (input.equalsIgnoreCase("connect")) {
      Serial.println("WiFi connect command received");
      wifi_set_enabled(true);
      wifi_reconnect_now();
      Serial.println("WiFi enabled. Will attempt to reconnect...");
    }
    // MQTT 命令
    else if (input.equalsIgnoreCase("mqtt_config")) {
      Serial.println("MQTT config command received");
      mqtt_serial_config();
    } else if (input.equalsIgnoreCase("mqtt_clear")) {
      Serial.println("MQTT clear config command received");
      mqtt_clear_config();
    } else if (input.equalsIgnoreCase("mqtt_status")) {
      Serial.println("MQTT status command received");
      mqtt_print_config();
      Serial.printf("MQTT connected: %s\n", mqtt_is_connected() ? "true" : "false");
    } else if (input.equalsIgnoreCase("mqtt_report")) {
      Serial.println("MQTT report command received");
      mqtt_report_status();
    }
    // 细分配置指令
    else if (input.startsWith("mqtt_config_server ")) {
      String server = input.substring(19);
      server.trim();
      Serial.printf("MQTT set server: %s\n", server.c_str());
      mqtt_set_server(server.c_str());
    } else if (input.startsWith("mqtt_config_port ")) {
      String port_str = input.substring(17);
      port_str.trim();
      int port = port_str.toInt();
      Serial.printf("MQTT set port: %d\n", port);
      mqtt_set_port(port);
    } else if (input.startsWith("mqtt_config_username ")) {
      String username = input.substring(22);
      username.trim();
      Serial.printf("MQTT set username: %s\n", username.c_str());
      mqtt_set_username(username.c_str());
    } else if (input.startsWith("mqtt_config_password ")) {
      String password = input.substring(22);
      password.trim();
      Serial.println("MQTT set password");
      mqtt_set_password(password.c_str());
    } else if (input.startsWith("mqtt_config_device_id ")) {
      String device_id = input.substring(24);
      device_id.trim();
      Serial.printf("MQTT set device ID: %s\n", device_id.c_str());
      mqtt_set_device_id(device_id.c_str());
    }
  }
  
  ws2812_task(wifi_is_connected(), mqtt_is_connected(), relay_get_relay1_status(), relay_get_relay2_status());
}
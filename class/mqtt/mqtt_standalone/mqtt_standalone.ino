#include <Arduino.h>
#include "wifi_connect.h"
#include "mqtt_connect.h"

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n===== MQTT Standalone Project =====");
  Serial.println("Version: V1.0.0");
  Serial.println("===================================\n");
  
  wifi_init();
  mqtt_init();
  
  Serial.println("\nSystem ready!");
  Serial.println("Available commands:");
  Serial.println("  wifi_config <ssid> <password>  - Configure WiFi");
  Serial.println("  wifi_clear                     - Clear WiFi config");
  Serial.println("  wifi_status                    - Check WiFi status");
  Serial.println("  connect                        - Enable WiFi");
  Serial.println("  disconnect                     - Disable WiFi");
  Serial.println("  mqtt_config                    - Configure MQTT");
  Serial.println("  mqtt_clear                     - Clear MQTT config");
  Serial.println("  mqtt_status                    - Check MQTT status");
  Serial.println("  mqtt_report                    - Report status now");
  Serial.println("  mqtt_config_server <server>    - Set MQTT server");
  Serial.println("  mqtt_config_port <port>        - Set MQTT port");
  Serial.println("  mqtt_config_username <user>    - Set MQTT username");
  Serial.println("  mqtt_config_password <pass>    - Set MQTT password");
  Serial.println("  mqtt_config_device_id <id>     - Set device ID");
}

void loop() {
  wifi_task();
  mqtt_task();
  
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    input.trim();
    
    if (input.length() > 0) {
      Serial.printf("\nReceived command: %s\n", input.c_str());
      
      if (input.startsWith("wifi_") || 
          input.equalsIgnoreCase("connect") || 
          input.equalsIgnoreCase("disconnect")) {
        wifi_check_serial_command(input);
      } else {
        mqtt_check_serial_command(input);
      }
    }
  }
  
  delay(10);
}
#include "wifi_connect.h"

void setup() {
  Serial.begin(115200);
  while (!Serial) { delay(10); }
  
  Serial.println("\n===== WiFi Connect Demo =====");
  Serial.println("Press 'c' to clear saved config and reconfigure");
  Serial.println("------------------------------");
  
  wifi_init();
}

void loop() {
  if (Serial.available()) {
    char cmd = Serial.read();
    if (cmd == 'c' || cmd == 'C') {
      Serial.println("\nClearing WiFi configuration...");
      wifi_clear_config();
      Serial.println("Please restart device to reconfigure WiFi.");
    }
  }
  
  wifi_task();
  
  static unsigned long last_print = 0;
  if (millis() - last_print >= 10000) {
    last_print = millis();
    
    if (wifi_is_connected()) {
      Serial.println("\n====================");
      Serial.print("WiFi Status: ");
      Serial.println("Connected");
      Serial.print("IP Address: ");
      Serial.println(wifi_get_ip());
      Serial.print("SSID: ");
      Serial.println(wifi_get_ssid());
      Serial.print("RSSI: ");
      Serial.print(wifi_get_rssi());
      Serial.println(" dBm");
      Serial.println("Press 'c' to clear config");
    }
  }
}
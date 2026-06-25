#include "display.h"

void setup() {
  Serial.begin(115200);
  
  display_init();
  display_show_logo();
  
  Serial.println("===== OLED显示屏测试 =====");
  Serial.println("SDA: GPIO4");
  Serial.println("SCL: GPIO5");
  Serial.println("屏幕: SH1106 128x64");
  Serial.println("驱动: U8G2_SH1106_128X64_VCOMH0_F_HW_I2C");
}

void loop() {
  static unsigned long last_update = 0;
  static int count = 0;
  
  if (millis() - last_update >= 1000) {
    last_update = millis();
    
    display_clear();
    
    display_show_text(0, 15, "OLED Demo");
    display_show_text(0, 35, "Count:");
    display_show_number(45, 35, count++, 0);
    display_show_text(0, 55, "I2C 128x64");
    
    display_update();
  }
}
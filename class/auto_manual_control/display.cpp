#include "display.h"
#include <U8g2lib.h>
#include <Wire.h>

#ifdef U8X8_HAVE_HW_I2C
U8G2_SH1106_128X64_VCOMH0_F_HW_I2C u8g2(U8G2_R0);
#else
#error "I2C support required"
#endif

void display_init(void) {
  Wire.begin(I2C_SDA, I2C_SCL);
  u8g2.begin();
  u8g2.enableUTF8Print();
  u8g2.setFont(u8g2_font_wqy12_t_gb2312a);
  u8g2.setContrast(0x60);
  u8g2.setPowerSave(0);
  
  u8g2.firstPage();
  do{
    u8g2.setCursor(2,12);
    u8g2.print("系统启动中...");
  }while(u8g2.nextPage());
}

void display_update_info(bool mode_auto, bool main_on, uint16_t light_val, uint16_t light_max, 
                         float temp_val, float temp_max, bool led_on, bool fan_on, bool wifi_connected) {
  u8g2.firstPage();
  do{
    u8g2.setCursor(2,12);
    if(mode_auto)
      u8g2.print("模式:自动 总闸:");
    else
      u8g2.print("模式:手动 总闸:");
    u8g2.print(main_on?"ON":"OFF");

    u8g2.setCursor(2,24);
    u8g2.print("光强:");
    u8g2.print(light_val);
    u8g2.print("/");
    u8g2.print(light_max);
    u8g2.print("lux");

    u8g2.setCursor(2,36);
    u8g2.print("温度:");
    u8g2.print(temp_val,1);
    u8g2.print("/");
    u8g2.print(temp_max,1);
    u8g2.print("C");

    u8g2.setCursor(2,48);
    u8g2.print("灯光:");
    u8g2.print(led_on?"ON":"OFF");
    u8g2.print(" 风扇:");
    u8g2.print(fan_on?"ON":"OFF");

    u8g2.setCursor(2,60);
    u8g2.print("WiFi:");
    u8g2.print(wifi_connected?"已连接":"未连接");

  }while(u8g2.nextPage());
}
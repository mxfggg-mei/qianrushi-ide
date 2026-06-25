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
  u8g2.setContrast(0x60);
  u8g2.setPowerSave(0);
  u8g2.setFont(u8g2_font_ncenB08_tr);
}

void display_clear(void) {
  u8g2.clearBuffer();
}

void display_show_text(uint8_t x, uint8_t y, const char* text) {
  u8g2.drawStr(x, y, text);
}

void display_show_number(uint8_t x, uint8_t y, float num, uint8_t decimals) {
  char buf[16];
  dtostrf(num, 6, decimals, buf);
  u8g2.drawStr(x, y, buf);
}

void display_update(void) {
  u8g2.sendBuffer();
}

void display_show_logo(void) {
  display_clear();
  u8g2.drawStr(0, 15, "PCT_100_CTL");
  u8g2.drawStr(0, 35, "FairyCat");
  u8g2.drawStr(0, 55, "V2.6.1.0");
  display_update();
}
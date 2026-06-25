#include "ws2812_rgb.h"

void setup() {
  Serial.begin(115200);
  while (!Serial) { delay(10); }
  
  ws2812_init();
  
  Serial.println("===== WS2812 RGB万色灯测试 =====");
  Serial.println("GPIO0连接万色灯DIN引脚");
  Serial.println("渐变彩色循环模式");
  Serial.println("-----------------------------");
}

void loop() {
  ws2812_rainbow_cycle(10);  // 渐变速度10ms，数字越小越快
}
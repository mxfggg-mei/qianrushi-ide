#include "exti.h"
#include "relay.h"

void setup() {
  Serial.begin(115200);
  exti_init();
  relay_init();

  Serial.println("===== 上电完成 =====");
  Serial.print("默认模式：");
  Serial.println(auto_mode ? "空模式" : "手动模式");
}

void loop() {
  exti_task();
  relay_task();
}
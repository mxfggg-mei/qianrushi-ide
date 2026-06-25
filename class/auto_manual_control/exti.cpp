#include "exti.h"
#include "key.h"

bool system_enable = false;
bool key2_short = false;
bool key2_long_2s = false;
bool auto_mode = true;   // 上电默认自动模式
bool long_triggered = false;

static unsigned long key2_press_time = 0;
static bool key2_last = false;

#define KEY_DEBOUNCE_TIME 2

void exti_init(void) {
  key_init();
}

void exti_task(void) {
  bool key1 = digitalRead(KEY1_PIN);
  bool key2 = digitalRead(KEY2_PIN);

  system_enable = (key1 == HIGH);

  if (!system_enable) {
    long_triggered = false;
  }

  key2_short = false;
  key2_long_2s = false;

  if (key2 != key2_last) {
    delay(KEY_DEBOUNCE_TIME);
    key2 = digitalRead(KEY2_PIN);
  }

  if (!key2_last && key2) {
    key2_press_time = millis();
    long_triggered = false;
  }

  if (key2_last && !key2) {
    if (millis() - key2_press_time > 2 && millis() - key2_press_time < 1000) {
      key2_short = true;
    }
  }

  if (key2 && !long_triggered && (millis() - key2_press_time >= 2000)) {
    key2_long_2s = true;
    long_triggered = true;
  }

  key2_last = key2;
}
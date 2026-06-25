#include "relay.h"
#include "exti.h"

int state = 0;
static unsigned long auto_tick = 0;

void relay_init(void) {
  pinMode(RELAY1_PIN, OUTPUT);
  pinMode(RELAY2_PIN, OUTPUT);
  
  digitalWrite(RELAY1_PIN, LOW);
  digitalWrite(RELAY2_PIN, LOW);
}

void relay_task(void) {
  if (!system_enable) {
    digitalWrite(RELAY1_PIN, LOW);
    digitalWrite(RELAY2_PIN, LOW);
    state = 0;
    return;
  }

  // ===================== 长按切换模式 =====================
  if (key2_long_2s) {
    auto_mode = !auto_mode;
    key2_long_2s = false;

    // ===================== 【关键】一切换到空模式 → 立即清零 =====================
    if (auto_mode) {
      state = 0;                   // 状态归零
      digitalWrite(RELAY1_PIN, LOW);  // 关灯1
      digitalWrite(RELAY2_PIN, LOW);  // 关灯2
      Serial.println("→ 空模式：已停止，全部关闭");
    } else {
      Serial.println("→ 手动模式：可以按键控制");
    }
  }

  // ===================== 空模式：停止，不响应任何按键 =====================
  if (auto_mode) {
    // 空模式 = 全关、不动
    return;
  }

  // ===================== 手动模式：正常按键控制 =====================
  if (key2_short) {
    state = (state + 1) % 4;
    key2_short = false;
    Serial.print("手动 → 状态：");
    Serial.println(state);
  }

  // 输出逻辑
  switch(state) {
    case 0:
      digitalWrite(RELAY1_PIN, LOW);
      digitalWrite(RELAY2_PIN, LOW);
      break;
    case 1:
      digitalWrite(RELAY1_PIN, LOW);
      digitalWrite(RELAY2_PIN, HIGH);
      break;
    case 2:
      digitalWrite(RELAY1_PIN, HIGH);
      digitalWrite(RELAY2_PIN, LOW);
      break;
    case 3:
      digitalWrite(RELAY1_PIN, HIGH);
      digitalWrite(RELAY2_PIN, HIGH);
      break;
  }
}

void relay_auto_run(void) {
}

void relay_manual_run(bool key2_short) {
}

void relay_all_off(void) {
  digitalWrite(RELAY1_PIN, LOW);
  digitalWrite(RELAY2_PIN, LOW);
}
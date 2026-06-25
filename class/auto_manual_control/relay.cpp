#include "relay.h"
#include "exti.h"
#include "light_sensor.h"
#include "temp_sensor.h"
#include "display.h"
#include "wifi_connect.h"
#include <Preferences.h>

#define THRESHOLD_NAMESPACE "relay_thr"

static Preferences preferences;

int state = 0;
float temp_threshold = TEMP_THRESHOLD_DEFAULT;
uint16_t light_threshold = LIGHT_THRESHOLD_DEFAULT;

void relay_init(void) {
  pinMode(RELAY1_PIN, OUTPUT);
  pinMode(RELAY2_PIN, OUTPUT);
  
  digitalWrite(RELAY1_PIN, LOW);
  digitalWrite(RELAY2_PIN, LOW);
  
  // 从 Flash 加载保存的阈值
  preferences.begin(THRESHOLD_NAMESPACE, true);
  if (preferences.isKey("temp") && preferences.isKey("light")) {
    temp_threshold = preferences.getFloat("temp", TEMP_THRESHOLD_DEFAULT);
    light_threshold = preferences.getUShort("light", LIGHT_THRESHOLD_DEFAULT);
    Serial.printf("Loaded thresholds from flash - Temp: %.1f, Light: %d\n", temp_threshold, light_threshold);
  }
  preferences.end();
}

void relay_task(void) {
  if (!system_enable) {
    digitalWrite(RELAY1_PIN, LOW);
    digitalWrite(RELAY2_PIN, LOW);
    state = 0;
    display_update_info(auto_mode, false, 0, 119, 0, 28.0, false, false, wifi_is_connected());
    return;
  }

  if (key2_long_2s) {
    auto_mode = !auto_mode;
    key2_long_2s = false;

    if (auto_mode) {
      state = 0;
      digitalWrite(RELAY1_PIN, LOW);
      digitalWrite(RELAY2_PIN, LOW);
      Serial.println("→ 自动模式：温度联动风扇，光照联动照明");
    } else {
      Serial.println("→ 手动模式：可以按键控制");
    }
  }

  if (auto_mode) {
    relay_auto_run();
    return;
  }

  relay_manual_run();
}

void relay_auto_run(void) {
  uint16_t light_raw = light_sensor_read_raw();
  float temp_c = temp_sensor_read_celsius();
  
  bool led_on = false;
  bool fan_on = false;
  
  // 使用用户设置的阈值判断光照
  uint16_t light_lux = (4095 - light_raw) * (4095 - light_raw) / 30000;
  if (light_lux < light_threshold) {
    digitalWrite(RELAY1_PIN, HIGH);
    led_on = true;
  } else {
    digitalWrite(RELAY1_PIN, LOW);
    led_on = false;
  }
  
  // 使用用户设置的阈值判断温度
  if (temp_c > temp_threshold) {
    digitalWrite(RELAY2_PIN, HIGH);
    fan_on = true;
  } else {
    digitalWrite(RELAY2_PIN, LOW);
    fan_on = false;
  }
  
  static unsigned long last_print = 0;
  if (millis() - last_print >= 2000) {
    last_print = millis();
    Serial.print("自动 | 光照:");
    Serial.print(light_lux);
    Serial.print(" lux | 阈值:");
    Serial.print(light_threshold);
    Serial.print(" | 照明:");
    Serial.print(led_on ? "开" : "关");
    Serial.print(" | 温度:");
    Serial.print(temp_c, 1);
    Serial.print("C | 阈值:");
    Serial.print(temp_threshold);
    Serial.print(" | 风扇:");
    Serial.println(fan_on ? "开" : "关");
    
    display_update_info(true, true, light_lux, light_threshold, temp_c, temp_threshold, led_on, fan_on, wifi_is_connected());
  }
}

void relay_manual_run(void) {
  if (key2_short) {
    state = (state + 1) % 4;
    key2_short = false;
    Serial.print("手动 → 状态:");
    Serial.println(state);
  }

  bool led_on = false;
  bool fan_on = false;
  
  switch(state) {
    case 0:
      digitalWrite(RELAY1_PIN, LOW);
      digitalWrite(RELAY2_PIN, LOW);
      led_on = false;
      fan_on = false;
      break;
    case 1:
      digitalWrite(RELAY1_PIN, LOW);
      digitalWrite(RELAY2_PIN, HIGH);
      led_on = false;
      fan_on = true;
      break;
    case 2:
      digitalWrite(RELAY1_PIN, HIGH);
      digitalWrite(RELAY2_PIN, LOW);
      led_on = true;
      fan_on = false;
      break;
    case 3:
      digitalWrite(RELAY1_PIN, HIGH);
      digitalWrite(RELAY2_PIN, HIGH);
      led_on = true;
      fan_on = true;
      break;
  }
  
  static unsigned long last_print = 0;
  if (millis() - last_print >= 2000) {
    last_print = millis();
    uint16_t light_raw = light_sensor_read_raw();
    uint16_t light_lux = (4095 - light_raw) * (4095 - light_raw) / 30000;
    float temp_c = temp_sensor_read_celsius();
    display_update_info(false, true, light_lux, 119, temp_c, 28.0, led_on, fan_on, wifi_is_connected());
  }
}

void relay_all_off(void) {
  digitalWrite(RELAY1_PIN, LOW);
  digitalWrite(RELAY2_PIN, LOW);
}

bool relay_get_relay1_status(void) {
  return digitalRead(RELAY1_PIN) == RELAY_ON;
}

bool relay_get_relay2_status(void) {
  return digitalRead(RELAY2_PIN) == RELAY_ON;
}

bool relay_set_relay1(bool on) {
  if (!system_enable) {
    Serial.println("System disabled, cannot set relay1");
    return false;
  }
  
  digitalWrite(RELAY1_PIN, on ? RELAY_ON : RELAY_OFF);
  Serial.printf("Relay1 set to: %s\n", on ? "ON" : "OFF");
  
  // 同步更新 state 变量，避免被 relay_manual_run() 覆盖
  if (auto_mode) {
    // 自动模式下，继电器由传感器控制，不需要更新 state
    return true;
  }
  
  bool relay2_status = digitalRead(RELAY2_PIN) == RELAY_ON;
  
  if (on && relay2_status) {
    state = 3;  // 11 - 两个都开
  } else if (on && !relay2_status) {
    state = 2;  // 10 - 只有继电器1开
  } else if (!on && relay2_status) {
    state = 1;  // 01 - 只有继电器2开
  } else {
    state = 0;  // 00 - 两个都关
  }
  
  return true;
}

bool relay_set_relay2(bool on) {
  if (!system_enable) {
    Serial.println("System disabled, cannot set relay2");
    return false;
  }
  
  digitalWrite(RELAY2_PIN, on ? RELAY_ON : RELAY_OFF);
  Serial.printf("Relay2 set to: %s\n", on ? "ON" : "OFF");
  
  // 同步更新 state 变量，避免被 relay_manual_run() 覆盖
  if (auto_mode) {
    // 自动模式下，继电器由传感器控制，不需要更新 state
    return true;
  }
  
  bool relay1_status = digitalRead(RELAY1_PIN) == RELAY_ON;
  
  if (relay1_status && on) {
    state = 3;  // 11 - 两个都开
  } else if (relay1_status && !on) {
    state = 2;  // 10 - 只有继电器1开
  } else if (!relay1_status && on) {
    state = 1;  // 01 - 只有继电器2开
  } else {
    state = 0;  // 00 - 两个都关
  }
  
  return true;
}

bool relay_set_mode(bool auto_mode_set) {
  if (auto_mode != auto_mode_set) {
    auto_mode = auto_mode_set;
    if (auto_mode) {
      state = 0;
      digitalWrite(RELAY1_PIN, LOW);
      digitalWrite(RELAY2_PIN, LOW);
      Serial.println("→ 自动模式：温度联动风扇，光照联动照明");
    } else {
      Serial.println("→ 手动模式：可以按键控制");
    }
  }
  return true;
}

void relay_set_temp_threshold(float temp) {
  temp_threshold = temp;
  Serial.printf("Temperature threshold set to: %.1f C\n", temp_threshold);
  
  // 保存到 Flash
  preferences.begin(THRESHOLD_NAMESPACE, false);
  preferences.putFloat("temp", temp_threshold);
  preferences.end();
  Serial.println("Temperature threshold saved to flash");
}

void relay_set_light_threshold(uint16_t light) {
  light_threshold = light;
  Serial.printf("Light threshold set to: %d lux\n", light_threshold);
  
  // 保存到 Flash
  preferences.begin(THRESHOLD_NAMESPACE, false);
  preferences.putUShort("light", light_threshold);
  preferences.end();
  Serial.println("Light threshold saved to flash");
}
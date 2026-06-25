#include "mqtt_connect.h"
#include "wifi_connect.h"
#include "relay.h"
#include "exti.h"
#include "light_sensor.h"
#include "temp_sensor.h"
#include <ArduinoJson.h>

static WiFiClient espClient;
static PubSubClient client(espClient);
static Preferences preferences;

static bool mqtt_connected = false;
static MQTTStatus mqtt_status = MQTT_STATUS_DISCONNECTED;
static MQTTConfig mqtt_config;
static unsigned long last_reconnect_time = 0;
static char status_topic[64];
static char command_topic[64];

// 状态变化检测变量
static bool last_key1_state = false;
static bool last_key2_state = false;
static bool last_wifi_state = false;
static bool last_relay1_state = false;
static bool last_relay2_state = false;
static bool last_auto_mode = false;

// get_status 频率限制
#define GET_STATUS_INTERVAL 5000  // 5秒间隔
static unsigned long last_get_status_time = 0;

// 配置模式标志
static bool mqtt_configuring = false;

void mqtt_callback(char* topic, byte* payload, unsigned int length);

void mqtt_init(void) {
  Serial.println("===== MQTT Connect =====");
  
  // 确保配置模式标志初始化为 false
  mqtt_configuring = false;
  
  if (mqtt_load_config(mqtt_config)) {
    Serial.println("Found saved MQTT configuration");
    mqtt_print_config();
  } else {
    Serial.println("No saved MQTT configuration, using defaults");
    strncpy(mqtt_config.server, MQTT_DEFAULT_SERVER, sizeof(mqtt_config.server) - 1);
    mqtt_config.port = MQTT_DEFAULT_PORT;
    strncpy(mqtt_config.username, MQTT_DEFAULT_USERNAME, sizeof(mqtt_config.username) - 1);
    strncpy(mqtt_config.password, MQTT_DEFAULT_PASSWORD, sizeof(mqtt_config.password) - 1);
    strncpy(mqtt_config.device_id, MQTT_DEFAULT_DEVICE_ID, sizeof(mqtt_config.device_id) - 1);
    mqtt_print_config();
  }
  
  snprintf(status_topic, sizeof(status_topic), "chemctrl/%s/status", mqtt_config.device_id);
  snprintf(command_topic, sizeof(command_topic), "chemctrl/%s/command", mqtt_config.device_id);
  
  client.setServer(mqtt_config.server, mqtt_config.port);
  client.setCallback(mqtt_callback);
  
  Serial.print("Status Topic: ");
  Serial.println(status_topic);
  Serial.print("Command Topic: ");
  Serial.println(command_topic);
}

void mqtt_callback(char* topic, byte* payload, unsigned int length) {
  // 配置模式下忽略所有MQTT命令
  if (mqtt_configuring) {
    Serial.println("MQTT callback ignored - in configuration mode");
    return;
  }
  
  Serial.print("Message arrived on topic: ");
  Serial.println(topic);
  
  char message[length + 1];
  memcpy(message, payload, length);
  message[length] = '\0';
  
  Serial.print("Message: ");
  Serial.println(message);
  
  DynamicJsonDocument doc(256);
  DeserializationError error = deserializeJson(doc, message);
  
  if (error) {
    Serial.print("Failed to parse JSON: ");
    Serial.println(error.c_str());
    return;
  }
  
  String cmd = doc["cmd"].as<String>();
  
  if (cmd == "set_relay") {
    int relay = doc["relay"];
    bool value = doc["value"];
    
    Serial.printf("set_relay: relay=%d, value=%s\n", relay, value ? "true" : "false");
    
    if (relay == 3) {
      relay_set_relay1(value);
    } else if (relay == 4) {
      relay_set_relay2(value);
    } else {
      Serial.println("Invalid relay number, use 3 or 4");
    }
    mqtt_report_status();
  } else if (cmd == "set_mode") {
    String mode = doc["mode"].as<String>();
    Serial.printf("set_mode: mode=%s\n", mode.c_str());
    relay_set_mode(mode == "auto");
    mqtt_report_status();
  } else if (cmd == "get_status") {
    Serial.println("get_status command received");
    // 5秒内只允许上报一次
    if (millis() - last_get_status_time >= GET_STATUS_INTERVAL) {
      last_get_status_time = millis();
      mqtt_report_status();
    } else {
      Serial.println("get_status rate limited, wait 5 seconds");
    }
  } else if (cmd == "set_threshold") {
    if (doc.containsKey("temp")) {
      float temp = doc["temp"];
      relay_set_temp_threshold(temp);
    }
    if (doc.containsKey("light")) {
      uint16_t light = doc["light"];
      relay_set_light_threshold(light);
    }
    mqtt_report_status();
  } else if (cmd == "reboot") {
    Serial.println("Reboot command received");
    Serial.println("Rebooting in 2 seconds...");
    delay(2000);
    ESP.restart();
  } else {
    Serial.printf("Unknown command: %s\n", cmd.c_str());
  }
}

void mqtt_task(void) {
  // 配置模式下跳过所有MQTT任务（除了client.loop保持连接）
  if (mqtt_configuring) {
    if (client.connected()) {
      client.loop();
    }
    return;
  }
  
  if (!wifi_is_connected()) {
    if (mqtt_connected) {
      mqtt_connected = false;
      mqtt_status = MQTT_STATUS_DISCONNECTED;
      Serial.println("WiFi disconnected, MQTT disconnected");
    }
    return;
  }
  
  if (!client.connected()) {
    if (mqtt_connected) {
      mqtt_connected = false;
      mqtt_status = MQTT_STATUS_RECONNECTING;
      Serial.println("\nMQTT disconnected!");
      Serial.println("Reconnecting...");
    }
    
    if (millis() - last_reconnect_time >= MQTT_RECONNECT_INTERVAL) {
      last_reconnect_time = millis();
      
      mqtt_status = MQTT_STATUS_CONNECTING;
      
      if (client.connect(mqtt_config.device_id, mqtt_config.username, mqtt_config.password)) {
        mqtt_connected = true;
        mqtt_status = MQTT_STATUS_CONNECTED;
        Serial.println("MQTT connected successfully!");
        
        client.subscribe(command_topic);
        Serial.printf("Subscribed to command topic: %s\n", command_topic);
        
        // 连接成功时立即上报一次状态，并初始化状态变量
        mqtt_report_status();
        last_key1_state = system_enable;
        last_auto_mode = auto_mode;
        last_wifi_state = true;
        last_relay1_state = relay_get_relay1_status();
        last_relay2_state = relay_get_relay2_status();
      } else {
        Serial.print("MQTT connection failed, rc=");
        Serial.print(client.state());
        Serial.println(" try again in 5 seconds");
      }
    }
  } else {
    if (!mqtt_connected) {
      mqtt_connected = true;
      mqtt_status = MQTT_STATUS_CONNECTED;
      Serial.println("MQTT reconnected successfully!");
      
      // 重连成功时立即上报一次状态，并初始化状态变量
      mqtt_report_status();
      last_key1_state = system_enable;
      last_auto_mode = auto_mode;
      last_wifi_state = true;
      last_relay1_state = relay_get_relay1_status();
      last_relay2_state = relay_get_relay2_status();
    }
    
    client.loop();
    
    // 状态变化检测：只有状态变化时才上报
    bool current_key1 = system_enable;
    bool current_auto_mode = auto_mode;
    bool current_wifi = wifi_is_connected();
    bool current_relay1 = relay_get_relay1_status();
    bool current_relay2 = relay_get_relay2_status();
    
    if (current_key1 != last_key1_state ||
        current_auto_mode != last_auto_mode ||
        current_wifi != last_wifi_state ||
        current_relay1 != last_relay1_state ||
        current_relay2 != last_relay2_state) {
      
      Serial.println("State changed, reporting...");
      mqtt_report_status();
      
      // 更新状态变量
      last_key1_state = current_key1;
      last_auto_mode = current_auto_mode;
      last_wifi_state = current_wifi;
      last_relay1_state = current_relay1;
      last_relay2_state = current_relay2;
    }
  }
}

void mqtt_report_status(void) {
  if (!mqtt_connected) {
    return;
  }
  
  StaticJsonDocument<256> doc;
  
  float temp_c = temp_sensor_read_celsius();
  uint16_t light_raw = light_sensor_read_raw();
  uint16_t light_lux = (4095 - light_raw) * (4095 - light_raw) / 30000;
  
  doc["temperature"] = temp_c;
  doc["light"] = light_lux;
  doc["mode"] = auto_mode ? "auto" : "manual";
  doc["key1_lock"] = system_enable;
  doc["relay3"] = relay_get_relay1_status();
  doc["relay4"] = relay_get_relay2_status();
  doc["temp_threshold"] = temp_threshold;
  doc["light_threshold"] = light_threshold;
  
  String output;
  serializeJson(doc, output);
  
  Serial.print("Reporting status: ");
  Serial.println(output);
  
  client.publish(status_topic, output.c_str());
}

void mqtt_check_serial_command(void) {
  if (!Serial.available()) {
    return;
  }
  
  String input = Serial.readStringUntil('\n');
  input.trim();
  
  // 如果正在配置中，忽略其他命令
  if (mqtt_configuring) {
    // 在配置模式下，只有 esc 或空输入有意义，其他命令忽略
    if (input.length() > 0 && !input.equalsIgnoreCase("esc")) {
      Serial.println("Ignoring command - in configuration mode");
    }
    return;
  }
  
  if (input.equalsIgnoreCase("mqtt_config")) {
    Serial.println("MQTT config command received");
    mqtt_serial_config();
  } else if (input.equalsIgnoreCase("mqtt_clear")) {
    Serial.println("MQTT clear config command received");
    mqtt_clear_config();
  } else if (input.equalsIgnoreCase("mqtt_status")) {
    Serial.println("MQTT status command received");
    mqtt_print_config();
    Serial.printf("MQTT connected: %s\n", mqtt_connected ? "true" : "false");
  } else if (input.equalsIgnoreCase("mqtt_report")) {
    Serial.println("MQTT report command received");
    mqtt_report_status();
  }
}

bool mqtt_is_connected(void) {
  return mqtt_connected;
}

MQTTStatus mqtt_get_status(void) {
  return mqtt_status;
}

void mqtt_save_config(const char* server, int port, const char* username, const char* password, const char* device_id) {
  preferences.begin(MQTT_CONFIG_NAMESPACE, false);
  preferences.putString("server", server);
  preferences.putInt("port", port);
  preferences.putString("username", username);
  preferences.putString("password", password);
  preferences.putString("device_id", device_id);
  preferences.end();
  
  Serial.println("MQTT configuration saved to flash!");
}

bool mqtt_load_config(MQTTConfig &config) {
  preferences.begin(MQTT_CONFIG_NAMESPACE, true);
  
  if (preferences.isKey("server") && preferences.isKey("port") && 
      preferences.isKey("username") && preferences.isKey("password") && 
      preferences.isKey("device_id")) {
    
    String server = preferences.getString("server", "");
    int port = preferences.getInt("port", MQTT_DEFAULT_PORT);
    String username = preferences.getString("username", "");
    String password = preferences.getString("password", "");
    String device_id = preferences.getString("device_id", "");
    
    strncpy(config.server, server.c_str(), sizeof(config.server) - 1);
    config.port = port;
    strncpy(config.username, username.c_str(), sizeof(config.username) - 1);
    strncpy(config.password, password.c_str(), sizeof(config.password) - 1);
    strncpy(config.device_id, device_id.c_str(), sizeof(config.device_id) - 1);
    
    preferences.end();
    return true;
  }
  
  preferences.end();
  return false;
}

void mqtt_clear_config(void) {
  preferences.begin(MQTT_CONFIG_NAMESPACE, false);
  preferences.clear();
  preferences.end();
  Serial.println("MQTT configuration cleared!");
}

void mqtt_print_config(void) {
  Serial.println("------------------------------");
  Serial.print("MQTT Server: ");
  Serial.print(mqtt_config.server);
  Serial.print(":");
  Serial.println(mqtt_config.port);
  Serial.print("Username: ");
  Serial.println(mqtt_config.username);
  Serial.print("Device ID: ");
  Serial.println(mqtt_config.device_id);
  Serial.println("------------------------------");
}

void mqtt_serial_config(void) {
  mqtt_configuring = true;
  mqtt_status = MQTT_STATUS_CONFIGURING;
  
  Serial.println("\n===== MQTT Configuration =====");
  Serial.println("Enter 'esc' at any time to exit without saving");
  
  Serial.println("\nCurrent MQTT configuration:");
  mqtt_print_config();
  
  Serial.println("\nEnter new MQTT server (or press Enter to keep current, 'esc' to exit):");
  while (!Serial.available()) {
    delay(100);
  }
  String server_input = Serial.readStringUntil('\n');
  server_input.trim();
  if (server_input.equalsIgnoreCase("esc")) {
    Serial.println("Exiting MQTT configuration without saving...");
    mqtt_status = MQTT_STATUS_DISCONNECTED;
    mqtt_configuring = false;
    return;
  }
  if (server_input.length() > 0) {
    strncpy(mqtt_config.server, server_input.c_str(), sizeof(mqtt_config.server) - 1);
  }
  
  Serial.println("Enter new MQTT port (or press Enter to keep current, 'esc' to exit):");
  while (!Serial.available()) {
    delay(100);
  }
  String port_input = Serial.readStringUntil('\n');
  port_input.trim();
  if (port_input.equalsIgnoreCase("esc")) {
    Serial.println("Exiting MQTT configuration without saving...");
    mqtt_status = MQTT_STATUS_DISCONNECTED;
    mqtt_configuring = false;
    return;
  }
  if (port_input.length() > 0) {
    mqtt_config.port = port_input.toInt();
  }
  
  Serial.println("Enter new MQTT username (or press Enter to keep current, 'esc' to exit):");
  while (!Serial.available()) {
    delay(100);
  }
  String username_input = Serial.readStringUntil('\n');
  username_input.trim();
  if (username_input.equalsIgnoreCase("esc")) {
    Serial.println("Exiting MQTT configuration without saving...");
    mqtt_status = MQTT_STATUS_DISCONNECTED;
    mqtt_configuring = false;
    return;
  }
  if (username_input.length() > 0) {
    strncpy(mqtt_config.username, username_input.c_str(), sizeof(mqtt_config.username) - 1);
  }
  
  Serial.println("Enter new MQTT password (or press Enter to keep current, 'esc' to exit):");
  while (!Serial.available()) {
    delay(100);
  }
  String password_input = Serial.readStringUntil('\n');
  password_input.trim();
  if (password_input.equalsIgnoreCase("esc")) {
    Serial.println("Exiting MQTT configuration without saving...");
    mqtt_status = MQTT_STATUS_DISCONNECTED;
    mqtt_configuring = false;
    return;
  }
  if (password_input.length() > 0) {
    strncpy(mqtt_config.password, password_input.c_str(), sizeof(mqtt_config.password) - 1);
  }
  
  Serial.println("Enter new Device ID (or press Enter to keep current, 'esc' to exit):");
  while (!Serial.available()) {
    delay(100);
  }
  String device_id_input = Serial.readStringUntil('\n');
  device_id_input.trim();
  if (device_id_input.equalsIgnoreCase("esc")) {
    Serial.println("Exiting MQTT configuration without saving...");
    mqtt_status = MQTT_STATUS_DISCONNECTED;
    mqtt_configuring = false;
    return;
  }
  if (device_id_input.length() > 0) {
    strncpy(mqtt_config.device_id, device_id_input.c_str(), sizeof(mqtt_config.device_id) - 1);
  }
  
  Serial.println("\nNew MQTT configuration:");
  mqtt_print_config();
  
  mqtt_save_config(mqtt_config.server, mqtt_config.port, mqtt_config.username, mqtt_config.password, mqtt_config.device_id);
  
  snprintf(status_topic, sizeof(status_topic), "chemctrl/%s/status", mqtt_config.device_id);
  snprintf(command_topic, sizeof(command_topic), "chemctrl/%s/command", mqtt_config.device_id);
  
  client.setServer(mqtt_config.server, mqtt_config.port);
  
  if (mqtt_connected) {
    client.disconnect();
    mqtt_connected = false;
    mqtt_status = MQTT_STATUS_DISCONNECTED;
  }
  
  last_reconnect_time = 0;
  mqtt_status = MQTT_STATUS_DISCONNECTED;
  mqtt_configuring = false;
  
  Serial.println("MQTT configuration updated! Will reconnect...");
}

// 细分配置指令实现
void mqtt_set_server(const char* server) {
  strncpy(mqtt_config.server, server, sizeof(mqtt_config.server) - 1);
  mqtt_save_config(mqtt_config.server, mqtt_config.port, mqtt_config.username, mqtt_config.password, mqtt_config.device_id);
  
  client.setServer(mqtt_config.server, mqtt_config.port);
  
  if (mqtt_connected) {
    client.disconnect();
    mqtt_connected = false;
    mqtt_status = MQTT_STATUS_DISCONNECTED;
  }
  
  last_reconnect_time = 0;
  Serial.printf("MQTT server set to: %s\n", mqtt_config.server);
}

void mqtt_set_port(int port) {
  mqtt_config.port = port;
  mqtt_save_config(mqtt_config.server, mqtt_config.port, mqtt_config.username, mqtt_config.password, mqtt_config.device_id);
  
  client.setServer(mqtt_config.server, mqtt_config.port);
  
  if (mqtt_connected) {
    client.disconnect();
    mqtt_connected = false;
    mqtt_status = MQTT_STATUS_DISCONNECTED;
  }
  
  last_reconnect_time = 0;
  Serial.printf("MQTT port set to: %d\n", mqtt_config.port);
}

void mqtt_set_username(const char* username) {
  strncpy(mqtt_config.username, username, sizeof(mqtt_config.username) - 1);
  mqtt_save_config(mqtt_config.server, mqtt_config.port, mqtt_config.username, mqtt_config.password, mqtt_config.device_id);
  
  if (mqtt_connected) {
    client.disconnect();
    mqtt_connected = false;
    mqtt_status = MQTT_STATUS_DISCONNECTED;
  }
  
  last_reconnect_time = 0;
  Serial.printf("MQTT username set to: %s\n", mqtt_config.username);
}

void mqtt_set_password(const char* password) {
  strncpy(mqtt_config.password, password, sizeof(mqtt_config.password) - 1);
  mqtt_save_config(mqtt_config.server, mqtt_config.port, mqtt_config.username, mqtt_config.password, mqtt_config.device_id);
  
  if (mqtt_connected) {
    client.disconnect();
    mqtt_connected = false;
    mqtt_status = MQTT_STATUS_DISCONNECTED;
  }
  
  last_reconnect_time = 0;
  Serial.println("MQTT password updated");
}

void mqtt_set_device_id(const char* device_id) {
  strncpy(mqtt_config.device_id, device_id, sizeof(mqtt_config.device_id) - 1);
  mqtt_save_config(mqtt_config.server, mqtt_config.port, mqtt_config.username, mqtt_config.password, mqtt_config.device_id);
  
  snprintf(status_topic, sizeof(status_topic), "chemctrl/%s/status", mqtt_config.device_id);
  snprintf(command_topic, sizeof(command_topic), "chemctrl/%s/command", mqtt_config.device_id);
  
  if (mqtt_connected) {
    client.disconnect();
    mqtt_connected = false;
    mqtt_status = MQTT_STATUS_DISCONNECTED;
  }
  
  last_reconnect_time = 0;
  Serial.printf("MQTT device ID set to: %s\n", mqtt_config.device_id);
}

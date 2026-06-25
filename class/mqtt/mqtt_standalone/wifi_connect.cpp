#include "wifi_connect.h"
#include <Preferences.h>

static Preferences preferences;
static WiFiStatus wifi_status = WIFI_STATUS_DISCONNECTED;
static unsigned long last_reconnect_time = 0;
static bool wifi_enabled = true;

void wifi_init(void) {
  Serial.println("===== WiFi Connect (Standalone) =====");
  
  preferences.begin(WIFI_CONFIG_NAMESPACE, true);
  String ssid = preferences.getString("ssid", "");
  String password = preferences.getString("password", "");
  preferences.end();
  
  if (ssid.length() > 0 && password.length() > 0) {
    Serial.printf("Found saved WiFi: %s\n", ssid.c_str());
    WiFi.begin(ssid.c_str(), password.c_str());
    wifi_status = WIFI_STATUS_CONNECTING;
  } else {
    Serial.println("No saved WiFi configuration found");
    wifi_status = WIFI_STATUS_DISCONNECTED;
    
    Serial.println("\nScanning for WiFi networks...");
    int n = WiFi.scanNetworks();
    Serial.printf("Found %d networks:\n", n);
    
    for (int i = 0; i < n; i++) {
      Serial.printf("%d: %s (%d%%)\n", i + 1, WiFi.SSID(i).c_str(), WiFi.RSSI(i));
    }
    
    Serial.println("\nPlease configure WiFi via serial:");
    Serial.println("Format: wifi_config <ssid> <password>");
  }
}

void wifi_task(void) {
  if (!wifi_enabled) {
    if (WiFi.status() == WL_CONNECTED) {
      WiFi.disconnect();
      wifi_status = WIFI_STATUS_DISCONNECTED;
      Serial.println("WiFi disabled");
    }
    return;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    if (wifi_status != WIFI_STATUS_CONNECTED) {
      wifi_status = WIFI_STATUS_CONNECTED;
      Serial.println("\nWiFi connected successfully!");
      Serial.print("IP Address: ");
      Serial.println(WiFi.localIP());
      Serial.print("Signal Strength: ");
      Serial.println(WiFi.RSSI());
    }
    return;
  }
  
  if (wifi_status != WIFI_STATUS_RECONNECTING) {
    wifi_status = WIFI_STATUS_RECONNECTING;
    Serial.println("WiFi disconnected, attempting to reconnect...");
  }
  
  if (millis() - last_reconnect_time >= WIFI_RECONNECT_INTERVAL) {
    last_reconnect_time = millis();
    
    preferences.begin(WIFI_CONFIG_NAMESPACE, true);
    String ssid = preferences.getString("ssid", "");
    String password = preferences.getString("password", "");
    preferences.end();
    
    if (ssid.length() > 0 && password.length() > 0) {
      WiFi.begin(ssid.c_str(), password.c_str());
    }
  }
}

bool wifi_is_connected(void) {
  return (WiFi.status() == WL_CONNECTED);
}

WiFiStatus wifi_get_status(void) {
  return wifi_status;
}

void wifi_clear_config(void) {
  preferences.begin(WIFI_CONFIG_NAMESPACE, false);
  preferences.clear();
  preferences.end();
  Serial.println("WiFi configuration cleared!");
}

void wifi_reconnect_now(void) {
  last_reconnect_time = 0;
  Serial.println("Will attempt to reconnect to WiFi...");
}

void wifi_set_enabled(bool enabled) {
  wifi_enabled = enabled;
}

void wifi_check_serial_command(String input) {
  if (input.startsWith("wifi_config ")) {
    int space_pos = input.indexOf(' ', 12);
    if (space_pos > 0) {
      String ssid = input.substring(12, space_pos);
      String password = input.substring(space_pos + 1);
      
      preferences.begin(WIFI_CONFIG_NAMESPACE, false);
      preferences.putString("ssid", ssid);
      preferences.putString("password", password);
      preferences.end();
      
      Serial.printf("WiFi configuration saved: %s\n", ssid.c_str());
      Serial.println("Connecting to WiFi...");
      
      WiFi.begin(ssid.c_str(), password.c_str());
      last_reconnect_time = millis();
    } else {
      Serial.println("Invalid format. Use: wifi_config <ssid> <password>");
    }
  } else if (input.equalsIgnoreCase("wifi_clear")) {
    wifi_clear_config();
  } else if (input.equalsIgnoreCase("wifi_status")) {
    Serial.printf("WiFi status: %s\n", WiFi.status() == WL_CONNECTED ? "Connected" : "Disconnected");
    if (WiFi.status() == WL_CONNECTED) {
      Serial.print("IP: ");
      Serial.println(WiFi.localIP());
    }
  } else if (input.equalsIgnoreCase("disconnect")) {
    wifi_set_enabled(false);
    Serial.println("WiFi disabled. Use 'connect' to re-enable.");
  } else if (input.equalsIgnoreCase("connect")) {
    wifi_set_enabled(true);
    wifi_reconnect_now();
    Serial.println("WiFi enabled. Will attempt to reconnect...");
  }
}
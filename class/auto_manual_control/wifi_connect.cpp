#include "wifi_connect.h"

static bool wifi_connected = false;
static unsigned long last_reconnect_time = 0;
static WiFiStatus wifi_status = WIFI_STATUS_DISCONNECTED;
static Preferences preferences;
static String current_ssid = "";
static String current_password = "";
static int reconnect_fail_count = 0;
static bool wifi_enabled = true;
#define MAX_RECONNECT_FAILS 3

void wifi_init(void) {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  
  Serial.println("===== WiFi Connect =====");
  
  String saved_ssid, saved_password;
  
  if (wifi_load_config(saved_ssid, saved_password)) {
    Serial.println("Found saved WiFi configuration");
    Serial.print("SSID: ");
    Serial.println(saved_ssid);
    
    current_ssid = saved_ssid;
    current_password = saved_password;
    
    wifi_status = WIFI_STATUS_CONNECTING;
    WiFi.begin(current_ssid.c_str(), current_password.c_str());
    
    unsigned long start_time = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - start_time < WIFI_CONNECT_TIMEOUT) {
      delay(500);
      Serial.print(".");
    }
    Serial.println();
    
    if (WiFi.status() == WL_CONNECTED) {
      wifi_connected = true;
      wifi_status = WIFI_STATUS_CONNECTED;
      Serial.println("WiFi connected successfully!");
      wifi_print_info();
      return;
    } else {
      Serial.println("Failed to connect with saved config!");
    }
  } else {
    Serial.println("No saved WiFi configuration found!");
  }
  
  wifi_serial_config();
}

void wifi_task(void) {
  if (!wifi_enabled) {
    if (WiFi.status() == WL_CONNECTED) {
      WiFi.disconnect();
      wifi_connected = false;
      wifi_status = WIFI_STATUS_DISCONNECTED;
      Serial.println("WiFi disconnected by user command");
    }
    return;
  }

  if (WiFi.status() != WL_CONNECTED) {
    if (wifi_connected) {
      wifi_connected = false;
      wifi_status = WIFI_STATUS_RECONNECTING;
      reconnect_fail_count = 0;
      Serial.println("\nWiFi disconnected!");
      Serial.println("Reconnecting...");
    }
    
    if (current_ssid.length() > 0 && millis() - last_reconnect_time >= WIFI_RECONNECT_INTERVAL) {
      last_reconnect_time = millis();
      Serial.println("Attempting to reconnect...");
      WiFi.reconnect();
      
      unsigned long start_time = millis();
      while (WiFi.status() != WL_CONNECTED && millis() - start_time < WIFI_CONNECT_TIMEOUT) {
        delay(500);
        Serial.print(".");
      }
      Serial.println();
      
      if (WiFi.status() == WL_CONNECTED) {
        wifi_connected = true;
        wifi_status = WIFI_STATUS_CONNECTED;
        reconnect_fail_count = 0;
        Serial.println("WiFi reconnected successfully!");
        wifi_print_info();
      } else {
        reconnect_fail_count++;
        Serial.printf("Reconnect failed (%d/%d)\n", reconnect_fail_count, MAX_RECONNECT_FAILS);
        
        if (reconnect_fail_count >= MAX_RECONNECT_FAILS) {
          Serial.println("Max reconnect attempts reached!");
          Serial.println("Entering WiFi configuration mode...");
          reconnect_fail_count = 0;
          wifi_serial_config();
        }
      }
    }
  } else {
    if (!wifi_connected) {
      wifi_connected = true;
      wifi_status = WIFI_STATUS_CONNECTED;
      reconnect_fail_count = 0;
      Serial.println("WiFi reconnected successfully!");
      wifi_print_info();
    }
  }
}

void wifi_check_serial_command(void) {
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    input.trim();
    
    if (input.equalsIgnoreCase("disconnect")) {
      Serial.println("WiFi disconnect command received");
      wifi_enabled = false;
      Serial.println("WiFi disabled. Use 'connect' to re-enable.");
    } else if (input.equalsIgnoreCase("connect")) {
      Serial.println("WiFi connect command received");
      wifi_enabled = true;
      Serial.println("WiFi enabled. Will attempt to reconnect...");
      last_reconnect_time = 0;
    }
  }
}

bool wifi_is_connected(void) {
  return wifi_connected;
}

void wifi_reconnect_now(void) {
  last_reconnect_time = 0;
  Serial.println("WiFi reconnect requested");
}

void wifi_set_enabled(bool enabled) {
  wifi_enabled = enabled;
}

WiFiStatus wifi_get_status(void) {
  return wifi_status;
}

String wifi_get_ip(void) {
  if (wifi_connected) {
    return WiFi.localIP().toString();
  }
  return "0.0.0.0";
}

String wifi_get_gateway(void) {
  if (wifi_connected) {
    return WiFi.gatewayIP().toString();
  }
  return "0.0.0.0";
}

String wifi_get_subnet(void) {
  if (wifi_connected) {
    return WiFi.subnetMask().toString();
  }
  return "0.0.0.0";
}

String wifi_get_ssid(void) {
  if (wifi_connected) {
    return WiFi.SSID();
  }
  return "";
}

int32_t wifi_get_rssi(void) {
  if (wifi_connected) {
    return WiFi.RSSI();
  }
  return 0;
}

int32_t wifi_scan_networks(void) {
  if (!wifi_enabled) {
    Serial.println("WiFi is disabled. Use 'connect' to enable.");
    return 0;
  }
  
  Serial.println("\nScanning WiFi networks...");
  WiFi.disconnect();
  delay(100);
  
  wifi_status = WIFI_STATUS_SCANNING;
  int32_t network_count = WiFi.scanNetworks();
  
  Serial.print("Found ");
  Serial.print(network_count);
  Serial.println(" WiFi networks:");
  Serial.println("------------------------------");
  
  for (int i = 0; i < network_count; i++) {
    String encryption = "OPEN";
    if (WiFi.encryptionType(i) == WIFI_AUTH_WEP) encryption = "WEP";
    else if (WiFi.encryptionType(i) == WIFI_AUTH_WPA_PSK) encryption = "WPA";
    else if (WiFi.encryptionType(i) == WIFI_AUTH_WPA2_PSK) encryption = "WPA2";
    else if (WiFi.encryptionType(i) == WIFI_AUTH_WPA_WPA2_PSK) encryption = "WPA/WPA2";
    
    Serial.printf("%2d. %-25s RSSI: %4d dBm  Ch:%d  %s\n", 
                  i + 1, 
                  WiFi.SSID(i).c_str(), 
                  WiFi.RSSI(i), 
                  WiFi.channel(i),
                  encryption.c_str());
  }
  Serial.println("------------------------------");
  
  return network_count;
}

void wifi_serial_config(void) {
  if (!wifi_enabled) {
    Serial.println("WiFi is disabled. Use 'connect' command to enable.");
    return;
  }
  
  wifi_status = WIFI_STATUS_CONFIGURING;
  
  while (true) {
    if (!wifi_enabled) {
      Serial.println("WiFi disabled during configuration");
      wifi_status = WIFI_STATUS_DISCONNECTED;
      return;
    }
    
    int32_t network_count = wifi_scan_networks();
    
    if (network_count == 0) {
      Serial.println("No WiFi networks found!");
      Serial.println("Enter 'esc' to skip WiFi and continue system, or wait 5 seconds to retry...");
      
      unsigned long start_time = millis();
      while (millis() - start_time < 5000) {
        if (Serial.available()) {
          String input = Serial.readStringUntil('\n');
          input.trim();
          if (input.equalsIgnoreCase("esc")) {
            Serial.println("Skipping WiFi configuration...");
            Serial.println("Continuing system without WiFi...");
            wifi_status = WIFI_STATUS_DISCONNECTED;
            return;
          } else if (input.equalsIgnoreCase("disconnect")) {
            Serial.println("WiFi disabled by user");
            wifi_enabled = false;
            wifi_status = WIFI_STATUS_DISCONNECTED;
            return;
          }
        }
        delay(100);
      }
      continue;
    }
    
    Serial.println("\nPlease select WiFi network (enter number):");
    Serial.println("Or enter 'esc' to skip WiFi and continue system");
    
    while (!Serial.available()) {
      delay(100);
    }
    
    String input = Serial.readStringUntil('\n');
    input.trim();
    
    if (input.equalsIgnoreCase("esc")) {
      Serial.println("Skipping WiFi configuration...");
      Serial.println("Continuing system without WiFi...");
      wifi_status = WIFI_STATUS_DISCONNECTED;
      return;
    }
    
    if (input.equalsIgnoreCase("disconnect")) {
      Serial.println("WiFi disabled by user");
      wifi_enabled = false;
      wifi_status = WIFI_STATUS_DISCONNECTED;
      return;
    }
    
    int selection = input.toInt();
    
    if (selection < 1 || selection > network_count) {
      Serial.println("Invalid selection!");
      continue;
    }
    
    current_ssid = WiFi.SSID(selection - 1);
    Serial.print("Selected: ");
    Serial.println(current_ssid);
    
    Serial.println("Enter WiFi password (press Enter when done):");
    while (!Serial.available()) {
      delay(100);
    }
    
    current_password = Serial.readStringUntil('\n');
    current_password.trim();
    
    Serial.print("Connecting to ");
    Serial.print(current_ssid);
    Serial.println("...");
    
    WiFi.begin(current_ssid.c_str(), current_password.c_str());
    
    unsigned long start_time = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - start_time < WIFI_CONNECT_TIMEOUT) {
      delay(500);
      Serial.print(".");
    }
    Serial.println();
    
    if (WiFi.status() == WL_CONNECTED) {
      wifi_connected = true;
      wifi_status = WIFI_STATUS_CONNECTED;
      Serial.println("\nWiFi connected successfully!");
      wifi_print_info();
      
      wifi_save_config(current_ssid.c_str(), current_password.c_str());
      Serial.println("WiFi configuration saved to flash!");
      break;
    } else {
      Serial.println("Connection failed! Please try again or enter 'esc' to skip.");
      current_ssid = "";
      current_password = "";
    }
  }
}

void wifi_save_config(const char* ssid, const char* password) {
  preferences.begin(WIFI_CONFIG_NAMESPACE, false);
  preferences.putString("ssid", ssid);
  preferences.putString("password", password);
  preferences.end();
}

bool wifi_load_config(String &ssid, String &password) {
  preferences.begin(WIFI_CONFIG_NAMESPACE, true);
  
  if (preferences.isKey("ssid") && preferences.isKey("password")) {
    ssid = preferences.getString("ssid", "");
    password = preferences.getString("password", "");
    preferences.end();
    return (ssid.length() > 0 && password.length() > 0);
  }
  
  preferences.end();
  return false;
}

void wifi_clear_config(void) {
  preferences.begin(WIFI_CONFIG_NAMESPACE, false);
  preferences.clear();
  preferences.end();
  Serial.println("WiFi configuration cleared!");
}

void wifi_print_info(void) {
  Serial.println("------------------------------");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.print("Gateway: ");
  Serial.println(WiFi.gatewayIP());
  Serial.print("Subnet Mask: ");
  Serial.println(WiFi.subnetMask());
  Serial.print("DNS: ");
  Serial.println(WiFi.dnsIP());
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
  Serial.print("RSSI: ");
  Serial.print(WiFi.RSSI());
  Serial.println(" dBm");
  Serial.println("------------------------------");
}
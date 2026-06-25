#include "wifi_connect.h"

static bool wifi_connected = false;
static unsigned long last_reconnect_time = 0;
static WiFiStatus wifi_status = WIFI_STATUS_DISCONNECTED;
static Preferences preferences;
static String current_ssid = "";
static String current_password = "";

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
      Serial.println("Failed to connect with saved config, entering config mode...");
    }
  } else {
    Serial.println("No saved WiFi configuration found, entering config mode...");
  }
  
  wifi_serial_config();
}

void wifi_task(void) {
  if (WiFi.status() != WL_CONNECTED) {
    if (wifi_connected) {
      wifi_connected = false;
      wifi_status = WIFI_STATUS_RECONNECTING;
      Serial.println("WiFi disconnected!");
      Serial.println("Reconnecting...");
    }
    
    if (current_ssid.length() > 0 && millis() - last_reconnect_time >= WIFI_RECONNECT_INTERVAL) {
      last_reconnect_time = millis();
      Serial.println("Attempting to reconnect...");
      WiFi.reconnect();
    }
  } else {
    if (!wifi_connected) {
      wifi_connected = true;
      wifi_status = WIFI_STATUS_CONNECTED;
      Serial.println("WiFi reconnected successfully!");
      wifi_print_info();
    }
  }
}

bool wifi_is_connected(void) {
  return wifi_connected;
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
  wifi_status = WIFI_STATUS_CONFIGURING;
  
  while (true) {
    int32_t network_count = wifi_scan_networks();
    
    if (network_count == 0) {
      Serial.println("No WiFi networks found!");
      Serial.println("Retrying in 5 seconds...");
      delay(5000);
      continue;
    }
    
    Serial.println("\nPlease select WiFi network (enter number):");
    while (!Serial.available()) {
      delay(100);
    }
    
    int selection = Serial.parseInt();
    while (Serial.available() > 0) {
      Serial.read();
    }
    
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
      Serial.println("Connection failed! Please try again.");
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
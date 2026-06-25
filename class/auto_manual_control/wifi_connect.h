#ifndef __WIFI_CONNECT_H
#define __WIFI_CONNECT_H

#include <Arduino.h>
#include <WiFi.h>
#include <Preferences.h>

#define WIFI_RECONNECT_INTERVAL 5000
#define WIFI_CONNECT_TIMEOUT 15000
#define WIFI_CONFIG_NAMESPACE "wifi_config"

typedef enum {
    WIFI_STATUS_DISCONNECTED = 0,
    WIFI_STATUS_CONNECTING,
    WIFI_STATUS_CONNECTED,
    WIFI_STATUS_RECONNECTING,
    WIFI_STATUS_SCANNING,
    WIFI_STATUS_CONFIGURING
} WiFiStatus;

void wifi_init(void);
void wifi_task(void);
void wifi_check_serial_command(void);
bool wifi_is_connected(void);
WiFiStatus wifi_get_status(void);
String wifi_get_ip(void);
String wifi_get_gateway(void);
String wifi_get_subnet(void);
String wifi_get_ssid(void);
int32_t wifi_get_rssi(void);
void wifi_reconnect_now(void);
void wifi_set_enabled(bool enabled);
int32_t wifi_scan_networks(void);
void wifi_serial_config(void);
void wifi_save_config(const char* ssid, const char* password);
bool wifi_load_config(String &ssid, String &password);
void wifi_clear_config(void);
void wifi_print_info(void);

#endif
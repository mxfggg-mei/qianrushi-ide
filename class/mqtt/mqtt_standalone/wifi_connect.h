#ifndef __WIFI_CONNECT_H
#define __WIFI_CONNECT_H

#include <Arduino.h>
#include <WiFi.h>

#define WIFI_CONFIG_NAMESPACE "wifi_config"
#define WIFI_RECONNECT_INTERVAL 5000
#define WIFI_MAX_SCAN_RETRY 3

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
bool wifi_is_connected(void);
WiFiStatus wifi_get_status(void);
void wifi_clear_config(void);
void wifi_reconnect_now(void);
void wifi_set_enabled(bool enabled);

#endif
#ifndef __MQTT_CONNECT_H
#define __MQTT_CONNECT_H

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <Preferences.h>

#define MQTT_CONFIG_NAMESPACE "mqtt_config"
#define MQTT_RECONNECT_INTERVAL 5000
#define MQTT_STATUS_REPORT_INTERVAL 10000
#define MQTT_DEFAULT_PORT 8081
#define MQTT_DEFAULT_SERVER "47.98.170.180"
#define MQTT_DEFAULT_USERNAME "dzdx_emqx"
#define MQTT_DEFAULT_PASSWORD "Jp4!sQ7$"
#define MQTT_DEFAULT_DEVICE_ID "PCT_100_006"

typedef enum {
    MQTT_STATUS_DISCONNECTED = 0,
    MQTT_STATUS_CONNECTING,
    MQTT_STATUS_CONNECTED,
    MQTT_STATUS_RECONNECTING,
    MQTT_STATUS_CONFIGURING
} MQTTStatus;

typedef struct {
    char server[64];
    int port;
    char username[32];
    char password[32];
    char device_id[32];
} MQTTConfig;

void mqtt_init(void);
void mqtt_task(void);
void mqtt_check_serial_command(void);
bool mqtt_is_connected(void);
MQTTStatus mqtt_get_status(void);
void mqtt_report_status(void);
void mqtt_save_config(const char* server, int port, const char* username, const char* password, const char* device_id);
bool mqtt_load_config(MQTTConfig &config);
void mqtt_clear_config(void);
void mqtt_print_config(void);
void mqtt_serial_config(void);

// 细分配置指令
void mqtt_set_server(const char* server);
void mqtt_set_port(int port);
void mqtt_set_username(const char* username);
void mqtt_set_password(const char* password);
void mqtt_set_device_id(const char* device_id);

#endif

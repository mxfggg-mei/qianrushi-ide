#MQTT 通信协议
MQTT TCP（设备/PC端）：47.98.170.180:8081
MQTT 协议版本：3.1.1（protocolVersion: 4）
用户名：dzdx_emqx
密码：Jp4!sQ7$
device_id 为设备表面丝印序列号，每台设备唯一标识。

##数据上报 Topic（设备 → 服务器）
Topic: chemctrl/{device_id}/status
Payload (JSON):
{
  "temperature": 25.3,
  "light": 680,
  "mode": "auto",
  "key1_lock": true,
  "relay3": false,
  "relay4": true,
  "temp_threshold": 40.0,
  "light_threshold": 320
}

##远程控制 Topic（服务器 → 设备）
Topic: chemctrl/{device_id}/command
Payload (JSON):
{
  "cmd": "set_relay",
  "relay": 3,
  "value": true
}

Payload (JSON):
{
  "cmd": "get_status"
}

Payload (JSON):
{
  "cmd": "set_threshold",
  "temp": 40.0
}

命令	参数	说明
set_relay	relay: 3-4, value: true/false	控制继电器（受 KEY1 约束；relay3 灯光，relay4 风机）
set_mode	mode: "auto"/"manual"	切换工作模式
get_status	—	查询设备状态，设备收到后立即上报一次 chemctrl/{id}/status
set_threshold	temp: 数值, light: 数值	设置温度高值阈值和光照低值阈值
reboot	—	远程重启设备


#include <PubSubClient.h>
#include <ArduinoJson.h>






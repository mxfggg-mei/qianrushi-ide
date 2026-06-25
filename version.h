#ifndef __VERSION_H
#define __VERSION_H

/* ==================== 版本信息 ==================== */
#define VERSION         "V2.6.50"     // 当前版本号
#define VERSION_DATE    "2026-06-05"    // 版本发布日期

/* ==================== 设备信息 ==================== */
#define DEVICE_ID       "PCT_100_006"   // 设备ID，FairyCat

/* ==================== 项目信息 ==================== */
#define PROJECT_NAME    "PCT_100_CTL_006"   // 项目名称
#define PROJECT_AUTHOR  "FairyCat"      // 项目作者
#define PROJECT_DATE    "2026-05-25"    // 项目创建日期

/* ==================== 功能列表 ==================== */

/* 1. 按键检测功能 */
/* 对应文件夹: class/key jiance/key/ */
/* 功能说明: 检测GPIO21引脚的按键状态，带软件消抖功能 */
#define FEATURE_KEY         1   // 按键检测功能（已实现）

/* 2. LED闪烁功能 */
/* 对应文件夹: class/led blingbling/led/ */
/* 功能说明: 控制GPIO1引脚的LED，实现闪烁效果（亮200ms/灭200ms） */
#define FEATURE_LED         1   // LED控制功能（已实现）

/* 3. 继电器控制功能（阻塞方式） */
/* 对应文件夹: class/jidianqi control/relay/ */
/* 功能说明: 控制GPIO2引脚的继电器，使用delay()实现定时切换（1秒周期） */
#define FEATURE_RELAY       1   // 继电器控制功能（已实现）

/* 4. 继电器控制功能（非阻塞方式） */
/* 对应文件夹: class/jidianqi millis/relay_noblock/ */
/* 功能说明: 使用millis()实现非阻塞延时，继电器每2秒切换一次状态 */
#define FEATURE_NON_BLOCK   1   // 非阻塞延时功能（已实现）

/* 5. 外部中断功能 */
/* 对应文件夹: class/wai bu zhong duan/interrupt_led/ */
/* 功能说明: 通过GPIO21外部中断检测按键，上升沿触发，实现LED状态翻转 */
/*           使用volatile关键字保护共享变量，避免在中断中使用阻塞函数 */
#define FEATURE_INTERRUPT   1   // 外部中断功能（已实现）

/* 6. 按键控制继电器功能 */
/* 对应文件夹: class/key control jidianqi/relay_control/ */
/* 功能说明: 双按键控制继电器开关，KEY1（自锁）为总开关，KEY2（普通）控制继电器 */
/*           KEY1打开后允许KEY2控制继电器，按KEY2一次吸合，再按一次断开 */
/*           KEY1关闭后继电器强制断开，实现安全开关功能 */
#define FEATURE_KEY_RELAY   1   // 按键控制继电器功能（已实现）

/* 7. 双按键控制双继电器功能（中断方式） */
/* 对应文件夹: class/00_11_relay_control/dual_relay_control/ */
/* 功能说明: 双按键控制双继电器四种状态轮流切换，KEY1（GPIO20）为自锁总开关，KEY2（GPIO21）控制状态切换 */
/*           继电器1接GPIO6，继电器2接GPIO7，支持四种状态：00(都断开)、01(断吸)、10(吸断)、11(都吸合) */
/*           使用外部中断检测按键，系统使能标志保护共享变量 */
#define FEATURE_DUAL_RELAY  1   // 双按键控制双继电器功能（已实现）

/* 8. 光感检测功能 */
/* 对应文件夹: class/light_sensor/light_sensor/ */
/* 功能说明: 使用GPIO1引脚的ADC检测光照强度，支持5级光照等级判断（黑暗/昏暗/正常/明亮/非常明亮） */
/*           光照弱时自动控制照明继电器（GPIO6），实现智能照明控制 */
#define FEATURE_LIGHT_SENSOR 1   // 光感检测与照明控制功能（已实现）

/* 9. 温度检测功能 */
/* 对应文件夹: class/temp_sensor/ */
/* 功能说明: 使用DS18B20温度传感器（GPIO10）检测环境温度，支持摄氏度/华氏度输出 */
/*           温度高时自动控制风扇继电器（GPIO7），实现智能温控 */
#define FEATURE_TEMP_SENSOR  1   // 温度检测与风扇控制功能（已实现）

/* 10. 智能环境控制功能（自动/手动模式） */
/* 对应文件夹: class/auto_manual_control/ */
/* 功能说明: 整合手动模式和自动模式，KEY2长按2秒切换模式 */
/*           自动模式：温度联动风扇（≥28°C开），光照联动照明（昏暗/黑暗开） */
/*           手动模式：短按KEY2切换继电器四种状态（00→01→10→11） */
/*           KEY1（GPIO20）自锁按键控制系统使能，KEY2（GPIO21）控制模式切换和状态切换 */
/*           集成OLED显示屏实时显示系统状态 */
#define FEATURE_AUTO_MANUAL   1   // 智能环境控制功能（已实现）

/* 11. OLED显示屏功能 */
/* 对应文件夹: class/display oled/display_oled/ */
/* 功能说明: 使用SH1106 128x64 OLED屏幕，I2C接口通信（SDA=GPIO4, SCL=GPIO5） */
/*           显示系统状态信息：模式、系统开关、光照值、温度值、LED状态、风扇状态 */
/*           每秒更新一次显示内容，实时反映环境状态 */
#define FEATURE_DISPLAY      1   // OLED显示屏功能（已实现）

/* 12. WiFi联网功能 */
/* 对应文件夹: class/wifi_connect/ */
/* 功能说明: 使用ESP32 WiFi模块实现网络连接功能 */
/*           上电自动扫描附近WiFi网络，通过串口选择要连接的网络并输入密码 */
/*           连接成功后显示IP地址、网关、子网掩码、DNS、信号强度等信息 */
/*           使用Preferences库将SSID和密码保存到Flash中，下次上电自动连接 */
/*           支持断网自动重连功能（5秒间隔），按'c'键清除保存的配置重新配置 */
/*           模块化设计，包含wifi_connect.h、wifi_connect.cpp、wifi_connect.ino */
/*           支持串口命令控制：disconnect断开WiFi并禁用扫描，connect恢复连接 */
#define FEATURE_WIFI         1   // WiFi联网功能（已实现）

/* 13. WS2812万色灯功能 */
/* 对应文件夹: class/ws2812_rgb/ */
/* 功能说明: 使用WS2812单总线RGB LED灯，GPIO0控制 */
/*           系统启动时彩虹渐变开机提示，WiFi未连接显示红色，已连接显示蓝色 */
/*           支持渐变色循环效果，使用HSV颜色空间实现平滑颜色过渡 */
/*           集成到智能环境控制系统中，作为状态指示 */
#define FEATURE_WS2812       1   // WS2812万色灯功能（已实现）

/* 14. MQTT通信功能 */
/* 对应文件夹: class/auto_manual_control/mqtt_connect.h/cpp */
/* 功能说明: 使用PubSubClient库实现MQTT通信，连接到47.98.170.180:8081 */
/*           设备状态上报主题: chemctrl/{device_id}/status */
/*           远程控制命令主题: chemctrl/{device_id}/command */
/*           支持的远程控制命令: */
/*             - set_relay: 控制继电器3或4的开关状态 */
/*             - set_mode: 切换自动/手动模式 */
/*             - get_status: 查询设备状态，立即上报 */
/*             - set_threshold: 设置温度和光照阈值 */
/*             - reboot: 远程重启设备 */
/*           支持串口配置MQTT参数: mqtt_config、mqtt_clear、mqtt_status、mqtt_report */
/*           使用Preferences库将MQTT配置保存到Flash中 */
/*           支持断网自动重连功能，每5秒尝试重连一次 */
/*           设备状态每10秒自动上报一次 */
/*           模块化设计，包含mqtt_connect.h、mqtt_connect.cpp */
#define FEATURE_MQTT         1   // MQTT通信功能（已实现）

/* ==================== 版本更新记录 ==================== */
/* V1.0     - 初始版本，基础框架搭建 */
/* V1.5.270  - 新增功能：
   - 按键检测模块（带消抖）
   - LED闪烁模块
   - 继电器控制模块（阻塞方式）
   - 继电器控制模块（非阻塞方式，使用millis）
*/
/* V1.5.280 - 新增功能：
   - 外部中断模块（使用attachInterrupt实现）
   - GPIO21下拉输入，按键按下产生上升沿触发中断
   - 中断服务函数中翻转LED状态，演示外部中断基本用法
*/
/* V1.5.281 - 新增功能：
   - 按键控制继电器模块（KEY1自锁总开关 + KEY2普通按键控制）
   - KEY1（GPIO21）自锁按键作为系统总开关
   - KEY2（GPIO20）普通按键控制继电器吸合/断开
   - 实现安全开关逻辑：总开关关闭时继电器强制断开
*/
/* V1.5.290 - 新增功能：
   - 双按键控制双继电器模块（中断方式）
   - KEY1（GPIO20）自锁按键作为系统总开关
   - KEY2（GPIO21）普通按键控制继电器状态切换
   - 继电器1接GPIO6，继电器2接GPIO7，支持四种状态轮流切换
   - 使用外部中断检测按键，系统使能标志保护共享变量
*/
/* V2.6.10 - 新增功能：
   - 光感检测模块（GPIO1）：ADC检测光照强度，支持5级光照等级判断
   - 温度检测模块（GPIO10）：DS18B20传感器，支持摄氏度/华氏度输出
   - 智能环境控制模块（auto_manual_control）：整合自动/手动双模式
   - 自动模式：温度联动风扇控制（≥28°C自动开风扇），光照联动照明控制（昏暗/黑暗自动开灯）
   - 手动模式：保留原有的四种状态轮流切换功能
   - KEY2长按2秒切换自动/手动模式，短按KEY2在手动模式下切换继电器状态
   - 模块化设计，各功能模块独立封装，便于维护和扩展
*/
/* V2.6.11 - 新增功能：
   - OLED显示屏模块（class/display oled）：使用SH1106 128x64屏幕，I2C接口通信
   - SDA接GPIO4，SCL接GPIO5，使用U8G2硬件I2C驱动
   - 显示屏实时显示系统状态：模式(auto/manu)、系统开关(ON/OFF)、光照值、温度值、LED状态、风扇状态
   - 将OLED显示屏集成到智能环境控制系统（auto_manual_control）中
   - 每秒更新一次显示内容，实时反映环境状态
*/
/* V2.6.20 - 新增功能：
   - WiFi联网模块（class/wifi_connect）：使用ESP32 WiFi模块实现网络连接
   - 上电自动扫描附近WiFi网络，通过串口选择要连接的网络并输入密码
   - 连接成功后显示IP地址、网关、子网掩码、DNS、信号强度等信息
   - 使用Preferences库将SSID和密码保存到Flash中，下次上电自动连接
   - 支持断网自动重连功能（5秒间隔），按'c'键清除保存的配置重新配置
   - 模块化设计，包含wifi_connect.h、wifi_connect.cpp、wifi_connect.ino三个文件
   - 支持WiFi状态管理（断开/连接中/已连接/重连中/扫描中/配置中）
   - 支持网络扫描功能，显示SSID、信号强度、频道、加密类型
   - 完善的错误处理机制，连接失败时显示详细错误信息
*/
/* V2.6.21 - 新增功能：
   - WS2812万色灯模块（class/ws2812_rgb）：使用单总线协议控制RGB LED灯
   - GPIO0连接万色灯DIN引脚，支持2^24种颜色显示
   - 系统启动时彩虹渐变开机提示，提升用户体验
   - WiFi状态指示：未连接显示红色，已连接显示蓝色
   - 支持渐变色循环效果，使用HSV颜色空间实现平滑过渡
   - WiFi串口命令控制：新增disconnect命令断开WiFi并禁用扫描
   - connect命令重新启用WiFi连接，恢复搜索和重连功能
   - 将万色灯功能集成到智能环境控制系统中，实时指示系统状态
*/
/* V2.6.31 - 新增功能：
   - MQTT通信模块（mqtt_connect.h/cpp）：使用PubSubClient库实现MQTT通信
   - 默认连接到47.98.170.180:8081，用户名dzdx_emqx，密码Jp4!sQ7$
   - 设备状态上报主题: chemctrl/{device_id}/status，每10秒自动上报一次
   - 远程控制命令主题: chemctrl/{device_id}/command，实时接收控制指令
   - 支持的远程控制命令:
     - set_relay: 控制继电器3(照明)或4(风扇)的开关状态
     - set_mode: 切换自动/手动模式
     - get_status: 查询设备状态，立即上报一次
     - set_threshold: 设置温度和光照阈值
     - reboot: 远程重启设备
   - 支持串口配置MQTT参数:
     - mqtt_config: 配置MQTT服务器、端口、用户名、密码、设备ID
     - mqtt_clear: 清除MQTT配置，恢复默认值
     - mqtt_status: 查看MQTT配置和连接状态
     - mqtt_report: 手动触发状态上报
   - 使用Preferences库将MQTT配置保存到Flash中，断电不丢失
   - 支持断网自动重连功能，每5秒尝试重连一次
   - 新增relay模块接口: relay_get_relay1/2_status、relay_set_relay1/2、
     relay_set_mode、relay_set_temp_threshold、relay_set_light_threshold
   - 新增阈值变量: temp_threshold、light_threshold，支持远程设置
   - 模块化设计，各功能独立封装，便于维护和扩展
*/
/* V2.6.32 - 优化功能：
   - 修复阈值远程下发不生效问题：修改relay_auto_run()函数，直接使用用户设置的阈值判断
   - 光照阈值逻辑：当检测到的光照值小于阈值时，自动开启照明继电器
   - 温度阈值逻辑：当检测到的温度大于阈值时，自动开启风扇继电器
   - 阈值配置持久化：使用Preferences库将阈值保存到Flash，断电后不丢失
   - 优化串口命令处理：统一在主循环中处理串口命令，避免多个函数竞争读取串口数据
   - 新增mqtt_config模式保护：配置过程中忽略所有MQTT命令，防止被get_status打断
   - 新增mqtt_config退出功能：输入esc可随时退出配置模式，不保存修改
   - 新增get_status频率限制：5秒内最多响应一次，避免频繁上报
   - 修复手动模式继电器状态重置问题：MQTT设置继电器后同步更新state变量
   - 完善状态上报机制：仅在状态变化时上报（KEY1/KEY2状态、WiFi状态、继电器状态）
*/
/* V2.6.50 - 新增功能：
   - 新增PCT化工过程控制实训软件完整项目交付
   - 项目包含：Web前端源码、Electron桌面端打包成品、技术文档、项目说明文档
   - 通信协议：基于MQTT实现传感器数据实时采集与执行器远程控制
   - 功能模块：环境监测（温度/光照）、设备控制（照明/风扇）、数据可视化、远程运维
   - 应用场景：化工类院校实训教学、智能工厂演示、物联网入门实践
   - 修复WS2812指示灯渐变阻塞导致KEY2短按失灵问题
   - 重构fade_color()函数为非阻塞状态机实现
   - 优化内网MQTT服务器连接支持，完善局域网EMQX服务器连接流程
*/
#endif
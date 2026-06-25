/**
 * @file relay.ino
 * @brief 继电器控制主程序（双继电器版）
 * @details 使用delay()实现双继电器交替控制
 *          继电器1（GPIO6）同时控制LED
 *          继电器2（GPIO7）独立控制
 */

#include "relay.h"

/**
 * @brief 初始化函数
 */
void setup()
{
    relay_init();  // 初始化双继电器
    Serial.begin(9600);
    Serial.println("Dual Relay Control");
}

/**
 * @brief 主循环函数
 * @details 双继电器交替吸合：继电器1亮1秒后，继电器2亮1秒
 */
void loop()
{
    // 继电器1吸合（LED亮），继电器2断开
    relay1_ctrl(RELAY_ON);
    relay2_ctrl(RELAY_OFF);
    Serial.println("Relay1 ON (LED ON), Relay2 OFF");
    delay(1000);
    
    // 继电器1断开（LED灭），继电器2吸合
    relay1_ctrl(RELAY_OFF);
    relay2_ctrl(RELAY_ON);
    Serial.println("Relay1 OFF (LED OFF), Relay2 ON");
    delay(1000);
}
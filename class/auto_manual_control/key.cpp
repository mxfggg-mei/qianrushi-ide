#include "key.h"

/**
 * @brief 按键初始化函数
 */
void key_init(void)
{
    pinMode(KEY1_PIN, INPUT);
    pinMode(KEY2_PIN, INPUT);
}
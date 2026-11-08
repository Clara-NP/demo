#include "gpio.h"

#include "common/error_code.h"

/**
 * @brief 初始化GPIO
 * 
 * @param config GPIO配置组
 * @param num GPIO数量
 * @return int 
 */
int gpioInit(const gpioConfig_t *config, int num)
{
    (void)config;
    (void)num;
    return RET_SUCCESS;
}
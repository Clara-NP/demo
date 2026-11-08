#ifndef MINI_TEST_GPIO_H
#define MINI_TEST_GPIO_H

/**
 * @file gpio.h
 * @brief GPIO 输出。端口号与引脚号的含义由当前 soc 端口定义。
 *
 * @par 执行上下文
 * 纯头文件，无运行期状态
 *
 * @par 可重入性
 * 不适用
 *
 * @par 资源所有权
 * 不持有资源
 */

#include <stdint.h>

#include "fw_err.h"

/**
 * @brief 把一只引脚配成推挽输出
 *
 * @param[in] port soc 定义的端口号
 * @param[in] pin  soc 定义的引脚号
 * @return fw_err_t
 * @retval FW_OK 成功
 * @retval FW_ERR_INVALID_ARGS 端口或引脚超出该 soc 的范围
 *
 * @note 调用上下文：主上下文；可重入：否
 */
fw_err_t gpio_init_output(uint8_t port, uint8_t pin);

/**
 * @brief 写引脚物理电平
 *
 * @param[in] port  soc 定义的端口号
 * @param[in] pin   soc 定义的引脚号
 * @param[in] level 0 为低电平，非 0 为高电平
 * @return fw_err_t
 * @retval FW_OK 成功
 * @retval FW_ERR_INVALID_ARGS 端口或引脚超出该 soc 的范围
 *
 * @note 调用上下文：主上下文；可重入：否
 */
fw_err_t gpio_write(uint8_t port, uint8_t pin, uint8_t level);

#endif /* MINI_TEST_GPIO_H */

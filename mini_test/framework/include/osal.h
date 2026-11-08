#ifndef MINI_TEST_OSAL_H
#define MINI_TEST_OSAL_H

/**
 * @file osal.h
 * @brief 毫秒延时。时间基准由当前 soc 端口提供。
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
 * @brief 启动端口的毫秒节拍
 *
 * @return fw_err_t
 * @retval FW_OK 成功
 *
 * @note 调用上下文：主上下文，在 osal_delay_ms 之前调用一次；可重入：否
 */
fw_err_t osal_init(void);

/**
 * @brief 阻塞等待指定毫秒
 *
 * @param[in] delay_ms 等待时长，0 立即返回
 * @return fw_err_t
 * @retval FW_OK 等待完成
 * @retval FW_ERR_FAILED 尚未调用 osal_init
 * @retval FW_ERR_TIMEOUT 节拍没有在时限内到来
 *
 * @note 调用上下文：主上下文；可重入：否
 */
fw_err_t osal_delay_ms(uint32_t delay_ms);

#endif /* MINI_TEST_OSAL_H */

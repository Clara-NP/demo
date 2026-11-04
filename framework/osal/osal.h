#ifndef __OSAL_OSAL_H__
#define __OSAL_OSAL_H__

/**
 * @file osal.h
 * @brief OSAL 最小时间接口
 *
 * @par 执行上下文
 * 任意上下文；实现必须无阻塞
 *
 * @par 可重入性
 * 是
 *
 * @par 资源所有权
 * 不持有资源
 */

#include <common/generic.h>

fw_err_t osTickMs(uint32_t *tickMs);
fw_err_t sysTimeStamp(uint32_t *timestampMs);

#endif // __OSAL_OSAL_H__

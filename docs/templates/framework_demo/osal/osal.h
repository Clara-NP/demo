#ifndef __OSAL_OSAL_H__
#define __OSAL_OSAL_H__

/**
 * @file osal.h
 * @author <author>
 * @brief OSAL 最小接口：只暴露与"时间"相关的可移植符号
 * @version 0.1
 * @date 2026-11-02
 *
 * @par 执行上下文
 *  osTickMs 可在任意上下文调用（实现必须无阻塞）
 *
 * @par 可重入性
 *  是（读取计数器的原子快照）
 *
 * @note 本文件是 framework_demo 骨架的一部分：复制到真实仓库时展开为 `libs/osal/osal.h`
 * @note 实现放在 `port/<soc>/`，本目录不含任何平台代码
 */

#include <common/generic.h>

/**
 * @brief 系统毫秒计数（单调递增，允许回绕）
 *
 * @return uint32_t 毫秒
 *
 * @note 差值计算必须用无符号减法：`(uint32_t)(now - start)`，回绕后仍正确
 */
uint32_t osTickMs(void);

/**
 * @brief 协议时间戳（通常等同 osTickMs，独立出来便于换成 RTC 时间）
 *
 * @return uint32_t 时间戳，单位 ms
 */
uint32_t sysTimeStamp(void);

#endif // __OSAL_OSAL_H__

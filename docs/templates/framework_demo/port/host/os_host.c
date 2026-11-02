/**
 * @file os_host.c
 * @author <author>
 * @brief host 后端：毫秒计数（POSIX CLOCK_MONOTONIC）
 * @version 0.1
 * @date 2026-11-02
 *
 * @par 执行上下文
 *  任意上下文；clock_gettime 不阻塞
 *
 * @par 可重入性
 *  是
 *
 * @par 资源所有权
 *  不持有资源
 */

#include <time.h>

#include <common/generic.h>
#include <osal/osal.h>

uint32_t osTickMs(void)
{
    struct timespec ts;
    uint64_t ms;

    if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0)
    {
        return 0u;
    }

    ms = ((uint64_t)ts.tv_sec * 1000u) + ((uint64_t)ts.tv_nsec / 1000000u);

    return (uint32_t)ms;
}

uint32_t sysTimeStamp(void)
{
    return osTickMs();
}

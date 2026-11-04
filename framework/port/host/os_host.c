/**
 * @file os_host.c
 * @brief host 单调毫秒计数后端
 *
 * @par 执行上下文
 * 任意上下文
 *
 * @par 可重入性
 * 是
 *
 * @par 资源所有权
 * 不持有资源
 */

#include <time.h>

#include <common/generic.h>
#include <osal/osal.h>

fw_err_t osTickMs(uint32_t *tickMs) {
    struct timespec ts;
    uint64_t ms;

    if (tickMs == NULL) {
        return FW_ERR_INVALID_ARGS;
    }
    *tickMs = 0u;

    if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0) {
        return FW_ERR_FAILED;
    }
    ms = ((uint64_t)ts.tv_sec * 1000u) + ((uint64_t)ts.tv_nsec / 1000000u);
    *tickMs = (uint32_t)ms;
    return FW_OK;
}

fw_err_t sysTimeStamp(uint32_t *timestampMs) {
    return osTickMs(timestampMs);
}

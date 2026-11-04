#ifndef __COMMON_GENERIC_H__
#define __COMMON_GENERIC_H__

/**
 * @file generic.h
 * @brief 框架公共类型、数组长度与统一错误码
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

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define ARRAY_SIZE(_a)                   (sizeof(_a) / sizeof((_a)[0]))
#define COMPILE_TIME_ASSERT(_cond, _tag) typedef char compile_time_assert_##_tag[(_cond) ? 1 : -1]

typedef uint16_t handle_t;

/**
 * @brief 框架统一状态码
 *
 * @note 公开 API 只用本类型返回状态；数据和计数必须通过出参返回。
 */
typedef int32_t fw_err_t;

#define FW_OK                ((fw_err_t)0)
#define FW_ERR_FAILED        ((fw_err_t) - 1)
#define FW_ERR_NO_OBJECT     ((fw_err_t) - 2)
#define FW_ERR_INVALID_ARGS  ((fw_err_t) - 3)
#define FW_ERR_NOT_SUPPORTED ((fw_err_t) - 4)
#define FW_ERR_TIMEOUT       ((fw_err_t) - 5)
#define FW_ERR_TYPE_MISMATCH ((fw_err_t) - 6)

#endif // __COMMON_GENERIC_H__

#ifndef __COMMON_GENERIC_H__
#define __COMMON_GENERIC_H__

/**
 * @file generic.h
 * @author <author>
 * @brief 框架统一入口：固定宽度类型、数组长度、错误码（模块只允许依赖本头）
 * @version 0.1
 * @date 2026-11-02
 *
 * @par 执行上下文
 *  纯头文件（类型与常量），无运行期状态
 *
 * @par 可重入性
 *  不适用
 *
 * @note 本文件是 framework_demo 骨架的一部分：
 *       复制到真实仓库时展开为 `framework/common/generic.h`，此后以 framework 为准
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/**
 * @brief 数组元素个数
 *
 * @param[in] _a 数组（不允许传指针）
 */
#define ARRAY_SIZE(_a)              (sizeof(_a) / sizeof((_a)[0]))

/**
 * @brief 编译期断言（C99 可用；C11 可直接用 _Static_assert）
 *
 * @param[in] _cond 常量表达式
 * @param[in] _tag  用于生成唯一类型名
 */
#define COMPILE_TIME_ASSERT(_cond, _tag) \
    typedef char compile_time_assert_##_tag[(_cond) ? 1 : -1]

/// 通用句柄：跨层传递的整数标识，禁止传内部指针
typedef uint16_t handle_t;

/// 成功
#define RET_SUCCESS             0
/// 一般失败
#define RET_FAILED             -1
/// 实例/对象不存在
#define RET_NO_OBJECT          -2
/// 参数非法
#define RET_INVALID_ARGS       -3
/// 当前芯片/型号不支持
#define RET_NOT_SUPPORT        -4
/// 超时
#define RET_TIMEOUT            -5
/// 类型不匹配
#define RET_TYPE_MISMATCH      -6

#endif // __COMMON_GENERIC_H__

#ifndef __COMMON_TRACE_H__
#define __COMMON_TRACE_H__

/**
 * @file trace.h
 * @author <author>
 * @brief 模块级 trace：按 TRACE_TAG 标识来源、按 TRACE_LEVEL 编译期裁剪
 * @version 0.1
 * @date 2026-11-02
 *
 * @par 使用方式（每个 .c 顶部，顺序不可变）
 * @code
 *   #define TRACE_ENABLE
 *   #define TRACE_LEVEL     T_INFO
 *   #define TRACE_TAG       "mymod"
 *   #include <common/trace.h>
 * @endcode
 *
 * @par 执行上下文
 *  任意上下文；host 实现走 stdout，目标板需替换 tracePrint 后端（不得在 ISR 中打印）
 *
 * @par 可重入性
 *  host 实现依赖 stdio，非线程安全；ISR 中禁止调用
 *
 * @note 本文件是 framework_demo 骨架的一部分：复制到真实仓库时展开为 `framework/common/trace.h`
 */

#include <stdarg.h>
#include <stdio.h>

/// 错误级
#define T_ERROR                 0
/// 警告级
#define T_WARN                  1
/// 信息级
#define T_INFO                  2
/// 调试级
#define T_DEBUG                 3

#ifndef TRACE_LEVEL
#define TRACE_LEVEL             T_WARN
#endif

#ifndef TRACE_TAG
#define TRACE_TAG               "?"
#endif

/**
 * @brief 输出一条 trace
 *
 * @param[in] level 级别，见 T_*
 * @param[in] tag   模块标签
 * @param[in] fmt   格式串
 *
 * @note host 实现；目标板应替换为串口/环形缓冲后端，接口保持不变
 */
static inline void tracePrint(int level, const char *tag, const char *fmt, ...)
{
    static const char kLevel[] = { 'E', 'W', 'I', 'D' };
    char mark = ((level >= T_ERROR) && (level <= T_DEBUG)) ? kLevel[level] : '?';

    va_list ap;

    (void)printf("[%c][%s] ", mark, tag);

    va_start(ap, fmt);
    (void)vprintf(fmt, ap);
    va_end(ap);

    (void)printf("\r\n");
}

#if defined(TRACE_ENABLE) && (TRACE_LEVEL >= T_ERROR)
#define elog(...)               tracePrint(T_ERROR, TRACE_TAG, __VA_ARGS__)
#else
#define elog(...)               ((void)0)
#endif

#if defined(TRACE_ENABLE) && (TRACE_LEVEL >= T_WARN)
#define wlog(...)               tracePrint(T_WARN, TRACE_TAG, __VA_ARGS__)
#else
#define wlog(...)               ((void)0)
#endif

#if defined(TRACE_ENABLE) && (TRACE_LEVEL >= T_INFO)
#define ilog(...)               tracePrint(T_INFO, TRACE_TAG, __VA_ARGS__)
#else
#define ilog(...)               ((void)0)
#endif

#if defined(TRACE_ENABLE) && (TRACE_LEVEL >= T_DEBUG)
#define dlog(...)               tracePrint(T_DEBUG, TRACE_TAG, __VA_ARGS__)
#else
#define dlog(...)               ((void)0)
#endif

#endif // __COMMON_TRACE_H__

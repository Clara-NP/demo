#ifndef __COMMON_TRACE_H__
#define __COMMON_TRACE_H__

/**
 * @file trace.h
 * @brief 模块级 trace：按标签标识来源、按级别编译期裁剪
 *
 * @par 执行上下文
 * host 实现用于任务上下文；ISR 中禁止打印
 *
 * @par 可重入性
 * host 实现依赖 stdio，不保证线程安全
 *
 * @par 资源所有权
 * 不持有资源
 */

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>

#define T_ERROR 0
#define T_WARN  1
#define T_INFO  2
#define T_DEBUG 3

#ifndef TRACE_LEVEL
#define TRACE_LEVEL T_WARN
#endif

#ifndef TRACE_TAG
#define TRACE_TAG "?"
#endif

static inline void tracePrint(int32_t level, const char *tag, const char *fmt, ...) {
    static const char kLevel[] = {'E', 'W', 'I', 'D'};
    char mark = ((level >= T_ERROR) && (level <= T_DEBUG)) ? kLevel[level] : '?';
    va_list ap;

    (void)printf("[%c][%s] ", mark, tag);
    va_start(ap, fmt);
    (void)vprintf(fmt, ap);
    va_end(ap);
    (void)printf("\r\n");
}

#if defined(TRACE_ENABLE) && (TRACE_LEVEL >= T_ERROR)
#define elog(...) tracePrint(T_ERROR, TRACE_TAG, __VA_ARGS__)
#else
#define elog(...) ((void)0)
#endif

#if defined(TRACE_ENABLE) && (TRACE_LEVEL >= T_WARN)
#define wlog(...) tracePrint(T_WARN, TRACE_TAG, __VA_ARGS__)
#else
#define wlog(...) ((void)0)
#endif

#if defined(TRACE_ENABLE) && (TRACE_LEVEL >= T_INFO)
#define ilog(...) tracePrint(T_INFO, TRACE_TAG, __VA_ARGS__)
#else
#define ilog(...) ((void)0)
#endif

#if defined(TRACE_ENABLE) && (TRACE_LEVEL >= T_DEBUG)
#define dlog(...) tracePrint(T_DEBUG, TRACE_TAG, __VA_ARGS__)
#else
#define dlog(...) ((void)0)
#endif

#endif // __COMMON_TRACE_H__

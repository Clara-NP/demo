#ifndef __SACP_LOCAL_COMMON_H__
#define __SACP_LOCAL_COMMON_H__

/**
 * @file local_common.h
 * @author <author>
 * @brief 协议层公共声明占位（由 SACP 组件提供实现）
 * @version 0.1
 * @date 2026-11-02
 *
 * @note
 *  - 本轮只落结构：这里声明 `sacp_*.c` 会用到的公共符号，**不提供实现**
 *  - SACP 组件就绪后，本文件应改为 `#include <sacp/...>`，不要长期保留空壳声明
 *
 * @par 待组件提供的符号
 *  - `uint32_t sysTimeStamp(void);`
 *  - `int      reportAttributesWithTimestamp(uint8_t groupId, uint8_t priority,`
 *                                             `uint32_t timestamp, const void *report);`
 *  - `uint8_t  REPORT_ID(uint8_t base, uint8_t index);`（宏）
 */

#include <common/generic.h>

#endif // __SACP_LOCAL_COMMON_H__

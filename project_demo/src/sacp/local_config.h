#ifndef __SACP_LOCAL_CONFIG_H__
#define __SACP_LOCAL_CONFIG_H__

/**
 * @file local_config.h
 * @author <author>
 * @brief 协议接入开关（按模块分段，工程层唯一来源）
 * @version 0.1
 * @date 2026-11-02
 *
 * @note 开关一律 `#ifndef` 包裹：允许命令行 `-D` 或板级配置先行覆盖
 */

/// 传感器属性组是否启用
#ifndef CONFIG_SACP_TMPL_SENSOR
#define CONFIG_SACP_TMPL_SENSOR 1
#endif

#endif // __SACP_LOCAL_CONFIG_H__

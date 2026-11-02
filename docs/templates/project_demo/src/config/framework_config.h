#ifndef __CONFIG_FRAMEWORK_CONFIG_H__
#define __CONFIG_FRAMEWORK_CONFIG_H__

/**
 * @file framework_config.h
 * @author <author>
 * @brief 产品配置（模块开发流程步骤 ③ 与 ⑥ 的衔接点）
 * @version 0.1
 * @date 2026-11-02
 *
 * @note
 *  - 只写"产品要什么"：开关与数量；**不写**引脚/地址/参数
 *  - 容量宏一律由导出枚举派生，禁止手写数字（否则"加资源忘改容量"必然发生）
 *  - 驱动公开头里的 `#ifndef CONFIG_XXX_MAX_NUM` 是默认值，本文件优先定义即覆盖
 *
 * @par 执行上下文
 *  编译期配置；不涉及运行期状态
 */

#include <config/export_ids.h>
#include <sacp/local_config.h>

/// 传感器实例容量：跟随逻辑 ID 枚举，增删资源无需改动此处
#define CONFIG_TMPL_SENSOR_MAX_NUM          EXPORT_TMPL_SENSOR_MAX_NUM

/// 抽象 I2C 总线数量
#define CONFIG_I2C_MAX_NUM                  EXPORT_I2C_MAX_NUM

/*
 * 板级差异（引脚 / 器件地址 / 参数）不在此文件，统一下沉到 src/system/board.c。
 */

#endif // __CONFIG_FRAMEWORK_CONFIG_H__

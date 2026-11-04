#ifndef __SYSTEM_BOARD_H__
#define __SYSTEM_BOARD_H__

/**
 * @file board.h
 * @author <author>
 * @brief 板级初始化对外接口（模块开发流程步骤 ⑥ 的调用入口）
 * @version 0.1
 * @date 2026-11-02
 *
 * @note 只暴露"初始化"语义，不暴露描述符表与任何引脚宏
 */

#include <common/generic.h>

/**
 * @brief 板级初始化总入口
 *
 * @param[out] initializedModules 已初始化模块数
 * @return fw_err_t
 *
 * @note 调用上下文：启动阶段（任务创建前），不可重入
 */
fw_err_t boardInit(uint8_t *initializedModules);

/**
 * @brief 初始化板载传感器
 *
 * @param[out] initializedSensors 已初始化传感器数
 * @return fw_err_t
 *
 * @note 部分失败只记录警告、不中断启动（见实现中的 wlog）
 * @note 调用上下文：启动阶段，不可重入
 */
fw_err_t boardTmplSensorInit(uint8_t *initializedSensors);

#endif // __SYSTEM_BOARD_H__

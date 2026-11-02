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
 * @return int
 *  - >= 0 成功（返回已初始化模块数）
 *  - <  0 失败，错误码见 `RET_*`
 *
 * @note 调用上下文：启动阶段（任务创建前），不可重入
 */
int boardInit(void);

/**
 * @brief 初始化板载传感器
 *
 * @return int
 *  - > 0 已初始化的实例数量
 *  - RET_NO_OBJECT 一个都没成功
 *
 * @note 部分失败只记录警告、不中断启动（见实现中的 wlog）
 * @note 调用上下文：启动阶段，不可重入
 */
int boardTmplSensorInit(void);

#endif // __SYSTEM_BOARD_H__

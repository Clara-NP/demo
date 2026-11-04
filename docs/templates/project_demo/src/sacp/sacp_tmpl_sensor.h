#ifndef __SACP_TMPL_SENSOR_H__
#define __SACP_TMPL_SENSOR_H__

/**
 * @file sacp_tmpl_sensor.h
 * @author <author>
 * @brief 模板模块的协议属性接入接口（模块开发流程步骤 ⑧）
 * @version 0.1
 * @date 2026-11-02
 *
 * @note 属性表在 `sacp_tmpl_sensor.c` 内部；本头只暴露"初始化 / 刷新 / 上报"三件事
 */

#include <common/generic.h>

/**
 * @brief 注册本模块的属性组
 *
 * @note 必须在 SACP 服务初始化完成之后调用；可重入：否
 * @note 调用上下文：启动阶段（任务创建前）
 */
fw_err_t sacpTmplSensorInit(void);

/**
 * @brief 采样一次并刷新属性值
 *
 * @param[in] id 逻辑 ID
 * @return fw_err_t
 *
 * @note 调用上下文：任务上下文（建议 100ms 周期）；不要在 ISR 中调用
 */
fw_err_t sacpTmplSensorUpdate(uint8_t id);

/**
 * @brief 上报属性组
 *
 * @param[in] groupId 上报组 ID，见 `local_groups.h`
 *
 * @return fw_err_t
 * @note 调用上下文：任务上下文；可重入：否
 */
fw_err_t sacpTmplSensorReport(uint8_t groupId);

#endif // __SACP_TMPL_SENSOR_H__

#ifndef __SACP_LOCAL_GROUPS_H__
#define __SACP_LOCAL_GROUPS_H__

/**
 * @file local_groups.h
 * @author <author>
 * @brief 上报组基址与组内编号（模块开发流程步骤 ⑧）
 * @version 0.1
 * @date 2026-11-02
 *
 * @note 上报组 ID 由 `REPORT_ID(基址, 组内编号)` 展开；基址按模块分段，禁止复用一个基址
 */

/// 传感器模块上报组基址
#define _TMPL_SENSOR_GROUP                  0

/// 组内编号：状态同步上报
#define _STATE_SYNC                         0

#endif // __SACP_LOCAL_GROUPS_H__

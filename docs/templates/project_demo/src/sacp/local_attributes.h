#ifndef __SACP_LOCAL_ATTRIBUTES_H__
#define __SACP_LOCAL_ATTRIBUTES_H__

/**
 * @file local_attributes.h
 * @author <author>
 * @brief 本地属性 ID 分配表（模块开发流程步骤 ⑧）
 * @version 0.1
 * @date 2026-11-02
 *
 * @note
 *  - 按模块分段，**段内必须升序**（属性组注册时按序校验）
 *  - 新增模块先在此申请号段，再写 `sacp_<name>.c`，避免多模块抢号
 *
 * @par 号段规划
 *  | 段 | 范围 | 模块 |
 *  |---|---|---|
 *  | 0 | 0-99   | 系统 |
 *  | 1 | 100-199| 运动 |
 *  | 2 | 200-219| 传感器（本文件） |
 */

/**
 * @brief 本地属性 ID
 */
enum LOCAL_ATTRIBUTES
{
    /// 传感器测量值，INT32，只读
    ATTR_TMPL_SENSOR_VALUE   = 200,
    /// 传感器温度，INT16，只读，单位 0.1℃
    ATTR_TMPL_SENSOR_TEMP    = 201,
    /// 器件状态字，UINT8，只读，0 表示正常
    ATTR_TMPL_SENSOR_STATUS  = 202,
    /// 采样模式，UINT8，可读写，取值见 `TMPL_SENSOR_MODE`
    ATTR_TMPL_SENSOR_MODE    = 203,
};

#endif // __SACP_LOCAL_ATTRIBUTES_H__

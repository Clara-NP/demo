/**
 * @file sacp_tmpl_sensor.c
 * @author <author>
 * @brief 模板模块的协议属性接入（模块开发流程步骤 ⑧）
 * @version 0.1
 * @date 2026-11-02
 *
 * @note
 *  - 属性表放在**应用层**，驱动不认识协议；驱动只提供 API
 *  - 一个模块 = 一张属性表 + 一个写回调 + 一个上报组，全部是"数据"
 *  - 属性 ID 必须按**从小到大**排列，否则注册时触发断言
 *
 * @par 执行上下文
 *  初始化与周期任务上下文；不在 ISR 中调用
 *
 * @par 可重入性
 *  否。属性组为单例，调用方需串行化
 *
 * @par 资源所有权
 *  拥有 `s_group`（属性组实例，静态存储）；上报组表由构建期生成，只读
 *
 * @par 依赖状态
 *  `sacp/attribute/sacp_attribute.h` 由 `docs/templates/framework_demo/` 提供最小实现，
 *  已由 `docs/templates/CMakeLists.txt` 的 `smoke_app` 编译验证
 */

#include <common/generic.h>
#include <osal/osal.h>
#include <sacp/attribute/sacp_attribute.h>

#include <tmpl_sensor.h>
#include <config/export_ids.h>

#include "local_config.h"
#include "local_attributes.h"
#include "local_groups.h"
#include "local_common.h"
#include "sacp_tmpl_sensor.h"

#define TRACE_ENABLE
#define TRACE_TAG       "sacp-tmplsensor"
#define TRACE_LEVEL     T_INFO
#include <common/trace.h>

#if CONFIG_SACP_TMPL_SENSOR

/// 本组监控的属性 ID 区间
#define LOCAL_ATTRIBUTE_MIN         200
#define LOCAL_ATTRIBUTE_MAX         219

/// 写回调声明
static void localAttributeWriteApply(const void *groupData, sacpRequest_t *request);

/* ==========================================================================
 * 属性表（32 位族：INT8/16/32、UINT8/16/32、FLOAT 都在这里）
 * ========================================================================== */

LOCAL_V32_TABLE(tmplsensor)
{
    _INT32_ATTRIBUTE(ATTR_TMPL_SENSOR_VALUE),
    _INT16_ATTRIBUTE(ATTR_TMPL_SENSOR_TEMP),
    _UINT8_ATTRIBUTE(ATTR_TMPL_SENSOR_STATUS),
    _UINT8_ATTRIBUTE_RW(ATTR_TMPL_SENSOR_MODE),
};

/// 没有 64 位属性时也必须保留空表（宏展开需要）
LOCAL_V64_TABLE(tmplsensor)
{
};

/// 没有 OCTET（字符串/二进制）属性时同样保留空表
LOCAL_OCTET_TABLE(tmplsensor)
{
};

/// 定义一个属性组（自动生成 s_group）
LOCAL_GROUP_DEFINE(tmplsensor, LOCAL_ATTRIBUTE_MIN, LOCAL_ATTRIBUTE_MAX, localAttributeWriteApply);

/* ==========================================================================
 * 上报组：一次上报的属性集合
 * ========================================================================== */

LOCAL_REPORT_DEFINE(tmplsensorState)
{
    ATTR_TMPL_SENSOR_VALUE,
    ATTR_TMPL_SENSOR_TEMP,
    ATTR_TMPL_SENSOR_STATUS,
};

/* ==========================================================================
 * 写回调：只处理"可写"属性，读属性不在此处理
 * ========================================================================== */

static void localAttributeWriteApply(const void *groupData, sacpRequest_t *request)
{
    const attributeGroup_t *group = (const attributeGroup_t *)groupData;

    (void)request;

    // 模式：由上层把写入值下发给驱动
    attributeV32_t *modeAttr = (attributeV32_t *)findAttributeInGroup(group, ATTR_TMPL_SENSOR_MODE);

    if (ATTRIBUTE_TEST_AND_CLEAR_WRITE_PENDING(modeAttr))
    {
        // 单实例示例：多实例时按属性 ID 反算逻辑 ID
        uint8_t id = SENSOR_PRESSURE;
        int ret = tmplSensorSetMode(id, (uint8_t)modeAttr->value.v32);

        if (ret != RET_SUCCESS)
        {
            wlog("apply mode failed, id:%d mode:%d ret:%d",
                 id, (int)modeAttr->value.v32, ret);
        }
        else
        {
            ilog("apply mode ok, id:%d mode:%d", id, (int)modeAttr->value.v32);
        }
    }
}

/* ==========================================================================
 * 数据刷新与上报
 * ========================================================================== */

int sacpTmplSensorUpdate(uint8_t id)
{
    tmplSensorData_t data = { 0 };
    int ret = tmplSensorRead(id, &data);

    if (ret != RET_SUCCESS)
    {
        localAttributeSetInt(s_group, ATTR_TMPL_SENSOR_STATUS, (uint32_t)ret);
        return ret;
    }

    localAttributeSetInt(s_group, ATTR_TMPL_SENSOR_VALUE,  (uint32_t)data.value);
    localAttributeSetInt(s_group, ATTR_TMPL_SENSOR_TEMP,   (uint32_t)(uint16_t)data.temperature);
    localAttributeSetInt(s_group, ATTR_TMPL_SENSOR_STATUS, data.status);

    return RET_SUCCESS;
}

void sacpTmplSensorReport(uint8_t groupId)
{
    if (groupId == REPORT_ID(_TMPL_SENSOR_GROUP, _STATE_SYNC))
    {
        reportAttributesWithTimestamp(groupId, SACP_PRIORITY_LOWEST,
                                      sysTimeStamp(), LOCAL_REPORT(tmplsensorState));
    }
}

/* ==========================================================================
 * 初始化：注册属性组（必须在 SACP 服务初始化之后调用）
 * ========================================================================== */

void sacpTmplSensorInit(void)
{
    if (attributeGroupRegister(s_group) != RET_SUCCESS)
    {
        elog("register tmplsensor attribute group failed");
        return;
    }

    // 初始值：先把"未采样"的语义写进去，避免上层读到 0 误判
    localAttributeSetInt(s_group, ATTR_TMPL_SENSOR_STATUS, 0xff);
    localAttributeSetInt(s_group, ATTR_TMPL_SENSOR_MODE, TMPL_SENSOR_MODE_SINGLE);

    ilog("tmplsensor attribute group registered");
}

#endif // CONFIG_SACP_TMPL_SENSOR

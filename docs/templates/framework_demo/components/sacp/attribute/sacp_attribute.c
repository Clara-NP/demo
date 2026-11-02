/**
 * @file sacp_attribute.c
 * @author <author>
 * @brief 协议属性组件实现（最小集）：注册、查找、写入、上报
 * @version 0.1
 * @date 2026-11-02
 *
 * @par 执行上下文
 *  注册在启动阶段；其余在任务上下文
 *
 * @par 可重入性
 *  否。属性组表为静态单例，调用方需串行化
 *
 * @par 资源所有权
 *  只保存指向调用方静态表的指针，不复制、不释放
 */

#include <common/generic.h>

#include "sacp_attribute.h"

/// 最多注册的属性组数量
#define SACP_GROUP_MAX 8

static attributeGroup_t *s_groups[SACP_GROUP_MAX];
static int s_groupCount = 0;

static struct {
    uint8_t valid;
    uint8_t groupId;
    uint8_t priority;
    uint16_t attrCount;
    uint32_t timestamp;
} s_lastReport;

int attributeGroupRegister(attributeGroup_t *group) {
    if ((group == NULL) || (group->v32 == NULL) || (group->v32Num == 0u)) {
        return RET_INVALID_ARGS;
    }

    if (group->minId > group->maxId) {
        return RET_INVALID_ARGS;
    }

    for (uint16_t i = 0; i < group->v32Num; i++) {
        const attributeV32_t *attr = &group->v32[i];

        if ((attr->id < group->minId) || (attr->id > group->maxId)) {
            return RET_INVALID_ARGS; // ID 超出本组区间
        }

        if ((i > 0u) && (attr->id <= group->v32[i - 1u].id)) {
            return RET_INVALID_ARGS; // 必须严格升序
        }
    }

    for (int i = 0; i < s_groupCount; i++) {
        if (s_groups[i] == group) {
            return RET_INVALID_ARGS; // 重复注册
        }
    }

    if (s_groupCount >= SACP_GROUP_MAX) {
        return RET_FAILED;
    }

    s_groups[s_groupCount] = group;
    s_groupCount++;

    return RET_SUCCESS;
}

attributeV32_t *findAttributeInGroup(const attributeGroup_t *group, uint16_t id) {
    if ((group == NULL) || (group->v32 == NULL)) {
        return NULL;
    }

    for (uint16_t i = 0; i < group->v32Num; i++) {
        if (group->v32[i].id == id) {
            return &group->v32[i];
        }
    }

    return NULL;
}

int localAttributeSetInt(attributeGroup_t *group, uint16_t id, uint32_t value) {
    attributeV32_t *attr = findAttributeInGroup(group, id);

    if (attr == NULL) {
        return RET_NO_OBJECT;
    }

    switch (attr->type) {
        case ATTR_TYPE_INT8:
            attr->value.v8 = (int8_t)value;
            break;

        case ATTR_TYPE_INT16:
            attr->value.v16 = (int16_t)value;
            break;

        case ATTR_TYPE_INT32:
            attr->value.v32 = (int32_t)value;
            break;

        case ATTR_TYPE_UINT8:
            attr->value.u8 = (uint8_t)value;
            break;

        case ATTR_TYPE_UINT16:
            attr->value.u16 = (uint16_t)value;
            break;

        case ATTR_TYPE_UINT32:
            attr->value.u32 = value;
            break;

        case ATTR_TYPE_FLOAT32:
            attr->value.f32 = (float)(int32_t)value;
            break;

        default:
            return RET_TYPE_MISMATCH;
    }

    return RET_SUCCESS;
}

int attributeTestAndClearWritePending(attributeV32_t *attr) {
    if (attr == NULL) {
        return 0;
    }

    if ((attr->flags & ATTR_FLAG_WRITE_PENDING) == 0u) {
        return 0;
    }

    attr->flags = (uint8_t)(attr->flags & (uint8_t)(~ATTR_FLAG_WRITE_PENDING));

    return 1;
}

int reportAttributesWithTimestamp(uint8_t groupId, uint8_t priority, uint32_t timestamp,
                                  const uint16_t *ids, uint16_t num) {
    attributeGroup_t *group = NULL;

    if ((ids == NULL) || (num == 0u)) {
        return RET_INVALID_ARGS;
    }

    for (int i = 0; i < s_groupCount; i++) {
        const attributeGroup_t *candidate = s_groups[i];

        if (num == 0u) {
            continue;
        }

        if ((ids[0] >= candidate->minId) && (ids[0] <= candidate->maxId)) {
            group = s_groups[i];
            break;
        }
    }

    if (group == NULL) {
        return RET_NOT_SUPPORT; // 属性不在任何已注册组内
    }

    for (uint16_t i = 0; i < num; i++) {
        if (findAttributeInGroup(group, ids[i]) == NULL) {
            return RET_NOT_SUPPORT;
        }
    }

    s_lastReport.valid = 1u;
    s_lastReport.groupId = groupId;
    s_lastReport.priority = priority;
    s_lastReport.attrCount = num;
    s_lastReport.timestamp = timestamp;

    return RET_SUCCESS;
}

int sacpLastReport(uint8_t *groupId, uint16_t *attrCount) {
    if (s_lastReport.valid == 0u) {
        return RET_NO_OBJECT;
    }

    if (groupId != NULL) {
        *groupId = s_lastReport.groupId;
    }

    if (attrCount != NULL) {
        *attrCount = s_lastReport.attrCount;
    }

    return RET_SUCCESS;
}

int sacpGroupCount(void) {
    return s_groupCount;
}

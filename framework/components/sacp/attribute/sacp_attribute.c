/**
 * @file sacp_attribute.c
 * @brief 协议属性组件最小实现
 *
 * @par 执行上下文
 * 启动注册与任务上下文
 *
 * @par 可重入性
 * 否
 *
 * @par 资源所有权
 * 只保存调用方静态表的指针
 */

#include <common/generic.h>

#include "sacp_attribute.h"

#define SACP_GROUP_MAX 8

static attributeGroup_t *s_groups[SACP_GROUP_MAX];
static uint8_t s_groupCount;

static struct {
    bool valid;
    uint8_t groupId;
    uint8_t priority;
    uint16_t attrCount;
    uint32_t timestamp;
} s_lastReport;

fw_err_t attributeGroupRegister(attributeGroup_t *group) {
    if ((group == NULL) || (group->v32 == NULL) || (group->v32Num == 0u) ||
        (group->minId > group->maxId)) {
        return FW_ERR_INVALID_ARGS;
    }

    for (uint16_t i = 0u; i < group->v32Num; i++) {
        const attributeV32_t *attr = &group->v32[i];

        if ((attr->id < group->minId) || (attr->id > group->maxId) ||
            ((i > 0u) && (attr->id <= group->v32[i - 1u].id))) {
            return FW_ERR_INVALID_ARGS;
        }
    }

    for (uint8_t i = 0u; i < s_groupCount; i++) {
        if (s_groups[i] == group) {
            return FW_ERR_INVALID_ARGS;
        }
    }
    if (s_groupCount >= SACP_GROUP_MAX) {
        return FW_ERR_FAILED;
    }

    s_groups[s_groupCount] = group;
    s_groupCount++;
    return FW_OK;
}

fw_err_t findAttributeInGroup(const attributeGroup_t *group, uint16_t id,
                              attributeV32_t **attribute) {
    if (attribute == NULL) {
        return FW_ERR_INVALID_ARGS;
    }
    *attribute = NULL;

    if ((group == NULL) || (group->v32 == NULL)) {
        return FW_ERR_INVALID_ARGS;
    }

    for (uint16_t i = 0u; i < group->v32Num; i++) {
        if (group->v32[i].id == id) {
            *attribute = &group->v32[i];
            return FW_OK;
        }
    }
    return FW_ERR_NO_OBJECT;
}

fw_err_t localAttributeSetInt(attributeGroup_t *group, uint16_t id, uint32_t value) {
    attributeV32_t *attr = NULL;
    fw_err_t ret = findAttributeInGroup(group, id, &attr);

    if (ret != FW_OK) {
        return ret;
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
            return FW_ERR_TYPE_MISMATCH;
    }
    return FW_OK;
}

fw_err_t attributeTestAndClearWritePending(attributeV32_t *attr, bool *pending) {
    if ((attr == NULL) || (pending == NULL)) {
        return FW_ERR_INVALID_ARGS;
    }

    *pending = (attr->flags & ATTR_FLAG_WRITE_PENDING) != 0u;
    if (*pending) {
        attr->flags = (uint8_t)(attr->flags & (uint8_t)(~ATTR_FLAG_WRITE_PENDING));
    }
    return FW_OK;
}

fw_err_t reportAttributesWithTimestamp(uint8_t groupId, uint8_t priority, uint32_t timestamp,
                                       const uint16_t *ids, uint16_t num) {
    attributeGroup_t *group = NULL;

    if ((ids == NULL) || (num == 0u)) {
        return FW_ERR_INVALID_ARGS;
    }

    for (uint8_t i = 0u; i < s_groupCount; i++) {
        const attributeGroup_t *candidate = s_groups[i];

        if ((ids[0] >= candidate->minId) && (ids[0] <= candidate->maxId)) {
            group = s_groups[i];
            break;
        }
    }
    if (group == NULL) {
        return FW_ERR_NOT_SUPPORTED;
    }

    for (uint16_t i = 0u; i < num; i++) {
        attributeV32_t *attribute = NULL;
        if (findAttributeInGroup(group, ids[i], &attribute) != FW_OK) {
            return FW_ERR_NOT_SUPPORTED;
        }
    }

    s_lastReport.valid = true;
    s_lastReport.groupId = groupId;
    s_lastReport.priority = priority;
    s_lastReport.attrCount = num;
    s_lastReport.timestamp = timestamp;
    return FW_OK;
}

fw_err_t sacpLastReport(uint8_t *groupId, uint16_t *attrCount) {
    if (!s_lastReport.valid) {
        return FW_ERR_NO_OBJECT;
    }
    if (groupId != NULL) {
        *groupId = s_lastReport.groupId;
    }
    if (attrCount != NULL) {
        *attrCount = s_lastReport.attrCount;
    }
    return FW_OK;
}

fw_err_t sacpGetGroupCount(uint8_t *count) {
    if (count == NULL) {
        return FW_ERR_INVALID_ARGS;
    }
    *count = s_groupCount;
    return FW_OK;
}

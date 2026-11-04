#ifndef __SACP_ATTRIBUTE_H__
#define __SACP_ATTRIBUTE_H__

/**
 * @file sacp_attribute.h
 * @brief 协议属性表、属性组与上报组最小接口
 *
 * @par 执行上下文
 * 启动注册与任务上下文
 *
 * @par 可重入性
 * 否
 *
 * @par 资源所有权
 * 组件保存调用方静态属性表的指针
 */

#include <common/generic.h>

#define ATTR_TYPE_INT8    1
#define ATTR_TYPE_INT16   2
#define ATTR_TYPE_INT32   3
#define ATTR_TYPE_UINT8   4
#define ATTR_TYPE_UINT16  5
#define ATTR_TYPE_UINT32  6
#define ATTR_TYPE_FLOAT32 7

#define ATTR_FLAG_READ          0x01
#define ATTR_FLAG_WRITE         0x02
#define ATTR_FLAG_WRITE_PENDING 0x04

#define SACP_PRIORITY_LOWEST 0u

typedef union {
    int8_t v8;
    int16_t v16;
    int32_t v32;
    uint8_t u8;
    uint16_t u16;
    uint32_t u32;
    float f32;
} attributeValue_t;

typedef struct {
    uint16_t id;
    uint8_t type;
    uint8_t flags;
    attributeValue_t value;
} attributeV32_t;

typedef struct {
    uint16_t id;
    uint8_t type;
    uint8_t flags;
    uint64_t value;
} attributeV64_t;

typedef struct {
    uint16_t id;
    uint16_t len;
    uint8_t *data;
} attributeOctet_t;

typedef enum { SACP_CMD_READ = 0, SACP_CMD_WRITE } sacpCommand_t;

typedef struct {
    uint16_t attrId;
    sacpCommand_t cmd;
} sacpRequest_t;

typedef struct {
    uint16_t minId;
    uint16_t maxId;
    attributeV32_t *v32;
    uint16_t v32Num;
    const attributeV64_t *v64;
    uint16_t v64Num;
    const attributeOctet_t *octet;
    uint16_t octetNum;
    void (*writeApply)(const void *groupData, sacpRequest_t *request);
} attributeGroup_t;

#define LOCAL_V32_TABLE(_name)   static attributeV32_t s_##_name##V32[] =
#define LOCAL_V64_TABLE(_name)   static const attributeV64_t s_##_name##V64[1] =
#define LOCAL_OCTET_TABLE(_name) static const attributeOctet_t s_##_name##Octet[1] =

#define _INT8_ATTRIBUTE(_id)                                                                       \
    { .id = (_id), .type = ATTR_TYPE_INT8, .flags = ATTR_FLAG_READ }
#define _INT16_ATTRIBUTE(_id)                                                                      \
    { .id = (_id), .type = ATTR_TYPE_INT16, .flags = ATTR_FLAG_READ }
#define _INT32_ATTRIBUTE(_id)                                                                      \
    { .id = (_id), .type = ATTR_TYPE_INT32, .flags = ATTR_FLAG_READ }
#define _UINT8_ATTRIBUTE(_id)                                                                      \
    { .id = (_id), .type = ATTR_TYPE_UINT8, .flags = ATTR_FLAG_READ }
#define _UINT16_ATTRIBUTE(_id)                                                                     \
    { .id = (_id), .type = ATTR_TYPE_UINT16, .flags = ATTR_FLAG_READ }
#define _UINT32_ATTRIBUTE(_id)                                                                     \
    { .id = (_id), .type = ATTR_TYPE_UINT32, .flags = ATTR_FLAG_READ }
#define _INT32_ATTRIBUTE_RW(_id)                                                                   \
    { .id = (_id), .type = ATTR_TYPE_INT32, .flags = (ATTR_FLAG_READ | ATTR_FLAG_WRITE) }
#define _UINT8_ATTRIBUTE_RW(_id)                                                                   \
    { .id = (_id), .type = ATTR_TYPE_UINT8, .flags = (ATTR_FLAG_READ | ATTR_FLAG_WRITE) }
#define _UINT32_ATTRIBUTE_RW(_id)                                                                  \
    { .id = (_id), .type = ATTR_TYPE_UINT32, .flags = (ATTR_FLAG_READ | ATTR_FLAG_WRITE) }
#define _FLOAT32_ATTRIBUTE(_id)                                                                    \
    { .id = (_id), .type = ATTR_TYPE_FLOAT32, .flags = ATTR_FLAG_READ }

#define LOCAL_GROUP_DEFINE(_name, _min, _max, _cb)                                                 \
    static attributeGroup_t s_##_name##Group = {                                                   \
        .minId = (uint16_t)(_min),                                                                 \
        .maxId = (uint16_t)(_max),                                                                 \
        .v32 = s_##_name##V32,                                                                     \
        .v32Num = (uint16_t)ARRAY_SIZE(s_##_name##V32),                                            \
        .v64 = s_##_name##V64,                                                                     \
        .v64Num = 0u,                                                                              \
        .octet = s_##_name##Octet,                                                                 \
        .octetNum = 0u,                                                                            \
        .writeApply = (_cb),                                                                       \
    };                                                                                             \
    static attributeGroup_t *const s_group = &s_##_name##Group

#define LOCAL_REPORT_DEFINE(_name) static const uint16_t s_report_##_name[] =
#define LOCAL_REPORT(_name)        s_report_##_name, (uint16_t)ARRAY_SIZE(s_report_##_name)
#define REPORT_ID(_base, _index)                                                                   \
    ((uint8_t)((((uint8_t)(_base)) << 4) | ((uint8_t)(_index) & 0x0Fu)))

#define ATTRIBUTE_TEST_AND_CLEAR_WRITE_PENDING(_attr, _pending)                                    \
    attributeTestAndClearWritePending((_attr), (_pending))

fw_err_t attributeGroupRegister(attributeGroup_t *group);
fw_err_t findAttributeInGroup(const attributeGroup_t *group, uint16_t id,
                              attributeV32_t **attribute);
fw_err_t localAttributeSetInt(attributeGroup_t *group, uint16_t id, uint32_t value);
fw_err_t attributeTestAndClearWritePending(attributeV32_t *attr, bool *pending);
fw_err_t reportAttributesWithTimestamp(uint8_t groupId, uint8_t priority, uint32_t timestamp,
                                       const uint16_t *ids, uint16_t num);
fw_err_t sacpLastReport(uint8_t *groupId, uint16_t *attrCount);
fw_err_t sacpGetGroupCount(uint8_t *count);

#endif // __SACP_ATTRIBUTE_H__

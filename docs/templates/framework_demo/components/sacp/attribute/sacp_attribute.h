#ifndef __SACP_ATTRIBUTE_H__
#define __SACP_ATTRIBUTE_H__

/**
 * @file sacp_attribute.h
 * @author <author>
 * @brief 协议属性组件：属性表 / 属性组 / 上报组（表驱动，属性即数据）
 * @version 0.1
 * @date 2026-11-02
 *
 * @par 执行上下文
 *  注册在启动阶段；读写与上报在任务上下文
 *
 * @par 可重入性
 *  否。属性组为单例，调用方需串行化
 *
 * @note 本文件是 framework_demo 骨架的一部分：复制到真实仓库时展开为
 *       `framework/components/sacp/attribute/sacp_attribute.h`
 * @note 这是**最小实现**：只覆盖 32 位族属性的读/写/上报，
 *       64 位与 OCTET 表仅保留空壳，便于后续替换为完整 SACP 组件
 */

#include <common/generic.h>

/// 属性类型：有符号
#define ATTR_TYPE_INT8              1
#define ATTR_TYPE_INT16             2
#define ATTR_TYPE_INT32             3
/// 属性类型：无符号
#define ATTR_TYPE_UINT8             4
#define ATTR_TYPE_UINT16            5
#define ATTR_TYPE_UINT32            6
/// 属性类型：浮点
#define ATTR_TYPE_FLOAT32           7

/// 只读
#define ATTR_FLAG_READ              0x01
/// 可写
#define ATTR_FLAG_WRITE             0x02
/// 写入待应用（上层写入后置位，写回调处理后清除）
#define ATTR_FLAG_WRITE_PENDING     0x04

/// 上报优先级：最低
#define SACP_PRIORITY_LOWEST        0u

/**
 * @brief 属性值容器
 */
typedef union
{
    int8_t   v8;
    int16_t  v16;
    int32_t  v32;
    uint8_t  u8;
    uint16_t u16;
    uint32_t u32;
    float    f32;
} attributeValue_t;

/**
 * @brief 32 位族属性
 */
typedef struct
{
    uint16_t         id;
    uint8_t          type;
    uint8_t          flags;
    attributeValue_t value;
} attributeV32_t;

/**
 * @brief 64 位族属性（最小实现仅占位）
 */
typedef struct
{
    uint16_t         id;
    uint8_t          type;
    uint8_t          flags;
    uint64_t         value;
} attributeV64_t;

/// OCTET（字符串/二进制）属性（最小实现仅占位）
typedef struct
{
    uint16_t    id;
    uint16_t    len;
    uint8_t    *data;
} attributeOctet_t;

/// 协议命令（最小实现只需要区分读/写）
typedef enum
{
    SACP_CMD_READ = 0,
    SACP_CMD_WRITE
} sacpCommand_t;

/**
 * @brief 一次读写请求
 */
typedef struct
{
    uint16_t      attrId;
    sacpCommand_t cmd;
} sacpRequest_t;

/**
 * @brief 属性组：一张表 + 一个写回调 + 一个 ID 区间
 */
typedef struct
{
    uint16_t minId;
    uint16_t maxId;
    attributeV32_t *v32;
    uint16_t  v32Num;
    const attributeV64_t *v64;
    uint16_t  v64Num;
    const attributeOctet_t *octet;
    uint16_t  octetNum;
    void (*writeApply)(const void *groupData, sacpRequest_t *request);
} attributeGroup_t;

/* ==========================================================================
 * 表定义宏
 * ========================================================================== */

/// 32 位族属性表（后面紧跟 `{ ... };`）
#define LOCAL_V32_TABLE(_name)      static attributeV32_t s_##_name##V32[] =

/// 64 位族空表（后面紧跟 `{ };`）
#define LOCAL_V64_TABLE(_name)      static const attributeV64_t s_##_name##V64[1] =

/// OCTET 空表（后面紧跟 `{ };`）
#define LOCAL_OCTET_TABLE(_name)    static const attributeOctet_t s_##_name##Octet[1] =

/// 只读属性项
#define _INT8_ATTRIBUTE(_id)        { .id = (_id), .type = ATTR_TYPE_INT8,  .flags = ATTR_FLAG_READ }
#define _INT16_ATTRIBUTE(_id)       { .id = (_id), .type = ATTR_TYPE_INT16, .flags = ATTR_FLAG_READ }
#define _INT32_ATTRIBUTE(_id)       { .id = (_id), .type = ATTR_TYPE_INT32, .flags = ATTR_FLAG_READ }
#define _UINT8_ATTRIBUTE(_id)       { .id = (_id), .type = ATTR_TYPE_UINT8, .flags = ATTR_FLAG_READ }
#define _UINT16_ATTRIBUTE(_id)      { .id = (_id), .type = ATTR_TYPE_UINT16, .flags = ATTR_FLAG_READ }
#define _UINT32_ATTRIBUTE(_id)      { .id = (_id), .type = ATTR_TYPE_UINT32, .flags = ATTR_FLAG_READ }
/// 可读写属性项
#define _INT32_ATTRIBUTE_RW(_id)    { .id = (_id), .type = ATTR_TYPE_INT32, .flags = (ATTR_FLAG_READ | ATTR_FLAG_WRITE) }
#define _UINT8_ATTRIBUTE_RW(_id)    { .id = (_id), .type = ATTR_TYPE_UINT8, .flags = (ATTR_FLAG_READ | ATTR_FLAG_WRITE) }
#define _UINT32_ATTRIBUTE_RW(_id)   { .id = (_id), .type = ATTR_TYPE_UINT32, .flags = (ATTR_FLAG_READ | ATTR_FLAG_WRITE) }
#define _FLOAT32_ATTRIBUTE(_id)     { .id = (_id), .type = ATTR_TYPE_FLOAT32, .flags = ATTR_FLAG_READ }

/**
 * @brief 定义属性组（生成 `s_group` 指针，一个编译单元只允许一个）
 *
 * @param[in] _name 与 LOCAL_V32_TABLE 相同的表名
 * @param[in] _min  属性 ID 下界
 * @param[in] _max  属性 ID 上界
 * @param[in] _cb   写回调
 *
 * @note `s_group` 是指针量：模板代码里 `localAttributeSetInt(s_group, ...)`、
 *       `attributeGroupRegister(s_group)` 都按指针传递，语义与完整 SACP 一致
 */
#define LOCAL_GROUP_DEFINE(_name, _min, _max, _cb)                                      \
    static attributeGroup_t s_##_name##Group = {                                        \
        .minId = (uint16_t)(_min),                                                      \
        .maxId = (uint16_t)(_max),                                                      \
        .v32 = s_##_name##V32,                                                          \
        .v32Num = (uint16_t)ARRAY_SIZE(s_##_name##V32),                                 \
        .v64 = s_##_name##V64,                                                          \
        .v64Num = 0u,                                                                   \
        .octet = s_##_name##Octet,                                                      \
        .octetNum = 0u,                                                                 \
        .writeApply = (_cb),                                                            \
    };                                                                                  \
    static attributeGroup_t *const s_group = &s_##_name##Group

/// 上报组表（后面紧跟 `{ id, id, ... };`）
#define LOCAL_REPORT_DEFINE(_name)  static const uint16_t s_report_##_name[] =

/// 上报组引用（展开成"数组 + 元素个数"两个实参）
#define LOCAL_REPORT(_name)         s_report_##_name, (uint16_t)ARRAY_SIZE(s_report_##_name)

/// 上报组 ID：高 4 位为组基址，低 4 位为组内编号
#define REPORT_ID(_base, _index)                                                        \
    ((uint8_t)((((uint8_t)(_base)) << 4) | ((uint8_t)(_index) & 0x0Fu)))

/// 测试并清除"写入待应用"标志（返回 1 表示有待应用写入）
#define ATTRIBUTE_TEST_AND_CLEAR_WRITE_PENDING(_attr)  attributeTestAndClearWritePending(_attr)

/* ==========================================================================
 * API
 * ========================================================================== */

/**
 * @brief 注册属性组
 *
 * @param[in] group 属性组，生命周期必须覆盖运行期（static 存储）
 * @return int
 *  - RET_SUCCESS       成功
 *  - RET_INVALID_ARGS  参数非法（NULL、min>max、表为空、ID 越界或未升序、重复注册）
 *  - RET_FAILED        容量已满
 *
 * @note 调用上下文：启动阶段；可重入：否
 */
int attributeGroupRegister(attributeGroup_t *group);

/**
 * @brief 在组内按 ID 查找属性
 *
 * @return attributeV32_t* NULL 表示不存在
 */
attributeV32_t *findAttributeInGroup(const attributeGroup_t *group, uint16_t id);

/**
 * @brief 按属性类型写入整数值
 *
 * @param[in] group 属性组
 * @param[in] id    属性 ID
 * @param[in] value 值（按属性类型截断；写只读属性返回 RET_NOT_SUPPORT）
 * @return int RET_SUCCESS / RET_NO_OBJECT / RET_NOT_SUPPORT
 */
int localAttributeSetInt(attributeGroup_t *group, uint16_t id, uint32_t value);

/**
 * @brief 测试并清除"写入待应用"标志
 *
 * @param[in] attr 属性（可为 NULL）
 * @return int 1 = 有待应用写入并已清除；0 = 无
 */
int attributeTestAndClearWritePending(attributeV32_t *attr);

/**
 * @brief 上报一组属性
 *
 * @param[in] groupId   上报组 ID（见 REPORT_ID）
 * @param[in] priority  上报优先级
 * @param[in] timestamp 时间戳（ms）
 * @param[in] ids       属性 ID 数组
 * @param[in] num       数量
 * @return int RET_SUCCESS / RET_INVALID_ARGS / RET_NOT_SUPPORT（属性未注册）
 *
 * @note 最小实现只做校验与记录；真正的发送由传输层接管
 */
int reportAttributesWithTimestamp(uint8_t groupId, uint8_t priority, uint32_t timestamp,
                                  const uint16_t *ids, uint16_t num);

/**
 * @brief 取最近一次上报的摘要（供 CLI / 单测验证）
 *
 * @param[out] groupId   组 ID（可 NULL）
 * @param[out] attrCount 属性数量（可 NULL）
 * @return int RET_SUCCESS / RET_NO_OBJECT（从未上报）
 */
int sacpLastReport(uint8_t *groupId, uint16_t *attrCount);

/**
 * @brief 已注册属性组数量
 */
int sacpGroupCount(void);

#endif // __SACP_ATTRIBUTE_H__

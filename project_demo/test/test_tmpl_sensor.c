/**
 * @file test_tmpl_sensor.c
 * @author <author>
 * @brief 模板模块的 host 单测（对应流程步骤 ⑨）——**AI 自闭环验证的关键**
 * @version 0.1
 * @date 2026-10-29
 *
 * @note
 *  - 纯逻辑（换算 / 状态机 / 参数解析 / 错误码）必须在 host 上有单测
 *  - 硬件适配层不追覆盖率，用"一致性测试"替代（见 docs/guides/porting-guide.md）
 *  - 本文件自带假总线（mock），不依赖任何芯片
 *
 * @par 编译
 *  cmake -S docs/templates -B build && cmake --build build &&
 *  ctest --test-dir build --output-on-failure
 */

#include <stdio.h>
#include <string.h>

#include "tmpl_sensor.h"

/* ==========================================================================
 * 断言
 * --------------------------------------------------------------------------
 *  模板默认自带极简断言，保证**零依赖可直接跑**。
 *  接入 Unity/CMock 后，只需把下面三个宏映射到 TEST_ASSERT_*，
 *  并把 main() 换成 UNITY_BEGIN / RUN_TEST / UNITY_END 形式。
 * ========================================================================== */

static int s_failures = 0;

#define CHECK_EQ(_expected, _actual)                                                               \
    do {                                                                                           \
        long _e = (long)(_expected);                                                               \
        long _a = (long)(_actual);                                                                 \
        if (_e != _a) {                                                                            \
            printf("  FAIL %s:%d  expected=%ld actual=%ld\n", __FILE__, __LINE__, _e, _a);         \
            s_failures++;                                                                          \
        }                                                                                          \
    } while (0)

#define CHECK_TRUE(_cond)                                                                          \
    do {                                                                                           \
        if (!(_cond)) {                                                                            \
            printf("  FAIL %s:%d  expected true\n", __FILE__, __LINE__);                           \
            s_failures++;                                                                          \
        }                                                                                          \
    } while (0)

/* ==========================================================================
 * 假总线（mock）：实现驱动所依赖的抽象总线接口
 * ========================================================================== */

/// 由测试预设的"器件返回值"
typedef struct {
    fw_err_t readRet;
    uint8_t regs[8]; /* 从 TMPL_REG_STATUS 起始的内容 */
    uint32_t writeCount;
} fakeBus_t;

static fakeBus_t s_fake;

fw_err_t i2cWriteRegs(uint8_t bus, uint16_t addr, uint8_t reg, const uint8_t *buf, uint16_t len) {
    (void)bus;
    (void)addr;
    (void)reg;
    (void)buf;
    (void)len;

    s_fake.writeCount++;

    return s_fake.readRet;
}

fw_err_t i2cReadRegs(uint8_t bus, uint16_t addr, uint8_t reg, uint8_t *buf, uint16_t len) {
    (void)bus;
    (void)addr;

    if ((s_fake.readRet != FW_OK) || (buf == NULL)) {
        return s_fake.readRet;
    }

    for (uint16_t i = 0; i < len; i++) {
        buf[i] = s_fake.regs[reg + i];
    }

    return FW_OK;
}

fw_err_t osTickMs(uint32_t *tickMs) {
    static uint32_t tick = 0;

    if (tickMs == NULL) {
        return FW_ERR_INVALID_ARGS;
    }
    *tickMs = tick++;
    return FW_OK;
}

/* ==========================================================================
 * 测试用例
 * ========================================================================== */

#define TEST_ID_A 0
#define TEST_ID_B 1

static const tmplSensorConfig_t s_cfgs[] = {
    {
        .id = TEST_ID_A,
        .chip = TMPL_SENSOR_CHIP_A,
        .bus = 0,
        .address = 0x48,
        .flags = 0,
        .chipParameter = "range:500;scale:10",
    },
    {
        .id = TEST_ID_B,
        .chip = TMPL_SENSOR_CHIP_B,
        .bus = 0,
        .address = 0x49,
        .flags = TMPL_SENSOR_FLAG_INVERT,
        .chipParameter = NULL,
    },
};

static void setUp(void) {
    memset(&s_fake, 0, sizeof(s_fake));
    s_fake.readRet = FW_OK;
}

/**
 * 场景：正常初始化两份描述符 → 返回数量与实例数一致
 */
static void test_init_binds_instances(void) {
    uint8_t initialized = 0u;
    uint8_t count = 0u;

    CHECK_EQ(FW_OK, tmplSensorInit(s_cfgs, 2u, &initialized));
    CHECK_EQ(2, initialized);
    CHECK_EQ(FW_OK, tmplSensorGetCount(&count));
    CHECK_EQ(2, count);
}

/**
 * 场景：描述符数量超过容量宏 → 返回参数错误，不允许越界写入
 */
static void test_init_rejects_overflow(void) {
    tmplSensorConfig_t many[CONFIG_TMPL_SENSOR_MAX_NUM + 1];
    uint8_t initialized = 0xffu;

    memset(many, 0, sizeof(many));
    for (uint8_t i = 0u; i <= CONFIG_TMPL_SENSOR_MAX_NUM; i++) {
        many[i].id = i;
        many[i].chip = TMPL_SENSOR_CHIP_A;
    }

    CHECK_EQ(FW_ERR_INVALID_ARGS,
             tmplSensorInit(many, (uint8_t)(CONFIG_TMPL_SENSOR_MAX_NUM + 1), &initialized));
    CHECK_EQ(0, initialized);
}

/**
 * 场景：重复 ID 只保留第一个，且总数正确
 */
static void test_init_skips_duplicate_id(void) {
    tmplSensorConfig_t dup[2];
    uint8_t initialized = 0u;
    uint8_t count = 0u;

    memset(dup, 0, sizeof(dup));
    dup[0].id = 7;
    dup[0].chip = TMPL_SENSOR_CHIP_A;
    dup[1].id = 7;
    dup[1].chip = TMPL_SENSOR_CHIP_A;

    CHECK_EQ(FW_OK, tmplSensorInit(dup, 2u, &initialized));
    CHECK_EQ(1, initialized);
    CHECK_EQ(FW_OK, tmplSensorGetCount(&count));
    CHECK_EQ(1, count);
}

/**
 * 场景：读未注册的 ID → FW_ERR_NO_OBJECT，而不是崩溃
 */
static void test_read_unknown_id_returns_no_object(void) {
    tmplSensorData_t data;
    uint8_t initialized = 0u;

    memset(&data, 0x5A, sizeof(data));
    CHECK_EQ(FW_OK, tmplSensorInit(s_cfgs, 1u, &initialized));

    CHECK_EQ(FW_ERR_NO_OBJECT, tmplSensorRead(99u, &data));
    CHECK_EQ(0x5A5A5A5A, data.value);
}

/**
 * 场景：型号 A 换算（chipParameter 的 scale 生效）
 *   原始值 1234，scale=10 → value 123
 */
static void test_chip_a_read_applies_scale(void) {
    tmplSensorData_t data;
    uint8_t initialized = 0u;

    CHECK_EQ(FW_OK, tmplSensorInit(s_cfgs, 1u, &initialized));

    s_fake.regs[0] = 0x00; /* status */
    s_fake.regs[1] = 0x04; /* value H */
    s_fake.regs[2] = 0xD2; /* value L  => 1234 */

    CHECK_EQ(FW_OK, tmplSensorRead(TEST_ID_A, &data));
    CHECK_EQ(123, data.value);
    CHECK_EQ(0, data.status);
    CHECK_EQ(1, data.sequence);
}

/**
 * 场景：型号 B 的 INVERT 标志生效
 */
static void test_chip_b_read_applies_invert(void) {
    tmplSensorData_t data;
    uint8_t initialized = 0u;

    CHECK_EQ(FW_OK, tmplSensorInit(s_cfgs, 2u, &initialized));

    s_fake.regs[2] = 0x00; /* value H */
    s_fake.regs[3] = 0x64; /* value L  => 100 */
    s_fake.regs[4] = 0x01; /* temp H */
    s_fake.regs[5] = 0x2C; /* temp L   => 300 (30.0℃) */

    CHECK_EQ(FW_OK, tmplSensorRead(TEST_ID_B, &data));
    CHECK_EQ(-100, data.value);
    CHECK_EQ(300, data.temperature);
}

/**
 * 场景：总线失败 → 映射为超时错误码，而不是返回陈旧数据
 */
static void test_read_maps_bus_error(void) {
    tmplSensorData_t data;
    uint8_t initialized = 0u;

    memset(&data, 0x5A, sizeof(data));
    CHECK_EQ(FW_OK, tmplSensorInit(s_cfgs, 1u, &initialized));
    s_fake.readRet = FW_ERR_TIMEOUT;

    CHECK_EQ(FW_ERR_TIMEOUT, tmplSensorRead(TEST_ID_A, &data));
    CHECK_EQ(0x5A5A5A5A, data.value);

    s_fake.readRet = FW_OK;
    s_fake.regs[0] = 0x00;
    s_fake.regs[1] = 0x04;
    s_fake.regs[2] = 0xD2;
    CHECK_EQ(FW_OK, tmplSensorRead(TEST_ID_A, &data));
    CHECK_EQ(1, data.sequence);
}

/**
 * 场景：型号 A 不支持周期模式
 */
static void test_mode_not_supported(void) {
    uint8_t initialized = 0u;

    CHECK_EQ(FW_OK, tmplSensorInit(s_cfgs, 1u, &initialized));
    CHECK_EQ(FW_ERR_NOT_SUPPORTED, tmplSensorSetMode(TEST_ID_A, TMPL_SENSOR_MODE_AUTO_100MS));
    CHECK_EQ(FW_OK, tmplSensorSetMode(TEST_ID_A, TMPL_SENSOR_MODE_SINGLE));
}

/**
 * 场景：按序号遍历（CLI 依赖此接口）
 */
static void test_get_id_by_index(void) {
    uint8_t id = 0xff;
    uint8_t initialized = 0u;

    CHECK_EQ(FW_OK, tmplSensorInit(s_cfgs, 2u, &initialized));

    CHECK_EQ(FW_OK, tmplSensorGetIdByIndex(0u, &id));
    CHECK_EQ(TEST_ID_A, id);
    CHECK_EQ(FW_OK, tmplSensorGetIdByIndex(1u, &id));
    CHECK_EQ(TEST_ID_B, id);
    CHECK_EQ(FW_ERR_NO_OBJECT, tmplSensorGetIdByIndex(2u, &id));
    CHECK_EQ(TEST_ID_B, id);
}

/* ==========================================================================
 * Runner
 * ========================================================================== */

typedef struct {
    const char *name;
    void (*fn)(void);
} testCase_t;

static const testCase_t s_cases[] = {
    {"init_binds_instances", test_init_binds_instances},
    {"init_rejects_overflow", test_init_rejects_overflow},
    {"init_skips_duplicate_id", test_init_skips_duplicate_id},
    {"read_unknown_id", test_read_unknown_id_returns_no_object},
    {"chip_a_read_applies_scale", test_chip_a_read_applies_scale},
    {"chip_b_read_applies_invert", test_chip_b_read_applies_invert},
    {"read_maps_bus_error", test_read_maps_bus_error},
    {"mode_not_supported", test_mode_not_supported},
    {"get_id_by_index", test_get_id_by_index},
};

int main(void) {
    for (unsigned i = 0; i < sizeof(s_cases) / sizeof(s_cases[0]); i++) {
        printf("[ RUN  ] %s\n", s_cases[i].name);

        setUp();
        s_cases[i].fn();

        printf("[ DONE ] %s\n", s_cases[i].name);
    }

    printf("\n%s (%d failure(s))\n", (s_failures == 0) ? "PASS" : "FAIL", s_failures);

    return (s_failures == 0) ? 0 : 1;
}

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
 *  make host 或 cmake --preset host && ctest --preset host
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

#define CHECK_EQ(_expected, _actual)                                            \
    do {                                                                        \
        long _e = (long)(_expected);                                            \
        long _a = (long)(_actual);                                              \
        if (_e != _a) {                                                         \
            printf("  FAIL %s:%d  expected=%ld actual=%ld\n",                   \
                   __FILE__, __LINE__, _e, _a);                                 \
            s_failures ++;                                                      \
        }                                                                       \
    } while (0)

#define CHECK_TRUE(_cond)                                                       \
    do {                                                                        \
        if (!(_cond)) {                                                         \
            printf("  FAIL %s:%d  expected true\n", __FILE__, __LINE__);         \
            s_failures ++;                                                      \
        }                                                                       \
    } while (0)

/* ==========================================================================
 * 假总线（mock）：实现驱动所依赖的抽象总线接口
 * ========================================================================== */

/// 由测试预设的"器件返回值"
typedef struct
{
    int     readRet;        /* 0 = 成功 */
    uint8_t regs[8];        /* 从 TMPL_REG_STATUS 起始的内容 */
    int     writeCount;
    int     lastMode;
} fakeBus_t;

static fakeBus_t s_fake;

int i2cWriteRegs(uint8_t bus, uint16_t addr, uint8_t reg, const uint8_t *buf, uint16_t len)
{
    (void)bus;
    (void)addr;
    (void)reg;
    (void)buf;
    (void)len;

    s_fake.writeCount ++;

    return s_fake.readRet;
}

int i2cReadRegs(uint8_t bus, uint16_t addr, uint8_t reg, uint8_t *buf, uint16_t len)
{
    (void)bus;
    (void)addr;

    if ((s_fake.readRet != 0) || (buf == NULL))
    {
        return -1;
    }

    for (uint16_t i = 0; i < len; i ++)
    {
        buf[i] = s_fake.regs[reg + i];
    }

    return 0;
}

uint32_t osTickMs(void)
{
    static uint32_t tick = 0;

    return tick ++;
}

/* ==========================================================================
 * 测试用例
 * ========================================================================== */

#define TEST_ID_A       0
#define TEST_ID_B       1

static const tmplSensorConfig_t s_cfgs[] =
{
    {
        .id            = TEST_ID_A,
        .chip          = TMPL_SENSOR_CHIP_A,
        .bus           = 0,
        .address       = 0x48,
        .flags         = 0,
        .chipParameter = "range:500;scale:10",
    },
    {
        .id            = TEST_ID_B,
        .chip          = TMPL_SENSOR_CHIP_B,
        .bus           = 0,
        .address       = 0x49,
        .flags         = TMPL_SENSOR_FLAG_INVERT,
        .chipParameter = NULL,
    },
};

static void setUp(void)
{
    memset(&s_fake, 0, sizeof(s_fake));
    s_fake.readRet = 0;
}

/**
 * 场景：正常初始化两份描述符 → 返回数量与实例数一致
 */
static void test_init_binds_instances(void)
{
    int n = tmplSensorInit(s_cfgs, 2);

    CHECK_EQ(2, n);
    CHECK_EQ(2, tmplSensorGetCount());
}

/**
 * 场景：描述符数量超过容量宏 → 返回参数错误，不允许越界写入
 */
static void test_init_rejects_overflow(void)
{
    tmplSensorConfig_t many[CONFIG_TMPL_SENSOR_MAX_NUM + 1];

    memset(many, 0, sizeof(many));
    for (int i = 0; i <= CONFIG_TMPL_SENSOR_MAX_NUM; i ++)
    {
        many[i].id   = (uint8_t)i;
        many[i].chip = TMPL_SENSOR_CHIP_A;
    }

    CHECK_EQ(RET_INVALID_ARGS, tmplSensorInit(many, CONFIG_TMPL_SENSOR_MAX_NUM + 1));
}

/**
 * 场景：重复 ID 只保留第一个，且总数正确
 */
static void test_init_skips_duplicate_id(void)
{
    tmplSensorConfig_t dup[2];

    memset(dup, 0, sizeof(dup));
    dup[0].id = 7; dup[0].chip = TMPL_SENSOR_CHIP_A;
    dup[1].id = 7; dup[1].chip = TMPL_SENSOR_CHIP_A;

    CHECK_EQ(1, tmplSensorInit(dup, 2));
    CHECK_EQ(1, tmplSensorGetCount());
}

/**
 * 场景：读未注册的 ID → RET_NO_OBJECT，而不是崩溃
 */
static void test_read_unknown_id_returns_no_object(void)
{
    tmplSensorData_t data;

    tmplSensorInit(s_cfgs, 1);

    CHECK_EQ(RET_NO_OBJECT, tmplSensorRead(99, &data));
}

/**
 * 场景：型号 A 换算（chipParameter 的 scale 生效）
 *   原始值 1234，scale=10 → value 123
 */
static void test_chip_a_read_applies_scale(void)
{
    tmplSensorData_t data;

    tmplSensorInit(s_cfgs, 1);

    s_fake.regs[0] = 0x00;      /* status */
    s_fake.regs[1] = 0x04;      /* value H */
    s_fake.regs[2] = 0xD2;      /* value L  => 1234 */

    CHECK_EQ(RET_SUCCESS, tmplSensorRead(TEST_ID_A, &data));
    CHECK_EQ(123, data.value);
    CHECK_EQ(0, data.status);
    CHECK_EQ(1, data.sequence);
}

/**
 * 场景：型号 B 的 INVERT 标志生效
 */
static void test_chip_b_read_applies_invert(void)
{
    tmplSensorData_t data;

    tmplSensorInit(s_cfgs, 2);

    s_fake.regs[2] = 0x00;      /* value H */
    s_fake.regs[3] = 0x64;      /* value L  => 100 */
    s_fake.regs[4] = 0x01;      /* temp H */
    s_fake.regs[5] = 0x2C;      /* temp L   => 300 (30.0℃) */

    CHECK_EQ(RET_SUCCESS, tmplSensorRead(TEST_ID_B, &data));
    CHECK_EQ(-100, data.value);
    CHECK_EQ(300, data.temperature);
}

/**
 * 场景：总线失败 → 映射为超时错误码，而不是返回陈旧数据
 */
static void test_read_maps_bus_error(void)
{
    tmplSensorData_t data;

    tmplSensorInit(s_cfgs, 1);
    s_fake.readRet = -1;

    CHECK_EQ(RET_TIMEOUT, tmplSensorRead(TEST_ID_A, &data));
}

/**
 * 场景：型号 A 不支持周期模式
 */
static void test_mode_not_supported(void)
{
    tmplSensorInit(s_cfgs, 1);

    CHECK_EQ(RET_NOT_SUPPORT, tmplSensorSetMode(TEST_ID_A, TMPL_SENSOR_MODE_AUTO_100MS));
    CHECK_EQ(RET_SUCCESS, tmplSensorSetMode(TEST_ID_A, TMPL_SENSOR_MODE_SINGLE));
}

/**
 * 场景：按序号遍历（CLI 依赖此接口）
 */
static void test_get_id_by_index(void)
{
    uint8_t id = 0xff;

    tmplSensorInit(s_cfgs, 2);

    CHECK_EQ(RET_SUCCESS, tmplSensorGetIdByIndex(0, &id));
    CHECK_EQ(TEST_ID_A, id);
    CHECK_EQ(RET_SUCCESS, tmplSensorGetIdByIndex(1, &id));
    CHECK_EQ(TEST_ID_B, id);
    CHECK_EQ(RET_NO_OBJECT, tmplSensorGetIdByIndex(2, &id));
}

/* ==========================================================================
 * Runner
 * ========================================================================== */

typedef struct
{
    const char *name;
    void (*fn)(void);
} testCase_t;

static const testCase_t s_cases[] =
{
    { "init_binds_instances",           test_init_binds_instances },
    { "init_rejects_overflow",          test_init_rejects_overflow },
    { "init_skips_duplicate_id",        test_init_skips_duplicate_id },
    { "read_unknown_id",                test_read_unknown_id_returns_no_object },
    { "chip_a_read_applies_scale",      test_chip_a_read_applies_scale },
    { "chip_b_read_applies_invert",     test_chip_b_read_applies_invert },
    { "read_maps_bus_error",            test_read_maps_bus_error },
    { "mode_not_supported",             test_mode_not_supported },
    { "get_id_by_index",                test_get_id_by_index },
};

int main(void)
{
    for (unsigned i = 0; i < sizeof(s_cases) / sizeof(s_cases[0]); i ++)
    {
        printf("[ RUN  ] %s\n", s_cases[i].name);

        setUp();
        s_cases[i].fn();

        printf("[ DONE ] %s\n", s_cases[i].name);
    }

    printf("\n%s (%d failure(s))\n", (s_failures == 0) ? "PASS" : "FAIL", s_failures);

    return (s_failures == 0) ? 0 : 1;
}

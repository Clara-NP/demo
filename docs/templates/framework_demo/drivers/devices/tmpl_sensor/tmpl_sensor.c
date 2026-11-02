/**
 * @file tmpl_sensor.c
 * @author <author>
 * @brief 模板模块实现：控制器单例 + 静态实例数组 + 芯片分发
 * @version 0.1
 * @date 2026-10-29
 *
 * @note
 *  - 本文件是"满分模块"的实现范例，对应 docs/guides/module-development.md 的步骤 ④
 *  - 只依赖抽象总线接口，**任何芯片相关代码都不允许出现在这里**
 *  - 无动态内存：实例数组在编译期确定大小
 */

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <common/generic.h>
#include <i2c/i2c.h>

#include "tmpl_sensor.h"

#define TRACE_ENABLE
#define TRACE_LEVEL T_INFO
#define TRACE_TAG   "tmplsensor"
#include <common/trace.h>

/* ==========================================================================
 * 外部依赖（唯一耦合点：抽象总线接口）
 * --------------------------------------------------------------------------
 * 需要由环境提供的抽象 i2c 驱动实现以下签名（名字以实际环境为准）：
 *
 *   int      i2cWriteRegs(uint8_t bus, uint16_t addr, uint8_t reg, const uint8_t *buf, uint16_t len);
 *   int      i2cReadRegs (uint8_t bus, uint16_t addr, uint8_t reg, uint8_t *buf, uint16_t len);
 *
 * 以及 osal 时间：
 *   uint32_t osTickMs(void);
 *
 * 这些都不含芯片符号，因此本驱动可跨平台复用。
 * ========================================================================== */

/**
 * @brief osal 时间接口声明
 *
 * @note 若环境已有 osal 公开头（如 <osal/osal.h>），请改为 include 并删除本行
 */
extern uint32_t osTickMs(void);

/// 寄存器定义（器件私有，集中在此，便于对照数据手册）
#define TMPL_REG_STATUS  0x00
#define TMPL_REG_CONFIG  0x01
/// 型号 B 的一次读取覆盖 value(H/L) + temp(H/L) 共 4 字节
#define TMPL_REG_VALUE_H 0x02

/*
 * 超时约定：本驱动不做忙等/循环重试，超时责任在抽象总线层
 * （i2cReadRegs 内部必须带超时并返回错误码）。若某总线实现无超时能力，
 * 应在其驱动内补 CONFIG_<BUS>_TIMEOUT_MS，而不是在业务模块里加重试。
 */

/* ==========================================================================
 * 实例与控制器
 * ========================================================================== */

/**
 * @brief 一个传感器实例（运行时状态）
 *
 * @details 只保存"随时间变化"的部分；只读配置通过 config 指针间接访问
 */
typedef struct {
    /// 绑定的描述符（指向板级 static const 表，生命周期覆盖运行期）
    const tmplSensorConfig_t *config;

    /// 当前模式，见 ::TMPL_SENSOR_MODE
    uint8_t mode;
    /// 采样序号
    uint8_t sequence;
    /// 最近一次采样 tick
    uint32_t lastTick;
    /// 由 chipParameter 解析出的量程，0 表示未指定
    uint16_t range;
    /// 由 chipParameter 解析出的缩放倍数，0 表示未指定
    uint16_t scale;
} tmplSensorInstance_t;

/**
 * @brief 模块控制器（单例）
 *
 * @details MCU 上每个驱动模块只有一个控制器；实例通过静态数组承载
 */
typedef struct {
    tmplSensorInstance_t instances[CONFIG_TMPL_SENSOR_MAX_NUM];
    uint8_t count;
    uint8_t initialized;
} tmplSensorController_t;

static tmplSensorController_t s_ctl = {0};

#define INSTANCE_SIZE ARRAY_SIZE(s_ctl.instances)

/* ==========================================================================
 * 内部工具
 * ========================================================================== */

/**
 * @brief ID → 实例
 *
 * @param id 逻辑 ID
 * @return tmplSensorInstance_t*
 *  - NULL 表示该 ID 未注册
 */
static tmplSensorInstance_t *instanceFromId(uint8_t id) {
    for (int i = 0; i < (int)INSTANCE_SIZE; i++) {
        if ((s_ctl.instances[i].config != NULL) && (s_ctl.instances[i].config->id == id)) {
            return &s_ctl.instances[i];
        }
    }

    return NULL;
}

/**
 * @brief 解析 chipParameter，形如 "range:500;scale:10"
 *
 * @param[in]  param 参数串，可为 NULL
 * @param[out] inst  解析结果写回实例
 *
 * @note 解析失败不致命：保留默认值并打印警告（见 docs 中"弱类型"优化项）
 */
static void parseChipParameter(const char *param, tmplSensorInstance_t *inst) {
    const char *p = param;

    if (p == NULL) {
        return;
    }

    while (*p != '\0') {
        if (strncmp(p, "range:", 6) == 0) {
            inst->range = (uint16_t)strtoul(p + 6, NULL, 10);
        } else if (strncmp(p, "scale:", 6) == 0) {
            inst->scale = (uint16_t)strtoul(p + 6, NULL, 10);
        } else {
            wlog("id:%d unknown chipParameter token: %.12s", inst->config->id, p);
        }

        // 跳到下一个 ';'
        while ((*p != '\0') && (*p != ';')) {
            p++;
        }
        if (*p == ';') {
            p++;
        }
    }
}

/**
 * @brief 对原始值应用 flags 与 chipParameter 换算
 */
static int32_t convertValue(const tmplSensorInstance_t *inst, int32_t raw) {
    int32_t value = raw;

    if (inst->config->flags & TMPL_SENSOR_FLAG_INVERT) {
        value = -value;
    }

    if (inst->scale > 0) {
        value = value / (int32_t)inst->scale;
    } else if (inst->config->flags & TMPL_SENSOR_FLAG_SCALE_0_1) {
        value = value / 10;
    }

    return value;
}

/* ==========================================================================
 * 芯片分发：所有型号差异集中在这几个函数
 * ========================================================================== */

/**
 * @brief 型号 A 探测（写配置寄存器并回读状态）
 */
static int chipAProbe(tmplSensorInstance_t *inst) {
    uint8_t status = 0xff;
    int ret = i2cReadRegs(inst->config->bus, inst->config->address, TMPL_REG_STATUS, &status, 1);
    if (ret != 0) {
        elog("id:%d chipA probe failed, ret:%d", inst->config->id, ret);
        return RET_NO_OBJECT;
    }

    return RET_SUCCESS;
}

/**
 * @brief 型号 A 读取（单次转换，3 字节：status + value）
 */
static int chipARead(tmplSensorInstance_t *inst, tmplSensorData_t *out) {
    uint8_t buf[3] = {0};
    int ret =
        i2cReadRegs(inst->config->bus, inst->config->address, TMPL_REG_STATUS, buf, sizeof(buf));
    if (ret != 0) {
        return RET_TIMEOUT;
    }

    out->status = buf[0];
    out->value = convertValue(inst, (int32_t)((buf[1] << 8) | buf[2]));

    return RET_SUCCESS;
}

/**
 * @brief 型号 B 探测
 */
static int chipBProbe(tmplSensorInstance_t *inst) {
    uint8_t cfg = 0;

    int ret = i2cWriteRegs(inst->config->bus, inst->config->address, TMPL_REG_CONFIG, &cfg, 1);
    if (ret != 0) {
        elog("id:%d chipB probe failed, ret:%d", inst->config->id, ret);
        return RET_NO_OBJECT;
    }

    return RET_SUCCESS;
}

/**
 * @brief 型号 B 读取（4 字节：status + value + temp）
 */
static int chipBRead(tmplSensorInstance_t *inst, tmplSensorData_t *out) {
    uint8_t buf[4] = {0};
    int ret =
        i2cReadRegs(inst->config->bus, inst->config->address, TMPL_REG_VALUE_H, buf, sizeof(buf));
    if (ret != 0) {
        return RET_TIMEOUT;
    }

    out->value = convertValue(inst, (int32_t)((buf[0] << 8) | buf[1]));
    out->temperature = (int16_t)((buf[2] << 8) | buf[3]);
    out->status = 0;

    return RET_SUCCESS;
}

/**
 * @brief 型号是否支持模式
 */
static bool chipSupportMode(uint8_t chip, uint8_t mode) {
    switch (chip) {
        case TMPL_SENSOR_CHIP_A:
            return (mode == TMPL_SENSOR_MODE_SINGLE);

        case TMPL_SENSOR_CHIP_B:
            return (mode < TMPL_SENSOR_MODE_END);

        default:
            return false;
    }
}

/* ==========================================================================
 * 对外 API
 * ========================================================================== */

int tmplSensorInit(const tmplSensorConfig_t *configs, int num) {
    int inited = 0;

    if ((configs == NULL) || (num <= 0)) {
        return RET_INVALID_ARGS;
    }

    if (num > (int)INSTANCE_SIZE) {
        elog("too many sensors: %d > %d", num, (int)INSTANCE_SIZE);
        return RET_INVALID_ARGS;
    }

    // 重复初始化：先清空控制器状态，保证幂等
    memset(&s_ctl, 0, sizeof(s_ctl));

    for (int i = 0; i < num; i++) {
        const tmplSensorConfig_t *cfg = &configs[i];
        tmplSensorInstance_t *inst = &s_ctl.instances[i];

        if (cfg->chip >= TMPL_SENSOR_CHIP_NUM) {
            elog("id:%d invalid chip:%d", cfg->id, cfg->chip);
            continue;
        }

        if (instanceFromId(cfg->id) != NULL) {
            elog("duplicated id:%d, skipped", cfg->id);
            continue;
        }

        inst->config = cfg;
        inst->mode = TMPL_SENSOR_MODE_SINGLE;
        parseChipParameter(cfg->chipParameter, inst);

        int ret = RET_NOT_SUPPORT;
        switch (cfg->chip) {
            case TMPL_SENSOR_CHIP_A:
                ret = chipAProbe(inst);
                break;

            case TMPL_SENSOR_CHIP_B:
                ret = chipBProbe(inst);
                break;

            default:
                break;
        }

        if (ret != RET_SUCCESS) {
            elog("id:%d init failed, chip:%d ret:%d", cfg->id, cfg->chip, ret);
            inst->config = NULL; // 释放槽位，避免半初始化实例被找到
            continue;
        }

        ilog("id:%d chip:%d bus:%d addr:0x%02x range:%d scale:%d", cfg->id, cfg->chip, cfg->bus,
             cfg->address, inst->range, inst->scale);
        inited++;
        s_ctl.count++;
    }

    s_ctl.initialized = (inited > 0) ? 1 : 0;

    return inited;
}

int tmplSensorRead(uint8_t id, tmplSensorData_t *out) {
    tmplSensorInstance_t *inst;

    if (out == NULL) {
        return RET_INVALID_ARGS;
    }

    if (!s_ctl.initialized) {
        return RET_INVALID_ARGS;
    }

    inst = instanceFromId(id);
    if (inst == NULL) {
        return RET_NO_OBJECT;
    }

    memset(out, 0, sizeof(*out));
    out->sequence = ++inst->sequence;
    inst->lastTick = osTickMs();

    switch (inst->config->chip) {
        case TMPL_SENSOR_CHIP_A:
            return chipARead(inst, out);

        case TMPL_SENSOR_CHIP_B:
            return chipBRead(inst, out);

        default:
            // 只能出现在"新增 chip 枚举但忘了加分支"的场景
            elog("id:%d unsupported chip:%d", id, inst->config->chip);
            return RET_NOT_SUPPORT;
    }
}

int tmplSensorSetMode(uint8_t id, uint8_t mode) {
    tmplSensorInstance_t *inst = instanceFromId(id);

    if (inst == NULL) {
        return RET_NO_OBJECT;
    }

    if (!chipSupportMode(inst->config->chip, mode)) {
        return RET_NOT_SUPPORT;
    }

    inst->mode = mode;
    ilog("id:%d set mode:%d", id, mode);

    return RET_SUCCESS;
}

int tmplSensorGetConfig(uint8_t id, const tmplSensorConfig_t **config) {
    tmplSensorInstance_t *inst;

    if (config == NULL) {
        return RET_INVALID_ARGS;
    }

    inst = instanceFromId(id);
    if (inst == NULL) {
        return RET_NO_OBJECT;
    }

    *config = inst->config;

    return RET_SUCCESS;
}

int tmplSensorGetCount(void) {
    return (int)s_ctl.count;
}

int tmplSensorGetIdByIndex(int index, uint8_t *id) {
    int seen = 0;

    if (id == NULL) {
        return RET_INVALID_ARGS;
    }

    for (int i = 0; i < (int)INSTANCE_SIZE; i++) {
        if (s_ctl.instances[i].config == NULL) {
            continue;
        }

        if (seen == index) {
            *id = s_ctl.instances[i].config->id;
            return RET_SUCCESS;
        }

        seen++;
    }

    return RET_NO_OBJECT;
}

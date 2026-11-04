/**
 * @file tmpl_sensor.c
 * @brief 传感器示例驱动：静态实例、参数解析与芯片分发
 *
 * @par 执行上下文
 * 初始化阶段与任务上下文
 *
 * @par 可重入性
 * 否，同一实例由调用方串行化
 *
 * @par 资源所有权
 * 持有静态实例数组；不动态分配
 */

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <common/generic.h>
#include <common/trace.h>
#include <i2c/i2c.h>
#include <osal/osal.h>

#include "tmpl_sensor.h"

#define TMPL_REG_STATUS  0x00
#define TMPL_REG_CONFIG  0x01
#define TMPL_REG_VALUE_H 0x02

typedef struct {
    const tmplSensorConfig_t *config;
    uint8_t mode;
    uint8_t sequence;
    uint32_t lastTick;
    uint16_t range;
    uint16_t scale;
} tmplSensorInstance_t;

typedef struct {
    tmplSensorInstance_t instances[CONFIG_TMPL_SENSOR_MAX_NUM];
    uint8_t count;
    bool initialized;
} tmplSensorController_t;

static tmplSensorController_t s_ctl;

#define INSTANCE_SIZE ((uint8_t)ARRAY_SIZE(s_ctl.instances))

static tmplSensorInstance_t *instanceFromId(uint8_t id) {
    for (uint8_t i = 0u; i < INSTANCE_SIZE; i++) {
        if ((s_ctl.instances[i].config != NULL) && (s_ctl.instances[i].config->id == id)) {
            return &s_ctl.instances[i];
        }
    }
    return NULL;
}

static void parseChipParameter(const char *param, tmplSensorInstance_t *inst) {
    const char *p = param;

    if (p == NULL) {
        return;
    }

    while (*p != '\0') {
        if (strncmp(p, "range:", 6u) == 0) {
            inst->range = (uint16_t)strtoul(p + 6, NULL, 10);
        } else if (strncmp(p, "scale:", 6u) == 0) {
            inst->scale = (uint16_t)strtoul(p + 6, NULL, 10);
        } else {
            wlog("id:%u unknown chipParameter token: %.12s", inst->config->id, p);
        }

        while ((*p != '\0') && (*p != ';')) {
            p++;
        }
        if (*p == ';') {
            p++;
        }
    }
}

static int32_t convertValue(const tmplSensorInstance_t *inst, int32_t raw) {
    int32_t value = raw;

    if ((inst->config->flags & TMPL_SENSOR_FLAG_INVERT) != 0u) {
        value = -value;
    }

    if (inst->scale > 0u) {
        value /= (int32_t)inst->scale;
    } else if ((inst->config->flags & TMPL_SENSOR_FLAG_SCALE_0_1) != 0u) {
        value /= 10;
    }
    return value;
}

static fw_err_t chipAProbe(tmplSensorInstance_t *inst) {
    uint8_t status = 0xffu;
    fw_err_t ret =
        i2cReadRegs(inst->config->bus, inst->config->address, TMPL_REG_STATUS, &status, 1u);

    if (ret != FW_OK) {
        elog("id:%u chipA probe failed, ret:%ld", inst->config->id, (long)ret);
        return FW_ERR_NO_OBJECT;
    }
    return FW_OK;
}

static fw_err_t chipARead(tmplSensorInstance_t *inst, tmplSensorData_t *out) {
    uint8_t buf[3] = {0};
    fw_err_t ret =
        i2cReadRegs(inst->config->bus, inst->config->address, TMPL_REG_STATUS, buf, sizeof(buf));

    if (ret != FW_OK) {
        return FW_ERR_TIMEOUT;
    }

    out->status = buf[0];
    out->value = convertValue(inst, (int32_t)(((uint16_t)buf[1] << 8u) | buf[2]));
    return FW_OK;
}

static fw_err_t chipBProbe(tmplSensorInstance_t *inst) {
    uint8_t cfg = 0u;
    fw_err_t ret =
        i2cWriteRegs(inst->config->bus, inst->config->address, TMPL_REG_CONFIG, &cfg, 1u);

    if (ret != FW_OK) {
        elog("id:%u chipB probe failed, ret:%ld", inst->config->id, (long)ret);
        return FW_ERR_NO_OBJECT;
    }
    return FW_OK;
}

static fw_err_t chipBRead(tmplSensorInstance_t *inst, tmplSensorData_t *out) {
    uint8_t buf[4] = {0};
    fw_err_t ret =
        i2cReadRegs(inst->config->bus, inst->config->address, TMPL_REG_VALUE_H, buf, sizeof(buf));

    if (ret != FW_OK) {
        return FW_ERR_TIMEOUT;
    }

    out->value = convertValue(inst, (int32_t)(((uint16_t)buf[0] << 8u) | buf[1]));
    out->temperature = (int16_t)(((uint16_t)buf[2] << 8u) | buf[3]);
    out->status = 0u;
    return FW_OK;
}

static bool chipSupportsMode(uint8_t chip, uint8_t mode) {
    switch (chip) {
        case TMPL_SENSOR_CHIP_A:
            return mode == TMPL_SENSOR_MODE_SINGLE;
        case TMPL_SENSOR_CHIP_B:
            return mode < TMPL_SENSOR_MODE_END;
        default:
            return false;
    }
}

fw_err_t tmplSensorInit(const tmplSensorConfig_t *configs, uint8_t num, uint8_t *initializedCount) {
    uint8_t initialized = 0u;

    if (initializedCount == NULL) {
        return FW_ERR_INVALID_ARGS;
    }
    *initializedCount = 0u;

    if ((configs == NULL) || (num == 0u) || (num > INSTANCE_SIZE)) {
        return FW_ERR_INVALID_ARGS;
    }

    (void)memset(&s_ctl, 0, sizeof(s_ctl));

    for (uint8_t i = 0u; i < num; i++) {
        const tmplSensorConfig_t *cfg = &configs[i];
        tmplSensorInstance_t *inst = &s_ctl.instances[i];
        fw_err_t ret = FW_ERR_NOT_SUPPORTED;

        if ((cfg->chip >= TMPL_SENSOR_CHIP_NUM) || (instanceFromId(cfg->id) != NULL)) {
            elog("id:%u invalid chip or duplicate id", cfg->id);
            continue;
        }

        inst->config = cfg;
        inst->mode = TMPL_SENSOR_MODE_SINGLE;
        parseChipParameter(cfg->chipParameter, inst);

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

        if (ret != FW_OK) {
            elog("id:%u init failed, chip:%u ret:%ld", cfg->id, cfg->chip, (long)ret);
            inst->config = NULL;
            continue;
        }

        initialized++;
        s_ctl.count++;
    }

    *initializedCount = initialized;
    s_ctl.initialized = initialized > 0u;
    return s_ctl.initialized ? FW_OK : FW_ERR_NO_OBJECT;
}

fw_err_t tmplSensorRead(uint8_t id, tmplSensorData_t *out) {
    tmplSensorInstance_t *inst;
    tmplSensorData_t sample;
    uint32_t tickMs = 0u;
    fw_err_t ret;

    if (out == NULL) {
        return FW_ERR_INVALID_ARGS;
    }
    if (!s_ctl.initialized) {
        return FW_ERR_INVALID_ARGS;
    }

    inst = instanceFromId(id);
    if (inst == NULL) {
        return FW_ERR_NO_OBJECT;
    }

    ret = osTickMs(&tickMs);
    if (ret != FW_OK) {
        return ret;
    }

    (void)memset(&sample, 0, sizeof(sample));
    sample.sequence = (uint8_t)(inst->sequence + 1u);

    switch (inst->config->chip) {
        case TMPL_SENSOR_CHIP_A:
            ret = chipARead(inst, &sample);
            break;
        case TMPL_SENSOR_CHIP_B:
            ret = chipBRead(inst, &sample);
            break;
        default:
            return FW_ERR_NOT_SUPPORTED;
    }

    if (ret != FW_OK) {
        elog("id:%u read failed, ret:%ld", id, (long)ret);
        return ret;
    }

    inst->lastTick = tickMs;
    inst->sequence = sample.sequence;
    *out = sample;
    return FW_OK;
}

fw_err_t tmplSensorSetMode(uint8_t id, uint8_t mode) {
    tmplSensorInstance_t *inst = instanceFromId(id);

    if (inst == NULL) {
        return FW_ERR_NO_OBJECT;
    }
    if (!chipSupportsMode(inst->config->chip, mode)) {
        return FW_ERR_NOT_SUPPORTED;
    }

    inst->mode = mode;
    return FW_OK;
}

fw_err_t tmplSensorGetConfig(uint8_t id, const tmplSensorConfig_t **config) {
    tmplSensorInstance_t *inst;

    if (config == NULL) {
        return FW_ERR_INVALID_ARGS;
    }
    *config = NULL;

    inst = instanceFromId(id);
    if (inst == NULL) {
        return FW_ERR_NO_OBJECT;
    }

    *config = inst->config;
    return FW_OK;
}

fw_err_t tmplSensorGetCount(uint8_t *count) {
    if (count == NULL) {
        return FW_ERR_INVALID_ARGS;
    }
    *count = s_ctl.count;
    return FW_OK;
}

fw_err_t tmplSensorGetIdByIndex(uint8_t index, uint8_t *id) {
    uint8_t seen = 0u;

    if (id == NULL) {
        return FW_ERR_INVALID_ARGS;
    }

    for (uint8_t i = 0u; i < INSTANCE_SIZE; i++) {
        if (s_ctl.instances[i].config == NULL) {
            continue;
        }
        if (seen == index) {
            *id = s_ctl.instances[i].config->id;
            return FW_OK;
        }
        seen++;
    }
    return FW_ERR_NO_OBJECT;
}

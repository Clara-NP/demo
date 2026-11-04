/**
 * @file board.c
 * @author <author>
 * @brief 板级实例化（模块开发流程步骤 ⑥）：描述符表 + 编译期容量校验 + 初始化调用
 * @version 0.1
 * @date 2026-11-02
 *
 * @note
 *  - **引脚 / 器件地址 / 参数只允许出现在本文件**，驱动与协议层看不到
 *  - 换板只改本文件的描述符表；换芯片只改 `port/<soc>/`
 *
 * @par 执行上下文
 *  启动阶段单线程调用；不涉及 ISR
 *
 * @par 可重入性
 *  否。`tmplSensorInit()` 会重建控制器状态，禁止并发调用 `boardInit()`
 *
 * @par 资源所有权
 *  拥有 `s_tmplSensors[]`（`static const`，ROM，生命周期覆盖运行期）；
 *  表指针交给驱动后仍归本文件所有，禁止释放/改写
 */

#include <stdint.h>

/* 顺序有意义：产品配置必须先于驱动头，否则 CONFIG_* 默认值会先生效 */
#include <config/export_ids.h>
#include <config/framework_config.h>

#include <common/generic.h>
#include <i2c/i2c.h>
#include <tmpl_sensor.h>

#define TRACE_ENABLE
#define TRACE_LEVEL T_INFO
#define TRACE_TAG   "board"
#include <common/trace.h>

#include "board.h"

/* ==========================================================================
 * 板级描述符表（本工程唯一硬件事实来源）
 * ========================================================================== */

static const tmplSensorConfig_t s_tmplSensors[] = {
    {
        .id = SENSOR_PRESSURE,
        .chip = TMPL_SENSOR_CHIP_A,
        .bus = I2C_BUS_0,
        .address = 0x48,
        .flags = 0,
        .chipParameter = "range:500",
    },
    {
        .id = SENSOR_TEMPERATURE,
        .chip = TMPL_SENSOR_CHIP_B,
        .bus = I2C_BUS_0,
        .address = 0x49,
        .flags = TMPL_SENSOR_FLAG_SCALE_0_1,
        .chipParameter = NULL,
    },
};

#define TMPL_SENSOR_NUM (sizeof(s_tmplSensors) / sizeof(s_tmplSensors[0]))

/**
 * @brief 编译期容量校验：描述符条数不得超过 CONFIG_TMPL_SENSOR_MAX_NUM
 *
 * @note 用 typedef 而不是 `#if`：`#if` 在预处理阶段拿不到 `sizeof`，
 *       写成 `#if (sizeof(x)/sizeof(x[0])) > N` 无法通过编译（模板中的待修正项）
 */
typedef char tmplSensorCapacityCheck_t[(TMPL_SENSOR_NUM <= CONFIG_TMPL_SENSOR_MAX_NUM) ? 1 : -1];

/* ==========================================================================
 * 初始化
 * ========================================================================== */

fw_err_t boardTmplSensorInit(uint8_t *initializedSensors) {
    fw_err_t ret;

    if (initializedSensors == NULL) {
        return FW_ERR_INVALID_ARGS;
    }

    ret = tmplSensorInit(s_tmplSensors, (uint8_t)TMPL_SENSOR_NUM, initializedSensors);
    if (ret != FW_OK) {
        elog("board: no tmpl sensor initialized, ret:%ld", (long)ret);
        return ret;
    }

    if (*initializedSensors < (uint8_t)TMPL_SENSOR_NUM) {
        wlog("board: only %u/%u tmpl sensors initialized", *initializedSensors,
             (uint8_t)TMPL_SENSOR_NUM);
    }
    return FW_OK;
}

fw_err_t boardInit(uint8_t *initializedModules) {
    uint8_t initializedSensors = 0u;
    fw_err_t ret;

    if (initializedModules == NULL) {
        return FW_ERR_INVALID_ARGS;
    }
    *initializedModules = 0u;

    ret = boardTmplSensorInit(&initializedSensors);
    if (ret != FW_OK) {
        return ret;
    }

    (*initializedModules)++;
    ilog("board init done, modules:%u", *initializedModules);
    return FW_OK;
}

/* ==========================================================================
 * 同步清单（新增/调整资源时逐条确认，漏一条即"编译成功但行为错"）
 * --------------------------------------------------------------------------
 * [x] src/config/export_ids.h       逻辑 ID + 哨兵
 * [x] src/config/framework_config.h CONFIG_<NAME>_MAX_NUM 派生
 * [x] src/system/board.c            描述符表 + 编译期校验 + Init 调用
 * [x] src/main.c                    调用 boardInit()
 * [ ] 构建清单                      CMakeLists.txt（待建）
 * [ ] 协议接入                      src/sacp/sacp_tmpl_sensor.c + local_attributes.h
 * ========================================================================== */

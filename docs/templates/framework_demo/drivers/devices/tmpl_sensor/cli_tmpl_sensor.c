/**
 * @file cli_tmpl_sensor.c
 * @author <author>
 * @brief 模板模块的自检命令行（对应流程步骤 ⑦）
 * @version 0.1
 * @date 2026-10-29
 *
 * @note
 *  - 目的：上板即可验证硬件与配置，不依赖上层业务逻辑
 *  - 约定：list / read / mode / dump 四个子命令起步
 *  - 本文件允许依赖 console（打印），但**驱动核心不允许**
 */

#include <stdlib.h>
#include <string.h>

#include <common/generic.h>
#include <console/command.h>

#include "tmpl_sensor.h"

#ifndef CONFIG_CLI_TMPL_SENSOR
#define CONFIG_CLI_TMPL_SENSOR 1
#endif

#if CONFIG_CLI_TMPL_SENSOR

static const char *modeName(uint8_t mode) {
    switch (mode) {
        case TMPL_SENSOR_MODE_SINGLE:
            return "single";
        case TMPL_SENSOR_MODE_AUTO_100MS:
            return "auto100ms";
        case TMPL_SENSOR_MODE_AUTO_1000MS:
            return "auto1000ms";
        default:
            return "unknown";
    }
}

static void printOne(uint8_t id) {
    const tmplSensorConfig_t *cfg = NULL;
    tmplSensorData_t data = {0};

    if (tmplSensorGetConfig(id, &cfg) != RET_SUCCESS) {
        rawPrint("id:%d not registered\r\n", id);
        return;
    }

    rawPrint("id:%-3d chip:%d bus:%d addr:0x%02x param:%s\r\n", id, cfg->chip, cfg->bus,
             cfg->address, (cfg->chipParameter != NULL) ? cfg->chipParameter : "-");

    int ret = tmplSensorRead(id, &data);
    if (ret == RET_SUCCESS) {
        rawPrint("  read: value=%d temp=%d status=0x%02x seq=%d\r\n", (int)data.value,
                 (int)data.temperature, data.status, data.sequence);
    } else {
        rawPrint("  read failed, ret=%d\r\n", ret);
    }
}

/**
 * @brief 传感器测试命令
 *
 * @code
 *   tmplsensor list
 *   tmplsensor read <id>
 *   tmplsensor mode <id> <0|1|2>
 *   tmplsensor dump
 * @endcode
 */
CMD_DEFINE(tmplsensor, "tmpl sensor test",
           "tmplsensor list\r\n"
           "tmplsensor read <id>\r\n"
           "tmplsensor mode <id> <0|1|2>\r\n"
           "tmplsensor dump\r\n") {
    if (argc < 1) {
        return 2;
    }

    const char *sub = argv[0];

    if (strcmp(sub, "list") == 0) {
        int count = tmplSensorGetCount();

        rawPrint("registered: %d\r\n", count);
        for (int i = 0; i < count; i++) {
            uint8_t id = 0;

            if (tmplSensorGetIdByIndex(i, &id) == RET_SUCCESS) {
                printOne(id);
            }
        }
        return 0;
    }

    if (strcmp(sub, "read") == 0) {
        if (argc < 2) {
            return 2;
        }

        uint8_t id = (uint8_t)strtoul(argv[1], NULL, 0);
        tmplSensorData_t data = {0};
        int ret = tmplSensorRead(id, &data);

        if (ret != RET_SUCCESS) {
            rawPrint("read id:%d failed, ret=%d\r\n", id, ret);
            return 3;
        }

        rawPrint("id:%d value=%d temp=%d status=0x%02x seq=%d\r\n", id, (int)data.value,
                 (int)data.temperature, data.status, data.sequence);
        return 0;
    }

    if (strcmp(sub, "mode") == 0) {
        if (argc < 3) {
            return 2;
        }

        uint8_t id = (uint8_t)strtoul(argv[1], NULL, 0);
        uint8_t mode = (uint8_t)strtoul(argv[2], NULL, 0);
        int ret = tmplSensorSetMode(id, mode);

        rawPrint("set id:%d mode:%s ret=%d\r\n", id, modeName(mode), ret);

        return (ret == RET_SUCCESS) ? 0 : 3;
    }

    if (strcmp(sub, "dump") == 0) {
        int count = tmplSensorGetCount();

        rawPrint("tmplsensor: count=%d\r\n", count);
        for (int i = 0; i < count; i++) {
            uint8_t id = 0;
            const tmplSensorConfig_t *cfg = NULL;

            if ((tmplSensorGetIdByIndex(i, &id) == RET_SUCCESS) &&
                (tmplSensorGetConfig(id, &cfg) == RET_SUCCESS)) {
                rawPrint("  [%d] id:%-3d chip:%d bus:%d addr:0x%02x flags:0x%04x param:%s\r\n", i,
                         id, cfg->chip, cfg->bus, cfg->address, cfg->flags,
                         (cfg->chipParameter != NULL) ? cfg->chipParameter : "-");
            }
        }
        return 0;
    }

    return 3;
}

#endif // CONFIG_CLI_TMPL_SENSOR

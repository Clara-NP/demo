/**
 * @file cli_tmpl_sensor.c
 * @brief 传感器示例的板上自检命令
 *
 * @par 执行上下文
 * 控制台任务
 *
 * @par 可重入性
 * 否
 *
 * @par 资源所有权
 * 不持有资源
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
    fw_err_t ret;

    if (tmplSensorGetConfig(id, &cfg) != FW_OK) {
        rawPrint("id:%u not registered\r\n", id);
        return;
    }

    rawPrint("id:%-3u chip:%u bus:%u addr:0x%02x param:%s\r\n", id, cfg->chip, cfg->bus,
             cfg->address, (cfg->chipParameter != NULL) ? cfg->chipParameter : "-");

    ret = tmplSensorRead(id, &data);
    if (ret == FW_OK) {
        rawPrint("  read: value=%ld temp=%d status=0x%02x seq=%u\r\n", (long)data.value,
                 (int)data.temperature, data.status, data.sequence);
    } else {
        rawPrint("  read failed, ret=%ld\r\n", (long)ret);
    }
}

CMD_DEFINE(tmplsensor, "tmpl sensor test",
           "tmplsensor list\r\n"
           "tmplsensor read <id>\r\n"
           "tmplsensor mode <id> <0|1|2>\r\n"
           "tmplsensor dump\r\n") {
    const char *sub;

    if (argc < 1) {
        return 2;
    }
    sub = argv[0];

    if (strcmp(sub, "list") == 0) {
        uint8_t count = 0u;

        if (tmplSensorGetCount(&count) != FW_OK) {
            return 3;
        }
        rawPrint("registered: %u\r\n", count);
        for (uint8_t i = 0u; i < count; i++) {
            uint8_t id = 0u;
            if (tmplSensorGetIdByIndex(i, &id) == FW_OK) {
                printOne(id);
            }
        }
        return 0;
    }

    if (strcmp(sub, "read") == 0) {
        uint8_t id;
        tmplSensorData_t data = {0};
        fw_err_t ret;

        if (argc < 2) {
            return 2;
        }
        id = (uint8_t)strtoul(argv[1], NULL, 0);
        ret = tmplSensorRead(id, &data);
        if (ret != FW_OK) {
            rawPrint("read id:%u failed, ret=%ld\r\n", id, (long)ret);
            return 3;
        }

        rawPrint("id:%u value=%ld temp=%d status=0x%02x seq=%u\r\n", id, (long)data.value,
                 (int)data.temperature, data.status, data.sequence);
        return 0;
    }

    if (strcmp(sub, "mode") == 0) {
        uint8_t id;
        uint8_t mode;
        fw_err_t ret;

        if (argc < 3) {
            return 2;
        }
        id = (uint8_t)strtoul(argv[1], NULL, 0);
        mode = (uint8_t)strtoul(argv[2], NULL, 0);
        ret = tmplSensorSetMode(id, mode);
        rawPrint("set id:%u mode:%s ret=%ld\r\n", id, modeName(mode), (long)ret);
        return (ret == FW_OK) ? 0 : 3;
    }

    if (strcmp(sub, "dump") == 0) {
        uint8_t count = 0u;

        if (tmplSensorGetCount(&count) != FW_OK) {
            return 3;
        }
        rawPrint("tmplsensor: count=%u\r\n", count);
        for (uint8_t i = 0u; i < count; i++) {
            uint8_t id = 0u;
            const tmplSensorConfig_t *cfg = NULL;

            if ((tmplSensorGetIdByIndex(i, &id) == FW_OK) &&
                (tmplSensorGetConfig(id, &cfg) == FW_OK)) {
                rawPrint("  [%u] id:%-3u chip:%u bus:%u addr:0x%02x flags:0x%04x param:%s\r\n", i,
                         id, cfg->chip, cfg->bus, cfg->address, cfg->flags,
                         (cfg->chipParameter != NULL) ? cfg->chipParameter : "-");
            }
        }
        return 0;
    }

    return 3;
}

#endif // CONFIG_CLI_TMPL_SENSOR

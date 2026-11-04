/**
 * @file main.c
 * @author <author>
 * @brief 工程入口：启动序列（board → driver → 协议 → 任务）
 * @version 0.1
 * @date 2026-11-02
 *
 * @par 执行上下文
 *  主上下文（启动线程 → 主循环）；不进入 ISR
 *
 * @par 可重入性
 *  否，仅启动调用一次
 *
 * @par 资源所有权
 *  不持有资源，只按顺序调用各模块初始化并创建任务
 *
 * @note 本文件参与根目录 `smoke_app` 的冒烟编译（不运行：主循环未替换为 osal 任务）
 */

#include <common/generic.h>

#include <config/framework_config.h>
#include <sacp/sacp_tmpl_sensor.h>

#include "system/board.h"

/**
 * @brief 启动序列
 *
 * @return fw_err_t
 *
 * @note 顺序不可调换：
 *       1) boardInit()          —— 绑定描述符、探测器件
 *       2) sacpTmplSensorInit() —— 注册属性组（依赖驱动已初始化）
 *       3) 任务创建              —— 周期采样与上报
 */
static fw_err_t appStartup(void) {
    uint8_t initializedModules = 0u;
    fw_err_t ret = boardInit(&initializedModules);

    if (ret != FW_OK) {
        return ret;
    }

#if CONFIG_SACP_TMPL_SENSOR
    ret = sacpTmplSensorInit();
    if (ret != FW_OK) {
        return ret;
    }
#endif

    return FW_OK;
}

int main(void) {
    fw_err_t ret = appStartup();

    if (ret != FW_OK) {
        return 1;
    }

    return 0;
}

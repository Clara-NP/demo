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
 * @return int 0 成功，非 0 失败
 *
 * @note 顺序不可调换：
 *       1) boardInit()          —— 绑定描述符、探测器件
 *       2) sacpTmplSensorInit() —— 注册属性组（依赖驱动已初始化）
 *       3) 任务创建              —— 周期采样与上报
 */
static int appStartup(void)
{
    int ret = boardInit();

    if (ret < 0)
    {
        return ret;
    }

#if CONFIG_SACP_TMPL_SENSOR
    sacpTmplSensorInit();
#endif

    return RET_SUCCESS;
}

int main(void)
{
    int ret = appStartup();

    if (ret != RET_SUCCESS)
    {
        return 1;
    }

    /* TODO: 用 osal 任务替换主循环
     *   - 周期任务：sacpTmplSensorUpdate(SENSOR_PRESSURE) → sacpTmplSensorReport(...)
     *   - 禁止在任务内忙等；所有等待必须带超时
     */
    for (;;)
    {
        /* 占位：不得在此写业务逻辑 */
    }
}

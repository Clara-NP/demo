/**
 * @file soc_tmpl_sensor_stub.c
 * @author <author>
 * @brief 平台适配层写法示例（对应流程步骤 ⑤）——**本模块通常不需要本文件**
 * @version 0.1
 * @date 2026-10-29
 *
 * @note
 *  ！何时需要本文件
 *  - 设备驱动（通过抽象总线访问器件）—— **不需要**，直接复用 i2c/spi 驱动
 *  - 总线/资源驱动（直接操作寄存器或 SDK）—— **需要**，按本文件形态在 port/<soc>/ 下实现
 *
 *  ！本文件是所有"芯片差异"的唯一合法落点
 *  - 允许：#include <vendor_hal.h>
 *  - 禁止：被 driver/、component/、src/ 直接包含
 */

#include <common/generic.h>

/*
 * 平台私有头：由各平台自己提供（如 <soc/xxx_private.h>），只在本文件可见。
 * 使用时取消下一行注释，并在 port/<soc>/ 下创建对应头文件。
 */
// #include "soc_tmpl_sensor_private.h"

#define TRACE_ENABLE
#define TRACE_TAG       "soc-tmplsensor"
#define TRACE_LEVEL     T_INFO
#include <common/trace.h>

/* ==========================================================================
 * 对外抽象接口（每个 soc 平台各实现一份，签名完全一致）
 * --------------------------------------------------------------------------
 *  头文件放在 common/soc/soc_tmpl_sensor.h，只有声明，不含芯片符号：
 *
 *      int      socTmplSensorSetMode(uint8_t port, uint8_t mode);
 *      int      socTmplSensorReadRaw(uint8_t port, uint32_t *raw);
 *      uint32_t socTmplSensorGetClockHz(uint8_t port);
 * ========================================================================== */

/**
 * @brief 配置硬件外设
 *
 * @param port 抽象端口号（与板级配置中的 bus/port 对应）
 * @param mode 模式
 * @return int 0 成功，其它失败
 */
int socTmplSensorSetMode(uint8_t port, uint8_t mode)
{
#if defined(SOC_FAMILY_A)
    /* 平台 A 实现：通过 SDK 配置 */
    // vendor_periph_config(port, mode);
    // return 0;
#elif defined(SOC_FAMILY_B)
    /* 平台 B 实现：通过寄存器配置 */
    // REG_WRITE(BASE(port) + MODE_OFS, mode);
    // return 0;
#else
#error "soc_tmpl_sensor: unsupported SoC, implement here"
#endif

    (void)port;
    (void)mode;

    return -1;
}

/**
 * @brief 读取原始值
 *
 * @param port 抽象端口号
 * @param raw  输出原始值
 * @return int 0 成功，其它失败
 */
int socTmplSensorReadRaw(uint8_t port, uint32_t *raw)
{
    if (raw == NULL)
    {
        return -1;
    }

    /* 平台相关读取；必须带超时，禁止无界等待 */
    *raw = 0;

    elog("soc_tmpl_sensor: ReadRaw not implemented, port:%d", port);

    return -1;
}

/**
 * @brief 获取该端口的时钟频率（供上层做换算，避免在驱动里写死）
 */
uint32_t socTmplSensorGetClockHz(uint8_t port)
{
    (void)port;

    return 0;
}

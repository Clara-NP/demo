/**
 * @file soc_tmpl_sensor_stub.c
 * @brief 直接访问 SDK 或寄存器的 port 层写法示例
 *
 * @par 执行上下文
 * 任务上下文，访问必须有超时
 *
 * @par 可重入性
 * 由具体平台实现定义
 *
 * @par 资源所有权
 * 由具体平台实现持有硬件资源
 *
 * @note 设备驱动通过抽象总线访问时不需要本文件；它只示范资源驱动的合法落点。
 */

#include <common/generic.h>

fw_err_t socTmplSensorSetMode(uint8_t port, uint8_t mode) {
    (void)port;
    (void)mode;
    return FW_ERR_NOT_SUPPORTED;
}

fw_err_t socTmplSensorReadRaw(uint8_t port, uint32_t *raw) {
    (void)port;

    if (raw == NULL) {
        return FW_ERR_INVALID_ARGS;
    }
    *raw = 0u;
    return FW_ERR_NOT_SUPPORTED;
}

fw_err_t socTmplSensorGetClockHz(uint8_t port, uint32_t *clockHz) {
    (void)port;

    if (clockHz == NULL) {
        return FW_ERR_INVALID_ARGS;
    }
    *clockHz = 0u;
    return FW_ERR_NOT_SUPPORTED;
}

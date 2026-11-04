#ifndef __PORT_HOST_I2C_HOST_H__
#define __PORT_HOST_I2C_HOST_H__

/**
 * @file i2c_host.h
 * @brief host 模拟 I2C 的测试辅助接口
 *
 * @par 执行上下文
 * host 测试线程
 *
 * @par 可重入性
 * 否
 *
 * @par 资源所有权
 * host 后端持有模拟寄存器
 */

#include <common/generic.h>

fw_err_t i2cHostAttach(uint8_t bus, uint16_t addr);
fw_err_t i2cHostSetRegs(uint8_t bus, uint16_t addr, uint8_t reg, const uint8_t *buf, uint16_t len);
fw_err_t i2cHostSetError(uint8_t bus, fw_err_t error);

#endif // __PORT_HOST_I2C_HOST_H__

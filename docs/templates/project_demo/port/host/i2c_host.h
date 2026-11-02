#ifndef __PORT_HOST_I2C_HOST_H__
#define __PORT_HOST_I2C_HOST_H__

/**
 * @file i2c_host.h
 * @author <author>
 * @brief host 后端（模拟 I2C 总线）的测试辅助接口
 * @version 0.1
 * @date 2026-11-02
 *
 * @note 仅供 host 构建与单测使用；目标板由 `port/<soc>/i2c_*.c` 实现同一套接口
 */

#include <common/generic.h>

/**
 * @brief 在指定总线/地址上挂一个模拟器件（寄存器初值为 0）
 *
 * @return int RET_SUCCESS / RET_INVALID_ARGS
 */
int i2cHostAttach(uint8_t bus, uint16_t addr);

/**
 * @brief 预置模拟器件的寄存器内容
 *
 * @return int RET_SUCCESS / RET_NO_OBJECT（器件未挂载）
 */
int i2cHostSetRegs(uint8_t bus, uint16_t addr, uint8_t reg, const uint8_t *buf, uint16_t len);

/**
 * @brief 注入总线错误（非 0 时后续读写均失败）
 */
int i2cHostSetError(uint8_t bus, int err);

#endif // __PORT_HOST_I2C_HOST_H__

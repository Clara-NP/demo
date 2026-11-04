#ifndef __I2C_I2C_H__
#define __I2C_I2C_H__

/**
 * @file i2c.h
 * @brief 抽象 I2C 总线接口
 *
 * @par 执行上下文
 * 任务上下文；实现内部必须带超时
 *
 * @par 可重入性
 * 同一 bus 不保证可重入，调用方需串行化
 *
 * @par 资源所有权
 * 总线实现持有硬件资源
 */

#include <common/generic.h>

/**
 * @brief 连续写寄存器
 * @param[in] bus 逻辑总线号
 * @param[in] addr 7bit 器件地址
 * @param[in] reg 起始寄存器
 * @param[in] buf 数据
 * @param[in] len 长度
 * @return fw_err_t
 * @retval FW_OK 成功
 * @retval FW_ERR_INVALID_ARGS 参数非法
 * @retval FW_ERR_NO_OBJECT 总线或器件未注册
 * @retval FW_ERR_TIMEOUT 总线无响应
 */
fw_err_t i2cWriteRegs(uint8_t bus, uint16_t addr, uint8_t reg, const uint8_t *buf, uint16_t len);

/**
 * @brief 连续读寄存器
 * @param[in] bus 逻辑总线号
 * @param[in] addr 7bit 器件地址
 * @param[in] reg 起始寄存器
 * @param[out] buf 输出缓冲；失败时内容无效
 * @param[in] len 长度
 * @return fw_err_t，错误含义同 i2cWriteRegs
 */
fw_err_t i2cReadRegs(uint8_t bus, uint16_t addr, uint8_t reg, uint8_t *buf, uint16_t len);

/**
 * @brief 探测器件
 * @param[in] bus 逻辑总线号
 * @param[in] addr 7bit 器件地址
 * @return fw_err_t
 * @retval FW_OK 存在
 * @retval FW_ERR_NO_OBJECT 无应答
 */
fw_err_t i2cProbe(uint8_t bus, uint16_t addr);

#endif // __I2C_I2C_H__

#ifndef __I2C_I2C_H__
#define __I2C_I2C_H__

/**
 * @file i2c.h
 * @author <author>
 * @brief 抽象 I2C 总线接口：只有逻辑总线号，不含任何芯片符号
 * @version 0.1
 * @date 2026-11-02
 *
 * @par 执行上下文
 *  任务上下文；实现内部必须带超时，超时返回错误码
 *
 * @par 可重入性
 *  同一 bus 不保证可重入；调用方需按 bus 串行化
 *
 * @note 本文件是 framework_demo 骨架的一部分：复制到真实仓库时展开为
 *       `libs/drivers/bus/i2c/i2c.h`，实现位于 `port/<soc>/`
 */

#include <common/generic.h>

/**
 * @brief 连续写寄存器
 *
 * @param[in] bus  逻辑总线号（见 export_ids.h 的 EXPORT_I2C）
 * @param[in] addr 7bit 器件地址
 * @param[in] reg  起始寄存器
 * @param[in] buf  数据
 * @param[in] len  长度（>0）
 * @return int
 *  - RET_SUCCESS   成功
 *  - RET_INVALID_ARGS 参数非法
 *  - RET_NO_OBJECT bus 未注册
 *  - RET_TIMEOUT   总线无响应
 */
int i2cWriteRegs(uint8_t bus, uint16_t addr, uint8_t reg, const uint8_t *buf, uint16_t len);

/**
 * @brief 连续读寄存器
 *
 * @param[in]  bus  逻辑总线号
 * @param[in]  addr 7bit 器件地址
 * @param[in]  reg  起始寄存器
 * @param[out] buf  输出缓冲
 * @param[in]  len  长度（>0）
 * @return int 同 i2cWriteRegs
 *
 * @note 失败时**不得**保留上次数据；调用方据错误码判定，不读 buf
 */
int i2cReadRegs(uint8_t bus, uint16_t addr, uint8_t reg, uint8_t *buf, uint16_t len);

/**
 * @brief 探测器件是否存在
 *
 * @param[in] bus  逻辑总线号
 * @param[in] addr 7bit 器件地址
 * @return int RET_SUCCESS 存在；RET_NO_OBJECT 无应答
 */
int i2cProbe(uint8_t bus, uint16_t addr);

#endif // __I2C_I2C_H__

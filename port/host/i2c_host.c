/**
 * @file i2c_host.c
 * @author <author>
 * @brief host 后端：内存模拟的 I2C 总线（无硬件依赖，供 host 构建与单测）
 * @version 0.1
 * @date 2026-11-02
 *
 * @par 执行上下文
 *  任务上下文；无阻塞、无真实时序
 *
 * @par 可重入性
 *  否（全局模拟状态），单线程使用
 *
 * @par 资源所有权
 *  拥有 s_dev / s_err（静态内存），不动态分配
 */

#include <string.h>

#include <common/generic.h>
#include <i2c/i2c.h>

#include "i2c_host.h"

/// 支持的总线数（与 export_ids.h 的 EXPORT_I2C 对齐）
#define I2C_HOST_BUS_MAX            4
/// 7bit 地址空间
#define I2C_HOST_ADDR_MAX           128
/// 每个器件的寄存器数
#define I2C_HOST_REG_MAX            256

typedef struct
{
    uint8_t present;
    uint8_t regs[I2C_HOST_REG_MAX];
} i2cHostDev_t;

static i2cHostDev_t s_dev[I2C_HOST_BUS_MAX][I2C_HOST_ADDR_MAX];
static int s_err[I2C_HOST_BUS_MAX];

static int busValid(uint8_t bus)
{
    return (bus < I2C_HOST_BUS_MAX) ? 1 : 0;
}

static i2cHostDev_t *devFrom(uint8_t bus, uint16_t addr)
{
    if ((busValid(bus) == 0) || (addr >= I2C_HOST_ADDR_MAX))
    {
        return NULL;
    }

    if (s_dev[bus][addr].present == 0u)
    {
        return NULL;
    }

    return &s_dev[bus][addr];
}

int i2cHostAttach(uint8_t bus, uint16_t addr)
{
    if ((busValid(bus) == 0) || (addr >= I2C_HOST_ADDR_MAX))
    {
        return RET_INVALID_ARGS;
    }

    (void)memset(&s_dev[bus][addr], 0, sizeof(s_dev[bus][addr]));
    s_dev[bus][addr].present = 1u;

    return RET_SUCCESS;
}

int i2cHostSetRegs(uint8_t bus, uint16_t addr, uint8_t reg, const uint8_t *buf, uint16_t len)
{
    i2cHostDev_t *dev = devFrom(bus, addr);

    if ((dev == NULL) || (buf == NULL) || (len == 0u))
    {
        return RET_NO_OBJECT;
    }

    if (((uint32_t)reg + (uint32_t)len) > (uint32_t)I2C_HOST_REG_MAX)
    {
        return RET_INVALID_ARGS;
    }

    (void)memcpy(&dev->regs[reg], buf, len);

    return RET_SUCCESS;
}

int i2cHostSetError(uint8_t bus, int err)
{
    if (busValid(bus) == 0)
    {
        return RET_INVALID_ARGS;
    }

    s_err[bus] = err;

    return RET_SUCCESS;
}

int i2cWriteRegs(uint8_t bus, uint16_t addr, uint8_t reg, const uint8_t *buf, uint16_t len)
{
    i2cHostDev_t *dev = devFrom(bus, addr);

    if ((dev == NULL) || (buf == NULL) || (len == 0u))
    {
        return RET_INVALID_ARGS;
    }

    if (s_err[bus] != 0)
    {
        return RET_NO_OBJECT;               // 模拟器件无应答
    }

    if (((uint32_t)reg + (uint32_t)len) > (uint32_t)I2C_HOST_REG_MAX)
    {
        return RET_INVALID_ARGS;
    }

    (void)memcpy(&dev->regs[reg], buf, len);

    return RET_SUCCESS;
}

int i2cReadRegs(uint8_t bus, uint16_t addr, uint8_t reg, uint8_t *buf, uint16_t len)
{
    i2cHostDev_t *dev = devFrom(bus, addr);

    if ((dev == NULL) || (buf == NULL) || (len == 0u))
    {
        return RET_INVALID_ARGS;
    }

    if (s_err[bus] != 0)
    {
        return RET_TIMEOUT;                 // 模拟总线超时：缓冲区不得被改写
    }

    if (((uint32_t)reg + (uint32_t)len) > (uint32_t)I2C_HOST_REG_MAX)
    {
        return RET_INVALID_ARGS;
    }

    (void)memcpy(buf, &dev->regs[reg], len);

    return RET_SUCCESS;
}

int i2cProbe(uint8_t bus, uint16_t addr)
{
    return (devFrom(bus, addr) != NULL) ? RET_SUCCESS : RET_NO_OBJECT;
}

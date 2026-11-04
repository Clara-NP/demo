/**
 * @file i2c_host.c
 * @brief 内存模拟的 host I2C 后端
 *
 * @par 执行上下文
 * host 测试线程
 *
 * @par 可重入性
 * 否
 *
 * @par 资源所有权
 * 持有静态模拟设备、寄存器和错误注入状态
 */

#include <string.h>

#include <common/generic.h>
#include <i2c/i2c.h>

#include "i2c_host.h"

#define I2C_HOST_BUS_MAX  4
#define I2C_HOST_ADDR_MAX 128
#define I2C_HOST_REG_MAX  256

typedef struct {
    bool present;
    uint8_t regs[I2C_HOST_REG_MAX];
} i2cHostDev_t;

static i2cHostDev_t s_devices[I2C_HOST_BUS_MAX][I2C_HOST_ADDR_MAX];
static fw_err_t s_errors[I2C_HOST_BUS_MAX];

static bool busValid(uint8_t bus) {
    return bus < I2C_HOST_BUS_MAX;
}

static i2cHostDev_t *deviceFrom(uint8_t bus, uint16_t addr) {
    if (!busValid(bus) || (addr >= I2C_HOST_ADDR_MAX) || !s_devices[bus][addr].present) {
        return NULL;
    }
    return &s_devices[bus][addr];
}

fw_err_t i2cHostAttach(uint8_t bus, uint16_t addr) {
    if (!busValid(bus) || (addr >= I2C_HOST_ADDR_MAX)) {
        return FW_ERR_INVALID_ARGS;
    }

    (void)memset(&s_devices[bus][addr], 0, sizeof(s_devices[bus][addr]));
    s_devices[bus][addr].present = true;
    return FW_OK;
}

fw_err_t i2cHostSetRegs(uint8_t bus, uint16_t addr, uint8_t reg, const uint8_t *buf, uint16_t len) {
    i2cHostDev_t *device = deviceFrom(bus, addr);

    if ((device == NULL) || (buf == NULL) || (len == 0u)) {
        return FW_ERR_INVALID_ARGS;
    }
    if (((uint32_t)reg + (uint32_t)len) > I2C_HOST_REG_MAX) {
        return FW_ERR_INVALID_ARGS;
    }

    (void)memcpy(&device->regs[reg], buf, len);
    return FW_OK;
}

fw_err_t i2cHostSetError(uint8_t bus, fw_err_t error) {
    if (!busValid(bus)) {
        return FW_ERR_INVALID_ARGS;
    }
    s_errors[bus] = error;
    return FW_OK;
}

fw_err_t i2cWriteRegs(uint8_t bus, uint16_t addr, uint8_t reg, const uint8_t *buf, uint16_t len) {
    i2cHostDev_t *device = deviceFrom(bus, addr);

    if ((device == NULL) || (buf == NULL) || (len == 0u)) {
        return FW_ERR_INVALID_ARGS;
    }
    if (s_errors[bus] != FW_OK) {
        return s_errors[bus];
    }
    if (((uint32_t)reg + (uint32_t)len) > I2C_HOST_REG_MAX) {
        return FW_ERR_INVALID_ARGS;
    }

    (void)memcpy(&device->regs[reg], buf, len);
    return FW_OK;
}

fw_err_t i2cReadRegs(uint8_t bus, uint16_t addr, uint8_t reg, uint8_t *buf, uint16_t len) {
    i2cHostDev_t *device = deviceFrom(bus, addr);

    if ((device == NULL) || (buf == NULL) || (len == 0u)) {
        return FW_ERR_INVALID_ARGS;
    }
    if (s_errors[bus] != FW_OK) {
        return s_errors[bus];
    }
    if (((uint32_t)reg + (uint32_t)len) > I2C_HOST_REG_MAX) {
        return FW_ERR_INVALID_ARGS;
    }

    (void)memcpy(buf, &device->regs[reg], len);
    return FW_OK;
}

fw_err_t i2cProbe(uint8_t bus, uint16_t addr) {
    return (deviceFrom(bus, addr) != NULL) ? FW_OK : FW_ERR_NO_OBJECT;
}

#ifndef __DRV_TMPL_SENSOR_H__
#define __DRV_TMPL_SENSOR_H__

/**
 * @file tmpl_sensor.h
 * @brief 挂在抽象总线上的传感器驱动示例
 *
 * @par 执行上下文
 * 初始化 API 用于启动阶段；读写 API 用于任务上下文
 *
 * @par 可重入性
 * 否，同一实例由调用方串行化
 *
 * @par 资源所有权
 * 驱动持有静态运行期状态；配置表由调用方持有且生命周期覆盖运行期
 */

#include <common/generic.h>
#include <i2c/i2c.h>

#ifndef CONFIG_TMPL_SENSOR_MAX_NUM
#define CONFIG_TMPL_SENSOR_MAX_NUM 2
#endif

enum TMPL_SENSOR_CHIP {
    TMPL_SENSOR_CHIP_A = 0,
    TMPL_SENSOR_CHIP_B,
    TMPL_SENSOR_CHIP_END,
    TMPL_SENSOR_CHIP_NUM = TMPL_SENSOR_CHIP_END
};

enum TMPL_SENSOR_MODE {
    TMPL_SENSOR_MODE_SINGLE = 0,
    TMPL_SENSOR_MODE_AUTO_100MS,
    TMPL_SENSOR_MODE_AUTO_1000MS,
    TMPL_SENSOR_MODE_END
};

#define TMPL_SENSOR_FLAG_INVERT       0x0001
#define TMPL_SENSOR_FLAG_SCALE_0_1    0x0002
#define TMPL_SENSOR_FLAG_USER_DEFINE0 0x8000

typedef struct {
    uint8_t id;
    uint8_t chip;
    uint8_t bus;
    uint16_t address;
    uint16_t flags;
    const char *chipParameter;
} tmplSensorConfig_t;

typedef struct {
    int32_t value;
    int16_t temperature;
    uint8_t status;
    uint8_t sequence;
} tmplSensorData_t;

/**
 * @brief 初始化传感器模块
 * @param[in] configs 描述符数组
 * @param[in] num 描述符数量
 * @param[out] initializedCount 成功初始化的实例数
 * @return fw_err_t
 * @retval FW_OK 至少一个实例初始化成功
 * @retval FW_ERR_INVALID_ARGS 参数非法
 * @retval FW_ERR_NO_OBJECT 没有实例初始化成功
 */
fw_err_t tmplSensorInit(const tmplSensorConfig_t *configs, uint8_t num, uint8_t *initializedCount);

/**
 * @brief 读取一次采样
 * @param[in] id 逻辑 ID
 * @param[out] out 本次采样结果；失败时不改写
 * @return fw_err_t
 * @retval FW_OK 成功
 * @retval FW_ERR_INVALID_ARGS 参数非法或未初始化
 * @retval FW_ERR_NO_OBJECT 实例不存在
 * @retval FW_ERR_TIMEOUT 总线超时
 *
 * @note 调用上下文：任务上下文；可重入：否
 */
fw_err_t tmplSensorRead(uint8_t id, tmplSensorData_t *out);

/**
 * @brief 设置采样模式
 * @param[in] id 逻辑 ID
 * @param[in] mode 采样模式
 * @return fw_err_t
 */
fw_err_t tmplSensorSetMode(uint8_t id, uint8_t mode);

/**
 * @brief 获取只读描述符
 * @param[in] id 逻辑 ID
 * @param[out] config 描述符指针
 * @return fw_err_t
 */
fw_err_t tmplSensorGetConfig(uint8_t id, const tmplSensorConfig_t **config);

/**
 * @brief 获取已初始化实例数量
 * @param[out] count 实例数量
 * @return fw_err_t
 */
fw_err_t tmplSensorGetCount(uint8_t *count);

/**
 * @brief 按序号获取逻辑 ID
 * @param[in] index 序号
 * @param[out] id 逻辑 ID
 * @return fw_err_t
 */
fw_err_t tmplSensorGetIdByIndex(uint8_t index, uint8_t *id);

#endif // __DRV_TMPL_SENSOR_H__

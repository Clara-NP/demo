#ifndef __DRV_TMPL_SENSOR_H__
#define __DRV_TMPL_SENSOR_H__

/**
 * @file tmpl_sensor.h
 * @author <author>
 * @brief 模板模块：挂在抽象总线上的通用传感器驱动（golden example）
 * @version 0.1
 * @date 2026-10-29
 *
 * @note
 *  - 本文件是"满分模块"的公开头：只允许依赖 generic.h 与**抽象总线**接口
 *  - 禁止出现任何芯片头 / 寄存器名 / 板级引脚宏
 *  - 所有对外 API 的第一参数都是逻辑 ID，调用方永远拿不到内部实例指针
 *
 * @par 分层位置
 *  设备驱动层（framework/drivers/devices/tmpl_sensor）→ 依赖 i2c/i2c.h（抽象总线）
 *
 * @par 逻辑 ID 来源
 *  config/export_ids.h 中的 EXPORT_TMPL_SENSOR 枚举（由板级配置绑定到实际器件）
 */

#include <common/generic.h>
#include <i2c/i2c.h>

/**
 * @brief 可支持的传感器数量，可在产品配置中覆盖
 *
 * @note 产品配置应写成： #define CONFIG_TMPL_SENSOR_MAX_NUM EXPORT_TMPL_SENSOR_MAX_NUM
 */
#ifndef CONFIG_TMPL_SENSOR_MAX_NUM
#define CONFIG_TMPL_SENSOR_MAX_NUM 2
#endif

/**
 * @brief 支持的芯片/载体类型
 *
 * @details 新增型号时只在此枚举与 xxx.c 的 switch 中增加分支，公共 API 不变
 */
enum TMPL_SENSOR_CHIP {
    /// 型号 A：单次转换，量程由 chipParameter 给出
    TMPL_SENSOR_CHIP_A = 0,
    /// 型号 B：支持周期转换
    TMPL_SENSOR_CHIP_B,
    // add more chip here

    TMPL_SENSOR_CHIP_END,
    TMPL_SENSOR_CHIP_NUM = TMPL_SENSOR_CHIP_END
};

/**
 * @brief 采样模式
 */
enum TMPL_SENSOR_MODE {
    /// 单次采样
    TMPL_SENSOR_MODE_SINGLE = 0,
    /// 周期采样：100ms
    TMPL_SENSOR_MODE_AUTO_100MS,
    /// 周期采样：1000ms
    TMPL_SENSOR_MODE_AUTO_1000MS,
    TMPL_SENSOR_MODE_END
};

/// 数值反向（低有效 / 反向换算）
#define TMPL_SENSOR_FLAG_INVERT       0x0001
/// 采样值需要除以 10（定点换算）
#define TMPL_SENSOR_FLAG_SCALE_0_1    0x0002
/// 保留给应用层使用的自定义标志
#define TMPL_SENSOR_FLAG_USER_DEFINE0 0x8000

/**
 * @brief 传感器硬件配置（描述符）
 *
 * @details 由板级配置声明为 static const 数组，只读，放在 Flash。
 *          驱动初始化时按 id 与内部实例绑定。
 */
typedef struct {
    /// 逻辑 ID，见 config/export_ids.h
    uint8_t id;
    /// 芯片类型，见 ::TMPL_SENSOR_CHIP
    uint8_t chip;
    /// 抽象总线 ID，见抽象 i2c 驱动中定义的总线枚举
    uint8_t bus;
    /// 器件地址（7bit）
    uint16_t address;
    /// 行为标志，见 TMPL_SENSOR_FLAG_*
    uint16_t flags;
    /// 型号私有参数，NULL 表示使用默认值
    /// 约定格式："range:500;scale:10"
    const char *chipParameter;
} tmplSensorConfig_t;

/**
 * @brief 一次采样结果
 */
typedef struct {
    /// 原始换算后的测量值
    int32_t value;
    /// 温度，单位 0.1 摄氏度
    int16_t temperature;
    /// 器件状态字，0 表示正常
    uint8_t status;
    /// 采样序号，用于判断是否为陈旧数据
    uint8_t sequence;
} tmplSensorData_t;

/**
 * @brief 初始化传感器模块
 *
 * @param[in] configs 描述符数组，生命周期必须覆盖运行期（建议 static const）
 * @param[in] num     描述符数量，必须 <= CONFIG_TMPL_SENSOR_MAX_NUM
 * @return int
 *  - 成功时返回**已成功初始化的实例数量**（>=0）
 *  - RET_INVALID_ARGS 参数非法（NULL / 超容量 / id 重复）
 *  - RET_NO_OBJECT    总线或器件无响应
 *
 * @note 调用上下文：初始化阶段（任务启动前或单一初始化任务），不可重入
 */
int tmplSensorInit(const tmplSensorConfig_t *configs, int num);

/**
 * @brief 读取一次采样结果
 *
 * @param[in]  id  逻辑 ID
 * @param[out] out 结果输出，不允许为 NULL
 * @return int
 *  - RET_SUCCESS       成功
 *  - RET_INVALID_ARGS  参数非法 / 未初始化
 *  - RET_NO_OBJECT     实例不存在（ID 未在描述符表中注册）
 *  - RET_TIMEOUT       器件响应超时
 *
 * @note 调用上下文：任务上下文；内部带超时，**可在 ISR 中调用但会返回 RET_FAILED**
 * @note 可重入：否（同一实例需由调用方串行化）
 */
int tmplSensorRead(uint8_t id, tmplSensorData_t *out);

/**
 * @brief 设置采样模式
 *
 * @param[in] id   逻辑 ID
 * @param[in] mode 模式，见 ::TMPL_SENSOR_MODE
 * @return int
 *  - RET_SUCCESS      成功
 *  - RET_NO_OBJECT    实例不存在
 *  - RET_NOT_SUPPORT  当前芯片类型不支持该模式
 *
 * @note 调用上下文：任务上下文；可重入：否
 */
int tmplSensorSetMode(uint8_t id, uint8_t mode);

/**
 * @brief 获取实例的描述符（只读）
 *
 * @param[in]  id     逻辑 ID
 * @param[out] config 返回描述符指针，不可修改
 * @return int
 *  - RET_SUCCESS    成功
 *  - RET_NO_OBJECT  实例不存在
 */
int tmplSensorGetConfig(uint8_t id, const tmplSensorConfig_t **config);

/**
 * @brief 获取当前已初始化的实例数量
 *
 * @return int 实例数量
 */
int tmplSensorGetCount(void);

/**
 * @brief 按序号获取已注册实例的逻辑 ID
 *
 * @param[in]  index 序号（0 .. tmplSensorGetCount()-1）
 * @param[out] id    返回逻辑 ID
 * @return int
 *  - RET_SUCCESS    成功
 *  - RET_NO_OBJECT  序号越界
 *
 * @note 供 CLI / 上层遍历使用；驱动本身不依赖 console，打印职责在调用方
 */
int tmplSensorGetIdByIndex(int index, uint8_t *id);

#endif // __DRV_TMPL_SENSOR_H__

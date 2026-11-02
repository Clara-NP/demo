#ifndef __CONFIG_EXPORT_IDS_H__
#define __CONFIG_EXPORT_IDS_H__

/**
 * @file export_ids.h
 * @author <author>
 * @brief 逻辑 ID 导出表（模块开发流程步骤 ②）
 * @version 0.1
 * @date 2026-11-02
 *
 * @note
 *  - 本文件只定义**逻辑名**，是"产品有哪些资源"的唯一清单
 *  - 禁止在此写引脚、器件地址、波特率 —— 这些只属于 `src/system/board.c`
 *  - 每个枚举末尾必须有 `EXPORT_<NAME>_MAX_NUM` 哨兵，供 `CONFIG_<NAME>_MAX_NUM` 自动跟随
 *
 * @par 执行上下文
 *  编译期常量，无运行期状态；不涉及中断/线程，可重入性不适用
 */

/**
 * @brief 抽象 I2C 总线逻辑 ID
 *
 * @note 物理总线号由 `port/<soc>/` 的总线驱动决定，应用层只认逻辑 ID
 */
enum EXPORT_I2C
{
    I2C_BUS_0 = 0,

    EXPORT_I2C_MAX_NUM
};

/**
 * @brief 传感器逻辑 ID（与 `tmplSensorConfig_t.id` 一一对应）
 *
 * @note 增删资源只改这里：描述符表与容量宏均由本枚举派生
 */
enum EXPORT_TMPL_SENSOR
{
    /// 压力传感器
    SENSOR_PRESSURE = 0,
    /// 温度传感器
    SENSOR_TEMPERATURE,

    EXPORT_TMPL_SENSOR_MAX_NUM
};

#endif // __CONFIG_EXPORT_IDS_H__

#ifndef __DRV_GPIO_H__
#define __DRV_GPIO_H__

/**
 * @file gpio.h
 * @brief 平台无关的通用GPIO驱动实现的头文件
 */

#include "common/generic.h"
#include "common/gpio_pins.h"

/**
 * @brief 定义支持的GPIO的芯片类型
 * 
 */
enum GPIO_CHIP
{
    /// 芯片自带的GPIO
    GPIO_CHIP_SOC = 0,
    /// 由PWM驱动的IO，支持呼吸效果
    /// 只能是输出
    GPIO_CHIP_PWM,
    // add more chip here
    GPIO_CHIP_END,
    GPIO_CHIP_NUM = GPIO_CHIP_END
};




// GPIO 控制字
#define GPIO_DIR_INPUT          0
#define GPIO_DIR_OUTPUT         1

#define GPIO_FLAG_ACTIVE_LOW      0x0001
#define GPIO_FLAG_INIT_ACTIVE     0x0002 
#define GPIO_FLAG_INPUT_PULLUP    0x0004
#define GPIO_FLAG_INPUT_PULLDOWN  0x0008

/**
 * @brief 设备GPIO的定义
 * 
 */
typedef struct 
{
    uint8_t id;        /// 在本工程中对应的GPIO-ID
    uint8_t chip;      /// 所属芯片
    // gpio_t  gpio;      /// 对应的板级GPIO-ID  需要抽象出来适配所有平台
    uint8_t gpio;         /// 与上述一致  本质上的类型是uint8_t
    uint8_t direction; /// 方向
    uint16_t flags;    /// 标志位
}gpioConfig_t;




/**
 * @brief 初始化GPIO
 * 
 * @param config GPIO配置组
 * @param num GPIO数量
 * @return int 
 */
int gpioInit(const gpioConfig_t *config, int num);



#endif /* __DRV_GPIO_H__ */
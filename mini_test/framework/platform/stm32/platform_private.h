#ifndef __STM32_PLATFORM_PRIVATE_H__
#define __STM32_PLATFORM_PRIVATE_H__

// #include "common/generic.h"
// #include "common/gpio_pins.h"
#include "platform_config.h"

#if defined(__stm32f4xx__)
#include <stm32f4xx.h>

#include "stm32f4xx_ll_gpio.h"
#include "stm32f4xx_ll_rcc.h"
#include "stm32f4xx_ll_bus.h"
#include "stm32f4xx_ll_utils.h"
#include "stm32f4xx_ll_system.h"
#include "stm32f4xx_ll_usart.h"
#include "stm32f4xx_ll_tim.h"
#include "stm32f4xx_ll_dma.h"
#include "stm32f4xx_ll_iwdg.h"
#include "stm32f4xx_ll_adc.h"
#include "stm32f4xx_ll_rtc.h"
#include "stm32f4xx_ll_pwr.h"
#include "stm32f4xx_ll_i2c.h"
#include "stm32f4xx_ll_spi.h"
// for GPIO_AF2_TIM3 etc
#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_gpio_ex.h"
// for NVIC_SetPriorityGrouping
#include "stm32f4xx_hal_cortex.h"
#include "stm32f4xx_hal_rcc_ex.h"

// for flash operation
#include "stm32f4xx_hal_flash.h"
#include "stm32f4xx_hal_flash_ex.h"

#ifdef CONFIG_STM32_HAL_ENABLE
#include "stm32f4xx_hal.h"
#endif 


#elif defined(__stm32g03xx__)
#include "stm32g0xx.h"
#ifdef STM32G030xx
#include <stm32g030xx.h>
#else
#include <stm32g031xx.h>
#endif

#include "stm32g0xx_ll_gpio.h"
#include "stm32g0xx_ll_rcc.h"
#include "stm32g0xx_ll_bus.h"
#include "stm32g0xx_ll_utils.h"
#include "stm32g0xx_ll_system.h"
#include "stm32g0xx_ll_tim.h"
#include "stm32g0xx_ll_usart.h"
#include "stm32g0xx_ll_adc.h"
#include "stm32g0xx_ll_dma.h"
#include "stm32g0xx_ll_iwdg.h"
#include "stm32g0xx_ll_rtc.h"
#include "stm32g0xx_ll_pwr.h"
#include "stm32g0xx_ll_i2c.h"

#else 
/// 默认支持STM32F10X 
#include <stm32f10x.h>
#error "stm32f1xx functions to be supplemented"
#endif 

// #include "peripheral.h"

// typedef struct 
// {
//     int remap; // 小于0表示没有remap
//     struct {
//         bool valid;
//         gpio_t gpio;
//         uint32_t af;
//     }ch[4];
// }timerGpioConfig_t;

// /**
//  * @brief  重启系统
//  */
// void stm32Reset(void);

// /**
//  * @brief 使能指定的GPIO端口
//  * 
//  * @param gpioBank 
//  * @param enable 
//  */
// void stm32GpioPortEnable(void *gpioBank, int enable);


// /**
//  * @brief 更高效返回GPIO的端口
//  *
//  * @param gpio
//  * @return GPIO_TypeDef*
//  */
// GPIO_TypeDef* stm32GpioToPort(gpio_t gpio);

// /**
//  * @brief 抽象的GPIO索引，转换为平台依赖的GPIO索引函数，供平台内的其他模块使用
//  * 
//  * @param gpio 
//  * @return uint8_t 
//  */
// #define stm32GpioToPin(_gpio) ((_gpio) & 0x0f)

// /**
//  * @brief 使能指定的定时器时钟
//  * 
//  * @param tim 
//  * @param enable 
//  */
// void stm32TimerClockCmd(void *tim, int enable);
// /**
//  * @brief 使能指定的DMA时钟
//  *
//  * @param dma
//  * @param enable
//  */
// void stm32DmaClockCmd(void *dma, int enable);

/*
// #define __NVIC_SetPriority(irq, PreemptPriority, SubPriority) \
//     do \
//     { \
//         uint32_t prioritygroup = NVIC_GetPriorityGrouping(); \
//         NVIC_SetPriority((IRQn_Type)irq, NVIC_EncodePriority(prioritygroup, PreemptPriority, SubPriority)); \
//     } while(0)

// #define __NVIC_EnableIRQ(irq) \
//     do \
//     { \
//         NVIC_EnableIRQ((IRQn_Type)irq); \
//     } while(0)

// #define __NVIC_DisableIRQ(irq) \
//     do \
//     { \
//         NVIC_DisableIRQ((IRQn_Type)irq); \
//     } while(0)
*/

// /**
//  * @brief 返回Remap配置ID
//  * 
//  * @param config 
//  * @return int 
//  */
// int stm32ParseRemapId(const char *config);

// /**
//  * @brief 获取定时器的时钟
//  * 
//  * @param id 
//  * @return const uint32_t 
//  *
//  */
// uint32_t stm32GetTimerClock(uint8_t id);


// /**
//  * @brief 获取定时器GPIO配置
//  * 
//  * @param tim 
//  * @param parameter 
//  * @param config
//  * @return int 
//  *  RET_SUCCESS: 成功
//  *  RET_INVALID_PARAMETER: 参数错误
//  *  RET_NO_SUPPORTED: 不支持
//  */ 
// int stm32GetTimerGpios(TIM_TypeDef *tim, const char *parameter, timerGpioConfig_t *config);


// /**
//  * @brief 芯片系列型号
//  * 
//  */
// enum CHIP_SERIAL
// {
//     _SERIES_F0 = 0,
//     _SERIES_F1,
//     _SERIES_F2,
//     _SERIES_F3,
//     _SERIES_F4,
//     _SERIES_G0,
// };


/**
 * @brief STM32F系列 时钟配置
 * STM32F10x@72MHZ
 * 
 * APB1 = SYSCLK/2 = 36MHZ
 * APB2 = SYSCLK/1 = 72MHZ
 * 
 * 关联文件：system_stm32f10x.h 函数 SetSysClockTo72() 中定义
 *   // HCLK = SYSCLK 
 *   RCC->CFGR |= (uint32_t)RCC_CFGR_HPRE_DIV1;   
 *   // PCLK2 = HCLK 
 *   RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE2_DIV1;
 *   // PCLK1 = HCLK 
 *   RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE1_DIV2;
 * 
 * STM32F40x@168MHZ
 * APB1 =  SYSCLK/4 = 42MHZ
 * APB2 =  SYSCLK/2 = 84MHZ
 * 
 * 关联文件: system_stm32f4xx.c 
 * 
 * 
 * STM32F446xx@180MHZ
 * 
 * AHB = SYSCLK / 1 = 180MHZ
 * APB1 = SYSCLK/4 = 45MHZ
 * APB2 = SYSCLK/2 = 90MHZ 
 *  
 */
#ifdef __stm32f4xx__

#define STM32_AHB_PRESCALER    1
#define STM32_APB1_PRESCALER   4
#define STM32_APB2_PRESCALER   2

#elif defined  __stm32g03xx__
#define STM32_AHB_PRESCALER    1
#define STM32_APB1_PRESCALER   1
#define STM32_APB2_PRESCALER   1

#else // stm32f10x

#define STM32_AHB_PRESCALER    1
#define STM32_APB1_PRESCALER   2
#define STM32_APB2_PRESCALER   1

#endif // 


#endif // __STM32_PLATFORM_PRIVATE_H__


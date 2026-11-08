
#ifndef __STM32_PLATFORM_H__
#define __STM32_PLATFORM_H__


// #include <stdio.h>
// #include <string.h>
// #include <stdlib.h>
// #include <stdint.h>
// #include <stdarg.h>
// #include <stdbool.h>

// #ifdef CONFIG_FREERTOS
// #include <FreeRTOS.h>
// #include <task.h>
// #endif 

// /**
//  * @addtogroup platform_stm32 STM32
//  * @ingroup platform
//  * @brief STM32的平台实现
//  * 
//  */

#include "platform_config.h"

// /**
//  * @brief  重启系统
//  */
// extern void stm32Reset(void);

// /// 系统函数抽象
// #define osMemcpy(_dst, _src, _size)     memcpy(_dst, _src, _size)
// #define osMemmove(_dst, _src, _size)    memmove(_dst, _src, _size)
// #define osMemcmp(_dst, _src, _size)     memcmp(_dst, _src, _size)
// #define osMemset(_dst, _val, _size)     memset(_dst, _val, _size)

// #define osStrcpy(_dst, _src)            strcpy(_dst, _src)
// #define osStrncpy(_dst, _src, _size)    strncpy(_dst, _src, _size)
// #define osStrcmp(_dst, _src)            strcmp(_dst, _src)
// #define osStrncmp(_dst, _src, _size)    strncmp(_dst, _src, _size)

// #define osSrand(_seed)  srand(_seed)
// #define osRand16()      (uint16_t)rand() 

// #define osReboot()     stm32Reset()

/*
// #define osSafeStrncpy(_dst, _src, _size) do { \
//     strncpy(_dst, _src, _size); \
//     _dst[(_size) - 1] = '\0'; \
// }while(0)
*/

// #ifdef CONFIG_FREERTOS

// #define osMalloc(_s)  pvPortMalloc(_s)
// #define osFree(_v)    vPortFree(_v)

// #define  osEnterCritical()   taskENTER_CRITICAL()
// #define  osExitCritical()    taskEXIT_CRITICAL()

// #else // FREERTOS

// #define osMalloc(_s)  malloc(_s)
// #define osFree(_v)    free(_v)

// #define  osEnterCritical()   
// #define  osExitCritical()    

// #endif // not FREERTOS


/**
 * @brief 平台初始化函数
 * 
 */
extern void platformInit(void);

// /**
//  * @brief 是否在中断上下文中
//  * 
//  * @return int 1 - 在中断中， 0 - 不在
//  */
// static inline int osInInterrupt(void)
// {
// #if defined(__GNUC__)
//     uint32_t val;
//     __asm volatile("mrs %0, ipsr" : "=r"(val) :: "memory");
//     return (val > 0) ? 1 : 0;
// #else
//     register uint32_t val __ASM("ipsr");
//     return (val > 0) ? 1 : 0;
// #endif
// }

// /**
//  * @brief 为了实现更快速的IO操作，将平台内部类型放开，有限使用 
//  * 
//  */

// #ifdef __stm32f4xx__
// #include <stm32f4xx.h>
// #elif defined  __stm32g03xx__
// #include "stm32g0xx.h"
// #ifdef STM32G030xx
// #include <stm32g030xx.h>
// #else
// #include <stm32g031xx.h>
// #endif
// #else 
// #include <stm32f10x.h>
// #endif 

// extern GPIO_TypeDef * const g_portBase[];
// #define _socGpioPort(_gpio)  g_portBase[(_gpio) >> 4]
// #define _socGpioPin(_gpio)   (1 << ((_gpio) & 0x0f))

// // 高效的GPIO操作宏， 刷新测试屏(128*160@spi):25ms
// #define _socGpioSet(_gpio)      _socGpioPort(_gpio)->BSRR = (1 << (_gpio & 0x0f))
// #ifdef __stm32f4xx__
// #define _socGpioReset(_gpio)    _socGpioPort(_gpio)->BSRR = (1 << ((_gpio & 0x0f) + 16))
// #else 
// #define _socGpioReset(_gpio)    _socGpioPort(_gpio)->BRR = (1 << (_gpio & 0x0f))
// #endif 
// #define _socGpioIn(_gpio)       ((_socGpioPort(_gpio)->IDR & (1 << (_gpio & 0x0f))) ? 1 : 0)

// /// 更高效的宏

// #define __socGpioSet(_port, _pin)      ((GPIO_TypeDef *)(_port))->BSRR = (_pin)
// #ifdef __stm32f4xx__
// #define __socGpioReset(_port, _pin)    ((GPIO_TypeDef *)(_port))->BSRR = ((uint32_t)(_pin) << 16)
// #else
// #define __socGpioReset(_port, _pin)    ((GPIO_TypeDef *)(_port))->BRR = (_pin)
// #endif 


#endif /* __STM32_PLATFORM_H__ */

#include "platform_private.h"

// #include "common/sys_tick.h"



// #define TRACE_ENABLE 
// #define TRACE_TAG "sys-tick"
// #define TRACE_LEVEL  T_INFO
// #include <common/trace.h>

#ifdef CONFIG_FREERTOS
#include <task.h>
#else 
/// 没有FREERTOS ，默认开启SYSTICK
#define CONFIG_SYSTICK_TIMER 1
#endif  /* CONFIG_FREERTOS */


/**
 * @brief 单位为秒
 * 
 */
#ifndef CONFIG_SYSTICK_HZ
#define CONFIG_SYSTICK_HZ  1000
#endif 


#ifdef CONFIG_SYSTICK_TIMER
/**
 * @brief 系统滴答计数器
 * 
 */
static volatile uint32_t s_sysTick = 0;

/**
 * @brief SYSTICK中断处理函数
 * 
 */
void SysTick_Handler(void)
{	
    s_sysTick ++;
}

#endif // CONFIG_SYSTICK_TIMER


/**
 * @brief 初始化SYS TICK
 * 
 * @note  1/FCPU * n = 1/1000
 *   n = FCPU/1000 = FCPU * (1/1000)
 */
void sysTickInit(void)
{
    #ifdef CONFIG_SYSTICK_TIMER

    SysTick_Config(SystemCoreClock / CONFIG_SYSTICK_HZ);
    
    #endif // CONFIG_SYSTICK_TIMER
}

// /**
//  * @brief 为了提高效率，目前只考虑1000HZ的系统TICK
//  * 
//  */
// // #if configTICK_RATE_HZ != 1000
// // #error "configTICK_RATE_HZ is not set to 1000, please check!!!"
// // #endif 

// /**
//  * @brief 获取系统的Tick时间戳，单位为ms
//  * @return 系统时间Tick，类型为sysTick_t
//  **/
// sysTick_t upTime(void)
// {
//     #ifdef CONFIG_FREERTOS
//     return (sysTick_t)xTaskGetTickCount();

//     #else 

//     #ifdef CONFIG_SYSTICK_TIMER
//     return (sysTick_t)s_sysTick;
//     #endif 
//     #endif 
// }



// #ifdef CONFIG_STM32_HAL_ENABLE

// HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority)
// {
//   /* Return function status */
//   return HAL_OK;
// }


// /**
//   * @brief Provides a tick value in millisecond.
//   * @note This function is declared as __weak to be overwritten in case of other 
//   *       implementations in user file.
//   * @retval tick value
//   */
// uint32_t HAL_GetTick(void)
// {
//   return upTime();
// }

// #endif // CONFIG_STM32_HAL_ENABLE


// #if defined (__CC_ARM) /*!< ARM Compiler */


// /**
//  * @brief 底层延时处理，3个时钟周期，请注意，汇编代码不能顶格写。。
//  * 
//  * @param count 
//  * @return __asm 
//  */
// __asm void __delayloop(unsigned long count)
// {
//     subs r0, #1;
//     bne.n __delayloop;
//     bx lr;
// }

// #elif defined ( __ICCARM__ ) /*!< IAR Compiler */

// void __delayloop(unsigned long count)
// {
//     __asm(" subs r0, #1\n"
//         " bne.n __delayloop\n"
//         " bx lr");
// }


// #elif defined (__GNUC__) /*!< GNU Compiler */

// void __attribute__((naked)) 
// __delayloop(unsigned long count)
// {
//     __asm(" subs r0, #1\n"
//             " bne __delayloop\n"
//             " bx lr");
// }

// #else 

// error("***Unsupported Compiler flag!!!");

// #endif 

// /**
//  * @brief 微秒级延时函数，仅支持小于1s内的输入参数
//  * @param[in] us 延时时间参数
//  * 
//  **/
// void udelay(uint32_t us)
// {
//     // 100 MHZ 那么一个时钟周期的时间是 1/1000 000 000 s ==> 1ns
//     // 每个count表示的是3个周期，那么是 C= ((1000 000 000 / CPU_FREQ) * 3)， ns
//     // 目标延时是T us, 需要的数据为 (T * 1000)/C
//     // count = T/C = ((T * 1000) / (3000 000 000 / CPU_FREQ))
//     // count = (T * 1000 * CPU_FREQ) / 3000 000 000
//     // count = (T * CPU_FREQ / 3000 000)

//     // 实测值预期值要大1us
//     return __delayloop(us * (SystemCoreClock / 3000000UL));
// }


// /**
//  * @brief 毫秒级延时函数
//  * @param[in] ms 延时时间参数
//  * 
//  **/
// void mdelay(uint32_t ms)
// {
//     #ifdef CONFIG_FREERTOS    
//     // 在OS调度之前，不使用 vTaskDelay
//     if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
//     {
//         // 已启动
//         vTaskDelay(ms);
//         return ;
//     }
//     #endif 

//     LL_mDelay(ms);
// }


// /// configGENERATE_RUN_TIME_STATS defined in FreeRTOSConfig.h
// #if (configGENERATE_RUN_TIME_STATS == 1)

// /**
//  * @brief 定义系RUNTIME使用的定时器，仅支持2，3，4，5, 6, 7
//  * 
//  */
// #ifndef CONFIG_RUN_TIME_STATS_TIMER  
// #define CONFIG_RUN_TIME_STATS_TIMER  4
// #endif 


// #include <common/soc/soc_timer.h>

// /**
//  * @brief 定义一个系统变量 
//  * 
//  */
// volatile unsigned long long socRunTimeTicks = 0;


// /**
//  * @brief 获取RUN TIME TICK
//  * 
//  * @return unsigned long long 
//  */
// unsigned long long socGetRunTimeTick(void)
// {
//     return socRunTimeTicks;
// }


// /**
//  * @brief 定时中断处理函数 
//  * 
//  * @param id 
//  */
// __interrupt void socRunTimeStatsTimerHandle(uint8_t id)
// {
//     socRunTimeTicks ++;
// }


// /**
//  * @brief 初始化运行状态统计的定时器
//  * 
//  */
// void socInitTimerForRunTimeStats(void)
// {
//     // 初始化定时器,使用中断 
//     socTimerInit(CONFIG_RUN_TIME_STATS_TIMER, CONFIG_RUN_TIME_STATS_TICK_PERIOD, true, socRunTimeStatsTimerHandle, NULL);
// }


// #endif // (configGENERATE_RUN_TIME_STATS == 1)

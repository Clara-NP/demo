/**
 * @file sys_tick.c
 * @brief SysTick 1 ms 心跳：为 HAL 超时与 osTickMs 提供时基
 *
 * @par 执行上下文
 *  启动阶段调用（platformInit 内）；SysTick 中断由 HAL 接管
 *
 * @par 可重入性
 *  否，重复配置会重置计数器
 *
 * @par 资源所有权
 *  不持有资源，只配置 SysTick 定时器
 */

#include <stm32f4xx_hal.h>

void sysTickInit(void) {
    uint32_t reload = HAL_RCC_GetHCLKFreq() / 1000U;

    if (HAL_SYSTICK_Config(reload) != 0U) {
        /* 配置失败时沿用 HAL_Init 已建立的 1 ms 时基 */
        return;
    }
}

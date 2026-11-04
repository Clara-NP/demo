/**
 * @file main.c
 * @brief led_project 最小入口：platformInit 后进入空闲循环
 *
 * @note 当前阶段只验证“能编过并链接出固件”：
 *       - 平台初始化走平台层的 platformInit（HAL + 时钟 + SysTick）
 *       - LED 点亮待 framework 的 gpio 模块落地后补
 *
 * @par 执行上下文
 *  复位后主上下文；不进入 ISR
 *
 * @par 可重入性
 *  否，仅启动调用一次
 *
 * @par 资源所有权
 *  不持有资源
 */

#include <stm32f4xx_hal.h>

#include "platform.h"

int main(void) {
    platformInit();

    for (;;) {
        __WFI(); /* 空闲时休眠，避免忙等 */
    }
}
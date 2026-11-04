/**
 * @file main.c
 * @brief 最小固件：复位后翻转 PA5，使板上 LED 闪烁
 *
 * @par 执行上下文
 *  复位后主上下文；不进入 ISR
 *
 * @par 可重入性
 *  否，仅启动调用一次
 *
 * @par 资源所有权
 *  直接占用 RCC 与 GPIOA，不经过其它模块
 *
 * 时钟保持复位默认的 HSI 16 MHz，不配 PLL。
 * LED 按 Nucleo-F446RE 的 LD2（PA5）。灯不在这只脚上时改 LED_PIN。
 * STM32F4 的 RCC / GPIOA 基址相同，换型号时只需改链接脚本里的容量。
 */

#include <stdint.h>

#define REG32(addr) (*(volatile uint32_t *)(uintptr_t)(addr))

#define RCC_AHB1ENR REG32(0x40023800u + 0x30u)
#define GPIOA_MODER REG32(0x40020000u + 0x00u)
#define GPIOA_BSRR  REG32(0x40020000u + 0x18u)

#define RCC_AHB1ENR_GPIOAEN (1u << 0)

#define LED_PIN 5u

/* HSI 16 MHz 下大约半秒，只要求肉眼能看出闪烁 */
#define DELAY_LOOPS 2000000u

static void delay_loops(uint32_t loops) {
    volatile uint32_t left = loops;

    while (left > 0u) {
        left--;
    }
}

int main(void) {
    uint32_t shift = LED_PIN * 2u;

    RCC_AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    (void)RCC_AHB1ENR;

    GPIOA_MODER &= ~(3u << shift);
    GPIOA_MODER |= (1u << shift);

    for (;;) {
        GPIOA_BSRR = (1u << LED_PIN);
        delay_loops(DELAY_LOOPS);
        GPIOA_BSRR = (1u << (LED_PIN + 16u));
        delay_loops(DELAY_LOOPS);
    }
}

/**
 * @file system_clock_config.c
 * @brief 系统时钟配置：SYSCLK 取 HSI 16 MHz，所有总线分频为 1
 *
 * @note 只建立最小可用时钟树，不启用 PLL / HSE：
 *       现阶段验证的是"能编过并能跑"，不依赖板载晶振是否存在。
 *       板上时钟确定后在此扩展 PLL 配置。
 *
 * @par 执行上下文
 *  启动阶段调用一次（platformInit 内）；不进入 ISR
 *
 * @par 可重入性
 *  否，运行期改时钟会破坏已配置的外设
 *
 * @par 资源所有权
 *  不持有资源，只写 RCC/FLASH 寄存器
 */

#include <stm32f4xx_hal.h>

void SystemClock_Config(void) {
    RCC_OscInitTypeDef oscInit = {0};
    RCC_ClkInitTypeDef clkInit = {0};

    oscInit.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    oscInit.HSIState = RCC_HSI_ON;
    oscInit.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    oscInit.PLL.PLLState = RCC_PLL_NONE;

    if (HAL_RCC_OscConfig(&oscInit) != HAL_OK) {
        /* 失败时保持复位默认时钟，不在此处死等 */
        return;
    }

    clkInit.ClockType =
        RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    clkInit.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
    clkInit.AHBCLKDivider = RCC_SYSCLK_DIV1;
    clkInit.APB1CLKDivider = RCC_HCLK_DIV1;
    clkInit.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&clkInit, FLASH_LATENCY_0) != HAL_OK) {
        /* 分频失败时保持上一级时钟，不在此处死等 */
        return;
    }
}

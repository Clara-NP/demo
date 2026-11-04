

#include "platform.h"


extern void sysTickInit(void);
// 使用外部的时钟配置
extern void SystemClock_Config(void);

/**
 * @brief 平台初始化函数
 *
 */
void platformInit(void)
{
    /**
     * @brief 如果是应用，需要修改向量地址
     *
     */
#if defined(APP_ENTRY)
    SCB->VTOR = APP_ENTRY;
#endif

    /* USER CODE BEGIN Init */

    /* USER CODE END Init */

    #ifdef CONFIG_STM32_HAL_ENABLE
    // 初始化HAL, 虽然HAL中也会使用SysTick, 但这里还是需要调用
    HAL_Init();
    #endif 

    /* Configure the system clock */
    SystemClock_Config();

    // set system core clock
    SystemCoreClockUpdate();


    // 开启SYS TICK
    sysTickInit();


    // #ifdef TRACE_ENABLE
    // // 初始化Trace
    // traceInit();

    // // 显示芯片信息
    // dumpChipInfo();
    // #endif 
}

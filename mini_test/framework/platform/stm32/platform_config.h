#ifndef __PLATFORM_CONFIG_H__
#define __PLATFORM_CONFIG_H__


/**
 * @addtogroup stm32_default_config 默认配置项
 * @ingroup platform_stm32
 * @brief STM32默认的平台相关的配置项
 * 
 * @{
 */

/// 配置默认的调试串口索引
#ifndef CONFIG_TRACE_UART
#define CONFIG_TRACE_UART  _UART1
#endif 

/// 自动选择芯片类型

/// STM32F4
#if defined(STM32F40_41xxx) || defined(STM32F427_437xx) || defined(STM32F429_439xx) || defined(STM32F401xx) || defined(STM32F410xx) || \
    defined(STM32F411xE) || defined(STM32F412xG) || defined(STM32F413_423xx) || defined(STM32F446xx) || defined(STM32F469_479xx)
#define __stm32f4xx__    1
#endif 


/// STM32F1
#if defined (STM32F10X_LD) || defined (STM32F10X_LD_VL) || defined (STM32F10X_MD) || defined (STM32F10X_MD_VL) || \
    defined (STM32F10X_HD) || defined (STM32F10X_HD_VL) || defined (STM32F10X_XL) || defined (STM32F10X_CL) 
#define __stm32f10x__    1 
#endif 


/// STM32G030xx / STM32G031xx
#if defined (STM32G030xx) || defined(STM32G031xx)
#define __stm32g03xx__    1 
#endif 

/// 启用字节序转换
#define CONFIG_NETWORK_BYTE_ORDER_SWAP  1
/// STM32 is little-endian
#define CONFIG_ORDER_LITTER_ENDIAN      1

/// STM32 使用16位的GPIO模式 
#define CONFIG_GPIO_PORT_WIDTH_16       1


/**
 * @brief 配置默认的中断优先级
 * 
 * @note  与平台初始化时使用的优先级配置有关
 *   NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);	 
 *   STM32中有4位用来表示优先级
 *   NVIC_PriorityGroup_4：表示有四位均为抢占优先级， 没有响应优先级
 */
#define CONFIG_IRQ_PREEMPTION_PRIORITY  15
#define CONFIG_IRQ_SUB_PRIORITY         0


/**
 * @brief 配置UART抢占优先级
 * 
 */
#ifndef CONFIG_UART_IRQ_PREEMPTION_PRIORITY
#define CONFIG_UART_IRQ_PREEMPTION_PRIORITY  CONFIG_IRQ_PREEMPTION_PRIORITY
#endif 

/**
 * @brief 配置GPIO抢占优先级
 * 
 */
#ifndef CONFIG_GPIO_IRQ_PREEMPTION_PRIORITY
#define CONFIG_GPIO_IRQ_PREEMPTION_PRIORITY  CONFIG_IRQ_PREEMPTION_PRIORITY
#endif 

/**
 * @brief 配置DMA抢占优先级
 * 
 */
#ifndef CONFIG_DMA_IRQ_PREEMPTION_PRIORITY
#define CONFIG_DMA_IRQ_PREEMPTION_PRIORITY   CONFIG_IRQ_PREEMPTION_PRIORITY
#endif 

/**
 * @brief 配置定时器抢占优先级
 * 
 */
#ifndef CONFIG_TIMER_IRQ_PREEMPTION_PRIORITY
#define CONFIG_TIMER_IRQ_PREEMPTION_PRIORITY  CONFIG_IRQ_PREEMPTION_PRIORITY
#endif 

/**
 * @brief 配置CAN总线抢占优先级
 * 
 */
#ifndef CONFIG_CAN_IRQ_PREEMPTION_PRIORITY
#define CONFIG_CAN_IRQ_PREEMPTION_PRIORITY  CONFIG_IRQ_PREEMPTION_PRIORITY
#endif 


/**
 * @brief 配置UART子优先级
 * 
 */
#ifndef CONFIG_UART_IRQ_SUB_PRIORITY
#define CONFIG_UART_IRQ_SUB_PRIORITY  CONFIG_IRQ_SUB_PRIORITY
#endif 

/**
 * @brief 配置GPIO子优先级
 * 
 */
#ifndef CONFIG_GPIO_IRQ_SUB_PRIORITY
#define CONFIG_GPIO_IRQ_SUB_PRIORITY  CONFIG_IRQ_SUB_PRIORITY
#endif 

/**
 * @brief 配置DMA子优先级
 * 
 */
#ifndef CONFIG_DMA_IRQ_SUB_PRIORITY
#define CONFIG_DMA_IRQ_SUB_PRIORITY   CONFIG_IRQ_SUB_PRIORITY
#endif 

/**
 * @brief 配置定时器子优先级
 * 
 */
#ifndef CONFIG_TIMER_IRQ_SUB_PRIORITY
#define CONFIG_TIMER_IRQ_SUB_PRIORITY  CONFIG_IRQ_SUB_PRIORITY
#endif 

/**
 * @brief 配置CAN总线子优先级
 * 
 */
#ifndef CONFIG_CAN_IRQ_SUB_PRIORITY
#define CONFIG_CAN_IRQ_SUB_PRIORITY  CONFIG_IRQ_SUB_PRIORITY
#endif 

/**
 * @brief 配置DMA通道的分配情况(DMA)
 * 
 * @note 程序已经固定分配好，这里不允许修改
 */
//STM32G030xx只有dma1，dma控制器的选择需要时再适配，暂时只考虑dma1
#define CONFIG_USART_DMA_DEV    DMA1
//接收先用中断接收，后续如果有需要再改成dma
//#define CONFIG_USART1_RX_DMA_CHANNEL   2    //LL_DMA_CHANNEL_3  
#define CONFIG_USART1_TX_DMA_CHANNEL   LL_DMA_CHANNEL_2
//#define CONFIG_USART2_RX_DMA_CHANNEL   3    //LL_DMA_CHANNEL_4
#define CONFIG_USART2_TX_DMA_CHANNEL   LL_DMA_CHANNEL_1


// 配置ADC的DMA设备和通道
#ifndef CONFIG_ADC_DMA_DEV
#define CONFIG_ADC_DMA_DEV      DMA1
#endif 

#ifndef CONFIG_ADC_DMA_CHANNEL
#define CONFIG_ADC_DMA_CHANNEL  LL_DMA_CHANNEL_4
#endif 


/** @}  */ 

#endif //__PLATFORM_CONFIG_H__

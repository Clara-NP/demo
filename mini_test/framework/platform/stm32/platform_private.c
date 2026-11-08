#include "platform_private.h"
// #include "common/soc/soc_timer.h"

// // If you trace function is not ready, you mask this macro
// #define TRACE_ENABLE 

// #ifdef TRACE_ENABLE
// #include "common/trace.h"
// #endif 

// extern void traceInit(void);

// /**
//  * @brief 端口基地址
//  *
//  * @todo 支持所有的GPIO端口
//  */
// GPIO_TypeDef *const g_portBase[] = {GPIOA, GPIOB, GPIOC, GPIOD,
// #ifdef CONFIG_GPIO_PORTEFG
// #ifdef GPIOE_BASE
//                                     GPIOE,
// #endif
// #ifdef GPIOF_BASE
//                                     GPIOF,
// #endif
// #ifdef GPIOG_BASE
//                                     GPIOG,
// #endif
// #endif // has port efg
// #ifdef CONFIG_GPIO_PORTHIJK
// #ifdef GPIOH_BASE
//                                     GPIOH,
// #endif
// #ifdef GPIOI_BASE
//                                     GPIOI,
// #endif
// #ifdef GPIOJ_BASE
//                                     GPIOJ,
// #endif
// #ifdef GPIOK_BASE
//                                     GPIOK.
// #endif
// #endif // has port hijk
// };


// /**
//  * @brief  重启系统
//  */
// void stm32Reset(void)
// {
//     __disable_irq();
//     NVIC_SystemReset();
// }


// /**
//  * @brief 使能指定的定时器时钟
//  *
//  * @param tim
//  * @param enable
//  */
// void stm32TimerClockCmd(void *tim, int enable)
// {
//     if (tim == NULL)
//     {

//     }
//     #ifdef _WITH_TIM1
//     else if (tim == TIM1)
//     {
//         if( enable ){
//             LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM1);
//         }
//         else{
//             LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_TIM1);
//         }
//     }
//     #endif 
//     #ifdef _WITH_TIM2
//     else if (tim == TIM2)
//     {
//         if( enable ){
//             LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM2);
//         }
//         else{
//             LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_TIM2);
//         }
//     }
//     #endif 
//     #ifdef _WITH_TIM3
//     else if (tim == TIM3)
//     {
//         if( enable ){
//             LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM3);
//         }
//         else{
//             LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_TIM3);
//         }
//     }
//     #endif 
//     #ifdef _WITH_TIM4
//     else if (tim == TIM4)
//     {
//         if( enable ){
//             LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM4);
//         }
//         else{
//             LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_TIM4);
//         }
//     }
//     #endif  
//     #ifdef _WITH_TIM5
//     else if (tim == TIM5)
//     {
//         if( enable ){
//             LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM5);
//         }
//         else{
//             LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_TIM5);
//         }
//     }
//     #endif   
//     #ifdef _WITH_TIM6
//     else if (tim == TIM6)
//     {
//         if( enable ){
//             LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM6);
//         }   
//         else{
//             LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_TIM6);
//         }
//     }
//     #endif   
//     #ifdef _WITH_TIM7
//     else if (tim == TIM7)
//     {
//         if( enable ){
//             LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM7);  
//         }
//         else{
//             LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_TIM7);
//         }
//     }
//     #endif 
//     #ifdef _WITH_TIM8
//     else if (tim == TIM8)
//     {
//         if( enable ){
//             LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM8);
//         }
//         else{
//             LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_TIM8);
//         }
//     }
//     #endif
//     #ifdef _WITH_TIM9
//     else if (tim == TIM9)
//     {
//         if( enable ){
//             LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM9);
//         }
//         else{
//             LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_TIM9);
//         }
//     }
//     #endif  
//     #ifdef _WITH_TIM10
//     else if (tim == TIM10)
//     {
//         if( enable ){
//             LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM10);
//         }
//         else{
//             LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_TIM10);
//         }
//     }
//     #endif  
//     #ifdef _WITH_TIM11
//     else if (tim == TIM11)
//     {
//         if( enable ){
//             LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM11);
//         }
//         else{
//             LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_TIM11);
//         }
//     }
//     #endif  
//     #ifdef _WITH_TIM12
//     else if (tim == TIM12)
//     {
//         if( enable ){
//             LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM12);
//         }
//         else{
//             LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_TIM12);
//         }
//     }
//     #endif  
//     #ifdef _WITH_TIM13
//     else if (tim == TIM13)
//     {
//         if( enable ){
//             LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM13);
//         }
//         else{
//             LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_TIM13);
//         }
//     }
//     #endif  
//     #ifdef _WITH_TIM14
//     else if (tim == TIM14)
//     {
//         if( enable ){
//             LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM14);
//         }
//         else{
//             LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_TIM14);
//         }
//     }
//     #endif 
//     #ifdef _WITH_TIM15
//     else if (tim == TIM15)
//     {
//         if( enable ){
//             LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM15);
//         }
//         else{
//             LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_TIM15);
//         }
//     }
//     #endif      
//     #ifdef _WITH_TIM16
//     else if (tim == TIM16)
//     {
//         if( enable ){
//             LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM16);
//         }
//         else{
//             LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_TIM16);
//         }
//     }
//     #endif 
//     #ifdef _WITH_TIM17
//     else if (tim == TIM17)
//     {
//         if( enable ){
//             LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM17);
//         }
//         else{
//             LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_TIM17);
//         }
//     }
//     #endif    
// }


// /**
//  * @brief 使能指定的DMA时钟
//  *
//  * @param dma
//  * @param enable
//  */
// void stm32DmaClockCmd(void *dma, int enable)
// {
//     if (dma == NULL)
//     {

//     }
// #ifdef _WITH_DMA1
//     if (dma == DMA1)
//     {
//         if( enable ){
//             LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);
//         }
//         else{
//             LL_AHB1_GRP1_DisableClock(LL_AHB1_GRP1_PERIPH_DMA1);
//         }
//     }
// #endif
// #ifdef _WITH_DMA2
//     if (dma == DMA2)
//     {
//         if( enable ){
//             LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA2);
//         }
//         else{
//             LL_AHB1_GRP1_DisableClock(LL_AHB1_GRP1_PERIPH_DMA2);
//         }
//     }
// #endif 
// }

// /**
//  * @brief 返回Remap配置ID
//  *
//  * @param config
//  * @return int
//  */
// int stm32ParseRemapId(const char *config)
// {
//     if (config && !strncmp(config, "remap:", 6))
//     {
//         return atoi(&config[6]);
//     }    
//     return -1;
// }

// /**
//  * @brief 获取定时器的时钟
//  * 
//  * @param id 
//  * @return const uint32_t 
//  * 
//  * @note
//  * STM32F10x@72MHZ
//  * 
//  * APB1 = SYSCLK/2 = 36MHZ
//  * APB2 = SYSCLK/1 = 72MHZ
//  * 
//  *   // HCLK = SYSCLK 
//  *   RCC->CFGR |= (uint32_t)RCC_CFGR_HPRE_DIV1;
//  *     
//  *   // PCLK2 = HCLK 
//  *   RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE2_DIV1;
//  *   
//  *   // PCLK1 = HCLK 
//  *   RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE1_DIV2;
//  * 
//  * TIM1 TIM8 1*APB2 = 72MHZ
//  * TIM2 .. TIM7 = 2* APB1 = 72MHZ
//  * 
//  * STM32F40x@168MHZ
//  * 
//  * APB1 =  SYSCLK/4 = 42MHZ
//  * APB2 =  SYSCLK/2 = 84MHZ
//  * 
//  * TIM1 TIM8-11   2*APB2 = 168MHZ
//  * TIM2-7 TIME12-14  2*APB1 = 84MHZ
//  * 
//  * STM32G030xx@64MHZ
//  * 
//  * APB1 =  SYSCLK/1 = 64MHZ
//  * APB2 =  SYSCLK/1 = 64MHZ
//  * 
//  * TIM1 TIM14、TIM16、TIM17   APB2 = 64MHZ
//  * TIME3  APB1 = 64MHZ
//  */
// uint32_t stm32GetTimerClock(uint8_t id)
// {
//     if ((id == _TIMER1) || (id == _TIMER8) 
//     || (id == _TIMER9) || (id == _TIMER10) 
//     || (id == _TIMER11)|| (id == _TIMER12)
//     || (id == _TIMER13)|| (id == _TIMER14)
//     || (id == _TIMER15)|| (id == _TIMER16)
//     || (id == _TIMER17))
//     {
//         // 2*APB2 = 168MHZ
//         #if defined(__stm32f4xx__)        
//         return (SystemCoreClock / STM32_APB2_PRESCALER) * 2;
//         #elif defined(__stm32g03xx__)
//         //64MHZ
//         return SystemCoreClock / STM32_APB2_PRESCALER;
//         #else 
//         return SystemCoreClock / STM32_APB2_PRESCALER;
//         #endif 
//     }
//     else   
//     {
//         #if defined(__stm32g03xx__)
//         //64MHZ
//          return (SystemCoreClock / STM32_APB1_PRESCALER);
//         #else
//         // 2*APB1 = 2 * 42MHZ = 84MHZ
//         return (SystemCoreClock / STM32_APB1_PRESCALER) * 2;
//         #endif
//     }
// }



// /**
//  * @brief 更高效返回GPIO的端口
//  *
//  * @param gpio
//  * @return GPIO_TypeDef*
//  */
// GPIO_TypeDef *stm32GpioToPort(gpio_t gpio)
// {
//     int base = (gpio >> 4) & 0x0f;
//     return (base < sizeof(g_portBase) / sizeof(g_portBase[0])) ? g_portBase[base] : NULL;
// }

// #ifdef __stm32f4xx__

// #define  _LL_ENABLE_CLOCK  LL_AHB1_GRP1_EnableClock
// #define  _LL_DISABLE_CLOCK LL_AHB1_GRP1_DisableClock

// #define  _LL_PERIPH_GPIOA  LL_AHB1_GRP1_PERIPH_GPIOA 
// #define  _LL_PERIPH_GPIOB  LL_AHB1_GRP1_PERIPH_GPIOB 
// #define  _LL_PERIPH_GPIOC  LL_AHB1_GRP1_PERIPH_GPIOC 
// #define  _LL_PERIPH_GPIOD  LL_AHB1_GRP1_PERIPH_GPIOD 
// #define  _LL_PERIPH_GPIOE  LL_AHB1_GRP1_PERIPH_GPIOE 
// #define  _LL_PERIPH_GPIOF  LL_AHB1_GRP1_PERIPH_GPIOF 
// #define  _LL_PERIPH_GPIOG  LL_AHB1_GRP1_PERIPH_GPIOG 
// #define  _LL_PERIPH_GPIOH  LL_AHB1_GRP1_PERIPH_GPIOH 
// #define  _LL_PERIPH_GPIOI  LL_AHB1_GRP1_PERIPH_GPIOI 
// #define  _LL_PERIPH_GPIOJ  LL_AHB1_GRP1_PERIPH_GPIOJ 
// #define  _LL_PERIPH_GPIOK  LL_AHB1_GRP1_PERIPH_GPIOK 

// #else // stm32g03xx

// #define  _LL_ENABLE_CLOCK  LL_IOP_GRP1_EnableClock
// #define  _LL_DISABLE_CLOCK LL_IOP_GRP1_DisableClock

// #define  _LL_PERIPH_GPIOA  LL_IOP_GRP1_PERIPH_GPIOA 
// #define  _LL_PERIPH_GPIOB  LL_IOP_GRP1_PERIPH_GPIOB 
// #define  _LL_PERIPH_GPIOC  LL_IOP_GRP1_PERIPH_GPIOC 
// #define  _LL_PERIPH_GPIOD  LL_IOP_GRP1_PERIPH_GPIOD 
// #define  _LL_PERIPH_GPIOE  LL_IOP_GRP1_PERIPH_GPIOE 
// #define  _LL_PERIPH_GPIOF  LL_IOP_GRP1_PERIPH_GPIOF 
// #define  _LL_PERIPH_GPIOG  LL_IOP_GRP1_PERIPH_GPIOG 
// #define  _LL_PERIPH_GPIOH  LL_IOP_GRP1_PERIPH_GPIOH 
// #define  _LL_PERIPH_GPIOI  LL_IOP_GRP1_PERIPH_GPIOI 
// #define  _LL_PERIPH_GPIOJ  LL_IOP_GRP1_PERIPH_GPIOJ 
// #define  _LL_PERIPH_GPIOK  LL_IOP_GRP1_PERIPH_GPIOK 
// #endif 

// /**
//  * @brief 使能指定的GPIO端口
//  *
//  * @param gpioBank
//  * @param enable
//  */
// void stm32GpioPortEnable(void *gpioBank, int enable)
// {
//     if(0)
//     {

//     }
// #ifdef GPIOA
//     else if( gpioBank == GPIOA )
//     {
//         if(enable){
//             _LL_ENABLE_CLOCK(_LL_PERIPH_GPIOA);
//         } else{
//             _LL_DISABLE_CLOCK(_LL_PERIPH_GPIOA);
//         }
//     }
// #endif
// #ifdef GPIOB
//     else if( gpioBank == GPIOB )
//     {
//         if(enable){
//             _LL_ENABLE_CLOCK(_LL_PERIPH_GPIOB);
//         } else{
//             _LL_DISABLE_CLOCK(_LL_PERIPH_GPIOB);
//         }
//     }
// #endif
// #ifdef GPIOC
//     else if( gpioBank == GPIOC )
//     {
//         if(enable){
//             _LL_ENABLE_CLOCK(_LL_PERIPH_GPIOC);
//         } else{
//             _LL_DISABLE_CLOCK(_LL_PERIPH_GPIOC);
//         }
//     }
// #endif
// #ifdef GPIOD
//     else if( gpioBank == GPIOD )
//     {
//         if(enable){
//             _LL_ENABLE_CLOCK(_LL_PERIPH_GPIOD);
//         } else{
//             _LL_DISABLE_CLOCK(_LL_PERIPH_GPIOD);
//         }
//     }
// #endif
// #ifdef GPIOE
//     else if( gpioBank == GPIOE )
//     {
//         if(enable){
//             _LL_ENABLE_CLOCK(_LL_PERIPH_GPIOE);
//         } else{
//             _LL_DISABLE_CLOCK(_LL_PERIPH_GPIOE);
//         }
//     }
// #endif
// #ifdef GPIOF
//     else if( gpioBank == GPIOF )
//     {
//         if (enable){
//             _LL_ENABLE_CLOCK(_LL_PERIPH_GPIOF);
//         } else{
//             _LL_DISABLE_CLOCK(_LL_PERIPH_GPIOF);
//         }
//     }
// #endif
// #ifdef GPIOG
//     else if( gpioBank == GPIOG )
//     {
//         if (enable){
//             _LL_ENABLE_CLOCK(_LL_PERIPH_GPIOG);
//         } else{
//             _LL_DISABLE_CLOCK(_LL_PERIPH_GPIOG);
//         }
//     }
// #endif
// #ifdef GPIOH
//     else if( gpioBank == GPIOH )
//     {
//         if (enable){
//             _LL_ENABLE_CLOCK(_LL_PERIPH_GPIOH);
//         } else{
//             _LL_DISABLE_CLOCK(_LL_PERIPH_GPIOH);
//         }
//     }
// #endif
// #ifdef GPIOI
//     else if( gpioBank == GPIOI )
//     {
//         if (enable){    
//             _LL_ENABLE_CLOCK(_LL_PERIPH_GPIOI);
//         } else{
//             _LL_DISABLE_CLOCK(_LL_PERIPH_GPIOI);
//         }
//     }
// #endif
// #ifdef GPIOJ
//     else if( gpioBank == GPIOJ )
//     {
//         if (enable){
//             _LL_ENABLE_CLOCK(_LL_PERIPH_GPIOJ);
//         } else{ 
//             _LL_DISABLE_CLOCK(_LL_PERIPH_GPIOJ);
//         }
//     }
// #endif
// #ifdef GPIOK
//     else if( gpioBank == GPIOK )
//     {
//         if (enable){
//             _LL_ENABLE_CLOCK(_LL_PERIPH_GPIOK);
//         } else{
//             _LL_DISABLE_CLOCK(_LL_PERIPH_GPIOK);
//         }
//     }
// #endif
// }


// /**
//  * @brief 打印芯片信息
//  *
//  */
// __WEAK void dumpChipInfo(void)
// {
// #ifdef TRACE_ENABLE
//     rawPrint("\r\n\r\nCPU@%uHZ\r\n", SystemCoreClock);
// #endif 
// }


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


//     #ifdef TRACE_ENABLE
//     // 初始化Trace
//     traceInit();

//     // 显示芯片信息
//     dumpChipInfo();
//     #endif 
}

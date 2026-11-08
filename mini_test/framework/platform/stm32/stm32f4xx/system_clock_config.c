#include "stm32f4xx.h"
#include "stm32f4xx_ll_rcc.h"
#include "stm32f4xx_ll_bus.h"
#include "stm32f4xx_ll_utils.h"
#include "stm32f4xx_ll_system.h"
#include "stm32f4xx_ll_pwr.h"


/**
 * @brief Set the SystemClock HSE 8Mhz object
 * 
 * HSE频率: 8MHz (外部晶体)
 * PLLM分频: 4 (8MHz / 4 = 2MHz)
 * PLLN倍频: 180 (2MHz * 180 = 360MHz)
 * PLLP分频: 2 (360MHz / 2 = 180MHz系统时钟)
 * PLLQ分频: 8 (360MHz / 8 = 45MHz用于USB等外设)
 */
// void setSystemClock_HSE_8Mhz(void)
// {
//     /* Enable Power Control clock */
//     LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);
    
//     /* Configure the main internal regulator output voltage */
//     LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE1);
    
//     /* Enable HSE oscillator */
//     LL_RCC_HSE_Enable();
//     while(LL_RCC_HSE_IsReady() != 1);
    
//     /* Configure PLL */
//     LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE, LL_RCC_PLLM_DIV_8, 360, LL_RCC_PLLP_DIV_2);
//     LL_RCC_PLL_ConfigDomain_48M(LL_RCC_PLLSOURCE_HSE, LL_RCC_PLLM_DIV_8, 360, LL_RCC_PLLQ_DIV_7);
    
//     /* Enable PLL and wait for it to be ready */
//     LL_RCC_PLL_Enable();
//     while(LL_RCC_PLL_IsReady() != 1);
    
//     /* Set FLASH latency */
//     LL_FLASH_SetLatency(LL_FLASH_LATENCY_5);
    
//     /* Configure AHB, APB1 and APB2 prescalers */
//     LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
//     LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_4);
//     LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_2);
    
//     /* Select PLL as system clock source */
//     LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);
//     while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL);

//     // 禁用HSI
//     //LL_RCC_HSI_Disable();
// }

#if defined(CONFIG_STM32_HSE_8M)

/**
 * @brief Set the SystemClock HSE 8Mhz object
 * 
 * HSE频率: 8MHz (外部晶体)
 * PLLM分频: 4 (8MHz / 4 = 2MHz)
 * PLLN倍频: 180 (2MHz * 180 = 360MHz)
 * PLLP分频: 2 (360MHz / 2 = 180MHz系统时钟)
 * PLLQ分频: 8 (360MHz / 8 = 45MHz用于USB等外设)
 */
void setSystemClock_HSE_8Mhz(void)
{
#if HSE_VALUE != 8000000
#error "HSE_VALUE is not defined and not equal to 8000000"
#endif 

    LL_UTILS_PLLInitTypeDef PLL_InitStruct = {0};
    LL_UTILS_ClkInitTypeDef UTILS_ClkInitStruct = {0};

    /* Enable Power Control clock */
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);
    
    /* Configure the main internal regulator output voltage */
    LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE1);

    /* Configure PLL */
    PLL_InitStruct.PLLM = LL_RCC_PLLM_DIV_4;        // 8MHz / 4 = 2MHz
    PLL_InitStruct.PLLN = 180;      // 2MHz * 180 = 360MHz
    PLL_InitStruct.PLLP = LL_RCC_PLLP_DIV_2;  // 360MHz / 2 = 180MHz

    /* Configure System clock */
    UTILS_ClkInitStruct.AHBCLKDivider = LL_RCC_SYSCLK_DIV_1;
    UTILS_ClkInitStruct.APB1CLKDivider = LL_RCC_APB1_DIV_4;
    UTILS_ClkInitStruct.APB2CLKDivider = LL_RCC_APB2_DIV_2;

    /* Configure Flash latency */
    LL_FLASH_SetLatency(LL_FLASH_LATENCY_5);

    /* Configure system clock using HSE */
    if(LL_PLL_ConfigSystemClock_HSE(HSE_VALUE, LL_UTILS_HSEBYPASS_OFF, &PLL_InitStruct, &UTILS_ClkInitStruct) != SUCCESS)
    {
        /* 配置失败，可以添加错误处理 */
        // while(1);
    }

    /* 选主PLL为系统时钟 */
    LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);
    while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL);

    // usb时钟源配置
#ifdef CONFIG_USB_CDC_ENABLE

#if 0
    // 配置48M时钟 (这个配置有问题， 360/7 = 51M，要想使用这个配置，需要将PLLM改成LL_RCC_PLLM_DIV_8，PLLN改成336)
    LL_RCC_PLL_ConfigDomain_48M(LL_RCC_PLLSOURCE_HSE, PLL_InitStruct.PLLM, PLL_InitStruct.PLLN, LL_RCC_PLLQ_DIV_7);
#else 
    /* 配置PLLSAI (用于USB时钟) */
    LL_RCC_PLLSAI_ConfigDomain_48M(LL_RCC_PLLSOURCE_HSE, LL_RCC_PLLSAIM_DIV_8, 192, LL_RCC_PLLSAIP_DIV_4);
    LL_RCC_PLLSAI_Enable();
    while(LL_RCC_PLLSAI_IsReady() != 1);

    /* 选择PLLSAI作为USB时钟源 */
    LL_RCC_SetUSBClockSource(LL_RCC_USB_CLKSOURCE_PLLSAI);

#endif 

#endif

}

#endif // CONFIG_STM32_HSE_8M


/**
 * @brief Set the SystemClock HSI 16Mhz object
 * 
 * HSI频率: 16MHz (内部RC振荡器)
 * PLLM分频: 16 (16MHz / 16 = 1MHz)
 * PLLN倍频: 360 (1MHz * 360 = 360MHz)
 * PLLP分频: 2 (360MHz / 2 = 180MHz系统时钟)
 */
void setSystemClock_HSI_16Mhz(void)
{
    // LL_PLL_ConfigSystemClock_HSI() directly use HSI_VALUE!!

#if HSI_VALUE != 16000000
#error "HSI_VALUE is not defined and not equal to 16000000"
#endif 

    LL_UTILS_PLLInitTypeDef PLL_InitStruct = {0};
    LL_UTILS_ClkInitTypeDef UTILS_ClkInitStruct = {0};

    /* Enable Power Control clock */
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);
    
    /* Configure the main internal regulator output voltage */
    LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE1);

    /* Configure PLL */
    PLL_InitStruct.PLLM = LL_RCC_PLLM_DIV_16;       // 16MHz / 16 = 1MHz
    PLL_InitStruct.PLLN = 360;      // 1MHz * 360 = 360MHz
    PLL_InitStruct.PLLP = LL_RCC_PLLP_DIV_2;  // 360MHz / 2 = 180MHz

    /* Configure System clock */
    UTILS_ClkInitStruct.AHBCLKDivider = LL_RCC_SYSCLK_DIV_1;
    UTILS_ClkInitStruct.APB1CLKDivider = LL_RCC_APB1_DIV_4;
    UTILS_ClkInitStruct.APB2CLKDivider = LL_RCC_APB2_DIV_2;

    /* Configure Flash latency */
    LL_FLASH_SetLatency(LL_FLASH_LATENCY_5);
		
    /* Configure system clock using HSI */
    if(LL_PLL_ConfigSystemClock_HSI(&PLL_InitStruct, &UTILS_ClkInitStruct) != SUCCESS)
    {
        /* 配置失败，可以添加错误处理 */
        // while(1);
    }
#if 0
    // 配置48M时钟
    //LL_RCC_PLL_ConfigDomain_48M(LL_RCC_PLLSOURCE_HSI, PLL_InitStruct.PLLM, PLL_InitStruct.PLLN, LL_RCC_PLLQ_DIV_7);
#else
    // 配置PLLSAI用于USB 48MHz时钟
    // PLLSAI_VCO = (HSI_VALUE / PLLM) * PLLSAIN = (16MHz / 16) * 192 = 192MHz
    // USB/48MHz = PLLSAI_VCO / PLLSAIP = 192MHz / 4 = 48MHz
    
    // 禁用PLLSAI
    LL_RCC_PLLSAI_Disable();
    while(LL_RCC_PLLSAI_IsReady() != 0);
    
    // 配置PLLSAI
    // PLLSAIM = 16, PLLSAIN = 192, PLLSAIP = 4
    uint32_t pllsaim = 16;    // 与PLL相同的PLLM
    uint32_t pllsain = 192;   // VCO = 1MHz * 192 = 192MHz
    uint32_t pllsaip = LL_RCC_PLLSAIP_DIV_4;  // 192MHz / 4 = 48MHz (USB需要)
    uint32_t pllsaiq = 7;     // 保留一个合理值但不使用
    
    // 使用STM32F446的方式配置PLLSAI
    RCC->PLLSAICFGR = pllsaim | (pllsain << 6) | (pllsaip) | (pllsaiq << 24);
    
    // 启用PLLSAI
    LL_RCC_PLLSAI_Enable();
    while(LL_RCC_PLLSAI_IsReady() == 0);
    
    // 设置USB使用PLLSAI作为时钟源 (在DCKCFGR2寄存器设置CK48MSEL位)
    SET_BIT(RCC->DCKCFGR2, RCC_DCKCFGR2_CK48MSEL);

#endif 
    // 当前版本有BUG，不可用
    //LL_RCC_PLL_ConfigDomain_SAI(LL_RCC_PLLSOURCE_HSI, LL_RCC_PLLM_DIV_32, 192, LL_RCC_PLLR_DIV_2);
}


void SystemClock_Config(void)
{
    // 复位所有外设
    LL_APB1_GRP1_ForceReset(LL_APB1_GRP1_PERIPH_ALL);
    LL_APB1_GRP1_ReleaseReset(LL_APB1_GRP1_PERIPH_ALL);
    LL_APB2_GRP1_ForceReset(LL_APB2_GRP1_PERIPH_ALL);
    LL_APB2_GRP1_ReleaseReset(LL_APB2_GRP1_PERIPH_ALL);
    LL_AHB1_GRP1_ForceReset(LL_AHB1_GRP1_PERIPH_ALL);
    LL_AHB1_GRP1_ReleaseReset(LL_AHB1_GRP1_PERIPH_ALL);
    LL_AHB2_GRP1_ForceReset(LL_AHB2_GRP1_PERIPH_ALL);
    LL_AHB2_GRP1_ReleaseReset(LL_AHB2_GRP1_PERIPH_ALL);
    LL_AHB3_GRP1_ForceReset(LL_AHB3_GRP1_PERIPH_ALL);
    LL_AHB3_GRP1_ReleaseReset(LL_AHB3_GRP1_PERIPH_ALL);   
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

    // 使用外部8Mhz晶体
#if defined(CONFIG_STM32_HSE_8M)
    setSystemClock_HSE_8Mhz();
#elif defined(CONFIG_STM32_HSI_16M)
    setSystemClock_HSI_16Mhz();
#else 
    #warning "***SystemClock is not defined: CONFIG_STM32_HSE_8M or CONFIG_STM32_HSI_16M"
    setSystemClock_HSI_16Mhz();
#endif 

    // 初始化flash时钟
    // LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_FLASH);
}


/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follows: 
  *            System Clock source            = PLL (HSI)
  *            SYSCLK(Hz)                     = 180000000
  *            HCLK(Hz)                       = 180000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 4
  *            APB2 Prescaler                 = 2
  *            HSI Frequency(Hz)              = 16000000
  *            PLL_M                          = 16
  *            PLL_N                          = 360
  *            PLL_P                          = 2
  *            PLL_Q                          = 7
  *            PLL_R                          = 6
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale1 mode
  *            Flash Latency(WS)              = 5
  * @param  None
  * @retval None
  */
// void SystemClock_Config_Hal(void)
// {
//   RCC_ClkInitTypeDef RCC_ClkInitStruct;
//   RCC_OscInitTypeDef RCC_OscInitStruct;
//   HAL_StatusTypeDef ret = HAL_OK;

//   /* Enable Power Control clock */
//   __HAL_RCC_PWR_CLK_ENABLE();
  
//   /* The voltage scaling allows optimizing the power consumption when the device is 
//      clocked below the maximum system frequency, to update the voltage scaling value 
//      regarding system frequency refer to product datasheet.  */
//   __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  
//   /* Enable HSI Oscillator and activate PLL with HSI as source */
//   RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
//   RCC_OscInitStruct.HSIState = RCC_HSI_ON;
//   RCC_OscInitStruct.HSICalibrationValue = 0x10;
//   RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
//   RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
//   RCC_OscInitStruct.PLL.PLLM = 16;
//   RCC_OscInitStruct.PLL.PLLN = 360;
//   RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
//   RCC_OscInitStruct.PLL.PLLQ = 7;
//   RCC_OscInitStruct.PLL.PLLR = 6;
//   if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
//   {
//     while(1) { ; }
//   }
  
//    /* Activate the OverDrive to reach the 180 MHz Frequency */  
//   ret = HAL_PWREx_EnableOverDrive();
//   if(ret != HAL_OK)
//   {
//     while(1) { ; }
//   }
//   /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
//      clocks dividers */
//   RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
//   RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
//   RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
//   RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;  
//   RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;  
//   if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
//   {
//     while(1) { ; }
//   }

// }

# STM32F4 系列共用这一份。具体芯片由 SOC_DEVICE、startup 与 ld 区分。

set(PLATFORM_CPU_OPTS
    -mcpu=cortex-m4
    -mthumb
)

set(PLATFORM_INC_DIRS
    ${CMAKE_CURRENT_LIST_DIR}/..
)

set(PLATFORM_C_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/../platform_private.c
    ${CMAKE_CURRENT_LIST_DIR}/system_clock_config.c
    ${CMAKE_CURRENT_LIST_DIR}/../sys_tick.c
)

if(NOT SOC_DEVICE)
    message(FATAL_ERROR "缺少 SOC_DEVICE，请用 ./build.sh 构建")
endif()

set(HAL_ROOT ${CMAKE_CURRENT_LIST_DIR}/../../../third/STM32F4xx_HAL)
set(HAL_SRC ${HAL_ROOT}/STM32F4xx_HAL_Driver/Src)

# 厂商代码单独成库，不套用工程的 -Werror。
set(PLATFORM_LIB stm32f4_hal)
add_library(${PLATFORM_LIB} STATIC
    ${HAL_SRC}/stm32f4xx_hal.c
    ${HAL_SRC}/stm32f4xx_hal_cortex.c
    ${HAL_SRC}/stm32f4xx_hal_rcc.c
    ${HAL_SRC}/stm32f4xx_hal_rcc_ex.c
    ${HAL_SRC}/stm32f4xx_hal_gpio.c
    ${HAL_SRC}/stm32f4xx_hal_flash.c
    ${HAL_SRC}/stm32f4xx_hal_flash_ex.c
    ${HAL_SRC}/stm32f4xx_hal_pwr.c
    ${HAL_SRC}/stm32f4xx_hal_pwr_ex.c
    ${HAL_SRC}/stm32f4xx_hal_dma.c
    ${HAL_SRC}/stm32f4xx_hal_exti.c
    ${HAL_ROOT}/CMSIS/Device/ST/STM32F4xx/Source/Templates/system_stm32f4xx.c
    # F4 的 LL 整套源文件，不按工程用到的外设增减。
    ${HAL_SRC}/stm32f4xx_ll_adc.c
    ${HAL_SRC}/stm32f4xx_ll_crc.c
    ${HAL_SRC}/stm32f4xx_ll_dac.c
    ${HAL_SRC}/stm32f4xx_ll_dma.c
    ${HAL_SRC}/stm32f4xx_ll_dma2d.c
    ${HAL_SRC}/stm32f4xx_ll_exti.c
    ${HAL_SRC}/stm32f4xx_ll_fmc.c
    ${HAL_SRC}/stm32f4xx_ll_fmpi2c.c
    ${HAL_SRC}/stm32f4xx_ll_fsmc.c
    ${HAL_SRC}/stm32f4xx_ll_gpio.c
    ${HAL_SRC}/stm32f4xx_ll_i2c.c
    ${HAL_SRC}/stm32f4xx_ll_lptim.c
    ${HAL_SRC}/stm32f4xx_ll_pwr.c
    ${HAL_SRC}/stm32f4xx_ll_rcc.c
    ${HAL_SRC}/stm32f4xx_ll_rng.c
    ${HAL_SRC}/stm32f4xx_ll_rtc.c
    ${HAL_SRC}/stm32f4xx_ll_sdmmc.c
    ${HAL_SRC}/stm32f4xx_ll_spi.c
    ${HAL_SRC}/stm32f4xx_ll_tim.c
    ${HAL_SRC}/stm32f4xx_ll_usart.c
    ${HAL_SRC}/stm32f4xx_ll_usb.c
    ${HAL_SRC}/stm32f4xx_ll_utils.c
)

target_include_directories(${PLATFORM_LIB} PUBLIC
    ${HAL_ROOT}/STM32F4xx_HAL_Driver/Inc
    ${HAL_ROOT}/CMSIS/Device/ST/STM32F4xx/Include
    ${CMAKE_CURRENT_LIST_DIR}/../../../third/CMSIS/5.8.0/CMSIS/Core/Include
    ${CMAKE_CURRENT_LIST_DIR}
)

target_compile_definitions(${PLATFORM_LIB} PUBLIC
    ${SOC_DEVICE}
    USE_HAL_DRIVER
    USE_FULL_LL_DRIVER
)

target_compile_options(${PLATFORM_LIB} PRIVATE
    ${PLATFORM_CPU_OPTS}
    -ffunction-sections
    -fdata-sections
)
